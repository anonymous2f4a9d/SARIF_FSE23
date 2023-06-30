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

#ifndef OHOS_DCAMERA_PROTOCOL_H
#define OHOS_DCAMERA_PROTOCOL_H

#include <string>

namespace OHOS {
namespace DistributedHardware {
static const std::string DCAMERA_PROTOCOL_TYPE_MESSAGE = "MESSAGE";
static const std::string DCAMERA_PROTOCOL_TYPE_OPERATION = "OPERATION";

static const std::string DCAMERA_PROTOCOL_CMD_GET_INFO = "GET_INFO";
static const std::string DCAMERA_PROTOCOL_CMD_CHAN_NEG = "CHANNEL_NEG";
static const std::string DCAMERA_PROTOCOL_CMD_UPDATE_METADATA = "UPDATE_METADATA";
static const std::string DCAMERA_PROTOCOL_CMD_METADATA_RESULT = "METADATA_RESULT";
static const std::string DCAMERA_PROTOCOL_CMD_STATE_NOTIFY = "STATE_NOTIFY";
static const std::string DCAMERA_PROTOCOL_CMD_CAPTURE = "CAPTURE";
static const std::string DCAMERA_PROTOCOL_CMD_STOP_CAPTURE = "STOP_CAPTURE";
static const std::string DCAMERA_PROTOCOL_CMD_OPEN_CHANNEL = "OPEN_CHANNEL";
static const std::string DCAMERA_PROTOCOL_CMD_CLOSE_CHANNEL = "CLOSE_CHANNEL";
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_DCAMERA_PROTOCOL_H
