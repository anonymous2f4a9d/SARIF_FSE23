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

#ifndef DISTRIBUTED_CAMERA_METADATA_PROCESSOR_H
#define DISTRIBUTED_CAMERA_METADATA_PROCESSOR_H

#include <set>
#include <map>
#include <vector>
#include "constants.h"
#include "dcamera.h"

#include <foundation/distributedhardware/distributedcamera/camera_hdf/interfaces/include/types.h>
#ifdef BALTIMORE_CAMERA
#include <drivers/peripheral/adapter/camera/interfaces/include/types.h>
#else
#include <drivers/peripheral/camera/interfaces/include/types.h>
#endif

namespace OHOS {
namespace DistributedHardware {
using namespace OHOS::Camera;
class DMetadataProcessor {
public:
    DMetadataProcessor() = default;
    ~DMetadataProcessor() = default;
    DMetadataProcessor(const DMetadataProcessor &other) = delete;
    DMetadataProcessor(DMetadataProcessor &&other) = delete;
    DMetadataProcessor& operator=(const DMetadataProcessor &other) = delete;
    DMetadataProcessor& operator=(DMetadataProcessor &&other) = delete;

public:
    DCamRetCode InitDCameraAbility(const std::string &abilityInfo);
    DCamRetCode GetDCameraAbility(std::shared_ptr<CameraAbility> &ability);
    DCamRetCode SetMetadataResultMode(const ResultCallbackMode &mode);
    DCamRetCode GetEnabledMetadataResults(std::vector<MetaType> &results);
    DCamRetCode EnableMetadataResult(const std::vector<MetaType> &results);
    DCamRetCode DisableMetadataResult(const std::vector<MetaType> &results);
    DCamRetCode ResetEnableResults();
    DCamRetCode SaveResultMetadata(std::string resultStr);
    DCamRetCode UpdateResultMetadata(bool &needReturn, std::shared_ptr<CameraStandard::CameraMetadata> &result);
    void PrintDCameraMetadata(const common_metadata_header_t *metadata);

private:
    DCamRetCode InitDCameraDefaultAbilityKeys(const std::string &abilityInfo);
    DCamRetCode InitDCameraOutputAbilityKeys(const std::string &abilityInfo);
    DCamRetCode AddAbilityEntry(uint32_t tag, const void *data, size_t size);
    DCamRetCode UpdateAbilityEntry(uint32_t tag, const void *data, size_t size);
    void ConvertToCameraMetadata(common_metadata_header_t *&input,
                                 std::shared_ptr<CameraStandard::CameraMetadata> &output);
    void ResizeMetadataHeader(common_metadata_header_t *header, uint32_t itemCapacity, uint32_t dataCapacity);
    uint32_t GetDataSize(uint32_t type);
    std::map<int, std::vector<DCResolution>> GetDCameraSupportedFormats(const std::string &abilityInfo);

private:
    std::shared_ptr<CameraAbility> dCameraAbility_;
    std::string protocolVersion_;
    std::string dCameraPosition_;
    DCResolution maxPreviewResolution_;
    DCResolution maxPhotoResolution_;
    ResultCallbackMode metaResultMode_;
    std::set<MetaType> allResultSet_;
    std::set<MetaType> enabledResultSet_;

    // The latest result metadata that received from the sink device.
    common_metadata_header_t *latestProducerResultMetadata_;

    // The latest result metadata that replied to the camera service.
    common_metadata_header_t *latestConsumerResultMetadata_;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // DISTRIBUTED_CAMERA_METADATA_PROCESSOR_H