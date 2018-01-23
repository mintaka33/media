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
//! \file     decode_pipeline.h
//! \brief    decode pipeline class definition
//! \details
//!

#pragma once

#include "packet_ff.h"
#include "mpeg2_decode.h"
#include "media_pipeline.h"

namespace Apogeios
{

class Mpeg2DecTask : public MediaTask
{
public:
    Mpeg2DecTask();
    ~Mpeg2DecTask();

    int32_t create();
    int32_t execute();
    int32_t destroy();

private:
    Mpeg2DecodePkt* decPkt_ = nullptr;
};

class DecodeMpeg2Pipe : public MediaPipe
{
public:
    DecodeMpeg2Pipe();
    virtual ~DecodeMpeg2Pipe();

    int32_t createPipe();
    int32_t executePipe();
    int32_t destroyPipe();

    int32_t allocateResources();
    int32_t destroyResources();

private:
    Mpeg2DecTask* decTask_ = nullptr;
};

}