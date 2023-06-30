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

#ifndef OHOS_DCAMERA_CLIENT_H
#define OHOS_DCAMERA_CLIENT_H

#include "icamera_operator.h"

#include "camera_info.h"
#include "camera_input.h"
#include "camera_manager.h"
#include "capture_input.h"
#include "capture_output.h"
#include "capture_session.h"
#include "photo_output.h"
#include "preview_output.h"
#include "video_output.h"

#include "dcamera_photo_surface_listener.h"
#include "dcamera_video_surface_listener.h"

namespace OHOS {
namespace DistributedHardware {
class DCameraClient : public ICameraOperator {
public:
    explicit DCameraClient(const std::string& dhId);
    ~DCameraClient();

    int32_t Init() override;
    int32_t UnInit() override;
    int32_t UpdateSettings(std::vector<std::shared_ptr<DCameraSettings>>& settings) override;
    int32_t StartCapture(std::vector<std::shared_ptr<DCameraCaptureInfo>>& captureInfos) override;
    int32_t StopCapture() override;
    int32_t SetStateCallback(std::shared_ptr<StateCallback>& callback) override;
    int32_t SetResultCallback(std::shared_ptr<ResultCallback>& callback) override;

private:
    int32_t ConfigCaptureSession(std::vector<std::shared_ptr<DCameraCaptureInfo>>& captureInfos);
    int32_t ConfigCaptureSessionInner();
    int32_t CreateCaptureOutput(std::vector<std::shared_ptr<DCameraCaptureInfo>>& captureInfos);
    int32_t CreatePhotoOutput(std::shared_ptr<DCameraCaptureInfo>& info);
    int32_t CreateVideoOutput(std::shared_ptr<DCameraCaptureInfo>& info);
    int32_t StartCaptureInner(std::shared_ptr<DCameraCaptureInfo>& info);
    int32_t StartPhotoOutput(std::shared_ptr<DCameraCaptureInfo>& info);
    int32_t StartVideoOutput();

    bool isInit_;
    std::string cameraId_;
    sptr<Surface> photoSurface_;
    sptr<Surface> videoSurface_;
    sptr<CameraStandard::CameraInfo> cameraInfo_;
    sptr<CameraStandard::CameraManager> cameraManager_;
    sptr<CameraStandard::CaptureSession> captureSession_;
    sptr<CameraStandard::CaptureInput> cameraInput_;
    sptr<CameraStandard::CaptureOutput> photoOutput_;
    sptr<CameraStandard::CaptureOutput> previewOutput_;
    sptr<CameraStandard::CaptureOutput> videoOutput_;
    std::shared_ptr<StateCallback> stateCallback_;
    std::shared_ptr<ResultCallback> resultCallback_;
    std::shared_ptr<DCameraPhotoSurfaceListener> photoListener_;
    std::shared_ptr<DCameraVideoSurfaceListener> videoListener_;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_DCAMERA_CLIENT_H