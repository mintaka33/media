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
//! \file     cmd_buffer.h
//! \brief    cmd buffer class definition
//! \details
//!

#pragma once

#include <vector>
#include <stdint.h>
#include "sharedata.h"

#include "gpu_cmd_base.h"
#include "media_resource.h"

namespace Apogeios
{

class CommandBuffer
{
public:
    CommandBuffer();
    ~CommandBuffer();

    uint32_t getGpuCmdSize() { return (uint32_t)gpuCmdData_.size(); }
    uint32_t getCmdDataSize() { return (uint32_t)cmdBufData_.size(); }
    uint32_t getSshOffset() { return gpuCmdRegionSize_; }
    uint32_t getCmdBufferSize() { return totalCmdBufSize_; }
    MediaResource * getCmdResource() { return cmdBufRes_; }

    int32_t copySSH(std::vector<uint8_t>& ssh);
    int32_t buildCmdBuffer(std::vector<GpuCmdBase*>& cmdArray);

private:
    void combineBuffer(std::vector<GpuCmdBase*>& cmdArray);

private:
    const uint32_t gpuCmdRegionSize_ = 32 * 1024; // gpu command data
    const uint32_t sshRegionSize_ = 32 * 1024; // surface state heap (BT + SS)
    const uint32_t totalCmdBufSize_ = gpuCmdRegionSize_ + sshRegionSize_;

    std::vector<uint8_t> gpuCmdData_;
    std::vector<uint8_t> sshData_;
    std::vector<uint8_t> cmdBufData_;
    MediaResource *cmdBufRes_ = nullptr;
};
    
}
