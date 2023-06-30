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
#include "dcamera_source_state.h"
#undef private

#include "dcamera_source_capture_state.h"
#include "dcamera_source_config_stream_state.h"
#include "dcamera_source_init_state.h"
#include "dcamera_source_opened_state.h"
#include "dcamera_source_regist_state.h"
#include "dcamera_source_state_factory.h"
#include "dcamera_source_state_machine.h"
#include "mock_dcamera_source_dev.h"
#include "mock_dcamera_source_state_listener.h"

#include "anonymous_string.h"
#include "distributed_camera_constants.h"
#include "distributed_camera_errno.h"
#include "distributed_hardware_log.h"

using namespace testing::ext;

namespace OHOS {
namespace DistributedHardware {
class DCameraSourceStateMachineTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

    std::shared_ptr<DCameraSourceDev> camDev_;
    std::shared_ptr<ICameraStateListener> stateListener_;
    std::shared_ptr<DCameraSourceStateMachine> stateMachine_;

private:
    static void SetStreamInfos();
    static void SetCaptureInfos();
};

namespace {
const std::string TEST_DEVICE_ID = "bb536a637105409e904d4da83790a4a7";
const std::string TEST_CAMERA_DH_ID_0 = "camera_0";
const std::string TEST_REQID = "0xFFFF";
const std::string TEST_PARAM = "0xFFFF";
const int32_t TEST_WIDTH = 1920;
const int32_t TEST_HEIGTH = 1080;
const int32_t TEST_STREAMID = 2;
}

std::vector<std::shared_ptr<DCStreamInfo>> g_streamInfosSnap;
std::vector<std::shared_ptr<DCCaptureInfo>> g_captureInfoSnap;
std::vector<std::shared_ptr<DCameraSettings>> g_cameraSettingSnap;
std::vector<int> g_streamIdSnap;
std::shared_ptr<DCameraEvent> g_camEvent;
std::shared_ptr<DCameraRegistParam> g_registParam;
DCameraIndex g_camIndex;

void DCameraSourceStateMachineTest::SetUpTestCase(void)
{
    SetStreamInfos();
    SetCaptureInfos();
}

void DCameraSourceStateMachineTest::SetStreamInfos()
{
    g_camIndex.devId_ = TEST_DEVICE_ID;
    g_camIndex.dhId_ = TEST_CAMERA_DH_ID_0;
    g_camEvent = std::make_shared<DCameraEvent>();
    std::shared_ptr<DCStreamInfo> streamInfo1 = std::make_shared<DCStreamInfo>();
    streamInfo1->streamId_ = 1;
    streamInfo1->width_ = TEST_WIDTH;
    streamInfo1->height_ = TEST_HEIGTH;
    streamInfo1->stride_ = 1;
    streamInfo1->format_ = 1;
    streamInfo1->dataspace_ = 1;
    streamInfo1->encodeType_ = ENCODE_TYPE_JPEG;
    streamInfo1->type_ = SNAPSHOT_FRAME;

    std::shared_ptr<DCStreamInfo> streamInfo2 = std::make_shared<DCStreamInfo>();
    streamInfo2->streamId_ = TEST_STREAMID;
    streamInfo2->width_ = TEST_WIDTH;
    streamInfo2->height_ = TEST_HEIGTH;
    streamInfo2->stride_ = 1;
    streamInfo2->format_ = 1;
    streamInfo2->dataspace_ = 1;
    streamInfo2->encodeType_ = ENCODE_TYPE_JPEG;
    streamInfo2->type_ = SNAPSHOT_FRAME;
    g_streamInfosSnap.push_back(streamInfo1);
    g_streamInfosSnap.push_back(streamInfo2);
}

void DCameraSourceStateMachineTest::SetCaptureInfos()
{
    std::shared_ptr<DCCaptureInfo> captureInfo1 = std::make_shared<DCCaptureInfo>();
    captureInfo1->streamIds_.push_back(1);
    captureInfo1->width_ = TEST_WIDTH;
    captureInfo1->height_ = TEST_HEIGTH;
    captureInfo1->stride_ = 1;
    captureInfo1->format_ = 1;
    captureInfo1->dataspace_ = 1;
    captureInfo1->encodeType_ = ENCODE_TYPE_H265;
    captureInfo1->type_ = CONTINUOUS_FRAME;

    std::shared_ptr<DCCaptureInfo> captureInfo2 = std::make_shared<DCCaptureInfo>();
    captureInfo2->streamIds_.push_back(1);
    captureInfo2->width_ = TEST_WIDTH;
    captureInfo2->height_ = TEST_HEIGTH;
    captureInfo2->stride_ = 1;
    captureInfo2->format_ = 1;
    captureInfo2->dataspace_ = 1;
    captureInfo2->encodeType_ = ENCODE_TYPE_H265;
    captureInfo2->type_ = CONTINUOUS_FRAME;
    g_captureInfoSnap.push_back(captureInfo1);
    g_captureInfoSnap.push_back(captureInfo2);

    std::shared_ptr<DCameraSettings> camSettings1 = std::make_shared<DCameraSettings>();
    camSettings1->type_ = UPDATE_METADATA;
    camSettings1->value_ = "SettingValue";

    std::shared_ptr<DCameraSettings> camSettings2 = std::make_shared<DCameraSettings>();
    camSettings2->type_ = ENABLE_METADATA;
    camSettings2->value_ = "SettingValue";
    g_cameraSettingSnap.push_back(camSettings1);
    g_cameraSettingSnap.push_back(camSettings2);

    g_streamIdSnap.push_back(1);
    g_streamIdSnap.push_back(TEST_STREAMID);

    g_registParam = std::make_shared<DCameraRegistParam>(TEST_DEVICE_ID, TEST_CAMERA_DH_ID_0, TEST_REQID, TEST_PARAM);
}

void DCameraSourceStateMachineTest::TearDownTestCase(void)
{
}

void DCameraSourceStateMachineTest::SetUp(void)
{
    stateListener_ = std::make_shared<MockDCameraSourceStateListener>();
    camDev_ = std::make_shared<MockDCameraSourceDev>(TEST_DEVICE_ID, TEST_CAMERA_DH_ID_0, stateListener_);
    stateMachine_ = std::make_shared<DCameraSourceStateMachine>(camDev_);
}

void DCameraSourceStateMachineTest::TearDown(void)
{
    stateMachine_ = nullptr;
    camDev_ = nullptr;
    stateListener_ = nullptr;
}

/**
 * @tc.name: dcamera_source_state_machine_test_001
 * @tc.desc: Verify source init state.
 * @tc.type: FUNC
 * @tc.require: AR000GK6MC
 */
HWTEST_F(DCameraSourceStateMachineTest, dcamera_source_state_machine_test_001, TestSize.Level1)
{
    DCameraSourceEvent event0(*camDev_, DCAMERA_EVENT_REGIST, g_registParam);
    DCameraSourceEvent event1(*camDev_, DCAMERA_EVENT_UNREGIST, g_registParam);
    stateMachine_ ->UpdateState(DCAMERA_STATE_INIT);
    int32_t ret = stateMachine_ ->Execute(DCAMERA_EVENT_REGIST, event0);
    EXPECT_EQ(DCAMERA_OK, ret);

    ret = stateMachine_ ->Execute(DCAMERA_EVENT_UNREGIST, event1);
    EXPECT_EQ(DCAMERA_OK, ret);
}

/**
 * @tc.name: dcamera_source_state_machine_test_002
 * @tc.desc: Verify source regist state.
 * @tc.type: FUNC
 * @tc.require: AR000GK6MC
 */
HWTEST_F(DCameraSourceStateMachineTest, dcamera_source_state_machine_test_002, TestSize.Level1)
{
    DCameraSourceEvent event0(*camDev_, DCAMERA_EVENT_REGIST, g_registParam);
    DCameraSourceEvent event1(*camDev_, DCAMERA_EVENT_UNREGIST, g_registParam);
    DCameraSourceEvent event2(*camDev_, DCAMERA_EVENT_OPEN, g_camIndex);
    DCameraSourceEvent event3(*camDev_, DCAMERA_EVENT_CLOSE, g_camIndex);
    DCameraSourceEvent event9(*camDev_, DCAMERA_EVENT_NOFIFY, g_camEvent);
    stateMachine_ ->UpdateState(DCAMERA_STATE_INIT);
    stateMachine_ ->UpdateState(DCAMERA_STATE_REGIST);
    int32_t ret = stateMachine_ ->Execute(DCAMERA_EVENT_REGIST, event0);
    EXPECT_EQ(DCAMERA_OK, ret);

    ret = stateMachine_ ->Execute(DCAMERA_EVENT_OPEN, event2);
    EXPECT_EQ(DCAMERA_OK, ret);

    ret = stateMachine_ ->Execute(DCAMERA_EVENT_CLOSE, event3);
    EXPECT_EQ(DCAMERA_OK, ret);

    ret = stateMachine_ ->Execute(DCAMERA_EVENT_NOFIFY, event9);
    EXPECT_EQ(DCAMERA_OK, ret);

    ret = stateMachine_ ->Execute(DCAMERA_EVENT_UNREGIST, event1);
    EXPECT_EQ(DCAMERA_OK, ret);
}

/**
 * @tc.name: dcamera_source_state_machine_test_003
 * @tc.desc: Verify source open state.
 * @tc.type: FUNC
 * @tc.require: AR000GK6MD
 */
HWTEST_F(DCameraSourceStateMachineTest, dcamera_source_state_machine_test_003, TestSize.Level1)
{
    DCameraSourceEvent event0(*camDev_, DCAMERA_EVENT_REGIST, g_registParam);
    DCameraSourceEvent event1(*camDev_, DCAMERA_EVENT_UNREGIST, g_registParam);
    DCameraSourceEvent event2(*camDev_, DCAMERA_EVENT_OPEN, g_camIndex);
    DCameraSourceEvent event3(*camDev_, DCAMERA_EVENT_CLOSE, g_camIndex);
    DCameraSourceEvent event4(*camDev_, DCAMERA_EVENT_CONFIG_STREAMS, g_streamInfosSnap);
    DCameraSourceEvent event5(*camDev_, DCAMERA_EVENT_RELEASE_STREAMS, g_streamIdSnap);
    DCameraSourceEvent event8(*camDev_, DCAMERA_EVENT_UPDATE_SETTINGS, g_cameraSettingSnap);
    DCameraSourceEvent event9(*camDev_, DCAMERA_EVENT_NOFIFY, g_camEvent);
    stateMachine_ ->UpdateState(DCAMERA_STATE_INIT);
    stateMachine_ ->UpdateState(DCAMERA_STATE_REGIST);
    stateMachine_ ->UpdateState(DCAMERA_STATE_OPENED);
    int32_t ret = stateMachine_ ->Execute(DCAMERA_EVENT_REGIST, event0);
    EXPECT_EQ(DCAMERA_OK, ret);

    ret = stateMachine_ ->Execute(DCAMERA_EVENT_OPEN, event2);
    EXPECT_EQ(DCAMERA_OK, ret);

    ret = stateMachine_ ->Execute(DCAMERA_EVENT_CONFIG_STREAMS, event4);
    EXPECT_EQ(DCAMERA_OK, ret);

    ret = stateMachine_ ->Execute(DCAMERA_EVENT_RELEASE_STREAMS, event5);
    EXPECT_EQ(DCAMERA_OK, ret);

    ret = stateMachine_ ->Execute(DCAMERA_EVENT_NOFIFY, event9);
    EXPECT_EQ(DCAMERA_OK, ret);

    ret = stateMachine_ ->Execute(DCAMERA_EVENT_UPDATE_SETTINGS, event8);
    EXPECT_EQ(DCAMERA_OK, ret);

    ret = stateMachine_ ->Execute(DCAMERA_EVENT_CLOSE, event3);
    EXPECT_EQ(DCAMERA_OK, ret);

    ret = stateMachine_ ->Execute(DCAMERA_EVENT_UNREGIST, event1);
    EXPECT_EQ(DCAMERA_OK, ret);
}

/**
 * @tc.name: dcamera_source_state_machine_test_004
 * @tc.desc: Verify source config Stream state.
 * @tc.type: FUNC
 * @tc.require: AR000GK6ME
 */
HWTEST_F(DCameraSourceStateMachineTest, dcamera_source_state_machine_test_004, TestSize.Level1)
{
    DCameraSourceEvent event0(*camDev_, DCAMERA_EVENT_REGIST, g_registParam);
    DCameraSourceEvent event1(*camDev_, DCAMERA_EVENT_UNREGIST, g_registParam);
    DCameraSourceEvent event2(*camDev_, DCAMERA_EVENT_OPEN, g_camIndex);
    DCameraSourceEvent event3(*camDev_, DCAMERA_EVENT_CLOSE, g_camIndex);
    DCameraSourceEvent event4(*camDev_, DCAMERA_EVENT_CONFIG_STREAMS, g_streamInfosSnap);
    DCameraSourceEvent event5(*camDev_, DCAMERA_EVENT_RELEASE_STREAMS, g_streamIdSnap);
    DCameraSourceEvent event6(*camDev_, DCAMERA_EVENT_START_CAPTURE, g_captureInfoSnap);
    DCameraSourceEvent event7(*camDev_, DCAMERA_EVENT_STOP_CAPTURE);
    DCameraSourceEvent event8(*camDev_, DCAMERA_EVENT_UPDATE_SETTINGS, g_cameraSettingSnap);
    DCameraSourceEvent event9(*camDev_, DCAMERA_EVENT_NOFIFY, g_camEvent);
    stateMachine_ ->UpdateState(DCAMERA_STATE_INIT);
    stateMachine_ ->UpdateState(DCAMERA_STATE_REGIST);
    stateMachine_ ->UpdateState(DCAMERA_STATE_OPENED);
    stateMachine_ ->UpdateState(DCAMERA_STATE_CONFIG_STREAM);
    int32_t ret = stateMachine_ ->Execute(DCAMERA_EVENT_REGIST, event0);
    EXPECT_EQ(DCAMERA_OK, ret);

    ret = stateMachine_ ->Execute(DCAMERA_EVENT_OPEN, event2);
    EXPECT_EQ(DCAMERA_OK, ret);

    ret = stateMachine_ ->Execute(DCAMERA_EVENT_CONFIG_STREAMS, event4);
    EXPECT_EQ(DCAMERA_OK, ret);

    ret = stateMachine_ ->Execute(DCAMERA_EVENT_START_CAPTURE, event6);
    EXPECT_EQ(DCAMERA_OK, ret);

    ret = stateMachine_ ->Execute(DCAMERA_EVENT_NOFIFY, event9);
    EXPECT_EQ(DCAMERA_OK, ret);

    ret = stateMachine_ ->Execute(DCAMERA_EVENT_UPDATE_SETTINGS, event8);
    EXPECT_EQ(DCAMERA_OK, ret);

    ret = stateMachine_ ->Execute(DCAMERA_EVENT_STOP_CAPTURE, event7);
    EXPECT_EQ(DCAMERA_OK, ret);

    ret = stateMachine_ ->Execute(DCAMERA_EVENT_RELEASE_STREAMS, event5);
    EXPECT_EQ(DCAMERA_OK, ret);

    ret = stateMachine_ ->Execute(DCAMERA_EVENT_CLOSE, event3);
    EXPECT_EQ(DCAMERA_OK, ret);

    ret = stateMachine_ ->Execute(DCAMERA_EVENT_UNREGIST, event1);
    EXPECT_EQ(DCAMERA_OK, ret);
}

/**
 * @tc.name: dcamera_source_state_machine_test_005
 * @tc.desc: Verify source capture state.
 * @tc.type: FUNC
 * @tc.require: AR000GK6ME
 */
HWTEST_F(DCameraSourceStateMachineTest, dcamera_source_state_machine_test_005, TestSize.Level1)
{
    DCameraSourceEvent event0(*camDev_, DCAMERA_EVENT_REGIST, g_registParam);
    DCameraSourceEvent event1(*camDev_, DCAMERA_EVENT_UNREGIST, g_registParam);
    DCameraSourceEvent event2(*camDev_, DCAMERA_EVENT_OPEN, g_camIndex);
    DCameraSourceEvent event3(*camDev_, DCAMERA_EVENT_CLOSE, g_camIndex);
    DCameraSourceEvent event6(*camDev_, DCAMERA_EVENT_START_CAPTURE, g_captureInfoSnap);
    DCameraSourceEvent event7(*camDev_, DCAMERA_EVENT_STOP_CAPTURE);
    DCameraSourceEvent event8(*camDev_, DCAMERA_EVENT_UPDATE_SETTINGS, g_cameraSettingSnap);
    DCameraSourceEvent event9(*camDev_, DCAMERA_EVENT_NOFIFY, g_camEvent);
    stateMachine_ ->UpdateState(DCAMERA_STATE_INIT);
    stateMachine_ ->UpdateState(DCAMERA_STATE_REGIST);
    stateMachine_ ->UpdateState(DCAMERA_STATE_OPENED);
    stateMachine_ ->UpdateState(DCAMERA_STATE_CONFIG_STREAM);
    stateMachine_ ->UpdateState(DCAMERA_STATE_CAPTURE);
    int32_t ret = stateMachine_ ->Execute(DCAMERA_EVENT_REGIST, event0);
    EXPECT_EQ(DCAMERA_OK, ret);

    ret = stateMachine_ ->Execute(DCAMERA_EVENT_OPEN, event2);
    EXPECT_EQ(DCAMERA_OK, ret);

    ret = stateMachine_ ->Execute(DCAMERA_EVENT_START_CAPTURE, event6);
    EXPECT_EQ(DCAMERA_OK, ret);

    ret = stateMachine_ ->Execute(DCAMERA_EVENT_NOFIFY, event9);
    EXPECT_EQ(DCAMERA_OK, ret);

    ret = stateMachine_ ->Execute(DCAMERA_EVENT_UPDATE_SETTINGS, event8);
    EXPECT_EQ(DCAMERA_OK, ret);

    ret = stateMachine_ ->Execute(DCAMERA_EVENT_STOP_CAPTURE, event7);
    EXPECT_EQ(DCAMERA_OK, ret);

    ret = stateMachine_ ->Execute(DCAMERA_EVENT_CLOSE, event3);
    EXPECT_EQ(DCAMERA_OK, ret);

    ret = stateMachine_ ->Execute(DCAMERA_EVENT_UNREGIST, event1);
    EXPECT_EQ(DCAMERA_OK, ret);
}
} // namespace DistributedHardware
} // namespace OHOS
