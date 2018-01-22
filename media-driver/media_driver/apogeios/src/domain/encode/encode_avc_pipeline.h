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
//! \file     encode_avc_pipeline.h
//! \brief    encode avc pipeline class definition
//! \details
//!

#pragma once

#include "media_pipeline.h"
#include "codec_def_encode_avc.h"

namespace Apogeios
{

class ScalingPacket;
class AvcHmePacket;
class AvcBrcPacket;
class AvcMBEncPacket;
class AvcPakPacket;

class ScalingTask : public MediaTask
{
public:
    ScalingTask();
    ScalingTask(MediaResource* srcSurf, MediaResource* surf4x);
    ScalingTask(MediaResource* srcSurf, MediaResource* surf4x, MediaResource* surf16x);
    ScalingTask(MediaResource* srcSurf, MediaResource* surf4x, MediaResource* surf16x, MediaResource* surf32x);
    ~ScalingTask();

    int32_t create();
    int32_t execute();
    int32_t destroy();

private:
    enum ScalingOP
    {
        Scale_None = 0,
        Scale_4X = 1,
        Scale_16X = 2,
        Scale_32X = 3
    };

    ScalingOP op_ = Scale_None;
    MediaResource* scaleSrc_ = nullptr;
    MediaResource* scale4x_ = nullptr;
    MediaResource* scale16x_ = nullptr;
    MediaResource* scale32x_ = nullptr;

    ScalingPacket* phase1Scale4X_ = nullptr;
    ScalingPacket* phase2Scale4X_ = nullptr;
    ScalingPacket* phase3Scale2X_ = nullptr;
};

class AvcEncTask : public MediaTask
{
public:
    AvcEncTask(MediaResource* srcYuv, MediaResource* pakObj);
    ~AvcEncTask();

    int32_t create();
    int32_t execute();
    int32_t destroy();

private:
    MediaResource* srcYuv_ = nullptr;
    MediaResource* pakObj_ = nullptr;

    AvcHmePacket* hme_ = nullptr;
    AvcMBEncPacket* mbenc_ = nullptr;
};

class AvcPakTask : public MediaTask
{
public:
    AvcPakTask(MediaResource* srcYuv, MediaResource* reconYuv, MediaResource* bitstream);
    ~AvcPakTask();

    int32_t create();
    int32_t execute();
    int32_t destroy();

private:
    MediaResource* srcYuv_ = nullptr;
    MediaResource* reconYuv_ = nullptr;
    MediaResource* bitstream_ = nullptr;

    AvcPakPacket* pak_ = nullptr;
};

class EncodeAvcPipe : public MediaPipe
{
public:
    struct PipeParams
    {
        PCODEC_AVC_ENCODE_SEQUENCE_PARAMS pSeq = nullptr;
        PCODEC_AVC_ENCODE_PIC_PARAMS pPic = nullptr;
        PCODEC_AVC_ENCODE_SLICE_PARAMS pSlc = nullptr;
        PCODECHAL_ENCODE_AVC_VUI_PARAMS pVui = nullptr;
    };

public:
    EncodeAvcPipe(PipeParams* pipeParams);
    virtual ~EncodeAvcPipe();

    int32_t createPipe();
    int32_t executePipe();
    int32_t destroyPipe();

    int32_t allocateResources();
    int32_t destroyResources();

private:
    ScalingTask* scaling_ = nullptr;
    AvcEncTask* avcEnc_ = nullptr;
    AvcPakTask* avcPak_ = nullptr;

    MediaResource* srcYuv_ = nullptr;
    MediaResource* srcYuv4x_ = nullptr;
    MediaResource* srcYuv16x_ = nullptr;
    MediaResource* srcYuv32x_ = nullptr;
    MediaResource* reconYuv_ = nullptr;
    MediaResource* pakObj_ = nullptr;
    MediaResource* bitstream_ = nullptr;

    PipeParams* pipeParams_ = nullptr;
    uint32_t frameWidth_ = 0;
    uint32_t frameHeight_ = 0;
};

}