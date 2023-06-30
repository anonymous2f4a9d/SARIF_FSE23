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

#include "dcamera_provider_callback_impl.h"

#include "dcamera_index.h"
#include "dcamera_source_dev.h"

#include "anonymous_string.h"
#include "distributed_camera_errno.h"
#include "distributed_hardware_log.h"

#include "types.h"

namespace OHOS {
namespace DistributedHardware {
DCameraProviderCallbackImpl::DCameraProviderCallbackImpl(std::string devId, std::string dhId,
    std::shared_ptr<DCameraSourceDev>& sourceDev) : devId_(devId), dhId_(dhId), sourceDev_(sourceDev)
{
    DHLOGI("DCameraProviderCallbackImpl create devId: %s dhId: %s", GetAnonyString(devId_).c_str(),
        GetAnonyString(dhId_).c_str());
}

DCameraProviderCallbackImpl::~DCameraProviderCallbackImpl()
{
    DHLOGI("DCameraProviderCallbackImpl delete devId: %s dhId: %s", GetAnonyString(devId_).c_str(),
        GetAnonyString(dhId_).c_str());
}

DCamRetCode DCameraProviderCallbackImpl::OpenSession(const std::shared_ptr<DHBase>& dhBase)
{
    DHLOGI("DCameraProviderCallbackImpl OpenSession devId: %s dhId: %s", GetAnonyString(devId_).c_str(),
        GetAnonyString(dhId_).c_str());
    std::shared_ptr<DCameraSourceDev> sourceDev = sourceDev_.lock();
    if (sourceDev == nullptr) {
        DHLOGE("DCameraProviderCallbackImpl OpenSession failed, can not get device, devId: %s, dhId: %s",
            GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str());
        return FAILED;
    }
    DCameraIndex camIndex(devId_, dhId_);
    int32_t ret = sourceDev->OpenSession(camIndex);
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraProviderCallbackImpl OpenSession failed, ret: %d, devId: %s, dhId: %s", ret,
            GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str());
        return FAILED;
    }
    return SUCCESS;
}

DCamRetCode DCameraProviderCallbackImpl::CloseSession(const std::shared_ptr<DHBase>& dhBase)
{
    DHLOGI("DCameraProviderCallbackImpl CloseSession devId: %s dhId: %s", GetAnonyString(devId_).c_str(),
        GetAnonyString(dhId_).c_str());
    std::shared_ptr<DCameraSourceDev> sourceDev = sourceDev_.lock();
    if (sourceDev == nullptr) {
        DHLOGE("DCameraProviderCallbackImpl CloseSession failed, can not get device, devId: %s, dhId: %s",
            GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str());
        return FAILED;
    }
    DCameraIndex camIndex(devId_, dhId_);
    int32_t ret = sourceDev->CloseSession(camIndex);
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraProviderCallbackImpl CloseSession failed, ret: %d, devId: %s, dhId: %s", ret,
            GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str());
        return FAILED;
    }
    return SUCCESS;
}

DCamRetCode DCameraProviderCallbackImpl::ConfigureStreams(const std::shared_ptr<DHBase>& dhBase,
    const std::vector<std::shared_ptr<DCStreamInfo>>& streamInfos)
{
    DHLOGI("DCameraProviderCallbackImpl ConfigStreams devId: %s dhId: %s", GetAnonyString(devId_).c_str(),
        GetAnonyString(dhId_).c_str());
    std::shared_ptr<DCameraSourceDev> sourceDev = sourceDev_.lock();
    if (sourceDev == nullptr) {
        DHLOGE("DCameraProviderCallbackImpl ConfigStreams failed, can not get device, devId: %s, dhId: %s",
            GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str());
        return FAILED;
    }
    int32_t ret = sourceDev->ConfigStreams(streamInfos);
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraProviderCallbackImpl CloseSession failed, ret: %d, devId: %s, dhId: %s", ret,
            GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str());
        return FAILED;
    }
    return SUCCESS;
}

DCamRetCode DCameraProviderCallbackImpl::ReleaseStreams(const std::shared_ptr<DHBase>& dhBase,
    const std::vector<int>& streamIds)
{
    DHLOGI("DCameraProviderCallbackImpl ReleaseStreams devId: %s dhId: %s", GetAnonyString(devId_).c_str(),
        GetAnonyString(dhId_).c_str());
    std::shared_ptr<DCameraSourceDev> sourceDev = sourceDev_.lock();
    if (sourceDev == nullptr) {
        DHLOGE("DCameraProviderCallbackImpl ReleaseStreams failed, can not get device, devId: %s, dhId: %s",
            GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str());
        return FAILED;
    }
    int32_t ret = sourceDev->ReleaseStreams(streamIds);
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraProviderCallbackImpl ReleaseStreams failed, ret: %d, devId: %s, dhId: %s", ret,
            GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str());
        return FAILED;
    }
    return SUCCESS;
}

DCamRetCode DCameraProviderCallbackImpl::StartCapture(const std::shared_ptr<DHBase>& dhBase,
    const std::vector<std::shared_ptr<DCCaptureInfo>>& captureInfos)
{
    DHLOGI("DCameraProviderCallbackImpl StartCapture devId: %s dhId: %s", GetAnonyString(devId_).c_str(),
        GetAnonyString(dhId_).c_str());
    std::shared_ptr<DCameraSourceDev> sourceDev = sourceDev_.lock();
    if (sourceDev == nullptr) {
        DHLOGE("DCameraProviderCallbackImpl StartCapture failed, can not get device, devId: %s, dhId: %s",
            GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str());
        return FAILED;
    }
    int32_t ret = sourceDev->StartCapture(captureInfos);
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraProviderCallbackImpl StartCapture failed, ret: %d, devId: %s, dhId: %s", ret,
            GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str());
        return FAILED;
    }
    return SUCCESS;
}

DCamRetCode DCameraProviderCallbackImpl::StopCapture(const std::shared_ptr<DHBase>& dhBase)
{
    DHLOGI("DCameraProviderCallbackImpl StopCapture devId: %s dhId: %s", GetAnonyString(devId_).c_str(),
        GetAnonyString(dhId_).c_str());
    std::shared_ptr<DCameraSourceDev> sourceDev = sourceDev_.lock();
    if (sourceDev == nullptr) {
        DHLOGE("DCameraProviderCallbackImpl StopCapture failed, can not get device, devId: %s, dhId: %s",
            GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str());
        return FAILED;
    }
    int32_t ret = sourceDev->StopCapture();
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraProviderCallbackImpl StopCapture failed, ret: %d, devId: %s, dhId: %s", ret,
            GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str());
        return FAILED;
    }
    return SUCCESS;
}

DCamRetCode DCameraProviderCallbackImpl::UpdateSettings(const std::shared_ptr<DHBase>& dhBase,
    const std::vector<std::shared_ptr<DCameraSettings>>& settings)
{
    DHLOGI("DCameraProviderCallbackImpl UpdateSettings devId: %s dhId: %s", GetAnonyString(devId_).c_str(),
        GetAnonyString(dhId_).c_str());
    std::shared_ptr<DCameraSourceDev> sourceDev = sourceDev_.lock();
    if (sourceDev == nullptr) {
        DHLOGE("DCameraProviderCallbackImpl UpdateSettings failed, can not get device, devId: %s, dhId: %s",
            GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str());
        return FAILED;
    }
    int32_t ret = sourceDev->UpdateCameraSettings(settings);
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraProviderCallbackImpl UpdateSettings failed, ret: %d, devId: %s, dhId: %s", ret,
            GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str());
        return FAILED;
    }
    return SUCCESS;
}
} // namespace DistributedHardware
} // namespace OHOS
