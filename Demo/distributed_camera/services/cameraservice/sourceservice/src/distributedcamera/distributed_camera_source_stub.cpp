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

#include "distributed_camera_source_stub.h"

#include "dcamera_source_callback_proxy.h"
#include "distributed_camera_errno.h"
#include "distributed_hardware_log.h"

namespace OHOS {
namespace DistributedHardware {
DistributedCameraSourceStub::DistributedCameraSourceStub()
{
    memberFuncMap_[INIT_SOURCE] = &DistributedCameraSourceStub::InitSourceInner;
    memberFuncMap_[RELEASE_SOURCE] = &DistributedCameraSourceStub::ReleaseSourceInner;
    memberFuncMap_[REGISTER_DISTRIBUTED_HARDWARE] = &DistributedCameraSourceStub::RegisterDistributedHardwareInner;
    memberFuncMap_[UNREGISTER_DISTRIBUTED_HARDWARE] = &DistributedCameraSourceStub::UnregisterDistributedHardwareInner;
    memberFuncMap_[CAMERA_NOTIFY] = &DistributedCameraSourceStub::DCameraNotifyInner;
}

DistributedCameraSourceStub::~DistributedCameraSourceStub()
{}

int32_t DistributedCameraSourceStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    DHLOGI("DistributedCameraSourceStub OnRemoteRequest code: %d", code);
    std::u16string desc = DistributedCameraSourceStub::GetDescriptor();
    std::u16string remoteDesc = data.ReadInterfaceToken();
    if (desc != remoteDesc) {
        DHLOGE("DistributedCameraSourceStub::OnRemoteRequest remoteDesc is invalid!");
        return ERR_INVALID_DATA;
    }
    auto itFunc = memberFuncMap_.find(code);
    if (itFunc == memberFuncMap_.end()) {
        return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }

    auto memberFunc = itFunc->second;
    return (this->*memberFunc)(data, reply);
}

int32_t DistributedCameraSourceStub::InitSourceInner(MessageParcel &data, MessageParcel &reply)
{
    DHLOGI("DistributedCameraSourceStub InitSourceInner");
    int32_t ret = DCAMERA_OK;
    do {
        std::string params = data.ReadString();
        sptr<IRemoteObject> remoteObj = data.ReadRemoteObject();
        if (remoteObj == nullptr) {
            DHLOGE("DistributedCameraSourceStub initSource read object failed");
            ret = DCAMERA_BAD_VALUE;
            break;
        }

        sptr<DCameraSourceCallbackProxy> callbackProxy(new DCameraSourceCallbackProxy(remoteObj));
        if (callbackProxy == nullptr) {
            DHLOGE("DistributedCameraSourceStub initSource get proxy failed");
            ret = DCAMERA_BAD_VALUE;
            break;
        }

        ret = InitSource(params, callbackProxy);
    } while (0);
    reply.WriteInt32(ret);
    return ret;
}

int32_t DistributedCameraSourceStub::ReleaseSourceInner(MessageParcel &data, MessageParcel &reply)
{
    DHLOGI("DistributedCameraSourceStub ReleaseSourceInner");
    (void)data;
    int32_t ret = ReleaseSource();
    reply.WriteInt32(ret);
    return ret;
}

int32_t DistributedCameraSourceStub::RegisterDistributedHardwareInner(MessageParcel &data, MessageParcel &reply)
{
    DHLOGI("DistributedCameraSourceStub RegisterDistributedHardwareInner");
    int32_t ret = DCAMERA_OK;
    do {
        std::string devId = data.ReadString();
        std::string dhId = data.ReadString();
        std::string reqId = data.ReadString();
        EnableParam params;
        params.version = data.ReadString();
        params.attrs = data.ReadString();
        ret = RegisterDistributedHardware(devId, dhId, reqId, params);
        DHLOGI("DistributedCameraSourceStub RegisterDistributedHardware %d", ret);
    } while (0);
    reply.WriteInt32(ret);
    return ret;
}

int32_t DistributedCameraSourceStub::UnregisterDistributedHardwareInner(MessageParcel &data, MessageParcel &reply)
{
    DHLOGI("DistributedCameraSourceStub UnregisterDistributedHardwareInner");
    int32_t ret = DCAMERA_OK;
    do {
        std::string devId = data.ReadString();
        std::string dhId = data.ReadString();
        std::string reqId = data.ReadString();
        ret = UnregisterDistributedHardware(devId, dhId, reqId);
    } while (0);
    reply.WriteInt32(ret);
    return ret;
}

int32_t DistributedCameraSourceStub::DCameraNotifyInner(MessageParcel &data, MessageParcel &reply)
{
    int32_t ret = DCAMERA_OK;
    do {
        std::string devId = data.ReadString();
        std::string dhId = data.ReadString();
        std::string events = data.ReadString();
        ret = DCameraNotify(devId, dhId, events);
    } while (0);
    reply.WriteInt32(ret);
    return ret;
}
} // namespace DistributedHardware
} // namespace OHOS
