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

#ifndef DISTRIBUTED_CAMERA_PROVIDER_SERVER_STUB_H
#define DISTRIBUTED_CAMERA_PROVIDER_SERVER_STUB_H

#include <message_option.h>
#include <message_parcel.h>
#include <refbase.h>
#include "dcamera_provider.h"
#include "iremote_stub.h"

namespace OHOS {
namespace DistributedHardware {
class DCameraProviderStub {
public:
    DCameraProviderStub();
    virtual ~DCameraProviderStub() {}
    int32_t Init();
    int32_t OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply,
                            MessageOption& option);

private:
    int32_t DCProviderStubEnableDCameraDevice(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    int32_t DCProviderStubDisableDCameraDevice(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    int32_t DCProviderStubAcquireBuffer(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    int32_t DCProviderStubShutterBuffer(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    int32_t DCProviderStubOnSettingsResult(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    int32_t DCProviderStubNotify(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    static inline const std::u16string metaDescriptor_ = u"HDI.DCamera.V1_0.Provider";
    static inline const std::u16string &GetDescriptor()
    {
        return metaDescriptor_;
    }

private:
    std::shared_ptr<DCameraProvider> dcameraProvider_ = nullptr;
};
} // namespace DistributedHardware
} // namespace OHOS

void* DCameraProviderStubInstance();

void DestroyDCameraProviderStub(void* stubObj);

int32_t DCProviderServiceOnRemoteRequest(void* stub, int cmdId, struct HdfSBuf* data, struct HdfSBuf* reply);

#endif // DISTRIBUTED_CAMERA_PROVIDER_SERVER_STUB_H