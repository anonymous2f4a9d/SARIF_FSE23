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

#include "distributed_camera_source_service.h"

#include "if_system_ability_manager.h"
#include "ipc_skeleton.h"
#include "ipc_types.h"
#include "iservice_registry.h"
#include "string_ex.h"
#include "system_ability_definition.h"

#include "anonymous_string.h"
#include "dcamera_service_state_listener.h"
#include "dcamera_source_service_ipc.h"
#include "distributed_camera_errno.h"
#include "distributed_hardware_log.h"

namespace OHOS {
namespace DistributedHardware {
REGISTER_SYSTEM_ABILITY_BY_ID(DistributedCameraSourceService, DISTRIBUTED_HARDWARE_CAMERA_SOURCE_SA_ID, true);

std::map<DCameraIndex, std::shared_ptr<DCameraSourceDev>> DistributedCameraSourceService::camerasMap_;

DistributedCameraSourceService::DistributedCameraSourceService(int32_t saId, bool runOnCreate)
    : SystemAbility(saId, runOnCreate)
{
}

void DistributedCameraSourceService::OnStart()
{
    DHLOGI("DistributedCameraSourceService::OnStart");
    if (state_ == DCameraServiceState::DCAMERA_SRV_STATE_RUNNING) {
        DHLOGI("DistributedCameraSourceService has already started.");
        return;
    }

    if (!Init()) {
        DHLOGE("DistributedCameraSourceService init failed");
        return;
    }
    state_ = DCameraServiceState::DCAMERA_SRV_STATE_RUNNING;
    DHLOGI("DCameraServiceState OnStart service success.");
}

bool DistributedCameraSourceService::Init()
{
    DHLOGI("DistributedCameraSourceService start init");
    DCameraSourceServiceIpc::GetInstance().Init();
    if (!registerToService_) {
        bool ret = Publish(this);
        if (!ret) {
            DHLOGE("DistributedCameraSourceService Publish service failed");
            return false;
        }
        registerToService_ = true;
    }
    DHLOGI("DistributedCameraSourceService init success");
    return true;
}

void DistributedCameraSourceService::OnStop()
{
    DHLOGI("DistributedCameraSourceService OnStop service");
    state_ = DCameraServiceState::DCAMERA_SRV_STATE_NOT_START;
    registerToService_ = false;
    DCameraSourceServiceIpc::GetInstance().UnInit();
}

int32_t DistributedCameraSourceService::InitSource(const std::string& params,
    const sptr<IDCameraSourceCallback>& callback)
{
    DHLOGI("DistributedCameraSourceService InitSource param: %s", params.c_str());
    int32_t ret = LoadDCameraHDF();
    if (ret != DCAMERA_OK) {
        DHLOGE("DistributedCameraSourceService InitSource LoadHDF failed, ret: %d", ret);
        return ret;
    }
    sourceVer_ = params;
    callbackProxy_ = callback;
    return DCAMERA_OK;
}

int32_t DistributedCameraSourceService::ReleaseSource()
{
    DHLOGI("DistributedCameraSourceService ReleaseSource");
    int32_t ret = UnLoadCameraHDF();
    if (ret != DCAMERA_OK) {
        DHLOGE("DistributedCameraSourceService ReleaseSource UnLoadHDF failed, ret: %d", ret);
        return ret;
    }
    callbackProxy_ = nullptr;
    return DCAMERA_OK;
}

int32_t DistributedCameraSourceService::RegisterDistributedHardware(const std::string& devId, const std::string& dhId,
    const std::string& reqId, const EnableParam& param)
{
    DHLOGI("DistributedCameraSourceService RegisterDistributedHardware devId: %s, dhId: %s, version: %s",
        GetAnonyString(devId).c_str(), GetAnonyString(dhId).c_str(), param.version.c_str());
    DCameraIndex camIndex(devId, dhId);
    std::shared_ptr<DCameraSourceDev> camDev = nullptr;
    int32_t ret = DCAMERA_OK;
    auto iter = camerasMap_.find(camIndex);
    if (iter == camerasMap_.end()) {
        DHLOGI("DistributedCameraSourceService RegisterDistributedHardware new dev devId: %s, dhId: %s, version: %s",
            GetAnonyString(devId).c_str(), GetAnonyString(dhId).c_str(), param.version.c_str());
        std::shared_ptr<ICameraStateListener> listener = std::make_shared<DCameraServiceStateListener>(callbackProxy_);
        camDev = std::make_shared<DCameraSourceDev>(devId, dhId, listener);
        if (camDev == nullptr) {
            return DCAMERA_MEMORY_OPT_ERROR;
        }
        ret = camDev->InitDCameraSourceDev();
        if (ret != DCAMERA_OK) {
            DHLOGE("DistributedCameraSourceService RegisterDistributedHardware failed %d InitDev devId: %s, dhId: %s",
                ret, GetAnonyString(devId).c_str(), GetAnonyString(dhId).c_str());
            return ret;
        }
        camerasMap_.emplace(camIndex, camDev);
    } else {
        DHLOGI("DistributedCameraSourceService RegisterDistributedHardware exist devId: %s, dhId: %s, version: %s",
            GetAnonyString(devId).c_str(), GetAnonyString(dhId).c_str(), param.version.c_str());
        camDev = iter->second;
    }

    ret = camDev->RegisterDistributedHardware(devId, dhId, reqId, param.version, param.attrs);
    if (ret != DCAMERA_OK) {
        DHLOGE("DistributedCameraSourceService RegisterDistributedHardware failed, ret: %d", ret);
        camerasMap_.erase(camIndex);
    }
    DHLOGI("DistributedCameraSourceService RegisterDistributedHardware end devId: %s, dhId: %s, version: %s",
        GetAnonyString(devId).c_str(), GetAnonyString(dhId).c_str(), param.version.c_str());
    return ret;
}

int32_t DistributedCameraSourceService::UnregisterDistributedHardware(const std::string& devId, const std::string& dhId,
    const std::string& reqId)
{
    DHLOGI("DistributedCameraSourceService UnregisterDistributedHardware devId: %s, dhId: %s",
        GetAnonyString(devId).c_str(), GetAnonyString(dhId).c_str());
    DCameraIndex camIndex(devId, dhId);
    auto iter = camerasMap_.find(camIndex);
    if (iter == camerasMap_.end()) {
        DHLOGE("DistributedCameraSourceService UnregisterDistributedHardware not found device");
        return DCAMERA_NOT_FOUND;
    }

    std::shared_ptr<DCameraSourceDev> camDev = iter->second;
    int32_t ret = camDev->UnRegisterDistributedHardware(devId, dhId, reqId);
    if (ret != DCAMERA_OK) {
        DHLOGE("DistributedCameraSourceService UnregisterDistributedHardware failed, ret: %d", ret);
    }
    return ret;
}

int32_t DistributedCameraSourceService::DCameraNotify(const std::string& devId, const std::string& dhId,
    std::string& events)
{
    DHLOGI("DistributedCameraSourceService DCameraNotify devId: %s, dhId: %s", GetAnonyString(devId).c_str(),
        GetAnonyString(dhId).c_str());
    DCameraIndex camIndex(devId, dhId);
    auto iter = camerasMap_.find(camIndex);
    if (iter == camerasMap_.end()) {
        DHLOGE("DistributedCameraSourceService DCameraNotify not found device");
        return DCAMERA_NOT_FOUND;
    }

    std::shared_ptr<DCameraSourceDev> camDev = iter->second;
    int32_t ret = camDev->DCameraNotify(events);
    if (ret != DCAMERA_OK) {
        DHLOGE("DistributedCameraSourceService DCameraNotify failed, ret: %d", ret);
    }
    return ret;
}

int32_t DistributedCameraSourceService::LoadDCameraHDF()
{
    return DCAMERA_OK;
}

int32_t DistributedCameraSourceService::UnLoadCameraHDF()
{
    return DCAMERA_OK;
}
} // namespace DistributedHardware
} // namespace OHOS
