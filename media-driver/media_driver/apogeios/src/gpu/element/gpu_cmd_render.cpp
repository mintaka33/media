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
//! \file     gpu_cmd_render.cpp
//! \brief    gpu Render commands implementation
//! \details
//!

#include "common.h"
#include "gpu_cmd_render.h"
#include "mhw_vdbox.h"

CmdPipelineSelect::CmdPipelineSelect(PIPELINE_SELECTION pipelineSelection)
    : GpuCmdRender(),
    pipelineSelection_(pipelineSelection)
{
    initCmd();
}

void CmdPipelineSelect::initCmd()
{
    pCmd_->DW0.PipelineSelection = pipelineSelection_;

    pCmd_->DW0.MaskBits = 0x13;
}

bool CmdPipelineSelect::validate()
{
    return true;
}

void CmdPipelineSelect::parse()
{
    LOG_CMD_NAME(typeid(pCmd_).name());

    dumpCmdBinary((uint32_t*)pCmd_, cmdSize_ / 4);

    LOG_CMD_HEX_DW(pCmd_->DW0.Value);
    LOG_CMD_SUB_VAL(pCmd_->DW0.PipelineSelection);
    LOG_CMD_SUB_VAL(pCmd_->DW0.RenderSliceCommonPowerGateEnable);
    LOG_CMD_SUB_VAL(pCmd_->DW0.RenderSamplerPowerGateEnable);
    LOG_CMD_SUB_VAL(pCmd_->DW0.MediaSamplerDopClockGateEnable);
    LOG_CMD_SUB_VAL(pCmd_->DW0.ForceMediaAwake);
    LOG_CMD_SUB_VAL(pCmd_->DW0.Reserved6);
    LOG_CMD_SUB_VAL(pCmd_->DW0.MaskBits);
    LOG_CMD_SUB_VAL(pCmd_->DW0.Command3DSubOpcode);
    LOG_CMD_SUB_VAL(pCmd_->DW0.Command3DOpcode);
    LOG_CMD_SUB_VAL(pCmd_->DW0.CommandSubtype);
    LOG_CMD_SUB_VAL(pCmd_->DW0.CommandType);

    logCmdFlush();
}

CmdStateBaseAddress::CmdStateBaseAddress()
    : GpuCmdRender()
{
    // set default parameters
}

int32_t CmdStateBaseAddress::addGSH(MediaResource *pRes, uint32_t offset)
{
    // GSH is optional
    if (pRes != nullptr)
    {
        // set GPU virtual address and offset
        uint64_t address = pRes->getCannoizedGpuAddress();
        setGpuAddress(&pCmd_->DW1_2.Value[0], &pCmd_->DW1_2.Value[1], address, offset, 12);

        pCmd_->DW1_2.GeneralStateBaseAddressModifyEnable = true;
        pCmd_->DW12.GeneralStateBufferSizeModifyEnable = true;

        pCmd_->DW12.GeneralStateBufferSize = (pRes->getSize() + MHW_PAGE_SIZE - 1) / MHW_PAGE_SIZE;

        addResource(pRes);
        pSurfaceState_ = pRes;
    }

    return 0;
}

int32_t CmdStateBaseAddress::addSSH(MediaResource *pRes, uint32_t offset)
{
    if (pRes == nullptr)
        return -1;

    // set GPU virtual address and offset
    uint64_t address = pRes->getCannoizedGpuAddress();
    setGpuAddress(&pCmd_->DW4_5.Value[0], &pCmd_->DW4_5.Value[1], address, offset, 12);

    pCmd_->DW4_5.SurfaceStateBaseAddressModifyEnable = true;

    addResource(pRes);
    pSurfaceState_ = pRes;

    return 0;
}

int32_t CmdStateBaseAddress::addDSH(MediaResource *pRes, uint32_t offset)
{
    if (pRes == nullptr)
        return -1;

    // set GPU virtual address and offset
    uint64_t address = pRes->getCannoizedGpuAddress();
    setGpuAddress(&pCmd_->DW6_7.Value[0], &pCmd_->DW6_7.Value[1], address, offset, 12);

    pCmd_->DW6_7.DynamicStateBaseAddressModifyEnable = true;
    pCmd_->DW13.DynamicStateBufferSizeModifyEnable = true;

    pCmd_->DW13.DynamicStateBufferSize = (pRes->getSize() + MHW_PAGE_SIZE - 1) / MHW_PAGE_SIZE;

    addResource(pRes);
    pDynamicState_ = pRes;

    return 0;
}

int32_t CmdStateBaseAddress::addISH(MediaResource *pRes, uint32_t offset)
{
    if (pRes == nullptr)
        return -1;

    // set GPU virtual address and offset
    uint64_t address = pRes->getCannoizedGpuAddress();
    setGpuAddress(&pCmd_->DW10_11.Value[0], &pCmd_->DW10_11.Value[1], address, offset, 12);

    pCmd_->DW10_11.InstructionBaseAddressModifyEnable = true;
    pCmd_->DW15.InstructionBufferSizeModifyEnable = true;

    pCmd_->DW15.InstructionBufferSize = (pRes->getSize() + MHW_PAGE_SIZE - 1) / MHW_PAGE_SIZE;

    addResource(pRes);
    pInstructionState_ = pRes;

    return 0;
}

int32_t CmdStateBaseAddress::addIndObj(MediaResource *pRes, uint32_t offset)
{
    // Indirect object is optional
    if (pRes != nullptr)
    {
        // set GPU virtual address and offset
        uint64_t address = pRes->getCannoizedGpuAddress();
        setGpuAddress(&pCmd_->DW8_9.Value[0], &pCmd_->DW8_9.Value[1], address, offset, 12);

        pCmd_->DW8_9.IndirectObjectBaseAddressModifyEnable = true;
        pCmd_->DW14.IndirectObjectBufferSizeModifyEnable = true;

        pCmd_->DW14.IndirectObjectBufferSize = (pRes->getSize() + MHW_PAGE_SIZE - 1) / MHW_PAGE_SIZE;

        addResource(pRes);
        pIndirectObject_ = pRes;
    }

    return 0;
}

bool CmdStateBaseAddress::validate()
{
    return true;
}

void CmdStateBaseAddress::parse()
{
    LOG_CMD_NAME(typeid(pCmd_).name());

    dumpCmdBinary((uint32_t*)pCmd_, cmdSize_ / 4);

    LOG_CMD_HEX_DW(pCmd_->DW0.Value);
    LOG_CMD_SUB_VAL(pCmd_->DW0.DwordLength);
    LOG_CMD_SUB_VAL(pCmd_->DW0.Command3DSubOpcode);
    LOG_CMD_SUB_VAL(pCmd_->DW0.Command3DOpcode);
    LOG_CMD_SUB_VAL(pCmd_->DW0.CommandSubtype);
    LOG_CMD_SUB_VAL(pCmd_->DW0.CommandType);

    LOG_CMD_HEX_DW(pCmd_->DW1_2.Value[0]);
    LOG_CMD_HEX_DW(pCmd_->DW1_2.Value[1]);
    LOG_CMD_SUB_VAL(pCmd_->DW1_2.GeneralStateBaseAddressModifyEnable);
    LOG_CMD_SUB_VAL(pCmd_->DW1_2.GeneralStateMemoryObjectControlState);
    LOG_CMD_SUB_VAL(pCmd_->DW1_2.GeneralStateBaseAddress);

    LOG_CMD_HEX_DW(pCmd_->DW3.Value);
    LOG_CMD_SUB_VAL(pCmd_->DW3.StatelessDataPortAccessMemoryObjectControlState);

    LOG_CMD_HEX_DW(pCmd_->DW4_5.Value[0]);
    LOG_CMD_HEX_DW(pCmd_->DW4_5.Value[1]);
    LOG_CMD_SUB_VAL(pCmd_->DW4_5.SurfaceStateBaseAddressModifyEnable);
    LOG_CMD_SUB_VAL(pCmd_->DW4_5.SurfaceStateMemoryObjectControlState);
    LOG_CMD_SUB_VAL(pCmd_->DW4_5.SurfaceStateBaseAddress);

    LOG_CMD_HEX_DW(pCmd_->DW6_7.Value[0]);
    LOG_CMD_HEX_DW(pCmd_->DW6_7.Value[1]);
    LOG_CMD_SUB_VAL(pCmd_->DW6_7.DynamicStateBaseAddressModifyEnable);
    LOG_CMD_SUB_VAL(pCmd_->DW6_7.DynamicStateMemoryObjectControlState);
    LOG_CMD_SUB_VAL(pCmd_->DW6_7.DynamicStateBaseAddress);

    LOG_CMD_HEX_DW(pCmd_->DW8_9.Value[0]);
    LOG_CMD_HEX_DW(pCmd_->DW8_9.Value[1]);
    LOG_CMD_SUB_VAL(pCmd_->DW8_9.IndirectObjectBaseAddressModifyEnable);
    LOG_CMD_SUB_VAL(pCmd_->DW8_9.IndirectObjectMemoryObjectControlState);
    LOG_CMD_SUB_VAL(pCmd_->DW8_9.IndirectObjectBaseAddress);

    LOG_CMD_HEX_DW(pCmd_->DW10_11.Value[0]);
    LOG_CMD_HEX_DW(pCmd_->DW10_11.Value[1]);
    LOG_CMD_SUB_VAL(pCmd_->DW10_11.InstructionBaseAddressModifyEnable);
    LOG_CMD_SUB_VAL(pCmd_->DW10_11.InstructionMemoryObjectControlState);
    LOG_CMD_SUB_VAL(pCmd_->DW10_11.InstructionBaseAddress);

    LOG_CMD_HEX_DW(pCmd_->DW12.Value);
    LOG_CMD_SUB_VAL(pCmd_->DW12.GeneralStateBufferSizeModifyEnable);
    LOG_CMD_SUB_VAL(pCmd_->DW12.GeneralStateBufferSize);

    LOG_CMD_HEX_DW(pCmd_->DW13.Value);
    LOG_CMD_SUB_VAL(pCmd_->DW13.DynamicStateBufferSizeModifyEnable);
    LOG_CMD_SUB_VAL(pCmd_->DW13.DynamicStateBufferSize);

    LOG_CMD_HEX_DW(pCmd_->DW14.Value);
    LOG_CMD_SUB_VAL(pCmd_->DW14.IndirectObjectBufferSizeModifyEnable);
    LOG_CMD_SUB_VAL(pCmd_->DW14.IndirectObjectBufferSize);

    LOG_CMD_HEX_DW(pCmd_->DW15.Value);
    LOG_CMD_SUB_VAL(pCmd_->DW15.InstructionBufferSizeModifyEnable);
    LOG_CMD_SUB_VAL(pCmd_->DW15.InstructionBufferSize);

    LOG_CMD_HEX_DW(pCmd_->DW16_17.Value[0]);
    LOG_CMD_HEX_DW(pCmd_->DW16_17.Value[1]);
    LOG_CMD_SUB_VAL(pCmd_->DW16_17.BindlessSurfaceStateBaseAddressModifyEnable);
    LOG_CMD_SUB_VAL(pCmd_->DW16_17.BindlessSurfaceStateMemoryObjectControlState);
    LOG_CMD_SUB_VAL(pCmd_->DW16_17.BindlessSurfaceStateBaseAddress);

    LOG_CMD_HEX_DW(pCmd_->DW18.Value);
    LOG_CMD_SUB_VAL(pCmd_->DW18.BindlessSurfaceStateSize);

    logCmdFlush();
}

void    CmdMediaVfeState::initHwCaps()
{
    //Hardcode based on SKL GT2 configurations
    hwCaps_.dwMaxUnormSamplers = MHW_RENDER_ENGINE_SAMPLERS_MAX;
    hwCaps_.dwMaxAVSSamplers = MHW_RENDER_ENGINE_SAMPLERS_AVS_MAX;
    hwCaps_.dwMaxBTIndex = MHW_RENDER_ENGINE_SSH_SURFACES_PER_BT_MAX - 1;
    hwCaps_.dwMaxThreads = 168;// gtSystemInfo->ThreadCount;
    hwCaps_.dwMaxMediaPayloadSize = MHW_RENDER_ENGINE_MEDIA_PALOAD_SIZE_MAX;
    hwCaps_.dwMaxURBSize = MHW_RENDER_ENGINE_URB_SIZE_MAX;
    hwCaps_.dwMaxURBEntries = MHW_RENDER_ENGINE_URB_ENTRIES_MAX;
    hwCaps_.dwMaxSubslice = 3;// gtSystemInfo->MaxSubSlicesSupported;
    hwCaps_.dwMaxEUIndex = MHW_RENDER_ENGINE_EU_INDEX_MAX;
    hwCaps_.dwNumThreadsPerEU = 7;// (gtSystemInfo->EUCount > 0) ? gtSystemInfo->ThreadCount / gtSystemInfo->EUCount : 0;
    hwCaps_.dwSizeRegistersPerThread = MHW_RENDER_ENGINE_SIZE_REGISTERS_PER_THREAD;

    hwCaps_.dwMaxInterfaceDescriptorEntries = MHW_RENDER_ENGINE_INTERFACE_DESCRIPTOR_ENTRIES_MAX;
    hwCaps_.dwMaxURBEntryAllocationSize =
        hwCaps_.dwMaxCURBEAllocationSize =
        hwCaps_.dwMaxURBSize - hwCaps_.dwMaxInterfaceDescriptorEntries;
}

MOS_STATUS CmdMediaVfeState::setParamsToCmd()
{
    if (vfeParams_.dwNumberofURBEntries == 0)
    {
        MHW_ASSERTMESSAGE("Parameter dwNumberofURBEntries is 0 will cause divided by zero.");
        return MOS_STATUS_INVALID_PARAMETER;
    }

    if (vfeParams_.dwPerThreadScratchSpace)
    {
        pCmd_->DW1.PerThreadScratchSpace = vfeParams_.dwPerThreadScratchSpace;
        pCmd_->DW1.ScratchSpaceBasePointer = vfeParams_.dwScratchSpaceBasePointer >> 10;
        pCmd_->DW2.ScratchSpaceBasePointerHigh = 0;
    }
    pCmd_->DW3.MaximumNumberOfThreads = (vfeParams_.dwMaximumNumberofThreads) ?
        vfeParams_.dwMaximumNumberofThreads - 1 : hwCaps_.dwMaxThreads - 1;
    pCmd_->DW3.NumberOfUrbEntries = vfeParams_.dwNumberofURBEntries;
    pCmd_->DW5.CurbeAllocationSize = vfeParams_.dwCURBEAllocationSize >> 5;
    pCmd_->DW5.UrbEntryAllocationSize = (vfeParams_.dwURBEntryAllocationSize) ?
        vfeParams_.dwURBEntryAllocationSize :
        (hwCaps_.dwMaxURBSize -
            pCmd_->DW5.CurbeAllocationSize -
            hwCaps_.dwMaxInterfaceDescriptorEntries) /
        vfeParams_.dwNumberofURBEntries;

    if ((pCmd_->DW3.NumberOfUrbEntries > hwCaps_.dwMaxURBEntries) ||
        (pCmd_->DW5.CurbeAllocationSize > hwCaps_.dwMaxCURBEAllocationSize) ||
        (pCmd_->DW5.UrbEntryAllocationSize > hwCaps_.dwMaxURBEntryAllocationSize) ||
        (pCmd_->DW3.NumberOfUrbEntries * pCmd_->DW5.UrbEntryAllocationSize +
            pCmd_->DW5.CurbeAllocationSize +
            hwCaps_.dwMaxInterfaceDescriptorEntries > hwCaps_.dwMaxURBSize))
    {
        MHW_ASSERTMESSAGE("Parameters requested exceed maximum supported by HW.");
        return MOS_STATUS_INVALID_PARAMETER;
    }

    pCmd_->DW6.ScoreboardType = vfeParams_.Scoreboard.ScoreboardType;
    pCmd_->DW6.ScoreboardMask = vfeParams_.Scoreboard.ScoreboardMask;
    pCmd_->DW6.ScoreboardEnable = vfeParams_.Scoreboard.ScoreboardEnable;

    pCmd_->DW7.Scoreboard0DeltaX = vfeParams_.Scoreboard.ScoreboardDelta[0].x;
    pCmd_->DW7.Scoreboard0DeltaY = vfeParams_.Scoreboard.ScoreboardDelta[0].y;
    pCmd_->DW7.Scoreboard1DeltaX = vfeParams_.Scoreboard.ScoreboardDelta[1].x;
    pCmd_->DW7.Scoreboard1DeltaY = vfeParams_.Scoreboard.ScoreboardDelta[1].y;
    pCmd_->DW7.Scoreboard2DeltaX = vfeParams_.Scoreboard.ScoreboardDelta[2].x;
    pCmd_->DW7.Scoreboard2DeltaY = vfeParams_.Scoreboard.ScoreboardDelta[2].y;
    pCmd_->DW7.Scoreboard3DeltaX = vfeParams_.Scoreboard.ScoreboardDelta[3].x;
    pCmd_->DW7.Scoreboard3DeltaY = vfeParams_.Scoreboard.ScoreboardDelta[3].y;

    pCmd_->DW8.Scoreboard4DeltaX = vfeParams_.Scoreboard.ScoreboardDelta[4].x;
    pCmd_->DW8.Scoreboard4DeltaY = vfeParams_.Scoreboard.ScoreboardDelta[4].y;
    pCmd_->DW8.Scoreboard5DeltaX = vfeParams_.Scoreboard.ScoreboardDelta[5].x;
    pCmd_->DW8.Scoreboard5DeltaY = vfeParams_.Scoreboard.ScoreboardDelta[5].y;
    pCmd_->DW8.Scoreboard6DeltaX = vfeParams_.Scoreboard.ScoreboardDelta[6].x;
    pCmd_->DW8.Scoreboard6DeltaY = vfeParams_.Scoreboard.ScoreboardDelta[6].y;
    pCmd_->DW8.Scoreboard7DeltaX = vfeParams_.Scoreboard.ScoreboardDelta[7].x;
    pCmd_->DW8.Scoreboard7DeltaY = vfeParams_.Scoreboard.ScoreboardDelta[7].y;

    return MOS_STATUS_SUCCESS;
}

MOS_STATUS CmdMediaVfeState::initCmd(const PMHW_VFE_PARAMS pVfeParams)
{
    if (pVfeParams == nullptr)
    {
        return MOS_STATUS_INVALID_PARAMETER;
    }

    vfeParams_ = *pVfeParams;

    return setParamsToCmd();
}

CmdMediaVfeState::CmdMediaVfeState()
    : GpuCmdRender()
{
    initHwCaps();
}

CmdMediaVfeState::CmdMediaVfeState(uint32_t curbeSize)
{
    pCmd_->DW3.MaximumNumberOfThreads = 0xa7; // APGTODO: need use HWCaps to calculate
    pCmd_->DW3.NumberOfUrbEntries = 0x20;
    pCmd_->DW5.CurbeAllocationSize = curbeSize >> 5;
    pCmd_->DW5.UrbEntryAllocationSize = 1;
    pCmd_->DW6.ScoreboardEnable = 1;
    pCmd_->DW6.ScoreboardType = 1;
    pCmd_->DW6.ScoreboardMask = 0;
}

bool CmdMediaVfeState::validate()
{
    return true;
}

void CmdMediaVfeState::parse()
{
    LOG_CMD_NAME(typeid(pCmd_).name());

    dumpCmdBinary((uint32_t*)pCmd_, cmdSize_ / 4);

    LOG_CMD_HEX_DW(pCmd_->DW0.Value);
    LOG_CMD_SUB_VAL(pCmd_->DW0.DwordLength);
    LOG_CMD_SUB_VAL(pCmd_->DW0.Subopcode);
    LOG_CMD_SUB_VAL(pCmd_->DW0.MediaCommandOpcode);
    LOG_CMD_SUB_VAL(pCmd_->DW0.Pipeline);
    LOG_CMD_SUB_VAL(pCmd_->DW0.CommandType);

    LOG_CMD_HEX_DW(pCmd_->DW1.Value);
    LOG_CMD_SUB_VAL(pCmd_->DW1.PerThreadScratchSpace);
    LOG_CMD_SUB_VAL(pCmd_->DW1.StackSize);
    LOG_CMD_SUB_VAL(pCmd_->DW1.ScratchSpaceBasePointer);

    LOG_CMD_HEX_DW(pCmd_->DW2.Value);
    LOG_CMD_SUB_VAL(pCmd_->DW2.ScratchSpaceBasePointerHigh);

    LOG_CMD_HEX_DW(pCmd_->DW3.Value);
    LOG_CMD_SUB_VAL(pCmd_->DW3.ResetGatewayTimer);
    LOG_CMD_SUB_VAL(pCmd_->DW3.NumberOfUrbEntries);
    LOG_CMD_SUB_VAL(pCmd_->DW3.MaximumNumberOfThreads);

    LOG_CMD_HEX_DW(pCmd_->DW4.Value);
    LOG_CMD_SUB_VAL(pCmd_->DW4.SliceDisable);

    LOG_CMD_HEX_DW(pCmd_->DW5.Value);
    LOG_CMD_SUB_VAL(pCmd_->DW5.CurbeAllocationSize);
    LOG_CMD_SUB_VAL(pCmd_->DW5.UrbEntryAllocationSize);

    LOG_CMD_HEX_DW(pCmd_->DW6.Value);
    LOG_CMD_SUB_VAL(pCmd_->DW6.ScoreboardMask);
    LOG_CMD_SUB_VAL(pCmd_->DW6.ScoreboardType);
    LOG_CMD_SUB_VAL(pCmd_->DW6.ScoreboardEnable);

    LOG_CMD_HEX_DW(pCmd_->DW7.Value);
    LOG_CMD_SUB_VAL(pCmd_->DW7.Scoreboard0DeltaX);
    LOG_CMD_SUB_VAL(pCmd_->DW7.Scoreboard0DeltaY);
    LOG_CMD_SUB_VAL(pCmd_->DW7.Scoreboard1DeltaX);
    LOG_CMD_SUB_VAL(pCmd_->DW7.Scoreboard1DeltaY);
    LOG_CMD_SUB_VAL(pCmd_->DW7.Scoreboard2DeltaX);
    LOG_CMD_SUB_VAL(pCmd_->DW7.Scoreboard2DeltaY);
    LOG_CMD_SUB_VAL(pCmd_->DW7.Scoreboard3DeltaX);
    LOG_CMD_SUB_VAL(pCmd_->DW7.Scoreboard3DeltaY);

    LOG_CMD_HEX_DW(pCmd_->DW8.Value);
    LOG_CMD_SUB_VAL(pCmd_->DW8.Scoreboard4DeltaX);
    LOG_CMD_SUB_VAL(pCmd_->DW8.Scoreboard4DeltaY);
    LOG_CMD_SUB_VAL(pCmd_->DW8.Scoreboard5DeltaX);
    LOG_CMD_SUB_VAL(pCmd_->DW8.Scoreboard5DeltaY);
    LOG_CMD_SUB_VAL(pCmd_->DW8.Scoreboard6DeltaX);
    LOG_CMD_SUB_VAL(pCmd_->DW8.Scoreboard6DeltaY);
    LOG_CMD_SUB_VAL(pCmd_->DW8.Scoreboard7DeltaX);
    LOG_CMD_SUB_VAL(pCmd_->DW8.Scoreboard7DeltaY);

    logCmdFlush();
}

CmdMediaCurbeLoad::CmdMediaCurbeLoad(uint32_t length, uint32_t address)
    : GpuCmdRender(),
    curbeLength_(length),
    curbeStartAddress_(address)
{
    // Alignment requirement: https://gfxspecs.intel.com/Predator/Home/Index/20319
    pCmd_->DW2.CurbeTotalDataLength = (uint32_t)MOS_ALIGN_CEIL(length, 64);
    pCmd_->DW3.CurbeDataStartAddress = (uint32_t)MOS_ALIGN_CEIL(address, 64);
}

bool CmdMediaCurbeLoad::validate()
{
    if ((curbeLength_ > MAX_CURBE_DATA_SIZE) || (curbeLength_ <= 0))
    {
        return false;
    }
    else
    {
        return true;
    }
}

void CmdMediaCurbeLoad::parse()
{
    LOG_CMD_NAME(typeid(pCmd_).name());

    dumpCmdBinary((uint32_t*)pCmd_, cmdSize_ / 4);

    LOG_CMD_HEX_DW(pCmd_->DW0.Value);
    LOG_CMD_SUB_VAL(pCmd_->DW0.DwordLength);
    LOG_CMD_SUB_VAL(pCmd_->DW0.Subopcode);
    LOG_CMD_SUB_VAL(pCmd_->DW0.MediaCommandOpcode);
    LOG_CMD_SUB_VAL(pCmd_->DW0.Pipeline);
    LOG_CMD_SUB_VAL(pCmd_->DW0.CommandType);

    LOG_CMD_HEX_DW(pCmd_->DW2.Value);
    LOG_CMD_SUB_VAL(pCmd_->DW2.CurbeTotalDataLength);

    LOG_CMD_HEX_DW(pCmd_->DW3.Value);
    LOG_CMD_SUB_VAL(pCmd_->DW3.CurbeDataStartAddress);

    logCmdFlush();
}

CmdMediaInterfaceDescriptorLoad::CmdMediaInterfaceDescriptorLoad(uint32_t length, uint32_t address)
    : GpuCmdRender(),
    MIDDLength_(length),
    MIDDStartAddress_(address)
{
    // Alginment requirement: https://gfxspecs.intel.com/Predator/Home/Index/6688
    pCmd_->DW2.InterfaceDescriptorTotalLength = (uint32_t)MOS_ALIGN_CEIL(length, 32);
    pCmd_->DW3.InterfaceDescriptorDataStartAddress = (uint32_t)MOS_ALIGN_CEIL(address, 64);
}

bool CmdMediaInterfaceDescriptorLoad::validate()
{
    if ((MIDDLength_ <= 0) || (MIDDLength_ > MAX_INTERFACE_DESCRIPTOR_DATA_SIZE))
    {
        return false;
    }
    else
    {
        return true;
    }
}

void CmdMediaInterfaceDescriptorLoad::parse()
{
    LOG_CMD_NAME(typeid(pCmd_).name());

    dumpCmdBinary((uint32_t*)pCmd_, cmdSize_ / 4);

    LOG_CMD_HEX_DW(pCmd_->DW0.Value);
    LOG_CMD_SUB_VAL(pCmd_->DW0.DwordLength);
    LOG_CMD_SUB_VAL(pCmd_->DW0.Subopcode);
    LOG_CMD_SUB_VAL(pCmd_->DW0.MediaCommandOpcode);
    LOG_CMD_SUB_VAL(pCmd_->DW0.Pipeline);
    LOG_CMD_SUB_VAL(pCmd_->DW0.CommandType);

    LOG_CMD_HEX_DW(pCmd_->DW2.Value);
    LOG_CMD_SUB_VAL(pCmd_->DW2.InterfaceDescriptorTotalLength);

    LOG_CMD_HEX_DW(pCmd_->DW3.Value);
    LOG_CMD_SUB_VAL(pCmd_->DW3.InterfaceDescriptorDataStartAddress);

    logCmdFlush();
}

MOS_STATUS CmdMediaObjectWalker::setParamsToCmd()
{
    if (walkerParams_.pInlineData)
    {
        pCmd_->DW0.DwordLength =
            mhw_render_g9_X::GetOpLength(pCmd_->dwSize + walkerParams_.InlineDataLength / sizeof(uint32_t));
    }

    pCmd_->DW1.InterfaceDescriptorOffset = walkerParams_.InterfaceDescriptorOffset;
    pCmd_->DW2.UseScoreboard = walkerParams_.UseScoreboard;
    pCmd_->DW5.ScoreboardMask = walkerParams_.ScoreboardMask;
    pCmd_->DW5.GroupIdLoopSelect = walkerParams_.GroupIdLoopSelect;
    pCmd_->DW6.ColorCountMinusOne = walkerParams_.ColorCountMinusOne;
    pCmd_->DW6.MiddleLoopExtraSteps = walkerParams_.MiddleLoopExtraSteps;
    pCmd_->DW6.MidLoopUnitX = walkerParams_.MidLoopUnitX;
    pCmd_->DW6.LocalMidLoopUnitY = walkerParams_.MidLoopUnitY;
    pCmd_->DW7.LocalLoopExecCount = walkerParams_.dwLocalLoopExecCount;
    pCmd_->DW7.GlobalLoopExecCount = walkerParams_.dwGlobalLoopExecCount;
    pCmd_->DW8.BlockResolutionX = walkerParams_.BlockResolution.x;
    pCmd_->DW8.BlockResolutionY = walkerParams_.BlockResolution.y;
    pCmd_->DW9.LocalStartX = walkerParams_.LocalStart.x;
    pCmd_->DW9.LocalStartY = walkerParams_.LocalStart.y;
    pCmd_->DW11.LocalOuterLoopStrideX = walkerParams_.LocalOutLoopStride.x;
    pCmd_->DW11.LocalOuterLoopStrideY = walkerParams_.LocalOutLoopStride.y;
    pCmd_->DW12.LocalInnerLoopUnitX = walkerParams_.LocalInnerLoopUnit.x;
    pCmd_->DW12.LocalInnerLoopUnitY = walkerParams_.LocalInnerLoopUnit.y;
    pCmd_->DW13.GlobalResolutionX = walkerParams_.GlobalResolution.x;
    pCmd_->DW13.GlobalResolutionY = walkerParams_.GlobalResolution.y;
    pCmd_->DW14.GlobalStartX = walkerParams_.GlobalStart.x;
    pCmd_->DW14.GlobalStartY = walkerParams_.GlobalStart.y;
    pCmd_->DW15.GlobalOuterLoopStrideX = walkerParams_.GlobalOutlerLoopStride.x;
    pCmd_->DW15.GlobalOuterLoopStrideY = walkerParams_.GlobalOutlerLoopStride.y;
    pCmd_->DW16.GlobalInnerLoopUnitX = walkerParams_.GlobalInnerLoopUnit.x;
    pCmd_->DW16.GlobalInnerLoopUnitY = walkerParams_.GlobalInnerLoopUnit.y;

    return MOS_STATUS_SUCCESS;
}

MOS_STATUS CmdMediaObjectWalker::initCmd(const PMHW_WALKER_PARAMS pWalkerParams)
{
    if (pWalkerParams == nullptr)
    {
        return MOS_STATUS_INVALID_PARAMETER;
    }

    walkerParams_ = *pWalkerParams;

    return setParamsToCmd();
}

CmdMediaObjectWalker::CmdMediaObjectWalker()
    : GpuCmdRender()
{
}

CmdMediaObjectWalker::CmdMediaObjectWalker(uint32_t blockWidth, uint32_t blockHeight)
{
    pCmd_->DW1.InterfaceDescriptorOffset = 0;
    pCmd_->DW2.UseScoreboard = 1;
    pCmd_->DW5.ScoreboardMask = 0;
    pCmd_->DW5.GroupIdLoopSelect = 0;
    pCmd_->DW6.ColorCountMinusOne = 0;
    pCmd_->DW6.MiddleLoopExtraSteps = 0;
    pCmd_->DW6.MidLoopUnitX = 0;
    pCmd_->DW6.LocalMidLoopUnitY = 0;
    pCmd_->DW7.LocalLoopExecCount = 14;
    pCmd_->DW7.GlobalLoopExecCount = 1;
    pCmd_->DW8.BlockResolutionX = blockWidth;
    pCmd_->DW8.BlockResolutionY = blockHeight;
    pCmd_->DW9.LocalStartX = 0;
    pCmd_->DW9.LocalStartY = 0;
    pCmd_->DW11.LocalOuterLoopStrideX = 0;
    pCmd_->DW11.LocalOuterLoopStrideY = 1;
    pCmd_->DW12.LocalInnerLoopUnitX = 1;
    pCmd_->DW12.LocalInnerLoopUnitY = 0;
    pCmd_->DW13.GlobalResolutionX = blockWidth;
    pCmd_->DW13.GlobalResolutionY = blockHeight;
    pCmd_->DW14.GlobalStartX = 0;
    pCmd_->DW14.GlobalStartY = 0;
    pCmd_->DW15.GlobalOuterLoopStrideX = blockWidth;
    pCmd_->DW15.GlobalOuterLoopStrideY = 0;
    pCmd_->DW16.GlobalInnerLoopUnitX = 0;
    pCmd_->DW16.GlobalInnerLoopUnitY = blockHeight;
}

bool CmdMediaObjectWalker::validate()
{
    //At least the GlobalResolution (X, Y) should not be ZERO
    if ((pCmd_->DW13.GlobalResolutionX == 0) || (pCmd_->DW13.GlobalResolutionY == 0))
    {
        return false;
    }

    else
    {
        return true;
    }
}

void CmdMediaObjectWalker::parse()
{
    LOG_CMD_NAME(typeid(pCmd_).name());

    dumpCmdBinary((uint32_t*)pCmd_, cmdSize_ / 4);

    LOG_CMD_HEX_DW(pCmd_->DW0.Value);
    LOG_CMD_SUB_VAL(pCmd_->DW0.DwordLength);
    LOG_CMD_SUB_VAL(pCmd_->DW0.Subopcode);
    LOG_CMD_SUB_VAL(pCmd_->DW0.MediaCommandOpcode);
    LOG_CMD_SUB_VAL(pCmd_->DW0.Pipeline);
    LOG_CMD_SUB_VAL(pCmd_->DW0.CommandType);

    LOG_CMD_HEX_DW(pCmd_->DW1.Value);
    LOG_CMD_SUB_VAL(pCmd_->DW1.InterfaceDescriptorOffset);

    LOG_CMD_HEX_DW(pCmd_->DW2.Value);
    LOG_CMD_SUB_VAL(pCmd_->DW2.IndirectDataLength);
    LOG_CMD_SUB_VAL(pCmd_->DW2.UseScoreboard);
    LOG_CMD_SUB_VAL(pCmd_->DW2.MaskedDispatch);
    LOG_CMD_SUB_VAL(pCmd_->DW2.ThreadSynchronization);

    LOG_CMD_HEX_DW(pCmd_->DW3.Value);
    LOG_CMD_SUB_VAL(pCmd_->DW3.IndirectDataStartAddress);

    LOG_CMD_HEX_DW(pCmd_->DW5.Value);
    LOG_CMD_SUB_VAL(pCmd_->DW5.ScoreboardMask);
    LOG_CMD_SUB_VAL(pCmd_->DW5.GroupIdLoopSelect);

    LOG_CMD_HEX_DW(pCmd_->DW6.Value);
    LOG_CMD_SUB_VAL(pCmd_->DW6.MidLoopUnitX);
    LOG_CMD_SUB_VAL(pCmd_->DW6.LocalMidLoopUnitY);
    LOG_CMD_SUB_VAL(pCmd_->DW6.MiddleLoopExtraSteps);
    LOG_CMD_SUB_VAL(pCmd_->DW6.ColorCountMinusOne);

    LOG_CMD_HEX_DW(pCmd_->DW7.Value);
    LOG_CMD_SUB_VAL(pCmd_->DW7.LocalLoopExecCount);
    LOG_CMD_SUB_VAL(pCmd_->DW7.GlobalLoopExecCount);

    LOG_CMD_HEX_DW(pCmd_->DW8.Value);
    LOG_CMD_SUB_VAL(pCmd_->DW8.BlockResolutionX);
    LOG_CMD_SUB_VAL(pCmd_->DW8.BlockResolutionY);

    LOG_CMD_HEX_DW(pCmd_->DW9.Value);
    LOG_CMD_SUB_VAL(pCmd_->DW9.LocalStartX);
    LOG_CMD_SUB_VAL(pCmd_->DW9.LocalStartY);

    LOG_CMD_HEX_DW(pCmd_->DW11.Value);
    LOG_CMD_SUB_VAL(pCmd_->DW11.LocalOuterLoopStrideX);
    LOG_CMD_SUB_VAL(pCmd_->DW11.LocalOuterLoopStrideY);

    LOG_CMD_HEX_DW(pCmd_->DW12.Value);
    LOG_CMD_SUB_VAL(pCmd_->DW12.LocalInnerLoopUnitX);
    LOG_CMD_SUB_VAL(pCmd_->DW12.LocalInnerLoopUnitY);

    LOG_CMD_HEX_DW(pCmd_->DW13.Value);
    LOG_CMD_SUB_VAL(pCmd_->DW13.GlobalResolutionX);
    LOG_CMD_SUB_VAL(pCmd_->DW13.GlobalResolutionY);

    LOG_CMD_HEX_DW(pCmd_->DW14.Value);
    LOG_CMD_SUB_VAL(pCmd_->DW14.GlobalStartX);
    LOG_CMD_SUB_VAL(pCmd_->DW14.GlobalStartY);

    LOG_CMD_HEX_DW(pCmd_->DW15.Value);
    LOG_CMD_SUB_VAL(pCmd_->DW15.GlobalOuterLoopStrideX);
    LOG_CMD_SUB_VAL(pCmd_->DW15.GlobalOuterLoopStrideY);

    LOG_CMD_HEX_DW(pCmd_->DW16.Value);
    LOG_CMD_SUB_VAL(pCmd_->DW16.GlobalInnerLoopUnitX);
    LOG_CMD_SUB_VAL(pCmd_->DW16.GlobalInnerLoopUnitY);

    logCmdFlush();
}