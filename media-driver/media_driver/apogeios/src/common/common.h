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
//! \file     common.h
//! \brief    common definitions, declares, include
//! \details
//!

#pragma once

#include <stdint.h>

#define UTIL_ALIGN(a, alignment) (((a) + (alignment) - 1) / (alignment) ) * (alignment)
#define UTIL_BITFIELD_RANGE(_startbit, _endbit) ((_endbit) - (_startbit) + 1)
#define UTIL_BITFIELD_BIT(_bit) 1
#define UTIL_MIN(_a, _b) (((_a) < (_b)) ? (_a) : (_b))

void setGpuAddress(uint32_t* lowPart, uint32_t* highPart, uint64_t baseAddr, uint32_t offset, uint32_t lsb);
