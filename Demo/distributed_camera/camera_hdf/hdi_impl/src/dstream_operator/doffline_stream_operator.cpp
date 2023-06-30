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

#include "doffline_stream_operator.h"
#include "distributed_hardware_log.h"

namespace OHOS {
namespace DistributedHardware {
CamRetCode DOfflineStreamOperator::CancelCapture(int captureId)
{
    (void)captureId;
    return CamRetCode::METHOD_NOT_SUPPORTED;
}

CamRetCode DOfflineStreamOperator::ReleaseStreams(const std::vector<int>& streamIds)
{
    (void)streamIds;
    return CamRetCode::METHOD_NOT_SUPPORTED;
}

CamRetCode DOfflineStreamOperator::Release()
{
    return CamRetCode::METHOD_NOT_SUPPORTED;
}
} // namespace DistributedHardware
} // namespace OHOS
