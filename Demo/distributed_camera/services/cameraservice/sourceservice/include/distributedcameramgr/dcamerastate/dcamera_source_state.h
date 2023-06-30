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

#ifndef OHOS_DCAMERA_SOURCE_STATE_H
#define OHOS_DCAMERA_SOURCE_STATE_H
#include "dcamera_source_event.h"

namespace OHOS {
namespace DistributedHardware {
typedef enum {
    DCAMERA_STATE_INIT = 0,
    DCAMERA_STATE_REGIST = 1,
    DCAMERA_STATE_OPENED = 2,
    DCAMERA_STATE_CONFIG_STREAM = 3,
    DCAMERA_STATE_CAPTURE = 4,
} DCameraStateType;

class DCameraSourceDev;
class DCameraSourceStateMachine;
class DCameraSourceState {
public:
    DCameraSourceState() {}
    virtual ~DCameraSourceState() {}

    virtual int32_t Execute(std::shared_ptr<DCameraSourceDev>& camDev, DCAMERA_EVENT eventType,
        DCameraSourceEvent& event) = 0;
    virtual DCameraStateType GetStateType() = 0;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_DCAMERA_SOURCE_STATE_H
