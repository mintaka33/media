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
//! \file     gpu_cmd_vdbox.cpp
//! \brief    gpu vdbox commands implementation
//! \details
//!

#include "common.h"
#include "gpu_cmd_vdbox.h"
#include "mhw_vdbox.h"

namespace Apogeios
{

CmdMfxPipeModeSelect::CmdMfxPipeModeSelect(
    VDBoxCodecStandard standard,
    bool isEncode,
    bool isVDEnc,
    bool enableStreamOut,
    bool enableDblkStreamOut,
    bool enablePostDblkOut,
    bool enablePreDblkOut)
{
    pCmd_->DW1.StandardSelect = standard;
    pCmd_->DW1.StreamOutEnable = enableStreamOut && (isVDEnc == false);
    pCmd_->DW1.DeblockerStreamOutEnable = enableDblkStreamOut;
    pCmd_->DW1.PostDeblockingOutputEnablePostdeblockoutenable = enablePostDblkOut;
    pCmd_->DW1.PreDeblockingOutputEnablePredeblockoutenable = enablePreDblkOut;

    if (isEncode)
    {
        pCmd_->DW1.CodecSelect = 1;
        pCmd_->DW1.DecoderShortFormatMode = 0;
        if (isVDEnc)
        {
            pCmd_->DW1.VdencMode = 1;
            pCmd_->DW1.ScaledSurfaceEnable = 1;
            pCmd_->DW1.StreamOutEnable = 0;
            pCmd_->DW1.FrameStatisticsStreamoutEnable = 1;
        }
    }
    else
    {
        pCmd_->DW1.CodecSelect = 0;
        pCmd_->DW1.DecoderModeSelect = 0; // 0 - VLD; 1 - IDCT
        pCmd_->DW1.DecoderShortFormatMode = 1;
    }
}

bool CmdMfxPipeModeSelect::validate()
{
    return true;
}

void CmdMfxPipeModeSelect::parse()
{
}

CmdMfxSurfaceState::CmdMfxSurfaceState(MediaResource* res, MfxSurfaceId surfId)
{
    RES_FORMAT resFormat = res->getFormat();

    pCmd_->DW1.SurfaceId = surfId;
    pCmd_->DW2.Height = res->getHeight() - 1;
    pCmd_->DW2.Width = res->getWidth() - 1;
    pCmd_->DW3.TileWalk = 1; // set to 1 for all video codec and for JPEG.
    pCmd_->DW3.TiledSurface = (res->getTileType() != TILE_TYPE_LINEAR) ? true : false;
    pCmd_->DW3.InterleaveChroma = (resFormat == Format_P8) ? 0 : 1;
    pCmd_->DW3.SurfacePitch = res->getPitch() - 1;
    pCmd_->DW3.SurfaceFormat = ResFormat2SurfaceStateFormat(resFormat);

    // APGTODO: getUOffsetY()/getVOffsetY() may return 0 for NV12 surface, need double check.
    uint32_t alignment = (surfId == MFX_SRC) ? 4 : 16;
    uint32_t offsetU = res->getUOffsetY();
    uint32_t offsetV = (isVPlanePresent(resFormat)) ? res->getVOffsetY() : offsetU;
    pCmd_->DW4.YOffsetForUCb = UTIL_ALIGN(offsetU, alignment);
    pCmd_->DW5.YOffsetForVCr = UTIL_ALIGN(offsetV, alignment);
}

bool CmdMfxSurfaceState::validate()
{
    return true;
}

void CmdMfxSurfaceState::parse()
{
}

CmdMfxPipeBufAddrState::CmdMfxPipeBufAddrState(PipeBufResources* pipeRes)
{
    if (pipeRes->preDblk)
    {
        uint32_t offset = 0;
        uint64_t address = pipeRes->preDblk->getCannoizedGpuAddress();
        setGpuAddress(&pCmd_->DW1.Value, &pCmd_->DW2.Value, address, offset, 12);
        addResource(pipeRes->preDblk);
    }

    if (pipeRes->postDblk)
    {
        uint32_t offset = 0;
        uint64_t address = pipeRes->postDblk->getCannoizedGpuAddress();
        setGpuAddress(&pCmd_->DW4.Value, &pCmd_->DW5.Value, address, offset, 12);
        addResource(pipeRes->postDblk);
    }

    if (pipeRes->orignalYuv)
    {
        uint32_t offset = 0;
        uint64_t address = pipeRes->orignalYuv->getCannoizedGpuAddress();
        setGpuAddress(&pCmd_->DW7.Value, &pCmd_->DW8.Value, address, offset, 12);
        addResource(pipeRes->orignalYuv);
    }

    if (pipeRes->streamOutData)
    {
        uint32_t offset = 0;
        uint64_t address = pipeRes->streamOutData->getCannoizedGpuAddress();
        setGpuAddress(&pCmd_->DW10.Value, &pCmd_->DW11.Value, address, offset, 12);
        addResource(pipeRes->streamOutData);
    }

    if (pipeRes->intraRowStore)
    {
        uint32_t offset = 0;
        uint64_t address = pipeRes->intraRowStore->getCannoizedGpuAddress();
        setGpuAddress(&pCmd_->DW13.Value, &pCmd_->DW14.Value, address, offset, 12);
        addResource(pipeRes->intraRowStore);
    }

    if (pipeRes->dblkRowStrore)
    {
        uint32_t offset = 0;
        uint64_t address = pipeRes->dblkRowStrore->getCannoizedGpuAddress();
        setGpuAddress(&pCmd_->DW16.Value, &pCmd_->DW17.Value, address, offset, 12);
        addResource(pipeRes->dblkRowStrore);
    }

    for (int32_t i = 0; i < 16; i++)
    {
        if (pipeRes->refPic[i])
        {
            uint32_t offset = 0;
            uint64_t address = pipeRes->refPic[i]->getCannoizedGpuAddress();
            setGpuAddress(&pCmd_->Refpicbaseaddr[i].DW0_1.Value[0], &pCmd_->Refpicbaseaddr[i].DW0_1.Value[1], address, offset, 12);
            addResource(pipeRes->refPic[i]);
        }
    }

    if (pipeRes->macroblockBuf)
    {
        uint32_t offset = 0;
        uint64_t address = pipeRes->macroblockBuf->getCannoizedGpuAddress();
        setGpuAddress(&pCmd_->DW52.Value, &pCmd_->DW53.Value, address, offset, 12);
        addResource(pipeRes->macroblockBuf);
    }

    MediaResource* streamOutILDB;
    if (pipeRes->streamOutILDB)
    {
        uint32_t offset = 0;
        uint64_t address = pipeRes->streamOutILDB->getCannoizedGpuAddress();
        setGpuAddress(&pCmd_->DW55.Value, &pCmd_->DW56.Value, address, offset, 12);
        addResource(pipeRes->streamOutILDB);
    }

    if (pipeRes->streamOutILDB2)
    {
        uint32_t offset = 0;
        uint64_t address = pipeRes->streamOutILDB2->getCannoizedGpuAddress();
        setGpuAddress(&pCmd_->DW58.Value, &pCmd_->DW59.Value, address, offset, 12);
        addResource(pipeRes->streamOutILDB2);
    }

    if (pipeRes->scaledRef)
    {
        uint32_t offset = 0;
        uint64_t address = pipeRes->scaledRef->getCannoizedGpuAddress();
        setGpuAddress(&pCmd_->DW62.Value, &pCmd_->DW63.Value, address, offset, 12);
        addResource(pipeRes->scaledRef);
    }

    if (pipeRes->streamOutSliceSize)
    {
        uint32_t offset = 0;
        uint64_t address = pipeRes->streamOutSliceSize->getCannoizedGpuAddress();
        //setGpuAddress(&pCmd_->DW65.Value, &pCmd_->DW66.Value, address, offset, 12);
        addResource(pipeRes->streamOutSliceSize);
    }
}

bool CmdMfxPipeBufAddrState::validate()
{
    return true;
}

void CmdMfxPipeBufAddrState::parse()
{
}

CmdMfxIndObjBaseAddrState::CmdMfxIndObjBaseAddrState(IndObjResources* indRes)
{
    if (indRes->indBitstream)
    {
        uint32_t offset = 0;
        uint64_t address = indRes->indBitstream->getCannoizedGpuAddress();
        setGpuAddress(&pCmd_->DW1.Value, &pCmd_->DW2.Value, address, offset, 12);
        addResource(indRes->indBitstream);
    }

    if (indRes->indMvObject)
    {
        uint32_t offset = 0;
        uint64_t address = indRes->indMvObject->getCannoizedGpuAddress();
        setGpuAddress(&pCmd_->DW6.Value, &pCmd_->DW7.Value, address, offset, 12);
        addResource(indRes->indMvObject);
    }

    if (indRes->indITCoeff)
    {
        uint32_t offset = 0;
        uint64_t address = indRes->indITCoeff->getCannoizedGpuAddress();
        setGpuAddress(&pCmd_->DW11.Value, &pCmd_->DW12.Value, address, offset, 12);
        addResource(indRes->indITCoeff);
    }

    if (indRes->indItDblk)
    {
        uint32_t offset = 0;
        uint64_t address = indRes->indItDblk->getCannoizedGpuAddress();
        setGpuAddress(&pCmd_->DW16.Value, &pCmd_->DW17.Value, address, offset, 12);
        addResource(indRes->indItDblk);
    }

    if (indRes->indPakBSE)
    {
        uint32_t offset = 0;
        uint64_t address = indRes->indPakBSE->getCannoizedGpuAddress();
        setGpuAddress(&pCmd_->DW21.Value, &pCmd_->DW22.Value, address, offset, 12);
        addResource(indRes->indPakBSE);
    }
}

bool CmdMfxIndObjBaseAddrState::validate()
{
    return true;
}

void CmdMfxIndObjBaseAddrState::parse()
{
}

CmdMfxBspBufBaseAddrState::CmdMfxBspBufBaseAddrState(BspBufResources* bspRes)
{
    if (bspRes->bsdRowStore)
    {
        uint32_t offset = 0;
        uint64_t address = bspRes->bsdRowStore->getCannoizedGpuAddress();
        setGpuAddress(&pCmd_->DW1.Value, &pCmd_->DW2.Value, address, offset, 12);
        addResource(bspRes->bsdRowStore);
    }

    if (bspRes->mprRowStore)
    {
        uint32_t offset = 0;
        uint64_t address = bspRes->mprRowStore->getCannoizedGpuAddress();
        setGpuAddress(&pCmd_->DW4.Value, &pCmd_->DW5.Value, address, offset, 12);
        addResource(bspRes->mprRowStore);
    }

    if (bspRes->bitplaneReadBuf)
    {
        uint32_t offset = 0;
        uint64_t address = bspRes->bitplaneReadBuf->getCannoizedGpuAddress();
        setGpuAddress(&pCmd_->DW7.Value, &pCmd_->DW8.Value, address, offset, 12);
        addResource(bspRes->bitplaneReadBuf);
    }
}

bool CmdMfxBspBufBaseAddrState::validate()
{
    return true;
}

void CmdMfxBspBufBaseAddrState::parse()
{
}

CmdMfxAvcImgState::CmdMfxAvcImgState(MfxAvcImgParams* params)
{
    // PPS/SPS should be passed from outside
    CODEC_AVC_ENCODE_SEQUENCE_PARAMS seq = {};
    CODEC_AVC_ENCODE_PIC_PARAMS pic = {};

    PCODEC_AVC_ENCODE_SEQUENCE_PARAMS pSeq = &seq; // params->pEncodeAvcSeqParams;
    PCODEC_AVC_ENCODE_PIC_PARAMS pPic = &pic; // params->pEncodeAvcPicParams;

    // DW1
    uint32_t numMBs = params->wPicWidthInMb * params->wPicHeightInMb;
    pCmd_->DW1.FrameSize = (numMBs > 0xFFFF) ? 0xFFFF : numMBs;

    // DW2
    pCmd_->DW2.FrameHeight = params->wPicHeightInMb - 1;
    pCmd_->DW2.FrameWidth = params->wPicWidthInMb - 1;

    // DW3
    pCmd_->DW3.ImgstructImageStructureImgStructure10 = 0; // 0 - frame; 1- top field; 3 - bottom field
    pCmd_->DW3.WeightedBipredIdc = pPic->weighted_bipred_idc;
    pCmd_->DW3.WeightedPredFlag = pPic->weighted_pred_flag;
    pCmd_->DW3.FirstChromaQpOffset = pPic->chroma_qp_index_offset;
    pCmd_->DW3.SecondChromaQpOffset = pPic->second_chroma_qp_index_offset;
    pCmd_->DW3.RhodomainRateControlEnable = params->bVdencEnabled;

    // DW4
    pCmd_->DW4.Fieldpicflag = 0;
    pCmd_->DW4.Mbaffflameflag = pSeq->mb_adaptive_frame_field_flag;
    pCmd_->DW4.Framembonlyflag = pSeq->frame_mbs_only_flag;
    pCmd_->DW4.Transform8X8Flag = pPic->transform_8x8_mode_flag;
    pCmd_->DW4.Direct8X8Infflag = pSeq->direct_8x8_inference_flag;
    pCmd_->DW4.Constrainedipredflag = pPic->constrained_intra_pred_flag;
    pCmd_->DW4.Entropycodingflag = pPic->entropy_coding_mode_flag;
    pCmd_->DW4.Chromaformatidc = pSeq->chroma_format_idc;
    pCmd_->DW4.Mbmvformatflag = 1;
    pCmd_->DW4.Mvunpackedflag = 1;
    pCmd_->DW4.Loadslicepointerflag = 0;
    pCmd_->DW4.Mbstatenabled = 0;
    if ((params->dwMaxFrameSize > 0) && params->ucCurrPass && params->pDeltaQp)
    {
        pCmd_->DW4.Mbstatenabled = 1;
    }
    pCmd_->DW4.Minframewsize = 0;

    // DW5
    pCmd_->DW5.IntrambmaxbitflagIntrambmaxsizereportmask = 1;
    pCmd_->DW5.IntermbmaxbitflagIntermbmaxsizereportmask = 1;
    pCmd_->DW5.FrameszoverflagFramebitratemaxreportmask = 1;
    pCmd_->DW5.FrameszunderflagFramebitrateminreportmask = 1;
    pCmd_->DW5.IntraIntermbipcmflagForceipcmcontrolmask = 1;
    pCmd_->DW5.MbratectrlflagMbLevelRateControlEnablingFlag = 0;
    pCmd_->DW5.Nonfirstpassflag = 0;
    pCmd_->DW5.TrellisQuantizationChromaDisableTqchromadisable = true;
    if (params->dwMaxFrameSize && params->ucCurrPass)
    {
        pCmd_->DW5.Nonfirstpassflag = 1;
    }

    if (params->dwTqEnabled &&
        pCmd_->DW4.Entropycodingflag)
    {
        pCmd_->DW5.TrellisQuantizationEnabledTqenb = params->dwTqEnabled;
        pCmd_->DW5.TrellisQuantizationRoundingTqr = params->dwTqRounding;
    }
    else
    {
        pCmd_->DW5.TrellisQuantizationEnabledTqenb = pCmd_->DW5.TrellisQuantizationRoundingTqr = 0;
    }

    //DW6
    pCmd_->DW6.Intrambmaxsz = avcIntraMbMaxSize_;
    pCmd_->DW6.Intermbmaxsz = avcInterMbMaxSize_;

    //DW8
    pCmd_->DW8.Slicedeltaqppmax0 =
        pCmd_->DW8.Slicedeltaqpmax1 =
        pCmd_->DW8.Slicedeltaqpmax2 =
        pCmd_->DW8.Slicedeltaqpmax3 = 0;

    //DW9
    pCmd_->DW9.Slicedeltaqpmin0 =
        pCmd_->DW9.Slicedeltaqpmin1 =
        pCmd_->DW9.Slicedeltaqpmin2 =
        pCmd_->DW9.Slicedeltaqpmin3 = 0;

    //DW10
    pCmd_->DW10.Framebitratemin = 0;
    pCmd_->DW10.Framebitrateminunit = 1;
    pCmd_->DW10.Framebitrateminunitmode = 1;
    pCmd_->DW10.Framebitratemax = (1 << 14) - 1;
    pCmd_->DW10.Framebitratemaxunit = 1;
    pCmd_->DW10.Framebitratemaxunitmode = 1;

    //DW11
    pCmd_->DW11.Framebitratemindelta = 0;
    pCmd_->DW11.Framebitratemaxdelta = 0;
    pCmd_->DW11.SliceStatsStreamoutEnable = params->bSliceSizeStreamOutEnabled;

    //DW12
    //pCmd_->DW12.VadErrorLogic = 1;

    //add for multiple pass
    if (params->dwMaxFrameSize > 0 && params->pDeltaQp && (!params->bIPCMPass))
    {
        pCmd_->DW8.Slicedeltaqppmax0 =
            pCmd_->DW8.Slicedeltaqpmax1 =
            pCmd_->DW8.Slicedeltaqpmax2 =
            pCmd_->DW8.Slicedeltaqpmax3 = params->pDeltaQp[params->ucCurrPass];
        pCmd_->DW10.Framebitratemaxunit = 0;
        pCmd_->DW10.Framebitratemaxunitmode = 0;
        //when FrameBitrateMaxUnit & FrameBitrateMaxUnitMode both are 0, the frame size unit is 128bytes.
        pCmd_->DW10.Framebitratemax = params->dwMaxFrameSize >> 7;
        pCmd_->DW11.Framebitratemaxdelta = params->dwMaxFrameSize >> 8;

        // In compatibility mode (DW10.FrameBitrateMaxUnitMode = 0), only 12 bits is used.
        // If the calculated value of max frame size exceeded 12 bits, need change unit from 128 bytes to 16K bytes.
        if (params->dwMaxFrameSize >= (0x1 << 12) * 128)
        {
            // use 16K bytes unit mode in compatibility mode.
            pCmd_->DW10.Framebitratemaxunit = 1;
            pCmd_->DW10.Framebitratemaxunitmode = 0;
            pCmd_->DW10.Framebitratemax = params->dwMaxFrameSize >> 14;
            pCmd_->DW11.Framebitratemaxdelta = params->dwMaxFrameSize >> 15;
        }
    }

    if (params->bIPCMPass)
    {
        // InterMbConfFlag, IntraMbConfFlag: not being used in HW
        pCmd_->DW4.Mbstatenabled = true;
        pCmd_->DW5.IntraIntermbipcmflagForceipcmcontrolmask = true;
    }

    pCmd_->DW16_17.ExtendedRhodomainStatisticsEnable = rhoDomainStatsEnabled_;

    if (pSeq->EnableSliceLevelRateCtrl)
    {
        uint8_t qpY = pPic->QpY;
        if (params->dwSliceThresholdTable == NO_SLICE_THRESHOLD_TABLE) // Do not use any Slice Threshold Table
        {
            pCmd_->DW19.ThresholdSizeInBytes = (pPic->SliceSizeInBytes > params->dwVdencSliceMinusBytes) ?
                (pPic->SliceSizeInBytes - params->dwVdencSliceMinusBytes) : 0;
        }
        else if (params->dwSliceThresholdTable == USE_SLICE_THRESHOLD_TABLE_100_PERCENT)
        {
            pCmd_->DW19.ThresholdSizeInBytes = pPic->SliceSizeInBytes -
                UTIL_MIN(pPic->SliceSizeInBytes, ((pPic->CodingType == I_TYPE) ? tblFrameDelta_[0][qpY].IFrameDelta : tblFrameDelta_[0][qpY].PFrameDelta));
        }
        else if (params->dwSliceThresholdTable == USE_SLICE_THRESHOLD_TABLE_90_PERCENT)
        {
            pCmd_->DW19.ThresholdSizeInBytes = pPic->SliceSizeInBytes -
                UTIL_MIN(pPic->SliceSizeInBytes, ((pPic->CodingType == I_TYPE) ? tblFrameDelta_[1][qpY].IFrameDelta : tblFrameDelta_[1][qpY].PFrameDelta));
        }
        pCmd_->DW20.TargetSliceSizeInBytes = pPic->SliceSizeInBytes;
    }
}

bool CmdMfxAvcImgState::validate()
{
    return true;
}

void CmdMfxAvcImgState::parse()
{
}

CmdMfxQmState::CmdMfxQmState(AvcQmType qmType, AvcQmList& qmList)
{
    uint8_t* qMatrix = (uint8_t*)pCmd_->ForwardQuantizerMatrix;
    pCmd_->DW1.Obj0.Avc = qmType;

    switch (qmType)
    {
    case avcQmIntra4x4:
        for (uint8_t i = 0; i < 3; i++)
        {
            for (uint8_t j = 0; j < 16; j++)
            {
                qMatrix[i * 16 + j] = qmList.qmList4x4_[i][j];
            }
        }
        break;
    case avcQmInter4x4:
        for (uint8_t i = 0; i < 3; i++)
        {
            for (uint8_t j = 0; j < 16; j++)
            {
                qMatrix[i * 16 + j] = qmList.qmList4x4_[i + 3][j];
            }
        }
        break;
    case avcQmIntra8x8:
        for (uint8_t i = 0; i < 64; i++)
        {
            qMatrix[i] = qmList.qmList8x8_[0][i];
        }
        break;
    case avcQmInter8x8:
        for (uint8_t i = 0; i < 64; i++)
        {
            qMatrix[i] = qmList.qmList8x8_[1][i];
        }
        break;
    default:
        break;
    }
}

bool CmdMfxQmState::validate()
{
    return true;
}

void CmdMfxQmState::parse()
{
}

CmdMfxFQmState::CmdMfxFQmState(AvcFQmType type, AvcFQmList& list)
{
    uint16_t* qMatrix = (uint16_t*)pCmd_->ForwardQuantizerMatrix;
    pCmd_->DW1.Obj0.Avc = type;

    switch (type)
    {
    case avcQmIntra4x4:
        for (uint8_t i = 0; i < 3; i++)
        {
            for (uint8_t j = 0; j < 16; j++)
            {
                qMatrix[i * 16 + j] = list.fqmList4x4_[i][j];
            }
        }
        break;
    case avcQmInter4x4:
        for (uint8_t i = 0; i < 3; i++)
        {
            for (uint8_t j = 0; j < 16; j++)
            {
                qMatrix[i * 16 + j] = list.fqmList4x4_[i + 3][j];
            }
        }
        break;
    case avcQmIntra8x8:
        for (uint8_t i = 0; i < 64; i++)
        {
            qMatrix[i] = list.fqmList8x8_[0][i];
        }
        break;
    case avcQmInter8x8:
        for (uint8_t i = 0; i < 64; i++)
        {
            qMatrix[i] = list.fqmList8x8_[1][i];
        }
        break;
    default:
        break;
    }
}

bool CmdMfxFQmState::validate()
{
    return true;
}

void CmdMfxFQmState::parse()
{
}

CmdMfxAvcDirectModeState::CmdMfxAvcDirectModeState(DirectMVResources* resources)
{
    for (int8_t i = 0; i < 16; i++)
    {
        if (resources->dmvBuf[i])
        {
            uint32_t offset = 0;
            uint64_t address = resources->dmvBuf[i]->getCannoizedGpuAddress();
            setGpuAddress(&pCmd_->DirectMvBufferBaseAddress[i].DW0_1.Value[0],
                &pCmd_->DirectMvBufferBaseAddress[i].DW0_1.Value[1], address, offset, 12);
            addResource(resources->dmvBuf[i]);
        }
    }

    if (resources->dmvWriteBuf)
    {
        uint32_t offset = 0;
        uint64_t address = resources->dmvWriteBuf->getCannoizedGpuAddress();
        setGpuAddress(&pCmd_->DirectMvBufferBaseAddressForWrite[0].DW0_1.Value[0],
            &pCmd_->DirectMvBufferBaseAddressForWrite[0].DW0_1.Value[1], address, offset, 12);
        addResource(resources->dmvWriteBuf);
    }

    // APGTODO: set POC list
}

bool CmdMfxAvcDirectModeState::validate()
{
    return true;
}

void CmdMfxAvcDirectModeState::parse()
{
}

CmdMfxAvcSliceState::CmdMfxAvcSliceState(AvcSliceState* pState)
{
    // APGTODO: use external seq/pic/slc parameters
    CODEC_AVC_ENCODE_SEQUENCE_PARAMS seqParams = {};
    CODEC_AVC_ENCODE_PIC_PARAMS      picParams = {};
    CODEC_AVC_ENCODE_SLICE_PARAMS    slcParams = {};

    PCODEC_AVC_ENCODE_SEQUENCE_PARAMS seq = &seqParams; //;pState->pEncodeAvcSeqParams;
    PCODEC_AVC_ENCODE_PIC_PARAMS pic = &picParams; // pState->pEncodeAvcPicParams;
    PCODEC_AVC_ENCODE_SLICE_PARAMS slc = &slcParams; // pState->pEncodeAvcSliceParams;

    uint16_t widthInMb = seq->pic_width_in_mbs_minus1 + 1;
    uint16_t frameFieldHeightInMb = pState->wFrameFieldHeightInMB;
    bool mbaffFrameFlag = seq->mb_adaptive_frame_field_flag ? true : false;
    uint32_t startMbNum = slc->first_mb_in_slice * (1 + mbaffFrameFlag);

    //DW1
    pCmd_->DW1.SliceType = Slice_Type[slc->slice_type];
    //DW2
    pCmd_->DW2.Log2WeightDenomLuma = slc->luma_log2_weight_denom;
    pCmd_->DW2.Log2WeightDenomChroma = slc->chroma_log2_weight_denom;
    pCmd_->DW2.NumberOfReferencePicturesInInterPredictionList0 = 0;
    pCmd_->DW2.NumberOfReferencePicturesInInterPredictionList1 = 0;
    //DW3
    pCmd_->DW3.SliceAlphaC0OffsetDiv2 = slc->slice_alpha_c0_offset_div2;
    pCmd_->DW3.SliceBetaOffsetDiv2 = slc->slice_beta_offset_div2;
    pCmd_->DW3.SliceQuantizationParameter = 26 + pic->pic_init_qp_minus26 + slc->slice_qp_delta;
    pCmd_->DW3.CabacInitIdc10 = slc->cabac_init_idc;
    pCmd_->DW3.DisableDeblockingFilterIndicator = slc->disable_deblocking_filter_idc;
    pCmd_->DW3.DirectPredictionType = (slc->slice_type == 1) ? slc->direct_spatial_mv_pred_flag : 0;
    pCmd_->DW3.WeightedPredictionIndicator = DEFAULT_WEIGHTED_INTER_PRED_MODE;
    //DW4
    pCmd_->DW4.SliceHorizontalPosition = startMbNum % widthInMb;
    pCmd_->DW4.SliceVerticalPosition = startMbNum / widthInMb;
    //DW5
    pCmd_->DW5.NextSliceHorizontalPosition = (startMbNum + slc->NumMbsForSlice) % widthInMb;
    pCmd_->DW5.NextSliceVerticalPosition = (startMbNum + slc->NumMbsForSlice) / widthInMb;
    //DW6
    pCmd_->DW6.StreamId10 = 0;
    pCmd_->DW6.SliceId30 = slc->slice_id;
    pCmd_->DW6.Cabaczerowordinsertionenable = (seq->RateControlMethod == RATECONTROL_CQP) ? 0 : 1;;
    pCmd_->DW6.Emulationbytesliceinsertenable = 1;
    pCmd_->DW6.IsLastSlice = (startMbNum + slc->NumMbsForSlice) >= (uint32_t)(widthInMb * frameFieldHeightInMb);
    // Driver only programs 1st slice state, VDENC will detect the last slice
    if (pState->bVdencInUse)
    {
        pCmd_->DW6.TailInsertionPresentInBitstream = pState->bVdencNoTailInsertion ? 0 : (pic->bLastPicInSeq || pic->bLastPicInStream);
    }
    else
    {
        pCmd_->DW6.TailInsertionPresentInBitstream = (pic->bLastPicInSeq || pic->bLastPicInStream) && pCmd_->DW6.IsLastSlice;
    }
    pCmd_->DW6.SlicedataInsertionPresentInBitstream = 1;
    pCmd_->DW6.HeaderInsertionPresentInBitstream = 1;
    pCmd_->DW6.MbTypeSkipConversionDisable = 0;
    pCmd_->DW6.MbTypeDirectConversionDisable = 0;
    pCmd_->DW6.RateControlCounterEnable = (pState->bBrcEnabled && (!pState->bFirstPass));

    if (pCmd_->DW6.RateControlCounterEnable == true)
    {
        // These fields are valid only when RateControlCounterEnable = 1
        pCmd_->DW6.RcPanicType = 1;    // CBP Panic
        pCmd_->DW6.RcPanicEnable =
            (pState->bRCPanicEnable &&
            (seq->RateControlMethod != RATECONTROL_AVBR) &&
                (seq->RateControlMethod != RATECONTROL_IWD_VBR) &&
                (seq->RateControlMethod != RATECONTROL_ICQ) &&
                (seq->RateControlMethod != RATECONTROL_VCM) &&
                (seq->RateControlMethod != RATECONTROL_CQP) &&
                pState->bLastPass);    // Enable only in the last pass
        pCmd_->DW6.RcStableTolerance = 0;
        pCmd_->DW6.RcTriggleMode = 2;    // Loose Rate Control
        pCmd_->DW6.Resetratecontrolcounter = !startMbNum;
    }

    pCmd_->DW9.Roundinter = 2;

    if (slc->slice_type == 0) // P slice
    {
        pCmd_->DW2.NumberOfReferencePicturesInInterPredictionList0 = slc->num_ref_idx_l0_active_minus1_from_DDI + 1;
        pCmd_->DW3.WeightedPredictionIndicator = pic->weighted_pred_flag;

        pCmd_->DW9.Roundinterenable = pState->bRoundingInterEnable;
        pCmd_->DW9.Roundinter = pState->dwRoundingValue;
    }
    else if (slc->slice_type == 1) // B slice
    {
        pCmd_->DW2.NumberOfReferencePicturesInInterPredictionList1 = slc->num_ref_idx_l1_active_minus1_from_DDI + 1;
        pCmd_->DW2.NumberOfReferencePicturesInInterPredictionList0 = slc->num_ref_idx_l0_active_minus1_from_DDI + 1;
        pCmd_->DW3.WeightedPredictionIndicator = pic->weighted_bipred_idc;
        if (pic->weighted_bipred_idc == IMPLICIT_WEIGHTED_INTER_PRED_MODE)
        {
            // SNB requirement
            pCmd_->DW2.Log2WeightDenomLuma = 5;
            pCmd_->DW2.Log2WeightDenomChroma = 5;
        }

        pCmd_->DW9.Roundinterenable = pState->bRoundingInterEnable;
        pCmd_->DW9.Roundinter = pState->dwRoundingValue;
    }

    pCmd_->DW9.Roundintra = pState->dwRoundingIntraValue;
    pCmd_->DW9.Roundintraenable = 1;
}

bool CmdMfxAvcSliceState::validate()
{
    return true;
}

void CmdMfxAvcSliceState::parse()
{
}

CmdMfxPakInsertObject::CmdMfxPakInsertObject(PakInsertObj* pObj)
{
    pCmd_->DW0.DwordLength = pObj->payload.size();
    pCmd_->DW1.BitstreamstartresetResetbitstreamstartingpos = pObj->bResetBitstreamStartingPos;
    pCmd_->DW1.EndofsliceflagLastdstdatainsertcommandflag = pObj->bEndOfSlice;
    pCmd_->DW1.LastheaderflagLastsrcheaderdatainsertcommandflag = pObj->bLastHeader;
    pCmd_->DW1.EmulationflagEmulationbytebitsinsertenable = pObj->bEmulationByteBitsInsert;
    pCmd_->DW1.SkipemulbytecntSkipEmulationByteCount = pObj->uiSkipEmulationCheckCount;
    pCmd_->DW1.DatabitsinlastdwSrcdataendingbitinclusion50 = (pObj->dwBitSize % 32 == 0) ? 32 : (pObj->dwBitSize % 32);
    pCmd_->DW1.DatabyteoffsetSrcdatastartingbyteoffset10 = 0;
    pCmd_->DW1.Headerlengthexcludefrmsize = pObj->bEmulationByteBitsInsert ? false : pObj->bHeaderLengthExcludeFrmSize;

    // copy base command data (DW0 & DW1) into extended cmd
    uint8_t* pBaseCmd = GpuCmdVDBox::getCmdData();
    uint32_t baseSize = GpuCmdVDBox::getCmdSize();
    cmdExt_.assign(pBaseCmd, pBaseCmd + baseSize);

    // copy payload data
    uint8_t* pPayload = (uint8_t*)&pObj->payload[0];
    uint32_t sizeInByte = pObj->payload.size() * sizeof(uint32_t);
    cmdExt_.insert(cmdExt_.end(), pPayload, pPayload + sizeInByte);
}

bool CmdMfxPakInsertObject::validate()
{
    return true;
}

void CmdMfxPakInsertObject::parse()
{
}

const CmdMfxAvcImgState::VDEncFrameDelta CmdMfxAvcImgState::tblFrameDelta_[2][52] =
{
    { // 100 Percent Slice Threshold Table
        { 1400, 1400 },
        { 1400, 1400 },
        { 1400, 1400 },
        { 1400, 1400 },
        { 1400, 1400 },
        { 1400, 1400 },
        { 1400, 1400 },
        { 1400, 1400 },
        { 1400, 1400 },
        { 1400, 1400 },
        { 1400, 1400 },
        { 1400, 1400 },
        { 1400, 1400 },
        { 1400, 1400 },
        { 1400, 1400 },
        { 1400, 1400 },
        { 1300, 1400 },
        { 1275, 1325 },
        { 1250, 1275 },
        { 1200, 1200 },
        { 1125, 1125 },
        { 1075, 1075 },
        { 1000, 1000 },
        { 800, 950 },
        { 725, 900 },
        { 650, 850 },
        { 600, 800 },
        { 575, 750 },
        { 500, 700 },
        { 425, 675 },
        { 375, 650 },
        { 350, 625 },
        { 325, 550 },
        { 275, 500 },
        { 250, 450 },
        { 225, 425 },
        { 200, 375 },
        { 180, 350 },
        { 170, 320 },
        { 160, 290 },
        { 150, 260 },
        { 140, 245 },
        { 125, 230 },
        { 110, 220 },
        { 110, 220 },
        { 110, 220 },
        { 110, 220 },
        { 110, 220 },
        { 110, 220 },
        { 110, 220 },
        { 110, 220 },
        { 110, 220 }
    },
    { // 90 Percent Slice Threshold Table
        { 1400, 1400 },
        { 1400, 1400 },
        { 1400, 1400 },
        { 1400, 1400 },
        { 1400, 1400 },
        { 1400, 1400 },
        { 1400, 1400 },
        { 1400, 1400 },
        { 1400, 1400 },
        { 1400, 1400 },
        { 960, 1400 },
        { 900, 1400 },
        { 840, 1300 },
        { 770, 1200 },
        { 710, 1100 },
        { 650, 1000 },
        { 590, 900 },
        { 560, 860 },
        { 530, 815 },
        { 500, 750 },
        { 470, 700 },
        { 430, 650 },
        { 400, 600 },
        { 380, 570 },
        { 350, 540 },
        { 320, 500 },
        { 285, 465 },
        { 270, 430 },
        { 250, 400 },
        { 235, 380 },
        { 215, 360 },
        { 200, 350 },
        { 180, 320 },
        { 160, 290 },
        { 140, 260 },
        { 130, 240 },
        { 115, 220 },
        { 100, 200 },
        { 100, 200 },
        { 100, 200 },
        { 100, 200 },
        { 100, 200 },
        { 100, 200 },
        { 100, 200 },
        { 100, 200 },
        { 100, 200 },
        { 100, 200 },
        { 100, 200 },
        { 100, 200 },
        { 100, 200 },
        { 100, 200 },
        { 100, 200 }
    }
};

}