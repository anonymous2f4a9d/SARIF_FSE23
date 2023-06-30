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

#include "dcamera_source_event.h"

#include "distributed_camera_errno.h"
#include "distributed_hardware_log.h"

namespace OHOS {
namespace DistributedHardware {
int32_t DCameraSourceEvent::GetDCameraIndex(DCameraIndex& index)
{
    auto indexPtr = std::get_if<DCameraIndex>(&eventParam_);
    if (indexPtr == nullptr) {
        DHLOGE("DCameraSourceEvent can not get_if DCameraIndex");
        return DCAMERA_NOT_FOUND;
    }

    index = *indexPtr;
    return DCAMERA_OK;
}

int32_t DCameraSourceEvent::GetDCameraRegistParam(std::shared_ptr<DCameraRegistParam>& param)
{
    auto paramPtr = std::get_if<std::shared_ptr<DCameraRegistParam>>(&eventParam_);
    if (paramPtr == nullptr) {
        DHLOGE("DCameraSourceEvent can not get_if DCameraRegistParam");
        return DCAMERA_NOT_FOUND;
    }
    param = *paramPtr;
    return DCAMERA_OK;
}

int32_t DCameraSourceEvent::GetStreamInfos(std::vector<std::shared_ptr<DCStreamInfo>>& streamInfos)
{
    auto streamInfosPtr = std::get_if<std::vector<std::shared_ptr<DCStreamInfo>>>(&eventParam_);
    if (streamInfosPtr == nullptr) {
        DHLOGE("DCameraSourceEvent can not get_if DCStreamInfo");
        return DCAMERA_NOT_FOUND;
    }
    streamInfos = std::move(*streamInfosPtr);
    return DCAMERA_OK;
}

int32_t DCameraSourceEvent::GetCaptureInfos(std::vector<std::shared_ptr<DCCaptureInfo>>& captureInfos)
{
    auto captureInfosPtr = std::get_if<std::vector<std::shared_ptr<DCCaptureInfo>>>(&eventParam_);
    if (captureInfosPtr == nullptr) {
        DHLOGE("DCameraSourceEvent can not get_if DCCaptureInfo");
        return DCAMERA_NOT_FOUND;
    }
    captureInfos = std::move(*captureInfosPtr);
    return DCAMERA_OK;
}

int32_t DCameraSourceEvent::GetCameraSettings(std::vector<std::shared_ptr<DCameraSettings>>& settings)
{
    auto settingsPtr = std::get_if<std::vector<std::shared_ptr<DCameraSettings>>>(&eventParam_);
    if (settingsPtr == nullptr) {
        DHLOGE("DCameraSourceEvent can not get_if DCameraSettings");
        return DCAMERA_NOT_FOUND;
    }
    settings = std::move(*settingsPtr);
    return DCAMERA_OK;
}

int32_t DCameraSourceEvent::GetStreamIds(std::vector<int>& streamIds)
{
    auto streamIdsPtr = std::get_if<std::vector<int>>(&eventParam_);
    if (streamIdsPtr == nullptr) {
        DHLOGE("DCameraSourceEvent can not get_if streamIds");
        return DCAMERA_NOT_FOUND;
    }
    streamIds = std::move(*streamIdsPtr);
    return DCAMERA_OK;
}

int32_t DCameraSourceEvent::GetCameraEvent(std::shared_ptr<DCameraEvent>& camEvent)
{
    auto camEventPtr = std::get_if<std::shared_ptr<DCameraEvent>>(&eventParam_);
    if (camEventPtr == nullptr) {
        DHLOGE("DCameraSourceEvent can not get_if camEventPtr");
        return DCAMERA_NOT_FOUND;
    }
    camEvent = *camEventPtr;
    return DCAMERA_OK;
}

DCAMERA_EVENT DCameraSourceEvent::GetEventType()
{
    return eventType_;
}
} // namespace DistributedHardware
} // namespace OHOS
