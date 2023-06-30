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

#ifndef OHOS_DISTRIBUTED_CAMERA_CONSTANTS_H
#define OHOS_DISTRIBUTED_CAMERA_CONSTANTS_H

#include <cstdint>
#include <string>
#include <unistd.h>

namespace OHOS {
namespace DistributedHardware {
typedef enum {
    DCAMERA_SRV_STATE_NOT_START,
    DCAMERA_SRV_STATE_RUNNING
} DCameraServiceState;

typedef enum {
    DCAMERA_AUTHORIZATION_DEFAULT = 0,
    DCAMERA_AUTHORIZATION_AGREE = 1,
    DCAMERA_AUTHORIZATION_REJECT = 2,
    DCAMERA_AUTHORIZATION_TIMEOUT = 3,
} AuthorizationState;

typedef enum {
    DCAMERA_SAME_ACCOUNT = 0,
    DCAMERA_DIFF_ACCOUNT = 1,
    DCAMERA_NO_ACCOUNT = 2,
} AccessControlType;

typedef enum {
    DCAMERA_MESSAGE = 0,
    DCAMERA_OPERATION = 1,
} DCameraEventType;

typedef enum {
    DCAMERA_EVENT_CHANNEL_DISCONNECTED = 0,
    DCAMERA_EVENT_CHANNEL_CONNECTED = 1,
    DCAMERA_EVENT_CAMERA_SUCCESS = 2,

    DCAMERA_EVENT_CAMERA_ERROR = -1,
    DCAMERA_EVENT_OPEN_CHANNEL_ERROR = -2,
    DCAMERA_EVENT_CLOSE_CHANNEL_ERROR = -3,
    DCAMERA_EVENT_CONFIG_STREAMS_ERROR = -4,
    DCAMERA_EVENT_RELEASE_STREAMS_ERROR = -5,
    DCAMERA_EVENT_START_CAPTURE_ERROR = -6,
    DCAMERA_EVENT_STOP_CAPTURE_ERROR = -7,
    DCAMERA_EVENT_UPDATE_SETTINGS_ERROR = -8,
} DCameraEventResult;

typedef enum {
    OHOS_CAMERA_FORMAT_INVALID = 0,
    OHOS_CAMERA_FORMAT_RGBA_8888,
    OHOS_CAMERA_FORMAT_YCBCR_420_888,
    OHOS_CAMERA_FORMAT_YCRCB_420_SP,
    OHOS_CAMERA_FORMAT_JPEG,
} DCameraFormat;

const uint32_t DCAMERA_MAX_NUM = 1;
const uint32_t DCAMERA_PRODUCER_ONE_MINUTE_MS = 1000;
const uint32_t DCAMERA_PRODUCER_FPS_DEFAULT = 30;
const uint32_t DCAMERA_MAX_RECV_DATA_LEN = 104857600;
const uint32_t DISTRIBUTED_HARDWARE_CAMERA_SOURCE_SA_ID = 4803;
const uint32_t DISTRIBUTED_HARDWARE_CAMERA_SINK_SA_ID = 4804;
const std::string DCAMERA_PKG_NAME = "ohos.dhardware";
const std::string SNAP_SHOT_SESSION_FLAG = "dataSnapshot";
const std::string CONTINUE_SESSION_FLAG = "dataContinue";

const std::string DISTRIBUTED_HARDWARE_ID_KEY = "dhID";
const std::string CAMERA_ID_PREFIX = "Camera_";
const std::string CAMERA_PROTOCOL_VERSION_KEY = "ProtocolVer";
const std::string CAMERA_PROTOCOL_VERSION_VALUE = "1.0";
const std::string CAMERA_POSITION_KEY = "Position";
const std::string CAMERA_POSITION_BACK = "BACK";
const std::string CAMERA_POSITION_FRONT = "FRONT";
const std::string CAMERA_POSITION_UNSPECIFIED = "UNSPECIFIED";
const std::string CAMERA_METADATA_KEY = "MetaData";
const std::string CAMERA_CODEC_TYPE_KEY = "CodecType";
const std::string CAMERA_FORMAT_KEY = "OutputFormat";
const std::string CAMERA_FORMAT_PREVIEW = "Preview";
const std::string CAMERA_FORMAT_VIDEO = "Video";
const std::string CAMERA_FORMAT_PHOTO = "Photo";
const std::string CAMERA_RESOLUTION_KEY = "Resolution";
const std::string CAMERA_SURFACE_FORMAT = "CAMERA_SURFACE_FORMAT";

const int32_t RESOLUTION_MAX_WIDTH_SNAPSHOT = 4096;
const int32_t RESOLUTION_MAX_HEIGHT_SNAPSHOT = 3072;
const int32_t RESOLUTION_MAX_WIDTH_CONTINUOUS = 1920;
const int32_t RESOLUTION_MAX_HEIGHT_CONTINUOUS = 1080;
const int32_t RESOLUTION_MIN_WIDTH = 320;
const int32_t RESOLUTION_MIN_HEIGHT = 240;

const uint32_t DCAMERA_SHIFT_32 = 32;
const uint32_t DCAMERA_SHIFT_24 = 24;
const uint32_t DCAMERA_SHIFT_16 = 16;
const uint32_t DCAMERA_SHIFT_8 = 8;

const uint32_t UINT32_SHIFT_MASK_24 = 0xff000000;
const uint32_t UINT32_SHIFT_MASK_16 = 0x00ff0000;
const uint32_t UINT32_SHIFT_MASK_8 = 0x0000ff00;
const uint32_t UINT32_SHIFT_MASK_0 = 0x000000ff;
const uint16_t UINT16_SHIFT_MASK_0 = 0x00ff;
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_DISTRIBUTED_CAMERA_CONSTANTS_H