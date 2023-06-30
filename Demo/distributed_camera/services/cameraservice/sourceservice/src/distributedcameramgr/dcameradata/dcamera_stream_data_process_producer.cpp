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

#include "dcamera_stream_data_process_producer.h"

#include <chrono>
#include <securec.h>

#include "anonymous_string.h"
#include "distributed_camera_constants.h"
#include "distributed_camera_errno.h"
#include "distributed_hardware_log.h"

namespace OHOS {
namespace DistributedHardware {
DCameraStreamDataProcessProducer::DCameraStreamDataProcessProducer(std::string devId, std::string dhId,
    int32_t streamId, DCStreamType streamType)
    : devId_(devId), dhId_(dhId), streamId_(streamId), streamType_(streamType)
{
    DHLOGI("DCameraStreamDataProcessProducer Constructor devId %s dhId %s streamType: %d streamId: %d",
        GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str(), streamType_, streamId_);
    state_ = DCAMERA_PRODUCER_STATE_STOP;
    interval_ = DCAMERA_PRODUCER_ONE_MINUTE_MS / DCAMERA_PRODUCER_FPS_DEFAULT;
}

DCameraStreamDataProcessProducer::~DCameraStreamDataProcessProducer()
{
    DHLOGI("DCameraStreamDataProcessProducer Destructor devId %s dhId %s state: %d streamType: %d",
        GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str(), state_, streamType_);
    if (state_ == DCAMERA_PRODUCER_STATE_START) {
        Stop();
    }
}

void DCameraStreamDataProcessProducer::Start()
{
    DHLOGI("DCameraStreamDataProcessProducer Start producer devId: %s dhId: %s streamType: %d streamId: %d",
        GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str(), streamType_, streamId_);
    state_ = DCAMERA_PRODUCER_STATE_START;
    if (streamType_ == CONTINUOUS_FRAME) {
        producerThread_ = std::thread(&DCameraStreamDataProcessProducer::LooperContinue, this);
        std::string threadName = "DCameraProducer_" + std::to_string(streamType_) + "_" + std::to_string(streamId_);
        auto runner = AppExecFwk::EventRunner::Create(threadName);
        eventHandler_ = std::make_shared<AppExecFwk::EventHandler>(runner);
    } else {
        producerThread_ = std::thread(&DCameraStreamDataProcessProducer::LooperSnapShot, this);
    }
}

void DCameraStreamDataProcessProducer::Stop()
{
    DHLOGI("DCameraStreamDataProcessProducer Stop devId: %s dhId: %s streamType: %d streamId: %d state: %d",
        GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str(), streamType_, streamId_, state_);
    state_ = DCAMERA_PRODUCER_STATE_STOP;
    producerCon_.notify_one();
    producerThread_.join();
    eventHandler_ = nullptr;
    DHLOGI("DCameraStreamDataProcessProducer Stop end devId: %s dhId: %s streamType: %d streamId: %d state: %d",
        GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str(), streamType_, streamId_, state_);
}

void DCameraStreamDataProcessProducer::FeedStream(const std::shared_ptr<DataBuffer>& buffer)
{
    DHLOGD("DCameraStreamDataProcessProducer FeedStream devId %s dhId %s streamType: %d streamSize: %d",
        GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str(), streamType_, buffer->Size());
    std::unique_lock<std::mutex> lock(producerMutex_);
    if (buffers_.size() >= DCAMERA_PRODUCER_MAX_BUFFER_SIZE) {
        DHLOGD("DCameraStreamDataProcessProducer FeedStream OverSize devId %s dhId %s streamType: %d streamSize: %d",
            GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str(), streamType_, buffer->Size());
        buffers_.pop();
    }
    buffers_.push(buffer);
    if (streamType_ == SNAPSHOT_FRAME) {
        producerCon_.notify_one();
    }
}

void DCameraStreamDataProcessProducer::LooperContinue()
{
    DHLOGI("LooperContinue producer devId: %s dhId: %s streamType: %d streamId: %d state: %d",
        GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str(), streamType_, streamId_, state_);
    std::shared_ptr<DHBase> dhBase = std::make_shared<DHBase>();
    dhBase->deviceId_ = devId_;
    dhBase->dhId_ = dhId_;

    while (state_ == DCAMERA_PRODUCER_STATE_START) {
        std::shared_ptr<DataBuffer> buffer = nullptr;
        {
            std::unique_lock<std::mutex> lock(producerMutex_);
            producerCon_.wait_for(lock, std::chrono::milliseconds(interval_), [this] {
                return (this->state_ == DCAMERA_PRODUCER_STATE_STOP);
            });
            if (state_ == DCAMERA_PRODUCER_STATE_STOP || buffers_.empty()) {
                continue;
            }

            buffer = buffers_.front();
            if (buffers_.size() > 1) {
                buffers_.pop();
                DHLOGD("common devId %s dhId %s streamSize: %d bufferSize: %d streamType: %d",
                    GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str(), buffer->Size(), buffers_.size(),
                    streamType_);
            } else {
                DHLOGD("repeat devId %s dhId %s streamSize: %d bufferSize: %d streamType: %d",
                    GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str(), buffer->Size(), buffers_.size(),
                    streamType_);
            }
        }

        auto feedFunc = [this, dhBase, buffer]() {
            FeedStreamToDriver(dhBase, buffer);
        };
        if (eventHandler_ != nullptr) {
            eventHandler_->PostTask(feedFunc);
        }
    }
    DHLOGI("LooperContinue producer end devId: %s dhId: %s streamType: %d streamId: %d state: %d",
        GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str(), streamType_, streamId_, state_);
}

void DCameraStreamDataProcessProducer::LooperSnapShot()
{
    DHLOGI("LooperSnapShot producer devId: %s dhId: %s streamType: %d streamId: %d state: %d",
        GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str(), streamType_, streamId_, state_);
    std::shared_ptr<DHBase> dhBase = std::make_shared<DHBase>();
    dhBase->deviceId_ = devId_;
    dhBase->dhId_ = dhId_;

    while (state_ == DCAMERA_PRODUCER_STATE_START) {
        std::shared_ptr<DataBuffer> buffer = nullptr;
        {
            std::unique_lock<std::mutex> lock(producerMutex_);
            producerCon_.wait(lock, [this] {
                return (!buffers_.empty() || state_ == DCAMERA_PRODUCER_STATE_STOP);
            });
            if (state_ == DCAMERA_PRODUCER_STATE_STOP) {
                continue;
            }

            if (!buffers_.empty()) {
                DHLOGI("LooperSnapShot producer get buffer devId: %s dhId: %s streamType: %d streamId: %d state: %d",
                    GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str(), streamType_, streamId_, state_);
                buffer = buffers_.front();
            }
        }

        if (buffer == nullptr) {
            DHLOGI("LooperSnapShot producer get buffer failed devId: %s dhId: %s streamType: %d streamId: %d state: %d",
                GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str(), streamType_, streamId_, state_);
            continue;
        }
        int32_t ret = FeedStreamToDriver(dhBase, buffer);
        if (ret != DCAMERA_OK) {
            std::this_thread::sleep_for(std::chrono::milliseconds(DCAMERA_PRODUCER_RETRY_SLEEP_MS));
            continue;
        }
        {
            std::unique_lock<std::mutex> lock(producerMutex_);
            buffers_.pop();
        }
    }
    DHLOGI("LooperSnapShot producer end devId: %s dhId: %s streamType: %d streamId: %d state: %d",
        GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str(), streamType_, streamId_, state_);
}

int32_t DCameraStreamDataProcessProducer::FeedStreamToDriver(const std::shared_ptr<DHBase>& dhBase,
    const std::shared_ptr<DataBuffer>& buffer)
{
    DHLOGD("LooperFeed devId %s dhId %s streamSize: %d streamType: %d", GetAnonyString(devId_).c_str(),
        GetAnonyString(dhId_).c_str(), buffer->Size(), streamType_);
    sptr<IDCameraProvider> camHdiProvider = IDCameraProvider::Get();
    if (camHdiProvider == nullptr) {
        DHLOGI("camHdiProvider is nullptr");
        return DCAMERA_BAD_VALUE;
    }
    std::shared_ptr<DCameraBuffer> sharedMemory;
    DCamRetCode retHdi = camHdiProvider->AcquireBuffer(dhBase, streamId_, sharedMemory);
    if (retHdi != SUCCESS) {
        DHLOGE("AcquireBuffer devId: %s dhId: %s streamId: %d ret: %d",
            GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str(), streamId_, retHdi);
        return DCAMERA_BAD_OPERATE;
    }

    int32_t ret = DCAMERA_OK;
    do {
        if (sharedMemory == nullptr) {
            DHLOGE("sharedMemory devId: %s dhId: %s streamId: %d, sharedMemory is null",
                GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str(), streamId_);
            ret = DCAMERA_MEMORY_OPT_ERROR;
            break;
        }

        if (buffer->Size() > sharedMemory->size_) {
            DHLOGE("sharedMemory devId: %s dhId: %s streamId: %d bufSize: %d, addressSize: %d",
                GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str(), streamId_, buffer->Size(),
                sharedMemory->size_);
            ret = DCAMERA_MEMORY_OPT_ERROR;
            break;
        }
        int32_t ret = memcpy_s(sharedMemory->bufferHandle_->virAddr, sharedMemory->size_, buffer->Data(),
            buffer->Size());
        if (ret != EOK) {
            DHLOGE("memcpy_s devId: %s dhId: %s streamId: %d bufSize: %d, addressSize: %d",
                GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str(), streamId_, buffer->Size(),
                sharedMemory->size_);
            ret = DCAMERA_MEMORY_OPT_ERROR;
            break;
        }
        sharedMemory->size_ = buffer->Size();
    } while (0);

    retHdi = camHdiProvider->ShutterBuffer(dhBase, streamId_, sharedMemory);
    if (retHdi != SUCCESS) {
        DHLOGE("ShutterBuffer devId: %s dhId: %s streamId: %d ret: %d",
            GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str(), streamId_, retHdi);
        return DCAMERA_BAD_OPERATE;
    }
    DHLOGD("LooperFeed end devId %s dhId %s streamSize: %d streamType: %d", GetAnonyString(devId_).c_str(),
        GetAnonyString(dhId_).c_str(), buffer->Size(), streamType_);
    return ret;
}
} // namespace DistributedHardware
} // namespace OHOS
