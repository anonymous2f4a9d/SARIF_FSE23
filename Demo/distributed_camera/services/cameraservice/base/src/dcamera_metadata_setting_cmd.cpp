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

#include "dcamera_metadata_setting_cmd.h"

#include "json/json.h"

#include "distributed_camera_constants.h"
#include "distributed_camera_errno.h"
#include "distributed_hardware_log.h"

namespace OHOS {
namespace DistributedHardware {
int32_t DCameraMetadataSettingCmd::Marshal(std::string& jsonStr)
{
    Json::Value rootValue;
    rootValue["Type"] = Json::Value(type_);
    rootValue["dhId"] = Json::Value(dhId_);
    rootValue["Command"] = Json::Value(command_);

    Json::Value settings;
    for (auto iter = value_.begin(); iter != value_.end(); iter++) {
        Json::Value setting;
        setting["SettingType"] = (*iter)->type_;
        setting["SettingValue"] = (*iter)->value_;
        settings.append(setting);
    }

    rootValue["Value"] = settings;
    jsonStr = rootValue.toStyledString();
    return DCAMERA_OK;
}

int32_t DCameraMetadataSettingCmd::Unmarshal(const std::string& jsonStr)
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

    if (!rootValue.isMember("Value") || !rootValue["Value"].isArray()) {
        return DCAMERA_BAD_VALUE;
    }

    for (Json::ArrayIndex i = 0; i < rootValue["Value"].size(); i++) {
        Json::Value valueJsonEle = rootValue["Value"][i];
        if (!valueJsonEle.isMember("SettingType") || !valueJsonEle["SettingType"].isInt()) {
            return DCAMERA_BAD_VALUE;
        }
        if (!valueJsonEle.isMember("SettingValue") || !valueJsonEle["SettingValue"].isString()) {
            return DCAMERA_BAD_VALUE;
        }
        std::shared_ptr<DCameraSettings> setting = std::make_shared<DCameraSettings>();
        setting->type_ = (DCSettingsType)valueJsonEle["SettingType"].asInt();
        setting->value_ = valueJsonEle["SettingValue"].asString();
        value_.push_back(setting);
    }
    return DCAMERA_OK;
}
} // namespace DistributedHardware
} // namespace OHOS
