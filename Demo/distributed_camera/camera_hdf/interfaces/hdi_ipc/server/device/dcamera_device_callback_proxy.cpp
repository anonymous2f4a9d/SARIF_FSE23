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

#include "dcamera_device_callback_proxy.h"
#include <hdf_base.h>
#include <message_parcel.h>
#include "distributed_hardware_log.h"
#include "ipc_data_utils.h"
#include "metadata_utils.h"

#ifndef BALTIMORE_CAMERA
#include "cmd_common.h"
#endif

namespace OHOS {
namespace DistributedHardware {
void DCameraDeviceCallbackProxy::OnError(ErrorType type, int32_t errorMsg)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(DCameraDeviceCallbackProxy::GetDescriptor())) {
        DHLOGE("Write token failed.");
        return;
    }

    if (!data.WriteUint32(type)) {
        DHLOGE("Write error type failed.");
        return;
    }

    if (!data.WriteInt32(errorMsg)) {
        DHLOGE("Write error message failed.");
        return;
    }

    int32_t ret = Remote()->SendRequest(CMD_CAMERA_DEVICE_CALLBACK_ON_ERROR, data, reply, option);
    if (ret != HDF_SUCCESS) {
        DHLOGE("SendRequest failed, error code is %d.", ret);
    }
}

void DCameraDeviceCallbackProxy::OnResult(uint64_t timestamp,
    const std::shared_ptr<CameraStandard::CameraMetadata> &result)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(DCameraDeviceCallbackProxy::GetDescriptor())) {
        DHLOGE("Write token failed.");
        return;
    }

    if (result == nullptr) {
        return;
    }

    if (!data.WriteUint64(timestamp)) {
        DHLOGE("Write timestamp failed.");
        return;
    }

    if (!CameraStandard::MetadataUtils::EncodeCameraMetadata(result, data)) {
        DHLOGE("Write metadata failed.");
        return;
    }

    int32_t ret = Remote()->SendRequest(CMD_CAMERA_DEVICE_CALLBACK_ON_RESULT, data, reply, option);
    if (ret != HDF_SUCCESS) {
        DHLOGE("SendRequest failed, error code is %d.", ret);
        return;
    }
}
} // namespace DistributedHardware
} // namespace OHOS
