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

#ifndef OHOS_DCAMERA_SOURCE_DATRA_PROCESS_H
#define OHOS_DCAMERA_SOURCE_DATRA_PROCESS_H

#include <set>
#include <string>

#include "dcamera_stream_data_process.h"
#include "icamera_source_data_process.h"

#include "types.h"

namespace OHOS {
namespace DistributedHardware {
class DCameraSourceDataProcess : public ICameraSourceDataProcess {
public:
    DCameraSourceDataProcess(std::string devId, std::string dhId, DCStreamType streamType);
    ~DCameraSourceDataProcess();

    int32_t FeedStream(std::vector<std::shared_ptr<DataBuffer>>& buffers) override;
    int32_t ConfigStreams(std::vector<std::shared_ptr<DCStreamInfo>>& streamInfos) override;
    int32_t ReleaseStreams(std::vector<int32_t>& streamIds) override;
    int32_t StartCapture(std::shared_ptr<DCCaptureInfo>& captureInfo) override;
    int32_t StopCapture() override;
    void GetAllStreamIds(std::vector<int32_t>& streamIds) override;

private:
    std::vector<std::shared_ptr<DCameraStreamDataProcess>> streamProcess_;
    std::set<int32_t> streamIds_;
    std::string devId_;
    std::string dhId_;
    DCStreamType streamType_;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_DCAMERA_SOURCE_DATRA_PROCESS_H