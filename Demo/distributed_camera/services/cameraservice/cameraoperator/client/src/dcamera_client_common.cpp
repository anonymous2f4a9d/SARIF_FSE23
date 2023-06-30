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
#include "dcamera_preview_callback.h"
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
    DHLOGI("DCameraClientCommon Constructor dhId: %s", GetAnonyString(dhId).c_str());
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
    DHLOGI("DCameraClientCommon::Init cameraId: %s", GetAnonyString(cameraId_).c_str());
    cameraManager_ = CameraStandard::CameraManager::GetInstance();
    if (cameraManager_ == nullptr) {
        DHLOGE("DCameraClientCommon::Init cameraManager getInstance failed");
        return DCAMERA_BAD_VALUE;
    }
    cameraManager_->SetCallback(std::make_shared<DCameraManagerCallback>());

    std::vector<sptr<CameraStandard::CameraInfo>> cameraList = cameraManager_->GetCameras();
    DHLOGI("DCameraClientCommon::Init camera size: %d", cameraList.size());
    for (auto& info : cameraList) {
        if (info->GetID() == cameraId_) {
            DHLOGI("DCameraClientCommon::Init cameraInfo get id: %s", GetAnonyString(info->GetID()).c_str());
            cameraInfo_ = info;
            break;
        }
    }
    if (cameraInfo_ == nullptr) {
        DHLOGE("DCameraClientCommon::Init cameraInfo is null");
        return DCAMERA_BAD_VALUE;
    }

    isInit_ = true;
    DHLOGI("DCameraClientCommon::Init %s success", GetAnonyString(cameraId_).c_str());
    return DCAMERA_OK;
}

int32_t DCameraClient::UnInit()
{
    DHLOGI("DCameraClientCommon::UnInit cameraId: %s", GetAnonyString(cameraId_).c_str());
    if (cameraManager_ != nullptr) {
        DHLOGI("DCameraClientCommon::UnInit unregister cameraManager callback");
        cameraManager_->SetCallback(nullptr);
    }

    isInit_ = false;
    cameraInfo_ = nullptr;
    cameraManager_ = nullptr;
    DHLOGI("DCameraClientCommon::UnInit %s success", GetAnonyString(cameraId_).c_str());
    return DCAMERA_OK;
}

int32_t DCameraClient::UpdateSettings(std::vector<std::shared_ptr<DCameraSettings>>& settings)
{
    DHLOGI("DCameraClientCommon::UpdateCameraSettings cameraId: %s", GetAnonyString(cameraId_).c_str());
    for (auto& setting : settings) {
        switch (setting->type_) {
            case UPDATE_METADATA: {
                DHLOGI("DCameraClientCommon::UpdateCameraSettings %s update metadata settings",
                       GetAnonyString(cameraId_).c_str());
                std::string metadataStr = Base64Decode(setting->value_);
                int32_t ret = ((sptr<CameraStandard::CameraInput> &)cameraInput_)->SetCameraSettings(metadataStr);
                if (ret != DCAMERA_OK) {
                    DHLOGE("DCameraClientCommon::UpdateSettings %s update metadata settings failed, ret: %d",
                           GetAnonyString(cameraId_).c_str(), ret);
                    return ret;
                }
                break;
            }
            default: {
                DHLOGE("DCameraClientCommon::UpdateSettings unknown setting type");
                break;
            }
        }
    }
    DHLOGI("DCameraClientCommon::UpdateCameraSettings %s success", GetAnonyString(cameraId_).c_str());
    return DCAMERA_OK;
}

int32_t DCameraClient::StartCapture(std::vector<std::shared_ptr<DCameraCaptureInfo>>& captureInfos)
{
    DHLOGI("DCameraClientCommon::StartCapture cameraId: %s", GetAnonyString(cameraId_).c_str());
    if ((photoOutput_ == nullptr) && (previewOutput_ == nullptr)) {
        DHLOGI("DCameraClientCommon::StartCapture %s config capture session", GetAnonyString(cameraId_).c_str());
        int32_t ret = ConfigCaptureSession(captureInfos);
        if (ret != DCAMERA_OK) {
            DHLOGE("DCameraClientCommon::StartCapture config capture session failed, cameraId: %s, ret: %d",
                   GetAnonyString(cameraId_).c_str(), ret);
            return ret;
        }
    }

    for (auto& info : captureInfos) {
        if ((info->streamType_ == CONTINUOUS_FRAME) || (!info->isCapture_)) {
            continue;
        }
        int32_t ret = StartCaptureInner(info);
        if (ret != DCAMERA_OK) {
            DHLOGE("DCameraClientCommon::StartCapture failed, cameraId: %s, ret: %d",
                GetAnonyString(cameraId_).c_str(), ret);
            return ret;
        }
    }
    DHLOGI("DCameraClientCommon::StartCapture %s success", GetAnonyString(cameraId_).c_str());
    return DCAMERA_OK;
}

int32_t DCameraClient::StopCapture()
{
    DHLOGI("DCameraClientCommon::StopCapture cameraId: %s", GetAnonyString(cameraId_).c_str());
    if (captureSession_ != nullptr) {
        DHLOGI("DCameraClientCommon::StopCapture %s stop captureSession", GetAnonyString(cameraId_).c_str());
        int32_t ret = captureSession_->Stop();
        if (ret != DCAMERA_OK) {
            DHLOGE("DCameraClientCommon::StopCapture captureSession stop failed, cameraId: %s, ret: %d",
                   GetAnonyString(cameraId_).c_str(), ret);
        }
        DHLOGI("DCameraClientCommon::StopCapture %s release captureSession", GetAnonyString(cameraId_).c_str());
        captureSession_->Release();
    }

    if (cameraInput_ != nullptr) {
        DHLOGI("DCameraClientCommon::StopCapture %s release cameraInput", GetAnonyString(cameraId_).c_str());
        cameraInput_->Release();
    }

    photoOutput_ = nullptr;
    previewOutput_ = nullptr;
    cameraInput_ = nullptr;
    captureSession_ = nullptr;
    DHLOGI("DCameraClientCommon::StopCapture %s success", GetAnonyString(cameraId_).c_str());
    return DCAMERA_OK;
}

int32_t DCameraClient::SetStateCallback(std::shared_ptr<StateCallback>& callback)
{
    DHLOGI("DCameraClientCommon::SetStateCallback cameraId: %s", GetAnonyString(cameraId_).c_str());
    if (callback == nullptr) {
        DHLOGE("DCameraClientCommon::SetStateCallback %s unregistering state callback",
            GetAnonyString(cameraId_).c_str());
    }
    stateCallback_ = callback;
    return DCAMERA_OK;
}

int32_t DCameraClient::SetResultCallback(std::shared_ptr<ResultCallback>& callback)
{
    DHLOGI("DCameraClientCommon::SetResultCallback cameraId: %s", GetAnonyString(cameraId_).c_str());
    if (callback == nullptr) {
        DHLOGE("DCameraClientCommon::SetResultCallback %s unregistering result callback",
            GetAnonyString(cameraId_).c_str());
    }
    resultCallback_ = callback;
    return DCAMERA_OK;
}

int32_t DCameraClient::ConfigCaptureSession(std::vector<std::shared_ptr<DCameraCaptureInfo>>& captureInfos)
{
    DHLOGI("DCameraClientCommon::ConfigCaptureSession cameraId: %s", GetAnonyString(cameraId_).c_str());
    cameraInput_ = cameraManager_->CreateCameraInput(cameraInfo_);
    if (cameraInput_ == nullptr) {
        DHLOGE("DCameraClientCommon::ConfigCaptureSession %s create cameraInput failed",
            GetAnonyString(cameraId_).c_str());
        return DCAMERA_BAD_VALUE;
    }
    std::shared_ptr<DCameraInputCallback> inputCallback = std::make_shared<DCameraInputCallback>(stateCallback_);
    ((sptr<CameraStandard::CameraInput> &)cameraInput_)->SetErrorCallback(inputCallback);

    captureSession_ = cameraManager_->CreateCaptureSession();
    if (captureSession_ == nullptr) {
        DHLOGE("DCameraClientCommon::ConfigCaptureSession %s create captureSession failed",
               GetAnonyString(cameraId_).c_str());
        return DCAMERA_BAD_VALUE;
    }
    captureSession_->SetCallback(std::make_shared<DCameraSessionCallback>(stateCallback_));

    int32_t ret = CreateCaptureOutput(captureInfos);
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraClientCommon::ConfigCaptureSession create capture output failed, cameraId: %s, ret: %d",
               GetAnonyString(cameraId_).c_str(), ret);
        return ret;
    }

    return ConfigCaptureSessionInner();
}

int32_t DCameraClient::ConfigCaptureSessionInner()
{
    int32_t ret = captureSession_->BeginConfig();
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraClientCommon::ConfigCaptureSession %s config captureSession failed, ret: %d",
               GetAnonyString(cameraId_).c_str(), ret);
        return ret;
    }

    ret = captureSession_->AddInput(cameraInput_);
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraClientCommon::ConfigCaptureSession %s add cameraInput to captureSession failed, ret: %d",
               GetAnonyString(cameraId_).c_str(), ret);
        return ret;
    }

    if (photoOutput_ != nullptr) {
        ret = captureSession_->AddOutput(photoOutput_);
        if (ret != DCAMERA_OK) {
            DHLOGE("DCameraClientCommon::ConfigCaptureSession %s add photoOutput to captureSession failed, ret: %d",
                   GetAnonyString(cameraId_).c_str(), ret);
            return ret;
        }
    }

    if (previewOutput_ != nullptr) {
        ret = captureSession_->AddOutput(previewOutput_);
        if (ret != DCAMERA_OK) {
            DHLOGE("DCameraClientCommon::ConfigCaptureSession %s add previewOutput to captureSession failed, ret: %d",
                   GetAnonyString(cameraId_).c_str(), ret);
            return ret;
        }
    }

    ret = captureSession_->CommitConfig();
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraClientCommon::ConfigCaptureSession %s commit captureSession failed, ret: %d",
               GetAnonyString(cameraId_).c_str(), ret);
        return ret;
    }

    ret = captureSession_->Start();
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraClientCommon::ConfigCaptureSession %s start captureSession failed, ret: %d",
               GetAnonyString(cameraId_).c_str(), ret);
    }

    DHLOGI("DCameraClientCommon::ConfigCaptureSession %s success", GetAnonyString(cameraId_).c_str());
    return DCAMERA_OK;
}

int32_t DCameraClient::CreateCaptureOutput(std::vector<std::shared_ptr<DCameraCaptureInfo>>& captureInfos)
{
    if (captureInfos.empty()) {
        DHLOGE("DCameraClientCommon::CreateCaptureOutput no capture info, cameraId: %s",
            GetAnonyString(cameraId_).c_str());
        return DCAMERA_BAD_VALUE;
    }

    for (auto& info : captureInfos) {
        if (info->streamType_ == SNAPSHOT_FRAME) {
            int32_t ret = CreatePhotoOutput(info);
            if (ret != DCAMERA_OK) {
                DHLOGE("DCameraClientCommon::CreateCaptureOutput %s create photo output failed, ret: %d",
                       GetAnonyString(cameraId_).c_str(), ret);
                return ret;
            }
        } else if (info->streamType_ == CONTINUOUS_FRAME) {
            int32_t ret = CreateVideoOutput(info);
            if (ret != DCAMERA_OK) {
                DHLOGE("DCameraClientCommon::CreateCaptureOutput %s create video output failed, ret: %d",
                       GetAnonyString(cameraId_).c_str(), ret);
                return ret;
            }
        } else {
            DHLOGE("DCameraClientCommon::CreateCaptureOutput unknown stream type");
            return DCAMERA_BAD_VALUE;
        }
    }
    return DCAMERA_OK;
}

int32_t DCameraClient::CreatePhotoOutput(std::shared_ptr<DCameraCaptureInfo>& info)
{
    DHLOGI("DCameraClientCommon::CreatePhotoOutput camId: %s, w: %d, h: %d, f: %d, stream: %d, isCapture: %d",
           GetAnonyString(cameraId_).c_str(), info->width_, info->height_,
           camera_format_t::OHOS_CAMERA_FORMAT_RGBA_8888, info->streamType_, info->isCapture_);
    photoSurface_ = Surface::CreateSurfaceAsConsumer();
    photoSurface_->SetDefaultWidthAndHeight(info->width_, info->height_);
    photoSurface_->SetUserData(CAMERA_SURFACE_FORMAT, std::to_string(camera_format_t::OHOS_CAMERA_FORMAT_RGBA_8888));
    photoListener_ = std::make_shared<DCameraPhotoSurfaceListener>(photoSurface_, resultCallback_);
    photoSurface_->RegisterConsumerListener((sptr<IBufferConsumerListener> &)photoListener_);
    photoOutput_ = cameraManager_->CreatePhotoOutput(photoSurface_);
    if (photoOutput_ == nullptr) {
        DHLOGE("DCameraClientCommon::CreatePhotoOutput %s create photo output failed",
            GetAnonyString(cameraId_).c_str());
        return DCAMERA_BAD_VALUE;
    }
    std::shared_ptr<DCameraPhotoCallback> photoCallback = std::make_shared<DCameraPhotoCallback>(stateCallback_);
    ((sptr<CameraStandard::PhotoOutput> &)photoOutput_)->SetCallback(photoCallback);
    DHLOGI("DCameraClientCommon::CreatePhotoOutput %s success", GetAnonyString(cameraId_).c_str());
    return DCAMERA_OK;
}

int32_t DCameraClient::CreateVideoOutput(std::shared_ptr<DCameraCaptureInfo>& info)
{
    DHLOGI("DCameraClientCommon::CreatePreviewOutput camId: %s, w: %d, h: %d, f: %d, stream: %d, isCapture: %d",
           GetAnonyString(cameraId_).c_str(), info->width_, info->height_,
           camera_format_t::OHOS_CAMERA_FORMAT_RGBA_8888, info->streamType_, info->isCapture_);
    videoSurface_ = Surface::CreateSurfaceAsConsumer();
    videoSurface_->SetDefaultWidthAndHeight(info->width_, info->height_);
    videoSurface_->SetUserData(CAMERA_SURFACE_FORMAT, std::to_string(camera_format_t::OHOS_CAMERA_FORMAT_RGBA_8888));
    videoListener_ = std::make_shared<DCameraVideoSurfaceListener>(videoSurface_, resultCallback_);
    videoSurface_->RegisterConsumerListener((sptr<IBufferConsumerListener> &)videoListener_);
    previewOutput_ = cameraManager_->CreateCustomPreviewOutput(videoSurface_, info->width_, info->height_);
    if (previewOutput_ == nullptr) {
        DHLOGE("DCameraClientCommon::CreatePreviewOutput %s create preview output failed",
            GetAnonyString(cameraId_).c_str());
        return DCAMERA_BAD_VALUE;
    }
    std::shared_ptr<DCameraPreviewCallback> previewCallback = std::make_shared<DCameraPreviewCallback>(stateCallback_);
    ((sptr<CameraStandard::PreviewOutput> &)previewOutput_)->SetCallback(previewCallback);
    DHLOGI("DCameraClientCommon::CreatePreviewOutput %s success", GetAnonyString(cameraId_).c_str());
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
            DHLOGE("DCameraClientCommon::StartCaptureInner unknown stream type");
            return DCAMERA_BAD_VALUE;
        }
    }
}

int32_t DCameraClient::StartPhotoOutput(std::shared_ptr<DCameraCaptureInfo>& info)
{
    DHLOGI("DCameraClientCommon::StartPhotoOutput cameraId: %s", GetAnonyString(cameraId_).c_str());
    if (photoOutput_ == nullptr) {
        DHLOGE("DCameraClientCommon::StartPhotoOutput photoOutput is null");
        return DCAMERA_BAD_VALUE;
    }
    int32_t ret = ((sptr<CameraStandard::PhotoOutput> &)photoOutput_)->Capture();
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraClientCommon::StartPhotoOutput %s photoOutput capture failed, ret: %d",
            GetAnonyString(cameraId_).c_str(), ret);
        return ret;
    }
    return DCAMERA_OK;
}

int32_t DCameraClient::StartVideoOutput()
{
    DHLOGI("DCameraClientCommon::StartVideoOutput cameraId: %s", GetAnonyString(cameraId_).c_str());
    if (videoOutput_ == nullptr) {
        DHLOGE("DCameraClientCommon::StartVideoOutput videoOutput is null");
        return DCAMERA_BAD_VALUE;
    }
    int32_t ret = ((sptr<CameraStandard::VideoOutput> &)videoOutput_)->Start();
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraClientCommon::StartVideoOutput %s videoOutput start failed, ret: %d",
            GetAnonyString(cameraId_).c_str(), ret);
        return ret;
    }
    return DCAMERA_OK;
}
} // namespace DistributedHardware
} // namespace OHOS