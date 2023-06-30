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

#include "dcamera_softbus_session.h"

#include <securec.h>

#include "anonymous_string.h"
#include "dcamera_softbus_adapter.h"
#include "distributed_camera_constants.h"
#include "distributed_camera_errno.h"
#include "distributed_hardware_log.h"

namespace OHOS {
namespace DistributedHardware {
DCameraSoftbusSession::DCameraSoftbusSession()
{
    sessionId_ = -1;
    state_ = DCAMERA_SOFTBUS_STATE_CLOSED;
    mode_ = DCAMERA_SESSION_MODE_CTRL;
    ResetAssembleFrag();
}

DCameraSoftbusSession::DCameraSoftbusSession(std::string myDevId, std::string mySessionName, std::string peerDevId,
    std::string peerSessionName, std::shared_ptr<ICameraChannelListener> listener, DCameraSessionMode mode)
    : myDevId_(myDevId), mySessionName_(mySessionName), peerDevId_(peerDevId), peerSessionName_(peerSessionName),
    listener_(listener), sessionId_(-1), state_(DCAMERA_SOFTBUS_STATE_CLOSED), mode_(mode)
{
    sendFuncMap_[DCAMERA_SESSION_MODE_CTRL] = &DCameraSoftbusSession::SendBytes;
    sendFuncMap_[DCAMERA_SESSION_MODE_VIDEO] = &DCameraSoftbusSession::SendStream;
    sendFuncMap_[DCAMERA_SESSION_MODE_JPEG] = &DCameraSoftbusSession::SendStream;
    auto runner = AppExecFwk::EventRunner::Create(mySessionName);
    eventHandler_ = std::make_shared<AppExecFwk::EventHandler>(runner);
    ResetAssembleFrag();
}

DCameraSoftbusSession::~DCameraSoftbusSession()
{
    if (sessionId_ != -1) {
        int32_t ret = DCameraSoftbusAdapter::GetInstance().CloseSoftbusSession(sessionId_);
        if (ret != DCAMERA_OK) {
            DHLOGE("DCameraSoftbusSession delete failed, ret: %d, sessId: %d peerDevId: %s peerSessionName: %s", ret,
                sessionId_, GetAnonyString(peerDevId_).c_str(), GetAnonyString(peerSessionName_).c_str());
        }
    }
    sendFuncMap_.clear();
    eventHandler_ = nullptr;
}

int32_t DCameraSoftbusSession::OpenSession()
{
    DHLOGI("DCameraSoftbusSession OpenSession peerDevId: %s peerSessionName: %s",
        GetAnonyString(peerDevId_).c_str(), GetAnonyString(peerSessionName_).c_str());
    int32_t ret = DCameraSoftbusAdapter::GetInstance().OpenSoftbusSession(mySessionName_, peerSessionName_, mode_,
        peerDevId_);
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSoftbusSession OpenSession failed, ret: %d, peerDevId: %s peerSessionName: %s", ret,
            GetAnonyString(peerDevId_).c_str(), GetAnonyString(peerSessionName_).c_str());
        return ret;
    }

    listener_->OnSessionState(DCAMERA_CHANNEL_STATE_CONNECTING);
    return DCAMERA_OK;
}

int32_t DCameraSoftbusSession::CloseSession()
{
    DHLOGI("DCameraSoftbusSession CloseSession sessionId: %d peerDevId: %s peerSessionName: %s", sessionId_,
        GetAnonyString(peerDevId_).c_str(), GetAnonyString(peerSessionName_).c_str());
    if (sessionId_ == -1) {
        DHLOGI("DCameraSoftbusSession CloseSession has already close peerDevId: %s peerSessionName: %s",
            GetAnonyString(peerDevId_).c_str(), GetAnonyString(peerSessionName_).c_str());
        return DCAMERA_OK;
    }
    int32_t ret = DCameraSoftbusAdapter::GetInstance().CloseSoftbusSession(sessionId_);
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSoftbusSession CloseSession failed, ret: %d, peerDevId: %s peerSessionName: %s", ret,
            GetAnonyString(peerDevId_).c_str(), GetAnonyString(peerSessionName_).c_str());
        return ret;
    }

    sessionId_ = -1;
    state_ = DCAMERA_SOFTBUS_STATE_CLOSED;
    return DCAMERA_OK;
}

int32_t DCameraSoftbusSession::OnSessionOpend(int32_t sessionId, int32_t result)
{
    DHLOGI("DCameraSoftbusSession OnSessionOpend sessionId: %d result: %d peerDevId: %s peerSessionName: %s",
        sessionId, result, GetAnonyString(peerDevId_).c_str(), GetAnonyString(peerSessionName_).c_str());
    if (result != DCAMERA_OK) {
        DHLOGE("DCameraSoftbusSession OnSessionOpend sessionId: %d result: %d peerDevId: %s peerSessionName: %s",
            sessionId_, result, GetAnonyString(peerDevId_).c_str(), GetAnonyString(peerSessionName_).c_str());
        listener_->OnSessionState(DCAMERA_CHANNEL_STATE_DISCONNECTED);
        listener_->OnSessionError(DCAMERA_MESSAGE, DCAMERA_EVENT_OPEN_CHANNEL_ERROR,
            std::string("softbus internal error"));
        return result;
    }

    sessionId_ = sessionId;
    state_ = DCAMERA_SOFTBUS_STATE_OPENED;
    listener_->OnSessionState(DCAMERA_CHANNEL_STATE_CONNECTED);
    return DCAMERA_OK;
}

int32_t DCameraSoftbusSession::OnSessionClose(int32_t sessionId)
{
    DHLOGI("DCameraSoftbusSession OnSessionClose sessionId: %d peerDevId: %s peerSessionName: %s", sessionId,
        GetAnonyString(peerDevId_).c_str(), GetAnonyString(peerSessionName_).c_str());
    sessionId_ = -1;
    state_ = DCAMERA_SOFTBUS_STATE_CLOSED;
    listener_->OnSessionState(DCAMERA_CHANNEL_STATE_DISCONNECTED);
    return DCAMERA_OK;
}

int32_t DCameraSoftbusSession::OnDataReceived(std::shared_ptr<DataBuffer>& buffer)
{
    auto recvDataFunc = [this, buffer]() mutable {
        DealRecvData(buffer);
    };
    if (eventHandler_ != nullptr) {
        eventHandler_->PostTask(recvDataFunc);
    }
    return DCAMERA_OK;
}

void DCameraSoftbusSession::DealRecvData(std::shared_ptr<DataBuffer>& buffer)
{
    if (mode_ == DCAMERA_SESSION_MODE_VIDEO) {
        PostData(buffer);
        return;
    }
    PackRecvData(buffer);
    return;
}

void DCameraSoftbusSession::PackRecvData(std::shared_ptr<DataBuffer>& buffer)
{
    if (buffer->Size() < BINARY_HEADER_FRAG_LEN) {
        DHLOGE("DCameraSoftbusSession PackRecvData failed, size: %d, sess: %s peerSess: %s",
            buffer->Size(), mySessionName_.c_str(), peerSessionName_.c_str());
        return;
    }
    uint8_t *ptrPacket = buffer->Data();
    SessionDataHeader headerPara;
    GetFragDataLen(ptrPacket, headerPara);
    if (buffer->Size() != (headerPara.dataLen + BINARY_HEADER_FRAG_LEN) || headerPara.dataLen > headerPara.totalLen ||
        headerPara.dataLen > BINARY_DATA_MAX_LEN || headerPara.totalLen > BINARY_DATA_MAX_TOTAL_LEN) {
        DHLOGE("DCameraSoftbusSession PackRecvData failed, size: %d, dataLen: %d, totalLen: %d sess: %s peerSess: %s",
            buffer->Size(), headerPara.dataLen, headerPara.totalLen, mySessionName_.c_str(), peerSessionName_.c_str());
        return;
    }

    DHLOGD("DCameraSoftbusSession PackRecvData Assemble, size: %d, dataLen: %d, totalLen: %d sess: %s peerSess: %s",
        buffer->Size(), headerPara.dataLen, headerPara.totalLen, mySessionName_.c_str(), peerSessionName_.c_str());
    if (headerPara.fragFlag == FRAG_START_END) {
        AssembleNoFrag(buffer, headerPara);
    } else {
        AssembleFrag(buffer, headerPara);
    }
}

void DCameraSoftbusSession::AssembleNoFrag(std::shared_ptr<DataBuffer>& buffer, SessionDataHeader& headerPara)
{
    if (headerPara.dataLen != headerPara.totalLen) {
        DHLOGE("DCameraSoftbusSession PackRecvData failed, dataLen: %d, totalLen: %d, sess: %s peerSess: %s",
            headerPara.dataLen, headerPara.totalLen, mySessionName_.c_str(), peerSessionName_.c_str());
        return;
    }
    std::shared_ptr<DataBuffer> postData = std::make_shared<DataBuffer>(headerPara.dataLen);
    int32_t ret = memcpy_s(postData->Data(), postData->Size(), buffer->Data() + BINARY_HEADER_FRAG_LEN,
        buffer->Size() - BINARY_HEADER_FRAG_LEN);
    if (ret != EOK) {
        DHLOGE("DCameraSoftbusSession PackRecvData failed, ret: %d, sess: %s peerSess: %s",
            ret, mySessionName_.c_str(), peerSessionName_.c_str());
        return;
    }
    PostData(postData);
}

void DCameraSoftbusSession::AssembleFrag(std::shared_ptr<DataBuffer>& buffer, SessionDataHeader& headerPara)
{
    if (headerPara.fragFlag == FRAG_START) {
        isWaiting_ = true;
        nowSeq_ = headerPara.seqNum;
        nowSubSeq_ = headerPara.subSeq;
        offset_ = 0;
        totalLen_ = headerPara.totalLen;
        packBuffer_ = std::make_shared<DataBuffer>(headerPara.totalLen);
        int32_t ret = memcpy_s(packBuffer_->Data(), packBuffer_->Size(), buffer->Data() + BINARY_HEADER_FRAG_LEN,
            buffer->Size() - BINARY_HEADER_FRAG_LEN);
        if (ret != EOK) {
            DHLOGE("DCameraSoftbusSession AssembleFrag failed, ret: %d, sess: %s peerSess: %s",
                ret, mySessionName_.c_str(), peerSessionName_.c_str());
            ResetAssembleFrag();
            return;
        }
        offset_ += headerPara.dataLen;
    }

    if (headerPara.fragFlag == FRAG_MID || headerPara.fragFlag == FRAG_END) {
        int32_t ret = CheckUnPackBuffer(headerPara);
        if (ret != DCAMERA_OK) {
            return;
        }

        nowSubSeq_ = headerPara.subSeq;
        ret = memcpy_s(packBuffer_->Data() + offset_, packBuffer_->Size() - offset_,
            buffer->Data() + BINARY_HEADER_FRAG_LEN, buffer->Size() - BINARY_HEADER_FRAG_LEN);
        if (ret != EOK) {
            DHLOGE("DCameraSoftbusSession AssembleFrag failed, memcpy_s ret: %d, sess: %s peerSess: %s",
                ret, mySessionName_.c_str(), peerSessionName_.c_str());
            ResetAssembleFrag();
            return;
        }
        offset_ += headerPara.dataLen;
    }

    if (headerPara.fragFlag == FRAG_END) {
        PostData(packBuffer_);
        ResetAssembleFrag();
    }
}

int32_t DCameraSoftbusSession::CheckUnPackBuffer(SessionDataHeader& headerPara)
{
    if (!isWaiting_) {
        DHLOGE("DCameraSoftbusSession AssembleFrag failed, not start one, sess: %s peerSess: %s",
            mySessionName_.c_str(), peerSessionName_.c_str());
        return DCAMERA_BAD_VALUE;
    }

    if (nowSeq_ != headerPara.seqNum) {
        DHLOGE("DCameraSoftbusSession AssembleFrag seq error nowSeq: %d actualSeq: %d, sess: %s peerSess: %s",
            nowSeq_, headerPara.seqNum, mySessionName_.c_str(), peerSessionName_.c_str());
        return DCAMERA_BAD_VALUE;
    }

    if (nowSubSeq_ + 1 != headerPara.subSeq) {
        DHLOGE("DCameraSoftbusSession AssembleFrag subSeq error nowSeq: %d actualSeq: %d, sess: %s peerSess: %s",
            nowSubSeq_, headerPara.subSeq, mySessionName_.c_str(), peerSessionName_.c_str());
        return DCAMERA_BAD_VALUE;
    }

    if (totalLen_ < headerPara.dataLen + offset_) {
        DHLOGE("DCameraSoftbusSession AssembleFrag len error cap: %d size: %d, dataLen: %d sess: %s peerSess: %s",
            totalLen_, offset_, headerPara.dataLen, mySessionName_.c_str(),
            peerSessionName_.c_str());
        return DCAMERA_BAD_VALUE;
    }
    return DCAMERA_OK;
}

void DCameraSoftbusSession::ResetAssembleFrag()
{
    isWaiting_ = false;
    nowSeq_ = 0;
    nowSubSeq_ = 0;
    offset_ = 0;
    totalLen_ = 0;
    packBuffer_ = nullptr;
}

void DCameraSoftbusSession::PostData(std::shared_ptr<DataBuffer>& buffer)
{
    std::vector<std::shared_ptr<DataBuffer>> buffers;
    buffers.push_back(buffer);
    listener_->OnDataReceived(buffers);
}

void DCameraSoftbusSession::GetFragDataLen(uint8_t *ptrPacket, SessionDataHeader& headerPara)
{
    headerPara.version = U16Get(ptrPacket);
    headerPara.fragFlag = ptrPacket[static_cast<int32_t>(BINARY_HEADER_FRAG_OFFSET)];
    headerPara.dataType = U32Get(ptrPacket + BINARY_HEADER_DATATYPE_OFFSET);
    headerPara.seqNum = U32Get(ptrPacket + BINARY_HEADER_SEQNUM_OFFSET);
    headerPara.totalLen = U32Get(ptrPacket + BINARY_HEADER_TOTALLEN_OFFSET);
    headerPara.subSeq = U16Get(ptrPacket + BINARY_HEADER_SUBSEQ_OFFSET);
    headerPara.dataLen = U32Get(ptrPacket + BINARY_HEADER_DATALEN_OFFSET);
}

uint16_t DCameraSoftbusSession::U16Get(const uint8_t *ptr)
{
    return (ptr[0] << DCAMERA_SHIFT_8) | ptr[1];
}

uint32_t DCameraSoftbusSession::U32Get(const uint8_t *ptr)
{
    return (ptr[0] << DCAMERA_SHIFT_24) | (ptr[1] << DCAMERA_SHIFT_16) | (ptr[2] << DCAMERA_SHIFT_8) | ptr[3];
}

int32_t DCameraSoftbusSession::SendData(DCameraSessionMode mode, std::shared_ptr<DataBuffer>& buffer)
{
    auto itFunc = sendFuncMap_.find(mode);
    if (itFunc == sendFuncMap_.end()) {
        return DCAMERA_NOT_FOUND;
    }
    auto memberFunc = itFunc->second;
    if (mode == DCAMERA_SESSION_MODE_VIDEO) {
        return (this->*memberFunc)(buffer);
    }

    return UnPackSendData(buffer, memberFunc);
}

int32_t DCameraSoftbusSession::UnPackSendData(std::shared_ptr<DataBuffer>& buffer, DCameraSendFuc memberFunc)
{
    uint16_t subSeq = 0;
    uint32_t seq = 0;
    uint32_t totalLen = buffer->Size();
    SessionDataHeader headPara = { PROTOCOL_VERSION, FRAG_START, mode_, seq, totalLen, subSeq };

    if (buffer->Size() <= BINARY_DATA_PACKET_MAX_LEN) {
        headPara.fragFlag = FRAG_START_END;
        headPara.dataLen = buffer->Size();
        std::shared_ptr<DataBuffer> unpackData = std::make_shared<DataBuffer>(buffer->Size() + BINARY_HEADER_FRAG_LEN);
        MakeFragDataHeader(headPara, unpackData->Data(), BINARY_HEADER_FRAG_LEN);
        int32_t ret = memcpy_s(unpackData->Data() + BINARY_HEADER_FRAG_LEN, unpackData->Size() - BINARY_HEADER_FRAG_LEN,
            buffer->Data(), buffer->Size());
        if (ret != EOK) {
            DHLOGE("DCameraSoftbusSession UnPackSendData START_END memcpy_s failed, ret: %d, sess: %s peerSess: %s",
                ret, mySessionName_.c_str(), peerSessionName_.c_str());
            return ret;
        }
        return (this->*memberFunc)(unpackData);
    }

    uint32_t offset = 0;
    while (totalLen > offset) {
        if (totalLen - offset > BINARY_DATA_PACKET_MAX_LEN) {
            headPara.dataLen = BINARY_DATA_PACKET_MAX_LEN;
        } else {
            headPara.fragFlag = FRAG_END;
            headPara.dataLen = totalLen - offset;
        }

        std::shared_ptr<DataBuffer> unpackData =
            std::make_shared<DataBuffer>(headPara.dataLen + BINARY_HEADER_FRAG_LEN);
        MakeFragDataHeader(headPara, unpackData->Data(), BINARY_HEADER_FRAG_LEN);
        int ret = memcpy_s(unpackData->Data() + BINARY_HEADER_FRAG_LEN, unpackData->Size() - BINARY_HEADER_FRAG_LEN,
            buffer->Data() + offset, headPara.dataLen);
        if (ret != EOK) {
            DHLOGE("DCameraSoftbusSession UnPackSendData memcpy_s failed, ret: %d, sess: %s peerSess: %s",
                ret, mySessionName_.c_str(), peerSessionName_.c_str());
            return ret;
        }
        ret = (this->*memberFunc)(unpackData);
        if (ret != DCAMERA_OK) {
            DHLOGE("DCameraSoftbusSession sendData failed, ret: %d, sess: %s peerSess: %s",
                ret, mySessionName_.c_str(), peerSessionName_.c_str());
            return ret;
        }
        headPara.subSeq++;
        headPara.fragFlag = FRAG_MID;
        offset += headPara.dataLen;
    }
    return DCAMERA_OK;
}

void DCameraSoftbusSession::MakeFragDataHeader(const SessionDataHeader& headPara, uint8_t *header, uint32_t len)
{
    uint32_t headerLen = sizeof(uint8_t) * HEADER_UINT8_NUM + sizeof(uint16_t) * HEADER_UINT16_NUM +
        sizeof(uint32_t) * HEADER_UINT32_NUM;
    if (headerLen > len) {
        DHLOGE("MakeFragDataHeader %d over len %d", headerLen, len);
        return;
    }
    uint32_t i = 0;
    header[i++] = headPara.version >> DCAMERA_SHIFT_8;
    header[i++] = headPara.version & UINT16_SHIFT_MASK_0;
    header[i++] = headPara.fragFlag;
    header[i++] = (headPara.dataType & UINT32_SHIFT_MASK_24) >> DCAMERA_SHIFT_24;
    header[i++] = (headPara.dataType & UINT32_SHIFT_MASK_16) >> DCAMERA_SHIFT_16;
    header[i++] = (headPara.dataType & UINT32_SHIFT_MASK_8) >> DCAMERA_SHIFT_8;
    header[i++] = (headPara.dataType & UINT32_SHIFT_MASK_0);
    header[i++] = (headPara.seqNum & UINT32_SHIFT_MASK_24) >> DCAMERA_SHIFT_24;
    header[i++] = (headPara.seqNum & UINT32_SHIFT_MASK_16) >> DCAMERA_SHIFT_16;
    header[i++] = (headPara.seqNum & UINT32_SHIFT_MASK_8) >> DCAMERA_SHIFT_8;
    header[i++] = (headPara.seqNum & UINT32_SHIFT_MASK_0);
    header[i++] = (headPara.totalLen & UINT32_SHIFT_MASK_24) >> DCAMERA_SHIFT_24;
    header[i++] = (headPara.totalLen & UINT32_SHIFT_MASK_16) >> DCAMERA_SHIFT_16;
    header[i++] = (headPara.totalLen & UINT32_SHIFT_MASK_8) >> DCAMERA_SHIFT_8;
    header[i++] = (headPara.totalLen & UINT32_SHIFT_MASK_0);
    header[i++] = headPara.subSeq >> DCAMERA_SHIFT_8;
    header[i++] = headPara.subSeq & UINT16_SHIFT_MASK_0;
    header[i++] = (headPara.dataLen & UINT32_SHIFT_MASK_24) >> DCAMERA_SHIFT_24;
    header[i++] = (headPara.dataLen & UINT32_SHIFT_MASK_16) >> DCAMERA_SHIFT_16;
    header[i++] = (headPara.dataLen & UINT32_SHIFT_MASK_8) >> DCAMERA_SHIFT_8;
    header[i++] = (headPara.dataLen & UINT32_SHIFT_MASK_0);
}

int32_t DCameraSoftbusSession::SendBytes(std::shared_ptr<DataBuffer>& buffer)
{
    if (state_ != DCAMERA_SOFTBUS_STATE_OPENED) {
        DHLOGE("DCameraSoftbusSession SendBytes session state %d is not opened sessionId: %d peerDev: %s peerName: %s",
            state_, sessionId_, GetAnonyString(peerDevId_).c_str(), GetAnonyString(peerSessionName_).c_str());
        return DCAMERA_WRONG_STATE;
    }

    int32_t ret = DCameraSoftbusAdapter::GetInstance().SendSofbusBytes(sessionId_, buffer);
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSoftbusSession SendBytes sessionId: %d failed: %d peerDevId: %s peerSessionName: %s", sessionId_,
            ret, GetAnonyString(peerDevId_).c_str(), GetAnonyString(peerSessionName_).c_str());
    }
    return ret;
}

int32_t DCameraSoftbusSession::SendStream(std::shared_ptr<DataBuffer>& buffer)
{
    if (state_ != DCAMERA_SOFTBUS_STATE_OPENED) {
        DHLOGE("DCameraSoftbusSession SendStream session state %d is not opened sessionId: %d peerDev: %s peerName: %s",
            state_, sessionId_, GetAnonyString(peerDevId_).c_str(), GetAnonyString(peerSessionName_).c_str());
        return DCAMERA_WRONG_STATE;
    }

    int32_t ret = DCameraSoftbusAdapter::GetInstance().SendSofbusStream(sessionId_, buffer);
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSoftbusSession SendStream sessionId: %d failed: %d peerDevId: %s peerSessionName: %s",
            sessionId_, ret, GetAnonyString(peerDevId_).c_str(), GetAnonyString(peerSessionName_).c_str());
    }
    return ret;
}

std::string DCameraSoftbusSession::GetPeerDevId()
{
    return peerDevId_;
}

std::string DCameraSoftbusSession::GetPeerSessionName()
{
    return peerSessionName_;
}

std::string DCameraSoftbusSession::GetMySessionName()
{
    return mySessionName_;
}
} // namespace DistributedHardware
} // namespace OHOS
