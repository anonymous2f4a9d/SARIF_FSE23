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

#ifndef DISTRIBUTED_CONSTANTS_H
#define DISTRIBUTED_CONSTANTS_H

#include <vector>
#include "types.h"

namespace OHOS {
namespace DistributedHardware {
const uint32_t YUV_WIDTH_RATIO = 3;
const uint32_t YUV_HEIGHT_RATIO = 2;

const uint32_t DEVID_MAX_LENGTH = 64;
const uint32_t DHID_MAX_LENGTH = 64;

constexpr size_t DEFAULT_ENTRY_CAPACITY = 100;
constexpr size_t DEFAULT_DATA_CAPACITY = 2000;

const uint32_t SIZE_FMT_LEN = 2;
const uint32_t MAX_SUPPORT_PREVIEW_WIDTH = 1920;
const uint32_t MAX_SUPPORT_PREVIEW_HEIGHT = 1080;
const uint32_t MAX_SUPPORT_PHOTO_WIDTH = 4096;
const uint32_t MAX_SUPPORT_PHOTO_HEIGHT = 3072;
const std::string STAR_SEPARATOR = "*";

const uint32_t MIN_SUPPORT_DEFAULT_FPS = 15;
const uint32_t MAX_SUPPORT_DEFAULT_FPS = 30;

const int64_t MAX_FRAME_DURATION = 1000000000LL / 10;

const uint32_t BUFFER_QUEUE_SIZE = 8;

const uint32_t DEGREE_180 = 180;
const uint32_t DEGREE_240 = 240;

const uint32_t INGNORE_STR_LEN = 2;

const uint32_t WAIT_OPEN_TIMEOUT_SEC = 5;

const std::string ENCODE_TYPE_STR_H264 = "OMX_hisi_video_encoder_avc";
const std::string ENCODE_TYPE_STR_H265 = "OMX_hisi_video_encoder_hevc";
const std::string ENCODE_TYPE_STR_JPEG = "jpeg";

enum DCameraBufferUsage : uint64_t {
    CAMERA_USAGE_SW_READ_OFTEN = (1 << 0),
    CAMERA_USAGE_SW_WRITE_OFTEN = (1 << 1),
    CAMERA_USAGE_MEM_DMA = (1 << 2),
};

/* Each virtual camera must include these default resolution. */
const std::vector<std::pair<uint32_t, uint32_t>> DEFAULT_FMT_VEC {
/*
    pair<uint32_t, uint32_t>(320, 240),
    pair<uint32_t, uint32_t>(480, 360),
    pair<uint32_t, uint32_t>(640, 360),
    pair<uint32_t, uint32_t>(640, 480),
    pair<uint32_t, uint32_t>(720, 540),
    pair<uint32_t, uint32_t>(960, 540),
    pair<uint32_t, uint32_t>(960, 720),
    pair<uint32_t, uint32_t>(1280, 720),
    pair<uint32_t, uint32_t>(1440, 1080),
    pair<uint32_t, uint32_t>(1920, 1080)
*/
};

using DCSceneType = enum _DCSceneType : int32_t {
    PREVIEW = 0,
    VIDEO = 1,
    PHOTO = 2
};

using RetCode = uint32_t;
enum Ret : uint32_t {
    RC_OK = 0,
    RC_ERROR,
};

struct DCResolution {
    int32_t width_;
    int32_t height_;

    DCResolution() : width_(0), height_(0) {}

    DCResolution(int32_t width, int32_t height) : width_(width), height_(height) {}

    bool operator ==(const DCResolution others) const
    {
        return (this->width_ == others.width_) && (this->height_ == others.height_);
    }

    bool operator <(const DCResolution others) const
    {
        return this->width_ < others.width_ ||
            (this->width_ == others.width_ && this->height_ < others.height_);
    }
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // DISTRIBUTED_CONSTANTS_H
