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

#ifndef DISTRIBUTED_CAMERA_PROVIDER_H
#define DISTRIBUTED_CAMERA_PROVIDER_H

#include "dcamera.h"
#include "idistributed_camera_provider_callback.h"

namespace OHOS {
namespace DistributedHardware {
class DCameraHost;
class DCameraDevice;
class DCameraProvider {
public:
    DCameraProvider() = default;
    virtual ~DCameraProvider() = default;
    DCameraProvider(const DCameraProvider &other) = delete;
    DCameraProvider(DCameraProvider &&other) = delete;
    DCameraProvider& operator=(const DCameraProvider &other) = delete;
    DCameraProvider& operator=(DCameraProvider &&other) = delete;

public:
    static std::shared_ptr<DCameraProvider> GetInstance();
    DCamRetCode EnableDCameraDevice(const std::shared_ptr<DHBase> &dhBase, const std::string &abilitySet,
                                    const sptr<IDCameraProviderCallback> &callback);
    DCamRetCode DisableDCameraDevice(const std::shared_ptr<DHBase> &dhBase);
    DCamRetCode AcquireBuffer(const std::shared_ptr<DHBase> &dhBase, int streamId,
                              std::shared_ptr<DCameraBuffer> &buffer);
    DCamRetCode ShutterBuffer(const std::shared_ptr<DHBase> &dhBase, int streamId,
                              const std::shared_ptr<DCameraBuffer> &buffer);
    DCamRetCode OnSettingsResult(const std::shared_ptr<DHBase> &dhBase, const std::shared_ptr<DCameraSettings> &result);
    DCamRetCode Notify(const std::shared_ptr<DHBase> &dhBase, const std::shared_ptr<DCameraHDFEvent> &event);

    DCamRetCode OpenSession(const std::shared_ptr<DHBase> &dhBase);
    DCamRetCode CloseSession(const std::shared_ptr<DHBase> &dhBase);
    DCamRetCode ConfigureStreams(const std::shared_ptr<DHBase> &dhBase,
                                 const std::vector<std::shared_ptr<DCStreamInfo>> &streamInfos);
    DCamRetCode ReleaseStreams(const std::shared_ptr<DHBase> &dhBase, const std::vector<int> &streamIds);
    DCamRetCode StartCapture(const std::shared_ptr<DHBase> &dhBase,
                             const std::vector<std::shared_ptr<DCCaptureInfo>> &captureInfos);
    DCamRetCode StopCapture(const std::shared_ptr<DHBase> &dhBase);
    DCamRetCode UpdateSettings(const std::shared_ptr<DHBase> &dhBase,
                               const std::vector<std::shared_ptr<DCameraSettings>> &settings);

private:
    bool IsDhBaseInfoInvalid(const std::shared_ptr<DHBase> &dhBase);
    sptr<IDCameraProviderCallback> GetCallbackBydhBase(const std::shared_ptr<DHBase> &dhBase);
    OHOS::sptr<DCameraDevice> GetDCameraDevice(const std::shared_ptr<DHBase> &dhBase);

private:
    class AutoRelease {
    public:
        AutoRelease() {};
        ~AutoRelease()
        {
            if (DCameraProvider::instance_ != nullptr) {
                DCameraProvider::instance_ = nullptr;
            }
        };
    };
    static AutoRelease autoRelease_;
    static std::shared_ptr<DCameraProvider> instance_;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // DISTRIBUTED_CAMERA_PROVIDER_H