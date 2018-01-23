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
    return 0;
}

int32_t Mpeg2DecodePkt::preparePacket()
{
    return 0;
}

int32_t Mpeg2DecodePkt::submitPacket()
{
    return 0;
}

int32_t Mpeg2DecodePkt::destroyPacket()
{
    return 0;
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
        // m_mfxInterface->AddMfdMpeg2BsdObject
        // mhw_vdbox_mfx_g9_skl::MFD_MPEG2_BSD_OBJECT_CMD
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