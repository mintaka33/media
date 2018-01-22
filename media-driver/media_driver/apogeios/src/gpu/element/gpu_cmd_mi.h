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
//! \file     gpu_cmd_mi.h
//! \brief    header file of gpu command class for MI interface
//! \details
//!

#pragma once

#include "gpu_cmd_base.h"
#include "mhw_mi_hwcmd_g9_X.h"
#include "media_resource.h"

namespace Apogeios
{

enum FLUSH_MODE
{
    FLUSH_NONE = 0,              // No flush
    FLUSH_WRITE_CACHE,           // Flush write cache
    FLUSH_READ_CACHE,            // Flush read cache
    FLUSH_CUSTOM                 // Flush with custom parameters
};

enum POST_SYNC_OPERATION
{
    PSO_NOWRITE = 0,
    PSO_WRITE_IMMEDIATE_DATA = 1,
    PSO_WRITE_TIMESTAMP_REG = 3
};

// Preemption control
#define CS_CHICKEN1_PREEMPTION_CONTROL_OFFSET       0x2580
#define CS_CHICKEN1_MID_BATCH_PREEMPT_VALUE         0x00060004

// L3 cache control
#define L3_CACHE_CNTL_REGISTER_OFFSET               0x7034
#define L3_CACHE_CNTL_REGISTER_VALUE_DEFAULT        0x60000121

template<typename CmdType>
class GpuCmdMI : public GpuCmdBase
{
public:
    GpuCmdMI()
    {
        pCmd_ = new CmdType;
        cmdSize_ = sizeof(CmdType);
    }

    virtual ~GpuCmdMI()
    {
        if (pCmd_)
        {
            delete pCmd_;
            pCmd_ = nullptr;
            cmdSize_ = 0;
        }
    };

    uint32_t getOpCode()
    {
        return pCmd_->DW0.Value;
    }

    uint32_t getCmdSize()
    {
        return pCmd_->byteSize;
    }

    uint8_t* getCmdData()
    {
        return (uint8_t*)pCmd_;
    }

    int loadCmd(char* pData, int size)
    {
        if (pData == nullptr || size != cmdSize_)
        {
            return -1;
        }

        memcpy(pCmd_, pData, size);

        return 0;
    }

    virtual bool validate() = 0;
    virtual void parse() = 0;

protected:
    CmdType *pCmd_;
    int cmdSize_;
};

class CmdPipeControl : public GpuCmdMI<mhw_mi_g9_X::PIPE_CONTROL_CMD>
{
public:
    CmdPipeControl(FLUSH_MODE mode);
    CmdPipeControl(FLUSH_MODE mode, POST_SYNC_OPERATION op);
    CmdPipeControl(FLUSH_MODE mode, POST_SYNC_OPERATION op, MediaResource* pRes, uint32_t offset, uint64_t data);
    virtual ~CmdPipeControl() {};

    bool validate();
    void parse();

private:
    void initCmd(FLUSH_MODE mode);

private:
    FLUSH_MODE flushMode_;
    POST_SYNC_OPERATION postSyncOp_;
    MediaResource* pResource_ = nullptr;
    uint32_t offset_ = 0;
    uint64_t value_ = 0;
};

class CmdMediaStateFlush : public GpuCmdMI<mhw_mi_g9_X::MEDIA_STATE_FLUSH_CMD>
{
public:
    CmdMediaStateFlush(bool bFlushToGo, uint8_t interfaceDescriptorOffset);
    virtual ~CmdMediaStateFlush() {};

    bool validate();
    void parse();

private:
    bool                bFlushToGo_ = false;
    uint8_t             ui8InterfaceDescriptorOffset_ = 0;
};

class CmdMiFlushDW : public GpuCmdMI<mhw_mi_g9_X::MI_FLUSH_DW_CMD>
{
public:
    CmdMiFlushDW(bool bInvalidCache);
    virtual ~CmdMiFlushDW() {};

    bool validate();
    void parse();
};

class CmdMiStoreRegisterMem : public GpuCmdMI<mhw_mi_g9_X::MI_STORE_REGISTER_MEM_CMD>
{
public:
    CmdMiStoreRegisterMem(MediaResource* res, uint32_t resOffset, uint32_t registerAddr);
    virtual ~CmdMiStoreRegisterMem() {};

    bool validate();
    void parse();
};

class CmdMiStoreDataImm : public GpuCmdMI<mhw_mi_g9_X::MI_STORE_DATA_IMM_CMD>
{
public:
    CmdMiStoreDataImm() : GpuCmdMI() {};
    virtual ~CmdMiStoreDataImm() {};

    bool validate();
    void parse();
};

class CmdMiLoadRegisterImm : public GpuCmdMI<mhw_mi_g9_X::MI_LOAD_REGISTER_IMM_CMD>
{
public:
    CmdMiLoadRegisterImm(uint32_t offset, uint32_t data);
    virtual ~CmdMiLoadRegisterImm() {};

    bool validate();
    void parse();

private:
    uint32_t offset_ = 0;
    uint32_t data_ = 0;
};

class CmdMiBatchBufferEnd : public GpuCmdMI<mhw_mi_g9_X::MI_BATCH_BUFFER_END_CMD>
{
public:
    CmdMiBatchBufferEnd() : GpuCmdMI() {};
    virtual ~CmdMiBatchBufferEnd() {};

    bool validate();
    void parse();
};

}