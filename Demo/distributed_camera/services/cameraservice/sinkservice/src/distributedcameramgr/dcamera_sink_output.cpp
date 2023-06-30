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

#include "dcamera_sink_output.h"

#include "anonymous_string.h"
#include "dcamera_channel_sink_impl.h"
#include "dcamera_client.h"
#include "dcamera_sink_data_process.h"
#include "dcamera_sink_output_channel_listener.h"
#include "dcamera_sink_output_result_callback.h"
#include "distributed_camera_constants.h"
#include "distributed_camera_errno.h"
#include "distributed_hardware_log.h"

namespace OHOS {
namespace DistributedHardware {
DCameraSinkOutput::DCameraSinkOutput(const std::string& dhId, std::shared_ptr<ICameraOperator>& cameraOperator)
    : dhId_(dhId), operator_(cameraOperator)
{
    DHLOGI("DCameraSinkOutput Constructor dhId: %s", GetAnonyString(dhId_).c_str());
    isInit_ = false;
}

DCameraSinkOutput::~DCameraSinkOutput()
{
    if (isInit_) {
        UnInit();
    }
}

int32_t DCameraSinkOutput::Init()
{
    DHLOGI("DCameraSinkOutput::Init dhId: %s", GetAnonyString(dhId_).c_str());
    auto output = std::shared_ptr<DCameraSinkOutput>(shared_from_this());
    std::shared_ptr<ResultCallback> resultCallback = std::make_shared<DCameraSinkOutputResultCallback>(output);
    operator_->SetResultCallback(resultCallback);

    InitInner(CONTINUOUS_FRAME);
    InitInner(SNAPSHOT_FRAME);
    isInit_ = true;
    DHLOGI("DCameraSinkOutput::Init %s success", GetAnonyString(dhId_).c_str());
    return DCAMERA_OK;
}

void DCameraSinkOutput::InitInner(DCStreamType type)
{
    std::shared_ptr<ICameraChannel> channel = std::make_shared<DCameraChannelSinkImpl>();
    std::shared_ptr<ICameraSinkDataProcess> dataProcess = std::make_shared<DCameraSinkDataProcess>(dhId_, channel);
    dataProcesses_.emplace(type, dataProcess);
    channels_.emplace(type, channel);
    sessionState_.emplace(type, DCAMERA_CHANNEL_STATE_DISCONNECTED);
}

int32_t DCameraSinkOutput::UnInit()
{
    DHLOGI("DCameraSinkOutput::UnInit dhId: %s", GetAnonyString(dhId_).c_str());
    channels_.clear();
    dataProcesses_.clear();
    sessionState_.clear();
    isInit_ = false;
    DHLOGI("DCameraSinkOutput::UnInit %s success", GetAnonyString(dhId_).c_str());
    return DCAMERA_OK;
}

int32_t DCameraSinkOutput::OpenChannel(std::shared_ptr<DCameraChannelInfo>& info)
{
    DHLOGI("DCameraSinkOutput::OpenChannel dhId: %s", GetAnonyString(dhId_).c_str());
    std::map<DCStreamType, DCameraSessionMode> modeMaps;
    modeMaps.emplace(CONTINUOUS_FRAME, DCAMERA_SESSION_MODE_VIDEO);
    modeMaps.emplace(SNAPSHOT_FRAME, DCAMERA_SESSION_MODE_JPEG);
    std::vector<DCameraIndex> indexs;
    indexs.push_back(DCameraIndex(info->sourceDevId_, dhId_));
    for (auto iter = info->detail_.begin(); iter != info->detail_.end(); iter++) {
        if (sessionState_[iter->streamType_] != DCAMERA_CHANNEL_STATE_DISCONNECTED) {
            DHLOGE("DCameraSinkOutput::OpenChannel wrong state, sessionState: %d", sessionState_[iter->streamType_]);
            return DCAMERA_WRONG_STATE;
        }
        auto iterCh = channels_.find(iter->streamType_);
        if (iterCh == channels_.end()) {
            continue;
        }
        auto output = std::shared_ptr<DCameraSinkOutput>(shared_from_this());
        std::shared_ptr<ICameraChannelListener> channelListener =
            std::make_shared<DCameraSinkOutputChannelListener>(iter->streamType_, output);
        int32_t ret = iterCh->second->CreateSession(indexs, iter->dataSessionFlag_, modeMaps[iter->streamType_],
            channelListener);
        if (ret != DCAMERA_OK) {
            DHLOGE("DCameraSinkOutput::Init channel create session failed, dhId: %s, ret: %d",
                   GetAnonyString(dhId_).c_str(), ret);
            return ret;
        }
    }
    return DCAMERA_OK;
}

int32_t DCameraSinkOutput::CloseChannel()
{
    DHLOGI("DCameraSinkOutput::CloseChannel dhId: %s", GetAnonyString(dhId_).c_str());
    int32_t ret = DCAMERA_OK;
    auto iterCon = channels_.find(CONTINUOUS_FRAME);
    if (iterCon != channels_.end()) {
        ret = iterCon->second->ReleaseSession();
        if (ret != DCAMERA_OK) {
            DHLOGI("DCameraSinkOutput UnInit release continue session failed, dhId: %s, ret: %d",
                GetAnonyString(dhId_).c_str(), ret);
        }
        sessionState_[CONTINUOUS_FRAME] = DCAMERA_CHANNEL_STATE_DISCONNECTED;
    }

    auto iterSnap = channels_.find(SNAPSHOT_FRAME);
    if (iterSnap != channels_.end()) {
        ret = iterSnap->second->ReleaseSession();
        if (ret != DCAMERA_OK) {
            DHLOGI("DCameraSinkOutput UnInit release snapshot session failed, dhId: %s, ret: %d",
                GetAnonyString(dhId_).c_str(), ret);
        }
        sessionState_[SNAPSHOT_FRAME] = DCAMERA_CHANNEL_STATE_DISCONNECTED;
    }
    return DCAMERA_OK;
}

int32_t DCameraSinkOutput::StartCapture(std::vector<std::shared_ptr<DCameraCaptureInfo>>& captureInfos)
{
    DHLOGI("DCameraSinkOutput::StartCapture dhId: %s", GetAnonyString(dhId_).c_str());
    for (auto& info : captureInfos) {
        if (dataProcesses_.find(info->streamType_) == dataProcesses_.end()) {
            DHLOGE("DCameraSinkOutput::StartCapture has no data process, streamType: %d", info->streamType_);
            break;
        }
        int32_t ret = dataProcesses_[info->streamType_]->StartCapture(info);
        if (ret != DCAMERA_OK) {
            DHLOGE("DCameraSinkOutput::StartCapture failed, dhId: %s, ret: %d", GetAnonyString(dhId_).c_str(), ret);
            return ret;
        }
    }
    return DCAMERA_OK;
}

int32_t DCameraSinkOutput::StopCapture()
{
    DHLOGI("DCameraSinkOutput::StopCapture dhId: %s", GetAnonyString(dhId_).c_str());
    int32_t ret = dataProcesses_[CONTINUOUS_FRAME]->StopCapture();
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSinkOutput::StopCapture continuous data process stop capture failed, dhId: %s, ret: %d",
               GetAnonyString(dhId_).c_str(), ret);
        return ret;
    }

    ret = dataProcesses_[SNAPSHOT_FRAME]->StopCapture();
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSinkOutput::StopCapture snapshot data process stop capture failed, dhId: %s, ret: %d",
               GetAnonyString(dhId_).c_str(), ret);
        return ret;
    }

    DHLOGI("DCameraSinkOutput::StopCapture %s success", GetAnonyString(dhId_).c_str());
    return DCAMERA_OK;
}

void DCameraSinkOutput::OnVideoResult(std::shared_ptr<DataBuffer>& buffer)
{
    if (sessionState_[CONTINUOUS_FRAME] != DCAMERA_CHANNEL_STATE_CONNECTED) {
        DHLOGE("DCameraSinkOutput::OnVideoResult dhId: %s, channel state: %d",
               GetAnonyString(dhId_).c_str(), sessionState_[CONTINUOUS_FRAME]);
        return;
    }
    if (dataProcesses_.find(CONTINUOUS_FRAME) == dataProcesses_.end()) {
        DHLOGE("DCameraSinkOutput::OnVideoResult %s has no continuous data process", GetAnonyString(dhId_).c_str());
        return;
    }
    dataProcesses_[CONTINUOUS_FRAME]->FeedStream(buffer);
}

void DCameraSinkOutput::OnPhotoResult(std::shared_ptr<DataBuffer>& buffer)
{
    if (dataProcesses_.find(SNAPSHOT_FRAME) == dataProcesses_.end()) {
        DHLOGE("DCameraSinkOutput::OnPhotoResult %s has no snapshot data process", GetAnonyString(dhId_).c_str());
        return;
    }
    dataProcesses_[SNAPSHOT_FRAME]->FeedStream(buffer);
}

void DCameraSinkOutput::OnSessionState(DCStreamType type, int32_t state)
{
    DHLOGI("DCameraSinkOutput::OnSessionState dhId: %s, stream type: %d, state: %d",
           GetAnonyString(dhId_).c_str(), type, state);
    sessionState_[type] = state;
    switch (state) {
        case DCAMERA_CHANNEL_STATE_CONNECTING: {
            DHLOGI("DCameraSinkOutput::OnSessionState channel is connecting, dhId: %s, stream type: %d",
                   GetAnonyString(dhId_).c_str(), type);
            break;
        }
        case DCAMERA_CHANNEL_STATE_CONNECTED: {
            DHLOGI("DCameraSinkOutput::OnSessionState channel is connected, dhId: %s, stream type: %d",
                   GetAnonyString(dhId_).c_str(), type);
            break;
        }
        case DCAMERA_CHANNEL_STATE_DISCONNECTED: {
            DHLOGI("DCameraSinkOutput::OnSessionState channel is disconnected, dhId: %s, stream type: %d",
                   GetAnonyString(dhId_).c_str(), type);
            break;
        }
        default: {
            DHLOGE("DCameraSinkOutput::OnSessionState %s unknown session state", GetAnonyString(dhId_).c_str());
            break;
        }
    }
}

void DCameraSinkOutput::OnSessionError(DCStreamType type, int32_t eventType, int32_t eventReason, std::string detail)
{
    DHLOGI("DCameraSinkOutput::OnSessionError dhId: %s, stream type: %d, eventType: %d, eventReason: %d, detail: %s",
           GetAnonyString(dhId_).c_str(), type, eventType, eventReason, detail.c_str());
}

void DCameraSinkOutput::OnDataReceived(DCStreamType type, std::vector<std::shared_ptr<DataBuffer>>& dataBuffers)
{
}
} // namespace DistributedHardware
} // namespace OHOS