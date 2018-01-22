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
//! \file     down_scaling_cm.h
//! \brief    down scaling cm packet class definition
//! \details
//!

#pragma once

#include "packet_mdf.h"

namespace Apogeios
{

class MdfPacketScaling : public MdfPacket
{
public:
    MdfPacketScaling(CmDevice *pDevice, CmSurface2D* pSrc, CmSurface2D* pDst, uint32_t width, uint32_t height);
    ~MdfPacketScaling();

    int32_t preparePacket();
    //int32_t submitPacket();

private:
    static std::vector<uint32_t> scalingKernel_;
    uint32_t surfWidth_ = 0;
    uint32_t surfHeight_ = 0;
    CmSurface2D* pSrcNV12_ = nullptr;
    CmSurface2D* pDstP010_ = nullptr;
    std::vector<KernelArg*> kernelArgArray_;
};

}