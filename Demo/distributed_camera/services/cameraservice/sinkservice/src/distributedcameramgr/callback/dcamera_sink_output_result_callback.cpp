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

#include "dcamera_sink_output_result_callback.h"

#include "distributed_hardware_log.h"

namespace OHOS {
namespace DistributedHardware {
DCameraSinkOutputResultCallback::DCameraSinkOutputResultCallback(
    std::shared_ptr<DCameraSinkOutput>& output) : output_(output)
{
}

void DCameraSinkOutputResultCallback::OnPhotoResult(std::shared_ptr<DataBuffer>& buffer)
{
    std::shared_ptr<DCameraSinkOutput> output = output_.lock();
    if (output == nullptr) {
        DHLOGE("DCameraSinkOutputResultCallback::OnPhotoResult output is null");
        return;
    }
    output->OnPhotoResult(buffer);
}

void DCameraSinkOutputResultCallback::OnVideoResult(std::shared_ptr<DataBuffer>& buffer)
{
    std::shared_ptr<DCameraSinkOutput> output = output_.lock();
    if (output == nullptr) {
        DHLOGE("DCameraSinkOutputResultCallback::OnVideoResult output is null");
        return;
    }
    output->OnVideoResult(buffer);
}
} // namespace DistributedHardware
} // namespace OHOS