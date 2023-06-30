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

#include "encode_data_process.h"

#include<cmath>

#include "display_type.h"
#include "distributed_hardware_log.h"
#include "graphic_common_c.h"

#include "dcamera_utils_tools.h"
#include "encode_video_callback.h"

#ifndef DH_LOG_TAG
#define DH_LOG_TAG "DCDP_NODE_ENCODEC"
#endif

namespace OHOS {
namespace DistributedHardware {
const std::map<int64_t, int32_t> EncodeDataProcess::ENCODER_BITRATE_TABLE = {
    std::map<int64_t, int32_t>::value_type(WIDTH_320_HEIGHT_240, BITRATE_500000),
    std::map<int64_t, int32_t>::value_type(WIDTH_480_HEIGHT_360, BITRATE_1110000),
    std::map<int64_t, int32_t>::value_type(WIDTH_640_HEIGHT_360, BITRATE_1500000),
    std::map<int64_t, int32_t>::value_type(WIDTH_640_HEIGHT_480, BITRATE_1800000),
    std::map<int64_t, int32_t>::value_type(WIDTH_720_HEIGHT_540, BITRATE_2100000),
    std::map<int64_t, int32_t>::value_type(WIDTH_960_HEIGHT_540, BITRATE_2300000),
    std::map<int64_t, int32_t>::value_type(WIDTH_960_HEIGHT_720, BITRATE_2800000),
    std::map<int64_t, int32_t>::value_type(WIDTH_1280_HEIGHT_720, BITRATE_3400000),
    std::map<int64_t, int32_t>::value_type(WIDTH_1440_HEIGHT_1080, BITRATE_5000000),
    std::map<int64_t, int32_t>::value_type(WIDTH_1920_HEIGHT_1080, BITRATE_6000000),
};

EncodeDataProcess::~EncodeDataProcess()
{
    if (isEncoderProcess_) {
        DHLOGD("~EncodeDataProcess : ReleaseProcessNode.");
        ReleaseProcessNode();
    }
}

int32_t EncodeDataProcess::InitNode()
{
    DHLOGD("Common Init DCamera EncodeNode start.");
    if (!(IsInEncoderRange(sourceConfig_) && IsInEncoderRange(targetConfig_))) {
        DHLOGE("Common Source config or target config are invalid.");
        return DCAMERA_BAD_VALUE;
    }
    if (!IsConvertible(sourceConfig_, targetConfig_)) {
        DHLOGE("Common The EncodeNode cannot convert source VideoCodecType %d to target VideoCodecType %d.",
            sourceConfig_.GetVideoCodecType(), targetConfig_.GetVideoCodecType());
        return DCAMERA_BAD_TYPE;
    }
    if (sourceConfig_.GetVideoCodecType() == targetConfig_.GetVideoCodecType()) {
        DHLOGD("Common Disable EncodeNode. The target VideoCodecType %d is the same as the source VideoCodecType %d.",
            sourceConfig_.GetVideoCodecType(), targetConfig_.GetVideoCodecType());
        return DCAMERA_OK;
    }

    int32_t err = InitEncoder();
    if (err != DCAMERA_OK) {
        DHLOGE("Common Init video encoder fail.");
        ReleaseProcessNode();
        return err;
    }
    isEncoderProcess_ = true;
    return DCAMERA_OK;
}

bool EncodeDataProcess::IsInEncoderRange(const VideoConfigParams& curConfig)
{
    return (curConfig.GetWidth() >= MIN_VIDEO_WIDTH || curConfig.GetWidth() <= MAX_VIDEO_WIDTH ||
        curConfig.GetHeight() >= MIN_VIDEO_HEIGHT || curConfig.GetHeight() <= MAX_VIDEO_HEIGHT ||
        curConfig.GetFrameRate() <= MAX_FRAME_RATE);
}

bool EncodeDataProcess::IsConvertible(const VideoConfigParams& sourceConfig, const VideoConfigParams& targetConfig)
{
    return (sourceConfig.GetVideoCodecType() == targetConfig.GetVideoCodecType() ||
        sourceConfig.GetVideoCodecType() == VideoCodecType::NO_CODEC);
}

int32_t EncodeDataProcess::InitEncoder()
{
    DHLOGD("Common Init video encoder.");
    int32_t err = InitEncoderMetadataFormat();
    if (err != DCAMERA_OK) {
        DHLOGE("Common Init video encoder metadata format fail.");
        return err;
    }
    err = InitEncoderBitrateFormat();
    if (err != DCAMERA_OK) {
        DHLOGE("Common Init video encoder bitrate format fail.");
        return err;
    }

    videoEncoder_ = Media::VideoEncoderFactory::CreateByMime(processType_);
    if (videoEncoder_ == nullptr) {
        DHLOGE("Create video encoder failed.");
        return DCAMERA_INIT_ERR;
    }
    encodeVideoCallback_ = std::make_shared<EncodeVideoCallback>(shared_from_this());
    int32_t retVal = videoEncoder_->SetCallback(encodeVideoCallback_);
    if (retVal != Media::MediaServiceErrCode::MSERR_OK) {
        DHLOGE("Set video encoder callback failed.");
        return DCAMERA_INIT_ERR;
    }
    retVal = videoEncoder_->Configure(metadataFormat_);
    if (retVal != Media::MediaServiceErrCode::MSERR_OK) {
        DHLOGE("Set video encoder metadata format failed.");
        return DCAMERA_INIT_ERR;
    }
    encodeProducerSurface_ = videoEncoder_->CreateInputSurface();
    if (encodeProducerSurface_ == nullptr) {
        DHLOGE("Get video encoder producer surface failed.");
        return DCAMERA_INIT_ERR;
    }
    retVal = videoEncoder_->Prepare();
    if (retVal != Media::MediaServiceErrCode::MSERR_OK) {
        DHLOGE("Video encoder prepare failed.");
        return DCAMERA_INIT_ERR;
    }
    retVal = videoEncoder_->Start();
    if (retVal != Media::MediaServiceErrCode::MSERR_OK) {
        DHLOGE("Video encoder start failed.");
        return DCAMERA_INIT_ERR;
    }
    DHLOGI("Common Init video encoder success");
    return DCAMERA_OK;
}

int32_t EncodeDataProcess::InitEncoderMetadataFormat()
{
    DHLOGD("Common Init video encoder metadata format.");
    processType_ = "video/mp4v-es";
    metadataFormat_.PutStringValue("codec_mime", processType_);
    metadataFormat_.PutIntValue("codec_profile", Media::MPEG4Profile::MPEG4_PROFILE_ADVANCED_CODING);

    int32_t width = (int32_t)sourceConfig_.GetWidth();
    int32_t height = (int32_t)sourceConfig_.GetHeight();
    metadataFormat_.PutIntValue("pixel_format",  Media::VideoPixelFormat::RGBA);
    metadataFormat_.PutLongValue("max_input_size", width * height * 4);
    metadataFormat_.PutIntValue("width", width);
    metadataFormat_.PutIntValue("height", height);
    metadataFormat_.PutIntValue("frame_rate", MAX_FRAME_RATE);
    return DCAMERA_OK;
}

int32_t EncodeDataProcess::InitEncoderBitrateFormat()
{
    DHLOGD("Init video encoder bitrate format.");
    if (!(IsInEncoderRange(sourceConfig_) && IsInEncoderRange(targetConfig_))) {
        DHLOGE("Source config or target config are invalid.");
        return DCAMERA_BAD_VALUE;
    }
    metadataFormat_.PutIntValue("i_frame_interval", IDR_FRAME_INTERVAL_MS);
    metadataFormat_.PutIntValue("video_encode_bitrate_mode", Media::VideoEncodeBitrateMode::VBR);

    if (ENCODER_BITRATE_TABLE.empty()) {
        DHLOGD("ENCODER_BITRATE_TABLE is null, use the default bitrate of the encoder.");
        return DCAMERA_OK;
    }
    int64_t pixelformat = static_cast<int64_t>(sourceConfig_.GetWidth() * sourceConfig_.GetHeight());
    int32_t matchedBitrate = BITRATE_6000000;
    int64_t minPixelformatDiff = WIDTH_1920_HEIGHT_1080 - pixelformat;
    for (auto it = ENCODER_BITRATE_TABLE.begin(); it != ENCODER_BITRATE_TABLE.end(); it++) {
        int64_t pixelformatDiff = abs(pixelformat - it->first);
        if (pixelformatDiff == 0) {
            matchedBitrate = it->second;
            break;
        }
        if (minPixelformatDiff >= pixelformatDiff) {
            minPixelformatDiff = pixelformatDiff;
            matchedBitrate = it->second;
        }
    }
    DHLOGD("Source config: width : %d, height : %d, matched bitrate %d.", sourceConfig_.GetWidth(),
        sourceConfig_.GetHeight(), matchedBitrate);
    metadataFormat_.PutIntValue("bitrate", matchedBitrate);
    return DCAMERA_OK;
}

void EncodeDataProcess::ReleaseProcessNode()
{
    DHLOGD("Start release [%d] node : EncodeNode.", nodeRank_);
    isEncoderProcess_ = false;
    if (nextDataProcess_ != nullptr) {
        nextDataProcess_->ReleaseProcessNode();
    }

    {
        std::lock_guard<std::mutex> lck(mtxEncoderState_);
        if (videoEncoder_ != nullptr) {
            DHLOGD("Start release videoEncoder.");
            videoEncoder_->Flush();
            videoEncoder_->Stop();
            videoEncoder_->Release();
            encodeProducerSurface_ = nullptr;
            videoEncoder_ = nullptr;
            encodeVideoCallback_ = nullptr;
        }
    }

    waitEncoderOutputCount_ = 0;
    lastFeedEncoderInputBufferTimeUs_ = 0;
    inputTimeStampUs_ = 0;
    processType_ = "";
    DHLOGD("Release [%d] node : EncodeNode end.", nodeRank_);
}

int32_t EncodeDataProcess::ProcessData(std::vector<std::shared_ptr<DataBuffer>>& inputBuffers)
{
    DHLOGD("Process data in EncodeDataProcess.");
    if (inputBuffers.empty()) {
        DHLOGE("The input data buffers is empty.");
        return DCAMERA_BAD_VALUE;
    }
    if (sourceConfig_.GetVideoCodecType() == targetConfig_.GetVideoCodecType()) {
        DHLOGD("The target VideoCodecType : %d is the same as the source VideoCodecType : %d.",
            sourceConfig_.GetVideoCodecType(), targetConfig_.GetVideoCodecType());
        return EncodeDone(inputBuffers);
    }

    if (videoEncoder_ == nullptr) {
        DHLOGE("The video encoder does not exist before encoding data.");
        return DCAMERA_INIT_ERR;
    }
    size_t bufferSize = 1920 * 1808 * 4;
    if (inputBuffers[0]->Size() > bufferSize) {
        DHLOGE("EncodeNode input buffer size %d error.", inputBuffers[0]->Size());
        return DCAMERA_MEMORY_OPT_ERROR;
    }
    if (!isEncoderProcess_) {
        DHLOGE("EncodeNode occurred error or start release.");
        return DCAMERA_DISABLE_PROCESS;
    }
    int32_t err = FeedEncoderInputBuffer(inputBuffers[0]);
    if (err != DCAMERA_OK) {
        DHLOGE("Feed encoder input Buffer fail.");
        return err;
    }
    {
        std::lock_guard<std::mutex> lck(mtxHoldCount_);
        if (inputTimeStampUs_ == 0) {
            waitEncoderOutputCount_ += FIRST_FRAME_OUTPUT_NUM;
        } else {
            waitEncoderOutputCount_++;
        }
        DHLOGD("Wait encoder output frames number is %d.", waitEncoderOutputCount_);
    }
    return DCAMERA_OK;
}

int32_t EncodeDataProcess::FeedEncoderInputBuffer(std::shared_ptr<DataBuffer>& inputBuffer)
{
    std::lock_guard<std::mutex> lck(mtxEncoderState_);
    DHLOGD("Feed encoder input buffer, buffer size %d.", inputBuffer->Size());
    if (encodeProducerSurface_ == nullptr) {
        DHLOGE("Get encoder input producer surface failed.");
        return DCAMERA_INIT_ERR;
    }
    sptr<SurfaceBuffer> surfacebuffer = GetEncoderInputSurfaceBuffer();
    if (surfacebuffer == nullptr) {
        DHLOGE("Get encoder input producer surface buffer failed.");
        return DCAMERA_BAD_OPERATE;
    }
    uint8_t *addr = static_cast<uint8_t *>(surfacebuffer->GetVirAddr());
    if (addr == nullptr) {
        DHLOGE("SurfaceBuffer address is nullptr");
        encodeProducerSurface_->CancelBuffer(surfacebuffer);
        return DCAMERA_BAD_OPERATE;
    }
    size_t size = static_cast<size_t>(surfacebuffer->GetSize());
    errno_t err = memcpy_s(addr, size, inputBuffer->Data(), inputBuffer->Size());
    DHLOGI("FeedEncoderInputBuffer size: %d, bufferSize: %d, err: %d", size, inputBuffer->Size(), err);
    if (err != EOK) {
        DHLOGE("memcpy_s encoder input producer surface buffer failed.");
        return DCAMERA_MEMORY_OPT_ERROR;
    }
    inputTimeStampUs_ = GetEncoderTimeStamp();
    DHLOGD("Encoder input buffer size %d, timeStamp %lld.", inputBuffer->Size(), (long long)inputTimeStampUs_);
    surfacebuffer->ExtraSet("timeStamp", inputTimeStampUs_);
    BufferFlushConfig flushConfig = { {0, 0, sourceConfig_.GetWidth(), sourceConfig_.GetHeight()}, 0};
    SurfaceError ret = encodeProducerSurface_->FlushBuffer(surfacebuffer, -1, flushConfig);
    if (ret != SURFACE_ERROR_OK) {
        DHLOGE("Flush encoder input producer surface buffer failed.");
        return DCAMERA_BAD_OPERATE;
    }
    return DCAMERA_OK;
}

sptr<SurfaceBuffer> EncodeDataProcess::GetEncoderInputSurfaceBuffer()
{
    BufferRequestConfig requestConfig;
    requestConfig.width = static_cast<int32_t>(sourceConfig_.GetWidth());
    requestConfig.height = static_cast<int32_t>(sourceConfig_.GetHeight());
    requestConfig.usage = HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA;
    requestConfig.timeout = 0;
    requestConfig.strideAlignment = ENCODER_STRIDE_ALIGNMENT;
    requestConfig.format = PixelFormat::PIXEL_FMT_RGBA_8888;
    sptr<SurfaceBuffer> surfacebuffer = nullptr;
    int32_t flushFence = -1;
    GSError err = encodeProducerSurface_->RequestBuffer(surfacebuffer, flushFence, requestConfig);
    if (err != GSERROR_OK || surfacebuffer == nullptr) {
        DHLOGE("Request encoder input producer surface buffer failed, error code: %d.", err);
    }
    return surfacebuffer;
}

int64_t EncodeDataProcess::GetEncoderTimeStamp()
{
    const int64_t nsPerUs = 1000L;
    int64_t nowTimeUs = GetNowTimeStampUs() * nsPerUs;
    return nowTimeUs;
}

int32_t EncodeDataProcess::GetEncoderOutputBuffer(uint32_t index, Media::AVCodecBufferInfo info)
{
    DHLOGD("Get encoder output buffer.");
    if (videoEncoder_ == nullptr) {
        DHLOGE("The video encoder does not exist before output encoded data.");
        return DCAMERA_BAD_VALUE;
    }
    std::shared_ptr<Media::AVSharedMemory> sharedMemoryOutput = videoEncoder_->GetOutputBuffer(index);
    if (sharedMemoryOutput == nullptr) {
        DHLOGE("Failed to get the output shared memory, index : %d", index);
        return DCAMERA_BAD_OPERATE;
    }

    if (info.size <= 0) {
        DHLOGE("AVCodecBufferInfo error, buffer size : %d", info.size);
        return DCAMERA_BAD_VALUE;
    }

    size_t outputMemoDataSize = static_cast<size_t>(info.size);
    DHLOGD("Encoder output buffer size : %d", outputMemoDataSize);
    std::shared_ptr<DataBuffer> bufferOutput = std::make_shared<DataBuffer>(outputMemoDataSize);
    errno_t err = memcpy_s(bufferOutput->Data(), bufferOutput->Size(),
        sharedMemoryOutput->GetBase(), outputMemoDataSize);
    if (err != EOK) {
        DHLOGE("memcpy_s buffer failed.");
        return DCAMERA_MEMORY_OPT_ERROR;
    }
    bufferOutput->SetInt64("timeUs", info.presentationTimeUs);

    std::vector<std::shared_ptr<DataBuffer>> nextInputBuffers;
    nextInputBuffers.push_back(bufferOutput);
    return EncodeDone(nextInputBuffers);
}

int32_t EncodeDataProcess::EncodeDone(std::vector<std::shared_ptr<DataBuffer>> outputBuffers)
{
    DHLOGD("Encoder done.");
    if (outputBuffers.empty()) {
        DHLOGE("The received data buffers is empty.");
        return DCAMERA_BAD_VALUE;
    }

    if (nextDataProcess_ != nullptr) {
        DHLOGD("Send to the next node of the encoder for processing.");
        int32_t err = nextDataProcess_->ProcessData(outputBuffers);
        if (err != DCAMERA_OK) {
            DHLOGE("Someone node after the encoder processes fail.");
        }
        return err;
    }
    DHLOGD("The current node is the last node, and Output the processed video buffer");
    std::shared_ptr<DCameraPipelineSink> targetPipelineSink = callbackPipelineSink_.lock();
    if (targetPipelineSink == nullptr) {
        DHLOGE("callbackPipelineSink_ is nullptr.");
        return DCAMERA_BAD_VALUE;
    }
    targetPipelineSink->OnProcessedVideoBuffer(outputBuffers[0]);
    return DCAMERA_OK;
}

void EncodeDataProcess::OnError()
{
    DHLOGD("EncodeDataProcess : OnError.");
    isEncoderProcess_ = false;
    videoEncoder_->Flush();
    videoEncoder_->Stop();
    std::shared_ptr<DCameraPipelineSink> targetPipelineSink = callbackPipelineSink_.lock();
    if (targetPipelineSink == nullptr) {
        DHLOGE("callbackPipelineSink_ is nullptr.");
        return;
    }
    targetPipelineSink->OnError(DataProcessErrorType::ERROR_PIPELINE_ENCODER);
}

void EncodeDataProcess::OnInputBufferAvailable(uint32_t index)
{
    DHLOGD("The available input buffer index : %d. No operation when using surface input.", index);
}

void EncodeDataProcess::OnOutputFormatChanged(const Media::Format &format)
{
    if (encodeOutputFormat_.GetFormatMap().empty()) {
        DHLOGE("The first changed video encoder output format is null.");
        return;
    }
    encodeOutputFormat_ = format;
}

void EncodeDataProcess::OnOutputBufferAvailable(uint32_t index, Media::AVCodecBufferInfo info,
    Media::AVCodecBufferFlag flag)
{
    if (!isEncoderProcess_) {
        DHLOGE("EncodeNode occurred error or start release.");
        return;
    }
    DHLOGD("Video encode buffer info: presentation TimeUs %lld, size %d, offset %d, flag %d",
        info.presentationTimeUs, info.size, info.offset, flag);
    int32_t err = GetEncoderOutputBuffer(index, info);
    if (err != DCAMERA_OK) {
        DHLOGE("Get encode output Buffer fail.");
        return;
    }
    {
        std::lock_guard<std::mutex> lck(mtxHoldCount_);
        if (waitEncoderOutputCount_ <= 0) {
            DHLOGE("The waitEncoderOutputCount_ = %d.", waitEncoderOutputCount_);
        }
        waitEncoderOutputCount_--;
        DHLOGD("Wait encoder output frames number is %d.", waitEncoderOutputCount_);
    }
    if (videoEncoder_ == nullptr) {
        DHLOGE("The video encoder does not exist before release output buffer index.");
        return;
    }
    int32_t errRelease = videoEncoder_->ReleaseOutputBuffer(index);
    if (errRelease != Media::MediaServiceErrCode::MSERR_OK) {
        DHLOGE("The video encoder release output buffer fail, index : [%d].", index);
    }
}
VideoConfigParams EncodeDataProcess::GetSourceConfig() const
{
    return sourceConfig_;
}

VideoConfigParams EncodeDataProcess::GetTargetConfig() const
{
    return targetConfig_;
}
} // namespace DistributedHardware
} // namespace OHOS
