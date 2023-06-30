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

#ifndef DISTRIBUTED_CAMERA_HOST_SERVER_STUB_H
#define DISTRIBUTED_CAMERA_HOST_SERVER_STUB_H

#include <hdf_sbuf_ipc.h>
#include <message_option.h>
#include <message_parcel.h>
#include <refbase.h>
#include "dcamera_host.h"
#include "icamera_device_callback.h"
#include "icamera_host_callback.h"

namespace OHOS {
namespace DistributedHardware {
using namespace OHOS::Camera;
enum {
    CMD_CAMERA_HOST_SET_CALLBACK = 0,
    CMD_CAMERA_HOST_GET_CAMERAID,
    CMD_CAMERA_HOST_GET_CAMERA_ABILITY,
    CMD_CAMERA_HOST_OPEN_CAMERA,
    CMD_CAMERA_HOST_SET_FLASH_LIGHT,
};

class DCameraHostStub {
public:
    DCameraHostStub();
    virtual ~DCameraHostStub() {}
    int32_t Init();
    int32_t OnRemoteRequest(int cmdId, MessageParcel& data, MessageParcel& reply, MessageOption& option);

private:
    int32_t DCHostStubSetCallback(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    int32_t DCHostStubGetCameraIds(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    int32_t DCHostStubGetCameraAbility(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    int32_t DCHostStubOpenCamera(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    int32_t DCHostStubSetFlashlight(MessageParcel& data, MessageParcel& reply, MessageOption& option);
    static inline const std::u16string metaDescriptor_ = u"HDI.Camera.V1_0.Host";
    static inline const std::u16string &GetDescriptor()
    {
        return metaDescriptor_;
    }

private:
    std::shared_ptr<DCameraHost> dcameraHost_ = nullptr;
};
} // namespace DistributedHardware
} // namespace OHOS

void* DCameraHostStubInstance();

void DestroyDCameraHostStub(void* stubObj);

int32_t DCHostServiceOnRemoteRequest(void* stub, int cmdId, struct HdfSBuf* data, struct HdfSBuf* reply);

#endif // DISTRIBUTED_CAMERA_HOST_SERVER_STUB_H