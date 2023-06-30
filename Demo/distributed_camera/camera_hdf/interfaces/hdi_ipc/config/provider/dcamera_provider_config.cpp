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

#include "dcamera_provider_stub.h"
#include <hdf_base.h>
#include <hdf_device_desc.h>
#include <hdf_log.h>
#include <osal_mem.h>

struct HdfDCameraProviderService {
    struct IDeviceIoService ioservice;
    void *instance;
};

static int32_t DCameraProviderServiceDispatch(struct HdfDeviceIoClient *client, int cmdId,
    struct HdfSBuf *data, struct HdfSBuf *reply)
{
    HdfDCameraProviderService *service = CONTAINER_OF(client->device->service, HdfDCameraProviderService, ioservice);
    return DCProviderServiceOnRemoteRequest(service->instance, cmdId, data, reply);
}

int HdfDCameraProviderConfigInit(struct HdfDeviceObject *deviceObject)
{
    return HDF_SUCCESS;
}

int HdfDCameraProviderConfigBind(HdfDeviceObject *deviceObject)
{
    HDF_LOGI("HdfDCameraProviderConfigBind enter!");
    if (deviceObject == nullptr) {
        HDF_LOGE("HdfDCameraProviderConfigBind: HdfDeviceObject is NULL !");
        return HDF_FAILURE;
    }

    HdfDCameraProviderService *service =
        reinterpret_cast<HdfDCameraProviderService *>(malloc(sizeof(HdfDCameraProviderService)));
    if (service == nullptr) {
        HDF_LOGE("HdfDCameraProviderConfigBind malloc HdfDCameraProviderService failed!");
        return HDF_FAILURE;
    }

    service->ioservice.Dispatch = DCameraProviderServiceDispatch;
    service->ioservice.Open = nullptr;
    service->ioservice.Release = nullptr;
    service->instance = DCameraProviderStubInstance();

    deviceObject->service = &service->ioservice;
    return HDF_SUCCESS;
}

void HdfDCameraProviderConfigRelease(HdfDeviceObject *deviceObject)
{
    if (deviceObject == nullptr || deviceObject->service == nullptr) {
        HDF_LOGE("HdfDCameraProviderConfigRelease: deviceObject or deviceObject->service is NULL!");
        return;
    }
    HdfDCameraProviderService *service = CONTAINER_OF(deviceObject->service, HdfDCameraProviderService, ioservice);
    if (service == nullptr) {
        HDF_LOGE("HdfDCameraProviderConfigRelease: service is NULL!");
        return;
    }
    free(service);
}

struct HdfDriverEntry g_dCameraProviderConfigEntry = {
    .moduleVersion = 1,
    .moduleName = "distributed_camera_provider_service",
    .Bind = HdfDCameraProviderConfigBind,
    .Init = HdfDCameraProviderConfigInit,
    .Release = HdfDCameraProviderConfigRelease,
};

#ifndef __cplusplus
extern "C" {
#endif // __cplusplus

HDF_INIT(g_dCameraProviderConfigEntry);

#ifndef __cplusplus
}
#endif // __cplusplus
