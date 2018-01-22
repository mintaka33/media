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
//! \file     gpu_context.h
//! \brief    gpu context class definition
//! \details
//!

#pragma once

#include "gpu_context_cb.h"
#include "cmd_buffer.h"

namespace Apogeios 
{

class GpuContext
{
public:
    GpuContext();
    ~GpuContext();

    int32_t createContext(uint32_t gpuNode);
    int32_t destroyContext();
    int32_t submitCommand(CommandBuffer* pCmdBuf);

    static void setCallbackInstance(IContextCb* pCallback) { pCallback_ = pCallback; };

private:
    uint32_t hRTContext_ = 0;
    uint32_t gpuNodeOrdinal_ = 0;
    uint64_t hwContextId_ = 0;
    static IContextCb* pCallback_;
};
    
}