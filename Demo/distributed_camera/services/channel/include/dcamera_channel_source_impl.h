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

#ifndef OHOS_DCAMERA_CHANNEL_SOURCE_IMPL_H
#define OHOS_DCAMERA_CHANNEL_SOURCE_IMPL_H

#include "icamera_channel.h"

#include "dcamera_softbus_session.h"

namespace OHOS {
namespace DistributedHardware {
class DCameraChannelSourceImpl : public ICameraChannel {
public:
    DCameraChannelSourceImpl();
    ~DCameraChannelSourceImpl();
    int32_t OpenSession() override;
    int32_t CloseSession() override;
    int32_t CreateSession(std::vector<DCameraIndex>& camIndexs, std::string sessionFlag, DCameraSessionMode sessionMode,
        std::shared_ptr<ICameraChannelListener>& listener) override;
    int32_t ReleaseSession() override;
    int32_t SendData(std::shared_ptr<DataBuffer>& buffer) override;

private:
    std::shared_ptr<ICameraChannelListener> listener_;
    std::vector<DCameraIndex> camIndexs_;
    std::vector<std::shared_ptr<DCameraSoftbusSession>> softbusSessions_;
    std::string mySessionName_;
    DCameraSessionMode mode_;
    const std::string SESSION_HEAD = "ohos.dhardware.dcamera_";
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_DCAMERA_CHANNEL_SOURCE_IMPL_H
