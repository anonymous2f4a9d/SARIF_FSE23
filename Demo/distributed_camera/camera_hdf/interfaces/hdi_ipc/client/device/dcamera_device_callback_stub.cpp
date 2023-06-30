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

#include "dcamera_device_callback_stub.h"
#include "dcamera_device_callback.h"
#include "distributed_hardware_log.h"
#include "ipc_data_utils.h"
#include "metadata_utils.h"

#include <hdf_base.h>

#ifndef BALTIMORE_CAMERA
#include "cmd_common.h"
#endif

namespace OHOS {
namespace DistributedHardware {
int32_t DCameraDeviceCallbackStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    switch (code) {
        case CMD_CAMERA_DEVICE_CALLBACK_ON_ERROR: {
            if (data.ReadInterfaceToken() != DCameraDeviceCallbackStub::GetDescriptor()) {
                DHLOGI("DCameraDeviceCallbackStub::OnError token failed.");
                return HDF_FAILURE;
            }

            ErrorType type = static_cast<ErrorType>(data.ReadUint32());
            int32_t errorMsg = data.ReadInt32();
            DHLOGI("DCameraDeviceCallbackStub::OnError entry.");
            OnError(type, errorMsg);
            break;
        }
        case CMD_CAMERA_DEVICE_CALLBACK_ON_RESULT: {
            if (data.ReadInterfaceToken() != DCameraDeviceCallbackStub::GetDescriptor()) {
                DHLOGI("DCameraDeviceCallbackStub::OnResult token failed.");
                return HDF_FAILURE;
            }

            uint64_t timestamp = data.ReadUint64();
            std::shared_ptr<CameraStandard::CameraMetadata> result = nullptr;
            CameraStandard::MetadataUtils::DecodeCameraMetadata(data, result);
            DHLOGI("DCameraDeviceCallbackStub::OnResult entry.");
            OnResult(timestamp, result);
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
