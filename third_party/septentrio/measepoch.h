/*
 * measepoch.h: definition of the MeasEpoch_t structure containing all
 * GNSS observables at a given epoch.
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

#ifndef MEASEPOCH_H
#define MEASEPOCH_H 1

#include "ssntypes.h"
#include "sviddef.h"
#include "measepochconfig.h"

/* definition of bits in the MeasEpoch_t commonFlags bit field */

#define COMMONFLAG_MULTIPATHMITIGATION (uint8_t)(1<<0) /**< set when
                                                 the multipath
                                                 mitigation is
                                                 enabled */

#define COMMONFLAG_ATLEASTONESMOOTHING (uint8_t)(1<<1) /**< set when
                                                 at least one of the
                                                 pseudoranges of this
                                                 epoch is smoothed. */

#define COMMONFLAG_CLOCKSTEERINGACTIVE (uint8_t)(1<<3) /**< set when
                                                clock steering is
                                                active.*/

#define COMMONFLAG_HIGHDYNAMICSMODE  (uint8_t)(1<<7)  /**< set when
                                                the receiver is in
                                                high-dynamics mode */


/* definition of bits in the MeasSet_t flags bit field */

#define MEASFLAG_SMOOTHING   (uint8_t)(1<<0) /**< this bit is set when
                                                the PR_m field is
                                                smoothed. */

#define MEASFLAG_HALFCYCLEAMBIGUITY (uint8_t)(1<<2) /** set when the
                                                phase measurement is
                                                known with an
                                                ambiguity of 1/2
                                                cycles (typically the
                                                at the beginning of
                                                GPS-CA tracking) */

#define MEASFLAG_FIRSTMEAS   (uint8_t)(1<<3) /**< this bit is set for
                                                the first measurement
                                                epoch after a (re)lock
                                                (not set when reading
                                                data from SBF file)*/

#define MEASFLAG_APMEINSYNC (uint8_t)(1<<5) /**< set when the
                                               multipath mitigation
                                               algorithm (APME) is in
                                               sync phase, meaning
                                               that multipath
                                               mitigation has not
                                               reached its full
                                               potential yet.  The
                                               APME sync phase takes a
                                               few seconds after
                                               signal acquisition. */

#define MEASFLAG_VALIDITY    (uint8_t)(1<<7) /**< this bit is set when
                                                this measSet is valid.
                                                A measSet may be
                                                invalid because a
                                                particular signal is
                                                not tracked, or
                                                because the smoothing
                                                alignment period is
                                                not elapsed yet, or
                                                because the C/NO is
                                                under the mask.  Note
                                                that if this bit is 0,
                                                all the other bits of
                                                the flags field are
                                                set to 0 as well,
                                                hence the meas can be
                                                considered as valid
                                                when (flags!=0) */


#define DEFAULT_DOPPLER_VARIANCE_FACTOR    163



/** level of time synchronization */
typedef enum {
  RXTOWSTATUS_INVALID = -1,
  RXTOWSTATUS_NOTSET = 0,
  RXTOWSTATUS_COARSE,
  RXTOWSTATUS_PRECISE
} RxTOWStatus_t;


/*----------------------------------------------------------------------------*/
/** structure containing all the observables from a given signal and a
    given antenna.  This is referred to as a "measurement set" */
typedef struct {
  double         PR_m;           /**< Pseudorange, in meters, or
                                      F64_NOTVALID if unknown */
  double         L_cycles;       /**< carrier phase measurement in cycles, or
                                      F64_NOTVALID if unknown */
  float          doppler_Hz;     /**< Doppler in Hz, or F32_NOTVALID if unknown */
  float          CN0_dBHz;       /**< C/N0 in dB-Hz, or F32_NOTVALID
                                      if unknown */
  uint32_t       PLLTimer_ms;    /**< Lock time of the PLL, in ms, or
                                      0 if unknown */
  uint8_t        signalType;     /**< signal identification (has SignalType_t
                                      type, but cast to uint8_t) */
  uint8_t        flags;          /**< bit field containing various
                                      status flags, defined by the
                                      MEASFLAG_xx macros hereabove */
  int16_t        MP_mm;          /**< estimate of the code multipath error
                                      in mm, or 0 if multipath
                                      mitigation is disabled.  Note
                                      that if multipath mitigation is
                                      enabled, the PR_m field is
                                      already corrected for the
                                      multipath error.  This field is
                                      only provided to allow going
                                      back to the "raw" pseudorange,
                                      before the multipath correction
                                      was applied: add MP_mm
                                      to PR_m to get the raw
                                      pseudorange. */
  uint8_t       InternalFlags;   /**  flags used during the generation
                                      of the measSet.  To be ignored. */
  int8_t        CarrierMP_1_512c;/**< estimate of the carrier multipath error
                                      in 1/512 cycles, or 0 if multipath
                                      mitigation is disabled.  Note
                                      that if multipath mitigation is
                                      enabled, the L_cycles field is
                                      already corrected for the
                                      multipath error.  This field is
                                      only provided to allow going
                                      back to the "raw" carrier phase,
                                      before the multipath correction
                                      was applied: add CarrierMP_1_512c
                                      to L_cycles to get the raw carrier phase.
                                   */
  int16_t        SmoothingCorr_mm; /**< smoothing correction in mm.
                                      To be added to PR_m to revert back to the
                                      unsmoothed pseudorange. */
  uint8_t        lockCount;        /**< lock counter (modulo 256) for
                                      the PRN/signalType pair
                                      corresponding to this measSet.
                                      This counter starts at receiver
                                      start-up and is not reset
                                      between satellite passes.  It is
                                      incremented each time a cycle
                                      slip is detected, and at each
                                      reacquisition */

  uint8_t        rawCN0_dBHz;      /**< Unfiltered version of the CN0, in dB-Hz */
         
  float          PRvariance_m2;     /**< Pseudorange variance, in meters^2 */
  float          Lvariance_cycles2; /**< carrier phase variance, in
                                       cycles^2.  The Doppler
                                       variance, in Hz^2, can be
                                       computed by multiplying this
                                       value by the dopplerVarFactor
                                       field in MeasEpoch_t. */
  
#ifdef  MEASSET_PRIVATEFIELDS
  MEASSET_PRIVATEFIELDS
#endif

} MeasSet_t;


/** structure containing all the measurements from a logical
    channel. A logical channel contains all measurements from a given
    satellite, i.e. all measurements from all antennas connected to
    the receiver and from all signals tracked for that satellite.*/
typedef struct {
  uint8_t     channel;  /**< logical channel number, starting at 0 */
  uint8_t     PRN;      /**< SVID, see numbering convention in sviddef.h */
  uint8_t     fnPlus8;  /**< frequency number of a GLONASS SV +8, or 0
                           for non-GLONASS SVs.  e.g. if fnPlus8 is 1,
                           the actual frequency number is -7 */

  /** a logical channel can contain a lot of sub-channels, each
      providing a measurement sets: there is space reserved for all
      types of signals and all types of antennas.  The measurement
      sets are arranged as a two-dimensional array.  The first index
      corresponds to the antenna (0 for the main antenna), and the
      second correspond to the different signals.  The signalType
      field in each MeasSet indicates which signal type a given
      measSet is referring to */
  MeasSet_t    measSet[NR_OF_ANTENNAS][MAX_NR_OF_SIGNALS_PER_SATELLITE];
} MeasChannel_t;

/** structure containing all the measurements from a measurement
    epoch. */
typedef struct {
  uint32_t      TOW_ms;             /**< current TOW in ms;
                                         set to U32_NOTVALID if not known */
  uint16_t      WNc;                /**< Week number (continuous) associated with
                                      TOW_ms; set to U16_NOTVALID if
                                      not known */

  RxTOWStatus_t rxTOWStatus;        /**< rxTOW sync level */
  bool          IsWNcSet;           /**< true when the week number is
                                        set. */

  int32_t        totalClockJump_ms;  /**< accumulated integer clock
                                        jump since start of the
                                        receiver (in ms).  Sign
                                        convention: an increase of
                                        this value corresponds to
                                        advancing the receiver time.
                                        Hence, after a positive
                                        increment of
                                        totalClockJump_ms, the pseudoranges
                                        increase and the ClockBias computed 
                                        by the PVT increases.  */

  float          dopplerVarFactor;  /**< factor to go from the
                                        variance of the carrier phase
                                        measurement, in cycles^2, to
                                        the variance of the Doppler
                                        measurement, in Hz^2. */

  uint8_t        commonFlags;       /**< bit field with flags as
                                       defined with the COMMONFLAG_XX
                                       macros above */

  uint32_t       nbrElements;        /**< number of ChannelData in this
                                        epoch */

#ifdef  MEASEPOCH_PRIVATEFIELDS
  MEASEPOCH_PRIVATEFIELDS
#endif

  MeasChannel_t  channelData[NR_OF_LOGICALCHANNELS];
} MeasEpoch_t;


#endif
