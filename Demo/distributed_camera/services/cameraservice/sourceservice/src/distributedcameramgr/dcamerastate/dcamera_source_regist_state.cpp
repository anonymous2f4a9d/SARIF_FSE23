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

#include "dcamera_source_regist_state.h"

#include "dcamera_source_dev.h"
#include "dcamera_source_state_machine.h"
#include "distributed_camera_errno.h"
#include "distributed_hardware_log.h"

namespace OHOS {
namespace DistributedHardware {
DCameraSourceRegistState::DCameraSourceRegistState(std::shared_ptr<DCameraSourceStateMachine>& stateMachine)
    : stateMachine_(stateMachine)
{
    memberFuncMap_[DCAMERA_EVENT_REGIST] = &DCameraSourceRegistState::DoRegisterTask;
    memberFuncMap_[DCAMERA_EVENT_UNREGIST] = &DCameraSourceRegistState::DoUnregisterTask;
    memberFuncMap_[DCAMERA_EVENT_OPEN] = &DCameraSourceRegistState::DoOpenTask;
    memberFuncMap_[DCAMERA_EVENT_CLOSE] = &DCameraSourceRegistState::DoCloseTask;
    memberFuncMap_[DCAMERA_EVENT_NOFIFY] = &DCameraSourceRegistState::DoEventNofityTask;
}

int32_t DCameraSourceRegistState::Execute(std::shared_ptr<DCameraSourceDev>& camDev, DCAMERA_EVENT eventType,
    DCameraSourceEvent& event)
{
    auto itFunc = memberFuncMap_.find(eventType);
    if (itFunc == memberFuncMap_.end()) {
        DHLOGE("DCameraSourceRegistState execute %d in wrong state", eventType);
        return DCAMERA_WRONG_STATE;
    }

    auto memberFunc = itFunc->second;
    int32_t ret = (this->*memberFunc)(camDev, event);
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSourceRegistState execute %d failed, ret: %d", eventType, ret);
    }
    return ret;
}

DCameraStateType DCameraSourceRegistState::GetStateType()
{
    return DCAMERA_STATE_REGIST;
}

int32_t DCameraSourceRegistState::DoRegisterTask(std::shared_ptr<DCameraSourceDev>& camDev, DCameraSourceEvent& event)
{
    DHLOGI("DCameraSourceRegistState DoRegisterTask Idempotent");
    return DCAMERA_OK;
}


int32_t DCameraSourceRegistState::DoUnregisterTask(std::shared_ptr<DCameraSourceDev>& camDev, DCameraSourceEvent& event)
{
    std::shared_ptr<DCameraRegistParam> param;
    int32_t ret = event.GetDCameraRegistParam(param);
    if (ret != DCAMERA_OK) {
        return ret;
    }
    ret = camDev->ExecuteUnRegister(param);
    if (ret != DCAMERA_OK) {
        return ret;
    }
    std::shared_ptr<DCameraSourceStateMachine> stateMachine = stateMachine_.lock();
    if (stateMachine == nullptr) {
        DHLOGE("DCameraSourceRegistState DoUnregisterTask can not get stateMachine");
        return DCAMERA_BAD_VALUE;
    }
    stateMachine->UpdateState(DCAMERA_STATE_INIT);
    return DCAMERA_OK;
}

int32_t DCameraSourceRegistState::DoOpenTask(std::shared_ptr<DCameraSourceDev>& camDev, DCameraSourceEvent& event)
{
    int32_t ret = camDev->ExecuteOpenCamera();
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSourceRegistState ExecuteOpenCamera failed, ret: %d", ret);
        return ret;
    }
    return DCAMERA_OK;
}

int32_t DCameraSourceRegistState::DoCloseTask(std::shared_ptr<DCameraSourceDev>& camDev, DCameraSourceEvent& event)
{
    DHLOGI("DCameraSourceRegistState DoCloseTask Idempotent");
    return DCAMERA_OK;
}

int32_t DCameraSourceRegistState::DoEventNofityTask(std::shared_ptr<DCameraSourceDev>& camDev,
    DCameraSourceEvent& event)
{
    std::shared_ptr<DCameraEvent> camEvent;
    int32_t ret = event.GetCameraEvent(camEvent);
    if (ret != DCAMERA_OK) {
        return ret;
    }

    ret = camDev->ExecuteCameraEventNotify(camEvent);
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSourceRegistState DoEventNofityTask ExecuteCameraEventNotify failed, ret: %d", ret);
        return ret;
    }
    return DCAMERA_OK;
}
} // namespace DistributedHardware
} // namespace OHOS
