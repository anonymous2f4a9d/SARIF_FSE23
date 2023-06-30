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

#ifndef OHOS_ICAMERA_CHANNEL_LISTENER_H
#define OHOS_ICAMERA_CHANNEL_LISTENER_H

#include <cstdint>
#include <memory>
#include <vector>

#include "data_buffer.h"

namespace OHOS {
namespace DistributedHardware {
typedef enum {
    DCAMERA_CHANNEL_STATE_DISCONNECTED = 0,
    DCAMERA_CHANNEL_STATE_CONNECTING = 1,
    DCAMERA_CHANNEL_STATE_CONNECTED = 2,
} DCameraChannelState;

class ICameraChannelListener {
public:
    virtual ~ICameraChannelListener() = default;

    virtual void OnSessionState(int32_t state) = 0;
    virtual void OnSessionError(int32_t eventType, int32_t eventReason, std::string detail) = 0;
    virtual void OnDataReceived(std::vector<std::shared_ptr<DataBuffer>>& buffers) = 0;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_ICAMERA_CHANNEL_LISTENER_H
