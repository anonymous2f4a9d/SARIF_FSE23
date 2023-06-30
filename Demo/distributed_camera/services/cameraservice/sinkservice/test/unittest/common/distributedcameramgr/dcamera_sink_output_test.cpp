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

#include <gtest/gtest.h>
#include <memory>
#define private public
#include "dcamera_sink_output.h"
#undef private

#include "mock_camera_channel.h"
#include "mock_camera_operator.h"
#include "mock_dcamera_sink_data_process.h"

#include "dcamera_sink_access_control.h"
#include "dcamera_sink_controller.h"
#include "dcamera_sink_data_process.h"
#include "dcamera_sink_dev.h"
#include "dcamera_utils_tools.h"
#include "distributed_camera_errno.h"

using namespace testing::ext;

namespace OHOS {
namespace DistributedHardware {
class DCameraSinkOutputTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

    std::shared_ptr<DCameraSinkOutput> output_;
    std::shared_ptr<ICameraOperator> operator_;
    std::shared_ptr<ICameraChannel> channel_;
    std::shared_ptr<ICameraSinkDataProcess> dataProcess_;
};
const std::string TEST_DH_ID = "Camera_device@3.5/legacy/1";
const std::string SESSION_FLAG_CONTINUE = "dataContinue";
const std::string SESSION_FLAG_SNAPSHOT = "dataSnapshot";
const int32_t TEST_WIDTH = 1080;
const int32_t TEST_HEIGHT = 1920;

std::string g_testDeviceIdOutput;
DCameraChannelDetail g_testDetailOutput;

std::shared_ptr<DCameraCaptureInfo> g_testCaptureInfoOutputContinuousNotEncode;
std::shared_ptr<DCameraCaptureInfo> g_testCaptureInfoOutputContinuousNeedEncode;
std::shared_ptr<DCameraCaptureInfo> g_testCaptureInfoOutputSnapshot;

std::shared_ptr<DCameraChannelInfo> g_testChannelInfo;

std::vector<std::shared_ptr<DCameraCaptureInfo>> g_testCaptureInfosOutput;

void DCameraSinkOutputTest::SetUpTestCase(void)
{
    GetLocalDeviceNetworkId(g_testDeviceIdOutput);

    std::shared_ptr<DCameraSettings> cameraSetting = std::make_shared<DCameraSettings>();
    cameraSetting->type_ = UPDATE_METADATA;
    cameraSetting->value_ = "";

    g_testCaptureInfoOutputContinuousNotEncode = std::make_shared<DCameraCaptureInfo>();
    g_testCaptureInfoOutputContinuousNotEncode->width_ = TEST_WIDTH;
    g_testCaptureInfoOutputContinuousNotEncode->height_ = TEST_HEIGHT;
    g_testCaptureInfoOutputContinuousNotEncode->format_  = ENCODE_TYPE_H264;
    g_testCaptureInfoOutputContinuousNotEncode->dataspace_ = 0;
    g_testCaptureInfoOutputContinuousNotEncode->encodeType_ = ENCODE_TYPE_H264;
    g_testCaptureInfoOutputContinuousNotEncode->streamType_ = CONTINUOUS_FRAME;
    g_testCaptureInfoOutputContinuousNotEncode->captureSettings_.push_back(cameraSetting);

    g_testCaptureInfoOutputContinuousNeedEncode = std::make_shared<DCameraCaptureInfo>();
    g_testCaptureInfoOutputContinuousNeedEncode->width_ = TEST_WIDTH;
    g_testCaptureInfoOutputContinuousNeedEncode->height_ = TEST_HEIGHT;
    g_testCaptureInfoOutputContinuousNeedEncode->format_  = ENCODE_TYPE_H264;
    g_testCaptureInfoOutputContinuousNeedEncode->dataspace_ = 0;
    g_testCaptureInfoOutputContinuousNeedEncode->encodeType_ = ENCODE_TYPE_H265;
    g_testCaptureInfoOutputContinuousNeedEncode->streamType_ = CONTINUOUS_FRAME;
    g_testCaptureInfoOutputContinuousNeedEncode->captureSettings_.push_back(cameraSetting);

    g_testCaptureInfoOutputSnapshot = std::make_shared<DCameraCaptureInfo>();
    g_testCaptureInfoOutputSnapshot->width_ = TEST_WIDTH;
    g_testCaptureInfoOutputSnapshot->height_ = TEST_HEIGHT;
    g_testCaptureInfoOutputSnapshot->format_  = ENCODE_TYPE_JPEG;
    g_testCaptureInfoOutputSnapshot->dataspace_ = 0;
    g_testCaptureInfoOutputSnapshot->encodeType_ = ENCODE_TYPE_JPEG;
    g_testCaptureInfoOutputSnapshot->streamType_ = SNAPSHOT_FRAME;
    g_testCaptureInfoOutputSnapshot->captureSettings_.push_back(cameraSetting);

    g_testDetailOutput.dataSessionFlag_ = SESSION_FLAG_CONTINUE;
    g_testDetailOutput.streamType_ = CONTINUOUS_FRAME;

    g_testChannelInfo = std::make_shared<DCameraChannelInfo>();
    g_testChannelInfo->sourceDevId_ = g_testDeviceIdOutput;
    std::vector<DCameraChannelDetail> detail0;
    detail0.push_back(g_testDetailOutput);
    g_testChannelInfo->detail_ = detail0;
}

void DCameraSinkOutputTest::TearDownTestCase(void)
{
}

void DCameraSinkOutputTest::SetUp(void)
{
    operator_ = std::make_shared<MockCameraOperator>();
    output_ = std::make_shared<DCameraSinkOutput>(TEST_DH_ID, operator_);
    channel_ = std::make_shared<MockCameraChannel>();
    dataProcess_ = std::make_shared<MockDCameraSinkDataProcess>(channel_);
    output_->channels_.emplace(SNAPSHOT_FRAME, channel_);
    output_->channels_.emplace(CONTINUOUS_FRAME, channel_);
    output_->dataProcesses_.emplace(SNAPSHOT_FRAME, dataProcess_);
    output_->dataProcesses_.emplace(CONTINUOUS_FRAME, dataProcess_);
}

void DCameraSinkOutputTest::TearDown(void)
{
    output_->channels_.clear();
    output_->dataProcesses_.clear();

    operator_ = nullptr;
    output_ = nullptr;
}

/**
 * @tc.name: dcamera_sink_output_test_001
 * @tc.desc: Verify the UnInit function.
 * @tc.type: FUNC
 * @tc.require: AR000GK6MV
 */
HWTEST_F(DCameraSinkOutputTest, dcamera_sink_output_test_001, TestSize.Level1)
{
    int32_t ret = output_->Init();
    EXPECT_EQ(DCAMERA_OK, ret);
    EXPECT_EQ(true, output_->isInit_);

    ret = output_->UnInit();
    EXPECT_EQ(DCAMERA_OK, ret);
    EXPECT_EQ(false, output_->isInit_);
}

/**
 * @tc.name: dcamera_sink_output_test_002
 * @tc.desc: Verify the StartCapture function.
 * @tc.type: FUNC
 * @tc.require: AR000GK6MU
 */
HWTEST_F(DCameraSinkOutputTest, dcamera_sink_output_test_002, TestSize.Level1)
{
    g_testCaptureInfosOutput.push_back(g_testCaptureInfoOutputContinuousNotEncode);
    int32_t ret = output_->StartCapture(g_testCaptureInfosOutput);
    EXPECT_EQ(DCAMERA_OK, ret);

    g_testCaptureInfosOutput.clear();
}

/**
 * @tc.name: dcamera_sink_output_test_003
 * @tc.desc: Verify the StartCapture function.
 * @tc.type: FUNC
 * @tc.require: AR000GK6MU
 */
HWTEST_F(DCameraSinkOutputTest, dcamera_sink_output_test_003, TestSize.Level1)
{
    g_testCaptureInfosOutput.push_back(g_testCaptureInfoOutputContinuousNeedEncode);
    int32_t ret = output_->StartCapture(g_testCaptureInfosOutput);
    EXPECT_EQ(DCAMERA_OK, ret);

    g_testCaptureInfosOutput.clear();
}

/**
 * @tc.name: dcamera_sink_output_test_004
 * @tc.desc: Verify the StartCapture function.
 * @tc.type: FUNC
 * @tc.require: AR000GK6MU
 */
HWTEST_F(DCameraSinkOutputTest, dcamera_sink_output_test_004, TestSize.Level1)
{
    g_testCaptureInfosOutput.push_back(g_testCaptureInfoOutputSnapshot);
    int32_t ret = output_->StartCapture(g_testCaptureInfosOutput);
    EXPECT_EQ(DCAMERA_OK, ret);

    g_testCaptureInfosOutput.clear();
}

/**
 * @tc.name: dcamera_sink_output_test_005
 * @tc.desc: Verify the StopCapture function.
 * @tc.type: FUNC
 * @tc.require: AR000GK6N1
 */
HWTEST_F(DCameraSinkOutputTest, dcamera_sink_output_test_005, TestSize.Level1)
{
    g_testCaptureInfosOutput.push_back(g_testCaptureInfoOutputContinuousNotEncode);
    int32_t ret = output_->StartCapture(g_testCaptureInfosOutput);
    EXPECT_EQ(DCAMERA_OK, ret);

    ret = output_->StopCapture();
    EXPECT_EQ(DCAMERA_OK, ret);

    g_testCaptureInfosOutput.clear();
}

/**
 * @tc.name: dcamera_sink_output_test_006
 * @tc.desc: Verify the OpenChannel function.
 * @tc.type: FUNC
 * @tc.require: AR000GK6N1
 */
HWTEST_F(DCameraSinkOutputTest, dcamera_sink_output_test_006, TestSize.Level1)
{
    int32_t ret = output_->OpenChannel(g_testChannelInfo);
    EXPECT_EQ(DCAMERA_OK, ret);
}

/**
 * @tc.name: dcamera_sink_output_test_007
 * @tc.desc: Verify the CloseChannel function.
 * @tc.type: FUNC
 * @tc.require: AR000GK6N1
 */
HWTEST_F(DCameraSinkOutputTest, dcamera_sink_output_test_007, TestSize.Level1)
{
    int32_t ret = output_->CloseChannel();
    EXPECT_EQ(DCAMERA_OK, ret);
}
} // namespace DistributedHardware
} // namespace OHOS