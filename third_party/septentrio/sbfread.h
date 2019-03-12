/*
 * sbfread.h: Declarations of SBF (Septentrio Binary
 * Format) reading and parsing functions.
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

#ifndef SBFREAD_H
#define SBFREAD_H 1

#include <stdio.h>

#include "ssntypes.h"

#ifndef S_SPLINT_S
#include <sys/types.h>
#else
  typedef int64_t off_t;
#endif

#include "measepoch.h"
#include "sbfdef.h"
#include "sbfsigtypes.h"
#include "sbfsvid.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 64 bit versions for fseek and ftell are different for the different compilers */
/* STD C version */
#if (defined(__rtems__))
  #define ssnfseek fseek
  #define ssnftell ftell
#elif (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L))
  #define ssnfseek fseeko
  #define ssnftell ftello
/* Microsoft compiler MSVC  */
#elif defined (_MSC_VER)
  #define ssnfseek _fseeki64
  #define ssnftell _ftelli64
  #define off_t __int64
/* MinGW-32  */
#elif defined (__MINGW32__) && !defined (__MINGW64__)
/* 32 bit on MinGW-64 already redefines them if _FILE_OFFSET_BITS=64 */
 #define ssnfseek fseeko64
 #define ssnftell ftello64
/* fall back to 32 bit version */
#else
  #define ssnfseek fseek
  #define ssnftell ftell
#endif

#define BLOCKNUMBER_ALL      (0xffff)

#define START_POS_FIELD   0x03LU
#define START_POS_CURRENT 0x00LU
#define START_POS_SET     0x01LU

#define END_POS_FIELD         0x0cLU
#define END_POS_AFTER_BLOCK   0x00LU
#define END_POS_DONT_CHANGE   0x04LU

#ifndef M_PI
#define	M_PI		3.14159265358979323846  /* pi */
#endif

#define c84   299792458.             /* WGS-84 value for lightspeed */

#define E5FREQ         1191.795e6
#define E5aFREQ        1176.45e6
#define E5bFREQ        1207.14e6
#define E6FREQ         1278.75e6
#define L1FREQ         1575.42e6
#define L2FREQ         1227.60e6
#define L5FREQ         1176.45e6
#define E2FREQ         1561.098e6
#define L1GLOFREQ      1602.00e6
#define L2GLOFREQ      1246.00e6
#define L3GLOFREQ      1202.025e6
#define B3FREQ         1268.52e6

#define E5WAVELENGTH   (c84/E5FREQ)
#define E5aWAVELENGTH  (c84/E5aFREQ)
#define E5bWAVELENGTH  (c84/E5bFREQ)
#define E6WAVELENGTH   (c84/E6FREQ)
#define L1WAVELENGTH   (c84/L1FREQ)
#define L2WAVELENGTH   (c84/L2FREQ)
#define L5WAVELENGTH   (c84/L5FREQ)
#define E2WAVELENGTH   (c84/E2FREQ)
#define L3WAVELENGTH   (c84/L3GLOFREQ)
#define B3WAVELENGTH   (c84/B3FREQ)

#define FIRSTEPOCHms_DONTCARE  (-1LL)
#define LASTEPOCHms_DONTCARE   (1LL<<62)
#define INTERVALms_DONTCARE    (1)


/* structure to keep the data from the last reference epoch when
   decoding Meas3 blocks. */
typedef struct {
  uint8_t           SigIdx[MEAS3_SYS_MAX][MEAS3_SAT_MAX][MEAS3_SIG_MAX];
  MeasSet_t         MeasSet[MEAS3_SYS_MAX][MEAS3_SAT_MAX][MEAS3_SIG_MAX];
  uint32_t          SlaveSigMask[MEAS3_SYS_MAX][MEAS3_SAT_MAX];
  int16_t           PRRate_64mm_s[MEAS3_SYS_MAX][MEAS3_SAT_MAX];
  uint32_t          TOW_ms;

  uint8_t           M3SatDataCopy[MEAS3_SYS_MAX][32];
} sbfread_Meas3_RefEpoch_t;


typedef struct {
  uint8_t   key[32];    /* decryption key */
  bool      has_key;    /* true if decryption key has been set */
  uint64_t  nonce;      /* decryption nonce */
  bool      has_nonce;  /* true if decryption nonce has been set */
} SBFDecrypt_t;


typedef struct {
  FILE*               F;       /* handle to the file */
  SBFDecrypt_t        decrypt; /* SBF decryption context */

  /* the following fields are used to collect and decode the measurement
     blocks */
  sbfread_Meas3_RefEpoch_t RefEpoch[NR_OF_ANTENNAS];
  Meas3Ranges_1_t     Meas3Ranges[NR_OF_ANTENNAS];
  Meas3Doppler_1_t    Meas3Doppler[NR_OF_ANTENNAS];
  Meas3CN0HiRes_1_t   Meas3CN0HiRes[NR_OF_ANTENNAS];
  Meas3PP_1_t         Meas3PP[NR_OF_ANTENNAS];
  Meas3MP_1_t         Meas3MP[NR_OF_ANTENNAS];
  MeasEpoch_2_t       MeasEpoch;
  MeasExtra_1_t       MeasExtra;
  MeasFullRange_1_t   MeasFullRange;
  uint32_t            MeasCollect_CurrentTOW;
  uint32_t            MeasCollect_BlocksSeenAtLastEpoch;
  uint32_t            MeasCollect_BlocksSeenAtThisEpoch;
  uint32_t            TOWAtLastMeasEpoch;
} SBFData_t;

void FW_EXPORT AlignSubBlockSize(void*  SBFBlock, 
                                 unsigned int    N,
                                 size_t SourceSBSize,
                                 size_t TargetSBSize);
  
int32_t FW_EXPORT CheckBlock(SBFData_t* SBFData, uint8_t* Buffer);
                           
int32_t FW_EXPORT GetNextBlock(SBFData_t* SBFData, 
                     /*@out@*/ void*      SBFBlock,
                               uint16_t BlockNumber1, uint16_t BlockNumber2,
                               uint32_t FilePos);

off_t FW_EXPORT GetSBFFilePos(SBFData_t* SBFData);

off_t FW_EXPORT GetSBFFileLength(SBFData_t* SBFData);

void FW_EXPORT InitializeSBFDecoding(char *FileName,
                          /*@out@*/  SBFData_t* SBFData);

void FW_EXPORT CloseSBFFile(SBFData_t* SBFData);

bool FW_EXPORT IsTimeValid(void* SBFBlock);

int FW_EXPORT GetCRCErrors();

#define SBFREAD_MEAS3_ENABLED      0x1
#define SBFREAD_MEASEPOCH_ENABLED  0x2
#define SBFREAD_ALLMEAS_ENABLED    0x3

bool FW_EXPORT sbfread_MeasCollectAndDecode(
  SBFData_t                      *SBFData,
  void                           *SBFBlock,
  MeasEpoch_t /*@out@*/          *MeasEpoch,
  uint32_t                        EnabledMeasTypes);

bool FW_EXPORT sbfread_FlushMeasEpoch(SBFData_t   *SBFData,
                                      MeasEpoch_t *MeasEpoch,
                                      uint32_t     EnabledMeasTypes);
  
void FW_EXPORT sbfread_MeasEpoch_Decode(MeasEpoch_2_t *sbfMeasEpoch,
                                        MeasEpoch_t   *MeasEpoch );

void FW_EXPORT sbfread_MeasExtra_Decode(MeasExtra_1_t *sbfMeasExtra,
                                        MeasEpoch_t   *measEpoch );


double FW_EXPORT GetWavelength_m(SignalType_t SignalType, int GLOfn);


MeasEpochChannelType1_t
FW_EXPORT *GetFirstType1SubBlock(const MeasEpoch_2_t *MeasEpoch);

MeasEpochChannelType1_t
FW_EXPORT *GetNextType1SubBlock(const MeasEpoch_2_t     *MeasEpoch,
                                MeasEpochChannelType1_t *CurrentType1SubBlock);

uint32_t FW_EXPORT GetMeasEpochSVID(MeasEpochChannelType1_t *Type1SubBlock);

uint32_t FW_EXPORT GetMeasEpochRXChannel(MeasEpochChannelType1_t *Type1SubBlock);

bool FW_EXPORT IncludeThisEpoch(void*   SBFBlock,
                                int64_t ForcedFirstEpoch_ms,
                                int64_t ForcedLastEpoch_ms,
                                int     ForcedInterval_ms,
                                bool    AcceptInvalidTimeStamp);

void FW_EXPORT SBFDecryptBlock(SBFDecrypt_t *decrypt,
                               VoidBlock_t  *block);

#define TerminateProgram {char S[255];   \
  (void)snprintf(S, sizeof(S), "Abnormal program termination (%s,%i)", \
             __FILE__, __LINE__); \
  S[254] = '\0'; \
  perror(S); \
  exit(EXIT_FAILURE); \
  }

#ifdef __cplusplus
}
#endif

#endif
/* End of "ifndef SBFREAD_H" */
