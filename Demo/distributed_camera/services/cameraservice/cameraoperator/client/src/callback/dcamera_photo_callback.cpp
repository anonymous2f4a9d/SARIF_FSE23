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

#include "dcamera_photo_callback.h"

#include "distributed_camera_constants.h"
#include "distributed_hardware_log.h"

namespace OHOS {
namespace DistributedHardware {
DCameraPhotoCallback::DCameraPhotoCallback(const std::shared_ptr<StateCallback>& callback) : callback_(callback)
{
}

void DCameraPhotoCallback::OnCaptureStarted(const int32_t captureID) const
{
    DHLOGI("DCameraPhotoCallback::OnCaptureStarted, captureID: %d", captureID);
}

void DCameraPhotoCallback::OnCaptureEnded(const int32_t captureID, const int32_t frameCount) const
{
    DHLOGI("DCameraPhotoCallback::OnCaptureEnded, captureID: %d, frameCount: %d", captureID, frameCount);
}

void DCameraPhotoCallback::OnFrameShutter(const int32_t captureId, const uint64_t timestamp) const
{
    DHLOGI("DCameraPhotoCallback::OnFrameShutter, captureId: %d, timestamp: %llu", captureId, timestamp);
}

void DCameraPhotoCallback::OnCaptureError(const int32_t captureId, const int32_t errorCode) const
{
    DHLOGE("DCameraPhotoCallback::OnCaptureError, captureId: %d, errorCode: %d", captureId, errorCode);
    if (callback_ == nullptr) {
        DHLOGE("DCameraPhotoCallback::OnCaptureError StateCallback is null");
        return;
    }

    std::shared_ptr<DCameraEvent> event = std::make_shared<DCameraEvent>();
    event->eventType_ = DCAMERA_MESSAGE;
    event->eventResult_ = DCAMERA_EVENT_CAMERA_ERROR;
    callback_->OnStateChanged(event);
}
} // namespace DistributedHardware
} // namespace OHOS