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

#include "dcamera_capture_info_cmd.h"

#include "distributed_camera_constants.h"
#include "distributed_camera_errno.h"
#include "distributed_hardware_log.h"

namespace OHOS {
namespace DistributedHardware {
int32_t DCameraCaptureInfoCmd::Marshal(std::string& jsonStr)
{
    Json::Value rootValue;
    rootValue["Type"] = Json::Value(type_);
    rootValue["dhId"] = Json::Value(dhId_);
    rootValue["Command"] = Json::Value(command_);
    Json::Value captureInfos;
    for (auto iter = value_.begin(); iter != value_.end(); iter++) {
        Json::Value captureInfo;
        std::shared_ptr<DCameraCaptureInfo> capture = *iter;
        captureInfo["Width"] = Json::Value(capture->width_);
        captureInfo["Height"] = Json::Value(capture->height_);
        captureInfo["Format"] = Json::Value(capture->format_);
        captureInfo["DataSpace"] = Json::Value(capture->dataspace_);
        captureInfo["IsCapture"] = Json::Value(capture->isCapture_);
        captureInfo["EncodeType"] = Json::Value(capture->encodeType_);
        captureInfo["StreamType"] = Json::Value(capture->streamType_);

        Json::Value captureSettings;
        for (auto settingIter = capture->captureSettings_.begin();
            settingIter != capture->captureSettings_.end(); settingIter++) {
            Json::Value captureSetting;
            captureSetting["SettingType"] = Json::Value((*settingIter)->type_);
            captureSetting["SettingValue"] = Json::Value((*settingIter)->value_);
            captureSettings.append(captureSetting);
        }

        captureInfo["CaptureSettings"] = captureSettings;
        captureInfos.append(captureInfo);
    }

    rootValue["Value"] = captureInfos;
    jsonStr = rootValue.toStyledString();
    return DCAMERA_OK;
}

int32_t DCameraCaptureInfoCmd::Unmarshal(const std::string& jsonStr)
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

    int32_t ret = UmarshalValue(rootValue);
    if (ret != DCAMERA_OK) {
        return ret;
    }
    return DCAMERA_OK;
}

int32_t DCameraCaptureInfoCmd::UmarshalValue(Json::Value& rootValue)
{
    for (Json::ArrayIndex i = 0; i < rootValue["Value"].size(); i++) {
        Json::Value valueJson = rootValue["Value"][i];
        std::shared_ptr<DCameraCaptureInfo> captureInfo = std::make_shared<DCameraCaptureInfo>();
        if (!valueJson.isMember("Width") || !valueJson["Width"].isInt()) {
            return DCAMERA_BAD_VALUE;
        }
        captureInfo->width_ = valueJson["Width"].asInt();

        if (!valueJson.isMember("Height") || !valueJson["Height"].isInt()) {
            return DCAMERA_BAD_VALUE;
        }
        captureInfo->height_ = valueJson["Height"].asInt();

        if (!valueJson.isMember("Format") || !valueJson["Format"].isInt()) {
            return DCAMERA_BAD_VALUE;
        }
        captureInfo->format_ = valueJson["Format"].asInt();

        if (!valueJson.isMember("DataSpace") || !valueJson["DataSpace"].isInt()) {
            return DCAMERA_BAD_VALUE;
        }
        captureInfo->dataspace_ = valueJson["DataSpace"].asInt();

        if (!valueJson.isMember("IsCapture") || !valueJson["IsCapture"].isBool()) {
            return DCAMERA_BAD_VALUE;
        }
        captureInfo->isCapture_ = valueJson["IsCapture"].asBool();

        if (!valueJson.isMember("EncodeType") || !valueJson["EncodeType"].isInt()) {
            return DCAMERA_BAD_VALUE;
        }
        captureInfo->encodeType_ = (DCEncodeType)valueJson["EncodeType"].asInt();

        if (!valueJson.isMember("StreamType") || !valueJson["StreamType"].isInt()) {
            return DCAMERA_BAD_VALUE;
        }
        captureInfo->streamType_ = (DCStreamType)valueJson["StreamType"].asInt();

        if (!valueJson.isMember("CaptureSettings") || !valueJson["CaptureSettings"].isArray()) {
            return DCAMERA_BAD_VALUE;
        }

        int32_t ret = UmarshalSettings(valueJson, captureInfo);
        if (ret != DCAMERA_OK) {
            return ret;
        }
        value_.push_back(captureInfo);
    }
    return DCAMERA_OK;
}

int32_t DCameraCaptureInfoCmd::UmarshalSettings(Json::Value& valueJson,
    std::shared_ptr<DCameraCaptureInfo>& captureInfo)
{
    for (Json::ArrayIndex j = 0; j < valueJson["CaptureSettings"].size(); j++) {
        Json::Value settingJson = valueJson["CaptureSettings"][j];
        if (!settingJson.isMember("SettingType") || !settingJson["SettingType"].isInt()) {
            return DCAMERA_BAD_VALUE;
        }
        if (!settingJson.isMember("SettingValue") || !settingJson["SettingValue"].isString()) {
            return DCAMERA_BAD_VALUE;
        }
        std::shared_ptr<DCameraSettings> setting = std::make_shared<DCameraSettings>();
        setting->type_ = (DCSettingsType)settingJson["SettingType"].asInt();
        setting->value_ = settingJson["SettingValue"].asString();
        captureInfo->captureSettings_.push_back(setting);
    }
    return DCAMERA_OK;
}
} // namespace DistributedHardware
} // namespace OHOS
