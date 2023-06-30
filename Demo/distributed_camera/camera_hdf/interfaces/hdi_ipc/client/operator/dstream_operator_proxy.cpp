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

#include "dstream_operator_proxy.h"
#include <hdf_base.h>
#include <hdf_log.h>
#include <message_parcel.h>
#include "distributed_hardware_log.h"
#include "ioffline_stream_operator.h"
#include "ipc_data_utils.h"
#include "istream_operator_callback.h"
#include "metadata_utils.h"

#ifndef BALTIMORE_CAMERA
#include "cmd_common.h"
#endif

namespace OHOS {
namespace DistributedHardware {
CamRetCode DStreamOperatorProxy::IsStreamsSupported(OperationMode mode,
    const std::shared_ptr<CameraStandard::CameraMetadata> &modeSetting,
    const std::vector<std::shared_ptr<StreamInfo>> &info,
    StreamSupportType &type)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(DStreamOperatorProxy::GetDescriptor())) {
        DHLOGE("Write token failed.");
        return CamRetCode::INVALID_ARGUMENT;
    }

    if (!data.WriteInt32(mode)) {
        DHLOGE("Write operation mode failed.");
        return CamRetCode::INVALID_ARGUMENT;
    }

    bool nullFlag = (modeSetting != nullptr);
    if (!data.WriteBool(nullFlag)) {
        DHLOGE("Write mode null flag failed.");
        return CamRetCode::INVALID_ARGUMENT;
    }

    if (nullFlag && !CameraStandard::MetadataUtils::EncodeCameraMetadata(modeSetting, data)) {
        DHLOGE("Write metadata failed.");
        return CamRetCode::INVALID_ARGUMENT;
    }

    nullFlag = info.size();
    if (!data.WriteBool(nullFlag)) {
        DHLOGE("Write streaminfo null flag failed.");
        return CamRetCode::INVALID_ARGUMENT;
    }

    size_t count = info.size();
    if (!data.WriteInt32(static_cast<int32_t>(count))) {
        HDF_LOGE("%s: write info count failed", __func__);
        return CamRetCode::INVALID_ARGUMENT;
    }

    for (size_t i = 0; i < count; i++) {
        std::shared_ptr<StreamInfo> streamInfo = info.at(i);
        bool ret = IpcDataUtils::EncodeStreamInfo(streamInfo, data);
        if (!ret) {
            HDF_LOGE("%s: write streamInfo failed. index = %zu", __func__, i);
            return CamRetCode::INVALID_ARGUMENT;
        }
    }

    int32_t ret = Remote()->SendRequest(CMD_STREAM_OPERATOR_IS_STREAMS_SUPPORTED, data, reply, option);
    if (ret != HDF_SUCCESS) {
        DHLOGE("SendRequest failed, error code = %d", ret);
        return CamRetCode::INVALID_ARGUMENT;
    }

    CamRetCode retCode = static_cast<CamRetCode>(reply.ReadInt32());
    type = static_cast<StreamSupportType>(reply.ReadInt32());
    return retCode;
}

CamRetCode DStreamOperatorProxy::CreateStreams(const std::vector<std::shared_ptr<StreamInfo>> &streamInfos)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(DStreamOperatorProxy::GetDescriptor())) {
        DHLOGE("Write token failed.");
        return CamRetCode::INVALID_ARGUMENT;
    }

    size_t count = streamInfos.size();
    if (!data.WriteInt32(static_cast<int32_t>(count))) {
        DHLOGE("Write streamInfos count failed.");
        return CamRetCode::INVALID_ARGUMENT;
    }

    for (size_t i = 0; i < count; i++) {
        std::shared_ptr<StreamInfo> streamInfo = streamInfos.at(i);
        bool bRet = IpcDataUtils::EncodeStreamInfo(streamInfo, data);
        if (!bRet) {
            DHLOGE("Write streamInfo failed. index = %d", i);
            return CamRetCode::INVALID_ARGUMENT;
        }
    }

    int32_t ret = Remote()->SendRequest(CMD_STREAM_OPERATOR_CREATE_STREAMS, data, reply, option);
    if (ret != HDF_SUCCESS) {
        DHLOGE("SendRequest failed, error code = %d.", ret);
        return CamRetCode::INVALID_ARGUMENT;
    }
    return static_cast<CamRetCode>(reply.ReadInt32());
}

CamRetCode DStreamOperatorProxy::ReleaseStreams(const std::vector<int> &streamIds)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(DStreamOperatorProxy::GetDescriptor())) {
        DHLOGE("Write token failed.");
        return CamRetCode::INVALID_ARGUMENT;
    }

    std::vector<int32_t> pxyStreamIds = streamIds;
    if (!data.WriteInt32Vector(pxyStreamIds)) {
        DHLOGE("Write streamIds failed.");
        return CamRetCode::INVALID_ARGUMENT;
    }

    int32_t ret = Remote()->SendRequest(CMD_STREAM_OPERATOR_RELEASE_STREAMS, data, reply, option);
    if (ret != HDF_SUCCESS) {
        DHLOGE("SendRequest failed, error code = %d.", ret);
        return CamRetCode::INVALID_ARGUMENT;
    }
    return static_cast<CamRetCode>(reply.ReadInt32());
}

CamRetCode DStreamOperatorProxy::CommitStreams(OperationMode mode,
    const std::shared_ptr<CameraStandard::CameraMetadata> &modeSetting)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(DStreamOperatorProxy::GetDescriptor())) {
        DHLOGE("Write token failed.");
        return CamRetCode::INVALID_ARGUMENT;
    }

    if (!data.WriteInt32(mode)) {
        DHLOGE("Write operation mode failed.");
        return CamRetCode::INVALID_ARGUMENT;
    }

    bool bRet = CameraStandard::MetadataUtils::EncodeCameraMetadata(modeSetting, data);
    if (!bRet) {
        DHLOGE("Write metadata failed.");
        return CamRetCode::INVALID_ARGUMENT;
    }

    int32_t ret = Remote()->SendRequest(CMD_STREAM_OPERATOR_COMMIT_STREAMS, data, reply, option);
    if (ret != HDF_SUCCESS) {
        DHLOGE("SendRequest failed, error code = %d.", ret);
        return CamRetCode::INVALID_ARGUMENT;
    }
    return static_cast<CamRetCode>(reply.ReadInt32());
}

CamRetCode DStreamOperatorProxy::GetStreamAttributes(std::vector<std::shared_ptr<StreamAttribute>> &attributes)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(DStreamOperatorProxy::GetDescriptor())) {
        DHLOGE("Write token failed.");
        return CamRetCode::INVALID_ARGUMENT;
    }

    int32_t ret = Remote()->SendRequest(CMD_STREAM_OPERATOR_GET_STREAM_ATTRIBUTES, data, reply, option);
    if (ret != HDF_SUCCESS) {
        DHLOGE("SendRequest failed, error code = %d.", ret);
        return CamRetCode::INVALID_ARGUMENT;
    }

    int32_t retCode = reply.ReadInt32();
    int32_t count = reply.ReadInt32();
    for (int i = 0; i < count; i++) {
        const uint8_t *buffer = data.ReadBuffer(sizeof(StreamAttribute));
        std::shared_ptr<StreamAttribute> attribute = std::shared_ptr<StreamAttribute>(
            reinterpret_cast<StreamAttribute *>(
                const_cast<uint8_t *>(buffer)));
        attributes.push_back(attribute);
    }
    return static_cast<CamRetCode>(retCode);
}

CamRetCode DStreamOperatorProxy::AttachBufferQueue(int streamId, const OHOS::sptr<OHOS::IBufferProducer> &producer)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(DStreamOperatorProxy::GetDescriptor())) {
        DHLOGE("Write token failed.");
        return CamRetCode::INVALID_ARGUMENT;
    }

    if (producer == nullptr) {
        DHLOGE("Input producer is NULL.");
        return CamRetCode::INVALID_ARGUMENT;
    }

    if (!data.WriteInt32(static_cast<int32_t>(streamId))) {
        DHLOGE("Write streamId failed.");
        return CamRetCode::INVALID_ARGUMENT;
    }

    if (!data.WriteRemoteObject(producer->AsObject())) {
        DHLOGE("Write buffer producer failed.");
        return CamRetCode::INVALID_ARGUMENT;
    }

    int32_t ret = Remote()->SendRequest(CMD_STREAM_OPERATOR_ATTACH_BUFFER_QUEUE, data, reply, option);
    if (ret != HDF_SUCCESS) {
        DHLOGE("SendRequest failed, error code = %d.", ret);
        return CamRetCode::INVALID_ARGUMENT;
    }
    return static_cast<CamRetCode>(reply.ReadInt32());
}

CamRetCode DStreamOperatorProxy::DetachBufferQueue(int streamId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(DStreamOperatorProxy::GetDescriptor())) {
        DHLOGE("Write token failed.");
        return CamRetCode::INVALID_ARGUMENT;
    }

    if (!data.WriteInt32(static_cast<int32_t>(streamId))) {
        DHLOGE("Write streamId failed.");
        return CamRetCode::INVALID_ARGUMENT;
    }

    int32_t ret = Remote()->SendRequest(CMD_STREAM_OPERATOR_DETACH_BUFFER_QUEUE, data, reply, option);
    if (ret != HDF_SUCCESS) {
        DHLOGE("SendRequest failed, error code = %d", ret);
        return CamRetCode::INVALID_ARGUMENT;
    }
    return static_cast<CamRetCode>(reply.ReadInt32());
}

CamRetCode DStreamOperatorProxy::Capture(int captureId,
    const std::shared_ptr<CaptureInfo> &info, bool isStreaming)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(DStreamOperatorProxy::GetDescriptor())) {
        DHLOGE("Write token failed.");
        return CamRetCode::INVALID_ARGUMENT;
    }

    if (info == nullptr) {
        return CamRetCode::INVALID_ARGUMENT;
    }

    if (!data.WriteInt32(static_cast<int32_t>(captureId))) {
        DHLOGE("Write captureId failed.");
        return CamRetCode::INVALID_ARGUMENT;
    }

    std::vector<int32_t> pxyStreamIds = info->streamIds_;
    if (!data.WriteInt32Vector(pxyStreamIds)) {
        DHLOGE("Write streamIds failed.");
        return CamRetCode::INVALID_ARGUMENT;
    }

    bool bRet = CameraStandard::MetadataUtils::EncodeCameraMetadata(info->captureSetting_, data);
    if (!bRet) {
        DHLOGE("Write metadata failed.");
        return CamRetCode::INVALID_ARGUMENT;
    }

    if (!data.WriteBool(info->enableShutterCallback_)) {
        DHLOGE("Write enableShutterCallback_ failed.");
        return CamRetCode::INVALID_ARGUMENT;
    }

    if (!data.WriteBool(isStreaming)) {
        DHLOGE("Write isStreaming failed.");
        return CamRetCode::INVALID_ARGUMENT;
    }

    int32_t ret = Remote()->SendRequest(CMD_STREAM_OPERATOR_CAPTURE, data, reply, option);
    if (ret != HDF_SUCCESS) {
        DHLOGE("SendRequest failed, error code = %d.", ret);
        return CamRetCode::INVALID_ARGUMENT;
    }
    return static_cast<CamRetCode>(reply.ReadInt32());
}

CamRetCode DStreamOperatorProxy::CancelCapture(int captureId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(DStreamOperatorProxy::GetDescriptor())) {
        DHLOGE("Write token failed.");
        return CamRetCode::INVALID_ARGUMENT;
    }

    if (!data.WriteInt32(static_cast<int32_t>(captureId))) {
        DHLOGE("Write captureId failed.");
        return CamRetCode::INVALID_ARGUMENT;
    }

    int32_t ret = Remote()->SendRequest(
        CMD_STREAM_OPERATOR_CANCEL_CAPTURE, data, reply, option);
    if (ret != HDF_SUCCESS) {
        DHLOGE("SendRequest failed, error code = %d.", ret);
        return CamRetCode::INVALID_ARGUMENT;
    }
    return static_cast<CamRetCode>(reply.ReadInt32());
}

CamRetCode DStreamOperatorProxy::ChangeToOfflineStream(
    const std::vector<int> &streamIds,
    OHOS::sptr<IStreamOperatorCallback> &callback,
    OHOS::sptr<IOfflineStreamOperator> &offlineOperator)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(DStreamOperatorProxy::GetDescriptor())) {
        DHLOGE("Write token failed.");
        return CamRetCode::INVALID_ARGUMENT;
    }

    if (callback == nullptr) {
        return CamRetCode::INVALID_ARGUMENT;
    }

    std::vector<int32_t> pxyStreamIds = streamIds;
    if (!data.WriteInt32Vector(pxyStreamIds)) {
        DHLOGE("Write streamIds failed.");
        return CamRetCode::INVALID_ARGUMENT;
    }

    if (!data.WriteRemoteObject(callback->AsObject())) {
        DHLOGE("Write offline stream operator callback failed.");
        return CamRetCode::INVALID_ARGUMENT;
    }

    int32_t ret = Remote()->SendRequest(CMD_STREAM_OPERATOR_CHANGE_TO_OFFLINE_STREAM, data, reply, option);
    if (ret != HDF_SUCCESS) {
        DHLOGE("SendRequest failed, error code = %d.", ret);
        return CamRetCode::INVALID_ARGUMENT;
    }

    CamRetCode retCode = static_cast<CamRetCode>(reply.ReadInt32());
    sptr<IRemoteObject> remoteObj = reply.ReadRemoteObject();
    offlineOperator = OHOS::iface_cast<IOfflineStreamOperator>(remoteObj);
    return retCode;
}
} // namespace DistributedHardware
} // namespace OHOS
