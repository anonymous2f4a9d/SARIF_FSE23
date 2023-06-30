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

#ifndef DISTRIBUTED_CAMERA_IMAGE_BUFFER_H
#define DISTRIBUTED_CAMERA_IMAGE_BUFFER_H

#include <mutex>
#include "constants.h"
#include "distributed_camera_constants.h"

namespace OHOS {
namespace DistributedHardware {
class DImageBuffer {
public:
    DImageBuffer() = default;
    virtual ~DImageBuffer();

    int32_t GetIndex() const;
    uint32_t GetWidth() const;
    uint32_t GetHeight() const;
    uint32_t GetStride() const;
    int32_t GetFormat() const;
    uint32_t GetSize() const;
    uint64_t GetUsage() const;
    uint64_t GetPhyAddress() const;
    int32_t GetFileDescriptor() const;
    uint64_t GetTimestamp() const;
    uint64_t GetFrameNumber() const;
    int32_t GetCaptureId() const;
    bool GetValidFlag() const;
    int32_t GetFenceId() const;
    int32_t GetEncodeType() const;
    BufferHandle* GetBufferHandle() const;

    void SetIndex(const int32_t index);
    void SetWidth(const uint32_t width);
    void SetHeight(const uint32_t height);
    void SetStride(const uint32_t stride);
    void SetFormat(const int32_t format);
    void SetSize(const uint32_t size);
    void SetUsage(const uint64_t usage);
    void SetPhyAddress(const uint64_t addr);
    void SetFileDescriptor(const int32_t fd);
    void SetTimestamp(const uint64_t timestamp);
    void SetFrameNumber(const uint64_t frameNumber);
    void SetCaptureId(const int32_t id);
    void SetValidFlag(const bool flag);
    void SetFenceId(const int32_t fence);
    void SetEncodeType(const int32_t type);
    void SetBufferHandle(const BufferHandle* bufHandle);

    void Free();
    bool operator==(const DImageBuffer& u);

private:
    int32_t index_ = -1;
    uint32_t width_ = 0;
    uint32_t height_ = 0;
    uint32_t stride_ = 0;
    uint32_t format_ = OHOS_CAMERA_FORMAT_INVALID;
    uint32_t size_ = 0;
    uint64_t usage_ = 0;
    uint64_t phyAddr_ = 0;
    int32_t fd_ = -1;
    uint64_t frameNumber_ = 0;
    uint64_t timeStamp_ = 0;
    int32_t captureId_ = -1;
    bool valid_ = true;
    int32_t fenceId_ = -1;
    int32_t encodeType_ = 0;
    BufferHandle* bufHandle_ = nullptr;

    std::mutex l_;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // DISTRIBUTED_CAMERA_IMAGE_BUFFER_H
