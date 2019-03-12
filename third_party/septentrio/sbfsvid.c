/*
 * sbfsvid.c: Definition of the functions to transform SVID number in
 * SBF to SVID numbering the MeasEpoch_t structure.
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

#include "sbfsvid.h"
#include "sviddef.h"

#define SBFSVID_GPS_MIN  (  1)
#define SBFSVID_GPS_MAX  ( 37)

#define SBFSVID_GLO_MIN1 ( 38)
#define SBFSVID_GLO_MAX1 ( 61)
#define SBFSVID_GLO_UNK  ( 62)
#define SBFSVID_GLO_MIN2 ( 63)
#define SBFSVID_GLO_MAX2 ( 70)
#define SBFSVID_GLO_GAP1 (SBFSVID_GLO_MIN2-SBFSVID_GLO_MAX1-1)

#define SBFSVID_GAL_MIN  ( 71)
#define SBFSVID_GAL_MAX  (106)

#define SBFSVID_LBR_MIN  (107)
#define SBFSVID_LBR_MAX  (118)
#define SBFSVID_LBR_UNK  (119)

#define SBFSVID_RA_MIN1  (120)
#define SBFSVID_RA_MAX1  (140)
#define SBFSVID_RA_MIN2  (198)
#define SBFSVID_RA_MAX2  (215)
#define SBFSVID_RA_GAP1  (SBFSVID_RA_MIN2-SBFSVID_RA_MAX1-1)

#define SBFSVID_BDS_MIN  (141)
#define SBFSVID_BDS_MAX  (177)

#define SBFSVID_QZSS_MIN (181)
#define SBFSVID_QZSS_MAX (187)

#define SBFSVID_IRNS_MIN (191)
#define SBFSVID_IRNS_MAX (197)

#define SBFSVID_INVALID  (255)

uint32_t convertSVIDfromSBF(uint32_t sbfSVID)
{
  uint32_t internalSBF = 0;

  // GPS
  if (sbfSVID >= SBFSVID_GPS_MIN && sbfSVID <= SBFSVID_GPS_MAX)
  {
    internalSBF = (uint32_t)(sbfSVID+(gpMINPRN-SBFSVID_GPS_MIN));
  }
  // GLO
  if (sbfSVID >= SBFSVID_GLO_MIN1 && sbfSVID <= SBFSVID_GLO_MAX1)
  {
    internalSBF = (uint32_t)(sbfSVID+(glMINPRN-SBFSVID_GLO_MIN1));
  }
  if (sbfSVID == SBFSVID_GLO_UNK)
  {
    internalSBF = (uint32_t)glPRNUNKWN;
  }
  if (sbfSVID >= SBFSVID_GLO_MIN2 && sbfSVID <= SBFSVID_GLO_MAX2)
  {
    internalSBF = (uint32_t)(sbfSVID+(glMINPRN-SBFSVID_GLO_MIN1-SBFSVID_GLO_GAP1));
  }
  // GAL
  if (sbfSVID >= SBFSVID_GAL_MIN && sbfSVID <= SBFSVID_GAL_MAX)
  {
    internalSBF = (uint32_t)(sbfSVID+(galMINPRN-SBFSVID_GAL_MIN));
  }
  // LBAND
  if (sbfSVID >= SBFSVID_LBR_MIN && sbfSVID <= SBFSVID_LBR_MAX)
  {
    internalSBF = (uint32_t)(sbfSVID+(lbMINPRN-SBFSVID_LBR_MIN));
  }
  if (sbfSVID == SBFSVID_LBR_UNK)
  {
    internalSBF = (uint32_t)lbPRNUNKWN;
  }
  // SBAS
  if (sbfSVID >= SBFSVID_RA_MIN1 && sbfSVID <= SBFSVID_RA_MAX1)
  {
    internalSBF = (uint32_t)(sbfSVID+(raMINPRN-SBFSVID_RA_MIN1));
  }
  if (sbfSVID >= SBFSVID_RA_MIN2 && sbfSVID <= SBFSVID_RA_MAX2)
  {
    internalSBF = (uint32_t)(sbfSVID+(raMINPRN-SBFSVID_RA_MIN1-SBFSVID_RA_GAP1));
  }
  // BDS
  if (sbfSVID >= SBFSVID_BDS_MIN && sbfSVID <= SBFSVID_BDS_MAX)
  {
    internalSBF = (uint32_t)(sbfSVID+(cmpMINPRN-SBFSVID_BDS_MIN));
  }
  // QZSS
  if (sbfSVID >= SBFSVID_QZSS_MIN && sbfSVID <= SBFSVID_QZSS_MAX)
  {
    internalSBF = (uint32_t)(sbfSVID+(qzMINPRN-SBFSVID_QZSS_MIN));
  }
  // IRNSS
  if (sbfSVID >= SBFSVID_IRNS_MIN && sbfSVID <= SBFSVID_IRNS_MAX)
  {
    internalSBF = (uint32_t)(sbfSVID+(irMINPRN-SBFSVID_IRNS_MIN));
  }

  return internalSBF;
}


uint8_t convertSVIDtoSBF(uint32_t internalSVID)
{
  uint8_t sbfSVID = SBFSVID_INVALID;

  if (agIsGPS(internalSVID))
  {
    sbfSVID = (uint8_t)(internalSVID-gpMINPRN+SBFSVID_GPS_MIN);
  }
  else if (agIsGLONASS(internalSVID))
  {
    sbfSVID = (uint8_t)(internalSVID-glMINPRN+SBFSVID_GLO_MIN1);
    /* this is for the GLONASS slots > 24 */
    if (internalSVID >= glMINPRN+24 && internalSVID <(int)glPRNUNKWN)
    {
      sbfSVID += SBFSVID_GLO_GAP1;
    }
    if (internalSVID == (int)glPRNUNKWN)
    {
      sbfSVID = (uint8_t)SBFSVID_GLO_UNK;
    }
  }
  else if (agIsGAL(internalSVID))
  {
    sbfSVID = (uint8_t)(internalSVID-galMINPRN+SBFSVID_GAL_MIN);
  }
  else if (agIsLBR(internalSVID))
  {
    sbfSVID = (uint8_t)(internalSVID-lbMINPRN+SBFSVID_LBR_MIN);
    if (internalSVID == (int)lbPRNUNKWN)
    {
      sbfSVID = (uint8_t)SBFSVID_LBR_UNK;
    }
  }
  else if (agIsRA(internalSVID))
  {
    sbfSVID = (uint8_t)(internalSVID-raMINPRN+SBFSVID_RA_MIN1);
    /* this is the second part of the SBAS PRNS, remapped in SBF in the 198-215 range */
    if (internalSVID >= raMINPRN+21)
    {
      sbfSVID += SBFSVID_RA_GAP1;
    }
  }
  else if (agIsCOMPASS(internalSVID))
  {
    sbfSVID = (uint8_t)(internalSVID-cmpMINPRN+SBFSVID_BDS_MIN);
  }
  else if (agIsQZSS(internalSVID))
  {
    sbfSVID = (uint8_t)(internalSVID-qzMINPRN+SBFSVID_QZSS_MIN);
  }
  else if (agIsIRNSS(internalSVID))
  {
    sbfSVID = (uint8_t)(internalSVID-irMINPRN+SBFSVID_IRNS_MIN);
  }

  return sbfSVID;
}
