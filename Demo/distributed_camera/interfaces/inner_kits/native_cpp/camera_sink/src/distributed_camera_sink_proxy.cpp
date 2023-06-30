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

#include "distributed_camera_sink_proxy.h"

#include "parcel.h"

#include "anonymous_string.h"
#include "distributed_camera_errno.h"
#include "distributed_hardware_log.h"

namespace OHOS {
namespace DistributedHardware {
int32_t DistributedCameraSinkProxy::InitSink(const std::string& params)
{
    DHLOGI("DistributedCameraSinkProxy::InitSink");
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        DHLOGE("DistributedCameraSinkProxy::InitSink remote service is null");
        return DCAMERA_BAD_VALUE;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(DistributedCameraSinkProxy::GetDescriptor())) {
        DHLOGE("DistributedCameraSinkProxy::InitSink write token failed");
        return DCAMERA_BAD_VALUE;
    }
    if (!data.WriteString(params)) {
        DHLOGE("DistributedCameraSinkProxy::InitSink write params failed");
        return DCAMERA_BAD_VALUE;
    }
    remote->SendRequest(INIT_SINK, data, reply, option);
    int32_t result = reply.ReadInt32();
    return result;
}

int32_t DistributedCameraSinkProxy::ReleaseSink()
{
    DHLOGI("DistributedCameraSinkProxy::ReleaseSink");
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        DHLOGE("DistributedCameraSinkProxy::ReleaseSink remote service is null");
        return DCAMERA_BAD_VALUE;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(DistributedCameraSinkProxy::GetDescriptor())) {
        DHLOGE("DistributedCameraSinkProxy::ReleaseSink write token failed");
        return DCAMERA_BAD_VALUE;
    }
    remote->SendRequest(RELEASE_SINK, data, reply, option);
    int32_t result = reply.ReadInt32();
    return result;
}

int32_t DistributedCameraSinkProxy::SubscribeLocalHardware(const std::string& dhId, const std::string& parameters)
{
    DHLOGI("DistributedCameraSinkProxy::SubscribeLocalHardware dhId: %s", GetAnonyString(dhId).c_str());
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        DHLOGE("DistributedCameraSinkProxy::SubscribeLocalHardware remote service is null");
        return DCAMERA_BAD_VALUE;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(DistributedCameraSinkProxy::GetDescriptor())) {
        DHLOGE("DistributedCameraSinkProxy::SubscribeLocalHardware write token failed");
        return DCAMERA_BAD_VALUE;
    }
    if (!data.WriteString(dhId) || !data.WriteString(parameters)) {
        DHLOGE("DistributedCameraSinkProxy::SubscribeLocalHardware write params failed");
        return DCAMERA_BAD_VALUE;
    }
    remote->SendRequest(SUBSCRIBE_LOCAL_HARDWARE, data, reply, option);
    int32_t result = reply.ReadInt32();
    return result;
}

int32_t DistributedCameraSinkProxy::UnsubscribeLocalHardware(const std::string& dhId)
{
    DHLOGI("DistributedCameraSinkProxy::UnsubscribeLocalHardware dhId: %s", GetAnonyString(dhId).c_str());
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        DHLOGE("DistributedCameraSinkProxy::UnsubscribeLocalHardware remote service is null");
        return DCAMERA_BAD_VALUE;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(DistributedCameraSinkProxy::GetDescriptor())) {
        DHLOGE("DistributedCameraSinkProxy::UnsubscribeLocalHardware write token failed");
        return DCAMERA_BAD_VALUE;
    }
    if (!data.WriteString(dhId)) {
        DHLOGE("DistributedCameraSinkProxy::UnsubscribeLocalHardware write params failed");
        return DCAMERA_BAD_VALUE;
    }
    remote->SendRequest(UNSUBSCRIBE_LOCAL_HARDWARE, data, reply, option);
    int32_t result = reply.ReadInt32();
    return result;
}

int32_t DistributedCameraSinkProxy::StopCapture(const std::string& dhId)
{
    DHLOGI("DistributedCameraSinkProxy::StopCapture dhId: %s", GetAnonyString(dhId).c_str());
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        DHLOGE("DistributedCameraSinkProxy::StopCapture remote service is null");
        return DCAMERA_BAD_VALUE;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option = { MessageOption::TF_ASYNC };
    if (!data.WriteInterfaceToken(DistributedCameraSinkProxy::GetDescriptor())) {
        DHLOGE("DistributedCameraSinkProxy::StopCapture write token failed");
        return DCAMERA_BAD_VALUE;
    }
    if (!data.WriteString(dhId)) {
        DHLOGE("DistributedCameraSinkProxy::StopCapture write params failed");
        return DCAMERA_BAD_VALUE;
    }
    remote->SendRequest(STOP_CAPTURE, data, reply, option);
    int32_t result = reply.ReadInt32();
    DHLOGI("DistributedCameraSinkProxy::StopCapture async dhId: %s", GetAnonyString(dhId).c_str());
    return result;
}

int32_t DistributedCameraSinkProxy::ChannelNeg(const std::string& dhId, std::string& channelInfo)
{
    DHLOGI("DistributedCameraSinkProxy::ChannelNeg dhId: %s", GetAnonyString(dhId).c_str());
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        DHLOGE("DistributedCameraSinkProxy::ChannelNeg remote service is null");
        return DCAMERA_BAD_VALUE;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(DistributedCameraSinkProxy::GetDescriptor())) {
        DHLOGE("DistributedCameraSinkProxy::ChannelNeg write token failed");
        return DCAMERA_BAD_VALUE;
    }
    if (!data.WriteString(dhId) || !data.WriteString(channelInfo)) {
        DHLOGE("DistributedCameraSinkProxy::ChannelNeg write params failed");
        return DCAMERA_BAD_VALUE;
    }
    remote->SendRequest(CHANNEL_NEG, data, reply, option);
    int32_t result = reply.ReadInt32();
    return result;
}

int32_t DistributedCameraSinkProxy::GetCameraInfo(const std::string& dhId, std::string& cameraInfo)
{
    DHLOGI("DistributedCameraSinkProxy::GetCameraInfo dhId: %s", GetAnonyString(dhId).c_str());
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        DHLOGE("DistributedCameraSinkProxy::GetCameraInfo remote service is null");
        return DCAMERA_BAD_VALUE;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(DistributedCameraSinkProxy::GetDescriptor())) {
        DHLOGE("DistributedCameraSinkProxy::GetCameraInfo write token failed");
        return DCAMERA_BAD_VALUE;
    }
    if (!data.WriteString(dhId) || !data.WriteString(cameraInfo)) {
        DHLOGE("DistributedCameraSinkProxy::GetCameraInfo write params failed");
        return DCAMERA_BAD_VALUE;
    }
    remote->SendRequest(GET_CAMERA_INFO, data, reply, option);
    int32_t result = reply.ReadInt32();
    return result;
}

int32_t DistributedCameraSinkProxy::OpenChannel(const std::string& dhId, std::string& openInfo)
{
    DHLOGI("DistributedCameraSinkProxy::OpenChannel dhId: %s", GetAnonyString(dhId).c_str());
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        DHLOGE("DistributedCameraSinkProxy::OpenChannel remote service is null");
        return DCAMERA_BAD_VALUE;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(DistributedCameraSinkProxy::GetDescriptor())) {
        DHLOGE("DistributedCameraSinkProxy::OpenChannel write token failed");
        return DCAMERA_BAD_VALUE;
    }
    if (!data.WriteString(dhId) || !data.WriteString(openInfo)) {
        DHLOGE("DistributedCameraSinkProxy::OpenChannel write params failed");
        return DCAMERA_BAD_VALUE;
    }
    remote->SendRequest(OPEN_CHANNEL, data, reply, option);
    int32_t result = reply.ReadInt32();
    return result;
}

int32_t DistributedCameraSinkProxy::CloseChannel(const std::string& dhId)
{
    DHLOGI("DistributedCameraSinkProxy::CloseChannel dhId: %s", GetAnonyString(dhId).c_str());
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        DHLOGE("DistributedCameraSinkProxy::CloseChannel remote service is null");
        return DCAMERA_BAD_VALUE;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(DistributedCameraSinkProxy::GetDescriptor())) {
        DHLOGE("DistributedCameraSinkProxy::CloseChannel write token failed");
        return DCAMERA_BAD_VALUE;
    }
    if (!data.WriteString(dhId)) {
        DHLOGE("DistributedCameraSinkProxy::CloseChannel write params failed");
        return DCAMERA_BAD_VALUE;
    }
    remote->SendRequest(CLOSE_CHANNEL, data, reply, option);
    int32_t result = reply.ReadInt32();
    return result;
}
} // namespace DistributedHardware
} // namespace OHOS