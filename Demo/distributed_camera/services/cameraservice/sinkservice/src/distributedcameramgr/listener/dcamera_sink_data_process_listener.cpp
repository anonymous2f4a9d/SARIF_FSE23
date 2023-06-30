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

#include "dcamera_sink_data_process_listener.h"

#include "distributed_hardware_log.h"

namespace OHOS {
namespace DistributedHardware {
DCameraSinkDataProcessListener::DCameraSinkDataProcessListener(
    std::shared_ptr<DCameraSinkDataProcess>& dataProcess) : dataProcess_(dataProcess)
{
}

void DCameraSinkDataProcessListener::OnProcessedVideoBuffer(const std::shared_ptr<DataBuffer>& videoResult)
{
    std::shared_ptr<DCameraSinkDataProcess> dataProcess = dataProcess_.lock();
    if (dataProcess == nullptr) {
        DHLOGE("DCameraSinkDataProcessListener::OnProcessedVideoBuffer dataProcess is null");
        return;
    }
    dataProcess->OnProcessedVideoBuffer(videoResult);
}

void DCameraSinkDataProcessListener::OnError(DataProcessErrorType errorType)
{
    std::shared_ptr<DCameraSinkDataProcess> dataProcess = dataProcess_.lock();
    if (dataProcess == nullptr) {
        DHLOGE("DCameraSinkDataProcessListener::OnError dataProcess is null");
        return;
    }
    dataProcess->OnError(errorType);
}
} // namespace DistributedHardware
} // namespace OHOS