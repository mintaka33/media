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
//! \file     media_task.cpp
//! \brief    media class implementation
//! \details
//!

#include "media_task.h"

namespace Apogeios
{

MediaTask::MediaTask()
{
}

MediaTask::~MediaTask()
{
}

int32_t MediaTask::createTask()
{
    return 0;
}

int32_t MediaTask::addPacket(CmdPacket* pPacket)
{
    if (pPacket == nullptr)
        return -1;

    packetArray_.push_back(pPacket);

    return 0;
}

int32_t MediaTask::submitTask(const bool singlePhase)
{
    int32_t result = 0;

    if (singlePhase)
    {
        // APGTODO: combine multiple packets together and submit once
    }
    else
    {
        // multiple command submission
        CmdPacket* packet = nullptr;
        for (packet : packetArray_)
        {
            if (packet->submitPacket() != 0)
            {
                return -1;
            }
        }
    }

    return 0;
}

int32_t MediaTask::destroyTask()
{
    return 0;
}

#if 0
MediaTaskCm::MediaTaskCm(CmDevice *pDevice)
{
    pCmDevice = pDevice;
    packetArray.clear();
}

MediaTaskCm::~MediaTaskCm()
{
}

int32_t MediaTaskCm::createTask()
{
    int32_t result = 0;

    if (pCmDevice == nullptr)
        return -1;

    if (!pCmQueue)
    {
        CM_QUEUE_CREATE_OPTION queueCreateOption = CM_DEFAULT_QUEUE_CREATE_OPTION;
        result = pCmDevice->CreateQueueEx(pCmQueue, queueCreateOption);
        if (result != 0) return -1;
    }

    if (!pCmTask)
    {
        result = pCmDevice->CreateTask(pCmTask);
        if (result != 0) return -1;
    }

    return 0;
}

int32_t MediaTaskCm::addPacket(CmdPacket* pPacket)
{
    int32_t result = 0;

    if (pPacket == nullptr)
        return -1;

    packetArray.push_back(pPacket);

    return 0;
}

int32_t MediaTaskCm::appendKernel(CmKernel* kernel)
{
    int32_t result = 0;

    if (kernel == nullptr || !pCmTask)
        return -1;

    result = pCmTask->AddKernel(kernel);
    if (result != 0) return -1;

    return 0;
}

int32_t MediaTaskCm::executeTask(const bool singlePhase)
{
    int32_t result = 0;

    if (!pCmTask || !pCmQueue)
        return -1;

    if (singlePhase)
    {
        // single command packet submission
        for each (CmdPacket* packet in packetArray)
        {
            result = packet->submitPacket(this);
            if (result != 0) return -1;

            result = pCmTask->AddSync();
            if (result != 0) return -1;
        }

        result = pCmQueue->Enqueue(pCmTask, pCmEvent);
        if (result != 0) return -1;
    }
    else
    {
        // multiple command submission
        for each (CmdPacket* packet in packetArray)
        {
            result = packet->submitPacket(this);
            if (result != 0) return -1;

            result = pCmQueue->Enqueue(pCmTask, pCmEvent);
            if (result != 0) return -1;
        }
    }

    return 0;
}

int32_t MediaTaskCm::destroyTask()
{
    int32_t result = 0;

    if (pCmDevice == nullptr)
        return -1;

    if (pCmTask)
    {
        result = pCmDevice->DestroyTask(pCmTask);
        if (result != 0) return -1;
    }

    return 0;
}
#endif

}