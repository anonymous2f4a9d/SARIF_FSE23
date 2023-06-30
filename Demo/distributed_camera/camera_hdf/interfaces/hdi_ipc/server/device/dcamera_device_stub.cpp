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

#include "dcamera_device_stub.h"
#include <hdf_base.h>
#include <hdf_log.h>
#include "distributed_hardware_log.h"
#include "ipc_data_utils.h"
#include "istream_operator.h"
#include "istream_operator_callback.h"
#include "metadata_utils.h"

#ifndef BALTIMORE_CAMERA
#include "cmd_common.h"
#endif

namespace OHOS {
namespace DistributedHardware {
int32_t DCameraDeviceStub::OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply,
    MessageOption& option)
{
    int32_t ret = HDF_SUCCESS;
    switch (code) {
        case CMD_CAMERA_DEVICE_GET_STREAM_OPERATOR: {
            ret = DCDeviceStubGetStreamOperator(data, reply, option);
            break;
        }
        case CMD_CAMERA_DEVICE_UPDATE_SETTINGS: {
            ret = DCDeviceStubUpdateSettings(data, reply, option);
            break;
        }
        case CMD_CAMERA_DEVICE_SET_RESULT_MODE: {
            ret = DCDeviceStubSetResultMode(data, reply, option);
            break;
        }
        case CMD_CAMERA_DEVICE_GET_ENABLED_RESULTS: {
            ret = DCDeviceStubGetEnabledReuslts(data, reply, option);
            break;
        }
        case CMD_CAMERA_DEVICE_ENABLE_RESULT: {
            ret = DCDeviceStubEnableResult(data, reply, option);
            break;
        }
        case CMD_CAMERA_DEVICE_DISABLE_RESULT: {
            ret = DCDeviceStubDisableResult(data, reply, option);
            break;
        }
        case CMD_CAMERA_DEVICE_CLOSE: {
            ret = DCDeviceStubClose(data, reply, option);
            break;
        }
        default: {
            ret = IPCObjectStub::OnRemoteRequest(code, data, reply, option);
        }
    }
    return ret;
}

int32_t DCameraDeviceStub::DCDeviceStubGetStreamOperator(MessageParcel& data, MessageParcel& reply,
    MessageOption& option)
{
    DHLOGI("DCameraDeviceStub::DCDeviceStubGetStreamOperator entry.");
    if (data.ReadInterfaceToken() != DCameraDeviceStub::GetDescriptor()) {
        DHLOGE("Invalid token.");
        return HDF_FAILURE;
    }

    sptr<IStreamOperatorCallback> spStreamOperatorCallback = nullptr;
    bool flag = data.ReadBool();
    if (flag) {
        sptr<IRemoteObject> remoteObj = data.ReadRemoteObject();
        spStreamOperatorCallback = OHOS::iface_cast<IStreamOperatorCallback>(remoteObj);
        if (spStreamOperatorCallback == nullptr) {
            DHLOGE("Read operator callback failed.");
            return HDF_FAILURE;
        }
    }

    OHOS::sptr<IStreamOperator> streamOperator = nullptr;
    CamRetCode ret = GetStreamOperator(spStreamOperatorCallback, streamOperator);
    if (!reply.WriteInt32(static_cast<int32_t>(ret))) {
        DHLOGE("Get stream operator failed.");
        return HDF_FAILURE;
    }

    bool nullFlag = (streamOperator != nullptr);
    if (!reply.WriteBool(nullFlag)) {
        DHLOGE("Write stream operator flag failed.");
        return INVALID_ARGUMENT;
    }

    if (nullFlag && !reply.WriteRemoteObject(streamOperator->AsObject())) {
        DHLOGE("Write stream operator failed.");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t DCameraDeviceStub::DCDeviceStubUpdateSettings(MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    DHLOGI("DCameraDeviceStub::DCDeviceStubUpdateSettings entry.");
    if (data.ReadInterfaceToken() != DCameraDeviceStub::GetDescriptor()) {
        DHLOGE("Invalid token.");
        return HDF_FAILURE;
    }

    std::shared_ptr<CameraStandard::CameraMetadata> metadata = nullptr;
    CameraStandard::MetadataUtils::DecodeCameraMetadata(data, metadata);

    CamRetCode ret = UpdateSettings(metadata);
    if (!reply.WriteInt32(static_cast<int32_t>(ret))) {
        DHLOGE("Get stream operator failed.");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t DCameraDeviceStub::DCDeviceStubSetResultMode(MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    DHLOGI("DCameraDeviceStub::DCDeviceStubSetResultMode entry.");
    if (data.ReadInterfaceToken() != DCameraDeviceStub::GetDescriptor()) {
        DHLOGE("Invalid token.");
        return HDF_FAILURE;
    }

    ResultCallbackMode mode = static_cast<ResultCallbackMode>(data.ReadInt32());
    CamRetCode ret = SetResultMode(mode);
    if (!reply.WriteInt32(static_cast<int32_t>(ret))) {
        DHLOGE("Write retcode failed.");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t DCameraDeviceStub::DCDeviceStubGetEnabledReuslts(MessageParcel& data, MessageParcel& reply,
    MessageOption& option)
{
    DHLOGI("DCameraDeviceStub::DCDeviceStubGetEnabledReuslts entry.");
    if (data.ReadInterfaceToken() != DCameraDeviceStub::GetDescriptor()) {
        DHLOGE("Invalid token.");
        return HDF_FAILURE;
    }

    std::vector<int32_t> results;
    CamRetCode ret = GetEnabledResults(results);
    if (!reply.WriteInt32(static_cast<CamRetCode>(ret))) {
        DHLOGE("Write retcode failed.");
        return HDF_FAILURE;
    }

    if (!reply.WriteInt32Vector(results)) {
        DHLOGE("Write results failed.");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t DCameraDeviceStub::DCDeviceStubEnableResult(MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    DHLOGI("DCameraDeviceStub::DCDeviceStubEnableResult entry.");
    if (data.ReadInterfaceToken() != DCameraDeviceStub::GetDescriptor()) {
        DHLOGE("Invalid token.");
        return HDF_FAILURE;
    }

    std::vector<int32_t> results;
    if (!data.ReadInt32Vector(&results)) {
        DHLOGE("Read results failed.");
        return HDF_FAILURE;
    }

    CamRetCode ret = EnableResult(results);
    if (!reply.WriteInt32(static_cast<CamRetCode>(ret))) {
        DHLOGE("Write retcode failed.");
        return HDF_FAILURE;
    }

    if (!reply.WriteInt32Vector(results)) {
        DHLOGE("Write results failed.");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t DCameraDeviceStub::DCDeviceStubDisableResult(MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    DHLOGI("DCameraDeviceStub::DCDeviceStubDisableResult entry.");
    if (data.ReadInterfaceToken() != DCameraDeviceStub::GetDescriptor()) {
        DHLOGE("Invalid token.");
        return HDF_FAILURE;
    }

    std::vector<int32_t> results;
    if (!data.ReadInt32Vector(&results)) {
        DHLOGE("Read results failed.");
        return HDF_FAILURE;
    }

    CamRetCode ret = DisableResult(results);
    if (!reply.WriteInt32(static_cast<CamRetCode>(ret))) {
        DHLOGE("Write retcode failed.");
        return HDF_FAILURE;
    }

    if (!reply.WriteInt32Vector(results)) {
        DHLOGE("Write results failed.");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t DCameraDeviceStub::DCDeviceStubClose(MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    DHLOGI("DCameraDeviceStub::DCDeviceStubClose entry.");
    if (data.ReadInterfaceToken() != DCameraDeviceStub::GetDescriptor()) {
        DHLOGE("Invalid token.");
        return HDF_FAILURE;
    }

    Close();
    return HDF_SUCCESS;
}
} // namespace DistributedHardware
} // namespace OHOS
