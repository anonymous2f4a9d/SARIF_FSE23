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

#ifndef OHOS_DCAMERA_SINK_CONTROLLER_STATE_CALLBACK_H
#define OHOS_DCAMERA_SINK_CONTROLLER_STATE_CALLBACK_H

#include "icamera_operator.h"

#include "dcamera_sink_controller.h"

namespace OHOS {
namespace DistributedHardware {
class DCameraSinkControllerStateCallback : public StateCallback {
public:
    explicit DCameraSinkControllerStateCallback(std::shared_ptr<DCameraSinkController>& controller);
    ~DCameraSinkControllerStateCallback() = default;

    void OnStateChanged(std::shared_ptr<DCameraEvent>& event) override;
    void OnMetadataResult() override;

private:
    std::weak_ptr<DCameraSinkController> controller_;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_DCAMERA_SINK_CONTROLLER_STATE_CALLBACK_H