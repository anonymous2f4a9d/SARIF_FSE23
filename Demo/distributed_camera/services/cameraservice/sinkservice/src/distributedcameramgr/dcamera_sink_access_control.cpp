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

#include "dcamera_sink_access_control.h"

#include "distributed_camera_constants.h"
#include "distributed_camera_errno.h"
#include "distributed_hardware_log.h"

namespace OHOS {
namespace DistributedHardware {
bool DCameraSinkAccessControl::IsSensitiveSrcAccess(const std::string& srcType)
{
    DHLOGI("DCameraSinkAccessControl::IsSensitiveSrcAccess srcType: %s", srcType.c_str());
    return true;
}

bool DCameraSinkAccessControl::NotifySensitiveSrc(const std::string& srcType)
{
    DHLOGI("DCameraSinkAccessControl::NotifySensitiveSrc srcType: %s", srcType.c_str());
    return true;
}

int32_t DCameraSinkAccessControl::TriggerFrame(const std::string& deviceName)
{
    DHLOGI("DCameraSinkAccessControl::TriggerFrame deviceName: %s", deviceName.c_str());
    return DCAMERA_OK;
}

int32_t DCameraSinkAccessControl::GetAccessControlType(const std::string& accessType)
{
    DHLOGI("DCameraSinkAccessControl::GetAccessControlType accessType: %s", accessType.c_str());
    return DCAMERA_SAME_ACCOUNT;
}
} // namespace DistributedHardware
} // namespace OHOS