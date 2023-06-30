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

#ifndef OHOS_DISTRIBUTED_CAMERA_SINK_PROXY_H
#define OHOS_DISTRIBUTED_CAMERA_SINK_PROXY_H

#include "idistributed_camera_sink.h"

#include "iremote_broker.h"
#include "iremote_proxy.h"
#include "refbase.h"

namespace OHOS {
namespace DistributedHardware {
class DistributedCameraSinkProxy : public IRemoteProxy<IDistributedCameraSink> {
public:
    explicit DistributedCameraSinkProxy(const sptr<IRemoteObject>& impl)
        : IRemoteProxy<IDistributedCameraSink>(impl)
    {
    }

    ~DistributedCameraSinkProxy() {}
    int32_t InitSink(const std::string& params) override;
    int32_t ReleaseSink() override;
    int32_t SubscribeLocalHardware(const std::string& dhId, const std::string& parameters) override;
    int32_t UnsubscribeLocalHardware(const std::string& dhId) override;
    int32_t StopCapture(const std::string& dhId) override;
    int32_t ChannelNeg(const std::string& dhId, std::string& channelInfo) override;
    int32_t GetCameraInfo(const std::string& dhId, std::string& cameraInfo) override;
    int32_t OpenChannel(const std::string& dhId, std::string& openInfo) override;
    int32_t CloseChannel(const std::string& dhId) override;

private:
    static inline BrokerDelegator<DistributedCameraSinkProxy> delegator_;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_DISTRIBUTED_CAMERA_SINK_PROXY_H