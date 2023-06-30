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
#ifndef OHOS_DISTRIBUTED_CAMERA_SOURCE_PROXY_H
#define OHOS_DISTRIBUTED_CAMERA_SOURCE_PROXY_H

#include <cstdint>

#include "iremote_proxy.h"
#include "iremote_broker.h"
#include "refbase.h"

#include "idistributed_camera_source.h"

namespace OHOS {
namespace DistributedHardware {
class DistributedCameraSourceProxy : public IRemoteProxy<IDistributedCameraSource> {
public:
    explicit DistributedCameraSourceProxy(const sptr<IRemoteObject>& impl)
        : IRemoteProxy<IDistributedCameraSource>(impl)
    {
    }

    ~DistributedCameraSourceProxy() {}
    int32_t InitSource(const std::string& params, const sptr<IDCameraSourceCallback>& callback) override;
    int32_t ReleaseSource() override;
    int32_t RegisterDistributedHardware(const std::string& devId, const std::string& dhId,
        const std::string& reqId, const EnableParam& param) override;
    int32_t UnregisterDistributedHardware(const std::string& devId, const std::string& dhId,
        const std::string& reqId) override;
    int32_t DCameraNotify(const std::string& devId, const std::string& dhId, std::string& events) override;

private:
    static inline BrokerDelegator<DistributedCameraSourceProxy> delegator_;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_DISTRIBUTED_CAMERA_SOURCE_PROXY_H
