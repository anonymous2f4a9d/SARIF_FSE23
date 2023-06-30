/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "dcamera_client.h"

#include "anonymous_string.h"
#include "dcamera_input_callback.h"
#include "dcamera_manager_callback.h"
#include "dcamera_photo_callback.h"
#include "dcamera_session_callback.h"
#include "dcamera_utils_tools.h"
#include "dcamera_video_callback.h"
#include "distributed_camera_constants.h"
#include "distributed_camera_errno.h"
#include "distributed_hardware_log.h"
#include "metadata_utils.h"

namespace OHOS {
namespace DistributedHardware {
DCameraClient::DCameraClient(const std::string& dhId)
{
    DHLOGI("DCameraClient Constructor dhId: %s", GetAnonyString(dhId).c_str());
    cameraId_ = dhId.substr(CAMERA_ID_PREFIX.size());
    isInit_ = false;
}

DCameraClient::~DCameraClient()
{
    if (isInit_) {
        UnInit();
    }
}

int32_t DCameraClient::Init()
{
    DHLOGI("DCameraClient::Init cameraId: %s", GetAnonyString(cameraId_).c_str());
    cameraManager_ = CameraStandard::CameraManager::GetInstance();
    if (cameraManager_ == nullptr) {
        DHLOGE("DCameraClient::Init cameraManager getInstance failed");
        return DCAMERA_BAD_VALUE;
    }
    cameraManager_->SetCallback(std::make_shared<DCameraManagerCallback>());

    std::vector<sptr<CameraStandard::CameraInfo>> cameraList = cameraManager_->GetCameras();
    DHLOGI("DCameraClient::Init camera size: %d", cameraList.size());
    for (auto& info : cameraList) {
        if (info->GetID() == cameraId_) {
            DHLOGI("DCameraClient::Init cameraInfo get id: %s", GetAnonyString(info->GetID()).c_str());
            cameraInfo_ = info;
            break;
        }
    }
    if (cameraInfo_ == nullptr) {
        DHLOGE("DCameraClient::Init cameraInfo is null");
        return DCAMERA_BAD_VALUE;
    }

    isInit_ = true;
    DHLOGI("DCameraClient::Init %s success", GetAnonyString(cameraId_).c_str());
    return DCAMERA_OK;
}

int32_t DCameraClient::UnInit()
{
    DHLOGI("DCameraClient::UnInit cameraId: %s", GetAnonyString(cameraId_).c_str());
    if (cameraManager_ != nullptr) {
        DHLOGI("DCameraClient::UnInit unregister cameraManager callback");
        cameraManager_->SetCallback(nullptr);
    }

    isInit_ = false;
    cameraInfo_ = nullptr;
    cameraManager_ = nullptr;
    DHLOGI("DCameraClient::UnInit %s success", GetAnonyString(cameraId_).c_str());
    return DCAMERA_OK;
}

int32_t DCameraClient::UpdateSettings(std::vector<std::shared_ptr<DCameraSettings>>& settings)
{
    DHLOGI("DCameraClient::UpdateCameraSettings cameraId: %s", GetAnonyString(cameraId_).c_str());
    for (auto& setting : settings) {
        switch (setting->type_) {
            case UPDATE_METADATA: {
                DHLOGI("DCameraClient::UpdateCameraSettings %s update metadata settings",
                       GetAnonyString(cameraId_).c_str());
                std::string metadataStr = Base64Decode(setting->value_);
                int32_t ret = ((sptr<CameraStandard::CameraInput> &)cameraInput_)->SetCameraSettings(metadataStr);
                if (ret != DCAMERA_OK) {
                    DHLOGE("DCameraClient::UpdateSettings %s update metadata settings failed, ret: %d",
                           GetAnonyString(cameraId_).c_str(), ret);
                    return ret;
                }
                break;
            }
            default: {
                DHLOGE("DCameraClient::UpdateSettings unknown setting type");
                break;
            }
        }
    }
    DHLOGI("DCameraClient::UpdateCameraSettings %s success", GetAnonyString(cameraId_).c_str());
    return DCAMERA_OK;
}

int32_t DCameraClient::StartCapture(std::vector<std::shared_ptr<DCameraCaptureInfo>>& captureInfos)
{
    DHLOGI("DCameraClient::StartCapture cameraId: %s", GetAnonyString(cameraId_).c_str());
    if ((photoOutput_ == nullptr) && (videoOutput_ == nullptr)) {
        DHLOGI("DCameraClient::StartCapture %s config capture session", GetAnonyString(cameraId_).c_str());
        int32_t ret = ConfigCaptureSession(captureInfos);
        if (ret != DCAMERA_OK) {
            DHLOGE("DCameraClient::StartCapture config capture session failed, cameraId: %s, ret: %d",
                   GetAnonyString(cameraId_).c_str(), ret);
            return ret;
        }
    }

    for (auto& info : captureInfos) {
        if (!(info->isCapture_)) {
            continue;
        }
        int32_t ret = StartCaptureInner(info);
        if (ret != DCAMERA_OK) {
            DHLOGE("DCameraClient::StartCapture failed, cameraId: %s, ret: %d", GetAnonyString(cameraId_).c_str(), ret);
            return ret;
        }
    }
    DHLOGI("DCameraClient::StartCapture %s success", GetAnonyString(cameraId_).c_str());
    return DCAMERA_OK;
}

int32_t DCameraClient::StopCapture()
{
    DHLOGI("DCameraClient::StopCapture cameraId: %s", GetAnonyString(cameraId_).c_str());
    if (videoOutput_ != nullptr) {
        DHLOGI("DCameraClient::StopCapture %s stop videoOutput", GetAnonyString(cameraId_).c_str());
        int32_t ret = ((sptr<CameraStandard::VideoOutput> &)videoOutput_)->Stop();
        if (ret != DCAMERA_OK) {
            DHLOGE("DCameraClient::StopCapture videoOutput stop failed, cameraId: %s, ret: %d",
                   GetAnonyString(cameraId_).c_str(), ret);
        }
    }

    if (captureSession_ != nullptr) {
        DHLOGI("DCameraClient::StopCapture %s stop captureSession", GetAnonyString(cameraId_).c_str());
        int32_t ret = captureSession_->Stop();
        if (ret != DCAMERA_OK) {
            DHLOGE("DCameraClient::StopCapture captureSession stop failed, cameraId: %s, ret: %d",
                   GetAnonyString(cameraId_).c_str(), ret);
        }
        DHLOGI("DCameraClient::StopCapture %s release captureSession", GetAnonyString(cameraId_).c_str());
        captureSession_->Release();
    }

    if (cameraInput_ != nullptr) {
        DHLOGI("DCameraClient::StopCapture %s release cameraInput", GetAnonyString(cameraId_).c_str());
        cameraInput_->Release();
    }

    photoOutput_ = nullptr;
    videoOutput_ = nullptr;
    cameraInput_ = nullptr;
    captureSession_ = nullptr;
    DHLOGI("DCameraClient::StopCapture %s success", GetAnonyString(cameraId_).c_str());
    return DCAMERA_OK;
}

int32_t DCameraClient::SetStateCallback(std::shared_ptr<StateCallback>& callback)
{
    DHLOGI("DCameraClient::SetStateCallback cameraId: %s", GetAnonyString(cameraId_).c_str());
    if (callback == nullptr) {
        DHLOGE("DCameraClient::SetStateCallback %s unregistering state callback", GetAnonyString(cameraId_).c_str());
    }
    stateCallback_ = callback;
    return DCAMERA_OK;
}

int32_t DCameraClient::SetResultCallback(std::shared_ptr<ResultCallback>& callback)
{
    DHLOGI("DCameraClient::SetResultCallback cameraId: %s", GetAnonyString(cameraId_).c_str());
    if (callback == nullptr) {
        DHLOGE("DCameraClient::SetResultCallback %s unregistering result callback", GetAnonyString(cameraId_).c_str());
    }
    resultCallback_ = callback;
    return DCAMERA_OK;
}

int32_t DCameraClient::ConfigCaptureSession(std::vector<std::shared_ptr<DCameraCaptureInfo>>& captureInfos)
{
    DHLOGI("DCameraClient::ConfigCaptureSession cameraId: %s", GetAnonyString(cameraId_).c_str());
    cameraInput_ = cameraManager_->CreateCameraInput(cameraInfo_);
    if (cameraInput_ == nullptr) {
        DHLOGE("DCameraClient::ConfigCaptureSession %s create cameraInput failed", GetAnonyString(cameraId_).c_str());
        return DCAMERA_BAD_VALUE;
    }
    std::shared_ptr<DCameraInputCallback> inputCallback = std::make_shared<DCameraInputCallback>(stateCallback_);
    ((sptr<CameraStandard::CameraInput> &)cameraInput_)->SetErrorCallback(inputCallback);

    captureSession_ = cameraManager_->CreateCaptureSession();
    if (captureSession_ == nullptr) {
        DHLOGE("DCameraClient::ConfigCaptureSession %s create captureSession failed",
               GetAnonyString(cameraId_).c_str());
        return DCAMERA_BAD_VALUE;
    }
    captureSession_->SetCallback(std::make_shared<DCameraSessionCallback>(stateCallback_));

    int32_t ret = CreateCaptureOutput(captureInfos);
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraClient::ConfigCaptureSession create capture output failed, cameraId: %s, ret: %d",
               GetAnonyString(cameraId_).c_str(), ret);
        return ret;
    }

    return ConfigCaptureSessionInner();
}

int32_t DCameraClient::ConfigCaptureSessionInner()
{
    int32_t ret = captureSession_->BeginConfig();
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraClient::ConfigCaptureSession %s config captureSession failed, ret: %d",
               GetAnonyString(cameraId_).c_str(), ret);
        return ret;
    }

    ret = captureSession_->AddInput(cameraInput_);
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraClient::ConfigCaptureSession %s add cameraInput to captureSession failed, ret: %d",
               GetAnonyString(cameraId_).c_str(), ret);
        return ret;
    }

    if (photoOutput_ != nullptr) {
        ret = captureSession_->AddOutput(photoOutput_);
        if (ret != DCAMERA_OK) {
            DHLOGE("DCameraClient::ConfigCaptureSession %s add photoOutput to captureSession failed, ret: %d",
                   GetAnonyString(cameraId_).c_str(), ret);
            return ret;
        }
    }

    if (videoOutput_ != nullptr) {
        ret = captureSession_->AddOutput(videoOutput_);
        if (ret != DCAMERA_OK) {
            DHLOGE("DCameraClient::ConfigCaptureSession %s add videoOutput to captureSession failed, ret: %d",
                   GetAnonyString(cameraId_).c_str(), ret);
            return ret;
        }
    }

    ret = captureSession_->CommitConfig();
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraClient::ConfigCaptureSession %s commit captureSession failed, ret: %d",
               GetAnonyString(cameraId_).c_str(), ret);
        return ret;
    }

    ret = captureSession_->Start();
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraClient::ConfigCaptureSession %s start captureSession failed, ret: %d",
               GetAnonyString(cameraId_).c_str(), ret);
    }

    DHLOGI("DCameraClient::ConfigCaptureSession %s success", GetAnonyString(cameraId_).c_str());
    return DCAMERA_OK;
}

int32_t DCameraClient::CreateCaptureOutput(std::vector<std::shared_ptr<DCameraCaptureInfo>>& captureInfos)
{
    if (captureInfos.empty()) {
        DHLOGE("DCameraClient::CreateCaptureOutput no capture info, cameraId: %s", GetAnonyString(cameraId_).c_str());
        return DCAMERA_BAD_VALUE;
    }

    for (auto& info : captureInfos) {
        if (info->streamType_ == SNAPSHOT_FRAME) {
            int32_t ret = CreatePhotoOutput(info);
            if (ret != DCAMERA_OK) {
                DHLOGE("DCameraClient::CreateCaptureOutput %s create photo output failed, ret: %d",
                       GetAnonyString(cameraId_).c_str(), ret);
                return ret;
            }
        } else if (info->streamType_ == CONTINUOUS_FRAME) {
            int32_t ret = CreateVideoOutput(info);
            if (ret != DCAMERA_OK) {
                DHLOGE("DCameraClient::CreateCaptureOutput %s create video output failed, ret: %d",
                       GetAnonyString(cameraId_).c_str(), ret);
                return ret;
            }
        } else {
            DHLOGE("DCameraClient::CreateCaptureOutput unknown stream type");
            return DCAMERA_BAD_VALUE;
        }
    }
    return DCAMERA_OK;
}

int32_t DCameraClient::CreatePhotoOutput(std::shared_ptr<DCameraCaptureInfo>& info)
{
    DHLOGI("DCameraClient::CreatePhotoOutput camId: %s, width: %d, height: %d, format: %d, stream: %d, isCapture: %d",
           GetAnonyString(cameraId_).c_str(), info->width_, info->height_, info->format_,
           info->streamType_, info->isCapture_);
    photoSurface_ = Surface::CreateSurfaceAsConsumer();
    photoSurface_->SetDefaultWidthAndHeight(info->width_, info->height_);
    photoSurface_->SetUserData(CAMERA_SURFACE_FORMAT, std::to_string(info->format_));
    photoListener_ = std::make_shared<DCameraPhotoSurfaceListener>(photoSurface_, resultCallback_);
    photoSurface_->RegisterConsumerListener((sptr<IBufferConsumerListener> &)photoListener_);
    photoOutput_ = cameraManager_->CreatePhotoOutput(photoSurface_);
    if (photoOutput_ == nullptr) {
        DHLOGE("DCameraClient::CreatePhotoOutput %s create photo output failed", GetAnonyString(cameraId_).c_str());
        return DCAMERA_BAD_VALUE;
    }
    std::shared_ptr<DCameraPhotoCallback> photoCallback = std::make_shared<DCameraPhotoCallback>(stateCallback_);
    ((sptr<CameraStandard::PhotoOutput> &)photoOutput_)->SetCallback(photoCallback);
    DHLOGI("DCameraClient::CreatePhotoOutput %s success", GetAnonyString(cameraId_).c_str());
    return DCAMERA_OK;
}

int32_t DCameraClient::CreateVideoOutput(std::shared_ptr<DCameraCaptureInfo>& info)
{
    DHLOGI("DCameraClient::CreateVideoOutput camId: %s, width: %d, height: %d, format: %d, stream: %d, isCapture: %d",
           GetAnonyString(cameraId_).c_str(), info->width_, info->height_, info->format_,
           info->streamType_, info->isCapture_);
    videoSurface_ = Surface::CreateSurfaceAsConsumer();
    videoSurface_->SetDefaultWidthAndHeight(info->width_, info->height_);
    videoSurface_->SetUserData(CAMERA_SURFACE_FORMAT, std::to_string(info->format_));
    videoListener_ = std::make_shared<DCameraVideoSurfaceListener>(videoSurface_, resultCallback_);
    videoSurface_->RegisterConsumerListener((sptr<IBufferConsumerListener> &)videoListener_);
    videoOutput_ = cameraManager_->CreateVideoOutput(videoSurface_);
    if (videoOutput_ == nullptr) {
        DHLOGE("DCameraClient::CreateVideoOutput %s create video output failed", GetAnonyString(cameraId_).c_str());
        return DCAMERA_BAD_VALUE;
    }
    std::shared_ptr<DCameraVideoCallback> videoCallback = std::make_shared<DCameraVideoCallback>(stateCallback_);
    ((sptr<CameraStandard::VideoOutput> &)videoOutput_)->SetCallback(videoCallback);
    DHLOGI("DCameraClient::CreateVideoOutput %s success", GetAnonyString(cameraId_).c_str());
    return DCAMERA_OK;
}

int32_t DCameraClient::StartCaptureInner(std::shared_ptr<DCameraCaptureInfo>& info)
{
    switch (info->streamType_) {
        case CONTINUOUS_FRAME: {
            return StartVideoOutput();
        }
        case SNAPSHOT_FRAME: {
            return StartPhotoOutput(info);
        }
        default: {
            DHLOGE("DCameraClient::StartCaptureInner unknown stream type");
            return DCAMERA_BAD_VALUE;
        }
    }
}

int32_t DCameraClient::StartPhotoOutput(std::shared_ptr<DCameraCaptureInfo>& info)
{
    DHLOGI("DCameraClient::StartPhotoOutput cameraId: %s", GetAnonyString(cameraId_).c_str());
    if (photoOutput_ == nullptr) {
        DHLOGE("DCameraClient::StartPhotoOutput photoOutput is null");
        return DCAMERA_BAD_VALUE;
    }

    std::vector<std::shared_ptr<DCameraSettings>> captureSettings = info->captureSettings_;
    std::string metadataSetting;
    for (auto& setting : captureSettings) {
        if (setting->type_ == UPDATE_METADATA) {
            DHLOGI("DCameraClient::StartPhotoOutput %s update metadata settings", GetAnonyString(cameraId_).c_str());
            metadataSetting = setting->value_;
        }
    }

    if (metadataSetting.empty()) {
        DHLOGE("DCameraClient::StartPhotoOutput no metadata settings to update");
        int32_t ret = ((sptr<CameraStandard::PhotoOutput> &)photoOutput_)->Capture();
        if (ret != DCAMERA_OK) {
            DHLOGE("DCameraClient::StartPhotoOutput %s photoOutput capture failed, ret: %d",
                   GetAnonyString(cameraId_).c_str(), ret);
            return ret;
        }
        return DCAMERA_OK;
    }

    camera_metadata_item_t item;
    CameraStandard::PhotoCaptureSetting::RotationConfig rotation =
            CameraStandard::PhotoCaptureSetting::RotationConfig::Rotation_0;
    std::shared_ptr<CameraStandard::CameraMetadata> cameraMetadata =
            CameraStandard::MetadataUtils::DecodeFromString(Base64Decode(metadataSetting));
    int32_t ret = CameraStandard::FindCameraMetadataItem(cameraMetadata->get(), OHOS_JPEG_ORIENTATION, &item);
    if (ret == DCAMERA_OK) {
        DHLOGI("DCameraClient::StartPhotoOutput %s find camera metadata item", GetAnonyString(cameraId_).c_str());
        rotation = static_cast<CameraStandard::PhotoCaptureSetting::RotationConfig>(item.data.i32[0]);
    }

    DHLOGI("DCameraClient::StartPhotoOutput %s photo capture settings set rotation: %d",
           GetAnonyString(cameraId_).c_str(), rotation);
    std::shared_ptr<CameraStandard::PhotoCaptureSetting> photoCaptureSettings =
            std::make_shared<CameraStandard::PhotoCaptureSetting>();
    photoCaptureSettings->SetRotation(rotation);
    ret = ((sptr<CameraStandard::PhotoOutput> &)photoOutput_)->Capture(photoCaptureSettings);
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraClient::StartPhotoOutput %s photoOutput capture failed, ret: %d",
               GetAnonyString(cameraId_).c_str(), ret);
        return ret;
    }
    return DCAMERA_OK;
}

int32_t DCameraClient::StartVideoOutput()
{
    DHLOGI("DCameraClient::StartVideoOutput cameraId: %s", GetAnonyString(cameraId_).c_str());
    if (videoOutput_ == nullptr) {
        DHLOGE("DCameraClient::StartVideoOutput videoOutput is null");
        return DCAMERA_BAD_VALUE;
    }
    int32_t ret = ((sptr<CameraStandard::VideoOutput> &)videoOutput_)->Start();
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraClient::StartVideoOutput %s videoOutput start failed, ret: %d",
               GetAnonyString(cameraId_).c_str(), ret);
        return ret;
    }
    return DCAMERA_OK;
}
} // namespace DistributedHardware
} // namespace OHOS