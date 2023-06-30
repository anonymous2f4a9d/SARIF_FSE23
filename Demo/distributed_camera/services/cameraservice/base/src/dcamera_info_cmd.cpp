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

#include "dcamera_info_cmd.h"

#include "json/json.h"

#include "distributed_camera_constants.h"
#include "distributed_camera_errno.h"
#include "distributed_hardware_log.h"

namespace OHOS {
namespace DistributedHardware {
int32_t DCameraInfoCmd::Marshal(std::string& jsonStr)
{
    Json::Value rootValue;
    rootValue["Type"] = Json::Value(type_);
    rootValue["dhId"] = Json::Value(dhId_);
    rootValue["Command"] = Json::Value(command_);

    Json::Value info;
    info["State"] = Json::Value(value_->state_);
    rootValue["Value"] = info;

    jsonStr = rootValue.toStyledString();
    return DCAMERA_OK;
}

int32_t DCameraInfoCmd::Unmarshal(const std::string& jsonStr)
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

    if (!valueJson.isMember("State") || !valueJson["State"].isInt()) {
        return DCAMERA_BAD_VALUE;
    }
    std::shared_ptr<DCameraInfo> info = std::make_shared<DCameraInfo>();
    info->state_ = valueJson["State"].asInt();

    value_ = info;
    return DCAMERA_OK;
}
} // namespace DistributedHardware
} // namespace OHOS
