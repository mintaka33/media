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
//! \file     state_heap.cpp
//! \brief    state heap class implementation
//! \details
//!

#include "common.h"
#include "state_heap.h"

namespace Apogeios
{

SSH::SSH() :
    cmdInit_()
{
    cmdInit_.DW0.SurfaceHorizontalAlignment = 1;
    cmdInit_.DW0.SurfaceVerticalAlignment = 1;
    cmdInit_.DW7.ShaderChannelSelectAlpha = cmdInit_.SHADER_CHANNEL_SELECT_ALPHA_ALPHA;
    cmdInit_.DW7.ShaderChannelSelectBlue = cmdInit_.SHADER_CHANNEL_SELECT_BLUE_BLUE;
    cmdInit_.DW7.ShaderChannelSelectGreen = cmdInit_.SHADER_CHANNEL_SELECT_GREEN_GREEN;
    cmdInit_.DW7.ShaderChannelSelectRed = cmdInit_.SHADER_CHANNEL_SELECT_RED_RED;

    cmdBT_.resize(numBindingTable_ * indexNumInBT_);
    cmdSS_.resize(numSurfaceState_);
}

SSH::~SSH()
{
}

int32_t SSH::initialize(std::vector<MediaResourceBind> resBindList)
{
    int32_t index = 0;

    for (auto resBind : resBindList)
    {
        if (resBind.res == nullptr)
        {
            // we should expect all media resource added into bind list is valid
            return -1;
        }

        index = resBind.surfIndex;
        RES_FORMAT resFormat = resBind.res->getFormat();
        if (resBind.numPlane == 2)
        {
            // Y plane
            cmdSS_[index] = cmdInit_;
            if (setSshEntry(cmdSS_[index], resBind.res, index, 0) != 0)
                return -1;

            // UV plane
            index++;
            cmdSS_[index] = cmdInit_;
            if (setSshEntry(cmdSS_[index], resBind.res, index, 1) != 0)
                return -1;
        }
        else if (resBind.numPlane == 1)
        {
            cmdSS_[index] = cmdInit_;
            if (setSshEntry(cmdSS_[index], resBind.res, index, 0) != 0)
                return -1;
        }
        else
        {
            return -1;
        }
    }

    // Copy BT and SS into a single buffer
    sshData_.insert(sshData_.end(), (uint8_t*)&cmdBT_[0], (uint8_t*)&cmdBT_[0] + btRegionSize_);
    sshData_.insert(sshData_.end(), (uint8_t*)&cmdSS_[0], (uint8_t*)&cmdSS_[0] + ssRegionSize_);

    return 0;
}

int32_t SSH::destroy()
{
    return 0;
}

int32_t SSH::setSshEntry(SurfaceState& state, MediaResource* res, int32_t index, bool isUV)
{
    int32_t offset = 0;
    RES_FORMAT resFormat = res->getFormat();

    if (resFormat == RES_FORMAT_NV12 || resFormat == RES_FORMAT_P010)
    {
        // Set binding table entry
        offset = btRegionSize_ + index * ssEntrySize_;
        cmdBT_[index].DW0.SurfaceStatePointer = (offset) >> 6;

        // Set surface state
        state.DW0.SurfaceType = 1; // SURFACE_TYPE_SURFTYPE2D
        state.DW0.SurfaceFormat = (isUV) ? GFX_FORMAT_R8G8_UNORM : GFX_FORMAT_R8_UNORM;;
        state.DW0.TileMode = (res->getTileType() == TILE_TYPE_Y) ? 3 : 0;
        state.DW0.VerticalLineStride = 0;
        state.DW0.VerticalLineStrideOffset = 0;
        state.DW0.SurfaceHorizontalAlignment = 1;
        state.DW0.SurfaceVerticalAlignment = 1;

        uint32_t height = (isUV) ? res->getHeight() / 2 : res->getHeight();
        state.DW1.MemoryObjectControlState = 2; // APGTODO
        state.DW1.SurfaceQpitch = 0;
        state.DW2.Width = res->getWidth() / 4 - 1; // Width in unit of DWORD
        state.DW2.Height = height - 1;
        state.DW3.SurfacePitch = res->getPitch() - 1;
        state.DW3.Depth = res->getDepth() - 1;
    }
    else if (resFormat == RES_FORMAT_P8 || resFormat == RES_FORMAT_Buffer || resFormat == RES_FORMAT_Buffer_2D)
    {
        state.DW0.SurfaceType = 4; // SURFACE_TYPE_SURFTYPEBUFFER
        state.DW0.SurfaceFormat = GFX_FORMAT_R32_UINT; // bRawSurface == false
        state.DW0.TileMode = 0; //TILE_MODE_LINEAR;
        state.DW0.VerticalLineStride = 0;
        state.DW0.VerticalLineStrideOffset = 0;
        state.DW0.SurfaceHorizontalAlignment = 1;
        state.DW0.SurfaceVerticalAlignment = 1;

        state.DW1.MemoryObjectControlState = 2; // APGTODO

        // Buffer resources - use original width/height/pitch/depth
        uint32_t bufSize = res->getSize();
        state.DW2.Width = (bufSize - 1) & 0x7F;;
        state.DW2.Height = ((bufSize - 1) & 0x1FFF80) >> 7;
        state.DW3.Depth = ((bufSize - 1) & 0xFE00000) >> 21;
        state.DW3.SurfacePitch = sizeof(uint32_t) - 1; // bRawSurface == false
    }
    else
    {
        return -1;
    }

    state.DW4.RenderTargetAndSampleUnormRotation = 0; // pParams->RotationMode;

    state.DW5.XOffset = 0; // pParams->iXOffset >> 2; // In multiples of 4 (low two bits missing).
    state.DW5.YOffset = 0; // pParams->iYOffset >> 2; // For S3D, the Yoffset of right-view surface in rerder target may be non-zero

    state.DW6.Obj1.SeparateUvPlaneEnable = 0; // pParams->bSeperateUVPlane;
    state.DW6.Obj1.XOffsetForUOrUvPlane = 0; // pParams->dwXOffsetForU;
    state.DW6.Obj1.YOffsetForUOrUvPlane = 0; // pParams->dwYOffsetForU;

    state.DW7.MemoryCompressionEnable = 0; // pParams->bCompressionEnabled;
    state.DW7.MemoryCompressionMode = 0; // pParams->bCompressionMode;
    state.DW7.ShaderChannelSelectAlpha = mhw_state_heap_g9_X::RENDER_SURFACE_STATE_CMD::SHADER_CHANNEL_SELECT_ALPHA_ALPHA;
    state.DW7.ShaderChannelSelectBlue = mhw_state_heap_g9_X::RENDER_SURFACE_STATE_CMD::SHADER_CHANNEL_SELECT_BLUE_BLUE;
    state.DW7.ShaderChannelSelectGreen = mhw_state_heap_g9_X::RENDER_SURFACE_STATE_CMD::SHADER_CHANNEL_SELECT_GREEN_GREEN;
    state.DW7.ShaderChannelSelectRed = mhw_state_heap_g9_X::RENDER_SURFACE_STATE_CMD::SHADER_CHANNEL_SELECT_RED_RED;

    uint64_t address = res->getCannoizedGpuAddress();
    uint32_t offsetUV = (isUV) ? res->getPitch() * res->getHeight() : 0;
    setGpuAddress(&state.DW8_9.Value[0], &state.DW8_9.Value[1], address, offsetUV, 0);

    state.DW10_11.Obj3.XOffsetForVPlane = 0; // pParams->dwXOffsetForV;
    state.DW10_11.Obj3.YOffsetForVPlane = 0; // pParams->dwYOffsetForV;

    return 0;
}

DSH::DSH() :
    cmdID_()
{
}

DSH::~DSH()
{
    if (pRes_)
    {
        delete pRes_;
    }
}

int32_t DSH::initialize(uint8_t* pCurbe, int32_t curbeSize)
{
    if (pCurbe == nullptr)
    {
        return -1;
    }

    // Set CURBE data (need to handle alignment)
    int32_t alignedSize = UTIL_ALIGN(curbeSize, curbeSizeAlignment_);
    curbe_.assign(alignedSize, 0);
    for (int32_t i = 0; i < curbeSize; i++)
    {
        curbe_[i] = *(pCurbe + i);
    }

    // Set Interface Descriptor parameters

    // Specifies the 64-byte aligned address offset of the first instruction in the kernel.
    // This pointer is relative to the Instruction Base Address.
    cmdID_.DW0.KernelStartPointer = 0;
    cmdID_.DW1.KernelStartPointerHigh = 0;
    cmdID_.DW3.SamplerStatePointer = 0;
    cmdID_.DW3.SamplerCount = 0;
    cmdID_.DW4.BindingTablePointer = 0; // since only has one BT, the pointer is 0 by default
    cmdID_.DW5.ConstantUrbEntryReadOffset = 0; // default 0
    cmdID_.DW5.ConstantIndirectUrbEntryReadLength = curbe_.size();
    cmdID_.DW6.GlobalBarrierEnable = 0;
    cmdID_.DW6.BarrierEnable = 0;
    cmdID_.DW6.NumberOfThreadsInGpgpuThreadGroup = 1; // APGTODO: Need to check
    cmdID_.DW6.SharedLocalMemorySize = 0;
    cmdID_.DW7.CrossThreadConstantDataReadLength = 0;

    // Curbe data offset requires alignment, so extend ID region
    idRegionSize_ = UTIL_ALIGN(cmdID_.byteSize, curbeOffsetAlignment_);

    // Combine ID and CURBE into a single DSH buffer
    dshData_.insert(dshData_.end(), (uint8_t*)&cmdID_, (uint8_t*)&cmdID_ + cmdID_.byteSize);
    dshData_.insert(dshData_.end(), idRegionSize_ - cmdID_.byteSize, 0); // ID padding region if any
    dshData_.insert(dshData_.end(), curbe_.begin(), curbe_.end());

    // Write DSH data into a resource
    uint32_t alignedDshSize = UTIL_ALIGN(dshData_.size(), dshAlignment_);
    pRes_ = new MediaResource(RES_TYPE_BUFFER, RES_FORMAT_Buffer, alignedDshSize, false, "DSHBuffer");
    if (pRes_->create() != 0)
    {
        return -1;
    }
    if (pRes_->writeData(&dshData_[0], dshData_.size()) != 0)
    {
        return -1;
    }

    return 0;
}

int32_t DSH::destroy()
{
    int32_t result = 0;

    if (pRes_)
    {
        result = pRes_->destroy();
        delete pRes_;
        pRes_ = nullptr;
    }

    return result;
}

ISH::ISH()
{
}

ISH::~ISH()
{
    if (pRes_)
    {
        delete pRes_;
    }
}

int32_t ISH::initialize(std::vector<uint8_t>& genISA)
{
    uint32_t alignedIshSize = UTIL_ALIGN(genISA.size(), ishAlignment_);
    pRes_ = new MediaResource(RES_TYPE_BUFFER, RES_FORMAT_Buffer, alignedIshSize, false, "ISHBuffer");

    if (pRes_->create() != 0)
    {
        return -1;
    }

    if (pRes_->writeData(&genISA[0], genISA.size()) != 0)
    {
        return -1;
    }

    return 0;
}

int32_t ISH::destroy()
{
    int32_t result = 0;

    if (pRes_)
    {
        result = pRes_->destroy();
        delete pRes_;
        pRes_ = nullptr;
    }

    return result;
}

}