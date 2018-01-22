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
//! \file     down_scaling.h
//! \brief    down scaling kernel packet class definition
//! \details
//!

#pragma once

#include "common.h"
#include "packet_kernel.h"

namespace Apogeios
{

typedef struct _ScalingCurbeDataGen9
{
    // DWORD 0 - GRF R1.0
    union
    {
        struct
        {
            uint32_t   InputPictureWidth : UTIL_BITFIELD_RANGE(0, 15);
            uint32_t   InputPictureHeight : UTIL_BITFIELD_RANGE(16, 31);
        };

        uint32_t       Value;
    } DW0;

    // DW1
    union
    {
        struct
        {
            uint32_t   InputYBTIFrame : UTIL_BITFIELD_RANGE(0, 31);
        };
        struct
        {
            uint32_t   InputYBTITopField : UTIL_BITFIELD_RANGE(0, 31);
        };
        struct
        {
            uint32_t   Value;
        };
    } DW1;

    // DW2
    union
    {
        struct
        {
            uint32_t   OutputYBTIFrame : UTIL_BITFIELD_RANGE(0, 31);
        };
        struct
        {
            uint32_t   OutputYBTITopField : UTIL_BITFIELD_RANGE(0, 31);
        };
        struct
        {
            uint32_t   Value;
        };
    } DW2;

    // DW3
    union
    {
        struct
        {
            uint32_t   InputYBTIBottomField : UTIL_BITFIELD_RANGE(0, 31);
        };
        struct
        {
            uint32_t   Value;
        };
    } DW3;

    // DW4
    union
    {
        struct
        {
            uint32_t   OutputYBTIBottomField : UTIL_BITFIELD_RANGE(0, 31);
        };
        struct
        {
            uint32_t   Value;
        };
    } DW4;

    // DW5
    union
    {
        struct
        {
            uint32_t   FlatnessThreshold : UTIL_BITFIELD_RANGE(0, 31);
        };
        struct
        {
            uint32_t   Value;
        };
    } DW5;

    // DW6
    union
    {
        struct
        {
            uint32_t   EnableMBFlatnessCheck : UTIL_BITFIELD_BIT(0);
            uint32_t   EnableMBVarianceOutput : UTIL_BITFIELD_BIT(1);
            uint32_t   EnableMBPixelAverageOutput : UTIL_BITFIELD_BIT(2);
            uint32_t   EnableBlock8x8StatisticsOutput : UTIL_BITFIELD_BIT(3);
        uint32_t: UTIL_BITFIELD_RANGE(4, 31);
        };
        struct
        {
            uint32_t   Value;
        };
    } DW6;

    // DW7
    union
    {
        struct
        {
            uint32_t   Reserved : UTIL_BITFIELD_RANGE(1, 31);
        };
        struct
        {
            uint32_t   Value;
        };
    } DW7;

    // DW8
    union
    {
        struct
        {
            uint32_t   MBVProcStatsBTIFrame : UTIL_BITFIELD_RANGE(0, 31);
        };
        struct
        {
            uint32_t   MBVProcStatsBTITopField : UTIL_BITFIELD_RANGE(0, 31);
        };
        struct
        {
            uint32_t   Value;
        };
    } DW8;

    // DW9
    union
    {
        struct
        {
            uint32_t   MBVProcStatsBTIBottomField : UTIL_BITFIELD_RANGE(0, 31);
        };
        struct
        {
            uint32_t   Value;
        };
    } DW9;

    // DW10
    union
    {
        struct
        {
            uint32_t   Reserved : UTIL_BITFIELD_RANGE(0, 31);
        };
        struct
        {
            uint32_t   Value;
        };
    } DW10;

    // DW11
    union
    {
        struct
        {
            uint32_t   Reserved : UTIL_BITFIELD_RANGE(0, 31);
        };
        struct
        {
            uint32_t   Value;
        };
    } DW11;

    //DW12
    union
    {
        struct
        {
            uint32_t   Reserved : UTIL_BITFIELD_RANGE(0, 31);
        };
        struct
        {
            uint32_t   Value;
        };
    } DW12;

    //DW13
    union
    {
        struct
        {
            uint32_t   Reserved : UTIL_BITFIELD_RANGE(0, 31);
        };
        struct
        {
            uint32_t   Value;
        };
    } DW13;

    //DW14
    union
    {
        struct
        {
            uint32_t   Reserved : UTIL_BITFIELD_RANGE(0, 31);
        };
        struct
        {
            uint32_t   Value;
        };
    } DW14;

    //DW15
    union
    {
        struct
        {
            uint32_t   Reserved : UTIL_BITFIELD_RANGE(0, 31);
        };
        struct
        {
            uint32_t   Value;
        };
    } DW15;
} ScalingCurbeDataGen9;

struct ScalingResBind
{
    MediaResourceBind scalingIn = { nullptr, 0, 2 };
    MediaResourceBind scalingOut = { nullptr, 2, 2 };
};

class ScalingPacket : public KernelPacket
{
public:
    ScalingPacket(MediaResource* pSrc, MediaResource* pDst);
    virtual ~ScalingPacket();

    int32_t createPacket();
    int32_t preparePacket();
    int32_t submitPacket();
    int32_t destroyPacket();

private:
    int32_t constructCmdSequence(uint32_t threadWidth, uint32_t threadHeight);
    int32_t initStateHeap();

private:
    bool isField_ = false;
    uint32_t kernelBlockWidth_ = 0;
    uint32_t kernelBlockHeight_ = 0;

    // external resources
    ScalingResBind resBind_;
    std::vector<MediaResourceBind> sshResBindList_;
    ScalingCurbeDataGen9 curbe_ = {};

    MediaResource* statusReport_ = nullptr;
    MediaResource* timeStamp_ = nullptr;
    MediaResource* cmdBufIdGlobal_ = nullptr;

    static std::vector<uint32_t> kernelScalingFrameGen9ISA_;
    static std::vector<uint32_t> kernelScalingFieldGen9ISA_;
};

}