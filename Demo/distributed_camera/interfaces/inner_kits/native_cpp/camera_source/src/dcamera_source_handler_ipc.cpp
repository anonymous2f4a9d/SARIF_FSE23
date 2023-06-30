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

#include "dcamera_source_handler_ipc.h"

#include "if_system_ability_manager.h"
#include "iservice_registry.h"

#include "anonymous_string.h"
#include "distributed_camera_constants.h"
#include "distributed_camera_errno.h"
#include "distributed_hardware_log.h"

namespace OHOS {
namespace DistributedHardware {
DCameraSourceHandlerIpc::DCameraSourceHandlerIpc() : isInit_(false)
{
    DHLOGI("DCameraSourceHandlerIpc Create");
}

DCameraSourceHandlerIpc::~DCameraSourceHandlerIpc()
{
    DHLOGI("DCameraSourceHandlerIpc Delete");
    UnInit();
}

IMPLEMENT_SINGLE_INSTANCE(DCameraSourceHandlerIpc);

void DCameraSourceHandlerIpc::Init()
{
    std::lock_guard<std::mutex> autoLock(initDmsLock_);
    DHLOGI("DCameraSourceHandlerIpc Init Start");
    if (isInit_) {
        DHLOGI("DCameraSourceHandlerIpc has already init");
        return;
    }
    auto runner = AppExecFwk::EventRunner::Create("DCameraSourceHandlerIpcHandler");
    serviceHandler_ = std::make_shared<AppExecFwk::EventHandler>(runner);
    sourceLocalRecipient_ = new SourceLocalRecipient();
    isInit_ = true;
    DHLOGI("DCameraSourceHandlerIpc Init End");
}

void DCameraSourceHandlerIpc::UnInit()
{
    std::lock_guard<std::mutex> autoLock(initDmsLock_);
    DHLOGI("DCameraSourceHandlerIpc UnInit Start");
    if (!isInit_) {
        DHLOGI("DCameraSourceHandlerIpc has already UnInit");
        return;
    }
    DeleteSourceLocalDhms();
    DHLOGI("DCameraSourceHandlerIpc Start free serviceHandler");
    serviceHandler_ = nullptr;
    DHLOGI("DCameraSourceHandlerIpc Start free recipient");
    sourceLocalRecipient_ = nullptr;
    isInit_ = false;
    DHLOGI("DCameraSourceHandlerIpc UnInit End");
}

sptr<IDistributedCameraSource> DCameraSourceHandlerIpc::GetSourceLocalDHMS()
{
    {
        std::lock_guard<std::mutex> autoLock(sourceLocalDmsLock_);
        if (localSource_ != nullptr) {
            DHLOGI("DCameraSourceHandlerIpc GetSourceLocalDHMS from cache");
            return localSource_;
        }
    }
    DHLOGI("GetSourceLocalDHMS Start");
    sptr<ISystemAbilityManager> sm = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sm == nullptr) {
        DHLOGE("GetSourceLocalDHMS GetSystemAbilityManager failed");
        return nullptr;
    }

    sptr<IRemoteObject> object = sm->GetSystemAbility(DISTRIBUTED_HARDWARE_CAMERA_SOURCE_SA_ID);
    if (object == nullptr) {
        DHLOGE("GetSourceLocalDHMS GetSystemAbility failed");
        return nullptr;
    }
    int32_t ret = object->AddDeathRecipient(sourceLocalRecipient_);
    sptr<IDistributedCameraSource> localSource = iface_cast<IDistributedCameraSource>(object);
    if (localSource == nullptr) {
        DHLOGI("GetSourceLocalDHMS failed, localSource is null ret: %d", ret);
        return nullptr;
    }
    {
        std::lock_guard<std::mutex> autoLock(sourceLocalDmsLock_);
        if (localSource_ != nullptr) {
            localSource_->AsObject()->RemoveDeathRecipient(sourceLocalRecipient_);
        }
        localSource_ = localSource;
    }
    DHLOGI("GetSourceLocalDHMS success, AddDeathRecipient ret: %d", ret);
    return localSource;
}

void DCameraSourceHandlerIpc::DeleteSourceLocalDhms()
{
    DHLOGI("DeleteSourceLocalDhms start");
    std::lock_guard<std::mutex> autoLock(sourceLocalDmsLock_);
    if (localSource_ != nullptr) {
        localSource_->AsObject()->RemoveDeathRecipient(sourceLocalRecipient_);
    }
    localSource_ = nullptr;
    DHLOGI("DeleteSourceLocalDhms end");
}

void DCameraSourceHandlerIpc::SourceLocalRecipient::OnRemoteDied(const wptr<IRemoteObject>& remote)
{
    DHLOGI("SourceLocalRecipient OnRemoteDied received died notify!");
    DCameraSourceHandlerIpc::GetInstance().OnSourceLocalDmsDied(remote);
}

void DCameraSourceHandlerIpc::OnSourceLocalDmsDied(const wptr<IRemoteObject>& remote)
{
    sptr<IRemoteObject> diedRemoted = remote.promote();
    if (diedRemoted == nullptr) {
        DHLOGE("OnSourceLocalDmsDied promote failed!");
        return;
    }
    DHLOGI("OnSourceLocalDmsDied delete diedRemoted");
    auto remoteDmsDiedFunc = [this, diedRemoted]() {
        OnSourceLocalDmsDied(diedRemoted);
    };
    if (serviceHandler_ != nullptr) {
        serviceHandler_->PostTask(remoteDmsDiedFunc);
    }
}

void DCameraSourceHandlerIpc::OnSourceLocalDmsDied(const sptr<IRemoteObject>& remote)
{
    std::lock_guard<std::mutex> autoLock(sourceLocalDmsLock_);
    if (localSource_->AsObject() != remote) {
        DHLOGI("OnSourceLocalDmsDied not found remote object");
        return;
    }

    DHLOGI("OnSourceLocalDmsDied Clear");
    if (localSource_ != nullptr) {
        localSource_->AsObject()->RemoveDeathRecipient(sourceLocalRecipient_);
    }
    localSource_ = nullptr;
}
} // namespace DistributedHardware
} // namespace OHOS
