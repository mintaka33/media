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
//! \file     resource_cb.h
//! \brief    media resource callback class definition
//! \details
//!

#pragma once

#include <stdint.h>

class IResourceCb
{
public:
    IResourceCb();
    virtual ~IResourceCb();

    virtual int32_t AllocateCb(void* pAllocateParams) = 0;
    virtual int32_t DeallocateCb(const void* pDeallocate) = 0;
    virtual int32_t Deallocate2Cb(const void* pDealloc) = 0;
    virtual int32_t LockCb(void* pLockParams) = 0;
    virtual int32_t Lock2Cb(void* pLock2Params) = 0;
    virtual int32_t UnlockCb(const void* pUnlockParams) = 0;
    virtual int32_t Unlock2Cb(const void* pUnlock2Params) = 0;
    virtual int32_t MapGpuVirtualAddressCb(void* pParams) = 0;
    virtual int32_t FreeGpuVirtualAddressCb(const void* pParams) = 0;
    virtual int32_t UpdateGpuVirtualAddressCb(const void* pParams) = 0;
    virtual int32_t MakeResidentCb(void* pParams) = 0;
    virtual int32_t EvictCb(void* pParams) = 0;

private:
};