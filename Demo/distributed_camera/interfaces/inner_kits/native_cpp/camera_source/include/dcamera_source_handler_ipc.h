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

#ifndef OHOS_DCAMERA_SOURCE_HANDLER_IPC_H
#define OHOS_DCAMERA_SOURCE_HANDLER_IPC_H

#include "event_handler.h"
#include "idistributed_camera_source.h"
#include "single_instance.h"

namespace OHOS {
namespace DistributedHardware {
class DCameraSourceHandlerIpc {
DECLARE_SINGLE_INSTANCE_BASE(DCameraSourceHandlerIpc);

public:
    void Init();
    void UnInit();
    sptr<IDistributedCameraSource> GetSourceLocalDHMS();
    void OnSourceLocalDmsDied(const wptr<IRemoteObject>& remote);

private:
    DCameraSourceHandlerIpc();
    ~DCameraSourceHandlerIpc();
    void OnSourceLocalDmsDied(const sptr<IRemoteObject>& remote);
    void DeleteSourceLocalDhms();

    class SourceLocalRecipient : public IRemoteObject::DeathRecipient {
    public:
        void OnRemoteDied(const wptr<IRemoteObject>& remote) override;
    };
    sptr<SourceLocalRecipient> sourceLocalRecipient_;
    sptr<IDistributedCameraSource> localSource_;
    std::mutex sourceLocalDmsLock_;

    bool isInit_;
    std::shared_ptr<AppExecFwk::EventHandler> serviceHandler_;
    std::mutex initDmsLock_;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_DCAMERA_SOURCE_HANDLER_IPC_H
