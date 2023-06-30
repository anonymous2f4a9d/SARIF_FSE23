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

#ifndef OHOS_ENCODE_DATA_PROCESS_H
#define OHOS_ENCODE_DATA_PROCESS_H

#include "securec.h"
#include <cstdint>
#include <vector>
#include <queue>

#include "surface.h"
#include "media_errors.h"
#include "avcodec_common.h"
#include "format.h"
#include "avsharedmemory.h"
#include "avcodec_video_encoder.h"

#include "data_buffer.h"
#include "distributed_camera_errno.h"
#include "image_common_type.h"
#include "abstract_data_process.h"
#include "dcamera_pipeline_sink.h"

namespace OHOS {
namespace DistributedHardware {
class DCameraPipelineSink;
class EncodeVideoCallback;

class EncodeDataProcess : public AbstractDataProcess, public std::enable_shared_from_this<EncodeDataProcess> {
public:
    EncodeDataProcess(const VideoConfigParams &sourceConfig, const VideoConfigParams &targetConfig,
        const std::weak_ptr<DCameraPipelineSink>& callbackPipSink)
        : sourceConfig_(sourceConfig), targetConfig_(targetConfig), callbackPipelineSink_(callbackPipSink) {}
    ~EncodeDataProcess();

    int32_t InitNode() override;
    int32_t ProcessData(std::vector<std::shared_ptr<DataBuffer>>& inputBuffers) override;
    void ReleaseProcessNode() override;

    void OnError();
    void OnInputBufferAvailable(uint32_t index);
    void OnOutputFormatChanged(const Media::Format &format);
    void OnOutputBufferAvailable(uint32_t index, Media::AVCodecBufferInfo info, Media::AVCodecBufferFlag flag);
    VideoConfigParams GetSourceConfig() const;
    VideoConfigParams GetTargetConfig() const;

private:
    bool IsInEncoderRange(const VideoConfigParams& curConfig);
    bool IsConvertible(const VideoConfigParams& sourceConfig, const VideoConfigParams& targetConfig);
    int32_t InitEncoder();
    int32_t InitEncoderMetadataFormat();
    int32_t InitEncoderBitrateFormat();
    int32_t FeedEncoderInputBuffer(std::shared_ptr<DataBuffer>& inputBuffer);
    sptr<SurfaceBuffer> GetEncoderInputSurfaceBuffer();
    int64_t GetEncoderTimeStamp();
    int32_t GetEncoderOutputBuffer(uint32_t index, Media::AVCodecBufferInfo info);
    int32_t EncodeDone(std::vector<std::shared_ptr<DataBuffer>> outputBuffers);

private:
    const static int32_t ENCODER_STRIDE_ALIGNMENT = 8;
    const static int64_t NORM_YUV420_BUFFER_SIZE = 1920 * 1080 * 3 / 2;
    const static uint32_t MAX_FRAME_RATE = 30;
    const static uint32_t MIN_VIDEO_WIDTH = 320;
    const static uint32_t MIN_VIDEO_HEIGHT = 240;
    const static uint32_t MAX_VIDEO_WIDTH = 1920;
    const static uint32_t MAX_VIDEO_HEIGHT = 1080;
    const static int32_t IDR_FRAME_INTERVAL_MS = 300;
    const static int32_t FIRST_FRAME_OUTPUT_NUM = 2;

    const static int64_t WIDTH_320_HEIGHT_240 = 320 * 240;
    const static int64_t WIDTH_480_HEIGHT_360 = 480 * 360;
    const static int64_t WIDTH_640_HEIGHT_360 = 640 * 360;
    const static int64_t WIDTH_640_HEIGHT_480 = 640 * 480;
    const static int64_t WIDTH_720_HEIGHT_540 = 720 * 540;
    const static int64_t WIDTH_960_HEIGHT_540 = 960 * 540;
    const static int64_t WIDTH_960_HEIGHT_720 = 960 * 720;
    const static int64_t WIDTH_1280_HEIGHT_720 = 1280 * 720;
    const static int64_t WIDTH_1440_HEIGHT_1080 = 1440 * 1080;
    const static int64_t WIDTH_1920_HEIGHT_1080 = 1920 * 1080;
    const static int32_t BITRATE_500000 = 500000;
    const static int32_t BITRATE_1110000 = 1110000;
    const static int32_t BITRATE_1500000 = 1500000;
    const static int32_t BITRATE_1800000 = 1800000;
    const static int32_t BITRATE_2100000 = 2100000;
    const static int32_t BITRATE_2300000 = 2300000;
    const static int32_t BITRATE_2800000 = 2800000;
    const static int32_t BITRATE_3400000 = 3400000;
    const static int32_t BITRATE_5000000 = 5000000;
    const static int32_t BITRATE_6000000 = 6000000;
    const static std::map<std::int64_t, int32_t> ENCODER_BITRATE_TABLE;

    std::mutex mtxEncoderState_;
    std::mutex mtxHoldCount_;
    VideoConfigParams sourceConfig_;
    VideoConfigParams targetConfig_;
    std::weak_ptr<DCameraPipelineSink> callbackPipelineSink_;
    std::shared_ptr<Media::VideoEncoder> videoEncoder_ = nullptr;
    std::shared_ptr<Media::AVCodecCallback> encodeVideoCallback_ = nullptr;
    sptr<Surface> encodeProducerSurface_ = nullptr;

    bool isEncoderProcess_ = false;
    int32_t waitEncoderOutputCount_ = 0;
    int64_t lastFeedEncoderInputBufferTimeUs_ = 0;
    int64_t inputTimeStampUs_ = 0;
    std::string processType_;
    Media::Format metadataFormat_;
    Media::Format encodeOutputFormat_;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_ENCODE_DATA_PROCESS_H
