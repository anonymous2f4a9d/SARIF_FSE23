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

#include "dbuffer_manager.h"
#include <buffer_handle_utils.h>
#include "distributed_camera_constants.h"
#include "distributed_hardware_log.h"

namespace OHOS {
namespace DistributedHardware {
std::shared_ptr<DImageBuffer> DBufferManager::AcquireBuffer()
{
    std::unique_lock<std::mutex> l(lock_);

    if (!idleList_.empty()) {
        auto it = idleList_.begin();
        busyList_.splice(busyList_.begin(), idleList_, it);
        DHLOGI("Acquire buffer success, index = %d", (*it)->GetIndex());
        return *it;
    }
    return nullptr;
}

RetCode DBufferManager::AddBuffer(std::shared_ptr<DImageBuffer>& buffer)
{
    std::unique_lock<std::mutex> l(lock_);
    if (idleList_.size() + busyList_.size() >= BUFFER_QUEUE_SIZE) {
        DHLOGI("Buffer list is full, cannot add buffer.");
        return RC_ERROR;
    }
    idleList_.emplace_back(buffer);

    return RC_OK;
}

RetCode DBufferManager::RemoveBuffer(std::shared_ptr<DImageBuffer>& buffer)
{
    std::unique_lock<std::mutex> l(lock_);

    auto it = std::find(busyList_.begin(), busyList_.end(), buffer);
    if (it == busyList_.end()) {
        DHLOGE("Busy list is empty, cannot remove buffer.");
        return RC_ERROR;
    }
    busyList_.erase(it);

    return RC_OK;
}

void DBufferManager::NotifyStop(bool state)
{
    streamStop_ = state;
}

RetCode DBufferManager::SurfaceBufferToDImageBuffer(const OHOS::sptr<OHOS::SurfaceBuffer> &surfaceBuffer,
    const std::shared_ptr<DImageBuffer> &buffer)
{
    if (surfaceBuffer == nullptr) {
        DHLOGE("Convert surface buffer failed, surfaceBuffer is null.");
        return RC_ERROR;
    }

    BufferHandle *bufHandle = surfaceBuffer->GetBufferHandle();
    if (bufHandle == nullptr) {
        DHLOGE("Convert surface buffer failed, BufferHandle is null.");
        return RC_ERROR;
    }
    if ((bufHandle->size <= 0) || (bufHandle->width <= 0) || (bufHandle->height <= 0) || (bufHandle->usage <= 0)) {
        DHLOGE("Convert surface buffer failed, BufferHandle is invalid.");
        return RC_ERROR;
    }

    buffer->SetPhyAddress(bufHandle->phyAddr);
    buffer->SetFileDescriptor(bufHandle->fd);
    buffer->SetStride(bufHandle->stride);
    buffer->SetWidth(bufHandle->width);
    buffer->SetHeight(bufHandle->height);
    buffer->SetFormat(PixelFormatToDCameraFormat(static_cast<PixelFormat>(bufHandle->format)));
    buffer->SetUsage(CameraUsageToGrallocUsage(bufHandle->usage));
    buffer->SetSize(static_cast<uint32_t>(bufHandle->size));
    buffer->SetBufferHandle(bufHandle);

    return RC_OK;
}

uint64_t DBufferManager::CameraUsageToGrallocUsage(const uint64_t cameraUsage)
{
    uint64_t grallocUsage = 0;
    uint64_t test = 1;
    const uint32_t BYTE = 8;
    for (uint32_t i = 0; i < sizeof(cameraUsage) * BYTE; i++) {
        switch (cameraUsage & (test << i)) {
            case CAMERA_USAGE_SW_READ_OFTEN:
                grallocUsage |= HBM_USE_CPU_READ;
                break;
            case CAMERA_USAGE_SW_WRITE_OFTEN:
                grallocUsage |= HBM_USE_CPU_WRITE;
                break;
            case CAMERA_USAGE_MEM_DMA:
                grallocUsage |= HBM_USE_MEM_DMA;
                break;
            default:
                break;
        }
    }

    return grallocUsage;
}

uint32_t DBufferManager::PixelFormatToDCameraFormat(const PixelFormat format)
{
    uint32_t cameraFormat = OHOS_CAMERA_FORMAT_INVALID;
    switch (format) {
        case PIXEL_FMT_RGBA_8888:
            cameraFormat = OHOS_CAMERA_FORMAT_RGBA_8888;
            break;
        case PIXEL_FMT_YCBCR_420_SP:
            cameraFormat = OHOS_CAMERA_FORMAT_YCBCR_420_888;
            break;
        case PIXEL_FMT_YCRCB_420_SP:
            cameraFormat = OHOS_CAMERA_FORMAT_YCRCB_420_SP;
            break;
        default:
            cameraFormat = OHOS_CAMERA_FORMAT_INVALID;
            break;
    }

    return cameraFormat;
}

RetCode DBufferManager::DImageBufferToDCameraBuffer(const std::shared_ptr<DImageBuffer> &imageBuffer,
    std::shared_ptr<DCameraBuffer> &buffer)
{
    BufferHandle *bufHandle = imageBuffer->GetBufferHandle();
    if (bufHandle == nullptr) {
        DHLOGE("Convert image surface buffer failed, BufferHandle is null.");
        return RC_ERROR;
    }
    if ((bufHandle->size <= 0) || (bufHandle->width <= 0) || (bufHandle->height <= 0) || (bufHandle->usage <= 0)) {
        DHLOGE("Convert image surface buffer failed, BufferHandle is invalid.");
        return RC_ERROR;
    }
    buffer->index_ = imageBuffer->GetIndex();
    buffer->size_ = imageBuffer->GetSize();
    buffer->bufferHandle_ = bufHandle;

    return RC_OK;
}
} // namespace DistributedHardware
} // namespace OHOS
