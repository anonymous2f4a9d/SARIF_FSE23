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

#include "dcamera_capture_info_cmd.h"
#include "dcamera_channel_info_cmd.h"
#include "dcamera_event_cmd.h"
#include "dcamera_info_cmd.h"
#include "dcamera_metadata_setting_cmd.h"
#include "dcamera_protocol.h"
#include "dcamera_utils_tools.h"
#include "distributed_camera_constants.h"
#include "distributed_camera_errno.h"

using namespace testing::ext;

namespace OHOS {
namespace DistributedHardware {
class DCameraProtocolTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

static std::string g_testDeviceId;
static const std::string TEST_CAPTURE_INFO_CMD_JSON = R"({
    "Type": "OPERATION",
    "dhId": "camrea_0",
    "Command": "CAPTURE",
    "Value": [
        {"Width": 1920, "Height": 1080, "Format": 1, "DataSpace": 1,
        "IsCapture":true, "EncodeType": 1, "StreamType": 1,
        "CaptureSettings": [{"SettingType": 1, "SettingValue": "TestSetting"}]}
    ]
})";

static const std::string TEST_CHANNEL_INFO_CMD_JSON = R"({
    "Type": "OPERATION",
    "dhId": "camrea_0",
    "Command": "CHANNEL_NEG",
    "Value": {"SourceDevId": "TestDevId", "Detail": [{"DataSessionFlag": "TestFlag", "StreamType": 1}]}
})";

static const std::string TEST_EVENT_CMD_JSON = R"({
    "Type": "MESSAGE",
    "dhId": "camrea_0",
    "Command": "STATE_NOTIFY",
    "Value": {"EventType": 1, "EventResult": 1, "EventContent": "TestContent"}
})";

static const std::string TEST_INFO_CMD_JSON = R"({
    "Type": "OPERATION",
    "dhId": "camrea_0",
    "Command": "GET_INFO",
    "Value": {"State": 1}
})";

static const std::string TEST_METADATA_SETTING_CMD_JSON = R"({
    "Type": "MESSAGE",
    "dhId": "camrea_0",
    "Command": "UPDATE_METADATA",
    "Value": [{"SettingType": 1, "SettingValue": "TestSetting"}]
})";

void DCameraProtocolTest::SetUpTestCase(void)
{
    GetLocalDeviceNetworkId(g_testDeviceId);
}

void DCameraProtocolTest::TearDownTestCase(void)
{
}

void DCameraProtocolTest::SetUp(void)
{
}

void DCameraProtocolTest::TearDown(void)
{
}

/**
 * @tc.name: dcamera_protocol_test_001
 * @tc.desc: Verify Get ServiceAbility.
 * @tc.type: FUNC
 * @tc.require: AR000GK6MH
 */
HWTEST_F(DCameraProtocolTest, dcamera_protocol_test_001, TestSize.Level1)
{
    int32_t ret = DCAMERA_OK;
    EXPECT_EQ(DCAMERA_OK, ret);
}

/**
 * @tc.name: dcamera_protocol_test_002
 * @tc.desc: Verify CaptureInfoCmd Json.
 * @tc.type: FUNC
 * @tc.require: AR000GK6MH
 */
HWTEST_F(DCameraProtocolTest, dcamera_protocol_test_002, TestSize.Level1)
{
    DCameraCaptureInfoCmd cmd;
    int32_t ret = cmd.Unmarshal(TEST_CAPTURE_INFO_CMD_JSON);
    EXPECT_EQ(DCAMERA_OK, ret);

    std::string jsonStr;
    ret = cmd.Marshal(jsonStr);
    EXPECT_EQ(DCAMERA_OK, ret);
}

/**
 * @tc.name: dcamera_protocol_test_003
 * @tc.desc: Verify ChannelInfoCmd Json.
 * @tc.type: FUNC
 * @tc.require: AR000GK6MI
 */
HWTEST_F(DCameraProtocolTest, dcamera_protocol_test_003, TestSize.Level1)
{
    DCameraChannelInfoCmd cmd;
    int32_t ret = cmd.Unmarshal(TEST_CHANNEL_INFO_CMD_JSON);
    EXPECT_EQ(DCAMERA_OK, ret);

    std::string jsonStr;
    ret = cmd.Marshal(jsonStr);
    EXPECT_EQ(DCAMERA_OK, ret);
}

/**
 * @tc.name: dcamera_protocol_test_004
 * @tc.desc: Verify EventCmd Json.
 * @tc.type: FUNC
 * @tc.require: AR000GK6MI
 */
HWTEST_F(DCameraProtocolTest, dcamera_protocol_test_004, TestSize.Level1)
{
    DCameraEventCmd cmd;
    int32_t ret = cmd.Unmarshal(TEST_EVENT_CMD_JSON);
    EXPECT_EQ(DCAMERA_OK, ret);

    std::string jsonStr;
    ret = cmd.Marshal(jsonStr);
    EXPECT_EQ(DCAMERA_OK, ret);
}

/**
 * @tc.name: dcamera_protocol_test_005
 * @tc.desc: Verify CameraInfoCmd Json.
 * @tc.type: FUNC
 * @tc.require: AR000GS3AA
 */
HWTEST_F(DCameraProtocolTest, dcamera_protocol_test_005, TestSize.Level1)
{
    DCameraInfoCmd cmd;
    int32_t ret = cmd.Unmarshal(TEST_INFO_CMD_JSON);
    EXPECT_EQ(DCAMERA_OK, ret);

    std::string jsonStr;
    ret = cmd.Marshal(jsonStr);
    EXPECT_EQ(DCAMERA_OK, ret);
}

/**
 * @tc.name: dcamera_protocol_test_006
 * @tc.desc: Verify MetaDataSettingCmd Json.
 * @tc.type: FUNC
 * @tc.require: AR000GS3AA
 */
HWTEST_F(DCameraProtocolTest, dcamera_protocol_test_006, TestSize.Level1)
{
    DCameraMetadataSettingCmd cmd;
    int32_t ret = cmd.Unmarshal(TEST_METADATA_SETTING_CMD_JSON);
    EXPECT_EQ(DCAMERA_OK, ret);

    std::string jsonStr;
    ret = cmd.Marshal(jsonStr);
    EXPECT_EQ(DCAMERA_OK, ret);
}
} // namespace DistributedHardware
} // namespace OHOS
