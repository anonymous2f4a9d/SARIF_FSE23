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

#ifndef OHOS_DCAMERA_PROVIDER_CALLBACK_IMPL_H
#define OHOS_DCAMERA_PROVIDER_CALLBACK_IMPL_H

#include "dcamera_provider_callback.h"

namespace OHOS {
namespace DistributedHardware {
class DCameraSourceDev;
class DCameraProviderCallbackImpl : public DCameraProviderCallback {
public:
    DCameraProviderCallbackImpl(std::string devId, std::string dhId, std::shared_ptr<DCameraSourceDev>& sourceDev);
    ~DCameraProviderCallbackImpl();

    DCamRetCode OpenSession(const std::shared_ptr<DHBase>& dhBase) override;
    DCamRetCode CloseSession(const std::shared_ptr<DHBase>& dhBase) override;
    DCamRetCode ConfigureStreams(const std::shared_ptr<DHBase>& dhBase,
        const std::vector<std::shared_ptr<DCStreamInfo>>& streamInfos) override;
    DCamRetCode ReleaseStreams(const std::shared_ptr<DHBase>& dhBase, const std::vector<int>& streamIds) override;
    DCamRetCode StartCapture(const std::shared_ptr<DHBase>& dhBase,
        const std::vector<std::shared_ptr<DCCaptureInfo>>& captureInfos) override;
    DCamRetCode StopCapture(const std::shared_ptr<DHBase>& dhBase) override;
    DCamRetCode UpdateSettings(const std::shared_ptr<DHBase>& dhBase,
        const std::vector<std::shared_ptr<DCameraSettings>>& settings) override;

private:
    std::string devId_;
    std::string dhId_;
    std::weak_ptr<DCameraSourceDev> sourceDev_;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_DCAMERA_PROVIDER_CALLBACK_IMPL_H
