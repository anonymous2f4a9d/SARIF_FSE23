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

#include "dcamera_device.h"
#include <iostream>
#include <iterator>
#include <sstream>
#include "dcamera_host.h"
#include "dcamera_provider.h"
#include "dcamera_utils_tools.h"
#include "distributed_hardware_log.h"
#include "metadata_utils.h"

namespace OHOS {
namespace DistributedHardware {
using ErrorCallback = std::function<void (ErrorType, int32_t)>;
using ResultCallback = std::function<void (uint64_t, std::shared_ptr<CameraStandard::CameraMetadata>)>;
DCameraDevice::DCameraDevice(const std::shared_ptr<DHBase> &dhBase, const std::string &abilityInfo)
    : isOpened_(false),
      dCameraId_(GenerateCameraId(dhBase)),
      dhBase_(dhBase),
      dCameraAbilityInfo_(abilityInfo),
      dCameraDeviceCallback_(nullptr),
      dCameraStreamOperator_(nullptr),
      dMetadataProcessor_(nullptr)
{
    DHLOGI("DCameraDevice::ctor, instance = %p.", this);
    Init(abilityInfo);
}

void DCameraDevice::Init(const std::string &abilityInfo)
{
    if (dMetadataProcessor_ == nullptr) {
        dMetadataProcessor_ = std::make_shared<DMetadataProcessor>();
    }
    dMetadataProcessor_->InitDCameraAbility(abilityInfo);
}

DCamRetCode DCameraDevice::CreateDStreamOperator()
{
    if (dCameraStreamOperator_ == nullptr) {
        dCameraStreamOperator_ = new (std::nothrow) DStreamOperator(dMetadataProcessor_);
        if (dCameraStreamOperator_ == nullptr) {
            DHLOGE("Create distributed camera stream operator failed.");
            return DEVICE_NOT_INIT;
        }
    }

    DCamRetCode ret = dCameraStreamOperator_->InitOutputConfigurations(dhBase_, dCameraAbilityInfo_);
    if (ret != SUCCESS) {
        DHLOGE("Init distributed camera stream operator failed, ret=%d.", ret);
        return ret;
    }

    ErrorCallback onErrorCallback =
        [this](ErrorType type, int32_t errorMsg) -> void {
            if (dCameraDeviceCallback_) {
                dCameraDeviceCallback_->OnError(type, errorMsg);
            }
        };
    ResultCallback onResultCallback =
        [this](uint64_t timestamp, const std::shared_ptr<CameraStandard::CameraMetadata> &result) -> void {
            if (dCameraDeviceCallback_) {
                dCameraDeviceCallback_->OnResult(timestamp, result);
            }
        };
    dCameraStreamOperator_->SetDeviceCallback(onErrorCallback, onResultCallback);

    return ret;
}

CamRetCode DCameraDevice::GetStreamOperator(const OHOS::sptr<IStreamOperatorCallback> &callback,
    OHOS::sptr<IStreamOperator> &streamOperator)
{
    if (dCameraStreamOperator_ == nullptr) {
        DHLOGE("Distributed camera stream operator not init.");
        return CamRetCode::DEVICE_ERROR;
    }

    if (callback == nullptr) {
        DHLOGE("Input callback is null.");
        return CamRetCode::INVALID_ARGUMENT;
    }

    DCamRetCode ret = dCameraStreamOperator_->SetCallBack(callback);
    if (ret != SUCCESS) {
        DHLOGE("Set stream operator callback failed, ret=%d.", ret);
        return MapToExternalRetCode(ret);
    }

    streamOperator = dCameraStreamOperator_;
    return CamRetCode::NO_ERROR;
}

CamRetCode DCameraDevice::UpdateSettings(const std::shared_ptr<CameraSetting> &settings)
{
    if (settings == nullptr) {
        DHLOGE("DCameraDevice::UpdateSettings, input settings is null.");
        return CamRetCode::INVALID_ARGUMENT;
    }

    if (!IsOpened()) {
        DHLOGE("DCameraDevice::UpdateSettings, dcamera device %s already closed.", dCameraId_.c_str());
        return CamRetCode::CAMERA_CLOSED;
    }

    std::shared_ptr<DCameraSettings> dcSetting = std::make_shared<DCameraSettings>();

    dcSetting->type_ = DCSettingsType::UPDATE_METADATA;
    std::string abilityStr = CameraStandard::MetadataUtils::EncodeToString(settings);
    dcSetting->value_ = Base64Encode(reinterpret_cast<const unsigned char *>(abilityStr.c_str()), abilityStr.length());

    std::vector<std::shared_ptr<DCameraSettings>> dcSettings;
    dcSettings.push_back(dcSetting);

    std::shared_ptr<DCameraProvider> provider = DCameraProvider::GetInstance();
    if (provider == nullptr) {
        DHLOGE("Distributed camera provider instance is null.");
        return CamRetCode::DEVICE_ERROR;
    }
    DCamRetCode ret = provider->UpdateSettings(dhBase_, dcSettings);

    return MapToExternalRetCode(ret);
}

CamRetCode DCameraDevice::SetResultMode(const ResultCallbackMode &mode)
{
    if (dMetadataProcessor_ == nullptr) {
        DHLOGE("Metadata processor not init.");
        return CamRetCode::DEVICE_ERROR;
    }

    DCamRetCode ret = dMetadataProcessor_->SetMetadataResultMode(mode);
    if (ret != SUCCESS) {
        DHLOGE("Set metadata result mode failed, ret=%d.", ret);
    }
    return MapToExternalRetCode(ret);
}

CamRetCode DCameraDevice::GetEnabledResults(std::vector<MetaType> &results)
{
    if (dMetadataProcessor_ == nullptr) {
        DHLOGE("Metadata processor not init.");
        return CamRetCode::DEVICE_ERROR;
    }

    DCamRetCode ret = dMetadataProcessor_->GetEnabledMetadataResults(results);
    if (ret != SUCCESS) {
        DHLOGE("Get enabled metadata results failed, ret=%d.", ret);
    }
    return MapToExternalRetCode(ret);
}

CamRetCode DCameraDevice::EnableResult(const std::vector<MetaType> &results)
{
    if (dMetadataProcessor_ == nullptr) {
        DHLOGE("Metadata processor not init.");
        return CamRetCode::DEVICE_ERROR;
    }

    DCamRetCode ret = dMetadataProcessor_->EnableMetadataResult(results);
    if (ret != SUCCESS) {
        DHLOGE("Enable metadata result failed, ret=%d.", ret);
        return MapToExternalRetCode(ret);
    }

    stringstream sstream;
    std::reverse_copy(results.begin(), results.end(), ostream_iterator<int32_t>(sstream, ""));
    std::shared_ptr<DCameraSettings> dcSetting = std::make_shared<DCameraSettings>();
    dcSetting->type_ = DCSettingsType::ENABLE_METADATA;
    dcSetting->value_ = sstream.str();

    std::shared_ptr<DCameraProvider> provider = DCameraProvider::GetInstance();
    if (provider != nullptr) {
        std::vector<std::shared_ptr<DCameraSettings>> dcSettings;
        dcSettings.push_back(dcSetting);
        ret = provider->UpdateSettings(dhBase_, dcSettings);
    }
    return MapToExternalRetCode(ret);
}

CamRetCode DCameraDevice::DisableResult(const std::vector<MetaType> &results)
{
    if (dMetadataProcessor_ == nullptr) {
        DHLOGE("Metadata processor not init.");
        return CamRetCode::DEVICE_ERROR;
    }

    DCamRetCode ret = dMetadataProcessor_->DisableMetadataResult(results);
    if (ret != SUCCESS) {
        DHLOGE("Disable metadata result failed, ret=%d.", ret);
        return MapToExternalRetCode(ret);
    }

    stringstream sstream;
    std::reverse_copy(results.begin(), results.end(), ostream_iterator<int32_t>(sstream, ""));
    std::shared_ptr<DCameraSettings> dcSetting = std::make_shared<DCameraSettings>();
    dcSetting->type_ = DCSettingsType::DISABLE_METADATA;
    dcSetting->value_ = sstream.str();

    std::shared_ptr<DCameraProvider> provider = DCameraProvider::GetInstance();
    if (provider != nullptr) {
        std::vector<std::shared_ptr<DCameraSettings>> dcSettings;
        dcSettings.push_back(dcSetting);
        ret = provider->UpdateSettings(dhBase_, dcSettings);
    }
    return MapToExternalRetCode(ret);
}

void DCameraDevice::Close()
{
    DHLOGI("DCameraDevice::Close distributed camera: %s", dCameraId_.c_str());

    std::shared_ptr<DCameraProvider> provider = DCameraProvider::GetInstance();
    if (provider != nullptr) {
        provider->StopCapture(dhBase_);
    }
    if (dCameraStreamOperator_ != nullptr) {
        dCameraStreamOperator_->Release();
        dCameraStreamOperator_ = nullptr;
    }
    if (provider != nullptr) {
        provider->CloseSession(dhBase_);
    }
    if (dMetadataProcessor_ != nullptr) {
        dMetadataProcessor_->ResetEnableResults();
    }
    dCameraDeviceCallback_ = nullptr;
    isOpenSessFailed_ = false;
    isOpened_ = false;
}

CamRetCode DCameraDevice::OpenDCamera(const OHOS::sptr<ICameraDeviceCallback> &callback)
{
    if (callback == nullptr) {
        DHLOGE("Input callback is null.");
        return CamRetCode::INVALID_ARGUMENT;
    }
    dCameraDeviceCallback_ = callback;

    std::shared_ptr<DCameraProvider> provider = DCameraProvider::GetInstance();
    if (provider == nullptr) {
        DHLOGE("Get distributed camera provider instance is null.");
        return CamRetCode::DEVICE_ERROR;
    }
    DCamRetCode ret = provider->OpenSession(dhBase_);
    if (ret != DCamRetCode::SUCCESS) {
        DHLOGE("Open distributed camera control session failed, ret = %d.", ret);
        return MapToExternalRetCode(ret);
    }

    unique_lock<mutex> lock(openSesslock_);
    auto st = openSessCV_.wait_for(lock, chrono::seconds(WAIT_OPEN_TIMEOUT_SEC));
    if (st == cv_status::timeout) {
        DHLOGE("Wait for distributed camera session open timeout.");
        return CamRetCode::DEVICE_ERROR;
    }
    {
        unique_lock<mutex> lock(isOpenSessFailedlock_);
        if (isOpenSessFailed_) {
            DHLOGE("Open distributed camera session failed.");
            return CamRetCode::DEVICE_ERROR;
        }
    }

    ret = CreateDStreamOperator();
    if (ret != SUCCESS) {
        DHLOGE("Create distributed camera stream operator failed.");
        return MapToExternalRetCode(ret);
    }
    isOpened_ = true;

    return MapToExternalRetCode(ret);
}

CamRetCode DCameraDevice::GetDCameraAbility(std::shared_ptr<CameraAbility> &ability)
{
    if (dMetadataProcessor_ == nullptr) {
        DHLOGE("Metadata processor not init.");
        return CamRetCode::DEVICE_ERROR;
    }

    DCamRetCode ret = dMetadataProcessor_->GetDCameraAbility(ability);
    if (ret != SUCCESS) {
        DHLOGE("Get distributed camera ability failed, ret=%d.", ret);
    }
    return MapToExternalRetCode(ret);
}

DCamRetCode DCameraDevice::AcquireBuffer(int streamId, std::shared_ptr<DCameraBuffer> &buffer)
{
    if (dCameraStreamOperator_ == nullptr) {
        DHLOGE("Stream operator not init.");
        return DEVICE_NOT_INIT;
    }

    DCamRetCode ret = dCameraStreamOperator_->AcquireBuffer(streamId, buffer);
    if (ret != SUCCESS) {
        DHLOGE("Acquire buffer failed, ret=%d.", ret);
    }
    return ret;
}

DCamRetCode DCameraDevice::ShutterBuffer(int streamId, const std::shared_ptr<DCameraBuffer> &buffer)
{
    if (dCameraStreamOperator_ == nullptr) {
        DHLOGE("Stream operator not init.");
        return DEVICE_NOT_INIT;
    }

    DCamRetCode ret = dCameraStreamOperator_->ShutterBuffer(streamId, buffer);
    if (ret != SUCCESS) {
        DHLOGE("Shutter buffer failed, ret=%d.", ret);
    }
    return ret;
}

DCamRetCode DCameraDevice::OnSettingsResult(const std::shared_ptr<DCameraSettings> &result)
{
    if (result == nullptr) {
        DHLOGE("Input camera settings is null.");
        return INVALID_ARGUMENT;
    }

    if (dMetadataProcessor_ == nullptr) {
        DHLOGE("Metadata processor not init.");
        return DEVICE_NOT_INIT;
    }

    if (result->type_ != DCSettingsType::METADATA_RESULT) {
        DHLOGE("Invalid camera setting type = %d.", result->type_);
        return INVALID_ARGUMENT;
    }
    if ((result->value_).empty()) {
        DHLOGE("Camera settings result is empty.");
        return INVALID_ARGUMENT;
    }

    DCamRetCode ret = dMetadataProcessor_->SaveResultMetadata(result->value_);
    if (ret != SUCCESS) {
        DHLOGE("Save result metadata failed, ret = %d", ret);
    }
    return ret;
}

DCamRetCode DCameraDevice::Notify(const std::shared_ptr<DCameraHDFEvent> &event)
{
    DHLOGI("DCameraDevice::Notify for event type = %d, result = %d, content = %s.", event->type_, event->result_,
        event->content_.c_str());
    if ((event->type_ != DCameraEventType::DCAMERA_MESSAGE) && (event->type_ != DCameraEventType::DCAMERA_OPERATION)) {
        DHLOGE("Invalid distributed camera event type = %d.", event->type_);
        return INVALID_ARGUMENT;
    }
    switch (event->result_) {
        case DCameraEventResult::DCAMERA_EVENT_CHANNEL_CONNECTED: {
            {
                unique_lock<mutex> lock(isOpenSessFailedlock_);
                isOpenSessFailed_ = false;
            }
            openSessCV_.notify_one();
            break;
        }
        case DCameraEventResult::DCAMERA_EVENT_OPEN_CHANNEL_ERROR: {
            {
                unique_lock<mutex> lock(isOpenSessFailedlock_);
                isOpenSessFailed_ = true;
            }
            openSessCV_.notify_one();
            break;
        }
        case DCameraEventResult::DCAMERA_EVENT_CHANNEL_DISCONNECTED: {
            if (dCameraDeviceCallback_ != nullptr) {
                dCameraDeviceCallback_->OnError(ErrorType::FATAL_ERROR, 0);
                Close();
            }
            break;
        }
        case DCameraEventResult::DCAMERA_EVENT_CONFIG_STREAMS_ERROR:
        case DCameraEventResult::DCAMERA_EVENT_START_CAPTURE_ERROR: {
            if (dCameraDeviceCallback_ != nullptr) {
                dCameraDeviceCallback_->OnError(ErrorType::REQUEST_TIMEOUT, 0);
            }
            std::shared_ptr<DCameraProvider> provider = DCameraProvider::GetInstance();
            if (provider != nullptr) {
                provider->StopCapture(dhBase_);
            }
            if (dCameraStreamOperator_ != nullptr) {
                dCameraStreamOperator_->Release();
            }
            break;
        }
        case DCameraEventResult::DCAMERA_EVENT_CAMERA_ERROR: {
            std::shared_ptr<DCameraHost> dCameraHost = DCameraHost::GetInstance();
            if (dCameraHost != nullptr) {
                dCameraHost->NotifyDCameraStatus(dhBase_, event->result_);
            }
            if (dCameraDeviceCallback_ != nullptr) {
                dCameraDeviceCallback_->OnError(ErrorType::REQUEST_TIMEOUT, 0);
                Close();
            }
            break;
        }
        default:
            break;
    }
    return SUCCESS;
}

void DCameraDevice::SetProviderCallback(const OHOS::sptr<IDCameraProviderCallback> &callback)
{
    dCameraProviderCallback_ = callback;
}

OHOS::sptr<IDCameraProviderCallback> DCameraDevice::GetProviderCallback()
{
    return dCameraProviderCallback_;
}

std::string DCameraDevice::GenerateCameraId(const std::shared_ptr<DHBase> &dhBase)
{
    return dhBase->deviceId_ + "__" + dhBase->dhId_;
}

std::string DCameraDevice::GetDCameraId()
{
    return dCameraId_;
}

bool DCameraDevice::IsOpened()
{
    return isOpened_;
}
} // namespace DistributedHardware
} // namespace OHOS
