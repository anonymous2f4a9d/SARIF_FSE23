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

#include "dcamera_source_config_stream_state.h"

#include "dcamera_source_dev.h"
#include "dcamera_source_state_machine.h"
#include "distributed_camera_errno.h"
#include "distributed_hardware_log.h"

namespace OHOS {
namespace DistributedHardware {
DCameraSourceConfigStreamState::DCameraSourceConfigStreamState(std::shared_ptr<DCameraSourceStateMachine>& stateMachine)
    : stateMachine_(stateMachine)
{
    memberFuncMap_[DCAMERA_EVENT_REGIST] = &DCameraSourceConfigStreamState::DoRegisterTask;
    memberFuncMap_[DCAMERA_EVENT_UNREGIST] = &DCameraSourceConfigStreamState::DoUnregisterTask;
    memberFuncMap_[DCAMERA_EVENT_OPEN] = &DCameraSourceConfigStreamState::DoOpenTask;
    memberFuncMap_[DCAMERA_EVENT_CLOSE] = &DCameraSourceConfigStreamState::DoCloseTask;
    memberFuncMap_[DCAMERA_EVENT_CONFIG_STREAMS] = &DCameraSourceConfigStreamState::DoConfigStreamsTask;
    memberFuncMap_[DCAMERA_EVENT_RELEASE_STREAMS] = &DCameraSourceConfigStreamState::DoReleaseStreamsTask;
    memberFuncMap_[DCAMERA_EVENT_START_CAPTURE] = &DCameraSourceConfigStreamState::DoStartCaptureTask;
    memberFuncMap_[DCAMERA_EVENT_STOP_CAPTURE] = &DCameraSourceConfigStreamState::DoStopCaptureTask;
    memberFuncMap_[DCAMERA_EVENT_UPDATE_SETTINGS] = &DCameraSourceConfigStreamState::DoUpdateSettingsTask;
    memberFuncMap_[DCAMERA_EVENT_NOFIFY] = &DCameraSourceConfigStreamState::DoEventNofityTask;
}

int32_t DCameraSourceConfigStreamState::Execute(std::shared_ptr<DCameraSourceDev>& camDev, DCAMERA_EVENT eventType,
    DCameraSourceEvent& event)
{
    auto itFunc = memberFuncMap_.find(eventType);
    if (itFunc == memberFuncMap_.end()) {
        DHLOGE("DCameraSourceConfigStreamState execute %d in wrong state", eventType);
        return DCAMERA_WRONG_STATE;
    }

    auto memberFunc = itFunc->second;
    int32_t ret = (this->*memberFunc)(camDev, event);
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSourceConfigStreamState execute %d failed, ret: %d", eventType, ret);
    }
    return ret;
}

DCameraStateType DCameraSourceConfigStreamState::GetStateType()
{
    return DCAMERA_STATE_CONFIG_STREAM;
}

int32_t DCameraSourceConfigStreamState::DoRegisterTask(std::shared_ptr<DCameraSourceDev>& camDev,
    DCameraSourceEvent& event)
{
    DHLOGI("DCameraSourceConfigStreamState DoRegisterTask Idempotent");
    return DCAMERA_OK;
}

int32_t DCameraSourceConfigStreamState::DoUnregisterTask(std::shared_ptr<DCameraSourceDev>& camDev,
    DCameraSourceEvent& event)
{
    std::shared_ptr<DCameraEvent> camEvent = std::make_shared<DCameraEvent>();
    camEvent->eventType_ = DCAMERA_MESSAGE;
    camEvent->eventResult_ = DCAMERA_EVENT_CHANNEL_DISCONNECTED;
    int32_t ret = camDev->ExecuteCameraEventNotify(camEvent);
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSourceConfigStreamState DoUnregisterTask ExecuteCameraEventNotify failed: %d", ret);
        return ret;
    }

    ret = camDev->ExecuteCloseCamera();
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSourceConfigStreamState DoUnregisterTask ExecuteCloseCamera failed, ret: %d", ret);
        return ret;
    }

    ret = camDev->ExecuteReleaseAllStreams();
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSourceConfigStreamState DoUnregisterTask ExecuteReleaseAllStreams failed, ret: %d", ret);
        return ret;
    }

    std::shared_ptr<DCameraRegistParam> param;
    ret = event.GetDCameraRegistParam(param);
    if (ret != DCAMERA_OK) {
        return ret;
    }
    ret = camDev->ExecuteUnRegister(param);
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSourceConfigStreamState DoUnregisterTask ExecuteUnRegister failed: %d", ret);
        return ret;
    }
    std::shared_ptr<DCameraSourceStateMachine> stateMachine = stateMachine_.lock();
    if (stateMachine == nullptr) {
        DHLOGE("DCameraSourceConfigStreamState DoUnregisterTask can not get stateMachine");
        return DCAMERA_BAD_VALUE;
    }
    stateMachine->UpdateState(DCAMERA_STATE_INIT);
    return DCAMERA_OK;
}

int32_t DCameraSourceConfigStreamState::DoOpenTask(std::shared_ptr<DCameraSourceDev>& camDev, DCameraSourceEvent& event)
{
    DHLOGI("DCameraSourceConfigStreamState DoOpenTask Idempotent");
    return DCAMERA_OK;
}

int32_t DCameraSourceConfigStreamState::DoCloseTask(std::shared_ptr<DCameraSourceDev>& camDev,
    DCameraSourceEvent& event)
{
    int32_t ret = camDev->ExecuteCloseCamera();
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSourceConfigStreamState ExecuteCloseCamera failed, ret: %d", ret);
        return ret;
    }

    ret = camDev->ExecuteReleaseAllStreams();
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSourceConfigStreamState ExecuteReleaseAllStreams failed, ret: %d", ret);
        return ret;
    }
    std::shared_ptr<DCameraSourceStateMachine> stateMachine = stateMachine_.lock();
    if (stateMachine == nullptr) {
        DHLOGE("DCameraSourceConfigStreamState DoOpenTask can not get stateMachine");
        return DCAMERA_BAD_VALUE;
    }
    stateMachine->UpdateState(DCAMERA_STATE_REGIST);
    return DCAMERA_OK;
}

int32_t DCameraSourceConfigStreamState::DoConfigStreamsTask(std::shared_ptr<DCameraSourceDev>& camDev,
    DCameraSourceEvent& event)
{
    std::vector<std::shared_ptr<DCStreamInfo>> streamInfos;
    int32_t ret = event.GetStreamInfos(streamInfos);
    if (ret != DCAMERA_OK) {
        return ret;
    }

    ret = camDev->ExecuteConfigStreams(streamInfos);
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSourceConfigStreamState ExecuteConfigStreams failed, ret: %d", ret);
        return ret;
    }

    if (streamInfos.empty()) {
        std::shared_ptr<DCameraSourceStateMachine> stateMachine = stateMachine_.lock();
        if (stateMachine == nullptr) {
            DHLOGE("DCameraSourceConfigStreamState DoOpenTask can not get stateMachine");
            return DCAMERA_BAD_VALUE;
        }
        stateMachine->UpdateState(DCAMERA_STATE_OPENED);
    }
    return DCAMERA_OK;
}

int32_t DCameraSourceConfigStreamState::DoReleaseStreamsTask(std::shared_ptr<DCameraSourceDev>& camDev,
    DCameraSourceEvent& event)
{
    std::vector<int> streamIds;
    int32_t ret = event.GetStreamIds(streamIds);
    if (ret != DCAMERA_OK) {
        return ret;
    }

    bool isAllRelease = false;
    ret = camDev->ExecuteReleaseStreams(streamIds, isAllRelease);
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSourceConfigStreamState ExecuteConfigStreams failed, ret: %d", ret);
        return ret;
    }

    if (isAllRelease) {
        std::shared_ptr<DCameraSourceStateMachine> stateMachine = stateMachine_.lock();
        if (stateMachine == nullptr) {
            DHLOGE("DCameraSourceConfigStreamState DoOpenTask can not get stateMachine");
            return DCAMERA_BAD_VALUE;
        }
        stateMachine->UpdateState(DCAMERA_STATE_OPENED);
    }
    return DCAMERA_OK;
}

int32_t DCameraSourceConfigStreamState::DoStartCaptureTask(std::shared_ptr<DCameraSourceDev>& camDev,
    DCameraSourceEvent& event)
{
    std::vector<std::shared_ptr<DCCaptureInfo>> captureInfos;
    int32_t ret = event.GetCaptureInfos(captureInfos);
    if (ret != DCAMERA_OK) {
        return ret;
    }

    ret = camDev->ExecuteStartCapture(captureInfos);
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSourceConfigStreamState ExecuteStartCapture failed, ret: %d", ret);
        return ret;
    }
    std::shared_ptr<DCameraSourceStateMachine> stateMachine = stateMachine_.lock();
    if (stateMachine == nullptr) {
        DHLOGE("DCameraSourceConfigStreamState DoOpenTask can not get stateMachine");
        return DCAMERA_BAD_VALUE;
    }
    stateMachine->UpdateState(DCAMERA_STATE_CAPTURE);
    return DCAMERA_OK;
}

int32_t DCameraSourceConfigStreamState::DoStopCaptureTask(std::shared_ptr<DCameraSourceDev>& camDev,
    DCameraSourceEvent& event)
{
    DHLOGI("DCameraSourceConfigStreamState DoStopCaptureTask Idempotent");
    return DCAMERA_OK;
}

int32_t DCameraSourceConfigStreamState::DoUpdateSettingsTask(std::shared_ptr<DCameraSourceDev>& camDev,
    DCameraSourceEvent& event)
{
    std::vector<std::shared_ptr<DCameraSettings>> settings;
    int32_t ret = event.GetCameraSettings(settings);
    if (ret != DCAMERA_OK) {
        return ret;
    }

    ret = camDev->ExecuteUpdateSettings(settings);
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSourceConfigStreamState ExecuteConfigStreams failed, ret: %d", ret);
        return ret;
    }
    return DCAMERA_OK;
}

int32_t DCameraSourceConfigStreamState::DoEventNofityTask(std::shared_ptr<DCameraSourceDev>& camDev,
    DCameraSourceEvent& event)
{
    std::shared_ptr<DCameraEvent> camEvent;
    int32_t ret = event.GetCameraEvent(camEvent);
    if (ret != DCAMERA_OK) {
        return ret;
    }

    ret = camDev->ExecuteCameraEventNotify(camEvent);
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSourceConfigStreamState ExecuteCameraEventNotify failed, ret: %d", ret);
        return ret;
    }
    return DCAMERA_OK;
}
} // namespace DistributedHardware
} // namespace OHOS
