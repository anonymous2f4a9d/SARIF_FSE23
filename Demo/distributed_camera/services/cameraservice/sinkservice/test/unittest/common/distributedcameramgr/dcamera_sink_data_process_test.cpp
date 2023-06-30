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

#include <fstream>
#include <memory>
#define private public
#include "dcamera_sink_data_process.h"
#undef private

#include <gtest/gtest.h>
#include <securec.h>

#include "distributed_camera_errno.h"
#include "mock_camera_channel.h"
#include "mock_data_process_pipeline.h"

using namespace testing::ext;

namespace OHOS {
namespace DistributedHardware {
class DCameraSinkDataProcessTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

    std::shared_ptr<DCameraSinkDataProcess> dataProcess_;
    std::shared_ptr<ICameraChannel> channel_;
};
const std::string TEST_DH_ID = "Camera_device@3.5/legacy/1";
const std::string TEST_STRING = "test_string";
const int32_t TEST_WIDTH = 1080;
const int32_t TEST_HEIGHT = 1920;

std::shared_ptr<DCameraCaptureInfo> g_testCaptureInfoContinuousNotEncode;
std::shared_ptr<DCameraCaptureInfo> g_testCaptureInfoContinuousNeedEncode;
std::shared_ptr<DCameraCaptureInfo> g_testCaptureInfoSnapshot;

std::shared_ptr<DataBuffer> g_testDataBuffer;

void DCameraSinkDataProcessTest::SetUpTestCase(void)
{
    std::shared_ptr<DCameraSettings> cameraSetting = std::make_shared<DCameraSettings>();
    cameraSetting->type_ = UPDATE_METADATA;
    cameraSetting->value_ = "";

    g_testCaptureInfoContinuousNotEncode = std::make_shared<DCameraCaptureInfo>();
    g_testCaptureInfoContinuousNotEncode->width_ = TEST_WIDTH;
    g_testCaptureInfoContinuousNotEncode->height_ = TEST_HEIGHT;
    g_testCaptureInfoContinuousNotEncode->format_  = ENCODE_TYPE_H264;
    g_testCaptureInfoContinuousNotEncode->dataspace_ = 0;
    g_testCaptureInfoContinuousNotEncode->encodeType_ = ENCODE_TYPE_H264;
    g_testCaptureInfoContinuousNotEncode->streamType_ = CONTINUOUS_FRAME;
    g_testCaptureInfoContinuousNotEncode->captureSettings_.push_back(cameraSetting);

    g_testCaptureInfoContinuousNeedEncode = std::make_shared<DCameraCaptureInfo>();
    g_testCaptureInfoContinuousNeedEncode->width_ = TEST_WIDTH;
    g_testCaptureInfoContinuousNeedEncode->height_ = TEST_HEIGHT;
    g_testCaptureInfoContinuousNeedEncode->format_  = ENCODE_TYPE_H264;
    g_testCaptureInfoContinuousNeedEncode->dataspace_ = 0;
    g_testCaptureInfoContinuousNeedEncode->encodeType_ = ENCODE_TYPE_H265;
    g_testCaptureInfoContinuousNeedEncode->streamType_ = CONTINUOUS_FRAME;
    g_testCaptureInfoContinuousNeedEncode->captureSettings_.push_back(cameraSetting);

    g_testCaptureInfoSnapshot = std::make_shared<DCameraCaptureInfo>();
    g_testCaptureInfoSnapshot->width_ = TEST_WIDTH;
    g_testCaptureInfoSnapshot->height_ = TEST_HEIGHT;
    g_testCaptureInfoSnapshot->format_  = ENCODE_TYPE_JPEG;
    g_testCaptureInfoSnapshot->dataspace_ = 0;
    g_testCaptureInfoSnapshot->encodeType_ = ENCODE_TYPE_JPEG;
    g_testCaptureInfoSnapshot->streamType_ = SNAPSHOT_FRAME;
    g_testCaptureInfoSnapshot->captureSettings_.push_back(cameraSetting);

    g_testDataBuffer = std::make_shared<DataBuffer>(TEST_STRING.length() + 1);
    memcpy_s(g_testDataBuffer->Data(), g_testDataBuffer->Capacity(),
        (uint8_t *)TEST_STRING.c_str(), TEST_STRING.length());
}

void DCameraSinkDataProcessTest::TearDownTestCase(void)
{
}

void DCameraSinkDataProcessTest::SetUp(void)
{
    channel_ = std::make_shared<MockCameraChannel>();
    dataProcess_ = std::make_shared<DCameraSinkDataProcess>(TEST_DH_ID, channel_);

    dataProcess_->pipeline_ = std::make_shared<MockDataProcessPipeline>();
    dataProcess_->captureInfo_ = g_testCaptureInfoContinuousNeedEncode;
}

void DCameraSinkDataProcessTest::TearDown(void)
{
    channel_ = nullptr;
    dataProcess_ = nullptr;
}

/**
 * @tc.name: dcamera_sink_data_process_test_001
 * @tc.desc: Verify the StartCapture function.
 * @tc.type: FUNC
 * @tc.require: AR000GK6MU
 */
HWTEST_F(DCameraSinkDataProcessTest, dcamera_sink_data_process_test_001, TestSize.Level1)
{
    int32_t ret = dataProcess_->StartCapture(g_testCaptureInfoContinuousNotEncode);
    EXPECT_EQ(DCAMERA_OK, ret);
}

/**
 * @tc.name: dcamera_sink_data_process_test_002
 * @tc.desc: Verify the StartCapture function.
 * @tc.type: FUNC
 * @tc.require: AR000GK6MU
 */
HWTEST_F(DCameraSinkDataProcessTest, dcamera_sink_data_process_test_002, TestSize.Level1)
{
    int32_t ret = dataProcess_->StartCapture(g_testCaptureInfoContinuousNeedEncode);
    EXPECT_EQ(DCAMERA_OK, ret);
}

/**
 * @tc.name: dcamera_sink_data_process_test_003
 * @tc.desc: Verify the StartCapture function.
 * @tc.type: FUNC
 * @tc.require: AR000GK6MU
 */
HWTEST_F(DCameraSinkDataProcessTest, dcamera_sink_data_process_test_003, TestSize.Level1)
{
    int32_t ret = dataProcess_->StartCapture(g_testCaptureInfoSnapshot);
    EXPECT_EQ(DCAMERA_OK, ret);
}

/**
 * @tc.name: dcamera_sink_data_process_test_004
 * @tc.desc: Verify the StopCapture function.
 * @tc.type: FUNC
 * @tc.require: AR000GK6N1
 */
HWTEST_F(DCameraSinkDataProcessTest, dcamera_sink_data_process_test_004, TestSize.Level1)
{
    int32_t ret = dataProcess_->StopCapture();
    EXPECT_EQ(DCAMERA_OK, ret);
}

/**
 * @tc.name: dcamera_sink_data_process_test_005
 * @tc.desc: Verify the FeedStream function.
 * @tc.type: FUNC
 * @tc.require: AR000GK6MV
 */
HWTEST_F(DCameraSinkDataProcessTest, dcamera_sink_data_process_test_005, TestSize.Level1)
{
    dataProcess_->captureInfo_ = g_testCaptureInfoContinuousNotEncode;
    int32_t ret = dataProcess_->FeedStream(g_testDataBuffer);
    EXPECT_EQ(DCAMERA_OK, ret);
}

/**
 * @tc.name: dcamera_sink_data_process_test_006
 * @tc.desc: Verify the FeedStream function.
 * @tc.type: FUNC
 * @tc.require: AR000GK6MV
 */
HWTEST_F(DCameraSinkDataProcessTest, dcamera_sink_data_process_test_006, TestSize.Level1)
{
    dataProcess_->captureInfo_ = g_testCaptureInfoContinuousNeedEncode;
    int32_t ret = dataProcess_->FeedStream(g_testDataBuffer);
    EXPECT_EQ(DCAMERA_OK, ret);
}

/**
 * @tc.name: dcamera_sink_data_process_test_007
 * @tc.desc: Verify the FeedStream function.
 * @tc.type: FUNC
 * @tc.require: AR000GK6MV
 */
HWTEST_F(DCameraSinkDataProcessTest, dcamera_sink_data_process_test_007, TestSize.Level1)
{
    dataProcess_->captureInfo_ = g_testCaptureInfoSnapshot;
    int32_t ret = dataProcess_->FeedStream(g_testDataBuffer);
    EXPECT_EQ(DCAMERA_OK, ret);
}
} // namespace DistributedHardware
} // namespace OHOS