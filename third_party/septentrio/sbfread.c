/*
 * sbfread.c: SBF (Septentrio Binary Format) reading and parsing
 * functions.
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "crc.h"
#include "sbfread.h"
#include "sbfsigtypes.h"

#ifndef NO_DECRYPTION
# include "sbfdecrypt.h"
#endif

/* size of the SBF header in bytes */
#define HEADER_SIZE        8

/* SBF sync bytes */
static const char SYNC_STRING[3]="$@";
static int intCRCErrors = 0;

/*---------------------------------------------------------------------------*/
bool IsTimeValid(void* SBFBlock)
/* Returns true if none of the TOW and WNc fields are set to its don't use
   value */
{
  return ((((HeaderAndTimeBlock_t*)SBFBlock)->TOW != 0xffffffffUL)  &&
          (((HeaderAndTimeBlock_t*)SBFBlock)->WNc != (uint16_t)0xffff) );
}


/*---------------------------------------------------------------------------*/
/* check whether a given epoch needs to be included in the ASCII file. 

   An epoch can be included in the ASCII file if the epoch time meets
   the following conditions:

   1) it is a multiple of the ForcedInterval_ms

   2) it is larger or equal than ForcedFirstEpoch_ms.  If
      ForcedFirstEpoch_ms is lower than 86400000, only the time-of-day
      is controlled, not the date.  If ForcedFirstEpoch_ms is
      FIRSTEPOCHms_DONTCARE, it is ignored.

   3) it is lower or equal than ForcedLastEpoch_ms.  If
      ForcedLastEpoch_ms is lower than 86400000, only the time-of-day
      is controlled, not the date.  If ForcedLastEpoch_ms is
      LASTEPOCHms_DONTCARE, it is ignored.
 */
bool IncludeThisEpoch(void*   SBFBlock,
                      int64_t ForcedFirstEpoch_ms,
                      int64_t ForcedLastEpoch_ms,
                      int     ForcedInterval_ms,
                      bool    AcceptInvalidTimeStamp)
{
  bool    ret = (AcceptInvalidTimeStamp || IsTimeValid(SBFBlock));
  int     TOW_ms     = (int)(((HeaderAndTimeBlock_t*)SBFBlock)->TOW);
  int64_t GPSTime_ms = (int64_t)(((HeaderAndTimeBlock_t*)SBFBlock)->WNc)
                       *(86400LL*7LL*1000LL)+TOW_ms;
  
  /* check interval */
  if (TOW_ms%ForcedInterval_ms != 0)
  {
    ret = false;
  }
  
  /* check the first epoch time */
  if (ForcedFirstEpoch_ms != FIRSTEPOCHms_DONTCARE)
  {
    if (ForcedFirstEpoch_ms < 86400000)
    {
      if (((int)TOW_ms%86400000)<ForcedFirstEpoch_ms)
      {
        ret = false;
      }
    }
    else if (GPSTime_ms < ForcedFirstEpoch_ms)
    {
      ret = false;
    }
  }

  /* check the last epoch time */
  if (ForcedLastEpoch_ms != LASTEPOCHms_DONTCARE)
  {
    if (ForcedLastEpoch_ms < 86400000)
    {
      if (((int)TOW_ms%86400000) > ForcedLastEpoch_ms)
      {
        ret = false;
      }
    }
    else if (GPSTime_ms > ForcedLastEpoch_ms)
    {
      ret = false;
    }
  }

  return ret;
}


/*---------------------------------------------------------------------------*/
void AlignSubBlockSize(void*  SBFBlock,
                       unsigned int    N,
                       size_t SourceSBSize,
                       size_t TargetSBSize)

/* This function should be called to align the sub-blocks contained in
 * the SBF block pointed to by SBFBlock.  N is the number of
 * sub-blocks in that SBF block and SourceSBSize is the size of these
 * sub-blocks.  TargetSBSize is the desired size of the sub-block.
 *
 * This function must be called when decoding SBF blocks containing
 * one type of sub-blocks, in order to ensure compatibility with
 * possible SBF upgrades.
 */
{
  uint32_t SourceLength, TargetLength, i;
  
  SourceLength  = ((HeaderBlock_t*)SBFBlock)->Length;
  TargetLength  = SourceLength - N*SourceSBSize + N*TargetSBSize;
  
  if (SourceSBSize > TargetSBSize)
  {
    /* block needs to schrink: copy starting from the beginning */
    for (i=0; i<N; i++)
    {
      memmove(&(((uint8_t*)SBFBlock)[TargetLength - (N-i)*TargetSBSize]),
              &(((uint8_t*)SBFBlock)[SourceLength - (N-i)*SourceSBSize]),
              TargetSBSize);
    }
  }
  else if (SourceSBSize < TargetSBSize)
  {
    /* block needs to expanded: copy starting from the end */
    for (i=0; i<N; i++)
    {
      memmove(&(((uint8_t*)SBFBlock)[TargetLength - (i+1)*TargetSBSize]),
              &(((uint8_t*)SBFBlock)[SourceLength - (i+1)*SourceSBSize]),
              TargetSBSize);
    }
  }

}

/*---------------------------------------------------------------------------*/
off_t GetSBFFilePos(SBFData_t* SBFData)
/* Get the current SBF file pointer position in bytes from the
 * beginning of the file.  */
{

  return (off_t)ssnftell(SBFData->F);

}

/*---------------------------------------------------------------------------*/
off_t GetSBFFileLength(SBFData_t* SBFData)
/* Get the length of the SBF file in bytes.
 */
{

  off_t CurrentPos = ssnftell(SBFData->F);
  off_t FileLength;

  if (ssnfseek(SBFData->F, 0, SEEK_END) != 0)
    TerminateProgram;

  FileLength = (off_t)ssnftell(SBFData->F);

  if (ssnfseek(SBFData->F, CurrentPos, SEEK_SET) != 0)
    TerminateProgram;

  return FileLength;

}

/*--------------------------------------------------------------------------*/
int GetCRCErrors()
/* Returns the number of CRC errors present */
{
    return intCRCErrors;    
}


/*--------------------------------------------------------------------------*/
int32_t CheckBlock(SBFData_t* SBFData, uint8_t* Buffer)

/* Check that the validity of a tentative SBF block in the file. When
 * calling this function, Buffer[0] has to contain the '$' character
 * which identifies the start of the tentative SBF block, and the file
 * pointer has to point to the character just after this '$'
 * character.
 *
 * When this function returns and the tentative block is valid, the
 * file pointer points to the first byte following the just-found SBF
 * block, and Buffer contains the bytes of the SBF block. If the
 * tentative block is not valid, the file pointer is not changed, nor
 * the contents of Buffer.
 *
 * Arguments:
 *   *SBFData: a pointer to the SBFData as initialized by
 *             InitializeSBFDecoding;
 *   *Buffer:  on success, contains the SBF block. Otherwise not
 *             changed
 *
 * Return:
 *    0   if the tentative block is indeed a valid SBF block. In this
 *        case the file pointer is moved to the byte just following
 *        the SBF block, and the SBF block is returned in the Buffer
 *        argument.
 *   -1   if the tentative block is not a valid SBF block. In
 *        this case the file pointer is not changed, neither SBFBlock.
 */
{
  VoidBlock_t* VoidBlock = (VoidBlock_t*)Buffer;
  off_t InitialFilePos;

  /* Remember the current file pointer to return there if needed */
  InitialFilePos = ssnftell(SBFData->F);

  /* Read the block header, remember that the first '$' is already
   * present in Buffer[0]. */
  if (fread(&Buffer[1], 1, HEADER_SIZE-1, SBFData->F) != HEADER_SIZE-1) {

    /* If the read could not be performed, we are probably at the end
     * of the file, go back to the initial file position and return
     * -1 */
    if (ssnfseek(SBFData->F, InitialFilePos, SEEK_SET) != 0)
      TerminateProgram;

    return -1;
  }

  /* Check the block header parameters. If not valid, go back to the
   * initial file position and return -1. */
  if ((VoidBlock->Sync !=
       ((uint16_t)SYNC_STRING[0] | (uint16_t)SYNC_STRING[1]<<8)) || 
#if (MAX_SBFSIZE<65535)
      (VoidBlock->Length   > MAX_SBFSIZE)                        ||   
#endif
      (VoidBlock->Length   < sizeof(HeaderAndTimeBlock_t))) 
  {
    
    if (ssnfseek(SBFData->F, InitialFilePos, SEEK_SET) != 0)
      TerminateProgram;

    return -2;
  }

  /* Fetch the block body */
  if ((uint16_t)fread(&Buffer[HEADER_SIZE], 1, VoidBlock->Length-HEADER_SIZE,
            SBFData->F) != VoidBlock->Length - HEADER_SIZE) {

    /* If the read could not be performed, we are probably at the end
     * of the file, return to the initial file position and return
     * -1 */
    if (ssnfseek(SBFData->F, InitialFilePos, SEEK_SET) !=0)
      TerminateProgram;

    return -3;
  }

  /* Check the CRC field */
  if (CRCIsValid(Buffer) == false){
    /* Increase the number of CRC errors */
    intCRCErrors++;
    /* If CRC not valid, go back to the initial file position and
     * return -1. */
    if (ssnfseek(SBFData->F, InitialFilePos, SEEK_SET) !=0)
      TerminateProgram;

    return -4;
  }

#ifndef NO_DECRYPTION
  SBFDecryptBlock(&(SBFData->decrypt), VoidBlock);
#endif

  return 0;

}


/*--------------------------------------------------------------------------*/
int32_t GetNextBlock(SBFData_t* SBFData, void* SBFBlock,
                     uint16_t BlockNumber1, 
                     uint16_t BlockNumber2, 
                     uint32_t FilePos)

/* Scans the SBF file forward to find the next block having one of the
 * requested block numbers (the block number is coded in the 13 LSB of
 * the ID field). If such a block can be found, read it into
 * SBFBlock. If no block can be found with the required numbers, the
 * contents of SBFBlock is not modified.
 *
 * Arguments:
 *   *SBFData: a pointer to the SBFData as initialized by
 *             InitializeSBFDecoding
 *
 *   *SBFBlock: on success, contains the SBF block. Otherwise not
 *              changed
 *
 *   BlockNumber1 & BlockNumber2: the two possible block numbers this
 *            function is looking for. To search only one
 *            particular block number, set the two numbers to the same
 *            value.  If at least one of these arguments is
 *            BLOCKNUMBER_ALL, the function searches the next block,
 *            whatever its number.
 *
 *   FilePos: by default (if FilePos is set to 0), the SBF
 *            file is scanned from the current file position, and the
 *            file pointer is set to the first byte after the block
 *            header if a SBF block is found, or is kept unchanged if
 *            none is found. FilePos is used to change this behavior:
 *            - the start position for the search is either
 *              START_POS_CURRENT (default) or START_POS_SET depending
 *              on the search has to start from the current file
 *              pointer or from the beginning of the file;
 *            - the file position if no block found is always
 *              unchanged. If a valid SBF block is found, the end
 *              position is set according to the following value in
 *              FilePos:
 *                END_POS_AFTER_BLOCK: (default) the file position is
 *                    set at the first byte after the valid SBF block.
 *                END_POS_DONT_CHANGE: the file position is kept
 *                    unchanged by the function, even if a block is
 *                    found.
 *            The start position and end position can be ORed.
 *
 * Return:
 *    0  if a block having one of the two numbers has been found.
 *       In this case, the SBF block is returned in the SBFBlock argument
 *   -1  if no block could be found.
 */
{
  bool     BlockFound;
  off_t    InitialFilePos;
  union
  {
    VoidBlock_t VoidBlock; /* used to align the buffer data */
    uint8_t     Data[MAX_SBFSIZE];
  } Buffer;

  int c;

  BlockFound = false;

  /* Remember the current file position */
  InitialFilePos = ssnftell(SBFData->F);

  /* Do we have to start searching from the beginning of the file, or
   * from the current file pointer? */
  if ((FilePos & START_POS_FIELD) == START_POS_SET)
    if (ssnfseek(SBFData->F, 0, SEEK_SET) != 0)
      TerminateProgram;

  do {
    /* Read the next character in the file */
    c = fgetc(SBFData->F);

    /* If it is '$', check if this is the first byte of a valid
     * block. */
    if (c==(int)SYNC_STRING[0]) {

      /* The '$' may be the first character of a valid SBF block. */
      Buffer.Data[0] = (uint8_t)c;

      /* Check if the file pointer is pointing to a valid SBF block. */
      if ((CheckBlock(SBFData, Buffer.Data) == 0) &&
          ((BlockNumber1 == BLOCKNUMBER_ALL)     ||
           (BlockNumber1 == SBF_ID_TO_NUMBER(Buffer.VoidBlock.ID)) ||
           (BlockNumber2 == BLOCKNUMBER_ALL)     ||
           (BlockNumber2 == SBF_ID_TO_NUMBER(Buffer.VoidBlock.ID)))
          ) {

        /* Valid block found, remember it. */
        BlockFound = true;

        /* Copy the block contents to the SBFBlock pointer. */
        memcpy(SBFBlock, Buffer.Data, (size_t)(Buffer.VoidBlock.Length));

      }
    }

  } while ((c != EOF) && (BlockFound == false));

  /* If the file position has to be maintained, or no block was found,
   * set the file pointer to its original value. */
  if (((FilePos & END_POS_FIELD) == END_POS_DONT_CHANGE)
      || (BlockFound == false)) {
    if (ssnfseek(SBFData->F, InitialFilePos, SEEK_SET) != 0)
      TerminateProgram;
  }

  /*@-mustdefine@*/
  return (BlockFound) ? 0 : -1;
  /*@=mustdefine@*/

}


/*---------------------------------------------------------------------------*/
void InitializeSBFDecoding(char *FileName,
                           SBFData_t* SBFData)

/* Initialize the reading of a SBF file, and prepare the SBFData_t
 * structure which is used by most of the functions in this file.
 *
 * Arguments:
 *   *FileName: The name of the SBF file which has to be read.
 *              NULL if SBF is not retrieved from a file.
 *
 *   *SBFData:  A pointer to a SBFData_t structure. The fields
 *              in this structure are initialized by this function.
 *              The FirstSec, LastSec and Interval fields of SBFData
 *              are set to -1.0 if irrelevant (i.e. when no
 *              measurement epoch is found in the file, or only one)
 *
 * Return : none
 */

{
  int ant;
  
  intCRCErrors = 0;

  memset(SBFData, 0, sizeof(SBFData_t));

  if (FileName != NULL)
  {
    /* Open the SBF file in read/binary mode. */
    if ((SBFData->F = fopen(FileName, "rb")) == NULL)
      TerminateProgram;
  }
  // otherwise, SBFData->F remains NULL from memset

  for (ant=0; ant<NR_OF_ANTENNAS; ant++) { SBFData->RefEpoch[ant].TOW_ms = U32_NOTVALID; }

  SBFData->MeasCollect_CurrentTOW = U32_NOTVALID;
  SBFData->TOWAtLastMeasEpoch     = U32_NOTVALID;

  return;
}

/*---------------------------------------------------------------------------*/
void CloseSBFFile(SBFData_t* SBFData)
{
  /* Close the SBF file. */
  if (fclose(SBFData->F) !=0)
    TerminateProgram;

}

/*---------------------------------------------------------------------------*/
double GetWavelength_m(SignalType_t SignalType, int GLOfn)
/* returns the carrier wavelength corresponding to a given Type field
 * in a MeasEpoch sub-block.
 *
 * Arguments:
 *   Type: the Type field as provided in the type-1 or type-2 sub-blocks.
 *
 *   GLOfn: GLONASS frequency number (from -7 to 12).  This argument
 *          is ignored for non-GLONASS signals.
 *
 */
{
  double wavelength;

  switch (SignalType)
  {
    case SIG_GPSL1CA:
    case SIG_GPSL1P:
    case SIG_GPSL1C:
    case SIG_GALE1A:
    case SIG_GALE1BC:
    case SIG_SBSL1CA:
    case SIG_QZSL1CA :
    case SIG_QZSL1C:
    case SIG_QZSL1S:
    case SIG_BDSB1C :
      wavelength = L1WAVELENGTH;
      break;
    case SIG_GPSL2P:
    case SIG_GPSL2C:
    case SIG_QZSL2C :
      wavelength = L2WAVELENGTH;
      break;
    case SIG_GALE5:
      wavelength = E5WAVELENGTH;
      break;
    case SIG_GALE5a:
      wavelength = E5aWAVELENGTH;
      break;
    case SIG_GALE5b:
    case SIG_BDSB2I:
      wavelength = E5bWAVELENGTH;
      break;
    case SIG_GALE6BC:
    case SIG_GALE6A:
    case SIG_QZSL6:
      wavelength = E6WAVELENGTH;
      break;
    case SIG_GPSL5:
    case SIG_SBSL5:
    case SIG_QZSL5:
    case SIG_IRNL5:
    case SIG_BDSB2a:
      wavelength = L5WAVELENGTH;
      break;
    case SIG_BDSB1I:
      wavelength = E2WAVELENGTH;
      break;
    case SIG_BDSB3:
      wavelength = B3WAVELENGTH;
      break;
    case SIG_GLOL1CA:
    case SIG_GLOL1P:
      wavelength = c84/(L1GLOFREQ+GLOfn*562500);
      break;
    case SIG_GLOL2CA:
    case SIG_GLOL2P:
      wavelength = c84/(L2GLOFREQ+GLOfn*437500);
      break;
    case SIG_GLOL3:
      wavelength = L3WAVELENGTH;
      break;
    default:
      wavelength = L1WAVELENGTH;
      break;
  }
  return(wavelength);
}

/*---------------------------------------------------------------------------*/

static int Type1Counter = 0;

/*---------------------------------------------------------------------------*/
MeasEpochChannelType1_t*
GetFirstType1SubBlock(const MeasEpoch_2_t *MeasEpoch)
/* returns a pointer to the first type-1 sub-block of a MeasEpoch SBF block.
 *
 * Arguments:
 *   *MeasEpoch: a pointer to a MeasEpoch_2 or GenMeasEpoch_1 SBF block
 *
 * Return : a pointer to the first type-1 sub-block, or NULL if there is
 *          no sub-block in this MeasEpoch
 */
{
  Type1Counter = 0;
  if (MeasEpoch->N != 0) {
    return (MeasEpochChannelType1_t*)(MeasEpoch->Data);
  } else {
    return NULL;
  }
}

/*---------------------------------------------------------------------------*/
MeasEpochChannelType1_t*
GetNextType1SubBlock(const MeasEpoch_2_t     *MeasEpoch,
                     MeasEpochChannelType1_t *CurrentType1SubBlock)
/* returns a pointer to the next type-1 sub-block of a MeasEpoch_2 SBF block.
 *
 * Arguments:
 *   *MeasEpoch: a pointer to a MeasEpoch_2 or GenMeasEpoch_1 SBF block
 *
 *   *CurrentType1SubBlock: a pointer to a type-1 sub-block in the
 *          MeasEpoch.  The function will return a pointer to the type-1
 *          sub-block following the one pointed to by this argument.
 *
 * Return : a pointer to the next type-1 sub-block, or NULL if there is
 *          no next sub-block in this MeasEpoch (i.e. CurrentType1SubBlock
 *          points to the last type-1 sub-block of the MeasEpoch).
 */
{
  if (++Type1Counter < MeasEpoch->N) {
    return (MeasEpochChannelType1_t*)
      ((uint8_t*)CurrentType1SubBlock
       + CurrentType1SubBlock->N_Type2*MeasEpoch->SB2Size
       + MeasEpoch->SB1Size);
  } else {
    return NULL;
  }

}
