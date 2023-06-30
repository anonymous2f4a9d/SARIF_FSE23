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

#include "dcamera.h"
#include <chrono>

namespace OHOS {
namespace DistributedHardware {
CamRetCode MapToExternalRetCode(DCamRetCode retCode)
{
    switch (retCode) {
        case DCamRetCode::SUCCESS:
            return CamRetCode::NO_ERROR;
        case DCamRetCode::CAMERA_BUSY:
            return CamRetCode::CAMERA_BUSY;
        case DCamRetCode::INVALID_ARGUMENT:
            return CamRetCode::INVALID_ARGUMENT;
        case DCamRetCode::METHOD_NOT_SUPPORTED:
            return CamRetCode::METHOD_NOT_SUPPORTED;
        case DCamRetCode::CAMERA_OFFLINE:
            return CamRetCode::CAMERA_CLOSED;
        case DCamRetCode::EXCEED_MAX_NUMBER:
            return CamRetCode::INSUFFICIENT_RESOURCES;
        case DCamRetCode::FAILED:
            return CamRetCode::DEVICE_ERROR;
        default:
            break;
    }
    return CamRetCode::DEVICE_ERROR;
}

DCamRetCode MapToInternalRetCode(CamRetCode retCode)
{
    switch (retCode) {
        case CamRetCode::NO_ERROR:
            return DCamRetCode::SUCCESS;
        case CamRetCode::CAMERA_BUSY:
            return DCamRetCode::CAMERA_BUSY;
        case CamRetCode::INSUFFICIENT_RESOURCES:
            return DCamRetCode::EXCEED_MAX_NUMBER;
        case CamRetCode::INVALID_ARGUMENT:
            return DCamRetCode::INVALID_ARGUMENT;
        case CamRetCode::METHOD_NOT_SUPPORTED:
            return DCamRetCode::METHOD_NOT_SUPPORTED;
        case CamRetCode::CAMERA_CLOSED:
            return DCamRetCode::CAMERA_OFFLINE;
        case CamRetCode::DEVICE_ERROR:
            return DCamRetCode::FAILED;
        default:
            break;
    }
    return DCamRetCode::FAILED;
}

uint64_t GetCurrentLocalTimeStamp()
{
    std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> tp =
        std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
    auto tmp = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch());
    return static_cast<uint64_t>(tmp.count());
}

void SplitString(const std::string &str, std::vector<std::string> &tokens, const std::string &delimiters)
{
    std::string::size_type lastPos = 0;
    std::string::size_type pos = str.find(delimiters);
    while (std::string::npos != pos) {
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        lastPos = pos + delimiters.size();
        pos = str.find(delimiters, lastPos);
    }
    if (lastPos != str.length()) {
        tokens.push_back(str.substr(lastPos));
    }
}
} // namespace DistributedHardware
} // namespace OHOS
