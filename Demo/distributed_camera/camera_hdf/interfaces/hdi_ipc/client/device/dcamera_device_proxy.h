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

#ifndef DISTRIBUTED_CAMERA_DEVICE_CLIENT_PROXY_H
#define DISTRIBUTED_CAMERA_DEVICE_CLIENT_PROXY_H

#include "iremote_proxy.h"
#include "icamera_device.h"
#include "istream_operator_callback.h"

namespace OHOS {
namespace DistributedHardware {
using namespace OHOS::Camera;
class DCameraDeviceProxy : public IRemoteProxy<ICameraDevice> {
public:
    explicit DCameraDeviceProxy(const sptr<IRemoteObject> &impl) : IRemoteProxy<ICameraDevice>(impl) {}
    virtual ~DCameraDeviceProxy() = default;

    virtual CamRetCode GetStreamOperator(const OHOS::sptr<IStreamOperatorCallback> &callback,
                                         OHOS::sptr<IStreamOperator> &streamOperator) override;
    virtual CamRetCode UpdateSettings(const std::shared_ptr<CameraSetting> &settings) override;
    virtual CamRetCode SetResultMode(const ResultCallbackMode &mode) override;
    virtual CamRetCode GetEnabledResults(std::vector<MetaType> &results) override;
    virtual CamRetCode EnableResult(const std::vector<MetaType> &results) override;
    virtual CamRetCode DisableResult(const std::vector<MetaType> &results) override;
    virtual void Close() override;

private:
    static constexpr int CMD_CAMERA_DEVICE_REMOTE_GET_STREAM_OPERATOR = 0;
    static constexpr int CMD_CAMERA_DEVICE_REMOTE_UPDATE_SERTTINGS = 1;
    static constexpr int CMD_CAMERA_DEVICE_REMOTE_SET_RESULT_MODE = 2;
    static constexpr int CMD_CAMERA_DEVICE_REMOTE_GET_ENABLED_RESULTS = 3;
    static constexpr int CMD_CAMERA_DEVICE_REMOTE_ENABLE_RESULT = 4;
    static constexpr int CMD_CAMERA_DEVICE_REMOTE_DISABLE_RESULT = 5;
    static constexpr int CMD_CAMERA_DEVICE_REMOTE_CLOSE = 6;

    static inline BrokerDelegator<DCameraDeviceProxy> delegator_;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // DISTRIBUTED_CAMERA_DEVICE_CLIENT_PROXY_H