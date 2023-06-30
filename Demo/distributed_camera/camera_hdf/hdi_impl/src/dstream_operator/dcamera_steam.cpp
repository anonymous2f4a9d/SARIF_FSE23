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

#include "dcamera_steam.h"
#include <condition_variable>
#include <securec.h>
#include "constants.h"
#include "dcamera.h"
#include "distributed_hardware_log.h"

namespace OHOS {
namespace DistributedHardware {
DCamRetCode DCameraStream::InitDCameraStream(const shared_ptr<StreamInfo> &info)
{
    if ((info->streamId_ < 0) || (info->width_ < 0) || (info->height_ < 0) ||
        (info->format_ < 0) || (info->datasapce_ < 0)) {
        DHLOGE("Stream info is invalid.");
        return DCamRetCode::INVALID_ARGUMENT;
    }
    dcStreamId_ = info->streamId_;
    dcStreamInfo_ = std::make_shared<StreamInfo>();
    dcStreamInfo_->streamId_ = info->streamId_;
    dcStreamInfo_->width_ = info->width_;
    dcStreamInfo_->height_ = info->height_;
    dcStreamInfo_->format_ = info->format_;
    dcStreamInfo_->datasapce_ = info->datasapce_;
    dcStreamInfo_->intent_ = info->intent_;
    dcStreamInfo_->tunneledMode_ = info->tunneledMode_;
    dcStreamInfo_->bufferQueue_ = info->bufferQueue_;
    dcStreamInfo_->minFrameDuration_ = info->minFrameDuration_;

    if (dcStreamAttribute_ == nullptr) {
        dcStreamAttribute_ = std::make_shared<StreamAttribute>();
        if (dcStreamAttribute_ == nullptr) {
            return DCamRetCode::FAILED;
        }
    }
    dcStreamAttribute_->streamId_ = dcStreamInfo_->streamId_;
    dcStreamAttribute_->width_ = dcStreamInfo_->width_;
    dcStreamAttribute_->height_ = dcStreamInfo_->height_;
    dcStreamAttribute_->overrideFormat_ = dcStreamInfo_->format_;
    dcStreamAttribute_->overrideDatasapce_ = dcStreamInfo_->datasapce_;
    dcStreamAttribute_->producerUsage_ = HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA;

    dcStreamAttribute_->producerBufferCount_ = BUFFER_QUEUE_SIZE;
    dcStreamAttribute_->maxBatchCaptureCount_ = BUFFER_QUEUE_SIZE;
    dcStreamAttribute_->maxCaptureCount_ = 1;

    DCamRetCode ret = DCamRetCode::SUCCESS;
    if (dcStreamInfo_->bufferQueue_ != nullptr) {
        DCamRetCode ret = InitDCameraBufferManager();
        if (ret != DCamRetCode::SUCCESS) {
            DHLOGE("Cannot init buffer manager.");
        }
    }
    return ret;
}

DCamRetCode DCameraStream::InitDCameraBufferManager()
{
    if (dcStreamInfo_ == nullptr) {
        DHLOGE("Distributed camera stream info is invalid.");
        return DCamRetCode::INVALID_ARGUMENT;
    }

    if (dcStreamInfo_->bufferQueue_ != nullptr) {
        dcStreamProducer_ = OHOS::Surface::CreateSurfaceAsProducer(dcStreamInfo_->bufferQueue_);
    }
    if (dcStreamProducer_ == nullptr) {
        DHLOGE("Distributed camera stream producer is invalid.");
        return DCamRetCode::INVALID_ARGUMENT;
    }
    dcStreamBufferMgr_ = std::make_shared<DBufferManager>();

    DCamRetCode ret = DCamRetCode::SUCCESS;
    if (!isBufferMgrInited_) {
        ret = FinishCommitStream();
    }
    return ret;
}

DCamRetCode DCameraStream::GetDCameraStreamInfo(shared_ptr<StreamInfo> &info)
{
    if (!dcStreamInfo_) {
        DHLOGE("Distributed camera stream info is not init.");
        return DCamRetCode::FAILED;
    }
    info = dcStreamInfo_;
    return DCamRetCode::SUCCESS;
}

DCamRetCode DCameraStream::SetDCameraBufferQueue(const OHOS::sptr<OHOS::IBufferProducer> producer)
{
    if (dcStreamInfo_->bufferQueue_) {
        DHLOGE("Stream [%d] has already have bufferQueue.", dcStreamId_);
        return DCamRetCode::SUCCESS;
    }

    dcStreamInfo_->bufferQueue_ = producer;
    DCamRetCode ret = InitDCameraBufferManager();
    if (ret != DCamRetCode::SUCCESS) {
        DHLOGE("Init distributed camera buffer manager failed.");
    }
    return ret;
}

DCamRetCode DCameraStream::ReleaseDCameraBufferQueue()
{
    DCamRetCode ret = FlushDCameraBuffer();
    if (ret != DCamRetCode::SUCCESS) {
        DHLOGE("Release distributed camera buffer queue failed.");
        return ret;
    }
    dcStreamInfo_->bufferQueue_ = nullptr;
    dcStreamProducer_ = nullptr;
    dcStreamBufferMgr_ = nullptr;

    return DCamRetCode::SUCCESS;
}

DCamRetCode DCameraStream::GetDCameraStreamAttribute(shared_ptr<StreamAttribute> &attribute)
{
    attribute = dcStreamAttribute_;
    if (attribute == nullptr) {
        return DCamRetCode::INVALID_ARGUMENT;
    }
    return DCamRetCode::SUCCESS;
}

DCamRetCode DCameraStream::FinishCommitStream()
{
    if (isBufferMgrInited_) {
        DHLOGI("Stream already inited.");
        return DCamRetCode::SUCCESS;
    }
    if (dcStreamProducer_ == nullptr) {
        DHLOGI("No bufferQueue.");
        return DCamRetCode::SUCCESS;
    }
    dcStreamProducer_->SetQueueSize(BUFFER_QUEUE_SIZE);
    isBufferMgrInited_ = true;

    for (uint32_t i = 0; i < BUFFER_QUEUE_SIZE; i++) {
        GetNextRequest();
    }
    return DCamRetCode::SUCCESS;
}

DCamRetCode DCameraStream::GetNextRequest()
{
    if (isBufferMgrInited_ == false) {
        DHLOGE("BufferManager not be init.");
        return DCamRetCode::INVALID_ARGUMENT;
    }
    if (dcStreamInfo_ == nullptr) {
        DHLOGE("Cannot create buffer manager by invalid streaminfo.");
        return DCamRetCode::INVALID_ARGUMENT;
    }
    if (dcStreamProducer_ == nullptr) {
        DHLOGE("Cannot create a buffer manager by invalid bufferqueue.");
        return DCamRetCode::INVALID_ARGUMENT;
    }

    OHOS::sptr<OHOS::SurfaceBuffer> surfaceBuffer = nullptr;
    int32_t fence = -1;
    int32_t usage = HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA;
    OHOS::BufferRequestConfig config = {
        .width = dcStreamInfo_->width_,
        .height = dcStreamInfo_->height_,
        .strideAlignment = 8,
        .format = dcStreamInfo_->format_,
        .usage = usage,
        .timeout = 0
    };

    OHOS::SurfaceError surfaceError = dcStreamProducer_->RequestBuffer(surfaceBuffer, fence, config);
    if (surfaceError == OHOS::SURFACE_ERROR_NO_BUFFER) {
        DHLOGE("No availiable buffer to request in surface.");
        return DCamRetCode::EXCEED_MAX_NUMBER;
    }

    if (surfaceError != OHOS::SURFACE_ERROR_OK || surfaceBuffer == nullptr) {
        DHLOGE("Get producer buffer failed. [streamId = %d] [sfError = %d]", dcStreamInfo_->streamId_, surfaceError);
        return DCamRetCode::EXCEED_MAX_NUMBER;
    }

    std::shared_ptr<DImageBuffer> imageBuffer = std::make_shared<DImageBuffer>();
    RetCode ret = DBufferManager::SurfaceBufferToDImageBuffer(surfaceBuffer, imageBuffer);
    if (ret != RC_OK) {
        DHLOGE("Convert surface buffer to image buffer failed, streamId = %d.", dcStreamInfo_->streamId_);
        return DCamRetCode::EXCEED_MAX_NUMBER;
    }

    imageBuffer->SetIndex(++index_);
    imageBuffer->SetFenceId(fence);
    ret = dcStreamBufferMgr_->AddBuffer(imageBuffer);
    if (ret != RC_OK) {
        DHLOGE("Add buffer to buffer manager failed. [streamId = %d]", dcStreamInfo_->streamId_);
        return DCamRetCode::EXCEED_MAX_NUMBER;
    }
    DHLOGI("Add new image buffer success: index = %d, fence = %d", imageBuffer->GetIndex(), fence);

    auto itr = bufferConfigMap_.find(imageBuffer);
    if (itr == bufferConfigMap_.end()) {
        auto bufferCfg = std::make_tuple(surfaceBuffer, fence, usage);
        bufferConfigMap_.insert(std::make_pair(imageBuffer, bufferCfg));
    }

    return DCamRetCode::SUCCESS;
}

DCamRetCode DCameraStream::GetDCameraBuffer(shared_ptr<DCameraBuffer> &buffer)
{
    DCamRetCode retCode = GetNextRequest();
    if (retCode != DCamRetCode::SUCCESS && retCode != DCamRetCode::EXCEED_MAX_NUMBER) {
        DHLOGE("Get next request failed.");
        return retCode;
    }

    std::shared_ptr<DImageBuffer> imageBuffer = dcStreamBufferMgr_->AcquireBuffer();
    if (imageBuffer == nullptr) {
        DHLOGE("Cannot get idle buffer.");
        return DCamRetCode::EXCEED_MAX_NUMBER;
    }

    RetCode ret = DBufferManager::DImageBufferToDCameraBuffer(imageBuffer, buffer);
    if (ret != RC_OK) {
        DHLOGE("Convert image buffer to distributed camera buffer failed.");
        return DCamRetCode::FAILED;
    }
    captureBufferCount_++;

    DHLOGI("Get buffer success. address = %p, index = %d, size = %d", buffer->bufferHandle_->virAddr,
        buffer->index_, buffer->size_);
    return DCamRetCode::SUCCESS;
}

DCamRetCode DCameraStream::ReturnDCameraBuffer(const shared_ptr<DCameraBuffer> &buffer)
{
    if (buffer == nullptr) {
        DHLOGE("result buffer is null. [streamId = %d]", dcStreamInfo_->streamId_);
        return DCamRetCode::INVALID_ARGUMENT;
    }

    shared_ptr<DImageBuffer> imageBuffer = nullptr;
    map<shared_ptr<DImageBuffer>, tuple<OHOS::sptr<OHOS::SurfaceBuffer>, int, int>>::iterator iter;
    for (iter = bufferConfigMap_.begin(); iter != bufferConfigMap_.end(); ++iter) {
        if (buffer->index_ == iter->first->GetIndex()) {
            imageBuffer = iter->first;
            break;
        }
    }
    if (imageBuffer == nullptr) {
        DHLOGE("Cannot found image buffer, buffer index = %d.", buffer->index_);
        return DCamRetCode::INVALID_ARGUMENT;
    }

    RetCode ret = dcStreamBufferMgr_->RemoveBuffer(imageBuffer);
    if (ret != RC_OK) {
        DHLOGE("Buffer manager remove buffer failed: %d", ret);
    }

    auto bufCfg = bufferConfigMap_.find(imageBuffer);
    if (bufCfg == bufferConfigMap_.end()) {
        DHLOGE("Cannot get bufferConfig.");
        return INVALID_ARGUMENT;
    }
    auto surfaceBuffer = std::get<0>(bufCfg->second);
    int32_t fence = std::get<1>(bufCfg->second);
    OHOS::BufferFlushConfig flushConf = {
        .damage = { .x = 0, .y = 0, .w = dcStreamInfo_->width_, .h = dcStreamInfo_->height_ },
        .timestamp = 0
    };
    if (dcStreamProducer_ != nullptr) {
        if (dcStreamInfo_->intent_ == StreamIntent::VIDEO) {
            int32_t size = (dcStreamInfo_->width_) * (dcStreamInfo_->height_) * YUV_WIDTH_RATIO / YUV_HEIGHT_RATIO;
            int64_t timeStamp = static_cast<int64_t>(GetCurrentLocalTimeStamp());
            surfaceBuffer->ExtraSet("dataSize", size);
            surfaceBuffer->ExtraSet("isKeyFrame", (int32_t)0);
            surfaceBuffer->ExtraSet("timeStamp", timeStamp);
        }
        int ret = dcStreamProducer_->FlushBuffer(surfaceBuffer, fence, flushConf);
        if (ret != 0) {
            DHLOGI("FlushBuffer error: %d", ret);
        }
    }
    bufferConfigMap_.erase(bufCfg);
    {
        captureBufferCount_--;
        cv_.notify_one();
    }
    return DCamRetCode::SUCCESS;
}

DCamRetCode DCameraStream::FlushDCameraBuffer()
{
    if (dcStreamBufferMgr_ == nullptr || dcStreamProducer_ == nullptr) {
        DHLOGE("BufferManager or Producer is null.");
        return DCamRetCode::SUCCESS;
    }

    if (captureBufferCount_ != 0) {
        DHLOGI("StreamId:%d has request that not return, captureBufferCount=%d",
            dcStreamInfo_->streamId_, captureBufferCount_);
    }
    {
        std::unique_lock<std::mutex> l(lock_);
        cv_.wait(l, [this] { return !captureBufferCount_; });
    }

    while (true) {
        std::shared_ptr<DImageBuffer> imageBuffer = dcStreamBufferMgr_->AcquireBuffer();
        if (imageBuffer == nullptr) {
            auto bufCfg = bufferConfigMap_.find(imageBuffer);
            if (bufCfg == bufferConfigMap_.end()) {
                DHLOGE("Buffer not in map.");
                return DCamRetCode::INVALID_ARGUMENT;
            }
            auto surfaceBuffer = std::get<0>(bufCfg->second);
            int32_t fence = std::get<1>(bufCfg->second);
            OHOS::BufferFlushConfig flushConf = {
                .damage = {
                    .x = 0,
                    .y = 0,
                    .w = dcStreamInfo_->width_,
                    .h = dcStreamInfo_->height_ },
                .timestamp = 0
            };
            if (dcStreamProducer_ != nullptr) {
                dcStreamProducer_->FlushBuffer(surfaceBuffer, fence, flushConf);
            }
            bufferConfigMap_.erase(bufCfg);
        } else {
            break;
        }
    }
    captureBufferCount_ = 0;
    index_ = -1;
    return DCamRetCode::SUCCESS;
}

bool DCameraStream::HasBufferQueue()
{
    if (dcStreamProducer_ == nullptr || isBufferMgrInited_ == false) {
        return false;
    }
    return true;
}
} // namespace DistributedHardware
} // namespace OHOS
