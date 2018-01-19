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
//! \file     media_resource.h
//! \brief    media resource class definition
//! \details
//!

#pragma once

#include <stdint.h>
#include <string>

#include "resource_base.h"
#include "resource_cb.h"

#include "GmmLib.h"
#include "media_fourcc.h"

class CmdPacket;

inline GMM_RESOURCE_FORMAT ResFmt2GmmFmt(RES_FORMAT format)
{
    switch (format)
    {
    case RES_FORMAT_A8R8G8B8: return GMM_FORMAT_B8G8R8A8_UNORM_TYPE;
    case RES_FORMAT_X8R8G8B8: return GMM_FORMAT_B8G8R8X8_UNORM_TYPE;
    case RES_FORMAT_A8B8G8R8: return GMM_FORMAT_R8G8B8A8_UNORM_TYPE;
    case RES_FORMAT_R5G6B5: return GMM_FORMAT_B5G6R5_UNORM_TYPE;
    case RES_FORMAT_R32U: return GMM_FORMAT_R32_UINT_TYPE;
    case RES_FORMAT_R32F: return GMM_FORMAT_R32_FLOAT_TYPE;
    case RES_FORMAT_YUY2: return GMM_FORMAT_YUY2_2x1;
    case RES_FORMAT_P8: return GMM_FORMAT_RENDER_8BIT_TYPE;
    case RES_FORMAT_A8P8: return GMM_FORMAT_R16_UNORM_TYPE;
    case RES_FORMAT_A8: return GMM_FORMAT_A8_UNORM_TYPE;
    case RES_FORMAT_L8: return GMM_FORMAT_GENERIC_8BIT;
    case RES_FORMAT_R16UN: return GMM_FORMAT_R16_UNORM_TYPE;
    case RES_FORMAT_L16: return GMM_FORMAT_L16_UNORM_TYPE;
    case RES_FORMAT_D16: return GMM_FORMAT_GENERIC_16BIT_TYPE;
    case RES_FORMAT_A4L4: return GMM_FORMAT_GENERIC_8BIT;
    case RES_FORMAT_A8L8: return GMM_FORMAT_GENERIC_16BIT;
    case RES_FORMAT_YVYU: return GMM_FORMAT_YVYU;
    case RES_FORMAT_UYVY: return GMM_FORMAT_UYVY;
    case RES_FORMAT_VYUY: return GMM_FORMAT_VYUY_2x1;
    case RES_FORMAT_AYUV: return GMM_FORMAT_R8G8B8A8_UINT_TYPE;
    case RES_FORMAT_P016: return GMM_FORMAT_P016_TYPE;
    case RES_FORMAT_P010: return GMM_FORMAT_P010_TYPE;
    case RES_FORMAT_NV12: return GMM_FORMAT_NV12_TYPE;
    case RES_FORMAT_NV11: return GMM_FORMAT_NV11_TYPE;
    case RES_FORMAT_P208: return GMM_FORMAT_P208_TYPE;
    case RES_FORMAT_IMC1: return GMM_FORMAT_IMC1_TYPE;
    case RES_FORMAT_IMC2: return GMM_FORMAT_IMC2_TYPE;
    case RES_FORMAT_IMC3: return GMM_FORMAT_IMC3_TYPE;
    case RES_FORMAT_IMC4: return GMM_FORMAT_IMC4_TYPE;
    case RES_FORMAT_I420: return GMM_FORMAT_I420_TYPE;
    case RES_FORMAT_IYUV: return GMM_FORMAT_IYUV_TYPE;
    case RES_FORMAT_YV12: return GMM_FORMAT_YV12_TYPE;
    case RES_FORMAT_YVU9: return GMM_FORMAT_YVU9_TYPE;
    case RES_FORMAT_AI44: return GMM_FORMAT_GENERIC_8BIT;
    case RES_FORMAT_IA44: return GMM_FORMAT_GENERIC_8BIT;
    case RES_FORMAT_400P: return GMM_FORMAT_IMC3_TYPE;
    case RES_FORMAT_411P: return GMM_FORMAT_MFX_JPEG_YUV411_TYPE;
    case RES_FORMAT_411R: return GMM_FORMAT_MFX_JPEG_YUV411R_TYPE;
    case RES_FORMAT_422H: return GMM_FORMAT_MFX_JPEG_YUV422H_TYPE;
    case RES_FORMAT_422V: return GMM_FORMAT_MFX_JPEG_YUV422V_TYPE;
    case RES_FORMAT_444P: return GMM_FORMAT_MFX_JPEG_YUV444_TYPE;
    case RES_FORMAT_RGBP: return GMM_FORMAT_RGBP_TYPE;
    case RES_FORMAT_BGRP: return GMM_FORMAT_BGRP_TYPE;
    case RES_FORMAT_STMM: return GMM_FORMAT_R8_UNORM_TYPE;
    case RES_FORMAT_R32S: return GMM_FORMAT_R32_SINT_TYPE;
    case RES_FORMAT_R8U: return GMM_FORMAT_R8_UINT_TYPE;
    case RES_FORMAT_R8G8UN: return GMM_FORMAT_R8G8_UNORM_TYPE;
    case RES_FORMAT_R8G8SN: return GMM_FORMAT_R8G8_SNORM_TYPE;
    case RES_FORMAT_R16U: return GMM_FORMAT_R16_UINT_TYPE;
    case RES_FORMAT_Buffer_2D: return GMM_FORMAT_GENERIC_8BIT;
    case RES_FORMAT_R16S: return GMM_FORMAT_R16_SINT_TYPE;
    case RES_FORMAT_V8U8: return GMM_FORMAT_R8G8_SNORM_TYPE;
    case RES_FORMAT_Y8: return GMM_FORMAT_MEDIA_Y8_UNORM;
    case RES_FORMAT_Y1: return GMM_FORMAT_MEDIA_Y1_UNORM;
    case RES_FORMAT_Y16U: return GMM_FORMAT_MEDIA_Y16_UNORM;
    case RES_FORMAT_Y16S: return GMM_FORMAT_MEDIA_Y16_SNORM;
    case RES_FORMAT_R10G10B10A2: return GMM_FORMAT_R10G10B10A2_UNORM;
    case RES_FORMAT_A16B16G16R16: return GMM_FORMAT_R16G16B16A16_UNORM;
    case RES_FORMAT_A16B16G16R16F: return GMM_FORMAT_R16G16B16A16_FLOAT;
    case RES_FORMAT_R16G16UN: return GMM_FORMAT_R16G16_UNORM_TYPE;
    case RES_FORMAT_R16F: return GMM_FORMAT_R16_FLOAT;
    //case RES_FORMAT_Y210: return GMM_FORMAT_Y210_TYPE;
    case RES_FORMAT_Y216: return GMM_FORMAT_Y216_TYPE;
    //case RES_FORMAT_Y410: return GMM_FORMAT_Y410_TYPE;
    case RES_FORMAT_Y416: return GMM_FORMAT_Y416_TYPE;
    default: return GMM_FORMAT_INVALID;
    }
}

class MediaResource : public IResource
{
public:
    MediaResource() {};
    ~MediaResource() {};

    MediaResource(RESOURCE_TYPE type, RES_FORMAT format, uint32_t size, bool bExecute = false, std::string name = "Unknown_Buffer");
    MediaResource(RESOURCE_TYPE type, RES_FORMAT format, TILE_TYPE tile, uint32_t width, uint32_t height, std::string name = "Unknown_2D");

    int32_t create();
    int32_t destroy();
    void*   lock();
    int32_t unlock();
    int32_t mapVA();
    int32_t freeVA();
    int32_t updateVA();
    int32_t makeResident();
    int32_t evict();

    int32_t readData(uint8_t* pDst, int32_t size, int32_t offset = 0);
    int32_t writeData(uint8_t* pSrc, int32_t size, int32_t offset = 0);
    int32_t readFromFile();
    int32_t dumpToFile();

    RES_FORMAT getFormat() { return format_; }
    TILE_TYPE getTileType() { return tileType_; }
    int32_t getWidth() { return width_; }
    int32_t getPitch() { return pitch_; }
    int32_t getHeight() { return height_; }
    int32_t getDepth() { return depth_; }
    int32_t getSize() { return resSize_; }
    int32_t getUOffsetX() { return offsetUX_; }
    int32_t getUOffsetY() { return offsetUY_; }
    int32_t getVOffsetX() { return offsetVX_; }
    int32_t getVOffsetY() { return offsetVY_; }
    uint64_t getFence() { return fenceOut_; }

    uint64_t getGpuAddress()
    {
        uint64_t gfxAddress = 0;
        return gfxAddress;
    };

    uint64_t getCannoizedGpuAddress()
    {
        uint64_t gfxAddress = 0;
        // HW requires cannoize GPU address
        return GMM_GFX_ADDRESS_CANONIZE(gfxAddress);
    };

    void* getCpuAddress()
    {
        void* addr = lock();
        return addr;
    };

    static void setCallbackInstance(IResourceCb* pCallback) { pCallback_ = pCallback; };

private:
    int32_t initGmmParam();

private:
    std::string name_ = "Unknown";
    RESOURCE_TYPE resType_ = RES_TYPE_INVALID;
    RES_FORMAT format_ = RES_FORMAT_INVALID;
    TILE_TYPE tileType_ = TILE_TYPE_INVALID;
    uint32_t width_ = 0;
    uint32_t pitch_ = 0;
    uint32_t height_ = 0;
    uint32_t depth_ = 0;
    uint32_t resSize_ = 0;
    int32_t offsetUX_ = 0;
    int32_t offsetUY_ = 0;
    int32_t offsetVX_ = 0;
    int32_t offsetVY_ = 0;
    bool bExecute_ = false;
    bool isDestroied_ = false; // avoid destroy same resource multiple time

    CmdPacket* lastReadPacket_ = nullptr;
    uint64_t lastReadFence_ = 0;
    CmdPacket* lastWritePacket_ = nullptr;
    uint64_t lastWriteFence_ = 0;

    GMM_RESCREATE_PARAMS gmmParams_ = {};
    GMM_RESOURCE_INFO* gmmResInfo_ = nullptr;
    uint32_t numAllocations_ = 0;
    uint64_t fenceOut_ = 0;

    static IResourceCb* pCallback_;
};

struct MediaResourceBind
{
    MediaResource* res;
    uint32_t surfIndex; // surface index in Curbe
    uint32_t numPlane;  // surface plane number
};
