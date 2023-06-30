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

#include "dcamera_host_stub.h"
#include <hdf_base.h>
#include <hdf_log.h>
#include <hdf_sbuf_ipc.h>
#include "distributed_hardware_log.h"
#include "icamera_device.h"
#include "icamera_host_callback.h"
#include "ipc_data_utils.h"
#include "metadata_utils.h"

#ifndef BALTIMORE_CAMERA
#include "cmd_common.h"
#endif

namespace OHOS {
namespace DistributedHardware {
DCameraHostStub::DCameraHostStub()
{
}

int32_t DCameraHostStub::Init()
{
    dcameraHost_ = DCameraHost::GetInstance();
    if (dcameraHost_ == nullptr) {
        DHLOGE("Distributed camera host service start failed.");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t DCameraHostStub::DCHostStubSetCallback(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    if (data.ReadInterfaceToken() != DCameraHostStub::GetDescriptor()) {
        DHLOGE("invalid token.");
        return HDF_FAILURE;
    }

    bool flag = data.ReadBool();
    sptr<ICameraHostCallback> hostCallback = nullptr;
    if (flag) {
        sptr<IRemoteObject> remoteObj = data.ReadRemoteObject();
        hostCallback = OHOS::iface_cast<ICameraHostCallback>(remoteObj);
    }
    CamRetCode ret = dcameraHost_->SetCallback(hostCallback);
    if (!reply.WriteInt32(static_cast<int32_t>(ret))) {
        DHLOGE("Write retcode failed.");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t DCameraHostStub::DCHostStubGetCameraIds(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    if (data.ReadInterfaceToken() != DCameraHostStub::GetDescriptor()) {
        DHLOGE("invalid token.");
        return HDF_FAILURE;
    }

    if (dcameraHost_ == nullptr) {
        return HDF_FAILURE;
    }

    std::vector<std::string> cameraIds;
    CamRetCode ret = dcameraHost_->GetCameraIds(cameraIds);
    if (!reply.WriteInt32(static_cast<int32_t>(ret))) {
        DHLOGE("Write retcode failed.");
        return HDF_FAILURE;
    }

    if (ret == CamRetCode::NO_ERROR) {
        if (!reply.WriteStringVector(cameraIds)) {
            DHLOGE("Write cameraIds failed.");
            return HDF_FAILURE;
        }
    }
    return HDF_SUCCESS;
}

int32_t DCameraHostStub::DCHostStubGetCameraAbility(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    if (data.ReadInterfaceToken() != DCameraHostStub::GetDescriptor()) {
        DHLOGE("invalid token.");
        return HDF_FAILURE;
    }

    const std::string cameraId = data.ReadString();
    if (cameraId.empty()) {
        DHLOGE("Read input param is empty.");
        return HDF_ERR_INVALID_PARAM;
    }

    std::shared_ptr<CameraAbility> ability = nullptr;
    CamRetCode ret = dcameraHost_->GetCameraAbility(cameraId, ability);
    if (!reply.WriteInt32(static_cast<int32_t>(ret))) {
        DHLOGE("Write retcode failed.");
        return HDF_FAILURE;
    }

    if (ret == CamRetCode::NO_ERROR) {
        bool bRet = CameraStandard::MetadataUtils::EncodeCameraMetadata(ability, reply);
        if (!bRet) {
            DHLOGE("Write ability failed.");
            return HDF_FAILURE;
        }
    }
    return HDF_SUCCESS;
}

int32_t DCameraHostStub::DCHostStubOpenCamera(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    if (data.ReadInterfaceToken() != DCameraHostStub::GetDescriptor()) {
        DHLOGE("invalid token.");
        return HDF_FAILURE;
    }

    const std::string cameraId = data.ReadString();
    if (cameraId.empty()) {
        DHLOGE("Read input param is empty.");
        return HDF_ERR_INVALID_PARAM;
    }

    bool flag = data.ReadBool();
    OHOS::sptr<ICameraDeviceCallback> deviceCallback = nullptr;
    if (flag) {
        OHOS::sptr<IRemoteObject> remoteCallback = data.ReadRemoteObject();
        deviceCallback = OHOS::iface_cast<ICameraDeviceCallback>(remoteCallback);
    }

    OHOS::sptr<ICameraDevice> cameraDevice = nullptr;
    CamRetCode ret = dcameraHost_->OpenCamera(cameraId, deviceCallback, cameraDevice);
    if (!reply.WriteInt32(static_cast<int32_t>(ret))) {
        DHLOGE("Get stream operator failed.");
        return HDF_FAILURE;
    }

    if (ret == CamRetCode::NO_ERROR) {
        bool deviceFlag = (cameraDevice != nullptr);
        if (!reply.WriteBool(deviceFlag)) {
            DHLOGE("Write camera device flag failed.");
            return HDF_FAILURE;
        }

        if (deviceFlag && !reply.WriteRemoteObject(cameraDevice->AsObject())) {
            DHLOGE("Write camera device failed.");
            return HDF_FAILURE;
        }
    }
    return HDF_SUCCESS;
}

int32_t DCameraHostStub::DCHostStubSetFlashlight(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    if (data.ReadInterfaceToken() != DCameraHostStub::GetDescriptor()) {
        DHLOGE("invalid token.");
        return HDF_FAILURE;
    }

    if (dcameraHost_ == nullptr) {
        DHLOGE("Camera host is null.");
        return HDF_FAILURE;
    }

    std::string cameraId = data.ReadString();
    bool isEnable = data.ReadBool();
    CamRetCode ret = dcameraHost_->SetFlashlight(cameraId, isEnable);
    if (!reply.WriteInt32(static_cast<int32_t>(ret))) {
        DHLOGE("Write retcode failed.");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t DCameraHostStub::OnRemoteRequest(int cmdId, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    switch (cmdId) {
        case CMD_CAMERA_HOST_SET_CALLBACK: {
            return DCHostStubSetCallback(data, reply, option);
        }
        case CMD_CAMERA_HOST_GET_CAMERAID: {
            return DCHostStubGetCameraIds(data, reply, option);
        }
        case CMD_CAMERA_HOST_GET_CAMERA_ABILITY: {
            return DCHostStubGetCameraAbility(data, reply, option);
        }
        case CMD_CAMERA_HOST_OPEN_CAMERA: {
            return DCHostStubOpenCamera(data, reply, option);
        }
        case CMD_CAMERA_HOST_SET_FLASH_LIGHT: {
            return DCHostStubSetFlashlight(data, reply, option);
        }
        default: {
            DHLOGE("Not support cmd %d.", cmdId);
            return HDF_ERR_INVALID_PARAM;
        }
    }
    return HDF_SUCCESS;
}
} // namespace DistributedHardware
} // namespace OHOS

void *DCameraHostStubInstance()
{
    OHOS::DistributedHardware::DCameraHostStub *stub = new (std::nothrow) OHOS::DistributedHardware::DCameraHostStub();
    if (stub == nullptr) {
        HDF_LOGE("%{public}s: camera host stub create failed.", __func__);
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

void DestroyDCameraHostStub(void *stubObj)
{
    delete reinterpret_cast<OHOS::DistributedHardware::DCameraHostStub *>(stubObj);
}

int32_t DCHostServiceOnRemoteRequest(void *stub, int cmdId, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    if (stub == nullptr) {
        HDF_LOGE("%{public}s:stub is null", __func__);
        return HDF_FAILURE;
    }

    OHOS::DistributedHardware::DCameraHostStub *dcameraHostStub =
        reinterpret_cast<OHOS::DistributedHardware::DCameraHostStub *>(stub);
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
    return dcameraHostStub->OnRemoteRequest(cmdId, *dataParcel, *replyParcel, option);
}
