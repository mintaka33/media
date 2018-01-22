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
//! \file     packet_kernel.cpp
//! \brief    kernel packet class implementation
//! \details
//!

#include "common.h"
#include "packet_kernel.h"

int32_t extractGenISA(std::vector<uint8_t>& kernelCISA, std::vector<uint8_t>& kernelGenISA)
{
    if (kernelCISA.size() == 0)
        return -1;

    uint32_t sizeAligned = UTIL_ALIGN(kernelCISA.size(), 1024 * 4);

    kernelGenISA.assign(sizeAligned, 0);

    // Currently we pass the gen specific ISA binary instead of CISA binary, so just copy it to outGenISA directly
    // TODO: parse and extract Gen specific kernel binary from CISA
    std::copy(kernelCISA.begin(), kernelCISA.end(), kernelGenISA.begin());

    return 0;
}

namespace Apogeios
{

KernelPacket::KernelPacket()
{
    pSsh_ = new SSH();
    pIsh_ = new ISH;
    pDsh_ = new DSH;
}

KernelPacket::~KernelPacket()
{
    if (pSsh_) delete pSsh_;
    if (pDsh_) delete pDsh_;
    if (pIsh_) delete pIsh_;
}

int32_t KernelPacket::createPacket()
{
    return 0;
}

int32_t KernelPacket::preparePacket()
{
    return 0;
}

int32_t KernelPacket::submitPacket()
{
    return 0;
}

int32_t KernelPacket::destroyPacket()
{
    return 0;
}

int32_t KernelPacket::initSSH(std::vector<MediaResourceBind> resBindList)
{
    if (pSsh_ == nullptr)
        return -1;

    if (pSsh_->initialize(resBindList) != 0)
        return -1;

    // for kernel packet, need to set SSH to command buffer
    if (associateSSH(pSsh_->getSshData()))
        return -1;

    return 0;
}

int32_t KernelPacket::initISH(std::vector<uint8_t>& kernelCISA)
{
    if (pIsh_ == nullptr)
        return -1;

    // Extract Gen specific kernel binary from CISA kernel binary
    std::vector<uint8_t> kernelGenISA;
    if (extractGenISA(kernelCISA, kernelGenISA) != 0)
        return -1;

    if (pIsh_->initialize(kernelGenISA) != 0)
        return -1;

    return 0;
}

int32_t KernelPacket::initDSH(uint8_t* pCurbeData, int32_t curbeSize)
{
    if (pDsh_ == nullptr)
        return -1;

    if (pDsh_->initialize(pCurbeData, curbeSize) != 0)
        return -1;

    return 0;
}

int32_t KernelPacket::destroyStateHeap()
{
    if (pSsh_)
    {
        if (pSsh_->destroy() != 0)
            return -1;
    }

    if (pIsh_)
    {
        if (pIsh_->destroy() != 0)
            return -1;
    }

    if (pDsh_)
    {
        if (pDsh_->destroy() != 0)
            return -1;
    }

    return 0;
}

}