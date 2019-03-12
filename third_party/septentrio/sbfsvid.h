/*
 * sbfsvid.h: Declaration of the functions to transform SVID number in
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

#ifndef SBFSVID_H
#define SBFSVID_H   1

#include "ssntypes.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! Convert the SBF SVID numbering into the internal SVID numbering .
 */
uint32_t FW_EXPORT convertSVIDfromSBF(uint32_t sbfSVID);

/*! Convert the internal SVID numbering into SBF SVID numbering
 */
uint8_t FW_EXPORT convertSVIDtoSBF(uint32_t internalSVID);

#ifdef __cplusplus
}
#endif

#endif
