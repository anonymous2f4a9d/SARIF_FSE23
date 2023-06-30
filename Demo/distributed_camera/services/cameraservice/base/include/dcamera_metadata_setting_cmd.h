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

#ifndef OHOS_DCAMERA_METADATA_SETTING_H
#define OHOS_DCAMERA_METADATA_SETTING_H

#include "types.h"

namespace OHOS {
namespace DistributedHardware {
class DCameraMetadataSettingCmd {
public:
    std::string type_;
    std::string dhId_;
    std::string command_;
    std::vector<std::shared_ptr<DCameraSettings>> value_;

public:
    int32_t Marshal(std::string& jsonStr);
    int32_t Unmarshal(const std::string& jsonStr);
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_DCAMERA_METADATA_SETTING_H
