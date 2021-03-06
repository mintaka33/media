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
//! \file     resource_cb_linux.cpp
//! \brief    media resource callback implementation for linux
//! \details
//!

#include "resource_cb_linux.h"

namespace Apogeios
{

ResourceCbImpl *ResourceCbImpl::pInstance_ = new ResourceCbImpl();

ResourceCbImpl *ResourceCbImpl::getInstance()
{
    return pInstance_;
}

int32_t ResourceCbImpl::AllocateCb(void **bo, uint32_t size, uint32_t alignment, int8_t *name)
{
    *bo = bufmgr_->bo_alloc(bufmgr_, (const char *)name, size, alignment);
    return (*bo == nullptr) ? -1 : 0;
}

int32_t ResourceCbImpl::DeallocateCb(void *bo)
{
    if (bo == nullptr)
        return 0;

    bufmgr_->bo_unreference((mos_linux_bo *)bo);

    return 0;
}

int32_t ResourceCbImpl::Deallocate2Cb(const void *pDealloc)
{
    return 0;
}

int32_t ResourceCbImpl::LockCb(void *pLockParams)
{
    LockArg *pArg = (LockArg *)pLockParams;
    pArg->ptr = bufmgr_->bo_map(pArg->bo, true);

    return 0;
}

int32_t ResourceCbImpl::Lock2Cb(void *pLock2Params)
{
    return 0;
}

int32_t ResourceCbImpl::UnlockCb(const void *pUnlockParams)
{
    UnlockArg *pArg = (UnlockArg *)pUnlockParams;
    return bufmgr_->bo_unmap(pArg->bo);
}

int32_t ResourceCbImpl::Unlock2Cb(const void *pUnlock2Params)
{
    return 0;
}

int32_t ResourceCbImpl::MapGpuVirtualAddressCb(void *pParams)
{
    return 0;
}

int32_t ResourceCbImpl::FreeGpuVirtualAddressCb(const void *pParams)
{
    return 0;
}

int32_t ResourceCbImpl::UpdateGpuVirtualAddressCb(const void *pParams)
{
    return 0;
}

int32_t ResourceCbImpl::MakeResidentCb(void *pParams)
{
    return 0;
}

int32_t ResourceCbImpl::EvictCb(void *pParams)
{
    return 0;
}
}