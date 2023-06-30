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

#include "dcamera_stream_data_process_pipeline_listener.h"

#include "dcamera_stream_data_process.h"

#include "anonymous_string.h"
#include "distributed_camera_errno.h"
#include "distributed_hardware_log.h"

namespace OHOS {
namespace DistributedHardware {
DCameraStreamDataProcessPipelineListener::DCameraStreamDataProcessPipelineListener(
    std::shared_ptr<DCameraStreamDataProcess>& process) : process_(process)
{
}

DCameraStreamDataProcessPipelineListener::~DCameraStreamDataProcessPipelineListener()
{
}

void DCameraStreamDataProcessPipelineListener::OnProcessedVideoBuffer(const std::shared_ptr<DataBuffer>& videoResult)
{
    std::shared_ptr<DCameraStreamDataProcess> process = process_.lock();
    if (process == nullptr) {
        DHLOGE("DCameraStreamDataProcessPipelineListener OnProcessedVideoBuffer not found process");
        return;
    }
    process->OnProcessedVideoBuffer(videoResult);
}

void DCameraStreamDataProcessPipelineListener::OnError(DataProcessErrorType errorType)
{
    std::shared_ptr<DCameraStreamDataProcess> process = process_.lock();
    if (process == nullptr) {
        DHLOGE("DCameraStreamDataProcessPipelineListener OnProcessedVideoBuffer not found process");
        return;
    }
    process->OnError(errorType);
}
} // namespace DistributedHardware
} // namespace OHOS
