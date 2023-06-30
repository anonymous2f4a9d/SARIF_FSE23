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

#ifndef DISTRIBUTED_STREAM_OPERATOR_CLIENT_PROXY_H
#define DISTRIBUTED_STREAM_OPERATOR_CLIENT_PROXY_H

#include "iremote_proxy.h"
#include "istream_operator.h"

namespace OHOS {
namespace DistributedHardware {
using namespace OHOS::Camera;
class DStreamOperatorProxy : public IRemoteProxy<IStreamOperator> {
public:
    explicit DStreamOperatorProxy(const sptr<IRemoteObject> &impl) : IRemoteProxy<IStreamOperator>(impl) {}
    virtual ~DStreamOperatorProxy() = default;

    virtual CamRetCode IsStreamsSupported(OperationMode mode,
                                          const std::shared_ptr<CameraStandard::CameraMetadata> &modeSetting,
                                          const std::vector<std::shared_ptr<StreamInfo>> &info,
                                          StreamSupportType &type) override;
    virtual CamRetCode CreateStreams(const std::vector<std::shared_ptr<StreamInfo>> &streamInfos) override;
    virtual CamRetCode ReleaseStreams(const std::vector<int> &streamIds) override;
    virtual CamRetCode CommitStreams(OperationMode mode,
                                     const std::shared_ptr<CameraStandard::CameraMetadata> &modeSetting) override;
    virtual CamRetCode GetStreamAttributes(std::vector<std::shared_ptr<StreamAttribute>> &attributes) override;
    virtual CamRetCode AttachBufferQueue(int streamId,
                                         const OHOS::sptr<OHOS::IBufferProducer> &producer) override;
    virtual CamRetCode DetachBufferQueue(int streamId) override;
    virtual CamRetCode Capture(int captureId,
                               const std::shared_ptr<CaptureInfo> &info, bool isStreaming) override;
    virtual CamRetCode CancelCapture(int captureId) override;
    virtual CamRetCode ChangeToOfflineStream(const std::vector<int> &streamIds,
                                             OHOS::sptr<IStreamOperatorCallback> &callback,
                                             OHOS::sptr<IOfflineStreamOperator> &offlineOperator) override;

private:
    static constexpr int CMD_STREAM_OPERATOR_IS_STREAMS_SUPPORTED = 0;
    static constexpr int CMD_STREAM_OPERATOR_CREATE_STREAMS = 1;
    static constexpr int CMD_STREAM_OPERATOR_RELEASE_STREAMS = 2;
    static constexpr int CMD_STREAM_OPERATOR_COMMIT_STREAMS = 3;
    static constexpr int CMD_STREAM_OPERATOR_GET_STREAM_ATTRIBUTES = 4;
    static constexpr int CMD_STREAM_OPERATOR_ATTACH_BUFFER_QUEUE = 5;
    static constexpr int CMD_STREAM_OPERATOR_DETACH_BUFFER_QUEUE = 6;
    static constexpr int CMD_STREAM_OPERATOR_CAPTURE = 7;
    static constexpr int CMD_STREAM_OPERATOR_CANCEL_CAPTURE = 8;
    static constexpr int CMD_STREAM_OPERATOR_CHANGE_TO_OFFLINE_STREAM = 9;

    static inline BrokerDelegator<DStreamOperatorProxy> delegator_;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // DISTRIBUTED_STREAM_OPERATOR_CLIENT_PROXY_H