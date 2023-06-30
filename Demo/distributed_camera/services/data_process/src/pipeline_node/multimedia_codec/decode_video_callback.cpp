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

#include <decode_video_callback.h>

#include "distributed_hardware_log.h"

namespace OHOS {
namespace DistributedHardware {
void DecodeVideoCallback::OnError(Media::AVCodecErrorType errorType, int32_t errorCode)
{
    DHLOGE("DecodeVideoCallback : OnError. Error type: %d . Error code: %d ", errorType, errorCode);
    std::shared_ptr<DecodeDataProcess> targetDecoderNode = decodeVideoNode_.lock();
    if (targetDecoderNode == nullptr) {
        DHLOGE("decodeVideoNode_ is nullptr.");
        return;
    }
    targetDecoderNode->OnError();
}

void DecodeVideoCallback::OnInputBufferAvailable(uint32_t index)
{
    DHLOGD("DecodeVideoCallback : OnInputBufferAvailable.");
    std::shared_ptr<DecodeDataProcess> targetDecoderNode = decodeVideoNode_.lock();
    if (targetDecoderNode == nullptr) {
        DHLOGE("decodeVideoNode_ is nullptr.");
        return;
    }
    targetDecoderNode->OnInputBufferAvailable(index);
}

void DecodeVideoCallback::OnOutputFormatChanged(const Media::Format &format)
{
    DHLOGD("DecodeVideoCallback : OnOutputFormatChanged.");
    std::shared_ptr<DecodeDataProcess> targetDecoderNode = decodeVideoNode_.lock();
    if (targetDecoderNode == nullptr) {
        DHLOGE("decodeVideoNode_ is nullptr.");
        return;
    }
    targetDecoderNode->OnOutputFormatChanged(format);
}

void DecodeVideoCallback::OnOutputBufferAvailable(uint32_t index, Media::AVCodecBufferInfo info,
    Media::AVCodecBufferFlag flag)
{
    DHLOGD("DecodeVideoCallback : OnOutputBufferAvailable. Only relaese buffer when using surface output.");
    std::shared_ptr<DecodeDataProcess> targetDecoderNode = decodeVideoNode_.lock();
    if (targetDecoderNode == nullptr) {
        DHLOGE("decodeVideoNode_ is nullptr.");
        return;
    }
    targetDecoderNode->OnOutputBufferAvailable(index, info, flag);
}
} // namespace DistributedHardware
} // namespace OHOS
