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

#ifndef OHOS_DCAMERA_SOURCE_EVENT_H
#define OHOS_DCAMERA_SOURCE_EVENT_H

#include <variant>

#include "event.h"
#include "types.h"

#include "dcamera_event_cmd.h"
#include "dcamera_index.h"

namespace OHOS {
namespace DistributedHardware {
typedef enum {
    DCAMERA_EVENT_REGIST = 0,
    DCAMERA_EVENT_UNREGIST = 1,
    DCAMERA_EVENT_OPEN = 2,
    DCAMERA_EVENT_CLOSE = 3,
    DCAMERA_EVENT_CONFIG_STREAMS = 4,
    DCAMERA_EVENT_RELEASE_STREAMS = 5,
    DCAMERA_EVENT_START_CAPTURE = 6,
    DCAMERA_EVENT_STOP_CAPTURE = 7,
    DCAMERA_EVENT_UPDATE_SETTINGS = 8,
    DCAMERA_EVENT_NOFIFY = 9,
} DCAMERA_EVENT;

class DCameraRegistParam {
public:
    DCameraRegistParam() = default;
    DCameraRegistParam(std::string devId, std::string dhId, std::string reqId, std::string param)
        : devId_(devId), dhId_(dhId), reqId_(reqId), param_(param)
    {}
    ~DCameraRegistParam() = default;
    std::string devId_;
    std::string dhId_;
    std::string reqId_;
    std::string param_;
};

class DCameraSourceEvent : public Event {
    TYPEINDENT(DCameraSourceEvent)

public:
    DCameraSourceEvent(EventSender& sender) : Event(sender) {}
    ~DCameraSourceEvent() = default;
    explicit DCameraSourceEvent(EventSender& sender, DCAMERA_EVENT eventType)
        : Event(sender), eventType_(eventType) {}
    explicit DCameraSourceEvent(EventSender& sender, DCAMERA_EVENT eventType, DCameraIndex& index)
        : Event(sender), eventType_(eventType)
    {
        eventParam_ = index;
    }

    explicit DCameraSourceEvent(EventSender& sender, DCAMERA_EVENT eventType,
        std::shared_ptr<DCameraRegistParam>& param) : Event(sender), eventType_(eventType)
    {
        eventParam_ = param;
    }

    explicit DCameraSourceEvent(EventSender& sender, DCAMERA_EVENT eventType,
        const std::vector<std::shared_ptr<DCStreamInfo>>& streamInfos) : Event(sender), eventType_(eventType)
    {
        eventParam_ = std::move(streamInfos);
    }

    explicit DCameraSourceEvent(EventSender& sender, DCAMERA_EVENT eventType,
        const std::vector<std::shared_ptr<DCCaptureInfo>>& captureInfos) : Event(sender), eventType_(eventType)
    {
        eventParam_ = std::move(captureInfos);
    }

    explicit DCameraSourceEvent(EventSender& sender, DCAMERA_EVENT eventType,
        const std::vector<std::shared_ptr<DCameraSettings>>& settings) : Event(sender), eventType_(eventType)
    {
        eventParam_ = std::move(settings);
    }

    explicit DCameraSourceEvent(EventSender& sender, DCAMERA_EVENT eventType, const std::vector<int>& streamIds)
        : Event(sender), eventType_(eventType)
    {
        eventParam_ = std::move(streamIds);
    }

    explicit DCameraSourceEvent(EventSender& sender, DCAMERA_EVENT eventType, std::shared_ptr<DCameraEvent>& camEvent)
        : Event(sender), eventType_(eventType)
    {
        eventParam_ = camEvent;
    }

    int32_t GetDCameraIndex(DCameraIndex& index);
    int32_t GetDCameraRegistParam(std::shared_ptr<DCameraRegistParam>& param);
    int32_t GetStreamInfos(std::vector<std::shared_ptr<DCStreamInfo>>& streamInfos);
    int32_t GetCaptureInfos(std::vector<std::shared_ptr<DCCaptureInfo>>& captureInfos);
    int32_t GetCameraSettings(std::vector<std::shared_ptr<DCameraSettings>>& settings);
    int32_t GetStreamIds(std::vector<int>& streamIds);
    int32_t GetCameraEvent(std::shared_ptr<DCameraEvent>& camEvent);
    DCAMERA_EVENT GetEventType();

private:
    using EventParam = std::variant<std::monostate, DCameraIndex, std::shared_ptr<DCameraRegistParam>,
        std::vector<std::shared_ptr<DCStreamInfo>>, std::vector<std::shared_ptr<DCCaptureInfo>>,
        std::vector<std::shared_ptr<DCameraSettings>>, std::vector<int>, std::shared_ptr<DCameraEvent>>;

private:
    DCAMERA_EVENT eventType_;
    EventParam eventParam_;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_DCAMERA_SOURCE_EVENT_H
