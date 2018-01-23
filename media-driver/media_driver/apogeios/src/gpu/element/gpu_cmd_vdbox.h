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
//! \file     gpu_cmd_vdbox.h
//! \brief    header of gpu vdbox commands class
//! \details
//!

#pragma once

#include <vector>

#include "media_resource.h"
#include "gpu_cmd_base.h"
#include "mhw_vdbox.h"
#include "mhw_vdbox_mfx_hwcmd_g9_skl.h"

namespace Apogeios
{

enum VDBoxCodecStandard
{
    MFX_MPEG2 = 0,
    MFX_VC1 = 1,
    MFX_AVC = 2,
    MFX_JPEG = 3,
    MFX_RESERVED = 4,
    MFX_VP8 = 5,
    MFX_UNDEFINED = 9,
    MFX_CENC = 63,
    HCP_BASE = 64,
    HCP_HEVC = HCP_BASE,
    HCP_VP9 = HCP_BASE + 1,
    STANDARD_MAX
};

enum MfxSurfaceId
{
    MFX_REF = 0,
    MFX_SRC = 4,
    MFX_DSRECON = 5
};

enum SurfaceStateFormat
{
    SSF_YCRCB_NORMAL = 0,
    SSF_YCRCB_SWAPUVY = 1,
    SSF_YCRCB_SWAPUV = 2,
    SSF_YCRCB_SWAPY = 3,
    SSF_PLANAR_420_8 = 4,
    SSF_PLANAR_411_8 = 5,    // deinterlace only
    SSF_PLANAR_422_8 = 6,    // deinterlace only
    SSF_STMM_DN_STATISTICS = 7,    // deinterlace only
    SSF_R10G10B10A2_UNORM = 8,    // sample_8x8 only
    SSF_R8G8B8A8_UNORM = 9,    // sample_8x8 only
    SSF_R8B8_UNORM = 10,   // CrCb, sample_8x8 only
    SSF_R8_UNORM = 11,   // Cr/Cb, sample_8x8 only
    SSF_Y8_UNORM = 12,   // sample_8x8 only
    SSF_A8Y8U8V8_UNORM = 13,   // sample_8x8 only
    SSF_B8G8R8A8_UNORM = 14,   // sample_8x8 only
    SSF_R16G16B16A16 = 15,   // Sample_8x8 only
    SSF_PLANAR_420_16 = 23,   // Sample_8x8 only
    SSF_R16B16_UNORM = 24    // Sample_8x8 only
};

inline uint32_t ResFormat2SurfaceStateFormat(RES_FORMAT format)
{
    switch (format)
    {
    case Format_A8R8G8B8:
    case Format_X8R8G8B8:
    case Format_A8B8G8R8:
        return SSF_R8G8B8A8_UNORM;
    case Format_422H:
    case Format_422V:
        return SSF_PLANAR_422_8;
    case Format_AYUV:
    case Format_AUYV:
        return SSF_A8Y8U8V8_UNORM;
    case Format_NV12:
    case Format_NV11:
    case Format_P208:
    case Format_IMC1:
    case Format_IMC3:
        return SSF_PLANAR_420_8;
    case Format_400P:
    case Format_P8:
        return SSF_Y8_UNORM;
    case Format_411P:
    case Format_411R:
        return SSF_PLANAR_411_8;
    case Format_UYVY:
        return SSF_YCRCB_SWAPY;
    case Format_YVYU:
        return SSF_YCRCB_SWAPUV;
    case Format_VYUY:
        return SSF_YCRCB_SWAPUVY;
    case Format_YUY2:
    case Format_YUYV:
    case Format_444P:
    case Format_IMC2:
    case Format_IMC4:
    default:
        return SSF_YCRCB_NORMAL;
    }

    return SSF_YCRCB_NORMAL;
}

inline bool isVPlanePresent(RES_FORMAT format)
{
    switch (format)
    {
    case Format_NV12:
    case Format_NV11:
    case Format_P208:
    case Format_IMC1:
    case Format_IMC3:
    case Format_YUY2:
    case Format_YUYV:
    case Format_YVYU:
    case Format_UYVY:
    case Format_VYUY:
    case Format_422H:
    case Format_422V:
        // Adding RGB formats because RGB is treated like YUV for JPEG encode and decode
    case Format_RGBP:
    case Format_BGRP:
    case Format_A8R8G8B8:
    case Format_X8R8G8B8:
    case Format_A8B8G8R8:
    case Format_411P:
    case Format_411R:
    case Format_444P:
    case Format_IMC2:
    case Format_IMC4:
        return true;
    default:
        return false;
    }
}

template<typename CmdType>
class GpuCmdVDBox : public GpuCmdBase
{
public:
    GpuCmdVDBox()
    {
        pCmd_ = new CmdType;
        cmdSize_ = sizeof(CmdType);
    }

    virtual ~GpuCmdVDBox()
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

    virtual uint32_t getCmdSize()
    {
        return pCmd_->byteSize;
    }

    virtual uint8_t* getCmdData()
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

class CmdMfxPipeModeSelect : public GpuCmdVDBox<mhw_vdbox_mfx_g9_skl::MFX_PIPE_MODE_SELECT_CMD>
{
public:
    CmdMfxPipeModeSelect(VDBoxCodecStandard standard, bool codecMode, bool isVDEnc, bool enableStreamOut,
        bool enableDblkStreamOut, bool enablePostDblkOut, bool enablePreDblkOut);
    virtual ~CmdMfxPipeModeSelect() {};
    bool validate();
    void parse();

private:
};

class CmdMfxSurfaceState : public GpuCmdVDBox<mhw_vdbox_mfx_g9_skl::MFX_SURFACE_STATE_CMD>
{
public:
    CmdMfxSurfaceState(MediaResource* res, MfxSurfaceId surfId);
    virtual ~CmdMfxSurfaceState() {};
    bool validate();
    void parse();

private:
};

class CmdMfxPipeBufAddrState : public GpuCmdVDBox<mhw_vdbox_mfx_g9_skl::MFX_PIPE_BUF_ADDR_STATE_CMD>
{
public:
    struct PipeBufResources
    {
        MediaResource* preDblk;            // DW 1 ~ 3
        MediaResource* postDblk;           // DW 4 ~ 6
        MediaResource* orignalYuv;         // DW 7 ~ 9
        MediaResource* streamOutData;      // DW 10 ~ 12
        MediaResource* intraRowStore;      // DW 13 ~ 15
        MediaResource* dblkRowStrore;      // DW 16 ~ 18
        MediaResource* refPic[16];         // DW 19 ~ 50
        MediaResource* macroblockBuf;      // DW 52 ~ 54
        MediaResource* streamOutILDB;      // DW 55 ~ 57
        MediaResource* streamOutILDB2;     // DW 58 ~ 60
        MediaResource* scaledRef;          // DW 61 ~ 63
        MediaResource* streamOutSliceSize; // DW 64 ~ 66
    };

public:
    CmdMfxPipeBufAddrState(CmdMfxPipeBufAddrState::PipeBufResources* pipeRes);
    virtual ~CmdMfxPipeBufAddrState() {};
    bool validate();
    void parse();

private:
};

class CmdMfxIndObjBaseAddrState : public GpuCmdVDBox<mhw_vdbox_mfx_g9_skl::MFX_IND_OBJ_BASE_ADDR_STATE_CMD>
{
public:
    struct IndObjResources
    {
        MediaResource* indBitstream; // DW 1 ~ 5
        MediaResource* indMvObject;  // DW 6 ~ 10
        MediaResource* indITCoeff;   // DW 11 ~ 15
        MediaResource* indItDblk;    // DW 16 ~ 20
        MediaResource* indPakBSE;    // DW 21 ~ 25
    };
public:
    CmdMfxIndObjBaseAddrState(IndObjResources* indRes);
    virtual ~CmdMfxIndObjBaseAddrState() {};
    bool validate();
    void parse();

private:
};

class CmdMfxBspBufBaseAddrState : public GpuCmdVDBox<mhw_vdbox_mfx_g9_skl::MFX_BSP_BUF_BASE_ADDR_STATE_CMD>
{
public:
    struct BspBufResources
    {
        MediaResource* bsdRowStore;     // DW 1 ~ 3
        MediaResource* mprRowStore;     // DW 4 ~ 6
        MediaResource* bitplaneReadBuf; // DW 7 ~ 9
    };
public:
    CmdMfxBspBufBaseAddrState(BspBufResources* bspRes);
    virtual ~CmdMfxBspBufBaseAddrState() {};
    bool validate();
    void parse();

private:
};

class CmdMfxAvcImgState : public GpuCmdVDBox<mhw_vdbox_mfx_g9_skl::MFX_AVC_IMG_STATE_CMD>
{
public:
    struct MfxAvcImgParams
    {
        // Decoding Params
        PCODEC_AVC_PIC_PARAMS             pAvcPicParams;
        PCODEC_MVC_EXT_PIC_PARAMS         pMvcExtPicParams;
        uint8_t                           ucActiveFrameCnt;
        // Encoding Params
        PCODEC_AVC_ENCODE_SEQUENCE_PARAMS pEncodeAvcSeqParams;
        PCODEC_AVC_ENCODE_PIC_PARAMS      pEncodeAvcPicParams;
        PCODEC_AVC_ENCODE_SLICE_PARAMS    pEncodeAvcSliceParams;

        PCODEC_REF_LIST *ppRefList;
        uint32_t         dwTqEnabled;
        uint32_t         dwTqRounding;
        uint32_t         dwMaxVmvR;
        uint16_t         wPicWidthInMb;
        uint16_t         wPicHeightInMb;
        uint16_t         wSlcHeightInMb;
        uint8_t          ucKernelMode; // normal, performance, quality.
        uint8_t          ucCurrPass;
        uint8_t         *pDeltaQp;
        uint32_t         dwMaxFrameSize;
        bool             bIPCMPass;
        bool             bVdencEnabled;
        bool             bVDEncPerfModeEnabled;
        bool             bVdencStreamInEnabled;
        bool             bVdencBRCEnabled;
        bool             bSliceSizeStreamOutEnabled;
        bool             bCrePrefetchEnable;
        uint32_t         dwMbSlcThresholdValue;
        uint32_t         dwSliceThresholdTable;
        uint32_t         dwVdencSliceMinusBytes;
        uint8_t         *pVDEncModeCost;
        uint8_t         *pVDEncMvCost;
        uint8_t         *pVDEncHmeMvCost;
    };

    struct VDEncFrameDelta
    {
        uint32_t PFrameDelta;
        uint32_t IFrameDelta;
    };

public:
    CmdMfxAvcImgState(MfxAvcImgParams* pParams);
    virtual ~CmdMfxAvcImgState() {};
    bool validate();
    void parse();

private:
    const uint32_t avcInterMbMaxSize_ = 4095; //! AVC inter MB max size
    const uint32_t avcIntraMbMaxSize_ = 2700; //! AVC intra MB max size
    const bool rhoDomainStatsEnabled_ = false;
    static const VDEncFrameDelta tblFrameDelta_[2][52];
};

class CmdMfxMpeg2ImgState : public GpuCmdVDBox<mhw_vdbox_mfx_g9_skl::MFX_MPEG2_PIC_STATE_CMD>
{
public:
    struct MfxMpeg2ImgParams
    {
    };
public:
    CmdMfxMpeg2ImgState(MfxMpeg2ImgParams* params);
    virtual ~CmdMfxMpeg2ImgState() {};
    bool validate();
    void parse();

private:
};

class CmdMfxQmState : public GpuCmdVDBox<mhw_vdbox_mfx_g9_skl::MFX_QM_STATE_CMD>
{
public:
    enum AvcQmType
    {
        avcQmIntra4x4 = 0,
        avcQmInter4x4 = 1,
        avcQmIntra8x8 = 2,
        avcQmInter8x8 = 3
    };

    enum Mpeg2QmType
    {
        mpeg2QmIntra = 0,
        mpeg2QmNonIntra,
    };

    struct AvcQmList
    {
        AvcQmList()
        {
            for (uint8_t i = 0; i < 6; i++)
            {
                for (uint8_t j = 0; j < 16; j++)
                {
                    qmList4x4_[i][j] = 16;
                }
            }
            for (uint8_t i = 0; i < 2; i++)
            {
                for (uint8_t j = 0; j < 64; j++)
                {
                    qmList8x8_[i][j] = 16;
                }
            }
        };

        uint8_t qmList4x4_[6][16];
        uint8_t qmList8x8_[2][64];
    };

    struct Mpeg2QmList
    {
        Mpeg2QmList()
        {
            for (auto i=0; i<64; i++)
                qmList64[i] = 16;
        }

        uint8_t qmList64[64];
    };

public:
    CmdMfxQmState(AvcQmType qmType, AvcQmList& qmList);
    CmdMfxQmState(Mpeg2QmType qmType, Mpeg2QmList& qmList);
    virtual ~CmdMfxQmState() {};
    bool validate();
    void parse();

private:
};

class CmdMfxFQmState : public GpuCmdVDBox<mhw_vdbox_mfx_g9_skl::MFX_FQM_STATE_CMD>
{
public:
    enum AvcFQmType
    {
        avcQmIntra4x4 = 0,
        avcQmInter4x4 = 1,
        avcQmIntra8x8 = 2,
        avcQmInter8x8 = 3
    };

    struct AvcFQmList
    {
        AvcFQmList()
        {
            for (uint8_t i = 0; i < 6; i++)
            {
                for (uint8_t j = 0; j < 16; j++)
                {
                    fqmList4x4_[i][j] = recip(defaultQM);
                }
            }
            for (uint8_t i = 0; i < 2; i++)
            {
                for (uint8_t j = 0; j < 64; j++)
                {
                    fqmList8x8_[i][j] = recip(defaultQM);;
                }
            }
        };

        uint16_t recip(uint8_t val)
        {
            return (val < 2) ? 0xffff : (4096 * 16) / val;
        }

        const uint8_t defaultQM = 16;
        uint16_t fqmList4x4_[6][16];
        uint16_t fqmList8x8_[2][64];
    };
public:
    CmdMfxFQmState(AvcFQmType type, AvcFQmList& list);
    virtual ~CmdMfxFQmState() {};
    bool validate();
    void parse();

private:
};

class CmdMfxAvcDirectModeState : public GpuCmdVDBox<mhw_vdbox_mfx_g9_skl::MFX_AVC_DIRECTMODE_STATE_CMD>
{
public:
    struct DirectMVResources
    {
        MediaResource* dmvBuf[16];
        MediaResource* dmvWriteBuf;
    };
public:
    CmdMfxAvcDirectModeState(DirectMVResources* resources);
    virtual ~CmdMfxAvcDirectModeState() {};
    bool validate();
    void parse();

private:
};

class CmdMfxAvcSliceState : public GpuCmdVDBox<mhw_vdbox_mfx_g9_skl::MFX_AVC_SLICE_STATE_CMD>
{
public:
    struct AvcSliceState
    {
        uint16_t wFrameFieldHeightInMB;
        bool bVdencInUse;
        bool bVdencNoTailInsertion;
        bool bBrcEnabled;
        bool bFirstPass;
        bool bLastPass;
        bool bRCPanicEnable;
        bool bRoundingInterEnable;
        uint32_t dwRoundingValue;
        uint32_t dwRoundingIntraValue;

        PCODEC_AVC_PIC_PARAMS     pAvcPicParams;
        PCODEC_MVC_EXT_PIC_PARAMS pMvcExtPicParams;
        PCODEC_AVC_SLICE_PARAMS   pAvcSliceParams;

        PCODEC_AVC_ENCODE_SEQUENCE_PARAMS pEncodeAvcSeqParams;
        PCODEC_AVC_ENCODE_PIC_PARAMS      pEncodeAvcPicParams;
        PCODEC_AVC_ENCODE_SLICE_PARAMS    pEncodeAvcSliceParams;
    };
public:
    CmdMfxAvcSliceState(AvcSliceState* pSlcState);
    virtual ~CmdMfxAvcSliceState() {};
    bool validate();
    void parse();

private:
};

class CmdMfxPakInsertObject : public GpuCmdVDBox<mhw_vdbox_mfx_g9_skl::MFX_PAK_INSERT_OBJECT_CMD>
{
public:
    struct PakInsertObj
    {
        uint32_t dwBitSize;
        uint32_t dwOffset;
        uint32_t uiSkipEmulationCheckCount;
        bool bLastHeader;
        bool bEmulationByteBitsInsert;
        bool bHeaderLengthExcludeFrmSize;
        bool bResetBitstreamStartingPos;
        bool bEndOfSlice;
        std::vector<uint32_t> payload;
    };
public:
    CmdMfxPakInsertObject(PakInsertObj* insertObj);
    virtual ~CmdMfxPakInsertObject() {};

    // this command has variable length, we need to extend from base GpuCmdVDBox class and override some functions
    uint32_t getCmdSize()
    {
        return (uint32_t)cmdExt_.size();
    }
    uint8_t* getCmdData()
    {
        return (uint8_t*)&cmdExt_[0];
    }

    bool validate();
    void parse();

private:
    std::vector<uint8_t> cmdExt_;
};

}