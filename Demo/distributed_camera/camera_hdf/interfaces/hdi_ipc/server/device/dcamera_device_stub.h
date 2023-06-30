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

#ifndef DISTRIBUTED_CAMERA_DEVICE_SERVER_STUB_H
#define DISTRIBUTED_CAMERA_DEVICE_SERVER_STUB_H

#include <message_option.h>
#include <message_parcel.h>
#include <refbase.h>
#include "icamera_device.h"
#include "iremote_stub.h"

namespace OHOS {
namespace DistributedHardware {
using namespace OHOS::Camera;
class DCameraDeviceStub : public IRemoteStub<ICameraDevice> {
public:
    int32_t OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply,
                            MessageOption& option) override;

private:
    int32_t DCDeviceStubGetStreamOperator(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    int32_t DCDeviceStubUpdateSettings(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    int32_t DCDeviceStubSetResultMode(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    int32_t DCDeviceStubGetEnabledReuslts(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    int32_t DCDeviceStubEnableResult(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    int32_t DCDeviceStubDisableResult(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    int32_t DCDeviceStubClose(MessageParcel& data, MessageParcel& reply, MessageOption& option);
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // DISTRIBUTED_CAMERA_DEVICE_SERVER_STUB_H