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

#ifndef OHOS_MOCK_DCAMERA_SINK_CONTROLLER_H
#define OHOS_MOCK_DCAMERA_SINK_CONTROLLER_H

#include "icamera_controller.h"
#include "icamera_sink_access_control.h"

namespace OHOS {
namespace DistributedHardware {
class MockDCameraSinkController : public ICameraController {
public:
    explicit MockDCameraSinkController(const std::shared_ptr<ICameraSinkAccessControl>& accessControl)
    {
    }

    ~MockDCameraSinkController()
    {
    }

    int32_t StartCapture(std::vector<std::shared_ptr<DCameraCaptureInfo>>& captureInfos)
    {
        return DCAMERA_OK;
    }
    int32_t StopCapture()
    {
        return DCAMERA_OK;
    }
    int32_t ChannelNeg(std::shared_ptr<DCameraChannelInfo>& info)
    {
        return DCAMERA_OK;
    }
    int32_t DCameraNotify(std::shared_ptr<DCameraEvent>& events)
    {
        return DCAMERA_OK;
    }
    int32_t UpdateSettings(std::vector<std::shared_ptr<DCameraSettings>>& settings)
    {
        return DCAMERA_OK;
    }
    int32_t GetCameraInfo(std::shared_ptr<DCameraInfo>& camInfo)
    {
        return DCAMERA_OK;
    }
    int32_t OpenChannel(std::shared_ptr<DCameraOpenInfo>& openInfo)
    {
        return DCAMERA_OK;
    }
    int32_t CloseChannel()
    {
        return DCAMERA_OK;
    }
    int32_t Init(std::vector<DCameraIndex>& indexs)
    {
        return DCAMERA_OK;
    }
    int32_t UnInit()
    {
        return DCAMERA_OK;
    }
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_MOCK_DCAMERA_SINK_CONTROLLER_H
