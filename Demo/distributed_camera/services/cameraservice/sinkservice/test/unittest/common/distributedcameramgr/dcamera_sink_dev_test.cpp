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

#include "anonymous_string.h"
#include "distributed_hardware_log.h"

#define private public
#include "dcamera_sink_access_control.h"
#include "dcamera_sink_controller.h"
#include "dcamera_sink_dev.h"
#undef private

#include "dcamera_utils_tools.h"
#include "distributed_camera_errno.h"
#include "mock_dcamera_sink_controller.h"


using namespace testing::ext;

namespace OHOS {
namespace DistributedHardware {
class DCameraSinkDevTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

    std::shared_ptr<DCameraSinkDev> dev_;
};
const std::string TEST_CAMERA_DH_ID_0 = "Camera_device@3.5/legacy/1";
const std::string TEST_PARAMETER = "";
std::string g_testCameraInfo = "";

std::string g_testChannelInfoDevContinue = R"({
    "Type": "OPERATION",
    "dhId": "camrea_0",
    "Command": "CHANNEL_NEG",
    "Value": {"SourceDevId": "TestDevId",
    "Detail": [{"DataSessionFlag": "dataContinue", "StreamType": 0}]}
})";

std::string g_testOpenInfoDev = R"({
    "Type": "OPERATION",
    "dhId": "camrea_0",
    "Command": "OPEN_CHANNEL",
    "Value": {"SourceDevId": "TestDevId"}
})";

std::string g_testChannelInfoDevEmpty = "";
std::string g_testOpenInfoDevEmpty = "";

void DCameraSinkDevTest::SetUpTestCase(void)
{
}

void DCameraSinkDevTest::TearDownTestCase(void)
{
}

void DCameraSinkDevTest::SetUp(void)
{
    dev_ = std::make_shared<DCameraSinkDev>(TEST_CAMERA_DH_ID_0);

    dev_->accessControl_ = std::make_shared<DCameraSinkAccessControl>();
    dev_->controller_ = std::make_shared<MockDCameraSinkController>(dev_->accessControl_);
}

void DCameraSinkDevTest::TearDown(void)
{
    dev_ = nullptr;
}

/**
 * @tc.name: dcamera_sink_dev_test_001
 * @tc.desc: Verify the Init and UnInit function.
 * @tc.type: FUNC
 * @tc.require: AR000GK6MV
 */
HWTEST_F(DCameraSinkDevTest, dcamera_sink_dev_test_001, TestSize.Level1)
{
    int32_t ret = dev_->Init();
    EXPECT_EQ(DCAMERA_OK, ret);
    EXPECT_EQ(true, dev_->isInit_);

    ret = dev_->UnInit();
    EXPECT_EQ(DCAMERA_OK, ret);
    EXPECT_EQ(false, dev_->isInit_);
}

/**
 * @tc.name: dcamera_sink_dev_test_002
 * @tc.desc: Verify the SubscribeLocalHardware function.
 * @tc.type: FUNC
 * @tc.require: AR000GK6MT
 */
HWTEST_F(DCameraSinkDevTest, dcamera_sink_dev_test_002, TestSize.Level1)
{
    int32_t ret = dev_->SubscribeLocalHardware(TEST_PARAMETER);
    EXPECT_EQ(DCAMERA_OK, ret);
}

/**
 * @tc.name: dcamera_sink_dev_test_003
 * @tc.desc: Verify the UnsubscribeLocalHardware function.
 * @tc.type: FUNC
 * @tc.require: AR000GK6MT
 */
HWTEST_F(DCameraSinkDevTest, dcamera_sink_dev_test_003, TestSize.Level1)
{
    int32_t ret = dev_->UnsubscribeLocalHardware();
    EXPECT_EQ(DCAMERA_OK, ret);
}

/**
 * @tc.name: dcamera_sink_dev_test_004
 * @tc.desc: Verify the GetCameraInfo function.
 * @tc.type: FUNC
 * @tc.require: AR000GK6MT
 */
HWTEST_F(DCameraSinkDevTest, dcamera_sink_dev_test_004, TestSize.Level1)
{
    int32_t ret = dev_->GetCameraInfo(g_testCameraInfo);
    EXPECT_EQ(DCAMERA_OK, ret);
    DHLOGI("DCameraSinkDevTest::GetCameraInfo cameraInfo is %s", GetAnonyString(g_testCameraInfo).c_str());
}

/**
 * @tc.name: dcamera_sink_dev_test_005
 * @tc.desc: Verify the ChannelNeg function.
 * @tc.type: FUNC
 * @tc.require: AR000GK6MT
 */
HWTEST_F(DCameraSinkDevTest, dcamera_sink_dev_test_005, TestSize.Level1)
{
    int32_t ret = dev_->ChannelNeg(g_testChannelInfoDevEmpty);
    EXPECT_EQ(DCAMERA_BAD_VALUE, ret);
}

/**
 * @tc.name: dcamera_sink_dev_test_006
 * @tc.desc: Verify the ChannelNeg function.
 * @tc.type: FUNC
 * @tc.require: AR000GK6MT
 */
HWTEST_F(DCameraSinkDevTest, dcamera_sink_dev_test_006, TestSize.Level1)
{
    int32_t ret = dev_->ChannelNeg(g_testChannelInfoDevContinue);
    EXPECT_EQ(DCAMERA_OK, ret);
}

/**
 * @tc.name: dcamera_sink_dev_test_007
 * @tc.desc: Verify the StopCapture function.
 * @tc.type: FUNC
 * @tc.require: AR000GK6N1
 */
HWTEST_F(DCameraSinkDevTest, dcamera_sink_dev_test_007, TestSize.Level1)
{
    int32_t ret = dev_->StopCapture();
    EXPECT_EQ(DCAMERA_OK, ret);
}

/**
 * @tc.name: dcamera_sink_dev_test_008
 * @tc.desc: Verify the OpenChannel function.
 * @tc.type: FUNC
 * @tc.require: AR000GK6N1
 */
HWTEST_F(DCameraSinkDevTest, dcamera_sink_dev_test_008, TestSize.Level1)
{
    int32_t ret = dev_->OpenChannel(g_testOpenInfoDevEmpty);
    EXPECT_EQ(DCAMERA_BAD_VALUE, ret);
}

/**
 * @tc.name: dcamera_sink_dev_test_009
 * @tc.desc: Verify the OpenChannel function.
 * @tc.type: FUNC
 * @tc.require: AR000GK6N1
 */
HWTEST_F(DCameraSinkDevTest, dcamera_sink_dev_test_009, TestSize.Level1)
{
    int32_t ret = dev_->OpenChannel(g_testOpenInfoDev);
    EXPECT_EQ(DCAMERA_OK, ret);
}

/**
 * @tc.name: dcamera_sink_dev_test_010
 * @tc.desc: Verify the CloseChannel function.
 * @tc.type: FUNC
 * @tc.require: AR000GK6N1
 */
HWTEST_F(DCameraSinkDevTest, dcamera_sink_dev_test_010, TestSize.Level1)
{
    int32_t ret = dev_->CloseChannel();
    EXPECT_EQ(DCAMERA_OK, ret);
}
} // namespace DistributedHardware
} // namespace OHOS