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

#include "doffline_stream_operator_proxy.h"
#include <hdf_base.h>
#include <message_parcel.h>
#include "distributed_hardware_log.h"

namespace OHOS {
namespace DistributedHardware {
CamRetCode DOfflineStreamOperatorProxy::CancelCapture(int captureId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(DOfflineStreamOperatorProxy::GetDescriptor())) {
        DHLOGE("Write token failed.");
        return CamRetCode::INVALID_ARGUMENT;
    }

    if (!data.WriteInt32(static_cast<int32_t>(captureId))) {
        DHLOGE("Write captureId object failed.");
        return CamRetCode::INVALID_ARGUMENT;
    }

    int32_t ret = Remote()->SendRequest(CMD_OFFLINE_STREAM_OPERATOR_CANCEL_CAPTURE, data, reply, option);
    if (ret != HDF_SUCCESS) {
        DHLOGE("SendRequest failed, error code = %d.", ret);
        return CamRetCode::INVALID_ARGUMENT;
    }
    return static_cast<CamRetCode>(reply.ReadInt32());
}

CamRetCode DOfflineStreamOperatorProxy::ReleaseStreams(const std::vector<int> &streamIds)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(DOfflineStreamOperatorProxy::GetDescriptor())) {
        DHLOGE("Write token failed.");
        return CamRetCode::INVALID_ARGUMENT;
    }

    std::vector<int32_t> pxyStreamIds = streamIds;
    if (!data.WriteInt32Vector(pxyStreamIds)) {
        DHLOGE("Write streamIds object failed.");
        return CamRetCode::INVALID_ARGUMENT;
    }

    int32_t ret = Remote()->SendRequest(CMD_OFFLINE_STREAM_OPERATOR_RELEASE_STREAMS, data, reply, option);
    if (ret != HDF_SUCCESS) {
        DHLOGE("SendRequest failed, error code = %d.", ret);
        return CamRetCode::INVALID_ARGUMENT;
    }
    return static_cast<CamRetCode>(reply.ReadInt32());
}

CamRetCode DOfflineStreamOperatorProxy::Release()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(DOfflineStreamOperatorProxy::GetDescriptor())) {
        DHLOGE("Write token failed.");
        return CamRetCode::INVALID_ARGUMENT;
    }

    int32_t ret = Remote()->SendRequest(CMD_OFFLINE_STREAM_OPERATOR_RELEASE, data, reply, option);
    if (ret != HDF_SUCCESS) {
        DHLOGE("SendRequest failed, error code = %d.", ret);
        return CamRetCode::INVALID_ARGUMENT;
    }
    return static_cast<CamRetCode>(reply.ReadInt32());
}
} // namespace DistributedHardware
} // namespace OHOS
