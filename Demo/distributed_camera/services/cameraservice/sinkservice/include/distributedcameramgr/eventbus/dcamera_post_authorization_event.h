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

#ifndef OHOS_DCAMERA_POST_AUTHORIZATION_EVENT_H
#define OHOS_DCAMERA_POST_AUTHORIZATION_EVENT_H

#include "event.h"

#include "dcamera_capture_info_cmd.h"

namespace OHOS {
namespace DistributedHardware {
class DCameraPostAuthorizationEvent : public Event {
TYPEINDENT(DCameraPostAuthorizationEvent);
public:
    explicit DCameraPostAuthorizationEvent(EventSender& sender);
    DCameraPostAuthorizationEvent(EventSender& sender, std::vector<std::shared_ptr<DCameraCaptureInfo>>& param);
    ~DCameraPostAuthorizationEvent() = default;
    std::vector<std::shared_ptr<DCameraCaptureInfo>> GetParam();

private:
    std::vector<std::shared_ptr<DCameraCaptureInfo>> param_;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_DCAMERA_POST_AUTHORIZATION_EVENT_H