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
//! \file     packet_cm.cpp
//! \brief    Generic kernel packet (for CMRT submission) class implementation
//! \details
//!

#include "common.h"
#include "packet_cm.h"
#include "gpu_cmd_mi.h"
#include "gpu_cmd_render.h"

namespace Apogeios
{

GenericCmPacket::GenericCmPacket()
{
}

GenericCmPacket::~GenericCmPacket()
{
}

int32_t GenericCmPacket::createPacket()
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

    // Put external resources into an array, which will be used to setup state heap later
    //extResList_.push_back(srcNV12_);
    //extResList_.push_back(dstP010_);

    //  Create GPU context (Render Ring) for this command packet
    if (createGpuContext(GPUNODE_3D) != 0)
    {
        return -1;
    }

    return 0;
}

int32_t GenericCmPacket::preparePacket(std::vector<uint8_t> &kernelBinary, std::vector<MediaResource*> &extResList, std::vector<uint32_t> &extResOffset, uint32_t threadWidth, uint32_t threadHeight)
{
    kernelBinCISA_ = kernelBinary;

    uint32_t resourceNum = extResList.size();
    sshBindResList_.resize(resourceNum);
    curbe_.resize(resourceNum);     //hardcode for the case of only resource args existed.

    //for each (MediaResource* extResource in extResList)
    for (uint32_t i = 0; i < resourceNum; i++)
    {
        sshBindResList_[i].res = extResList[i];
        sshBindResList_[i].numPlane = 2;    //hardcode as 2 planes now
        sshBindResList_[i].surfIndex = i * 2;

        //Put resource BTI value to CURBE
        curbe_.at(extResOffset[i] / 4) = sshBindResList_[i].surfIndex;

        // add external resources
        addResource(extResList[i]);
    }

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

    return 0;
}

int32_t GenericCmPacket::submitPacket()
{
    if (buildCmdBuffer() != 0)
    {
        return -1;
    }

    if (submitCmdBuffer() != 0)
    {
        return -1;
    }

    // Sleep(10);
    // std::vector<uint8_t> tsData(4096, 0);
    // timeStamp_->readData(&tsData[0], tsData.size());

    return 0;
}

int32_t GenericCmPacket::destroyPacket()
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

    // Destroy state heap
    if (destroyStateHeap() != 0)
    {
        return -1;
    }

    //  Destroy GPU context
    if (destroyGpuContext() != 0)
    {
        return -1;
    }

    return 0;
}

int32_t GenericCmPacket::constructCmdSequence(uint32_t threadWidth, uint32_t threadHeight)
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

    //addGpuCmd(new CmdPipeControl(FLUSH_WRITE_CACHE));
    addGpuCmd(new CmdMiBatchBufferEnd);

    return 0;
}

int32_t GenericCmPacket::initStateHeap()
{
    // Initialize Surface State heap
    if (initSSH(sshBindResList_) != 0)
        return -1;

    // Initialize Instruction State heap
    // Note: currently we pass the gen specific ISA binary instead of CISA binary
    if (initISH(kernelBinCISA_) != 0)
        return -1;

    // Initialize Dynamical State heap
    if (initDSH((uint8_t*)&curbe_[0], curbe_.size() * sizeof(uint32_t)) != 0)
        return -1;

    return 0;
}

#if 0
std::vector<uint32_t> GenericCmPacket::kernelBinCISA_ =
{
    0x41534943, 0x00010203, 0x31564e0a, 0x506f5432, 0x31303130, 0x88000000, 0x0b000005, 0x00000003,
    0x01000000, 0x0005b905, 0x00027000, 0x00000000, 0x00002d00, 0x3231564e, 0x30506f54, 0x41003031,
    0x614e6d73, 0x5300656d, 0x69534d4c, 0x4e00657a, 0x7261426f, 0x72656972, 0x72755300, 0x65636166,
    0x67617355, 0x754f0065, 0x74757074, 0x5f424200, 0x42420030, 0x4300315f, 0x6e495c3a, 0x5c6c6574,
    0x4946444d, 0x7265746e, 0x366c616e, 0x455c302e, 0x706d6178, 0x5c73656c, 0x3231766e, 0x5f6f745f,
    0x30313070, 0x504d545c, 0x5249445f, 0x31766e5c, 0x6f745f32, 0x3130705f, 0x65675f30, 0x632e786e,
    0x5f007070, 0x3231564e, 0x30506f54, 0x425f3031, 0x5f315f42, 0x62690032, 0x315f6675, 0x33363333,
    0x3232565f, 0x626f0037, 0x315f6675, 0x33363333, 0x3232565f, 0x33560038, 0x33560032, 0x33560033,
    0x33560034, 0x33560035, 0x756f5f36, 0x56005974, 0x56003733, 0x56003833, 0x56003933, 0x56003034,
    0x56003134, 0x56003234, 0x56003334, 0x56003434, 0x695f3534, 0x5600596e, 0x56003634, 0x56003734,
    0x56003834, 0x56003934, 0x6f5f3035, 0x56557475, 0x31355600, 0x32355600, 0x33355600, 0x34355600,
    0x35355600, 0x36355600, 0x37355600, 0x556e695f, 0x35560056, 0x35560038, 0x36560039, 0x36560030,
    0x42420031, 0x0033325f, 0x325f4242, 0x00010034, 0x000d001e, 0x00000112, 0x00000000, 0x20000e00,
    0x00000001, 0x00000000, 0x0112000f, 0x00000000, 0x10000000, 0x00012000, 0x00000000, 0x00110000,
    0x00010064, 0x00000000, 0x00001200, 0x00240040, 0x00000000, 0x40000013, 0x00002400, 0x14000000,
    0x00400000, 0x00000024, 0x00150000, 0x24004000, 0x00000000, 0x21001600, 0x00000001, 0x00000000,
    0x01210017, 0x00000000, 0x18000000, 0x00010000, 0x00000029, 0x00190000, 0x2a000100, 0x00000000,
    0x64001a00, 0x00000080, 0x00000000, 0x0121001b, 0x00000000, 0x1c000000, 0x00012100, 0x00000000,
    0x001d0000, 0x2e000100, 0x00000000, 0x00001e00, 0x002f0001, 0x00000000, 0x8064001f, 0x00000000,
    0x20000000, 0x00200000, 0x00000032, 0x00210000, 0x32002000, 0x00000000, 0x21002200, 0x00000001,
    0x00000000, 0x01210023, 0x00000000, 0x24000000, 0x00010000, 0x00000035, 0x00250000, 0x36000100,
    0x00000000, 0x64002600, 0x00000040, 0x00000000, 0x01210027, 0x00000000, 0x28000000, 0x00012100,
    0x00000000, 0x00290000, 0x3a000100, 0x00000000, 0x00002a00, 0x003b0001, 0x00000000, 0x00000000,
    0x00070005, 0x00080000, 0x000a0000, 0x002b0001, 0x002c0000, 0x02000000, 0x0001000b, 0x01000501,
    0x01000c00, 0x00050100, 0x02000001, 0x20000602, 0x02000400, 0x00240007, 0x02740004, 0x03140000,
    0x00030000, 0x6e170002, 0x5f323176, 0x705f6f74, 0x5f303130, 0x786e6567, 0x612e305f, 0x00036d73,
    0x00040001, 0x00003100, 0x51000131, 0x15520009, 0x30000000, 0x1b520002, 0x29000000, 0x00000000,
    0x00000020, 0x01000200, 0x21000000, 0x00001001, 0x00210000, 0x02000000, 0x00002000, 0x05012100,
    0x00000802, 0x001c5200, 0x00290000, 0x22000000, 0x00000000, 0x00020002, 0x01210000, 0x00000010,
    0x00002300, 0x00020000, 0x00000022, 0x02050121, 0x00000010, 0x00001f52, 0x00042900, 0x00250000,
    0x02000000, 0x00000205, 0x04290000, 0x26000000, 0x00000200, 0x00000502, 0x29000000, 0x00000004,
    0x00040027, 0x00050200, 0x00000000, 0x00000429, 0x06002800, 0x05020000, 0x00000000, 0x00215200,
    0x00290000, 0x29000000, 0x00000000, 0x00210002, 0x01210000, 0x00000029, 0x00002a00, 0x00020000,
    0x00000023, 0x00370121, 0x10080006, 0x00002b00, 0x00012100, 0x0000002c, 0x002d0121, 0x22520000,
    0x29000000, 0x00000005, 0x01000024, 0x2d000300, 0x66000000, 0x00052902, 0x00240000, 0x03000102,
    0x01002d00, 0x29026600, 0x00000005, 0x01040024, 0x2d000300, 0x66000200, 0x00052902, 0x00240000,
    0x03000106, 0x03002d00, 0x52026600, 0x00000023, 0x00000024, 0x00002e00, 0x00020000, 0x00000021,
    0x00050121, 0x00000001, 0x00000029, 0x00002f00, 0x00020000, 0x00000023, 0x00380121, 0x10100007,
    0x00003000, 0x00012100, 0x00000031, 0x00240121, 0x26520000, 0x29000000, 0x00000004, 0x00000033,
    0x02050200, 0x00000000, 0x00000429, 0x02003400, 0x05020000, 0x00000002, 0x00285200, 0x00290000,
    0x35000000, 0x00000000, 0x00210002, 0x01210000, 0x00000026, 0x00003600, 0x00020000, 0x00000023,
    0x00050121, 0x00000001, 0x01060037, 0x37000808, 0x21000000, 0x00380001, 0x01210000, 0x00000039,
    0x00002952, 0x00052900, 0x00320000, 0x03000100, 0x00003900, 0x29026600, 0x00000005, 0x01020032,
    0x39000300, 0x66000100, 0x002a5202, 0x00290000, 0x3a000000, 0x00000000, 0x002e0002, 0x01210000,
    0x00000029, 0x00003b00, 0x00020000, 0x00000036, 0x00380121, 0x08100107, 0x00003c00, 0x00012100,
    0x0000003d, 0x00320121, 0x03310000, 0x002b5200, 0x00340000, 0x04310000, 0x00000500, 0x28124c00,
    0x00000420, 0xff07ff16, 0x00000507, 0x2a124c00, 0x00000620, 0xff07ff16, 0x004d0107, 0x00140720,
    0x00004100, 0x2c120800, 0x00002820, 0x08000816, 0x00004100, 0x30120800, 0x00002a20, 0x10001016,
    0x00004000, 0x00020400, 0x00002022, 0x49000006, 0x00000102, 0x88060c00, 0x00000022, 0x0f000700,
    0x00000100, 0x80022800, 0x00002c22, 0x00000000, 0x00000100, 0x84022800, 0x00003022, 0x00000000,
    0x60003100, 0x40020c0c, 0x00028021, 0x00020000, 0x80000100, 0x00160800, 0x00000021, 0x00000000,
    0x80000100, 0xc0160800, 0x00000020, 0x00000000, 0x80000100, 0x80160800, 0x00000020, 0x00000000,
    0x80000100, 0x40160800, 0x00000020, 0x00000000, 0x004d0100, 0x00150720, 0x00000100, 0xa8060c00,
    0x00000022, 0x0f000f00, 0x00004000, 0x00020400, 0x00002422, 0x0a800006, 0x00000902, 0xa0022800,
    0x00002c22, 0x01000116, 0x00000100, 0xa4022800, 0x00003022, 0x00000000, 0xa0000100, 0x01228800,
    0x8d01a041, 0x00000000, 0xa0000100, 0xc1228800, 0x8d018040, 0x00000000, 0xa0000100, 0x81228800,
    0x8d016040, 0x00000000, 0xa0000100, 0x41228800, 0x8d014040, 0x00000000, 0x60003300, 0x0020140c,
    0x0022a800, 0x00000000, 0x00000100, 0x80022800, 0x00002c22, 0x00000000, 0x00000c00, 0x84022800,
    0x00003022, 0x01000116, 0x00000100, 0x88060c00, 0x00000022, 0x07000700, 0x004d0100, 0x00160720,
    0x00000100, 0xc8060c00, 0x00000022, 0x07000f00, 0x00004000, 0x00020400, 0x00002022, 0x29000106,
    0x60003102, 0x40020c0c, 0x00028022, 0x00020000, 0x80000100, 0x00160800, 0x00000022, 0x00000000,
    0x80000100, 0xc0160800, 0x00000021, 0x00000000, 0x00000100, 0xc40a2800, 0x00028422, 0x00000000,
    0x00000100, 0xc00a2800, 0x0002a022, 0x00000000, 0x00004000, 0x00020400, 0x00002422, 0x0a800106,
    0xa0000102, 0x01228800, 0x8d026042, 0x00000000, 0xa0000100, 0xc1228800, 0x8d024041, 0x00000000,
    0x60003300, 0x00e0140c, 0x0022c400, 0x00000000, 0x004d0100, 0x007f0720, 0x00003100, 0x00020007,
    0x000fe020, 0x00001006, 0x00000082
};
#endif

}