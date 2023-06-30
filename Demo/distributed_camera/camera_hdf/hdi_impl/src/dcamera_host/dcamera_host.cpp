/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "dcamera_host.h"
#include "anonymous_string.h"
#include "distributed_hardware_log.h"

namespace OHOS {
namespace DistributedHardware {
std::shared_ptr<DCameraHost> DCameraHost::instance_ = nullptr;
DCameraHost::AutoRelease DCameraHost::autoRelease_;

std::shared_ptr<DCameraHost> DCameraHost::GetInstance()
{
    if (instance_ == nullptr) {
        instance_ = std::make_shared<DCameraHost>();
        if (instance_ == nullptr) {
            DHLOGE("Get distributed camera host instance failed.");
            return nullptr;
        }
    }
    return instance_;
}

CamRetCode DCameraHost::SetCallback(const OHOS::sptr<ICameraHostCallback> &callback)
{
    if (callback == nullptr) {
        DHLOGE("DCameraHost::SetCallback, camera host callback is null.");
        return CamRetCode::INVALID_ARGUMENT;
    }
    dCameraHostCallback_ = callback;
    return CamRetCode::NO_ERROR;
}

CamRetCode DCameraHost::GetCameraIds(std::vector<std::string> &cameraIds)
{
    auto iter = dhBaseHashDCamIdMap_.begin();
    while (iter != dhBaseHashDCamIdMap_.end()) {
        if (!(iter->second).empty()) {
            cameraIds.push_back(iter->second);
        }
        iter++;
    }
    return CamRetCode::NO_ERROR;
}

CamRetCode DCameraHost::GetCameraAbility(const std::string &cameraId,
    std::shared_ptr<CameraAbility> &ability)
{
    DHLOGE("DCameraHost::GetCameraAbility for cameraId: %s", cameraId.c_str());

    if (IsCameraIdInvalid(cameraId)) {
        DHLOGE("DCameraHost::GetCameraAbility, invalid camera id.");
        return CamRetCode::INVALID_ARGUMENT;
    }

    auto iter = dCameraDeviceMap_.find(cameraId);
    return (iter->second)->GetDCameraAbility(ability);
}

CamRetCode DCameraHost::OpenCamera(const std::string &cameraId,
    const OHOS::sptr<ICameraDeviceCallback> &callback,
    OHOS::sptr<ICameraDevice> &pDevice)
{
    DHLOGI("DCameraHost::OpenCamera for cameraId: %s", cameraId.c_str());

    if (IsCameraIdInvalid(cameraId) || callback == nullptr) {
        DHLOGE("DCameraHost::OpenCamera, open camera id is empty or callback is null.");
        return CamRetCode::INVALID_ARGUMENT;
    }

    auto iter = dCameraDeviceMap_.find(cameraId);
    if (iter == dCameraDeviceMap_.end()) {
        DHLOGE("DCameraHost::OpenCamera, dcamera device not found.");
        return CamRetCode::INSUFFICIENT_RESOURCES;
    }

    OHOS::sptr<DCameraDevice> dcameraDevice = iter->second;
    if (dcameraDevice == nullptr) {
        DHLOGE("DCameraHost::OpenCamera, dcamera device is null.");
        return INSUFFICIENT_RESOURCES;
    }

    if (dcameraDevice->IsOpened()) {
        DHLOGE("DCameraHost::OpenCamera, dcamera device %s already opened.", cameraId.c_str());
        return CamRetCode::CAMERA_BUSY;
    }

    CamRetCode ret = dcameraDevice->OpenDCamera(callback);
    if (ret != CamRetCode::NO_ERROR) {
        DHLOGE("DCameraHost::OpenCamera, open camera failed.");
        return ret;
    }
    pDevice = dcameraDevice;

    DHLOGI("DCameraHost::OpenCamera, open camera %s success.", cameraId.c_str());
    return CamRetCode::NO_ERROR;
}

CamRetCode DCameraHost::SetFlashlight(const std::string &cameraId,  bool &isEnable)
{
    (void)cameraId;
    (void)isEnable;
    DHLOGI("DCameraHost::SetFlashlight, distributed camera not support.");

    return CamRetCode::METHOD_NOT_SUPPORTED;
}

DCamRetCode DCameraHost::AddDCameraDevice(const std::shared_ptr<DHBase> &dhBase, const std::string &abilityInfo,
    const sptr<IDCameraProviderCallback> &callback)
{
    DHLOGI("DCameraHost::AddDCameraDevice for {devId: %s, dhId: %s}",
        GetAnonyString(dhBase->deviceId_).c_str(), dhBase->dhId_.c_str());

    OHOS::sptr<DCameraDevice> dcameraDevice = new (std::nothrow) DCameraDevice(dhBase, abilityInfo);
    if (dcameraDevice == nullptr) {
        DHLOGE("DCameraHost::AddDCameraDevice, create dcamera device failed.");
        return DCamRetCode::INVALID_ARGUMENT;
    }

    std::string dCameraId = dcameraDevice->GetDCameraId();
    dCameraDeviceMap_[dCameraId] = dcameraDevice;
    DHBase dhBaseKey(dhBase->deviceId_, dhBase->dhId_);
    dhBaseHashDCamIdMap_.emplace(dhBaseKey, dCameraId);
    dcameraDevice->SetProviderCallback(callback);

    if (dCameraHostCallback_ != nullptr) {
        dCameraHostCallback_->OnCameraEvent(dCameraId, CameraEvent::CAMERA_EVENT_DEVICE_ADD);
    }

    DHLOGI("DCameraHost::AddDCameraDevice, create dcamera device success, dCameraId: %s", dCameraId.c_str());
    return DCamRetCode::SUCCESS;
}

DCamRetCode DCameraHost::RemoveDCameraDevice(const std::shared_ptr<DHBase> &dhBase)
{
    DHLOGI("DCameraHost::RemoveDCameraDevice for {devId: %s, dhId: %s}",
        GetAnonyString(dhBase->deviceId_).c_str(), dhBase->dhId_.c_str());

    std::string dCameraId = GetCameraIdByDHBase(dhBase);
    if (dCameraId.empty()) {
        DHLOGE("DCameraHost::RemoveDCameraDevice, dhBase not exist.");
        return DCamRetCode::INVALID_ARGUMENT;
    }

    OHOS::sptr<DCameraDevice> dcameraDevice = GetDCameraDeviceByDHBase(dhBase);
    if (dcameraDevice != nullptr) {
        if (dcameraDevice->IsOpened()) {
            dcameraDevice->Close();
        }
        dcameraDevice->SetProviderCallback(nullptr);
    }

    DHBase dhBaseKey(dhBase->deviceId_, dhBase->dhId_);
    dhBaseHashDCamIdMap_.erase(dhBaseKey);
    dCameraDeviceMap_.erase(dCameraId);

    if (dCameraHostCallback_ != nullptr) {
        dCameraHostCallback_->OnCameraEvent(dCameraId, CameraEvent::CAMERA_EVENT_DEVICE_RMV);
    }

    DHLOGI("DCameraHost::RemoveDCameraDevice, remove dcamera device success, dCameraId: %s", dCameraId.c_str());
    return DCamRetCode::SUCCESS;
}

bool DCameraHost::IsCameraIdInvalid(const std::string &cameraId)
{
    if (cameraId.empty()) {
        return true;
    }

    auto iter = dhBaseHashDCamIdMap_.begin();
    while (iter != dhBaseHashDCamIdMap_.end()) {
        if (cameraId == iter->second) {
            return false;
        }
        iter++;
    }
    return true;
}

std::string DCameraHost::GetCameraIdByDHBase(const std::shared_ptr<DHBase> &dhBase)
{
    DHBase dhBaseKey(dhBase->deviceId_, dhBase->dhId_);
    auto iter = dhBaseHashDCamIdMap_.find(dhBaseKey);
    if (iter == dhBaseHashDCamIdMap_.end()) {
        return "";
    }
    return iter->second;
}

OHOS::sptr<DCameraDevice> DCameraHost::GetDCameraDeviceByDHBase(const std::shared_ptr<DHBase> &dhBase)
{
    std::string dCameraId = GetCameraIdByDHBase(dhBase);
    if (dCameraId.empty()) {
        DHLOGE("DCameraHost::GetDCameraDeviceByDHBase, dhBase not exist.");
        return nullptr;
    }

    auto iter = dCameraDeviceMap_.find(dCameraId);
    if (iter == dCameraDeviceMap_.end()) {
        DHLOGE("DCameraHost::GetDCameraDeviceByDHBase, dcamera device not found.");
        return nullptr;
    }
    return iter->second;
}

void DCameraHost::NotifyDCameraStatus(const std::shared_ptr<DHBase> &dhBase, int32_t result)
{
    std::string dCameraId = GetCameraIdByDHBase(dhBase);
    if (dCameraId.empty()) {
        DHLOGE("DCameraHost::NotifyDCameraStatus, dhBase not exist.");
        return;
    }
    if (dCameraHostCallback_ != nullptr) {
        dCameraHostCallback_->OnCameraStatus(dCameraId, CameraStatus::UN_AVAILABLE);
    }
}
} // namespace DistributedHardware
} // namespace OHOS
