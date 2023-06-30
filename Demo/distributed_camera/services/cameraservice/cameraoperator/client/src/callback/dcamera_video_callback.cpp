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

#include "dcamera_video_callback.h"

#include "distributed_camera_constants.h"
#include "distributed_hardware_log.h"

namespace OHOS {
namespace DistributedHardware {
DCameraVideoCallback::DCameraVideoCallback(const std::shared_ptr<StateCallback>& callback) : callback_(callback)
{
}

void DCameraVideoCallback::OnFrameStarted() const
{
    DHLOGI("DCameraVideoCallback::OnFrameStarted");
}

void DCameraVideoCallback::OnFrameEnded(const int32_t frameCount) const
{
    DHLOGI("DCameraVideoCallback::OnFrameEnded, frameCount: %d", frameCount);
}

void DCameraVideoCallback::OnError(const int32_t errorCode) const
{
    DHLOGE("DCameraVideoCallback::OnError, errorCode: %d", errorCode);
    if (callback_ == nullptr) {
        DHLOGE("DCameraVideoCallback StateCallback is null");
        return;
    }

    std::shared_ptr<DCameraEvent> event = std::make_shared<DCameraEvent>();
    event->eventType_ = DCAMERA_MESSAGE;
    event->eventResult_ = DCAMERA_EVENT_CAMERA_ERROR;
    callback_->OnStateChanged(event);
}
} // namespace DistributedHardware
} // namespace OHOS