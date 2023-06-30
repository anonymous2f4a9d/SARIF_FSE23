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

#ifndef OHOS_DCAMERA_SOFTBUS_ADAPTER_H
#define OHOS_DCAMERA_SOFTBUS_ADAPTER_H

#include <mutex>
#include <map>
#include <unistd.h>

#include "session.h"
#include "single_instance.h"

#include "dcamera_softbus_session.h"

namespace OHOS {
namespace DistributedHardware {
typedef enum {
    DCAMERA_CHANNLE_ROLE_SOURCE = 0,
    DCAMERA_CHANNLE_ROLE_SINK = 1,
} DCAMERA_CHANNEL_ROLE;

class DCameraSoftbusAdapter {
DECLARE_SINGLE_INSTANCE_BASE(DCameraSoftbusAdapter);

public:
    int32_t CreateSoftbusSessionServer(std::string sessionName, DCAMERA_CHANNEL_ROLE role);
    int32_t DestroySoftbusSessionServer(std::string sessionName);
    int32_t OpenSoftbusSession(std::string mySessName, std::string peerSessName, int32_t sessionMode,
        std::string peerDevId);
    int32_t CloseSoftbusSession(int32_t sessionId);
    int32_t SendSofbusBytes(int32_t sessionId, std::shared_ptr<DataBuffer>& buffer);
    int32_t SendSofbusStream(int32_t sessionId, std::shared_ptr<DataBuffer>& buffer);
    int32_t GetLocalNetworkId(std::string& myDevId);

    int32_t OnSourceSessionOpened(int32_t sessionId, int32_t result);
    void OnSourceSessionClosed(int32_t sessionId);
    void OnSourceBytesReceived(int32_t sessionId, const void *data, uint32_t dataLen);
    void OnSourceMessageReceived(int32_t sessionId, const void *data, uint32_t dataLen);
    void OnSourceStreamReceived(int32_t sessionId, const StreamData *data, const StreamData *ext,
        const StreamFrameInfo *param);

    int32_t OnSinkSessionOpened(int32_t sessionId, int32_t result);
    void OnSinkSessionClosed(int32_t sessionId);
    void OnSinkBytesReceived(int32_t sessionId, const void *data, uint32_t dataLen);
    void OnSinkMessageReceived(int32_t sessionId, const void *data, uint32_t dataLen);
    void OnSinkStreamReceived(int32_t sessionId, const StreamData *data, const StreamData *ext,
        const StreamFrameInfo *param);

public:
    std::map<std::string, std::shared_ptr<DCameraSoftbusSession>> sourceSessions_;
    std::map<std::string, std::shared_ptr<DCameraSoftbusSession>> sinkSessions_;

private:
    DCameraSoftbusAdapter();
    ~DCameraSoftbusAdapter();

    int32_t DCameraSoftbusSourceGetSession(int32_t sessionId, std::shared_ptr<DCameraSoftbusSession>& session);
    int32_t DCameraSoftbusSinkGetSession(int32_t sessionId, std::shared_ptr<DCameraSoftbusSession>& session);
    int32_t DCameraSoftbusGetSessionById(int32_t sessionId, std::shared_ptr<DCameraSoftbusSession>& session);

private:
    std::mutex optLock_;
    const string PKG_NAME = "ohos.dhardware";
    static const uint32_t DCAMERA_SESSION_NAME_MAX_LEN = 128;
    map<DCAMERA_CHANNEL_ROLE, ISessionListener> sessListeners_;
    std::map<std::string, uint32_t> sessionTotal_;
    static const uint32_t DCAMERA_LINK_TYPE_MAX = 4;
    static const uint32_t DCAMERA_LINK_TYPE_INDEX_2 = 2;
    std::mutex idMapLock_;
    std::map<int32_t, std::shared_ptr<DCameraSoftbusSession>> sessionIdMap_;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_DCAMERA_SOFTBUS_ADAPTER_H
