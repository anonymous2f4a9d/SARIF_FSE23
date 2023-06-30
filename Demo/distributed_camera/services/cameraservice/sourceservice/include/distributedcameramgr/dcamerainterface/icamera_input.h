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

#ifndef OHOS_ICAMERA_INPUT_H
#define OHOS_ICAMERA_INPUT_H

#include "types.h"

namespace OHOS {
namespace DistributedHardware {
class ICameraInput {
public:
    virtual ~ICameraInput() = default;

    virtual int32_t ConfigStreams(std::vector<std::shared_ptr<DCStreamInfo>>& streamInfos) = 0;
    virtual int32_t ReleaseStreams(std::vector<int>& streamIds, bool& isAllRelease) = 0;
    virtual int32_t ReleaseAllStreams() = 0;
    virtual int32_t StartCapture(std::vector<std::shared_ptr<DCCaptureInfo>>& captureInfos) = 0;
    virtual int32_t StopCapture() = 0;
    virtual int32_t OpenChannel(std::vector<DCameraIndex>& indexs) = 0;
    virtual int32_t CloseChannel() = 0;
    virtual int32_t Init() = 0;
    virtual int32_t UnInit() = 0;
    virtual int32_t UpdateSettings(std::vector<std::shared_ptr<DCameraSettings>>& settings) = 0;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_ICAMERA_INPUT_H
