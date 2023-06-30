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

#include "distributed_camera_sink_service.h"

#include "if_system_ability_manager.h"
#include "ipc_skeleton.h"
#include "ipc_types.h"
#include "iservice_registry.h"
#include "string_ex.h"
#include "system_ability_definition.h"

#include "anonymous_string.h"
#include "dcamera_handler.h"
#include "dcamera_sink_service_ipc.h"
#include "distributed_camera_errno.h"
#include "distributed_hardware_log.h"

namespace OHOS {
namespace DistributedHardware {
REGISTER_SYSTEM_ABILITY_BY_ID(DistributedCameraSinkService, DISTRIBUTED_HARDWARE_CAMERA_SINK_SA_ID, true);

DistributedCameraSinkService::DistributedCameraSinkService(int32_t saId, bool runOnCreate)
    : SystemAbility(saId, runOnCreate)
{
}

void DistributedCameraSinkService::OnStart()
{
    DHLOGI("DistributedCameraSinkService::OnStart");
    if (state_ == DCameraServiceState::DCAMERA_SRV_STATE_RUNNING) {
        DHLOGI("DistributedCameraSinkService has already started.");
        return;
    }

    if (!Init()) {
        DHLOGE("DistributedCameraSinkService init failed");
        return;
    }
    state_ = DCameraServiceState::DCAMERA_SRV_STATE_RUNNING;
    DHLOGI("DCameraServiceState OnStart service success.");
}

bool DistributedCameraSinkService::Init()
{
    DHLOGI("DistributedCameraSinkService start init");
    DCameraSinkServiceIpc::GetInstance().Init();
    if (!registerToService_) {
        bool ret = Publish(this);
        if (!ret) {
            DHLOGE("DistributedCameraSinkService Publish service failed");
            return false;
        }
        registerToService_ = true;
    }
    DHLOGI("DistributedCameraSinkService init success");
    return true;
}

void DistributedCameraSinkService::OnStop()
{
    DHLOGI("DistributedCameraSinkService OnStop service");
    state_ = DCameraServiceState::DCAMERA_SRV_STATE_NOT_START;
    registerToService_ = false;
    DCameraSinkServiceIpc::GetInstance().UnInit();
}

int32_t DistributedCameraSinkService::InitSink(const std::string& params)
{
    DHLOGI("DistributedCameraSinkService::InitSink");
    sinkVer_ = params;
    int32_t ret = DCameraHandler::GetInstance().Initialize();
    if (ret != DCAMERA_OK) {
        DHLOGE("DistributedCameraSinkService::InitSink handler initialize failed, ret: %d", ret);
        return ret;
    }

    std::vector<std::string> cameras = DCameraHandler::GetInstance().GetCameras();
    if (cameras.empty()) {
        DHLOGE("DistributedCameraSinkService::InitSink no camera device");
        return DCAMERA_BAD_VALUE;
    }
    for (auto& dhId : cameras) {
        std::shared_ptr<DCameraSinkDev> sinkDevice = std::make_shared<DCameraSinkDev>(dhId);
        ret = sinkDevice->Init();
        if (ret != DCAMERA_OK) {
            DHLOGE("DistributedCameraSinkService::InitSink sink device init failed, ret: %d", ret);
            return ret;
        }
        camerasMap_.emplace(dhId, sinkDevice);
    }
    DHLOGI("DistributedCameraSinkService::InitSink success");
    return DCAMERA_OK;
}

int32_t DistributedCameraSinkService::ReleaseSink()
{
    DHLOGI("DistributedCameraSinkService::ReleaseSink");
    for (auto iter = camerasMap_.begin(); iter != camerasMap_.end(); iter++) {
        std::shared_ptr<DCameraSinkDev> sinkDevice = iter->second;
        int32_t ret = sinkDevice->UnInit();
        if (ret != DCAMERA_OK) {
            DHLOGE("DistributedCameraSinkService::ReleaseSink release sink device failed, ret: %d", ret);
        }
    }
    camerasMap_.clear();
    DHLOGI("DistributedCameraSinkService::ReleaseSink success");
    return DCAMERA_OK;
}

int32_t DistributedCameraSinkService::SubscribeLocalHardware(const std::string& dhId, const std::string& parameters)
{
    DHLOGI("DistributedCameraSinkService::SubscribeLocalHardware dhId: %s", GetAnonyString(dhId).c_str());
    auto iter = camerasMap_.find(dhId);
    if (iter == camerasMap_.end()) {
        DHLOGE("DistributedCameraSinkService::SubscribeLocalHardware device not found");
        return DCAMERA_NOT_FOUND;
    }

    std::shared_ptr<DCameraSinkDev> sinkDevice = iter->second;
    int32_t ret = sinkDevice->SubscribeLocalHardware(parameters);
    if (ret != DCAMERA_OK) {
        DHLOGE("DistributedCameraSinkService::SubscribeLocalHardware failed, ret: %d", ret);
        return ret;
    }
    DHLOGI("DistributedCameraSinkService::SubscribeLocalHardware success");
    return DCAMERA_OK;
}

int32_t DistributedCameraSinkService::UnsubscribeLocalHardware(const std::string& dhId)
{
    DHLOGI("DistributedCameraSinkService::UnsubscribeLocalHardware dhId: %s", GetAnonyString(dhId).c_str());
    auto iter = camerasMap_.find(dhId);
    if (iter == camerasMap_.end()) {
        DHLOGE("DistributedCameraSinkService::UnsubscribeLocalHardware device not found");
        return DCAMERA_NOT_FOUND;
    }

    std::shared_ptr<DCameraSinkDev> sinkDevice = iter->second;
    int32_t ret = sinkDevice->UnsubscribeLocalHardware();
    if (ret != DCAMERA_OK) {
        DHLOGE("DistributedCameraSinkService::UnsubscribeLocalHardware failed, ret: %d", ret);
        return ret;
    }
    DHLOGI("DistributedCameraSinkService::UnsubscribeLocalHardware success");
    return DCAMERA_OK;
}

int32_t DistributedCameraSinkService::StopCapture(const std::string& dhId)
{
    DHLOGI("DistributedCameraSinkService::StopCapture dhId: %s", GetAnonyString(dhId).c_str());
    auto iter = camerasMap_.find(dhId);
    if (iter == camerasMap_.end()) {
        DHLOGE("DistributedCameraSinkService::StopCapture device not found");
        return DCAMERA_NOT_FOUND;
    }

    std::shared_ptr<DCameraSinkDev> sinkDevice = iter->second;
    int32_t ret = sinkDevice->StopCapture();
    if (ret != DCAMERA_OK) {
        DHLOGE("DistributedCameraSinkService::StopCapture failed, ret: %d", ret);
        return ret;
    }
    DHLOGI("DistributedCameraSinkService::StopCapture success");
    return DCAMERA_OK;
}

int32_t DistributedCameraSinkService::ChannelNeg(const std::string& dhId, std::string& channelInfo)
{
    DHLOGI("DistributedCameraSinkService::ChannelNeg dhId: %s", GetAnonyString(dhId).c_str());
    auto iter = camerasMap_.find(dhId);
    if (iter == camerasMap_.end()) {
        DHLOGE("DistributedCameraSinkService::ChannelNeg device not found");
        return DCAMERA_NOT_FOUND;
    }

    std::shared_ptr<DCameraSinkDev> sinkDevice = iter->second;
    int32_t ret = sinkDevice->ChannelNeg(channelInfo);
    if (ret != DCAMERA_OK) {
        DHLOGE("DistributedCameraSinkService::ChannelNeg failed, ret: %d", ret);
        return ret;
    }
    DHLOGI("DistributedCameraSinkService::ChannelNeg success");
    return DCAMERA_OK;
}

int32_t DistributedCameraSinkService::GetCameraInfo(const std::string& dhId, std::string& cameraInfo)
{
    DHLOGI("DistributedCameraSinkService::GetCameraInfo dhId: %s", GetAnonyString(dhId).c_str());
    auto iter = camerasMap_.find(dhId);
    if (iter == camerasMap_.end()) {
        DHLOGE("DistributedCameraSinkService::GetCameraInfo device not found");
        return DCAMERA_NOT_FOUND;
    }

    std::shared_ptr<DCameraSinkDev> sinkDevice = iter->second;
    int32_t ret = sinkDevice->GetCameraInfo(cameraInfo);
    if (ret != DCAMERA_OK) {
        DHLOGE("DistributedCameraSinkService::GetCameraInfo failed, ret: %d", ret);
        return ret;
    }
    DHLOGI("DistributedCameraSinkService::GetCameraInfo success");
    return DCAMERA_OK;
}

int32_t DistributedCameraSinkService::OpenChannel(const std::string& dhId, std::string& openInfo)
{
    DHLOGI("DistributedCameraSinkService::OpenChannel dhId: %s", GetAnonyString(dhId).c_str());
    auto iter = camerasMap_.find(dhId);
    if (iter == camerasMap_.end()) {
        DHLOGE("DistributedCameraSinkService::OpenChannel device not found");
        return DCAMERA_NOT_FOUND;
    }

    std::shared_ptr<DCameraSinkDev> sinkDevice = iter->second;
    int32_t ret = sinkDevice->OpenChannel(openInfo);
    if (ret != DCAMERA_OK) {
        DHLOGE("DistributedCameraSinkService::OpenChannel failed, ret: %d", ret);
        return ret;
    }
    DHLOGI("DistributedCameraSinkService::OpenChannel success");
    return DCAMERA_OK;
}

int32_t DistributedCameraSinkService::CloseChannel(const std::string& dhId)
{
    DHLOGI("DistributedCameraSinkService::CloseChannel dhId: %s", GetAnonyString(dhId).c_str());
    auto iter = camerasMap_.find(dhId);
    if (iter == camerasMap_.end()) {
        DHLOGE("DistributedCameraSinkService::CloseChannel device not found");
        return DCAMERA_NOT_FOUND;
    }

    std::shared_ptr<DCameraSinkDev> sinkDevice = iter->second;
    int32_t ret = sinkDevice->CloseChannel();
    if (ret != DCAMERA_OK) {
        DHLOGE("DistributedCameraSinkService::CloseChannel failed, ret: %d", ret);
        return ret;
    }
    DHLOGI("DistributedCameraSinkService::CloseChannel success");
    return DCAMERA_OK;
}
} // namespace DistributedHardware
} // namespace OHOS