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

#ifndef DISTRIBUTED_CAMERA_H
#define DISTRIBUTED_CAMERA_H

#include <foundation/distributedhardware/distributedcamera/camera_hdf/interfaces/include/types.h>
#ifdef BALTIMORE_CAMERA
#include <drivers/peripheral/adapter/camera/interfaces/include/types.h>
#else
#include <drivers/peripheral/camera/interfaces/include/types.h>
#endif

namespace OHOS {
namespace DistributedHardware {
using namespace OHOS::Camera;
using RetCode = uint32_t;
using MetaType = int32_t;

CamRetCode MapToExternalRetCode(DCamRetCode retCode);

DCamRetCode MapToInternalRetCode(CamRetCode retCode);

uint64_t GetCurrentLocalTimeStamp();

void SplitString(const std::string &str, std::vector<std::string> &tokens, const std::string &delimiters);
} // namespace DistributedHardware
} // namespace OHOS
#endif // DISTRIBUTED_CAMERA_H
