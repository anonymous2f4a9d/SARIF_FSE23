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

#include "dcamera_sink_handler_ipc.h"

#include "if_system_ability_manager.h"
#include "iservice_registry.h"

#include "anonymous_string.h"
#include "distributed_camera_constants.h"
#include "distributed_camera_errno.h"
#include "distributed_hardware_log.h"

namespace OHOS {
namespace DistributedHardware {
DCameraSinkHandlerIpc::DCameraSinkHandlerIpc() : isInit_(false)
{
    DHLOGI("DCameraSinkHandlerIpc Create");
}

DCameraSinkHandlerIpc::~DCameraSinkHandlerIpc()
{
    DHLOGI("DCameraSinkHandlerIpc Delete");
    UnInit();
}

IMPLEMENT_SINGLE_INSTANCE(DCameraSinkHandlerIpc);

void DCameraSinkHandlerIpc::Init()
{
    std::lock_guard<std::mutex> autoLock(initDmsLock_);
    DHLOGI("DCameraSinkHandlerIpc Init Start");
    if (isInit_) {
        DHLOGI("DCameraSinkHandlerIpc has already init");
        return;
    }
    auto runner = AppExecFwk::EventRunner::Create("DCameraSinkHandlerIpcHandler");
    serviceHandler_ = std::make_shared<AppExecFwk::EventHandler>(runner);
    sinkLocalRecipient_ = new SinkLocalRecipient();
    isInit_ = true;
    DHLOGI("DCameraSinkHandlerIpc Init End");
}

void DCameraSinkHandlerIpc::UnInit()
{
    std::lock_guard<std::mutex> autoLock(initDmsLock_);
    DHLOGI("DCameraSinkHandlerIpc UnInit Start");
    if (!isInit_) {
        DHLOGI("DCameraSinkHandlerIpc has already UnInit");
        return;
    }
    DeleteSinkLocalDhms();
    DHLOGI("DCameraSinkHandlerIpc Start free serviceHandler");
    serviceHandler_ = nullptr;
    DHLOGI("DCameraSinkHandlerIpc Start free recipient");
    sinkLocalRecipient_ = nullptr;
    isInit_ = false;
    DHLOGI("DCameraSinkHandlerIpc UnInit End");
}

sptr<IDistributedCameraSink> DCameraSinkHandlerIpc::GetSinkLocalDHMS()
{
    {
        std::lock_guard<std::mutex> autoLock(sinkLocalDmsLock_);
        if (localSink_ != nullptr) {
            DHLOGI("DCameraSinkHandlerIpc GetSinkLocalDHMS from cache");
            return localSink_;
        }
    }
    DHLOGI("GetSinkLocalDHMS Start");
    sptr<ISystemAbilityManager> sm = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sm == nullptr) {
        DHLOGE("GetSinkLocalDHMS GetSystemAbilityManager failed");
        return nullptr;
    }

    sptr<IRemoteObject> object = sm->GetSystemAbility(DISTRIBUTED_HARDWARE_CAMERA_SINK_SA_ID);
    if (object == nullptr) {
        DHLOGE("GetSinkLocalDHMS GetSystemAbility failed");
        return nullptr;
    }
    int32_t ret = object->AddDeathRecipient(sinkLocalRecipient_);
    sptr<IDistributedCameraSink> localSink = iface_cast<IDistributedCameraSink>(object);
    if (localSink == nullptr) {
        DHLOGI("GetSinkLocalDHMS failed, localSink is null ret: %d", ret);
        return nullptr;
    }
    {
        std::lock_guard<std::mutex> autoLock(sinkLocalDmsLock_);
        if (localSink_ != nullptr) {
            localSink_->AsObject()->RemoveDeathRecipient(sinkLocalRecipient_);
        }
        localSink_ = localSink;
    }
    DHLOGI("GetSinkLocalDHMS success, AddDeathRecipient ret: %d", ret);
    return localSink;
}

void DCameraSinkHandlerIpc::DeleteSinkLocalDhms()
{
    DHLOGI("DeleteSinkLocalDhms start");
    std::lock_guard<std::mutex> autoLock(sinkLocalDmsLock_);
    if (localSink_ != nullptr) {
        localSink_->AsObject()->RemoveDeathRecipient(sinkLocalRecipient_);
    }
    localSink_ = nullptr;
    DHLOGI("DeleteSinkLocalDhms end");
}

void DCameraSinkHandlerIpc::SinkLocalRecipient::OnRemoteDied(const wptr<IRemoteObject>& remote)
{
    DHLOGI("SinkLocalRecipient OnRemoteDied received died notify!");
    DCameraSinkHandlerIpc::GetInstance().OnSinkLocalDmsDied(remote);
}

void DCameraSinkHandlerIpc::OnSinkLocalDmsDied(const wptr<IRemoteObject>& remote)
{
    sptr<IRemoteObject> diedRemoted = remote.promote();
    if (diedRemoted == nullptr) {
        DHLOGE("OnSinkLocalDmsDied promote failed!");
        return;
    }
    DHLOGI("OnSinkLocalDmsDied delete diedRemoted");
    auto remoteDmsDiedFunc = [this, diedRemoted]() {
        OnSinkLocalDmsDied(diedRemoted);
    };
    if (serviceHandler_ != nullptr) {
        serviceHandler_->PostTask(remoteDmsDiedFunc);
    }
}

void DCameraSinkHandlerIpc::OnSinkLocalDmsDied(const sptr<IRemoteObject>& remote)
{
    std::lock_guard<std::mutex> autoLock(sinkLocalDmsLock_);
    if (localSink_->AsObject() != remote) {
        DHLOGI("OnSinkLocalDmsDied not found remote object");
        return;
    }

    DHLOGI("OnSinkLocalDmsDied Clear");
    if (localSink_ != nullptr) {
        localSink_->AsObject()->RemoveDeathRecipient(sinkLocalRecipient_);
    }
    localSink_ = nullptr;
}
} // namespace DistributedHardware
} // namespace OHOS
