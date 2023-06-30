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

#include "dcamera_source_controller_channel_listener.h"

#include "distributed_camera_errno.h"
#include "distributed_hardware_log.h"

#include "dcamera_source_controller.h"

namespace OHOS {
namespace DistributedHardware {
DCameraSourceControllerChannelListener::DCameraSourceControllerChannelListener(
    std::shared_ptr<DCameraSourceController>& controller) : controller_(controller)
{
}

DCameraSourceControllerChannelListener::~DCameraSourceControllerChannelListener()
{
}

void DCameraSourceControllerChannelListener::OnSessionState(int32_t state)
{
    std::shared_ptr<DCameraSourceController> controller = controller_.lock();
    if (controller == nullptr) {
        DHLOGE("DCameraSourceController OnSessionState not found controller");
        return;
    }

    controller->OnSessionState(state);
}

void DCameraSourceControllerChannelListener::OnSessionError(int32_t eventType, int32_t eventReason, std::string detail)
{
    std::shared_ptr<DCameraSourceController> controller = controller_.lock();
    if (controller == nullptr) {
        DHLOGE("DCameraSourceController OnSessionError not found controller");
        return;
    }

    controller->OnSessionError(eventType, eventReason, detail);
}

void DCameraSourceControllerChannelListener::OnDataReceived(std::vector<std::shared_ptr<DataBuffer>>& buffers)
{
    std::shared_ptr<DCameraSourceController> controller = controller_.lock();
    if (controller == nullptr) {
        DHLOGE("DCameraSourceController OnDataReceived not found controller");
        return;
    }

    controller->OnDataReceived(buffers);
}
} // namespace DistributedHardware
} // namespace OHOS
