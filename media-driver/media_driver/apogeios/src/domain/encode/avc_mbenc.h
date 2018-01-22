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
//! \file     avc_mbenc.h
//! \brief    avc mbenc kernel packet class definition
//! \details
//!

#pragma once

#include <string>
#include <map>
#include "common.h"
#include "packet_kernel.h"

namespace Apogeios
{

typedef struct _MESearchPathDelta
{
    uint8_t   SearchPathDelta_X : 4;
    uint8_t   SearchPathDelta_Y : 4;
} MESearchPathDelta;

typedef struct _MbEncCurbeDataGen9
{
    // DW0
    union
    {
        struct
        {
            uint32_t   SkipModeEn : UTIL_BITFIELD_BIT(0);
            uint32_t   AdaptiveEn : UTIL_BITFIELD_BIT(1);
            uint32_t   BiMixDis : UTIL_BITFIELD_BIT(2);
            uint32_t: UTIL_BITFIELD_RANGE(3, 4);
            uint32_t   EarlyImeSuccessEn : UTIL_BITFIELD_BIT(5);
            uint32_t: UTIL_BITFIELD_BIT(6);
            uint32_t   T8x8FlagForInterEn : UTIL_BITFIELD_BIT(7);
            uint32_t: UTIL_BITFIELD_RANGE(8, 23);
            uint32_t   EarlyImeStop : UTIL_BITFIELD_RANGE(24, 31);
        };
        struct
        {
            uint32_t   Value;
        };
    } DW0;

    // DW1
    union
    {
        struct
        {
            uint32_t   MaxNumMVs : UTIL_BITFIELD_RANGE(0, 5);
            uint32_t   ExtendedMvCostRange : UTIL_BITFIELD_BIT(6);
            uint32_t: UTIL_BITFIELD_RANGE(7, 15);
            uint32_t   BiWeight : UTIL_BITFIELD_RANGE(16, 21);
            uint32_t: UTIL_BITFIELD_RANGE(22, 27);
            uint32_t   UniMixDisable : UTIL_BITFIELD_BIT(28);
            uint32_t: UTIL_BITFIELD_RANGE(29, 31);
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
            uint32_t   LenSP : UTIL_BITFIELD_RANGE(0, 7);
            uint32_t   MaxNumSU : UTIL_BITFIELD_RANGE(8, 15);
            uint32_t   PicWidth : UTIL_BITFIELD_RANGE(16, 31);
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
            uint32_t   SrcSize : UTIL_BITFIELD_RANGE(0, 1);
            uint32_t: UTIL_BITFIELD_RANGE(2, 3);
            uint32_t   MbTypeRemap : UTIL_BITFIELD_RANGE(4, 5);
            uint32_t   SrcAccess : UTIL_BITFIELD_BIT(6);
            uint32_t   RefAccess : UTIL_BITFIELD_BIT(7);
            uint32_t   SearchCtrl : UTIL_BITFIELD_RANGE(8, 10);
            uint32_t   DualSearchPathOption : UTIL_BITFIELD_BIT(11);
            uint32_t   SubPelMode : UTIL_BITFIELD_RANGE(12, 13);
            uint32_t   SkipType : UTIL_BITFIELD_BIT(14);
            uint32_t   DisableFieldCacheAlloc : UTIL_BITFIELD_BIT(15);
            uint32_t   InterChromaMode : UTIL_BITFIELD_BIT(16);
            uint32_t   FTEnable : UTIL_BITFIELD_BIT(17);
            uint32_t   BMEDisableFBR : UTIL_BITFIELD_BIT(18);
            uint32_t   BlockBasedSkipEnable : UTIL_BITFIELD_BIT(19);
            uint32_t   InterSAD : UTIL_BITFIELD_RANGE(20, 21);
            uint32_t   IntraSAD : UTIL_BITFIELD_RANGE(22, 23);
            uint32_t   SubMbPartMask : UTIL_BITFIELD_RANGE(24, 30);
            uint32_t: UTIL_BITFIELD_BIT(31);
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
            uint32_t   PicHeightMinus1 : UTIL_BITFIELD_RANGE(0, 15);
            uint32_t   MvRestrictionInSliceEnable : UTIL_BITFIELD_BIT(16);
            uint32_t   DeltaMvEnable : UTIL_BITFIELD_BIT(17);
            uint32_t   TrueDistortionEnable : UTIL_BITFIELD_BIT(18);
            uint32_t   EnableWavefrontOptimization : UTIL_BITFIELD_BIT(19);
            uint32_t   EnableFBRBypass : UTIL_BITFIELD_BIT(20);
            uint32_t   EnableIntraCostScalingForStaticFrame : UTIL_BITFIELD_BIT(21);
            uint32_t   EnableIntraRefresh : UTIL_BITFIELD_BIT(22);
            uint32_t   Reserved : UTIL_BITFIELD_BIT(23);
            uint32_t   EnableDirtyRect : UTIL_BITFIELD_BIT(24);
            uint32_t   bCurFldIDR : UTIL_BITFIELD_BIT(25);
            uint32_t   ConstrainedIntraPredFlag : UTIL_BITFIELD_BIT(26);
            uint32_t   FieldParityFlag : UTIL_BITFIELD_BIT(27);
            uint32_t   HMEEnable : UTIL_BITFIELD_BIT(28);
            uint32_t   PictureType : UTIL_BITFIELD_RANGE(29, 30);
            uint32_t   UseActualRefQPValue : UTIL_BITFIELD_BIT(31);
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
            uint32_t   SliceMbHeight : UTIL_BITFIELD_RANGE(0, 15);
            uint32_t   RefWidth : UTIL_BITFIELD_RANGE(16, 23);
            uint32_t   RefHeight : UTIL_BITFIELD_RANGE(24, 31);
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
            uint32_t   BatchBufferEnd : UTIL_BITFIELD_RANGE(0, 31);
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
            uint32_t   IntraPartMask : UTIL_BITFIELD_RANGE(0, 4);
            uint32_t   NonSkipZMvAdded : UTIL_BITFIELD_BIT(5);
            uint32_t   NonSkipModeAdded : UTIL_BITFIELD_BIT(6);
            uint32_t   LumaIntraSrcCornerSwap : UTIL_BITFIELD_BIT(7);
            uint32_t: UTIL_BITFIELD_RANGE(8, 15);
            uint32_t   MVCostScaleFactor : UTIL_BITFIELD_RANGE(16, 17);
            uint32_t   BilinearEnable : UTIL_BITFIELD_BIT(18);
            uint32_t   SrcFieldPolarity : UTIL_BITFIELD_BIT(19);
            uint32_t   WeightedSADHAAR : UTIL_BITFIELD_BIT(20);
            uint32_t   AConlyHAAR : UTIL_BITFIELD_BIT(21);
            uint32_t   RefIDCostMode : UTIL_BITFIELD_BIT(22);
            uint32_t: UTIL_BITFIELD_BIT(23);
            uint32_t   SkipCenterMask : UTIL_BITFIELD_RANGE(24, 31);
        };
        struct
        {
            uint32_t   Value;
        };
    } DW7;

    struct
    {
        // DW8
        union
        {
            struct
            {
                uint32_t   Mode0Cost : UTIL_BITFIELD_RANGE(0, 7);
                uint32_t   Mode1Cost : UTIL_BITFIELD_RANGE(8, 15);
                uint32_t   Mode2Cost : UTIL_BITFIELD_RANGE(16, 23);
                uint32_t   Mode3Cost : UTIL_BITFIELD_RANGE(24, 31);
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
                uint32_t   Mode4Cost : UTIL_BITFIELD_RANGE(0, 7);
                uint32_t   Mode5Cost : UTIL_BITFIELD_RANGE(8, 15);
                uint32_t   Mode6Cost : UTIL_BITFIELD_RANGE(16, 23);
                uint32_t   Mode7Cost : UTIL_BITFIELD_RANGE(24, 31);
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
                uint32_t   Mode8Cost : UTIL_BITFIELD_RANGE(0, 7);
                uint32_t   Mode9Cost : UTIL_BITFIELD_RANGE(8, 15);
                uint32_t   RefIDCost : UTIL_BITFIELD_RANGE(16, 23);
                uint32_t   ChromaIntraModeCost : UTIL_BITFIELD_RANGE(24, 31);
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
                uint32_t   MV0Cost : UTIL_BITFIELD_RANGE(0, 7);
                uint32_t   MV1Cost : UTIL_BITFIELD_RANGE(8, 15);
                uint32_t   MV2Cost : UTIL_BITFIELD_RANGE(16, 23);
                uint32_t   MV3Cost : UTIL_BITFIELD_RANGE(24, 31);
            };
            struct
            {
                uint32_t   Value;
            };
        } DW11;

        // DW12
        union
        {
            struct
            {
                uint32_t   MV4Cost : UTIL_BITFIELD_RANGE(0, 7);
                uint32_t   MV5Cost : UTIL_BITFIELD_RANGE(8, 15);
                uint32_t   MV6Cost : UTIL_BITFIELD_RANGE(16, 23);
                uint32_t   MV7Cost : UTIL_BITFIELD_RANGE(24, 31);
            };
            struct
            {
                uint32_t   Value;
            };
        } DW12;

        // DW13
        union
        {
            struct
            {
                uint32_t   QpPrimeY : UTIL_BITFIELD_RANGE(0, 7);
                uint32_t   QpPrimeCb : UTIL_BITFIELD_RANGE(8, 15);
                uint32_t   QpPrimeCr : UTIL_BITFIELD_RANGE(16, 23);
                uint32_t   TargetSizeInWord : UTIL_BITFIELD_RANGE(24, 31);
            };
            struct
            {
                uint32_t   Value;
            };
        } DW13;

        // DW14
        union
        {
            struct
            {
                uint32_t   SICFwdTransCoeffThreshold_0 : UTIL_BITFIELD_RANGE(0, 15);
                uint32_t   SICFwdTransCoeffThreshold_1 : UTIL_BITFIELD_RANGE(16, 23);
                uint32_t   SICFwdTransCoeffThreshold_2 : UTIL_BITFIELD_RANGE(24, 31);
            };
            struct
            {
                uint32_t   Value;
            };
        } DW14;

        // DW15
        union
        {
            struct
            {
                uint32_t   SICFwdTransCoeffThreshold_3 : UTIL_BITFIELD_RANGE(0, 7);
                uint32_t   SICFwdTransCoeffThreshold_4 : UTIL_BITFIELD_RANGE(8, 15);
                uint32_t   SICFwdTransCoeffThreshold_5 : UTIL_BITFIELD_RANGE(16, 23);
                uint32_t   SICFwdTransCoeffThreshold_6 : UTIL_BITFIELD_RANGE(24, 31);    // Highest Freq
            };
            struct
            {
                uint32_t   Value;
            };
        } DW15;
    } ModeMvCost;

    struct
    {
        // DW16
        union
        {
            struct
            {
                MESearchPathDelta   SPDelta_0;
                MESearchPathDelta   SPDelta_1;
                MESearchPathDelta   SPDelta_2;
                MESearchPathDelta   SPDelta_3;
            };
            struct
            {
                uint32_t   Value;
            };
        } DW16;

        // DW17
        union
        {
            struct
            {
                MESearchPathDelta   SPDelta_4;
                MESearchPathDelta   SPDelta_5;
                MESearchPathDelta   SPDelta_6;
                MESearchPathDelta   SPDelta_7;
            };
            struct
            {
                uint32_t   Value;
            };
        } DW17;

        // DW18
        union
        {
            struct
            {
                MESearchPathDelta   SPDelta_8;
                MESearchPathDelta   SPDelta_9;
                MESearchPathDelta   SPDelta_10;
                MESearchPathDelta   SPDelta_11;
            };
            struct
            {
                uint32_t   Value;
            };
        } DW18;

        // DW19
        union
        {
            struct
            {
                MESearchPathDelta   SPDelta_12;
                MESearchPathDelta   SPDelta_13;
                MESearchPathDelta   SPDelta_14;
                MESearchPathDelta   SPDelta_15;
            };
            struct
            {
                uint32_t   Value;
            };
        } DW19;

        // DW20
        union
        {
            struct
            {
                MESearchPathDelta   SPDelta_16;
                MESearchPathDelta   SPDelta_17;
                MESearchPathDelta   SPDelta_18;
                MESearchPathDelta   SPDelta_19;
            };
            struct
            {
                uint32_t   Value;
            };
        } DW20;

        // DW21
        union
        {
            struct
            {
                MESearchPathDelta   SPDelta_20;
                MESearchPathDelta   SPDelta_21;
                MESearchPathDelta   SPDelta_22;
                MESearchPathDelta   SPDelta_23;
            };
            struct
            {
                uint32_t   Value;
            };
        } DW21;

        // DW22
        union
        {
            struct
            {
                MESearchPathDelta   SPDelta_24;
                MESearchPathDelta   SPDelta_25;
                MESearchPathDelta   SPDelta_26;
                MESearchPathDelta   SPDelta_27;
            };
            struct
            {
                uint32_t   Value;
            };
        } DW22;

        // DW23
        union
        {
            struct
            {
                MESearchPathDelta   SPDelta_28;
                MESearchPathDelta   SPDelta_29;
                MESearchPathDelta   SPDelta_30;
                MESearchPathDelta   SPDelta_31;
            };
            struct
            {
                uint32_t   Value;
            };
        } DW23;

        // DW24
        union
        {
            struct
            {
                MESearchPathDelta   SPDelta_32;
                MESearchPathDelta   SPDelta_33;
                MESearchPathDelta   SPDelta_34;
                MESearchPathDelta   SPDelta_35;
            };
            struct
            {
                uint32_t   Value;
            };
        } DW24;

        // DW25
        union
        {
            struct
            {
                MESearchPathDelta   SPDelta_36;
                MESearchPathDelta   SPDelta_37;
                MESearchPathDelta   SPDelta_38;
                MESearchPathDelta   SPDelta_39;
            };
            struct
            {
                uint32_t   Value;
            };
        } DW25;

        // DW26
        union
        {
            struct
            {
                MESearchPathDelta   SPDelta_40;
                MESearchPathDelta   SPDelta_41;
                MESearchPathDelta   SPDelta_42;
                MESearchPathDelta   SPDelta_43;
            };
            struct
            {
                uint32_t   Value;
            };
        } DW26;

        // DW27
        union
        {
            struct
            {
                MESearchPathDelta   SPDelta_44;
                MESearchPathDelta   SPDelta_45;
                MESearchPathDelta   SPDelta_46;
                MESearchPathDelta   SPDelta_47;
            };
            struct
            {
                uint32_t   Value;
            };
        } DW27;

        // DW28
        union
        {
            struct
            {
                MESearchPathDelta   SPDelta_48;
                MESearchPathDelta   SPDelta_49;
                MESearchPathDelta   SPDelta_50;
                MESearchPathDelta   SPDelta_51;
            };
            struct
            {
                uint32_t   Value;
            };
        } DW28;

        // DW29
        union
        {
            struct
            {
                MESearchPathDelta   SPDelta_52;
                MESearchPathDelta   SPDelta_53;
                MESearchPathDelta   SPDelta_54;
                MESearchPathDelta   SPDelta_55;
            };
            struct
            {
                uint32_t   Value;
            };
        } DW29;

        // DW30
        union
        {
            struct
            {
                uint32_t   Intra4x4ModeMask : UTIL_BITFIELD_RANGE(0, 8);
                uint32_t: UTIL_BITFIELD_RANGE(9, 15);
                uint32_t   Intra8x8ModeMask : UTIL_BITFIELD_RANGE(16, 24);
                uint32_t: UTIL_BITFIELD_RANGE(25, 31);
            };
            struct
            {
                uint32_t   Value;
            };
        } DW30;

        // DW31
        union
        {
            struct
            {
                uint32_t   Intra16x16ModeMask : UTIL_BITFIELD_RANGE(0, 3);
                uint32_t   IntraChromaModeMask : UTIL_BITFIELD_RANGE(4, 7);
                uint32_t   IntraComputeType : UTIL_BITFIELD_RANGE(8, 9);
                uint32_t: UTIL_BITFIELD_RANGE(10, 31);
            };
            struct
            {
                uint32_t   Value;
            };
        } DW31;
    } SPDelta;

    // DW32
    union
    {
        struct
        {
            uint32_t   SkipVal : UTIL_BITFIELD_RANGE(0, 15);
            uint32_t   MultiPredL0Disable : UTIL_BITFIELD_RANGE(16, 23);
            uint32_t   MultiPredL1Disable : UTIL_BITFIELD_RANGE(24, 31);
        };
        struct
        {
            uint32_t   Value;
        };
    } DW32;

    // DW33
    union
    {
        struct
        {
            uint32_t   Intra16x16NonDCPredPenalty : UTIL_BITFIELD_RANGE(0, 7);
            uint32_t   Intra8x8NonDCPredPenalty : UTIL_BITFIELD_RANGE(8, 15);
            uint32_t   Intra4x4NonDCPredPenalty : UTIL_BITFIELD_RANGE(16, 23);
            uint32_t: UTIL_BITFIELD_RANGE(24, 31);
        };
        struct
        {
            uint32_t   Value;
        };
    } DW33;

    // DW34
    union
    {
        struct
        {
            uint32_t   List0RefID0FieldParity : UTIL_BITFIELD_BIT(0);
            uint32_t   List0RefID1FieldParity : UTIL_BITFIELD_BIT(1);
            uint32_t   List0RefID2FieldParity : UTIL_BITFIELD_BIT(2);
            uint32_t   List0RefID3FieldParity : UTIL_BITFIELD_BIT(3);
            uint32_t   List0RefID4FieldParity : UTIL_BITFIELD_BIT(4);
            uint32_t   List0RefID5FieldParity : UTIL_BITFIELD_BIT(5);
            uint32_t   List0RefID6FieldParity : UTIL_BITFIELD_BIT(6);
            uint32_t   List0RefID7FieldParity : UTIL_BITFIELD_BIT(7);
            uint32_t   List1RefID0FrameFieldFlag : UTIL_BITFIELD_BIT(8);
            uint32_t   List1RefID1FrameFieldFlag : UTIL_BITFIELD_BIT(9);
            uint32_t   IntraRefreshEn : UTIL_BITFIELD_RANGE(10, 11);
            uint32_t   ArbitraryNumMbsPerSlice : UTIL_BITFIELD_BIT(12);
            uint32_t   TQEnable : UTIL_BITFIELD_BIT(13);
            uint32_t   ForceNonSkipMbEnable : UTIL_BITFIELD_BIT(14);
            uint32_t   DisableEncSkipCheck : UTIL_BITFIELD_BIT(15);
            uint32_t   EnableDirectBiasAdjustment : UTIL_BITFIELD_BIT(16);
            uint32_t   bForceToSkip : UTIL_BITFIELD_BIT(17);
            uint32_t   EnableGlobalMotionBiasAdjustment : UTIL_BITFIELD_BIT(18);
            uint32_t   EnableAdaptiveTxDecision : UTIL_BITFIELD_BIT(19);
            uint32_t   EnablePerMBStaticCheck : UTIL_BITFIELD_BIT(20);
            uint32_t   EnableAdaptiveSearchWindowSize : UTIL_BITFIELD_BIT(21);
            uint32_t: UTIL_BITFIELD_BIT(22);
            uint32_t   CQPFlag : UTIL_BITFIELD_BIT(23);
            uint32_t   List1RefID0FieldParity : UTIL_BITFIELD_BIT(24);
            uint32_t   List1RefID1FieldParity : UTIL_BITFIELD_BIT(25);
            uint32_t   MADEnableFlag : UTIL_BITFIELD_BIT(26);
            uint32_t   ROIEnableFlag : UTIL_BITFIELD_BIT(27);
            uint32_t   EnableMBFlatnessChkOptimization : UTIL_BITFIELD_BIT(28);
            uint32_t   bDirectMode : UTIL_BITFIELD_BIT(29);
            uint32_t   MBBrcEnable : UTIL_BITFIELD_BIT(30);
            uint32_t   bOriginalBff : UTIL_BITFIELD_BIT(31);
        };
        struct
        {
            uint32_t   Value;
        };
    } DW34;

    // DW35
    union
    {
        struct
        {
            uint32_t   PanicModeMBThreshold : UTIL_BITFIELD_RANGE(0, 15);
            uint32_t   SmallMbSizeInWord : UTIL_BITFIELD_RANGE(16, 23);
            uint32_t   LargeMbSizeInWord : UTIL_BITFIELD_RANGE(24, 31);
        };
        struct
        {
            uint32_t   Value;
        };
    } DW35;

    // DW36
    union
    {
        struct
        {
            uint32_t   NumRefIdxL0MinusOne : UTIL_BITFIELD_RANGE(0, 7);
            uint32_t   HMECombinedExtraSUs : UTIL_BITFIELD_RANGE(8, 15);
            uint32_t   NumRefIdxL1MinusOne : UTIL_BITFIELD_RANGE(16, 23);
            uint32_t: UTIL_BITFIELD_RANGE(24, 27);
            uint32_t   IsFwdFrameShortTermRef : UTIL_BITFIELD_BIT(28);
            uint32_t   CheckAllFractionalEnable : UTIL_BITFIELD_BIT(29);
            uint32_t   HMECombineOverlap : UTIL_BITFIELD_RANGE(30, 31);
        };
        struct
        {
            uint32_t   Value;
        };
    } DW36;

    // DW37
    union
    {
        struct
        {
            uint32_t   SkipModeEn : UTIL_BITFIELD_BIT(0);
            uint32_t   AdaptiveEn : UTIL_BITFIELD_BIT(1);
            uint32_t   BiMixDis : UTIL_BITFIELD_BIT(2);
            uint32_t: UTIL_BITFIELD_RANGE(3, 4);
            uint32_t   EarlyImeSuccessEn : UTIL_BITFIELD_BIT(5);
            uint32_t: UTIL_BITFIELD_BIT(6);
            uint32_t   T8x8FlagForInterEn : UTIL_BITFIELD_BIT(7);
            uint32_t: UTIL_BITFIELD_RANGE(8, 23);
            uint32_t   EarlyImeStop : UTIL_BITFIELD_RANGE(24, 31);
        };
        struct
        {
            uint32_t   Value;
        };
    } DW37;

    // DW38
    union
    {
        struct
        {
            uint32_t   LenSP : UTIL_BITFIELD_RANGE(0, 7);
            uint32_t   MaxNumSU : UTIL_BITFIELD_RANGE(8, 15);
            uint32_t   RefThreshold : UTIL_BITFIELD_RANGE(16, 31);
        };
        struct
        {
            uint32_t   Value;
        };
    } DW38;

    // DW39
    union
    {
        struct
        {
            uint32_t: UTIL_BITFIELD_RANGE(0, 7);
            uint32_t   HMERefWindowsCombThreshold : UTIL_BITFIELD_RANGE(8, 15);
            uint32_t   RefWidth : UTIL_BITFIELD_RANGE(16, 23);
            uint32_t   RefHeight : UTIL_BITFIELD_RANGE(24, 31);
        };
        struct
        {
            uint32_t   Value;
        };
    } DW39;

    // DW40
    union
    {
        struct
        {
            uint32_t   DistScaleFactorRefID0List0 : UTIL_BITFIELD_RANGE(0, 15);
            uint32_t   DistScaleFactorRefID1List0 : UTIL_BITFIELD_RANGE(16, 31);
        };
        struct
        {
            uint32_t   Value;
        };
    } DW40;

    // DW41
    union
    {
        struct
        {
            uint32_t   DistScaleFactorRefID2List0 : UTIL_BITFIELD_RANGE(0, 15);
            uint32_t   DistScaleFactorRefID3List0 : UTIL_BITFIELD_RANGE(16, 31);
        };
        struct
        {
            uint32_t   Value;
        };
    } DW41;

    // DW42
    union
    {
        struct
        {
            uint32_t   DistScaleFactorRefID4List0 : UTIL_BITFIELD_RANGE(0, 15);
            uint32_t   DistScaleFactorRefID5List0 : UTIL_BITFIELD_RANGE(16, 31);
        };
        struct
        {
            uint32_t   Value;
        };
    } DW42;

    // DW43
    union
    {
        struct
        {
            uint32_t   DistScaleFactorRefID6List0 : UTIL_BITFIELD_RANGE(0, 15);
            uint32_t   DistScaleFactorRefID7List0 : UTIL_BITFIELD_RANGE(16, 31);
        };
        struct
        {
            uint32_t   Value;
        };
    } DW43;

    // DW44
    union
    {
        struct
        {
            uint32_t   ActualQPValueForRefID0List0 : UTIL_BITFIELD_RANGE(0, 7);
            uint32_t   ActualQPValueForRefID1List0 : UTIL_BITFIELD_RANGE(8, 15);
            uint32_t   ActualQPValueForRefID2List0 : UTIL_BITFIELD_RANGE(16, 23);
            uint32_t   ActualQPValueForRefID3List0 : UTIL_BITFIELD_RANGE(24, 31);
        };
        struct
        {
            uint32_t   Value;
        };
    } DW44;

    // DW45
    union
    {
        struct
        {
            uint32_t   ActualQPValueForRefID4List0 : UTIL_BITFIELD_RANGE(0, 7);
            uint32_t   ActualQPValueForRefID5List0 : UTIL_BITFIELD_RANGE(8, 15);
            uint32_t   ActualQPValueForRefID6List0 : UTIL_BITFIELD_RANGE(16, 23);
            uint32_t   ActualQPValueForRefID7List0 : UTIL_BITFIELD_RANGE(24, 31);
        };
        struct
        {
            uint32_t   Value;
        };
    } DW45;

    // DW46
    union
    {
        struct
        {
            uint32_t   ActualQPValueForRefID0List1 : UTIL_BITFIELD_RANGE(0, 7);
            uint32_t   ActualQPValueForRefID1List1 : UTIL_BITFIELD_RANGE(8, 15);
            uint32_t   RefCost : UTIL_BITFIELD_RANGE(16, 31);
        };
        struct
        {
            uint32_t   Value;
        };
    } DW46;

    // DW47
    union
    {
        struct
        {
            uint32_t   MbQpReadFactor : UTIL_BITFIELD_RANGE(0, 7);
            uint32_t   IntraCostSF : UTIL_BITFIELD_RANGE(8, 15);
            uint32_t   MaxVmvR : UTIL_BITFIELD_RANGE(16, 31);
        };
        struct
        {
            uint32_t   Value;
        };
    } DW47;

    //DW48
    union
    {
        struct
        {
            uint32_t   IntraRefreshMBx : UTIL_BITFIELD_RANGE(0, 15);
            uint32_t   IntraRefreshUnitInMBMinus1 : UTIL_BITFIELD_RANGE(16, 23);
            uint32_t   IntraRefreshQPDelta : UTIL_BITFIELD_RANGE(24, 31);
        };
        struct
        {
            uint32_t   Value;
        };
    } DW48;

    // DW49
    union
    {
        struct
        {
            uint32_t   ROI1_X_left : UTIL_BITFIELD_RANGE(0, 15);
            uint32_t   ROI1_Y_top : UTIL_BITFIELD_RANGE(16, 31);
        };
        struct
        {
            uint32_t   Value;
        };
    } DW49;

    // DW50
    union
    {
        struct
        {
            uint32_t   ROI1_X_right : UTIL_BITFIELD_RANGE(0, 15);
            uint32_t   ROI1_Y_bottom : UTIL_BITFIELD_RANGE(16, 31);
        };
        struct
        {
            uint32_t   Value;
        };
    } DW50;

    // DW51
    union
    {
        struct
        {
            uint32_t   ROI2_X_left : UTIL_BITFIELD_RANGE(0, 15);
            uint32_t   ROI2_Y_top : UTIL_BITFIELD_RANGE(16, 31);
        };
        struct
        {
            uint32_t   Value;
        };
    } DW51;

    // DW52
    union
    {
        struct
        {
            uint32_t   ROI2_X_right : UTIL_BITFIELD_RANGE(0, 15);
            uint32_t   ROI2_Y_bottom : UTIL_BITFIELD_RANGE(16, 31);
        };
        struct
        {
            uint32_t   Value;
        };
    } DW52;

    // DW53
    union
    {
        struct
        {
            uint32_t   ROI3_X_left : UTIL_BITFIELD_RANGE(0, 15);
            uint32_t   ROI3_Y_top : UTIL_BITFIELD_RANGE(16, 31);
        };
        struct
        {
            uint32_t   Value;
        };
    } DW53;

    // DW54
    union
    {
        struct
        {
            uint32_t   ROI3_X_right : UTIL_BITFIELD_RANGE(0, 15);
            uint32_t   ROI3_Y_bottom : UTIL_BITFIELD_RANGE(16, 31);
        };
        struct
        {
            uint32_t   Value;
        };
    } DW54;

    // DW55
    union
    {
        struct
        {
            uint32_t   ROI4_X_left : UTIL_BITFIELD_RANGE(0, 15);
            uint32_t   ROI4_Y_top : UTIL_BITFIELD_RANGE(16, 31);
        };
        struct
        {
            uint32_t   Value;
        };
    } DW55;

    // DW56
    union
    {
        struct
        {
            uint32_t   ROI4_X_right : UTIL_BITFIELD_RANGE(0, 15);
            uint32_t   ROI4_Y_bottom : UTIL_BITFIELD_RANGE(16, 31);
        };
        struct
        {
            uint32_t   Value;
        };
    } DW56;

    // DW57
    union
    {
        struct
        {
            uint32_t   ROI1_dQpPrimeY : UTIL_BITFIELD_RANGE(0, 7);
            uint32_t   ROI2_dQpPrimeY : UTIL_BITFIELD_RANGE(8, 15);
            uint32_t   ROI3_dQpPrimeY : UTIL_BITFIELD_RANGE(16, 23);
            uint32_t   ROI4_dQpPrimeY : UTIL_BITFIELD_RANGE(24, 31);
        };
        struct
        {
            uint32_t   Value;
        };
    } DW57;

    // DW58
    union
    {
        struct
        {
            uint32_t   Lambda_8x8Inter : UTIL_BITFIELD_RANGE(0, 15);
            uint32_t   Lambda_8x8Intra : UTIL_BITFIELD_RANGE(16, 31);
        };
        struct
        {
            uint32_t   Value;
        };
    } DW58;

    // DW59
    union
    {
        struct
        {
            uint32_t   Lambda_Inter : UTIL_BITFIELD_RANGE(0, 15);
            uint32_t   Lambda_Intra : UTIL_BITFIELD_RANGE(16, 31);
        };
        struct
        {
            uint32_t   Value;
        };
    } DW59;

    // DW60
    union
    {
        struct
        {
            uint32_t   MBTextureThreshold : UTIL_BITFIELD_RANGE(0, 15);
            uint32_t   TxDecisonThreshold : UTIL_BITFIELD_RANGE(16, 31);
        };
        struct
        {
            uint32_t   Value;
        };
    } DW60;

    // DW61
    union
    {
        struct
        {
            uint32_t   HMEMVCostScalingFactor : UTIL_BITFIELD_RANGE(0, 7);
            uint32_t   Reserved : UTIL_BITFIELD_RANGE(8, 15);
            uint32_t   IntraRefreshMBy : UTIL_BITFIELD_RANGE(16, 31);
        };
        struct
        {
            uint32_t   Value;
        };
    } DW61;

    // DW62
    union
    {
        struct
        {
            uint32_t   IPCM_QP0 : UTIL_BITFIELD_RANGE(0, 7);
            uint32_t   IPCM_QP1 : UTIL_BITFIELD_RANGE(8, 15);
            uint32_t   IPCM_QP2 : UTIL_BITFIELD_RANGE(16, 23);
            uint32_t   IPCM_QP3 : UTIL_BITFIELD_RANGE(24, 31);
        };
        struct
        {
            uint32_t   Value;
        };
    } DW62;

    // DW63
    union
    {
        struct
        {
            uint32_t   IPCM_QP4 : UTIL_BITFIELD_RANGE(0, 7);
            uint32_t   Reserved : UTIL_BITFIELD_RANGE(8, 15);
            uint32_t   IPCM_Thresh0 : UTIL_BITFIELD_RANGE(16, 31);
        };
        struct
        {
            uint32_t   Value;
        };
    } DW63;

    // DW64
    union
    {
        struct
        {
            uint32_t   IPCM_Thresh1 : UTIL_BITFIELD_RANGE(0, 15);
            uint32_t   IPCM_Thresh2 : UTIL_BITFIELD_RANGE(16, 31);
        };
        struct
        {
            uint32_t   Value;
        };
    } DW64;

    // DW65
    union
    {
        struct
        {
            uint32_t   IPCM_Thresh3 : UTIL_BITFIELD_RANGE(0, 15);
            uint32_t   IPCM_Thresh4 : UTIL_BITFIELD_RANGE(16, 31);
        };
        struct
        {
            uint32_t   Value;
        };
    } DW65;

    // DW66
    union
    {
        struct
        {
            uint32_t   MBDataSurfIndex : UTIL_BITFIELD_RANGE(0, 31);
        };
        struct
        {
            uint32_t   Value;
        };
    } DW66;

    // DW67
    union
    {
        struct
        {
            uint32_t   MVDataSurfIndex : UTIL_BITFIELD_RANGE(0, 31);
        };
        struct
        {
            uint32_t   Value;
        };
    } DW67;

    // DW68
    union
    {
        struct
        {
            uint32_t   IDistSurfIndex : UTIL_BITFIELD_RANGE(0, 31);
        };
        struct
        {
            uint32_t   Value;
        };
    } DW68;

    // DW69
    union
    {
        struct
        {
            uint32_t   SrcYSurfIndex : UTIL_BITFIELD_RANGE(0, 31);
        };
        struct
        {
            uint32_t   Value;
        };
    } DW69;

    // DW70
    union
    {
        struct
        {
            uint32_t   MBSpecificDataSurfIndex : UTIL_BITFIELD_RANGE(0, 31);
        };
        struct
        {
            uint32_t   Value;
        };
    } DW70;

    // DW71
    union
    {
        struct
        {
            uint32_t   AuxVmeOutSurfIndex : UTIL_BITFIELD_RANGE(0, 31);
        };
        struct
        {
            uint32_t   Value;
        };
    } DW71;

    // DW72
    union
    {
        struct
        {
            uint32_t   CurrRefPicSelSurfIndex : UTIL_BITFIELD_RANGE(0, 31);
        };
        struct
        {
            uint32_t   Value;
        };
    } DW72;

    // DW73
    union
    {
        struct
        {
            uint32_t   HMEMVPredFwdBwdSurfIndex : UTIL_BITFIELD_RANGE(0, 31);
        };
        struct
        {
            uint32_t   Value;
        };
    } DW73;

    // DW74
    union
    {
        struct
        {
            uint32_t   HMEDistSurfIndex : UTIL_BITFIELD_RANGE(0, 31);
        };
        struct
        {
            uint32_t   Value;
        };
    } DW74;

    // DW75
    union
    {
        struct
        {
            uint32_t   SliceMapSurfIndex : UTIL_BITFIELD_RANGE(0, 31);
        };
        struct
        {
            uint32_t   Value;
        };
    } DW75;

    // DW76
    union
    {
        struct
        {
            uint32_t   FwdFrmMBDataSurfIndex : UTIL_BITFIELD_RANGE(0, 31);
        };
        struct
        {
            uint32_t   Value;
        };
    } DW76;

    // DW77
    union
    {
        struct
        {
            uint32_t   FwdFrmMVSurfIndex : UTIL_BITFIELD_RANGE(0, 31);
        };
        struct
        {
            uint32_t   Value;
        };
    } DW77;

    // DW78
    union
    {
        struct
        {
            uint32_t   MBQPBuffer : UTIL_BITFIELD_RANGE(0, 31);
        };
        struct
        {
            uint32_t   Value;
        };
    } DW78;

    // DW79
    union
    {
        struct
        {
            uint32_t   MBBRCLut : UTIL_BITFIELD_RANGE(0, 31);
        };
        struct
        {
            uint32_t   Value;
        };
    } DW79;

    // DW80
    union
    {
        struct
        {
            uint32_t   VMEInterPredictionSurfIndex : UTIL_BITFIELD_RANGE(0, 31);
        };
        struct
        {
            uint32_t   Value;
        };
    } DW80;

    // DW81
    union
    {
        struct
        {
            uint32_t   VMEInterPredictionMRSurfIndex : UTIL_BITFIELD_RANGE(0, 31);
        };
        struct
        {
            uint32_t   Value;
        };
    } DW81;

    // DW82
    union
    {
        struct
        {
            uint32_t   MBStatsSurfIndex : UTIL_BITFIELD_RANGE(0, 31);
        };
        struct
        {
            uint32_t   Value;
        };
    } DW82;

    // DW83
    union
    {
        struct
        {
            uint32_t   MADSurfIndex : UTIL_BITFIELD_RANGE(0, 31);
        };
        struct
        {
            uint32_t   Value;
        };
    } DW83;

    // DW84
    union
    {
        struct
        {
            uint32_t   BRCCurbeSurfIndex : UTIL_BITFIELD_RANGE(0, 31);
        };
        struct
        {
            uint32_t   Value;
        };
    } DW84;

    // DW85
    union
    {
        struct
        {
            uint32_t   ForceNonSkipMBmapSurface : UTIL_BITFIELD_RANGE(0, 31);
        };
        struct
        {
            uint32_t   Value;
        };
    } DW85;

    // DW86
    union
    {
        struct
        {
            uint32_t   ReservedIndex : UTIL_BITFIELD_RANGE(0, 31);
        };
        struct
        {
            uint32_t   Value;
        };
    } DW86;

    // DW87
    union
    {
        struct
        {
            uint32_t   StaticDetectionCostTableIndex : UTIL_BITFIELD_RANGE(0, 31);
        };
        struct
        {
            uint32_t   Value;
        };
    } DW87;
} MbEncCurbeDataGen9;

struct MBEncConstCurbe
{
    std::vector<uint32_t> normalframeI = {
        0x00000082, 0x00000000, 0x00003910, 0x00a83000, 0x00000000, 0x28300000, 0x05000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x80800000, 0x00040c24, 0x00000000, 0xffff00ff, 0x40000000, 0x00000080, 0x00003900, 0x28300000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000002,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
        0x00000000, 0x00000000, 0x00000000, 0xffffffff, 0x00000000, 0x00000000, 0xffffffff, 0xffffffff,
        0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0x00000000, 0x00000000, 0x00000000
    };
    std::vector<uint32_t> normalfieldI = {
        0x00000082, 0x00000000, 0x00003910, 0x00a830c0, 0x02000000, 0x28300000, 0x05000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x80800000, 0x00040c24, 0x00000000, 0xffff00ff, 0x40000000, 0x00000080, 0x00003900, 0x28300000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000002,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
        0x00000000, 0x00000000, 0x00000000, 0xffffffff, 0x00000000, 0x00000000, 0xffffffff, 0xffffffff,
        0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0x00000000, 0x00000000, 0x00000000
    };
    std::vector<uint32_t> normalframeP = {
        0x000000a3, 0x00000008, 0x00003910, 0x00ae3000, 0x30000000, 0x28300000, 0x05000000, 0x01400060,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x80010000, 0x00040c24, 0x00000000, 0xffff00ff, 0x60000000, 0x000000a1, 0x00003900, 0x28300000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x08000002,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
        0x00000000, 0x00000000, 0x00000000, 0xffffffff, 0x00000000, 0x00000000, 0xffffffff, 0xffffffff,
        0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0x00000000, 0x00000000, 0x00000000
    };
    std::vector<uint32_t> normalfieldP = {
        0x000000a3, 0x00000008, 0x00003910, 0x00ae30c0, 0x30000000, 0x28300000, 0x05000000, 0x01400060,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x80010000, 0x00040c24, 0x00000000, 0xffff00ff, 0x40000000, 0x000000a1, 0x00003900, 0x28300000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x04000002,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
        0x00000000, 0x00000000, 0x00000000, 0xffffffff, 0x00000000, 0x00000000, 0xffffffff, 0xffffffff,
        0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0x00000000, 0x00000000, 0x00000000
    };
    std::vector<uint32_t> normalframeB = {
        0x000000a3, 0x00200008, 0x00003910, 0x00aa7700, 0x50020000, 0x20200000, 0x05000000, 0xff400000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x01010000, 0x00040c24, 0x00000000, 0xffff00ff, 0x60000000, 0x000000a1, 0x00003900, 0x28300000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x08000002,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
        0x00000000, 0x00000000, 0x00000000, 0xffffffff, 0x00000000, 0x00000000, 0xffffffff, 0xffffffff,
        0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0x00000000, 0x00000000, 0x00000000
    };
    std::vector<uint32_t> normalfieldB = {
        0x000000a3, 0x00200008, 0x00003919, 0x00aa77c0, 0x50020000, 0x20200000, 0x05000000, 0xff400000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x01010000, 0x00040c24, 0x00000000, 0xffff00ff, 0x40000000, 0x000000a1, 0x00003900, 0x28300000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x04000002,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
        0x00000000, 0x00000000, 0x00000000, 0xffffffff, 0x00000000, 0x00000000, 0xffffffff, 0xffffffff,
        0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0x00000000, 0x00000000, 0x00000000
    };
    std::vector<uint32_t> frameIDist = {
        0x00000082, 0x00200008, 0x001e3910, 0x00a83000, 0x90000000, 0x28300000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xff000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000100,
        0x80800000, 0x00000000, 0x00000800, 0xffff00ff, 0x40000000, 0x00000080, 0x00003900, 0x28300000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xffffffff, 0xffffffff, 0xffffffff,
        0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
        0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
    };
};

struct MBEncResBind
{
    MBEncResBind()
    {
        for (int32_t i = 0; i < 16; i++)
        {
            vmeRefList0[i].res = nullptr;
            vmeRefList0[i].surfIndex = 16 + i;
            vmeRefList0[i].numPlane = 1;  // VME only uses Y plane
        }
        for (int32_t i = 0; i < 4; i++)
        {
            vmeRefList1[i].res = nullptr;
            vmeRefList1[i].surfIndex = 33 + i;
            vmeRefList1[i].numPlane = 1;  // VME only uses Y plane
        }
    };

    ~MBEncResBind() {};

    // Resources used by MBEnc (put in the order of binding table)
    // The resource layout in binding table is defined by corresponding kernel HLD
    MediaResourceBind pakObj                   = { nullptr,  0, 1 }; // 0 - PAK object to be given to PAK 
    MediaResourceBind mvData                   = { nullptr,  1, 1 }; // 1 - Indirect MV Data Surface of Current Picture 
    MediaResourceBind brcDistI                 = { nullptr,  2, 1 }; // 2 - Only Used in BRC I DIST Kernel
    MediaResourceBind srcYuv                   = { nullptr,  3, 2 }; // 3&4 - Y/UV planes of current frame
    MediaResourceBind mbSpecificData           = { nullptr,  5, 1 }; // 5 - MB Specific Data Surface (Apple Only)
    MediaResourceBind auxVmeOut                = { nullptr,  6, 1 }; // 6 - Auxiliary VME Output Buffer (Apple Only)
    MediaResourceBind curRefPicSelect          = { nullptr,  7, 1 }; // 7 - RefPicSelect Surface of Current Picture
    MediaResourceBind mvPredictorFromHME       = { nullptr,  8, 1 }; // 8 - MV predictor surface from AVCME
    MediaResourceBind distFromHME              = { nullptr,  9, 1 }; // 9 - Distortion surface from AVCME
    MediaResourceBind sliceMap                 = { nullptr, 10, 1 }; // 10
    MediaResourceBind forwardMbDataForBframe   = { nullptr, 11, 1 }; // 11
    MediaResourceBind forwardMVForBframe       = { nullptr, 12, 1 }; // 12
    MediaResourceBind mbQP                     = { nullptr, 13, 1 }; // 13
    MediaResourceBind mbBrcConst               = { nullptr, 14, 1 }; // 14
    MediaResourceBind vmeInterPredCurPic       = { nullptr, 15, 1 }; // 15 - Current Picture for VMD
    MediaResourceBind vmeRefList0[16]          = {};                 // 16~31 - L0 references
    MediaResourceBind vmeInterPredFwdMR        = { nullptr, 32, 1 }; // 32 - Current Picture
    MediaResourceBind vmeRefList1[4]           = {};                 // 33~36 - L1 references (used in B frame Multiple reference)
    MediaResourceBind mbStats                  = { nullptr, 37, 1 }; // 37
    MediaResourceBind dataMAD                  = { nullptr, 38, 1 }; // 38
    MediaResourceBind brcBufForMbEnc           = { nullptr, 39, 1 }; // 39 - copy BRC updated CURBE surface from BRCFrameUpdate kernel.
    MediaResourceBind forceNonSkipMbMap        = { nullptr, 40, 1 }; // 40
    MediaResourceBind advWA                    = { nullptr, 41, 1 }; // 41 (never used)
    MediaResourceBind staticDetectionCostTable = { nullptr, 42, 1 }; // 42
};

class AvcMBEncPacket : public KernelPacket
{
public:
    AvcMBEncPacket(MediaResource* pSrc, MediaResource* pOutPakObj);
    virtual ~AvcMBEncPacket();

    int32_t createPacket();
    int32_t preparePacket();
    int32_t submitPacket();
    int32_t destroyPacket();

private:
    int32_t initStateHeap();
    int32_t setCurbe();
    int32_t constructCmdSequence(uint32_t threadWidth, uint32_t threadHeight);
    int32_t allocateResource();
    int32_t destroyResource();

private:
    bool isField_ = false;
    const uint32_t mbSize_ = 16;
    uint32_t picWidth_ = 0;
    uint32_t picHeight_ = 0;
    uint32_t picWInMB_ = 0;
    uint32_t picHInMB_ = 0;
    // Note: the DS output surface should be created by HME kernel and passed to this MBEnc kernel
    // APGTODO: the below parameters can be derived from media resource, will remove from MBEnc when HME packet is ready
    uint32_t picDs4xWidth_ = 0;
    uint32_t picDs4xHeight_ = 0;
    uint32_t picDs4xWInMB_ = 0;
    uint32_t picDs4xHInMB_ = 0;
    const uint32_t meDataSizeMultiplier_ = 10;

    uint32_t kernelBlockWidth_ = 0;
    uint32_t kernelBlockHeight_ = 0;

    MBEncResBind resBind_;
    std::vector<MediaResourceBind> sshResBindList_;
    MbEncCurbeDataGen9 curbe_ = {};

    MediaResource* statusReport_ = nullptr;
    MediaResource* timeStamp_ = nullptr;
    MediaResource* cmdBufIdGlobal_ = nullptr;

    static MBEncConstCurbe constCurbe_;
    static std::map<std::string, std::vector<uint32_t>> tables_;
    static const uint32_t modeMvCost_[3][52][8];
    static const uint32_t searchPath_[2][8][16];
    static const uint16_t refCostMultiRefQp_[3][64];
    static std::vector<uint32_t> kernelMbEncPerfIGen9ISA_;
    static std::vector<uint32_t> kernelMbEncPerfPGen9ISA_;
    static std::vector<uint32_t> kernelMbEncPerfBGen9ISA_;
};

}