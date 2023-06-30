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

#ifndef DISTRIBUTED_CAMERA_STREAM_OPERATOR_H
#define DISTRIBUTED_CAMERA_STREAM_OPERATOR_H

#include <map>
#include <set>
#include <vector>
#include "istream_operator.h"
#include "dstream_operator_stub.h"
#include "dcamera.h"
#include "dmetadata_processor.h"
#include "types.h"
#include "constants.h"
#include "dcamera_steam.h"

namespace OHOS {
namespace DistributedHardware {
using namespace std;
class DCameraProvider;
class DStreamOperator : public DStreamOperatorStub {
public:
    DStreamOperator(std::shared_ptr<DMetadataProcessor> &dMetadataProcessor);
    DStreamOperator() = default;
    virtual ~DStreamOperator() = default;
    DStreamOperator(const DStreamOperator &other) = delete;
    DStreamOperator(DStreamOperator &&other) = delete;
    DStreamOperator& operator=(const DStreamOperator &other) = delete;
    DStreamOperator& operator=(DStreamOperator &&other) = delete;

public:
    CamRetCode IsStreamsSupported(OperationMode mode,
                                  const std::shared_ptr<CameraStandard::CameraMetadata> &modeSetting,
                                  const std::vector<std::shared_ptr<StreamInfo>> &info,
                                  StreamSupportType &type) override;
    CamRetCode CreateStreams(const std::vector<std::shared_ptr<StreamInfo>>& streamInfos) override;
    CamRetCode ReleaseStreams(const std::vector<int>& streamIds) override;
    CamRetCode CommitStreams(OperationMode mode,
                             const std::shared_ptr<CameraStandard::CameraMetadata>& modeSetting) override;
    CamRetCode GetStreamAttributes(std::vector<std::shared_ptr<StreamAttribute>>& attributes) override;
    CamRetCode AttachBufferQueue(int streamId, const OHOS::sptr<OHOS::IBufferProducer>& producer) override;
    CamRetCode DetachBufferQueue(int streamId) override;
    CamRetCode Capture(int captureId, const std::shared_ptr<CaptureInfo>& captureInfo, bool isStreaming) override;
    CamRetCode CancelCapture(int captureId) override;
    CamRetCode ChangeToOfflineStream(const std::vector<int>& streamIds, OHOS::sptr<IStreamOperatorCallback>& callback,
                                     OHOS::sptr<IOfflineStreamOperator>& offlineOperator) override;

    DCamRetCode InitOutputConfigurations(const std::shared_ptr<DHBase> &dhBase, const std::string &abilityInfo);
    DCamRetCode AcquireBuffer(int streamId, std::shared_ptr<DCameraBuffer> &buffer);
    DCamRetCode ShutterBuffer(int streamId, const std::shared_ptr<DCameraBuffer> &buffer);
    DCamRetCode SetCallBack(OHOS::sptr<IStreamOperatorCallback> const &callback);
    DCamRetCode SetDeviceCallback(function<void(ErrorType, int)> &errorCbk,
                                  function<void(uint64_t, std::shared_ptr<CameraStandard::CameraMetadata>)> &resultCbk);
    void Release();

private:
    bool IsCapturing();
    void SetCapturing(bool isCapturing);
    DCamRetCode NegotiateSuitableCaptureInfo(const std::shared_ptr<CaptureInfo>& srcCaptureInfo, bool isStreaming);
    void ChooseSuitableFormat(std::vector<std::shared_ptr<DCStreamInfo>> &streamInfo,
                              std::shared_ptr<DCCaptureInfo> &captureInfo);
    void ChooseSuitableResolution(std::vector<std::shared_ptr<DCStreamInfo>> &streamInfo,
                                  std::shared_ptr<DCCaptureInfo> &captureInfo);
    void ChooseSuitableDataSpace(std::vector<std::shared_ptr<DCStreamInfo>> &streamInfo,
                                 std::shared_ptr<DCCaptureInfo> &captureInfo);
    void ChooseSuitableEncodeType(std::vector<std::shared_ptr<DCStreamInfo>> &streamInfo,
                                  std::shared_ptr<DCCaptureInfo> &captureInfo);
    void ConvertStreamInfo(std::shared_ptr<StreamInfo> &srcInfo, std::shared_ptr<DCStreamInfo> &dstInfo);
    DCEncodeType ConvertDCEncodeType(std::string &srcEncodeType);
    std::shared_ptr<DCCaptureInfo> BuildSuitableCaptureInfo(const shared_ptr<CaptureInfo>& srcCaptureInfo,
        std::vector<std::shared_ptr<DCStreamInfo>> &srcStreamInfo);
    void SnapShotStreamOnCaptureEnded(int32_t captureId, int streamId);

private:
    std::shared_ptr<DMetadataProcessor> dMetadataProcessor_;
    OHOS::sptr<IStreamOperatorCallback> dcStreamOperatorCallback_;
    function<void(ErrorType, int)> errorCallback_;
    function<void(uint64_t, std::shared_ptr<CameraStandard::CameraMetadata>)> resultCallback_;

    std::shared_ptr<DHBase> dhBase_;
    std::vector<DCEncodeType> dcSupportedCodecType_;
    std::map<DCSceneType, std::vector<int>> dcSupportedFormatMap_;
    std::map<int, std::vector<DCResolution>> dcSupportedResolutionMap_;

    std::map<int, std::shared_ptr<DCameraStream>> halStreamMap_;
    std::map<int, std::shared_ptr<DCStreamInfo>> dcStreamInfoMap_;
    std::map<int, std::shared_ptr<CaptureInfo>> halCaptureInfoMap_;
    std::vector<std::shared_ptr<DCCaptureInfo>> cachedDCaptureInfoList_;
    std::map<int, bool> enableShutterCbkMap_;
    std::map<pair<int, int>, int> acceptedBufferNum_;

    std::mutex requestLock_;
    bool isCapturing_ = false;
    std::mutex isCapturingLock_;
    OperationMode currentOperMode_ = OperationMode::NORMAL;
    std::shared_ptr<CameraStandard::CameraMetadata> latestStreamSetting_;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // DISTRIBUTED_CAMERA_STREAM_OPERATOR_H
