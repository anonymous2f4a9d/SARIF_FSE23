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

#ifndef OHOS_DCAMERA_SOURCE_CALLBACK_STUB_H
#define OHOS_DCAMERA_SOURCE_CALLBACK_STUB_H

#include <map>
#include "iremote_stub.h"

#include "idcamera_source_callback.h"

namespace OHOS {
namespace DistributedHardware {
class DCameraSourceCallbackStub : public IRemoteStub<IDCameraSourceCallback> {
public:
    DCameraSourceCallbackStub();
    virtual ~DCameraSourceCallbackStub();
    int32_t OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

private:
    int32_t NotifyRegResultInner(MessageParcel &data, MessageParcel &reply);
    int32_t NotifyUnregResultInner(MessageParcel &data, MessageParcel &reply);

    using DCameraFunc = int32_t (DCameraSourceCallbackStub::*)(MessageParcel &data, MessageParcel &reply);
    std::map<uint32_t, DCameraFunc> memberFuncMap_;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_DCAMERA_SOURCE_CALLBACK_STUB_H
