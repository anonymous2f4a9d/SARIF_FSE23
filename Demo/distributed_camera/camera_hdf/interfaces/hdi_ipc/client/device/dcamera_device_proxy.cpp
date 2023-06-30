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

#include "dcamera_device_proxy.h"
#include <hdf_base.h>
#include <message_parcel.h>
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
CamRetCode DCameraDeviceProxy::GetStreamOperator(
    const OHOS::sptr<IStreamOperatorCallback> &callback,
    OHOS::sptr<IStreamOperator> &streamOperator)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(DCameraDeviceProxy::GetDescriptor())) {
        DHLOGE("Write stream operator descriptor failed.");
        return CamRetCode::INVALID_ARGUMENT;
    }

    bool nullFlag = (callback != nullptr);
    if (!data.WriteBool(nullFlag)) {
        DHLOGE("Write stream operator callback flag failed.");
        return CamRetCode::INVALID_ARGUMENT;
    }

    if (nullFlag && !data.WriteRemoteObject(callback->AsObject())) {
        DHLOGE("Write stream operator object failed.");
        return CamRetCode::INVALID_ARGUMENT;
    }

    int32_t ret = Remote()->SendRequest(CMD_CAMERA_DEVICE_REMOTE_GET_STREAM_OPERATOR, data, reply, option);
    if (ret != HDF_SUCCESS) {
        DHLOGE("SendRequest failed, error code = %d.", ret);
        return CamRetCode::INVALID_ARGUMENT;
    }

    CamRetCode retCode = static_cast<CamRetCode>(reply.ReadInt32());
    bool flag = reply.ReadBool();
    if (flag) {
        sptr<IRemoteObject> remoteStreamOperator = reply.ReadRemoteObject();
        streamOperator = OHOS::iface_cast<IStreamOperator>(remoteStreamOperator);
    }
    return retCode;
}

CamRetCode DCameraDeviceProxy::UpdateSettings(const std::shared_ptr<CameraSetting> &settings)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(DCameraDeviceProxy::GetDescriptor())) {
        DHLOGE("Write stream operator descriptor failed.");
        return CamRetCode::INVALID_ARGUMENT;
    }

    bool bRet = CameraStandard::MetadataUtils::EncodeCameraMetadata(settings, data);
    if (!bRet) {
        DHLOGE("Write update settings metadata failed.");
        return CamRetCode::INVALID_ARGUMENT;
    }

    int32_t ret = Remote()->SendRequest(CMD_CAMERA_DEVICE_UPDATE_SETTINGS, data, reply, option);
    if (ret != HDF_SUCCESS) {
        DHLOGE("SendRequest failed, error code = %d.", ret);
        return CamRetCode::INVALID_ARGUMENT;
    }
    return static_cast<CamRetCode>(reply.ReadInt32());
}

CamRetCode DCameraDeviceProxy::SetResultMode(const ResultCallbackMode &mode)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(DCameraDeviceProxy::GetDescriptor())) {
        DHLOGE("Write stream operator descriptor failed.");
        return CamRetCode::INVALID_ARGUMENT;
    }

    if (!data.WriteInt32(static_cast<int32_t>(mode))) {
        DHLOGE("Write result callback mode failed.");
        return CamRetCode::INVALID_ARGUMENT;
    }

    int32_t ret = Remote()->SendRequest(CMD_CAMERA_DEVICE_REMOTE_SET_RESULT_MODE, data, reply, option);
    if (ret != HDF_SUCCESS) {
        DHLOGE("SendRequest failed, error code = %d.", ret);
        return CamRetCode::INVALID_ARGUMENT;
    }
    return static_cast<CamRetCode>(reply.ReadInt32());
}

CamRetCode DCameraDeviceProxy::GetEnabledResults(std::vector<MetaType> &results)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(DCameraDeviceProxy::GetDescriptor())) {
        DHLOGE("Write stream operator descriptor failed.");
        return CamRetCode::INVALID_ARGUMENT;
    }

    int32_t ret = Remote()->SendRequest(CMD_CAMERA_DEVICE_REMOTE_GET_ENABLED_RESULTS, data, reply, option);
    if (ret != HDF_SUCCESS) {
        DHLOGE("SendRequest failed, error code = %d.", ret);
        return CamRetCode::INVALID_ARGUMENT;
    }

    CamRetCode retCode = static_cast<CamRetCode>(reply.ReadInt32());
    if (!reply.ReadInt32Vector(&results)) {
        DHLOGE("Read results failed.");
        return CamRetCode::INVALID_ARGUMENT;
    }
    return retCode;
}

CamRetCode DCameraDeviceProxy::EnableResult(const std::vector<MetaType> &results)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(DCameraDeviceProxy::GetDescriptor())) {
        DHLOGE("Write stream operator descriptor failed.");
        return CamRetCode::INVALID_ARGUMENT;
    }

    if (!data.WriteInt32Vector(results)) {
        DHLOGE("Write results failed.");
        return CamRetCode::INVALID_ARGUMENT;
    }

    int32_t ret = Remote()->SendRequest(CMD_CAMERA_DEVICE_REMOTE_ENABLE_RESULT, data, reply, option);
    if (ret != HDF_SUCCESS) {
        DHLOGE("SendRequest failed, error code = %d.", ret);
        return CamRetCode::INVALID_ARGUMENT;
    }
    return static_cast<CamRetCode>(reply.ReadInt32());
}

CamRetCode DCameraDeviceProxy::DisableResult(const std::vector<MetaType> &results)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(DCameraDeviceProxy::GetDescriptor())) {
        DHLOGE("Write stream operator descriptor failed.");
        return CamRetCode::INVALID_ARGUMENT;
    }

    if (!data.WriteInt32Vector(results)) {
        DHLOGE("Write results failed.");
        return CamRetCode::INVALID_ARGUMENT;
    }

    int32_t ret = Remote()->SendRequest(CMD_CAMERA_DEVICE_REMOTE_DISABLE_RESULT, data, reply, option);
    if (ret != HDF_SUCCESS) {
        DHLOGE("SendRequest failed, error code = %d.", ret);
        return CamRetCode::INVALID_ARGUMENT;
    }
    return static_cast<CamRetCode>(reply.ReadInt32());
}

void DCameraDeviceProxy::Close()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(DCameraDeviceProxy::GetDescriptor())) {
        DHLOGE("Write stream operator descriptor failed.");
        return;
    }

    int32_t ret = Remote()->SendRequest(CMD_CAMERA_DEVICE_REMOTE_CLOSE, data, reply, option);
    if (ret != HDF_SUCCESS) {
        DHLOGE("SendRequest failed, error code is %d.", ret);
    }
}
} // namespace DistributedHardware
} // namespace OHOS
