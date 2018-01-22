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
//! \file     convert_nv12_p010.h
//! \brief    nv12 to p010 conversion kernel packet class definition
//! \details
//!

#pragma once

#include "packet_kernel.h"

namespace Apogeios
{

class CovertNV12Packet : public KernelPacket
{
public:
    struct CurbeData
    {
        uint32_t srcSurfIndex;
        uint32_t dstSurfIndex;
    };

public:
    CovertNV12Packet(MediaResource* pSrc, MediaResource* pDst);
    virtual ~CovertNV12Packet();

    int32_t createPacket();
    int32_t preparePacket();
    int32_t submitPacket();
    int32_t destroyPacket();

private:
    int32_t constructCmdSequence(uint32_t threadWidth, uint32_t threadHeight);
    int32_t initStateHeap();

private:
    static std::vector<uint8_t> kernelNV12P010Gen9ISA_;
    MediaResource* statusReport_ = nullptr;
    MediaResource* timeStamp_ = nullptr;
    MediaResource* cmdBufIdGlobal_ = nullptr;
    // external resources
    MediaResourceBind srcNV12Bind_ = { nullptr, 0, 2 };
    MediaResourceBind dstP010Bind_ = { nullptr, 2, 2 };
    std::vector<MediaResourceBind> sshBindResList_;
    CurbeData curbe_ = {};
};

}