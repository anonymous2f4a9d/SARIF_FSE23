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

#include "dcamera_host_callback_stub.h"
#include "dcamera_host_callback.h"
#include "distributed_hardware_log.h"

#include <hdf_base.h>

#ifndef BALTIMORE_CAMERA
#include "cmd_common.h"
#endif

namespace OHOS {
namespace DistributedHardware {
int32_t DCameraHostCallbackStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    switch (code) {
        case CMD_CAMERA_HOST_CALLBACK_ON_STATUS: {
            DHLOGI("DCameraHostCallbackStub::OnCameraStatus entry.");
            if (data.ReadInterfaceToken() != DCameraHostCallbackStub::GetDescriptor()) {
                DHLOGI("DCameraDeviceCallbackStub::OnCameraStatus token failed.");
                return HDF_FAILURE;
            }

            std::string cameraId = data.ReadString();
            CameraStatus status = static_cast<CameraStatus>(data.ReadInt32());
            OnCameraStatus(cameraId, status);
            break;
        }
        case CMD_CAMERA_HOST_CALLBACK_ON_FLASHLIGHT_STATUS: {
            DHLOGI("DCameraHostCallbackStub::OnFlashlightStatus entry.");
            if (data.ReadInterfaceToken() != DCameraHostCallbackStub::GetDescriptor()) {
                DHLOGI("DCameraDeviceCallbackStub::OnFlashlightStatus token failed.");
                return HDF_FAILURE;
            }

            std::string cameraId = data.ReadString();
            FlashlightStatus status = static_cast<FlashlightStatus>(data.ReadInt32());
            OnFlashlightStatus(cameraId, status);
            break;
        }
        case CMD_CAMERA_HOST_CALLBACK_ON_CAMERA_EVENT: {
            DHLOGI("DCameraHostCallbackStub::OnCameraEvent entry.");
            if (data.ReadInterfaceToken() != DCameraHostCallbackStub::GetDescriptor()) {
                DHLOGI("DCameraDeviceCallbackStub::OnCameraEvent token failed.");
                return HDF_FAILURE;
            }

            std::string cameraId = data.ReadString();
            CameraEvent event = static_cast<CameraEvent>(data.ReadInt32());
            OnCameraEvent(cameraId, event);
            break;
        }
        default: {
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
        }
    }
    return 0;
}
} // namespace DistributedHardware
} // namespace OHOS
