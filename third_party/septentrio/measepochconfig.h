/*
 * measepochconfig.h: definition of number of elements in the
 * MeasEpoch_t structure.
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

#ifndef MEASEPOCHCONFIG_DEFAULT_H
#define MEASEPOCHCONFIG_DEFAULT_H 1

/* when converting a Meas3 SBF block into MeasEpoch_t, support up to
   100 logical channels (a logical channel contains all measurements
   for a given satellite), 2 antennas (MAIN and AUX) and up to 8
   signals per satellite. */
#define NR_OF_LOGICALCHANNELS           100
#define NR_OF_ANTENNAS                    2
#define MAX_NR_OF_SIGNALS_PER_SATELLITE   8


#endif
