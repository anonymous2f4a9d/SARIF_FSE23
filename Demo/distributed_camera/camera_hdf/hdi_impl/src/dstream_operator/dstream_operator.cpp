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

#include "dstream_operator.h"
#include "dbuffer_manager.h"
#include "dcamera_provider.h"
#include "dcamera_utils_tools.h"
#include "distributed_hardware_log.h"
#include "json/json.h"
#include "metadata_utils.h"

namespace OHOS {
namespace DistributedHardware {
DStreamOperator::DStreamOperator(std::shared_ptr<DMetadataProcessor> &dMetadataProcessor)
    : dMetadataProcessor_(dMetadataProcessor)
{
    DHLOGI("DStreamOperator::ctor, instance = %p", this);
}

CamRetCode DStreamOperator::IsStreamsSupported(OperationMode mode,
    const std::shared_ptr<CameraStandard::CameraMetadata> &modeSetting,
    const std::vector<std::shared_ptr<StreamInfo>> &info,
    StreamSupportType &type)
{
    (void)mode;
    (void)type;

    if (modeSetting == nullptr) {
        DHLOGE("Input invalid argument: modeSetting:%p.",
            modeSetting.get());
        return CamRetCode::INVALID_ARGUMENT;
    }

    for (auto it : info) {
        int id = it->streamId_;
        if (halStreamMap_.find(id) != halStreamMap_.end()) {
            DHLOGE("Repeat streamId.");
            return CamRetCode::INVALID_ARGUMENT;
        }
    }
    return CamRetCode::NO_ERROR;
}

CamRetCode DStreamOperator::CreateStreams(const std::vector<std::shared_ptr<StreamInfo>>& streamInfos)
{
    DHLOGI("DStreamOperator::CreateStreams, input stream info size=%d.", streamInfos.size());
    if (streamInfos.empty()) {
        DHLOGE("DStreamOperator::CreateStreams, input stream info is empty.");
        return CamRetCode::INVALID_ARGUMENT;
    }

    for (auto info : streamInfos) {
        if (halStreamMap_.find(info->streamId_) != halStreamMap_.end()) {
            return CamRetCode::INVALID_ARGUMENT;
        }
        if (info->tunneledMode_ != true) {
            return CamRetCode::METHOD_NOT_SUPPORTED;
        }

        std::shared_ptr<DCameraStream> dcStream = std::make_shared<DCameraStream>();
        if (!dcStream) {
            DHLOGE("Create distributed camera stream failed.");
            return CamRetCode::INSUFFICIENT_RESOURCES;
        }
        DCamRetCode ret = dcStream->InitDCameraStream(info);
        if (ret != SUCCESS) {
            DHLOGE("Init distributed camera stream failed.");
            return CamRetCode::INVALID_ARGUMENT;
        }
        halStreamMap_[info->streamId_] = dcStream;

        std::shared_ptr<DCStreamInfo> dcStreamInfo = std::make_shared<DCStreamInfo>();
        ConvertStreamInfo(info, dcStreamInfo);
        dcStreamInfoMap_[info->streamId_] = dcStreamInfo;

        DHLOGI("Create stream info: id=%d, width=%d, height=%d, format=%d, intent=%d", info->streamId_,
            info->width_, info->height_, info->format_, info->intent_);
    }
    DHLOGI("DStreamOperator::Create distributed camera streams success.");
    return CamRetCode::NO_ERROR;
}

CamRetCode DStreamOperator::ReleaseStreams(const std::vector<int>& streamIds)
{
    DHLOGI("DStreamOperator::ReleaseStreams, input stream id list size=%d.", streamIds.size());
    if (IsCapturing()) {
        DHLOGE("Can not release streams when capture.");
        return CamRetCode::CAMERA_BUSY;
    }

    for (int id : streamIds) {
        auto iter = halStreamMap_.find(id);
        if (iter != halStreamMap_.end()) {
            auto stream = iter->second;
            DCamRetCode ret = stream->ReleaseDCameraBufferQueue();
            if (ret != SUCCESS) {
                DHLOGE("Release distributed camera buffer queue for stream %d failed.", id);
                return MapToExternalRetCode(ret);
            } else {
                DHLOGE("Release distributed camera buffer queue for stream %d successs.", id);
            }
            stream = nullptr;
            halStreamMap_.erase(id);
            dcStreamInfoMap_.erase(id);
        } else {
            DHLOGE("Error streamId %d.", id);
            return CamRetCode::INVALID_ARGUMENT;
        }
    }

    std::shared_ptr<DCameraProvider> provider = DCameraProvider::GetInstance();
    if (provider == nullptr) {
        DHLOGE("Distributed camera provider not init.");
        return CamRetCode::DEVICE_ERROR;
    }
    DCamRetCode ret = provider->ReleaseStreams(dhBase_, streamIds);
    if (ret != SUCCESS) {
        DHLOGE("Release distributed camera streams failed.");
        return MapToExternalRetCode(ret);
    }

    DHLOGI("DStreamOperator::Release distributed camera streams success.");
    return CamRetCode::NO_ERROR;
}

CamRetCode DStreamOperator::CommitStreams(OperationMode mode,
    const std::shared_ptr<CameraStandard::CameraMetadata>& modeSetting)
{
    DHLOGI("DStreamOperator::CommitStreams, input operation mode=%d.", mode);
    if (IsCapturing()) {
        DHLOGE("Can not commit streams when capture.");
        return CamRetCode::CAMERA_BUSY;
    }

    if (currentOperMode_ != mode) {
        currentOperMode_ = mode;
    }
    if (modeSetting == nullptr || modeSetting.get() == nullptr) {
        DHLOGE("Input stream mode setting is invalid.");
    } else {
        latestStreamSetting_ = modeSetting;
    }

    if (dcStreamInfoMap_.size() == 0) {
        DHLOGE("No stream to commit.");
        return CamRetCode::INVALID_ARGUMENT;
    }
    std::vector<std::shared_ptr<DCStreamInfo>> dCameraStreams;
    for (auto streamInfo : dcStreamInfoMap_) {
        dCameraStreams.push_back(streamInfo.second);
    }

    std::shared_ptr<DCameraProvider> provider = DCameraProvider::GetInstance();
    if (provider == nullptr) {
        DHLOGE("Distributed camera provider not init.");
        return CamRetCode::DEVICE_ERROR;
    }
    DCamRetCode ret = provider->ConfigureStreams(dhBase_, dCameraStreams);
    if (ret != DCamRetCode::SUCCESS) {
        DHLOGE("Commit distributed camera streams failed.");
        return MapToExternalRetCode(ret);
    }

    for (size_t i = 0; i < dCameraStreams.size(); i++) {
        auto streamInfo = dCameraStreams[i];
        for (auto halStream : halStreamMap_) {
            if (streamInfo->streamId_ == halStream.first) {
                ret = halStream.second->FinishCommitStream();
                if (ret != DCamRetCode::SUCCESS) {
                    DHLOGE("Stream %d cannot init.", streamInfo->streamId_);
                    return MapToExternalRetCode(ret);
                }
            }
        }
    }
    DHLOGI("DStreamOperator::Commit distributed camera streams success.");
    return CamRetCode::NO_ERROR;
}

CamRetCode DStreamOperator::GetStreamAttributes(std::vector<std::shared_ptr<StreamAttribute>>& attributes)
{
    attributes.clear();
    for (auto stream : halStreamMap_) {
        std::shared_ptr<StreamAttribute> attribute;
        DCamRetCode ret = stream.second->GetDCameraStreamAttribute(attribute);
        if (ret != SUCCESS) {
            DHLOGE("Get distributed camera stream attribute failed.");
            attributes.clear();
            return MapToExternalRetCode(ret);
        }
        attributes.push_back(attribute);
    }
    return CamRetCode::NO_ERROR;
}

CamRetCode DStreamOperator::AttachBufferQueue(int streamId, const OHOS::sptr<OHOS::IBufferProducer>& producer)
{
    if (IsCapturing()) {
        DHLOGE("Can not attach buffer queue when capture.");
        return CamRetCode::CAMERA_BUSY;
    }

    auto iter = halStreamMap_.find(streamId);
    if (iter != halStreamMap_.end()) {
        DCamRetCode ret = iter->second->SetDCameraBufferQueue(producer);
        if (ret != SUCCESS) {
            DHLOGE("Attach distributed camera buffer queue failed.");
        }
        return MapToExternalRetCode(ret);
    } else {
        DHLOGE("Not found stream id %d when attach bubfer queue.", streamId);
        return CamRetCode::INVALID_ARGUMENT;
    }
}

CamRetCode DStreamOperator::DetachBufferQueue(int streamId)
{
    if (IsCapturing()) {
        DHLOGE("Can not detach buffer queue when capture.");
        return CamRetCode::CAMERA_BUSY;
    }

    auto iter = halStreamMap_.find(streamId);
    if (iter != halStreamMap_.end()) {
        DCamRetCode ret = iter->second->ReleaseDCameraBufferQueue();
        if (ret != SUCCESS) {
            DHLOGE("Detach distributed camera buffer queue failed.");
        }
        return MapToExternalRetCode(ret);
    } else {
        DHLOGE("Not found stream id %d when detach bubfer queue.", streamId);
        return CamRetCode::INVALID_ARGUMENT;
    }
}

CamRetCode DStreamOperator::Capture(int captureId, const std::shared_ptr<CaptureInfo>& captureInfo, bool isStreaming)
{
    if (captureId < 0 || halCaptureInfoMap_.find(captureId) != halCaptureInfoMap_.end()) {
        DHLOGE("Input captureId %d is exist.", captureId);
        return CamRetCode::INVALID_ARGUMENT;
    }

    if (!captureInfo) {
        DHLOGE("Input capture info is null.");
        return CamRetCode::INVALID_ARGUMENT;
    }

    for (auto &id : captureInfo->streamIds_) {
        if (halStreamMap_.find(id) == halStreamMap_.end()) {
            DHLOGE("Invalid stream id %d", id);
            return CamRetCode::INVALID_ARGUMENT;
        }
        auto iter = halStreamMap_.find(id);
        if (!iter->second->HasBufferQueue()) {
            DHLOGE("Stream %d has not bufferQueue.", iter->first);
            return CamRetCode::INVALID_ARGUMENT;
        }
        enableShutterCbkMap_[id] = captureInfo->enableShutterCallback_;
        DHLOGI("DStreamOperator::Capture info: captureId=%d, streamId=%d, isStreaming=%d", captureId, id, isStreaming);
    }

    DCamRetCode ret = NegotiateSuitableCaptureInfo(captureInfo, isStreaming);
    if (ret != SUCCESS) {
        DHLOGE("Negotiate suitable capture info failed.");
        return MapToExternalRetCode(ret);
    }

    std::shared_ptr<DCameraProvider> provider = DCameraProvider::GetInstance();
    if (provider == nullptr) {
        DHLOGE("Distributed camera provider not init.");
        return CamRetCode::DEVICE_ERROR;
    }
    ret = provider->StartCapture(dhBase_, cachedDCaptureInfoList_);
    if (ret != SUCCESS) {
        DHLOGE("Start distributed camera capture failed.");
        return MapToExternalRetCode(ret);
    }
    halCaptureInfoMap_[captureId] = captureInfo;

    if (dcStreamOperatorCallback_) {
        dcStreamOperatorCallback_->OnCaptureStarted(captureId, captureInfo->streamIds_);
    }
    SetCapturing(true);
    DHLOGI("DStreamOperator::Capture, start distributed camera capture success.");

    return CamRetCode::NO_ERROR;
}

CamRetCode DStreamOperator::CancelCapture(int captureId)
{
    DHLOGI("DStreamOperator::CancelCapture, cancel distributed camera capture, captureId=%d.", captureId);

    std::unique_lock<std::mutex> lock(requestLock_);
    if (captureId < 0 || halCaptureInfoMap_.find(captureId) == halCaptureInfoMap_.end()) {
        DHLOGE("Input captureId %d is exist.", captureId);
        return CamRetCode::INVALID_ARGUMENT;
    }

    SetCapturing(false);
    std::shared_ptr<DCameraProvider> provider = DCameraProvider::GetInstance();
    if (provider == nullptr) {
        DHLOGE("Distributed camera provider not init.");
        return CamRetCode::DEVICE_ERROR;
    }
    DCamRetCode ret = provider->StopCapture(dhBase_);
    if (ret != SUCCESS) {
        DHLOGE("Cancel distributed camera capture failed.");
        return MapToExternalRetCode(ret);
    }

    std::vector<std::shared_ptr<CaptureEndedInfo>> info;
    for (auto id : halCaptureInfoMap_[captureId]->streamIds_) {
        auto iter = halStreamMap_.find(id);
        if (iter != halStreamMap_.end()) {
            iter->second->FlushDCameraBuffer();
        }
        std::shared_ptr<CaptureEndedInfo> tmp = std::make_shared<CaptureEndedInfo>();
        tmp->frameCount_ = acceptedBufferNum_[std::make_pair(captureId, id)];
        tmp->streamId_ = id;
        info.push_back(tmp);
        acceptedBufferNum_.erase(std::make_pair(captureId, id));
    }
    if (dcStreamOperatorCallback_) {
        dcStreamOperatorCallback_->OnCaptureEnded(captureId, info);
    }
    cachedDCaptureInfoList_.clear();
    halCaptureInfoMap_.erase(captureId);

    return CamRetCode::NO_ERROR;
}

CamRetCode DStreamOperator::ChangeToOfflineStream(const std::vector<int>& streamIds,
    OHOS::sptr<IStreamOperatorCallback>& callback, OHOS::sptr<IOfflineStreamOperator>& offlineOperator)
{
    (void)streamIds;
    (void)callback;
    offlineOperator = nullptr;
    return CamRetCode::METHOD_NOT_SUPPORTED;
}

DCamRetCode DStreamOperator::InitOutputConfigurations(const std::shared_ptr<DHBase> &dhBase,
    const std::string &abilityInfo)
{
    dhBase_ = dhBase;

    JSONCPP_STRING errs;
    Json::CharReaderBuilder readerBuilder;
    Json::Value rootValue;

    std::unique_ptr<Json::CharReader> const jsonReader(readerBuilder.newCharReader());
    if (!jsonReader->parse(abilityInfo.c_str(), abilityInfo.c_str() + abilityInfo.length(), &rootValue, &errs) ||
        !rootValue.isObject()) {
        DHLOGE("Input ablity info is not json object.");
        return INVALID_ARGUMENT;
    }

    if (rootValue["CodecType"].isArray()) {
        uint32_t size = rootValue["CodecType"].size();
        for (uint32_t i = 0; i < size; i++) {
            std::string codeType = (rootValue["CodecType"][i]).asString();
            dcSupportedCodecType_.push_back(ConvertDCEncodeType(codeType));
        }
    }

    std::set<int> allFormats;
    if (rootValue["OutputFormat"]["Preview"].isArray() && (rootValue["OutputFormat"]["Preview"].size() > 0)) {
        std::vector<int> previewFormats;
        uint32_t size = rootValue["OutputFormat"]["Preview"].size();
        for (uint32_t i = 0; i < size; i++) {
            previewFormats.push_back(rootValue["OutputFormat"]["Preview"][i].asInt());
            allFormats.insert((rootValue["OutputFormat"]["Preview"][i]).asInt());
        }
        dcSupportedFormatMap_[DCSceneType::PREVIEW] = previewFormats;
    }

    if (rootValue["OutputFormat"]["Video"].isArray() && (rootValue["OutputFormat"]["Video"].size() > 0)) {
        std::vector<int> videoFormats;
        uint32_t size = rootValue["OutputFormat"]["Video"].size();
        for (uint32_t i = 0; i < size; i++) {
            videoFormats.push_back(rootValue["OutputFormat"]["Video"][i].asInt());
            allFormats.insert((rootValue["OutputFormat"]["Video"][i]).asInt());
        }
        dcSupportedFormatMap_[DCSceneType::VIDEO] = videoFormats;
    }

    std::vector<int> photoFormats;
    if (rootValue["OutputFormat"]["Photo"].isArray() && (rootValue["OutputFormat"]["Photo"].size() > 0)) {
        uint32_t size = rootValue["OutputFormat"]["Photo"].size();
        for (uint32_t i = 0; i < size; i++) {
            photoFormats.push_back(rootValue["OutputFormat"]["Photo"][i].asInt());
            allFormats.insert((rootValue["OutputFormat"]["Photo"][i]).asInt());
        }
        dcSupportedFormatMap_[DCSceneType::PHOTO] = photoFormats;
    }

    for (const auto &format : allFormats) {
        bool isPhotoFormat = count(photoFormats.begin(), photoFormats.end(), format);
        std::string formatStr = std::to_string(format);
        if (rootValue["Resolution"][formatStr].isArray() && rootValue["Resolution"][formatStr].size() > 0) {
            std::vector<DCResolution> resolutionVec;
            uint32_t size = rootValue["Resolution"][formatStr].size();
            for (uint32_t i = 0; i < size; i++) {
                std::string resoStr = rootValue["Resolution"][formatStr][i].asString();
                std::vector<std::string> reso;
                SplitString(resoStr, reso, STAR_SEPARATOR);
                if (reso.size() != SIZE_FMT_LEN) {
                    continue;
                }
                uint32_t width = static_cast<uint32_t>(std::atoi(reso[0].c_str()));
                uint32_t height = static_cast<uint32_t>(std::atoi(reso[1].c_str()));
                if (height == 0 || width == 0 ||
                    (isPhotoFormat && (width > MAX_SUPPORT_PHOTO_WIDTH || height > MAX_SUPPORT_PHOTO_HEIGHT)) ||
                    (!isPhotoFormat &&
                    (width > MAX_SUPPORT_PREVIEW_WIDTH || height > MAX_SUPPORT_PREVIEW_HEIGHT))) {
                    continue;
                }
                DCResolution resolution(width, height);
                resolutionVec.push_back(resolution);
            }
            if (!resolutionVec.empty()) {
                std::sort(resolutionVec.begin(), resolutionVec.end());
                dcSupportedResolutionMap_[format] = resolutionVec;
            }
        }
    }

    if (dcSupportedCodecType_.empty() || dcSupportedFormatMap_.empty() || dcSupportedResolutionMap_.empty()) {
        DHLOGE("Input ablity info is invalid.");
        return DEVICE_NOT_INIT;
    }
    return SUCCESS;
}

DCamRetCode DStreamOperator::AcquireBuffer(int streamId, std::shared_ptr<DCameraBuffer> &buffer)
{
    std::unique_lock<std::mutex> lock(requestLock_);
    if (!IsCapturing()) {
        DHLOGE("Not in capturing state, can not acquire buffer.");
        return DCamRetCode::CAMERA_OFFLINE;
    }

    auto iter = halStreamMap_.find(streamId);
    if (iter == halStreamMap_.end()) {
        DHLOGE("streamId %d is invalid, can not acquire buffer.", streamId);
        return DCamRetCode::INVALID_ARGUMENT;
    }

    DCamRetCode ret = iter->second->GetDCameraBuffer(buffer);
    if (ret == DCamRetCode::EXCEED_MAX_NUMBER) {
        DHLOGE("Buffer list is full, cannot get new idle buffer.");
    } else if (ret == DCamRetCode::INVALID_ARGUMENT) {
        DHLOGE("Get distributed camera buffer failed, invalid buffer parameter.");
    }
    return ret;
}

DCamRetCode DStreamOperator::ShutterBuffer(int streamId, const std::shared_ptr<DCameraBuffer> &buffer)
{
    DHLOGI("DStreamOperator::ShutterBuffer begin shutter buffer for streamId = %d", streamId);

    int32_t captureId = -1;
    for (auto iter = halCaptureInfoMap_.begin(); iter != halCaptureInfoMap_.end(); iter++) {
        std::shared_ptr<CaptureInfo> captureInfo = iter->second;
        std::vector<int> streamIds = captureInfo->streamIds_;
        if (std::find(streamIds.begin(), streamIds.end(), streamId) != streamIds.end()) {
            captureId = iter->first;
            break;
        }
    }
    if (captureId == -1) {
        DHLOGE("ShutterBuffer falied, invalid streamId = %d", streamId);
        return DCamRetCode::INVALID_ARGUMENT;
    }

    auto iter = halStreamMap_.find(streamId);
    if (iter != halStreamMap_.end()) {
        DCamRetCode ret = iter->second->ReturnDCameraBuffer(buffer);
        if (ret != DCamRetCode::SUCCESS) {
            DHLOGE("Flush distributed camera buffer failed.");
            return ret;
        }
        acceptedBufferNum_[std::make_pair(captureId, streamId)]++;

        SnapShotStreamOnCaptureEnded(captureId, streamId);
    }

    uint64_t resultTimestamp = GetCurrentLocalTimeStamp();
    bool needReturn = false;
    std::shared_ptr<CameraStandard::CameraMetadata> result = nullptr;
    DCamRetCode ret = dMetadataProcessor_->UpdateResultMetadata(needReturn, result);
    if (ret != DCamRetCode::SUCCESS) {
        DHLOGE("Cannot handle result metadata.");
        return ret;
    }
    if (needReturn) {
        resultCallback_(resultTimestamp, result);
    }

    auto anIter = enableShutterCbkMap_.find(streamId);
    if (anIter->second) {
        if (dcStreamOperatorCallback_ == nullptr) {
            DHLOGE("DStreamOperator::ShutterBuffer failed, need shutter frame, but stream operator callback is null.");
            return DCamRetCode::FAILED;
        }
        std::vector<int32_t> streamIds;
        streamIds.push_back(anIter->first);
        dcStreamOperatorCallback_->OnFrameShutter(captureId, streamIds, resultTimestamp);
    }
    return DCamRetCode::SUCCESS;
}

DCamRetCode DStreamOperator::SetCallBack(OHOS::sptr<IStreamOperatorCallback> const &callback)
{
    dcStreamOperatorCallback_ = callback;
    return SUCCESS;
}

DCamRetCode DStreamOperator::SetDeviceCallback(
    std::function<void(ErrorType, int)> &errorCbk,
    std::function<void(uint64_t, std::shared_ptr<CameraStandard::CameraMetadata>)> &resultCbk)
{
    errorCallback_ = errorCbk;
    resultCallback_ = resultCbk;
    return SUCCESS;
}

void DStreamOperator::SnapShotStreamOnCaptureEnded(int32_t captureId, int streamId)
{
    auto dcStreamInfo = dcStreamInfoMap_.find(streamId);
    if (dcStreamInfo == dcStreamInfoMap_.end()) {
        return;
    }
    if (dcStreamInfo->second->type_ != DCStreamType::SNAPSHOT_FRAME) {
        return;
    }
    if (dcStreamOperatorCallback_ == nullptr) {
        return;
    }
    std::vector<std::shared_ptr<CaptureEndedInfo>> info;
    std::shared_ptr<CaptureEndedInfo> tmp = std::make_shared<CaptureEndedInfo>();
    tmp->frameCount_ = acceptedBufferNum_[std::make_pair(captureId, streamId)];
    tmp->streamId_ = streamId;
    info.push_back(tmp);
    dcStreamOperatorCallback_->OnCaptureEnded(captureId, info);
    DHLOGD("snapshot stream successfully reported captureId = %d streamId = %d.", captureId, streamId);
}

void DStreamOperator::Release()
{
    DHLOGI("DStreamOperator::Release, begin release stream operator.");

    std::unique_lock<std::mutex> lock(requestLock_);
    std::vector<int> streamIds;
    for (auto iter : halStreamMap_) {
        streamIds.push_back(iter.first);
    }
    ReleaseStreams(streamIds);
    if (latestStreamSetting_) {
        latestStreamSetting_ = nullptr;
    }
    SetCapturing(false);
    halStreamMap_.clear();
    dcStreamInfoMap_.clear();
    halCaptureInfoMap_.clear();
    enableShutterCbkMap_.clear();
    acceptedBufferNum_.clear();
    cachedDCaptureInfoList_.clear();
    dcStreamOperatorCallback_ = nullptr;
}

bool DStreamOperator::IsCapturing()
{
    std::unique_lock<mutex> lock(isCapturingLock_);
    return isCapturing_;
}

void DStreamOperator::SetCapturing(bool isCapturing)
{
    std::unique_lock<mutex> lock(isCapturingLock_);
    isCapturing_ = isCapturing;
}

void DStreamOperator::ConvertStreamInfo(std::shared_ptr<StreamInfo> &srcInfo, std::shared_ptr<DCStreamInfo> &dstInfo)
{
    dstInfo->streamId_ = srcInfo->streamId_;
    dstInfo->width_ = srcInfo->width_;
    dstInfo->stride_ = srcInfo->width_;
    dstInfo->height_ = srcInfo->height_;
    dstInfo->dataspace_ = srcInfo->datasapce_;
    dstInfo->encodeType_ = (DCEncodeType)srcInfo->encodeType_;

    if ((srcInfo->intent_ == STILL_CAPTURE) || (srcInfo->intent_ == POST_VIEW) ||
        (dstInfo->encodeType_ == ENCODE_TYPE_JPEG)) {
        dstInfo->type_ = DCStreamType::SNAPSHOT_FRAME;
        dstInfo->format_ = OHOS_CAMERA_FORMAT_JPEG;
    } else {
        dstInfo->type_ = DCStreamType::CONTINUOUS_FRAME;
        dstInfo->format_ =
            static_cast<int>(DBufferManager::PixelFormatToDCameraFormat(static_cast<PixelFormat>(srcInfo->format_)));
    }
}

DCamRetCode DStreamOperator::NegotiateSuitableCaptureInfo(const std::shared_ptr<CaptureInfo>& srcCaptureInfo,
    bool isStreaming)
{
    std::vector<std::shared_ptr<DCStreamInfo>> srcStreamInfo;
    for (auto &id : srcCaptureInfo->streamIds_) {
        auto iter = dcStreamInfoMap_.find(id);
        if (iter != dcStreamInfoMap_.end()) {
            srcStreamInfo.push_back(iter->second);
        }
    }
    if (srcStreamInfo.empty()) {
        DHLOGE("Input source stream info vector is empty.");
        return INVALID_ARGUMENT;
    }

    std::shared_ptr<DCCaptureInfo> inputCaptureInfo = BuildSuitableCaptureInfo(srcCaptureInfo, srcStreamInfo);
    inputCaptureInfo->type_ = isStreaming ? DCStreamType::CONTINUOUS_FRAME : DCStreamType::SNAPSHOT_FRAME;
    inputCaptureInfo->isCapture_ = true;

    std::shared_ptr<DCCaptureInfo> appendCaptureInfo = nullptr;
    if (cachedDCaptureInfoList_.empty()) {
        std::vector<std::shared_ptr<DCStreamInfo>> appendStreamInfo;
        auto iter = dcStreamInfoMap_.begin();
        while (iter != dcStreamInfoMap_.end()) {
            if ((isStreaming && (iter->second->type_ == DCStreamType::SNAPSHOT_FRAME)) ||
                (!isStreaming && (iter->second->type_ == DCStreamType::CONTINUOUS_FRAME))) {
                appendStreamInfo.push_back(iter->second);
            }
            iter++;
        }
        if (!appendStreamInfo.empty()) {
            appendCaptureInfo = BuildSuitableCaptureInfo(srcCaptureInfo, appendStreamInfo);
            appendCaptureInfo->type_ = isStreaming ? DCStreamType::SNAPSHOT_FRAME : DCStreamType::CONTINUOUS_FRAME;
            appendCaptureInfo->isCapture_ = false;
        }
    } else {
        for (auto cacheCapture : cachedDCaptureInfoList_) {
            if ((isStreaming && (cacheCapture->type_ == DCStreamType::SNAPSHOT_FRAME)) ||
                (!isStreaming && (cacheCapture->type_ == DCStreamType::CONTINUOUS_FRAME))) {
                cacheCapture->isCapture_ = false;
                appendCaptureInfo = cacheCapture;
                break;
            }
        }
    }
    cachedDCaptureInfoList_.clear();
    cachedDCaptureInfoList_.push_back(inputCaptureInfo);
    if (appendCaptureInfo != nullptr) {
        cachedDCaptureInfoList_.push_back(appendCaptureInfo);
    }
    return SUCCESS;
}

std::shared_ptr<DCCaptureInfo> DStreamOperator::BuildSuitableCaptureInfo(const shared_ptr<CaptureInfo>& srcCaptureInfo,
    std::vector<std::shared_ptr<DCStreamInfo>> &srcStreamInfo)
{
    std::shared_ptr<DCCaptureInfo> captureInfo = std::make_shared<DCCaptureInfo>();

    ChooseSuitableFormat(srcStreamInfo, captureInfo);
    ChooseSuitableResolution(srcStreamInfo, captureInfo);
    ChooseSuitableDataSpace(srcStreamInfo, captureInfo);
    ChooseSuitableEncodeType(srcStreamInfo, captureInfo);

    std::shared_ptr<DCameraSettings> dcSetting = std::make_shared<DCameraSettings>();
    dcSetting->type_ = DCSettingsType::UPDATE_METADATA;
    std::string settingStr = CameraStandard::MetadataUtils::EncodeToString(srcCaptureInfo->captureSetting_);
    dcSetting->value_ = Base64Encode(reinterpret_cast<const unsigned char *>(settingStr.c_str()), settingStr.length());

    captureInfo->captureSettings_.push_back(dcSetting);

    return captureInfo;
}

void DStreamOperator::ChooseSuitableFormat(std::vector<std::shared_ptr<DCStreamInfo>> &streamInfo,
    std::shared_ptr<DCCaptureInfo> &captureInfo)
{
    for (auto stream : streamInfo) {
        if (dcSupportedResolutionMap_.count(stream->format_) > 0) {
            captureInfo->format_ = stream->format_;
            return;
        }
    }
    if ((streamInfo.at(0))->type_ == DCStreamType::CONTINUOUS_FRAME) {
        if (dcSupportedFormatMap_.count(DCSceneType::PREVIEW) > 0) {
            captureInfo->format_ = dcSupportedFormatMap_[DCSceneType::PREVIEW].at(0);
        } else if (dcSupportedFormatMap_.count(DCSceneType::VIDEO) > 0) {
            captureInfo->format_ = dcSupportedFormatMap_[DCSceneType::VIDEO].at(0);
        } else {
            captureInfo->format_ = OHOS_CAMERA_FORMAT_YCRCB_420_SP;
        }
    } else {
        if (dcSupportedFormatMap_.count(DCSceneType::PHOTO) > 0) {
            captureInfo->format_ = dcSupportedFormatMap_[DCSceneType::PHOTO].at(0);
        } else {
            captureInfo->format_ = OHOS_CAMERA_FORMAT_JPEG;
        }
    }
}

void DStreamOperator::ChooseSuitableResolution(std::vector<std::shared_ptr<DCStreamInfo>> &streamInfo,
    std::shared_ptr<DCCaptureInfo> &captureInfo)
{
    std::vector<DCResolution> supportedResolutionList = dcSupportedResolutionMap_[captureInfo->format_];

    DCResolution tempResolution = { 0, 0 };
    for (auto stream : streamInfo) {
        for (auto resolution : supportedResolutionList) {
            if ((resolution.width_ == stream->width_) && (resolution.height_ == stream->height_)) {
                if (tempResolution < resolution) {
                    tempResolution = resolution;
                    break;
                }
            }
        }
        captureInfo->streamIds_.push_back(stream->streamId_);
    };

    if ((tempResolution.width_ == 0) || (tempResolution.height_ == 0)) {
        captureInfo->width_ = MAX_SUPPORT_PREVIEW_WIDTH;
        captureInfo->height_ = MAX_SUPPORT_PREVIEW_HEIGHT;
    } else {
        captureInfo->width_ = tempResolution.width_;
        captureInfo->height_ = tempResolution.height_;
    }
}

void DStreamOperator::ChooseSuitableDataSpace(std::vector<std::shared_ptr<DCStreamInfo>> &streamInfo,
    std::shared_ptr<DCCaptureInfo> &captureInfo)
{
    captureInfo->dataspace_ = (streamInfo.at(0))->dataspace_;
}

void DStreamOperator::ChooseSuitableEncodeType(std::vector<std::shared_ptr<DCStreamInfo>> &streamInfo,
    std::shared_ptr<DCCaptureInfo> &captureInfo)
{
    if ((streamInfo.at(0))->type_ == DCStreamType::CONTINUOUS_FRAME) {
        if (count(dcSupportedCodecType_.begin(), dcSupportedCodecType_.end(), DCEncodeType::ENCODE_TYPE_H265)) {
            captureInfo->encodeType_ = DCEncodeType::ENCODE_TYPE_H265;
        } else if (count(dcSupportedCodecType_.begin(), dcSupportedCodecType_.end(), DCEncodeType::ENCODE_TYPE_H264)) {
            captureInfo->encodeType_ = DCEncodeType::ENCODE_TYPE_H264;
        } else {
            captureInfo->encodeType_ = DCEncodeType::ENCODE_TYPE_NULL;
        }
    } else {
        captureInfo->encodeType_ = DCEncodeType::ENCODE_TYPE_JPEG;
    }
}

DCEncodeType DStreamOperator::ConvertDCEncodeType(std::string &srcEncodeType)
{
    if (srcEncodeType == ENCODE_TYPE_STR_H264) {
        return DCEncodeType::ENCODE_TYPE_H264;
    } else if (srcEncodeType == ENCODE_TYPE_STR_H265) {
        return DCEncodeType::ENCODE_TYPE_H265;
    } else if (srcEncodeType == ENCODE_TYPE_STR_JPEG) {
        return DCEncodeType::ENCODE_TYPE_JPEG;
    } else {
        return DCEncodeType::ENCODE_TYPE_NULL;
    }
}
} // namespace DistributedHardware
} // namespace OHOS
