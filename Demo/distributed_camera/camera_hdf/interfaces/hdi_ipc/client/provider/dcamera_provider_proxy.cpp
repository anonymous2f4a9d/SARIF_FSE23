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

#include "dcamera_provider_proxy.h"
#include <buffer_handle_parcel.h>
#include <buffer_handle_utils.h>
#include <hdf_base.h>
#include <message_parcel.h>
#include <sys/mman.h>
#include "distributed_hardware_log.h"
#include "ipc_data_utils.h"
#include "iservmgr_hdi.h"

namespace OHOS {
namespace DistributedHardware {
const std::string DC_PROVIDER_HDI_SERVER_NAME = "distributed_camera_provider_service";

sptr<IDCameraProvider> IDCameraProvider::Get()
{
    using namespace OHOS::HDI::ServiceManager::V1_0;
    OHOS::sptr<IServiceManager> serviceMgr = IServiceManager::Get();
    if (serviceMgr == nullptr) {
        DHLOGE("Get IServiceManager failed.");
        return nullptr;
    }

    OHOS::sptr<IRemoteObject> remote = serviceMgr->GetService(DC_PROVIDER_HDI_SERVER_NAME.c_str());
    if (remote == nullptr) {
        DHLOGE("GetService failed! serviceName = %s", DC_PROVIDER_HDI_SERVER_NAME.c_str());
        return nullptr;
    }

    return iface_cast<DCameraProviderProxy>(remote);
}

DCamRetCode DCameraProviderProxy::EnableDCameraDevice(const std::shared_ptr<DHBase> &dhBase,
    const std::string &abilityInfo, const sptr<IDCameraProviderCallback> &callback)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(DCameraProviderProxy::GetDescriptor())) {
        DHLOGE("Write token failed.");
        return DCamRetCode::INVALID_ARGUMENT;
    }

    if (!data.WriteString(dhBase->deviceId_) || !data.WriteString(dhBase->dhId_) || !data.WriteString(abilityInfo)) {
        DHLOGE("Write distributed camera base info or ability info failed.");
        return DCamRetCode::INVALID_ARGUMENT;
    }

    bool nullFlag = (callback != nullptr);
    if (!data.WriteBool(nullFlag)) {
        DHLOGE("Write distributed camera provider callback null flag failed.");
        return DCamRetCode::INVALID_ARGUMENT;
    }
    if (nullFlag && !data.WriteRemoteObject(callback->AsObject())) {
        DHLOGE("Set callback write remote obj failed.");
        return DCamRetCode::INVALID_ARGUMENT;
    }

    int32_t ret = Remote()->SendRequest(CMD_DISTRIBUTED_CAMERA_PROVIDER_ENABLE_DEVICE, data, reply, option);
    if (ret != HDF_SUCCESS) {
        DHLOGE("SendRequest failed, error code is %d", ret);
        return DCamRetCode::INVALID_ARGUMENT;
    }
    return static_cast<DCamRetCode>(reply.ReadInt32());
}

DCamRetCode DCameraProviderProxy::DisableDCameraDevice(const std::shared_ptr<DHBase> &dhBase)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(DCameraProviderProxy::GetDescriptor())) {
        DHLOGE("Write token failed.");
        return DCamRetCode::INVALID_ARGUMENT;
    }

    if (!data.WriteString(dhBase->deviceId_) || !data.WriteString(dhBase->dhId_)) {
        DHLOGE("Write distributed camera base info or ability info failed.");
        return DCamRetCode::INVALID_ARGUMENT;
    }

    int32_t ret = Remote()->SendRequest(CMD_DISTRIBUTED_CAMERA_PROVIDER_DISABLE_DEVICE, data, reply, option);
    if (ret != HDF_SUCCESS) {
        DHLOGE("SendRequest failed, error code is %d.", ret);
        return DCamRetCode::INVALID_ARGUMENT;
    }
    return static_cast<DCamRetCode>(reply.ReadInt32());
}

DCamRetCode DCameraProviderProxy::AcquireBuffer(const std::shared_ptr<DHBase> &dhBase, int streamId,
    std::shared_ptr<DCameraBuffer> &buffer)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(DCameraProviderProxy::GetDescriptor())) {
        DHLOGE("Write token failed.");
        return DCamRetCode::INVALID_ARGUMENT;
    }

    if (!data.WriteString(dhBase->deviceId_) || !data.WriteString(dhBase->dhId_)) {
        DHLOGE("Write distributed camera base info or ability info failed.");
        return DCamRetCode::INVALID_ARGUMENT;
    }

    if (!data.WriteInt32(static_cast<int32_t>(streamId))) {
        DHLOGE("Write streamId failed.");
        return DCamRetCode::INVALID_ARGUMENT;
    }

    int32_t ret = Remote()->SendRequest(CMD_DISTRIBUTED_CAMERA_PROVIDER_ACQUIRE_BUFFER, data, reply, option);
    if (ret != HDF_SUCCESS) {
        DHLOGE("SendRequest failed, error code is %d.", ret);
        return DCamRetCode::INVALID_ARGUMENT;
    }
    int32_t retCode = reply.ReadInt32();
    if (retCode != DCamRetCode::SUCCESS) {
        DHLOGE("Acquire avaliable buffer from stub failed.");
        return static_cast<DCamRetCode>(retCode);
    }

    buffer = std::make_shared<DCameraBuffer>();
    buffer->index_ = reply.ReadInt32();
    buffer->size_ = reply.ReadInt32();

    BufferHandle* retHandle = ReadBufferHandle(reply);
    if (retHandle == nullptr) {
        DHLOGE("Read retrun buffer handle failed.");
        FreeBufferHandle(buffer->bufferHandle_);
        return DCamRetCode::INVALID_ARGUMENT;
    }
    retHandle->virAddr = DCameraMemoryMap(retHandle);
    buffer->bufferHandle_ = retHandle;

    return DCamRetCode::SUCCESS;
}

DCamRetCode DCameraProviderProxy::ShutterBuffer(const std::shared_ptr<DHBase> &dhBase, int streamId,
    const std::shared_ptr<DCameraBuffer> &buffer)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(DCameraProviderProxy::GetDescriptor())) {
        DHLOGE("Write token failed.");
        return DCamRetCode::INVALID_ARGUMENT;
    }

    DCamRetCode ret = DCamRetCode::SUCCESS;
    do {
        if (!data.WriteString(dhBase->deviceId_) || !data.WriteString(dhBase->dhId_)) {
            DHLOGE("Write distributed camera base info or ability info failed.");
            ret = DCamRetCode::INVALID_ARGUMENT;
            break;
        }

        if (!data.WriteInt32(static_cast<int32_t>(streamId))) {
            DHLOGE("Write streamId failed.");
            ret = DCamRetCode::INVALID_ARGUMENT;
            break;
        }

        bool nullFlag = (buffer != nullptr);
        if (!data.WriteBool(nullFlag)) {
            DHLOGE("Write distributed camera buffer null flag failed.");
            ret = DCamRetCode::INVALID_ARGUMENT;
            break;
        }

        if (!data.WriteInt32(buffer->index_) || !data.WriteInt32(buffer->size_)) {
            DHLOGE("write buffer index and size parameter failed.");
            ret = DCamRetCode::INVALID_ARGUMENT;
            break;
        }

        int32_t retCode = Remote()->SendRequest(CMD_DISTRIBUTED_CAMERA_PROVIDER_SHUTTER_BUFFER, data, reply, option);
        if (retCode != HDF_SUCCESS) {
            DHLOGE("SendRequest failed, error code is %d.", retCode);
            ret = DCamRetCode::FAILED;
        }
    } while (0);

    DCameraMemoryUnmap(buffer->bufferHandle_);
    if (ret != DCamRetCode::SUCCESS) {
        return ret;
    }
    return static_cast<DCamRetCode>(reply.ReadInt32());
}

DCamRetCode DCameraProviderProxy::OnSettingsResult(const std::shared_ptr<DHBase> &dhBase,
    const std::shared_ptr<DCameraSettings> &result)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(DCameraProviderProxy::GetDescriptor())) {
        DHLOGE("Write token failed.");
        return DCamRetCode::INVALID_ARGUMENT;
    }

    if (!data.WriteString(dhBase->deviceId_) || !data.WriteString(dhBase->dhId_)) {
        DHLOGE("Write distributed camera base info or ability info failed.");
        return DCamRetCode::INVALID_ARGUMENT;
    }

    bool nullFlag = (result != nullptr);
    if (!data.WriteBool(nullFlag)) {
        DHLOGE("Write distributed camera settings null flag failed.");
        return DCamRetCode::INVALID_ARGUMENT;
    }
    if (nullFlag && !IpcDataUtils::EncodeDCameraSettings(result, data)) {
        DHLOGE("Write distributed camera settings failed.");
        return DCamRetCode::INVALID_ARGUMENT;
    }

    int32_t ret = Remote()->SendRequest(CMD_DISTRIBUTED_CAMERA_PROVIDER_ON_SETTINGS_RESULT, data, reply, option);
    if (ret != HDF_SUCCESS) {
        DHLOGE("SendRequest failed, error code is %d.", ret);
        return DCamRetCode::INVALID_ARGUMENT;
    }
    return static_cast<DCamRetCode>(reply.ReadInt32());
}

DCamRetCode DCameraProviderProxy::Notify(const std::shared_ptr<DHBase> &dhBase,
    const std::shared_ptr<DCameraHDFEvent> &event)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(DCameraProviderProxy::GetDescriptor())) {
        DHLOGE("Write token failed.");
        return DCamRetCode::INVALID_ARGUMENT;
    }

    if (!data.WriteString(dhBase->deviceId_) || !data.WriteString(dhBase->dhId_)) {
        DHLOGE("Write distributed camera base info or ability info failed.");
        return DCamRetCode::INVALID_ARGUMENT;
    }

    bool nullFlag = (event != nullptr);
    if (!data.WriteBool(nullFlag)) {
        DHLOGE("Write distributed camera hdf event null flag failed.");
        return DCamRetCode::INVALID_ARGUMENT;
    }
    if (nullFlag && !IpcDataUtils::EncodeDCameraHDFEvent(event, data)) {
        DHLOGE("Write distributed camera hdf event failed.");
        return DCamRetCode::INVALID_ARGUMENT;
    }

    int32_t ret = Remote()->SendRequest(CMD_DISTRIBUTED_CAMERA_PROVIDER_NOTIFY, data, reply, option);
    if (ret != HDF_SUCCESS) {
        DHLOGE("SendRequest failed, error code is %d.", ret);
        return DCamRetCode::INVALID_ARGUMENT;
    }
    return static_cast<DCamRetCode>(reply.ReadInt32());
}

#ifdef BALTIMORE_CAMERA
void* DCameraProviderProxy::DCameraMemoryMap(const BufferHandle *buffer)
{
    if (buffer == nullptr) {
        DHLOGE("mmap the buffer handle is NULL");
        return nullptr;
    }
    if (buffer->reserveFds <= 0) {
        DHLOGE("invalid file descriptor num : %d", buffer->reserveFds);
        return nullptr;
    }
    void* virAddr = mmap(NULL, buffer->size, PROT_READ | PROT_WRITE, MAP_SHARED, buffer->reserve[0], 0);
    if (virAddr == MAP_FAILED) {
        DHLOGE("mmap failed errno %s, fd : %d", strerror(errno), buffer->fd);
        return nullptr;
    }
    return virAddr;
}
#else
void* DCameraProviderProxy::DCameraMemoryMap(const BufferHandle *buffer)
{
    if (buffer == nullptr) {
        DHLOGE("mmap the buffer handle is NULL");
        return nullptr;
    }

    void* virAddr = mmap(NULL, buffer->size, PROT_READ | PROT_WRITE, MAP_SHARED, buffer->fd, 0);
    if (virAddr == MAP_FAILED) {
        DHLOGE("mmap failed errno %s, fd : %d", strerror(errno), buffer->fd);
        return nullptr;
    }
    return virAddr;
}
#endif

void DCameraProviderProxy::DCameraMemoryUnmap(BufferHandle *buffer)
{
    if (buffer == nullptr) {
        DHLOGE("unmmap the buffer handle is NULL");
        return;
    }
    if (buffer->virAddr == nullptr) {
        DHLOGE("virAddr is NULL , has not map the buffer");
        return;
    }
    int ret = munmap(buffer->virAddr, buffer->size);
    if (ret != 0) {
        DHLOGE("munmap failed err: %s", strerror(errno));
    }
    buffer->virAddr = nullptr;
    FreeBufferHandle(buffer);
}
} // namespace DistributedHardware
} // namespace OHOS
