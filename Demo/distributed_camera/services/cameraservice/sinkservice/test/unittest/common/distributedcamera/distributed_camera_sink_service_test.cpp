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
#include "distributed_camera_sink_service.h"
#undef private

#include "distributed_camera_errno.h"

using namespace testing::ext;

namespace OHOS {
namespace DistributedHardware {
class DistributedCameraSinkServiceTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

    std::shared_ptr<DistributedCameraSinkService> service_;
};
std::string g_testParams = "";
std::string g_testCameraInfo = "";
const int32_t TEST_SA_ID = 4804;
const bool TEST_RUN_ON_CREATE = true;

std::string g_testChannelInfoContinue = R"({
    "Type": "OPERATION",
    "dhId": "camrea_0",
    "Command": "CHANNEL_NEG",
    "Value": {"SourceDevId": "TestDevId",
    "Detail": [{"DataSessionFlag": "dataContinue", "StreamType": 0}]}
})";

std::string g_testOpenInfoService = R"({
    "Type": "OPERATION",
    "dhId": "camrea_0",
    "Command": "OPEN_CHANNEL",
    "Value": {"SourceDevId": "TestDevId"}
})";

void DistributedCameraSinkServiceTest::SetUpTestCase(void)
{
}

void DistributedCameraSinkServiceTest::TearDownTestCase(void)
{
}

void DistributedCameraSinkServiceTest::SetUp(void)
{
    service_ = std::make_shared<DistributedCameraSinkService>(TEST_SA_ID, TEST_RUN_ON_CREATE);
}

void DistributedCameraSinkServiceTest::TearDown(void)
{
    service_ = nullptr;
}

/**
 * @tc.name: dcamera_sink_service_test_001
 * @tc.desc: Verify the InitSink and ReleaseSink function.
 * @tc.type: FUNC
 * @tc.require: AR000GK6MV
 */
HWTEST_F(DistributedCameraSinkServiceTest, dcamera_sink_service_test_001, TestSize.Level1)
{
    int32_t ret = service_->InitSink(g_testParams);
    EXPECT_EQ(DCAMERA_OK, ret);
    EXPECT_EQ(g_testParams, service_->sinkVer_);
    EXPECT_NE(0, (int32_t)service_->camerasMap_.size());

    ret = service_->ReleaseSink();
    EXPECT_EQ(DCAMERA_OK, ret);
    EXPECT_EQ(0, (int32_t)service_->camerasMap_.size());
}

/**
 * @tc.name: dcamera_sink_service_test_002
 * @tc.desc: Verify the SubscribeLocalHardware function.
 * @tc.type: FUNC
 * @tc.require: AR000GK6MT
 */
HWTEST_F(DistributedCameraSinkServiceTest, dcamera_sink_service_test_002, TestSize.Level1)
{
    int32_t ret = service_->InitSink(g_testParams);
    EXPECT_EQ(DCAMERA_OK, ret);

    ret = service_->SubscribeLocalHardware(service_->camerasMap_.begin()->first, g_testParams);
    EXPECT_EQ(DCAMERA_OK, ret);

    ret = service_->ReleaseSink();
    EXPECT_EQ(DCAMERA_OK, ret);
}

/**
 * @tc.name: dcamera_sink_service_test_003
 * @tc.desc: Verify the UnSubscribeLocalHardware function.
 * @tc.type: FUNC
 * @tc.require: AR000GK6MT
 */
HWTEST_F(DistributedCameraSinkServiceTest, dcamera_sink_service_test_003, TestSize.Level1)
{
    int32_t ret = service_->InitSink(g_testParams);
    EXPECT_EQ(DCAMERA_OK, ret);

    ret = service_->UnsubscribeLocalHardware(service_->camerasMap_.begin()->first);
    EXPECT_EQ(DCAMERA_OK, ret);

    ret = service_->ReleaseSink();
    EXPECT_EQ(DCAMERA_OK, ret);
}

/**
 * @tc.name: dcamera_sink_service_test_004
 * @tc.desc: Verify the StopCapture function.
 * @tc.type: FUNC
 * @tc.require: AR000GK6N1
 */
HWTEST_F(DistributedCameraSinkServiceTest, dcamera_sink_service_test_004, TestSize.Level1)
{
    int32_t ret = service_->InitSink(g_testParams);
    EXPECT_EQ(DCAMERA_OK, ret);

    ret = service_->StopCapture(service_->camerasMap_.begin()->first);
    EXPECT_EQ(DCAMERA_OK, ret);

    ret = service_->ReleaseSink();
    EXPECT_EQ(DCAMERA_OK, ret);
}

/**
 * @tc.name: dcamera_sink_service_test_005
 * @tc.desc: Verify the ChannelNeg function.
 * @tc.type: FUNC
 * @tc.require: AR000GK6MT
 */
HWTEST_F(DistributedCameraSinkServiceTest, dcamera_sink_service_test_005, TestSize.Level1)
{
    int32_t ret = service_->InitSink(g_testParams);
    EXPECT_EQ(DCAMERA_OK, ret);

    ret = service_->ChannelNeg(service_->camerasMap_.begin()->first, g_testChannelInfoContinue);
    EXPECT_EQ(DCAMERA_OK, ret);

    ret = service_->ReleaseSink();
    EXPECT_EQ(DCAMERA_OK, ret);
}

/**
 * @tc.name: dcamera_sink_service_test_006
 * @tc.desc: Verify the GetCameraInfo function.
 * @tc.type: FUNC
 * @tc.require: AR000GK6MT
 */
HWTEST_F(DistributedCameraSinkServiceTest, dcamera_sink_service_test_006, TestSize.Level1)
{
    int32_t ret = service_->InitSink(g_testParams);
    EXPECT_EQ(DCAMERA_OK, ret);

    ret = service_->GetCameraInfo(service_->camerasMap_.begin()->first, g_testCameraInfo);
    EXPECT_EQ(DCAMERA_OK, ret);

    ret = service_->ReleaseSink();
    EXPECT_EQ(DCAMERA_OK, ret);
}

/**
 * @tc.name: dcamera_sink_service_test_007
 * @tc.desc: Verify the OpenChannel and CloseChannel function.
 * @tc.type: FUNC
 * @tc.require: AR000GK6N1
 */
HWTEST_F(DistributedCameraSinkServiceTest, dcamera_sink_service_test_007, TestSize.Level1)
{
    int32_t ret = service_->InitSink(g_testParams);
    EXPECT_EQ(DCAMERA_OK, ret);

    ret = service_->OpenChannel(service_->camerasMap_.begin()->first, g_testOpenInfoService);
    EXPECT_EQ(DCAMERA_OK, ret);

    ret = service_->CloseChannel(service_->camerasMap_.begin()->first);
    EXPECT_EQ(DCAMERA_OK, ret);

    ret = service_->ReleaseSink();
    EXPECT_EQ(DCAMERA_OK, ret);
}
} // namespace DistributedHardware
} // namespace OHOS