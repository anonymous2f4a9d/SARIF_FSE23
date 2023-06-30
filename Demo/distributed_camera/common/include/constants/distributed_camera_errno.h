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

#ifndef OHOS_DISTRIBUTED_CAMERA_ERRNO_H
#define OHOS_DISTRIBUTED_CAMERA_ERRNO_H

namespace OHOS {
namespace DistributedHardware {
    enum DistributedCameraErrno {
        DCAMERA_OK = 0,
        DCAMERA_MEMORY_OPT_ERROR = -1,
        DCAMERA_BAD_VALUE = -2,
        DCAMERA_BAD_TYPE = -3,
        DCAMERA_ALREADY_EXISTS = -4,
        DCAMERA_INIT_ERR = -5,
        DCAMERA_NOT_FOUND = -6,
        DCAMERA_WRONG_STATE = -7,
        DCAMERA_BAD_OPERATE = -8,
        DCAMERA_OPEN_CONFLICT = -9,
        DCAMERA_DISABLE_PROCESS = -10,
        DCAMERA_INDEX_OVERFLOW = -11,
        DCAMERA_REGIST_HAL_FAILED = -12,
        DCAMERA_UNREGIST_HAL_FAILED = -13,
    };
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_DISTRIBUTED_CAMERA_ERRNO_H