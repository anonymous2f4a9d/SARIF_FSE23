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

#include "dcamera_source_controller.h"

#include <securec.h>
#include "json/json.h"

#include "dcamera_capture_info_cmd.h"
#include "dcamera_channel_source_impl.h"
#include "dcamera_metadata_setting_cmd.h"
#include "dcamera_protocol.h"
#include "dcamera_source_controller_channel_listener.h"
#include "dcamera_source_service_ipc.h"
#include "dcamera_utils_tools.h"

#include "distributed_camera_constants.h"
#include "distributed_camera_errno.h"
#include "distributed_hardware_log.h"
#include "idistributed_camera_sink.h"

namespace OHOS {
namespace DistributedHardware {
DCameraSourceController::DCameraSourceController(std::string devId, std::string dhId,
    std::shared_ptr<DCameraSourceStateMachine>& stateMachine, std::shared_ptr<EventBus>& eventBus)
    : devId_(devId), dhId_(dhId), stateMachine_(stateMachine), eventBus_(eventBus),
    channelState_(DCAMERA_CHANNEL_STATE_DISCONNECTED)
{
    DHLOGI("DCameraSourceController create devId: %s dhId: %s", GetAnonyString(devId_).c_str(),
        GetAnonyString(dhId_).c_str());
    isInit = false;
}

DCameraSourceController::~DCameraSourceController()
{
    DHLOGI("DCameraSourceController delete devId: %s dhId: %s", GetAnonyString(devId_).c_str(),
        GetAnonyString(dhId_).c_str());
    if (isInit) {
        UnInit();
    }
}

int32_t DCameraSourceController::StartCapture(std::vector<std::shared_ptr<DCameraCaptureInfo>>& captureInfos)
{
    if (indexs_.size() > DCAMERA_MAX_NUM) {
        DHLOGE("DCameraSourceController StartCapture not support operate %d camera", indexs_.size());
        return DCAMERA_BAD_OPERATE;
    }

    std::string dhId = indexs_.begin()->dhId_;
    std::string devId = indexs_.begin()->devId_;
    DHLOGI("DCameraSourceController StartCapture devId: %s, dhId: %s", GetAnonyString(devId).c_str(),
        GetAnonyString(dhId).c_str());
    DCameraCaptureInfoCmd cmd;
    cmd.type_ = DCAMERA_PROTOCOL_TYPE_OPERATION;
    cmd.dhId_ = dhId;
    cmd.command_ = DCAMERA_PROTOCOL_CMD_CAPTURE;
    cmd.value_.assign(captureInfos.begin(), captureInfos.end());
    std::string jsonStr;
    int32_t ret = cmd.Marshal(jsonStr);
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSourceController StartCapture Marshal faied %d, devId: %s, dhId: %s", ret,
            GetAnonyString(devId).c_str(), GetAnonyString(dhId).c_str());
        return ret;
    }
    DHLOGI("DCameraSourceController StartCapture devId: %s, dhId: %s captureJson: %s", GetAnonyString(devId).c_str(),
        GetAnonyString(dhId).c_str(), jsonStr.c_str());
    std::shared_ptr<DataBuffer> buffer = std::make_shared<DataBuffer>(jsonStr.length() + 1);
    ret = memcpy_s(buffer->Data(), buffer->Capacity(), (uint8_t *)jsonStr.c_str(), jsonStr.length());
    if (ret != EOK) {
        DHLOGE("DCameraSourceController StartCapture memcpy_s failed %d, devId: %s, dhId: %s", ret,
            GetAnonyString(devId).c_str(), GetAnonyString(dhId).c_str());
        return ret;
    }
    ret = channel_->SendData(buffer);
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSourceController StartCapture SendData failed %d, devId: %s, dhId: %s", ret,
            GetAnonyString(devId).c_str(), GetAnonyString(dhId).c_str());
        return ret;
    }
    DHLOGI("DCameraSourceController StartCapture devId: %s, dhId: %s success", GetAnonyString(devId).c_str(),
        GetAnonyString(dhId).c_str());
    return DCAMERA_OK;
}

int32_t DCameraSourceController::StopCapture()
{
    if (indexs_.size() > DCAMERA_MAX_NUM) {
        DHLOGE("DCameraSourceController StopCapture not support operate %d camera", indexs_.size());
        return DCAMERA_BAD_OPERATE;
    }

    std::string dhId = indexs_.begin()->dhId_;
    std::string devId = indexs_.begin()->devId_;
    DHLOGI("DCameraSourceController StopCapture devId: %s, dhId: %s", GetAnonyString(devId).c_str(),
        GetAnonyString(dhId).c_str());
    sptr<IDistributedCameraSink> camSinkSrv = DCameraSourceServiceIpc::GetInstance().GetSinkRemoteDHMS(devId);
    if (camSinkSrv == nullptr) {
        DHLOGE("DCameraSourceController StopCapture can not get service, devId: %s", GetAnonyString(devId).c_str());
        return DCAMERA_BAD_OPERATE;
    }
    int32_t ret = camSinkSrv->StopCapture(dhId);
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSourceController StopCapture failed: %d, devId: %s, dhId: %s", ret,
            GetAnonyString(devId).c_str(), GetAnonyString(dhId).c_str());
        return DCAMERA_BAD_OPERATE;
    }
    DHLOGI("DCameraSourceController StopCapture devId: %s, dhId: %s success", GetAnonyString(devId).c_str(),
        GetAnonyString(dhId).c_str());
    return DCAMERA_OK;
}

int32_t DCameraSourceController::ChannelNeg(std::shared_ptr<DCameraChannelInfo>& info)
{
    if (indexs_.size() > DCAMERA_MAX_NUM) {
        DHLOGE("DCameraSourceController ChannelNeg not support operate %d camera", indexs_.size());
        return DCAMERA_BAD_OPERATE;
    }

    std::string dhId = indexs_.begin()->dhId_;
    std::string devId = indexs_.begin()->devId_;
    DHLOGI("DCameraSourceController ChannelNeg devId: %s, dhId: %s", GetAnonyString(devId).c_str(),
        GetAnonyString(dhId).c_str());
    sptr<IDistributedCameraSink> camSinkSrv = DCameraSourceServiceIpc::GetInstance().GetSinkRemoteDHMS(devId);
    if (camSinkSrv == nullptr) {
        DHLOGE("DCameraSourceController ChannelNeg can not get service, devId: %s", GetAnonyString(devId).c_str());
        return DCAMERA_BAD_OPERATE;
    }
    DCameraChannelInfoCmd cmd;
    cmd.type_ = DCAMERA_PROTOCOL_TYPE_MESSAGE;
    cmd.dhId_ = dhId;
    cmd.command_ = DCAMERA_PROTOCOL_CMD_CHAN_NEG;
    cmd.value_ = info;
    std::string jsonStr;
    int32_t ret = cmd.Marshal(jsonStr);
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSourceController ChannelNeg Marshal failed ret: %d, devId: %s, dhId: %s", ret,
            GetAnonyString(devId).c_str(), GetAnonyString(dhId).c_str());
        return ret;
    }
    DHLOGD("DCameraSourceController ChannelNeg devId: %s, dhId: %s channelNegJson: %s", GetAnonyString(devId).c_str(),
        GetAnonyString(dhId).c_str(), jsonStr.c_str());
    ret = camSinkSrv->ChannelNeg(dhId, jsonStr);
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSourceController ChannelNeg rpc failed ret: %d, devId: %s, dhId: %s", ret,
            GetAnonyString(devId).c_str(), GetAnonyString(dhId).c_str());
        return ret;
    }
    DHLOGD("DCameraSourceController ChannelNeg devId: %s, dhId: %s success", GetAnonyString(devId).c_str(),
        GetAnonyString(dhId).c_str());
    return DCAMERA_OK;
}

int32_t DCameraSourceController::DCameraNotify(std::shared_ptr<DCameraEvent>& events)
{
    sptr<IDCameraProvider> camHdiProvider = IDCameraProvider::Get();
    if (camHdiProvider == nullptr) {
        DHLOGI("DCameraNotify camHdiProvider is nullptr devId: %s dhId: %s", GetAnonyString(devId_).c_str(),
            GetAnonyString(dhId_).c_str());
        return DCAMERA_BAD_OPERATE;
    }
    std::shared_ptr<DHBase> dhBase = std::make_shared<DHBase>();
    dhBase->deviceId_ = devId_;
    dhBase->dhId_ = dhId_;
    std::shared_ptr<DCameraHDFEvent> hdiEvent = std::make_shared<DCameraHDFEvent>();
    hdiEvent->type_ = events->eventType_;
    hdiEvent->result_ = events->eventResult_;
    hdiEvent->content_ = events->eventContent_;
    DCamRetCode retHdi = camHdiProvider->Notify(dhBase, hdiEvent);
    DHLOGI("Nofify hal, ret: %d, devId: %s dhId: %s, type: %d, result: %d, content: %s", retHdi,
        GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str(), events->eventType_, events->eventResult_,
        events->eventContent_.c_str());
    if (retHdi != SUCCESS) {
        return DCAMERA_BAD_OPERATE;
    }
    return DCAMERA_OK;
}

int32_t DCameraSourceController::UpdateSettings(std::vector<std::shared_ptr<DCameraSettings>>& settings)
{
    if (indexs_.size() > DCAMERA_MAX_NUM) {
        DHLOGE("DCameraSourceController UpdateSettings not support operate %d camera", indexs_.size());
        return DCAMERA_BAD_OPERATE;
    }

    std::string dhId = indexs_.begin()->dhId_;
    std::string devId = indexs_.begin()->devId_;
    DHLOGI("DCameraSourceController UpdateSettings devId: %s, dhId: %s", GetAnonyString(devId).c_str(),
        GetAnonyString(dhId).c_str());
    DCameraMetadataSettingCmd cmd;
    cmd.type_ = DCAMERA_PROTOCOL_TYPE_MESSAGE;
    cmd.dhId_ = dhId;
    cmd.command_ = DCAMERA_PROTOCOL_CMD_UPDATE_METADATA;
    cmd.value_.assign(settings.begin(), settings.end());
    std::string jsonStr;
    int32_t ret = cmd.Marshal(jsonStr);
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSourceController UpdateSettings Marshal faied %d, devId: %s, dhId: %s", ret,
            GetAnonyString(devId).c_str(), GetAnonyString(dhId).c_str());
        return ret;
    }
    std::shared_ptr<DataBuffer> buffer = std::make_shared<DataBuffer>(jsonStr.length() + 1);
    ret = memcpy_s(buffer->Data(), buffer->Capacity(), (uint8_t *)jsonStr.c_str(), jsonStr.length());
    if (ret != EOK) {
        DHLOGE("DCameraSourceController UpdateSettings memcpy_s failed %d, devId: %s, dhId: %s", ret,
            GetAnonyString(devId).c_str(), GetAnonyString(dhId).c_str());
        return ret;
    }
    ret = channel_->SendData(buffer);
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSourceController UpdateSettings SendData failed %d, devId: %s, dhId: %s", ret,
            GetAnonyString(devId).c_str(), GetAnonyString(dhId).c_str());
        return ret;
    }
    DHLOGI("DCameraSourceController UpdateSettings devId: %s, dhId: %s success", GetAnonyString(devId).c_str(),
        GetAnonyString(dhId).c_str());
    return DCAMERA_OK;
}

int32_t DCameraSourceController::GetCameraInfo(std::shared_ptr<DCameraInfo>& camInfo)
{
    if (indexs_.size() > DCAMERA_MAX_NUM) {
        DHLOGE("DCameraSourceController GetCameraInfo not support operate %d camera", indexs_.size());
        return DCAMERA_BAD_OPERATE;
    }

    std::string dhId = indexs_.begin()->dhId_;
    std::string devId = indexs_.begin()->devId_;
    DHLOGI("DCameraSourceController GetCameraInfo devId: %s, dhId: %s", GetAnonyString(devId).c_str(),
        GetAnonyString(dhId).c_str());
    sptr<IDistributedCameraSink> camSinkSrv = DCameraSourceServiceIpc::GetInstance().GetSinkRemoteDHMS(devId);
    if (camSinkSrv == nullptr) {
        DHLOGE("DCameraSourceController GetCameraInfo can not get service, devId: %s", GetAnonyString(devId).c_str());
        return DCAMERA_BAD_OPERATE;
    }
    std::string camInfoJson;
    int32_t ret = camSinkSrv->GetCameraInfo(dhId, camInfoJson);
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSourceController GetCameraInfo GetCameraInfo failed: %d, devId: %s, dhId: %s", ret,
            GetAnonyString(devId).c_str(), GetAnonyString(dhId).c_str());
        return ret;
    }
    DCameraInfoCmd cmd;
    ret = cmd.Unmarshal(camInfoJson);
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSourceController GetCameraInfo DCameraInfoCmd Unmarshal failed: %d", ret);
        return ret;
    }
    camInfo = cmd.value_;
    return DCAMERA_OK;
}

int32_t DCameraSourceController::OpenChannel(std::shared_ptr<DCameraOpenInfo>& openInfo)
{
    if (indexs_.size() > DCAMERA_MAX_NUM) {
        DHLOGE("DCameraSourceController OpenChannel not support operate %d camera", indexs_.size());
        return DCAMERA_BAD_OPERATE;
    }
    std::string dhId = indexs_.begin()->dhId_;
    std::string devId = indexs_.begin()->devId_;
    DHLOGI("DCameraSourceController OpenChannel devId: %s, dhId: %s", GetAnonyString(devId).c_str(),
        GetAnonyString(dhId).c_str());
    sptr<IDistributedCameraSink> camSinkSrv = DCameraSourceServiceIpc::GetInstance().GetSinkRemoteDHMS(devId);
    if (camSinkSrv == nullptr) {
        DHLOGE("DCameraSourceController can not get service, devId: %s", GetAnonyString(devId).c_str());
        return DCAMERA_BAD_OPERATE;
    }

    DCameraOpenInfoCmd cmd;
    cmd.type_ = DCAMERA_PROTOCOL_TYPE_MESSAGE;
    cmd.dhId_ = dhId;
    cmd.command_ = DCAMERA_PROTOCOL_CMD_OPEN_CHANNEL;
    cmd.value_ = openInfo;
    std::string jsonStr;
    int32_t ret = cmd.Marshal(jsonStr);
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSourceController Marshal OpenInfo failed %d", ret);
        return ret;
    }
    DHLOGD("DCameraSourceController OpenChannel devId: %s, dhId: %s openJson: %s", GetAnonyString(devId).c_str(),
        GetAnonyString(dhId).c_str(), jsonStr.c_str());
    ret = camSinkSrv->OpenChannel(dhId, jsonStr);
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSourceController SA OpenChannel failed %d", ret);
        return ret;
    }
    DHLOGD("DCameraSourceController OpenChannel devId: %s, dhId: %s success", GetAnonyString(devId).c_str(),
        GetAnonyString(dhId).c_str(), jsonStr.c_str());

    std::vector<DCameraIndex> indexs;
    indexs.push_back(DCameraIndex(devId, dhId));
    ret = channel_->CreateSession(indexs, SESSION_FLAG, DCAMERA_SESSION_MODE_CTRL, listener_);
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSourceController CreateSession failed %d", ret);
        return ret;
    }
    ret = channel_->OpenSession();
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSourceController OpenChannel OpenChannel failed %d", ret);
        return ret;
    }

    return DCAMERA_OK;
}

int32_t DCameraSourceController::CloseChannel()
{
    if (indexs_.size() > DCAMERA_MAX_NUM) {
        DHLOGE("DCameraSourceController CloseChannel not support operate %d camera", indexs_.size());
        return DCAMERA_BAD_OPERATE;
    }

    std::string dhId = indexs_.begin()->dhId_;
    std::string devId = indexs_.begin()->devId_;
    DHLOGI("DCameraSourceController CloseChannel devId: %s, dhId: %s", GetAnonyString(devId).c_str(),
        GetAnonyString(dhId).c_str());
    int32_t ret = channel_->CloseSession();
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSourceController CloseChannel CloseSession failed %d", ret);
    }
    DHLOGI("DCameraSourceController CloseChannel devId: %s, dhId: %s success", GetAnonyString(devId).c_str(),
        GetAnonyString(dhId).c_str());
    channelState_ = DCAMERA_CHANNEL_STATE_DISCONNECTED;
    ret = channel_->ReleaseSession();
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSourceController CloseChannel ReleaseSession failed %d", ret);
    }
    sptr<IDistributedCameraSink> camSinkSrv = DCameraSourceServiceIpc::GetInstance().GetSinkRemoteDHMS(devId);
    if (camSinkSrv != nullptr) {
        ret = camSinkSrv->CloseChannel(dhId);
        if (ret != DCAMERA_OK) {
            DHLOGE("DCameraSourceController SA CloseChannel failed %d", ret);
        }
        DCameraSourceServiceIpc::GetInstance().DeleteSinkRemoteDhms(devId);
    }
    return ret;
}

int32_t DCameraSourceController::Init(std::vector<DCameraIndex>& indexs)
{
    DHLOGI("DCameraSourceController Init");
    if (indexs.size() > DCAMERA_MAX_NUM) {
        DHLOGE("DCameraSourceController init error");
        return DCAMERA_INIT_ERR;
    }
    indexs_.assign(indexs.begin(), indexs.end());
    std::string dhId = indexs_.begin()->dhId_;
    std::string devId = indexs_.begin()->devId_;
    auto controller = std::shared_ptr<DCameraSourceController>(shared_from_this());
    listener_ = std::make_shared<DCameraSourceControllerChannelListener>(controller);
    channel_ = std::make_shared<DCameraChannelSourceImpl>();
    DHLOGI("DCameraSourceController Init GetProvider end devId: %s, dhId: %s", GetAnonyString(devId).c_str(),
        GetAnonyString(dhId).c_str());
    isInit = true;
    return DCAMERA_OK;
}

int32_t DCameraSourceController::UnInit()
{
    DHLOGI("DCameraSourceController UnInit");
    indexs_.clear();
    isInit = false;
    return DCAMERA_OK;
}

void DCameraSourceController::OnSessionState(int32_t state)
{
    DHLOGI("DCameraSourceController OnSessionState state %d", state);
    channelState_ = state;
    switch (state) {
        case DCAMERA_CHANNEL_STATE_CONNECTED: {
            stateMachine_->UpdateState(DCAMERA_STATE_OPENED);
            std::shared_ptr<DCameraEvent> camEvent = std::make_shared<DCameraEvent>();
            camEvent->eventType_ = DCAMERA_MESSAGE;
            camEvent->eventResult_ = DCAMERA_EVENT_CHANNEL_CONNECTED;
            DCameraSourceEvent event(*this, DCAMERA_EVENT_NOFIFY, camEvent);
            eventBus_->PostEvent<DCameraSourceEvent>(event);
            break;
        }
        case DCAMERA_CHANNEL_STATE_DISCONNECTED: {
            DHLOGI("DCameraSourceDev PostTask Controller CloseSession OnClose devId %s dhId %s",
                GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str());
            DCameraIndex camIndex(devId_, dhId_);
            DCameraSourceEvent event(*this, DCAMERA_EVENT_CLOSE, camIndex);
            eventBus_->PostEvent<DCameraSourceEvent>(event);
            std::shared_ptr<DCameraEvent> camEvent = std::make_shared<DCameraEvent>();
            camEvent->eventType_ = DCAMERA_MESSAGE;
            camEvent->eventResult_ = DCAMERA_EVENT_CHANNEL_DISCONNECTED;
            DCameraSourceEvent eventNotify(*this, DCAMERA_EVENT_NOFIFY, camEvent);
            eventBus_->PostEvent<DCameraSourceEvent>(eventNotify);
            break;
        }
        default: {
            break;
        }
    }
}

void DCameraSourceController::OnSessionError(int32_t eventType, int32_t eventReason, std::string detail)
{
    DHLOGI("DCameraSourceController OnSessionError devId: %s, dhId: %s, eventType: %d, eventReason: %d, detail %s",
        GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str(), eventType, eventReason, detail.c_str());
    return;
}

void DCameraSourceController::OnDataReceived(std::vector<std::shared_ptr<DataBuffer>>& buffers)
{
    if (buffers.empty()) {
        DHLOGI("DCameraSourceController OnDataReceived empty, devId: %s, dhId: %s", GetAnonyString(devId_).c_str(),
            GetAnonyString(dhId_).c_str());
        return;
    }
    std::shared_ptr<DataBuffer> buffer = *(buffers.begin());
    std::string jsonStr((char *)buffer->Data());
    JSONCPP_STRING errs;
    Json::CharReaderBuilder readerBuilder;
    Json::Value rootValue;

    std::unique_ptr<Json::CharReader> const jsonReader(readerBuilder.newCharReader());
    if (!jsonReader->parse(jsonStr.c_str(), jsonStr.c_str() + jsonStr.length(), &rootValue, &errs) ||
        !rootValue.isObject()) {
        return;
    }

    if (!rootValue.isMember("Command") || !rootValue["Command"].isString()) {
        return;
    }
    std::string command = rootValue["Command"].asString();
    if (command == DCAMERA_PROTOCOL_CMD_METADATA_RESULT) {
        HandleMetaDataResult(jsonStr);
    }
    return;
}

void DCameraSourceController::HandleMetaDataResult(std::string& jsonStr)
{
    sptr<IDCameraProvider> camHdiProvider = IDCameraProvider::Get();
    if (camHdiProvider == nullptr) {
        DHLOGI("DCameraSourceController HandleMetaDataResult camHdiProvider is null, devId: %s, dhId: %s",
            GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str());
        return;
    }
    DCameraMetadataSettingCmd cmd;
    int32_t ret = cmd.Unmarshal(jsonStr);
    if (ret != DCAMERA_OK) {
        DHLOGI("DCameraSourceController HandleMetaDataResult failed, ret: %d, devId: %s, dhId: %s", ret,
            GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str());
        return;
    }
    std::shared_ptr<DHBase> dhBase = std::make_shared<DHBase>();
    dhBase->deviceId_ = devId_;
    dhBase->dhId_ = dhId_;
    for (auto iter = cmd.value_.begin(); iter != cmd.value_.end(); iter++) {
        DCamRetCode retHdi = camHdiProvider->OnSettingsResult(dhBase, (*iter));
        DHLOGI("OnSettingsResult hal, ret: %d, devId: %s dhId: %s, type: %d, result: %d, content: %s", retHdi,
            GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str());
    }
}
} // namespace DistributedHardware
} // namespace OHOS
