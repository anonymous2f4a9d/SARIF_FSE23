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

#include "dcamera_provider_callback.h"
#include "distributed_hardware_log.h"

namespace OHOS {
namespace DistributedHardware {
DCamRetCode DCameraProviderCallback::OpenSession(const std::shared_ptr<DHBase> &dhBase)
{
    DHLOGW("DCameraProviderCallback::OpenSession enter.");
    return DCamRetCode::SUCCESS;
}

DCamRetCode DCameraProviderCallback::CloseSession(const std::shared_ptr<DHBase> &dhBase)
{
    DHLOGW("DCameraProviderCallback::CloseSession enter.");
    return DCamRetCode::SUCCESS;
}

DCamRetCode DCameraProviderCallback::ConfigureStreams(const std::shared_ptr<DHBase> &dhBase,
    const std::vector<std::shared_ptr<DCStreamInfo>> &streamInfos)
{
    DHLOGW("DCameraProviderCallback::ConfigureStreams enter.");
    return DCamRetCode::SUCCESS;
}

DCamRetCode DCameraProviderCallback::ReleaseStreams(const std::shared_ptr<DHBase> &dhBase,
    const std::vector<int> &streamIds)
{
    DHLOGW("DCameraProviderCallback::ReleaseStreams enter.");
    return DCamRetCode::SUCCESS;
}

DCamRetCode DCameraProviderCallback::StartCapture(const std::shared_ptr<DHBase> &dhBase,
    const std::vector<std::shared_ptr<DCCaptureInfo>> &captureInfos)
{
    DHLOGW("DCameraProviderCallback::StartCapture enter.");
    return DCamRetCode::SUCCESS;
}

DCamRetCode DCameraProviderCallback::StopCapture(const std::shared_ptr<DHBase> &dhBase)
{
    DHLOGW("DCameraProviderCallback::StopCapture enter.");
    return DCamRetCode::SUCCESS;
}

DCamRetCode DCameraProviderCallback::UpdateSettings(const std::shared_ptr<DHBase> &dhBase,
    const std::vector<std::shared_ptr<DCameraSettings>> &settings)
{
    DHLOGW("DCameraProviderCallback::UpdateSettings enter.");
    return DCamRetCode::SUCCESS;
}
} // namespace DistributedHardware
} // namespace OHOS
