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

#include "distributed_camera_sink_stub.h"
#include "distributed_camera_errno.h"
#include "distributed_hardware_log.h"

namespace OHOS {
namespace DistributedHardware {
DistributedCameraSinkStub::DistributedCameraSinkStub()
{
    memberFuncMap_[INIT_SINK] = &DistributedCameraSinkStub::InitSinkInner;
    memberFuncMap_[RELEASE_SINK] = &DistributedCameraSinkStub::ReleaseSinkInner;
    memberFuncMap_[SUBSCRIBE_LOCAL_HARDWARE] = &DistributedCameraSinkStub::SubscribeLocalHardwareInner;
    memberFuncMap_[UNSUBSCRIBE_LOCAL_HARDWARE] = &DistributedCameraSinkStub::UnsubscribeLocalHardwareInner;
    memberFuncMap_[STOP_CAPTURE] = &DistributedCameraSinkStub::StopCaptureInner;
    memberFuncMap_[CHANNEL_NEG] = &DistributedCameraSinkStub::ChannelNegInner;
    memberFuncMap_[GET_CAMERA_INFO] = &DistributedCameraSinkStub::GetCameraInfoInner;
    memberFuncMap_[OPEN_CHANNEL] = &DistributedCameraSinkStub::OpenChannelInner;
    memberFuncMap_[CLOSE_CHANNEL] = &DistributedCameraSinkStub::CloseChannelInner;
}

DistributedCameraSinkStub::~DistributedCameraSinkStub()
{}

int32_t DistributedCameraSinkStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    DHLOGI("DistributedCameraSinkStub::OnRemoteRequest code: %d", code);
    std::u16string desc = DistributedCameraSinkStub::GetDescriptor();
    std::u16string remoteDesc = data.ReadInterfaceToken();
    if (desc != remoteDesc) {
        DHLOGE("DistributedCameraSinkStub::OnRemoteRequest remoteDesc is invalid!");
        return ERR_INVALID_DATA;
    }
    auto itFunc = memberFuncMap_.find(code);
    if (itFunc == memberFuncMap_.end()) {
        return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }

    auto memberFunc = itFunc->second;
    return (this->*memberFunc)(data, reply);
}

int32_t DistributedCameraSinkStub::InitSinkInner(MessageParcel &data, MessageParcel &reply)
{
    DHLOGI("DistributedCameraSinkStub::InitSinkInner");
    std::string params = data.ReadString();
    int32_t ret = InitSink(params);
    reply.WriteInt32(ret);
    return ret;
}

int32_t DistributedCameraSinkStub::ReleaseSinkInner(MessageParcel &data, MessageParcel &reply)
{
    DHLOGI("DistributedCameraSinkStub::ReleaseSinkInner");
    int32_t ret = ReleaseSink();
    reply.WriteInt32(ret);
    return ret;
}

int32_t DistributedCameraSinkStub::SubscribeLocalHardwareInner(MessageParcel &data, MessageParcel &reply)
{
    DHLOGI("DistributedCameraSinkStub::SubscribeLocalHardwareInner");
    std::string dhId = data.ReadString();
    std::string parameters = data.ReadString();
    int32_t ret = SubscribeLocalHardware(dhId, parameters);
    reply.WriteInt32(ret);
    return ret;
}

int32_t DistributedCameraSinkStub::UnsubscribeLocalHardwareInner(MessageParcel &data, MessageParcel &reply)
{
    DHLOGI("DistributedCameraSinkStub::UnsubscribeLocalHardwareInner");
    std::string dhId = data.ReadString();
    int32_t ret = UnsubscribeLocalHardware(dhId);
    reply.WriteInt32(ret);
    return ret;
}

int32_t DistributedCameraSinkStub::StopCaptureInner(MessageParcel &data, MessageParcel &reply)
{
    DHLOGI("DistributedCameraSinkStub::StopCaptureInner");
    std::string dhId = data.ReadString();
    int32_t ret = StopCapture(dhId);
    reply.WriteInt32(ret);
    return ret;
}

int32_t DistributedCameraSinkStub::ChannelNegInner(MessageParcel &data, MessageParcel &reply)
{
    DHLOGI("DistributedCameraSinkStub::ChannelNegInner");
    std::string dhId = data.ReadString();
    std::string channelInfo = data.ReadString();
    int32_t ret = ChannelNeg(dhId, channelInfo);
    reply.WriteInt32(ret);
    return ret;
}

int32_t DistributedCameraSinkStub::GetCameraInfoInner(MessageParcel &data, MessageParcel &reply)
{
    DHLOGI("DistributedCameraSinkStub::GetCameraInfoInner");
    std::string dhId = data.ReadString();
    std::string cameraInfo = data.ReadString();
    int32_t ret = GetCameraInfo(dhId, cameraInfo);
    reply.WriteInt32(ret);
    return ret;
}

int32_t DistributedCameraSinkStub::OpenChannelInner(MessageParcel &data, MessageParcel &reply)
{
    DHLOGI("DistributedCameraSinkStub::OpenChannelInner");
    std::string dhId = data.ReadString();
    std::string openInfo = data.ReadString();
    int32_t ret = OpenChannel(dhId, openInfo);
    reply.WriteInt32(ret);
    return ret;
}

int32_t DistributedCameraSinkStub::CloseChannelInner(MessageParcel &data, MessageParcel &reply)
{
    DHLOGI("DistributedCameraSinkStub::CloseChannelInner");
    std::string dhId = data.ReadString();
    int32_t ret = CloseChannel(dhId);
    reply.WriteInt32(ret);
    return ret;
}
} // namespace DistributedHardware
} // namespace OHOS