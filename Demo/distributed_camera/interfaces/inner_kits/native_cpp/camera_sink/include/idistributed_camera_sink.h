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

#ifndef OHOS_IDISTRIBUTED_CAMERA_SINK_H
#define OHOS_IDISTRIBUTED_CAMERA_SINK_H

#include "iremote_broker.h"

namespace OHOS {
namespace DistributedHardware {
class IDistributedCameraSink : public OHOS::IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.distributedhardware.distributedcamerasink");
    enum {
        INIT_SINK = 0,
        RELEASE_SINK = 1,
        SUBSCRIBE_LOCAL_HARDWARE = 2,
        UNSUBSCRIBE_LOCAL_HARDWARE = 3,
        STOP_CAPTURE = 4,
        CHANNEL_NEG = 5,
        GET_CAMERA_INFO = 6,
        OPEN_CHANNEL = 7,
        CLOSE_CHANNEL = 8,
    };

    IDistributedCameraSink() = default;
    virtual ~IDistributedCameraSink() = default;
    virtual int32_t InitSink(const std::string& params) = 0;
    virtual int32_t ReleaseSink() = 0;
    virtual int32_t SubscribeLocalHardware(const std::string& dhId, const std::string& parameters) = 0;
    virtual int32_t UnsubscribeLocalHardware(const std::string& dhId) = 0;
    virtual int32_t StopCapture(const std::string& dhId) = 0;
    virtual int32_t ChannelNeg(const std::string& dhId, std::string& channelInfo) = 0;
    virtual int32_t GetCameraInfo(const std::string& dhId, std::string& cameraInfo) = 0;
    virtual int32_t OpenChannel(const std::string& dhId, std::string& openInfo) = 0;
    virtual int32_t CloseChannel(const std::string& dhId) = 0;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_IDISTRIBUTED_CAMERA_SINK_H