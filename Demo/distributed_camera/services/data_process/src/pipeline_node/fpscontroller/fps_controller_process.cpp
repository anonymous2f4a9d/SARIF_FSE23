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

#include "fps_controller_process.h"

#include "dcamera_utils_tools.h"
#include "distributed_camera_errno.h"
#include "distributed_hardware_log.h"

namespace OHOS {
namespace DistributedHardware {
FpsControllerProcess::~FpsControllerProcess()
{
    if (isFpsControllerProcess_) {
        DHLOGD("~DecodeDataProcess : ReleaseProcessNode.");
        ReleaseProcessNode();
    }
}

int32_t FpsControllerProcess::InitNode()
{
    if (targetConfig_.GetFrameRate() > MAX_TARGET_FRAME_RATE) {
        DHLOGE("The target framerate : %d is greater than the max framerate : %d.",
            targetConfig_.GetFrameRate(), MAX_TARGET_FRAME_RATE);
        return DCAMERA_BAD_TYPE;
    }
    targetFrameRate_ = targetConfig_.GetFrameRate();
    isFpsControllerProcess_ = true;
    return DCAMERA_OK;
}

void FpsControllerProcess::ReleaseProcessNode()
{
    DHLOGD("Start release [%d] node : FPS controller.", nodeRank_);
    if (nextDataProcess_ != nullptr) {
        nextDataProcess_->ReleaseProcessNode();
    }

    isFpsControllerProcess_ = false;
    isFirstFrame_ = false;
    targetFrameRate_ = 0;
    lastFrameIncomeTimeMs_ = 0;
    recentFrameTimeSpanMs_ = -1;
    keepCorrectionCount_ = 0;
    keepLessThanDoubleCount_ = 0;
    keepMoreThanDoubleCount_ = 0;
    frameRateCorrectionFactor_ = 0.0;
    frameRateOvershootMdf_ = 0;
    for (int i = 0; i < INCOME_FRAME_TIME_HISTORY_WINDOWS_SIZE; i++) {
        incomingFrameTimesMs_[i] = 0;
    }
}

int32_t FpsControllerProcess::ProcessData(std::vector<std::shared_ptr<DataBuffer>>& inputBuffers)
{
    if (inputBuffers.empty()) {
        DHLOGE("Data buffers is null.");
        return DCAMERA_BAD_TYPE;
    }
    if (!isFpsControllerProcess_) {
        DHLOGE("Decoder node occurred error.");
        return DCAMERA_DISABLE_PROCESS;
    }
    int64_t timeStampUs = 0;
    if (!inputBuffers[0]->FindInt64("timeUs", timeStampUs)) {
        DHLOGE("Find decoder output timestamp fail.");
        return DCAMERA_BAD_TYPE;
    }

    std::lock_guard<std::mutex> lck (mtx);
    int64_t nowTimeMs = GetNowTimeStampMs();
    UpdateFPSControllerInfo(nowTimeMs);

    float curFrameRate = CalculateFrameRate(nowTimeMs);
    if (IsDropFrame(curFrameRate)) {
        DHLOGD("frame control, currect frameRate %u, targetRate %u, drop it", curFrameRate, targetFrameRate_);
        return DCAMERA_OK;
    }

    DHLOGD("frame control render PushVideoFrame, frame info width %d height %d, timeStampUs %lld, fps %d",
        sourceConfig_.GetWidth(), sourceConfig_.GetHeight(), (long long)timeStampUs, curFrameRate);
    return FpsControllerDone(inputBuffers);
}

void FpsControllerProcess::UpdateFPSControllerInfo(int64_t nowMs)
{
    DHLOGD("Frame control, update control info.");
    if (targetFrameRate_ <= 0) {
        DHLOGD("Frame control, targetFrameRate_ : %d", targetFrameRate_);
        return;
    }

    isFirstFrame_ = false;
    if (lastFrameIncomeTimeMs_ == 0) {
        DHLOGD("Frame control, income fisrt frame.");
        isFirstFrame_ = true;
    }
    lastFrameIncomeTimeMs_ = nowMs;
    recentFrameTimeSpanMs_ = nowMs - lastFrameIncomeTimeMs_;
    DHLOGD("Frame control, lastFrameIncomeTimeMs_ %lld, receive Frame after last frame(ms): %lld",
        (long long)lastFrameIncomeTimeMs_, (long long)recentFrameTimeSpanMs_);
    UpdateIncomingFrameTimes(nowMs);
    UpdateFrameRateCorrectionFactor(nowMs);
    return;
}

void FpsControllerProcess::UpdateFrameRateCorrectionFactor(int64_t nowMs)
{
    DHLOGD("Frame control, update FPS correction factor.");
    if (targetFrameRate_ <= 0) {
        DHLOGD("Frame control, targetFrameRate_ : %d", targetFrameRate_);
        return;
    }
    if (isFirstFrame_) {
        DHLOGD("No frame rate correction factor when the first frame.");
        return;
    }

    const float minDropFrmValue = 0.5;
    const float maxDropFrmValue = 1.0;
    const float msPerSecond = 1000;
    const float maxInstantaneousFrameRateCoefficient = 1.1;
    float maxInstantaneousFrameRateThreshold = targetFrameRate_ * maxInstantaneousFrameRateCoefficient;
    float instantaneousFrameRate = msPerSecond / recentFrameTimeSpanMs_;
    if (instantaneousFrameRate < 0) {
        instantaneousFrameRate = -instantaneousFrameRate;
    }
    if (instantaneousFrameRate <= maxInstantaneousFrameRateThreshold) {
        frameRateCorrectionFactor_ = minDropFrmValue;
    } else {
        if (keepCorrectionCount_ >= VIDEO_FRAME_DROP_INTERVAL) {
            frameRateCorrectionFactor_ = maxDropFrmValue;
            keepCorrectionCount_ = 0;
        } else {
            frameRateCorrectionFactor_ = 0;
            keepCorrectionCount_++;
        }
        DHLOGD("Frame control, instantaneousFrameRate %.3f is more than maxInstantaneousFrameRateThreshold %.3f, " +
            "keepCorrectionCount %d", instantaneousFrameRate, maxInstantaneousFrameRateThreshold,
            keepCorrectionCount_);
    }

    DHLOGD("Frame control, targetFramerate %d, maxInstantaneousFrameRateThreshold %.3f," +
        "instantaneousFrameRate %.3f, frameRateCorrectionFactor %.3f", targetFrameRate_,
        maxInstantaneousFrameRateThreshold, instantaneousFrameRate, frameRateCorrectionFactor_);
    return;
}

void FpsControllerProcess::UpdateIncomingFrameTimes(int64_t nowMs)
{
    DHLOGD("Frame control, update incoming frame times array.");
    if (targetFrameRate_ <= 0) {
        DHLOGD("Frame control, targetFrameRate_ : %d", targetFrameRate_);
        return;
    }
    if (isFirstFrame_) {
        incomingFrameTimesMs_[0] = nowMs;
        return;
    }

    int64_t intervalNewAndFirst = nowMs - incomingFrameTimesMs_[0];
    if (intervalNewAndFirst < 0) {
        intervalNewAndFirst = -intervalNewAndFirst;
    }
    if (intervalNewAndFirst > FRMAE_MAX_INTERVAL_TIME_WINDOW_MS) {
        DHLOGD("frame control, nowMs: %lld mIncomingFrameT[0]: %lld intervalNewAndFirst: %lld",
            (long long)nowMs, (long long)incomingFrameTimesMs_[0], (long long)intervalNewAndFirst);
        for (int i = 0; i < INCOME_FRAME_TIME_HISTORY_WINDOWS_SIZE; i++) {
            incomingFrameTimesMs_[i] = 0;
        }
    } else {
        DHLOGD("frame control shift, nowMs: %lld mIncomingFrameT[0]: %lld intervalNewAndFirst: %lld",
            (long long)nowMs, (long long)incomingFrameTimesMs_[0], (long long)intervalNewAndFirst);
        const int32_t windowLeftNum =  2;
        for (int i = (INCOME_FRAME_TIME_HISTORY_WINDOWS_SIZE - windowLeftNum); i >= 0; --i) {
            incomingFrameTimesMs_[i + 1] = incomingFrameTimesMs_[i];
        }
    }
    incomingFrameTimesMs_[0] = nowMs;
    return;
}

float FpsControllerProcess::CalculateFrameRate(int64_t nowMs)
{
    DHLOGD("Frame control, calculate frame rate.");
    if (targetFrameRate_ <= 0) {
        DHLOGE("Frame control, targetFrameRate_ : %d", targetFrameRate_);
        return 0.0;
    }

    int32_t num = 0;
    int32_t validFramesNumber = 0;
    if (nowMs < 0) {
        nowMs = -nowMs;
    }
    for (; num < INCOME_FRAME_TIME_HISTORY_WINDOWS_SIZE; num++) {
        if (incomingFrameTimesMs_[num] <= 0 || nowMs - incomingFrameTimesMs_[num] > FRAME_HISTORY_TIME_WINDOWS_MS) {
            break;
        } else {
            validFramesNumber++;
        }
    }

    const float msPerSecond = 1000;
    const int32_t minValidCalculatedFrameRatesNum = 2;
    int32_t minIncomingFrameNum = static_cast<int32_t>(targetFrameRate_) / MIN_INCOME_FRAME_NUM_COEFFICIENT;
    if (validFramesNumber > minIncomingFrameNum && validFramesNumber > minValidCalculatedFrameRatesNum) {
        int64_t validTotalTimeDifference = (nowMs - incomingFrameTimesMs_[num - 1]);
        if (validTotalTimeDifference < 0) {
            validTotalTimeDifference = -validTotalTimeDifference;
        }
        if (validTotalTimeDifference > 0) {
            return validFramesNumber * msPerSecond / validTotalTimeDifference + frameRateCorrectionFactor_;
        }
    }
    return static_cast<float>(validFramesNumber);
}

bool FpsControllerProcess::IsDropFrame(float incomingFps)
{
    DHLOGD("Frame control, IsDropFrame");
    if (targetFrameRate_ == 0) {
        DHLOGD("target fps is 0, drop all frame.");
        return true;
    }
    if (incomingFps <= 0) {
        DHLOGD("incoming fps not more than 0, not drop");
        return false;
    }
    const int32_t incomingFrmRate = static_cast<int32_t>(incomingFps);
    if (incomingFrmRate > static_cast<int32_t>(targetFrameRate_)) {
        DHLOGD("incoming fps not more than targetFrameRate_, not drop");
        return false;
    }
    bool isDrop = ReduceFrameRateByUniformStrategy(incomingFrmRate);
    DHLOGD("drop frame result: %s", isDrop ? "drop" : "no drop");
    return isDrop;
}

bool FpsControllerProcess::ReduceFrameRateByUniformStrategy(int32_t incomingFrmRate)
{
    DHLOGD("Frame control, reduce frame rate by uniform rate strategy");
    if (incomingFrmRate > static_cast<int32_t>(targetFrameRate_)) {
        DHLOGD("incoming fps not more than targetFrameRate_, not drop");
        return false;
    }

    /*
     * When the actual incoming frame rate correction value is greater than the target frame
     * rate, the incoming frames are reduced uniformly.
     */
    bool isDrop = false;
    int32_t overshoot = frameRateOvershootMdf_ + (incomingFrmRate - targetFrameRate_);
    if (overshoot < 0) {
        overshoot = 0;
        frameRateOvershootMdf_ = 0;
    }
    if (overshoot && DOUBLE_MULTIPLE * overshoot < incomingFrmRate) {
        /*
         * When the actual input frame rate is less than or equal to twice the target frame rate,
         * one frame is droped every (incomingFrmRate / overshoot) frames.
         */
        if (keepMoreThanDoubleCount_) {
            keepMoreThanDoubleCount_ = 0;
            return true;
        }
        const int32_t dropVar = incomingFrmRate / overshoot;
        if (keepLessThanDoubleCount_ >= dropVar) {
            isDrop = true;
            frameRateOvershootMdf_ = -(incomingFrmRate % overshoot) / OVERSHOOT_MODIFY_COEFFICIENT;
            keepLessThanDoubleCount_ = 1;
        } else {
            keepLessThanDoubleCount_++;
        }
    } else {
        /*
         * When the actual frame rate is more than twice the target frame rate or the overshoot is
         * equal to 0, one frame is reserved every (overshoot / targetFrameRate_) frames.
         */
        keepLessThanDoubleCount_ = 0;
        const int32_t dropVar = overshoot / targetFrameRate_;
        if (keepMoreThanDoubleCount_ < dropVar) {
            isDrop = true;
            keepMoreThanDoubleCount_++;
        } else {
            frameRateOvershootMdf_ = overshoot % static_cast<int32_t>(targetFrameRate_);
            isDrop = false;
            keepMoreThanDoubleCount_ = 0;
        }
    }
    return isDrop;
}

int32_t FpsControllerProcess::FpsControllerDone(std::vector<std::shared_ptr<DataBuffer>> outputBuffers)
{
    if (outputBuffers.empty()) {
        DHLOGE("The received data buffers is empty.");
        return DCAMERA_BAD_VALUE;
    }

    if (nextDataProcess_ != nullptr) {
        DHLOGD("Send to the next node of the FpsController for processing.");
        int32_t err = nextDataProcess_->ProcessData(outputBuffers);
        if (err != DCAMERA_OK) {
            DHLOGE("Someone node after the FpsController processes fail.");
        }
        return err;
    }
    DHLOGD("The current node is the last node, and Output the processed video buffer");
    std::shared_ptr<DCameraPipelineSource> targetPipelineSource = callbackPipelineSource_.lock();
    if (targetPipelineSource == nullptr) {
        DHLOGE("callbackPipelineSource_ is nullptr.");
        return DCAMERA_BAD_VALUE;
    }
    targetPipelineSource->OnProcessedVideoBuffer(outputBuffers[0]);
    return DCAMERA_OK;
}
} // namespace DistributedHardware
} // namespace OHOS
