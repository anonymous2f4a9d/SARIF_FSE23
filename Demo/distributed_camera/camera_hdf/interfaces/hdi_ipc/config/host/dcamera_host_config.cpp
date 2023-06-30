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

#include "dcamera_host_stub.h"
#include <hdf_base.h>
#include <hdf_device_desc.h>
#include <hdf_log.h>
#include <osal_mem.h>

struct HdfDCameraService {
    struct IDeviceIoService ioservice;
    void *instance;
};

static int32_t DCameraServiceDispatch(struct HdfDeviceIoClient *client, int cmdId,
    struct HdfSBuf *data, struct HdfSBuf *reply)
{
    HdfDCameraService *service = CONTAINER_OF(client->device->service, HdfDCameraService, ioservice);
    return DCHostServiceOnRemoteRequest(service->instance, cmdId, data, reply);
}

int HdfDCameraHostDriverInit(struct HdfDeviceObject *deviceObject)
{
    return HDF_SUCCESS;
}

int HdfDCameraHostDriverBind(HdfDeviceObject *deviceObject)
{
    HDF_LOGI("HdfDCameraHostDriverBind enter!");
    if (deviceObject == nullptr) {
        HDF_LOGE("HdfDCameraHostDriverBind: HdfDeviceObject is NULL !");
        return HDF_FAILURE;
    }

    HdfDCameraService *service = reinterpret_cast<HdfDCameraService *>(malloc(sizeof(HdfDCameraService)));
    if (service == nullptr) {
        HDF_LOGE("HdfDCameraHostDriverBind malloc HdfDCameraService failed!");
        return HDF_FAILURE;
    }

    service->ioservice.Dispatch = DCameraServiceDispatch;
    service->ioservice.Open = nullptr;
    service->ioservice.Release = nullptr;
    service->instance = DCameraHostStubInstance();

    deviceObject->service = &service->ioservice;
    return HDF_SUCCESS;
}

void HdfDCameraHostDriverRelease(HdfDeviceObject *deviceObject)
{
    if (deviceObject == nullptr || deviceObject->service == nullptr) {
        HDF_LOGE("HdfDCameraHostDriverRelease: deviceObject or deviceObject->service is NULL!");
        return;
    }
    HdfDCameraService *service = CONTAINER_OF(deviceObject->service, HdfDCameraService, ioservice);
    if (service == nullptr) {
        HDF_LOGE("HdfDCameraHostDriverRelease: service is NULL!");
        return;
    }
    free(service);
}

struct HdfDriverEntry g_dCameraHostDriverEntry = {
    .moduleVersion = 1,
    .moduleName = "distributed_camera_service",
    .Bind = HdfDCameraHostDriverBind,
    .Init = HdfDCameraHostDriverInit,
    .Release = HdfDCameraHostDriverRelease,
};

#ifndef __cplusplus
extern "C" {
#endif // __cplusplus

HDF_INIT(g_dCameraHostDriverEntry);

#ifndef __cplusplus
}
#endif // __cplusplus