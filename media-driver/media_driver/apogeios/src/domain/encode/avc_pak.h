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
//! \file     avc_pak.h
//! \brief    avc PAK packet class definition
//! \details
//!

#pragma once

#include "packet_ff.h"

namespace Apogeios
{

class AvcPakPacket : public FfPacket
{
public:
    AvcPakPacket(MediaResource* inputYuv, MediaResource* reconYuv, MediaResource* bitstreamBuf);
    ~AvcPakPacket();

    int32_t createPacket();
    int32_t preparePacket();
    int32_t submitPacket();
    int32_t destroyPacket();

private:
    int32_t constructCmdSequence();
    int32_t allocateResource();
    int32_t destroyResource();

private:
    const bool bDisable2ndLevelBatchBuf_ = true;
    MediaResource* inputYuv_ = nullptr;
    MediaResource* reconYuv_ = nullptr;
    MediaResource* bitstreamBuf_ = nullptr;
    MediaResource* statusReport_ = nullptr;
};

}