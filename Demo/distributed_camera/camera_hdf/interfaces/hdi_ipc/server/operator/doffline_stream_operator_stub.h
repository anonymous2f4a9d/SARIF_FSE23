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

#ifndef DISTRIBUTED_OFFLINE_STREAM_OPERATOR_SERVER_STUB_H
#define DISTRIBUTED_OFFLINE_STREAM_OPERATOR_SERVER_STUB_H

#include <message_option.h>
#include <message_parcel.h>
#include <refbase.h>
#include "ioffline_stream_operator.h"
#include "iremote_stub.h"

namespace OHOS {
namespace DistributedHardware {
using namespace OHOS::Camera;
class DOfflineStreamOperatorStub : public IRemoteStub<IOfflineStreamOperator> {
public:
    int32_t OnRemoteRequest(uint32_t code,
                            MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

    int32_t DOfflineStreamOperatorStubCancelCapture(MessageParcel &data, MessageParcel &reply, MessageOption &option);
    int32_t DOfflineStreamOperatorStubReleaseStreams(MessageParcel &data, MessageParcel &reply, MessageOption &option);
    int32_t DOfflineStreamOperatorStubRelease(MessageParcel &data, MessageParcel &reply, MessageOption &option);
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // DISTRIBUTED_OFFLINE_STREAM_OPERATOR_SERVER_STUB_H