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
#ifndef OHOS_DCAMERA_INDEX_H
#define OHOS_DCAMERA_INDEX_H

#include <string>

namespace OHOS {
namespace DistributedHardware {
class DCameraIndex {
public:
    DCameraIndex() = default;
    explicit DCameraIndex(std::string devId, std::string dhId) : devId_(devId), dhId_(dhId) {}
    ~DCameraIndex() = default;

    bool operator == (const DCameraIndex& index) const
    {
        return this->devId_ == index.devId_ && this->dhId_ == index.dhId_;
    }

    bool operator < (const DCameraIndex& index) const
    {
        return (this->devId_ + this->dhId_) < (index.devId_ + index.dhId_);
    }

    std::string devId_;
    std::string dhId_;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_DCAMERA_INDEX_H
