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

#include "dcamera_photo_surface_listener.h"

#include <securec.h>

#include "data_buffer.h"
#include "distributed_camera_errno.h"
#include "distributed_hardware_log.h"

namespace OHOS {
namespace DistributedHardware {
DCameraPhotoSurfaceListener::DCameraPhotoSurfaceListener(const sptr<Surface>& surface,
    const std::shared_ptr<ResultCallback>& callback) : surface_(surface), callback_(callback)
{
}

void DCameraPhotoSurfaceListener::OnBufferAvailable()
{
    DHLOGI("DCameraPhotoSurfaceListener::OnBufferAvailable");
    OHOS::sptr<OHOS::SurfaceBuffer> buffer = nullptr;
    int32_t flushFence = 0;
    int64_t timestamp = 0;
    OHOS::Rect damage;

    do {
        surface_->AcquireBuffer(buffer, flushFence, timestamp, damage);
        if (buffer == nullptr) {
            DHLOGE("DCameraPhotoSurfaceListener AcquireBuffer failed");
            break;
        }

        char *address = static_cast<char *>(buffer->GetVirAddr());
        int32_t size = static_cast<int32_t>(buffer->GetSize());
        if ((address == nullptr) || (size <= 0)) {
            DHLOGE("DCameraPhotoSurfaceListener invalid params, size: %d", size);
            break;
        }

        DHLOGI("DCameraPhotoSurfaceListener size: %d", size);
        std::shared_ptr<DataBuffer> dataBuffer = std::make_shared<DataBuffer>(size);
        int32_t ret = memcpy_s(dataBuffer->Data(), dataBuffer->Capacity(), address, size);
        if (ret != EOK) {
            DHLOGE("DCameraPhotoSurfaceListener Memory Copy failed, ret: %d", ret);
            break;
        }

        if (callback_ == nullptr) {
            DHLOGE("DCameraPhotoSurfaceListener ResultCallback is null");
            break;
        }
        callback_->OnPhotoResult(dataBuffer);
    } while (0);
    surface_->ReleaseBuffer(buffer, -1);
}
} // namespace DistributedHardware
} // namespace OHOS