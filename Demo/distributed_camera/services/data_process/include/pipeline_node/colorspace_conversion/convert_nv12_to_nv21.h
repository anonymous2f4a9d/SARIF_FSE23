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

#ifndef OHOS_CONVERT_NV12TONV21_H
#define OHOS_CONVERT_NV12TONV21_H

#include "securec.h"
#include "data_buffer.h"
#include "image_common_type.h"

namespace OHOS {
namespace DistributedHardware {
class ConvertNV12ToNV21 {
public:
    ConvertNV12ToNV21() = default;
    ~ConvertNV12ToNV21() = default;
    std::shared_ptr<DataBuffer> ProcessData(const std::shared_ptr<DataBuffer>& srcBuf,
        const VideoConfigParams& sourceConfig, const VideoConfigParams& targetConfig);

private:
    bool IsConvertible(const VideoConfigParams& sourceConfig, const VideoConfigParams& targetConfig);
    int32_t GetImageUnitInfo(ImageUnitInfo& imgInfo, const std::shared_ptr<DataBuffer>& imgBuf);
    bool IsCorrectImageUnitInfo(const ImageUnitInfo& imgInfo);
    int32_t CheckColorConvertInfo(const ImageUnitInfo& srcImgInfo, const ImageUnitInfo& dstImgInfo);
    void SeparateUVPlaneByRow(const uint8_t *srcUVPlane, uint8_t *dstUPlane, uint8_t *dstVPlane,
        int32_t srcHalfWidth);
    int32_t SeparateNV12UVPlane(const ImageUnitInfo& srcImgInfo, const ImageUnitInfo& dstImgInfo);
    void CombineUVPlaneByRow(const uint8_t *srcUPlane, const uint8_t *srcVPlane, uint8_t *dstUVPlane,
        int32_t dstHalfWidth);
    int32_t CombineNV12UVPlane(const ImageUnitInfo& srcImgInfo, const ImageUnitInfo& dstImgInfo);
    int32_t CopyYPlane(const ImageUnitInfo& srcImgInfo, const ImageUnitInfo& dstImgInfo);
    int32_t ColorConvertNV12ToNV21(const ImageUnitInfo& srcImgInfo, const ImageUnitInfo& dstImgInfo);
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_CONVERT_NV12TONV21_H
