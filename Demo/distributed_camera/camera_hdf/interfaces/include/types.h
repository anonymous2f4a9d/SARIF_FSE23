/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @file types.h
 *
 * @brief Declares data types used by the Hardware Driver Interfaces (HDIs) of this module.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef DISTRIBUTED_CAMERA_PROVIDER_TYPES_H
#define DISTRIBUTED_CAMERA_PROVIDER_TYPES_H

#include <string>
#include <vector>
#include "buffer_handle.h"

namespace OHOS {
namespace DistributedHardware {
/**
 * @brief Enumerates distributed camera metadata updating types.
 */
using DCSettingsType = enum _DCSettingsType : int32_t {
    /**
     * Set the whole package metadata.
     */
    UPDATE_METADATA = 0,
    /**
     * Enable metadata.
     */
    ENABLE_METADATA = 1,
    /**
     * Disable metadata.
     */
    DISABLE_METADATA = 2,
    /**
     * Metadata result.
     */
    METADATA_RESULT = 3,
    /**
     * Set flash light.
     */
    SET_FLASH_LIGHT = 4,
    /**
     * Set fps range.
     */
    FPS_RANGE = 5
};

/**
 * @brief Enumerates return values of the HDIs.
 */
using DCamRetCode = enum _DCamRetCode : int32_t {
    /**
     * Successful call.
     */
    SUCCESS = 0,
    /**
     * The camera device is busy.
     */
    CAMERA_BUSY = 1,
    /**
     * Invalid parameters.
     */
    INVALID_ARGUMENT = 2,
    /**
     * Unsupported function.
     */
    METHOD_NOT_SUPPORTED = 3,
    /**
     * The camera device is offlined.
     */
    CAMERA_OFFLINE = 4,
    /**
     * The number of distributed camera devices enabled exceeds the limit.
     */
    EXCEED_MAX_NUMBER = 5,
    /**
     * The device is not initialized.
     */
    DEVICE_NOT_INIT = 6,
    /**
     * Failed call.
     */
    FAILED = -1,
};

 /**
 * @brief Enumerates encoding types of stream data.
 */
using DCEncodeType = enum _DCEncodeType : int32_t {
    /**
     * Unspecified
     */
    ENCODE_TYPE_NULL = 0,

    /**
     * H.264
     */
    ENCODE_TYPE_H264 = 1,

    /**
     * H.265
     */
    ENCODE_TYPE_H265 = 2,

    /**
     * JPEG
     */
    ENCODE_TYPE_JPEG = 3,
};

/**
 * @brief Enumerates distributed camera inner stream types.
 */
using DCStreamType = enum _DCStreamType : int32_t {
    /**
     * Continuous capture stream. For example preview streams, video streams.
     */
    CONTINUOUS_FRAME = 0,
    /**
     * Single capture stream. For example photographing streams.
     */
    SNAPSHOT_FRAME = 1
};

/**
 * @brief Distributed hardware device base info.
 */
using DHBase = struct _DHBase {
    /**
     * The device id.
    */
    std::string deviceId_;
    /**
     * The distributed hardware id.
     */
    std::string dhId_;

    _DHBase() : deviceId_(""), dhId_("") {}

    _DHBase(const std::string &deviceId, const std::string &dhId) : deviceId_(deviceId), dhId_(dhId) {}

    bool operator ==(const _DHBase& others) const
    {
        return this->deviceId_ == others.deviceId_ && this->dhId_ == others.dhId_;
    }

    bool operator < (const _DHBase& others) const
    {
        return (this->deviceId_ + this->dhId_) < (others.deviceId_ + others.dhId_);
    }
};

/**
 * @brief The control settings of the distributed camera device.
 */
using DCameraSettings = struct _DCameraSettings {
    /**
     * Settings type.
     */
    DCSettingsType type_;
    /**
     * Settings value. Serialized from bool, array, structure, etc.
     */
    std::string value_;
};

/**
 * @brief Defines the inner stream information of the distributed camera,
 * which is used to pass configuration parameters during stream creation.
 */
using DCStreamInfo = struct _DCStreamInfo {
    /**
     * Stream ID, which uniquely identifies a stream on a camera device.
     */
    int streamId_;
    /**
     * Image width.
     */
    int width_;
    /**
     * Image height.
     */
    int height_;
    /**
     * Image stride.
     */
    int stride_;
    /**
     * Image format.
     */
    int format_;
    /**
     * Image color space.
     */
    int dataspace_;
    /**
     * Encoding type.
     */
    DCEncodeType encodeType_;
     /**
     * Stream type.
     */
    DCStreamType type_;
};

/**
 * @brief Defines the information about a inner capture request of the distributed camera.
 */
using DCCaptureInfo = struct _DCCaptureInfo {
    /**
     * IDs of captured streams.
    */
    std::vector<int> streamIds_;
    /**
     * Image width.
     */
    int width_;
    /**
     * Image height.
     */
    int height_;
    /**
     * Image stride.
     */
    int stride_;
    /**
     * Image format.
     */
    int format_;
    /**
     * Image color space.
     */
    int dataspace_;
    /**
     * Capture Info
     */
    bool isCapture_;
    /**
     * Encoding type.
     */
    DCEncodeType encodeType_;
     /**
     * Stream type.
     */
    DCStreamType type_;
     /**
     * Stream settings.
     */
    std::vector<std::shared_ptr<DCameraSettings>> captureSettings_;
};

/**
 * @brief Defines the inner buffer of the distributed camera,
 * which is used to acquire buffer during procesing capture requests.
 */
using DCameraBuffer = struct _DCameraBuffer {
    /**
     * Buffer index.
     */
    int32_t index_;
    /**
     * Buffer size.
     */
    uint32_t size_;
    /**
     * Buffer handle.
     */
    BufferHandle* bufferHandle_;
};

/**
 * @brief Notification event of the distributed camera.
 */
using DCameraHDFEvent = struct _DCameraHDFEvent {
    /**
     * Event type.
     */
    int32_t type_;
    /**
     * Event result.
     */
    int32_t result_;
    /**
     * Extended content (optional).
     */
    std::string content_;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // DISTRIBUTED_CAMERA_PROVIDER_TYPES_H