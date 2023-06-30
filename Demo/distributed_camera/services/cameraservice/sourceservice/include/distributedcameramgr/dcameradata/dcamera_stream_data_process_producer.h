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

#ifndef OHOS_ICAMERA_SOURCE_DATA_PROCESS_PRODUCER_H
#define OHOS_ICAMERA_SOURCE_DATA_PROCESS_PRODUCER_H

#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>

#include "data_buffer.h"
#include "event_handler.h"
#include "idistributed_camera_provider.h"

namespace OHOS {
namespace DistributedHardware {
class DCameraStreamDataProcessProducer {
public:
    typedef enum {
        DCAMERA_PRODUCER_STATE_STOP = 0,
        DCAMERA_PRODUCER_STATE_START = 1,
    } DCameraProducerState;

    DCameraStreamDataProcessProducer(std::string devId, std::string dhId, int32_t streamId, DCStreamType streamType);
    ~DCameraStreamDataProcessProducer();
    void Start();
    void Stop();
    void FeedStream(const std::shared_ptr<DataBuffer>& buffer);
    void UpdateInterval(uint32_t fps);

private:
    void LooperContinue();
    void LooperSnapShot();
    int32_t FeedStreamToDriver(const std::shared_ptr<DHBase>& dhBase, const std::shared_ptr<DataBuffer>& buffer);

    const uint32_t DCAMERA_PRODUCER_MAX_BUFFER_SIZE = 30;
    const uint32_t DCAMERA_PRODUCER_RETRY_SLEEP_MS = 500;

private:
    std::string devId_;
    std::string dhId_;

    std::thread producerThread_;
    std::condition_variable producerCon_;
    std::mutex producerMutex_;
    std::queue<std::shared_ptr<DataBuffer>> buffers_;
    DCameraProducerState state_;
    uint32_t interval_;
    int32_t streamId_;
    DCStreamType streamType_;
    std::shared_ptr<AppExecFwk::EventHandler> eventHandler_;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_ICAMERA_SOURCE_DATA_PROCESS_PRODUCER_H
