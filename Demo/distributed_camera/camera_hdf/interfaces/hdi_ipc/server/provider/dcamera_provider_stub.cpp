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

#include "dcamera_provider_stub.h"

#include <buffer_handle_parcel.h>
#include <hdf_base.h>
#include <hdf_log.h>
#include <hdf_sbuf_ipc.h>
#include "distributed_hardware_log.h"
#include "idistributed_camera_provider.h"
#include "ipc_data_utils.h"

namespace OHOS {
namespace DistributedHardware {
DCameraProviderStub::DCameraProviderStub()
{
}

int32_t DCameraProviderStub::Init()
{
    dcameraProvider_ = DCameraProvider::GetInstance();
    if (dcameraProvider_ == nullptr) {
        DHLOGE("Get distributed camera provider instance failed.");
        return HDF_FAILURE;
    }
    DHLOGI("Get distributed camera provider instance success.");
    return HDF_SUCCESS;
}

int32_t DCameraProviderStub::DCProviderStubEnableDCameraDevice(MessageParcel& data, MessageParcel& reply,
    MessageOption& option)
{
    if (data.ReadInterfaceToken() != DCameraProviderStub::GetDescriptor()) {
        DHLOGE("invalid token.");
        return HDF_FAILURE;
    }

    std::shared_ptr<DHBase> dhBase = std::make_shared<DHBase>(data.ReadString(), data.ReadString());

    const std::string abilitySet = data.ReadString();
    if (abilitySet.empty()) {
        DHLOGE("Read input param is empty.");
        return HDF_ERR_INVALID_PARAM;
    }

    sptr<IDCameraProviderCallback> dcProviderCallback = nullptr;
    bool flag = data.ReadBool();
    if (flag) {
        sptr<IRemoteObject> remoteObj = data.ReadRemoteObject();
        if (remoteObj == nullptr) {
            DHLOGE("Read distributed camera provider callback failed.");
            return HDF_FAILURE;
        }
        dcProviderCallback = OHOS::iface_cast<IDCameraProviderCallback>(remoteObj);
    }

    DCamRetCode ret = dcameraProvider_->EnableDCameraDevice(dhBase, abilitySet, dcProviderCallback);
    if (!reply.WriteInt32(static_cast<int32_t>(ret))) {
        DHLOGE("Write retcode failed.");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t DCameraProviderStub::DCProviderStubDisableDCameraDevice(MessageParcel& data, MessageParcel& reply,
    MessageOption& option)
{
    if (data.ReadInterfaceToken() != DCameraProviderStub::GetDescriptor()) {
        DHLOGE("invalid token.");
        return HDF_FAILURE;
    }

    std::shared_ptr<DHBase> dhBase = std::make_shared<DHBase>(data.ReadString(), data.ReadString());

    DCamRetCode ret = dcameraProvider_->DisableDCameraDevice(dhBase);
    if (!reply.WriteInt32(static_cast<int32_t>(ret))) {
        DHLOGE("Write retcode failed.");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t DCameraProviderStub::DCProviderStubAcquireBuffer(MessageParcel& data, MessageParcel& reply,
    MessageOption& option)
{
    if (data.ReadInterfaceToken() != DCameraProviderStub::GetDescriptor()) {
        DHLOGE("invalid token.");
        return HDF_FAILURE;
    }

    std::shared_ptr<DHBase> dhBase = std::make_shared<DHBase>(data.ReadString(), data.ReadString());
    int32_t streamId = data.ReadInt32();

    std::shared_ptr<DCameraBuffer> dCamerabuffer = std::make_shared<DCameraBuffer>();
    DCamRetCode ret = dcameraProvider_->AcquireBuffer(dhBase, streamId, dCamerabuffer);
    if (!reply.WriteInt32(static_cast<int32_t>(ret))) {
        DHLOGE("Write retcode failed.");
        return HDF_FAILURE;
    }
    if (ret != DCamRetCode::SUCCESS) {
        DHLOGE("Acquire avaliable buffer failed.");
        return HDF_SUCCESS;
    }
    if (!reply.WriteInt32(dCamerabuffer->index_) || !reply.WriteInt32(dCamerabuffer->size_)) {
        DHLOGE("write buffer index and size parameter failed.");
        return HDF_FAILURE;
    }

    BufferHandle* bufferHandle = dCamerabuffer->bufferHandle_;
    if (!WriteBufferHandle(reply, *bufferHandle)) {
        DHLOGE("Write buffer handle failed.");
        FreeBufferHandle(bufferHandle);
        return HDF_ERR_INVALID_PARAM;
    }

    return HDF_SUCCESS;
}

int32_t DCameraProviderStub::DCProviderStubShutterBuffer(MessageParcel& data, MessageParcel& reply,
    MessageOption& option)
{
    if (data.ReadInterfaceToken() != DCameraProviderStub::GetDescriptor()) {
        DHLOGE("invalid token.");
        return HDF_FAILURE;
    }

    std::shared_ptr<DHBase> dhBase = std::make_shared<DHBase>(data.ReadString(), data.ReadString());
    int32_t streamId = data.ReadInt32();

    std::shared_ptr<DCameraBuffer> buffer = nullptr;
    bool flag = data.ReadBool();
    if (flag) {
        buffer = std::make_shared<DCameraBuffer>();
        buffer->index_ = data.ReadInt32();
        buffer->size_ = data.ReadInt32();
        buffer->bufferHandle_ = nullptr;
    }
    DCamRetCode ret = dcameraProvider_->ShutterBuffer(dhBase, streamId, buffer);
    if (!reply.WriteInt32(static_cast<int32_t>(ret))) {
        DHLOGE("Write retcode failed.");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t DCameraProviderStub::DCProviderStubOnSettingsResult(MessageParcel& data, MessageParcel& reply,
    MessageOption& option)
{
    if (data.ReadInterfaceToken() != DCameraProviderStub::GetDescriptor()) {
        DHLOGE("invalid token.");
        return HDF_FAILURE;
    }

    std::shared_ptr<DHBase> dhBase = std::make_shared<DHBase>(data.ReadString(), data.ReadString());

    std::shared_ptr<DCameraSettings> dCameraSettings = std::make_shared<DCameraSettings>();
    bool flag = data.ReadBool();
    if (flag) {
        IpcDataUtils::DecodeDCameraSettings(data, dCameraSettings);
        if (dCameraSettings == nullptr) {
            DHLOGE("Read distributed camera settings failed.");
            return HDF_FAILURE;
        }
    }
    DCamRetCode ret = dcameraProvider_->OnSettingsResult(dhBase, dCameraSettings);
    if (!reply.WriteInt32(static_cast<int32_t>(ret))) {
        DHLOGE("Write retcode failed.");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t DCameraProviderStub::DCProviderStubNotify(MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    if (data.ReadInterfaceToken() != DCameraProviderStub::GetDescriptor()) {
        DHLOGE("invalid token.");
        return HDF_FAILURE;
    }

    std::shared_ptr<DHBase> dhBase = std::make_shared<DHBase>(data.ReadString(), data.ReadString());

    std::shared_ptr<DCameraHDFEvent> dCameraEvent = std::make_shared<DCameraHDFEvent>();
    bool flag = data.ReadBool();
    if (flag) {
        IpcDataUtils::DecodeDCameraHDFEvent(data, dCameraEvent);
        if (dCameraEvent == nullptr) {
            DHLOGE("Read distributed camera hdf event failed.");
            return HDF_FAILURE;
        }
    }
    DCamRetCode ret = dcameraProvider_->Notify(dhBase, dCameraEvent);
    if (!reply.WriteInt32(static_cast<int32_t>(ret))) {
        DHLOGE("Write retcode failed.");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t DCameraProviderStub::OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply,
    MessageOption& option)
{
    int32_t ret = HDF_SUCCESS;
    switch (code) {
        case CMD_DISTRIBUTED_CAMERA_PROVIDER_ENABLE_DEVICE: {
            ret = DCProviderStubEnableDCameraDevice(data, reply, option);
            break;
        }
        case CMD_DISTRIBUTED_CAMERA_PROVIDER_DISABLE_DEVICE: {
            ret = DCProviderStubDisableDCameraDevice(data, reply, option);
            break;
        }
        case CMD_DISTRIBUTED_CAMERA_PROVIDER_ACQUIRE_BUFFER: {
            ret = DCProviderStubAcquireBuffer(data, reply, option);
            break;
        }
        case CMD_DISTRIBUTED_CAMERA_PROVIDER_SHUTTER_BUFFER: {
            ret = DCProviderStubShutterBuffer(data, reply, option);
            break;
        }
        case CMD_DISTRIBUTED_CAMERA_PROVIDER_ON_SETTINGS_RESULT: {
            ret = DCProviderStubOnSettingsResult(data, reply, option);
            break;
        }
        case CMD_DISTRIBUTED_CAMERA_PROVIDER_NOTIFY: {
            ret = DCProviderStubNotify(data, reply, option);
            break;
        }
        default: {
            DHLOGE("Unknown remote request code=%d.", code);
        }
    }
    return ret;
}
} // namespace DistributedHardware
} // namespace OHOS

void *DCameraProviderStubInstance()
{
    OHOS::DistributedHardware::DCameraProviderStub *stub =
        new (std::nothrow) OHOS::DistributedHardware::DCameraProviderStub();
    if (stub == nullptr) {
        HDF_LOGE("%{public}s: camera provider stub create failed.", __func__);
        return nullptr;
    }

    int32_t ret = stub->Init();
    if (ret != HDF_SUCCESS) {
        delete stub;
        stub = nullptr;
        return nullptr;
    }
    return reinterpret_cast<void *>(stub);
}

void DestroyDCameraProviderStub(void *stubObj)
{
    delete reinterpret_cast<OHOS::DistributedHardware::DCameraProviderStub *>(stubObj);
}

int32_t DCProviderServiceOnRemoteRequest(void *stub, int cmdId, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    if (stub == nullptr) {
        HDF_LOGE("%{public}s:stub is null", __func__);
        return HDF_FAILURE;
    }

    OHOS::DistributedHardware::DCameraProviderStub *providerStub =
        reinterpret_cast<OHOS::DistributedHardware::DCameraProviderStub *>(stub);
    OHOS::MessageParcel *dataParcel = nullptr;
    OHOS::MessageParcel *replyParcel = nullptr;

    if (SbufToParcel(reply, &replyParcel) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s:invalid reply sbuf object to dispatch", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (SbufToParcel(data, &dataParcel) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s:invalid data sbuf object to dispatch", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    OHOS::MessageOption option;
    return providerStub->OnRemoteRequest(cmdId, *dataParcel, *replyParcel, option);
}