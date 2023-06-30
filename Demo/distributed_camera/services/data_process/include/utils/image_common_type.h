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

#ifndef OHOS_IMAGE_COMMON_TYPE_H
#define OHOS_IMAGE_COMMON_TYPE_H

#include <cstdlib>
#include <cstdint>

namespace OHOS {
namespace DistributedHardware {
enum class PipelineType : int32_t {
    VIDEO = 0,
    PHOTO_JPEG,
};

enum class VideoCodecType : int32_t {
    NO_CODEC = 0,
    CODEC_H264,
    CODEC_H265,
};

enum class Videoformat : int32_t {
    YUVI420 = 0,
    NV12,
    NV21,
};

class VideoConfigParams {
public:
    VideoConfigParams(VideoCodecType videoCodec, Videoformat pixelFormat, uint32_t frameRate, uint32_t width,
        uint32_t height)
        : videoCodec_(videoCodec), pixelFormat_(pixelFormat), frameRate_(frameRate), width_ (width), height_(height)
        {}
    ~VideoConfigParams() = default;

    void SetVideoCodecType(VideoCodecType videoCodec);
    void SetVideoformat(Videoformat pixelFormat);
    void SetFrameRate(uint32_t frameRate);
    void SetWidthAndHeight(uint32_t width, uint32_t height);
    VideoCodecType GetVideoCodecType() const;
    Videoformat GetVideoformat() const;
    uint32_t GetFrameRate() const;
    uint32_t GetWidth() const;
    uint32_t GetHeight() const;

private:
    VideoCodecType videoCodec_;
    Videoformat pixelFormat_;
    uint32_t frameRate_;
    uint32_t width_;
    uint32_t height_;
};

struct ImageUnitInfo {
    Videoformat colorFormat;
    int32_t width;
    int32_t height;
    int32_t alignedWidth;
    int32_t alignedHeight;
    size_t chromaOffset;
    size_t imgSize;
    uint8_t *imgData;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_IMAGE_COMMON_TYPE_H
