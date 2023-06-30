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

#include "dcamera_video_output_event.h"

namespace OHOS {
namespace DistributedHardware {
DCameraVideoOutputEvent::DCameraVideoOutputEvent(EventSender& sender) : Event(sender)
{
}

DCameraVideoOutputEvent::DCameraVideoOutputEvent(EventSender& sender, const std::shared_ptr<DataBuffer>& param)
    : Event(sender), param_(param)
{
}

std::shared_ptr<DataBuffer> DCameraVideoOutputEvent::GetParam()
{
    return param_;
}
} // namespace DistributedHardware
} // namespace OHOS