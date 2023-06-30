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

#include "dcamera_channel_info_cmd.h"

#include "json/json.h"

#include "distributed_camera_constants.h"
#include "distributed_camera_errno.h"
#include "distributed_hardware_log.h"

namespace OHOS {
namespace DistributedHardware {
int32_t DCameraChannelInfoCmd::Marshal(std::string& jsonStr)
{
    Json::Value rootValue;
    rootValue["Type"] = Json::Value(type_);
    rootValue["dhId"] = Json::Value(dhId_);
    rootValue["Command"] = Json::Value(command_);

    Json::Value channelInfo;
    channelInfo["SourceDevId"] = Json::Value(value_->sourceDevId_);
    Json::Value details;
    for (auto iter = value_->detail_.begin(); iter != value_->detail_.end(); iter++) {
        Json::Value detail;
        detail["DataSessionFlag"] = Json::Value(iter->dataSessionFlag_);
        detail["StreamType"] = Json::Value(iter->streamType_);
        details.append(detail);
    }
    channelInfo["Detail"] = details;
    rootValue["Value"] = channelInfo;

    jsonStr = rootValue.toStyledString();
    return DCAMERA_OK;
}

int32_t DCameraChannelInfoCmd::Unmarshal(const std::string& jsonStr)
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

    if (!valueJson.isMember("SourceDevId") || !valueJson["SourceDevId"].isString()) {
        return DCAMERA_BAD_VALUE;
    }
    std::shared_ptr<DCameraChannelInfo> channelInfo = std::make_shared<DCameraChannelInfo>();
    channelInfo->sourceDevId_ = valueJson["SourceDevId"].asString();

    if (!valueJson.isMember("Detail") || !valueJson["Detail"].isArray()) {
        return DCAMERA_BAD_VALUE;
    }

    for (Json::ArrayIndex i = 0; i < valueJson["Detail"].size(); i++) {
        Json::Value detailJson = valueJson["Detail"][i];
        DCameraChannelDetail channelDetail;
        if (!detailJson.isMember("DataSessionFlag") || !detailJson["DataSessionFlag"].isString()) {
            return DCAMERA_BAD_VALUE;
        }
        if (!detailJson.isMember("StreamType") || !detailJson["StreamType"].isInt()) {
            return DCAMERA_BAD_VALUE;
        }
        channelDetail.dataSessionFlag_ = detailJson["DataSessionFlag"].asString();
        channelDetail.streamType_ = (DCStreamType)detailJson["StreamType"].asInt();
        channelInfo->detail_.push_back(channelDetail);
    }
    value_ = channelInfo;
    return DCAMERA_OK;
}
} // namespace DistributedHardware
} // namespace OHOS
