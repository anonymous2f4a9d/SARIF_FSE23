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

#include "dcamera_source_callback.h"

#include "anonymous_string.h"
#include "distributed_camera_errno.h"
#include "distributed_hardware_log.h"

namespace OHOS {
namespace DistributedHardware {
DCameraSourceCallback::~DCameraSourceCallback()
{
    regCallbacks_.clear();
    unregCallbacks_.clear();
}

int32_t DCameraSourceCallback::OnNotifyRegResult(const std::string& devId, const std::string& dhId,
    const std::string& reqId, int32_t status, std::string& data)
{
    DHLOGI("DCameraSourceCallback OnNotifyRegResult devId: %s dhId: %s",
        GetAnonyString(devId).c_str(), GetAnonyString(dhId).c_str());
    auto iter = regCallbacks_.find(reqId);
    if (iter == regCallbacks_.end()) {
        DHLOGE("DCameraSourceCallback OnNotifyRegResult not found devId: %s dhId: %s",
            GetAnonyString(devId).c_str(), GetAnonyString(dhId).c_str());
        return DCAMERA_NOT_FOUND;
    }
    int32_t ret = iter->second->OnRegisterResult(devId, dhId, status, data);
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSourceCallback OnNotifyRegResult failed, devId: %s dhId: %s ret: %d",
            GetAnonyString(devId).c_str(), GetAnonyString(dhId).c_str(), ret);
    }
    regCallbacks_.erase(iter);
    return ret;
}

int32_t DCameraSourceCallback::OnNotifyUnregResult(const std::string& devId, const std::string& dhId,
    const std::string& reqId, int32_t status, std::string& data)
{
    DHLOGI("DCameraSourceCallback OnNotifyUnregResult devId: %s dhId: %s",
        GetAnonyString(devId).c_str(), GetAnonyString(dhId).c_str());
    auto iter = unregCallbacks_.find(reqId);
    if (iter == unregCallbacks_.end()) {
        DHLOGE("DCameraSourceCallback OnNotifyUnregResult not found devId: %s dhId: %s",
            GetAnonyString(devId).c_str(), GetAnonyString(dhId).c_str());
        return DCAMERA_NOT_FOUND;
    }
    int32_t ret = iter->second->OnUnregisterResult(devId, dhId, status, data);
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSourceCallback OnNotifyUnregResult failed, devId: %s dhId: %s ret: %d",
            GetAnonyString(devId).c_str(), GetAnonyString(dhId).c_str(), ret);
    }
    unregCallbacks_.erase(iter);
    return ret;
}

void DCameraSourceCallback::PushRegCallback(std::string& reqId, std::shared_ptr<RegisterCallback>& callback)
{
    regCallbacks_.emplace(reqId, callback);
}

void DCameraSourceCallback::PopRegCallback(std::string& reqId)
{
    regCallbacks_.erase(reqId);
}

void DCameraSourceCallback::PushUnregCallback(std::string& reqId, std::shared_ptr<UnregisterCallback>& callback)
{
    unregCallbacks_.emplace(reqId, callback);
}

void DCameraSourceCallback::PopUnregCallback(std::string& reqId)
{
    unregCallbacks_.erase(reqId);
}
} // namespace DistributedHardware
} // namespace OHOS
