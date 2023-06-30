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

#include "dcamera_sink_controller_state_callback.h"

#include "distributed_hardware_log.h"

namespace OHOS {
namespace DistributedHardware {
DCameraSinkControllerStateCallback::DCameraSinkControllerStateCallback(
    std::shared_ptr<DCameraSinkController>& controller) : controller_(controller)
{
}

void DCameraSinkControllerStateCallback::OnStateChanged(std::shared_ptr<DCameraEvent>& event)
{
    std::shared_ptr<DCameraSinkController> controller = controller_.lock();
    if (controller == nullptr) {
        DHLOGE("DCameraSinkControllerStateCallback::OnStateChanged controller is null");
        return;
    }
    controller->OnStateChanged(event);
}

void DCameraSinkControllerStateCallback::OnMetadataResult()
{
    std::shared_ptr<DCameraSinkController> controller = controller_.lock();
    if (controller == nullptr) {
        DHLOGE("DCameraSinkControllerStateCallback::OnMetadataResult controller is null");
        return;
    }
    controller->OnMetadataResult();
}
} // namespace DistributedHardware
} // namespace OHOS