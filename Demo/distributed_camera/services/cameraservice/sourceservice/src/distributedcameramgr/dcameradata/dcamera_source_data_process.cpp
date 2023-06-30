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

#include "dcamera_source_data_process.h"

#include "anonymous_string.h"
#include "distributed_camera_constants.h"
#include "distributed_camera_errno.h"
#include "distributed_hardware_log.h"

namespace OHOS {
namespace DistributedHardware {
DCameraSourceDataProcess::DCameraSourceDataProcess(std::string devId, std::string dhId, DCStreamType streamType)
    : devId_(devId), dhId_(dhId), streamType_(streamType)
{
    DHLOGI("DCameraSourceDataProcess Constructor devId %s dhId %s streamType %d", GetAnonyString(devId_).c_str(),
        GetAnonyString(dhId_).c_str(), streamType_);
}

DCameraSourceDataProcess::~DCameraSourceDataProcess()
{
    DHLOGI("DCameraSourceDataProcess Destructor devId %s dhId %s streamType %d", GetAnonyString(devId_).c_str(),
        GetAnonyString(dhId_).c_str(), streamType_);
    streamProcess_.clear();
    streamIds_.clear();
}

int32_t DCameraSourceDataProcess::FeedStream(std::vector<std::shared_ptr<DataBuffer>>& buffers)
{
    if (buffers.size() > DCAMERA_MAX_NUM) {
        DHLOGI("DCameraSourceDataProcess FeedStream devId %s dhId %s size: %d over flow",
            GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str(), buffers.size());
        return DCAMERA_BAD_VALUE;
    }

    auto buffer = *(buffers.begin());
    DHLOGD("DCameraSourceDataProcess FeedStream devId %s dhId %s streamType %d streamSize: %d",
        GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str(), streamType_, buffer->Size());
    for (auto iter = streamProcess_.begin(); iter != streamProcess_.end(); iter++) {
        (*iter)->FeedStream(buffer);
    }
    return DCAMERA_OK;
}

int32_t DCameraSourceDataProcess::ConfigStreams(std::vector<std::shared_ptr<DCStreamInfo>>& streamInfos)
{
    DHLOGI("DCameraSourceDataProcess ConfigStreams devId %s dhId %s streamType %d size %d",
        GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str(), streamType_, streamInfos.size());
    if (streamInfos.empty()) {
        DHLOGI("DCameraSourceDataProcess ConfigStreams is empty");
        return DCAMERA_OK;
    }
    std::map<DCameraStreamConfig, std::set<int>> streamConfigs;
    for (auto iter = streamInfos.begin(); iter != streamInfos.end(); iter++) {
        std::shared_ptr<DCStreamInfo> streamInfo = *iter;
        DCameraStreamConfig streamConfig(streamInfo->width_, streamInfo->height_, streamInfo->format_,
            streamInfo->dataspace_, streamInfo->encodeType_, streamInfo->type_);
        DHLOGI("DCameraSourceDataProcess ConfigStreams devId %s dhId %s, streamId: %d info: width: %d, height: %d," +
            "format: %d, dataspace: %d, encodeType: %d streamType: %d", GetAnonyString(devId_).c_str(),
            GetAnonyString(dhId_).c_str(), streamInfo->streamId_, streamConfig.width_, streamConfig.height_,
            streamConfig.format_, streamConfig.dataspace_, streamConfig.encodeType_, streamConfig.type_);
        if (streamConfigs.find(streamConfig) == streamConfigs.end()) {
            std::set<int> streamIdSet;
            streamConfigs.emplace(streamConfig, streamIdSet);
        }
        streamConfigs[streamConfig].insert(streamInfo->streamId_);
        streamIds_.insert(streamInfo->streamId_);
    }

    for (auto iter = streamConfigs.begin(); iter != streamConfigs.end(); iter++) {
        DHLOGI("DCameraSourceDataProcess ConfigStreams devId %s dhId %s, info: width: %d, height: %d, format: %d," +
            "dataspace: %d, encodeType: %d streamType: %d", GetAnonyString(devId_).c_str(),
            GetAnonyString(dhId_).c_str(), iter->first.width_, iter->first.height_, iter->first.format_,
            iter->first.dataspace_, iter->first.encodeType_, iter->first.type_);

        std::shared_ptr<DCameraStreamDataProcess> streamProcess =
            std::make_shared<DCameraStreamDataProcess>(devId_, dhId_, streamType_);
        std::shared_ptr<DCameraStreamConfig> streamConfig =
            std::make_shared<DCameraStreamConfig>(iter->first.width_, iter->first.height_, iter->first.format_,
            iter->first.dataspace_, iter->first.encodeType_, iter->first.type_);
        streamProcess->ConfigStreams(streamConfig, iter->second);

        streamProcess_.push_back(streamProcess);
    }

    return DCAMERA_OK;
}

int32_t DCameraSourceDataProcess::ReleaseStreams(std::vector<int32_t>& streamIds)
{
    DHLOGI("DCameraSourceDataProcess ReleaseStreams devId %s dhId %s streamType: %d", GetAnonyString(devId_).c_str(),
        GetAnonyString(dhId_).c_str(), streamType_);
    std::set<int32_t> streamIdSet(streamIds.begin(), streamIds.end());
    auto iter = streamProcess_.begin();
    while (iter != streamProcess_.end()) {
        (*iter)->ReleaseStreams(streamIdSet);
        std::set<int32_t> processStreamIds;
        (*iter)->GetAllStreamIds(processStreamIds);
        if (processStreamIds.empty()) {
            iter = streamProcess_.erase(iter);
        } else {
            iter++;
        }
    }

    std::string strStreams;
    for (auto iterSet = streamIdSet.begin(); iterSet != streamIdSet.end(); iterSet++) {
        strStreams += (std::to_string(*iterSet) + std::string(" "));
        streamIds_.erase(*iterSet);
    }
    DHLOGI("DCameraSourceDataProcess ReleaseStreams devId %s dhId %s streamType: %d streamProcessSize: %d streams: %s",
        GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str(), streamType_, streamProcess_.size(),
        strStreams.c_str());
    return DCAMERA_OK;
}

int32_t DCameraSourceDataProcess::StartCapture(std::shared_ptr<DCCaptureInfo>& captureInfo)
{
    DHLOGI("DCameraSourceDataProcess StartCapture devId %s dhId %s, info: width: %d, height: %d, format: %d,\
        dataspace: %d, encodeType: %d streamType: %d", GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str(),
        captureInfo->width_, captureInfo->height_, captureInfo->format_, captureInfo->dataspace_,
        captureInfo->encodeType_, captureInfo->type_);

    std::shared_ptr<DCameraStreamConfig> streamConfig =
        std::make_shared<DCameraStreamConfig>(captureInfo->width_, captureInfo->height_, captureInfo->format_,
        captureInfo->dataspace_, captureInfo->encodeType_, captureInfo->type_);
    std::set<int32_t> streamIds(captureInfo->streamIds_.begin(), captureInfo->streamIds_.end());
    for (auto iterSet = streamIds.begin(); iterSet != streamIds.end(); iterSet++) {
        DHLOGI("DCameraSourceDataProcess StartCapture devId %s dhId %s StartCapture id: %d",
            GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str(), *iterSet);
    }
    for (auto iter = streamProcess_.begin(); iter != streamProcess_.end(); iter++) {
        (*iter)->StartCapture(streamConfig, streamIds);
    }
    return DCAMERA_OK;
}

int32_t DCameraSourceDataProcess::StopCapture()
{
    DHLOGI("DCameraSourceDataProcess StopCapture devId %s dhId %s streamType: %d", GetAnonyString(devId_).c_str(),
        GetAnonyString(dhId_).c_str(), streamType_);
    for (auto iter = streamProcess_.begin(); iter != streamProcess_.end(); iter++) {
        (*iter)->StopCapture();
    }
    return DCAMERA_OK;
}

void DCameraSourceDataProcess::GetAllStreamIds(std::vector<int32_t>& streamIds)
{
    streamIds.assign(streamIds_.begin(), streamIds_.end());
}
} // namespace DistributedHardware
} // namespace OHOS
