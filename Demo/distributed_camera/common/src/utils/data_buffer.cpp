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

#include "data_buffer.h"
#include "distributed_camera_errno.h"

namespace OHOS {
namespace DistributedHardware {
DataBuffer::DataBuffer(size_t capacity)
{
    if (capacity != 0) {
        data_ = new uint8_t[capacity] {0};
        if (data_ != nullptr) {
            capacity_ = capacity;
            rangeLength_ = capacity;
        }
    }
}

size_t DataBuffer::Capacity() const
{
    return capacity_;
}

size_t DataBuffer::Size() const
{
    return rangeLength_;
}

size_t DataBuffer::Offset() const
{
    return rangeOffset_;
}

uint8_t *DataBuffer::Data() const
{
    return data_ + rangeOffset_;
}

int32_t DataBuffer::SetRange(size_t offset, size_t size)
{
    if (!(offset <= capacity_) || !(offset + size <= capacity_)) {
        return DCAMERA_BAD_VALUE;
    }

    rangeOffset_ = offset;
    rangeLength_ = size;
    return DCAMERA_OK;
}

void DataBuffer::SetInt32(const string name, int32_t value)
{
    int32Map_[name] = value;
}

void DataBuffer::SetInt64(const string name, int64_t value)
{
    int64Map_[name] = value;
}

void DataBuffer::SetString(const string name, string value)
{
    stringMap_[name] = value;
}

bool DataBuffer::FindInt32(const string& name, int32_t& value)
{
    if (int32Map_.count(name) != 0) {
        value = int32Map_[name];
        return true;
    } else {
        value = 0;
        return false;
    }
}

bool DataBuffer::FindInt64(const string& name, int64_t& value)
{
    if (int64Map_.count(name) != 0) {
        value = int64Map_[name];
        return true;
    } else {
        value = 0;
        return false;
    }
}

bool DataBuffer::FindString(const string& name, string& value)
{
    if (stringMap_.count(name) != 0) {
        value = stringMap_[name];
        return true;
    } else {
        value = "";
        return false;
    }
}

DataBuffer::~DataBuffer()
{
    if (data_ != nullptr) {
        delete[] data_;
        data_ = nullptr;
    }
}
} // namespace DistributedHardware
} // namespace OHOS
