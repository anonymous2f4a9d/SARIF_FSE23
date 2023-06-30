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

#include "dcamera_source_service_ipc.h"

#include "if_system_ability_manager.h"
#include "iservice_registry.h"

#include "anonymous_string.h"
#include "distributed_camera_constants.h"
#include "distributed_camera_errno.h"
#include "distributed_hardware_log.h"

namespace OHOS {
namespace DistributedHardware {
DCameraSourceServiceIpc::DCameraSourceServiceIpc() : isInit_(false)
{
    DHLOGI("DCameraSourceServiceIpc Create");
}

DCameraSourceServiceIpc::~DCameraSourceServiceIpc()
{
    DHLOGI("DCameraSourceServiceIpc Delete");
    UnInit();
}

IMPLEMENT_SINGLE_INSTANCE(DCameraSourceServiceIpc);

void DCameraSourceServiceIpc::Init()
{
    std::lock_guard<std::mutex> autoLock(initDmsLock_);
    DHLOGI("DCameraSourceServiceIpc Init Start");
    if (isInit_) {
        DHLOGI("DCameraSourceServiceIpc has already init");
        return;
    }
    auto runner = AppExecFwk::EventRunner::Create("DCameraSourceServiceIpcHandler");
    serviceHandler_ = std::make_shared<AppExecFwk::EventHandler>(runner);
    sinkRemoteRecipient_ = new SinkRemoteRecipient();
    isInit_ = true;
    DHLOGI("DCameraSourceServiceIpc Init End");
}

void DCameraSourceServiceIpc::UnInit()
{
    std::lock_guard<std::mutex> autoLock(initDmsLock_);
    DHLOGI("DCameraSourceServiceIpc UnInit Start");
    if (!isInit_) {
        DHLOGI("DCameraSourceServiceIpc has already UnInit");
        return;
    }
    ClearSinkRemoteDhms();
    DHLOGI("DCameraSourceServiceIpc UnInit Start free servicehandle");
    serviceHandler_ = nullptr;
    DHLOGI("DCameraSourceServiceIpc UnInit Start free recipient");
    sinkRemoteRecipient_ = nullptr;
    isInit_ = false;
    DHLOGI("DCameraSourceServiceIpc UnInit End");
}

sptr<IDistributedCameraSink> DCameraSourceServiceIpc::GetSinkRemoteDHMS(const std::string& deviceId)
{
    if (deviceId.empty()) {
        DHLOGE("GetSinkRemoteDHMS deviceId is empty");
        return nullptr;
    }
    {
        std::lock_guard<std::mutex> autoLock(sinkRemoteDmsLock_);
        auto iter = remoteSinks_.find(deviceId);
        if (iter != remoteSinks_.end()) {
            auto object = iter->second;
            if (object != nullptr) {
                DHLOGI("DCameraSourceServiceIpc GetSinkRemoteDHMS from cache devId: %s",
                    GetAnonyString(deviceId).c_str());
                return object;
            }
        }
    }
    DHLOGI("GetSinkRemoteDHMS remote deviceid is %s", GetAnonyString(deviceId).c_str());
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgr == nullptr) {
        DHLOGE("GetSinkRemoteDHMS failed to connect to systemAbilityMgr!");
        return nullptr;
    }

    auto object = samgr->CheckSystemAbility(DISTRIBUTED_HARDWARE_CAMERA_SINK_SA_ID, deviceId);
    if (object == nullptr) {
        DHLOGE("GetSinkRemoteDHMS failed get remote DHMS %s", GetAnonyString(deviceId).c_str());
        return nullptr;
    }
    int32_t ret = object->AddDeathRecipient(sinkRemoteRecipient_);
    sptr<IDistributedCameraSink> remoteDmsObj = iface_cast<IDistributedCameraSink>(object);
    if (remoteDmsObj == nullptr) {
        DHLOGI("GetSinkRemoteDHMS failed, remoteDmsObj is null ret: %d", ret);
        return nullptr;
    }
    {
        std::lock_guard<std::mutex> autoLock(sinkRemoteDmsLock_);
        auto iter = remoteSinks_.find(deviceId);
        if (iter != remoteSinks_.end()) {
            iter->second->AsObject()->RemoveDeathRecipient(sinkRemoteRecipient_);
        }
        remoteSinks_[deviceId] = remoteDmsObj;
    }
    DHLOGI("GetSinkRemoteDHMS success, AddDeathRecipient ret: %d", ret);
    return remoteDmsObj;
}

void DCameraSourceServiceIpc::DeleteSinkRemoteDhms(const std::string& deviceId)
{
    DHLOGI("DeleteSinkRemoteDhms devId: %s", GetAnonyString(deviceId).c_str());
    std::lock_guard<std::mutex> autoLock(sinkRemoteDmsLock_);
    auto item = remoteSinks_.find(deviceId);
    if (item == remoteSinks_.end()) {
        DHLOGI("DeleteSinkRemoteDhms not found device: %s", GetAnonyString(deviceId).c_str());
        return;
    }

    if (item->second != nullptr) {
        item->second->AsObject()->RemoveDeathRecipient(sinkRemoteRecipient_);
    }
    remoteSinks_.erase(item);
}

void DCameraSourceServiceIpc::ClearSinkRemoteDhms()
{
    DHLOGI("ClearSinkRemoteDhms Start");
    std::lock_guard<std::mutex> autoLock(sinkRemoteDmsLock_);
    for (auto iter = remoteSinks_.begin(); iter != remoteSinks_.end(); iter++) {
        if (iter->second != nullptr) {
            iter->second->AsObject()->RemoveDeathRecipient(sinkRemoteRecipient_);
        }
    }
    remoteSinks_.clear();
    DHLOGI("ClearSinkRemoteDhms end");
}

void DCameraSourceServiceIpc::SinkRemoteRecipient::OnRemoteDied(const wptr<IRemoteObject>& remote)
{
    DHLOGI("SinkRemoteRecipient OnRemoteDied received died notify!");
    DCameraSourceServiceIpc::GetInstance().OnSinkRemoteDmsDied(remote);
}

void DCameraSourceServiceIpc::OnSinkRemoteDmsDied(const wptr<IRemoteObject>& remote)
{
    sptr<IRemoteObject> diedRemoted = remote.promote();
    if (diedRemoted == nullptr) {
        DHLOGE("OnSinkRemoteDmsDied promote failed!");
        return;
    }
    DHLOGI("OnSinkRemoteDmsDied delete diedRemoted");
    auto remoteDmsDiedFunc = [this, diedRemoted]() {
        OnSinkRemoteDmsDied(diedRemoted);
    };
    if (serviceHandler_ != nullptr) {
        serviceHandler_->PostTask(remoteDmsDiedFunc);
    }
}

void DCameraSourceServiceIpc::OnSinkRemoteDmsDied(const sptr<IRemoteObject>& remote)
{
    std::lock_guard<std::mutex> autoLock(sinkRemoteDmsLock_);
    auto iter = std::find_if(remoteSinks_.begin(), remoteSinks_.end(), [&](
        const std::pair<std::string, sptr<IDistributedCameraSink>> &item)->bool {
            return item.second->AsObject() == remote;
        });
    if (iter == remoteSinks_.end()) {
        DHLOGI("OnSinkRemoteDmsDied not found remote object");
        return;
    }

    DHLOGI("OnSinkRemoteDmsDied remote.devId: %s", GetAnonyString(iter->first).c_str());
    if (iter->second != nullptr) {
        iter->second->AsObject()->RemoveDeathRecipient(sinkRemoteRecipient_);
    }
    remoteSinks_.erase(iter);
}
} // namespace DistributedHardware
} // namespace OHOS
