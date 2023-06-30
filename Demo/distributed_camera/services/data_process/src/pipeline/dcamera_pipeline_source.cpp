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

#include "dcamera_pipeline_source.h"

#include "distributed_hardware_log.h"

#include "decode_data_process.h"
#include "fps_controller_process.h"

namespace OHOS {
namespace DistributedHardware {
const std::string DCameraPipelineSource::PIPELINE_OWNER = "Source";

DCameraPipelineSource::~DCameraPipelineSource()
{
    if (isProcess_) {
        DHLOGD("~DCameraPipelineSource : Destroy source data process pipeline.");
        DestroyDataProcessPipeline();
    }
}

int32_t DCameraPipelineSource::CreateDataProcessPipeline(PipelineType piplineType,
    const VideoConfigParams& sourceConfig, const VideoConfigParams& targetConfig,
    const std::shared_ptr<DataProcessListener>& listener)
{
    DHLOGD("Create source data process pipeline.");
    switch (piplineType) {
        case PipelineType::VIDEO:
            if (!(IsInRange(sourceConfig) && IsInRange(targetConfig))) {
                DHLOGE("Source config or target config of source pipeline are invalid.");
                return DCAMERA_BAD_VALUE;
            }
            break;
        default:
            DHLOGE("JPEG or other pipeline type are not supported in source pipeline.");
            return DCAMERA_NOT_FOUND;
    }
    if (listener == nullptr) {
        DHLOGE("The process listener of source pipeline is empty.");
        return DCAMERA_BAD_VALUE;
    }

    if (pipelineHead_ != nullptr) {
        DHLOGD("The source pipeline already exists.");
        return DCAMERA_OK;
    }

    InitDCameraPipEvent();
    int32_t err = InitDCameraPipNodes(sourceConfig, targetConfig);
    if (err != DCAMERA_OK) {
        DestroyDataProcessPipeline();
        return err;
    }
    piplineType_ = piplineType;
    processListener_ = listener;
    isProcess_ = true;
    return DCAMERA_OK;
}

bool DCameraPipelineSource::IsInRange(const VideoConfigParams& curConfig)
{
    return (curConfig.GetFrameRate() <= MAX_FRAME_RATE || curConfig.GetWidth() >= MIN_VIDEO_WIDTH ||
        curConfig.GetWidth() <= MAX_VIDEO_WIDTH || curConfig.GetHeight() >= MIN_VIDEO_HEIGHT ||
        curConfig.GetHeight() <= MAX_VIDEO_HEIGHT);
}

void DCameraPipelineSource::InitDCameraPipEvent()
{
    DHLOGD("Init source DCamera pipeline event to asynchronously process data.");
    eventBusSource_ = std::make_shared<EventBus>();
    DCameraPipelineEvent pipelineEvent(*this, std::make_shared<PipelineConfig>());
    eventBusSource_->AddHandler<DCameraPipelineEvent>(pipelineEvent.GetType(), *this);
}

int32_t DCameraPipelineSource::InitDCameraPipNodes(const VideoConfigParams& sourceConfig,
    const VideoConfigParams& targetConfig)
{
    DHLOGD("Init source DCamera pipeline Nodes.");
    if (piplineType_ == PipelineType::PHOTO_JPEG) {
        DHLOGE("JPEG data process is not supported.");
        return DCAMERA_NOT_FOUND;
    }
    if (eventBusSource_ == nullptr) {
        DHLOGE("eventBusSource is nullptr.");
        return DCAMERA_BAD_VALUE;
    }
    pipNodeRanks_.push_back(std::make_shared<DecodeDataProcess>(sourceConfig, targetConfig,
        eventBusSource_, shared_from_this()));
    if (pipNodeRanks_.size() == 0) {
        DHLOGD("Creating an empty source pipeline.");
        pipelineHead_ = nullptr;
        return DCAMERA_BAD_VALUE;
    }
    for (size_t i = 0; i < pipNodeRanks_.size(); i++) {
        pipNodeRanks_[i]->SetNodeRank(i);
        int32_t err = pipNodeRanks_[i]->InitNode();
        if (err != DCAMERA_OK) {
            DHLOGE("Init source DCamera pipeline Node [%d] failed.", i);
            return DCAMERA_INIT_ERR;
        }
        if (i == 0) {
            continue;
        }
        err = pipNodeRanks_[i - 1]->SetNextNode(pipNodeRanks_[i]);
        if (err != DCAMERA_OK) {
            DHLOGE("Set the next node of Node [%d] failed in source pipeline.", i - 1);
            return DCAMERA_INIT_ERR;
        }
    }
    DHLOGD("All nodes have been linked in source pipeline.");
    pipelineHead_ = pipNodeRanks_[0];
    return DCAMERA_OK;
}

int32_t DCameraPipelineSource::ProcessData(std::vector<std::shared_ptr<DataBuffer>>& dataBuffers)
{
    DHLOGD("Process data buffers in source pipeline.");
    if (piplineType_ == PipelineType::PHOTO_JPEG) {
        DHLOGE("JPEG data process is not supported in source pipeline.");
        return DCAMERA_NOT_FOUND;
    }
    if (pipelineHead_ == nullptr) {
        DHLOGE("The current source pipeline node is empty. Processing failed.");
        return DCAMERA_INIT_ERR;
    }
    if (dataBuffers.empty()) {
        DHLOGE("Source Pipeline Input data buffers is empty.");
        return DCAMERA_BAD_VALUE;
    }
    if (!isProcess_) {
        DHLOGE("Source Pipeline node occurred error or start destroy.");
        return DCAMERA_DISABLE_PROCESS;
    }

    DHLOGD("Send asynchronous event to process data in source pipeline.");
    std::shared_ptr<PipelineConfig> pipConfigSource = std::make_shared<PipelineConfig>(piplineType_,
        PIPELINE_OWNER, dataBuffers);
    DCameraPipelineEvent dCamPipelineEvent(*this, pipConfigSource);
    if (eventBusSource_ == nullptr) {
        DHLOGE("eventBusSource_ is nullptr.");
        return DCAMERA_BAD_VALUE;
    }
    eventBusSource_->PostEvent<DCameraPipelineEvent>(dCamPipelineEvent, POSTMODE::POST_ASYNC);
    return DCAMERA_OK;
}

void DCameraPipelineSource::DestroyDataProcessPipeline()
{
    DHLOGD("Destroy source data process pipeline start.");
    isProcess_ = false;
    if (pipelineHead_ != nullptr) {
        pipelineHead_->ReleaseProcessNode();
        pipelineHead_ = nullptr;
    }
    eventBusSource_ = nullptr;
    processListener_ = nullptr;
    pipNodeRanks_.clear();
    piplineType_ = PipelineType::VIDEO;
    DHLOGD("Destroy source data process pipeline end.");
}

void DCameraPipelineSource::OnEvent(DCameraPipelineEvent& ev)
{
    DHLOGD("Receive asynchronous event then start process data in source pipeline.");
    std::shared_ptr<PipelineConfig> pipelineConfig = ev.GetPipelineConfig();
    std::vector<std::shared_ptr<DataBuffer>> inputBuffers = pipelineConfig->GetDataBuffers();
    if (inputBuffers.empty()) {
        DHLOGE("Receiving process data buffers is empty in source pipeline.");
        OnError(ERROR_PIPELINE_EVENTBUS);
        return;
    }
    pipelineHead_->ProcessData(inputBuffers);
}

void DCameraPipelineSource::OnError(DataProcessErrorType errorType)
{
    DHLOGE("A runtime error occurred in the source pipeline.");
    isProcess_ = false;
    if (processListener_ == nullptr) {
        DHLOGE("The process listener of source pipeline is empty.");
        return;
    }
    processListener_->OnError(errorType);
}

void DCameraPipelineSource::OnProcessedVideoBuffer(const std::shared_ptr<DataBuffer>& videoResult)
{
    DHLOGD("Source pipeline output the processed video buffer.");
    if (processListener_ == nullptr) {
        DHLOGE("The process listener of source pipeline is empty.");
        return;
    }
    processListener_->OnProcessedVideoBuffer(videoResult);
}
} // namespace DistributedHardware
} // namespace OHOS
