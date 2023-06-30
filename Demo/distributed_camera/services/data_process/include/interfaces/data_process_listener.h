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

#ifndef OHOS_DATA_PROCESS_LISTENER_H
#define OHOS_DATA_PROCESS_LISTENER_H

#include "data_buffer.h"

namespace OHOS {
namespace DistributedHardware {
enum DataProcessErrorType : int32_t {
    ERROR_PIPELINE_ENCODER = 0,
    ERROR_PIPELINE_DECODER = -1,
    ERROR_PIPELINE_EVENTBUS = -2,
    ERROR_DISABLE_PROCESS = -3,
};

class DataProcessListener {
public:
    virtual ~DataProcessListener() = default;
    virtual void OnProcessedVideoBuffer(const std::shared_ptr<DataBuffer>& videoResult) = 0;
    virtual void OnError(DataProcessErrorType errorType) = 0;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_DATA_PROCESS_LISTENER_H
