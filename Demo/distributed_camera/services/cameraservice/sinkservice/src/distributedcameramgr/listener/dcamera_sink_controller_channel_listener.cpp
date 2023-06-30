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

#include "dcamera_sink_controller_channel_listener.h"

#include "distributed_hardware_log.h"

namespace OHOS {
namespace DistributedHardware {
DCameraSinkControllerChannelListener::DCameraSinkControllerChannelListener(
    std::shared_ptr<DCameraSinkController>& controller) : controller_(controller)
{
}

void DCameraSinkControllerChannelListener::OnSessionState(int32_t state)
{
    std::shared_ptr<DCameraSinkController> controller = controller_.lock();
    if (controller == nullptr) {
        DHLOGE("DCameraSinkControllerChannelListener::OnSessionState controller is null");
        return;
    }
    controller->OnSessionState(state);
}

void DCameraSinkControllerChannelListener::OnSessionError(int32_t eventType, int32_t eventReason, std::string detail)
{
    std::shared_ptr<DCameraSinkController> controller = controller_.lock();
    if (controller == nullptr) {
        DHLOGE("DCameraSinkControllerChannelListener::OnSessionError controller is null");
        return;
    }
    controller->OnSessionError(eventType, eventReason, detail);
}

void DCameraSinkControllerChannelListener::OnDataReceived(std::vector<std::shared_ptr<DataBuffer>>& buffers)
{
    std::shared_ptr<DCameraSinkController> controller = controller_.lock();
    if (controller == nullptr) {
        DHLOGE("DCameraSinkControllerChannelListener::OnReceivedData controller is null");
        return;
    }
    controller->OnDataReceived(buffers);
}
} // namespace DistributedHardware
} // namespace OHOS