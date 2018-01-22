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
//! \file     media_task.h
//! \brief    media task class definition
//! \details
//!

#pragma once

#include "packet.h"
#include "cm_wrapper.h"

namespace Apogeios
{

class MediaTask
{
public:
    MediaTask();
    virtual ~MediaTask();

    // APGTODO: should change it as pure virtual function once delete old client code
    virtual int32_t create() { return 0; };
    virtual int32_t execute() { return 0; };
    virtual int32_t destroy() { return 0; };

    int32_t createTask();
    int32_t addPacket(CmdPacket* pPacket);
    int32_t submitTask(const bool singlePhase);
    int32_t destroyTask();

private:
    std::vector<CmdPacket*> packetArray_;
};

#if 0
class MediaTaskCm
{
public:
    MediaTaskCm(CmDevice *pDevice);
    virtual ~MediaTaskCm();

    int32_t createTask();
    int32_t addPacket(CmdPacket* pPacket);
    int32_t appendKernel(CmKernel* kernel);
    int32_t executeTask(const bool singlePhase);
    int32_t destroyTask();

private:
    CmDevice *pCmDevice = nullptr; // CmDevice is created by client from external
    CmQueue *pCmQueue = nullptr;
    CmTask *pCmTask = nullptr;
    CmEvent *pCmEvent = nullptr;

    std::vector<CmdPacket*> packetArray;
};
#endif

}