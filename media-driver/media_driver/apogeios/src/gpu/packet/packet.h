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
//! \file     cmd_packet.h
//! \brief    cmd packet base class definition
//! \details
//!

#pragma once

#include <vector>
#include "gpu_cmd_base.h"
#include "cmd_buffer.h"
#include "gpu_context.h"

class MediaResource;
class MediaTaskCm;

namespace Apogeios
{

class CmdPacket
{
public:
    CmdPacket();
    virtual ~CmdPacket();
    virtual int32_t createPacket() = 0;
    virtual int32_t preparePacket() = 0;
    virtual int32_t submitPacket() = 0;
    virtual int32_t submitPacket(MediaTaskCm* pTask) = 0;
    virtual int32_t destroyPacket() = 0;

    CommandBuffer* getCmdBuffer() { return cmdBuffer_; };
    void addResource(MediaResource* res) { resourceArray_.push_back(res); };
    int32_t validateCmdSequence();
    int32_t dumpCmdSequence();

protected:
    int32_t addGpuCmd(GpuCmdBase* pCmd);
    int32_t deleteGpuCmd();
    void    updateResArray();
    int32_t associateSSH(std::vector<uint8_t>& ssh);
    int32_t createGpuContext(GPUNODE_ORDINAL gpuNode);
    int32_t destroyGpuContext();
    int32_t buildCmdBuffer();
    int32_t submitCmdBuffer();
    int32_t waitForPaging();
    int32_t syncGpuContext();

private:
    std::vector<GpuCmdBase*> cmdSequence_;
    std::vector<MediaResource*> resourceArray_;
    CommandBuffer* cmdBuffer_;
    GpuContext* gpuContext_;
    uint64_t submissionCount_ = 0;
};

}