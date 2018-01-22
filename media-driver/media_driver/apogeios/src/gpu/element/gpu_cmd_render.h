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
//! \file     gpu_cmd_render.h
//! \brief    header of gpu Render commands class
//! \details
//!

#pragma once

#include <vector>

#include "media_resource.h"
#include "gpu_cmd_base.h"
#include "mhw_render.h"
#include "mhw_render_hwcmd_g9_X.h"

#define     MAX_CURBE_DATA_SIZE                 0x01FFFF
#define     MAX_INTERFACE_DESCRIPTOR_DATA_SIZE  2048

namespace Apogeios
{

enum PIPELINE_SELECTION
{
    PIPELINE_SELECTION_3D = 0,    //!< 3D pipeline is selected
    PIPELINE_SELECTION_MEDIA = 1,    //!< Media pipeline is selected (Includes HD optical disc playback, HD video playback, and generic media workloads)
    PIPELINE_SELECTION_GPGPU = 2,    //!< GPGPU pipeline is selected
};

template<typename CmdType>
class GpuCmdRender : public GpuCmdBase
{
public:
    GpuCmdRender()
    {
        pCmd_ = new CmdType;
        cmdSize_ = sizeof(CmdType);
    }

    virtual ~GpuCmdRender()
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

    CmdType *pCmd_;
    int cmdSize_;

private:
};

class CmdPipelineSelect : public GpuCmdRender<mhw_render_g9_X::PIPELINE_SELECT_CMD>
{
public:
    CmdPipelineSelect(PIPELINE_SELECTION pipelineSelection);
    virtual ~CmdPipelineSelect() {};
    bool validate();
    void parse();

private:
    void initCmd();

private:
    PIPELINE_SELECTION pipelineSelection_;
};

class CmdStateBaseAddress : public GpuCmdRender<mhw_render_g9_X::STATE_BASE_ADDRESS_CMD>
{
public:
    CmdStateBaseAddress();
    virtual ~CmdStateBaseAddress() {};

    int32_t addGSH(MediaResource *pRes, uint32_t offset = 0);
    int32_t addSSH(MediaResource *pRes, uint32_t offset = 0);
    int32_t addDSH(MediaResource *pRes, uint32_t offset = 0);
    int32_t addISH(MediaResource *pRes, uint32_t offset = 0);
    int32_t addIndObj(MediaResource *pRes, uint32_t offset = 0);

    bool validate();
    void parse();

private:
    MediaResource *pGeneralState_ = nullptr;
    MediaResource *pSurfaceState_ = nullptr;
    MediaResource *pDynamicState_ = nullptr;
    MediaResource *pIndirectObject_ = nullptr;
    MediaResource *pInstructionState_ = nullptr;
    MediaResource *pBindlessSurfaceState_ = nullptr; // media never use it
};

class CmdMediaVfeState : public GpuCmdRender<mhw_render_g9_X::MEDIA_VFE_STATE_CMD>
{
public:
    CmdMediaVfeState();
    CmdMediaVfeState(uint32_t curbeSize);
    virtual ~CmdMediaVfeState() {};
    bool validate();
    void parse();
    MOS_STATUS initCmd(const PMHW_VFE_PARAMS pVfeParams);

private:
    MOS_STATUS setParamsToCmd();
    MHW_VFE_PARAMS vfeParams_ = { 0 };

    //Temporary hardcode for this, in the future, it should be passed from caller.
    void        initHwCaps();
    MHW_RENDER_ENGINE_CAPS hwCaps_ = { 0 };
};

class CmdMediaCurbeLoad : public GpuCmdRender<mhw_render_g9_X::MEDIA_CURBE_LOAD_CMD>
{
public:
    CmdMediaCurbeLoad(uint32_t lenght, uint32_t address);
    virtual ~CmdMediaCurbeLoad() {};
    bool validate();
    void parse();

private:
    uint32_t curbeLength_ = 0;
    uint32_t curbeStartAddress_ = 0;
};

class CmdMediaInterfaceDescriptorLoad : public GpuCmdRender<mhw_render_g9_X::MEDIA_INTERFACE_DESCRIPTOR_LOAD_CMD>
{
public:
    CmdMediaInterfaceDescriptorLoad(uint32_t length, uint32_t address);
    virtual ~CmdMediaInterfaceDescriptorLoad() {};
    bool validate();
    void parse();

private:
    uint32_t MIDDLength_ = 0;
    uint32_t MIDDStartAddress_ = 0;
};

class CmdMediaObjectWalker : public GpuCmdRender<mhw_render_g9_X::MEDIA_OBJECT_WALKER_CMD>
{
public:
    CmdMediaObjectWalker();
    CmdMediaObjectWalker(uint32_t blockWidth, uint32_t blockHeight);
    virtual ~CmdMediaObjectWalker() {};
    bool validate();
    void parse();
    MOS_STATUS initCmd(const PMHW_WALKER_PARAMS pWalkerParams);

private:
    MOS_STATUS setParamsToCmd();
    MHW_WALKER_PARAMS walkerParams_ = { 0 };
};

}