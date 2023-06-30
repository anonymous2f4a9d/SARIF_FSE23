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

#include "dcamera_source_handler.h"
#include "dcamera_source_callback.h"

#include "anonymous_string.h"
#include "dcamera_source_handler_ipc.h"
#include "dh_utils_tool.h"
#include "distributed_camera_errno.h"
#include "distributed_hardware_log.h"

namespace OHOS {
namespace DistributedHardware {
IMPLEMENT_SINGLE_INSTANCE(DCameraSourceHandler);

DCameraSourceHandler::~DCameraSourceHandler()
{
    DHLOGI("~DCameraSourceHandler");
}

int32_t DCameraSourceHandler::InitSource(const std::string& params)
{
    DHLOGI("DCameraSourceHandler InitSource Start");
    DCameraSourceHandlerIpc::GetInstance().Init();
    sptr<IDistributedCameraSource> dCameraSourceSrv = DCameraSourceHandlerIpc::GetInstance().GetSourceLocalDHMS();
    if (dCameraSourceSrv == nullptr) {
        DHLOGE("DCameraSourceHandler InitSource get Service failed");
        return DCAMERA_INIT_ERR;
    }

    callback_ = new DCameraSourceCallback();
    if (callback_ == nullptr) {
        DHLOGE("DCameraSourceHandler InitSource init callback failed");
        return DCAMERA_INIT_ERR;
    }
    int32_t ret = dCameraSourceSrv->InitSource(params, callback_);
    DHLOGI("DCameraSourceHandler InitSource end, ret: %d", ret);
    return ret;
}

int32_t DCameraSourceHandler::ReleaseSource()
{
    DHLOGI("DCameraSourceHandler ReleaseSource Start");
    sptr<IDistributedCameraSource> dCameraSourceSrv = DCameraSourceHandlerIpc::GetInstance().GetSourceLocalDHMS();
    if (dCameraSourceSrv == nullptr) {
        DHLOGE("DCameraSourceHandler ReleaseSource get Service failed");
        return DCAMERA_INIT_ERR;
    }
    dCameraSourceSrv->ReleaseSource();
    DCameraSourceHandlerIpc::GetInstance().UnInit();
    DHLOGI("DCameraSourceHandler ReleaseSource end");
    return DCAMERA_OK;
}

int32_t DCameraSourceHandler::RegisterDistributedHardware(const std::string& devId, const std::string& dhId,
    const EnableParam& param, std::shared_ptr<RegisterCallback> callback)
{
    DHLOGI("DCameraSourceHandler RegisterDistributedHardware devId: %s dhId: %s", GetAnonyString(devId).c_str(),
        GetAnonyString(dhId).c_str());
    sptr<IDistributedCameraSource> dCameraSourceSrv = DCameraSourceHandlerIpc::GetInstance().GetSourceLocalDHMS();
    if (dCameraSourceSrv == nullptr) {
        DHLOGE("DCameraSourceHandler RegisterDistributedHardware get Service failed");
        return DCAMERA_BAD_VALUE;
    }

    std::lock_guard<std::mutex> autoLock(optLock_);
    std::string reqId = GetRandomID();
    callback_->PushRegCallback(reqId, callback);
    int32_t ret = dCameraSourceSrv->RegisterDistributedHardware(devId, dhId, reqId, param);
    if (ret != DCAMERA_OK) {
        callback_->PopRegCallback(reqId);
    }
    DHLOGI("DCameraSourceHandler RegisterDistributedHardware end, ret: %d devId: %s dhId: %s version: %s",
        ret, GetAnonyString(devId).c_str(), GetAnonyString(dhId).c_str(), param.version.c_str());
    return ret;
}

int32_t DCameraSourceHandler::UnregisterDistributedHardware(const std::string& devId, const std::string& dhId,
    std::shared_ptr<UnregisterCallback> callback)
{
    DHLOGI("DCameraSourceHandler UnregisterDistributedHardware devId: %s dhId: %s", GetAnonyString(devId).c_str(),
        GetAnonyString(dhId).c_str());
    sptr<IDistributedCameraSource> dCameraSourceSrv = DCameraSourceHandlerIpc::GetInstance().GetSourceLocalDHMS();
    if (dCameraSourceSrv == nullptr) {
        DHLOGE("DCameraSourceHandler UnregisterDistributedHardware get Service failed");
        return DCAMERA_BAD_VALUE;
    }

    std::lock_guard<std::mutex> autoLock(optLock_);
    std::string reqId = GetRandomID();
    callback_->PushUnregCallback(reqId, callback);
    int32_t ret = dCameraSourceSrv->UnregisterDistributedHardware(devId, dhId, reqId);
    if (ret != DCAMERA_OK) {
        callback_->PopUnregCallback(reqId);
    }
    DHLOGI("DCameraSourceHandler UnregisterDistributedHardware end, ret: %d devId: %s dhId: %s", ret,
        GetAnonyString(devId).c_str(), GetAnonyString(dhId).c_str());
    return ret;
}

int32_t DCameraSourceHandler::ConfigDistributedHardware(const std::string& devId, const std::string& dhId,
    const std::string& key, const std::string& value)
{
    return DCAMERA_OK;
}

IDistributedHardwareSource *GetSourceHardwareHandler()
{
    DHLOGI("DCameraSourceHandler GetSourceHardwareHandler Start");
    return &DCameraSourceHandler::GetInstance();
}
} // namespace DistributedHardware
} // namespace OHOS
