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

#ifndef OHOS_DCAMERA_SOURCE_STATE_MACHINE_H
#define OHOS_DCAMERA_SOURCE_STATE_MACHINE_H
#include "dcamera_source_event.h"
#include "dcamera_source_state.h"

namespace OHOS {
namespace DistributedHardware {
class DCameraSourceDev;
class DCameraSourceStateMachine : public std::enable_shared_from_this<DCameraSourceStateMachine> {
public:
    DCameraSourceStateMachine(std::shared_ptr<DCameraSourceDev>& camDev);
    ~DCameraSourceStateMachine();
    int32_t Execute(DCAMERA_EVENT eventType, DCameraSourceEvent& event);
    void UpdateState(DCameraStateType stateType);

private:
    std::shared_ptr<DCameraSourceState> currentState_;
    std::weak_ptr<DCameraSourceDev> camDev_;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_DCAMERA_SOURCE_STATE_MACHINE_H
