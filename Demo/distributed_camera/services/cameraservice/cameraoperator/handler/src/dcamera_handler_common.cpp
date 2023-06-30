/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "dcamera_handler.h"

#include "anonymous_string.h"
#include "avcodec_info.h"
#include "avcodec_list.h"
#include "dcamera_manager_callback.h"
#include "dcamera_utils_tools.h"
#include "distributed_camera_constants.h"
#include "distributed_camera_errno.h"
#include "distributed_hardware_log.h"
#include "metadata_utils.h"

namespace OHOS {
namespace DistributedHardware {
IMPLEMENT_SINGLE_INSTANCE(DCameraHandler);

DCameraHandler::~DCameraHandler()
{
    DHLOGI("~DCameraHandlerCommon");
}

int32_t DCameraHandler::Initialize()
{
    DHLOGI("DCameraHandlerCommon::Initialize");
    cameraManager_ = CameraStandard::CameraManager::GetInstance();
    if (cameraManager_ == nullptr) {
        DHLOGE("DCameraHandlerCommon::Initialize cameraManager getInstance failed");
        return DCAMERA_INIT_ERR;
    }
    std::shared_ptr<DCameraManagerCallback> cameraMgrCallback = std::make_shared<DCameraManagerCallback>();
    cameraManager_->SetCallback(cameraMgrCallback);
    DHLOGI("DCameraHandlerCommon::Initialize success");
    return DCAMERA_OK;
}

std::vector<DHItem> DCameraHandler::Query()
{
    std::vector<DHItem> itemList;
    std::vector<sptr<CameraStandard::CameraInfo>> cameraList = cameraManager_->GetCameras();
    DHLOGI("DCameraHandlerCommon::Query get %d cameras", cameraList.size());
    if (cameraList.empty()) {
        DHLOGE("DCameraHandlerCommon::Query no camera device");
        return itemList;
    }
    for (auto& info : cameraList) {
        if (info->GetConnectionType() != OHOS_CAMERA_CONNECTION_TYPE_BUILTIN) {
            DHLOGI("DCameraHandlerCommon::Query connection type: %d", info->GetConnectionType());
            continue;
        }
        if ((info->GetPosition() == OHOS_CAMERA_POSITION_OTHER) ||
            (info->GetPosition() == OHOS_CAMERA_POSITION_FRONT) ||
            (info->GetPosition() == OHOS_CAMERA_POSITION_BACK &&
             info->GetCameraType() == OHOS_CAMERA_TYPE_LOGICAL)) {
            DHItem item = CreateDHItem(info);
            itemList.push_back(item);
        }
    }
    DHLOGI("DCameraHandlerCommon::Query success, get %d items", itemList.size());
    return itemList;
}

std::map<std::string, std::string> DCameraHandler::QueryExtraInfo()
{
    DHLOGI("DCameraHandler::QueryExtraInfo");
    std::map<std::string, std::string> extraInfo;
    return extraInfo;
}

bool DCameraHandler::IsSupportPlugin()
{
    DHLOGI("DCameraHandler::IsSupportPlugin");
    return false;
}

void DCameraHandler::RegisterPluginListener(std::shared_ptr<PluginListener> listener)
{
    DHLOGI("DCameraHandler::RegisterPluginListener");
    if (listener == nullptr) {
        DHLOGE("DCameraHandler unregistering plugin listener");
    }
    pluginListener_ = listener;
}

std::vector<std::string> DCameraHandler::GetCameras()
{
    std::vector<std::string> cameras;
    std::vector<sptr<CameraStandard::CameraInfo>> cameraList = cameraManager_->GetCameras();
    DHLOGI("DCameraHandlerCommon::GetCameras get %d cameras", cameraList.size());
    if (cameraList.empty()) {
        DHLOGE("DCameraHandlerCommon::GetCameras no camera device");
        return cameras;
    }
    for (auto& info : cameraList) {
        if (info->GetConnectionType() != OHOS_CAMERA_CONNECTION_TYPE_BUILTIN) {
            DHLOGI("DCameraHandlerCommon::GetCameras connection type: %d", info->GetConnectionType());
            continue;
        }
        if ((info->GetPosition() == OHOS_CAMERA_POSITION_OTHER) ||
            (info->GetPosition() == OHOS_CAMERA_POSITION_FRONT) ||
            (info->GetPosition() == OHOS_CAMERA_POSITION_BACK &&
             info->GetCameraType() == OHOS_CAMERA_TYPE_LOGICAL)) {
            std::string dhId = CAMERA_ID_PREFIX + info->GetID();
            cameras.push_back(dhId);
        }
    }
    DHLOGI("DCameraHandlerCommon::GetCameras success, get %d items", cameras.size());
    return cameras;
}

DHItem DCameraHandler::CreateDHItem(sptr<CameraStandard::CameraInfo>& info)
{
    DHItem item;
    std::string id = info->GetID();
    item.dhId = CAMERA_ID_PREFIX + id;
    DHLOGI("DCameraHandlerCommon::CreateDHItem camera id: %s", GetAnonyString(id).c_str());

    Json::Value root;
    root[CAMERA_PROTOCOL_VERSION_KEY] = Json::Value(CAMERA_PROTOCOL_VERSION_VALUE);
    root[CAMERA_POSITION_KEY] = Json::Value(GetCameraPosition(info->GetPosition()));
    root[CAMERA_CODEC_TYPE_KEY].append("OMX_hisi_video_encoder_avc");

    sptr<CameraStandard::CameraInput> cameraInput = cameraManager_->CreateCameraInput(info);
    if (cameraInput == nullptr) {
        DHLOGE("DCameraHandlerCommon::CreateDHItem create cameraInput failed");
        return item;
    }

    Json::Value outputFormat;
    Json::Value resolution;
    std::set<camera_format_t> formatSet;

    std::vector<camera_format_t> videoFormats;
    videoFormats.push_back(camera_format_t::OHOS_CAMERA_FORMAT_RGBA_8888);
    ConfigInfo videoConfig = {CONTINUOUS_FRAME, CAMERA_FORMAT_VIDEO, cameraInput};
    ConfigFormatAndResolution(videoConfig, outputFormat,  resolution, videoFormats, formatSet);

    std::vector<camera_format_t> previewFormats;
    previewFormats.push_back(camera_format_t::OHOS_CAMERA_FORMAT_RGBA_8888);
    ConfigInfo previewInfo = {CONTINUOUS_FRAME, CAMERA_FORMAT_PREVIEW, cameraInput};
    ConfigFormatAndResolution(previewInfo, outputFormat, resolution, previewFormats, formatSet);

    std::vector<camera_format_t> photoFormats;
    photoFormats.push_back(camera_format_t::OHOS_CAMERA_FORMAT_RGBA_8888);
    ConfigInfo photoConfig = {SNAPSHOT_FRAME, CAMERA_FORMAT_PHOTO, cameraInput};
    ConfigFormatAndResolution(photoConfig, outputFormat, resolution, photoFormats, formatSet);

    root[CAMERA_FORMAT_KEY] = outputFormat;
    root[CAMERA_RESOLUTION_KEY] = resolution;

    std::string abilityString = cameraInput->GetCameraSettings();
    std::string encodeString = Base64Encode(reinterpret_cast<const unsigned char *>(abilityString.c_str()),
                                            abilityString.length());
    root[CAMERA_METADATA_KEY] = Json::Value(encodeString);

    item.attrs = root.toStyledString();
    cameraInput->Release();
    return item;
}

std::string DCameraHandler::GetCameraPosition(camera_position_enum_t position)
{
    DHLOGI("DCameraHandler::GetCameraPosition position: %d", position);
    std::string ret = "";
    switch (position) {
        case OHOS_CAMERA_POSITION_BACK: {
            ret = CAMERA_POSITION_BACK;
            break;
        }
        case OHOS_CAMERA_POSITION_FRONT: {
            ret = CAMERA_POSITION_FRONT;
            break;
        }
        case OHOS_CAMERA_POSITION_OTHER: {
            ret = CAMERA_POSITION_UNSPECIFIED;
            break;
        }
        default: {
            DHLOGE("DCameraHandler::GetCameraPosition unknown camera position");
            break;
        }
    }
    DHLOGI("DCameraHandler::GetCameraPosition success ret: %s", ret.c_str());
    return ret;
}

void DCameraHandler::ConfigFormatAndResolution(ConfigInfo& info, Json::Value& outputFormat, Json::Value& resolution,
    std::vector<camera_format_t>& formatList, std::set<camera_format_t>& formatSet)
{
    DHLOGI("DCameraHandlerCommon::ConfigFormatAndResolution camera format size: %d", formatList.size());
    for (auto& format : formatList) {
        DHLOGI("DCameraHandlerCommon::ConfigFormatAndResolution %s format: %d", info.formatKey.c_str(), format);
        outputFormat[info.formatKey].append(format);
        if (formatSet.insert(format).second) {
            std::string keyName = std::to_string(format);
            resolution[keyName].append("640*480");
        }
    }
}

bool DCameraHandler::IsValid(DCStreamType type, CameraStandard::CameraPicSize& size)
{
    bool ret = false;
    switch (type) {
        case CONTINUOUS_FRAME: {
            ret = (size.width >= RESOLUTION_MIN_WIDTH) &&
                    (size.height >= RESOLUTION_MIN_HEIGHT) &&
                    (size.width <= RESOLUTION_MAX_WIDTH_CONTINUOUS) &&
                    (size.height <= RESOLUTION_MAX_HEIGHT_CONTINUOUS);
            break;
        }
        case SNAPSHOT_FRAME: {
            ret = (size.width >= RESOLUTION_MIN_WIDTH) &&
                    (size.height >= RESOLUTION_MIN_HEIGHT) &&
                    (size.width <= RESOLUTION_MAX_WIDTH_SNAPSHOT) &&
                    (size.height <= RESOLUTION_MAX_HEIGHT_SNAPSHOT);
            break;
        }
        default: {
            DHLOGE("DCameraHandler::isValid unknown stream type");
            break;
        }
    }
    return ret;
}

IHardwareHandler* GetHardwareHandler()
{
    DHLOGI("DCameraHandler::GetHardwareHandler");
    return &DCameraHandler::GetInstance();
}
} // namespace DistributedHardware
} // namespace OHOS