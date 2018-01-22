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
//! \file     packet_cm.h
//! \brief    Generic kernel packet class definition for CMRT submission
//! \details
//!

#pragma once

#include "packet_kernel.h"

namespace Apogeios
{

class GenericCmPacket : public KernelPacket
{
#if 0
public:
    struct CurbeData
    {
        uint32_t srcSurfIndex;
        uint32_t dstSurfIndex;
    };
#endif

public:
    GenericCmPacket();
    virtual ~GenericCmPacket();

    int32_t createPacket();
    int32_t preparePacket(std::vector<uint8_t> &kernelBinary, std::vector<MediaResource*> &extResList, std::vector<uint32_t> &extResOffset, uint32_t threadWidth, uint32_t threadHeight);
    int32_t submitPacket();
    int32_t destroyPacket();

private:
    int32_t constructCmdSequence(uint32_t threadWidth, uint32_t threadHeight);
    int32_t initStateHeap();

private:
    std::vector<uint8_t> kernelBinCISA_;
    //uint8_t* kernelBinary_ = nullptr;
    //uint32_t kernelBinarySizeInBytes_ = 0;

    MediaResource* statusReport_ = nullptr;
    MediaResource* timeStamp_ = nullptr;
    MediaResource* cmdBufIdGlobal_ = nullptr;
    // external resources
    //MediaResourceBind srcNV12Bind_ = { nullptr, 0, 2 };
    //MediaResourceBind dstP010Bind_ = { nullptr, 2, 2 };
    std::vector<MediaResourceBind> sshBindResList_;
    std::vector<uint32_t> curbe_;
    //uint32_t curbeSizeInBytes_ = 0;
};

}