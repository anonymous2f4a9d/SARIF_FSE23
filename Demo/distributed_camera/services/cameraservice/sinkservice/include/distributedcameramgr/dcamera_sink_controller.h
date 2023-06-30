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

#ifndef OHOS_DCAMERA_SINK_CONTROLLER_H
#define OHOS_DCAMERA_SINK_CONTROLLER_H

#include "event_bus.h"
#include "dcamera_frame_trigger_event.h"
#include "dcamera_post_authorization_event.h"
#include "icamera_controller.h"

#include "icamera_channel.h"
#include "icamera_operator.h"
#include "icamera_sink_access_control.h"
#include "icamera_sink_output.h"
#include <mutex>

namespace OHOS {
namespace DistributedHardware {
class DCameraSinkController : public ICameraController, public EventSender,
    public DistributedHardware::EventBusHandler<DCameraFrameTriggerEvent>,
    public DistributedHardware::EventBusHandler<DCameraPostAuthorizationEvent>,
    public std::enable_shared_from_this<DCameraSinkController> {
public:
    explicit DCameraSinkController(std::shared_ptr<ICameraSinkAccessControl>& accessControl);
    ~DCameraSinkController();

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

    void OnEvent(DCameraFrameTriggerEvent& event) override;
    void OnEvent(DCameraPostAuthorizationEvent& event) override;

    void OnStateChanged(std::shared_ptr<DCameraEvent>& event);
    void OnMetadataResult();

    void OnSessionState(int32_t state);
    void OnSessionError(int32_t eventType, int32_t eventReason, std::string detail);
    void OnDataReceived(std::vector<std::shared_ptr<DataBuffer>>& buffers);

private:
    int32_t StartCaptureInner(std::vector<std::shared_ptr<DCameraCaptureInfo>>& captureInfos);
    int32_t DCameraNotifyInner(int32_t type, int32_t result, std::string content);
    int32_t HandleReceivedData(std::shared_ptr<DataBuffer>& dataBuffer);
    void PostAuthorization(std::vector<std::shared_ptr<DCameraCaptureInfo>>& captureInfos);

    bool isInit_;
    int32_t sessionState_;
    std::mutex captureLock_;
    std::mutex channelLock_;
    std::string dhId_;
    std::string srcDevId_;
    std::shared_ptr<EventBus> eventBus_;
    std::shared_ptr<ICameraChannel> channel_;
    std::shared_ptr<ICameraOperator> operator_;
    std::shared_ptr<ICameraSinkAccessControl> accessControl_;
    std::shared_ptr<ICameraSinkOutput> output_;

    const std::string SESSION_FLAG = "control";
    const std::string SRC_TYPE = "camera";
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_DCAMERA_SINK_CONTROLLER_H