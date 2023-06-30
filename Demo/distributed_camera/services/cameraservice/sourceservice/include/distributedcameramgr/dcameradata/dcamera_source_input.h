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

#ifndef OHOS_DCAMERA_SOURCE_INPUT_H
#define OHOS_DCAMERA_SOURCE_INPUT_H

#include "icamera_channel.h"
#include "icamera_channel_listener.h"
#include "icamera_input.h"
#include "icamera_source_data_process.h"

#include "event_bus.h"
#include "event_sender.h"

namespace OHOS {
namespace DistributedHardware {
class DCameraSourceInput : public ICameraInput, public EventSender,
    public std::enable_shared_from_this<DCameraSourceInput> {
public:
    DCameraSourceInput(std::string devId, std::string dhId, std::shared_ptr<EventBus>& eventBus);
    ~DCameraSourceInput();

    int32_t ConfigStreams(std::vector<std::shared_ptr<DCStreamInfo>>& streamInfos) override;
    int32_t ReleaseStreams(std::vector<int>& streamIds, bool& isAllRelease) override;
    int32_t ReleaseAllStreams() override;
    int32_t StartCapture(std::vector<std::shared_ptr<DCCaptureInfo>>& captureInfos) override;
    int32_t StopCapture() override;
    int32_t OpenChannel(std::vector<DCameraIndex>& indexs) override;
    int32_t CloseChannel() override;
    int32_t Init() override;
    int32_t UnInit() override;
    int32_t UpdateSettings(std::vector<std::shared_ptr<DCameraSettings>>& settings) override;

    void OnSessionState(DCStreamType streamType, int32_t state);
    void OnSessionError(DCStreamType streamType, int32_t eventType, int32_t eventReason, std::string detail);
    void OnDataReceived(DCStreamType streamType, std::vector<std::shared_ptr<DataBuffer>>& buffers);

private:
    std::map<DCStreamType, std::shared_ptr<ICameraChannel>> channels_;
    std::map<DCStreamType, std::shared_ptr<ICameraChannelListener>> listeners_;
    std::map<DCStreamType, std::shared_ptr<ICameraSourceDataProcess>> dataProcess_;
    std::map<DCStreamType, DCameraChannelState> channelState_;
    std::string devId_;
    std::string dhId_;
    std::shared_ptr<EventBus> eventBus_;

    bool isInit = false;
    std::mutex inputMutex_;
    bool isCapture_;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_DCAMERA_SOURCE_INPUT_H
