/*
 * sbfread_meas.c: SBF (Septentrio Binary Format) reading and parsing
 * functions for the measurement related blocks.
 *
 * Septentrio grants permission to use, copy, modify, and/or distribute
 * this software for any purpose with or without fee.  
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND SEPTENTRIO DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL
 * SEPTENTRIO BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <string.h>

#include "sbfread.h"

#ifndef NO_DECRYPTION
# include "sbfdecrypt.h"
#endif



/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*--- FUNCTIONS TO DECODE MEAS3 SBF BLOCKS ----------------------------------*/
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/


/* base pseudorange for the different constellations */
static const double PRBase_m[MEAS3_SYS_MAX]
= { 19e6,    /* GPS        */
    19e6,    /* GLO        */
    22e6,    /* GAL        */
    20e6,    /* BDS  !!redefined to 34000km for GEO/IGSO    */
    34e6,    /* SBAS       */
    34e6,    /* QZS        */
    34e6     /* IRN        */
};


/* a counter of the number of occurrence of the different types of
   Meas3 sub-blocks. */
uint32_t Meas3Ranges_Count_MasterLong  = 0;
uint32_t Meas3Ranges_Count_MasterShort = 0;
uint32_t Meas3Ranges_Count_MasterDelta = 0;
uint32_t Meas3Ranges_Count_SlaveLong  = 0;
uint32_t Meas3Ranges_Count_SlaveShort = 0;
uint32_t Meas3Ranges_Count_SlaveDelta = 0;


/* returns the number of bits set to 1 in the byte */
static uint32_t bitcnt(uint8_t b)
{
  uint32_t i;
  uint32_t n=0;

  for (i=0;i<8;i++)
  {
    n+= ((b>>i)&1);
  }
  return n;
}

/* returns the position of the first bit set to 1 in v (starting at 0
   for the LSB), or 32 if v is zero. */
static uint32_t lsbpos(uint32_t v)
{
  uint32_t i;

  for (i=0;i<32;i++) if (((v>>i)&1) != 0) return i;
  
  return 32;
}


/* mapping of the Meas3 lock time indicator into actual lock time in
   milliseconds */
static const uint32_t LTItoPLLTimer_ms[16] =
  {   0,
      60000,
      30000,
      15000,
      10000,
      5000,
      2000,
      1000,
      500,
      200,
      100,
      50,
      40,
      20,
      10,
      0
  };



static const SignalType_t Meas3SigIdx2SignalType_Default[MEAS3_SYS_MAX][MEAS3_SIG_MAX] =
  {
    /*   Idx:      0           1           2          3           4          5          6        7        8        9        10       11       12       13       14       15       */
    /*   GPS */ {SIG_GPSL1CA,SIG_GPSL2C ,SIG_GPSL5 ,SIG_GPSL1P ,SIG_GPSL2P,SIG_GPSL1C,SIG_LAST,SIG_LAST,SIG_LAST,SIG_LAST,SIG_LAST,SIG_LAST,SIG_LAST,SIG_LAST,SIG_LAST,SIG_LAST},
    /*   GLO */ {SIG_GLOL1CA,SIG_GLOL2CA,SIG_GLOL1P,SIG_GLOL2P ,SIG_GLOL3, SIG_LAST,  SIG_LAST,SIG_LAST,SIG_LAST,SIG_LAST,SIG_LAST,SIG_LAST,SIG_LAST,SIG_LAST,SIG_LAST,SIG_LAST},
    /*   GAL */ {SIG_GALE1BC,SIG_GALE5a ,SIG_GALE5b,SIG_GALE6BC,SIG_GALE5, SIG_LAST,  SIG_LAST,SIG_LAST,SIG_LAST,SIG_LAST,SIG_LAST,SIG_LAST,SIG_LAST,SIG_LAST,SIG_LAST,SIG_LAST},
    /*   BDS */ {SIG_BDSB1I, SIG_BDSB2I ,SIG_BDSB3, SIG_BDSB1C, SIG_BDSB2a,SIG_LAST,  SIG_LAST,SIG_LAST,SIG_LAST,SIG_LAST,SIG_LAST,SIG_LAST,SIG_LAST,SIG_LAST,SIG_LAST,SIG_LAST},
    /*   SBA */ {SIG_SBSL1CA,SIG_SBSL5,  SIG_LAST,  SIG_LAST,   SIG_LAST,  SIG_LAST,  SIG_LAST,SIG_LAST,SIG_LAST,SIG_LAST,SIG_LAST,SIG_LAST,SIG_LAST,SIG_LAST,SIG_LAST,SIG_LAST},
    /*   QZS */ {SIG_QZSL1CA,SIG_QZSL2C, SIG_QZSL5, SIG_QZSL6,  SIG_QZSL1C,SIG_QZSL1S,SIG_LAST,SIG_LAST,SIG_LAST,SIG_LAST,SIG_LAST,SIG_LAST,SIG_LAST,SIG_LAST,SIG_LAST,SIG_LAST},
    /*   IRN */ {SIG_IRNL5,  SIG_LAST,   SIG_LAST,  SIG_LAST,   SIG_LAST,  SIG_LAST,  SIG_LAST,SIG_LAST,SIG_LAST,SIG_LAST,SIG_LAST,SIG_LAST,SIG_LAST,SIG_LAST,SIG_LAST,SIG_LAST}
};


/*@-fixedformalarray@*/


/*---------------------------------------------------------------------------*/
static SignalType_t sbfread_Meas3_SigIdx2SignalType(SignalType_t     Meas3SigIdx2SignalType[MEAS3_SYS_MAX][MEAS3_SIG_MAX],
                                                    Meas3SatSystem_t SatSys,
                                                    uint32_t         SigIdx)
{
  SignalType_t SignalType;

  if (SatSys<MEAS3_SYS_MAX && SigIdx<MEAS3_SIG_MAX)
    SignalType = Meas3SigIdx2SignalType[SatSys][SigIdx];
  else
    SignalType = SIG_LAST;

  return SignalType;
}


/*---------------------------------------------------------------------------*/
static void sbfread_PrepareSigTable(SignalType_t     Meas3SigIdx2SignalType[MEAS3_SYS_MAX][MEAS3_SIG_MAX],
                                    Meas3SatSystem_t SatSys,
                                    uint8_t          SigExcluded)
{
  unsigned int i;
  
  unsigned int NrZeros = 0;
  unsigned int PosZeros[MEAS3_SIG_MAX];

  /* note the positions of the zero bits in SigExcluded (extended to
     MEAS3_SIG_MAX bits).  Only the first 8 signals can be excluded */
  for (i=0; i<MEAS3_SIG_MAX; i++)
  {
    if (((uint32_t)SigExcluded&(1<<i)) == 0)
    {
      PosZeros[NrZeros]=i;
      NrZeros++;
    }
  }
  
  /* those signals that correspond to the zero bits must be included */
  for (i=0; i<NrZeros; i++)
  {
    Meas3SigIdx2SignalType[SatSys][i] = Meas3SigIdx2SignalType_Default[SatSys][PosZeros[i]];
  }
  /* remaining signals do not exist */
  for (i=NrZeros; i<MEAS3_SIG_MAX; i++)
  {
    Meas3SigIdx2SignalType[SatSys][i] = SIG_LAST;
  }

}



/*---------------------------------------------------------------------------*/
static uint32_t sbfread_Meas3_GetAntennaIdx(const uint8_t* const  SBFBlock)
{
  HeaderAndTimeBlock_t *HT = (HeaderAndTimeBlock_t*)SBFBlock;
  uint32_t BlockNumber = SBF_ID_TO_NUMBER(HT->Header.ID);

  uint32_t AntIdx;

  if      (BlockNumber == sbfnr_Meas3Ranges_1)
    AntIdx = (uint32_t)((Meas3Ranges_1_t*)SBFBlock)->Misc&7;
  
  else if (BlockNumber == sbfnr_Meas3CN0HiRes_1)
    AntIdx = (uint32_t)((Meas3CN0HiRes_1_t*)SBFBlock)->Flags&7;
  
  else if (BlockNumber == sbfnr_Meas3Doppler_1)
    AntIdx = (uint32_t)((Meas3Doppler_1_t*)SBFBlock)->Flags&7;

  else if (BlockNumber == sbfnr_Meas3PP_1)
    AntIdx = (uint32_t)((Meas3PP_1_t*)SBFBlock)->Flags&7;

  else if (BlockNumber == sbfnr_Meas3MP_1)
    AntIdx = (uint32_t)((Meas3MP_1_t*)SBFBlock)->Flags&7;
  
  else
    AntIdx = 0;

  return AntIdx;
}
    

/*---------------------------------------------------------------------------*/
static double sbfread_Meas3_GetWavelength_m(SignalType_t     Meas3SigIdx2SignalType[MEAS3_SYS_MAX][MEAS3_SIG_MAX],
                                            Meas3SatSystem_t SatSys,
                                            uint32_t         SigIdx, 
                                            int              GLOfn)
{
  return GetWavelength_m(sbfread_Meas3_SigIdx2SignalType(Meas3SigIdx2SignalType, SatSys, SigIdx), GLOfn);
}


/*---------------------------------------------------------------------------*/
static uint32_t
sbfread_Meas3_DecodeMaster(const uint8_t* const buf,
                           SignalType_t     Meas3SigIdx2SignalType[MEAS3_SYS_MAX][MEAS3_SIG_MAX],
                           Meas3SatSystem_t SatSys,
                           uint32_t         SatIdx,
                           int              GLOfn,
                           double        Short_PRBase_m,
                           uint32_t      SigIdxMasterShort,
                           const sbfread_Meas3_RefEpoch_t* const Ref,
                           uint32_t                       TimeSinceRefEpoch_ms,
                           /*@out@*/MeasSet_t*            MeasSet,
                           /*@out@*/uint32_t*             MasterSigIdx,
                           /*@out@*/uint32_t*             SlaveSigMask,          /* slave signal mask decoded in this function */
                           bool                           PRRateAvailable,
                           /*@out@*/int16_t*              PRRate_64mm_s
#ifndef NO_DECRYPTION
                           ,/*@null@*/decrypt_ctx_t*       decryptctx
#endif
                           )
{
  uint32_t ret;
  double       Wavelength_m;

  memset (MeasSet, 0, sizeof(*MeasSet));

  if ((*buf&1) == 1)
  {
    /*MasterShort*/
    uint32_t BF1    = *(uint32_t*)buf;
    uint32_t PRLSB  = *(uint32_t*)(buf+4);
    uint32_t CmC    = (BF1>>1)&0x3ffff;
    uint32_t PRMSB  = (BF1>>19)&1;
    uint32_t LTI3   = (BF1>>20)&0x7;
    uint32_t CN0    = (BF1>>23)&0x1f;
    uint32_t SigList= (BF1>>28)&0xf;

#ifndef NO_DECRYPTION
    DecryptMeas3Ranges(decryptctx, &PRLSB, &CmC);
#endif

    *MasterSigIdx = SigIdxMasterShort;
    Wavelength_m = sbfread_Meas3_GetWavelength_m(Meas3SigIdx2SignalType, SatSys, *MasterSigIdx, GLOfn);
    
    MeasSet->signalType    = sbfread_Meas3_SigIdx2SignalType(Meas3SigIdx2SignalType, SatSys, *MasterSigIdx);
    MeasSet->flags         = (uint8_t)MEASFLAG_VALIDITY;
    MeasSet->PR_m          = Short_PRBase_m + ((double)PRLSB + 4294967296.0*(double)PRMSB)*.001;
    MeasSet->L_cycles      = CmC == 0 ? F64_NOTVALID : MeasSet->PR_m/Wavelength_m - 131.072 + (double)CmC*.001;
    MeasSet->CN0_dBHz      = (float)CN0+24.0F;
    MeasSet->PLLTimer_ms   = LTItoPLLTimer_ms[LTI3];
    if (LTI3 == 0) MeasSet->flags |= (uint8_t)MEASFLAG_HALFCYCLEAMBIGUITY;
    
    if (PRRateAvailable)
      *PRRate_64mm_s = *(int16_t*)(buf+8);
    else
      *PRRate_64mm_s = 0;

    *SlaveSigMask = SigList<<((*MasterSigIdx)+1);

    ret = PRRateAvailable ? (uint32_t)10 : (uint32_t)8;
    Meas3Ranges_Count_MasterShort++;
  }
  else if ((*buf&3) == 0)
  {
    /*MasterLong*/
    uint32_t BF1    = *(uint32_t*)buf;
    uint32_t PRLSB  = *(uint32_t*)(buf+4);
    uint16_t BF2    = *(uint16_t*)(buf+8);
    uint8_t  BF3    = *(buf+10);
    uint32_t PRMSB  = (BF1>>2)&0xf;
    uint32_t CmC    = (BF1>>6)&0x3fffff;
    uint32_t LTI4   = (BF1>>28)&0xf;
    uint32_t CN0    = (BF2>>0)&0x3f;
    uint32_t SigMask= (BF2>>6)&0x1ff;
    uint32_t Cont   = (BF2>>15)&0x1;
    bool     IsGPSPCode;

#ifndef NO_DECRYPTION
    DecryptMeas3Ranges(decryptctx, &PRLSB, &CmC);
#endif

    if (Cont != 0) SigMask |= (uint32_t)(BF3&0x7f)<<9;

    if (SigMask == 0) goto EXIT_INVALIDFORMAT;

    *MasterSigIdx = lsbpos(SigMask);
    Wavelength_m = sbfread_Meas3_GetWavelength_m(Meas3SigIdx2SignalType, SatSys, *MasterSigIdx, GLOfn);
    IsGPSPCode = (sbfread_Meas3_SigIdx2SignalType(Meas3SigIdx2SignalType, SatSys, *MasterSigIdx) == SIG_GPSL1P ||
                  sbfread_Meas3_SigIdx2SignalType(Meas3SigIdx2SignalType, SatSys, *MasterSigIdx) == SIG_GPSL2P);
    
    MeasSet->signalType    = sbfread_Meas3_SigIdx2SignalType(Meas3SigIdx2SignalType, SatSys, *MasterSigIdx);
    MeasSet->flags         = (uint8_t)MEASFLAG_VALIDITY;
    MeasSet->PR_m          = ((double)PRLSB + 4294967296.0*(double)PRMSB)*.001;
    MeasSet->L_cycles      = CmC == 0 ? F64_NOTVALID : MeasSet->PR_m/Wavelength_m - 2097.152 + (double)CmC*.001;
    MeasSet->CN0_dBHz      = IsGPSPCode ? (float)CN0 : (float)CN0+10.0F;
    MeasSet->PLLTimer_ms   = LTItoPLLTimer_ms[LTI4];
    if (LTI4 == 0) MeasSet->flags |= (uint8_t)MEASFLAG_HALFCYCLEAMBIGUITY;
    
    if (PRRateAvailable)
      *PRRate_64mm_s = *(int16_t*)(buf+10);
    else
      *PRRate_64mm_s = 0;
    
    *SlaveSigMask = SigMask ^= 1<<(*MasterSigIdx);

    ret = PRRateAvailable ? (uint32_t)(12+Cont) : (uint32_t)(10+Cont);

    Meas3Ranges_Count_MasterLong++;
  }
  else if ((*buf&0xc) == 0xc)
  {
    /*MasterDeltaL (long delta)*/
    uint8_t   BF1   = *buf;
    uint32_t  BF2   = *(uint32_t*)(buf+1);
    uint32_t  PR    = (((uint32_t)(BF1>>4)<<13) | (BF2&0x1fff));
    uint32_t  CN0   = (BF2>>13)&0x7;
    uint32_t  CmC   = BF2>>16;
    const MeasSet_t* MeasSetMasterRef;

    *MasterSigIdx = Ref->SigIdx[SatSys][SatIdx][0];
    Wavelength_m = sbfread_Meas3_GetWavelength_m(Meas3SigIdx2SignalType, SatSys, *MasterSigIdx, GLOfn);

    MeasSetMasterRef       = &(Ref->MeasSet[SatSys][SatIdx][*MasterSigIdx]);

    MeasSet->signalType    = sbfread_Meas3_SigIdx2SignalType(Meas3SigIdx2SignalType, SatSys, *MasterSigIdx);
    MeasSet->flags         = MeasSetMasterRef->flags & (MEASFLAG_VALIDITY | MEASFLAG_HALFCYCLEAMBIGUITY);
    MeasSet->PLLTimer_ms   = MeasSetMasterRef->PLLTimer_ms;
    
    MeasSet->PR_m          = MeasSetMasterRef->PR_m + ((int64_t)Ref->PRRate_64mm_s[SatSys][SatIdx]*64*(int32_t)TimeSinceRefEpoch_ms/1000)*.001 + (double)PR*.001 - 65.536;
    MeasSet->L_cycles      = CmC == 0 ? F64_NOTVALID : (MeasSet->PR_m-MeasSetMasterRef->PR_m)/Wavelength_m + MeasSetMasterRef->L_cycles - 32.768 + (double)CmC*.001;
    MeasSet->CN0_dBHz      = MeasSetMasterRef->CN0_dBHz - 4.0F + (float)CN0;

    *PRRate_64mm_s         = 0;
    *SlaveSigMask          = Ref->SlaveSigMask[SatSys][SatIdx];

    ret = 5;

    Meas3Ranges_Count_MasterDelta++;
  }
  else
  {
    /*MasterDeltaS (short delta)*/
    uint32_t  BF1   = *(uint32_t*)buf;
    uint32_t  PR    = (BF1>>4)&0x3fff;
    uint32_t  CmC   = (BF1>>18)&0x3fff;
    uint32_t  CN0   = (BF1>>2)&0x3;
    const MeasSet_t* MeasSetMasterRef;

    *MasterSigIdx = Ref->SigIdx[SatSys][SatIdx][0];
    Wavelength_m = sbfread_Meas3_GetWavelength_m(Meas3SigIdx2SignalType, SatSys, *MasterSigIdx, GLOfn);

    MeasSetMasterRef = &(Ref->MeasSet[SatSys][SatIdx][*MasterSigIdx]);
    
    MeasSet->signalType    = sbfread_Meas3_SigIdx2SignalType(Meas3SigIdx2SignalType, SatSys, *MasterSigIdx);
    MeasSet->flags         = MeasSetMasterRef->flags & (MEASFLAG_VALIDITY | MEASFLAG_HALFCYCLEAMBIGUITY);
    MeasSet->PLLTimer_ms   = MeasSetMasterRef->PLLTimer_ms;
    
    MeasSet->PR_m          = MeasSetMasterRef->PR_m + ((int64_t)Ref->PRRate_64mm_s[SatSys][SatIdx]*64*(int32_t)TimeSinceRefEpoch_ms/1000)*.001 + (double)PR*.001 - 8.192;
    MeasSet->L_cycles      = CmC == 0 ? F64_NOTVALID : (MeasSet->PR_m-MeasSetMasterRef->PR_m)/Wavelength_m + MeasSetMasterRef->L_cycles - 8.192 + (double)CmC*.001;
    MeasSet->CN0_dBHz      = MeasSetMasterRef->CN0_dBHz - 1.0F + (float)CN0;

    *PRRate_64mm_s         = 0;
    *SlaveSigMask          = Ref->SlaveSigMask[SatSys][SatIdx];

    ret = 4;

    Meas3Ranges_Count_MasterDelta++;
  }

  return ret;
  
 EXIT_INVALIDFORMAT:
  return 0;
}



/*---------------------------------------------------------------------------*/
static uint32_t
sbfread_Meas3_DecodeSlave(const uint8_t* const buf,
                          SignalType_t     Meas3SigIdx2SignalType[MEAS3_SYS_MAX][MEAS3_SIG_MAX],
                          Meas3SatSystem_t SatSys,
                          uint32_t         SigIdx,
                          int              GLOfn,
                          /*@out@*/MeasSet_t*   MeasSet,
                          const MeasSet_t* const MeasSetMaster,
                          uint32_t                       MasterSigIdx,
                          const MeasSet_t* const MeasSetMasterRef,
                          const MeasSet_t* const MeasSetSlaveRef)
{
  uint32_t ret;
  double WavelengthMaster_m = sbfread_Meas3_GetWavelength_m(Meas3SigIdx2SignalType, SatSys, MasterSigIdx, GLOfn);
  double WavelengthSlave_m  = sbfread_Meas3_GetWavelength_m(Meas3SigIdx2SignalType, SatSys, SigIdx, GLOfn);

  memset (MeasSet, 0, sizeof(*MeasSet));

  MeasSet->flags         = (uint8_t)MEASFLAG_VALIDITY;
  MeasSet->signalType    = sbfread_Meas3_SigIdx2SignalType(Meas3SigIdx2SignalType, SatSys, SigIdx);

  if ((*buf&1) == 1)
  {
    /*SlaveShort*/
    uint32_t BF1    = *(uint32_t*)buf;
    uint8_t  BF2    = *(buf+4);
    uint32_t CmCres = (BF1>>1)&0xffff;
    uint32_t PRrel  = BF1>>17;
    uint32_t LTI3   = BF2&0x7;
    uint32_t CN0    = BF2>>3;
    
    if (WavelengthMaster_m < WavelengthSlave_m)
      MeasSet->PR_m = MeasSetMaster->PR_m + PRrel*.001 - 10;
    else
      MeasSet->PR_m = MeasSetMaster->PR_m - PRrel*.001 + 10;

    MeasSet->L_cycles    = (CmCres == 0 ?
                            F64_NOTVALID :
                            (MeasSet->PR_m/WavelengthSlave_m +
                             (MeasSetMaster->L_cycles-MeasSetMaster->PR_m/WavelengthMaster_m)*WavelengthSlave_m/WavelengthMaster_m
                             - 32.768 + CmCres*.001));
    
    if (sbfread_Meas3_SigIdx2SignalType(Meas3SigIdx2SignalType, SatSys, SigIdx) == SIG_GPSL1P ||
        sbfread_Meas3_SigIdx2SignalType(Meas3SigIdx2SignalType, SatSys, SigIdx) == SIG_GPSL2P)
      MeasSet->CN0_dBHz    = MeasSetMaster->CN0_dBHz - 3.0F - (float)CN0;
    else
      MeasSet->CN0_dBHz    = (float)CN0+24.0F;
    
    MeasSet->PLLTimer_ms = LTItoPLLTimer_ms[LTI3];
    if (LTI3 == 0) MeasSet->flags |= (uint8_t)MEASFLAG_HALFCYCLEAMBIGUITY;
    
    ret = 5;
    Meas3Ranges_Count_SlaveShort++;
  }
  else if ((*buf&3) == 0)
  {
    /*SlaveLong*/
    uint32_t BF1      = *(uint32_t*)buf;
    uint16_t PRLSBrel = *(uint16_t*)(buf+4);
    uint8_t  BF3      = *(buf+6);
    uint32_t CmC      = (BF1>>2)&0x3fffff;
    uint32_t LTI4     = (BF1>>24)&0xf;
    uint32_t PRMSBrel = (BF1>>28)&0x7;
    uint32_t CN0      = BF3&0x3f;

    MeasSet->PR_m        = MeasSetMaster->PR_m + (PRMSBrel*65536+PRLSBrel)*.001 - 262.144;

    MeasSet->L_cycles    = CmC == 0 ? F64_NOTVALID : MeasSet->PR_m/WavelengthSlave_m - 2097.152 + CmC*0.001;

    if (sbfread_Meas3_SigIdx2SignalType(Meas3SigIdx2SignalType, SatSys, SigIdx) == SIG_GPSL1P ||
        sbfread_Meas3_SigIdx2SignalType(Meas3SigIdx2SignalType, SatSys, SigIdx) == SIG_GPSL2P)
      MeasSet->CN0_dBHz    = (float)CN0;
    else
      MeasSet->CN0_dBHz    = (float)CN0+10.0F;

    MeasSet->PLLTimer_ms = LTItoPLLTimer_ms[LTI4];
    if (LTI4 == 0) MeasSet->flags |= (uint8_t)MEASFLAG_HALFCYCLEAMBIGUITY;
    
    ret = 7;
    Meas3Ranges_Count_SlaveLong++;
  }
  else
  {
    /*SlaveDelta*/
    uint16_t BF1      = *(uint16_t*)buf;
    uint8_t  dCarrier = *(uint8_t*)(buf+2);
    uint32_t dPR      = (BF1>>2)&0xfff;
    uint32_t CN0      = BF1>>14;

    MeasSet->flags       = MeasSetSlaveRef->flags & (MEASFLAG_VALIDITY | MEASFLAG_HALFCYCLEAMBIGUITY);
    MeasSet->L_cycles    = (MeasSetSlaveRef->L_cycles
                            + (MeasSetMaster->L_cycles-MeasSetMasterRef->L_cycles)*WavelengthMaster_m/WavelengthSlave_m
                            - 0.128 + dCarrier*0.001);
    MeasSet->PR_m        = (MeasSetSlaveRef->PR_m + (MeasSet->L_cycles-MeasSetSlaveRef->L_cycles)*WavelengthSlave_m
                            - 2.048 + dPR*0.001);
    MeasSet->CN0_dBHz    = MeasSetSlaveRef->CN0_dBHz - 2.0F + (float)CN0;
    MeasSet->PLLTimer_ms = MeasSetSlaveRef->PLLTimer_ms;
    
    ret = 3;
    Meas3Ranges_Count_SlaveDelta++;
  }

  return ret;
}


/*---------------------------------------------------------------------------*/
static int32_t
sbfread_Meas3_GetPRRate_mm_s(/*@null@*/const Meas3Doppler_1_t *sbfMeas3Doppler,
                             uint32_t       *DopplerIdx)
{
  int32_t PRR_mm_s = I32_NOTVALID;

  if (sbfMeas3Doppler != NULL)
  {
    uint32_t Value = *((uint32_t*)(sbfMeas3Doppler->Data + *DopplerIdx));
    int32_t  AbsPRR_mm_s;

    if ((Value&2) == 0)
    {
      /* 1 byte */
      AbsPRR_mm_s = (int32_t)((Value&0xff)>>2);
      *DopplerIdx += 1;
    }
    else if ((Value&6) == 2)
    {
      /* 2 bytes */
      AbsPRR_mm_s = (int32_t)((Value&0xffff)>>3);
      *DopplerIdx += 2;
    }
    else if ((Value&0xe) == 6)
    {
      /* 3 bytes */
      AbsPRR_mm_s = (int32_t)((Value&0xffffff)>>4);
      *DopplerIdx += 3;
    }
    else
    {
      /* 4 bytes */
      AbsPRR_mm_s = (int32_t)(Value>>4);
      *DopplerIdx += 4;
    }
    
    if ((Value&1) == 0)
      PRR_mm_s =  AbsPRR_mm_s;
    else
      PRR_mm_s = -AbsPRR_mm_s;
  }
  
  return PRR_mm_s;
}


/*---------------------------------------------------------------------------*/
static void
sbfread_Meas3_AddMasterDoppler(/*@out@*/MeasSet_t*   MeasSet,
                               /*@null@*/const Meas3Doppler_1_t *sbfMeas3Doppler,
                               double         Wavelength_m,
                               int16_t        RefPRRate_64mm_s,
                               uint32_t      *DopplerIdx)
{
  int32_t PRR_mm_s;

  PRR_mm_s = sbfread_Meas3_GetPRRate_mm_s(sbfMeas3Doppler, DopplerIdx);
  
  if (PRR_mm_s == I32_NOTVALID || PRR_mm_s == (int32_t)-268435455)
  {
    MeasSet->doppler_Hz = F32_NOTVALID;
  }
  else
  {
    MeasSet->doppler_Hz = (float)(-(PRR_mm_s+(int32_t)RefPRRate_64mm_s*64) * 0.001 / Wavelength_m);
  }
}


/*---------------------------------------------------------------------------*/
static void
sbfread_Meas3_AddSlaveDoppler(/*@out@*/MeasSet_t*   MeasSet,
                              const MeasSet_t* const MeasSetMaster,
                              /*@null@*/const Meas3Doppler_1_t *sbfMeas3Doppler,
                              double         WavelengthMaster_m,
                              double         WavelengthSlave_m,
                              uint32_t      *DopplerIdx)
{
  int32_t PRR_mm_s;

  PRR_mm_s = sbfread_Meas3_GetPRRate_mm_s(sbfMeas3Doppler, DopplerIdx);

  if (PRR_mm_s == I32_NOTVALID || PRR_mm_s == (int32_t)-1048575)
  {
    MeasSet->doppler_Hz = F32_NOTVALID;
  }
  else
  {
    MeasSet->doppler_Hz = (float)((MeasSetMaster->doppler_Hz*WavelengthMaster_m*1000.0 - PRR_mm_s)*0.001 / WavelengthSlave_m);
  }
}


/*---------------------------------------------------------------------------*/
static void
sbfread_Meas3_AddPPInfo(/*@out@*/MeasSet_t*   MeasSet,
                        /*@null@*/const Meas3PP_1_t *sbfMeas3PP,
                        uint32_t      *PP1Idx,
                        uint32_t      *PP2Idx)
{
  MeasSet->rawCN0_dBHz = (uint8_t)(((int)MeasSet->CN0_dBHz/2)*2);

  if (sbfMeas3PP != NULL)
  {
    uint16_t dummy = sbfMeas3PP->Data[*PP1Idx/8] | sbfMeas3PP->Data[*PP1Idx/8+1]<<8;
    uint16_t val   = dummy >> (unsigned)(*PP1Idx%8);
    uint32_t start2= (sbfMeas3PP->Flags>>8)*4;

    MeasSet->flags     |= ((val&1) != 0 ? MEASFLAG_APMEINSYNC : 0);
    MeasSet->lockCount  = (val>>1)&0xf;

    *PP1Idx += 5;

    /* if there is a sub-block type 0 available (containing raw CN0 values), decode it as well. */
    if (start2 != 0 && (sbfMeas3PP->Data[start2]&0xf) == 0)
    {
      dummy = sbfMeas3PP->Data[*PP2Idx/8+start2+2] | sbfMeas3PP->Data[*PP2Idx/8+1+start2+2]<<8;
      val   = dummy >> (unsigned)(*PP2Idx%8);
      
      if ((val&1)==0)
      {
        /* the raw CN0 is the same as the standard CN0 */
        (*PP2Idx)++;
      }
      else
      {
        /* the raw CN0 is contained in bits 1-5 of val. */
        MeasSet->rawCN0_dBHz = (uint8_t)(((val>>1)&0x1f)*2);
        if (MeasSet->signalType != SIG_GPSL1P && MeasSet->signalType != SIG_GPSL2P) MeasSet->rawCN0_dBHz += 10;

        *PP2Idx+=6;
      }
    }
  }
}


/*---------------------------------------------------------------------------*/
static void
sbfread_Meas3_AddMPInfo(/*@out@*/MeasSet_t*   MeasSet,
                        /*@null@*/const Meas3MP_1_t *sbfMeas3MP,
                        uint32_t      *MPIdx)
{
  if (sbfMeas3MP != NULL)
  {
    uint32_t dummy = (sbfMeas3MP->Data[*MPIdx/8]       |
                      sbfMeas3MP->Data[*MPIdx/8+1]<<8  |
                      sbfMeas3MP->Data[*MPIdx/8+2]<<16 |
                      sbfMeas3MP->Data[*MPIdx/8+3]<<24  );
    uint32_t val   = dummy >> (unsigned)(*MPIdx%8);
    uint32_t type  = val&3;
    int c, l;

    switch (type)
    {
    case 0:
      MeasSet->MP_mm = 0;
      MeasSet->CarrierMP_1_512c = 0;
      *MPIdx += 2;
      break;

    case 1:
      c = (int)((val>>2)&0x7f);
      l = (int)((val>>9)&0x1f);

      MeasSet->MP_mm = (int16_t)(c*10);

      if (l < 16) MeasSet->CarrierMP_1_512c = (int8_t)l;
      else        MeasSet->CarrierMP_1_512c = (int8_t)(l-32);

      *MPIdx += 14;
      break;

    case 2:
      c = (int)((val>>2)&0x7f);
      l = (int)((val>>9)&0x1f);

      MeasSet->MP_mm = -(int16_t)(c*10);

      if (l < 16) MeasSet->CarrierMP_1_512c = (int8_t)l;
      else        MeasSet->CarrierMP_1_512c = (int8_t)(l-32);

      *MPIdx += 14;
      break;

    default:
      c = (int)((val>>2)&0x7ff);
      l = (int)((val>>13)&0xff);

      if (c < 1024) MeasSet->MP_mm = (int16_t)(c*10);
      else          MeasSet->MP_mm = (int16_t)((c-2048)*10);

      if (l < 128) MeasSet->CarrierMP_1_512c = (int8_t)l;
      else         MeasSet->CarrierMP_1_512c = (int8_t)(l-256);

      *MPIdx += 21;
      break;
    }
  }
  else
  {
    MeasSet->MP_mm = 0;
    MeasSet->CarrierMP_1_512c = 0;
  }

}

static const uint8_t SVIDBase[MEAS3_SYS_MAX]
= { (uint8_t)gpMINPRN,   /* GPS        */
    (uint8_t)glMINPRN,   /* GLO        */
    (uint8_t)galMINPRN,  /* GAL        */
    (uint8_t)cmpMINPRN,  /* BDS        */
    (uint8_t)raMINPRN,   /* SBAS       */
    (uint8_t)qzMINPRN,   /* QZS        */
    (uint8_t)irMINPRN    /* IRN        */
};


/*---------------------------------------------------------------------------*/
/*@-immediatetrans@*/
static MeasChannel_t*
sbfread_Meas3_GetMeasChannel(MeasEpoch_t         *MeasEpoch,
                             Meas3SatSystem_t     SatSys,
                             uint32_t             SatIdx)
{
  uint32_t i;
  
  for (i=0; i<MeasEpoch->nbrElements; i++)
  {
    if (MeasEpoch->channelData[i].PRN == (uint8_t)(SVIDBase[SatSys]+SatIdx)) return &(MeasEpoch->channelData[i]);
  }
  
  if ((int)MeasEpoch->nbrElements < NR_OF_LOGICALCHANNELS)
    MeasEpoch->nbrElements++;
  
  /* the channel number is just a counter */
  MeasEpoch->channelData[MeasEpoch->nbrElements-1].channel = MeasEpoch->nbrElements-1; 

  return &(MeasEpoch->channelData[MeasEpoch->nbrElements-1]);
}
/*@=immediatetrans@*/



/*---------------------------------------------------------------------------*/
static uint32_t
sbfread_Meas3_DecodeConstellation(const uint8_t *buf,
                                  /*@null@*/const Meas3CN0HiRes_1_t *sbfMeas3CN0HiRes,
                                  uint32_t      *CN0HiResIdx,
                                  /*@null@*/const Meas3Doppler_1_t *sbfMeas3Doppler,
                                  uint32_t      *DopplerIdx,
                                  /*@null@*/const Meas3PP_1_t *sbfMeas3PP,
                                  uint32_t      *PP1Idx,
                                  uint32_t      *PP2Idx,
                                  /*@null@*/const Meas3MP_1_t *sbfMeas3MP,
                                  uint32_t      *MPIdx,
                                  uint32_t       AntIdx,
                                  Meas3SatSystem_t  SatSys,
                                  MeasEpoch_t   *MeasEpoch,
                                  sbfread_Meas3_RefEpoch_t* RefEpoch,
                                  uint32_t       RefInterval_ms,
                                  bool           RefEpochContainsPRRate
#ifndef NO_DECRYPTION
                                  ,/*@null@*/decrypt_ctx_t *decryptctx
#endif
                                  )
{
  const uint8_t* start = buf;
  const uint8_t* SatDataBuf;
  uint32_t NSats = 0;
  uint32_t SatIdx;
  uint32_t SigIdx;
  uint32_t N = 0;
  uint32_t ii;
  uint64_t SatMask = 0;
  uint16_t BDSLongRange = 0;
  uint32_t Nb;
  uint32_t SigIdxMasterShort;
  bool     SigExcludedPresent;
  uint8_t  SigExcluded;
  uint32_t SatCnt = 0;
  uint8_t  GLOFnList[32];

  SignalType_t Meas3SigIdx2SignalType[MEAS3_SYS_MAX][MEAS3_SIG_MAX];

  memset (GLOFnList, 0, sizeof(GLOFnList));

  /* if first byte of M3SatData is zero, copy the M3SatData from the
     reference epoch */
  if (*buf == 0) SatDataBuf = RefEpoch->M3SatDataCopy[SatSys];
  else           SatDataBuf = buf;

  /* read BF1 */
  Nb = SatDataBuf[N]&0x7;
  SigIdxMasterShort = (*(SatDataBuf+N)>>3)&0xf;
  SigExcludedPresent= (*(SatDataBuf+N)>>7) != 0;
  N++;

  /* read SatMask */
  for (ii=0;ii<Nb;ii++)
  {
    SatMask |= (uint64_t)(*(SatDataBuf+N)) << (ii*8);
    NSats += bitcnt(*(SatDataBuf+N));
    N++;
  }

  /* read GLOFnList if applicable */
  if (SatSys == MEAS3_SYS_GLO)
  {
    memcpy(GLOFnList, SatDataBuf+N, (NSats+1)/2);
    N += (NSats+1)/2;
  } 
  
  /* read BDSLongRange if applicable */
  if (SatSys == MEAS3_SYS_BDS)
  {
    BDSLongRange = *(uint16_t*)(SatDataBuf+N);
    N+=2;
  }
  
  /* read the SigExcluded field if present */
  if (SigExcludedPresent)
  {
    SigExcluded = *(SatDataBuf+N);
    N++;
  }
  else
  {
    SigExcluded = 0;
  }

  /* go to the first M3Master subblock */
  if (*buf == 0) buf += 1;
  else           buf += N;
  
  /*@-mayaliasunique@*/
  /* if this is a reference epoch, remember this M3SatData */
  if (MeasEpoch->TOW_ms%RefInterval_ms == 0)
  {
    memcpy(RefEpoch->M3SatDataCopy[SatSys], start, N);
  }
  /*@=mayaliasunique@*/

  sbfread_PrepareSigTable(Meas3SigIdx2SignalType, SatSys, SigExcluded);

  /* decoder all sats from this constellation */
  for (SatIdx=0; SatIdx<MEAS3_SAT_MAX && SatCnt < NSats; SatIdx++)
  {
    if ((SatMask&(1ULL<<SatIdx)) != 0)
    {
      MeasSet_t          MeasSetMaster;
      uint32_t           MasterSigIdx;
      uint32_t           SlaveSigMask = 0;
      int16_t            PRRate_64mm_s = 0;
      int                GLOfn = (int)((GLOFnList[SatCnt/2]>>(4*(SatCnt%2)))&0xf)-8;
      float              CN0Master_HiRes_dBHz = 0.0F;
      int                SlaveCnt = 0;
      uint32_t           MasterSize;
      MeasChannel_t*     MeasChannel = sbfread_Meas3_GetMeasChannel(MeasEpoch, SatSys, SatIdx);

      MeasChannel->PRN     = (uint8_t)(SVIDBase[SatSys] + SatIdx);
      MeasChannel->fnPlus8 = (uint8_t)(GLOfn+8);

      /* there is always at least a master measurement set available for each satellite.  Decode it now */
      MasterSize = sbfread_Meas3_DecodeMaster(buf,
                                              Meas3SigIdx2SignalType,
                                              SatSys,
                                              SatIdx,
                                              GLOfn,
                                              (BDSLongRange&(1<<SatCnt)) != 0 ? 34e6 : PRBase_m[SatSys],
                                              SigIdxMasterShort,
                                              RefEpoch,
                                              MeasEpoch->TOW_ms%RefInterval_ms,
                                              &MeasSetMaster,
                                              &MasterSigIdx,
                                              &SlaveSigMask,
                                              RefEpochContainsPRRate,
                                              &PRRate_64mm_s
#ifndef NO_DECRYPTION
                                              ,decryptctx
#endif
                                              );
      
      if (MasterSize == 0) goto EXIT_INVALIDFORMAT;
      
      buf += MasterSize;

      /* keep reference measurement to decode the delta measurements */
      if (MeasEpoch->TOW_ms % RefInterval_ms == 0)
      {
        RefEpoch->SigIdx[SatSys][SatIdx][0]             = (uint8_t)MasterSigIdx;
        RefEpoch->SlaveSigMask[SatSys][SatIdx]          = SlaveSigMask;
        RefEpoch->PRRate_64mm_s[SatSys][SatIdx]         = PRRate_64mm_s;
        RefEpoch->MeasSet[SatSys][SatIdx][MasterSigIdx] = MeasSetMaster;
      }
      if (MeasSetMaster.PLLTimer_ms > RefEpoch->MeasSet[SatSys][SatIdx][MasterSigIdx].PLLTimer_ms)
      {
        RefEpoch->MeasSet[SatSys][SatIdx][MasterSigIdx].PLLTimer_ms = MeasSetMaster.PLLTimer_ms;
      }
      
      sbfread_Meas3_AddMasterDoppler(&MeasSetMaster, sbfMeas3Doppler, 
                                     sbfread_Meas3_GetWavelength_m(Meas3SigIdx2SignalType, SatSys, MasterSigIdx, GLOfn),
                                     RefEpoch->PRRate_64mm_s[SatSys][SatIdx],
                                     DopplerIdx);
      
      sbfread_Meas3_AddPPInfo(&MeasSetMaster, sbfMeas3PP, PP1Idx, PP2Idx);
      sbfread_Meas3_AddMPInfo(&MeasSetMaster, sbfMeas3MP, MPIdx);

      if (sbfMeas3CN0HiRes != NULL)
      {
        /* remember the HiRes adjustment, but do not apply it yet (as
           we need the unadjusted master meas to decode the slave
           meas */
        CN0Master_HiRes_dBHz = (float)((sbfMeas3CN0HiRes->CN0HiRes[*CN0HiResIdx/2]>>((*CN0HiResIdx%2)*4))&0xf)*.0625F - 0.5F;
        (*CN0HiResIdx)++;
      }

      /* decode all slave data */
      for (SigIdx=1; SigIdx<MEAS3_SIG_MAX && SlaveSigMask != 0; SigIdx++)
      {
        if ((SlaveSigMask&(1<<SigIdx)) != 0)
        {
          MeasSet_t MeasSetSlave;

          buf += sbfread_Meas3_DecodeSlave(buf,
                                           Meas3SigIdx2SignalType,
                                           SatSys,
                                           SigIdx,
                                           GLOfn,
                                           &MeasSetSlave,
                                           &MeasSetMaster,
                                           MasterSigIdx,
                                           &(RefEpoch->MeasSet[SatSys][SatIdx][RefEpoch->SigIdx[SatSys][SatIdx][0]]),
                                           &(RefEpoch->MeasSet[SatSys][SatIdx][RefEpoch->SigIdx[SatSys][SatIdx][SlaveCnt+1]]));

          sbfread_Meas3_AddSlaveDoppler(&MeasSetSlave, &MeasSetMaster, sbfMeas3Doppler, 
                                        sbfread_Meas3_GetWavelength_m(Meas3SigIdx2SignalType, SatSys, MasterSigIdx, GLOfn),
                                        sbfread_Meas3_GetWavelength_m(Meas3SigIdx2SignalType, SatSys, SigIdx, GLOfn),
                                        DopplerIdx);

          sbfread_Meas3_AddPPInfo(&MeasSetSlave, sbfMeas3PP, PP1Idx, PP2Idx);
          sbfread_Meas3_AddMPInfo(&MeasSetSlave, sbfMeas3MP, MPIdx);

          /* keep reference measurement to decode the delta measurements */
          if (MeasEpoch->TOW_ms % RefInterval_ms == 0)
          {
            RefEpoch->SigIdx[SatSys][SatIdx][SlaveCnt+1] = (uint8_t)SigIdx;
            RefEpoch->MeasSet[SatSys][SatIdx][SigIdx]    = MeasSetSlave;
          }
          if (MeasSetSlave.PLLTimer_ms > RefEpoch->MeasSet[SatSys][SatIdx][SigIdx].PLLTimer_ms)
          {
            RefEpoch->MeasSet[SatSys][SatIdx][SigIdx].PLLTimer_ms = MeasSetSlave.PLLTimer_ms;
          }
          
          if (sbfMeas3CN0HiRes != NULL)
          {
            MeasSetSlave.CN0_dBHz += ((sbfMeas3CN0HiRes->CN0HiRes[*CN0HiResIdx/2]>>((*CN0HiResIdx%2)*4))&0xf)*.0625F - 0.5F;
            (*CN0HiResIdx)++;
          }
          
          if (SigIdx < MAX_NR_OF_SIGNALS_PER_SATELLITE)
            MeasChannel->measSet[AntIdx][SigIdx] = MeasSetSlave;
          
          SlaveCnt++;
          
          /* delete this bit of the mask */
          SlaveSigMask ^= (1<<SigIdx);
        }
      }

      /* now it is time to apply the C/N0 adjustment and to store the master MeasSet*/
      MeasSetMaster.CN0_dBHz += CN0Master_HiRes_dBHz;

      if (MasterSigIdx < MAX_NR_OF_SIGNALS_PER_SATELLITE)
        MeasChannel->measSet[AntIdx][MasterSigIdx] = MeasSetMaster;
      
      SatCnt++;

      /* when decoding files containing signals not known yet, the signal
         type of those unknown signals is set to "SIG_LAST". In that case,
         it is safer to invalidate the whole measurement set. */
      for (SigIdx=0; SigIdx<MAX_NR_OF_SIGNALS_PER_SATELLITE; SigIdx++)
      {
        if (MeasChannel->measSet[AntIdx][SigIdx].signalType >= SIG_LAST) MeasChannel->measSet[AntIdx][SigIdx].flags = 0;
      }
    }
  }

  return (uint32_t)(buf-start);

 EXIT_INVALIDFORMAT:
  return (uint32_t)0;
}


/*---------------------------------------------------------------------------*/
static void
sbfread_Meas3_Decode(
  const Meas3Ranges_1_t    Meas3Ranges[NR_OF_ANTENNAS],
  const Meas3Doppler_1_t   Meas3Doppler[NR_OF_ANTENNAS],
  const Meas3CN0HiRes_1_t  Meas3CN0HiRes[NR_OF_ANTENNAS],
  const Meas3PP_1_t        Meas3PP[NR_OF_ANTENNAS],
  const Meas3MP_1_t        Meas3MP[NR_OF_ANTENNAS],
  sbfread_Meas3_RefEpoch_t RefEpoch[NR_OF_ANTENNAS],
  MeasEpoch_t*             MeasEpoch
#ifndef NO_DECRYPTION
  ,SBFDecrypt_t*           decrypt
#endif
  )
{
  uint32_t AntIdx;

  /* First initialize all to 0 */
  memset (MeasEpoch, 0, sizeof(*MeasEpoch));

  /* decode the measurements from all antennas */
  for (AntIdx = 0; (int)AntIdx < NR_OF_ANTENNAS; AntIdx++)
  {
    const Meas3Ranges_1_t*   ThisMeas3Ranges   = Meas3Ranges[AntIdx].Header.ID != 0 ? Meas3Ranges+AntIdx : NULL;
    const Meas3Doppler_1_t*  ThisMeas3Doppler  = Meas3Doppler[AntIdx].Header.ID != 0 ? Meas3Doppler+AntIdx : NULL;
    const Meas3CN0HiRes_1_t* ThisMeas3CN0HiRes = Meas3CN0HiRes[AntIdx].Header.ID != 0 ? Meas3CN0HiRes+AntIdx : NULL;
    const Meas3PP_1_t*       ThisMeas3PP       = Meas3PP[AntIdx].Header.ID != 0 ? Meas3PP+AntIdx : NULL;
    const Meas3MP_1_t*       ThisMeas3MP       = Meas3MP[AntIdx].Header.ID != 0 ? Meas3MP+AntIdx : NULL;

    /* at least the Meas3ranges block must be available, as this is
       the master meas3 block */
    if (ThisMeas3Ranges != NULL)
    {
      const uint8_t *buf;
      uint32_t SatSys;
      uint32_t CN0HiResIdx = 0;
      uint32_t DopplerIdx = 0;
      uint32_t PP1Idx = 0;
      uint32_t PP2Idx = 0;
      uint32_t MPIdx = 0;
      uint32_t RefEpochInterval_ms;

#ifndef NO_DECRYPTION
      decrypt_ctx_t decryptctx;
      bool          ApplyDescrambling = DecryptMeas3Init(&decryptctx, ThisMeas3Ranges, decrypt);
#endif

      /* decode the reference epoch interval */
      if      ((ThisMeas3Ranges->Misc>>4) == 0) RefEpochInterval_ms =     1;
      else if ((ThisMeas3Ranges->Misc>>4) == 1) RefEpochInterval_ms =   500;
      else if ((ThisMeas3Ranges->Misc>>4) == 2) RefEpochInterval_ms =  1000;
      else if ((ThisMeas3Ranges->Misc>>4) == 3) RefEpochInterval_ms =  2000;
      else if ((ThisMeas3Ranges->Misc>>4) == 4) RefEpochInterval_ms =  5000;
      else if ((ThisMeas3Ranges->Misc>>4) == 5) RefEpochInterval_ms = 10000;
      else if ((ThisMeas3Ranges->Misc>>4) == 6) RefEpochInterval_ms = 15000;
      else if ((ThisMeas3Ranges->Misc>>4) == 7) RefEpochInterval_ms = 30000;
      else if ((ThisMeas3Ranges->Misc>>4) == 8) RefEpochInterval_ms = 60000;
      else if ((ThisMeas3Ranges->Misc>>4) == 9) RefEpochInterval_ms =120000;
      else                                      RefEpochInterval_ms =     1;

      if (ThisMeas3Ranges->CumClkJumps >= 128)
        MeasEpoch->totalClockJump_ms = (int32_t)ThisMeas3Ranges->CumClkJumps - 256;
      else
        MeasEpoch->totalClockJump_ms = (int32_t)ThisMeas3Ranges->CumClkJumps;

      /* set the time (TOW/WN) */
      MeasEpoch->TOW_ms = ThisMeas3Ranges->TOW;
      MeasEpoch->WNc    = ThisMeas3Ranges->WNc;

      MeasEpoch->rxTOWStatus = RXTOWSTATUS_NOTSET;
      if (MeasEpoch->TOW_ms != U32_NOTVALID)
      {
        MeasEpoch->rxTOWStatus = RXTOWSTATUS_COARSE;
      }
      if (MeasEpoch->WNc != U16_NOTVALID)
      {
        MeasEpoch->rxTOWStatus = RXTOWSTATUS_PRECISE;
        MeasEpoch->IsWNcSet = true;
      }
      else
      {
        MeasEpoch->IsWNcSet = false;
      }
      
      MeasEpoch->dopplerVarFactor = DEFAULT_DOPPLER_VARIANCE_FACTOR;
      
      /* Copying bits of common flags; some occupy same positions in
         SBF and in MeasEpoch_t, others don't */
      MeasEpoch->commonFlags = 0;
      
      /* Bit 0 : multipath mitigation */
      if ( (ThisMeas3Ranges->CommonFlags & SBF_MEASEPOCH_2_COMMONFLAG_MULTIPATHMITIGATION) != 0)
      {
        MeasEpoch->commonFlags |=  COMMONFLAG_MULTIPATHMITIGATION;
      }
      /* Bit 1 : code smoothing */
      if ( (ThisMeas3Ranges->CommonFlags & SBF_MEASEPOCH_2_COMMONFLAG_ATLEASTONESMOOTHING) != 0)
      {
        MeasEpoch->commonFlags |=  COMMONFLAG_ATLEASTONESMOOTHING;
      }
      /* Bit 3 : clock steering */
      if ( (ThisMeas3Ranges->CommonFlags & SBF_MEASEPOCH_2_COMMONFLAG_CLOCKSTEERINGACTIVE) != 0)
      {
        MeasEpoch->commonFlags |=  COMMONFLAG_CLOCKSTEERINGACTIVE;
      }
      /* Bit 5 : high-dynamics mode */
      if ( (ThisMeas3Ranges->CommonFlags & SBF_MEASEPOCH_2_COMMONFLAG_HIGHDYNAMICSMODE) != 0)
      {
        MeasEpoch->commonFlags |=  COMMONFLAG_HIGHDYNAMICSMODE;
      }
      /* Bits 6,7 Reserved */
      

      buf = &(ThisMeas3Ranges->Reserved) /*+ ThisMeas3Ranges->Reserved*/ + 1;

      /* if this is a reference epoch, clean up all past reference epoch
         data for this antenna */
      if ((MeasEpoch->TOW_ms%RefEpochInterval_ms) == 0)
      {
        memset(&(RefEpoch[AntIdx]), 0, sizeof(sbfread_Meas3_RefEpoch_t));
        RefEpoch[AntIdx].TOW_ms = MeasEpoch->TOW_ms;
      }
  
      /* only decode the data if this is a reference epoch, or if the
         matching reference epoch is available */
      if (((MeasEpoch->TOW_ms%RefEpochInterval_ms) == 0 ||
           RefEpoch[AntIdx].TOW_ms == (MeasEpoch->TOW_ms/RefEpochInterval_ms)*RefEpochInterval_ms))
      {
        for (SatSys=MEAS3_SYS_GPS; SatSys<MEAS3_SYS_MAX; SatSys++)
        {
          if ((ThisMeas3Ranges->Constellations & (1<<SatSys)) != 0)
          {
            uint32_t N;
            /*@-compdef@*/
            N = sbfread_Meas3_DecodeConstellation(buf,
                                                  ThisMeas3CN0HiRes, &CN0HiResIdx,
                                                  ThisMeas3Doppler, &DopplerIdx,
                                                  ThisMeas3PP, &PP1Idx, &PP2Idx,
                                                  ThisMeas3MP, &MPIdx,
                                                  AntIdx, (Meas3SatSystem_t)SatSys, MeasEpoch, 
                                                  &(RefEpoch[AntIdx]),
                                                  RefEpochInterval_ms,
                                                  (ThisMeas3Ranges->Misc&8) != 0   /* PRR availability */
#ifndef NO_DECRYPTION
                                                  ,ApplyDescrambling ? &decryptctx : NULL
#endif
                                                  );
            if (N == 0) goto EXIT_INVALIDFORMAT;
            
            buf+=N;
            
            /*@=compdef@*/
          }
        }
      }
    }
  }
  return;
  
 EXIT_INVALIDFORMAT:
  /* something wrong, discard all data */
  memset (MeasEpoch, 0, sizeof(*MeasEpoch));

}


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*--- FUNCTIONS TO DECODE A MEASEPOCH SBF BLOCK -----------------------------*/
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
static int32_t sbfread_ExtendSignBit(uint32_t x, uint32_t N)
/* extend the sign bit of a 2's complement N-bit integer
 *
 * Arguments:
 *   x:  the N-bit integer in two's complement
 *
 *   N:  the size of the integer in bits
*/
{
  if ((x & (1<<(N-1))) == 0) {
    /* the sign bit is 0, clear all the bits that are not part of a
       N-bit integer */
    return x & ((1<<N)-1);
  } else {
    /* the sign bit is 1, set all the bits that are not part of a
       N-bit integer */
    return x | (~((1<<N)-1));
  }
}


static double  PR_Type1;
static double  Doppler_Type1;
static uint8_t Type2Cnt;
static double  Wavelength1;

/*---------------------------------------------------------------------------*/
static void
GetObsFromType1(MeasEpoch_2_t           *MeasEpoch,
                MeasEpochChannelType1_t *Type1SubBlock,
                SignalType_t            *SignalType,
                uint32_t                *AntIdx,
                double    *PR,
                double    *Carrier,
                double    *Doppler,
                float     *CN0,
                uint32_t  *LockTime,
                bool      *HCF)
/* Extract measurements from a Type-1 sub-block.
 *
 * Arguments:
 *
 *  *MeasEpoch: a pointer to a MeasEpoch_2 or GenMeasEpoch_1 SBF block
 *
 *  *Type1SubBlock: a pointer to a type-1 sub-block in the
 *        MeasEpoch. The data will be extracted from this type-1
 *        sub-block.
 *
 *  *SignalType:the signal type of the type-1 sub-block
 *
 *  *AntIdx:    the antenna index (starting from 0 for the main antenna)
 *
 *  *PR:        the extracted pseudorange, in meters, or F64_NOTVALID
 *
 *  *Carrier:   the extracted carrier phase, in cycles, or F64_NOTVALID
 *
 *  *Doppler:   the extracted Doppler, in Hz, or F64_NOTVALID
 *
 *  *CN0:       the extracted C/N0, in dB-Hz, or F32_NOTVALID
 *
 *  *LockTime:  the extracted lock time, in seconds, or 0xffffffff
 *
 *  *HCF:       half-cycle flag (set when the half cycle ambiguity is not resolved)
 *
 *
 *  Note: the data in the subsequent type-2 sub-blocks can be read by
 *  the function GetNextObsFromType2().
 */
{
  *AntIdx = Type1SubBlock->Type>>5;

  if ((SBF_ID_TO_NUMBER(MeasEpoch->Header.ID) == sbfnr_GenMeasEpoch_1) &&
      (Type1SubBlock->SVID >= 120UL && Type1SubBlock->SVID <= 140UL))
  {
    *SignalType = SIG_SBSL1CA;
  }
  else
  {
    *SignalType = Type1SubBlock->Type&0x1f;
    if (*SignalType == 31) *SignalType = (Type1SubBlock->ObsInfo>>3)+32;
  }

  /* compute the carrier wavelength */
  Wavelength1 = GetWavelength_m(*SignalType, (int)(Type1SubBlock->ObsInfo>>3)-8);

  /* pseudorange */
  if ( ((Type1SubBlock->Misc&0xf) == 0) && (Type1SubBlock->CodeLSB == 0) ) {
    *PR = F64_NOTVALID;
  } else {
    *PR = (Type1SubBlock->Misc&0xf)*4294967.296+Type1SubBlock->CodeLSB*0.001;
  }
  PR_Type1 = *PR;

  /* Doppler */
  if (Type1SubBlock->Doppler == (int32_t)0x80000000) {
    *Doppler    = F64_NOTVALID;
  } else {
    *Doppler    = Type1SubBlock->Doppler*0.0001;
  }
  Doppler_Type1 = *Doppler;

  /* carrier phase */
  if ((Type1SubBlock->CarrierLSB == 0)  &&
      (Type1SubBlock->CarrierMSB == (int8_t)0x80)){
    *Carrier = F64_NOTVALID;
  } else {
    *Carrier = *PR/Wavelength1
      +(Type1SubBlock->CarrierMSB*65536+Type1SubBlock->CarrierLSB)/1000.0;
  }

  /* CN0 */
  if (Type1SubBlock->CN0 == (uint8_t)255) {
    *CN0 = F32_NOTVALID;
  } else {
    if ( (*SignalType != SIG_GPSL1P)   &&
         (*SignalType != SIG_GPSL2P)  ){
      *CN0      =  (float)(Type1SubBlock->CN0/4.0+10.0);
    } else {
      *CN0      =  (float)(Type1SubBlock->CN0/4.0);
    }
  }

  if (Type1SubBlock->LockTime == 65535) {
    *LockTime = 0xffffffff;
  } else {
    *LockTime = Type1SubBlock->LockTime;
  }
  
  /* in SBF 2.0, if the half-cycle ambiguity flag is set, report it */
  *HCF = ((SBF_ID_TO_NUMBER(MeasEpoch->Header.ID) == sbfnr_MeasEpoch_2) && 
          ((Type1SubBlock->ObsInfo&4) != 0));
          
  Type2Cnt = 0;
}


/*---------------------------------------------------------------------------*/
static void
GetNextObsFromType2(MeasEpoch_2_t           *MeasEpoch,
                    MeasEpochChannelType1_t *Type1SubBlock,
                    SignalType_t            *SignalType,
                    uint32_t                *AntIdx,
                    double    *PR,
                    double    *Carrier,
                    double    *Doppler,
                    float     *CN0,
                    uint32_t  *LockTime,
                    bool      *HCF)
/* Extract measurements from the next type-2 sub-block (if any).  The
 * function GetObsFromType1() must be called prior to calling this
 * function.
 *
 * Arguments:
 *  *MeasEpoch: a pointer to a MeasEpoch_2 or GenMeasEpoch_1 SBF block
 *
 *  *Type1SubBlock: a pointer to a type-1 sub-block in the
 *        MeasEpoch. The data will be extracted from one of the type-2 
 *        sub-blocks depending on this type-1 sub-block.
 *
 *  *SignalType:the signal type of the type-1 sub-block
 *
 *  *AntIdx:    the antenna index (starting from 0 for the main antenna)
 *
 *  *PR:        the extracted pseudorange, in meters, or F64_NOTVALID
 *
 *  *Carrier:   the extracted carrier phase, in cycles, or F64_NOTVALID
 *
 *  *Doppler:   the extracted Doppler, in Hz, or F64_NOTVALID
 *
 *  *CN0:       the extracted C/N0, in dB-Hz, or F32_NOTVALID
 *
 *  *LockTime:  the extracted lock time, in seconds, or 0xffffffff
 *
 *  *HCF:       half-cycle flag (set when the half cycle ambiguity is not resolved)
 *
 */
{
  /* if there are still type-2 sub-blocks to read, proceed*/
  if (Type2Cnt < Type1SubBlock->N_Type2)
  {
    MeasEpochChannelType2_t *Type2SubBlock
      = (MeasEpochChannelType2_t*)
      ((uint8_t*)Type1SubBlock 
       + MeasEpoch->SB1Size + Type2Cnt*MeasEpoch->SB2Size);
    double Wavelength2;
    int32_t CodeOffsetMSB
      = sbfread_ExtendSignBit(Type2SubBlock->OffsetsMSB, 3);
    int32_t  DopplerOffsetMSB
      = sbfread_ExtendSignBit(Type2SubBlock->OffsetsMSB>>3, 5);

    Type2Cnt++;

    *AntIdx = Type2SubBlock->Type>>5;

    *SignalType = Type2SubBlock->Type&0x1f;
    if (*SignalType == 31) *SignalType = (Type2SubBlock->ObsInfo>>3)+32;

    Wavelength2 = GetWavelength_m(*SignalType, (int)(Type1SubBlock->ObsInfo>>3)-8);

    /* pseudorange.  Note: if this field is not set to its
       Do-Not-Use value, then the pseudorange in the type-1
       sub-block is necessarily valid, so we don't need to check it */
    if ((Type2SubBlock->CodeOffsetLSB == 0 ) &&
        (CodeOffsetMSB                == -4)){
      *PR = F64_NOTVALID;
    } else {
      *PR = PR_Type1+(CodeOffsetMSB*65536 + Type2SubBlock->CodeOffsetLSB)*1e-3;
    }
    
    /* carrier phase */
    if ((Type2SubBlock->CarrierLSB == 0)  &&
        (Type2SubBlock->CarrierMSB == (int8_t)0x80)){
      *Carrier = F64_NOTVALID;
    } else {
      *Carrier = *PR/Wavelength2
        +(Type2SubBlock->CarrierMSB*65536+Type2SubBlock->CarrierLSB)*0.001;
    }
    
    /* L2 Doppler */
    if ((Type2SubBlock->DopplerOffsetLSB == 0   ) &&
        (DopplerOffsetMSB                == -16)) {
      *Doppler = F64_NOTVALID;
    } else {
      *Doppler =
        Doppler_Type1*Wavelength1/Wavelength2
        +(DopplerOffsetMSB*65536+Type2SubBlock->DopplerOffsetLSB)*0.0001;
    }
    
    /* P2 CN0 */
    if (Type2SubBlock->CN0 == (uint8_t)255) {
      *CN0 = F32_NOTVALID;
    } else {
      if ( (*SignalType != SIG_GPSL1P) &&
           (*SignalType != SIG_GPSL2P) ){
        *CN0      =  (float)(Type2SubBlock->CN0/4.0+10.0);
      } else {
        *CN0      =  (float)(Type2SubBlock->CN0/4.0);
      }
    }

    if (Type2SubBlock->LockTime == 255) {
      *LockTime = 0xffffffff;
    } else {
      *LockTime = Type2SubBlock->LockTime;
    }
    
    /* in SBF 2.0, if the half-cycle ambiguity flag is set, discard
     * the carrier phase measurement */
    *HCF = ((SBF_ID_TO_NUMBER(MeasEpoch->Header.ID) == sbfnr_MeasEpoch_2) && 
            ((Type2SubBlock->ObsInfo&4)    != 0));
  } 
  else 
  {
    /* no more sub-blocks to be read.  Set all values to
       their "Do-Not-Use" value */
    *SignalType= 0;
    *AntIdx    = 0;
    *PR        = F64_NOTVALID;
    *Carrier   = F64_NOTVALID;
    *Doppler   = F64_NOTVALID;
    *CN0       = F32_NOTVALID;
    *LockTime  = 0xffffffff;
    *HCF       = false;
  }
  
}


/*---------------------------------------------------------------------------*/
uint32_t GetMeasEpochSVID(MeasEpochChannelType1_t *Type1SubBlock)
/* returns the SVID from a type-1 sub-block of a MeasEpoch */
{
  return (uint32_t)Type1SubBlock->SVID;
}

/*---------------------------------------------------------------------------*/
uint32_t GetMeasEpochRXChannel(MeasEpochChannelType1_t *Type1SubBlock)
/* returns the logical receiver channel ID from a type-1 sub-block of a
 * MeasEpoch */
{
  return (uint32_t)Type1SubBlock->RXChannel;
}




#ifndef getSigIdxInConstellation
static const int8_t DefaultSigIdxInConstellation[SIG_LAST]
= {         0,  /* SIG_GPSL1CA  */ 
            3,  /* SIG_GPSL1P   */ 
            4,  /* SIG_GPSL2P   */ 
            1,  /* SIG_GPSL2C   */ 
            2,  /* SIG_GPSL5    */ 
            5,  /* SIG_GPSL1C   */ 
            0,  /* SIG_QZSL1CA  */ 
            1,  /* SIG_QZSL2C   */ 

            0,  /* SIG_GLOL1CA  */ 
            2,  /* SIG_GLOL1P   */ 
            3,  /* SIG_GLOL2P   */ 
            1,  /* SIG_GLOL2CA  */ 
            4,  /* SIG_GLOL3    */ 
            3,  /* SIG_BDSB1C   */ 
            4,  /* SIG_BDSB2a   */ 
            0,  /* SIG_IRNL5    */ 

            5,  /* SIG_GALE1A   */ 
            0,  /* SIG_GALE1BC  */ 
            6,  /* SIG_GALE6A   */ 
            3,  /* SIG_GALE6BC  */ 
            1,  /* SIG_GALE5a   */ 
            2,  /* SIG_GALE5b   */ 
            4,  /* SIG_GALE5    */ 
            0,  /* SIG_MSS      */ 

            0,  /* SIG_SBSL1CA  */   
            1,  /* SIG_SBSL5    */
            2,  /* SIG_QZSL5    */ 
            3,  /* SIG_QZSL6    */
            0,  /* SIG_BDSB1I   */ 
            1,  /* SIG_BDSB2I   */ 
            2,  /* SIG_BDSB3    */ 
           -1,  /* reserved     */

            4,  /* SIG_QZSL1C   */
            5,  /* SIG_QZSL1S   */
   };
#endif

/*---------------------------------------------------------------------------*/
static bool 
sbfread_AddMeasSet(MeasChannel_t * trackChan,
                   SignalType_t    SignalType,
                   uint32_t        AntIdx,
                   double          PR_m,
                   double          Carrier_cycles,
                   double          Doppler_Hz,
                   float           CN0_dBHz,
                   uint32_t        LockTime_s,
                   bool            HCF)
{
  bool         ret = false;
#ifdef getSigIdxInConstellation
  int          sigIdx = getSigIdxInConstellation(SignalType);
#else
  int          sigIdx = (int)DefaultSigIdxInConstellation[SignalType];
#endif

  /*@-realcompare@*/
  if ((int)AntIdx<NR_OF_ANTENNAS &&
      sigIdx >= 0 && sigIdx < MAX_NR_OF_SIGNALS_PER_SATELLITE)
  {
    MeasSet_t     *measSet = &(trackChan->measSet[AntIdx][sigIdx]);

    measSet->signalType  = SignalType;
    measSet->PR_m        = PR_m;
    measSet->L_cycles    = Carrier_cycles;
    measSet->doppler_Hz  = (float)Doppler_Hz;
    measSet->CN0_dBHz    = CN0_dBHz;
    measSet->rawCN0_dBHz = (uint8_t)(((int)measSet->CN0_dBHz/2)*2);
    measSet->flags       = (uint8_t)(MEASFLAG_VALIDITY |
                                     (HCF ? MEASFLAG_HALFCYCLEAMBIGUITY : 0));

    if      (LockTime_s == 0xffffffff)  measSet->PLLTimer_ms = 0;
    else if (LockTime_s == 0)           measSet->PLLTimer_ms = 10; /* assume a short non-zero lock time */
    else                                measSet->PLLTimer_ms = LockTime_s*1000;

    /*@=realcompare@*/

    ret = true;
    
  }
  
  return ret;
}


/*---------------------------------------------------------------------------*/
void sbfread_MeasEpoch_Decode(MeasEpoch_2_t       *sbfMeasEpoch,
                              MeasEpoch_t         *trackMeasEpoch )
{
  uint32_t         chNR = 0;
  /*@null@*/ /*@temp@*/ MeasEpochChannelType1_t* Type1SubBlock;

  /* Initialize all to 0 */
  memset (trackMeasEpoch, 0, sizeof(*trackMeasEpoch));
  
  /* as of revision 1 of that block, the exact total clock jump is
     available from the block (8LSB only). */
  if (SBF_ID_TO_REV(sbfMeasEpoch->Header.ID) >= 1)
  {
    if (sbfMeasEpoch->CumClkJumps >= 128)
      trackMeasEpoch->totalClockJump_ms = (int32_t)sbfMeasEpoch->CumClkJumps - 256;
    else
      trackMeasEpoch->totalClockJump_ms = (int32_t)sbfMeasEpoch->CumClkJumps;
  }
  else
  {
    trackMeasEpoch->totalClockJump_ms = I32_NOTVALID;
  }
  
  trackMeasEpoch->TOW_ms = sbfMeasEpoch->TOW;
  trackMeasEpoch->WNc    = sbfMeasEpoch->WNc;

  trackMeasEpoch->rxTOWStatus = RXTOWSTATUS_NOTSET;
  if (trackMeasEpoch->TOW_ms != U32_NOTVALID)
  {
    trackMeasEpoch->rxTOWStatus = RXTOWSTATUS_COARSE;
  }
  if (trackMeasEpoch->WNc != U16_NOTVALID)
  {
    trackMeasEpoch->rxTOWStatus = RXTOWSTATUS_PRECISE;
    trackMeasEpoch->IsWNcSet = true;
  }
  else
  {
    trackMeasEpoch->IsWNcSet = false;
  }
  trackMeasEpoch->nbrElements = sbfMeasEpoch->N;

  trackMeasEpoch->dopplerVarFactor = DEFAULT_DOPPLER_VARIANCE_FACTOR;

  /* Copying bits of common flags */
  trackMeasEpoch->commonFlags = 0;

  /* Bit 0 : multipath mitigation  */
  if ( (sbfMeasEpoch->CommonFlags & SBF_MEASEPOCH_2_COMMONFLAG_MULTIPATHMITIGATION) != 0)
  {
    trackMeasEpoch->commonFlags |=  COMMONFLAG_MULTIPATHMITIGATION;
  }
  /* Bit 1 : code smoothing  */
  if ( (sbfMeasEpoch->CommonFlags & SBF_MEASEPOCH_2_COMMONFLAG_ATLEASTONESMOOTHING) != 0)
  {
    trackMeasEpoch->commonFlags |=  COMMONFLAG_ATLEASTONESMOOTHING;
  }
  /* Bit 3 : clock steering  */
  if ( (sbfMeasEpoch->CommonFlags & SBF_MEASEPOCH_2_COMMONFLAG_CLOCKSTEERINGACTIVE) != 0)
  {
    trackMeasEpoch->commonFlags |=  COMMONFLAG_CLOCKSTEERINGACTIVE;
  }
  /* Bit 4 Not applicable */
  if ( (sbfMeasEpoch->CommonFlags & SBF_MEASEPOCH_2_COMMONFLAG_HIGHDYNAMICSMODE) != 0)
  {
    trackMeasEpoch->commonFlags |=  COMMONFLAG_HIGHDYNAMICSMODE;
  }
  /* Bits 6,7 Reserved */

  Type1SubBlock = GetFirstType1SubBlock(sbfMeasEpoch);

  while (Type1SubBlock != NULL) 
  {
    int i;
    SignalType_t SignalType;
    uint32_t     AntIdx;
    double   PR_m;
    double   Carrier_cycles;
    double   Doppler_Hz;
    float    CN0_dBHz;
    uint32_t LockTime_s;
    bool     HCF; /* halfcycle flag */
    bool     AtLeastOneMeas = false;
    MeasChannel_t *trackChan = &(trackMeasEpoch->channelData[chNR]);

    trackChan->PRN     = convertSVIDfromSBF(GetMeasEpochSVID(Type1SubBlock));
    
    if (trackChan->PRN == 0)
    {
      /* SVID not recognized, skip */
      /*@-mustfreefresh@*/
      Type1SubBlock = GetNextType1SubBlock(sbfMeasEpoch, Type1SubBlock);
      /*@=mustfreefresh@*/
      continue;
    }

    if (agIsGLONASS((unsigned long)trackChan->PRN))
    {
      trackChan->fnPlus8 = (uint8_t)(Type1SubBlock->ObsInfo>>3);
      if (trackChan->fnPlus8 == 0  || trackChan->fnPlus8 > (uint8_t)glNBRFN)
      {
        /*@-mustfreefresh@*/
        Type1SubBlock = GetNextType1SubBlock(sbfMeasEpoch, Type1SubBlock);
        /*@=mustfreefresh@*/
        continue;
      }
    }
    else
    {
      trackChan->fnPlus8 = (uint8_t)0;
    }

    /* For some files (e.g. converted from rinex to sbf) the channel nr is unknown.
       In SBF this is then set to 0 or UI8_NOTVALID (depending on version of the tool)
       This should actually be between 1 and max nr of logical channels.
       For those files the channel nr is replaced by a fictive one */
    if ( (GetMeasEpochRXChannel(Type1SubBlock) != 0) &&
         (GetMeasEpochRXChannel(Type1SubBlock) != UI8_NOTVALID) )
    {
      trackChan->channel = (GetMeasEpochRXChannel(Type1SubBlock) - 1);
    }
    else
    {
      trackChan->channel = chNR;
    }

    /*@-compdef@*/
    GetObsFromType1(sbfMeasEpoch, Type1SubBlock,
                    &SignalType, &AntIdx, &PR_m, &Carrier_cycles, &Doppler_Hz, &CN0_dBHz, &LockTime_s, &HCF);
    /*@=compdef@*/
    
    AtLeastOneMeas |= sbfread_AddMeasSet(trackChan, SignalType, AntIdx,
                                         PR_m, Carrier_cycles, Doppler_Hz, CN0_dBHz, LockTime_s, HCF);
        
    for (i=0; i<(int)Type1SubBlock->N_Type2; i++)
    {
      /*@-compdef@*/
      GetNextObsFromType2(sbfMeasEpoch, Type1SubBlock,
                          &SignalType, &AntIdx, &PR_m, &Carrier_cycles, &Doppler_Hz, &CN0_dBHz, &LockTime_s, &HCF);
      /*@=compdef@*/

      AtLeastOneMeas |= sbfread_AddMeasSet(trackChan, SignalType, AntIdx,
                                           PR_m, Carrier_cycles, Doppler_Hz, CN0_dBHz, LockTime_s, HCF);
    }
      

    if (AtLeastOneMeas) chNR++;
    
    /*@-mustfreefresh@*/
    Type1SubBlock = GetNextType1SubBlock(sbfMeasEpoch, Type1SubBlock);
    /*@=mustfreefresh@*/
  }

  trackMeasEpoch->nbrElements = chNR;
}



/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*--- FUNCTIONS TO DECODE MEASEXTRA SBF BLOCKS ------------------------------*/
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*! decodes a MeasExtra_1_t (SBF Block) into an existing MeasEpoch_t
 */
void
sbfread_MeasExtra_Decode(MeasExtra_1_t   *sbfMeasExtra,
                         MeasEpoch_t     *measEpoch )
{
  MeasExtra_1_t  sbfMeasExtra_Aligned;
  uint32_t       chNR = 0;
  uint32_t       i;

  /* align the sub-block size to what we expect */
  memcpy(&sbfMeasExtra_Aligned, sbfMeasExtra, sbfMeasExtra->Header.Length);
  AlignSubBlockSize(&sbfMeasExtra_Aligned, (unsigned)sbfMeasExtra->N,
                    sbfMeasExtra->SBSize, sizeof(MeasExtraChannel_1_t));

  
  chNR = (uint32_t)sbfMeasExtra_Aligned.N;

  if (sbfMeasExtra_Aligned.DopplerVarFactor != 0.0)
  {
    measEpoch->dopplerVarFactor = sbfMeasExtra_Aligned.DopplerVarFactor;
  }

  /* loop over all MeasExtra channels */
  for (i = 0; i < chNR; i++)
  {
    MeasExtraChannel_1_t *extraChan = &(sbfMeasExtra_Aligned.MeasExtraChannel[i]);
    MeasChannel_t        *measChan  = measEpoch->channelData;

    int      antNR = (int)((extraChan->Type>>5) & 0x7);
    uint8_t  sigID;
    int      j     = 0;
    int      chIdx = 0;
    bool     chFound = false;

    sigID = (uint8_t) (extraChan->Type & 0x1F);
    if (sigID == 31) sigID = (uint8_t)((extraChan->Misc>>3)+32);

    /* find corresponding channel, do not process the signal if it is  
       being tracked on an antenna that is not supported in this platform */
    /*@-unsignedcompare@*/
    if (antNR < NR_OF_ANTENNAS)
    {
      do
      {
        measChan = &(measEpoch->channelData[chIdx]);
        if (measChan->channel == (extraChan->RXChannel - 1))
        {
          chFound = true;
        }
        chIdx++;
      } while ((!chFound) && (chIdx < (int)(measEpoch->nbrElements)));
    }
    /*@-unsignedcompare@*/

    if (chFound)
    {
      /* find corresponding signal location*/
      do
      {
        if (measChan->measSet[antNR][j].signalType == sigID)
        {
          MeasSet_t *meas = &(measChan->measSet[antNR][j]);
          
          if (extraChan->CarrierVar != U16_NOTVALID)
            meas->Lvariance_cycles2 = (float)(extraChan->CarrierVar*1e-6);
          else
            meas->Lvariance_cycles2 = F32_NOTVALID;
          
          if (extraChan->CodeVar != U16_NOTVALID)
            meas->PRvariance_m2 = (float)(extraChan->CodeVar*1e-4);
          else
            meas->PRvariance_m2 = F32_NOTVALID;

          if (extraChan->LockTime == 0)
            meas->PLLTimer_ms = 10;
          else if (extraChan->LockTime < U16_NOTVALID)
            meas->PLLTimer_ms = extraChan->LockTime*1000;
          else
            meas->PLLTimer_ms = 0;
          
          meas->MP_mm = extraChan->MPCorr;

          meas->SmoothingCorr_mm = extraChan->SmoothingCorr;  

          if (SBF_ID_TO_REV(sbfMeasExtra_Aligned.Header.ID) >= 1)
            meas->lockCount = extraChan->CumLossCont;
          else 
            meas->lockCount = 0;

          /* extract the carrier multipath and the APMEINSYNC bit if this is
             the revision 2 of the block */
          if (SBF_ID_TO_REV(sbfMeasExtra_Aligned.Header.ID) >= 2)
          {
            if ((extraChan->Info&1) != 0) 
            {
              meas->flags |= MEASFLAG_APMEINSYNC;
            }
            meas->CarrierMP_1_512c      = extraChan->CarMPCorr;
          }
          else
          {
            meas->CarrierMP_1_512c      = (int8_t)0;
          }
          
          if (SBF_ID_TO_REV(sbfMeasExtra_Aligned.Header.ID) >= 3)
          {
            meas->CN0_dBHz += (extraChan->Misc&0x7)*0.03125;
          }

          j = MAX_NR_OF_SIGNALS_PER_SATELLITE;
          chFound = false;
        }
        j++;
      }
      while ( j<MAX_NR_OF_SIGNALS_PER_SATELLITE );

    }
  }
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*--- FUNCTIONS TO DECODE MEASFULLRANGE SBF BLOCKS --------------------------*/
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*! decodes a MeasFullRange_1_t (SBF Block) into an existing
    MeasEpoch_t
 */
static void
sbfread_MeasFullRange_Decode(MeasFullRange_1_t  *sbfMeasFullRange,
                             MeasEpoch_t        *trackMeasEpoch )
{
  /* Local declarations */
  uint32_t  n;
  MeasFullRange_1_t  sbfMeasFullRange_Aligned;

  /* align the sub-block size to what we expect */
  memcpy(&sbfMeasFullRange_Aligned, sbfMeasFullRange, sbfMeasFullRange->Header.Length);
  AlignSubBlockSize(&sbfMeasFullRange_Aligned, (unsigned)sbfMeasFullRange->N,
                    sbfMeasFullRange->SBLength, sizeof(MeasFullRangeSub_1_t));

  /* Body */
#ifdef SWAP_DOUBLE_NEEDED
  for (n = 0; in < sbfMeasFullRange_Aligned.N; n++)
  {
    swapDouble(&(sbfMeasFullRange_Aligned.measFullRangeSub[n].CodeObs));
  }
#endif

  /* loop over all subblocks */
  for (n = 0; n < sbfMeasFullRange_Aligned.N; n++)
  {
    MeasFullRangeSub_1_t *fullRangeSub = &(sbfMeasFullRange_Aligned.MeasFullRangeSub[n]);
    MeasChannel_t        *measChan     = trackMeasEpoch->channelData;

    int      antNR   = (int)((fullRangeSub->FreqNrAnt>>5) & 0x7);
    uint8_t  fnPlus8 = fullRangeSub->FreqNrAnt & 0x1F;
    uint8_t  sigID   = fullRangeSub->Type;
    int      j       = 0;
    int      chIdx   = 0;
    bool     chFound = false;

    /* find corresponding tracker channel, do not process the signal if it is
       being tracked on an antenna that not supported in this platform */
    /*@-unsignedcompare@*/
    if (antNR < NR_OF_ANTENNAS)
    {
      do
      {
        measChan = &(trackMeasEpoch->channelData[chIdx]);
        if (measChan->channel == (fullRangeSub->RxChannel - 1))
        {
          chFound = true;
        }
        chIdx++;
      } while ((!chFound) && (chIdx < (int)(trackMeasEpoch->nbrElements)));
    }
    /*@-unsignedcompare@*/

    if (chFound)
    {
      /* find corresponding signal location */
      do
      {
        if (measChan->measSet[antNR][j].signalType == sigID)
        {
          MeasSet_t *meas   = &(measChan->measSet[antNR][j]);
          double waveLength = GetWavelength_m((SignalType_t)sigID, (int)fnPlus8-8);

          meas->PR_m     = fullRangeSub->CodeObs;
          meas->L_cycles = (fullRangeSub->CarrierMinCode + meas->PR_m) / waveLength;

          /* C/N0 encoded in block from rev1 */
          if (SBF_ID_TO_REV(sbfMeasFullRange_Aligned.Header.ID) > 0)
            meas->CN0_dBHz = (float)fullRangeSub->CN0/100.0F;
          
          j = MAX_NR_OF_SIGNALS_PER_SATELLITE;
          chFound = false;
        }
        j++;
      } while (j < MAX_NR_OF_SIGNALS_PER_SATELLITE);
    }
  }
}


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*--- FUNCTIONS TO COLLECT AND DECODE ALL MEASUREMENT BLOCK -----------------*/
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
static bool
sbfread_ProcessEpoch(SBFData_t           *SBFData,
                     MeasEpoch_t         *MeasEpoch,
                     uint32_t             EnabledMeasTypes)
{
  int      i;
  bool     MeasReady = false;
  
  if ((EnabledMeasTypes&SBFREAD_MEAS3_ENABLED)!=0 && SBFData->Meas3Ranges[0].Header.ID != 0 &&
      SBFData->MeasCollect_CurrentTOW != SBFData->TOWAtLastMeasEpoch)
  {
    sbfread_Meas3_Decode(SBFData->Meas3Ranges,
                         SBFData->Meas3Doppler,
                         SBFData->Meas3CN0HiRes,
                         SBFData->Meas3PP,
                         SBFData->Meas3MP,
                         SBFData->RefEpoch,
                         MeasEpoch
#ifndef NO_DECRYPTION
                         ,&(SBFData->decrypt)
#endif
                         );
    
    SBFData->TOWAtLastMeasEpoch = SBFData->MeasCollect_CurrentTOW;
    
    MeasReady = true;
  }
  else if ((EnabledMeasTypes&SBFREAD_MEASEPOCH_ENABLED)!=0 && SBFData->MeasEpoch.Header.ID != 0 &&
      SBFData->MeasCollect_CurrentTOW != SBFData->TOWAtLastMeasEpoch)
  {
    sbfread_MeasEpoch_Decode(&(SBFData->MeasEpoch), MeasEpoch);
    
    // include MeasExtra if available
    if (SBFData->MeasExtra.Header.ID != 0)
      sbfread_MeasExtra_Decode(&(SBFData->MeasExtra), MeasEpoch);
    
    // include MeasFullRange if available
    if (SBFData->MeasFullRange.Header.ID != 0)
      sbfread_MeasFullRange_Decode(&(SBFData->MeasFullRange), MeasEpoch);

    SBFData->TOWAtLastMeasEpoch = SBFData->MeasCollect_CurrentTOW;
    
    MeasReady = true;
  }

  /* forget all collected blocks to start a new epoch */
  for (i=0; i<NR_OF_ANTENNAS; i++)
  {
    SBFData->Meas3Ranges[i].Header.ID   = 0;
    SBFData->Meas3Doppler[i].Header.ID  = 0;
    SBFData->Meas3CN0HiRes[i].Header.ID = 0;
    SBFData->Meas3PP[i].Header.ID       = 0;
    SBFData->Meas3MP[i].Header.ID       = 0;
  }
  SBFData->MeasEpoch.Header.ID        = 0;
  SBFData->MeasExtra.Header.ID        = 0;
  SBFData->MeasFullRange.Header.ID    = 0;

  return MeasReady;
}


/*---------------------------------------------------------------------------*/
static bool sbfread_IsMeasBlock(const void* const SBFBlock)
{
  uint32_t BlockNumber = SBF_ID_TO_NUMBER(((HeaderAndTimeBlock_t*)SBFBlock)->Header.ID);
  
  return (BlockNumber == sbfnr_Meas3Ranges_1   ||
          BlockNumber == sbfnr_Meas3Doppler_1  ||
          BlockNumber == sbfnr_Meas3CN0HiRes_1 ||
          BlockNumber == sbfnr_Meas3PP_1       ||
          BlockNumber == sbfnr_Meas3MP_1       ||
          BlockNumber == sbfnr_GenMeasEpoch_1  ||
          BlockNumber == sbfnr_MeasEpoch_2     ||
          BlockNumber == sbfnr_MeasExtra_1     ||
          BlockNumber == sbfnr_MeasFullRange_1    );
}

#define MEASCOLLECT_SEEN_MEAS3RANGES         (1<<0)
#define MEASCOLLECT_SEEN_MEAS3DOPPLER        (1<<1) 
#define MEASCOLLECT_SEEN_MEAS3CN0HIRES       (1<<2)
#define MEASCOLLECT_SEEN_MEAS3PP             (1<<3)
#define MEASCOLLECT_SEEN_MEAS3MP             (1<<4)
#define MEASCOLLECT_SEEN_MEASEPOCH           (1<<5)
#define MEASCOLLECT_SEEN_MEASEXTRA           (1<<6)
#define MEASCOLLECT_SEEN_MEASFULLRANGE       (1<<7)


/*---------------------------------------------------------------------------*/
bool
sbfread_MeasCollectAndDecode(SBFData_t           *SBFData,
                             void                *SBFBlock,
                             MeasEpoch_t         *MeasEpoch,
                             uint32_t             EnabledMeasTypes)
{
  uint32_t BlockNumber;
  uint32_t AntIdx;
  bool     MeasReady = false;
  bool     EndOfEpoch = false;

  BlockNumber = SBF_ID_TO_NUMBER(((HeaderAndTimeBlock_t*)SBFBlock)->Header.ID);

  /* if this block belongs to a different epoch as the previous one,
     or is not a measurement block, process all the blocks collected
     so far */
  if ((((HeaderAndTimeBlock_t*)SBFBlock)->TOW != SBFData->MeasCollect_CurrentTOW ||
       !sbfread_IsMeasBlock(SBFBlock)) &&
      SBFData->MeasCollect_BlocksSeenAtThisEpoch != 0)
  {
    MeasReady = sbfread_ProcessEpoch(SBFData, MeasEpoch, EnabledMeasTypes);
    
    SBFData->MeasCollect_BlocksSeenAtLastEpoch = SBFData->MeasCollect_BlocksSeenAtThisEpoch;
    SBFData->MeasCollect_BlocksSeenAtThisEpoch = 0;
  }
  
  SBFData->MeasCollect_CurrentTOW = ((HeaderAndTimeBlock_t*)SBFBlock)->TOW;


  /* store the measurement block */
  /*@-mayaliasunique@*/
  switch (BlockNumber)
  {
  case sbfnr_Meas3Ranges_1:
    AntIdx = sbfread_Meas3_GetAntennaIdx((uint8_t*)SBFBlock);
    if ((int)AntIdx < NR_OF_ANTENNAS)
    { 
      memcpy(SBFData->Meas3Ranges+AntIdx, SBFBlock, ((HeaderAndTimeBlock_t*)SBFBlock)->Header.Length);
    }
    SBFData->MeasCollect_BlocksSeenAtThisEpoch |= MEASCOLLECT_SEEN_MEAS3RANGES;
    break;
    
  case sbfnr_Meas3Doppler_1:
    AntIdx = sbfread_Meas3_GetAntennaIdx((uint8_t*)SBFBlock);
    if ((int)AntIdx < NR_OF_ANTENNAS)
    { 
      memcpy(SBFData->Meas3Doppler+AntIdx, SBFBlock, ((HeaderAndTimeBlock_t*)SBFBlock)->Header.Length);
    }
    SBFData->MeasCollect_BlocksSeenAtThisEpoch |= MEASCOLLECT_SEEN_MEAS3DOPPLER;
    break;

  case sbfnr_Meas3CN0HiRes_1:
    AntIdx = sbfread_Meas3_GetAntennaIdx((uint8_t*)SBFBlock);
    if ((int)AntIdx < NR_OF_ANTENNAS)
    { 
      memcpy(SBFData->Meas3CN0HiRes+AntIdx, SBFBlock, ((HeaderAndTimeBlock_t*)SBFBlock)->Header.Length);
    }
    SBFData->MeasCollect_BlocksSeenAtThisEpoch |= MEASCOLLECT_SEEN_MEAS3CN0HIRES;
    break;

  case sbfnr_Meas3PP_1:
    AntIdx = sbfread_Meas3_GetAntennaIdx((uint8_t*)SBFBlock);
    if ((int)AntIdx < NR_OF_ANTENNAS)
    { 
      memcpy(SBFData->Meas3PP+AntIdx, SBFBlock, ((HeaderAndTimeBlock_t*)SBFBlock)->Header.Length);
    }
    SBFData->MeasCollect_BlocksSeenAtThisEpoch |= MEASCOLLECT_SEEN_MEAS3PP;
    break;

  case sbfnr_Meas3MP_1:
    AntIdx = sbfread_Meas3_GetAntennaIdx((uint8_t*)SBFBlock);
    if ((int)AntIdx < NR_OF_ANTENNAS)
    {
      memcpy(SBFData->Meas3MP+AntIdx, SBFBlock, ((HeaderAndTimeBlock_t*)SBFBlock)->Header.Length);
    }
    SBFData->MeasCollect_BlocksSeenAtThisEpoch |= MEASCOLLECT_SEEN_MEAS3MP;
    break;

  case sbfnr_MeasEpoch_2:
  case sbfnr_GenMeasEpoch_1:
    memcpy(&(SBFData->MeasEpoch), SBFBlock, ((HeaderAndTimeBlock_t*)SBFBlock)->Header.Length);
    SBFData->MeasCollect_BlocksSeenAtThisEpoch |= MEASCOLLECT_SEEN_MEASEPOCH;
    break;
    
  case sbfnr_MeasExtra_1:
    memcpy(&(SBFData->MeasExtra), SBFBlock, ((HeaderAndTimeBlock_t*)SBFBlock)->Header.Length);
    SBFData->MeasCollect_BlocksSeenAtThisEpoch |= MEASCOLLECT_SEEN_MEASEXTRA;
    break;
    
  case sbfnr_MeasFullRange_1:
    memcpy(&(SBFData->MeasFullRange), SBFBlock, ((HeaderAndTimeBlock_t*)SBFBlock)->Header.Length);
    SBFData->MeasCollect_BlocksSeenAtThisEpoch |= MEASCOLLECT_SEEN_MEASFULLRANGE;
    break;
  }
  /*@=mayaliasunique@*/
  

  /* if we are reading an SBF file, process the epoch as soon as the
     next block on file is no measurement block, or if its TOW differs
     from the TOW of the current block, or when we are at the end of
     the file */
  if (SBFData->F != NULL)
  {
    uint8_t  NextSBFBlock[MAX_SBFSIZE];
    HeaderAndTimeBlock_t *NextHT = (HeaderAndTimeBlock_t*)NextSBFBlock;
    
    if (GetNextBlock(SBFData, NextSBFBlock, BLOCKNUMBER_ALL, BLOCKNUMBER_ALL,
                     START_POS_CURRENT | END_POS_DONT_CHANGE) == 0)
    {
      EndOfEpoch = (NextHT->TOW != ((HeaderAndTimeBlock_t*)SBFBlock)->TOW ||
                    !sbfread_IsMeasBlock(NextSBFBlock));
    }
    else
    {
      /* end of file implies end of epoch */
      EndOfEpoch = true;
    }
  }
  else if (SBFData->MeasCollect_BlocksSeenAtThisEpoch != 0 &&
           SBFData->MeasCollect_BlocksSeenAtThisEpoch == SBFData->MeasCollect_BlocksSeenAtLastEpoch)
  {
    /* if we have collected all blocks that were present at the last
       epoch, also consider that the epoch is complete */
    EndOfEpoch = true;
  }
  
  if (EndOfEpoch)
  {
    /*@-nullstate@*/
    MeasReady = sbfread_ProcessEpoch(SBFData, MeasEpoch, EnabledMeasTypes);
    /*@=nullstate@*/
  }
  
  /*@-mustdefine@*/
  return MeasReady;
  /*@=mustdefine@*/
}


/*---------------------------------------------------------------------------*/
bool
sbfread_FlushMeasEpoch(SBFData_t           *SBFData,
                       MeasEpoch_t         *MeasEpoch,
                       uint32_t             EnabledMeasTypes)
{
  return sbfread_ProcessEpoch(SBFData, MeasEpoch, EnabledMeasTypes);
}
