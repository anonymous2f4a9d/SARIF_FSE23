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

#ifndef OHOS_IDATA_PROCESS_PIPELINE_H
#define OHOS_IDATA_PROCESS_PIPELINE_H

#include <cstdlib>
#include <vector>

#include "data_buffer.h"
#include "image_common_type.h"
#include "distributed_camera_errno.h"
#include "data_process_listener.h"

namespace OHOS {
namespace DistributedHardware {
class IDataProcessPipeline {
public:
    virtual ~IDataProcessPipeline() = default;

    virtual int32_t CreateDataProcessPipeline(PipelineType piplineType, const VideoConfigParams& sourceConfig,
        const VideoConfigParams& targetConfig, const std::shared_ptr<DataProcessListener>& listener) = 0;
    virtual int32_t ProcessData(std::vector<std::shared_ptr<DataBuffer>>& dataBuffers) = 0;
    virtual void DestroyDataProcessPipeline() = 0;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_IDATA_PROCESS_PIPELINE_H
