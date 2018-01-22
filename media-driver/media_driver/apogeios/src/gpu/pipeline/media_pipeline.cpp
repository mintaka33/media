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
//! \file     media_pipeline.cpp
//! \brief    media pipeline class implementation
//! \details
//!

#include "media_pipeline.h"

namespace Apogeios
{

MediaPipe::MediaPipe()
{
}

MediaPipe::~MediaPipe()
{
}

void MediaPipe::addTask(MediaTask* task)
{
    if (task != nullptr)
    {
        taskList_.push_back(task);
    }
}

int32_t MediaPipe::flushPipe()
{
    int32_t result = 0;
    for (MediaTask* task : taskList_)
    {
        if (task)
        {
            result = task->execute();
            if (result != 0)
                return -1;
        }
    }

    return 0;
}

int32_t MediaPipe::resetPipe()
{
    taskList_.clear();

    return 0;
}

}