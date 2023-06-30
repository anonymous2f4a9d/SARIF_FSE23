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

#include "dstream_operator_callback.h"
#include "distributed_hardware_log.h"

namespace OHOS {
namespace DistributedHardware {
void DStreamOperatorCallback::OnCaptureStarted(int32_t captureId, const std::vector<int32_t> &streamId)
{
    DHLOGW("DStreamOperatorCallback::OnCaptureStarted enter.");
}

void DStreamOperatorCallback::OnCaptureEnded(int32_t captureId,
    const std::vector<std::shared_ptr<CaptureEndedInfo>> &info)
{
    DHLOGW("DStreamOperatorCallback::OnCaptureEnded enter.");
}

void DStreamOperatorCallback::OnCaptureError(int32_t captureId,
    const std::vector<std::shared_ptr<CaptureErrorInfo>> &info)
{
    DHLOGW("DStreamOperatorCallback::OnCaptureError enter.");
}

void DStreamOperatorCallback::OnFrameShutter(int32_t captureId, const std::vector<int32_t> &streamId,
    uint64_t timestamp)
{
    DHLOGW("DStreamOperatorCallback::OnFrameShutter enter.");
}
} // namespace DistributedHardware
} // namespace OHOS
