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

#ifndef OHOS_DCAMERA_SINK_ACCESS_CONTROL_H
#define OHOS_DCAMERA_SINK_ACCESS_CONTROL_H

#include "icamera_sink_access_control.h"

namespace OHOS {
namespace DistributedHardware {
class DCameraSinkAccessControl : public ICameraSinkAccessControl {
public:
    DCameraSinkAccessControl() = default;
    ~DCameraSinkAccessControl() = default;

    bool IsSensitiveSrcAccess(const std::string& srcType) override;
    bool NotifySensitiveSrc(const std::string& srcType) override;
    int32_t TriggerFrame(const std::string& deviceName) override;
    int32_t GetAccessControlType(const std::string& accessType) override;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_DCAMERA_SINK_ACCESS_CONTROL_H