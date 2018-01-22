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
//! \file     media_resource.cpp
//! \brief    media resource class implementation
//! \details
//!

#include "media_resource.h"

namespace Apogeios
{

MediaResource::MediaResource(RESOURCE_TYPE type, RES_FORMAT format, uint32_t size, bool bExecute, std::string name)
{
    resType_ = type;
    format_ = format;
    tileType_ = TILE_TYPE_LINEAR;
    width_ = size;
    height_ = 1;
    bExecute_ = bExecute;
    name_ = name;
}

MediaResource::MediaResource(RESOURCE_TYPE type, RES_FORMAT format, TILE_TYPE tile, uint32_t width, uint32_t height, std::string name)
{
    resType_ = type;
    format_ = format;
    tileType_ = tile;
    width_ = width;
    height_ = height;
    bExecute_ = false;
    name_ = name;
}

int32_t MediaResource::create()
{
    return 0;
}

int32_t MediaResource::destroy()
{
    return 0;
}

int32_t MediaResource::mapVA()
{
    return 0;
}

int32_t MediaResource::freeVA()
{
    return 0;
}

int32_t MediaResource::updateVA()
{
    return 0;
}

int32_t MediaResource::makeResident()
{
    return 0;
}

int32_t MediaResource::evict()
{
    return 0;
}

void* MediaResource::lock()
{
    return nullptr;
}

int32_t MediaResource::unlock()
{
    return 0;
}

int32_t MediaResource::readData(uint8_t* pDst, int32_t size, int32_t offset /*= 0*/)
{
    return 0;
}

int32_t MediaResource::writeData(uint8_t* pSrc, int32_t size, int32_t offset /*= 0*/)
{
    return 0;
}

int32_t MediaResource::readFromFile()
{
    return 0;
}

int32_t MediaResource::dumpToFile()
{
    return 0;
}

int32_t MediaResource::initGmmParam()
{
    return 0;
}

}