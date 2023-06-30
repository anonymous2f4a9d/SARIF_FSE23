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

#include "anonymous_string.h"
#include "dcamera_client.h"
#include "distributed_camera_constants.h"
#include "distributed_camera_errno.h"
#include "distributed_hardware_log.h"

using namespace testing::ext;

namespace OHOS {
namespace DistributedHardware {
class DCameraClientTestStateCallback : public StateCallback {
public:
    void OnStateChanged(std::shared_ptr<DCameraEvent>& event)
    {
        DHLOGI("DCameraClientTestStateCallback::OnStateChanged type: %d, result: %d",
               event->eventType_, event->eventResult_);
    }

    void OnMetadataResult()
    {
        DHLOGI("DCameraClientTestStateCallback::OnMetadataResult");
    }
};

class DCameraClientTestResultCallback : public ResultCallback {
public:
    void OnPhotoResult(std::shared_ptr<DataBuffer>& buffer)
    {
        DHLOGI("DCameraClientTestResultCallback::OnPhotoResult");
    }

    void OnVideoResult(std::shared_ptr<DataBuffer>& buffer)
    {
        DHLOGI("DCameraClientTestResultCallback::OnVideoResult");
    }
};

const int32_t TEST_WIDTH = 1920;
const int32_t TEST_HEIGHT = 1080;
const int32_t TEST_FORMAT_3 = 3;
const int32_t TEST_FORMAT_4 = 4;
const int32_t TEST_SLEEP_SEC = 2;
const std::string TEST_CAMERA_ID = "Camera_device@3.5/legacy/1";

class DCameraClientTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

    std::shared_ptr<DCameraClient> client_;
    std::shared_ptr<DCameraCaptureInfo> photoInfo_false_;
    std::shared_ptr<DCameraCaptureInfo> videoInfo_false_;
    std::shared_ptr<DCameraCaptureInfo> photoInfo_true_;
    std::shared_ptr<DCameraCaptureInfo> videoInfo_true_;
};

void DCameraClientTest::SetUpTestCase(void)
{
    DHLOGI("DCameraClientTest::SetUpTestCase");
}

void DCameraClientTest::TearDownTestCase(void)
{
    DHLOGI("DCameraClientTest::TearDownTestCase");
}

void DCameraClientTest::SetUp(void)
{
    DHLOGI("DCameraClientTest::SetUp");
    client_ = std::make_shared<DCameraClient>(TEST_CAMERA_ID);

    photoInfo_false_ = std::make_shared<DCameraCaptureInfo>();
    photoInfo_false_->width_ = TEST_WIDTH;
    photoInfo_false_->height_ = TEST_HEIGHT;
    photoInfo_false_->format_ = TEST_FORMAT_4;
    photoInfo_false_->isCapture_ = false;
    photoInfo_false_->streamType_ = SNAPSHOT_FRAME;

    videoInfo_false_ = std::make_shared<DCameraCaptureInfo>();
    videoInfo_false_->width_ = TEST_WIDTH;
    videoInfo_false_->height_ = TEST_HEIGHT;
    videoInfo_false_->format_ = TEST_FORMAT_3;
    videoInfo_false_->isCapture_ = false;
    videoInfo_false_->streamType_ = CONTINUOUS_FRAME;

    photoInfo_true_ = std::make_shared<DCameraCaptureInfo>();
    photoInfo_true_->width_ = TEST_WIDTH;
    photoInfo_true_->height_ = TEST_HEIGHT;
    photoInfo_true_->format_ = TEST_FORMAT_4;
    photoInfo_true_->isCapture_ = true;
    photoInfo_true_->streamType_ = SNAPSHOT_FRAME;

    videoInfo_true_ = std::make_shared<DCameraCaptureInfo>();
    videoInfo_true_->width_ = TEST_WIDTH;
    videoInfo_true_->height_ = TEST_HEIGHT;
    videoInfo_true_->format_ = TEST_FORMAT_3;
    videoInfo_true_->isCapture_ = true;
    videoInfo_true_->streamType_ = CONTINUOUS_FRAME;
}

void DCameraClientTest::TearDown(void)
{
    DHLOGI("DCameraClientTest::TearDown");
    client_ = nullptr;
    photoInfo_false_ = nullptr;
    videoInfo_false_ = nullptr;
    photoInfo_true_ = nullptr;
    videoInfo_true_ = nullptr;
}

/**
 * @tc.name: dcamera_client_test_001
 * @tc.desc: Verify SetStateCallback
 * @tc.type: FUNC
 * @tc.require: AR000GK6MN
 */
HWTEST_F(DCameraClientTest, dcamera_client_test_001, TestSize.Level1)
{
    DHLOGI("DCameraClientTest dcamera_client_test_001: test set state callback");
    std::shared_ptr<StateCallback> stateCallback = std::make_shared<DCameraClientTestStateCallback>();
    int32_t ret = client_->SetStateCallback(stateCallback);
    EXPECT_EQ(DCAMERA_OK, ret);
}

/**
 * @tc.name: dcamera_client_test_002
 * @tc.desc: Verify SetResultCallback
 * @tc.type: FUNC
 * @tc.require: AR000GK6MN
 */
HWTEST_F(DCameraClientTest, dcamera_client_test_002, TestSize.Level1)
{
    DHLOGI("DCameraClientTest dcamera_client_test_002: test set result callback");
    std::shared_ptr<ResultCallback> resultCallback = std::make_shared<DCameraClientTestResultCallback>();
    int32_t ret = client_->SetResultCallback(resultCallback);
    EXPECT_EQ(DCAMERA_OK, ret);
}

/**
 * @tc.name: dcamera_client_test_003
 * @tc.desc: Verify Init and UnInit
 * @tc.type: FUNC
 * @tc.require: AR000GK6ML
 */
HWTEST_F(DCameraClientTest, dcamera_client_test_003, TestSize.Level1)
{
    DHLOGI("DCameraClientTest dcamera_client_test_003: test init and release client");
    std::shared_ptr<StateCallback> stateCallback = std::make_shared<DCameraClientTestStateCallback>();
    int32_t ret = client_->SetStateCallback(stateCallback);
    EXPECT_EQ(DCAMERA_OK, ret);

    std::shared_ptr<ResultCallback> resultCallback = std::make_shared<DCameraClientTestResultCallback>();
    ret = client_->SetResultCallback(resultCallback);
    EXPECT_EQ(DCAMERA_OK, ret);

    ret = client_->Init();
    EXPECT_EQ(DCAMERA_OK, ret);

    ret = client_->UnInit();
    EXPECT_EQ(DCAMERA_OK, ret);
}

/**
 * @tc.name: dcamera_client_test_004
 * @tc.desc: Verify StartCapture and StopCapture
 * @tc.type: FUNC
 * @tc.require: AR000GK6ML
 */
HWTEST_F(DCameraClientTest, dcamera_client_test_004, TestSize.Level1)
{
    DHLOGI("DCameraClientTest dcamera_client_test_004: test startCapture and stopCapture");
    std::shared_ptr<StateCallback> stateCallback = std::make_shared<DCameraClientTestStateCallback>();
    int32_t ret = client_->SetStateCallback(stateCallback);
    EXPECT_EQ(DCAMERA_OK, ret);

    std::shared_ptr<ResultCallback> resultCallback = std::make_shared<DCameraClientTestResultCallback>();
    ret = client_->SetResultCallback(resultCallback);
    EXPECT_EQ(DCAMERA_OK, ret);

    ret = client_->Init();
    EXPECT_EQ(DCAMERA_OK, ret);

    DHLOGI("DCameraClientTest dcamera_client_test_004: video width: %d, height: %d, format: %d, isCapture: %d",
        videoInfo_true_->width_, videoInfo_true_->height_, videoInfo_true_->format_, videoInfo_true_->isCapture_);
    std::vector<std::shared_ptr<DCameraCaptureInfo>> captureInfos;
    captureInfos.push_back(videoInfo_true_);
    ret = client_->StartCapture(captureInfos);
    EXPECT_EQ(DCAMERA_OK, ret);

    sleep(TEST_SLEEP_SEC);

    ret = client_->StopCapture();
    EXPECT_EQ(DCAMERA_OK, ret);

    ret = client_->UnInit();
    EXPECT_EQ(DCAMERA_OK, ret);
}

/**
 * @tc.name: dcamera_client_test_005
 * @tc.desc: Verify StartCapture and StopCapture
 * @tc.type: FUNC
 * @tc.require: AR000GK6ML
 */
HWTEST_F(DCameraClientTest, dcamera_client_test_005, TestSize.Level1)
{
    DHLOGI("DCameraClientTest dcamera_client_test_005: test startCapture and stopCapture");
    std::shared_ptr<StateCallback> stateCallback = std::make_shared<DCameraClientTestStateCallback>();
    int32_t ret = client_->SetStateCallback(stateCallback);
    EXPECT_EQ(DCAMERA_OK, ret);

    std::shared_ptr<ResultCallback> resultCallback = std::make_shared<DCameraClientTestResultCallback>();
    ret = client_->SetResultCallback(resultCallback);
    EXPECT_EQ(DCAMERA_OK, ret);

    ret = client_->Init();
    EXPECT_EQ(DCAMERA_OK, ret);

    DHLOGI("DCameraClientTest dcamera_client_test_005: video width: %d, height: %d, format: %d, isCapture: %d",
        videoInfo_true_->width_, videoInfo_true_->height_, videoInfo_true_->format_, videoInfo_true_->isCapture_);
    DHLOGI("DCameraClientTest dcamera_client_test_005: photo width: %d, height: %d, format: %d, isCapture: %d",
        photoInfo_false_->width_, photoInfo_false_->height_, photoInfo_false_->format_, photoInfo_false_->isCapture_);
    std::vector<std::shared_ptr<DCameraCaptureInfo>> captureInfos;
    captureInfos.push_back(videoInfo_true_);
    captureInfos.push_back(photoInfo_false_);
    ret = client_->StartCapture(captureInfos);
    EXPECT_EQ(DCAMERA_OK, ret);

    sleep(TEST_SLEEP_SEC);

    DHLOGI("DCameraClientTest dcamera_client_test_005: video width: %d, height: %d, format: %d, isCapture: %d",
        videoInfo_false_->width_, videoInfo_false_->height_, videoInfo_false_->format_, videoInfo_false_->isCapture_);
    DHLOGI("DCameraClientTest dcamera_client_test_005: photo width: %d, height: %d, format: %d, isCapture: %d",
        photoInfo_true_->width_, photoInfo_true_->height_, photoInfo_true_->format_, photoInfo_true_->isCapture_);
    captureInfos.clear();
    captureInfos.push_back(videoInfo_false_);
    captureInfos.push_back(photoInfo_true_);
    ret = client_->StartCapture(captureInfos);
    EXPECT_EQ(DCAMERA_OK, ret);

    sleep(TEST_SLEEP_SEC);

    ret = client_->StopCapture();
    EXPECT_EQ(DCAMERA_OK, ret);

    ret = client_->UnInit();
    EXPECT_EQ(DCAMERA_OK, ret);
}
} // namespace DistributedHardware
} // namespace OHOS