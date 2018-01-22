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
//! \file     avc_pak.cpp
//! \brief    avc PAK packet class implementation
//! \details
//!

#include "avc_pak.h"
#include "gpu_cmd_mi.h"
#include "gpu_cmd_vdbox.h"

namespace Apogeios
{

AvcPakPacket::AvcPakPacket(MediaResource* inputYuv, MediaResource* reconYuv, MediaResource* bitstreamBuf) :
    inputYuv_(inputYuv),
    reconYuv_(reconYuv),
    bitstreamBuf_(bitstreamBuf)
{
}

AvcPakPacket::~AvcPakPacket()
{
}

int32_t AvcPakPacket::createPacket()
{
    if (statusReport_ == nullptr)
    {
        statusReport_ = new MediaResource(RES_TYPE_BUFFER, RES_FORMAT_Buffer, 4096, false, "StatusReport");
        if (statusReport_->create() != 0)
            return -1;
    }

    //  Create GPU context (VDBox Ring) for AVC PAK packet
    if (createGpuContext(GPUNODE_VIDEO) != 0)
    {
        return -1;
    }

    return 0;
}

int32_t AvcPakPacket::preparePacket()
{
    // Construct a GPU command sequence for running a kernel
    if (constructCmdSequence() != 0)
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

    return 0;
}

int32_t AvcPakPacket::submitPacket()
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

int32_t AvcPakPacket::destroyPacket()
{
    bool bFail = false;

    if (statusReport_)
    {
        statusReport_->destroy();
        delete statusReport_;
        statusReport_ = nullptr;
    }

    deleteGpuCmd();

    if (destroyResource() != 0)
        bFail = true;

    //  Destroy GPU context
    if (destroyGpuContext() != 0)
        bFail = true;

    return (bFail) ? -1 : 0;
}

int32_t AvcPakPacket::constructCmdSequence()
{
    addGpuCmd(new CmdMiFlushDW(true));

    addGpuCmd(new CmdPipeControl(FLUSH_READ_CACHE, PSO_WRITE_IMMEDIATE_DATA, statusReport_, 0, 0));

    addGpuCmd(new CmdMfxPipeModeSelect(MFX_AVC, 1, 0, 1, 0, 1, 0));

    addGpuCmd(new CmdMfxSurfaceState(reconYuv_, MFX_REF));
    addGpuCmd(new CmdMfxSurfaceState(inputYuv_, MFX_SRC));

    CmdMfxPipeBufAddrState::PipeBufResources pipeBufRes = {};
    addGpuCmd(new CmdMfxPipeBufAddrState(&pipeBufRes));

    CmdMfxIndObjBaseAddrState::IndObjResources indObjRes = {};
    addGpuCmd(new CmdMfxIndObjBaseAddrState(&indObjRes));

    CmdMfxBspBufBaseAddrState::BspBufResources bspBufRes = {};
    addGpuCmd(new CmdMfxBspBufBaseAddrState(&bspBufRes));

    CmdMfxAvcImgState::MfxAvcImgParams avcImgParams = {};
    addGpuCmd(new CmdMfxAvcImgState(&avcImgParams));

    // QM matrices (uint8_t)
    CmdMfxQmState::AvcQmList qmList;  // Use default matrix
    addGpuCmd(new CmdMfxQmState(CmdMfxQmState::avcQmIntra4x4, qmList));
    addGpuCmd(new CmdMfxQmState(CmdMfxQmState::avcQmInter4x4, qmList));
    addGpuCmd(new CmdMfxQmState(CmdMfxQmState::avcQmIntra8x8, qmList));
    addGpuCmd(new CmdMfxQmState(CmdMfxQmState::avcQmInter8x8, qmList));

    // FQM matrices (uint16_t)
    CmdMfxFQmState::AvcFQmList fqmList; // Use default matrix
    addGpuCmd(new CmdMfxFQmState(CmdMfxFQmState::avcQmIntra4x4, fqmList));
    addGpuCmd(new CmdMfxFQmState(CmdMfxFQmState::avcQmInter4x4, fqmList));
    addGpuCmd(new CmdMfxFQmState(CmdMfxFQmState::avcQmIntra8x8, fqmList));
    addGpuCmd(new CmdMfxFQmState(CmdMfxFQmState::avcQmInter8x8, fqmList));

    CmdMfxAvcDirectModeState::DirectMVResources dmvRes = {};
    addGpuCmd(new CmdMfxAvcDirectModeState(&dmvRes));

    if (bDisable2ndLevelBatchBuf_)
    {
        int32_t slcNum = 1;
        CmdMfxAvcSliceState::AvcSliceState slcState = {};
        for (int32_t i = 0; i < slcNum; i++)
        {
            addGpuCmd(new CmdMfxAvcSliceState(&slcState));
        }

        CmdMfxPakInsertObject::PakInsertObj nalAUD = {};
        CmdMfxPakInsertObject::PakInsertObj nalSPS = {};
        CmdMfxPakInsertObject::PakInsertObj nalPPS = {};
        CmdMfxPakInsertObject::PakInsertObj nalSEI = {};
        CmdMfxPakInsertObject::PakInsertObj nalSlice = {};
        addGpuCmd(new CmdMfxPakInsertObject(&nalAUD));
        addGpuCmd(new CmdMfxPakInsertObject(&nalSPS));
        addGpuCmd(new CmdMfxPakInsertObject(&nalPPS));
        addGpuCmd(new CmdMfxPakInsertObject(&nalSEI));
        addGpuCmd(new CmdMfxPakInsertObject(&nalSlice));
    }
    else
    {
        // APGTODO: consider to put below commands into second level batch buffer
    }

    addGpuCmd(new CmdMiFlushDW(false));
    addGpuCmd(new CmdMiFlushDW(false));

    addGpuCmd(new CmdMiStoreRegisterMem(statusReport_, 8, 0x000128a0)); // bitstream byte count
    addGpuCmd(new CmdMiStoreRegisterMem(statusReport_, 12, 0x000128a4)); // bitstream SE bit count
    addGpuCmd(new CmdMiStoreRegisterMem(statusReport_, 16, 0x00000268)); // QP status count
    addGpuCmd(new CmdMiStoreRegisterMem(statusReport_, 20, 0x00000218)); // number of slice

    addGpuCmd(new CmdPipeControl(FLUSH_READ_CACHE, PSO_WRITE_IMMEDIATE_DATA, statusReport_, 4, 1));

    addGpuCmd(new CmdMiBatchBufferEnd);

    return 0;
}

int32_t AvcPakPacket::allocateResource()
{
    return 0;
}

int32_t AvcPakPacket::destroyResource()
{
    return 0;
}

}