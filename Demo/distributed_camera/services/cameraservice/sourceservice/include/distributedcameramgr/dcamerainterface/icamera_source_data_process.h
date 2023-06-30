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

#ifndef OHOS_ICAMERA_SOURCE_DATA_PROCESS_H
#define OHOS_ICAMERA_SOURCE_DATA_PROCESS_H

#include <vector>

#include "data_buffer.h"
#include "types.h"

namespace OHOS {
namespace DistributedHardware {
class DCameraStreamConfig {
public:
    DCameraStreamConfig(int32_t width, int32_t height, int32_t format, int32_t dataspace,
        DCEncodeType encodeType, DCStreamType streamType)
        : width_(width), height_(height), format_(format), dataspace_(dataspace), encodeType_(encodeType),
        type_(streamType)
    {}
    ~DCameraStreamConfig() = default;
    int32_t width_;
    int32_t height_;
    int32_t format_;
    int32_t dataspace_;
    DCEncodeType encodeType_;
    DCStreamType type_;

    bool operator == (const DCameraStreamConfig& others) const
    {
        return this->width_ == others.width_ && this->height_ == others.height_ && this->format_ == others.format_ &&
            this->dataspace_ == others.dataspace_ && this->encodeType_ == others.encodeType_ &&
            this->type_ == others.type_;
    }

    bool operator < (const DCameraStreamConfig& others) const
    {
        return (this->width_ < others.width_) || ((this->width_ == others.width_) && (this->height_ < others.height_));
    }
};

class ICameraSourceDataProcess {
public:
    virtual ~ICameraSourceDataProcess() = default;

    virtual int32_t FeedStream(std::vector<std::shared_ptr<DataBuffer>>& buffers) = 0;
    virtual int32_t ConfigStreams(std::vector<std::shared_ptr<DCStreamInfo>>& streamInos) = 0;
    virtual int32_t ReleaseStreams(std::vector<int32_t>& streamIds) = 0;
    virtual int32_t StartCapture(std::shared_ptr<DCCaptureInfo>& captureInfo) = 0;
    virtual int32_t StopCapture() = 0;
    virtual void GetAllStreamIds(std::vector<int32_t>& streamIds) = 0;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_ICAMERA_SOURCE_DATA_PROCESS_H
