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

#include "dstream_operator_callback_proxy.h"
#include <hdf_base.h>
#include <message_parcel.h>
#include "distributed_hardware_log.h"

#ifndef BALTIMORE_CAMERA
#include "cmd_common.h"
#endif

namespace OHOS {
namespace DistributedHardware {
void DStreamOperatorCallbackProxy::OnCaptureStarted(int32_t captureId, const std::vector<int32_t> &streamId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(DStreamOperatorCallbackProxy::GetDescriptor())) {
        DHLOGE("Write token failed.");
        return;
    }

    if (!data.WriteInt32(captureId)) {
        DHLOGE("Write captureId failed.");
        return;
    }

    if (!data.WriteInt32Vector(streamId)) {
        DHLOGE("Write streamIds failed.");
        return;
    }

    int32_t ret = Remote()->SendRequest(CMD_STREAM_OPERATOR_CALLBACK_ON_CAPTURE_STARTED, data, reply, option);
    if (ret != HDF_SUCCESS) {
        DHLOGE("SendRequest failed, error code is %d.", ret);
    }
}

void DStreamOperatorCallbackProxy::OnCaptureEnded(int32_t captureId,
    const std::vector<std::shared_ptr<CaptureEndedInfo>> &info)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(DStreamOperatorCallbackProxy::GetDescriptor())) {
        DHLOGE("Write token failed.");
        return;
    }

    if (!data.WriteInt32(captureId)) {
        DHLOGE("Write captureId failed.");
        return;
    }

    size_t size = info.size();
    if (!data.WriteInt32(static_cast<int32_t>(size))) {
        DHLOGE("Write info size failed.");
        return;
    }
    for (size_t i = 0; i < size; i++) {
        auto captureEndInfo = info.at(i);
        bool bRet = data.WriteBuffer((void *)captureEndInfo.get(), sizeof(CaptureEndedInfo));
        if (!bRet) {
            DHLOGE("Write info index = %d failed.", i);
            return;
        }
    }

    int32_t ret = Remote()->SendRequest(CMD_STREAM_OPERATOR_CALLBACK_ON_CAPTURE_ENDED, data, reply, option);
    if (ret != HDF_SUCCESS) {
        DHLOGE("SendRequest failed, error code is %d.", ret);
        return;
    }
}

void DStreamOperatorCallbackProxy::OnCaptureError(int32_t captureId,
    const std::vector<std::shared_ptr<CaptureErrorInfo>> &info)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(DStreamOperatorCallbackProxy::GetDescriptor())) {
        DHLOGE("Write token failed.");
        return;
    }

    if (!data.WriteInt32(captureId)) {
        DHLOGE("Write captureId failed.");
        return;
    }

    size_t size = info.size();
    if (!data.WriteInt32(static_cast<int32_t>(size))) {
        DHLOGE("Write info size failed.");
        return;
    }
    for (size_t i = 0; i < size; i++) {
        auto captureErrorInfo = info.at(i);
        bool bRet = data.WriteBuffer((void *)captureErrorInfo.get(), sizeof(CaptureErrorInfo));
        if (!bRet) {
            DHLOGE("Write info index = %d failed.", i);
            return;
        }
    }

    int32_t ret = Remote()->SendRequest(CMD_STREAM_OPERATOR_CALLBACK_ON_CAPTURE_ERROR, data, reply, option);
    if (ret != HDF_SUCCESS) {
        DHLOGE("SendRequest failed, error code is %d.", ret);
        return;
    }
}

void DStreamOperatorCallbackProxy::OnFrameShutter(int32_t captureId,
    const std::vector<int32_t> &streamId, uint64_t timestamp)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(DStreamOperatorCallbackProxy::GetDescriptor())) {
        DHLOGE("Write token failed.");
        return;
    }

    if (!data.WriteInt32(captureId)) {
        DHLOGE("Write captureId failed.");
        return;
    }

    if (!data.WriteInt32Vector(streamId)) {
        DHLOGE("Write streamId failed.");
        return;
    }

    if (!data.WriteUint64(timestamp)) {
        DHLOGE("Write streamId failed.");
        return;
    }

    int32_t ret = Remote()->SendRequest(CMD_STREAM_OPERATOR_CALLBACK_ON_FRAME_SHUTTER, data, reply, option);
    if (ret != HDF_SUCCESS) {
        DHLOGE("SendRequest failed, error code is %d.", ret);
    }
}
} // namespace DistributedHardware
} // namespace OHOS
