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

#include "dcamera_source_dev.h"

#include "anonymous_string.h"
#include "distributed_camera_constants.h"
#include "distributed_camera_errno.h"
#include "distributed_hardware_log.h"

#include "dcamera_channel_info_cmd.h"
#include "dcamera_info_cmd.h"
#include "dcamera_provider_callback_impl.h"
#include "dcamera_source_controller.h"
#include "dcamera_source_input.h"
#include "dcamera_utils_tools.h"

namespace OHOS {
namespace DistributedHardware {
DCameraSourceDev::DCameraSourceDev(std::string devId, std::string dhId,
    std::shared_ptr<ICameraStateListener>& stateLisener) : devId_(devId), dhId_(dhId), stateListener_(stateLisener)
{
    DHLOGI("DCameraSourceDev Construct devId %s dhId %s", GetAnonyString(devId_).c_str(),
        GetAnonyString(dhId_).c_str());
    memberFuncMap_[DCAMERA_EVENT_REGIST] = &DCameraSourceDev::NotifyRegisterResult;
    memberFuncMap_[DCAMERA_EVENT_UNREGIST] = &DCameraSourceDev::NotifyUnregisterResult;
    memberFuncMap_[DCAMERA_EVENT_OPEN] = &DCameraSourceDev::NotifyHalResult;
    memberFuncMap_[DCAMERA_EVENT_CLOSE] = &DCameraSourceDev::NotifyHalResult;
    memberFuncMap_[DCAMERA_EVENT_CONFIG_STREAMS] = &DCameraSourceDev::NotifyHalResult;
    memberFuncMap_[DCAMERA_EVENT_RELEASE_STREAMS] = &DCameraSourceDev::NotifyHalResult;
    memberFuncMap_[DCAMERA_EVENT_START_CAPTURE] = &DCameraSourceDev::NotifyHalResult;
    memberFuncMap_[DCAMERA_EVENT_STOP_CAPTURE] = &DCameraSourceDev::NotifyHalResult;
    memberFuncMap_[DCAMERA_EVENT_UPDATE_SETTINGS] = &DCameraSourceDev::NotifyHalResult;

    eventResultMap_[DCAMERA_EVENT_OPEN] = DCAMERA_EVENT_OPEN_CHANNEL_ERROR;
    eventResultMap_[DCAMERA_EVENT_CLOSE] = DCAMERA_EVENT_CLOSE_CHANNEL_ERROR;
    eventResultMap_[DCAMERA_EVENT_CONFIG_STREAMS] = DCAMERA_EVENT_CONFIG_STREAMS_ERROR;
    eventResultMap_[DCAMERA_EVENT_RELEASE_STREAMS] = DCAMERA_EVENT_RELEASE_STREAMS_ERROR;
    eventResultMap_[DCAMERA_EVENT_START_CAPTURE] = DCAMERA_EVENT_START_CAPTURE_ERROR;
    eventResultMap_[DCAMERA_EVENT_STOP_CAPTURE] = DCAMERA_EVENT_STOP_CAPTURE_ERROR;
    eventResultMap_[DCAMERA_EVENT_UPDATE_SETTINGS] = DCAMERA_EVENT_UPDATE_SETTINGS_ERROR;
}

DCameraSourceDev::~DCameraSourceDev()
{
    DHLOGI("DCameraSourceDev Delete devId %s dhId %s", GetAnonyString(devId_).c_str(),
        GetAnonyString(dhId_).c_str());
}

int32_t DCameraSourceDev::InitDCameraSourceDev()
{
    DHLOGI("DCameraSourceDev InitDCameraSourceDev devId %s dhId %s", GetAnonyString(devId_).c_str(),
        GetAnonyString(dhId_).c_str());
    eventBus_ = std::make_shared<EventBus>();
    DCameraSourceEvent event(*this);
    eventBus_->AddHandler<DCameraSourceEvent>(event.GetType(), *this);
    auto cameraSourceDev = std::shared_ptr<DCameraSourceDev>(shared_from_this());
    stateMachine_ = std::make_shared<DCameraSourceStateMachine>(cameraSourceDev);
    stateMachine_->UpdateState(DCAMERA_STATE_INIT);
    controller_ = std::make_shared<DCameraSourceController>(devId_, dhId_, stateMachine_, eventBus_);
    input_ = std::make_shared<DCameraSourceInput>(devId_, dhId_, eventBus_);
    hdiCallback_ = new (std::nothrow) DCameraProviderCallbackImpl(devId_, dhId_, cameraSourceDev);
    DHLOGI("DCameraSourceDev InitDCameraSourceDev end devId %s dhId %s", GetAnonyString(devId_).c_str(),
        GetAnonyString(dhId_).c_str());
    return DCAMERA_OK;
}

int32_t DCameraSourceDev::RegisterDistributedHardware(const std::string& devId, const std::string& dhId,
    const std::string& reqId, const std::string& ver, const std::string attrs)
{
    DHLOGI("DCameraSourceDev PostTask RegisterDistributedHardware devId %s dhId %s", GetAnonyString(devId).c_str(),
        GetAnonyString(dhId).c_str());
    version_ = ver;
    DCameraIndex index(devId, dhId);
    actualDevInfo_.insert(index);

    std::shared_ptr<DCameraRegistParam> regParam = std::make_shared<DCameraRegistParam>(devId, dhId, reqId, attrs);
    DCameraSourceEvent event(*this, DCAMERA_EVENT_REGIST, regParam);
    eventBus_->PostEvent<DCameraSourceEvent>(event);
    return DCAMERA_OK;
}

int32_t DCameraSourceDev::UnRegisterDistributedHardware(const std::string devId, const std::string dhId,
    const std::string reqId)
{
    DHLOGI("DCameraSourceDev PostTask UnRegisterDistributedHardware devId %s dhId %s", GetAnonyString(devId).c_str(),
        GetAnonyString(dhId).c_str());
    std::string param;
    std::shared_ptr<DCameraRegistParam> regParam = std::make_shared<DCameraRegistParam>(devId, dhId, reqId, param);
    DCameraSourceEvent event(*this, DCAMERA_EVENT_UNREGIST, regParam);
    eventBus_->PostEvent<DCameraSourceEvent>(event);
    return DCAMERA_OK;
}

int32_t DCameraSourceDev::DCameraNotify(std::string& eventStr)
{
    DHLOGI("DCameraSourceDev PostTask DCameraNotify devId %s dhId %s", GetAnonyString(devId_).c_str(),
        GetAnonyString(dhId_).c_str());
    DCameraEventCmd cmd;
    int32_t ret = cmd.Unmarshal(eventStr);
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSourceDev DCameraNotify devId %s dhId %s marshal failed, ret: %d",
            GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str(), ret);
        return ret;
    }

    DCameraSourceEvent event(*this, DCAMERA_EVENT_NOFIFY, cmd.value_);
    eventBus_->PostEvent<DCameraSourceEvent>(event);
    return DCAMERA_OK;
}

int32_t DCameraSourceDev::OpenSession(DCameraIndex& camIndex)
{
    DHLOGI("DCameraSourceDev PostTask OpenSession devId %s dhId %s", GetAnonyString(devId_).c_str(),
        GetAnonyString(dhId_).c_str());
    DCameraSourceEvent event(*this, DCAMERA_EVENT_OPEN, camIndex);
    eventBus_->PostEvent<DCameraSourceEvent>(event);
    return DCAMERA_OK;
}

int32_t DCameraSourceDev::CloseSession(DCameraIndex& camIndex)
{
    DHLOGI("DCameraSourceDev PostTask CloseSession devId %s dhId %s", GetAnonyString(devId_).c_str(),
        GetAnonyString(dhId_).c_str());
    DCameraSourceEvent event(*this, DCAMERA_EVENT_CLOSE, camIndex);
    eventBus_->PostEvent<DCameraSourceEvent>(event);
    return DCAMERA_OK;
}

int32_t DCameraSourceDev::ConfigStreams(const std::vector<std::shared_ptr<DCStreamInfo>>& streamInfos)
{
    DHLOGI("DCameraSourceDev PostTask ConfigStreams devId %s dhId %s", GetAnonyString(devId_).c_str(),
        GetAnonyString(dhId_).c_str());
    DCameraSourceEvent event(*this, DCAMERA_EVENT_CONFIG_STREAMS, streamInfos);
    eventBus_->PostEvent<DCameraSourceEvent>(event);
    return DCAMERA_OK;
}

int32_t DCameraSourceDev::ReleaseStreams(const std::vector<int>& streamIds)
{
    DHLOGI("DCameraSourceDev PostTask ReleaseStreams devId %s dhId %s", GetAnonyString(devId_).c_str(),
        GetAnonyString(dhId_).c_str());
    DCameraSourceEvent event(*this, DCAMERA_EVENT_RELEASE_STREAMS, streamIds);
    eventBus_->PostEvent<DCameraSourceEvent>(event);
    return DCAMERA_OK;
}

int32_t DCameraSourceDev::StartCapture(const std::vector<std::shared_ptr<DCCaptureInfo>>& captureInfos)
{
    DHLOGI("DCameraSourceDev PostTask StartCapture devId %s dhId %s", GetAnonyString(devId_).c_str(),
        GetAnonyString(dhId_).c_str());
    DCameraSourceEvent event(*this, DCAMERA_EVENT_START_CAPTURE, captureInfos);
    eventBus_->PostEvent<DCameraSourceEvent>(event);
    return DCAMERA_OK;
}

int32_t DCameraSourceDev::StopCapture()
{
    DHLOGI("DCameraSourceDev PostTask StopCapture devId %s dhId %s", GetAnonyString(devId_).c_str(),
        GetAnonyString(dhId_).c_str());
    DCameraSourceEvent event(*this, DCAMERA_EVENT_STOP_CAPTURE);
    eventBus_->PostEvent<DCameraSourceEvent>(event);
    return DCAMERA_OK;
}

int32_t DCameraSourceDev::UpdateCameraSettings(const std::vector<std::shared_ptr<DCameraSettings>>& settings)
{
    DHLOGI("DCameraSourceDev PostTask UpdateCameraSettings devId %s dhId %s", GetAnonyString(devId_).c_str(),
        GetAnonyString(dhId_).c_str());
    DCameraSourceEvent event(*this, DCAMERA_EVENT_UPDATE_SETTINGS, settings);
    eventBus_->PostEvent<DCameraSourceEvent>(event);
    return DCAMERA_OK;
}

void DCameraSourceDev::OnEvent(DCameraSourceEvent& event)
{
    DHLOGI("DCameraSourceDev OnEvent devId %s dhId %s eventType: %d", GetAnonyString(devId_).c_str(),
        GetAnonyString(dhId_).c_str(), event.GetEventType());
    int32_t ret = stateMachine_->Execute(event.GetEventType(), event);
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSourceDev OnEvent failed, ret: %d, devId: %s dhId: %s", ret,
            GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str());
    }
    NotifyResult(event.GetEventType(), event, ret);
}

int32_t DCameraSourceDev::ExecuteRegister(std::shared_ptr<DCameraRegistParam>& param)
{
    DHLOGI("DCameraSourceDev Execute Register devId: %s dhId: %s",
        GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str());
    std::vector<DCameraIndex> actualDevInfo;
    actualDevInfo.assign(actualDevInfo_.begin(), actualDevInfo_.end());
    int32_t ret = controller_->Init(actualDevInfo);
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSourceDev Execute Register controller init failed, ret: %d, devId: %s dhId: %s", ret,
            GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str());
        return ret;
    }

    ret = input_->Init();
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSourceDev Execute Register input init failed, ret: %d, devId: %s dhId: %s", ret,
            GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str());
        controller_->UnInit();
        return ret;
    }

    sptr<IDCameraProvider> camHdiProvider = IDCameraProvider::Get();
    if (camHdiProvider == nullptr) {
        DHLOGI("ExecuteRegister camHdiProvider is nullptr devId: %s dhId: %s", GetAnonyString(devId_).c_str(),
            GetAnonyString(dhId_).c_str());
        controller_->UnInit();
        input_->UnInit();
        return DCAMERA_BAD_OPERATE;
    }
    std::shared_ptr<DHBase> dhBase = std::make_shared<DHBase>();
    dhBase->deviceId_ = param->devId_;
    dhBase->dhId_ = param->dhId_;
    DCamRetCode retHdi = camHdiProvider->EnableDCameraDevice(dhBase, param->param_, hdiCallback_);
    DHLOGI("DCameraSourceDev Execute Register register hal, ret: %d, devId: %s dhId: %s", retHdi,
        GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str());
    if (retHdi != SUCCESS) {
        controller_->UnInit();
        input_->UnInit();
        return DCAMERA_REGIST_HAL_FAILED;
    }
    return DCAMERA_OK;
}

int32_t DCameraSourceDev::ExecuteUnRegister(std::shared_ptr<DCameraRegistParam>& param)
{
    DHLOGI("DCameraSourceDev Execute UnRegister devId: %s dhId: %s", GetAnonyString(devId_).c_str(),
        GetAnonyString(dhId_).c_str());
    int32_t ret = controller_->UnInit();
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSourceDev Execute UnRegister controller uninit failed, ret: %d, devId: %s dhId: %s", ret,
            GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str());
    }

    ret = input_->UnInit();
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSourceDev Execute UnRegister input uninit failed, ret: %d, devId: %s dhId: %s", ret,
            GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str());
    }

    sptr<IDCameraProvider> camHdiProvider = IDCameraProvider::Get();
    if (camHdiProvider == nullptr) {
        DHLOGI("ExecuteUnRegister camHdiProvider is nullptr devId: %s dhId: %s", GetAnonyString(devId_).c_str(),
            GetAnonyString(dhId_).c_str());
        return DCAMERA_BAD_OPERATE;
    }

    std::shared_ptr<DHBase> dhBase = std::make_shared<DHBase>();
    dhBase->deviceId_ = param->devId_;
    dhBase->dhId_ = param->dhId_;
    DCamRetCode retHdi = camHdiProvider->DisableDCameraDevice(dhBase);
    DHLOGI("DCameraSourceDev Execute UnRegister unregister hal, ret: %d, devId: %s dhId: %s", retHdi,
        GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str());
    if (retHdi != SUCCESS) {
        return DCAMERA_UNREGIST_HAL_FAILED;
    }
    return DCAMERA_OK;
}

int32_t DCameraSourceDev::ExecuteOpenCamera()
{
    DHLOGI("DCameraSourceDev Execute OpenCamera devId %s dhId %s", GetAnonyString(devId_).c_str(),
        GetAnonyString(dhId_).c_str());
    std::shared_ptr<DCameraOpenInfo> openInfo = std::make_shared<DCameraOpenInfo>();
    int32_t ret = GetLocalDeviceNetworkId(openInfo->sourceDevId_);
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSourceDev getMyId failed ret: %d, devId: %s, dhId: %s", ret,
            GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str());
        return ret;
    }

    ret = controller_->OpenChannel(openInfo);
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSourceDev Execute OpenCamera OpenChannel failed, ret: %d, devId: %s dhId: %s", ret,
            GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str());
        return DCAMERA_OPEN_CONFLICT;
    }
    return DCAMERA_OK;
}

int32_t DCameraSourceDev::ExecuteCloseCamera()
{
    DHLOGI("DCameraSourceDev Execute CloseCamera devId %s dhId %s", GetAnonyString(devId_).c_str(),
        GetAnonyString(dhId_).c_str());
    int32_t ret = input_->CloseChannel();
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSourceDev Execute CloseCamera input CloseChannel failed, ret: %d, devId: %s dhId: %s", ret,
            GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str());
    }
    ret = controller_->CloseChannel();
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSourceDev Execute CloseCamera controller CloseChannel failed, ret: %d, devId: %s dhId: %s", ret,
            GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str());
    }
    return DCAMERA_OK;
}

int32_t DCameraSourceDev::ExecuteConfigStreams(std::vector<std::shared_ptr<DCStreamInfo>>& streamInfos)
{
    DHLOGI("DCameraSourceDev Execute ConfigStreams devId %s dhId %s", GetAnonyString(devId_).c_str(),
        GetAnonyString(dhId_).c_str());
    int32_t ret = input_->ConfigStreams(streamInfos);
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSourceDev Execute ConfigStreams ConfigStreams failed, ret: %d, devId: %s dhId: %s", ret,
            GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str());
        return ret;
    }

    std::shared_ptr<DCameraChannelInfo> chanInfo = std::make_shared<DCameraChannelInfo>();
    ret = GetLocalDeviceNetworkId(chanInfo->sourceDevId_);
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSourceDev getMyId failed ret: %d, devId: %s, dhId: %s", ret,
            GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str());
        return ret;
    }
    DCameraChannelDetail continueChInfo(CONTINUE_SESSION_FLAG, CONTINUOUS_FRAME);
    DCameraChannelDetail snapShotChInfo(SNAP_SHOT_SESSION_FLAG, SNAPSHOT_FRAME);
    chanInfo->detail_.push_back(continueChInfo);
    chanInfo->detail_.push_back(snapShotChInfo);

    ret = controller_->ChannelNeg(chanInfo);
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSourceDev ChannelNeg failed ret: %d, devId: %s, dhId: %s", ret,
            GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str());
        return ret;
    }

    std::vector<DCameraIndex> actualDevInfo;
    actualDevInfo.assign(actualDevInfo_.begin(), actualDevInfo_.end());
    ret = input_->OpenChannel(actualDevInfo);
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSourceDev ChannelNeg OpenChannel failed ret: %d, devId: %s, dhId: %s", ret,
            GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str());
        return ret;
    }
    return DCAMERA_OK;
}

int32_t DCameraSourceDev::ExecuteReleaseStreams(std::vector<int>& streamIds, bool& isAllRelease)
{
    DHLOGI("DCameraSourceDev Execute ReleaseStreams devId %s dhId %s", GetAnonyString(devId_).c_str(),
        GetAnonyString(dhId_).c_str());
    int32_t ret = input_->ReleaseStreams(streamIds, isAllRelease);
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSourceDev Execute ReleaseStreams failed ret: %d, devId: %s, dhId: %s", ret,
            GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str());
        return ret;
    }
    return DCAMERA_OK;
}

int32_t DCameraSourceDev::ExecuteReleaseAllStreams()
{
    DHLOGI("DCameraSourceDev Execute ReleaseAllStreams devId %s dhId %s", GetAnonyString(devId_).c_str(),
        GetAnonyString(dhId_).c_str());
    int32_t ret = input_->ReleaseAllStreams();
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSourceDev Execute ReleaseAllStreams failed ret: %d, devId: %s, dhId: %s", ret,
            GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str());
        return ret;
    }
    return DCAMERA_OK;
}

int32_t DCameraSourceDev::ExecuteStartCapture(std::vector<std::shared_ptr<DCCaptureInfo>>& captureInfos)
{
    DHLOGI("DCameraSourceDev Execute StartCapture devId %s dhId %s", GetAnonyString(devId_).c_str(),
        GetAnonyString(dhId_).c_str());
    int32_t ret = input_->StartCapture(captureInfos);
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSourceDev input StartCapture failed ret: %d, devId: %s, dhId: %s", ret,
            GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str());
        return ret;
    }
    std::vector<std::shared_ptr<DCameraCaptureInfo>> captures;
    for (auto iter = captureInfos.begin(); iter != captureInfos.end(); iter++) {
        std::shared_ptr<DCameraCaptureInfo> capture = std::make_shared<DCameraCaptureInfo>();
        capture->width_ = (*iter)->width_;
        capture->height_ = (*iter)->height_;
        capture->format_ = (*iter)->format_;
        capture->dataspace_ = (*iter)->dataspace_;
        capture->isCapture_ = (*iter)->isCapture_;
        capture->encodeType_ = (*iter)->encodeType_;
        capture->streamType_ = (*iter)->type_;
        DHLOGI("ExecuteStartCapture devId %s dhId %s settings size: %d w: %d h: %d fmt: %d isC: %d enc: %d streamT: %d",
            GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str(), (*iter)->captureSettings_.size(),
            capture->width_, capture->height_, capture->format_, capture->isCapture_ ? 1 : 0, capture->encodeType_,
            capture->streamType_);
        for (auto settingIter = (*iter)->captureSettings_.begin(); settingIter != (*iter)->captureSettings_.end();
            settingIter++) {
            std::shared_ptr<DCameraSettings> setting = std::make_shared<DCameraSettings>();
            setting->type_ = (*settingIter)->type_;
            setting->value_ = (*settingIter)->value_;
            capture->captureSettings_.push_back(setting);
        }
        captures.push_back(capture);
    }

    ret = controller_->StartCapture(captures);
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSourceDev Execute StartCapture StartCapture failed, ret: %d, devId: %s dhId: %s", ret,
            GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str());
    }
    return ret;
}

int32_t DCameraSourceDev::ExecuteStopCapture()
{
    DHLOGI("DCameraSourceDev Execute StopCapture devId %s dhId %s", GetAnonyString(devId_).c_str(),
        GetAnonyString(dhId_).c_str());
    int32_t ret = input_->StopCapture();
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSourceDev Execute StopCapture input StopCapture failed, ret: %d, devId: %s dhId: %s", ret,
            GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str());
    }
    ret = controller_->StopCapture();
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSourceDev Execute StopCapture controller StopCapture failed, ret: %d, devId: %s dhId: %s", ret,
            GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str());
    }
    return DCAMERA_OK;
}

int32_t DCameraSourceDev::ExecuteUpdateSettings(std::vector<std::shared_ptr<DCameraSettings>>& settings)
{
    DHLOGI("DCameraSourceDev Execute UpdateSettings devId %s dhId %s", GetAnonyString(devId_).c_str(),
        GetAnonyString(dhId_).c_str());
    int32_t ret = input_->UpdateSettings(settings);
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSourceDev Execute UpdateSettings input UpdateSettings failed, ret: %d, devId: %s dhId: %s", ret,
            GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str());
        return ret;
    }
    ret = controller_->UpdateSettings(settings);
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSourceDev Execute UpdateSettings controller UpdateSettings failed, ret: %d, devId: %s dhId: %s",
            ret, GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str());
        return ret;
    }
    return DCAMERA_OK;
}

int32_t DCameraSourceDev::ExecuteCameraEventNotify(std::shared_ptr<DCameraEvent>& events)
{
    DHLOGI("DCameraSourceDev Execute CameraEventNotify devId %s dhId %s", GetAnonyString(devId_).c_str(),
        GetAnonyString(dhId_).c_str());
    int32_t ret = controller_->DCameraNotify(events);
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSourceDev Execute CameraEventNotify DCameraNotify failed, ret: %d, devId: %s dhId: %s", ret,
            GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str());
        return ret;
    }
    return DCAMERA_OK;
}

void DCameraSourceDev::NotifyResult(DCAMERA_EVENT eventType, DCameraSourceEvent& event, int32_t result)
{
    auto itFunc = memberFuncMap_.find(eventType);
    if (itFunc == memberFuncMap_.end()) {
        DHLOGE("Notify func map not find, execute %d notify, devId: %s dhId: %s", eventType,
            GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str());
        return;
    }

    auto memberFunc = itFunc->second;
    (this->*memberFunc)(eventType, event, result);
    return;
}

void DCameraSourceDev::NotifyRegisterResult(DCAMERA_EVENT eventType, DCameraSourceEvent& event, int32_t result)
{
    std::string data = "";
    std::shared_ptr<DCameraRegistParam> param;
    int32_t ret = event.GetDCameraRegistParam(param);
    if (ret != DCAMERA_OK) {
        return;
    }

    if (stateListener_ == nullptr) {
        DHLOGE("DCameraSourceDev can not get listener");
        return;
    }
    stateListener_->OnRegisterNotify(param->devId_, param->dhId_, param->reqId_, result, data);
}

void DCameraSourceDev::NotifyUnregisterResult(DCAMERA_EVENT eventType, DCameraSourceEvent& event, int32_t result)
{
    std::string data = "";
    std::shared_ptr<DCameraRegistParam> param;
    int32_t ret = event.GetDCameraRegistParam(param);
    if (ret != DCAMERA_OK) {
        return;
    }

    if (stateListener_ == nullptr) {
        DHLOGE("DCameraSourceDev can not get listener");
        return;
    }
    stateListener_->OnUnregisterNotify(param->devId_, param->dhId_, param->reqId_, result, data);
}

void DCameraSourceDev::NotifyHalResult(DCAMERA_EVENT eventType, DCameraSourceEvent& event, int32_t result)
{
    DHLOGI("DCameraSourceDev NotifyHalResult eventType: %d, result: %d devId: %s dhId: %s", eventType, result,
        GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str());
    if (result == DCAMERA_OK) {
        return;
    }

    std::shared_ptr<DCameraEvent> events = std::make_shared<DCameraEvent>();
    events->eventType_ = DCAMERA_OPERATION;
    auto iter = eventResultMap_.find(eventType);
    if (iter == eventResultMap_.end()) {
        return;
    }
    events->eventResult_ = iter->second;
    ExecuteCameraEventNotify(events);
    return;
}
} // namespace DistributedHardware
} // namespace OHOS
