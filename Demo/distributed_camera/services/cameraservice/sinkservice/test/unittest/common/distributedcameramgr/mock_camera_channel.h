/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef OHOS_MOCK_CAMERA_CHANNEL_H
#define OHOS_MOCK_CAMERA_CHANNEL_H

#include "distributed_camera_errno.h"
#include "icamera_channel.h"

namespace OHOS {
namespace DistributedHardware {
class MockCameraChannel : public ICameraChannel {
public:
    explicit MockCameraChannel()
    {
    }

    ~MockCameraChannel()
    {
    }

    int32_t OpenSession()
    {
        return DCAMERA_OK;
    }

    int32_t CloseSession()
    {
        return DCAMERA_OK;
    }

    int32_t CreateSession(std::vector<DCameraIndex>& camIndexs, std::string sessionFlag,
        DCameraSessionMode sessionMode, std::shared_ptr<ICameraChannelListener>& listener)
    {
        return DCAMERA_OK;
    }

    int32_t ReleaseSession()
    {
        return DCAMERA_OK;
    }

    int32_t SendData(std::shared_ptr<DataBuffer>& buffer)
    {
        return DCAMERA_OK;
    }
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_MOCK_CAMERA_CHANNEL_H
