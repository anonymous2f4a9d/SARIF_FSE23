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

#ifndef OHOS_ICAMERA_CONTROLLER_H
#define OHOS_ICAMERA_CONTROLLER_H

#include "dcamera_capture_info_cmd.h"
#include "dcamera_channel_info_cmd.h"
#include "dcamera_event_cmd.h"
#include "dcamera_info_cmd.h"
#include "dcamera_index.h"
#include "dcamera_open_info_cmd.h"

namespace OHOS {
namespace DistributedHardware {
class ICameraController {
public:
    ICameraController() = default;
    virtual ~ICameraController() = default;

    virtual int32_t StartCapture(std::vector<std::shared_ptr<DCameraCaptureInfo>>& captureInfos) = 0;
    virtual int32_t StopCapture() = 0;
    virtual int32_t ChannelNeg(std::shared_ptr<DCameraChannelInfo>& info) = 0;
    virtual int32_t DCameraNotify(std::shared_ptr<DCameraEvent>& events) = 0;
    virtual int32_t UpdateSettings(std::vector<std::shared_ptr<DCameraSettings>>& settings) = 0;
    virtual int32_t GetCameraInfo(std::shared_ptr<DCameraInfo>& camInfo) = 0;
    virtual int32_t OpenChannel(std::shared_ptr<DCameraOpenInfo>& openInfo) = 0;
    virtual int32_t CloseChannel() = 0;
    virtual int32_t Init(std::vector<DCameraIndex>& indexs) = 0;
    virtual int32_t UnInit() = 0;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_ICAMERA_CONTROLLER_H
