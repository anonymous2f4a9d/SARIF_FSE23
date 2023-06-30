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

#include "dcamera_manager_callback.h"

#include "anonymous_string.h"
#include "distributed_camera_constants.h"
#include "distributed_hardware_log.h"

namespace OHOS {
namespace DistributedHardware {
void DCameraManagerCallback::OnCameraStatusChanged(const CameraStandard::CameraStatusInfo &cameraStatusInfo) const
{
    DHLOGI("DCameraManagerCallback::OnCameraStatusChanged, cameraStatus: %d", cameraStatusInfo.cameraStatus);
}

void DCameraManagerCallback::OnFlashlightStatusChanged(const std::string &cameraID,
                                                       const CameraStandard::FlashlightStatus flashStatus) const
{
    DHLOGI("DCameraManagerCallback::OnFlashlightStatusChanged, cameraID: %s, flashStatus: %d",
           GetAnonyString(cameraID).c_str(), flashStatus);
}
} // namespace DistributedHardware
} // namespace OHOS