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

#include "dcamera_post_authorization_event.h"

namespace OHOS {
namespace DistributedHardware {
DCameraPostAuthorizationEvent::DCameraPostAuthorizationEvent(EventSender& sender) : Event(sender)
{
}

DCameraPostAuthorizationEvent::DCameraPostAuthorizationEvent(EventSender& sender,
    std::vector<std::shared_ptr<DCameraCaptureInfo>>& param) : Event(sender), param_(param)
{
}

std::vector<std::shared_ptr<DCameraCaptureInfo>> DCameraPostAuthorizationEvent::GetParam()
{
    return param_;
}
} // namespace DistributedHardware
} // namespace OHOS