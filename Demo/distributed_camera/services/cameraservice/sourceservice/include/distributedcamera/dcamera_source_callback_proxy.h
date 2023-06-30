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

#ifndef OHOS_DCAMERA_SOURCE_CALLBACK_PROXY_H
#define OHOS_DCAMERA_SOURCE_CALLBACK_PROXY_H
#include <cstdint>

#include "iremote_proxy.h"
#include "iremote_broker.h"
#include "refbase.h"

#include "idcamera_source_callback.h"

namespace OHOS {
namespace DistributedHardware {
class DCameraSourceCallbackProxy : public IRemoteProxy<IDCameraSourceCallback> {
public:
    explicit DCameraSourceCallbackProxy(const sptr<IRemoteObject> &impl) : IRemoteProxy<IDCameraSourceCallback>(impl)
    {}
    ~DCameraSourceCallbackProxy()
    {}

    int32_t OnNotifyRegResult(const std::string& devId, const std::string& dhId, const std::string& reqId,
        int32_t status, std::string& data) override;
    int32_t OnNotifyUnregResult(const std::string& devId, const std::string& dhId, const std::string& reqId,
        int32_t status, std::string& data) override;
private:
    static inline BrokerDelegator<DCameraSourceCallbackProxy> delegator_;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_DCAMERA_SOURCE_CALLBACK_PROXY_H
