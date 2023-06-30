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

#include "dcamera_channel_sink_impl.h"

#include "dcamera_softbus_adapter.h"

#include "anonymous_string.h"
#include "distributed_camera_constants.h"
#include "distributed_camera_errno.h"
#include "distributed_hardware_log.h"

namespace OHOS {
namespace DistributedHardware {
DCameraChannelSinkImpl::DCameraChannelSinkImpl()
{
    softbusSession_ = nullptr;
}

DCameraChannelSinkImpl::~DCameraChannelSinkImpl()
{
}

int32_t DCameraChannelSinkImpl::OpenSession()
{
    DHLOGI("DCameraChannelSinkImpl OpenSession name: %s", mySessionName_.c_str());
    if (softbusSession_ == nullptr) {
        DHLOGE("DCameraChannelSinkImpl OpenSession %s failed", mySessionName_.c_str());
        return DCAMERA_BAD_OPERATE;
    }
    int32_t ret = softbusSession_->OpenSession();
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraChannelSinkImpl OpenSession %s ret: %d", mySessionName_.c_str(), ret);
    }

    return ret;
}

int32_t DCameraChannelSinkImpl::CloseSession()
{
    DHLOGI("DCameraChannelSinkImpl CloseSession name: %s", mySessionName_.c_str());
    if (softbusSession_ == nullptr) {
        DHLOGE("DCameraChannelSinkImpl CloseSession %s failed", mySessionName_.c_str());
        return DCAMERA_BAD_OPERATE;
    }
    int32_t ret = softbusSession_->CloseSession();
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraChannelSinkImpl CloseSession %s ret: %d", mySessionName_.c_str(), ret);
    }

    return ret;
}

int32_t DCameraChannelSinkImpl::CreateSession(std::vector<DCameraIndex>& camIndexs, std::string sessionFlag,
    DCameraSessionMode sessionMode, std::shared_ptr<ICameraChannelListener>& listener)
{
    if (camIndexs.size() > DCAMERA_MAX_NUM || listener == nullptr) {
        return DCAMERA_BAD_VALUE;
    }
    if (softbusSession_ != nullptr) {
        DHLOGI("DCameraChannelSinkImpl session has already create %s", sessionFlag.c_str());
        return DCAMERA_OK;
    }
    camIndexs_.assign(camIndexs.begin(), camIndexs.end());
    listener_ = listener;
    mySessionName_ = SESSION_HEAD + camIndexs[0].dhId_ + std::string("_") + sessionFlag;
    mode_ = sessionMode;
    std::string myDevId;
    DCameraSoftbusAdapter::GetInstance().GetLocalNetworkId(myDevId);
    DHLOGI("DCameraChannelSinkImpl session create name: %s devId: %s", mySessionName_.c_str(),
        GetAnonyString(myDevId).c_str());
    int32_t ret = DCameraSoftbusAdapter::GetInstance().CreateSoftbusSessionServer(mySessionName_,
        DCAMERA_CHANNLE_ROLE_SINK);
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraChannelSinkImpl CreateSession %s failed, ret: %d", mySessionName_.c_str(), ret);
        return ret;
    }
    std::string peerDevId = camIndexs[0].devId_;
    std::string peerSessionName = SESSION_HEAD + sessionFlag;
    softbusSession_ = std::make_shared<DCameraSoftbusSession>(myDevId, mySessionName_, peerDevId, peerSessionName,
        listener, sessionMode);
    DCameraSoftbusAdapter::GetInstance().sinkSessions_[mySessionName_] = softbusSession_;
    return DCAMERA_OK;
}

int32_t DCameraChannelSinkImpl::ReleaseSession()
{
    DHLOGI("DCameraChannelSinkImpl ReleaseSession name: %s", mySessionName_.c_str());
    if (softbusSession_ == nullptr) {
        return DCAMERA_OK;
    }
    DCameraSoftbusAdapter::GetInstance().sourceSessions_.erase(softbusSession_->GetMySessionName());
    int32_t ret = DCameraSoftbusAdapter::GetInstance().DestroySoftbusSessionServer(softbusSession_->GetMySessionName());
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraChannelSinkImpl ReleaseSession %s failed, ret: %d", mySessionName_.c_str(), ret);
    }
    softbusSession_ = nullptr;
    return ret;
}

int32_t DCameraChannelSinkImpl::SendData(std::shared_ptr<DataBuffer>& buffer)
{
    if (softbusSession_ == nullptr) {
        DHLOGE("DCameraChannelSinkImpl SendData %s failed", mySessionName_.c_str());
        return DCAMERA_BAD_OPERATE;
    }
    int32_t ret = softbusSession_->SendData(mode_, buffer);
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraChannelSinkImpl SendData %s failed, ret: %d", mySessionName_.c_str(), ret);
    }
    return ret;
}
} // namespace DistributedHardware
} // namespace OHOS
