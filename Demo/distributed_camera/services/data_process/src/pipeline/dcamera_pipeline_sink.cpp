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

#include "dcamera_pipeline_sink.h"

#include "distributed_hardware_log.h"

#include "encode_data_process.h"

namespace OHOS {
namespace DistributedHardware {
const std::string DCameraPipelineSink::PIPELINE_OWNER = "Sink";

DCameraPipelineSink::~DCameraPipelineSink()
{
    if (isProcess_) {
        DHLOGD("~DCameraPipelineSink : Destroy sink data process pipeline.");
        DestroyDataProcessPipeline();
    }
}

int32_t DCameraPipelineSink::CreateDataProcessPipeline(PipelineType piplineType,
    const VideoConfigParams& sourceConfig, const VideoConfigParams& targetConfig,
    const std::shared_ptr<DataProcessListener>& listener)
{
    DHLOGD("Create sink data process pipeline.");
    switch (piplineType) {
        case PipelineType::VIDEO:
            if (!(IsInRange(sourceConfig) && IsInRange(targetConfig))) {
                DHLOGE("Source config or target config of sink pipeline are invalid.");
                return DCAMERA_BAD_VALUE;
            }
            break;
        default:
            DHLOGE("JPEG or other pipeline type are not supported in sink pipeline.");
            return DCAMERA_NOT_FOUND;
    }
    if (listener == nullptr) {
        DHLOGE("The process listener of sink pipeline is empty.");
        return DCAMERA_BAD_VALUE;
    }
    if (pipelineHead_ != nullptr) {
        DHLOGD("The sink pipeline already exists.");
        return DCAMERA_OK;
    }

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

bool DCameraPipelineSink::IsInRange(const VideoConfigParams& curConfig)
{
    return (curConfig.GetFrameRate() <= MAX_FRAME_RATE || curConfig.GetWidth() >= MIN_VIDEO_WIDTH ||
        curConfig.GetWidth() <= MAX_VIDEO_WIDTH || curConfig.GetHeight() >= MIN_VIDEO_HEIGHT ||
        curConfig.GetHeight() <= MAX_VIDEO_HEIGHT);
}

int32_t DCameraPipelineSink::InitDCameraPipNodes(const VideoConfigParams& sourceConfig,
    const VideoConfigParams& targetConfig)
{
    DHLOGD("Init sink DCamera pipeline Nodes.");
    if (piplineType_ == PipelineType::PHOTO_JPEG) {
        DHLOGE("JPEG data process is not supported.");
        return DCAMERA_NOT_FOUND;
    }

    pipNodeRanks_.push_back(std::make_shared<EncodeDataProcess>(sourceConfig, targetConfig, shared_from_this()));
    if (pipNodeRanks_.size() == 0) {
        DHLOGD("Creating an empty sink pipeline.");
        pipelineHead_ = nullptr;
        return DCAMERA_BAD_VALUE;
    }
    for (size_t i = 0; i < pipNodeRanks_.size(); i++) {
        pipNodeRanks_[i]->SetNodeRank(i);
        int32_t err = pipNodeRanks_[i]->InitNode();
        if (err != DCAMERA_OK) {
            DHLOGE("Init sink DCamera pipeline Node [%d] failed.", i);
            return DCAMERA_INIT_ERR;
        }
        if (i == 0) {
            continue;
        }
        err = pipNodeRanks_[i - 1]->SetNextNode(pipNodeRanks_[i]);
        if (err != DCAMERA_OK) {
            DHLOGE("Set the next node of Node [%d] failed in sink pipeline.", i - 1);
            return DCAMERA_INIT_ERR;
        }
    }
    DHLOGD("All nodes have been linked in sink pipeline.");
    pipelineHead_ = pipNodeRanks_[0];
    return DCAMERA_OK;
}

int32_t DCameraPipelineSink::ProcessData(std::vector<std::shared_ptr<DataBuffer>>& dataBuffers)
{
    DHLOGD("Process data buffers in sink pipeline.");
    if (piplineType_ == PipelineType::PHOTO_JPEG) {
        DHLOGE("JPEG data process is not supported in sink pipeline.");
        return DCAMERA_NOT_FOUND;
    }
    if (pipelineHead_ == nullptr) {
        DHLOGE("The current sink pipeline node is empty. Processing failed.");
        return DCAMERA_INIT_ERR;
    }
    if (dataBuffers.empty()) {
        DHLOGE("Sink Pipeline Input Data buffers is null.");
        return DCAMERA_BAD_VALUE;
    }
    if (!isProcess_) {
        DHLOGE("Sink pipeline node occurred error or start destroy.");
        return DCAMERA_DISABLE_PROCESS;
    }

    int32_t err = pipelineHead_->ProcessData(dataBuffers);
    if (err != DCAMERA_OK) {
        DHLOGE("Sink plpeline process data buffers fail.");
    }
    return err;
}

void DCameraPipelineSink::DestroyDataProcessPipeline()
{
    DHLOGD("Destroy sink data process pipeline start.");
    isProcess_ = false;
    if (pipelineHead_ != nullptr) {
        pipelineHead_->ReleaseProcessNode();
        pipelineHead_ = nullptr;
    }

    pipNodeRanks_.clear();
    piplineType_ = PipelineType::VIDEO;
    processListener_ = nullptr;
    DHLOGD("Destroy sink data process pipeline end.");
}

void DCameraPipelineSink::OnError(DataProcessErrorType errorType)
{
    DHLOGE("A runtime error occurred in sink pipeline.");
    isProcess_ = false;
    if (processListener_ == nullptr) {
        DHLOGE("The process listener of sink pipeline is empty.");
        return;
    }
    processListener_->OnError(errorType);
}

void DCameraPipelineSink::OnProcessedVideoBuffer(const std::shared_ptr<DataBuffer>& videoResult)
{
    DHLOGD("Sink pipeline output the processed video buffer.");
    if (processListener_ == nullptr) {
        DHLOGE("The process listener of sink pipeline is empty.");
        return;
    }
    processListener_->OnProcessedVideoBuffer(videoResult);
}
} // namespace DistributedHardware
} // namespace OHOS
