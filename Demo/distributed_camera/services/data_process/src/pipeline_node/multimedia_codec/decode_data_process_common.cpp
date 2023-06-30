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

#include "decode_data_process.h"

#include "distributed_hardware_log.h"
#include "graphic_common_c.h"

#include "convert_nv12_to_nv21.h"
#include "dcamera_utils_tools.h"
#include "decode_video_callback.h"

namespace OHOS {
namespace DistributedHardware {
DecodeDataProcess::~DecodeDataProcess()
{
    if (isDecoderProcess_) {
        DHLOGD("~DecodeDataProcess : ReleaseProcessNode.");
        ReleaseProcessNode();
    }
}

int32_t DecodeDataProcess::InitNode()
{
    DHLOGD("Common Init DCamera DecodeNode start.");
    if (!(IsInDecoderRange(sourceConfig_) && IsInDecoderRange(targetConfig_))) {
        DHLOGE("Common Source config or target config are invalid.");
        return DCAMERA_BAD_VALUE;
    }
    if (!IsConvertible(sourceConfig_, targetConfig_)) {
        DHLOGE("Common The DecodeNode can't convert %d to %d.", sourceConfig_.GetVideoCodecType(),
            targetConfig_.GetVideoCodecType());
        return DCAMERA_BAD_TYPE;
    }
    if (sourceConfig_.GetVideoCodecType() == targetConfig_.GetVideoCodecType()) {
        DHLOGD("Disable DecodeNode. The target video codec type %d is the same as the source video codec type %d.",
            sourceConfig_.GetVideoCodecType(), targetConfig_.GetVideoCodecType());
        return DCAMERA_OK;
    }

    InitCodecEvent();
    int32_t err = InitDecoder();
    if (err != DCAMERA_OK) {
        DHLOGE("Common Init video decoder fail.");
        ReleaseProcessNode();
        return err;
    }
    alignedHeight_ = GetAlignedHeight();
    isDecoderProcess_ = true;
    return DCAMERA_OK;
}

int32_t DecodeDataProcess::GetAlignedHeight()
{
    int32_t alignedBits = 32;
    int32_t alignedHeight = static_cast<int32_t>(sourceConfig_.GetHeight());
    if (alignedHeight % alignedBits != 0) {
        alignedHeight = ((alignedHeight / alignedBits) + 1) * alignedBits;
    }
    return alignedHeight;
}

bool DecodeDataProcess::IsInDecoderRange(const VideoConfigParams& curConfig)
{
    return (curConfig.GetWidth() >= MIN_VIDEO_WIDTH || curConfig.GetWidth() <= MAX_VIDEO_WIDTH ||
        curConfig.GetHeight() >= MIN_VIDEO_HEIGHT || curConfig.GetHeight() <= MAX_VIDEO_HEIGHT ||
        curConfig.GetFrameRate() <= MAX_FRAME_RATE);
}

bool DecodeDataProcess::IsConvertible(const VideoConfigParams& sourceConfig, const VideoConfigParams& targetConfig)
{
    return (sourceConfig.GetVideoCodecType() == targetConfig.GetVideoCodecType() ||
        targetConfig.GetVideoCodecType() == VideoCodecType::NO_CODEC);
}

void DecodeDataProcess::InitCodecEvent()
{
    DHLOGD("Common Init DecodeNode eventBus, and add handler for it.");
    eventBusDecode_ = std::make_shared<EventBus>();
    DCameraCodecEvent codecEvent(*this, std::make_shared<CodecPacket>());
    eventBusRegHandleDecode_ = eventBusDecode_->AddHandler<DCameraCodecEvent>(codecEvent.GetType(), *this);

    DHLOGD("Common Add handler for DCamera pipeline eventBus.");
    eventBusRegHandlePipeline2Decode_ = eventBusPipeline_->AddHandler<DCameraCodecEvent>(codecEvent.GetType(), *this);
}

int32_t DecodeDataProcess::InitDecoder()
{
    DHLOGD("Common Init video decoder.");
    int32_t err = InitDecoderMetadataFormat();
    if (err != DCAMERA_OK) {
        DHLOGE("Init video decoder metadata format fail.");
        return err;
    }

    videoDecoder_ = Media::VideoDecoderFactory::CreateByMime(processType_);
    if (videoDecoder_ == nullptr) {
        DHLOGE("Create video decoder failed.");
        return DCAMERA_INIT_ERR;
    }
    decodeVideoCallback_ = std::make_shared<DecodeVideoCallback>(shared_from_this());
    int32_t retVal = videoDecoder_->SetCallback(decodeVideoCallback_);
    if (retVal != Media::MediaServiceErrCode::MSERR_OK) {
        DHLOGE("Set video decoder callback failed.");
        return DCAMERA_INIT_ERR;
    }
    retVal = videoDecoder_->Configure(metadataFormat_);
    if (retVal != Media::MediaServiceErrCode::MSERR_OK) {
        DHLOGE("Set video decoder metadata format failed.");
        return DCAMERA_INIT_ERR;
    }
    retVal = SetDecoderOutputSurface();
    if (retVal != DCAMERA_OK) {
        DHLOGE("Set decoder output surface fail.");
        return retVal;
    }

    retVal = videoDecoder_->Prepare();
    if (retVal != Media::MediaServiceErrCode::MSERR_OK) {
        DHLOGE("Video decoder prepare failed.");
        return DCAMERA_INIT_ERR;
    }
    retVal = videoDecoder_->Start();
    if (retVal != Media::MediaServiceErrCode::MSERR_OK) {
        DHLOGE("Video decoder start failed.");
        return DCAMERA_INIT_ERR;
    }
    return DCAMERA_OK;
}

int32_t DecodeDataProcess::InitDecoderMetadataFormat()
{
    DHLOGD("Common Init video decoder metadata format.");
    processType_ = "video/mp4v-es";
    metadataFormat_.PutStringValue("codec_mime", processType_);
    
    int32_t width = (int32_t)sourceConfig_.GetWidth();
    int32_t height = (int32_t)sourceConfig_.GetHeight();
    metadataFormat_.PutIntValue("pixel_format", Media::VideoPixelFormat::RGBA);
    metadataFormat_.PutIntValue("max_input_size", width * height * 4 * 2);
    metadataFormat_.PutIntValue("width", width);
    metadataFormat_.PutIntValue("height", height);
    metadataFormat_.PutIntValue("frame_rate", MAX_FRAME_RATE);
    return DCAMERA_OK;
}

int32_t DecodeDataProcess::SetDecoderOutputSurface()
{
    DHLOGD("Set the video decoder output surface.");
    if (videoDecoder_ == nullptr) {
        DHLOGE("The video decoder is null.");
        return DCAMERA_BAD_VALUE;
    }

    decodeConsumerSurface_ = Surface::CreateSurfaceAsConsumer();
    if (decodeConsumerSurface_ == nullptr) {
        DHLOGE("Creat the decode consumer surface fail.");
        return DCAMERA_INIT_ERR;
    }
    decodeConsumerSurface_->SetDefaultWidthAndHeight((int32_t)sourceConfig_.GetWidth(),
        (int32_t)sourceConfig_.GetHeight());
    decodeSurfaceListener_ = new DecodeSurfaceListener(decodeConsumerSurface_, shared_from_this());
    if (decodeConsumerSurface_->RegisterConsumerListener(decodeSurfaceListener_) !=
        SURFACE_ERROR_OK) {
        DHLOGE("Register consumer listener fail.");
        return DCAMERA_INIT_ERR;
    }

    sptr<IBufferProducer> surfaceProducer = decodeConsumerSurface_->GetProducer();
    if (surfaceProducer == nullptr) {
        DHLOGE("Get the surface producer of the decode consumer surface fail.");
        return DCAMERA_INIT_ERR;
    }
    decodeProducerSurface_ = Surface::CreateSurfaceAsProducer(surfaceProducer);
    if (decodeProducerSurface_ == nullptr) {
        DHLOGE("Creat the decode producer surface of the decode consumer surface fail.");
        return DCAMERA_INIT_ERR;
    }

    DHLOGD("Set the producer surface to video decoder output surface.");
    int32_t err = videoDecoder_->SetOutputSurface(decodeProducerSurface_);
    if (err != Media::MediaServiceErrCode::MSERR_OK) {
        DHLOGE("Set decoder output surface fail.");
        return DCAMERA_INIT_ERR;
    }
    return DCAMERA_OK;
}

void DecodeDataProcess::ReleaseProcessNode()
{
    DHLOGD("Start release [%d] node : DecodeNode.", nodeRank_);
    isDecoderProcess_ = false;
    if (nextDataProcess_ != nullptr) {
        nextDataProcess_->ReleaseProcessNode();
    }
    if (eventBusDecode_ != nullptr && eventBusPipeline_ != nullptr) {
        DHLOGD("Start release DecodeNode eventBusDecode_ and eventBusPipeline_.");
        DCameraCodecEvent codecEvent(*this, std::make_shared<CodecPacket>());
        eventBusDecode_->RemoveHandler<DCameraCodecEvent>(codecEvent.GetType(), eventBusRegHandleDecode_);
        eventBusDecode_ = nullptr;
        eventBusPipeline_->RemoveHandler<DCameraCodecEvent>(codecEvent.GetType(), eventBusRegHandlePipeline2Decode_);
        eventBusPipeline_ = nullptr;
    }

    {
        std::lock_guard<std::mutex> lck(mtxDecoderState_);
        if (videoDecoder_ != nullptr) {
            DHLOGD("Start release videoDecoder.");
            videoDecoder_->Flush();
            videoDecoder_->Stop();
            videoDecoder_->Release();
            videoDecoder_ = nullptr;
            decodeVideoCallback_ = nullptr;
        }
    }
    if (decodeConsumerSurface_ != nullptr) {
        int32_t ret = decodeConsumerSurface_->UnregisterConsumerListener();
        if (ret != SURFACE_ERROR_OK) {
            DHLOGE("Unregister consumer listener failed. Error type: %d.", ret);
        }
        decodeConsumerSurface_ = nullptr;
        decodeProducerSurface_ = nullptr;
        decodeSurfaceListener_ = nullptr;
    }

    processType_ = "";
    std::queue<std::shared_ptr<DataBuffer>> emptyBuffersQueue;
    inputBuffersQueue_.swap(emptyBuffersQueue);
    std::queue<uint32_t> emptyIndexsQueue;
    availableInputIndexsQueue_.swap(emptyIndexsQueue);
    waitDecoderOutputCount_ = 0;
    lastFeedDecoderInputBufferTimeUs_ = 0;
    outputTimeStampUs_ = 0;
    alignedHeight_ = 0;
    DHLOGD("Release [%d] node : DecodeNode end.", nodeRank_);
}

int32_t DecodeDataProcess::ProcessData(std::vector<std::shared_ptr<DataBuffer>>& inputBuffers)
{
    DHLOGD("Process data in DecodeDataProcess.");
    if (inputBuffers.empty()) {
        DHLOGE("The input data buffers is empty.");
        return DCAMERA_BAD_VALUE;
    }
    if (sourceConfig_.GetVideoCodecType() == targetConfig_.GetVideoCodecType()) {
        DHLOGD("The target VideoCodecType : %d is the same as the source VideoCodecType : %d.",
            sourceConfig_.GetVideoCodecType(), targetConfig_.GetVideoCodecType());
        return DecodeDone(inputBuffers);
    }

    if (videoDecoder_ == nullptr) {
        DHLOGE("The video decoder does not exist before decoding data.");
        return DCAMERA_INIT_ERR;
    }
    if (inputBuffersQueue_.size() > VIDEO_DECODER_QUEUE_MAX) {
        DHLOGE("video decoder input buffers queue over flow.");
        return DCAMERA_INDEX_OVERFLOW;
    }
    int32_t bufferSize = 1920 * 1808 * 4 * 2;
    if (inputBuffers[0]->Size() > bufferSize) {
        DHLOGE("DecodeNode input buffer size %d error.", inputBuffers[0]->Size());
        return DCAMERA_MEMORY_OPT_ERROR;
    }
    if (!isDecoderProcess_) {
        DHLOGE("Decoder node occurred error or start release.");
        return DCAMERA_DISABLE_PROCESS;
    }
    inputBuffersQueue_.push(inputBuffers[0]);
    DHLOGD("Push inputBuffer sucess. BufSize %d, QueueSize %d.", inputBuffers[0]->Size(), inputBuffersQueue_.size());
    int32_t err = FeedDecoderInputBuffer();
    if (err != DCAMERA_OK) {
        int32_t sleepTimeUs = 5000;
        std::this_thread::sleep_for(std::chrono::microseconds(sleepTimeUs));
        DHLOGD("Feed decoder input buffer fail. Try FeedDecoderInputBuffer again.");
        std::shared_ptr<CodecPacket> reFeedInputPacket = std::make_shared<CodecPacket>();
        reFeedInputPacket->SetVideoCodecType(sourceConfig_.GetVideoCodecType());
        DCameraCodecEvent dCamCodecEv(*this, reFeedInputPacket, VideoCodecAction::ACTION_ONCE_AGAIN);
        if (eventBusPipeline_ == nullptr) {
            DHLOGE("eventBusPipeline_ is nullptr.");
            return DCAMERA_BAD_VALUE;
        }
        eventBusPipeline_->PostEvent<DCameraCodecEvent>(dCamCodecEv, POSTMODE::POST_ASYNC);
    }
    return DCAMERA_OK;
}

int32_t DecodeDataProcess::FeedDecoderInputBuffer()
{
    DHLOGD("Feed decoder input buffer.");
    while ((!inputBuffersQueue_.empty()) && (isDecoderProcess_)) {
        std::shared_ptr<DataBuffer> buffer = inputBuffersQueue_.front();
        if (buffer == nullptr || availableInputIndexsQueue_.empty()) {
            DHLOGE("inputBuffersQueue size %d, availableInputIndexsQueue size %d.",
                inputBuffersQueue_.size(), availableInputIndexsQueue_.size());
            return DCAMERA_BAD_VALUE;
        }

        {
            std::lock_guard<std::mutex> lck(mtxDecoderState_);
            if (videoDecoder_ == nullptr) {
                DHLOGE("The video decoder does not exist before GetInputBuffer.");
                return DCAMERA_OK;
            }
            uint32_t index = availableInputIndexsQueue_.front();
            std::shared_ptr<Media::AVSharedMemory> sharedMemoryInput = videoDecoder_->GetInputBuffer(index);
            if (sharedMemoryInput == nullptr) {
                DHLOGE("Failed to obtain the input shared memory corresponding to the [%d] index.", index);
                return DCAMERA_BAD_VALUE;
            }
            size_t inputMemoDataSize = static_cast<size_t>(sharedMemoryInput->GetSize());
            errno_t err = memcpy_s(sharedMemoryInput->GetBase(), inputMemoDataSize, buffer->Data(), buffer->Size());
            if (err != EOK) {
                DHLOGE("memcpy_s buffer failed.");
                return DCAMERA_MEMORY_OPT_ERROR;
            }
            int64_t timeUs = GetDecoderTimeStamp();
            DHLOGD("Decoder input buffer size %d, timeStamp %lld.", buffer->Size(), (long long)timeUs);
            Media::AVCodecBufferInfo bufferInfo {timeUs, static_cast<int32_t>(buffer->Size()), 0};
            int32_t ret = videoDecoder_->QueueInputBuffer(index, bufferInfo,
                Media::AVCODEC_BUFFER_FLAG_NONE);
            if (ret != Media::MediaServiceErrCode::MSERR_OK) {
                DHLOGE("queue Input buffer failed.");
                return DCAMERA_BAD_OPERATE;
            }
        }

        inputBuffersQueue_.pop();
        DHLOGD("Push inputBuffer sucess. inputBuffersQueue size is %d.", inputBuffersQueue_.size());

        {
            std::lock_guard<std::mutex> lck(mtxHoldCount_);
            availableInputIndexsQueue_.pop();
            waitDecoderOutputCount_++;
            DHLOGD("Wait decoder output frames number is %d.", waitDecoderOutputCount_);
        }
    }
    return DCAMERA_OK;
}

int64_t DecodeDataProcess::GetDecoderTimeStamp()
{
    int64_t TimeDifferenceStampUs = 0;
    int64_t nowTimeUs = GetNowTimeStampUs();
    if (lastFeedDecoderInputBufferTimeUs_ == 0) {
        lastFeedDecoderInputBufferTimeUs_ = nowTimeUs;
        return TimeDifferenceStampUs;
    }
    TimeDifferenceStampUs = nowTimeUs - lastFeedDecoderInputBufferTimeUs_;
    lastFeedDecoderInputBufferTimeUs_ = nowTimeUs;
    return TimeDifferenceStampUs;
}

void DecodeDataProcess::GetDecoderOutputBuffer(const sptr<Surface>& surface)
{
    DHLOGD("Get decoder output buffer.");
    if (surface == nullptr) {
        DHLOGE("Get decode consumer surface failed.");
        return;
    }
    Rect damage = {0, 0, 0, 0};
    int32_t acquireFence = 0;
    int64_t timeStampUs = 0;
    sptr<SurfaceBuffer> surfaceBuffer = nullptr;
    GSError ret = surface->AcquireBuffer(surfaceBuffer, acquireFence, timeStampUs, damage);
    if (ret != GSERROR_OK || surfaceBuffer == nullptr) {
        DHLOGE("Acquire surface buffer failed!");
        return;
    }
    int32_t alignedWidth = surfaceBuffer->GetStride();
    int32_t alignedHeight = alignedHeight_;
    DHLOGD("OutputBuffer alignedWidth %d, alignedHeight %d, TimeUs %lld.", alignedWidth, alignedHeight, timeStampUs);
    CopyDecodedImage(surfaceBuffer, timeStampUs, alignedWidth, alignedHeight);
    surface->ReleaseBuffer(surfaceBuffer, -1);
    outputTimeStampUs_ = timeStampUs;
    {
        std::lock_guard<std::mutex> lck(mtxHoldCount_);
        if (waitDecoderOutputCount_ <= 0) {
            DHLOGE("The waitDecoderOutputCount_ = %d.", waitDecoderOutputCount_);
        }
        if (outputTimeStampUs_ == 0) {
            waitDecoderOutputCount_ -= FIRST_FRAME_INPUT_NUM;
        } else {
            waitDecoderOutputCount_--;
        }
        DHLOGD("Wait decoder output frames number is %d.", waitDecoderOutputCount_);
    }
}

void DecodeDataProcess::CopyDecodedImage(const sptr<SurfaceBuffer>& surBuf, int64_t timeStampUs, int32_t alignedWidth,
    int32_t alignedHeight)
{
    if (surBuf == nullptr) {
        DHLOGE("surface buffer is null!");
        return;
    }
    size_t validDecodedImageSize = static_cast<size_t>(sourceConfig_.GetWidth() * sourceConfig_.GetHeight() * 4);
    size_t surfaceBufSize = static_cast<size_t>(surBuf->GetSize());
    if (validDecodedImageSize > surfaceBufSize) {
        DHLOGE("Buffer size error, validDecodedImageSize %d, surBufSize %d.",
            validDecodedImageSize, surBuf->GetSize());
        return;
    }
    std::shared_ptr<DataBuffer> bufferOutput = std::make_shared<DataBuffer>(validDecodedImageSize);
    uint8_t *addr = static_cast<uint8_t *>(surBuf->GetVirAddr());
    errno_t err = memcpy_s(bufferOutput->Data(), bufferOutput->Size(), addr, validDecodedImageSize);
    if (err != EOK) {
        DHLOGE("memcpy_s surface buffer failed.");
        return;
    }
    bufferOutput->SetInt64("timeUs", timeStampUs);
    bufferOutput->SetInt32("Videoformat", static_cast<int32_t>(sourceConfig_.GetVideoformat()));
    bufferOutput->SetInt32("alignedWidth", static_cast<int32_t>(sourceConfig_.GetWidth()));
    bufferOutput->SetInt32("alignedHeight", static_cast<int32_t>(sourceConfig_.GetHeight()));
    bufferOutput->SetInt32("width", static_cast<int32_t>(sourceConfig_.GetWidth()));
    bufferOutput->SetInt32("height", static_cast<int32_t>(sourceConfig_.GetHeight()));
    PostOutputDataBuffers(bufferOutput);
}

int32_t DecodeDataProcess::CopyYUVPlaneByRow(const ImageUnitInfo& srcImgInfo, const ImageUnitInfo& dstImgInfo)
{
    int32_t ret = CheckCopyImageInfo(srcImgInfo, dstImgInfo);
    if (ret != DCAMERA_OK) {
        DHLOGE("Check CopyImageUnitInfo failed.");
        return ret;
    }
    errno_t err = EOK;
    int32_t srcDataOffset = 0;
    int32_t dstDataOffset = 0;
    for (int32_t yh = 0; yh < dstImgInfo.height; yh++) {
        err = memcpy_s(dstImgInfo.imgData + dstDataOffset, dstImgInfo.chromaOffset - dstDataOffset,
            srcImgInfo.imgData + srcDataOffset, dstImgInfo.width);
        if (err != EOK) {
            DHLOGE("memcpy_s YPlane in line[%d] failed.", yh);
            return DCAMERA_MEMORY_OPT_ERROR;
        }
        dstDataOffset += dstImgInfo.alignedWidth;
        srcDataOffset += srcImgInfo.alignedWidth;
    }
    DHLOGD("Copy Yplane end, dstDataOffset %d, srcDataOffset %d, validYPlaneSize %d.",
        dstDataOffset, srcDataOffset, dstImgInfo.chromaOffset);

    int32_t y2UvRatio = 2;
    dstDataOffset = dstImgInfo.chromaOffset;
    srcDataOffset = srcImgInfo.chromaOffset;
    for (int32_t uvh = 0; uvh < dstImgInfo.height / y2UvRatio; uvh++) {
        err = memcpy_s(dstImgInfo.imgData + dstDataOffset, dstImgInfo.imgSize - dstDataOffset,
            srcImgInfo.imgData + srcDataOffset, dstImgInfo.width);
        if (err != EOK) {
            DHLOGE("memcpy_s UVPlane in line[%d] failed.", uvh);
            return DCAMERA_MEMORY_OPT_ERROR;
        }
        dstDataOffset += dstImgInfo.alignedWidth;
        srcDataOffset += srcImgInfo.alignedWidth;
    }
    DHLOGD("Copy UVplane end, dstDataOffset %d, srcDataOffset %d.", dstDataOffset, srcDataOffset);
    return DCAMERA_OK;
}

int32_t DecodeDataProcess::CheckCopyImageInfo(const ImageUnitInfo& srcImgInfo, const ImageUnitInfo& dstImgInfo)
{
    if (srcImgInfo.imgData == nullptr || dstImgInfo.imgData == nullptr) {
        DHLOGE("The imgData of srcImgInfo or the imgData of dstImgInfo are null!");
        return DCAMERA_BAD_VALUE;
    }
    if (srcImgInfo.colorFormat != dstImgInfo.colorFormat) {
        DHLOGE("CopyInfo error : srcImgInfo colorFormat %d, dstImgInfo colorFormat %d.",
            srcImgInfo.colorFormat, dstImgInfo.colorFormat);
        return DCAMERA_BAD_VALUE;
    }

    if (!IsCorrectImageUnitInfo(srcImgInfo)) {
        DHLOGE("srcImginfo fail: width %d, height %d, alignedWidth %d, alignedHeight %d, chromaOffset %lld, " +
            "imgSize %lld.", srcImgInfo.width, srcImgInfo.height, srcImgInfo.alignedWidth, srcImgInfo.alignedHeight,
            srcImgInfo.chromaOffset, srcImgInfo.imgSize);
        return DCAMERA_BAD_VALUE;
    }
    if (!IsCorrectImageUnitInfo(dstImgInfo)) {
        DHLOGE("dstImginfo fail: width %d, height %d, alignedWidth %d, alignedHeight %d, chromaOffset %lld, " +
            "imgSize %lld.", dstImgInfo.width, dstImgInfo.height, dstImgInfo.alignedWidth, dstImgInfo.alignedHeight,
            dstImgInfo.chromaOffset, dstImgInfo.imgSize);
        return DCAMERA_BAD_VALUE;
    }

    if (dstImgInfo.width > srcImgInfo.alignedWidth || dstImgInfo.height > srcImgInfo.alignedHeight) {
        DHLOGE("Comparison ImgInfo fail: dstwidth %d, dstheight %d, srcAlignedWidth %d, srcAlignedHeight %d.",
            dstImgInfo.width, dstImgInfo.height, srcImgInfo.alignedWidth, srcImgInfo.alignedHeight);
        return DCAMERA_BAD_VALUE;
    }
    return DCAMERA_OK;
}

bool DecodeDataProcess::IsCorrectImageUnitInfo(const ImageUnitInfo& imgInfo)
{
    int32_t y2UvRatio = 2;
    int32_t bytesPerPixel = 3;
    size_t expectedImgSize = static_cast<size_t>(imgInfo.alignedWidth * imgInfo.alignedHeight *
                                                 bytesPerPixel / y2UvRatio);
    size_t expectedChromaOffset = static_cast<size_t>(imgInfo.alignedWidth * imgInfo.alignedHeight);
    return (imgInfo.width <= imgInfo.alignedWidth && imgInfo.height <= imgInfo.alignedHeight &&
        imgInfo.imgSize >= expectedImgSize && imgInfo.chromaOffset == expectedChromaOffset);
}

void DecodeDataProcess::PostOutputDataBuffers(std::shared_ptr<DataBuffer>& outputBuffer)
{
    if (eventBusDecode_ == nullptr || outputBuffer == nullptr) {
        DHLOGE("eventBusDecode_ or outputBuffer is null.");
        return;
    }
    std::vector<std::shared_ptr<DataBuffer>> multiDataBuffers;
    multiDataBuffers.push_back(outputBuffer);
    std::shared_ptr<CodecPacket> transNextNodePacket = std::make_shared<CodecPacket>(VideoCodecType::NO_CODEC,
        multiDataBuffers);
    DCameraCodecEvent dCamCodecEv(*this, transNextNodePacket, VideoCodecAction::NO_ACTION);
    eventBusDecode_->PostEvent<DCameraCodecEvent>(dCamCodecEv, POSTMODE::POST_ASYNC);
    DHLOGD("Send video decoder output asynchronous DCameraCodecEvents success.");
}

int32_t DecodeDataProcess::DecodeDone(std::vector<std::shared_ptr<DataBuffer>> outputBuffers)
{
    DHLOGD("Decoder Done.");
    if (outputBuffers.empty()) {
        DHLOGE("The received data buffers is empty.");
        return DCAMERA_BAD_VALUE;
    }

    if (nextDataProcess_ != nullptr) {
        DHLOGD("Send to the next node of the decoder for processing.");
        int32_t err = nextDataProcess_->ProcessData(outputBuffers);
        if (err != DCAMERA_OK) {
            DHLOGE("Someone node after the decoder processes fail.");
        }
        return err;
    }
    DHLOGD("The current node is the last node, and Output the processed video buffer");
    std::shared_ptr<DCameraPipelineSource> targetPipelineSource = callbackPipelineSource_.lock();
    if (targetPipelineSource == nullptr) {
        DHLOGE("callbackPipelineSource_ is nullptr.");
        return DCAMERA_BAD_VALUE;
    }
    targetPipelineSource->OnProcessedVideoBuffer(outputBuffers[0]);
    return DCAMERA_OK;
}

void DecodeDataProcess::OnEvent(DCameraCodecEvent& ev)
{
    DHLOGD("Receiving asynchronous DCameraCodecEvents.");
    std::shared_ptr<CodecPacket> receivedCodecPacket = ev.GetCodecPacket();
    VideoCodecAction action = ev.GetAction();
    switch (action) {
        case VideoCodecAction::NO_ACTION: {
            if (receivedCodecPacket == nullptr) {
                DHLOGE("the received codecPacket of action [%d] is null.", action);
                OnError();
                return;
            }
            DecodeDone(receivedCodecPacket->GetDataBuffers());
            break;
        }
        case VideoCodecAction::ACTION_ONCE_AGAIN:
            DHLOGD("Try FeedDecoderInputBuffer again.");
            FeedDecoderInputBuffer();
            return;
        default:
            DHLOGD("The action : %d is not supported.", action);
            return;
    }
}

void DecodeDataProcess::OnError()
{
    DHLOGD("DecodeDataProcess : OnError.");
    isDecoderProcess_ = false;
    videoDecoder_->Stop();
    std::shared_ptr<DCameraPipelineSource> targetPipelineSource = callbackPipelineSource_.lock();
    if (targetPipelineSource == nullptr) {
        DHLOGE("callbackPipelineSource_ is nullptr.");
        return;
    }
    targetPipelineSource->OnError(DataProcessErrorType::ERROR_PIPELINE_DECODER);
}

void DecodeDataProcess::OnInputBufferAvailable(uint32_t index)
{
    DHLOGD("DecodeDataProcess::OnInputBufferAvailable");
    std::lock_guard<std::mutex> lck(mtxHoldCount_);
    if (availableInputIndexsQueue_.size() > VIDEO_DECODER_QUEUE_MAX) {
        DHLOGE("Video decoder available indexs queue overflow.");
        return;
    }
    DHLOGD("Video decoder available indexs queue push index [%d].", index);
    availableInputIndexsQueue_.push(index);
}

void DecodeDataProcess::OnOutputFormatChanged(const Media::Format &format)
{
    if (decodeOutputFormat_.GetFormatMap().empty()) {
        DHLOGE("The first changed video decoder output format is null.");
        return;
    }
    decodeOutputFormat_ = format;
}

void DecodeDataProcess::OnOutputBufferAvailable(uint32_t index, const Media::AVCodecBufferInfo& info,
    const Media::AVCodecBufferFlag& flag)
{
    if (!isDecoderProcess_) {
        DHLOGE("Decoder node occurred error or start release.");
        return;
    }
    DHLOGD("Video decode buffer info: presentation TimeUs %lld, size %d, offset %d, flag %d",
        info.presentationTimeUs, info.size, info.offset, flag);
    outputInfo_ = info;
    {
        std::lock_guard<std::mutex> lck(mtxDecoderState_);
        if (videoDecoder_ == nullptr) {
            DHLOGE("The video decoder does not exist before decoding data.");
            return;
        }
        int32_t errRelease = videoDecoder_->ReleaseOutputBuffer(index, true);
        if (errRelease != Media::MediaServiceErrCode::MSERR_OK) {
            DHLOGE("The video decoder output decoded data to surface fail, index : [%d].", index);
        }
    }
}

VideoConfigParams DecodeDataProcess::GetSourceConfig() const
{
    return sourceConfig_;
}

VideoConfigParams DecodeDataProcess::GetTargetConfig() const
{
    return targetConfig_;
}

void DecodeSurfaceListener::OnBufferAvailable()
{
    DHLOGD("DecodeSurfaceListener : OnBufferAvailable.");
    std::shared_ptr<DecodeDataProcess> targetDecoderNode = decodeVideoNode_.lock();
    if (targetDecoderNode == nullptr) {
        DHLOGE("decodeVideoNode_ is nullptr.");
        return;
    }
    targetDecoderNode->GetDecoderOutputBuffer(surface_);
}

void DecodeSurfaceListener::SetSurface(const sptr<Surface>& surface)
{
    surface_ = surface;
}

void DecodeSurfaceListener::SetDecodeVideoNode(const std::weak_ptr<DecodeDataProcess>& decodeVideoNode)
{
    decodeVideoNode_ = decodeVideoNode;
}

DecodeSurfaceListener::~DecodeSurfaceListener()
{
    DHLOGD("DecodeSurfaceListener : ~DecodeSurfaceListener.");
    surface_ = nullptr;
}
} // namespace DistributedHardware
} // namespace OHOS
