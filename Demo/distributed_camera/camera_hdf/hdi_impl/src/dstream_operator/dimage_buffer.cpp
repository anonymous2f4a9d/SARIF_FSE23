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

#include "dimage_buffer.h"
#include <buffer_handle_utils.h>

namespace OHOS {
namespace DistributedHardware {
DImageBuffer::~DImageBuffer()
{
    Free();
}

int32_t DImageBuffer::GetIndex() const
{
    return index_;
}

uint32_t DImageBuffer::GetWidth() const
{
    return width_;
}

uint32_t DImageBuffer::GetHeight() const
{
    return height_;
}

uint32_t DImageBuffer::GetStride() const
{
    return stride_;
}

int32_t DImageBuffer::GetFormat() const
{
    return format_;
}

uint32_t DImageBuffer::GetSize() const
{
    return size_;
}

uint64_t DImageBuffer::GetUsage() const
{
    return usage_;
}

uint64_t DImageBuffer::GetPhyAddress() const
{
    return phyAddr_;
}

int32_t DImageBuffer::GetFileDescriptor() const
{
    return fd_;
}

uint64_t DImageBuffer::GetTimestamp() const
{
    return timeStamp_;
}

uint64_t DImageBuffer::GetFrameNumber() const
{
    return frameNumber_;
}

int32_t DImageBuffer::GetCaptureId() const
{
    return captureId_;
}

bool DImageBuffer::GetValidFlag() const
{
    return valid_;
}

int32_t DImageBuffer::GetFenceId() const
{
    return fenceId_;
}

int32_t DImageBuffer::GetEncodeType() const
{
    return encodeType_;
}

BufferHandle* DImageBuffer::GetBufferHandle() const
{
    return bufHandle_;
}

void DImageBuffer::SetIndex(const int32_t index)
{
    std::lock_guard<std::mutex> l(l_);
    index_ = index;
    return;
}

void DImageBuffer::SetWidth(const uint32_t width)
{
    std::lock_guard<std::mutex> l(l_);
    width_ = width;
    return;
}

void DImageBuffer::SetHeight(const uint32_t height)
{
    std::lock_guard<std::mutex> l(l_);
    height_ = height;
    return;
}

void DImageBuffer::SetStride(const uint32_t stride)
{
    std::lock_guard<std::mutex> l(l_);
    stride_ = stride;
    return;
}

void DImageBuffer::SetFormat(const int32_t format)
{
    std::lock_guard<std::mutex> l(l_);
    format_ = format;
    return;
}

void DImageBuffer::SetSize(const uint32_t size)
{
    std::lock_guard<std::mutex> l(l_);
    size_ = size;
    return;
}

void DImageBuffer::SetUsage(const uint64_t usage)
{
    std::lock_guard<std::mutex> l(l_);
    usage_ = usage;
    return;
}

void DImageBuffer::SetPhyAddress(const uint64_t addr)
{
    std::lock_guard<std::mutex> l(l_);
    phyAddr_ = addr;
    return;
}

void DImageBuffer::SetFileDescriptor(const int32_t fd)
{
    std::lock_guard<std::mutex> l(l_);
    fd_ = fd;
    return;
}

void DImageBuffer::SetTimestamp(const uint64_t timeStamp)
{
    std::lock_guard<std::mutex> l(l_);
    timeStamp_ = timeStamp;
    return;
}

void DImageBuffer::SetFrameNumber(const uint64_t frameNumber)
{
    std::lock_guard<std::mutex> l(l_);
    frameNumber_ = frameNumber;
    return;
}

void DImageBuffer::SetCaptureId(const int32_t id)
{
    std::lock_guard<std::mutex> l(l_);
    captureId_ = id;
    return;
}

void DImageBuffer::SetValidFlag(const bool flag)
{
    std::lock_guard<std::mutex> l(l_);
    valid_ = flag;
    return;
}

void DImageBuffer::SetFenceId(const int32_t fence)
{
    std::lock_guard<std::mutex> l(l_);
    fenceId_ = fence;
    return;
}

void DImageBuffer::SetEncodeType(const int32_t type)
{
    std::lock_guard<std::mutex> l(l_);
    encodeType_ = type;
    return;
}

void DImageBuffer::SetBufferHandle(const BufferHandle* bufHandle)
{
    std::lock_guard<std::mutex> l(l_);
    bufHandle_ = const_cast<BufferHandle*>(bufHandle);
    return;
}

void DImageBuffer::Free()
{
    index_ = -1;
    width_ = 0;
    height_ = 0;
    stride_ = 0;
    format_ = OHOS_CAMERA_FORMAT_INVALID;
    size_ = 0;
    usage_ = 0;
    bufHandle_ = nullptr;
    phyAddr_ = 0;
    fd_ = -1;

    return;
}

bool DImageBuffer::operator==(const DImageBuffer& u)
{
    if (u.GetPhyAddress() == 0 || phyAddr_ == 0) {
        return u.GetIndex() == index_;
    }
    return u.GetPhyAddress() == phyAddr_;
}
} // namespace DistributedHardware
} // namespace OHOS
