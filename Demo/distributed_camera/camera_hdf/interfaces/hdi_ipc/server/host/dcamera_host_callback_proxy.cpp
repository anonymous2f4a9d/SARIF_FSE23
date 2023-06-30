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

#include "dcamera_host_callback_proxy.h"
#include <hdf_base.h>
#include <message_parcel.h>
#include "distributed_hardware_log.h"

#ifndef BALTIMORE_CAMERA
#include "cmd_common.h"
#endif

namespace OHOS {
namespace DistributedHardware {
void DCameraHostCallbackProxy::OnCameraStatus(const std::string &cameraId, CameraStatus status)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(DCameraHostCallbackProxy::GetDescriptor())) {
        DHLOGE("Write token failed.");
        return;
    }

    if (!data.WriteString(cameraId)) {
        DHLOGE("Write cameraId failed.");
        return;
    }

    if (!data.WriteInt32(status)) {
        DHLOGE("Write status failed.");
        return;
    }

    int32_t ret = Remote()->SendRequest(CMD_CAMERA_HOST_CALLBACK_ON_STATUS, data, reply, option);
    if (ret != HDF_SUCCESS) {
        DHLOGE("SendRequest failed, error code is %d.", ret);
    }
}

void DCameraHostCallbackProxy::OnFlashlightStatus(const std::string &cameraId, FlashlightStatus status)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(DCameraHostCallbackProxy::GetDescriptor())) {
        DHLOGE("Write token failed.");
        return;
    }

    if (!data.WriteString(cameraId)) {
        DHLOGE("Write cameraId failed.");
        return;
    }

    if (!data.WriteInt32(status)) {
        DHLOGE("Write status failed.");
        return;
    }

    int32_t ret = Remote()->SendRequest(CMD_CAMERA_HOST_CALLBACK_ON_FLASHLIGHT_STATUS, data, reply, option);
    if (ret != HDF_SUCCESS) {
        DHLOGE("SendRequest failed, error code is %d.", ret);
    }
}

void DCameraHostCallbackProxy::OnCameraEvent(const std::string &cameraId, CameraEvent event)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(DCameraHostCallbackProxy::GetDescriptor())) {
        DHLOGE("Write token failed.");
        return;
    }

    if (!data.WriteString(cameraId)) {
        DHLOGE("Write cameraId failed.");
        return;
    }

    if (!data.WriteInt32(event)) {
        DHLOGE("Write event failed.");
        return;
    }

    int32_t ret = Remote()->SendRequest(CMD_CAMERA_HOST_CALLBACK_ON_CAMERA_EVENT, data, reply, option);
    if (ret != HDF_SUCCESS) {
        DHLOGE("SendRequest failed, error code is %d.", ret);
    }
}
} // namespace DistributedHardware
} // namespace OHOS
