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
//! \file     gpu_cmd_base.cpp
//! \brief    base class of general gpu commands
//! \details
//!

#include "gpu_cmd_base.h"

std::ofstream GpuCmdBase::logFile_("gpu_cmd_log.txt");

void GpuCmdBase::logCmdName(const char* name)
{
    logFile_ << std::endl << "################  " << name << "  ################" << std::endl;
}

void GpuCmdBase::logCmdHexDW(uint32_t val, char* name)
{
    logFile_ << std::endl << name << " : 0x" << std::right << std::setw(8) << std::setfill('0') << val << std::endl;
}

void GpuCmdBase::logCmdHexQW(uint64_t val, char* name)
{
    logFile_ << std::endl << std::left << std::setw(68) << std::setfill(' ') << name << " : 0x" << std::hex << std::setw(16) << std::setfill('0') << val << std::endl;
}

void GpuCmdBase::logCmdHexQW2(uint64_t val, char* name)
{
    logFile_ << std::endl << std::left << std::setw(120) << std::setfill(' ') << name << " : 0x" << std::hex << std::setw(16) << std::setfill('0') << val << std::endl;
}

void GpuCmdBase::logCmdSubVal(uint32_t val, char* name)
{
    logFile_ << "    " << std::left << std::setw(64) << std::setfill(' ') << name << " = 0x" << std::hex << val << std::endl;
}

void GpuCmdBase::logCmdSubVal(uint64_t val, char* name)
{
    logFile_ << "    " << std::left << std::setw(64) << std::setfill(' ') << name << " = 0x" << std::hex << val << std::endl;
}

void GpuCmdBase::logCmdFlush()
{
    logFile_.flush();
}