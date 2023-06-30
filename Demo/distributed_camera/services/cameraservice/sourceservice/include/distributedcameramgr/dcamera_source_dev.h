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

#ifndef OHOS_DCAMERA_SOURCE_DEV_H
#define OHOS_DCAMERA_SOURCE_DEV_H

#include "dcamera_index.h"
#include "dcamera_source_event.h"
#include "dcamera_source_state_machine.h"
#include "event_bus.h"
#include "icamera_controller.h"
#include "icamera_state_listener.h"
#include "icamera_input.h"

#include "idistributed_camera_provider_callback.h"
#include "idistributed_camera_provider.h"

namespace OHOS {
namespace DistributedHardware {
class DCameraSourceDev : public EventSender, public DistributedHardware::EventBusHandler<DCameraSourceEvent>,
    public std::enable_shared_from_this<DCameraSourceDev> {
public:
    explicit DCameraSourceDev(std::string devId, std::string dhId, std::shared_ptr<ICameraStateListener>& stateLisener);
    virtual ~DCameraSourceDev();

    int32_t InitDCameraSourceDev();
    int32_t RegisterDistributedHardware(const std::string& devId, const std::string& dhId, const std::string& reqId,
        const std::string& ver, const std::string attrs);
    int32_t UnRegisterDistributedHardware(const std::string devId, const std::string dhId, const std::string reqId);
    int32_t DCameraNotify(std::string& eventStr);

    int32_t OpenSession(DCameraIndex& camIndex);
    int32_t CloseSession(DCameraIndex& camIndex);
    int32_t ConfigStreams(const std::vector<std::shared_ptr<DCStreamInfo>>& streamInfos);
    int32_t ReleaseStreams(const std::vector<int>& streamIds);
    int32_t StartCapture(const std::vector<std::shared_ptr<DCCaptureInfo>>& captureInfos);
    int32_t StopCapture();
    int32_t UpdateCameraSettings(const std::vector<std::shared_ptr<DCameraSettings>>& settings);

    void OnEvent(DCameraSourceEvent& event) override;

public:
    virtual int32_t ExecuteRegister(std::shared_ptr<DCameraRegistParam>& param);
    virtual int32_t ExecuteUnRegister(std::shared_ptr<DCameraRegistParam>& param);
    virtual int32_t ExecuteOpenCamera();
    virtual int32_t ExecuteCloseCamera();
    virtual int32_t ExecuteConfigStreams(std::vector<std::shared_ptr<DCStreamInfo>>& streamInfos);
    virtual int32_t ExecuteReleaseStreams(std::vector<int>& streamIds, bool& isAllRelease);
    virtual int32_t ExecuteReleaseAllStreams();
    virtual int32_t ExecuteStartCapture(std::vector<std::shared_ptr<DCCaptureInfo>>& captureInfos);
    virtual int32_t ExecuteStopCapture();
    virtual int32_t ExecuteUpdateSettings(std::vector<std::shared_ptr<DCameraSettings>>& settings);
    virtual int32_t ExecuteCameraEventNotify(std::shared_ptr<DCameraEvent>& events);

private:
    using DCameraNotifyFunc = void (DCameraSourceDev::*)(DCAMERA_EVENT eventType, DCameraSourceEvent& event,
        int32_t result);

    void NotifyResult(DCAMERA_EVENT eventType, DCameraSourceEvent& event, int32_t result);
    void NotifyRegisterResult(DCAMERA_EVENT eventType, DCameraSourceEvent& event, int32_t result);
    void NotifyUnregisterResult(DCAMERA_EVENT eventType, DCameraSourceEvent& event, int32_t result);
    void NotifyHalResult(DCAMERA_EVENT eventType, DCameraSourceEvent& event, int32_t result);

private:
    std::string devId_;
    std::string dhId_;
    std::string version_;
    std::set<DCameraIndex> actualDevInfo_;
    std::shared_ptr<ICameraStateListener> stateListener_;
    std::shared_ptr<EventBus> eventBus_;
    std::shared_ptr<DCameraSourceStateMachine> stateMachine_;
    std::shared_ptr<ICameraController> controller_;
    std::shared_ptr<ICameraInput> input_;
    sptr<IDCameraProviderCallback> hdiCallback_;

    std::map<uint32_t, DCameraNotifyFunc> memberFuncMap_;
    std::map<uint32_t, DCameraEventResult> eventResultMap_;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_DCAMERA_SOURCE_DEV_H
