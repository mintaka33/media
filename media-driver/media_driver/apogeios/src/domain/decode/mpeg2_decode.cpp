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
//! \file     mpeg2_decode.cpp
//! \brief    mpeg2 decode packet class implementation
//! \details
//!

#include "mpeg2_decode.h"
#include "gpu_cmd_vdbox.h"
#include "gpu_cmd_mi.h"

namespace Apogeios
{

Mpeg2DecodePkt::Mpeg2DecodePkt()
{
}

Mpeg2DecodePkt::~Mpeg2DecodePkt()
{
}

int32_t Mpeg2DecodePkt::createPacket()
{
    if (statusReport_ == nullptr)
    {
        statusReport_ = new MediaResource(RES_TYPE_BUFFER, RES_FORMAT_Buffer, 4096, false, "StatusReport");
        if (statusReport_->create() != 0)
            return -1;
    }

    if (allocateResource() != 0)
        return -1;

    //  Create GPU context (VDBox Ring) for fix function packet
    if (createGpuContext(GPUNODE_VIDEO) != 0)
    {
        return -1;
    }

    return 0;
}

int32_t Mpeg2DecodePkt::preparePacket()
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

int32_t Mpeg2DecodePkt::submitPacket()
{
    if (buildCmdBuffer() != 0)
        return -1;

    if (submitCmdBuffer() != 0)
        return -1;

    return 0;
}

int32_t Mpeg2DecodePkt::destroyPacket()
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

int32_t Mpeg2DecodePkt::constructCmdSequence()
{
    addGpuCmd(new CmdMiFlushDW(true));

    addGpuCmd(new CmdPipeControl(FLUSH_READ_CACHE, PSO_WRITE_IMMEDIATE_DATA, statusReport_, 0, 0));

    addGpuCmd(new CmdMfxPipeModeSelect(MFX_MPEG2, 1, 0, 1, 0, 1, 0));

    addGpuCmd(new CmdMfxSurfaceState(decOut_, MFX_REF));

    CmdMfxPipeBufAddrState::PipeBufResources pipeBufRes = {};
    addGpuCmd(new CmdMfxPipeBufAddrState(&pipeBufRes));

    CmdMfxIndObjBaseAddrState::IndObjResources indObjRes = {};
    addGpuCmd(new CmdMfxIndObjBaseAddrState(&indObjRes));

    CmdMfxBspBufBaseAddrState::BspBufResources bspBufRes = {};
    addGpuCmd(new CmdMfxBspBufBaseAddrState(&bspBufRes));

    CmdMfxMpeg2ImgState::MfxMpeg2ImgParams imgParam = {};
    addGpuCmd(new CmdMfxMpeg2ImgState(&imgParam));

    CmdMfxQmState::Mpeg2QmList qmList;  // Use default matrix
    addGpuCmd(new CmdMfxQmState(CmdMfxQmState::mpeg2QmIntra, qmList));

    for(int32_t slcIndex=0; slcIndex<1; slcIndex++)
    {
        CmdMfxMpeg2BsdObject::MfxMpeg2BsdObjParams bsdObjParams = {};
        addGpuCmd(new CmdMfxMpeg2BsdObject(&bsdObjParams));
    }

    addGpuCmd(new CmdPipeControl(FLUSH_READ_CACHE, PSO_WRITE_IMMEDIATE_DATA, statusReport_, 4, 1));

    addGpuCmd(new CmdMiBatchBufferEnd);

    return 0;
}

int32_t Mpeg2DecodePkt::allocateResource()
{
    return 0;
}

int32_t Mpeg2DecodePkt::destroyResource()
{
    return 0;
}

}