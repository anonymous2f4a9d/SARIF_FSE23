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

#include "dstream_operator_stub.h"
#include <hdf_base.h>
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
int32_t DStreamOperatorStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    int32_t ret = HDF_SUCCESS;
    switch (code) {
        case CMD_STREAM_OPERATOR_IS_STREAMS_SUPPORTED: {
            ret = DStreamOperatorStubIsStreamsSupported(data, reply, option);
            break;
        }
        case CMD_STREAM_OPERATOR_CREATE_STREAMS: {
            ret = DStreamOperatorStubCreateStreams(data, reply, option);
            break;
        }
        case CMD_STREAM_OPERATOR_RELEASE_STREAMS: {
            ret = DStreamOperatorStubReleaseStreams(data, reply, option);
            break;
        }
        case CMD_STREAM_OPERATOR_COMMIT_STREAMS: {
            ret = DStreamOperatorStubCommitStreams(data, reply, option);
            break;
        }
        case CMD_STREAM_OPERATOR_GET_STREAM_ATTRIBUTES: {
            ret = DStreamOperatorStubGetStreamAttributes(data, reply, option);
            break;
        }
        case CMD_STREAM_OPERATOR_ATTACH_BUFFER_QUEUE: {
            ret = DStreamOperatorStubAttachBufferQueue(data, reply, option);
            break;
        }
        case CMD_STREAM_OPERATOR_DETACH_BUFFER_QUEUE: {
            ret = DStreamOperatorStubDetachBufferQueue(data, reply, option);
            break;
        }
        case CMD_STREAM_OPERATOR_CAPTURE: {
            ret = DStreamOperatorStubCapture(data, reply, option);
            break;
        }
        case CMD_STREAM_OPERATOR_CANCEL_CAPTURE: {
            ret = DStreamOperatorStubCancelCapture(data, reply, option);
            break;
        }
        case CMD_STREAM_OPERATOR_CHANGE_TO_OFFLINE_STREAM: {
            ret = DStreamOperatorStubChangeToOfflineStream(data, reply, option);
            break;
        }
        default: {
            ret = IPCObjectStub::OnRemoteRequest(code, data, reply, option);
        }
    }
    return ret;
}

int32_t DStreamOperatorStub::DStreamOperatorStubIsStreamsSupported(MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    DHLOGI("DStreamOperatorStub::DStreamOperatorStubIsStreamsSupported entry.");
    if (data.ReadInterfaceToken() != DStreamOperatorStub::GetDescriptor()) {
        DHLOGE("invalid token.");
        return HDF_FAILURE;
    }

    OperationMode operationMode = static_cast<OperationMode>(data.ReadInt32());

    std::shared_ptr<CameraStandard::CameraMetadata> metadata = nullptr;
    bool nullFlag = data.ReadBool();
    if (nullFlag) {
        CameraStandard::MetadataUtils::DecodeCameraMetadata(data, metadata);
    }

    int32_t count = data.ReadInt32();
    std::vector<std::shared_ptr<StreamInfo>> streamInfos;
    for (int i = 0; i < count; i++) {
        std::shared_ptr<StreamInfo> streamInfo = std::make_shared<StreamInfo>();
        IpcDataUtils::DecodeStreamInfo(data, streamInfo);
        streamInfos.push_back(streamInfo);
    }

    StreamSupportType streamSupportType;
    CamRetCode ret = IsStreamsSupported(operationMode, metadata, streamInfos, streamSupportType);
    if (!reply.WriteInt32(static_cast<int32_t>(ret))) {
        DHLOGE("Write retcode failed.");
        return HDF_FAILURE;
    }

    if (!reply.WriteInt32(static_cast<int32_t>(streamSupportType))) {
        DHLOGE("Write retcode failed.");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t DStreamOperatorStub::DStreamOperatorStubCreateStreams(MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    DHLOGI("DStreamOperatorStub::DStreamOperatorStubCreateStreams entry.");
    if (data.ReadInterfaceToken() != DStreamOperatorStub::GetDescriptor()) {
        DHLOGE("invalid token.");
        return HDF_FAILURE;
    }

    int32_t count = data.ReadInt32();
    std::vector<std::shared_ptr<StreamInfo>> streamInfos;
    for (int i = 0; i < count; i++) {
        std::shared_ptr<StreamInfo> streamInfo = std::make_shared<StreamInfo>();
        IpcDataUtils::DecodeStreamInfo(data, streamInfo);
        streamInfos.push_back(streamInfo);
    }

    CamRetCode ret = CreateStreams(streamInfos);
    if (!reply.WriteInt32(static_cast<int32_t>(ret))) {
        DHLOGE("Write retcode failed.");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t DStreamOperatorStub::DStreamOperatorStubReleaseStreams(MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    DHLOGI("DStreamOperatorStub::DStreamOperatorStubReleaseStreams entry.");
    if (data.ReadInterfaceToken() != DStreamOperatorStub::GetDescriptor()) {
        DHLOGE("invalid token.");
        return HDF_FAILURE;
    }

    std::vector<int32_t> streamIds;
    if (!data.ReadInt32Vector(&streamIds)) {
        DHLOGE("Read streamIds failed.");
        return HDF_FAILURE;
    }

    CamRetCode ret = ReleaseStreams(streamIds);
    if (!reply.WriteInt32(static_cast<int32_t>(ret))) {
        DHLOGE("Write retcode failed.");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t DStreamOperatorStub::DStreamOperatorStubCommitStreams(MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    DHLOGI("DStreamOperatorStub::DStreamOperatorStubCommitStreams entry.");
    if (data.ReadInterfaceToken() != DStreamOperatorStub::GetDescriptor()) {
        DHLOGE("invalid token.");
        return HDF_FAILURE;
    }

    OperationMode mode = static_cast<OperationMode>(data.ReadInt32());

    std::shared_ptr<CameraStandard::CameraMetadata> metadata = nullptr;
    CameraStandard::MetadataUtils::DecodeCameraMetadata(data, metadata);

    CamRetCode ret = CommitStreams(mode, metadata);
    if (!reply.WriteInt32(static_cast<int32_t>(ret))) {
        DHLOGE("Write retcode failed.");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t DStreamOperatorStub::DStreamOperatorStubGetStreamAttributes(MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    DHLOGI("DStreamOperatorStub::DStreamOperatorStubGetStreamAttributes entry.");
    if (data.ReadInterfaceToken() != DStreamOperatorStub::GetDescriptor()) {
        DHLOGE("invalid token.");
        return HDF_FAILURE;
    }

    std::vector<std::shared_ptr<StreamAttribute>> attributes;
    CamRetCode ret = GetStreamAttributes(attributes);
    if (!reply.WriteInt32(static_cast<int32_t>(ret))) {
        DHLOGE("Write retcode failed.");
        return HDF_FAILURE;
    }

    size_t count = attributes.size();
    if (!reply.WriteInt32(static_cast<int32_t>(count))) {
        DHLOGE("Write attributes count failed.");
        return HDF_FAILURE;
    }

    for (size_t i = 0; i < count; i++) {
        if (!reply.WriteBuffer((void *)(attributes[i].get()), sizeof(StreamAttribute))) {
            DHLOGE("Write attribute failed. index = %d.", i);
            return HDF_FAILURE;
        }
    }
    return HDF_SUCCESS;
}

int32_t DStreamOperatorStub::DStreamOperatorStubAttachBufferQueue(MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    DHLOGI("DStreamOperatorStub::DStreamOperatorStubAttachBufferQueue entry.");
    if (data.ReadInterfaceToken() != DStreamOperatorStub::GetDescriptor()) {
        DHLOGE("invalid token.");
        return HDF_FAILURE;
    }

    int32_t streamId = data.ReadInt32();
    sptr<IRemoteObject> remoteObj = data.ReadRemoteObject();
    const sptr<OHOS::IBufferProducer> bufferProducer = OHOS::iface_cast<OHOS::IBufferProducer>(remoteObj);

    CamRetCode ret = AttachBufferQueue(streamId, bufferProducer);
    if (!reply.WriteInt32(static_cast<int32_t>(ret))) {
        DHLOGE("Write retcode failed.");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t DStreamOperatorStub::DStreamOperatorStubDetachBufferQueue(
    MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    DHLOGI("DStreamOperatorStub::DStreamOperatorStubDetachBufferQueue entry.");
    if (data.ReadInterfaceToken() != DStreamOperatorStub::GetDescriptor()) {
        DHLOGE("invalid token.");
        return HDF_FAILURE;
    }

    int32_t streamId = data.ReadInt32();
    CamRetCode ret = DetachBufferQueue(streamId);
    if (!reply.WriteInt32(static_cast<int32_t>(ret))) {
        DHLOGE("Write retcode failed.");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t DStreamOperatorStub::DStreamOperatorStubCapture(MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    DHLOGI("DStreamOperatorStub::DStreamOperatorStubCapture entry.");
    if (data.ReadInterfaceToken() != DStreamOperatorStub::GetDescriptor()) {
        DHLOGE("invalid token.");
        return HDF_FAILURE;
    }

    int captureId = static_cast<int>(data.ReadInt32());

    std::vector<int32_t> streamIds;
    if (!data.ReadInt32Vector(&streamIds)) {
        DHLOGE("Write streamIds failed.");
        return HDF_FAILURE;
    }

    std::shared_ptr<CameraStandard::CameraMetadata> metadata = nullptr;
    CameraStandard::MetadataUtils::DecodeCameraMetadata(data, metadata);

    bool enableShutterCallback = data.ReadBool();
    std::shared_ptr<CaptureInfo> pInfo = std::make_shared<CaptureInfo>();
    pInfo->streamIds_ = streamIds;
    pInfo->captureSetting_ = metadata;
    pInfo->enableShutterCallback_ = enableShutterCallback;

    bool isStreaming = data.ReadBool();

    CamRetCode ret = Capture(captureId, pInfo, isStreaming);
    if (!reply.WriteInt32(static_cast<int32_t>(ret))) {
        DHLOGE("Write retcode failed.");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t DStreamOperatorStub::DStreamOperatorStubCancelCapture(MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    DHLOGI("DStreamOperatorStub::DStreamOperatorStubCancelCapture entry.");
    if (data.ReadInterfaceToken() != DStreamOperatorStub::GetDescriptor()) {
        DHLOGE("invalid token.");
        return HDF_FAILURE;
    }

    int32_t captureId = data.ReadInt32();
    CamRetCode ret = CancelCapture(captureId);
    if (!reply.WriteInt32(static_cast<int32_t>(ret))) {
        DHLOGE("Write retcode failed.");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t DStreamOperatorStub::DStreamOperatorStubChangeToOfflineStream(MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    DHLOGI("DStreamOperatorStub::DStreamOperatorStubChangeToOfflineStream entry.");
    if (data.ReadInterfaceToken() != DStreamOperatorStub::GetDescriptor()) {
        DHLOGE("invalid token.");
        return HDF_FAILURE;
    }

    std::vector<int32_t> streamIds;
    if (!data.ReadInt32Vector(&streamIds)) {
        DHLOGE("Read streamIds failed.");
        return HDF_FAILURE;
    }

    sptr<IRemoteObject> remoteObj = data.ReadRemoteObject();
    sptr<IStreamOperatorCallback> spStreamOperatorCallback = OHOS::iface_cast<IStreamOperatorCallback>(remoteObj);
    if (spStreamOperatorCallback == nullptr) {
        DHLOGE("Read operator callback failed.");
        return HDF_FAILURE;
    }

    OHOS::sptr<IOfflineStreamOperator> offlineOperator = nullptr;
    CamRetCode ret = ChangeToOfflineStream(streamIds, spStreamOperatorCallback, offlineOperator);
    if (!reply.WriteInt32(static_cast<int32_t>(ret))) {
        DHLOGE("%s: write retcode failed.");
        return HDF_FAILURE;
    }

    if (offlineOperator == nullptr) {
        DHLOGE("Change to offline stream failed, offlineOperator is null.");
        return HDF_FAILURE;
    }

    if (!reply.WriteRemoteObject(offlineOperator->AsObject())) {
        DHLOGE("Write offline stream operator failed.");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}
} // namespace DistributedHardware
} // namespace OHOS
