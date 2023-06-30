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

#include "dcamera_utils_tools.h"

#include "distributed_camera_constants.h"
#include "distributed_camera_errno.h"
#include "distributed_hardware_log.h"

#include "softbus_bus_center.h"

namespace OHOS {
namespace DistributedHardware {
const int OFFSET2 = 2;
const int OFFSET4 = 4;
const int OFFSET6 = 6;
const int INDEX_FIRST = 0;
const int INDEX_SECOND = 1;
const int INDEX_THIRD = 2;
const int INDEX_FORTH = 3;
int32_t GetLocalDeviceNetworkId(std::string& networkId)
{
    NodeBasicInfo basicInfo = { { 0 } };
    int32_t ret = GetLocalNodeDeviceInfo(DCAMERA_PKG_NAME.c_str(), &basicInfo);
    if (ret != DCAMERA_OK) {
        DHLOGE("GetLocalDeviceNetworkId GetLocalNodeDeviceInfo failed ret: %d", ret);
        return ret;
    }

    networkId = std::string(basicInfo.networkId);
    return DCAMERA_OK;
}

int64_t GetNowTimeStampMs()
{
    std::chrono::milliseconds nowMs = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch());
    return nowMs.count();
}

int64_t GetNowTimeStampUs()
{
    std::chrono::microseconds nowUs = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::system_clock::now().time_since_epoch());
    return nowUs.count();
}

std::string Base64Encode(const unsigned char *toEncode, unsigned int len)
{
    std::string ret;
    uint32_t i = 0;
    unsigned char charArray3[3];
    unsigned char charArray4[4];

    while (len--) {
        charArray3[i++] = *(toEncode++);
        if (i == sizeof(charArray3)) {
            charArray4[INDEX_FIRST] = (charArray3[INDEX_FIRST] & 0xfc) >> OFFSET2;
            charArray4[INDEX_SECOND] = ((charArray3[INDEX_FIRST] & 0x03) << OFFSET4) +
                ((charArray3[INDEX_SECOND] & 0xf0) >> OFFSET4);
            charArray4[INDEX_THIRD] = ((charArray3[INDEX_SECOND] & 0x0f) << OFFSET2) +
                ((charArray3[INDEX_THIRD] & 0xc0) >> OFFSET6);
            charArray4[INDEX_FORTH] = charArray3[INDEX_THIRD] & 0x3f;
            for (i = 0; i < sizeof(charArray4); i++) {
                ret += BASE_64_CHARS[charArray4[i]];
            }
            i = 0;
        }
    }

    if (i) {
        uint32_t j = 0;
        for (j = i; j < sizeof(charArray3); j++) {
            charArray3[j] = '\0';
        }
        charArray4[INDEX_FIRST] = (charArray3[INDEX_FIRST] & 0xfc) >> OFFSET2;
        charArray4[INDEX_SECOND] = ((charArray3[INDEX_FIRST] & 0x03) << OFFSET4) +
            ((charArray3[INDEX_SECOND] & 0xf0) >> OFFSET4);
        charArray4[INDEX_THIRD] = ((charArray3[INDEX_SECOND] & 0x0f) << OFFSET2) +
            ((charArray3[INDEX_THIRD] & 0xc0) >> OFFSET6);
        charArray4[INDEX_FORTH] = charArray3[INDEX_THIRD] & 0x3f;
        for (j = 0; j < i + 1; j++) {
            ret += BASE_64_CHARS[charArray4[j]];
        }
        while (i++ < sizeof(charArray3)) {
            ret += '=';
        }
    }
    return ret;
}

std::string Base64Decode(const std::string& basicString)
{
    std::string ret;
    uint32_t i = 0;
    int index = 0;
    int len = static_cast<int>(basicString.size());
    unsigned char charArray3[3];
    unsigned char charArray4[4];

    while (len-- && (basicString[index] != '=') && IsBase64(basicString[index])) {
        charArray4[i++] = basicString[index];
        index++;
        if (i == sizeof(charArray4)) {
            for (i = 0; i < sizeof(charArray4); i++) {
                charArray4[i] = BASE_64_CHARS.find(charArray4[i]);
            }
            charArray3[INDEX_FIRST] = (charArray4[INDEX_FIRST] << OFFSET2) +
                ((charArray4[INDEX_SECOND] & 0x30) >> OFFSET4);
            charArray3[INDEX_SECOND] = ((charArray4[INDEX_SECOND] & 0xf) << OFFSET4) +
                ((charArray4[INDEX_THIRD] & 0x3c) >> OFFSET2);
            charArray3[INDEX_THIRD] = ((charArray4[INDEX_THIRD] & 0x3) << OFFSET6) + charArray4[INDEX_FORTH];
            for (i = 0; i < sizeof(charArray3); i++) {
                ret += charArray3[i];
            }
            i = 0;
        }
    }

    if (i) {
        uint32_t j = 0;
        for (j = i; j < sizeof(charArray4); j++) {
            charArray4[j] = 0;
        }
        for (j = 0; j < sizeof(charArray4); j++) {
            charArray4[j] = BASE_64_CHARS.find(charArray4[j]);
        }
        charArray3[INDEX_FIRST] = (charArray4[INDEX_FIRST] << OFFSET2) +
            ((charArray4[INDEX_SECOND] & 0x30) >> OFFSET4);
        charArray3[INDEX_SECOND] = ((charArray4[INDEX_SECOND] & 0xf) << OFFSET4) +
            ((charArray4[INDEX_THIRD] & 0x3c) >> OFFSET2);
        charArray3[INDEX_THIRD] = ((charArray4[INDEX_THIRD] & 0x3) << OFFSET6) + charArray4[INDEX_FORTH];
        for (j = 0; j < i - 1; j++) {
            ret += charArray3[j];
        }
    }
    return ret;
}

bool IsBase64(unsigned char c)
{
    return (isalnum(c) || (c == '+') || (c == '/'));
}
} // namespace DistributedHardware
} // namespace OHOS
