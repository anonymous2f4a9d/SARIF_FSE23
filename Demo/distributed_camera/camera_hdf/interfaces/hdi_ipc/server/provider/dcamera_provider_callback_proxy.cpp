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

#include "dcamera_provider_callback_proxy.h"
#include <hdf_base.h>
#include <message_parcel.h>
#include "distributed_hardware_log.h"
#include "ipc_data_utils.h"

namespace OHOS {
namespace DistributedHardware {
DCamRetCode DCameraProviderCallbackProxy::OpenSession(const std::shared_ptr<DHBase> &dhBase)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(DCameraProviderCallbackProxy::GetDescriptor())) {
        DHLOGE("Write token failed.");
        return DCamRetCode::INVALID_ARGUMENT;
    }

    if (!data.WriteString(dhBase->deviceId_) || !data.WriteString(dhBase->dhId_)) {
        DHLOGE("Write distributed camera base info or ability info failed.");
        return DCamRetCode::INVALID_ARGUMENT;
    }

    int32_t ret = Remote()->SendRequest(CMD_DISTRIBUTED_CAMERA_PROVIDER_CALLBACK_OPEN_SESSION, data, reply, option);
    if (ret != HDF_SUCCESS) {
        DHLOGE("SendRequest failed, error code is %d.", ret);
        return INVALID_ARGUMENT;
    }
    return static_cast<DCamRetCode>(reply.ReadInt32());
}

DCamRetCode DCameraProviderCallbackProxy::CloseSession(const std::shared_ptr<DHBase> &dhBase)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(DCameraProviderCallbackProxy::GetDescriptor())) {
        DHLOGE("Write token failed.");
        return DCamRetCode::INVALID_ARGUMENT;
    }

    if (!data.WriteString(dhBase->deviceId_) || !data.WriteString(dhBase->dhId_)) {
        DHLOGE("Write distributed camera base info or ability info failed.");
        return DCamRetCode::INVALID_ARGUMENT;
    }

    int32_t ret = Remote()->SendRequest(CMD_DISTRIBUTED_CAMERA_PROVIDER_CALLBACK_CLOSE_SESSION, data, reply, option);
    if (ret != HDF_SUCCESS) {
        DHLOGE("SendRequest failed, error code is %d.", ret);
        return INVALID_ARGUMENT;
    }
    return static_cast<DCamRetCode>(reply.ReadInt32());
}

DCamRetCode DCameraProviderCallbackProxy::ConfigureStreams(const std::shared_ptr<DHBase> &dhBase,
    const std::vector<std::shared_ptr<DCStreamInfo>> &streamInfos)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(DCameraProviderCallbackProxy::GetDescriptor())) {
        DHLOGE("Write token failed.");
        return DCamRetCode::INVALID_ARGUMENT;
    }

    if (!data.WriteString(dhBase->deviceId_) || !data.WriteString(dhBase->dhId_)) {
        DHLOGE("Write distributed camera base info or ability info failed.");
        return DCamRetCode::INVALID_ARGUMENT;
    }

    size_t size = streamInfos.size();
    if (!data.WriteInt32(static_cast<int32_t>(size))) {
        DHLOGE("Write stream info size failed.");
        return INVALID_ARGUMENT;
    }
    for (size_t i = 0; i < size; i++) {
        auto streamInfo = streamInfos.at(i);
        bool bRet = data.WriteBuffer((void *)streamInfo.get(), sizeof(DCStreamInfo));
        if (!bRet) {
            DHLOGE("Write stream info index = %d failed.", i);
            return INVALID_ARGUMENT;
        }
    }

    int32_t ret = Remote()->SendRequest(CMD_DISTRIBUTED_CAMERA_PROVIDER_CALLBACK_CONFIGURE_STREAMS,
        data, reply, option);
    if (ret != HDF_SUCCESS) {
        DHLOGE("SendRequest failed, error code is %d.", ret);
        return INVALID_ARGUMENT;
    }
    return static_cast<DCamRetCode>(reply.ReadInt32());
}

DCamRetCode DCameraProviderCallbackProxy::ReleaseStreams(const std::shared_ptr<DHBase> &dhBase,
    const std::vector<int> &streamIds)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(DCameraProviderCallbackProxy::GetDescriptor())) {
        DHLOGE("Write token failed.");
        return DCamRetCode::INVALID_ARGUMENT;
    }

    if (!data.WriteString(dhBase->deviceId_) || !data.WriteString(dhBase->dhId_)) {
        DHLOGE("Write distributed camera base info or ability info failed.");
        return DCamRetCode::INVALID_ARGUMENT;
    }

    std::vector<int32_t> pxyStreamIds = streamIds;
    if (!data.WriteInt32Vector(pxyStreamIds)) {
        DHLOGE("Write streamId failed.");
        return INVALID_ARGUMENT;
    }

    int32_t ret = Remote()->SendRequest(CMD_DISTRIBUTED_CAMERA_PROVIDER_CALLBACK_RELEASE_STREAMS,
        data, reply, option);
    if (ret != HDF_SUCCESS) {
        DHLOGE("SendRequest failed, error code is %d", ret);
        return INVALID_ARGUMENT;
    }
    return static_cast<DCamRetCode>(reply.ReadInt32());
}

DCamRetCode DCameraProviderCallbackProxy::StartCapture(const std::shared_ptr<DHBase> &dhBase,
    const std::vector<std::shared_ptr<DCCaptureInfo>> &captureInfos)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(DCameraProviderCallbackProxy::GetDescriptor())) {
        DHLOGE("Write token failed.");
        return DCamRetCode::INVALID_ARGUMENT;
    }

    if (!data.WriteString(dhBase->deviceId_) || !data.WriteString(dhBase->dhId_)) {
        DHLOGE("Write distributed camera base info or ability info failed.");
        return DCamRetCode::INVALID_ARGUMENT;
    }

    size_t size = captureInfos.size();
    if (!data.WriteInt32(static_cast<int32_t>(size))) {
        DHLOGE("Write distributed camera capture info size failed.");
        return INVALID_ARGUMENT;
    }
    for (size_t i = 0; i < size; i++) {
        auto captureInfo = captureInfos.at(i);
        if (!data.WriteInt32Vector(captureInfo->streamIds_)) {
            DHLOGE("Write streamIds vector failed.");
            return INVALID_ARGUMENT;
        }
        data.WriteInt32(static_cast<int32_t>(captureInfo->width_));
        data.WriteInt32(static_cast<int32_t>(captureInfo->height_));
        data.WriteInt32(static_cast<int32_t>(captureInfo->stride_));
        data.WriteInt32(static_cast<int32_t>(captureInfo->format_));
        data.WriteInt32(static_cast<int32_t>(captureInfo->dataspace_));
        data.WriteBool(static_cast<int32_t>(captureInfo->isCapture_));
        data.WriteInt32(static_cast<int32_t>(captureInfo->encodeType_));
        data.WriteInt32(static_cast<int32_t>(captureInfo->type_));

        std::vector<std::shared_ptr<DCameraSettings>> capSettings = captureInfo->captureSettings_;
        size_t settingsSize = capSettings.size();
        if (!data.WriteInt32(static_cast<int32_t>(settingsSize))) {
            DHLOGE("Write distributed camera capture settings size failed.");
            return INVALID_ARGUMENT;
        }
        for (size_t k = 0; k < settingsSize; k++) {
            bool bRet = IpcDataUtils::EncodeDCameraSettings(capSettings.at(k), data);
            if (!bRet) {
                DHLOGE("Write distributed camera capture settings failed.");
                return INVALID_ARGUMENT;
            }
        }
    }

    int32_t ret = Remote()->SendRequest(CMD_DISTRIBUTED_CAMERA_PROVIDER_CALLBACK_START_CAPTURE, data, reply, option);
    if (ret != HDF_SUCCESS) {
        DHLOGE("SendRequest failed, error code is %d", ret);
        return INVALID_ARGUMENT;
    }
    return static_cast<DCamRetCode>(reply.ReadInt32());
}

DCamRetCode DCameraProviderCallbackProxy::StopCapture(const std::shared_ptr<DHBase> &dhBase)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(DCameraProviderCallbackProxy::GetDescriptor())) {
        DHLOGE("Write token failed.");
        return DCamRetCode::INVALID_ARGUMENT;
    }

    if (!data.WriteString(dhBase->deviceId_) || !data.WriteString(dhBase->dhId_)) {
        DHLOGE("Write distributed camera base info or ability info failed.");
        return DCamRetCode::INVALID_ARGUMENT;
    }

    int32_t ret = Remote()->SendRequest(CMD_DISTRIBUTED_CAMERA_PROVIDER_CALLBACK_STOP_CAPTURE, data, reply, option);
    if (ret != HDF_SUCCESS) {
        DHLOGE("SendRequest failed, error code is %d.", ret);
        return INVALID_ARGUMENT;
    }
    return static_cast<DCamRetCode>(reply.ReadInt32());
}

DCamRetCode DCameraProviderCallbackProxy::UpdateSettings(const std::shared_ptr<DHBase> &dhBase,
    const std::vector<std::shared_ptr<DCameraSettings>> &settings)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(DCameraProviderCallbackProxy::GetDescriptor())) {
        DHLOGE("Write token failed.");
        return DCamRetCode::INVALID_ARGUMENT;
    }

    if (!data.WriteString(dhBase->deviceId_) || !data.WriteString(dhBase->dhId_)) {
        DHLOGE("Write distributed camera base info or ability info failed.");
        return DCamRetCode::INVALID_ARGUMENT;
    }

    size_t size = settings.size();
    if (!data.WriteInt32(static_cast<int32_t>(size))) {
        DHLOGE("Write distributed camera settings size failed.");
        return INVALID_ARGUMENT;
    }
    for (size_t i = 0; i < size; i++) {
        bool bRet = IpcDataUtils::EncodeDCameraSettings(settings.at(i), data);
        if (!bRet) {
            DHLOGE("Write distributed camera settings index = %d failed.", i);
            return INVALID_ARGUMENT;
        }
    }

    int32_t ret = Remote()->SendRequest(CMD_DISTRIBUTED_CAMERA_PROVIDER_CALLBACK_UPDATE_SETTINGS,
        data, reply, option);
    if (ret != HDF_SUCCESS) {
        DHLOGE("SendRequest failed, error code is %d.", ret);
        return INVALID_ARGUMENT;
    }
    return static_cast<DCamRetCode>(reply.ReadInt32());
}
} // namespace DistributedHardware
} // namespace OHOS
