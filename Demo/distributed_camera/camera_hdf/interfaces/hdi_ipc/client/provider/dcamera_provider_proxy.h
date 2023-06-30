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

#ifndef DISTRIBUTED_CAMERA_PROVIDER_CLIENT_PROXY_H
#define DISTRIBUTED_CAMERA_PROVIDER_CLIENT_PROXY_H

#include "iremote_proxy.h"
#include "buffer_handle.h"
#include "idistributed_camera_provider.h"

namespace OHOS {
namespace DistributedHardware {
class DCameraProviderProxy : public IRemoteProxy<IDCameraProvider> {
public:
    explicit DCameraProviderProxy(const sptr<IRemoteObject> &impl) : IRemoteProxy<IDCameraProvider>(impl) {}
    virtual ~DCameraProviderProxy() = default;

    virtual DCamRetCode EnableDCameraDevice(const std::shared_ptr<DHBase> &dhBase,
                                            const std::string &abilityInfo,
                                            const sptr<IDCameraProviderCallback> &callback) override;
    virtual DCamRetCode DisableDCameraDevice(const std::shared_ptr<DHBase> &dhBase) override;
    virtual DCamRetCode AcquireBuffer(const std::shared_ptr<DHBase> &dhBase, int streamId,
                                      std::shared_ptr<DCameraBuffer> &buffer) override;
    virtual DCamRetCode ShutterBuffer(const std::shared_ptr<DHBase> &dhBase, int streamId,
                                      const std::shared_ptr<DCameraBuffer> &buffer) override;
    virtual DCamRetCode OnSettingsResult(const std::shared_ptr<DHBase> &dhBase,
                                         const std::shared_ptr<DCameraSettings> &result) override;
    virtual DCamRetCode Notify(const std::shared_ptr<DHBase> &dhBase,
                               const std::shared_ptr<DCameraHDFEvent> &event) override;

private:
    static void* DCameraMemoryMap(const BufferHandle *buffer);
    static void DCameraMemoryUnmap(BufferHandle *buffer);

private:
    static constexpr int CMD_DISTRIBUTED_CAMERA_PROVIDER_ENABLE_DEVICE = 0;
    static constexpr int CMD_DISTRIBUTED_CAMERA_PROVIDER_DISABLE_DEVICE = 1;
    static constexpr int CMD_DISTRIBUTED_CAMERA_PROVIDER_ACQUIRE_BUFFER = 2;
    static constexpr int CMD_DISTRIBUTED_CAMERA_PROVIDER_SHUTTER_BUFFER = 3;
    static constexpr int CMD_DISTRIBUTED_CAMERA_PROVIDER_ON_SETTINGS_RESULT = 4;
    static constexpr int CMD_DISTRIBUTED_CAMERA_PROVIDER_NOTIFY = 5;

    static inline BrokerDelegator<DCameraProviderProxy> delegator_;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // DISTRIBUTED_CAMERA_PROVIDER_CLIENT_PROXY_H