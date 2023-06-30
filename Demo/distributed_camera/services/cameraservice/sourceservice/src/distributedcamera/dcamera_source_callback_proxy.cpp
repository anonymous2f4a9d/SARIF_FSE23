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

#include "dcamera_source_callback_proxy.h"

#include "parcel.h"

#include "anonymous_string.h"
#include "distributed_camera_errno.h"
#include "distributed_hardware_log.h"

namespace OHOS {
namespace DistributedHardware {
int32_t DCameraSourceCallbackProxy::OnNotifyRegResult(const std::string& devId, const std::string& dhId,
    const std::string& reqId, int32_t status, std::string& data)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        DHLOGE("DCameraSourceCallbackProxy remote service null");
        return DCAMERA_BAD_VALUE;
    }
    MessageParcel req;
    MessageParcel reply;
    MessageOption option;
    if (!req.WriteInterfaceToken(DCameraSourceCallbackProxy::GetDescriptor())) {
        DHLOGE("DCameraSourceCallbackProxy OnNotifyRegResult write token failed");
        return DCAMERA_BAD_VALUE;
    }

    if (!req.WriteString(devId) || !req.WriteString(dhId) || !req.WriteString(reqId) ||
        !req.WriteInt32(status) || !req.WriteString(data)) {
        DHLOGE("DistributedCameraSourceProxy InitSource write params failed");
        return DCAMERA_BAD_VALUE;
    }
    remote->SendRequest(NOTIFY_REG_RESULT, req, reply, option);
    int32_t result = reply.ReadInt32();
    return result;
}

int32_t DCameraSourceCallbackProxy::OnNotifyUnregResult(const std::string& devId, const std::string& dhId,
    const std::string& reqId, int32_t status, std::string& data)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        DHLOGE("DCameraSourceCallbackProxy remote service null");
        return DCAMERA_BAD_VALUE;
    }
    MessageParcel req;
    MessageParcel reply;
    MessageOption option;
    if (!req.WriteInterfaceToken(DCameraSourceCallbackProxy::GetDescriptor())) {
        DHLOGE("DCameraSourceCallbackProxy OnNotifyRegResult write token failed");
        return DCAMERA_BAD_VALUE;
    }

    if (!req.WriteString(devId) || !req.WriteString(dhId) || !req.WriteString(reqId) ||
        !req.WriteInt32(status) || !req.WriteString(data)) {
        DHLOGE("DistributedCameraSourceProxy InitSource write params failed");
        return DCAMERA_BAD_VALUE;
    }
    remote->SendRequest(NOTIFY_UNREG_RESULT, req, reply, option);
    int32_t result = reply.ReadInt32();
    return result;
}
} // namespace DistributedHardware
} // namespace OHOS
