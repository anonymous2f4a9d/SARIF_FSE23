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

#include "dcamera_stream_data_process.h"

#include "anonymous_string.h"
#include "distributed_camera_constants.h"
#include "distributed_camera_errno.h"
#include "distributed_hardware_log.h"

#include "dcamera_pipeline_source.h"
#include "dcamera_stream_data_process_pipeline_listener.h"

namespace OHOS {
namespace DistributedHardware {
DCameraStreamDataProcess::DCameraStreamDataProcess(std::string devId, std::string dhId, DCStreamType streamType)
    : devId_(devId), dhId_(dhId), streamType_(streamType)
{
    DHLOGI("DCameraSourceDataProcess Constructor devId %s dhId %s", GetAnonyString(devId_).c_str(),
        GetAnonyString(dhId_).c_str());
    pipeline_ = nullptr;
    listener_ = nullptr;
}

DCameraStreamDataProcess::~DCameraStreamDataProcess()
{
    DHLOGI("DCameraStreamDataProcess Destructor devId %s dhId %s streamType: %d", GetAnonyString(devId_).c_str(),
        GetAnonyString(dhId_).c_str(), streamType_);
    streamIds_.clear();
    producers_.clear();
    if (pipeline_ != nullptr) {
        pipeline_->DestroyDataProcessPipeline();
    }
}

void DCameraStreamDataProcess::FeedStream(std::shared_ptr<DataBuffer>& buffer)
{
    DHLOGD("DCameraStreamDataProcess FeedStream devId %s dhId %s streamType %d streamSize: %d",
        GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str(), streamType_, buffer->Size());
    switch (streamType_) {
        case SNAPSHOT_FRAME: {
            FeedStreamToSnapShot(buffer);
            break;
        }
        case CONTINUOUS_FRAME: {
            FeedStreamToContinue(buffer);
            break;
        }
        default:
            break;
    }
}

void DCameraStreamDataProcess::ConfigStreams(std::shared_ptr<DCameraStreamConfig>& dstConfig,
    std::set<int32_t>& streamIds)
{
    DHLOGI("DCameraStreamDataProcess ConfigStreams devId %s dhId %s streamType: %d",
        GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str(), streamType_);
    dstConfig_ = dstConfig;
    streamIds_ = streamIds;
}

void DCameraStreamDataProcess::ReleaseStreams(std::set<int32_t>& streamIds)
{
    for (auto iter = streamIds.begin(); iter != streamIds.end(); iter++) {
        int32_t streamId = *iter;
        DHLOGI("DCameraStreamDataProcess ReleaseStreams devId %s dhId %s streamId: %d", GetAnonyString(devId_).c_str(),
            GetAnonyString(dhId_).c_str(), streamId);
        streamIds_.erase(streamId);
        auto producerIter = producers_.find(streamId);
        if (producerIter == producers_.end()) {
            continue;
        }
        producerIter->second->Stop();
        producers_.erase(streamId);
    }
}

void DCameraStreamDataProcess::StartCapture(std::shared_ptr<DCameraStreamConfig>& srcConfig,
    std::set<int32_t>& streamIds)
{
    srcConfig_ = srcConfig;
    if (streamType_ == CONTINUOUS_FRAME) {
        CreatePipeline();
    }
    for (auto iter = streamIds_.begin(); iter != streamIds_.end(); iter++) {
        uint32_t streamId = *iter;
        DHLOGI("DCameraStreamDataProcess StartCapture devId %s dhId %s streamType: %d streamId: %d",
            GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str(), streamType_, streamId);
        if (streamIds.find(streamId) == streamIds.end()) {
            continue;
        }

        DHLOGI("DCameraStreamDataProcess StartCapture findProducer devId %s dhId %s streamType: %d streamId: %d",
            GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str(), streamType_, streamId);
        auto producerIter = producers_.find(streamId);
        if (producerIter != producers_.end()) {
            continue;
        }
        DHLOGI("DCameraStreamDataProcess StartCapture CreateProducer devId %s dhId %s streamType: %d streamId: %d",
            GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str(), streamType_, streamId);
        producers_[streamId] = std::make_shared<DCameraStreamDataProcessProducer>(devId_, dhId_, streamId, streamType_);
        producers_[streamId]->Start();
    }
}

void DCameraStreamDataProcess::StopCapture()
{
    DHLOGI("DCameraStreamDataProcess StopCapture devId %s dhId %s", GetAnonyString(devId_).c_str(),
        GetAnonyString(dhId_).c_str());
    if (streamType_ == CONTINUOUS_FRAME) {
        DestroyPipeline();
    }
    auto iter = producers_.begin();
    while (iter != producers_.end()) {
        iter->second->Stop();
        iter = producers_.erase(iter);
    }
}

void DCameraStreamDataProcess::GetAllStreamIds(std::set<int32_t>& streamIds)
{
    streamIds = streamIds_;
}

void DCameraStreamDataProcess::FeedStreamToSnapShot(const std::shared_ptr<DataBuffer>& buffer)
{
    DHLOGD("DCameraStreamDataProcess FeedStreamToSnapShot devId %s dhId %s streamType %d streamSize: %d",
        GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str(), streamType_, buffer->Size());
    for (auto iter = producers_.begin(); iter != producers_.end(); iter++) {
        iter->second->FeedStream(buffer);
    }
}

void DCameraStreamDataProcess::FeedStreamToContinue(const std::shared_ptr<DataBuffer>& buffer)
{
    DHLOGD("DCameraStreamDataProcess FeedStreamToContinue devId %s dhId %s streamType %d streamSize: %d",
        GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str(), streamType_, buffer->Size());
    std::vector<std::shared_ptr<DataBuffer>> buffers;
    buffers.push_back(buffer);
    if (pipeline_ == nullptr) {
        DHLOGE("DCameraStreamDataProcess FeedStreamToContinue pipeline null devId %s dhId %s type: %d streamSize: %d",
            GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str(), streamType_, buffer->Size());
        return;
    }
    int32_t ret = pipeline_->ProcessData(buffers);
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraStreamDataProcess FeedStreamToContinue pipeline ProcessData failed, ret: %d", ret);
    }
}

void DCameraStreamDataProcess::OnProcessedVideoBuffer(const std::shared_ptr<DataBuffer>& videoResult)
{
    DHLOGI("DCameraStreamDataProcess OnProcessedVideoBuffer devId %s dhId %s streamType: %d streamSize: %d",
        GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str(), streamType_, videoResult->Size());
    for (auto iter = producers_.begin(); iter != producers_.end(); iter++) {
        iter->second->FeedStream(videoResult);
    }
}

void DCameraStreamDataProcess::OnError(DataProcessErrorType errorType)
{
    DHLOGE("DCameraStreamDataProcess OnError pipeline errorType: %d", errorType);
}

void DCameraStreamDataProcess::CreatePipeline()
{
    if (pipeline_ != nullptr) {
        DHLOGI("DCameraStreamDataProcess CreatePipeline already exist, devId %s dhId %s",
            GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str());
        return;
    }
    pipeline_ = std::make_shared<DCameraPipelineSource>();
    auto process = std::shared_ptr<DCameraStreamDataProcess>(shared_from_this());
    listener_ = std::make_shared<DCameraStreamDataProcessPipelineListener>(process);
    VideoConfigParams srcParams(GetPipelineCodecType(srcConfig_->encodeType_), GetPipelineFormat(srcConfig_->format_),
        DCAMERA_PRODUCER_FPS_DEFAULT, srcConfig_->width_, srcConfig_->height_);
    VideoConfigParams dstParams(GetPipelineCodecType(dstConfig_->encodeType_), GetPipelineFormat(dstConfig_->format_),
        DCAMERA_PRODUCER_FPS_DEFAULT, dstConfig_->width_, dstConfig_->height_);
    int32_t ret = pipeline_->CreateDataProcessPipeline(PipelineType::VIDEO, srcParams, dstParams, listener_);
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraStreamDataProcess CreateDataProcessPipeline type: %d failed, ret: %d", PipelineType::VIDEO, ret);
    }
}

void DCameraStreamDataProcess::DestroyPipeline()
{
    if (pipeline_ == nullptr) {
        return;
    }
    pipeline_->DestroyDataProcessPipeline();
    pipeline_ = nullptr;
}

VideoCodecType DCameraStreamDataProcess::GetPipelineCodecType(DCEncodeType encodeType)
{
    VideoCodecType codecType;
    switch (encodeType) {
        case ENCODE_TYPE_H264:
            codecType = VideoCodecType::CODEC_H264;
            break;
        case ENCODE_TYPE_H265:
            codecType = VideoCodecType::CODEC_H265;
            break;
        default:
            codecType = VideoCodecType::NO_CODEC;
            break;
    }
    return codecType;
}

Videoformat DCameraStreamDataProcess::GetPipelineFormat(int32_t format)
{
    return Videoformat::NV21;
}
} // namespace DistributedHardware
} // namespace OHOS
