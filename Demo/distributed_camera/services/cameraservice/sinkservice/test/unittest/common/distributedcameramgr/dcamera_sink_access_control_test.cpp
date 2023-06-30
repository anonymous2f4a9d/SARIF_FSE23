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
#include "dcamera_sink_access_control.h"
#undef private

#include "distributed_camera_constants.h"
#include "distributed_camera_errno.h"

using namespace testing::ext;

namespace OHOS {
namespace DistributedHardware {
class DCameraSinkAccessControlTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

    std::shared_ptr<DCameraSinkAccessControl> accessControl_;
};
const std::string TEST_SRC_TYPE = "camera";
const std::string TEST_DEVICE_NAME = "";
const std::string TEST_ACCESS_TYPE = "";

void DCameraSinkAccessControlTest::SetUpTestCase(void)
{
}

void DCameraSinkAccessControlTest::TearDownTestCase(void)
{
}

void DCameraSinkAccessControlTest::SetUp(void)
{
    accessControl_ = std::make_shared<DCameraSinkAccessControl>();
}

void DCameraSinkAccessControlTest::TearDown(void)
{
    accessControl_ = nullptr;
}

/**
 * @tc.name: dcamera_sink_access_control_test_001
 * @tc.desc: Verify the IsSensitiveSrcAccess function.
 * @tc.type: FUNC
 * @tc.require: AR000GK6MU
 */
HWTEST_F(DCameraSinkAccessControlTest, dcamera_sink_access_control_test_001, TestSize.Level1)
{
    bool ret = accessControl_->IsSensitiveSrcAccess(TEST_SRC_TYPE);
    EXPECT_EQ(true, ret);
}

/**
 * @tc.name: dcamera_sink_access_control_test_002
 * @tc.desc: Verify the NotifySensitiveSrc function.
 * @tc.type: FUNC
 * @tc.require: AR000GK6MU
 */
HWTEST_F(DCameraSinkAccessControlTest, dcamera_sink_access_control_test_002, TestSize.Level1)
{
    bool ret = accessControl_->NotifySensitiveSrc(TEST_SRC_TYPE);
    EXPECT_EQ(true, ret);
}

/**
 * @tc.name: dcamera_sink_access_control_test_003
 * @tc.desc: Verify the GetAccessControlType function.
 * @tc.type: FUNC
 * @tc.require: AR000GK6MU
 */
HWTEST_F(DCameraSinkAccessControlTest, dcamera_sink_access_control_test_003, TestSize.Level1)
{
    int32_t ret = accessControl_->GetAccessControlType(TEST_ACCESS_TYPE);
    EXPECT_EQ(DCAMERA_SAME_ACCOUNT, ret);
}

/**
 * @tc.name: dcamera_sink_access_control_test_004
 * @tc.desc: Verify the TriggerFrame function.
 * @tc.type: FUNC
 * @tc.require: AR000GK6MU
 */
HWTEST_F(DCameraSinkAccessControlTest, dcamera_sink_access_control_test_004, TestSize.Level1)
{
    int32_t ret = accessControl_->TriggerFrame(TEST_DEVICE_NAME);
    EXPECT_EQ(DCAMERA_OK, ret);
}
} // namespace DistributedHardware
} // namespace OHOS