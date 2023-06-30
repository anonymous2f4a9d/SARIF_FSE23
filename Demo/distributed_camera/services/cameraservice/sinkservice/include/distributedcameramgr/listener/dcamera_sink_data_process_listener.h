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

#ifndef OHOS_DCAMERA_SINK_DATA_PROCESS_LISTENER_H
#define OHOS_DCAMERA_SINK_DATA_PROCESS_LISTENER_H

#include "data_process_listener.h"

#include "dcamera_sink_data_process.h"

namespace OHOS {
namespace DistributedHardware {
class DCameraSinkDataProcessListener : public DataProcessListener {
public:
    explicit DCameraSinkDataProcessListener(std::shared_ptr<DCameraSinkDataProcess>& dataProcess);
    ~DCameraSinkDataProcessListener() = default;

    void OnProcessedVideoBuffer(const std::shared_ptr<DataBuffer>& videoResult) override;
    void OnError(DataProcessErrorType errorType) override;

private:
    std::weak_ptr<DCameraSinkDataProcess> dataProcess_;
};
} // namespace OHOS
} // namespace DistributedHardware
#endif // OHOS_DCAMERA_SINK_DATA_PROCESS_LISTENER_H
