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

#include "dcamera_device_callback.h"
#include "distributed_hardware_log.h"

namespace OHOS {
namespace DistributedHardware {
void DCameraDeviceCallback::OnError(ErrorType type, int32_t errorMsg)
{
    DHLOGW("DCameraDeviceCallback::OnError enter.");
}

void DCameraDeviceCallback::OnResult(uint64_t timestamp, const std::shared_ptr<CameraStandard::CameraMetadata> &result)
{
    DHLOGW("DCameraDeviceCallback::OnResult enter.");
}
} // namespace DistributedHardware
} // namespace OHOS
