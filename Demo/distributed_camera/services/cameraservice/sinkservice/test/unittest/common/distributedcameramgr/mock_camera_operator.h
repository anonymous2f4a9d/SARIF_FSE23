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

#ifndef OHOS_MOCK_CAMERA_OPERATOR_H
#define OHOS_MOCK_CAMERA_OPERATOR_H

#include "distributed_camera_errno.h"
#include "icamera_operator.h"

namespace OHOS {
namespace DistributedHardware {
class MockCameraOperator : public ICameraOperator {
public:
    explicit MockCameraOperator()
    {
    }

    ~MockCameraOperator()
    {
    }

    int32_t Init()
    {
        return DCAMERA_OK;
    }

    int32_t UnInit()
    {
        return DCAMERA_OK;
    }

    int32_t UpdateSettings(std::vector<std::shared_ptr<DCameraSettings>>& settings)
    {
        return DCAMERA_OK;
    }

    int32_t OpenCamera(std::vector<std::shared_ptr<DCameraCaptureInfo>>& captureInfos)
    {
        return DCAMERA_OK;
    }

    int32_t CloseCamera()
    {
        return DCAMERA_OK;
    }

    int32_t StartCapture(std::vector<std::shared_ptr<DCameraCaptureInfo>>& captureInfos)
    {
        return DCAMERA_OK;
    }

    int32_t StopCapture()
    {
        return DCAMERA_OK;
    }

    int32_t SetStateCallback(std::shared_ptr<StateCallback>& callback)
    {
        return DCAMERA_OK;
    }

    int32_t SetResultCallback(std::shared_ptr<ResultCallback>& callback)
    {
        return DCAMERA_OK;
    }
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_MOCK_CAMERA_OPERATOR_H