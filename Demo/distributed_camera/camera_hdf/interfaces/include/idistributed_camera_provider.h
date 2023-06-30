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

/**
 * @file idistributed_camera_provider.h
 *
 * @brief Transfer interfaces call between distributed camera SA service and distributed camera HDF service,
 * and provide Hardware Driver Interfaces (HDIs) for the upper layer.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef HDI_DISTRIBUTED_CAMERA_PROVIDER_H
#define HDI_DISTRIBUTED_CAMERA_PROVIDER_H

#include "idistributed_camera_provider_callback.h"

namespace OHOS {
namespace DistributedHardware {
enum {
    CMD_DISTRIBUTED_CAMERA_PROVIDER_ENABLE_DEVICE = 0,
    CMD_DISTRIBUTED_CAMERA_PROVIDER_DISABLE_DEVICE,
    CMD_DISTRIBUTED_CAMERA_PROVIDER_ACQUIRE_BUFFER,
    CMD_DISTRIBUTED_CAMERA_PROVIDER_SHUTTER_BUFFER,
    CMD_DISTRIBUTED_CAMERA_PROVIDER_ON_SETTINGS_RESULT,
    CMD_DISTRIBUTED_CAMERA_PROVIDER_NOTIFY,
};

class IDCameraProvider : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"HDI.DCamera.V1_0.Provider");
    virtual ~IDCameraProvider() {}

public:
    /**
     * @brief Obtains an <b>IDCameraProvider</b> instance.
     * This function provides the entry to the distributed camera SA service.
     * You must use this function to obtain an <b>IDCameraProvider</b> instance before performing other operations.
     *
     * @return Returns the <b>IDCameraProvider</b> instance if the operation is successful,
     * returns <b>nullptr</b> otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    static sptr<IDCameraProvider> Get();

    /**
     * @brief Enable distributed camera device and set callback. For details about the callbacks,
     * see {@link IDCameraProviderCallback}.
     *
     * @param dhBase [in] Distributed hardware device base info.
     *
     * @param abilityInfo [in] The static capability info of the distributed camera device to be enabled.
     *
     * @param callback [in] Indicates the callbacks to set.
     *
     * @return Returns <b>NO_ERROR</b> if the operation is successful,
     * returns an error code defined in {@link DCamRetCode} otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual DCamRetCode EnableDCameraDevice(const std::shared_ptr<DHBase> &dhBase,
        const std::string &abilityInfo, const sptr<IDCameraProviderCallback> &callback) = 0;

    /**
     * @brief Disable distributed camera device.
     *
     * @param dhBase [in] Distributed hardware device base info
     *
     * @return Returns <b>NO_ERROR</b> if the operation is successful,
     * returns an error code defined in {@link DCamRetCode} otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual DCamRetCode DisableDCameraDevice(const std::shared_ptr<DHBase> &dhBase) = 0;

    /**
     * @brief Acquire a frame buffer from the procedure handle which attached to the streamId.
     *
     * @param dhBase [in] Distributed hardware device base info
     *
     * @param streamId [in] Indicates the ID of the stream to which the procedure handle is to be attached.
     *
     * @param buffer [out] A frame buffer
     *
     * @return Returns <b>NO_ERROR</b> if the operation is successful,
     * returns an error code defined in {@link DCamRetCode} otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual DCamRetCode AcquireBuffer(const std::shared_ptr<DHBase> &dhBase, int streamId,
        std::shared_ptr<DCameraBuffer> &buffer) = 0;

    /**
     * @brief Notify distributed camera HDF service when a frame buffer has been filled.
     *
     * @param dhBase [in] Distributed hardware device base info
     *
     * @param streamId [in] Indicates the ID of the stream to which the frame buffer is to be attached.
     *
     * @param buffer [out] output frame buffer
     *
     * @return Returns <b>NO_ERROR</b> if the operation is successful,
     * returns an error code defined in {@link DCamRetCode} otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual DCamRetCode ShutterBuffer(const std::shared_ptr<DHBase> &dhBase, int streamId,
        const std::shared_ptr<DCameraBuffer> &buffer) = 0;

    /**
     * @brief Called to report metadata related to the distributed camera device.
     *
     * @param dhBase [in] Distributed hardware device base info
     *
     * @param result Indicates the metadata reported.
     *
     * @return Returns <b>NO_ERROR</b> if the operation is successful,
     * returns an error code defined in {@link DCamRetCode} otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual DCamRetCode OnSettingsResult(const std::shared_ptr<DHBase> &dhBase,
        const std::shared_ptr<DCameraSettings> &result) = 0;

    /**
     * @brief Called to notify some events from distributed camera SA service to distributed camera HDF service.
     *
     * @param dhBase [in] Distributed hardware device base info
     *
     * @param event [in] Detail event contents
     *
     * @return Returns <b>NO_ERROR</b> if the operation is successful,
     * returns an error code defined in {@link DCamRetCode} otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual DCamRetCode Notify(const std::shared_ptr<DHBase> &dhBase,
        const std::shared_ptr<DCameraHDFEvent> &event) = 0;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // HDI_DISTRIBUTED_CAMERA_PROVIDER_H