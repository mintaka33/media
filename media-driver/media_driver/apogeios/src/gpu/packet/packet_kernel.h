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
//! \file     packet_kernel.h
//! \brief    kernel packet class definition
//! \details
//!

#pragma once

#include <vector>
#include "packet.h"
#include "state_heap.h"

int32_t extractGenISA(std::vector<uint8_t>& kernelCISA, std::vector<uint8_t>& kernelGenISA);

namespace Apogeios
{

class KernelPacket : public CmdPacket
{
public:
    KernelPacket();
    ~KernelPacket();

    virtual int32_t createPacket();
    virtual int32_t preparePacket();
    virtual int32_t submitPacket();
    virtual int32_t destroyPacket();
    int32_t submitPacket(MediaTaskCm* pTask) { return 0; };

protected:
    SSH* getSSH() { return pSsh_; };
    ISH* getISH() { return pIsh_; };
    DSH* getDSH() { return pDsh_; };

    int32_t initSSH(std::vector<MediaResourceBind> resBindList);
    int32_t initISH(std::vector<uint8_t>& kernelCISA);
    int32_t initDSH(uint8_t* pCurbeData, int32_t curbeSize);
    int32_t destroyStateHeap();

private:
    SSH* pSsh_;
    ISH* pIsh_;
    DSH* pDsh_;
};

}
