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

#include "dcamera_event_cmd.h"

#include "json/json.h"

#include "distributed_camera_constants.h"
#include "distributed_camera_errno.h"
#include "distributed_hardware_log.h"

namespace OHOS {
namespace DistributedHardware {
int32_t DCameraEventCmd::Marshal(std::string& jsonStr)
{
    Json::Value rootValue;
    rootValue["Type"] = Json::Value(type_);
    rootValue["dhId"] = Json::Value(dhId_);
    rootValue["Command"] = Json::Value(command_);

    Json::Value event;
    event["EventType"] = Json::Value(value_->eventType_);
    event["EventResult"] = Json::Value(value_->eventResult_);
    event["EventContent"] = Json::Value(value_->eventContent_);
    rootValue["Value"] = event;

    jsonStr = rootValue.toStyledString();
    return DCAMERA_OK;
}

int32_t DCameraEventCmd::Unmarshal(const std::string& jsonStr)
{
    JSONCPP_STRING errs;
    Json::CharReaderBuilder readerBuilder;
    Json::Value rootValue;

    std::unique_ptr<Json::CharReader> const jsonReader(readerBuilder.newCharReader());
    if (!jsonReader->parse(jsonStr.c_str(), jsonStr.c_str() + jsonStr.length(), &rootValue, &errs) ||
        !rootValue.isObject()) {
        return DCAMERA_BAD_VALUE;
    }

    if (!rootValue.isMember("Type") || !rootValue["Type"].isString()) {
        return DCAMERA_BAD_VALUE;
    }
    type_ = rootValue["Type"].asString();

    if (!rootValue.isMember("dhId") || !rootValue["dhId"].isString()) {
        return DCAMERA_BAD_VALUE;
    }
    dhId_ = rootValue["dhId"].asString();

    if (!rootValue.isMember("Command") || !rootValue["Command"].isString()) {
        return DCAMERA_BAD_VALUE;
    }
    command_ = rootValue["Command"].asString();

    if (!rootValue.isMember("Value") || !rootValue["Value"].isObject()) {
        return DCAMERA_BAD_VALUE;
    }
    Json::Value valueJson = rootValue["Value"];

    if (!valueJson.isMember("EventType") || !valueJson["EventType"].isInt()) {
        return DCAMERA_BAD_VALUE;
    }
    std::shared_ptr<DCameraEvent> event = std::make_shared<DCameraEvent>();
    event->eventType_ = valueJson["EventType"].asInt();

    if (!valueJson.isMember("EventResult") || !valueJson["EventResult"].isInt()) {
        return DCAMERA_BAD_VALUE;
    }
    event->eventResult_ = valueJson["EventResult"].asInt();

    if (!valueJson.isMember("EventContent") || !valueJson["EventContent"].isString()) {
        return DCAMERA_BAD_VALUE;
    }
    event->eventContent_ = valueJson["EventContent"].asString();

    value_ = event;
    return DCAMERA_OK;
}
} // namespace DistributedHardware
} // namespace OHOS
