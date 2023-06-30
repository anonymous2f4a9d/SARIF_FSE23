/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef OHOS_MOCK_DATA_PROCESS_PIPELINE_H
#define OHOS_MOCK_DATA_PROCESS_PIPELINE_H

#include <cstdlib>
#include <vector>

#include "data_buffer.h"
#include "data_process_listener.h"
#include "distributed_camera_errno.h"
#include "idata_process_pipeline.h"
#include "image_common_type.h"

namespace OHOS {
namespace DistributedHardware {
class MockDataProcessPipeline : public IDataProcessPipeline,
    public std::enable_shared_from_this<MockDataProcessPipeline> {
public:
    ~MockDataProcessPipeline()
    {
    }

    int32_t CreateDataProcessPipeline(PipelineType piplineType,
        const VideoConfigParams& sourceConfig, const VideoConfigParams& targetConfig,
        const std::shared_ptr<DataProcessListener>& listener)
    {
        return DCAMERA_OK;
    }

    int32_t ProcessData(std::vector<std::shared_ptr<DataBuffer>>& dataBuffers)
    {
        return DCAMERA_OK;
    }

    void DestroyDataProcessPipeline()
    {
    }
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_MOCK_DATA_PROCESS_PIPELINE_H
