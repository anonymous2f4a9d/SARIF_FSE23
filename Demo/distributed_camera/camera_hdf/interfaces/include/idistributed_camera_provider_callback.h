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
 * @file idistributed_camera_provider_callback.h
 *
 * @brief Declares callbacks for distributed camera SA service. The caller needs to implement the callbacks.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef HDI_DISTRIBUTED_CAMERA_PROVIDER_CALLBACK_H
#define HDI_DISTRIBUTED_CAMERA_PROVIDER_CALLBACK_H

#include <vector>
#include <iremote_broker.h>
#include "types.h"

namespace OHOS {
namespace DistributedHardware {
enum {
    CMD_DISTRIBUTED_CAMERA_PROVIDER_CALLBACK_OPEN_SESSION = 0,
    CMD_DISTRIBUTED_CAMERA_PROVIDER_CALLBACK_CLOSE_SESSION,
    CMD_DISTRIBUTED_CAMERA_PROVIDER_CALLBACK_CONFIGURE_STREAMS,
    CMD_DISTRIBUTED_CAMERA_PROVIDER_CALLBACK_RELEASE_STREAMS,
    CMD_DISTRIBUTED_CAMERA_PROVIDER_CALLBACK_START_CAPTURE,
    CMD_DISTRIBUTED_CAMERA_PROVIDER_CALLBACK_STOP_CAPTURE,
    CMD_DISTRIBUTED_CAMERA_PROVIDER_CALLBACK_UPDATE_SETTINGS,
};

class IDCameraProviderCallback : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"HDI.DCamera.V1_0.ProviderCallback");
    virtual ~IDCameraProviderCallback() {}

public:
    /**
     * @brief Create the transmission channel between the source device and the sink device.
     * Open and initialize the distributed camera session.
     *
     * @param dhBase [in] Distributed hardware device base info
     *
     * @return Returns <b>NO_ERROR</b> if the operation is successful,
     * returns an error code defined in {@link DCamRetCode} otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual DCamRetCode OpenSession(const std::shared_ptr<DHBase> &dhBase) = 0;

    /**
     * @brief Close the distributed camera session, and destroy the transmission channel between
     * the source device and the sink device.
     *
     * @param dhBase [in] Distributed hardware device base info
     *
     * @return Returns <b>NO_ERROR</b> if the operation is successful,
     * returns an error code defined in {@link DCamRetCode} otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual DCamRetCode CloseSession(const std::shared_ptr<DHBase> &dhBase) = 0;

    /**
     * @brief Configures streams.
     *
     * @param dhBase [in] Distributed hardware device base info
     *
     * @param streamInfos [in] Indicates the list of stream information, which is defined by {@link DCStreamInfo}.
     *
     * @return Returns <b>NO_ERROR</b> if the operation is successful,
     * returns an error code defined in {@link DCamRetCode} otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual DCamRetCode ConfigureStreams(const std::shared_ptr<DHBase> &dhBase,
        const std::vector<std::shared_ptr<DCStreamInfo>> &streamInfos) = 0;

    /**
     * @brief Releases streams.
     *
     * @param dhBase [in] Distributed hardware device base info
     *
     * @param streamIds [IN] Indicates the IDs of the streams to release.
     *
     * @return Returns <b>NO_ERROR</b> if the operation is successful,
     * returns an error code defined in {@link DCamRetCode} otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual DCamRetCode ReleaseStreams(const std::shared_ptr<DHBase> &dhBase, const std::vector<int> &streamIds) = 0;

    /**
     * @brief Start capture images.
     * This function must be called after {@link ConfigStreams}.
     * There are two image capture modes: continuous capture and single capture.
     *
     * @param dhBase [in] Distributed hardware device base info
     *
     * @param captureInfos [in] Indicates the capture request configuration information.
     * For details, see {@link DCCaptureInfo}.
     *
     * @return Returns <b>NO_ERROR</b> if the operation is successful,
     * returns an error code defined in {@link DCamRetCode} otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual DCamRetCode StartCapture(const std::shared_ptr<DHBase> &dhBase,
        const std::vector<std::shared_ptr<DCCaptureInfo>> &captureInfos) = 0;

    /**
     * @brief Stop capture images.
     *
     * @param dhBase [in] Distributed hardware device base info
     *
     * @return Returns <b>NO_ERROR</b> if the operation is successful,
     * returns an error code defined in {@link DCamRetCode} otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual DCamRetCode StopCapture(const std::shared_ptr<DHBase> &dhBase) = 0;

    /**
     * @brief Updates distributed camera device control parameters.
     *
     * @param dhBase [in] Distributed hardware device base info
     *
     * @param settings [in] Indicates the camera parameters, including the sensor frame rate and 3A parameters.
     * For details about the settings, see {@link DCameraSettings}.
     *
     * @return Returns <b>NO_ERROR</b> if the operation is successful,
     * returns an error code defined in {@link DCamRetCode} otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual DCamRetCode UpdateSettings(const std::shared_ptr<DHBase> &dhBase,
        const std::vector<std::shared_ptr<DCameraSettings>> &settings) = 0;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // HDI_DISTRIBUTED_CAMERA_PROVIDER_CALLBACK_H