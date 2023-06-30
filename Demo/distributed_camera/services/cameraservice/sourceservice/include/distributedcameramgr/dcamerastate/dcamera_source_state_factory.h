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

#ifndef OHOS_DCAMERA_SOURCE_STATE_FACTORY_H
#define OHOS_DCAMERA_SOURCE_STATE_FACTORY_H
#include "single_instance.h"
#include "dcamera_source_state.h"

namespace OHOS {
namespace DistributedHardware {
class DCameraSourceStateFactory {
DECLARE_SINGLE_INSTANCE(DCameraSourceStateFactory);
public:
    std::shared_ptr<DCameraSourceState> CreateState(DCameraStateType stateType,
        std::shared_ptr<DCameraSourceStateMachine>& stateMachine);
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_DCAMERA_SOURCE_STATE_FACTORY_H
