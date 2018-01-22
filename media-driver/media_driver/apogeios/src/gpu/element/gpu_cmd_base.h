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
//! \file     gpu_cmd_base.h
//! \brief    header of gpu base class
//! \details
//!

#pragma once

#include <fstream>
#include <iostream>
#include <iomanip>
#include <vector>

namespace Apogeios
{

class MediaResource;

#define LOG_CMD_NAME(val)    logCmdName(val)
#define LOG_CMD_HEX_DW(val)  logCmdHexDW(val, #val);
#define LOG_CMD_HEX_QW(val)  logCmdHexQW(val, #val);
#define LOG_CMD_HEX_QW2(val) logCmdHexQW2(val, #val);
#define LOG_CMD_SUB_VAL(val) logCmdSubVal(val, #val);

class GpuCmdBase
{
public:
    GpuCmdBase() {};
    virtual ~GpuCmdBase() { resArrayInCmd_.clear(); };
    virtual uint32_t getOpCode() = 0;
    virtual uint32_t getCmdSize() = 0;
    virtual uint8_t* getCmdData() = 0;
    virtual int loadCmd(char* pData, int size) = 0;
    virtual bool validate() = 0;
    virtual void parse() = 0;

    void addResource(MediaResource* res) { resArrayInCmd_.push_back(res); }
    std::vector<MediaResource*>& getResArray() { return resArrayInCmd_; }
    uint32_t getResNumber() { return (uint32_t)resArrayInCmd_.size(); }

    void dumpCmdBinary(uint32_t* pData, uint32_t size)
    {
        for (uint32_t i = 0; i < size; i++)
        {
            uint32_t value = *(pData + i);

            if (i % 8 == 0)
                logFile_ << std::endl;

            logFile_ << "0x" << std::hex << std::right << std::setw(8) << std::setfill('0') << value << "  ";
        }
        logFile_ << std::endl;
    }

    void logCmdName(const char* name);
    void logCmdHexDW(uint32_t val, char* name);
    void logCmdHexQW(uint64_t val, char* name);
    void logCmdHexQW2(uint64_t val, char* name);
    void logCmdSubVal(uint32_t val, char* name);
    void logCmdSubVal(uint64_t val, char* name);
    void logCmdFlush();

private:
    static std::ofstream logFile_;
    std::vector<MediaResource*> resArrayInCmd_;
};

}