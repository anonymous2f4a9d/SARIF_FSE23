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

#include "dcamera_sink_dev.h"

#include "anonymous_string.h"
#include "dcamera_channel_info_cmd.h"
#include "dcamera_info_cmd.h"
#include "dcamera_protocol.h"
#include "dcamera_sink_access_control.h"
#include "dcamera_sink_controller.h"
#include "distributed_camera_errno.h"
#include "distributed_hardware_log.h"

namespace OHOS {
namespace DistributedHardware {
DCameraSinkDev::DCameraSinkDev(const std::string& dhId) : dhId_(dhId)
{
    DHLOGI("DCameraSinkDev Constructor dhId: %s", GetAnonyString(dhId_).c_str());
    isInit_ = false;
}

DCameraSinkDev::~DCameraSinkDev()
{
    if (isInit_) {
        UnInit();
    }
}

int32_t DCameraSinkDev::Init()
{
    DHLOGI("DCameraSinkDev::Init dhId: %s", GetAnonyString(dhId_).c_str());
    accessControl_ = std::make_shared<DCameraSinkAccessControl>();
    controller_ = std::make_shared<DCameraSinkController>(accessControl_);
    DCameraIndex index("", dhId_);
    std::vector<DCameraIndex> indexs;
    indexs.push_back(index);
    int32_t ret = controller_->Init(indexs);
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSinkDev::Init init controller failed, dhId: %s, ret: %d", GetAnonyString(dhId_).c_str(), ret);
        return ret;
    }

    isInit_ = true;
    DHLOGI("DCameraSinkDev::Init %s success", GetAnonyString(dhId_).c_str());
    return DCAMERA_OK;
}

int32_t DCameraSinkDev::UnInit()
{
    if (controller_ != nullptr) {
        int32_t ret = controller_->UnInit();
        if (ret != DCAMERA_OK) {
            DHLOGE("DCameraSinkDev::UnInit release controller failed, dhId: %s, ret: %d",
                   GetAnonyString(dhId_).c_str(), ret);
        }
    }
    isInit_ = false;
    return DCAMERA_OK;
}

int32_t DCameraSinkDev::SubscribeLocalHardware(const std::string& parameters)
{
    DHLOGI("DCameraSinkDev::SubscribeLocalHardware");
    return DCAMERA_OK;
}

int32_t DCameraSinkDev::UnsubscribeLocalHardware()
{
    DHLOGI("DCameraSinkDev::UnsubscribeLocalHardware");
    return DCAMERA_OK;
}

int32_t DCameraSinkDev::StopCapture()
{
    DHLOGI("DCameraSinkDev::StopCapture dhId: %s", GetAnonyString(dhId_).c_str());
    return controller_->StopCapture();
}

int32_t DCameraSinkDev::ChannelNeg(std::string& channelInfo)
{
    DHLOGI("DCameraSinkDev::ChannelNeg dhId: %s", GetAnonyString(dhId_).c_str());
    if (channelInfo.empty()) {
        DHLOGE("DCameraSinkDev::ChannelNeg channelInfo is empty");
        return DCAMERA_BAD_VALUE;
    }

    DCameraChannelInfoCmd channelInfoCmd;
    int32_t ret = channelInfoCmd.Unmarshal(channelInfo);
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSinkDev::ChannelNeg channelInfo unmarshal failed, dhId: %s, ret: %d",
               GetAnonyString(dhId_).c_str(), ret);
        return ret;
    }
    return controller_->ChannelNeg(channelInfoCmd.value_);
}

int32_t DCameraSinkDev::GetCameraInfo(std::string& cameraInfo)
{
    DHLOGI("DCameraSinkDev::GetCameraInfo dhId: %s", GetAnonyString(dhId_).c_str());
    std::shared_ptr<DCameraInfo> info = std::make_shared<DCameraInfo>();
    int32_t ret = controller_->GetCameraInfo(info);
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSinkDev::GetCameraInfo get state failed, dhId: %s, ret: %d",
               GetAnonyString(dhId_).c_str(), ret);
        return ret;
    }

    DCameraInfoCmd cameraInfoCmd;
    cameraInfoCmd.type_ = DCAMERA_PROTOCOL_TYPE_MESSAGE;
    cameraInfoCmd.dhId_ = dhId_;
    cameraInfoCmd.command_ = DCAMERA_PROTOCOL_CMD_GET_INFO;
    cameraInfoCmd.value_ = info;
    ret = cameraInfoCmd.Marshal(cameraInfo);
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSinkDev::GetCameraInfo cameraInfoCmd marshal failed, dhId: %s, ret: %d",
               GetAnonyString(dhId_).c_str(), ret);
        return ret;
    }
    DHLOGI("DCameraSinkDev::GetCameraInfo %s success", GetAnonyString(dhId_).c_str());
    return DCAMERA_OK;
}

int32_t DCameraSinkDev::OpenChannel(std::string& openInfo)
{
    DHLOGI("DCameraSinkDev::OpenChannel dhId: %s", GetAnonyString(dhId_).c_str());
    if (openInfo.empty()) {
        DHLOGE("DCameraSinkDev::OpenChannel openInfo is empty");
        return DCAMERA_BAD_VALUE;
    }

    DCameraOpenInfoCmd cmd;
    int32_t ret = cmd.Unmarshal(openInfo);
    if (ret != DCAMERA_OK) {
        DHLOGE("DCameraSinkDev::OpenChannel openInfo unmarshal failed, dhId: %s, ret: %d",
               GetAnonyString(dhId_).c_str(), ret);
        return ret;
    }
    return controller_->OpenChannel(cmd.value_);
}

int32_t DCameraSinkDev::CloseChannel()
{
    DHLOGI("DCameraSinkDev::CloseChannel dhId: %s", GetAnonyString(dhId_).c_str());
    return controller_->CloseChannel();
}
} // namespace DistributedHardware
} // namespace OHOS