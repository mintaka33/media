/*===================== begin_copyright_notice ==================================

INTEL CONFIDENTIAL
Copyright 2017
Intel Corporation All Rights Reserved.

The source code contained or described herein and all documents related to the
source code ("Material") are owned by Intel Corporation or its suppliers or
licensors. Title to the Material remains with Intel Corporation or its suppliers
and licensors. The Material contains trade secrets and proprietary and confidential
information of Intel or its suppliers and licensors. The Material is protected by
worldwide copyright and trade secret laws and treaty provisions. No part of the
Material may be used, copied, reproduced, modified, published, uploaded, posted,
transmitted, distributed, or disclosed in any way without Intel's prior express
written permission.

No license under any patent, copyright, trade secret or other intellectual
property right is granted to or conferred upon you by disclosure or delivery
of the Materials, either expressly, by implication, inducement, estoppel
or otherwise. Any license under such intellectual property rights must be
express and approved by Intel in writing.

======================= end_copyright_notice ==================================*/

//!
//! \file     state_heap.h
//! \brief    state heap class definition
//! \details
//!

#pragma once

#include <stdint.h>
#include <vector>
#include "media_resource.h"
#include "mhw_state_heap_hwcmd_g9_X.h"

namespace Apogeios
{

class SSH
{
public:
    SSH();
    ~SSH();

    using BindingTable = mhw_state_heap_g9_X::BINDING_TABLE_STATE_CMD;
    using SurfaceState = mhw_state_heap_g9_X::RENDER_SURFACE_STATE_CMD;

    // SSH resides in command buffer, so there is no independent resource for it.
    // the command buffer object will copy SSH data into command buffer resource
    std::vector<uint8_t>& getSshData() { return sshData_; };
    int32_t initialize(std::vector<MediaResourceBind> resBindList);
    int32_t destroy();

private:
    int32_t setSshEntry(SurfaceState& state, MediaResource* res, int32_t index, bool isUV);

private:
    const int32_t numBindingTable_ = 1;
    const int32_t indexNumInBT_ = 256;
    const int32_t numSurfaceState_ = 256;
    const int32_t btEntrySize_ = sizeof(BindingTable);
    const int32_t ssEntrySize_ = sizeof(SurfaceState);
    const int32_t btRegionSize_ = numBindingTable_ * indexNumInBT_ * btEntrySize_;
    const int32_t ssRegionSize_ = numSurfaceState_ * ssEntrySize_;
    const int32_t sshSize_ = btRegionSize_ + ssRegionSize_;

    SurfaceState cmdInit_;
    std::vector<BindingTable> cmdBT_;
    std::vector<SurfaceState> cmdSS_;
    std::vector<uint8_t> sshData_;
};

class DSH
{
public:
    DSH();
    ~DSH();

    MediaResource* getResource() { return pRes_; };
    int32_t initialize(uint8_t* pCurbe, int32_t curbeSize);
    int32_t destroy();

    uint32_t getIdSize() { return cmdID_.byteSize; }
    uint32_t getIdOffset() { return 0; }
    uint32_t getCurbeSize() { return (uint32_t)curbe_.size(); }
    uint32_t getCurbeOffset() { return idRegionSize_; }

private:
    const int32_t curbeSizeAlignment_ = 64;
    const int32_t curbeOffsetAlignment_ = 64;
    const int32_t dshAlignment_ = 64 * 1024;

    mhw_state_heap_g9_X::INTERFACE_DESCRIPTOR_DATA_CMD cmdID_;
    std::vector<uint8_t> curbe_;
    uint32_t idRegionSize_ = 0;
    std::vector<uint8_t> dshData_;
    MediaResource* pRes_ = nullptr;
};

class ISH
{
public:
    ISH();
    ~ISH();

    MediaResource* getResource() { return pRes_; };
    int32_t initialize(std::vector<uint8_t>& genISA);
    int32_t destroy();

private:
    const int32_t ishAlignment_ = 64 * 1024;

    std::vector<uint32_t> kernelBinary_;
    MediaResource* pRes_ = nullptr;
};

}