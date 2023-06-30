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

#include "distributed_camera_source_proxy.h"

#include "parcel.h"

#include "anonymous_string.h"
#include "distributed_camera_errno.h"
#include "distributed_hardware_log.h"

namespace OHOS {
namespace DistributedHardware {
int32_t DistributedCameraSourceProxy::InitSource(const std::string& params,
    const sptr<IDCameraSourceCallback>& callback)
{
    DHLOGI("DistributedCameraSourceProxy InitSource");
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        DHLOGE("DistributedCameraSourceProxy remote service null");
        return DCAMERA_BAD_VALUE;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(DistributedCameraSourceProxy::GetDescriptor())) {
        DHLOGE("DistributedCameraSourceProxy InitSource write token failed");
        return DCAMERA_BAD_VALUE;
    }

    if (!data.WriteString(params)) {
        DHLOGE("DistributedCameraSourceProxy InitSource write params failed");
        return DCAMERA_BAD_VALUE;
    }

    if (!data.WriteRemoteObject(callback->AsObject())) {
        DHLOGE("DistributedCameraSourceProxy InitSource write callback failed");
        return DCAMERA_BAD_VALUE;
    }

    remote->SendRequest(INIT_SOURCE, data, reply, option);
    int32_t result = reply.ReadInt32();
    return result;
}

int32_t DistributedCameraSourceProxy::ReleaseSource()
{
    DHLOGI("DistributedCameraSourceProxy ReleaseSource");
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        DHLOGE("DistributedCameraSourceProxy remote service null");
        return DCAMERA_BAD_VALUE;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(DistributedCameraSourceProxy::GetDescriptor())) {
        DHLOGE("DistributedCameraSourceProxy InitSource write token failed");
        return DCAMERA_BAD_VALUE;
    }
    remote->SendRequest(RELEASE_SOURCE, data, reply, option);
    int32_t result = reply.ReadInt32();
    return result;
}

int32_t DistributedCameraSourceProxy::RegisterDistributedHardware(const std::string& devId, const std::string& dhId,
    const std::string& reqId, const EnableParam& param)
{
    DHLOGI("DistributedCameraSourceProxy RegisterDistributedHardware devId: %s dhId: %s",
        GetAnonyString(devId).c_str(), GetAnonyString(dhId).c_str());
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        DHLOGE("DistributedCameraSourceProxy remote service null");
        return DCAMERA_BAD_VALUE;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(DistributedCameraSourceProxy::GetDescriptor())) {
        DHLOGE("DistributedCameraSourceProxy RegisterDistributedHardware write token failed");
        return DCAMERA_BAD_VALUE;
    }

    if (!data.WriteString(devId) || !data.WriteString(dhId) || !data.WriteString(reqId) ||
        !data.WriteString(param.version) || !data.WriteString(param.attrs)) {
        DHLOGE("DistributedCameraSourceProxy RegisterDistributedHardware write params failed");
        return DCAMERA_BAD_VALUE;
    }
    remote->SendRequest(REGISTER_DISTRIBUTED_HARDWARE, data, reply, option);
    int32_t result = reply.ReadInt32();
    return result;
}

int32_t DistributedCameraSourceProxy::UnregisterDistributedHardware(const std::string& devId, const std::string& dhId,
    const std::string& reqId)
{
    DHLOGI("DistributedCameraSourceProxy UnregisterDistributedHardware devId: %s dhId: %s",
        GetAnonyString(devId).c_str(), GetAnonyString(dhId).c_str());
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        DHLOGE("DistributedCameraSourceProxy remote service null");
        return DCAMERA_BAD_VALUE;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(DistributedCameraSourceProxy::GetDescriptor())) {
        DHLOGE("DistributedCameraSourceProxy UnregisterDistributedHardware write token failed");
        return DCAMERA_BAD_VALUE;
    }

    if (!data.WriteString(devId) || !data.WriteString(dhId) || !data.WriteString(reqId)) {
        DHLOGE("DistributedCameraSourceProxy UnregisterDistributedHardware write params failed");
        return DCAMERA_BAD_VALUE;
    }
    remote->SendRequest(UNREGISTER_DISTRIBUTED_HARDWARE, data, reply, option);
    int32_t result = reply.ReadInt32();
    return result;
}

int32_t DistributedCameraSourceProxy::DCameraNotify(const std::string& devId, const std::string& dhId,
    std::string& events)
{
    DHLOGI("DCameraNotify devId: %s dhId: %s events: %s",
        GetAnonyString(devId).c_str(), GetAnonyString(dhId).c_str(), events.c_str());
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        DHLOGE("DistributedCameraSourceProxy remote service null");
        return DCAMERA_BAD_VALUE;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(DistributedCameraSourceProxy::GetDescriptor())) {
        DHLOGE("DistributedCameraSourceProxy DCameraNotify write token failed");
        return DCAMERA_BAD_VALUE;
    }

    if (!data.WriteString(devId) || !data.WriteString(dhId) || !data.WriteString(events)) {
        DHLOGE("DistributedCameraSourceProxy DCameraNotify write params failed");
        return DCAMERA_BAD_VALUE;
    }
    remote->SendRequest(CAMERA_NOTIFY, data, reply, option);
    int32_t result = reply.ReadInt32();
    return result;
}
} // namespace DistributedHardware
} // namespace OHOS
