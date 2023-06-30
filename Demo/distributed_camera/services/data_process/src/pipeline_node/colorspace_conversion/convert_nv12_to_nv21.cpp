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

#include "convert_nv12_to_nv21.h"

#include "distributed_hardware_log.h"

#include "distributed_camera_errno.h"

namespace OHOS {
namespace DistributedHardware {
bool ConvertNV12ToNV21::IsConvertible(const VideoConfigParams& sourceConfig, const VideoConfigParams& targetConfig)
{
    return (sourceConfig.GetVideoformat() == Videoformat::NV12 && targetConfig.GetVideoformat() == Videoformat::NV21 &&
        sourceConfig.GetWidth() == targetConfig.GetWidth() && sourceConfig.GetHeight() == targetConfig.GetHeight());
}

int32_t ConvertNV12ToNV21::GetImageUnitInfo(ImageUnitInfo& imgInfo, const std::shared_ptr<DataBuffer>& imgBuf)
{
    if (imgBuf == nullptr) {
        DHLOGE("GetImageUnitInfo failed, imgBuf is nullptr.");
        return DCAMERA_BAD_VALUE;
    }

    bool findErr = true;
    int32_t colorFormat = 0;
    findErr = findErr && imgBuf->FindInt32("Videoformat", colorFormat);
    if (!findErr) {
        DHLOGE("GetImageUnitInfo failed, Videoformat is null.");
        return DCAMERA_NOT_FOUND;
    }
    if (colorFormat != static_cast<int32_t>(Videoformat::YUVI420) &&
        colorFormat != static_cast<int32_t>(Videoformat::NV12) &&
        colorFormat != static_cast<int32_t>(Videoformat::NV21)) {
        DHLOGE("GetImageUnitInfo failed, colorFormat %d are not supported.", colorFormat);
        return DCAMERA_NOT_FOUND;
    }
    imgInfo.colorFormat = static_cast<Videoformat>(colorFormat);
    findErr = findErr && imgBuf->FindInt32("width", imgInfo.width);
    findErr = findErr && imgBuf->FindInt32("height", imgInfo.height);
    findErr = findErr && imgBuf->FindInt32("alignedWidth", imgInfo.alignedWidth);
    findErr = findErr && imgBuf->FindInt32("alignedHeight", imgInfo.alignedHeight);
    if (!findErr) {
        DHLOGE("GetImageUnitInfo failed, width %d, height %d, alignedWidth %d, alignedHeight %d.",
            imgInfo.width, imgInfo.height, imgInfo.alignedWidth, imgInfo.alignedHeight);
        return DCAMERA_NOT_FOUND;
    }

    imgInfo.chromaOffset = static_cast<size_t>(imgInfo.alignedWidth * imgInfo.alignedHeight);
    imgInfo.imgSize = imgBuf->Size();
    imgInfo.imgData = imgBuf->Data();
    if (imgInfo.imgData == nullptr) {
        DHLOGE("Get the imgData of the imgBuf failed.");
        return DCAMERA_BAD_VALUE;
    }
    DHLOGD("imgBuf info : Videoformat %d, alignedWidth %d, alignedHeight %d, width %d, height %d, chromaOffset %d, " +
        "imgSize %d.", imgInfo.colorFormat, imgInfo.width, imgInfo.height, imgInfo.alignedWidth,
        imgInfo.alignedHeight, imgInfo.chromaOffset, imgInfo.imgSize);
    return DCAMERA_OK;
}

bool ConvertNV12ToNV21::IsCorrectImageUnitInfo(const ImageUnitInfo& imgInfo)
{
    int32_t y2UvRatio = 2;
    int32_t bytesPerPixel = 3;
    size_t expectedImgSize = static_cast<size_t>(imgInfo.alignedWidth * imgInfo.alignedHeight *
                                                 bytesPerPixel / y2UvRatio);
    size_t expectedChromaOffset = static_cast<size_t>(imgInfo.alignedWidth * imgInfo.alignedHeight);
    return (imgInfo.width <= imgInfo.alignedWidth && imgInfo.height <= imgInfo.alignedHeight &&
        imgInfo.imgSize >= expectedImgSize && imgInfo.chromaOffset == expectedChromaOffset);
}

int32_t ConvertNV12ToNV21::CheckColorConvertInfo(const ImageUnitInfo& srcImgInfo, const ImageUnitInfo& dstImgInfo)
{
    if (srcImgInfo.imgData == nullptr || dstImgInfo.imgData == nullptr) {
        DHLOGE("The imgData of srcImgInfo or the imgData of dstImgInfo are null!");
        return DCAMERA_BAD_VALUE;
    }
    if (srcImgInfo.colorFormat != Videoformat::NV12 && dstImgInfo.colorFormat != Videoformat::NV21) {
        DHLOGE("CopyInfo error : srcImgInfo colorFormat %d, dstImgInfo colorFormat %d.",
            srcImgInfo.colorFormat, dstImgInfo.colorFormat);
        return DCAMERA_BAD_VALUE;
    }

    if (!IsCorrectImageUnitInfo(srcImgInfo)) {
        DHLOGE("srcImginfo fail: width %d, height %d, alignedWidth %d, alignedHeight %d, chromaOffset %lld, " +
            "imgSize %lld.", srcImgInfo.width, srcImgInfo.height, srcImgInfo.alignedWidth, srcImgInfo.alignedHeight,
            srcImgInfo.chromaOffset, srcImgInfo.imgSize);
        return DCAMERA_BAD_VALUE;
    }
    if (!IsCorrectImageUnitInfo(dstImgInfo)) {
        DHLOGE("dstImginfo fail: width %d, height %d, alignedWidth %d, alignedHeight %d, chromaOffset %lld, " +
            "imgSize %lld.", dstImgInfo.width, dstImgInfo.height, dstImgInfo.alignedWidth, dstImgInfo.alignedHeight,
            dstImgInfo.chromaOffset, dstImgInfo.imgSize);
        return DCAMERA_BAD_VALUE;
    }

    if (dstImgInfo.width > srcImgInfo.alignedWidth || dstImgInfo.height > srcImgInfo.alignedHeight) {
        DHLOGE("Comparison ImgInfo fail: dstwidth %d, dstheight %d, srcAlignedWidth %d, srcAlignedHeight %d.",
            dstImgInfo.width, dstImgInfo.height, srcImgInfo.alignedWidth, srcImgInfo.alignedHeight);
        return DCAMERA_BAD_VALUE;
    }
    return DCAMERA_OK;
}

/**
* @brief Separate a row of srcUVPlane into half a row of dstUPlane and half a row of dstVPlane. For example,
* converts the UVPlane memory arrangement of NV12 to the UV memory arrangement of YUVI420. Note that the
* stride and width of the dstImage must be the same.
*/
void ConvertNV12ToNV21::SeparateUVPlaneByRow(const uint8_t *srcUVPlane, uint8_t *dstUPlane, uint8_t *dstVPlane,
    int32_t srcHalfWidth)
{
    int32_t memoryOffset0 = 0;
    int32_t memoryOffset1 = 1;
    int32_t memoryOffset2 = 2;
    int32_t memoryOffset3 = 3;
    int32_t perSeparatebytes = 4;
    for (int32_t x = 0; x < srcHalfWidth - 1; x += memoryOffset2) {
        dstUPlane[x] = srcUVPlane[memoryOffset0];
        dstUPlane[x + memoryOffset1] = srcUVPlane[memoryOffset2];
        dstVPlane[x] = srcUVPlane[memoryOffset1];
        dstVPlane[x + memoryOffset1] = srcUVPlane[memoryOffset3];
        srcUVPlane += perSeparatebytes;
    }
    if (static_cast<uint32_t>(srcHalfWidth) & 1) {
        dstUPlane[srcHalfWidth - 1] = srcUVPlane[memoryOffset0];
        dstVPlane[srcHalfWidth - 1] = srcUVPlane[memoryOffset1];
    }
}

int32_t ConvertNV12ToNV21::SeparateNV12UVPlane(const ImageUnitInfo& srcImgInfo, const ImageUnitInfo& dstImgInfo)
{
    int32_t ret = CheckColorConvertInfo(srcImgInfo, dstImgInfo);
    if (ret != DCAMERA_OK) {
        DHLOGE("ColorConvert : CheckColorConvertInfo failed.");
        return ret;
    }

    int32_t y2UvRatio = 2;
    uint8_t *srcUVPlane = srcImgInfo.imgData + srcImgInfo.chromaOffset;
    int32_t srcUVStride = srcImgInfo.alignedWidth;
    uint8_t *dstUPlane = dstImgInfo.imgData + dstImgInfo.chromaOffset;
    int32_t dstUStride = dstImgInfo.alignedWidth / y2UvRatio;
    uint8_t *dstVPlane = dstUPlane + (dstImgInfo.chromaOffset / y2UvRatio) / y2UvRatio;
    int32_t dstVStride = dstImgInfo.alignedWidth / y2UvRatio;
    int32_t width = srcImgInfo.width / y2UvRatio;
    int32_t height = srcImgInfo.height / y2UvRatio;
    DHLOGD("srcUVStride %d, dstUStride %d, dstVStride %d, src half width %d, src half height %d.",
        srcUVStride, dstUStride, dstVStride, width, height);

    /* Negative height means invert the image. */
    if (height < 0) {
        height = -height;
        dstUPlane = dstUPlane + (height - 1) * dstUStride;
        dstVPlane = dstVPlane + (height - 1) * dstVStride;
        dstUStride = -dstUStride;
        dstVStride = -dstVStride;
    }
    /* No black border of srcImage and dstImage, and the strides of srcImage and dstImage are equal. */
    if (srcUVStride == width * y2UvRatio && dstUStride == width && dstVStride == width) {
        SeparateUVPlaneByRow(srcUVPlane, dstUPlane, dstVPlane, width * height);
        return DCAMERA_OK;
    }
    /* Black borders exist in srcImage or dstImage. */
    for (int32_t y = 0; y < height; ++y) {
        SeparateUVPlaneByRow(srcUVPlane, dstUPlane, dstVPlane, width);
        dstUPlane += dstUStride;
        dstVPlane += dstVStride;
        srcUVPlane += srcUVStride;
    }
    return DCAMERA_OK;
}

/**
* @brief Combine half a row of srcUPlane and half a row of srcVPlane into a row of dstUVPlane. For example,
* converts the UVPlane memory arrangement of YUVI420 to the UV memory arrangement of NV12. Note that the
* stride and width of the srcImage must be the same.
*/
void ConvertNV12ToNV21::CombineUVPlaneByRow(const uint8_t *srcUPlane, const uint8_t *srcVPlane, uint8_t *dstUVPlane,
    int32_t dstHalfWidth)
{
    int32_t memoryOffset0 = 0;
    int32_t memoryOffset1 = 1;
    int32_t memoryOffset2 = 2;
    int32_t memoryOffset3 = 3;
    int32_t perCombinebytes = 4;
    for (int32_t x = 0; x < dstHalfWidth - 1; x += memoryOffset2) {
        dstUVPlane[memoryOffset0] = srcUPlane[x];
        dstUVPlane[memoryOffset1] = srcVPlane[x];
        dstUVPlane[memoryOffset2] = srcUPlane[x + memoryOffset1];
        dstUVPlane[memoryOffset3] = srcVPlane[x + memoryOffset1];
        dstUVPlane += perCombinebytes;
    }
    if (static_cast<uint32_t>(dstHalfWidth) & 1) {
        dstUVPlane[memoryOffset0] = srcUPlane[dstHalfWidth - 1];
        dstUVPlane[memoryOffset1] = srcVPlane[dstHalfWidth - 1];
    }
}

int32_t ConvertNV12ToNV21::CombineNV12UVPlane(const ImageUnitInfo& srcImgInfo, const ImageUnitInfo& dstImgInfo)
{
    int32_t ret = CheckColorConvertInfo(srcImgInfo, dstImgInfo);
    if (ret != DCAMERA_OK) {
        DHLOGE("ColorConvert : CheckColorConvertInfo failed.");
        return ret;
    }

    int32_t y2UvRatio = 2;
    uint8_t *srcVPlane = srcImgInfo.imgData + srcImgInfo.chromaOffset;
    int32_t srcVStride = srcImgInfo.alignedWidth / y2UvRatio;
    uint8_t *srcUPlane = srcVPlane + (srcImgInfo.chromaOffset / y2UvRatio) / y2UvRatio;
    int32_t srcUStride = srcImgInfo.alignedWidth / y2UvRatio;
    uint8_t *dstUVPlane = dstImgInfo.imgData + dstImgInfo.chromaOffset;
    int32_t dstUVStride = dstImgInfo.alignedWidth;
    int32_t width = dstImgInfo.width / y2UvRatio;
    int32_t height = dstImgInfo.height / y2UvRatio;
    DHLOGD("srcUStride %d, srcVStride %d, dstUVStride %d, dst half width %d, dst half height %d.",
        srcUStride, srcVStride, dstUVStride, width, height);

    /* Negative height means invert the image. */
    if (height < 0) {
        height = -height;
        dstUVPlane = dstUVPlane + (height - 1) * dstUVStride;
        dstUVStride = -dstUVStride;
    }
    /* No black border of srcImage and dstImage, and the strides of srcImage and dstImage are equal. */
    if (srcUStride == width && srcVStride == width && dstUVStride == width * y2UvRatio) {
        CombineUVPlaneByRow(srcUPlane, srcVPlane, dstUVPlane, width * height);
        return DCAMERA_OK;
    }
    /* Black borders exist in srcImage or dstImage. */
    for (int32_t y = 0; y < height; ++y) {
        CombineUVPlaneByRow(srcUPlane, srcVPlane, dstUVPlane, width);
        srcUPlane += srcUStride;
        srcVPlane += srcVStride;
        dstUVPlane += dstUVStride;
    }
    return DCAMERA_OK;
}

int32_t ConvertNV12ToNV21::CopyYPlane(const ImageUnitInfo& srcImgInfo, const ImageUnitInfo& dstImgInfo)
{
    int32_t ret = CheckColorConvertInfo(srcImgInfo, dstImgInfo);
    if (ret != DCAMERA_OK) {
        DHLOGE("ColorConvert : CheckColorConvertInfo failed.");
        return ret;
    }

    errno_t err = EOK;
    size_t totalCopyYPlaneSize = dstImgInfo.alignedWidth * dstImgInfo.height;
    if (srcImgInfo.alignedWidth == dstImgInfo.width && dstImgInfo.alignedWidth == dstImgInfo.width) {
        /* No black border of srcImage and dstImage, and the strides of srcImage and dstImage are equal. */
        err = memcpy_s(dstImgInfo.imgData, totalCopyYPlaneSize, srcImgInfo.imgData, totalCopyYPlaneSize);
        if (err != EOK) {
            DHLOGE("ColorConvert : memcpy_s CopyYPlaner failed by Coalesce rows.");
            return DCAMERA_MEMORY_OPT_ERROR;
        }
    } else {
        /* Black borders exist in srcImage or dstImage. */
        int32_t srcDataOffset = 0;
        int32_t dstDataOffset = 0;
        for (int32_t yh = 0; yh < dstImgInfo.height; yh++) {
            DHLOGE("ColorConvert : memcpy_s Line[%d] source buffer failed.", yh);
            err = memcpy_s(dstImgInfo.imgData + dstDataOffset, totalCopyYPlaneSize - dstDataOffset,
                srcImgInfo.imgData + srcDataOffset, dstImgInfo.width);
            if (err != EOK) {
                DHLOGE("memcpy_s YPlane in line[%d] failed.", yh);
                return DCAMERA_MEMORY_OPT_ERROR;
            }
            dstDataOffset += dstImgInfo.alignedWidth;
            srcDataOffset += srcImgInfo.alignedWidth;
        }
        DHLOGD("ColorConvert :get valid yplane OK, srcImgInfo: alignedWidth %d, width %d, height %d. " +
            "dstImgInfo: alignedWidth %d, width %d, height %d. dstDataOffset %d, srcDataOffset %d.",
            srcImgInfo.alignedWidth, srcImgInfo.width, srcImgInfo.height, dstImgInfo.alignedWidth,
            dstImgInfo.width, dstImgInfo.height, dstDataOffset, srcDataOffset);
    }
    return DCAMERA_OK;
}

int32_t ConvertNV12ToNV21::ColorConvertNV12ToNV21(const ImageUnitInfo& srcImgInfo, const ImageUnitInfo& dstImgInfo)
{
    int32_t err = CheckColorConvertInfo(srcImgInfo, dstImgInfo);
    if (err != DCAMERA_OK) {
        DHLOGE("ColorConvertNV12ToNV21 : CheckColorConvertInfo failed.");
        return err;
    }
    err = CopyYPlane(srcImgInfo, dstImgInfo);
    if (err != DCAMERA_OK) {
        DHLOGE("ColorConvertNV12ToNV21 : CopyYPlane failed.");
        return err;
    }

    std::shared_ptr<DataBuffer> tempPlaneYUV = std::make_shared<DataBuffer>(dstImgInfo.imgSize);
    ImageUnitInfo tempImgInfo = dstImgInfo;
    tempImgInfo.imgData = tempPlaneYUV->Data();
    SeparateNV12UVPlane(srcImgInfo, tempImgInfo);
    CombineNV12UVPlane(tempImgInfo, dstImgInfo);
    return DCAMERA_OK;
}

std::shared_ptr<DataBuffer> ConvertNV12ToNV21::ProcessData(const std::shared_ptr<DataBuffer>& srcBuf,
    const VideoConfigParams& sourceConfig, const VideoConfigParams& targetConfig)
{
    if (srcBuf == nullptr) {
        DHLOGE("ColorConvertProcessData : srcBuf is null.");
        return nullptr;
    }
    if (!IsConvertible(sourceConfig, targetConfig)) {
        DHLOGE("ColorConvertProcessData : Only supported convert videoformat NV12 to NV21.");
        DHLOGE("sourceConfig: Videoformat %d Width %d, Height %d, targetConfig: Videoformat %d Width %d, Height %d.",
            sourceConfig.GetVideoformat(), sourceConfig.GetWidth(), sourceConfig.GetHeight(),
            targetConfig.GetVideoformat(), targetConfig.GetWidth(), targetConfig.GetHeight());
        return nullptr;
    }
    int64_t timeStamp = 0;
    if (!(srcBuf->FindInt64("timeUs", timeStamp))) {
        DHLOGE("ColorConvertProcessData : Find srcBuf timeStamp failed.");
        return nullptr;
    }

    ImageUnitInfo srcImgInfo {Videoformat::YUVI420, 0, 0, 0, 0, 0, 0, nullptr};
    if (GetImageUnitInfo(srcImgInfo, srcBuf) != DCAMERA_OK) {
        DHLOGE("ColorConvertProcessData : Get srcImgInfo failed.");
        return nullptr;
    }
    int32_t y2UvRatio = 2;
    int32_t bytesPerPixel = 3;
    size_t dstBufsize = sourceConfig.GetWidth() * sourceConfig.GetHeight() * bytesPerPixel / y2UvRatio;
    std::shared_ptr<DataBuffer> dstBuf = std::make_shared<DataBuffer>(dstBufsize);
    ImageUnitInfo dstImgInfo = { targetConfig.GetVideoformat(), static_cast<int32_t>(sourceConfig.GetWidth()),
        static_cast<int32_t>(sourceConfig.GetHeight()), static_cast<int32_t>(sourceConfig.GetWidth()),
        static_cast<int32_t>(sourceConfig.GetHeight()), sourceConfig.GetWidth() * sourceConfig.GetHeight(),
        dstBuf->Size(), dstBuf->Data() };
    int32_t err = ColorConvertNV12ToNV21(srcImgInfo, dstImgInfo);
    if (err != DCAMERA_OK) {
        return nullptr;
    }
    dstBuf->SetInt64("timeUs", timeStamp);
    dstBuf->SetInt32("Videoformat", static_cast<int32_t>(targetConfig.GetVideoformat()));
    dstBuf->SetInt32("alignedWidth", static_cast<int32_t>(sourceConfig.GetWidth()));
    dstBuf->SetInt32("alignedHeight", static_cast<int32_t>(sourceConfig.GetHeight()));
    dstBuf->SetInt32("width", static_cast<int32_t>(sourceConfig.GetWidth()));
    dstBuf->SetInt32("height", static_cast<int32_t>(sourceConfig.GetHeight()));
    DHLOGD("ColorConvert end, dstBuf Videoformat %d, width %d, height %d, alignedWidth %d, alignedHeight %d, " +
        "ImgSize%d, timeUs %lld.", targetConfig.GetVideoformat(), sourceConfig.GetWidth(), sourceConfig.GetHeight(),
        sourceConfig.GetWidth(), sourceConfig.GetHeight(), dstBuf->Size(), timeStamp);
    return dstBuf;
}
} // namespace DistributedHardware
} // namespace OHOS
