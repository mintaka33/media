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
//! \file     cmd_buffer.cpp
//! \brief    cmd buffer class implementation
//! \details
//!

#include <assert.h>
#include "cmd_buffer.h"

CommandBuffer::CommandBuffer()
{
    // Create a big enough command buffer resource in constructor. The reason is, SSH resides in command buffer. When set
    // StateBaseAddress GPU command, it needs SSH (Cmd buffer) address, we cannot provide such address if defer command
    // buffer resource creation in buildCmdBuffer.
    cmdBufRes_ = new MediaResource(RES_TYPE_BUFFER, RES_FORMAT_Buffer, totalCmdBufSize_, true, "CmdBuffer");
    if (cmdBufRes_->create() != 0)
    {
        delete cmdBufRes_;
        cmdBufRes_ = nullptr;
    }
}

CommandBuffer::~CommandBuffer()
{
    if (cmdBufRes_)
    {
        cmdBufRes_->destroy();
        delete cmdBufRes_;
    }
}

int32_t CommandBuffer::copySSH(std::vector<uint8_t>& ssh)
{
    if (ssh.size() <= 0)
        return -1;

    sshData_.assign(ssh.begin(), ssh.end());

    return 0;
}

int32_t CommandBuffer::buildCmdBuffer(std::vector<GpuCmdBase*>& cmdArray)
{
    // cmd buffer data = GPU commands + SSH (if any)
    combineBuffer(cmdArray);

    assert(totalCmdBufSize_ >= cmdBufData_.size());

    if (cmdBufRes_->writeData(&cmdBufData_[0], cmdBufData_.size()) != 0)
    {
        return -1;
    }

    return 0;
}

void CommandBuffer::combineBuffer(std::vector<GpuCmdBase*>& cmdArray)
{
    int8_t *pData = nullptr;

    // GPU commands sequence
    for (auto cmd : cmdArray)
    {
        pData = (int8_t*)cmd->getCmdData();
        for (uint32_t i = 0; i < cmd->getCmdSize(); i++)
        {
            // copy GPU command data into an intermediate array first
            gpuCmdData_.push_back(*(pData + i));
        }
    }
    cmdBufData_.insert(cmdBufData_.end(), gpuCmdData_.begin(), gpuCmdData_.end());

    // padding cmd buffer
    uint32_t remainingSize = gpuCmdRegionSize_ - cmdBufData_.size();
    for (uint32_t i = 0; i < remainingSize; i++)
    {
        cmdBufData_.push_back(0);
    }

    // Surface state heap
    if (sshData_.size() > 0)
    {
        cmdBufData_.insert(cmdBufData_.end(), sshData_.begin(), sshData_.end());
    }
}