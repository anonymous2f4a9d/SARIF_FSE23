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

#ifndef OHOS_DCAMERA_SOURCE_CONTROLLER_CHANNEL_LISTENER_H
#define OHOS_DCAMERA_SOURCE_CONTROLLER_CHANNEL_LISTENER_H

#include <memory>

#include "icamera_channel_listener.h"

namespace OHOS {
namespace DistributedHardware {
class DCameraSourceController;
class DCameraSourceControllerChannelListener : public ICameraChannelListener {
public:
    DCameraSourceControllerChannelListener(std::shared_ptr<DCameraSourceController>& controller);
    ~DCameraSourceControllerChannelListener();
    void OnSessionState(int32_t state) override;
    void OnSessionError(int32_t eventType, int32_t eventReason, std::string detail) override;
    void OnDataReceived(std::vector<std::shared_ptr<DataBuffer>>& buffers) override;

private:
    std::weak_ptr<DCameraSourceController> controller_;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_DCAMERA_SOURCE_CONTROLLER_CHANNEL_LISTENER_H
