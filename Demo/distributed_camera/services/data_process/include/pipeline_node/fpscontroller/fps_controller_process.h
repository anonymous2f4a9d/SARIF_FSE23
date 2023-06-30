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
#ifndef OHOS_FPS_CONTROLLER_PROCESS_H
#define OHOS_FPS_CONTROLLER_PROCESS_H

#include <cstdint>
#include <vector>

#include "abstract_data_process.h"
#include "dcamera_pipeline_source.h"

namespace OHOS {
namespace DistributedHardware {
class DCameraPipelineSource;

class FpsControllerProcess : public AbstractDataProcess {
public:
    FpsControllerProcess(const VideoConfigParams& sourceConfig, const VideoConfigParams& targetConfig,
        const std::weak_ptr<DCameraPipelineSource>& callbackPipSource)
        : sourceConfig_(sourceConfig), targetConfig_(targetConfig), callbackPipelineSource_(callbackPipSource) {}
    ~FpsControllerProcess();

    int32_t InitNode() override;
    int32_t ProcessData(std::vector<std::shared_ptr<DataBuffer>>& inputBuffers) override;
    void ReleaseProcessNode() override;

private:
    void UpdateFPSControllerInfo(int64_t nowMs);
    void UpdateFrameRateCorrectionFactor(int64_t nowMs);
    void UpdateIncomingFrameTimes(int64_t nowMs);
    float CalculateFrameRate(int64_t nowMs);
    bool IsDropFrame(float incomingFps);
    bool ReduceFrameRateByUniformStrategy(int32_t incomingFps);
    int32_t FpsControllerDone(std::vector<std::shared_ptr<DataBuffer>> outputBuffers);

private:
    const static uint32_t MAX_TARGET_FRAME_RATE = 30;
    const static int32_t VIDEO_FRAME_DROP_INTERVAL = 4;
    const static int32_t MIN_INCOME_FRAME_NUM_COEFFICIENT = 3;
    const static int32_t INCOME_FRAME_TIME_HISTORY_WINDOWS_SIZE = 60;
    /* Receive video frame detect time windows */
    const static int32_t FRAME_HISTORY_TIME_WINDOWS_MS = 2000;
    const static int64_t FRMAE_MAX_INTERVAL_TIME_WINDOW_MS = 700;
    const static int32_t OVERSHOOT_MODIFY_COEFFICIENT = 3;
    const static int32_t DOUBLE_MULTIPLE = 2;

    std::mutex mtx;
    VideoConfigParams sourceConfig_;
    VideoConfigParams targetConfig_;
    std::weak_ptr<DCameraPipelineSource> callbackPipelineSource_;
    bool isFpsControllerProcess_ = false;
    bool isFirstFrame_ = false;
    uint32_t targetFrameRate_ = 0;
    int64_t lastFrameIncomeTimeMs_ = 0;
    /* the time span between current and last frame */
    int64_t recentFrameTimeSpanMs_ = -1;
    int32_t keepCorrectionCount_ = 0;
    int32_t keepLessThanDoubleCount_ = 0;
    int32_t keepMoreThanDoubleCount_ = 0;
    float frameRateCorrectionFactor_ = 0.0;
    /* modify the frame rate controller argument */
    int32_t frameRateOvershootMdf_ = 0;
    int64_t incomingFrameTimesMs_[INCOME_FRAME_TIME_HISTORY_WINDOWS_SIZE];
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_FPS_CONTROLLER_PROCESS_H