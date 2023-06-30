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

#include "dcamera_host_proxy.h"
#include <hdf_base.h>
#include <hdf_log.h>
#include <message_parcel.h>
#include "distributed_hardware_log.h"
#include "icamera_device.h"
#include "icamera_device_callback.h"
#include "icamera_host_callback.h"
#include "ipc_data_utils.h"
#include "metadata_utils.h"

#ifndef BALTIMORE_CAMERA
#include "cmd_common.h"
#endif

OHOS::sptr<OHOS::Camera::ICameraHost> OHOS::Camera::ICameraHost::Get(const char *serviceName)
{
    using namespace OHOS::HDI::ServiceManager::V1_0;
    OHOS::sptr<IServiceManager> serviceMgr = IServiceManager::Get();
    if (serviceMgr == nullptr) {
        HDF_LOGE("%{public}s: IServiceManager failed!", __func__);
        return nullptr;
    }

    OHOS::sptr<IRemoteObject> remote = serviceMgr->GetService(serviceName);
    if (remote == nullptr) {
        HDF_LOGE("%{public}s: get %{public}s failed!", __func__, serviceName);
        return nullptr;
    }

    return iface_cast<OHOS::DistributedHardware::DCameraHostProxy>(remote);
}

namespace OHOS {
namespace DistributedHardware {
CamRetCode DCameraHostProxy::SetCallback(const OHOS::sptr<ICameraHostCallback> &callback)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(DCameraHostProxy::GetDescriptor())) {
        DHLOGE("Write remote token failed.");
        return CamRetCode::INVALID_ARGUMENT;
    }

    bool callbackFlag = (callback != nullptr);
    if (!data.WriteBool(callbackFlag)) {
        DHLOGE("Set callback flag failed.");
        return CamRetCode::INVALID_ARGUMENT;
    }

    if (callbackFlag && !data.WriteRemoteObject(callback->AsObject())) {
        DHLOGE("Write remote callback object failed.");
        return CamRetCode::INVALID_ARGUMENT;
    }

    int32_t ret = Remote()->SendRequest(CMD_CAMERA_HOST_REMOTE_SET_CALLBACK, data, reply, option);
    if (ret != HDF_SUCCESS) {
        DHLOGE("SendRequest failed, error code = %d.", ret);
        return CamRetCode::INVALID_ARGUMENT;
    }
    return static_cast<CamRetCode>(reply.ReadInt32());
}

CamRetCode DCameraHostProxy::GetCameraIds(std::vector<std::string> &cameraIds)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(DCameraHostProxy::GetDescriptor())) {
        DHLOGE("Write remote token failed.");
        return CamRetCode::INVALID_ARGUMENT;
    }

    int32_t ret = Remote()->SendRequest(CMD_CAMERA_HOST_REMOTE_GET_CAMERAID, data, reply, option);
    if (ret != HDF_SUCCESS) {
        DHLOGE("SendRequest failed, error code = %d.", ret);
        return CamRetCode::INVALID_ARGUMENT;
    }

    CamRetCode retCode = static_cast<CamRetCode>(reply.ReadInt32());
    if (retCode == CamRetCode::NO_ERROR && !reply.ReadStringVector(&cameraIds)) {
        DHLOGE("Read camera ids failed.");
        return CamRetCode::INVALID_ARGUMENT;
    }
    return retCode;
}

CamRetCode DCameraHostProxy::GetCameraAbility(const std::string &cameraId,
    std::shared_ptr<CameraAbility> &ability)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(DCameraHostProxy::GetDescriptor())) {
        DHLOGE("Write remote token failed.");
        return CamRetCode::INVALID_ARGUMENT;
    }

    if (!data.WriteString(cameraId)) {
        DHLOGE("Write cameraId failed.");
        return CamRetCode::INVALID_ARGUMENT;
    }

    int32_t ret = Remote()->SendRequest(CMD_CAMERA_HOST_REMOTE_GET_CAMERA_ABILITY, data, reply, option);
    if (ret != HDF_SUCCESS) {
        DHLOGE("SendRequest failed, error code = %d.", ret);
        return CamRetCode::INVALID_ARGUMENT;
    }

    CamRetCode retCode = static_cast<CamRetCode>(reply.ReadInt32());
    if (retCode == CamRetCode::NO_ERROR) {
        CameraStandard::MetadataUtils::DecodeCameraMetadata(reply, ability);
    }
    return retCode;
}

CamRetCode DCameraHostProxy::OpenCamera(const std::string &cameraId,
    const OHOS::sptr<ICameraDeviceCallback> &callback, OHOS::sptr<ICameraDevice> &pDevice)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(DCameraHostProxy::GetDescriptor())) {
        DHLOGE("Write remote token failed.");
        return CamRetCode::INVALID_ARGUMENT;
    }

    if (!data.WriteString(cameraId)) {
        DHLOGE("Write cameraId failed.");
        return CamRetCode::INVALID_ARGUMENT;
    }

    bool callbackFlag = (callback != nullptr);
    if (!data.WriteBool(callbackFlag)) {
        DHLOGE("Write camera callback flag failed.");
        return CamRetCode::INVALID_ARGUMENT;
    }

    if (callbackFlag && !data.WriteRemoteObject(callback->AsObject())) {
        DHLOGE("Write camera device callback failed.");
        return CamRetCode::INVALID_ARGUMENT;
    }

    int32_t ret = Remote()->SendRequest(CMD_CAMERA_HOST_REMOTE_OPEN_CAMERA, data, reply, option);
    if (ret != HDF_SUCCESS) {
        DHLOGE("SendRequest failed, error code = %d.", ret);
        return CamRetCode::INVALID_ARGUMENT;
    }

    CamRetCode retCode = static_cast<CamRetCode>(reply.ReadInt32());
    bool flag = reply.ReadBool();
    if (flag) {
        sptr<IRemoteObject> remoteCameraDevice = reply.ReadRemoteObject();
        if (remoteCameraDevice == nullptr) {
            DHLOGE("Read remote camera device is null.");
        }
        pDevice = OHOS::iface_cast<ICameraDevice>(remoteCameraDevice);
    }
    return retCode;
}

CamRetCode DCameraHostProxy::SetFlashlight(const std::string &cameraId, bool &isEnable)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(DCameraHostProxy::GetDescriptor())) {
        DHLOGE("Write remote token failed.");
        return CamRetCode::INVALID_ARGUMENT;
    }

    if (!data.WriteString(cameraId)) {
        DHLOGE("Write cameraId failed.");
        return CamRetCode::INVALID_ARGUMENT;
    }

    if (!data.WriteBool(isEnable)) {
        DHLOGE("Write isEnable failed.");
        return CamRetCode::INVALID_ARGUMENT;
    }

    int32_t ret = Remote()->SendRequest(CMD_CAMERA_HOST_REMOTE_SET_FLASH_LIGHT, data, reply, option);
    if (ret != HDF_SUCCESS) {
        DHLOGE("SendRequest failed, error code = %d.", ret);
        return CamRetCode::INVALID_ARGUMENT;
    }
    return static_cast<CamRetCode>(reply.ReadInt32());
}
} // namespace DistributedHardware
} // namespace OHOS
