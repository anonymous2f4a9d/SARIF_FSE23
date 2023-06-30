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

#ifndef OHOS_ICAMERA_CHANNEL_H
#define OHOS_ICAMERA_CHANNEL_H

#include "icamera_channel_listener.h"
#include "data_buffer.h"
#include "dcamera_index.h"

namespace OHOS {
namespace DistributedHardware {
typedef enum {
    DCAMERA_SESSION_MODE_CTRL = 0,
    DCAMERA_SESSION_MODE_VIDEO = 1,
    DCAMERA_SESSION_MODE_JPEG = 2,
} DCameraSessionMode;

class ICameraChannel {
public:
    virtual ~ICameraChannel() = default;

    virtual int32_t OpenSession() = 0;
    virtual int32_t CloseSession() = 0;
    virtual int32_t CreateSession(std::vector<DCameraIndex>& camIndexs, std::string sessionFlag,
        DCameraSessionMode sessionMode, std::shared_ptr<ICameraChannelListener>& listener) = 0;
    virtual int32_t ReleaseSession();
    virtual int32_t SendData(std::shared_ptr<DataBuffer>& buffer) = 0;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_ICAMERA_CHANNEL_H