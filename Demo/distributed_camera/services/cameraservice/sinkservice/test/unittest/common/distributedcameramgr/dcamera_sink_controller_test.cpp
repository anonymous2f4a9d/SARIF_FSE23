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
#include "dcamera_sink_controller.h"
#undef private

#include "distributed_hardware_log.h"

#include "mock_camera_channel.h"
#include "mock_camera_operator.h"
#include "mock_dcamera_sink_output.h"

#include "dcamera_metadata_setting_cmd.h"
#include "dcamera_sink_access_control.h"
#include "dcamera_sink_dev.h"
#include "dcamera_utils_tools.h"
#include "distributed_camera_errno.h"

using namespace testing::ext;

namespace OHOS {
namespace DistributedHardware {
class DCameraSinkControllerTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

    std::shared_ptr<DCameraSinkController> controller_;
    std::shared_ptr<ICameraSinkAccessControl> accessControl_;
};
std::string g_testDeviceIdController;
const std::string TEST_DH_ID = "Camera_device@3.5/legacy/1";
const std::string SESSION_FLAG_CONTINUE = "dataContinue";
const std::string SESSION_FLAG_SNAPSHOT = "dataSnapshot";
const std::string TEST_DEVICE_ID_EMPTY = "";

const std::string TEST_CAPTURE_INFO_CMD_JSON = R"({
    "Type": "OPERATION",
    "dhId": "camrea_0",
    "Command": "CAPTURE",
    "Value": [
        {"Width": 1920, "Height": 1080, "Format": 1,
        "DataSpace": 1, "IsCapture":true, "EncodeType": 1, "StreamType": 1,
        "CaptureSettings": [{"SettingType": 1, "SettingValue": "TestSetting"}]}
    ]
})";

const std::string TEST_CHANNEL_INFO_CMD_CONTINUE_JSON = R"({
    "Type": "OPERATION",
    "dhId": "camrea_0",
    "Command": "CHANNEL_NEG",
    "Value": {"SourceDevId": "TestDevId",
    "Detail": [{"DataSessionFlag": "dataContinue", "StreamType": 0}]}
})";

const std::string TEST_EVENT_CMD_JSON = R"({
    "Type": "MESSAGE",
    "dhId": "camrea_0",
    "Command": "STATE_NOTIFY",
    "Value": {"EventType": 1, "EventResult": 1, "EventContent": "TestContent"}
})";

const std::string TEST_INFO_CMD_JSON = R"({
    "Type": "OPERATION",
    "dhId": "camrea_0",
    "Command": "GET_INFO",
    "Value": {"State": 1}
})";

const std::string TEST_METADATA_SETTING_CMD_JSON = R"({
    "Type": "MESSAGE",
    "dhId": "camrea_0",
    "Command": "UPDATE_METADATA",
    "Value": [{"SettingType": 1, "SettingValue": "TestSetting"}]
})";

const std::string TEST_OPEN_INFO_CMD_JSON = R"({
    "Type": "OPERATION",
    "dhId": "camrea_0",
    "Command": "OPEN_CHANNEL",
    "Value": {"SourceDevId": "TestDevId" }
})";

std::vector<DCameraIndex> g_testCamIndex;

void DCameraSinkControllerTest::SetUpTestCase(void)
{
    GetLocalDeviceNetworkId(g_testDeviceIdController);
    g_testCamIndex.push_back(DCameraIndex(g_testDeviceIdController, TEST_DH_ID));
}

void DCameraSinkControllerTest::TearDownTestCase(void)
{
}

void DCameraSinkControllerTest::SetUp(void)
{
    accessControl_ = std::make_shared<DCameraSinkAccessControl>();
    controller_ = std::make_shared<DCameraSinkController>(accessControl_);

    controller_->channel_ = std::make_shared<MockCameraChannel>();
    controller_->operator_ = std::make_shared<MockCameraOperator>();
    controller_->output_ = std::make_shared<MockDCameraSinkOutput>(TEST_DH_ID, controller_->operator_);
    controller_->srcDevId_ = g_testDeviceIdController;
    controller_->dhId_ = TEST_DH_ID;
}

void DCameraSinkControllerTest::TearDown(void)
{
    accessControl_ = nullptr;
    controller_ = nullptr;
}

/**
 * @tc.name: dcamera_sink_controller_test_001
 * @tc.desc: Verify the Init and UnInit function.
 * @tc.type: FUNC
 * @tc.require: AR000GK6MV
 */
HWTEST_F(DCameraSinkControllerTest, dcamera_sink_controller_test_001, TestSize.Level1)
{
    int32_t ret = controller_->Init(g_testCamIndex);
    EXPECT_EQ(DCAMERA_OK, ret);
    EXPECT_EQ(true, controller_->isInit_);

    ret = controller_->UnInit();
    EXPECT_EQ(DCAMERA_OK, ret);
    EXPECT_EQ(false, controller_->isInit_);
}

/**
 * @tc.name: dcamera_sink_controller_test_002
 * @tc.desc: Verify the GetCameraInfo function.
 * @tc.type: FUNC
 * @tc.require: AR000GK6MT
 */
HWTEST_F(DCameraSinkControllerTest, dcamera_sink_controller_test_002, TestSize.Level1)
{
    DCameraInfoCmd cmd;
    cmd.value_ = std::make_shared<DCameraInfo>();
    int32_t ret = controller_->GetCameraInfo(cmd.value_);
    EXPECT_EQ(DCAMERA_OK, ret);
    EXPECT_EQ(DCAMERA_CHANNEL_STATE_DISCONNECTED, cmd.value_->state_);
}

/**
 * @tc.name: dcamera_sink_controller_test_003
 * @tc.desc: Verify the ChannelNeg function.
 * @tc.type: FUNC
 * @tc.require: AR000GK6MT
 */
HWTEST_F(DCameraSinkControllerTest, dcamera_sink_controller_test_003, TestSize.Level1)
{
    DCameraChannelInfoCmd cmd;
    cmd.Unmarshal(TEST_CHANNEL_INFO_CMD_CONTINUE_JSON);
    int32_t ret = controller_->ChannelNeg(cmd.value_);
    EXPECT_EQ(DCAMERA_OK, ret);
}

/**
 * @tc.name: dcamera_sink_controller_test_004
 * @tc.desc: Verify the StartCapture function.
 * @tc.type: FUNC
 * @tc.require: AR000GK6MU
 */
HWTEST_F(DCameraSinkControllerTest, dcamera_sink_controller_test_004, TestSize.Level1)
{
    DCameraCaptureInfoCmd cmd;
    cmd.Unmarshal(TEST_CAPTURE_INFO_CMD_JSON);
    int32_t ret = controller_->StartCapture(cmd.value_);
    EXPECT_EQ(DCAMERA_OK, ret);
}

/**
 * @tc.name: dcamera_sink_controller_test_005
 * @tc.desc: Verify the UpdateSettings function.
 * @tc.type: FUNC
 * @tc.require: AR000GK6N1
 */
HWTEST_F(DCameraSinkControllerTest, dcamera_sink_controller_test_005, TestSize.Level1)
{
    DCameraCaptureInfoCmd cmd;
    cmd.Unmarshal(TEST_CAPTURE_INFO_CMD_JSON);
    int32_t ret = controller_->StartCapture(cmd.value_);
    EXPECT_EQ(DCAMERA_OK, ret);

    DCameraMetadataSettingCmd cmdMetadata;
    cmdMetadata.Unmarshal(TEST_METADATA_SETTING_CMD_JSON);
    ret = controller_->UpdateSettings(cmdMetadata.value_);
    EXPECT_EQ(DCAMERA_OK, ret);
}

/**
 * @tc.name: dcamera_sink_controller_test_006
 * @tc.desc: Verify the StopCapture function.
 * @tc.type: FUNC
 * @tc.require: AR000GK6N1
 */
HWTEST_F(DCameraSinkControllerTest, dcamera_sink_controller_test_006, TestSize.Level1)
{
    DCameraCaptureInfoCmd cmd;
    cmd.Unmarshal(TEST_CAPTURE_INFO_CMD_JSON);
    int32_t ret = controller_->StartCapture(cmd.value_);
    EXPECT_EQ(DCAMERA_OK, ret);

    ret = controller_->StopCapture();
    EXPECT_EQ(DCAMERA_OK, ret);
}

/**
 * @tc.name: dcamera_sink_controller_test_007
 * @tc.desc: Verify the DCameraNotify function.
 * @tc.type: FUNC
 * @tc.require: AR000GK6MT
 */
HWTEST_F(DCameraSinkControllerTest, dcamera_sink_controller_test_007, TestSize.Level1)
{
    controller_->srcDevId_ = TEST_DEVICE_ID_EMPTY;

    DCameraEventCmd cmd;
    cmd.Unmarshal(TEST_EVENT_CMD_JSON);
    int32_t ret = controller_->DCameraNotify(cmd.value_);
    EXPECT_EQ(DCAMERA_BAD_VALUE, ret);
}

/**
 * @tc.name: dcamera_sink_controller_test_008
 * @tc.desc: Verify the DCameraNotify function.
 * @tc.type: FUNC
 * @tc.require: AR000GK6MT
 */
HWTEST_F(DCameraSinkControllerTest, dcamera_sink_controller_test_008, TestSize.Level1)
{
    DCameraEventCmd cmd;
    cmd.Unmarshal(TEST_EVENT_CMD_JSON);
    int32_t ret = controller_->DCameraNotify(cmd.value_);
    EXPECT_NE(DCAMERA_OK, ret);
}

/**
 * @tc.name: dcamera_sink_controller_test_009
 * @tc.desc: Verify the OnSessionState and GetCameraInfo function.
 * @tc.type: FUNC
 * @tc.require: AR000GK6MT
 */
HWTEST_F(DCameraSinkControllerTest, dcamera_sink_controller_test_009, TestSize.Level1)
{
    controller_->OnSessionState(DCAMERA_CHANNEL_STATE_CONNECTING);

    DCameraInfoCmd cmd;
    cmd.value_ = std::make_shared<DCameraInfo>();
    int32_t ret = controller_->GetCameraInfo(cmd.value_);
    EXPECT_EQ(DCAMERA_OK, ret);
    EXPECT_EQ(DCAMERA_CHANNEL_STATE_CONNECTING, cmd.value_->state_);
}

/**
 * @tc.name: dcamera_sink_controller_test_010
 * @tc.desc: Verify the OnSessionState function.
 * @tc.type: FUNC
 * @tc.require: AR000GK6N1
 */
HWTEST_F(DCameraSinkControllerTest, dcamera_sink_controller_test_010, TestSize.Level1)
{
    controller_->OnSessionState(DCAMERA_CHANNEL_STATE_CONNECTED);

    DCameraInfoCmd cmd;
    cmd.value_ = std::make_shared<DCameraInfo>();
    int32_t ret = controller_->GetCameraInfo(cmd.value_);
    EXPECT_EQ(DCAMERA_OK, ret);
    EXPECT_EQ(DCAMERA_CHANNEL_STATE_CONNECTED, cmd.value_->state_);
}

/**
 * @tc.name: dcamera_sink_controller_test_011
 * @tc.desc: Verify the OnSessionState function.
 * @tc.type: FUNC
 * @tc.require: AR000GK6N1
 */
HWTEST_F(DCameraSinkControllerTest, dcamera_sink_controller_test_011, TestSize.Level1)
{
    controller_->OnSessionState(DCAMERA_CHANNEL_STATE_DISCONNECTED);

    DCameraInfoCmd cmd;
    cmd.value_ = std::make_shared<DCameraInfo>();
    int32_t ret = controller_->GetCameraInfo(cmd.value_);
    EXPECT_EQ(DCAMERA_OK, ret);
    EXPECT_EQ(DCAMERA_CHANNEL_STATE_DISCONNECTED, cmd.value_->state_);
}

/**
 * @tc.name: dcamera_sink_controller_test_012
 * @tc.desc: Verify the OpenChannel function.
 * @tc.type: FUNC
 * @tc.require: AR000GK6N1
 */
HWTEST_F(DCameraSinkControllerTest, dcamera_sink_controller_test_012, TestSize.Level1)
{
    DCameraOpenInfoCmd cmd;
    cmd.Unmarshal(TEST_OPEN_INFO_CMD_JSON);
    int32_t ret = controller_->OpenChannel(cmd.value_);
    EXPECT_EQ(DCAMERA_OK, ret);
}

/**
 * @tc.name: dcamera_sink_controller_test_013
 * @tc.desc: Verify the OpenChannel function.
 * @tc.type: FUNC
 * @tc.require: AR000GK6N1
 */
HWTEST_F(DCameraSinkControllerTest, dcamera_sink_controller_test_013, TestSize.Level1)
{
    controller_->OnSessionState(DCAMERA_CHANNEL_STATE_CONNECTED);

    DCameraInfoCmd cmd;
    cmd.value_ = std::make_shared<DCameraInfo>();
    int32_t ret = controller_->GetCameraInfo(cmd.value_);
    EXPECT_EQ(DCAMERA_OK, ret);
    EXPECT_EQ(DCAMERA_CHANNEL_STATE_CONNECTED, cmd.value_->state_);

    DCameraOpenInfoCmd cmdOpenInfo;
    cmdOpenInfo.Unmarshal(TEST_OPEN_INFO_CMD_JSON);
    ret = controller_->OpenChannel(cmdOpenInfo.value_);
    EXPECT_EQ(DCAMERA_WRONG_STATE, ret);
}

/**
 * @tc.name: dcamera_sink_controller_test_014
 * @tc.desc: Verify the CloseChannel function.
 * @tc.type: FUNC
 * @tc.require: AR000GK6N1
 */
HWTEST_F(DCameraSinkControllerTest, dcamera_sink_controller_test_014, TestSize.Level1)
{
    int32_t ret = controller_->CloseChannel();
    EXPECT_EQ(DCAMERA_OK, ret);
}
} // namespace DistributedHardware
} // namespace OHOS