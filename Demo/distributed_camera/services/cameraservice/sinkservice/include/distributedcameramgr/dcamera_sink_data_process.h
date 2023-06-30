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

#ifndef OHOS_DCAMERA_SINK_DATA_PROCESS_H
#define OHOS_DCAMERA_SINK_DATA_PROCESS_H

#include "event_bus.h"
#include "dcamera_photo_output_event.h"
#include "dcamera_video_output_event.h"
#include "icamera_sink_data_process.h"

#include "icamera_channel.h"
#include "idata_process_pipeline.h"
#include "image_common_type.h"

namespace OHOS {
namespace DistributedHardware {
class DCameraSinkDataProcess : public ICameraSinkDataProcess, public EventSender,
    public DistributedHardware::EventBusHandler<DCameraPhotoOutputEvent>,
    public DistributedHardware::EventBusHandler<DCameraVideoOutputEvent>,
    public std::enable_shared_from_this<DCameraSinkDataProcess> {
public:
    DCameraSinkDataProcess(const std::string& dhId, std::shared_ptr<ICameraChannel>& channel);
    ~DCameraSinkDataProcess() = default;

    int32_t StartCapture(std::shared_ptr<DCameraCaptureInfo>& captureInfo) override;
    int32_t StopCapture() override;
    int32_t FeedStream(std::shared_ptr<DataBuffer>& dataBuffer) override;

    void OnEvent(DCameraPhotoOutputEvent& event) override;
    void OnEvent(DCameraVideoOutputEvent& event) override;

    void OnProcessedVideoBuffer(const std::shared_ptr<DataBuffer>& videoResult);
    void OnError(DataProcessErrorType errorType);

private:
    int32_t FeedStreamInner(std::shared_ptr<DataBuffer>& dataBuffer);
    VideoCodecType GetPipelineCodecType(DCEncodeType encodeType);
    Videoformat GetPipelineFormat(int32_t format);

    std::string dhId_;
    std::shared_ptr<DCameraCaptureInfo> captureInfo_;
    std::shared_ptr<EventBus> eventBus_;
    std::shared_ptr<ICameraChannel> channel_;
    std::shared_ptr<IDataProcessPipeline> pipeline_;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_DCAMERA_SINK_DATA_PROCESS_H