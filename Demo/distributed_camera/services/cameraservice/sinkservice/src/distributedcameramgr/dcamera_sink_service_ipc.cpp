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

#include "dcamera_sink_service_ipc.h"

#include "if_system_ability_manager.h"
#include "iservice_registry.h"

#include "anonymous_string.h"
#include "distributed_camera_constants.h"
#include "distributed_camera_errno.h"
#include "distributed_hardware_log.h"

namespace OHOS {
namespace DistributedHardware {
DCameraSinkServiceIpc::DCameraSinkServiceIpc() : isInit_(false)
{
    DHLOGI("DCameraSinkServiceIpc Create");
}

DCameraSinkServiceIpc::~DCameraSinkServiceIpc()
{
    DHLOGI("DCameraSinkServiceIpc Delete");
    UnInit();
}

IMPLEMENT_SINGLE_INSTANCE(DCameraSinkServiceIpc);

void DCameraSinkServiceIpc::Init()
{
    std::lock_guard<std::mutex> autoLock(initDmsLock_);
    DHLOGI("DCameraSinkServiceIpc Init Start");
    if (isInit_) {
        DHLOGI("DCameraSinkServiceIpc has already init");
        return;
    }
    auto runner = AppExecFwk::EventRunner::Create("DCameraSinkServiceIpcHandler");
    serviceHandler_ = std::make_shared<AppExecFwk::EventHandler>(runner);
    sourceRemoteRecipient_ = new SourceRemoteRecipient();
    isInit_ = true;
    DHLOGI("DCameraSinkServiceIpc Init End");
}

void DCameraSinkServiceIpc::UnInit()
{
    std::lock_guard<std::mutex> autoLock(initDmsLock_);
    DHLOGI("DCameraSinkServiceIpc UnInit Start");
    if (!isInit_) {
        DHLOGI("DCameraSinkServiceIpc has already UnInit");
        return;
    }
    ClearSourceRemoteDhms();
    DHLOGI("DCameraSinkServiceIpc Start free serviceHandler");
    serviceHandler_ = nullptr;
    DHLOGI("DCameraSinkServiceIpc Start free recipient");
    sourceRemoteRecipient_ = nullptr;
    isInit_ = false;
    DHLOGI("DCameraSinkServiceIpc UnInit End");
}

sptr<IDistributedCameraSource> DCameraSinkServiceIpc::GetSourceRemoteDHMS(const std::string& deviceId)
{
    if (deviceId.empty()) {
        DHLOGE("GetSourceRemoteDHMS deviceId is empty");
        return nullptr;
    }
    {
        std::lock_guard<std::mutex> autoLock(sourceRemoteDmsLock_);
        auto iter = remoteSources_.find(deviceId);
        if (iter != remoteSources_.end()) {
            auto object = iter->second;
            if (object != nullptr) {
                DHLOGI("DCameraSinkServiceIpc GetSourceRemoteDHMS from cache devId: %s",
                    GetAnonyString(deviceId).c_str());
                return object;
            }
        }
    }
    DHLOGI("GetSourceRemoteDHMS remote deviceid is %s", GetAnonyString(deviceId).c_str());
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgr == nullptr) {
        DHLOGE("GetSourceRemoteDHMS failed to connect to systemAbilityMgr!");
        return nullptr;
    }

    auto object = samgr->CheckSystemAbility(DISTRIBUTED_HARDWARE_CAMERA_SOURCE_SA_ID, deviceId);
    if (object == nullptr) {
        DHLOGE("GetSourceRemoteDHMS failed get remote DHMS %s", GetAnonyString(deviceId).c_str());
        return nullptr;
    }
    int32_t ret = object->AddDeathRecipient(sourceRemoteRecipient_);
    sptr<IDistributedCameraSource> remoteDmsObj = iface_cast<IDistributedCameraSource>(object);
    if (remoteDmsObj == nullptr) {
        DHLOGI("GetSourceRemoteDHMS failed, remoteDmsObj is null ret: %d", ret);
        return nullptr;
    }
    {
        std::lock_guard<std::mutex> autoLock(sourceRemoteDmsLock_);
        auto iter = remoteSources_.find(deviceId);
        if (iter != remoteSources_.end()) {
            iter->second->AsObject()->RemoveDeathRecipient(sourceRemoteRecipient_);
        }
        remoteSources_[deviceId] = remoteDmsObj;
    }
    DHLOGI("GetSourceRemoteDHMS success, AddDeathRecipient ret: %d", ret);
    return remoteDmsObj;
}

void DCameraSinkServiceIpc::DeleteSourceRemoteDhms(const std::string& deviceId)
{
    DHLOGI("DeleteSourceRemoteDhms devId: %s", GetAnonyString(deviceId).c_str());
    std::lock_guard<std::mutex> autoLock(sourceRemoteDmsLock_);
    auto item = remoteSources_.find(deviceId);
    if (item == remoteSources_.end()) {
        DHLOGI("DeleteSourceRemoteDhms not found device: %s", GetAnonyString(deviceId).c_str());
        return;
    }

    if (item->second != nullptr) {
        item->second->AsObject()->RemoveDeathRecipient(sourceRemoteRecipient_);
    }
    remoteSources_.erase(item);
}

void DCameraSinkServiceIpc::ClearSourceRemoteDhms()
{
    DHLOGI("ClearSourceRemoteDhms Start");
    std::lock_guard<std::mutex> autoLock(sourceRemoteDmsLock_);
    for (auto iter = remoteSources_.begin(); iter != remoteSources_.end(); iter++) {
        if (iter->second != nullptr) {
            iter->second->AsObject()->RemoveDeathRecipient(sourceRemoteRecipient_);
        }
    }
    remoteSources_.clear();
    DHLOGI("ClearSourceRemoteDhms end");
}

void DCameraSinkServiceIpc::SourceRemoteRecipient::OnRemoteDied(const wptr<IRemoteObject>& remote)
{
    DHLOGI("SourceRemoteRecipient OnRemoteDied received died notify!");
    DCameraSinkServiceIpc::GetInstance().OnSourceRemoteDmsDied(remote);
}

void DCameraSinkServiceIpc::OnSourceRemoteDmsDied(const wptr<IRemoteObject>& remote)
{
    sptr<IRemoteObject> diedRemoted = remote.promote();
    if (diedRemoted == nullptr) {
        DHLOGE("OnSourceRemoteDmsDied promote failed!");
        return;
    }
    DHLOGI("OnSourceRemoteDmsDied delete diedRemoted");
    auto remoteDmsDiedFunc = [this, diedRemoted]() {
        OnSourceRemoteDmsDied(diedRemoted);
    };
    if (serviceHandler_ != nullptr) {
        serviceHandler_->PostTask(remoteDmsDiedFunc);
    }
}

void DCameraSinkServiceIpc::OnSourceRemoteDmsDied(const sptr<IRemoteObject>& remote)
{
    std::lock_guard<std::mutex> autoLock(sourceRemoteDmsLock_);
    auto iter = std::find_if(remoteSources_.begin(), remoteSources_.end(), [&](
        const std::pair<std::string, sptr<IDistributedCameraSource>> &item)->bool {
            return item.second->AsObject() == remote;
        });
    if (iter == remoteSources_.end()) {
        DHLOGI("OnSourceRemoteDmsDied not found remote object");
        return;
    }

    DHLOGI("OnSourceRemoteDmsDied remote.devId: %s", GetAnonyString(iter->first).c_str());
    if (iter->second != nullptr) {
        iter->second->AsObject()->RemoveDeathRecipient(sourceRemoteRecipient_);
    }
    remoteSources_.erase(iter);
}
} // namespace DistributedHardware
} // namespace OHOS
