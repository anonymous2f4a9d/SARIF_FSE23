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

#ifndef OHOS_DCAMERA_UTILS_TOOL_H
#define OHOS_DCAMERA_UTILS_TOOL_H

#include <cstdint>
#include <string>
#include <chrono>

namespace OHOS {
namespace DistributedHardware {
const std::string BASE_64_CHARS = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int32_t GetLocalDeviceNetworkId(std::string& networkId);
int64_t GetNowTimeStampMs();
int64_t GetNowTimeStampUs();
std::string Base64Encode(const unsigned char *toEncode, unsigned int len);
std::string Base64Decode(const std::string& basicString);
bool IsBase64(unsigned char c);
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_DCAMERA_UTILS_TOOL_H
