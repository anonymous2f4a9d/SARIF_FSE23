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

#include "dcamera_sink_handler.h"

#include "anonymous_string.h"
#include "dcamera_sink_handler_ipc.h"
#include "distributed_camera_errno.h"
#include "distributed_hardware_log.h"

namespace OHOS {
namespace DistributedHardware {
IMPLEMENT_SINGLE_INSTANCE(DCameraSinkHandler);

DCameraSinkHandler::~DCameraSinkHandler()
{
    DHLOGI("~DCameraSinkHandler");
}

int32_t DCameraSinkHandler::InitSink(const std::string& params)
{
    DHLOGI("DCameraSinkHandler::InitSink");
    DCameraSinkHandlerIpc::GetInstance().Init();
    sptr<IDistributedCameraSink> dCameraSinkSrv = DCameraSinkHandlerIpc::GetInstance().GetSinkLocalDHMS();
    if (dCameraSinkSrv == nullptr) {
        DHLOGE("DCameraSinkHandler::InitSink get Service failed");
        return DCAMERA_INIT_ERR;
    }
    return dCameraSinkSrv->InitSink(params);
}

int32_t DCameraSinkHandler::ReleaseSink()
{
    DHLOGI("DCameraSinkHandler::ReleaseSink");
    sptr<IDistributedCameraSink> dCameraSinkSrv = DCameraSinkHandlerIpc::GetInstance().GetSinkLocalDHMS();
    if (dCameraSinkSrv == nullptr) {
        DHLOGE("DCameraSinkHandler::ReleaseSink get Service failed");
        return DCAMERA_BAD_VALUE;
    }

    int32_t ret = dCameraSinkSrv->ReleaseSink();
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSinkHandler::ReleaseSink sink service release failed, ret: %d", ret);
        return ret;
    }

    DCameraSinkHandlerIpc::GetInstance().UnInit();
    DHLOGI("DCameraSinkHandler::ReleaseSink success");
    return DCAMERA_OK;
}

int32_t DCameraSinkHandler::SubscribeLocalHardware(const std::string& dhId, const std::string& parameters)
{
    DHLOGI("DCameraSinkHandler::SubscribeLocalHardware dhId: %s", GetAnonyString(dhId).c_str());
    sptr<IDistributedCameraSink> dCameraSinkSrv = DCameraSinkHandlerIpc::GetInstance().GetSinkLocalDHMS();
    if (dCameraSinkSrv == nullptr) {
        DHLOGE("DCameraSinkHandler::SubscribeLocalHardware get Service failed");
        return DCAMERA_BAD_VALUE;
    }
    return dCameraSinkSrv->SubscribeLocalHardware(dhId, parameters);
}

int32_t DCameraSinkHandler::UnsubscribeLocalHardware(const std::string& dhId)
{
    DHLOGI("DCameraSinkHandler::UnsubscribeLocalHardware dhId: %s", GetAnonyString(dhId).c_str());
    sptr<IDistributedCameraSink> dCameraSinkSrv = DCameraSinkHandlerIpc::GetInstance().GetSinkLocalDHMS();
    if (dCameraSinkSrv == nullptr) {
        DHLOGE("DCameraSinkHandler::UnsubscribeLocalHardware get Service failed");
        return DCAMERA_BAD_VALUE;
    }
    return dCameraSinkSrv->UnsubscribeLocalHardware(dhId);
}

IDistributedHardwareSink *GetSinkHardwareHandler()
{
    DHLOGI("DCameraSinkHandler::GetSinkHardwareHandler");
    return &DCameraSinkHandler::GetInstance();
}
} // namespace DistributedHardware
} // namespace OHOS