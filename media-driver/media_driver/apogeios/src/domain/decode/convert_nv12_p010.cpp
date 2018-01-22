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
//! \file     convert_nv12_p010.cpp
//! \brief    nv12 to p010 conversion kernel packet class implementation
//! \details
//!

#include "common.h"
#include "convert_nv12_p010.h"
#include "gpu_cmd_mi.h"
#include "gpu_cmd_render.h"

namespace Apogeios
{

CovertNV12Packet::CovertNV12Packet(MediaResource* pSrc, MediaResource* pDst)
{
    srcNV12Bind_.res = pSrc;
    dstP010Bind_.res = pDst;
}

CovertNV12Packet::~CovertNV12Packet()
{
}

int32_t CovertNV12Packet::createPacket()
{
    if (statusReport_ == nullptr)
    {
        statusReport_ = new MediaResource(RES_TYPE_BUFFER, RES_FORMAT_Buffer, 4096, false, "StatusReport");
        if (statusReport_->create() != 0)
            return -1;
    }

    if (timeStamp_ == nullptr)
    {
        timeStamp_ = new MediaResource(RES_TYPE_BUFFER, RES_FORMAT_Buffer, 4096, false, "TimeStamp");
        if (timeStamp_->create() != 0)
            return -1;
    }

    if (cmdBufIdGlobal_ == nullptr)
    {
        cmdBufIdGlobal_ = new MediaResource(RES_TYPE_BUFFER, RES_FORMAT_Buffer, 4096, false, "CommandBufferId");
        if (cmdBufIdGlobal_->create() != 0)
            return -1;
    }

    // Put external resources into bind list, which will be used to setup state heap later
    sshBindResList_.push_back(srcNV12Bind_);
    sshBindResList_.push_back(dstP010Bind_);

    //  Create GPU context (Render Ring) for this command packet
    if (createGpuContext(GPUNODE_3D) != 0)
    {
        return -1;
    }

    return 0;
}

int32_t CovertNV12Packet::preparePacket()
{
    GpuCmdBase* pCmd = nullptr;
    uint32_t threadWidth = UTIL_ALIGN(srcNV12Bind_.res->getWidth(), 8) / 8;
    uint32_t threadHeight = UTIL_ALIGN(srcNV12Bind_.res->getHeight(), 16) / 16;

    // Initialize SSH, DSH (ID & Curbe) and ISH
    if (initStateHeap() != 0)
        return -1;

    // Construct a GPU command sequence for running a kernel
    if (constructCmdSequence(threadWidth, threadHeight) != 0)
        return -1;

    // Validate each GPU commands programming correctness
    if (validateCmdSequence() != 0)
        return -1;

    // Dump parsed command buffer into file
    if (dumpCmdSequence() != 0)
        return -1;

    // Collect all resources that are being referred by command packet
    // We need this resource list to residency management and synchronization
    updateResArray();

    // add external resources
    addResource(srcNV12Bind_.res);
    addResource(dstP010Bind_.res);

    return 0;
}

int32_t CovertNV12Packet::submitPacket()
{
    if (buildCmdBuffer() != 0)
    {
        return -1;
    }

    if (submitCmdBuffer() != 0)
    {
        return -1;
    }

    return 0;
}

int32_t CovertNV12Packet::destroyPacket()
{
    deleteGpuCmd();

    if (statusReport_)
    {
        statusReport_->destroy();
        delete statusReport_;
        statusReport_ = nullptr;
    }

    if (timeStamp_)
    {
        timeStamp_->destroy();
        delete timeStamp_;
        timeStamp_ = nullptr;
    }

    if (cmdBufIdGlobal_)
    {
        cmdBufIdGlobal_->destroy();
        delete cmdBufIdGlobal_;
        cmdBufIdGlobal_ = nullptr;
    }

    if (destroyStateHeap() != 0)
        return -1;

    //  Destroy GPU context
    if (destroyGpuContext() != 0)
        return -1;

    return 0;
}

int32_t CovertNV12Packet::constructCmdSequence(uint32_t threadWidth, uint32_t threadHeight)
{
    // Construct GPU command sequence
    addGpuCmd(new CmdPipeControl(FLUSH_WRITE_CACHE));
    //addGpuCmd(new CmdPipeControl(FLUSH_READ_CACHE, PSO_WRITE_IMMEDIATE_DATA, statusReport_, 0, 0));
    //addGpuCmd(new CmdPipeControl(FLUSH_WRITE_CACHE, PSO_WRITE_TIMESTAMP_REG, timeStamp_, 0, 0));
    //addGpuCmd(new CmdPipeControl(FLUSH_WRITE_CACHE, PSO_WRITE_IMMEDIATE_DATA, timeStamp_, 16, 1));

    // Enable L3 cache
    // APGTODO: encapsulate specific functions with operation code
    uint32_t offsetL3CacheConfigReg = 0x7034;
    uint32_t cacheConfigSetting = 0x60000121;
    addGpuCmd(new CmdMiLoadRegisterImm(offsetL3CacheConfigReg, cacheConfigSetting));

    // Adds preemption granularity control for Gen9.
    // Support preemption granularity control for 3D/GPGPU with non-privileged access
    // APGTODO: encapsulate specific functions with operation code
    uint32_t offsetPreemptionCtrlReg = 0x2580;
    uint32_t preemptionCtrlData = 0x00060004;
    addGpuCmd(new CmdMiLoadRegisterImm(offsetPreemptionCtrlReg, preemptionCtrlData));

#if 1
    addGpuCmd(new CmdPipelineSelect(PIPELINE_SELECTION_MEDIA));

    CmdStateBaseAddress* pCmdBaseAddr = new CmdStateBaseAddress;
    pCmdBaseAddr->addSSH(getCmdBuffer()->getCmdResource(), getCmdBuffer()->getSshOffset());
    pCmdBaseAddr->addDSH(getDSH()->getResource());
    pCmdBaseAddr->addISH(getISH()->getResource());
    addGpuCmd(pCmdBaseAddr);

    addGpuCmd(new CmdMediaVfeState(getDSH()->getCurbeSize()));
    addGpuCmd(new CmdMediaCurbeLoad(getDSH()->getCurbeSize(), getDSH()->getCurbeOffset()));
    addGpuCmd(new CmdMediaInterfaceDescriptorLoad(getDSH()->getIdSize(), getDSH()->getIdOffset()));

    // APGTODO: only set some key parameters, other parameters use default value
    addGpuCmd(new CmdMediaObjectWalker(threadWidth, threadHeight));

    //addGpuCmd(new CmdPipeControl(FLUSH_WRITE_CACHE, PSO_NOWRITE, timeStamp_, 0, 0));
    //addGpuCmd(new CmdPipeControl(FLUSH_READ_CACHE, PSO_WRITE_TIMESTAMP_REG, timeStamp_, 0, 0));
    //addGpuCmd(new CmdPipeControl(FLUSH_WRITE_CACHE, PSO_NOWRITE, cmdBufIdGlobal_, 0, 0));
    //addGpuCmd(new CmdPipeControl(FLUSH_READ_CACHE, PSO_WRITE_IMMEDIATE_DATA, cmdBufIdGlobal_, 0, 1));
    addGpuCmd(new CmdPipeControl(FLUSH_WRITE_CACHE));

    // Send Dummy VFE after PipelineSelect
    addGpuCmd(new CmdMediaVfeState(0));
    addGpuCmd(new CmdMediaStateFlush(true, 0));
#endif
    //addGpuCmd(new CmdPipeControl(FLUSH_WRITE_CACHE));
    addGpuCmd(new CmdMiBatchBufferEnd);

    return 0;
}

int32_t CovertNV12Packet::initStateHeap()
{
    // Initialize Surface State heap
    if (initSSH(sshBindResList_) != 0)
        return -1;

    // Initialize Instruction State heap
    // Note: currently we pass the gen specific ISA binary instead of CISA binary
    if (initISH(kernelNV12P010Gen9ISA_) != 0)
        return -1;

    // Set Curbe data of NV12 conversion kernel
    curbe_.srcSurfIndex = srcNV12Bind_.surfIndex;
    curbe_.dstSurfIndex = dstP010Bind_.surfIndex;

    // Initialize Dynamical State heap
    if (initDSH((uint8_t*)&curbe_, sizeof(curbe_)) != 0)
        return -1;

    return 0;
}

std::vector<uint8_t> CovertNV12Packet::kernelNV12P010Gen9ISA_ = {
    0x05, 0x00, 0x00, 0x00, 0x4C, 0x12, 0x28, 0x20, 0x04, 0x00, 0x00, 0x16, 0xFF, 0x07, 0xFF, 0x07,
    0x01, 0x4D, 0x00, 0x20, 0x07, 0x14, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x4C, 0x12, 0x2A, 0x20,
    0x06, 0x00, 0x00, 0x16, 0xFF, 0x07, 0xFF, 0x07, 0x40, 0x00, 0x00, 0x00, 0x04, 0x02, 0x00, 0x22,
    0x20, 0x00, 0x00, 0x06, 0x00, 0x00, 0x49, 0x02, 0x09, 0x00, 0x00, 0x00, 0x28, 0x12, 0x2C, 0x20,
    0x28, 0x00, 0x00, 0x1E, 0x03, 0x00, 0x03, 0x00, 0x01, 0x00, 0x00, 0x00, 0x0C, 0x06, 0x88, 0x22,
    0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x0F, 0x00, 0x09, 0x00, 0x00, 0x00, 0x28, 0x12, 0x84, 0x22,
    0x2A, 0x00, 0x00, 0x1E, 0x04, 0x00, 0x04, 0x00, 0x09, 0x00, 0x00, 0x00, 0x28, 0x12, 0x30, 0x20,
    0x28, 0x00, 0x00, 0x1E, 0x04, 0x00, 0x04, 0x00, 0x01, 0x40, 0x1C, 0x20, 0x00, 0x14, 0x01, 0x00,
    0x01, 0x00, 0x80, 0x00, 0x28, 0x1E, 0xC0, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x31, 0x00, 0x60, 0x0C, 0x0C, 0x02, 0x40, 0x20, 0x80, 0x02, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00,
    0x01, 0x00, 0x80, 0x00, 0x28, 0x1E, 0x00, 0x21, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x80, 0x00, 0x28, 0x1E, 0x40, 0x21, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x80, 0x00, 0x28, 0x1E, 0x80, 0x21, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x0C, 0x06, 0x88, 0x22, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x0F, 0x00,
    0x40, 0x00, 0x00, 0x00, 0x04, 0x02, 0x00, 0x22, 0x24, 0x00, 0x00, 0x06, 0x00, 0x80, 0x0A, 0x02,
    0x01, 0x40, 0x20, 0x20, 0x00, 0x14, 0x01, 0x00, 0x01, 0x00, 0x80, 0x00, 0x28, 0x1E, 0x00, 0x22,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x80, 0x00, 0x28, 0x1E, 0x40, 0x22,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x4D, 0x00, 0x20, 0x07, 0x7F, 0x00, 0x00,
    0x01, 0x00, 0xA0, 0x00, 0x88, 0x22, 0xC1, 0x40, 0x40, 0x00, 0x8D, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0xA0, 0x00, 0x88, 0x22, 0x01, 0x41, 0x60, 0x00, 0x8D, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0xA0, 0x00, 0x88, 0x22, 0x41, 0x41, 0x80, 0x00, 0x8D, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0xA0, 0x00, 0x88, 0x22, 0x81, 0x41, 0xA0, 0x00, 0x8D, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x33, 0x00, 0x60, 0x0C, 0x14, 0x60, 0x00, 0x00, 0x88, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x09, 0x00, 0x00, 0x00, 0x28, 0x12, 0x84, 0x22, 0x2A, 0x00, 0x00, 0x1E, 0x03, 0x00, 0x03, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x0C, 0x06, 0x88, 0x22, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x07, 0x00,
    0x01, 0x40, 0x1C, 0x20, 0x00, 0x14, 0x01, 0x00, 0x40, 0x00, 0x00, 0x00, 0x04, 0x02, 0x00, 0x22,
    0x20, 0x00, 0x00, 0x06, 0x01, 0x00, 0x29, 0x02, 0x31, 0x00, 0x60, 0x0C, 0x0C, 0x02, 0xC0, 0x21,
    0x80, 0x02, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x0C, 0x06, 0x88, 0x22,
    0x00, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x07, 0x00, 0x01, 0x40, 0x20, 0x20, 0x00, 0x14, 0x01, 0x00,
    0x40, 0x00, 0x00, 0x00, 0x04, 0x02, 0x00, 0x22, 0x24, 0x00, 0x00, 0x06, 0x01, 0x80, 0x0A, 0x02,
    0x01, 0x00, 0xA0, 0x00, 0x88, 0x22, 0x01, 0x42, 0xC0, 0x01, 0x8D, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0xA0, 0x00, 0x88, 0x22, 0x41, 0x42, 0xE0, 0x01, 0x8D, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x33, 0x00, 0x60, 0x0C, 0x14, 0x00, 0x01, 0x00, 0x84, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x31, 0x00, 0x00, 0x07, 0x00, 0x02, 0x00, 0x20, 0xE0, 0x0F, 0x00, 0x06, 0x10, 0x00, 0x00, 0x82
};

}