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
//! \file     cmd_packet.cpp
//! \brief    cmd packet base class implementation
//! \details
//!

#include "packet.h"
#include <set>

namespace Apogeios
{

CmdPacket::CmdPacket()
{
    cmdBuffer_ = new CommandBuffer;
    gpuContext_ = new GpuContext;
}

CmdPacket::~CmdPacket()
{
    if (cmdBuffer_)
    {
        delete cmdBuffer_;
        cmdBuffer_ = nullptr;
    }

    if (gpuContext_)
    {
        delete gpuContext_;
        gpuContext_ = nullptr;
    }
}

int32_t CmdPacket::validateCmdSequence()
{
    for (auto cmd : cmdSequence_)
    {
        if (cmd == nullptr)
            return -1;

        if (cmd->validate() != true)
            return 0; // Always return pass temporarily
    }

    return 0;
}

int32_t CmdPacket::dumpCmdSequence()
{
    for (auto cmd : cmdSequence_)
    {
        if (cmd == nullptr)
            return -1;

        cmd->parse();
    }

    return 0;
}

int32_t  CmdPacket::addGpuCmd(GpuCmdBase* pCmd)
{
    if (pCmd == nullptr)
    {
        return -1;
    }

    cmdSequence_.push_back(pCmd);

    return 0;
}

int32_t CmdPacket::deleteGpuCmd()
{
    for (auto cmd : cmdSequence_)
    {
        if (cmd != nullptr)
        {
            delete cmd;
        }
    }

    cmdSequence_.clear();

    return 0;
}

void CmdPacket::updateResArray()
{
    resourceArray_.clear();

    for (auto cmd : cmdSequence_)
    {
        for (auto res : cmd->getResArray())
        {
            resourceArray_.push_back(res);
        }
    }

    // remove duplicated resources
    std::set<MediaResource*> s(resourceArray_.begin(), resourceArray_.end());
    resourceArray_.assign(s.begin(), s.end());
}

int32_t CmdPacket::associateSSH(std::vector<uint8_t>& ssh)
{
    return cmdBuffer_->copySSH(ssh);
}

int32_t CmdPacket::createGpuContext(GPUNODE_ORDINAL gpuNode)
{
    return gpuContext_->createContext(gpuNode);
}

int32_t CmdPacket::destroyGpuContext()
{
    return gpuContext_->destroyContext();
}

int32_t CmdPacket::buildCmdBuffer()
{
    if (cmdBuffer_->buildCmdBuffer(cmdSequence_) != 0)
    {
        return -1;
    }

    return 0;
}

int32_t CmdPacket::submitCmdBuffer()
{
    if (gpuContext_->submitCommand(cmdBuffer_) != 0)
    {
        return -1;
    }

    return 0;
}

int32_t CmdPacket::waitForPaging()
{
    return 0;
}

int32_t CmdPacket::syncGpuContext()
{
    for (auto res : resourceArray_)
    {
    }

    return 0;
}

}