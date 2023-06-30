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

#ifndef HDI_IPC_DATA_UTILS_H
#define HDI_IPC_DATA_UTILS_H

#include <list>
#include <map>
#include <vector>
#include <message_parcel.h>
#include <iostream>
#include <iservmgr_hdi.h>
#include "camera_metadata_info.h"

#include <foundation/distributedhardware/distributedcamera/camera_hdf/interfaces/include/types.h>
#ifdef BALTIMORE_CAMERA
#include <drivers/peripheral/adapter/camera/interfaces/include/types.h>
#else
#include <drivers/peripheral/camera/interfaces/include/types.h>
#endif

namespace OHOS {
namespace DistributedHardware {
class IpcDataUtils {
static const uint32_t RATIONAL_TYPE_STEP = 2;
public:
    static bool EncodeStreamInfo(const std::shared_ptr<Camera::StreamInfo> &pInfo, MessageParcel &parcel)
    {
        bool bRet = true;
        bRet = (bRet && parcel.WriteInt32(static_cast<int32_t>(pInfo->streamId_)));
        bRet = (bRet && parcel.WriteInt32(static_cast<int32_t>(pInfo->width_)));
        bRet = (bRet && parcel.WriteInt32(static_cast<int32_t>(pInfo->height_)));
        bRet = (bRet && parcel.WriteInt32(static_cast<int32_t>(pInfo->format_)));
        bRet = (bRet = (bRet && parcel.WriteInt32(pInfo->intent_)));
        bRet = (bRet && parcel.WriteBool(pInfo->tunneledMode_));
        bool bufferQueueFlag = (pInfo->bufferQueue_ != nullptr) ? true : false;
        bRet = (bRet && parcel.WriteBool(bufferQueueFlag));
        if (bufferQueueFlag) {
            bRet = (bRet && parcel.WriteRemoteObject(pInfo->bufferQueue_->AsObject()));
        }
        bRet = (bRet && parcel.WriteInt32(static_cast<int32_t>(pInfo->minFrameDuration_)));
        bRet = (bRet && parcel.WriteInt32(pInfo->encodeType_));
        return bRet;
    }

    static void DecodeStreamInfo(MessageParcel &parcel, std::shared_ptr<Camera::StreamInfo> &pInfo)
    {
        pInfo->streamId_ = static_cast<int>(parcel.ReadInt32());
        pInfo->width_ = static_cast<int>(parcel.ReadInt32());
        pInfo->height_ = static_cast<int>(parcel.ReadInt32());
        pInfo->format_ = static_cast<int>(parcel.ReadInt32());
        pInfo->intent_ = static_cast<Camera::StreamIntent>(parcel.ReadInt32());
        pInfo->tunneledMode_ = parcel.ReadBool();
        bool bufferQueueFlag = parcel.ReadBool();
        if (bufferQueueFlag) {
            sptr<IRemoteObject> remoteBufferProducer = parcel.ReadRemoteObject();
            pInfo->bufferQueue_ = OHOS::iface_cast<OHOS::IBufferProducer>(remoteBufferProducer);
        }
        pInfo->minFrameDuration_ = static_cast<int>(parcel.ReadInt32());
        pInfo->encodeType_ = static_cast<Camera::EncodeType>(parcel.ReadInt32());
    }

    static bool EncodeDCameraSettings(const std::shared_ptr<DCameraSettings> &pSettings, MessageParcel &parcel)
    {
        bool bRet = true;
        bRet = (bRet && parcel.WriteInt32(pSettings->type_));
        bRet = (bRet && parcel.WriteString(pSettings->value_));
        return bRet;
    }

    static void DecodeDCameraSettings(MessageParcel &parcel, std::shared_ptr<DCameraSettings> &pSettings)
    {
        pSettings->type_ = (DCSettingsType)(parcel.ReadInt32());
        pSettings->value_ = parcel.ReadString();
    }

    static bool EncodeDCameraHDFEvent(const std::shared_ptr<DCameraHDFEvent> &pEvent, MessageParcel &parcel)
    {
        bool bRet = true;
        bRet = (bRet && parcel.WriteInt32(pEvent->type_));
        bRet = (bRet && parcel.WriteInt32(pEvent->result_));
        bRet = (bRet && parcel.WriteString(pEvent->content_));
        return bRet;
    }

    static void DecodeDCameraHDFEvent(MessageParcel &parcel, std::shared_ptr<DCameraHDFEvent> &pEvent)
    {
        pEvent->type_ = parcel.ReadInt32();
        pEvent->result_ = parcel.ReadInt32();
        pEvent->content_ = parcel.ReadString();
    }
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // HDI_IPC_DATA_UTILS_H