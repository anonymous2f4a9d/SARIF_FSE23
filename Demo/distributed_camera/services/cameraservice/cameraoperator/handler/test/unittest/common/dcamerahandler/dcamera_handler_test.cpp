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

#include "dcamera_handler.h"
#include "distributed_camera_errno.h"
#include "distributed_hardware_log.h"

using namespace testing::ext;

namespace OHOS {
namespace DistributedHardware {
class DCameraHandlerTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void DCameraHandlerTest::SetUpTestCase(void)
{
    DHLOGI("DCameraHandlerTest::SetUpTestCase");
}

void DCameraHandlerTest::TearDownTestCase(void)
{
    DHLOGI("DCameraHandlerTest::TearDownTestCase");
}

void DCameraHandlerTest::SetUp(void)
{
    DHLOGI("DCameraHandlerTest::SetUp");
}

void DCameraHandlerTest::TearDown(void)
{
    DHLOGI("DCameraHandlerTest::TearDown");
}

/**
 * @tc.name: dcamera_handler_test_001
 * @tc.desc: Verify Initialize
 * @tc.type: FUNC
 * @tc.require: AR000GK6MF
 */
HWTEST_F(DCameraHandlerTest, dcamera_handler_test_001, TestSize.Level1)
{
    int32_t ret = DCameraHandler::GetInstance().Initialize();
    EXPECT_EQ(ret, DCAMERA_OK);
}

/**
 * @tc.name: dcamera_handler_test_002
 * @tc.desc: Verify GetCameras
 * @tc.type: FUNC
 * @tc.require: AR000GK6MF
 */
HWTEST_F(DCameraHandlerTest, dcamera_handler_test_002, TestSize.Level1)
{
    int32_t ret = DCameraHandler::GetInstance().GetCameras().size();
    EXPECT_GT(ret, DCAMERA_OK);
}

/**
 * @tc.name: dcamera_handler_test_003
 * @tc.desc: Verify Query
 * @tc.type: FUNC
 * @tc.require: AR000GK6MF
 */
HWTEST_F(DCameraHandlerTest, dcamera_handler_test_003, TestSize.Level1)
{
    int32_t ret = DCameraHandler::GetInstance().Query().size();
    EXPECT_GT(ret, DCAMERA_OK);
}
} // namespace DistributedHardware
} // namespace OHOS