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

#ifndef OHOS_DISTRIBUTED_CAMERA_MOCKSOURCE_STATE_LISTENER_H
#define OHOS_DISTRIBUTED_CAMERA_MOCKSOURCE_STATE_LISTENER_H

#include "icamera_state_listener.h"

#include "distributed_camera_errno.h"

namespace OHOS {
namespace DistributedHardware {
class MockDCameraSourceStateListener : public ICameraStateListener {
public:
    MockDCameraSourceStateListener() {};
    ~MockDCameraSourceStateListener() = default;

    int32_t OnRegisterNotify(const std::string& devId, const std::string& dhId, const std::string& reqId,
        int32_t status, std::string& data)
    {
        return DCAMERA_OK;
    }

    int32_t OnUnregisterNotify(const std::string& devId, const std::string& dhId, const std::string& reqId,
        int32_t status, std::string& data)
    {
        return DCAMERA_OK;
    }
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_DISTRIBUTED_CAMERA_MOCKSOURCE_STATE_LISTENER_H
