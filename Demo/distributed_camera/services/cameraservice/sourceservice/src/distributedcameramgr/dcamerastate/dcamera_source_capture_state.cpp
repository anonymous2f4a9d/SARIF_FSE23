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

#include "dcamera_source_capture_state.h"

#include "dcamera_source_dev.h"
#include "dcamera_source_state_machine.h"
#include "distributed_camera_errno.h"
#include "distributed_hardware_log.h"

namespace OHOS {
namespace DistributedHardware {
DCameraSourceCaptureState::DCameraSourceCaptureState(std::shared_ptr<DCameraSourceStateMachine>& stateMachine)
    : stateMachine_(stateMachine)
{
    memberFuncMap_[DCAMERA_EVENT_REGIST] = &DCameraSourceCaptureState::DoRegisterTask;
    memberFuncMap_[DCAMERA_EVENT_UNREGIST] = &DCameraSourceCaptureState::DoUnregisterTask;
    memberFuncMap_[DCAMERA_EVENT_OPEN] = &DCameraSourceCaptureState::DoOpenTask;
    memberFuncMap_[DCAMERA_EVENT_CLOSE] = &DCameraSourceCaptureState::DoCloseTask;
    memberFuncMap_[DCAMERA_EVENT_START_CAPTURE] = &DCameraSourceCaptureState::DoStartCaptureTask;
    memberFuncMap_[DCAMERA_EVENT_STOP_CAPTURE] = &DCameraSourceCaptureState::DoStopCaptureTask;
    memberFuncMap_[DCAMERA_EVENT_UPDATE_SETTINGS] = &DCameraSourceCaptureState::DoUpdateSettingsTask;
    memberFuncMap_[DCAMERA_EVENT_NOFIFY] = &DCameraSourceCaptureState::DoEventNofityTask;
}

int32_t DCameraSourceCaptureState::Execute(std::shared_ptr<DCameraSourceDev>& camDev, DCAMERA_EVENT eventType,
    DCameraSourceEvent& event)
{
    auto itFunc = memberFuncMap_.find(eventType);
    if (itFunc == memberFuncMap_.end()) {
        DHLOGE("DCameraSourceCaptureState execute %d in wrong state", eventType);
        return DCAMERA_WRONG_STATE;
    }

    auto memberFunc = itFunc->second;
    int32_t ret = (this->*memberFunc)(camDev, event);
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSourceCaptureState execute %d failed, ret: %d", eventType, ret);
    }
    return ret;
}

DCameraStateType DCameraSourceCaptureState::GetStateType()
{
    return DCAMERA_STATE_CAPTURE;
}

int32_t DCameraSourceCaptureState::DoRegisterTask(std::shared_ptr<DCameraSourceDev>& camDev, DCameraSourceEvent& event)
{
    DHLOGI("DCameraSourceCaptureState DoRegisterTask Idempotent");
    return DCAMERA_OK;
}

int32_t DCameraSourceCaptureState::DoUnregisterTask(std::shared_ptr<DCameraSourceDev>& camDev,
    DCameraSourceEvent& event)
{
    std::shared_ptr<DCameraEvent> camEvent = std::make_shared<DCameraEvent>();
    camEvent->eventType_ = DCAMERA_MESSAGE;
    camEvent->eventResult_ = DCAMERA_EVENT_CHANNEL_DISCONNECTED;
    int32_t ret = camDev->ExecuteCameraEventNotify(camEvent);
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSourceCaptureState DoUnregisterTask ExecuteCameraEventNotify failed: %d", ret);
        return ret;
    }

    ret = camDev->ExecuteStopCapture();
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSourceCaptureState DoUnregisterTask ExecuteStopCapture failed: %d", ret);
        return ret;
    }

    ret = camDev->ExecuteCloseCamera();
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSourceCaptureState DoUnregisterTask ExecuteCloseCamera failed, ret: %d", ret);
        return ret;
    }

    ret = camDev->ExecuteReleaseAllStreams();
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSourceCaptureState DoUnregisterTask ExecuteReleaseAllStreams failed, ret: %d", ret);
        return ret;
    }

    std::shared_ptr<DCameraRegistParam> param;
    ret = event.GetDCameraRegistParam(param);
    if (ret != DCAMERA_OK) {
        return ret;
    }
    ret = camDev->ExecuteUnRegister(param);
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSourceCaptureState DoUnregisterTask ExecuteUnRegister failed: %d", ret);
        return ret;
    }
    std::shared_ptr<DCameraSourceStateMachine> stateMachine = stateMachine_.lock();
    if (stateMachine == nullptr) {
        DHLOGE("DCameraSourceCaptureState DoUnregisterTask can not get stateMachine");
        return DCAMERA_BAD_VALUE;
    }
    stateMachine->UpdateState(DCAMERA_STATE_INIT);
    return DCAMERA_OK;
}

int32_t DCameraSourceCaptureState::DoOpenTask(std::shared_ptr<DCameraSourceDev>& camDev, DCameraSourceEvent& event)
{
    DHLOGI("DCameraSourceCaptureState DoOpenTask Idempotent");
    return DCAMERA_OK;
}

int32_t DCameraSourceCaptureState::DoCloseTask(std::shared_ptr<DCameraSourceDev>& camDev, DCameraSourceEvent& event)
{
    int32_t ret = camDev->ExecuteStopCapture();
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSourceCaptureState ExecuteStopCapture failed, ret: %d", ret);
        return ret;
    }

    ret = camDev->ExecuteReleaseAllStreams();
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSourceCaptureState ExecuteReleaseAllStreams failed, ret: %d", ret);
        return ret;
    }

    ret = camDev->ExecuteCloseCamera();
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSourceCaptureState ExecuteCloseCamera failed, ret: %d", ret);
        return ret;
    }
    std::shared_ptr<DCameraSourceStateMachine> stateMachine = stateMachine_.lock();
    if (stateMachine == nullptr) {
        DHLOGE("DCameraSourceCaptureState DoOpenTask can not get stateMachine");
        return DCAMERA_BAD_VALUE;
    }
    stateMachine->UpdateState(DCAMERA_STATE_REGIST);
    return DCAMERA_OK;
}

int32_t DCameraSourceCaptureState::DoStartCaptureTask(std::shared_ptr<DCameraSourceDev>& camDev,
    DCameraSourceEvent& event)
{
    std::vector<std::shared_ptr<DCCaptureInfo>> captureInfos;
    int32_t ret = event.GetCaptureInfos(captureInfos);
    if (ret != DCAMERA_OK) {
        return ret;
    }

    ret = camDev->ExecuteStartCapture(captureInfos);
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSourceCaptureState DoStartCaptureTask failed, ret: %d", ret);
        return ret;
    }
    return DCAMERA_OK;
}

int32_t DCameraSourceCaptureState::DoStopCaptureTask(std::shared_ptr<DCameraSourceDev>& camDev,
    DCameraSourceEvent& event)
{
    int32_t ret = camDev->ExecuteStopCapture();
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSourceCaptureState ExecuteStopCapture failed, ret: %d", ret);
        return ret;
    }
    std::shared_ptr<DCameraSourceStateMachine> stateMachine = stateMachine_.lock();
    if (stateMachine == nullptr) {
        DHLOGE("DCameraSourceCaptureState DoStopCaptureTask can not get stateMachine");
        return DCAMERA_BAD_VALUE;
    }
    stateMachine->UpdateState(DCAMERA_STATE_CONFIG_STREAM);
    return DCAMERA_OK;
}

int32_t DCameraSourceCaptureState::DoUpdateSettingsTask(std::shared_ptr<DCameraSourceDev>& camDev,
    DCameraSourceEvent& event)
{
    std::vector<std::shared_ptr<DCameraSettings>> settings;
    int32_t ret = event.GetCameraSettings(settings);
    if (ret != DCAMERA_OK) {
        return ret;
    }

    ret = camDev->ExecuteUpdateSettings(settings);
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSourceCaptureState ExecuteUpdateSettings failed, ret: %d", ret);
        return ret;
    }
    return DCAMERA_OK;
}

int32_t DCameraSourceCaptureState::DoEventNofityTask(std::shared_ptr<DCameraSourceDev>& camDev,
    DCameraSourceEvent& event)
{
    std::shared_ptr<DCameraEvent> camEvent;
    int32_t ret = event.GetCameraEvent(camEvent);
    if (ret != DCAMERA_OK) {
        return ret;
    }

    ret = camDev->ExecuteCameraEventNotify(camEvent);
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSourceCaptureState ExecuteCameraEventNotify failed, ret: %d", ret);
        return ret;
    }
    return DCAMERA_OK;
}
} // namespace DistributedHardware
} // namespace OHOS
