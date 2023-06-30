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

#ifndef DISTRIBUTED_CAMERA_DEVICE_H
#define DISTRIBUTED_CAMERA_DEVICE_H

#include <vector>
#include <string>
#include "dcamera_device_stub.h"
#include "dmetadata_processor.h"
#include "dstream_operator.h"
#include "icamera_device_callback.h"
#include "idistributed_camera_provider_callback.h"
#include "types.h"

namespace OHOS {
namespace DistributedHardware {
class DCameraDevice : public DCameraDeviceStub {
public:
    DCameraDevice(const std::shared_ptr<DHBase> &dhBase, const std::string &abilityInfo);
    DCameraDevice() = default;
    virtual ~DCameraDevice() = default;
    DCameraDevice(const DCameraDevice &other) = delete;
    DCameraDevice(DCameraDevice &&other) = delete;
    DCameraDevice& operator=(const DCameraDevice &other) = delete;
    DCameraDevice& operator=(DCameraDevice &&other) = delete;

public:
    CamRetCode GetStreamOperator(const OHOS::sptr<IStreamOperatorCallback> &callback,
                                 OHOS::sptr<IStreamOperator> &streamOperator) override;
    CamRetCode UpdateSettings(const std::shared_ptr<CameraSetting> &settings) override;
    CamRetCode SetResultMode(const ResultCallbackMode &mode) override;
    CamRetCode GetEnabledResults(std::vector<MetaType> &results) override;
    CamRetCode EnableResult(const std::vector<MetaType> &results) override;
    CamRetCode DisableResult(const std::vector<MetaType> &results) override;
    void Close() override;

    CamRetCode OpenDCamera(const OHOS::sptr<ICameraDeviceCallback> &callback);
    CamRetCode GetDCameraAbility(std::shared_ptr<CameraAbility> &ability);
    DCamRetCode AcquireBuffer(int streamId, std::shared_ptr<DCameraBuffer> &buffer);
    DCamRetCode ShutterBuffer(int streamId, const std::shared_ptr<DCameraBuffer> &buffer);
    DCamRetCode OnSettingsResult(const std::shared_ptr<DCameraSettings> &result);
    DCamRetCode Notify(const std::shared_ptr<DCameraHDFEvent> &event);
    void SetProviderCallback(const OHOS::sptr<IDCameraProviderCallback> &callback);
    OHOS::sptr<IDCameraProviderCallback> GetProviderCallback();
    std::string GetDCameraId();
    bool IsOpened();

private:
    void Init(const std::string &abilityInfo);
    DCamRetCode CreateDStreamOperator();
    std::string GenerateCameraId(const std::shared_ptr<DHBase> &dhBase);

private:
    bool isOpened_;
    std::string dCameraId_;
    std::shared_ptr<DHBase> dhBase_;
    std::string dCameraAbilityInfo_;
    OHOS::sptr<ICameraDeviceCallback> dCameraDeviceCallback_;
    OHOS::sptr<IDCameraProviderCallback> dCameraProviderCallback_;
    OHOS::sptr<DStreamOperator> dCameraStreamOperator_;
    std::shared_ptr<DMetadataProcessor> dMetadataProcessor_;

    std::mutex openSesslock_;
    std::condition_variable openSessCV_;
    bool isOpenSessFailed_ = false;
    std::mutex isOpenSessFailedlock_;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // DISTRIBUTED_CAMERA_DEVICE_H