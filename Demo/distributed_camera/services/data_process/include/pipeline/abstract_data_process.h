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

#ifndef OHOS_ABSTRACT_DATA_PROCESS_H
#define OHOS_ABSTRACT_DATA_PROCESS_H

#include <cstdint>
#include <vector>

#include "data_buffer.h"
#include "image_common_type.h"
#include "distributed_camera_errno.h"

namespace OHOS {
namespace DistributedHardware {
class AbstractDataProcess {
public:
    virtual ~AbstractDataProcess() = default;
    int32_t SetNextNode(std::shared_ptr<AbstractDataProcess>& nextDataProcess);
    void SetNodeRank(size_t curNodeRank);

    virtual int32_t InitNode() = 0;
    virtual int32_t ProcessData(std::vector<std::shared_ptr<DataBuffer>>& inputBuffers) = 0;
    virtual void ReleaseProcessNode() = 0;

protected:
    std::shared_ptr<AbstractDataProcess> nextDataProcess_ = nullptr;
    size_t nodeRank_;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_ABSTRACT_DATA_PROCESS_H
