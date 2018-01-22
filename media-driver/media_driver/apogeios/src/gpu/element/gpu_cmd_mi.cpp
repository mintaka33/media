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
//! \file     gpu_cmd_mi.cpp
//! \brief    MI interface command implementation
//! \details
//!

#include "common.h"
#include "gpu_cmd_mi.h"
#include "mhw_vdbox.h"

namespace Apogeios
{

void CmdPipeControl::initCmd(FLUSH_MODE mode)
{
    pCmd_->DW1.PipeControlFlushEnable = true;
    pCmd_->DW1.CommandStreamerStallEnable = true;

    switch (mode)
    {
    case FLUSH_WRITE_CACHE:
        pCmd_->DW1.RenderTargetCacheFlushEnable = true;
        pCmd_->DW1.DcFlushEnable = true;
        break;
    case FLUSH_READ_CACHE:
        pCmd_->DW1.RenderTargetCacheFlushEnable = false;
        pCmd_->DW1.StateCacheInvalidationEnable = true;
        pCmd_->DW1.ConstantCacheInvalidationEnable = true;
        pCmd_->DW1.VfCacheInvalidationEnable = true;
        pCmd_->DW1.InstructionCacheInvalidateEnable = true;
        break;
    case FLUSH_CUSTOM:
        // TODO
        break;
    case FLUSH_NONE:
    default:
        pCmd_->DW1.RenderTargetCacheFlushEnable = false;
        break;
    }

    pCmd_->DW1.StateCacheInvalidationEnable = true;
    pCmd_->DW1.ConstantCacheInvalidationEnable = true;
    pCmd_->DW1.VfCacheInvalidationEnable = true;
    pCmd_->DW1.InstructionCacheInvalidateEnable = true;
    pCmd_->DW1.RenderTargetCacheFlushEnable = true;
}

CmdPipeControl::CmdPipeControl(FLUSH_MODE mode)
    : GpuCmdMI(),
    flushMode_(mode)
{
    initCmd(flushMode_);

    pCmd_->DW1.PostSyncOperation = PSO_NOWRITE;
}

CmdPipeControl::CmdPipeControl(FLUSH_MODE mode, POST_SYNC_OPERATION op)
    : GpuCmdMI(),
    flushMode_(mode),
    postSyncOp_(op)
{
    initCmd(flushMode_);

    pCmd_->DW1.PostSyncOperation = op;
}

CmdPipeControl::CmdPipeControl(FLUSH_MODE mode, POST_SYNC_OPERATION op, MediaResource* pRes, uint32_t offset, uint64_t data)
    : GpuCmdMI(),
    flushMode_(mode),
    postSyncOp_(op),
    pResource_(pRes),
    offset_(offset),
    value_(data)
{
    initCmd(flushMode_);

    pCmd_->DW1.PostSyncOperation = op;

    if (pResource_)
    {
        pCmd_->DW1.StateCacheInvalidationEnable = true;
        pCmd_->DW1.ConstantCacheInvalidationEnable = true;
        pCmd_->DW1.VfCacheInvalidationEnable = true;
        pCmd_->DW1.InstructionCacheInvalidateEnable = true;
        pCmd_->DW1.RenderTargetCacheFlushEnable = true;

        uint64_t address = pResource_->getCannoizedGpuAddress();
        setGpuAddress(&pCmd_->DW2.Value, &pCmd_->DW3.Value, address, offset, 2);

        pCmd_->DW4_5.ImmediateData = data;
    }

    addResource(pRes);
}

bool CmdPipeControl::validate()
{
    return true;
}

void CmdPipeControl::parse()
{
    LOG_CMD_NAME(typeid(pCmd_).name());

    dumpCmdBinary((uint32_t*)pCmd_, cmdSize_ / 4);

    LOG_CMD_HEX_DW(pCmd_->DW0.Value);
    LOG_CMD_SUB_VAL(pCmd_->DW0.DwordLength);
    LOG_CMD_SUB_VAL(pCmd_->DW0.Reserved8);
    LOG_CMD_SUB_VAL(pCmd_->DW0.Command3DSubOpcode);
    LOG_CMD_SUB_VAL(pCmd_->DW0.Command3DOpcode);
    LOG_CMD_SUB_VAL(pCmd_->DW0.CommandSubtype);
    LOG_CMD_SUB_VAL(pCmd_->DW0.CommandType);

    LOG_CMD_HEX_DW(pCmd_->DW1.Value);
    LOG_CMD_SUB_VAL(pCmd_->DW1.DepthCacheFlushEnable);
    LOG_CMD_SUB_VAL(pCmd_->DW1.StallAtPixelScoreboard);
    LOG_CMD_SUB_VAL(pCmd_->DW1.StateCacheInvalidationEnable);
    LOG_CMD_SUB_VAL(pCmd_->DW1.ConstantCacheInvalidationEnable);
    LOG_CMD_SUB_VAL(pCmd_->DW1.VfCacheInvalidationEnable);
    LOG_CMD_SUB_VAL(pCmd_->DW1.DcFlushEnable);
    //LOG_CMD_SUB_VAL(pCmd_->DW1.ProtectedMemoryApplicationId);
    LOG_CMD_SUB_VAL(pCmd_->DW1.PipeControlFlushEnable);
    LOG_CMD_SUB_VAL(pCmd_->DW1.NotifyEnable);
    LOG_CMD_SUB_VAL(pCmd_->DW1.IndirectStatePointersDisable);
    LOG_CMD_SUB_VAL(pCmd_->DW1.TextureCacheInvalidationEnable);
    LOG_CMD_SUB_VAL(pCmd_->DW1.InstructionCacheInvalidateEnable);
    LOG_CMD_SUB_VAL(pCmd_->DW1.RenderTargetCacheFlushEnable);
    LOG_CMD_SUB_VAL(pCmd_->DW1.DepthStallEnable);
    LOG_CMD_SUB_VAL(pCmd_->DW1.PostSyncOperation);
    LOG_CMD_SUB_VAL(pCmd_->DW1.GenericMediaStateClear);
    LOG_CMD_SUB_VAL(pCmd_->DW1.Reserved49);
    LOG_CMD_SUB_VAL(pCmd_->DW1.TlbInvalidate);
    LOG_CMD_SUB_VAL(pCmd_->DW1.GlobalSnapshotCountReset);
    LOG_CMD_SUB_VAL(pCmd_->DW1.CommandStreamerStallEnable);
    LOG_CMD_SUB_VAL(pCmd_->DW1.StoreDataIndex);
    LOG_CMD_SUB_VAL(pCmd_->DW1.Reserved54);
    LOG_CMD_SUB_VAL(pCmd_->DW1.LriPostSyncOperation);
    LOG_CMD_SUB_VAL(pCmd_->DW1.DestinationAddressType);
    LOG_CMD_SUB_VAL(pCmd_->DW1.Reserved57);
    LOG_CMD_SUB_VAL(pCmd_->DW1.FlushLlc);
    //LOG_CMD_SUB_VAL(pCmd_->DW1.ProtectedMemoryDisable);
    //LOG_CMD_SUB_VAL(pCmd_->DW1.Reserved60);

    LOG_CMD_HEX_DW(pCmd_->DW2.Value);
    LOG_CMD_SUB_VAL(pCmd_->DW2.Address);
    LOG_CMD_SUB_VAL(pCmd_->DW2.Reserved64);

    LOG_CMD_HEX_DW(pCmd_->DW3.Value);
    LOG_CMD_SUB_VAL(pCmd_->DW3.AddressHigh);

    LOG_CMD_HEX_DW(pCmd_->DW4_5.Value[0]);
    LOG_CMD_HEX_DW(pCmd_->DW4_5.Value[1]);
    LOG_CMD_SUB_VAL(pCmd_->DW4_5.ImmediateData);

    logCmdFlush();
}

CmdMediaStateFlush::CmdMediaStateFlush(bool bFlushToGo, uint8_t interfaceDescriptorOffset)
    : GpuCmdMI(),
    bFlushToGo_(bFlushToGo),
    ui8InterfaceDescriptorOffset_(interfaceDescriptorOffset)
{
    pCmd_->DW1.FlushToGo = bFlushToGo;
    pCmd_->DW1.InterfaceDescriptorOffset = interfaceDescriptorOffset;
}

bool CmdMediaStateFlush::validate()
{
    return true;
}

void CmdMediaStateFlush::parse()
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
    LOG_CMD_SUB_VAL(pCmd_->DW1.WatermarkRequired);
    LOG_CMD_SUB_VAL(pCmd_->DW1.FlushToGo);
    LOG_CMD_SUB_VAL(pCmd_->DW1.Reserved40);

    logCmdFlush();
}

bool CmdMiStoreDataImm::validate()
{
    return true;
}

void CmdMiStoreDataImm::parse()
{
    LOG_CMD_NAME(typeid(pCmd_).name());

    dumpCmdBinary((uint32_t*)pCmd_, cmdSize_ / 4);

    LOG_CMD_HEX_DW(pCmd_->DW0.Value);
    LOG_CMD_SUB_VAL(pCmd_->DW0.DwordLength);
    LOG_CMD_SUB_VAL(pCmd_->DW0.Reserved10);
    LOG_CMD_SUB_VAL(pCmd_->DW0.StoreQword);
    LOG_CMD_SUB_VAL(pCmd_->DW0.UseGlobalGtt);
    LOG_CMD_SUB_VAL(pCmd_->DW0.MiCommandOpcode);
    LOG_CMD_SUB_VAL(pCmd_->DW0.CommandType);

    LOG_CMD_HEX_DW(pCmd_->DW1_2.Value[0]);
    LOG_CMD_HEX_DW(pCmd_->DW1_2.Value[1]);
    LOG_CMD_SUB_VAL(pCmd_->DW1_2.CoreModeEnable);
    LOG_CMD_SUB_VAL(pCmd_->DW1_2.Reserved33);
    LOG_CMD_SUB_VAL(pCmd_->DW1_2.Address);

    LOG_CMD_HEX_DW(pCmd_->DW3.Value);
    LOG_CMD_SUB_VAL(pCmd_->DW3.DataDword0);

    LOG_CMD_HEX_DW(pCmd_->DW4.Value);
    LOG_CMD_SUB_VAL(pCmd_->DW4.DataDword1);

    logCmdFlush();
}

CmdMiLoadRegisterImm::CmdMiLoadRegisterImm(uint32_t offset, uint32_t data)
    : GpuCmdMI(),
    offset_(offset),
    data_(data)
{
    pCmd_->DW1.RegisterOffset = offset >> 2;
    pCmd_->DW2.DataDword = data;
}

bool CmdMiLoadRegisterImm::validate()
{
    return true;
}

void CmdMiLoadRegisterImm::parse()
{
    LOG_CMD_NAME(typeid(pCmd_).name());

    dumpCmdBinary((uint32_t*)pCmd_, cmdSize_ / 4);

    LOG_CMD_HEX_DW(pCmd_->DW0.Value);
    LOG_CMD_SUB_VAL(pCmd_->DW0.DwordLength);
    LOG_CMD_SUB_VAL(pCmd_->DW0.ByteWriteDisables);
    LOG_CMD_SUB_VAL(pCmd_->DW0.Reserved12);
    LOG_CMD_SUB_VAL(pCmd_->DW0.MiCommandOpcode);
    LOG_CMD_SUB_VAL(pCmd_->DW0.CommandType);

    LOG_CMD_HEX_DW(pCmd_->DW1.Value);
    LOG_CMD_SUB_VAL(pCmd_->DW1.Reserved32);
    LOG_CMD_SUB_VAL(pCmd_->DW1.RegisterOffset);
    LOG_CMD_SUB_VAL(pCmd_->DW1.Reserved55);

    LOG_CMD_HEX_DW(pCmd_->DW2.Value);
    LOG_CMD_SUB_VAL(pCmd_->DW2.DataDword);

    logCmdFlush();
}

bool CmdMiBatchBufferEnd::validate()
{
    return true;
}

void CmdMiBatchBufferEnd::parse()
{
    LOG_CMD_NAME(typeid(pCmd_).name());

    dumpCmdBinary((uint32_t*)pCmd_, cmdSize_ / 4);

    LOG_CMD_HEX_DW(pCmd_->DW0.Value);
    LOG_CMD_SUB_VAL(pCmd_->DW0.Reserved0);
    LOG_CMD_SUB_VAL(pCmd_->DW0.MiCommandOpcode);
    LOG_CMD_SUB_VAL(pCmd_->DW0.CommandType);

    logCmdFlush();
}

CmdMiFlushDW::CmdMiFlushDW(bool bInvalidCache)
{
    pCmd_->DW0.VideoPipelineCacheInvalidate = bInvalidCache;
}

bool CmdMiFlushDW::validate()
{
    return true;
}

void CmdMiFlushDW::parse()
{
    LOG_CMD_NAME(typeid(pCmd_).name());

    dumpCmdBinary((uint32_t*)pCmd_, cmdSize_ / 4);

    logCmdFlush();
}

CmdMiStoreRegisterMem::CmdMiStoreRegisterMem(MediaResource* res, uint32_t resOffset, uint32_t registerAddr)
{
    pCmd_->DW0.UseGlobalGtt = 0;
    pCmd_->DW1.RegisterAddress = registerAddr >> 2;
    if (res)
    {
        uint64_t address = res->getCannoizedGpuAddress();
        setGpuAddress(&pCmd_->DW2_3.Value[0], &pCmd_->DW2_3.Value[1], address, resOffset, 4);
    }
}

bool CmdMiStoreRegisterMem::validate()
{
    return true;
}

void CmdMiStoreRegisterMem::parse()
{
}

}