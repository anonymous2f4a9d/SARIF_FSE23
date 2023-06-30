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

#include "dcamera_provider_callback_stub.h"
#include <hdf_base.h>
#include "dcamera_provider_callback.h"
#include "distributed_hardware_log.h"
#include "ipc_data_utils.h"

namespace OHOS {
namespace DistributedHardware {
int32_t DCameraProviderCallbackStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    int32_t ret = HDF_SUCCESS;
    switch (code) {
        case CMD_DISTRIBUTED_CAMERA_PROVIDER_CALLBACK_OPEN_SESSION: {
            ret = DCProviderOpenSessionStub(data, reply, option);
            break;
        }
        case CMD_DISTRIBUTED_CAMERA_PROVIDER_CALLBACK_CLOSE_SESSION: {
            ret = DCProviderCloseSessionStub(data, reply, option);
            break;
        }
        case CMD_DISTRIBUTED_CAMERA_PROVIDER_CALLBACK_CONFIGURE_STREAMS: {
            ret = DCProviderConfigureStreamsStub(data, reply, option);
            break;
        }
        case CMD_DISTRIBUTED_CAMERA_PROVIDER_CALLBACK_RELEASE_STREAMS: {
            ret = DCProviderReleaseStreamsStub(data, reply, option);
            break;
        }
        case CMD_DISTRIBUTED_CAMERA_PROVIDER_CALLBACK_START_CAPTURE: {
            ret = DCProviderStartCaptureStub(data, reply, option);
            break;
        }
        case CMD_DISTRIBUTED_CAMERA_PROVIDER_CALLBACK_STOP_CAPTURE: {
            ret = DCProviderStopCaptureStub(data, reply, option);
            break;
        }
        case CMD_DISTRIBUTED_CAMERA_PROVIDER_CALLBACK_UPDATE_SETTINGS: {
            ret = DCProviderUpdateSettingsStub(data, reply, option);
            break;
        }
        default: {
            ret = IPCObjectStub::OnRemoteRequest(code, data, reply, option);
        }
    }
    return ret;
}

int32_t DCameraProviderCallbackStub::DCProviderOpenSessionStub(MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    if (data.ReadInterfaceToken() != DCameraProviderCallbackStub::GetDescriptor()) {
        DHLOGE("OnCaptureStarted invalid token.");
        return HDF_FAILURE;
    }

    std::shared_ptr<DHBase> dhBase = std::make_shared<DHBase>(data.ReadString(), data.ReadString());

    DCamRetCode ret = OpenSession(dhBase);
    if (!reply.WriteInt32(static_cast<int32_t>(ret))) {
        DHLOGE("Write retcode failed.");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t DCameraProviderCallbackStub::DCProviderCloseSessionStub(MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    if (data.ReadInterfaceToken() != DCameraProviderCallbackStub::GetDescriptor()) {
        DHLOGE("OnCaptureStarted invalid token.");
        return HDF_FAILURE;
    }

    std::shared_ptr<DHBase> dhBase = std::make_shared<DHBase>(data.ReadString(), data.ReadString());

    DCamRetCode ret = CloseSession(dhBase);
    if (!reply.WriteInt32(static_cast<int32_t>(ret))) {
        DHLOGE("Write retcode failed.");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t DCameraProviderCallbackStub::DCProviderConfigureStreamsStub(MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    if (data.ReadInterfaceToken() != DCameraProviderCallbackStub::GetDescriptor()) {
        DHLOGE("OnCaptureStarted invalid token.");
        return HDF_FAILURE;
    }

    std::shared_ptr<DHBase> dhBase = std::make_shared<DHBase>(data.ReadString(), data.ReadString());

    int32_t count = data.ReadInt32();
    std::vector<std::shared_ptr<DCStreamInfo>> streamInfos;
    for (int32_t i = 0; i < count; i++) {
        const DCStreamInfo *pInfo = reinterpret_cast<const DCStreamInfo *>(
            data.ReadBuffer(sizeof(DCStreamInfo)));
        if (pInfo == nullptr) {
            DHLOGE("Read distributed camera stream info failed.");
            return HDF_FAILURE;
        }
        std::shared_ptr<DCStreamInfo> streamInfo = std::make_shared<DCStreamInfo>();
        streamInfo->streamId_ = pInfo->streamId_;
        streamInfo->width_ = pInfo->width_;
        streamInfo->height_ = pInfo->height_;
        streamInfo->stride_ = pInfo->stride_;
        streamInfo->format_ = pInfo->format_;
        streamInfo->dataspace_ = pInfo->dataspace_;
        streamInfo->encodeType_ = pInfo->encodeType_;
        streamInfo->type_ = pInfo->type_;
        streamInfos.push_back(streamInfo);
    }

    DCamRetCode ret = ConfigureStreams(dhBase, streamInfos);
    if (!reply.WriteInt32(static_cast<int32_t>(ret))) {
        DHLOGE("Write retcode failed.");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t DCameraProviderCallbackStub::DCProviderReleaseStreamsStub(MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    if (data.ReadInterfaceToken() != DCameraProviderCallbackStub::GetDescriptor()) {
        DHLOGE("OnCaptureStarted invalid token.");
        return HDF_FAILURE;
    }

    std::shared_ptr<DHBase> dhBase = std::make_shared<DHBase>(data.ReadString(), data.ReadString());

    std::vector<int> streamIds;
    if (!data.ReadInt32Vector(&streamIds)) {
        DHLOGE("Read streamIds failed.");
        return HDF_FAILURE;
    }

    DCamRetCode ret = ReleaseStreams(dhBase, streamIds);
    if (!reply.WriteInt32(static_cast<int32_t>(ret))) {
        DHLOGE("Write retcode failed.");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t DCameraProviderCallbackStub::DCProviderStartCaptureStub(MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    if (data.ReadInterfaceToken() != DCameraProviderCallbackStub::GetDescriptor()) {
        DHLOGE("OnCaptureStarted invalid token.");
        return HDF_FAILURE;
    }

    std::shared_ptr<DHBase> dhBase = std::make_shared<DHBase>(data.ReadString(), data.ReadString());

    int32_t count = data.ReadInt32();
    std::vector<std::shared_ptr<DCCaptureInfo>> captureInfos;
    for (int32_t i = 0; i < count; i++) {
        std::shared_ptr<DCCaptureInfo> captureInfo = std::make_shared<DCCaptureInfo>();
        std::vector<int32_t> streamIds;
        if (!data.ReadInt32Vector(&streamIds)) {
            DHLOGE("Read streamIds failed.");
            return HDF_FAILURE;
        }
        captureInfo->streamIds_ = streamIds;
        captureInfo->width_ = static_cast<int>(data.ReadInt32());
        captureInfo->height_ = static_cast<int>(data.ReadInt32());
        captureInfo->stride_ = static_cast<int>(data.ReadInt32());
        captureInfo->format_ = static_cast<int>(data.ReadInt32());
        captureInfo->dataspace_ = static_cast<int>(data.ReadInt32());
        captureInfo->isCapture_ = data.ReadBool();
        captureInfo->encodeType_ = static_cast<DCEncodeType>(data.ReadInt32());
        captureInfo->type_ = static_cast<DCStreamType>(data.ReadInt32());

        int32_t settingsSize = data.ReadInt32();
        std::vector<std::shared_ptr<DCameraSettings>> capSettings;
        for (int32_t k = 0; k < settingsSize; k++) {
            std::shared_ptr<DCameraSettings> metadata = std::make_shared<DCameraSettings>();
            IpcDataUtils::DecodeDCameraSettings(data, metadata);
            capSettings.push_back(metadata);
        }
        captureInfo->captureSettings_ = capSettings;
        captureInfos.push_back(captureInfo);
    }

    DCamRetCode ret = StartCapture(dhBase, captureInfos);
    if (!reply.WriteInt32(static_cast<int32_t>(ret))) {
        DHLOGE("Write retcode failed.");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t DCameraProviderCallbackStub::DCProviderStopCaptureStub(MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    if (data.ReadInterfaceToken() != DCameraProviderCallbackStub::GetDescriptor()) {
        DHLOGE("OnCaptureStarted invalid token.");
        return HDF_FAILURE;
    }

    std::shared_ptr<DHBase> dhBase = std::make_shared<DHBase>(data.ReadString(), data.ReadString());

    DCamRetCode ret = StopCapture(dhBase);
    if (!reply.WriteInt32(static_cast<int32_t>(ret))) {
        DHLOGE("Write retcode failed.");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t DCameraProviderCallbackStub::DCProviderUpdateSettingsStub(MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    if (data.ReadInterfaceToken() != DCameraProviderCallbackStub::GetDescriptor()) {
        DHLOGE("OnCaptureStarted invalid token.");
        return HDF_FAILURE;
    }

    std::shared_ptr<DHBase> dhBase = std::make_shared<DHBase>(data.ReadString(), data.ReadString());

    int32_t count = data.ReadInt32();
    std::vector<std::shared_ptr<DCameraSettings>> settings;
    for (int32_t i = 0; i < count; i++) {
        std::shared_ptr<DCameraSettings> metadata = std::make_shared<DCameraSettings>();
        IpcDataUtils::DecodeDCameraSettings(data, metadata);
        settings.push_back(metadata);
    }

    DCamRetCode ret = UpdateSettings(dhBase, settings);
    if (!reply.WriteInt32(static_cast<int32_t>(ret))) {
        DHLOGE("Write retcode failed.");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}
} // namespace DistributedHardware
} // namespace OHOS
