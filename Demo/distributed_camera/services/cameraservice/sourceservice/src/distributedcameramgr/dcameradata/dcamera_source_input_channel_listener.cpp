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

#include "dcamera_source_input_channel_listener.h"

#include "dcamera_source_input.h"

#include "distributed_camera_errno.h"
#include "distributed_hardware_log.h"

namespace OHOS {
namespace DistributedHardware {
DCameraSourceInputChannelListener::DCameraSourceInputChannelListener(std::shared_ptr<DCameraSourceInput>& input,
    DCStreamType streamType) : input_(input), streamType_(streamType)
{
}

DCameraSourceInputChannelListener::~DCameraSourceInputChannelListener()
{
}

void DCameraSourceInputChannelListener::OnSessionState(int32_t state)
{
    std::shared_ptr<DCameraSourceInput> input = input_.lock();
    if (input == nullptr) {
        DHLOGE("DCameraSourceInput OnSessionState not found input");
        return;
    }

    input->OnSessionState(streamType_, state);
}

void DCameraSourceInputChannelListener::OnSessionError(int32_t eventType, int32_t eventReason, std::string detail)
{
    std::shared_ptr<DCameraSourceInput> input = input_.lock();
    if (input == nullptr) {
        DHLOGE("DCameraSourceInput OnSessionError not found input");
        return;
    }

    input->OnSessionError(streamType_, eventType, eventReason, detail);
}

void DCameraSourceInputChannelListener::OnDataReceived(std::vector<std::shared_ptr<DataBuffer>>& buffers)
{
    std::shared_ptr<DCameraSourceInput> input = input_.lock();
    if (input == nullptr) {
        DHLOGE("DCameraSourceInput OnDataReceived not found input");
        return;
    }

    input->OnDataReceived(streamType_, buffers);
}
} // namespace DistributedHardware
} // namespace OHOS
