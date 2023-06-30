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

#ifndef OHOS_DCAMERA_SINK_OUTPUT_RESULT_CALLBACK_H
#define OHOS_DCAMERA_SINK_OUTPUT_RESULT_CALLBACK_H

#include "icamera_operator.h"

#include "dcamera_sink_output.h"

namespace OHOS {
namespace DistributedHardware {
class DCameraSinkOutputResultCallback : public ResultCallback {
public:
    explicit DCameraSinkOutputResultCallback(std::shared_ptr<DCameraSinkOutput>& output);
    ~DCameraSinkOutputResultCallback() = default;

    void OnPhotoResult(std::shared_ptr<DataBuffer>& buffer) override;
    void OnVideoResult(std::shared_ptr<DataBuffer>& buffer) override;

private:
    std::weak_ptr<DCameraSinkOutput> output_;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_DCAMERA_SINK_OUTPUT_RESULT_CALLBACK_H