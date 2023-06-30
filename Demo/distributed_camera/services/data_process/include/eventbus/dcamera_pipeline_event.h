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

#ifndef OHOS_DCAMERA_PIPELINE_EVENT_H
#define OHOS_DCAMERA_PIPELINE_EVENT_H

#include <vector>

#include "event.h"
#include "data_buffer.h"
#include "image_common_type.h"

namespace OHOS {
namespace DistributedHardware {
enum class PipelineAction : int32_t {
    NO_ACTION = 0,
};

class PipelineConfig {
public:
    PipelineConfig() : pipelineType_(PipelineType::VIDEO) {}
    PipelineConfig(PipelineType pipelineType, const std::string& pipelineOwner,
        const std::vector<std::shared_ptr<DataBuffer>>& multiDataBuffers)
        : pipelineType_(pipelineType), pipelineOwner_(pipelineOwner), multiDataBuffers_(multiDataBuffers) {}
    ~PipelineConfig() = default;

    void SetPipelineType(PipelineType pipelineType)
    {
        pipelineType_ = pipelineType;
    }

    PipelineType GetPipelineType() const
    {
        return pipelineType_;
    }

    void SetPipelineOwner(std::string pipelineOwner)
    {
        pipelineOwner_ = pipelineOwner;
    }

    std::string GetPipelineOwner() const
    {
        return pipelineOwner_;
    }

    void SetDataBuffers(std::vector<std::shared_ptr<DataBuffer>>& multiDataBuffers)
    {
        multiDataBuffers_ = multiDataBuffers;
    }

    std::vector<std::shared_ptr<DataBuffer>> GetDataBuffers() const
    {
        return multiDataBuffers_;
    }

private:
    PipelineType pipelineType_;
    std::string pipelineOwner_;
    std::vector<std::shared_ptr<DataBuffer>> multiDataBuffers_;
};

class DCameraPipelineEvent : public Event {
    TYPEINDENT(DCameraPipelineEvent)
public:
    DCameraPipelineEvent(EventSender& sender, const std::shared_ptr<PipelineConfig>& pipelineConfig)
        : Event(sender), pipelineConfig_(pipelineConfig), action_(PipelineAction::NO_ACTION) {}
    DCameraPipelineEvent(EventSender& sender, const std::shared_ptr<PipelineConfig>& pipelineConfig,
        PipelineAction otherAction)
        : Event(sender), pipelineConfig_(pipelineConfig), action_(otherAction) {}
    ~DCameraPipelineEvent() = default;

    std::shared_ptr<PipelineConfig> GetPipelineConfig() const
    {
        return pipelineConfig_;
    }

    PipelineAction GetAction() const
    {
        return action_;
    }

private:
    std::shared_ptr<PipelineConfig> pipelineConfig_ = nullptr;
    PipelineAction action_;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_DCAMERA_PIPELINE_EVENT_H
