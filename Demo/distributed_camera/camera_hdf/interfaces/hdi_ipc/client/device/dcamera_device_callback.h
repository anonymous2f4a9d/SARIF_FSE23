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

#ifndef DISTRIBUTED_CAMERA_DEVICE_CALLBACK_H
#define DISTRIBUTED_CAMERA_DEVICE_CALLBACK_H

#include "dcamera_device_callback_stub.h"

namespace OHOS {
namespace DistributedHardware {
using namespace OHOS::Camera;
class DCameraDeviceCallback : public DCameraDeviceCallbackStub {
public:
    DCameraDeviceCallback() = default;
    virtual ~DCameraDeviceCallback() = default;

public:
    virtual void OnError(ErrorType type, int32_t errorMsg) override;
    virtual void OnResult(uint64_t timestamp, const std::shared_ptr<CameraStandard::CameraMetadata> &result) override;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // DISTRIBUTED_CAMERA_DEVICE_CALLBACK_H