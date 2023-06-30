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

#ifndef DISTRIBUTED_CAMERA_STREAM_H
#define DISTRIBUTED_CAMERA_STREAM_H

#include "surface.h"
#include "dimage_buffer.h"
#include "dbuffer_manager.h"

#include <foundation/distributedhardware/distributedcamera/camera_hdf/interfaces/include/types.h>
#ifdef BALTIMORE_CAMERA
#include <drivers/peripheral/adapter/camera/interfaces/include/types.h>
#else
#include <drivers/peripheral/camera/interfaces/include/types.h>
#endif

namespace OHOS {
namespace DistributedHardware {
using namespace std;
using namespace OHOS::Camera;
class DCameraStream {
public:
    DCameraStream() = default;
    ~DCameraStream() = default;
    DCameraStream(const DCameraStream &other) = delete;
    DCameraStream(DCameraStream &&other) = delete;
    DCameraStream &operator=(const DCameraStream &other) = delete;
    DCameraStream &operator=(DCameraStream &&other) = delete;

public:
    DCamRetCode InitDCameraStream(const shared_ptr<StreamInfo> &info);
    DCamRetCode GetDCameraStreamInfo(shared_ptr<StreamInfo> &info);
    DCamRetCode SetDCameraBufferQueue(const OHOS::sptr<OHOS::IBufferProducer> producer);
    DCamRetCode ReleaseDCameraBufferQueue();
    DCamRetCode GetDCameraStreamAttribute(shared_ptr<StreamAttribute> &attribute);
    DCamRetCode GetDCameraBuffer(shared_ptr<DCameraBuffer> &buffer);
    DCamRetCode ReturnDCameraBuffer(const shared_ptr<DCameraBuffer> &buffer);
    DCamRetCode FlushDCameraBuffer();
    DCamRetCode FinishCommitStream();
    bool HasBufferQueue();

private:
    DCamRetCode InitDCameraBufferManager();
    DCamRetCode GetNextRequest();

private:
    int32_t index_ = -1;
    int dcStreamId_;
    shared_ptr<StreamInfo> dcStreamInfo_ = nullptr;
    shared_ptr<StreamAttribute> dcStreamAttribute_ = nullptr;
    shared_ptr<DBufferManager> dcStreamBufferMgr_ = nullptr;
    OHOS::sptr<OHOS::Surface> dcStreamProducer_ = nullptr;
    map<shared_ptr<DImageBuffer>, tuple<OHOS::sptr<OHOS::SurfaceBuffer>, int, int>> bufferConfigMap_;
    mutex lock_;
    condition_variable cv_;
    int captureBufferCount_ = 0;
    bool isBufferMgrInited_ = false;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // DISTRIBUTED_CAMERA_STREAM_H