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

#ifndef DISTRIBUTED_CAMERA_HOST_CLIENT_PROXY_H
#define DISTRIBUTED_CAMERA_HOST_CLIENT_PROXY_H

#include "iremote_proxy.h"
#include "icamera_host.h"

namespace OHOS {
namespace DistributedHardware {
using namespace OHOS::Camera;
class DCameraHostProxy : public IRemoteProxy<ICameraHost> {
public:
    explicit DCameraHostProxy(const sptr<IRemoteObject> &impl) : IRemoteProxy<ICameraHost>(impl) {}
    virtual ~DCameraHostProxy() {}

    virtual CamRetCode SetCallback(const OHOS::sptr<ICameraHostCallback> &callback) override;
    virtual CamRetCode GetCameraIds(std::vector<std::string> &cameraIds) override;
    virtual CamRetCode GetCameraAbility(const std::string &cameraId,
                                        std::shared_ptr<CameraAbility> &ability) override;
    virtual CamRetCode OpenCamera(const std::string &cameraId,
                                  const OHOS::sptr<ICameraDeviceCallback> &callback,
                                  OHOS::sptr<ICameraDevice> &pDevice) override;
    virtual CamRetCode SetFlashlight(const std::string &cameraId, bool &isEnable) override;

private:
    static constexpr int CMD_CAMERA_HOST_REMOTE_SET_CALLBACK = 0;
    static constexpr int CMD_CAMERA_HOST_REMOTE_GET_CAMERAID = 1;
    static constexpr int CMD_CAMERA_HOST_REMOTE_GET_CAMERA_ABILITY = 2;
    static constexpr int CMD_CAMERA_HOST_REMOTE_OPEN_CAMERA = 3;
    static constexpr int CMD_CAMERA_HOST_REMOTE_SET_FLASH_LIGHT = 4;

    static inline BrokerDelegator<DCameraHostProxy> delegator_;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // DISTRIBUTED_CAMERA_HOST_CLIENT_PROXY_H