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

#ifndef DISTRIBUTED_CAMERA_HOST_H
#define DISTRIBUTED_CAMERA_HOST_H

#include "dcamera.h"
#include "dcamera_device.h"
#include "icamera_device.h"
#include "icamera_host_callback.h"
#include "icamera_device_callback.h"

#include <foundation/distributedhardware/distributedcamera/camera_hdf/interfaces/include/types.h>
#ifdef BALTIMORE_CAMERA
#include <drivers/peripheral/adapter/camera/interfaces/include/types.h>
#else
#include <drivers/peripheral/camera/interfaces/include/types.h>
#endif

namespace OHOS {
namespace DistributedHardware {
class DCameraHost {
public:
    DCameraHost() = default;
    virtual ~DCameraHost() = default;
    DCameraHost(const DCameraHost &other) = delete;
    DCameraHost(DCameraHost &&other) = delete;
    DCameraHost& operator=(const DCameraHost &other) = delete;
    DCameraHost& operator=(DCameraHost &&other) = delete;

public:
    static std::shared_ptr<DCameraHost> GetInstance();
    CamRetCode SetCallback(const OHOS::sptr<ICameraHostCallback> &callback);
    CamRetCode GetCameraIds(std::vector<std::string> &cameraIds);
    CamRetCode GetCameraAbility(const std::string &cameraId, std::shared_ptr<CameraAbility> &ability);
    CamRetCode OpenCamera(const std::string &cameraId, const OHOS::sptr<ICameraDeviceCallback> &callback,
                          OHOS::sptr<ICameraDevice> &pDevice);
    CamRetCode SetFlashlight(const std::string &cameraId,  bool &isEnable);

    DCamRetCode AddDCameraDevice(const std::shared_ptr<DHBase> &dhBase, const std::string &abilityInfo,
                                 const sptr<IDCameraProviderCallback> &callback);
    DCamRetCode RemoveDCameraDevice(const std::shared_ptr<DHBase> &dhBase);
    OHOS::sptr<DCameraDevice> GetDCameraDeviceByDHBase(const std::shared_ptr<DHBase> &dhBase);
    void NotifyDCameraStatus(const std::shared_ptr<DHBase> &dhBase, int32_t result);

private:
    bool IsCameraIdInvalid(const std::string &cameraId);
    std::string GetCameraIdByDHBase(const std::shared_ptr<DHBase> &dhBase);

private:
    class AutoRelease {
    public:
        AutoRelease() {};
        ~AutoRelease()
        {
            if (DCameraHost::instance_ != nullptr) {
                DCameraHost::instance_ = nullptr;
            }
        };
    };
    static AutoRelease autoRelease_;
    static std::shared_ptr<DCameraHost> instance_;

    OHOS::sptr<ICameraHostCallback> dCameraHostCallback_;
    std::map<DHBase, std::string> dhBaseHashDCamIdMap_;
    std::map<std::string, OHOS::sptr<DCameraDevice>> dCameraDeviceMap_;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // DISTRIBUTED_CAMERA_HOST_H