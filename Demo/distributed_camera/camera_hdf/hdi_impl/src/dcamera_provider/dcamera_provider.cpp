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

#include "dcamera_provider.h"
#include "anonymous_string.h"
#include "constants.h"
#include "dcamera_device.h"
#include "dcamera_host.h"
#include "distributed_hardware_log.h"

namespace OHOS {
namespace DistributedHardware {
std::shared_ptr<DCameraProvider> DCameraProvider::instance_ = nullptr;
DCameraProvider::AutoRelease DCameraProvider::autoRelease_;

std::shared_ptr<DCameraProvider> DCameraProvider::GetInstance()
{
    if (instance_ == nullptr) {
        instance_ = std::make_shared<DCameraProvider>();
        if (instance_ == nullptr) {
            DHLOGE("Get distributed camera provider instance failed.");
            return nullptr;
        }
    }
    return instance_;
}

DCamRetCode DCameraProvider::EnableDCameraDevice(const std::shared_ptr<DHBase> &dhBase,
    const std::string &abilityInfo, const sptr<IDCameraProviderCallback> &callback)
{
    DHLOGI("DCameraProvider::EnableDCameraDevice for {devId: %s, dhId: %s, abilityInfo length: %d}.",
        GetAnonyString(dhBase->deviceId_).c_str(), dhBase->dhId_.c_str(), abilityInfo.length());

    if (IsDhBaseInfoInvalid(dhBase)) {
        DHLOGE("DCameraProvider::EnableDCameraDevice, devId or dhId is invalid.");
        return DCamRetCode::INVALID_ARGUMENT;
    }
    if (abilityInfo.empty()) {
        DHLOGE("DCameraProvider::EnableDCameraDevice, dcamera ability is empty.");
        return DCamRetCode::INVALID_ARGUMENT;
    }
    if (callback == nullptr) {
        DHLOGE("DCameraProvider::EnableDCameraDevice, dcamera provider callback is null.");
        return DCamRetCode::INVALID_ARGUMENT;
    }

    std::shared_ptr<DCameraHost> dCameraHost = DCameraHost::GetInstance();
    if (dCameraHost == nullptr) {
        DHLOGE("DCameraProvider::EnableDCameraDevice, dcamera host is null.");
        return DCamRetCode::DEVICE_NOT_INIT;
    }
    DCamRetCode ret = dCameraHost->AddDCameraDevice(dhBase, abilityInfo, callback);
    if (ret != DCamRetCode::SUCCESS) {
        DHLOGE("DCameraProvider::EnableDCameraDevice failed, ret = %d.", ret);
    }

    return ret;
}

DCamRetCode DCameraProvider::DisableDCameraDevice(const std::shared_ptr<DHBase> &dhBase)
{
    DHLOGI("DCameraProvider::DisableDCameraDevice for {devId: %s, dhId: %s}.",
        GetAnonyString(dhBase->deviceId_).c_str(), dhBase->dhId_.c_str());

    if (IsDhBaseInfoInvalid(dhBase)) {
        DHLOGE("DCameraProvider::DisableDCameraDevice, devId or dhId is invalid.");
        return DCamRetCode::INVALID_ARGUMENT;
    }

    std::shared_ptr<DCameraHost> dCameraHost = DCameraHost::GetInstance();
    if (dCameraHost == nullptr) {
        DHLOGE("DCameraProvider::DisableDCameraDevice, dcamera host is null.");
        return DCamRetCode::DEVICE_NOT_INIT;
    }
    DCamRetCode ret = dCameraHost->RemoveDCameraDevice(dhBase);
    if (ret != DCamRetCode::SUCCESS) {
        DHLOGE("DCameraProvider::DisableDCameraDevice failed, ret = %d.", ret);
        return ret;
    }

    return DCamRetCode::SUCCESS;
}

DCamRetCode DCameraProvider::AcquireBuffer(const std::shared_ptr<DHBase> &dhBase, int streamId,
    std::shared_ptr<DCameraBuffer> &buffer)
{
    DHLOGI("DCameraProvider::AcquireBuffer for {devId: %s, dhId: %s}, streamId: %d.",
        GetAnonyString(dhBase->deviceId_).c_str(), dhBase->dhId_.c_str(), streamId);

    OHOS::sptr<DCameraDevice> device = GetDCameraDevice(dhBase);
    if (device == nullptr) {
        DHLOGE("DCameraProvider::AcquireBuffer failed, dcamera device not found.");
        return DCamRetCode::INVALID_ARGUMENT;
    }

    return device->AcquireBuffer(streamId, buffer);
}

DCamRetCode DCameraProvider::ShutterBuffer(const std::shared_ptr<DHBase> &dhBase, int streamId,
    const std::shared_ptr<DCameraBuffer> &buffer)
{
    if (buffer == nullptr) {
        DHLOGE("DCameraProvider::ShutterBuffer, input distributed camera buffer is null.");
        return DCamRetCode::INVALID_ARGUMENT;
    }
    DHLOGI("DCameraProvider::ShutterBuffer for {devId: %s, dhId: %s}, streamId = %d, buffer index = %d.",
        GetAnonyString(dhBase->deviceId_).c_str(), dhBase->dhId_.c_str(), streamId, buffer->index_);

    OHOS::sptr<DCameraDevice> device = GetDCameraDevice(dhBase);
    if (device == nullptr) {
        DHLOGE("DCameraProvider::ShutterBuffer failed, dcamera device not found.");
        return DCamRetCode::INVALID_ARGUMENT;
    }

    return device->ShutterBuffer(streamId, buffer);
}

DCamRetCode DCameraProvider::OnSettingsResult(const std::shared_ptr<DHBase> &dhBase,
    const std::shared_ptr<DCameraSettings> &result)
{
    DHLOGI("DCameraProvider::OnSettingsResult for {devId: %s, dhId: %s}.",
        GetAnonyString(dhBase->deviceId_).c_str(), dhBase->dhId_.c_str());

    OHOS::sptr<DCameraDevice> device = GetDCameraDevice(dhBase);
    if (device == nullptr) {
        DHLOGE("DCameraProvider::OnSettingsResult failed, dcamera device not found.");
        return DCamRetCode::INVALID_ARGUMENT;
    }

    return device->OnSettingsResult(result);
}

DCamRetCode DCameraProvider::Notify(const std::shared_ptr<DHBase> &dhBase,
    const std::shared_ptr<DCameraHDFEvent> &event)
{
    DHLOGI("DCameraProvider::Notify for {devId: %s, dhId: %s}.",
        GetAnonyString(dhBase->deviceId_).c_str(), dhBase->dhId_.c_str());

    OHOS::sptr<DCameraDevice> device = GetDCameraDevice(dhBase);
    if (device == nullptr) {
        DHLOGE("DCameraProvider::Notify failed, dcamera device not found.");
        return DCamRetCode::INVALID_ARGUMENT;
    }

    return device->Notify(event);
}

DCamRetCode DCameraProvider::OpenSession(const std::shared_ptr<DHBase> &dhBase)
{
    DHLOGI("DCameraProvider::OpenSession for {devId: %s, dhId: %s}.",
        GetAnonyString(dhBase->deviceId_).c_str(), dhBase->dhId_.c_str());

    sptr<IDCameraProviderCallback> callback = GetCallbackBydhBase(dhBase);
    if (callback == nullptr) {
        DHLOGE("DCameraProvider::OpenSession, dcamera provider callback not found.");
        return DCamRetCode::INVALID_ARGUMENT;
    }

    return callback->OpenSession(dhBase);
}

DCamRetCode DCameraProvider::CloseSession(const std::shared_ptr<DHBase> &dhBase)
{
    DHLOGI("DCameraProvider::CloseSession for {devId: %s, dhId: %s}.",
        GetAnonyString(dhBase->deviceId_).c_str(), dhBase->dhId_.c_str());

    sptr<IDCameraProviderCallback> callback = GetCallbackBydhBase(dhBase);
    if (callback == nullptr) {
        DHLOGE("DCameraProvider::CloseSession, dcamera provider callback not found.");
        return DCamRetCode::INVALID_ARGUMENT;
    }

    return callback->CloseSession(dhBase);
}

DCamRetCode DCameraProvider::ConfigureStreams(const std::shared_ptr<DHBase> &dhBase,
    const std::vector<std::shared_ptr<DCStreamInfo>> &streamInfos)
{
    DHLOGI("DCameraProvider::ConfigureStreams for {devId: %s, dhId: %s}.",
        GetAnonyString(dhBase->deviceId_).c_str(), dhBase->dhId_.c_str());

    sptr<IDCameraProviderCallback> callback = GetCallbackBydhBase(dhBase);
    if (callback == nullptr) {
        DHLOGE("DCameraProvider::ConfigStreams, dcamera provider callback not found.");
        return DCamRetCode::INVALID_ARGUMENT;
    }

    for (auto info : streamInfos) {
        DHLOGI("ConfigureStreams: id=%d, width=%d, height=%d, format=%d, " +
               "type=%d.", info->streamId_, info->width_, info->height_, info->format_, info->type_);
    }
    return callback->ConfigureStreams(dhBase, streamInfos);
}

DCamRetCode DCameraProvider::ReleaseStreams(const std::shared_ptr<DHBase> &dhBase,
    const std::vector<int> &streamIds)
{
    DHLOGI("DCameraProvider::ReleaseStreams for {devId: %s, dhId: %s}.",
        GetAnonyString(dhBase->deviceId_).c_str(), dhBase->dhId_.c_str());

    sptr<IDCameraProviderCallback> callback = GetCallbackBydhBase(dhBase);
    if (callback == nullptr) {
        DHLOGE("DCameraProvider::ReleaseStreams, dcamera provider callback not found.");
        return DCamRetCode::INVALID_ARGUMENT;
    }

    std::string idString = "";
    for (int id : streamIds) {
        idString += (std::to_string(id) + ", ");
    }
    DHLOGI("ReleaseStreams: ids=[%s].", idString.c_str());
    return callback->ReleaseStreams(dhBase, streamIds);
}

DCamRetCode DCameraProvider::StartCapture(const std::shared_ptr<DHBase> &dhBase,
    const std::vector<std::shared_ptr<DCCaptureInfo>> &captureInfos)
{
    DHLOGI("DCameraProvider::StartCapture for {devId: %s, dhId: %s}.",
        GetAnonyString(dhBase->deviceId_).c_str(), dhBase->dhId_.c_str());

    sptr<IDCameraProviderCallback> callback = GetCallbackBydhBase(dhBase);
    if (callback == nullptr) {
        DHLOGE("DCameraProvider::StartCapture, dcamera provider callback not found.");
        return DCamRetCode::INVALID_ARGUMENT;
    }

    for (auto info : captureInfos) {
        std::string idString = "";
        for (int id : info->streamIds_) {
            idString += (std::to_string(id) + ", ");
        }
        DHLOGI("StartCapture: ids=[%s], width=%d, height=%d, format=%d, type=%d.",
            (idString.empty() ? idString.c_str() : (idString.substr(0, idString.length() - INGNORE_STR_LEN)).c_str()),
            info->width_, info->height_, info->format_, info->type_);
    }
    return callback->StartCapture(dhBase, captureInfos);
}

DCamRetCode DCameraProvider::StopCapture(const std::shared_ptr<DHBase> &dhBase)
{
    DHLOGI("DCameraProvider::StopCapture for {devId: %s, dhId: %s}.",
        GetAnonyString(dhBase->deviceId_).c_str(), dhBase->dhId_.c_str());

    sptr<IDCameraProviderCallback> callback = GetCallbackBydhBase(dhBase);
    if (callback == nullptr) {
        DHLOGE("DCameraProvider::StopCapture, dcamera provider callback not found.");
        return DCamRetCode::INVALID_ARGUMENT;
    }

    return callback->StopCapture(dhBase);
}

DCamRetCode DCameraProvider::UpdateSettings(const std::shared_ptr<DHBase> &dhBase,
    const std::vector<std::shared_ptr<DCameraSettings>> &settings)
{
    DHLOGI("DCameraProvider::UpdateSettings for {devId: %s, dhId: %s}.",
        GetAnonyString(dhBase->deviceId_).c_str(), dhBase->dhId_.c_str());

    sptr<IDCameraProviderCallback> callback = GetCallbackBydhBase(dhBase);
    if (callback == nullptr) {
        DHLOGE("DCameraProvider::UpdateSettings, dcamera provider callback not found.");
        return DCamRetCode::INVALID_ARGUMENT;
    }

    return callback->UpdateSettings(dhBase, settings);
}

bool DCameraProvider::IsDhBaseInfoInvalid(const std::shared_ptr<DHBase> &dhBase)
{
    return dhBase->deviceId_.empty() || (dhBase->deviceId_.size() > DEVID_MAX_LENGTH) ||
        dhBase->dhId_.empty() || (dhBase->dhId_.size() > DHID_MAX_LENGTH);
}

sptr<IDCameraProviderCallback> DCameraProvider::GetCallbackBydhBase(const std::shared_ptr<DHBase> &dhBase)
{
    OHOS::sptr<DCameraDevice> device = GetDCameraDevice(dhBase);
    if (device == nullptr) {
        DHLOGE("DCameraProvider::GetCallbackBydhBase failed, dcamera device not found.");
        return nullptr;
    }
    return device->GetProviderCallback();
}

OHOS::sptr<DCameraDevice> DCameraProvider::GetDCameraDevice(const std::shared_ptr<DHBase> &dhBase)
{
    std::shared_ptr<DCameraHost> dCameraHost = DCameraHost::GetInstance();
    if (dCameraHost == nullptr) {
        DHLOGE("DCameraProvider::GetDCameraDevice, dcamera host is null.");
        return nullptr;
    }
    return dCameraHost->GetDCameraDeviceByDHBase(dhBase);
}
} // namespace DistributedHardware
} // namespace OHOS
