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

#include "dcamera_source_state_factory.h"

#include "dcamera_source_capture_state.h"
#include "dcamera_source_config_stream_state.h"
#include "dcamera_source_init_state.h"
#include "dcamera_source_opened_state.h"
#include "dcamera_source_regist_state.h"

#include "distributed_camera_errno.h"
#include "distributed_hardware_log.h"

namespace OHOS {
namespace DistributedHardware {
IMPLEMENT_SINGLE_INSTANCE(DCameraSourceStateFactory);

std::shared_ptr<DCameraSourceState> DCameraSourceStateFactory::CreateState(DCameraStateType stateType,
    std::shared_ptr<DCameraSourceStateMachine>& stateMachine)
{
    std::shared_ptr<DCameraSourceState> state = nullptr;
    switch (stateType) {
        case DCAMERA_STATE_INIT: {
            state = std::make_shared<DCameraSourceInitState>(stateMachine);
            break;
        }
        case DCAMERA_STATE_REGIST: {
            state = std::make_shared<DCameraSourceRegistState>(stateMachine);
            break;
        }
        case DCAMERA_STATE_OPENED: {
            state = std::make_shared<DCameraSourceOpenedState>(stateMachine);
            break;
        }
        case DCAMERA_STATE_CONFIG_STREAM: {
            state = std::make_shared<DCameraSourceConfigStreamState>(stateMachine);
            break;
        }
        case DCAMERA_STATE_CAPTURE: {
            state = std::make_shared<DCameraSourceCaptureState>(stateMachine);
            break;
        }
        default: {
            DHLOGE("DCameraSourceStateFactory create state failed, wrong type %d", stateType);
            return nullptr;
        }
    }

    return state;
}
} // namespace DistributedHardware
} // namespace OHOS
