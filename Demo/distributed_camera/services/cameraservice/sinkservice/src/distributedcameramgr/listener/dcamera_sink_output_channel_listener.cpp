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

#include "dcamera_sink_output_channel_listener.h"

#include "distributed_hardware_log.h"

namespace OHOS {
namespace DistributedHardware {
DCameraSinkOutputChannelListener::DCameraSinkOutputChannelListener(
    DCStreamType type, std::shared_ptr<DCameraSinkOutput>& output) : streamType_(type), output_(output)
{
}

void DCameraSinkOutputChannelListener::OnSessionState(int32_t state)
{
    std::shared_ptr<DCameraSinkOutput> output = output_.lock();
    if (output == nullptr) {
        DHLOGE("DCameraSinkOutputChannelListener::OnSessionState output is null");
        return;
    }
    output->OnSessionState(streamType_, state);
}

void DCameraSinkOutputChannelListener::OnSessionError(int32_t eventType, int32_t eventReason, std::string detail)
{
    std::shared_ptr<DCameraSinkOutput> output = output_.lock();
    if (output == nullptr) {
        DHLOGE("DCameraSinkOutputChannelListener::OnSessionError output is null");
        return;
    }
    output->OnSessionError(streamType_, eventType, eventReason, detail);
}

void DCameraSinkOutputChannelListener::OnDataReceived(std::vector<std::shared_ptr<DataBuffer>>& buffers)
{
    std::shared_ptr<DCameraSinkOutput> output = output_.lock();
    if (output == nullptr) {
        DHLOGE("DCameraSinkOutputChannelListener::OnReceivedData output is null");
        return;
    }
    output->OnDataReceived(streamType_, buffers);
}
} // namespace DistributedHardware
} // namespace OHOS