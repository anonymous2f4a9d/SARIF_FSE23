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

#ifndef OHOS_DCAMERA_SOURCE_CONTROLLER_H
#define OHOS_DCAMERA_SOURCE_CONTROLLER_H

#include "icamera_controller.h"

#include "dcamera_index.h"
#include "icamera_channel_listener.h"
#include "dcamera_source_state_machine.h"
#include "event_bus.h"
#include "icamera_channel.h"

#include "idistributed_camera_provider.h"

namespace OHOS {
namespace DistributedHardware {
class DCameraSourceController : public ICameraController, public EventSender,
    public std::enable_shared_from_this<DCameraSourceController> {
public:
    DCameraSourceController(std::string devId, std::string dhId,
        std::shared_ptr<DCameraSourceStateMachine>& stateMachine, std::shared_ptr<EventBus>& eventBus);
    ~DCameraSourceController();
    int32_t StartCapture(std::vector<std::shared_ptr<DCameraCaptureInfo>>& captureInfos) override;
    int32_t StopCapture() override;
    int32_t ChannelNeg(std::shared_ptr<DCameraChannelInfo>& info) override;
    int32_t DCameraNotify(std::shared_ptr<DCameraEvent>& events) override;
    int32_t UpdateSettings(std::vector<std::shared_ptr<DCameraSettings>>& settings) override;
    int32_t GetCameraInfo(std::shared_ptr<DCameraInfo>& camInfo) override;
    int32_t OpenChannel(std::shared_ptr<DCameraOpenInfo>& openInfo) override;
    int32_t CloseChannel() override;
    int32_t Init(std::vector<DCameraIndex>& indexs) override;
    int32_t UnInit() override;

    void OnSessionState(int32_t state);
    void OnSessionError(int32_t eventType, int32_t eventReason, std::string detail);
    void OnDataReceived(std::vector<std::shared_ptr<DataBuffer>>& buffers);

private:
    void HandleMetaDataResult(std::string& jsonStr);

private:
    std::string devId_;
    std::string dhId_;
    std::shared_ptr<ICameraChannel> channel_;
    std::shared_ptr<ICameraChannelListener> listener_;
    std::vector<DCameraIndex> indexs_;
    std::shared_ptr<DCameraSourceStateMachine> stateMachine_;
    std::shared_ptr<EventBus> eventBus_;
    int32_t channelState_;

    bool isInit;
    const std::string SESSION_FLAG = "control";
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_DCAMERA_SOURCE_CONTROLLER_H
