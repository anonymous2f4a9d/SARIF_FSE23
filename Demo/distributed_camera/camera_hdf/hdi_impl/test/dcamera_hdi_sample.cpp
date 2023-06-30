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

#include <iostream>
#include "common.h"
#include "dcamera_source_handler.h"

using namespace std;
using namespace OHOS::DistributedHardware;

const std::string TEST_DEV_ID = "11111111111111111111111111111111";
const std::string TEST_CAM_ID = "camera_0";
const std::string TEST_ATTR = R"({"CodecType":["avenc_mpeg4"],
    "OutputFormat":{"Photo":[4],"Preview":[2,3],"Video":[2,3]},
    "Position":"BACK",
    "ProtocolVer":"1.0",
    "MetaData":"",
    "Resolution":{
    "2":["1920*1080","1504*720","1440*1080","1280*960","1280*720","1232*768","1152*720","960*720","960*544",
    "880*720","720*720","720*480","640*480","352*288","320*240"],
    "3":["1920*1080","1504*720","1440*1080","1280*960","1280*720","1232*768","1152*720","960*720","960*544",
    "880*720","720*720","720*480","640*480","352*288","320*240"],
    "4":["3840*2160","3264*2448","3264*1840","2304*1728","2048*1536","1920*1440","1920*1080","1744*1088",
    "1280*720","1232*768","1152*720","640*480","320*240"]}})";

int main()
{
    cout << "distributed camera hdf start" << endl;

    IDistributedHardwareSource *sourceSA = GetSourceHardwareHandler();
    EnableParam param;
    param.version = "1.0";
    param.attrs = TEST_ATTR;
    std::shared_ptr<OHOS::DistributedHardware::RegisterCallback> regCb =
        std::make_shared<OHOS::DistributedHardware::DCameraMockRegisterCallback>();
    int32_t ret = sourceSA->InitSource("1.0");
    cout << "Init Source " << ret << endl;
    ret = sourceSA->RegisterDistributedHardware(TEST_DEV_ID, TEST_CAM_ID, param, regCb);
    unsigned int sleepSeconds = 2;
    sleep(sleepSeconds);
    cout << "sleep" << sleepSeconds << "second wait register end: " << ret << endl;

    std::shared_ptr<OHOS::DistributedHardware::Test> test = std::make_shared<OHOS::DistributedHardware::Test>();
    test->Init();
    test->Open();
    sleepSeconds = 1;
    sleep(sleepSeconds);
    cout << "sleep" << sleepSeconds << "second wait open camera end." << endl;

    std::cout << "==========[test log]Preview stream, 640*480, expected success." << std::endl;
    // 启动流
    test->intents = {OHOS::Camera::PREVIEW};
    test->StartStream(test->intents);
    sleepSeconds = 1;
    sleep(sleepSeconds);
    cout << "sleep" << sleepSeconds << "second wait start stream end." << endl;

    // 获取预览图
    test->StartCapture(test->streamId_preview, test->captureId_preview, false, true);
    // 释放流
    test->captureIds = {test->captureId_preview};
    test->streamIds = {test->streamId_preview};
    sleepSeconds = 1;
    sleep(sleepSeconds);
    cout << "sleep" << sleepSeconds << "second wait start capture end." << endl;

    test->StopStream(test->captureIds, test->streamIds);
    sleepSeconds = 1;
    sleep(sleepSeconds);
    cout << "sleep" << sleepSeconds << "second wait stop stream end." << endl;

    test->Close();
    sleepSeconds = 1;
    sleep(sleepSeconds);
    cout << "sleep" << sleepSeconds << "second wait close end." << endl;

    std::shared_ptr<OHOS::DistributedHardware::UnregisterCallback> unRegCb =
        std::make_shared<OHOS::DistributedHardware::DCameraMockUnRegisterCallback>();
    ret = sourceSA->UnregisterDistributedHardware(TEST_DEV_ID, TEST_CAM_ID, unRegCb);
    cout << "distributed camera hdf end ret: " << ret << endl;

    return 0;
}