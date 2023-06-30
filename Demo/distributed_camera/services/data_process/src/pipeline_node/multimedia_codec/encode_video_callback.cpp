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
#include <encode_video_callback.h>

#include "distributed_hardware_log.h"

namespace OHOS {
namespace DistributedHardware {
void EncodeVideoCallback::OnError(Media::AVCodecErrorType errorType, int32_t errorCode)
{
    DHLOGD("EncodeVideoCallback : OnError. Error type: %d . Error code: %d ", errorType, errorCode);
    std::shared_ptr<EncodeDataProcess> targetEncoderNode = encodeVideoNode_.lock();
    if (targetEncoderNode == nullptr) {
        DHLOGE("encodeVideoNode_ is nullptr.");
        return;
    }
    targetEncoderNode->OnError();
}

void EncodeVideoCallback::OnInputBufferAvailable(uint32_t index)
{
    DHLOGD("EncodeVideoCallback : OnInputBufferAvailable. No operation when using surface input.");
    std::shared_ptr<EncodeDataProcess> targetEncoderNode = encodeVideoNode_.lock();
    if (targetEncoderNode == nullptr) {
        DHLOGE("encodeVideoNode_ is nullptr.");
        return;
    }
    targetEncoderNode->OnInputBufferAvailable(index);
}

void EncodeVideoCallback::OnOutputFormatChanged(const Media::Format &format)
{
    DHLOGD("EncodeVideoCallback : OnOutputFormatChanged.");
    std::shared_ptr<EncodeDataProcess> targetEncoderNode = encodeVideoNode_.lock();
    if (targetEncoderNode == nullptr) {
        DHLOGE("encodeVideoNode_ is nullptr.");
        return;
    }
    targetEncoderNode->OnOutputFormatChanged(format);
}

void EncodeVideoCallback::OnOutputBufferAvailable(uint32_t index, Media::AVCodecBufferInfo info,
    Media::AVCodecBufferFlag flag)
{
    DHLOGD("EncodeVideoCallback : OnOutputBufferAvailable.");
    std::shared_ptr<EncodeDataProcess> targetEncoderNode = encodeVideoNode_.lock();
    if (targetEncoderNode == nullptr) {
        DHLOGE("encodeVideoNode_ is nullptr.");
        return;
    }
    targetEncoderNode->OnOutputBufferAvailable(index, info, flag);
}
} // namespace DistributedHardware
} // namespace OHOS
