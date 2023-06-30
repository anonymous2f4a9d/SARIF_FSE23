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

#ifndef DISTRIBUTED_CAMERA_PROVIDER_CALLBACK_CLIENT_STUB_H
#define DISTRIBUTED_CAMERA_PROVIDER_CALLBACK_CLIENT_STUB_H

#include "idistributed_camera_provider_callback.h"
#include "iremote_stub.h"
#include "message_parcel.h"
#include "parcel.h"

namespace OHOS {
namespace DistributedHardware {
class DCameraProviderCallbackStub : public IRemoteStub<IDCameraProviderCallback> {
public:
    virtual ~DCameraProviderCallbackStub() = default;

    int32_t OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
                            MessageOption &option) override;

private:
    int32_t DCProviderOpenSessionStub(MessageParcel &data, MessageParcel &reply, MessageOption &option);
    int32_t DCProviderCloseSessionStub(MessageParcel &data, MessageParcel &reply, MessageOption &option);
    int32_t DCProviderConfigureStreamsStub(MessageParcel &data, MessageParcel &reply, MessageOption &option);
    int32_t DCProviderReleaseStreamsStub(MessageParcel &data, MessageParcel &reply, MessageOption &option);
    int32_t DCProviderStartCaptureStub(MessageParcel &data, MessageParcel &reply, MessageOption &option);
    int32_t DCProviderStopCaptureStub(MessageParcel &data, MessageParcel &reply, MessageOption &option);
    int32_t DCProviderUpdateSettingsStub(MessageParcel &data, MessageParcel &reply, MessageOption &option);
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // DISTRIBUTED_CAMERA_PROVIDER_CALLBACK_CLIENT_STUB_H
