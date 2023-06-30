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

#ifndef OHOS_DCAMERA_SINK_HANDLER_H
#define OHOS_DCAMERA_SINK_HANDLER_H

#include "idistributed_hardware_sink.h"
#include "single_instance.h"

namespace OHOS {
namespace DistributedHardware {
class DCameraSinkHandler : public IDistributedHardwareSink {
DECLARE_SINGLE_INSTANCE_BASE(DCameraSinkHandler);
public:
    int32_t InitSink(const std::string& params) override;
    int32_t ReleaseSink() override;
    int32_t SubscribeLocalHardware(const std::string& dhId, const std::string& parameters) override;
    int32_t UnsubscribeLocalHardware(const std::string& dhId) override;

private:
    DCameraSinkHandler() = default;
    ~DCameraSinkHandler();
};

#ifdef __cplusplus
extern "C" {
#endif
__attribute__((visibility("default"))) IDistributedHardwareSink *GetSinkHardwareHandler();
#ifdef __cplusplus
}
#endif
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_DCAMERA_SINK_HANDLER_H