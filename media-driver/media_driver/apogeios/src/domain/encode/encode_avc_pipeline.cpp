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
//! \file     encode_avc_pipeline.cpp
//! \brief    encode avc pipeline class implementation
//! \details
//!

#include "encode_avc_pipeline.h"
#include "down_scaling.h"
#include "avc_brc.h"
#include "avc_hme.h"
#include "avc_mbenc.h"
#include "avc_pak.h"

namespace Apogeios
{

ScalingTask::ScalingTask()
{
}

ScalingTask::ScalingTask(MediaResource* srcSurf, MediaResource* surf4x) :
    scaleSrc_(srcSurf),
    scale4x_(surf4x)
{
    op_ = Scale_4X;
}

ScalingTask::ScalingTask(MediaResource* srcSurf, MediaResource* surf4x, MediaResource* surf16x) :
    scaleSrc_(srcSurf),
    scale4x_(surf4x),
    scale16x_(surf16x)
{
    op_ = Scale_16X;
}

ScalingTask::ScalingTask(MediaResource* srcSurf, MediaResource* surf4x, MediaResource* surf16x, MediaResource* surf32x) :
    scaleSrc_(srcSurf),
    scale4x_(surf4x),
    scale16x_(surf16x),
    scale32x_(surf32x)
{
    op_ = Scale_32X;
}

ScalingTask::~ScalingTask()
{
}

int32_t ScalingTask::create()
{
    if (op_ >= Scale_4X)
    {
        if (scaleSrc_ == nullptr || scale4x_ == nullptr)
            return -1;

        phase1Scale4X_ = new ScalingPacket(scaleSrc_, scale4x_);
        if (phase1Scale4X_->createPacket() != 0)
            return -1;
    }

    if (op_ >= Scale_16X)
    {
        if (scale4x_ == nullptr || scale16x_ == nullptr)
            return -1;

        phase2Scale4X_ = new ScalingPacket(scale4x_, scale16x_);
        if (phase2Scale4X_->createPacket() != 0)
            return -1;
    }

    if (op_ >= Scale_32X)
    {
        if (scale16x_ == nullptr || scale32x_ == nullptr)
            return -1;

        phase3Scale2X_ = new ScalingPacket(scale16x_, scale32x_);
        if (phase3Scale2X_->createPacket() != 0)
            return -1;
    }

    return 0;
}

int32_t ScalingTask::execute()
{
    int32_t result = 0;

    if (op_ >= Scale_4X)
    {
        if (phase1Scale4X_ == nullptr)
            return -1;

        if (phase1Scale4X_->preparePacket() != 0)
            return -1;

        addPacket(phase1Scale4X_);
    }

    if (op_ >= Scale_16X)
    {
        if (phase2Scale4X_ == nullptr)
            return -1;

        if (phase2Scale4X_->preparePacket() != 0)
            return -1;

        addPacket(phase2Scale4X_);
    }

    if (op_ >= Scale_32X)
    {
        if (phase3Scale2X_ == nullptr)
            return -1;

        if (phase3Scale2X_->preparePacket() != 0)
            return -1;

        addPacket(phase3Scale2X_);
    }

    result = submitTask(false);

    return result;
}

int32_t ScalingTask::destroy()
{
    int32_t result = 0;

    if (phase1Scale4X_)
    {
        if (phase1Scale4X_->destroyPacket() != 0)
            return -1;
        delete phase1Scale4X_;
    }

    if (phase2Scale4X_)
    {
        if (phase2Scale4X_->destroyPacket() != 0)
            return -1;
        delete phase2Scale4X_;
    }

    if (phase3Scale2X_)
    {
        if (phase3Scale2X_->destroyPacket() != 0)
            return -1;
        delete phase3Scale2X_;
    }

    result = destroyTask();

    return result;
}

AvcEncTask::AvcEncTask(MediaResource* srcYuv, MediaResource* pakObj) :
    srcYuv_(srcYuv),
    pakObj_(pakObj)
{
}

AvcEncTask::~AvcEncTask()
{
}

int32_t AvcEncTask::create()
{
    if (srcYuv_ == nullptr || pakObj_ == nullptr)
        return -1;

    mbenc_ = new AvcMBEncPacket(srcYuv_, pakObj_);
    if (mbenc_->createPacket() != 0)
        return -1;

    return 0;
}

int32_t AvcEncTask::execute()
{
    // add AVC HME packet into Enc task
    if (hme_)
    {
        if (hme_->preparePacket() != 0)
            return -1;

        addPacket(hme_);
    }

    // add AVC MBEnc packet into Enc task
    if (mbenc_)
    {
        if (mbenc_->preparePacket() != 0)
            return -1;

        addPacket(mbenc_);
    }

    if (submitTask(false) != 0)
        return -1;

    return 0;
}

int32_t AvcEncTask::destroy()
{
    if (hme_)
    {
        if (hme_->destroyPacket() != 0)
            return -1;
        delete hme_;
    }

    if (mbenc_)
    {
        if (mbenc_->destroyPacket() != 0)
            return -1;
        delete mbenc_;
    }

    if (destroyTask() != 0)
        return -1;

    return 0;
}

AvcPakTask::AvcPakTask(MediaResource* srcYuv, MediaResource* reconYuv, MediaResource* bitstream) :
    srcYuv_(srcYuv),
    reconYuv_(reconYuv),
    bitstream_(bitstream)
{
}

AvcPakTask::~AvcPakTask()
{
}

int32_t AvcPakTask::create()
{
    if (srcYuv_ == nullptr || reconYuv_ == nullptr || bitstream_ == nullptr)
        return -1;

    pak_ = new AvcPakPacket(srcYuv_, reconYuv_, bitstream_);

    if (pak_->createPacket() != 0)
        return -1;

    return 0;
}

int32_t AvcPakTask::execute()
{
    // add AVC PAK packet into PAK task
    if (pak_)
    {
        if (pak_->preparePacket() != 0)
            return -1;

        addPacket(pak_);
    }

    if (submitTask(false) != 0)
        return -1;

    return 0;
}

int32_t AvcPakTask::destroy()
{
    if (pak_)
    {
        if (pak_->destroyPacket() != 0)
            return -1;
        delete pak_;
    }

    if (destroyTask() != 0)
        return -1;

    return 0;
}

EncodeAvcPipe::EncodeAvcPipe(PipeParams* pipeParams) :
    pipeParams_(pipeParams)
{
    frameWidth_ = pipeParams_->pSeq->FrameWidth;
    frameHeight_ = pipeParams_->pSeq->FrameHeight;
}

EncodeAvcPipe::~EncodeAvcPipe()
{
}

int32_t EncodeAvcPipe::createPipe()
{
    if (allocateResources() != 0)
    {
        return -1;
    }

    scaling_ = new ScalingTask(srcYuv_, srcYuv4x_, srcYuv16x_);
    if (scaling_->create() != 0)
    {
        return -1;
    }

    avcEnc_ = new AvcEncTask(srcYuv_, pakObj_);
    if (avcEnc_->create() != 0)
    {
        return -1;
    }

    avcPak_ = new AvcPakTask(srcYuv_, reconYuv_, bitstream_);
    if (avcPak_->create() != 0)
    {
        return -1;
    }

    return 0;
}

int32_t EncodeAvcPipe::executePipe()
{
    if (resetPipe() != 0)
        return -1;

    addTask(scaling_);

    addTask(avcEnc_);

    addTask(avcPak_);

    if (flushPipe() != 0)
        return -1;

    return 0;
}

int32_t EncodeAvcPipe::destroyPipe()
{
    if (resetPipe() != 0)
        return -1;

    if (scaling_)
    {
        if (scaling_->destroy() != 0)
            return -1;
        delete scaling_;
    }

    if (avcEnc_)
    {
        if (avcEnc_->destroy() != 0)
            return -1;
        delete avcEnc_;
    }

    if (avcPak_)
    {
        if (avcPak_->destroy() != 0)
            return -1;
        delete avcPak_;
    }

    if (destroyResources() != 0)
    {
        return -1;
    }

    return 0;
}

int32_t EncodeAvcPipe::allocateResources()
{
    // allocate input yuv surface
    srcYuv_ = new MediaResource(RES_TYPE_2D, RES_FORMAT_NV12, TILE_TYPE_Y, frameWidth_, frameHeight_, "SrcYuv");
    if (srcYuv_->create() != 0)
        return -1;

    // initialize src surface with with fixed values
    int32_t srcSize = srcYuv_->getSize();
    std::vector<uint8_t> fillData(srcSize, 0x12);
    srcYuv_->writeData(&fillData[0], srcSize, 0);

    // Scaling 4x surface
    srcYuv4x_ = new MediaResource(RES_TYPE_2D, RES_FORMAT_NV12, TILE_TYPE_Y, frameWidth_ / 4, frameHeight_ / 4, "SrcYuv4X");
    if (srcYuv4x_->create() != 0)
        return -1;

    // Scaling 16x surface
    srcYuv16x_ = new MediaResource(RES_TYPE_2D, RES_FORMAT_NV12, TILE_TYPE_Y, frameWidth_ / 16, frameHeight_ / 16, "SrcYuv16X");
    if (srcYuv16x_->create() != 0)
        return -1;

    // allocate reconstruction yuv surface
    reconYuv_ = new MediaResource(RES_TYPE_2D, RES_FORMAT_NV12, TILE_TYPE_Y, frameWidth_, frameHeight_, "ReconYuv");
    if (reconYuv_->create() != 0)
        return -1;

    // allocate output PAK Object buffer
    uint32_t allocSize = (frameWidth_ / 16) * (frameHeight_ / 16) * (16 * sizeof(uint32_t));  // MFC_AVC_PAK_OBJECT command is zero-padded to 16 DWORDs
    pakObj_ = new MediaResource(RES_TYPE_BUFFER, RES_FORMAT_Buffer, allocSize, false, "PakObject");
    if (pakObj_->create() != 0)
        return -1;

    // allocate encode output bitstream buffer
    uint32_t bsSize = 64 * 1024;
    bitstream_ = new MediaResource(RES_TYPE_BUFFER, RES_FORMAT_Buffer, bsSize, false, "Bitstream");
    if (bitstream_->create() != 0)
        return -1;

    return 0;
}

int32_t EncodeAvcPipe::destroyResources()
{
    if (srcYuv_)
    {
        if (srcYuv_->destroy() != 0)
            return -1;
        delete srcYuv_;
    }

    if (srcYuv4x_)
    {
        if (srcYuv4x_->destroy() != 0)
            return -1;
        delete srcYuv4x_;
    }

    if (srcYuv16x_)
    {
        if (srcYuv16x_->destroy() != 0)
            return -1;
        delete srcYuv16x_;
    }

    if (srcYuv32x_)
    {
        if (srcYuv32x_->destroy() != 0)
            return -1;
        delete srcYuv32x_;
    }

    if (reconYuv_)
    {
        if (reconYuv_->destroy() != 0)
            return -1;
        delete reconYuv_;
    }

    if (pakObj_)
    {
        if (pakObj_->destroy() != 0)
            return -1;
        delete pakObj_;
    }

    if (bitstream_)
    {
        if (bitstream_->destroy() != 0)
            return -1;
        delete bitstream_;
    }

    return 0;
}

}