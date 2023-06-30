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

#include "doffline_stream_operator_stub.h"
#include <hdf_base.h>
#include "distributed_hardware_log.h"

#ifndef BALTIMORE_CAMERA
#include "cmd_common.h"
#endif

namespace OHOS {
namespace DistributedHardware {
int32_t DOfflineStreamOperatorStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    int32_t ret = HDF_SUCCESS;
    switch (code) {
        case CMD_OFFLINE_STREAM_OPERATOR_CANCEL_CAPTURE: {
            ret = DOfflineStreamOperatorStubCancelCapture(data, reply, option);
            break;
        }
        case CMD_OFFLINE_STREAM_OPERATOR_RELEASE_STREAMS: {
            ret = DOfflineStreamOperatorStubReleaseStreams(data, reply, option);
            break;
        }
        case CMD_OFFLINE_STREAM_OPERATOR_RELEASE: {
            ret = DOfflineStreamOperatorStubRelease(data, reply, option);
            break;
        }
        default: {
            ret = IPCObjectStub::OnRemoteRequest(code, data, reply, option);
        }
    }
    return ret;
}

int32_t DOfflineStreamOperatorStub::DOfflineStreamOperatorStubCancelCapture(MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    DHLOGI("DOfflineStreamOperatorStub::DOfflineStreamOperatorStubCancelCapture entry.");
    if (data.ReadInterfaceToken() != DOfflineStreamOperatorStub::GetDescriptor()) {
        DHLOGE("invalid token.");
        return HDF_FAILURE;
    }

    int32_t captureId = data.ReadInt32();
    CamRetCode ret = CancelCapture(captureId);
    if (reply.WriteInt32(static_cast<int32_t>(ret))) {
        DHLOGE("Write retcode failed.");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t DOfflineStreamOperatorStub::DOfflineStreamOperatorStubReleaseStreams(MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    DHLOGI("DOfflineStreamOperatorStub::DOfflineStreamOperatorStubReleaseStreams entry.");
    if (data.ReadInterfaceToken() != DOfflineStreamOperatorStub::GetDescriptor()) {
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

int32_t DOfflineStreamOperatorStub::DOfflineStreamOperatorStubRelease(MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    DHLOGI("DOfflineStreamOperatorStub::DOfflineStreamOperatorStubRelease entry.");
    if (data.ReadInterfaceToken() != DOfflineStreamOperatorStub::GetDescriptor()) {
        DHLOGE("invalid token.");
        return HDF_FAILURE;
    }

    CamRetCode ret = Release();
    if (!reply.WriteInt32(static_cast<int32_t>(ret))) {
        DHLOGE("%s: write retcode failed.");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}
} // namespace DistributedHardware
} // namespace OHOS
