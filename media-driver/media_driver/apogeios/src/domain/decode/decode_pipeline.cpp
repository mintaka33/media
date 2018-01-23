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
//! \file     decode_pipeline.cpp
//! \brief    decode pipeline class implementation
//! \details
//!

#include "common.h"
#include "decode_pipeline.h"

namespace Apogeios
{

Mpeg2DecTask::Mpeg2DecTask()
{
}

Mpeg2DecTask::~Mpeg2DecTask()
{
}

int32_t Mpeg2DecTask::create()
{
    decPkt_ = new Mpeg2DecodePkt();

    if (decPkt_->createPacket() != 0)
        return -1;

    return 0;
}

int32_t Mpeg2DecTask::execute()
{
    // add mpeg2 decode packet into task
    if (decPkt_)
    {
        if (decPkt_->preparePacket() != 0)
            return -1;

        addPacket(decPkt_);
    }

    if (submitTask(false) != 0)
        return -1;

    return 0;
}

int32_t Mpeg2DecTask::destroy()
{
    if (decPkt_)
    {
        if (decPkt_->destroyPacket() != 0)
            return -1;
        delete decPkt_;
    }

    if (destroyTask() != 0)
        return -1;

    return 0;
}

DecodeMpeg2Pipe::DecodeMpeg2Pipe()
{
}
DecodeMpeg2Pipe::~DecodeMpeg2Pipe()
{
}

int32_t DecodeMpeg2Pipe::createPipe()
{
    if (allocateResources() != 0)
    {
        return -1;
    }

    decTask_ = new Mpeg2DecTask();
    if (decTask_->create() != 0)
    {
        return -1;
    }

    return 0;
}

int32_t DecodeMpeg2Pipe::executePipe()
{
    if (resetPipe() != 0)
        return -1;

    addTask(decTask_);

    if (flushPipe() != 0)
        return -1;

    return 0;
}

int32_t DecodeMpeg2Pipe::destroyPipe()
{
    if (resetPipe() != 0)
        return -1;

    if (decTask_)
    {
        if (decTask_->destroy() != 0)
            return -1;
        delete decTask_;
    }

    if (destroyResources() != 0)
    {
        return -1;
    }

    return 0;
}

int32_t DecodeMpeg2Pipe::allocateResources()
{
    return 0;
}

int32_t DecodeMpeg2Pipe::destroyResources()
{
    return 0;
}

}