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

#ifndef OHOS_DCAMERA_SINK_OUTPUT_H
#define OHOS_DCAMERA_SINK_OUTPUT_H

#include "icamera_sink_output.h"

#include <map>

#include "icamera_channel.h"
#include "icamera_operator.h"
#include "icamera_sink_data_process.h"

namespace OHOS {
namespace DistributedHardware {
class DCameraSinkOutput : public ICameraSinkOutput, public std::enable_shared_from_this<DCameraSinkOutput> {
public:
    DCameraSinkOutput(const std::string& dhId, std::shared_ptr<ICameraOperator>& cameraOperator);
    ~DCameraSinkOutput();

    int32_t Init() override;
    int32_t UnInit() override;
    int32_t StartCapture(std::vector<std::shared_ptr<DCameraCaptureInfo>>& captureInfos) override;
    int32_t StopCapture() override;
    int32_t OpenChannel(std::shared_ptr<DCameraChannelInfo>& info) override;
    int32_t CloseChannel() override;

    void OnPhotoResult(std::shared_ptr<DataBuffer>& buffer);
    void OnVideoResult(std::shared_ptr<DataBuffer>& buffer);

    void OnSessionState(DCStreamType type, int32_t state);
    void OnSessionError(DCStreamType type, int32_t eventType, int32_t eventReason, std::string detail);
    void OnDataReceived(DCStreamType type, std::vector<std::shared_ptr<DataBuffer>>& dataBuffers);

private:
    void InitInner(DCStreamType type);

    bool isInit_;
    std::string dhId_;
    std::map<DCStreamType, int32_t> sessionState_;
    std::map<DCStreamType, std::shared_ptr<ICameraChannel>> channels_;
    std::map<DCStreamType, std::shared_ptr<ICameraSinkDataProcess>> dataProcesses_;
    std::shared_ptr<ICameraOperator> operator_;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_DCAMERA_SINK_OUTPUT_H