/*
 * sbfdef.h: Declarations and definitions of SBF (Septentrio Binary
 * Format) blocks.
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

#ifndef SBFDEF_H
#define SBFDEF_H 1

#include "ssntypes.h"

#if defined(__GNUC__) || defined(__ARMCC__)
/* Before the advent of the CPMF platform, double data types were always
 * 32-bit aligned. On the CPMF, double data types are 64-bit aligned. The
 * "packed, aligned" attribute combination is necessary to enforce 32-bit
 * alignment for double data types and to port the sbf encoding/decoding 
 * functionality to the CPMF.
 */
#  define SBFDOUBLE double __attribute__((packed, aligned(4)))
#else
#  define SBFDOUBLE double
#endif

/* Force packing the structs on 4-byte alignment (needed for GCC 64 bit compilations) */
#pragma pack(push,4)


#define RTCM16MSG_LENGTH            90LU
#define CMR2SHORTID_LENGTH          8LU
#define CMR2COGO_LENGTH             16LU
#define CMR2LONGID_LENGTH           51LU
#define EXTSENSORSERIALNBR_LENGTH   20
#define EXTSENSORVERSION_LENGTH     20

/* Use the max values if they are not defined yet */
#ifndef NR_OF_LOGICALCHANNELS
/* Using maximum value of NR_OF_LOGICALCHANNELS for SBF definitions */
#define NR_OF_LOGICALCHANNELS  72
#endif

#ifndef MAX_NB_INMARSATCHANNELS
#define MAX_NB_INMARSATCHANNELS 1
#endif

#ifndef MAX_NR_OF_SIGNALS_PER_SATELLITE
/* Using maximum value of MAX_NR_OF_SIGNALS_PER_SATELLITE for SBF definitions */
#define MAX_NR_OF_SIGNALS_PER_SATELLITE 7
#endif

#ifndef NAVSUPP_NBR_ACTIVESAT
/* Using maximum value of NAVSUPP_NBR_ACTIVESAT for SBF definitions */
#define NAVSUPP_NBR_ACTIVESAT NR_OF_LOGICALCHANNELS
#endif

#ifndef NR_OF_ANTENNAS
/* Using maximum value of NR_OF_ANTENNAS for SBF definitions */
#define NR_OF_ANTENNAS 3
#endif

#ifndef NAVSUPP_NBR_ACTIVESIG
/* Using maximum value of NAVSUPP_NBR_ACTIVESIG for SBF definitions */
#define NAVSUPP_NBR_ACTIVESIG MAX_NR_OF_SIGNALS_PER_SATELLITE
#endif

#ifndef MAX_NR_SATSYSTEMS
/* Using maximum value of MAX_NR_SATSYSTEMS for SBF definitions */
#define MAX_NR_SATSYSTEMS 5
#endif

/* Max Nr of Subblocks for different SBF messages */
#define MAXSB_MEASEPOCH_T1       (NR_OF_LOGICALCHANNELS + MAX_NB_INMARSATCHANNELS)
#define MAXSB_MEASEPOCH_T2       ((MAXSB_MEASEPOCH_T1) * ( ((MAX_NR_OF_SIGNALS_PER_SATELLITE) * (NR_OF_ANTENNAS)) -1))
#define MAXSB_NBRANTENNA         4
#define MAXSB_CHANNELSATINFO     (NR_OF_LOGICALCHANNELS + MAX_NB_INMARSATCHANNELS)
#define MAXSB_CHANNELSTATEINFO   (MAXSB_CHANNELSATINFO * MAXSB_NBRANTENNA)
#define MAXSB_SATOBS             (NAVSUPP_NBR_ACTIVESAT * NR_OF_ANTENNAS * NAVSUPP_NBR_ACTIVESIG)
#define MAXSB_SATRES             3
#define MAXSB_CONNDESCR          16
#define MAXSB_DATATYPES          16

#ifndef I8_NOTVALID
# define  I8_NOTVALID   (int8_t)  (0x80)
#endif
#ifndef UI8_NOTVALID
# define UI8_NOTVALID   (uint8_t) (0xFF)
#endif
#ifndef I16_NOTVALID
# define I16_NOTVALID   (int16_t) (0x8000)
#endif
#ifndef U16_NOTVALID
# define U16_NOTVALID   (uint16_t)(0xFFFF)
#endif
#ifndef I32_NOTVALID
# define I32_NOTVALID   (int32_t) (0x80000000)
#endif
#ifndef U32_NOTVALID
# define U32_NOTVALID   (uint32_t)(0xFFFFFFFF)
#endif
#ifndef I64_NOTVALID
# define I64_NOTVALID   (int64_t) (0x8000000000000000LL)
#endif
#ifndef U64_NOTVALID
# define U64_NOTVALID   (uint64_t)(0xFFFFFFFFFFFFFFFFULL)
#endif
#ifndef F32_NOTVALID
# define F32_NOTVALID             (-2e10F)
#endif
#ifndef F64_NOTVALID
# define F64_NOTVALID             (-2e10)
#endif

/*--BeiDou types, mapped to their GPS counterparts : ------------------------*/
#define cmpAlm_t gpAlm_t


/*==SBF-IDs==============================================================*/
/* minor version are indicated in first 3 bits */
#define SBF_ID_REVISION_0 (0x0000)
#define SBF_ID_REVISION_1 (0x2000)
#define SBF_ID_REVISION_2 (0x4000)
#define SBF_ID_REVISION_3 (0x6000)
#define SBF_ID_REVISION_4 (0x8000)
#define SBF_ID_REVISION_5 (0xA000)
#define SBF_ID_REVISION_6 (0xC000)
#define SBF_ID_REVISION_7 (0xE000)

#define SBF_ID_TO_NUMBER(a)   ( ((uint16_t)(a)) & 0x1fff )
#define SBF_ID_TO_REV(a)      ( ((uint16_t)(a)) >> 13 )

#define MIN_SBFSIZE    8       /* minimum size of a SBF block in bytes */
#define MAX_SBFSIZE    65535   /* maximum size of a SBF block in bytes */


/*==CONNECTION DESCRIPTOR definition=========================================*/
#define CD_TYPE_COM       (0x00)
#define CD_TYPE_USB       (0x20)
#define CD_TYPE_IP        (0x40)
#define CD_TYPE_DSK       (0x60)
#define CD_TYPE_NTR       (0x80)
#define CD_TYPE_IPS       (0xA0)
#define CD_TYPE_RESERVED3 (0xC0)
#define CD_TYPE_RESERVED4 (0xE0)


/*==DATA TYPE definition=====================================================*/
#define DATA_TYPE_UNKNOWN   (0x00)
#define DATA_TYPE_SEPT      (0x20)
#define DATA_TYPE_NMEA      (0x40)
#define DATA_TYPE_DIFFCORR  (0x60)
#define DATA_TYPE_IMU       (0x80)
#define DATA_TYPE_RESERVED1 (0xA0) /* Proper naming to be found */
#define DATA_TYPE_RESERVED2 (0xC0)
#define DATA_TYPE_RESERVED3 (0xE0)


/*==Measurement Blocks=======================================================*/

/*--GenMeasEpoch_1_0_t : ----------------------------------------------------*/
#define SBF_GENMEASEPOCH_1_0_DATA_LENGTH (MAXSB_MEASEPOCH_T1 * sizeof(MeasEpochChannelType1_1_0_t) + \
                            MAXSB_MEASEPOCH_T2 * sizeof(MeasEpochChannelType2_1_0_t))


/*--MeasEpoch_2_0_t : -------------------------------------------------------*/
#define SBF_MEASEPOCH_2_0_DATA_LENGTH SBF_GENMEASEPOCH_1_0_DATA_LENGTH


/*--MeasEpoch_2_1_t : -------------------------------------------------------*/
#define SBF_MEASEPOCH_2_1_DATA_LENGTH SBF_GENMEASEPOCH_1_0_DATA_LENGTH

#define SBF_MEASEPOCH_2_COMMONFLAG_MULTIPATHMITIGATION (uint8_t)(1<<0)  // = COMMONFLAG_MULTIPATHMITIGATION (same as in tracker)
#define SBF_MEASEPOCH_2_COMMONFLAG_ATLEASTONESMOOTHING (uint8_t)(1<<1)  // = COMMONFLAG_ATLEASTONESMOOTHING (same as in tracker)
#define SBF_MEASEPOCH_2_COMMONFLAG_CARRIERPHASEALIGN   (uint8_t)(1<<2)  
#define SBF_MEASEPOCH_2_COMMONFLAG_CLOCKSTEERINGACTIVE (uint8_t)(1<<3)  // = COMMONFLAG_CLOCKSTEERINGACTIVE (same as in tracker)
#define SBF_MEASEPOCH_2_COMMONFLAG_HIGHDYNAMICSMODE     (uint8_t)(1<<5)  

/*--MeasExtra_1_0_t : -------------------------------------------------------*/
#define SBF_MEASEXTRA_1_0_CHANNELSUB_LENGTH (MAXSB_MEASEPOCH_T1 + MAXSB_MEASEPOCH_T2)


/*--MeasExtra_1_1_t : -------------------------------------------------------*/
#define SBF_MEASEXTRA_1_1_CHANNELSUB_LENGTH (MAXSB_MEASEPOCH_T1 + MAXSB_MEASEPOCH_T2)


/*--MeasExtra_1_2_t : -------------------------------------------------------*/
#define SBF_MEASEXTRA_1_2_CHANNELSUB_LENGTH (MAXSB_MEASEPOCH_T1 + MAXSB_MEASEPOCH_T2)

/*--MeasExtra_1_3_t : -------------------------------------------------------*/
#define SBF_MEASEXTRA_1_3_CHANNELSUB_LENGTH (MAXSB_MEASEPOCH_T1 + MAXSB_MEASEPOCH_T2)


/*--IQCorr_1_0_t : ----------------------------------------------------------*/
#define SBF_IQCORR_1_0_CHANNELSUB_LENGTH  (MAXSB_MEASEPOCH_T1 + MAXSB_MEASEPOCH_T2)


/*--IQCorr_1_1_t : ----------------------------------------------------------*/
#define SBF_IQCORR_1_1_CHANNELSUB_LENGTH  SBF_IQCORR_1_0_CHANNELSUB_LENGTH


/*--SQMSamples_1_0_t : ------------------------------------------------------*/
#define SBF_SQMSAMPLES_1_0_DATA_LENGTH    8172


/*==Galileo Decoded Message Blocks===========================================*/
#define  SBF_GAL_SOURCE_GNAV_L1A     1
#define  SBF_GAL_SOURCE_GNAV_E6A     4
#define  SBF_GAL_SOURCE_GNAV_MERGED  5
#define  SBF_GAL_SOURCE_INAV         2
#define  SBF_GAL_SOURCE_FNAV        16



/*==Position, Velocity and Time Blocks=======================================*/
#define SBF_PVTERR_NONE                       0 /*!< no error */
#define SBF_PVTERR_NOTENOUGHMEAS              1 /*!< not enough measurements */
#define SBF_PVTERR_NOTENOUGHEPH               2 /*!< not enough ephemerides */
#define SBF_PVTERR_DOPTOOHIGH                 3 /*!< DOP too large (larger than 15) */
#define SBF_PVTERR_ETETOOLARGE                4 /*!< sum of sqrd residuals too big */
#define SBF_PVTERR_NOCONVERGENCE              5 /*!< no convergence  */
#define SBF_PVTERR_TOOMANYOUTLIERS            6 /*!< not enough measurements after 
                                                     outlier rejection  */
#define SBF_PVTERR_EXCEEDNATOLIMITS           7 /*!< position output prohibited 
                                                     due to export laws  */
#define SBF_PVTERR_NOTENOUGHDIFFCORR          8 /*!< not enough differential 
                                                     corrections available  */
#define SBF_PVTERR_NOBASEAVL                  9 /*!< base station coordinates 
                                                     unavailable */
#define SBF_PVTERR_AMBIGUITIESNOTFIXED       10 /*!< Error flag only used when 
                                                     RTKfixed only is requested
                                                     and receiver is still in RTK 
                                                     float mode */
#define SBF_PVTERR_NOTRANSFOPARAMS           17 /*!< Datum transformation parameters
                                                     unknown */
#define SBF_PVTERR_INSNOTREQUESTEDBYUSER     20 /*!< Error flag only used for integrated
                                                     loosely-coupled GNSS/IMU PVT. This
                                                     error is provided when the user has
                                                     selected 'ExtSensorIntegration off'
                                                     in the SPM command. */
#define SBF_PVTERR_NOTENOUGHEXTSENSORMEAS    21 /*!< Not enough external sensor measurements
                                                     available for integrated PVA */
#define SBF_PVTERR_CALIBRATIONNOTREADY       22 /*!< External sensor calibration (automatic
                                                     orientation and lever-arm detection)
                                                     is still ongoing */
#define SBF_PVTERR_ALIGNMENTNOTREADY         23 /*!< External sensor static alignment (coarse
                                                     alignment) is still ongoing */
#define SBF_PVTERR_WAITINGFORGNSSPVT         24 /*!< Waiting for GNSS PVT solution */
#define SBF_PVTERR_FINETIMENOTREACHED        27 /*!< FINETIME mode not reached yet */
#define SBF_PVTERR_INMOTIONALIGNMENTNOTREADY 28 /*!< External sensor in motion alignment is 
                                                     still ongoing */
#define SBF_PVTERR_WAITINGFORGNSSHEAD        29 /*!< Waiting for GNSS heading
                                                     solution */
#define SBF_PVTERR_WAITINGFORPPSSYNC         30 /*!< External sensor waiting to
                                                     sync with PPS from
                                                     receiver */
#define SBF_PVTERR_STDLIMITEXCEEDED          31 /*!< Error flag if standard deviation of
                                                     position/attitude solution exceeds
                                                     user limit */

/* Defines for GNSS PVT mode */
#define MODE_NO_PVT_AVAILABLE               (0x00)
#define MODE_STAND_ALONE_PVT                (0x01)
#define MODE_DIFFERENTIAL_PVT               (0x02)
#define MODE_FIXED_LOCATION                 (0x03)
#define MODE_RTK_FIXED_AMBIGUITIES          (0x04)
#define MODE_RTK_FLOAT_AMBIGUITIES          (0x05)
#define MODE_SBAS_AIDED_PVT                 (0x06)
#define MODE_MOVBASERTK_FIXED_AMBIGUITIES   (0x07)
#define MODE_MOVBASERTK_FLOAT_AMBIGUITIES   (0x08)
#define MODE_PPP_FIXED_AMBIGUITIES          (0x09)
#define MODE_PPP_FLOAT_AMBIGUITIES          (0x0A)
#define MODE_RTK_FLOAT_WL_FIXED_AMBIGUITIES (0x0B)

#define MODE_STATICAUTO_LOOKING             (0x40)
#define MODE_2D_PVT                         (0x80)

/* Defines for GNSS/INS Integrated PVT mode */
#define MODEINTPVA_NO_PVA_AVAILABLE         (0x00)
#define MODEINTPVA_EXTRAP_EXTSENSORS        (0x01)
#define MODEINTPVA_LOOSELY_INTEGRATED       (0x02)
#define MODEINTPVA_TIGHTLY_INTEGRATED       (0x03)
#define MODEINTPVA_GNSSONLY_EXTRAPOLATED    (0x04)

/* Defines for Attitude mode */
#define SBF_ATT_MODE_NOT_AVAILABLE             0
#define SBF_ATT_MODE_HEADING_PITCH_FLOAT       1
#define SBF_ATT_MODE_HEADING_PITCH_FIXED       2
#define SBF_ATT_MODE_HEADING_PITCH_ROLL_FLOAT  3
#define SBF_ATT_MODE_HEADING_PITCH_ROLL_FIXED  4

/*--PVTSatCartesian_1_0_t : -------------------------------------------------*/

#define SBF_PVTSATCARTESIAN_1_0_SATPOS_LENGTH  NR_OF_LOGICALCHANNELS

/*--PVTSatCartesian_1_1_t : -------------------------------------------------*/

#define SBF_PVTSATCARTESIAN_1_1_SATPOS_LENGTH  SBF_PVTSATCARTESIAN_1_0_SATPOS_LENGTH

#define SATPOS_IONOMODEL_NOT_APPLICABLE  0
#define SATPOS_IONOMODEL_KLOBUCHAR       1
#define SATPOS_IONOMODEL_DO229           2
#define SATPOS_IONOMODEL_NEQUICK         3
#define SATPOS_IONOMODEL_MEASURED        4
#define SATPOS_IONOMODEL_ESTIMATED       5
#define SATPOS_IONOMODEL_KLOBUCHARBEIDOU 6


/*--PVTResiduals_1_0_t : ----------------------------------------------------*/
#define SBF_PVTRESIDUALS_1_0_SATRESIDUAL_LENGTH NR_OF_LOGICALCHANNELS


/*--PVTResiduals_2_0_t : ----------------------------------------------------*/
#define SBF_PVTRESIDUALS_2_0_DATA_LENGTH  (MAX_SBFSIZE - sizeof(BlockHeader_t) - 12)


/*--PVTResiduals_2_1_t : ----------------------------------------------------*/
#define SBF_PVTRESIDUALS_2_1_DATA_LENGTH  SBF_PVTRESIDUALS_2_0_DATA_LENGTH


/*--RAIMStatistics_1_0_t : --------------------------------------------------*/
#define  RAIM_INTEGRITY_OK      (0U)
#define  RAIM_INTEGRITY_FAIL    (1U)
#define  RAIM_INTEGRITY_NOTAVL  (2U)

#define  RAIM_UI16SCALE         (50000.0)
#define  RAIM_MDB_SCALE          (1000.0)

#define SBF_RAIMSTATISTICS_1_0_RAIMCHANNEL_LENGTH  NR_OF_LOGICALCHANNELS


/*--GEOCorrections_1_0_t : --------------------------------------------------*/
#define SBF_GEOCORRECTIONS_1_0_SATCORR_LENGTH  NR_OF_LOGICALCHANNELS


/*-- PosCart_1_0_t : --------------------------------------------------------*/
#define POSCART_MISC_MASK_BASELINE_TO_ARP  (0x01)
#define POSCART_MISC_MASK_PCV_COMPENSATED  (0x02)
#define POSCART_MISC_MASK_SIGIL_ENABLED    (0x04)
#define POSCART_MISC_MASK_RTCMV_USED       (0x08)


/*==INS/GNSS Integrated Blocks===============================================*/

/*--IntPVCart_1_0_t : -------------------------------------------------------*/
#define INTPVA_INFO_HEADING_AMBIGUITY_VALIDATED  0x0800    /* Bit 11 */
#define INTPVA_INFO_MOTIONCONSTRAINTUSED         0x1000    /* Bit 12 */
#define INTPVA_INFO_GNSSPOSUSED                  0x2000    /* Bit 13 */
#define INTPVA_INFO_GNSSVELUSED                  0x4000    /* Bit 14 */
#define INTPVA_INFO_GNSSATTUSED                  0x8000    /* Bit 15 */


/*--INSNavCart_1_0_t : ------------------------------------------------------*/
#define INSNAV_INFO_OUTPUT_LOCATION_MAINANT      0x0000    /* Bit 3-5: 0 */
#define INSNAV_INFO_OUTPUT_LOCATION_POI1         0x0008    /* Bit 3-5: 1 */
#define INSNAV_INFO_HEADING_AMBIGUITY_VALIDATED  0x0040    /* Bit 6 */
#define INSNAV_INFO_MOTIONCONSTRAINTUSED         0x0080    /* Bit 7 */

#define INSNAV_SBLIST_POSITION_STTDEV            0x0001    /* Bit 0 */
#define INSNAV_SBLIST_ATTITDE                    0x0002    /* Bit 1 */
#define INSNAV_SBLIST_ATTITDE_STTDEV             0x0004    /* Bit 2 */
#define INSNAV_SBLIST_VELOCITY                   0x0008    /* Bit 3 */
#define INSNAV_SBLIST_VELOCITY_STTDEV            0x0010    /* Bit 4 */


/*==GNSS Attitude Blocks=====================================================*/


/*--AttEuler_1_0_t : --------------------------------------------------------*/
#define SBF_ATTERR_NONE                    0x0  /*!< no error */
#define SBF_ATTERR_NOTENOUGHMEAS           0x1  /*!< not enough measurements */
#define SBF_ATTERR_ANTENNASALIGNED         0x2  /*!< antennas are aligned */
#define SBF_ATTERR_INCONSISTENCYWITHANTPOS 0x3  /*!< inconsistency with manual
                                                  antenna position information
                                                  */
#define SBF_ATTERR_NO_ATTITUDE_REQUESTED 0x80 /*!< attitude not requested by
                                                  user */
/* next define is kept for backwards compatibility, better use 
   SBF_ATTERR_NO_ATTITUDE_REQUESTED */ 
#define ATTEULER_ERROR_NO_ATTITUDE_REQUESTED 0x80 /*!< attitude not requested by
                                                  user */

/*--AuxAntPositions_1_0_t : -------------------------------------------------*/
#define ATT_MAX_NBR_ANT  SBF_AUXANTPOSITIONS_1_0_AUXANTPOSITION_LENGTH


/*==Differential Correction Blocks===========================================*/


/*--DiffCorrIn_1_0_t : ------------------------------------------------------*/
#define RTCM2_MAX_NBR_WORDS       33
#define CMR_MAX_NBR_WORDS        (((4 + 6 + (31*15) + 2) /4) +1)
#define RTCM3_MAX_NBR_WORDS      258

#if (RTCM2_MAX_NBR_WORDS  >=  CMR_MAX_NBR_WORDS) && (RTCM2_MAX_NB_WORDS >= RTCM3_MAX_NBR_WORDS)
#define DIFFCORR_FRAME_MAX_LEN  RTCM2_MAX_NBR_WORDS
#elif (CMR_MAX_NBR_WORDS >= RTCM2_MAX_NBR_WORDS) && (CMR_MAX_NBR_WORDS >= RTCM3_MAX_NBR_WORDS)
#define DIFFCORR_FRAME_MAX_LEN  CMR_MAX_NBR_WORDS
#else
#define DIFFCORR_FRAME_MAX_NBR_WORDS  RTCM3_MAX_NBR_WORDS
#endif

#define SBF_RTCM   0x00
#define SBF_CMR    0x01     /* used in Reserved[0] from CMR release onwards */
#define SBF_RTCM3  0x02
#define SBF_RTCMV  0x03

#define SBF_DIFFCORRIN_1_0_FRAME_LENGTH  DIFFCORR_FRAME_MAX_NBR_WORDS * 4


/*--BaseStation_1_0_t : -----------------------------------------------------*/
#define BASESTATION_BASETYPE_FIXED  0
#define BASESTATION_BASETYPE_MOVING 1

#define BASESTATION_SOURCE_RTCM2X_MSG3     0
#define BASESTATION_SOURCE_RTCM2X_MSG24    2
#define BASESTATION_SOURCE_CMR2X_MSG1      4
#define BASESTATION_SOURCE_RTCM3X_MSG1005  8
#define BASESTATION_SOURCE_RTCMV           9
#define BASESTATION_SOURCE_CMRP_MSG2      10


/*==External Sensor Blocks===================================================*/


/*--ExtSensor macros --------------------------------------------------------*/
#define EXTSENSORSBF_SENSORMODEL_MMQ50          0
#define EXTSENSORSBF_SENSORMODEL_MTI            1
#define EXTSENSORSBF_SENSORMODEL_ELLIPSE        2
#define EXTSENSORSBF_SENSORMODEL_MTI10          3
#define EXTSENSORSBF_SENSORMODEL_SIMSENSOR      4
#define EXTSENSORSBF_SENSORMODEL_ELLIPSE2       5
#define EXTSENSORSBF_SENSORMODEL_EKINOX2        6
#define EXTSENSORSBF_SENSORMODEL_VN100          7
#define EXTSENSORSBF_SENSORMODEL_HG             8
#define EXTSENSORSBF_SENSORMODEL_ZUPT           20


#define EXTSENSORSBF_MEASTYPE_ACC             0
#define EXTSENSORSBF_MEASTYPE_RATE            1
#define EXTSENSORSBF_MEASTYPE_MAGNETICFIELD   2
#define EXTSENSORSBF_MEASTYPE_INFO            3
#define EXTSENSORSBF_MEASTYPE_ZUPT            20


/*--ExtSensorStatus_1_0_t : -------------------------------------------------*/
#define EXTSENSORSTATUS_STATUSTYPE_CONFIG        0
#define EXTSENSORSTATUS_STATUSTYPE_STATUS        1


/*--ExtSensorSetup_1_0_t : --------------------------------------------------*/
#define EXTSENSORSETUP_MEASTYPE_ACCELERATIONS  0x0001
#define EXTSENSORSETUP_MEASTYPE_ANGULAR_RATES  0x0002
#define EXTSENSORSETUP_MEASTYPE_MAGNETICFIELD  0x0004
#define EXTSENSORSETUP_MEASTYPE_PRESSURE       0x0008
#define EXTSENSORSETUP_MEASTYPE_TEMPERATURE    0x0010


/*--ExtSensorSetup_1_1_t : --------------------------------------------------*/
#define EXTSENSORSETUP_LEVERARMSOURCE_NVRAM       0x00
#define EXTSENSORSETUP_LEVERARMSOURCE_MANUAL      0x01
#define EXTSENSORSETUP_LEVERARMSOURCE_CALIBRATION 0x02


/*==Status Blocks============================================================*/


/*--TrackingStatus_1_0_t : --------------------------------------------------*/
#define SBF_TRACKINGSTATUS_1_0_CHANNELDATA_LENGTH  NR_OF_LOGICALCHANNELS


/*--ChannelStatus_1_0_t : ---------------------------------------------------*/
#define SBF_CHANNELSTATUS_1_0_DATA_LENGTH  MAXSB_CHANNELSATINFO   * sizeof ( ChannelSatInfo_1_0_t ) + \
                         MAXSB_CHANNELSTATEINFO * sizeof ( ChannelStateInfo_1_0_t )


/*--ReceiverStatus_2_0_t : --------------------------------------------------*/
#define          FRONTENDID_GPSGALL1      0
#define          FRONTENDID_GLOL1         1
#define          FRONTENDID_GALE6         2
#define          FRONTENDID_GPSL2         3
#define          FRONTENDID_GLOL2         4
#define          FRONTENDID_GPSGALL5E5a   5
#define          FRONTENDID_GALE5b        6
#define          FRONTENDID_GALE5ab       7
#define          FRONTENDID_GPSGLOGALL1   8
#define          FRONTENDID_GPSGLOL2      9
#define          FRONTENDID_LBAND        10
#define          FRONTENDID_CMPB1        11
#define          FRONTENDID_CMPB3        12
#define          FRONTENDID_UNUSED       31

/*--InputLink_1_0_t : -------------------------------------------------------*/
#define SBF_INPUTLINK_1_0_DATA_LENGTH  (MAXSB_CONNDESCR * sizeof(InputStatsSub_1_0_t))


/*--OutputLink_1_0_t : ------------------------------------------------------*/
#define SBF_OUTPUTLINK_1_0_DATA_LENGTH  (MAXSB_CONNDESCR * (sizeof(OutputStatsSub_1_0_t) + (MAXSB_DATATYPES * sizeof(OutputTypeSub_1_0_t))))


/*--OutputLink_1_1_t : ------------------------------------------------------*/
#define SBF_OUTPUTLINK_1_1_DATA_LENGTH  (MAXSB_CONNDESCR * (sizeof(OutputStatsSub_1_1_t) + (MAXSB_DATATYPES * sizeof(OutputTypeSub_1_0_t))))


/*--QualityInd_1_0_t : ------------------------------------------------------*/
#define QUALITYIND_OVERALL          0
#define QUALITYIND_MAINSIGNAL       1
#define QUALITYIND_AUX1SIGNAL       2
#define QUALITYIND_MAINANTCABLING  11
#define QUALITYIND_AUX1ANTCABLING  12
#define QUALITYIND_CPUHEADROOM     21
#define QUALITYIND_BASEMEAS        30


/*==TUR Specific Blocks======================================================*/


/*--TURPVTSatCorrections_1_0_t : ---------------------------------------------*/
#define SBF_TURPVTSATCORRECTIONS_1_0_DATA_LENGTH \
  (NAVSUPP_NBR_ACTIVESAT * (sizeof(SatCorrInfo_1_0_t) + \
                            NR_OF_ANTENNAS * NAVSUPP_NBR_ACTIVESIG * sizeof(SatClkCorrInfo_1_0_t)))


/*--TURHPCAInfo_1_0_t : -----------------------------------------------------*/
#define SBF_TURHPCAINFO_1_0_HPCA_LENGTH  NR_OF_LOGICALCHANNELS


/*--SysTimeOffset_1_0_t : ---------------------------------------------------*/
#define SBF_SYSTIMEOFFSET_1_0_TIMEOFFSET_LENGTH  (2 * MAX_NR_SATSYSTEMS - 2)


/*--Meas3Ranges_1_0_t : ------------------------------------------------*/
#define SBF_MEAS3RANGES_1_0_DATA_LENGTH      (32768)

/* maximum number of signal types per satellite in a Meas3Ranges SBF block */
#define MEAS3_SIG_MAX 16

/* maximum number of satellites per constellation in a Meas3Ranges SBF block */
#define MEAS3_SAT_MAX 56

/* Index of constellations in a Meas3Ranges SBF block */
typedef enum {
  MEAS3_SYS_GPS = 0,
  MEAS3_SYS_GLO,
  MEAS3_SYS_GAL,
  MEAS3_SYS_BDS,
  MEAS3_SYS_SBAS,
  MEAS3_SYS_QZS,
  MEAS3_SYS_IRN,
  MEAS3_SYS_MAX
} Meas3SatSystem_t;



/*==SBF-IDs==============================================================*/
typedef enum {
  /* Measurement Blocks */
  sbfnr_GenMeasEpoch_1            = (5944),
  sbfid_GenMeasEpoch_1_0          = (5944 | 0x0),
  sbfnr_MeasEpoch_2               = (4027),
  sbfid_MeasEpoch_2_0             = (4027 | 0x0),
  sbfid_MeasEpoch_2_1             = (4027 | 0x2000),
  sbfnr_MeasExtra_1               = (4000),
  sbfid_MeasExtra_1_0             = (4000 | 0x0),
  sbfid_MeasExtra_1_1             = (4000 | 0x2000),
  sbfid_MeasExtra_1_2             = (4000 | 0x4000),
  sbfid_MeasExtra_1_3             = (4000 | 0x6000),
  sbfnr_MeasFullRange_1           = (4098),
  sbfid_MeasFullRange_1_0         = (4098 | 0x0),
  sbfid_MeasFullRange_1_1         = (4098 | 0x2000),
  sbfnr_Meas3Ranges_1             = (4109),
  sbfid_Meas3Ranges_1_0           = (4109 | 0x0),
  sbfnr_Meas3CN0HiRes_1           = (4110),
  sbfid_Meas3CN0HiRes_1_0         = (4110 | 0x0),
  sbfnr_Meas3Doppler_1            = (4111),
  sbfid_Meas3Doppler_1_0          = (4111 | 0x0),
  sbfnr_Meas3PP_1                 = (4112),
  sbfid_Meas3PP_1_0               = (4112 | 0x0),
  sbfnr_Meas3MP_1                 = (4113),
  sbfid_Meas3MP_1_0               = (4113 | 0x0),
  sbfnr_IQCorr_1                  = (4046),
  sbfid_IQCorr_1_0                = (4046 | 0x0),
  sbfid_IQCorr_1_1                = (4046 | 0x2000),
  sbfnr_ISMR_1                    = (4086),
  sbfid_ISMR_1_0                  = (4086 | 0x0),
  sbfnr_SQMSamples_1              = (4087),
  sbfid_SQMSamples_1_0            = (4087 | 0x0),
  sbfnr_EndOfMeas_1               = (5922),
  sbfid_EndOfMeas_1_0             = (5922 | 0x0),
  /* Navigation Page Blocks */
  sbfnr_GPSRaw_1                  = (5895),
  sbfid_GPSRaw_1_0                = (5895 | 0x0),
  sbfnr_CNAVRaw_1                 = (5947),
  sbfid_CNAVRaw_1_0               = (5947 | 0x0),
  sbfnr_GEORaw_1                  = (5898),
  sbfid_GEORaw_1_0                = (5898 | 0x0),
  sbfnr_GPSRawCA_1                = (4017),
  sbfid_GPSRawCA_1_0              = (4017 | 0x0),
  sbfnr_GPSRawL2C_1               = (4018),
  sbfid_GPSRawL2C_1_0             = (4018 | 0x0),
  sbfnr_GPSRawL5_1                = (4019),
  sbfid_GPSRawL5_1_0              = (4019 | 0x0),
  sbfnr_GPSRawL1C_1               = (4221),
  sbfid_GPSRawL1C_1_0             = (4221 | 0x0),
  sbfnr_GLORawCA_1                = (4026),
  sbfid_GLORawCA_1_0              = (4026 | 0x0),
  sbfnr_GALRawFNAV_1              = (4022),
  sbfid_GALRawFNAV_1_0            = (4022 | 0x0),
  sbfnr_GALRawINAV_1              = (4023),
  sbfid_GALRawINAV_1_0            = (4023 | 0x0),
  sbfnr_GALRawCNAV_1              = (4024),
  sbfid_GALRawCNAV_1_0            = (4024 | 0x0),
  sbfnr_GALRawGNAV_1              = (4025),
  sbfid_GALRawGNAV_1_0            = (4025 | 0x0),
  sbfnr_GALRawGNAVe_1             = (4029),
  sbfid_GALRawGNAVe_1_0           = (4029 | 0x0),
  sbfnr_GEORawL1_1                = (4020),
  sbfid_GEORawL1_1_0              = (4020 | 0x0),
  sbfnr_GEORawL5_1                = (4021),
  sbfid_GEORawL5_1_0              = (4021 | 0x0),
  sbfnr_BDSRaw_1                  = (4047),
  sbfid_BDSRaw_1_0                = (4047 | 0x0),
  sbfnr_BDSRawB1C_1               = (4218),
  sbfid_BDSRawB1C_1_0             = (4218 | 0x0),
  sbfnr_BDSRawB2a_1               = (4219),
  sbfid_BDSRawB2a_1_0             = (4219 | 0x0),
  sbfnr_QZSRawL1CA_1              = (4066),
  sbfid_QZSRawL1CA_1_0            = (4066 | 0x0),
  sbfnr_QZSRawL2C_1               = (4067),
  sbfid_QZSRawL2C_1_0             = (4067 | 0x0),
  sbfnr_QZSRawL5_1                = (4068),
  sbfid_QZSRawL5_1_0              = (4068 | 0x0),
  sbfnr_QZSRawL6_1                = (4069),
  sbfid_QZSRawL6_1_0              = (4069 | 0x0),
  sbfnr_QZSRawL1C_1               = (4227),
  sbfid_QZSRawL1C_1_0             = (4227 | 0x0),
  sbfnr_QZSRawL1S_1               = (4228),
  sbfid_QZSRawL1S_1_0             = (4228 | 0x0),
  sbfnr_IRNSSRaw_1                = (4093),
  sbfid_IRNSSRaw_1_0              = (4093 | 0x0),
  sbfnr_GNSSNavBits_1             = (4088),
  sbfid_GNSSNavBits_1_0           = (4088 | 0x0),
  sbfnr_GNSSSymbols_1             = (4099),
  sbfid_GNSSSymbols_1_0           = (4099 | 0x0),
  /* GPS Decoded Message Blocks */
  sbfnr_GPSNav_1                  = (5891),
  sbfid_GPSNav_1_0                = (5891 | 0x0),
  sbfnr_GPSAlm_1                  = (5892),
  sbfid_GPSAlm_1_0                = (5892 | 0x0),
  sbfnr_GPSIon_1                  = (5893),
  sbfid_GPSIon_1_0                = (5893 | 0x0),
  sbfnr_GPSUtc_1                  = (5894),
  sbfid_GPSUtc_1_0                = (5894 | 0x0),
  sbfnr_GPSCNav_1                 = (4042),
  sbfid_GPSCNav_1_0               = (4042 | 0x0),
  /* GLONASS Decoded Message Blocks */
  sbfnr_GLONav_1                  = (4004),
  sbfid_GLONav_1_0                = (4004 | 0x0),
  sbfnr_GLOAlm_1                  = (4005),
  sbfid_GLOAlm_1_0                = (4005 | 0x0),
  sbfnr_GLOTime_1                 = (4036),
  sbfid_GLOTime_1_0               = (4036 | 0x0),
  /* Galileo Decoded Message Blocks */
  sbfnr_GALNav_1                  = (4002),
  sbfid_GALNav_1_0                = (4002 | 0x0),
  sbfnr_GALAlm_1                  = (4003),
  sbfid_GALAlm_1_0                = (4003 | 0x0),
  sbfnr_GALIon_1                  = (4030),
  sbfid_GALIon_1_0                = (4030 | 0x0),
  sbfnr_GALUtc_1                  = (4031),
  sbfid_GALUtc_1_0                = (4031 | 0x0),
  sbfnr_GALGstGps_1               = (4032),
  sbfid_GALGstGps_1_0             = (4032 | 0x0),
  sbfnr_GALSARRLM_1               = (4034),
  sbfid_GALSARRLM_1_0             = (4034 | 0x0),
  /* BeiDou Decoded Message Blocks */
  sbfnr_BDSNav_1                  = (4081),
  sbfid_BDSNav_1_0                = (4081 | 0x0),
  sbfnr_BDSIon_1                  = (4120),
  sbfid_BDSIon_1_0                = (4120 | 0x0),
  sbfnr_BDSUtc_1                  = (4121),
  sbfid_BDSUtc_1_0                = (4121 | 0x0),
  /* QZSS Decoded Message Blocks */
  sbfnr_QZSNav_1                  = (4095),
  sbfid_QZSNav_1_0                = (4095 | 0x0),
  /* SBAS L1 Decoded Message Blocks */
  sbfnr_GEOMT00_1                 = (5925),
  sbfid_GEOMT00_1_0               = (5925 | 0x0),
  sbfnr_GEOPRNMask_1              = (5926),
  sbfid_GEOPRNMask_1_0            = (5926 | 0x0),
  sbfnr_GEOFastCorr_1             = (5927),
  sbfid_GEOFastCorr_1_0           = (5927 | 0x0),
  sbfnr_GEOIntegrity_1            = (5928),
  sbfid_GEOIntegrity_1_0          = (5928 | 0x0),
  sbfnr_GEOFastCorrDegr_1         = (5929),
  sbfid_GEOFastCorrDegr_1_0       = (5929 | 0x0),
  sbfnr_GEONav_1                  = (5896),
  sbfid_GEONav_1_0                = (5896 | 0x0),
  sbfnr_GEODegrFactors_1          = (5930),
  sbfid_GEODegrFactors_1_0        = (5930 | 0x0),
  sbfnr_GEONetworkTime_1          = (5918),
  sbfid_GEONetworkTime_1_0        = (5918 | 0x0),
  sbfnr_GEOAlm_1                  = (5897),
  sbfid_GEOAlm_1_0                = (5897 | 0x0),
  sbfnr_GEOIGPMask_1              = (5931),
  sbfid_GEOIGPMask_1_0            = (5931 | 0x0),
  sbfnr_GEOLongTermCorr_1         = (5932),
  sbfid_GEOLongTermCorr_1_0       = (5932 | 0x0),
  sbfnr_GEOIonoDelay_1            = (5933),
  sbfid_GEOIonoDelay_1_0          = (5933 | 0x0),
  sbfnr_GEOServiceLevel_1         = (5917),
  sbfid_GEOServiceLevel_1_0       = (5917 | 0x0),
  sbfnr_GEOClockEphCovMatrix_1    = (5934),
  sbfid_GEOClockEphCovMatrix_1_0  = (5934 | 0x0),
  /* SBAS L5 Decoded Message Blocks */
  sbfnr_SBASL5Nav_1               = (5958),
  sbfid_SBASL5Nav_1_0             = (5958 | 0x0),
  sbfnr_SBASL5Alm_1               = (5959),
  sbfid_SBASL5Alm_1_0             = (5959 | 0x0),
  /* Position, Velocity and Time Blocks */
  sbfnr_PVTCartesian_1            = (5903),
  sbfid_PVTCartesian_1_0          = (5903 | 0x0),
  sbfnr_PVTGeodetic_1             = (5904),
  sbfid_PVTGeodetic_1_0           = (5904 | 0x0),
  sbfnr_DOP_1                     = (5909),
  sbfid_DOP_1_0                   = (5909 | 0x0),
  sbfnr_PVTResiduals_1            = (5910),
  sbfid_PVTResiduals_1_0          = (5910 | 0x0),
  sbfnr_RAIMStatistics_1          = (5915),
  sbfid_RAIMStatistics_1_0        = (5915 | 0x0),
  sbfnr_PVTCartesian_2            = (4006),
  sbfid_PVTCartesian_2_0          = (4006 | 0x0),
  sbfid_PVTCartesian_2_1          = (4006 | 0x2000),
  sbfid_PVTCartesian_2_2          = (4006 | 0x4000),
  sbfnr_PVTGeodetic_2             = (4007),
  sbfid_PVTGeodetic_2_0           = (4007 | 0x0),
  sbfid_PVTGeodetic_2_1           = (4007 | 0x2000),
  sbfid_PVTGeodetic_2_2           = (4007 | 0x4000),
  sbfnr_PosCovCartesian_1         = (5905),
  sbfid_PosCovCartesian_1_0       = (5905 | 0x0),
  sbfnr_PosCovGeodetic_1          = (5906),
  sbfid_PosCovGeodetic_1_0        = (5906 | 0x0),
  sbfnr_VelCovCartesian_1         = (5907),
  sbfid_VelCovCartesian_1_0       = (5907 | 0x0),
  sbfnr_VelCovGeodetic_1          = (5908),
  sbfid_VelCovGeodetic_1_0        = (5908 | 0x0),
  sbfnr_DOP_2                     = (4001),
  sbfid_DOP_2_0                   = (4001 | 0x0),
  sbfnr_PosCart_1                 = (4044),
  sbfid_PosCart_1_0               = (4044 | 0x0),
  sbfnr_PosLocal_1                = (4052),
  sbfid_PosLocal_1_0              = (4052 | 0x0),
  sbfnr_PosProjected_1            = (4094),
  sbfid_PosProjected_1_0          = (4094 | 0x0),
  sbfnr_PVTSatCartesian_1         = (4008),
  sbfid_PVTSatCartesian_1_0       = (4008 | 0x0),
  sbfid_PVTSatCartesian_1_1       = (4008 | 0x2000),
  sbfnr_PVTResiduals_2            = (4009),
  sbfid_PVTResiduals_2_0          = (4009 | 0x0),
  sbfid_PVTResiduals_2_1          = (4009 | 0x2000),
  sbfnr_RAIMStatistics_2          = (4011),
  sbfid_RAIMStatistics_2_0        = (4011 | 0x0),
  sbfnr_GEOCorrections_1          = (5935),
  sbfid_GEOCorrections_1_0        = (5935 | 0x0),
  sbfnr_BaseVectorCart_1          = (4043),
  sbfid_BaseVectorCart_1_0        = (4043 | 0x0),
  sbfnr_BaseVectorGeod_1          = (4028),
  sbfid_BaseVectorGeod_1_0        = (4028 | 0x0),
  sbfnr_EndOfPVT_1                = (5921),
  sbfid_EndOfPVT_1_0              = (5921 | 0x0),
  sbfnr_BaseLine_1                = (5950),
  sbfid_BaseLine_1_0              = (5950 | 0x0),
  /* INS/GNSS Integrated Blocks */
  sbfnr_IntPVCart_1               = (4060),
  sbfid_IntPVCart_1_0             = (4060 | 0x0),
  sbfnr_IntPVGeod_1               = (4061),
  sbfid_IntPVGeod_1_0             = (4061 | 0x0),
  sbfnr_IntPosCovCart_1           = (4062),
  sbfid_IntPosCovCart_1_0         = (4062 | 0x0),
  sbfnr_IntVelCovCart_1           = (4063),
  sbfid_IntVelCovCart_1_0         = (4063 | 0x0),
  sbfnr_IntPosCovGeod_1           = (4064),
  sbfid_IntPosCovGeod_1_0         = (4064 | 0x0),
  sbfnr_IntVelCovGeod_1           = (4065),
  sbfid_IntVelCovGeod_1_0         = (4065 | 0x0),
  sbfnr_IntAttEuler_1             = (4070),
  sbfid_IntAttEuler_1_0           = (4070 | 0x0),
  sbfid_IntAttEuler_1_1           = (4070 | 0x2000),
  sbfnr_IntAttCovEuler_1          = (4072),
  sbfid_IntAttCovEuler_1_0        = (4072 | 0x0),
  sbfnr_IntPVAAGeod_1             = (4045),
  sbfid_IntPVAAGeod_1_0           = (4045 | 0x0),
  sbfnr_INSNavCart_1              = (4225),
  sbfid_INSNavCart_1_0            = (4225 | 0x0),
  sbfnr_INSNavGeod_1              = (4226),
  sbfid_INSNavGeod_1_0            = (4226 | 0x0),
  /* GNSS Attitude Blocks */
  sbfnr_AttEuler_1                = (5938),
  sbfid_AttEuler_1_0              = (5938 | 0x0),
  sbfnr_AttCovEuler_1             = (5939),
  sbfid_AttCovEuler_1_0           = (5939 | 0x0),
  sbfnr_AuxAntPositions_1         = (5942),
  sbfid_AuxAntPositions_1_0       = (5942 | 0x0),
  sbfnr_EndOfAtt_1                = (5943),
  sbfid_EndOfAtt_1_0              = (5943 | 0x0),
  sbfnr_AttQuat_1                 = (5940),
  sbfid_AttQuat_1_0               = (5940 | 0x0),
  sbfnr_AttCovQuat_1              = (5941),
  sbfid_AttCovQuat_1_0            = (5941 | 0x0),
  /* Receiver Time Blocks */
  sbfnr_ReceiverTime_1            = (5914),
  sbfid_ReceiverTime_1_0          = (5914 | 0x0),
  sbfnr_xPPSOffset_1              = (5911),
  sbfid_xPPSOffset_1_0            = (5911 | 0x0),
  sbfnr_SysTimeOffset_1           = (4039),
  sbfid_SysTimeOffset_1_0         = (4039 | 0x0),
  sbfid_SysTimeOffset_1_1         = (4039 | 0x2000),
  /* External Event Blocks */
  sbfnr_ExtEvent_1                = (5924),
  sbfid_ExtEvent_1_0              = (5924 | 0x0),
  sbfid_ExtEvent_1_1              = (5924 | 0x2000),
  sbfnr_ExtEventPVTCartesian_1    = (4037),
  sbfid_ExtEventPVTCartesian_1_0  = (4037 | 0x0),
  sbfid_ExtEventPVTCartesian_1_1  = (4037 | 0x2000),
  sbfid_ExtEventPVTCartesian_1_2  = (4037 | 0x4000),
  sbfnr_ExtEventPVTGeodetic_1     = (4038),
  sbfid_ExtEventPVTGeodetic_1_0   = (4038 | 0x0),
  sbfid_ExtEventPVTGeodetic_1_1   = (4038 | 0x2000),
  sbfid_ExtEventPVTGeodetic_1_2   = (4038 | 0x4000),
  sbfnr_ExtEventBaseVectCart_1    = (4216),
  sbfid_ExtEventBaseVectCart_1_0  = (4216 | 0x0),
  sbfnr_ExtEventBaseVectGeod_1    = (4217),
  sbfid_ExtEventBaseVectGeod_1_0  = (4217 | 0x0),
  sbfnr_ExtEventINSNavCart_1      = (4229),
  sbfid_ExtEventINSNavCart_1_0    = (4229 | 0x0),
  sbfnr_ExtEventINSNavGeod_1      = (4230),
  sbfid_ExtEventINSNavGeod_1_0    = (4230 | 0x0),
  /* Differential Correction Blocks */
  sbfnr_DiffCorrIn_1              = (5919),
  sbfid_DiffCorrIn_1_0            = (5919 | 0x0),
  sbfnr_BaseStation_1             = (5949),
  sbfid_BaseStation_1_0           = (5949 | 0x0),
  sbfnr_RTCMDatum_1               = (4049),
  sbfid_RTCMDatum_1_0             = (4049 | 0x0),
  sbfnr_BaseLink_1                = (5948),
  sbfid_BaseLink_1_0              = (5948 | 0x0),
  /* L-Band Demodulator Blocks */
  sbfnr_LBandReceiverStatus_1     = (4200),
  sbfid_LBandReceiverStatus_1_0   = (4200 | 0x0),
  sbfnr_LBandTrackerStatus_1      = (4201),
  sbfid_LBandTrackerStatus_1_0    = (4201 | 0x0),
  sbfid_LBandTrackerStatus_1_1    = (4201 | 0x2000),
  sbfid_LBandTrackerStatus_1_2    = (4201 | 0x4000),
  sbfid_LBandTrackerStatus_1_3    = (4201 | 0x6000),
  sbfnr_LBAS1DecoderStatus_1      = (4202),
  sbfid_LBAS1DecoderStatus_1_0    = (4202 | 0x0),
  sbfid_LBAS1DecoderStatus_1_1    = (4202 | 0x2000),
  sbfid_LBAS1DecoderStatus_1_2    = (4202 | 0x4000),
  sbfnr_LBAS1Messages_1           = (4203),
  sbfid_LBAS1Messages_1_0         = (4203 | 0x0),
  sbfnr_LBandBeams_1              = (4204),
  sbfid_LBandBeams_1_0            = (4204 | 0x0),
  sbfnr_LBandRaw_1                = (4212),
  sbfid_LBandRaw_1_0              = (4212 | 0x0),
  sbfnr_FugroStatus_1             = (4214),
  sbfid_FugroStatus_1_0           = (4214 | 0x0),
  /* External Sensor Blocks */
  sbfnr_ExtSensorMeas_1           = (4050),
  sbfid_ExtSensorMeas_1_0         = (4050 | 0x0),
  sbfnr_ExtSensorStatus_1         = (4056),
  sbfid_ExtSensorStatus_1_0       = (4056 | 0x0),
  sbfnr_ExtSensorSetup_1          = (4057),
  sbfid_ExtSensorSetup_1_0        = (4057 | 0x0),
  sbfid_ExtSensorSetup_1_1        = (4057 | 0x2000),
  sbfid_ExtSensorSetup_1_2        = (4057 | 0x4000),
  sbfnr_ExtSensorStatus_2         = (4223),
  sbfid_ExtSensorStatus_2_0       = (4223 | 0x0),
  sbfnr_ExtSensorInfo_1           = (4222),
  sbfid_ExtSensorInfo_1_0         = (4222 | 0x0),
  sbfnr_IMUSetup_1                = (4224),
  sbfid_IMUSetup_1_0              = (4224 | 0x0),
  /* Status Blocks */
  sbfnr_ReceiverStatus_1          = (5913),
  sbfid_ReceiverStatus_1_0        = (5913 | 0x0),
  sbfnr_TrackingStatus_1          = (5912),
  sbfid_TrackingStatus_1_0        = (5912 | 0x0),
  sbfnr_ChannelStatus_1           = (4013),
  sbfid_ChannelStatus_1_0         = (4013 | 0x0),
  sbfnr_ReceiverStatus_2          = (4014),
  sbfid_ReceiverStatus_2_0        = (4014 | 0x0),
  sbfid_ReceiverStatus_2_1        = (4014 | 0x2000),
  sbfnr_SatVisibility_1           = (4012),
  sbfid_SatVisibility_1_0         = (4012 | 0x0),
  sbfnr_InputLink_1               = (4090),
  sbfid_InputLink_1_0             = (4090 | 0x0),
  sbfnr_OutputLink_1              = (4091),
  sbfid_OutputLink_1_0            = (4091 | 0x0),
  sbfid_OutputLink_1_1            = (4091 | 0x2000),
  sbfnr_NTRIPClientStatus_1       = (4053),
  sbfid_NTRIPClientStatus_1_0     = (4053 | 0x0),
  sbfnr_IPStatus_1                = (4058),
  sbfid_IPStatus_1_0              = (4058 | 0x0),
  sbfid_IPStatus_1_1              = (4058 | 0x2000),
  sbfnr_WiFiAPStatus_1            = (4054),
  sbfid_WiFiAPStatus_1_0          = (4054 | 0x0),
  sbfnr_WiFiClientStatus_1        = (4096),
  sbfid_WiFiClientStatus_1_0      = (4096 | 0x0),
  sbfnr_CellularStatus_1          = (4055),
  sbfid_CellularStatus_1_0        = (4055 | 0x0),
  sbfid_CellularStatus_1_1        = (4055 | 0x2000),
  sbfnr_BluetoothStatus_1         = (4051),
  sbfid_BluetoothStatus_1_0       = (4051 | 0x0),
  sbfnr_DynDNSStatus_1            = (4105),
  sbfid_DynDNSStatus_1_0          = (4105 | 0x0),
  sbfid_DynDNSStatus_1_1          = (4105 | 0x2000),
  sbfnr_BatteryStatus_1           = (4083),
  sbfid_BatteryStatus_1_0         = (4083 | 0x0),
  sbfid_BatteryStatus_1_1         = (4083 | 0x2000),
  sbfid_BatteryStatus_1_2         = (4083 | 0x4000),
  sbfnr_PowerStatus_1             = (4101),
  sbfid_PowerStatus_1_0           = (4101 | 0x0),
  sbfnr_QualityInd_1              = (4082),
  sbfid_QualityInd_1_0            = (4082 | 0x0),
  sbfnr_DiskStatus_1              = (4059),
  sbfid_DiskStatus_1_0            = (4059 | 0x0),
  sbfid_DiskStatus_1_1            = (4059 | 0x2000),
  sbfnr_LogStatus_1               = (4102),
  sbfid_LogStatus_1_0             = (4102 | 0x0),
  sbfnr_UHFStatus_1               = (4085),
  sbfid_UHFStatus_1_0             = (4085 | 0x0),
  sbfnr_RFStatus_1                = (4092),
  sbfid_RFStatus_1_0              = (4092 | 0x0),
  sbfnr_RIMSHealth_1              = (4089),
  sbfid_RIMSHealth_1_0            = (4089 | 0x0),
  /* Miscellaneous Blocks */
  sbfnr_ReceiverSetup_1           = (5902),
  sbfid_ReceiverSetup_1_0         = (5902 | 0x0),
  sbfid_ReceiverSetup_1_1         = (5902 | 0x2000),
  sbfid_ReceiverSetup_1_2         = (5902 | 0x4000),
  sbfid_ReceiverSetup_1_3         = (5902 | 0x6000),
  sbfid_ReceiverSetup_1_4         = (5902 | 0x8000),
  sbfnr_RxComponents_1            = (4084),
  sbfid_RxComponents_1_0          = (4084 | 0x0),
  sbfnr_RxMessage_1               = (4103),
  sbfid_RxMessage_1_0             = (4103 | 0x0),
  sbfnr_Commands_1                = (4015),
  sbfid_Commands_1_0              = (4015 | 0x0),
  sbfnr_Comment_1                 = (5936),
  sbfid_Comment_1_0               = (5936 | 0x0),
  sbfnr_BBSamples_1               = (4040),
  sbfid_BBSamples_1_0             = (4040 | 0x0),
  sbfnr_ASCIIIn_1                 = (4075),
  sbfid_ASCIIIn_1_0               = (4075 | 0x0),
  sbfnr_EncapsulatedOutput_1      = (4097),
  sbfid_EncapsulatedOutput_1_0    = (4097 | 0x0),
  /* TUR Specific Blocks */
  sbfnr_TURPVTSatCorrections_1    = (4035),
  sbfid_TURPVTSatCorrections_1_0  = (4035 | 0x0),
  sbfnr_TURHPCAInfo_1             = (4010),
  sbfid_TURHPCAInfo_1_0           = (4010 | 0x0),
  sbfnr_CorrPeakSample_1          = (4016),
  sbfid_CorrPeakSample_1_0        = (4016 | 0x0),
  sbfnr_CorrValues_1              = (4100),
  sbfid_CorrValues_1_0            = (4100 | 0x0),
  sbfnr_TURStatus_1               = (4041),
  sbfid_TURStatus_1_0             = (4041 | 0x0),
  sbfid_TURStatus_1_1             = (4041 | 0x2000),
  sbfid_TURStatus_1_2             = (4041 | 0x4000),
  sbfnr_GALIntegrity_1            = (4033),
  sbfid_GALIntegrity_1_0          = (4033 | 0x0),
  sbfnr_GALNavMonitor_1           = (4108),
  sbfid_GALNavMonitor_1_0         = (4108 | 0x0),
  sbfnr_TURFormat_1               = (4080),
  sbfid_TURFormat_1_0             = (4080 | 0x0),
  sbfnr_CalibrationValues_1       = (4215),
  sbfid_CalibrationValues_1_0     = (4215 | 0x0),
  sbfnr_MultipathMonitor_1        = (4220),
  sbfid_MultipathMonitor_1_0      = (4220 | 0x0),
  /* PinPoint-GIS RX */
  sbfnr_GISAction_1               = (4106),
  sbfid_GISAction_1_0             = (4106 | 0x0),
  sbfnr_GISStatus_1               = (4107),
  sbfid_GISStatus_1_0             = (4107 | 0x0),
  /* some special values */
  sbfnr_MIN                       = (4000),
  sbfnr_MAX                       = (6015), // This is the maximum allowed SBF ID which can be output by a Septentrio receiver using the SBF protocol as is today.
  sbfid_ALL                       = (0xFFFF),
  sbfnr_INVALID                   = (0x0)
} SBFID_t;

/*==CONNECTION DESCRIPTOR definition=========================================*/
typedef enum {
  CD_COM1 = (1),
  CD_COM2 = (2),
  CD_COM3 = (3),
  CD_COM4 = (4),
  CD_USB1 = (33),
  CD_USB2 = (34),
  CD_OTG1 = (49),
  CD_OTG2 = (50),
  CD_IP10 = (64),
  CD_IP11 = (65),
  CD_IP12 = (66),
  CD_IP13 = (67),
  CD_IP14 = (68),
  CD_IP15 = (69),
  CD_IP16 = (70),
  CD_IP17 = (71),
  CD_DSK1 = (97),
  CD_DSK2 = (98),
  CD_NTR1 = (129),
  CD_NTR2 = (130),
  CD_NTR3 = (131),
  CD_NTR4 = (132),
  CD_NTR5 = (133),
  CD_IPS1 = (161),
  CD_IPS2 = (162),
  CD_IPS3 = (163),
  CD_IPS4 = (164),
  CD_IPS5 = (165),
  CD_BT01 = (192),
  CD_UHF1 = (196),
  CD_IPR1 = (201),
  CD_IPR2 = (202),
  CD_IPR3 = (203),
  CD_IPR4 = (204),
  CD_IPR5 = (205),
  CD_DCL1 = (210)
} connDescr_t;

/*==DATA TYPE definition=====================================================*/
typedef enum {
  DATA_NONE = (0),
  DATA_DAISYCHAIN = (1),
  DATA_CMD = (32),
  DATA_SBF = (33),
  DATA_ASCIIDISPLAY = (34),
  DATA_RINEX = (35),
  DATA_BINEX = (40),
  DATA_NMEA = (64),
  DATA_RTCMV2 = (96),
  DATA_RTCMV3 = (97),
  DATA_CMRV2 = (98),
  DATA_RTCMV = (99),
  DATA_LBAS1 = (110),
  DATA_MTI = (128),
  DATA_MMQ = (129),
  DATA_ELLIPSE = (130),
  DATA_SBG = (131),
  DATA_MTI10 = (132),
  DATA_SPIRENT = (133),
  DATA_VECTORNAV = (134),
  DATA_HONEYWELL = (135),
  DATA_ASCIIIN = (160)
} dataTypes_t;

/*==SBF-BLOCKS definition====================================================*/

/*--Block Header : ----------------------------------------------------------*/
typedef struct
{
  uint16_t       Sync;
  uint16_t       CRC;
  uint16_t       ID;
  uint16_t       Length;
} BlockHeader_t;

/*--Time Header : -----------------------------------------------------------*/
typedef struct
{
  BlockHeader_t Header;

  uint32_t       TOW;
  uint16_t       WNc;
} TimeHeader_t;

/*--Receiver Time Header : --------------------------------------------------*/
typedef struct
{
  TimeHeader_t Header;

} ReceiverTimeHeader_t;

/*--SIS Time Header : -------------------------------------------------------*/
typedef struct
{
  TimeHeader_t Header;

} SISTimeHeader_t;

/*--External Time Header : --------------------------------------------------*/
typedef struct
{
  TimeHeader_t Header;

} ExternalTimeHeader_t;

/*--Navigation Header : -----------------------------------------------------*/
typedef struct
{
  SISTimeHeader_t Header;

  uint8_t        SVID;
  uint8_t        CRCPassed;
} NavigationHeader_t;

/*--MeasEpoch Header : ------------------------------------------------------*/
typedef struct
{
  ReceiverTimeHeader_t Header;

  uint8_t        N1;
  uint8_t        SB1Length;
  uint8_t        SB2Length;
} MeasEpochHeader_t;

/*==Measurement Blocks=======================================================*/

/*--GenMeasEpoch_1_0_t : ----------------------------------------------------*/
/* Measurement set of one epoch */

typedef struct
{
  uint8_t        Type;         
  uint8_t        LockTime;     
  uint8_t        CN0;          
  uint8_t        OffsetsMSB;   
  int8_t         CarrierMSB;   
  uint8_t        ObsInfo;      
  uint16_t       CodeOffsetLSB;
  uint16_t       CarrierLSB;   
  uint16_t       DopplerOffsetLSB;
} MeasEpochChannelType2_1_0_t;

typedef struct
{
  uint8_t        RXChannel;    
  uint8_t        Type;         
  uint8_t        SVID;         
  uint8_t        Misc;         
  uint32_t       CodeLSB;      
  int32_t        Doppler;      
  uint16_t       CarrierLSB;   
  int8_t         CarrierMSB;   
  uint8_t        CN0;          
  uint16_t       LockTime;     
  uint8_t        ObsInfo;      
  uint8_t        N_Type2;      
} MeasEpochChannelType1_1_0_t;

#ifndef SBF_GENMEASEPOCH_1_0_DATA_LENGTH 
#define SBF_GENMEASEPOCH_1_0_DATA_LENGTH -1 
#endif

#define SBF_GENMEASEPOCH_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  /* MeasEpoch Header */
  uint8_t        N;            
  uint8_t        SB1Size;      
  uint8_t        SB2Size;      

  uint8_t        CommonFlags;  
  uint8_t        Reserved[2];  
  uint8_t        Data[SBF_GENMEASEPOCH_1_0_DATA_LENGTH];
} GenMeasEpoch_1_0_t;

typedef MeasEpochChannelType2_1_0_t MeasEpochChannelType2_1_t;

typedef MeasEpochChannelType1_1_0_t MeasEpochChannelType1_1_t;

#define SBF_GENMEASEPOCH_1_DATA_LENGTH SBF_GENMEASEPOCH_1_0_DATA_LENGTH
typedef GenMeasEpoch_1_0_t GenMeasEpoch_1_t;

/*--MeasEpoch_2_0_t : -------------------------------------------------------*/
/* Measurement set of one epoch */

typedef struct
{
  uint8_t        Type;         
  uint8_t        LockTime;     
  uint8_t        CN0;          
  uint8_t        OffsetsMSB;   
  int8_t         CarrierMSB;   
  uint8_t        ObsInfo;      
  uint16_t       CodeOffsetLSB;
  uint16_t       CarrierLSB;   
  uint16_t       DopplerOffsetLSB;
} MeasEpochChannelType2_2_0_t;

typedef struct
{
  uint8_t        RXChannel;    
  uint8_t        Type;         
  uint8_t        SVID;         
  uint8_t        Misc;         
  uint32_t       CodeLSB;      
  int32_t        Doppler;      
  uint16_t       CarrierLSB;   
  int8_t         CarrierMSB;   
  uint8_t        CN0;          
  uint16_t       LockTime;     
  uint8_t        ObsInfo;      
  uint8_t        N_Type2;      
} MeasEpochChannelType1_2_0_t;

#ifndef SBF_MEASEPOCH_2_0_DATA_LENGTH 
#define SBF_MEASEPOCH_2_0_DATA_LENGTH -1 
#endif

#define SBF_MEASEPOCH_2_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  /* MeasEpoch Header */
  uint8_t        N;            
  uint8_t        SB1Size;      
  uint8_t        SB2Size;      

  uint8_t        CommonFlags;  
  uint8_t        Reserved2;    /* Reserved for future use */
  uint8_t        Reserved;     
  uint8_t        Data[SBF_MEASEPOCH_2_0_DATA_LENGTH];
} MeasEpoch_2_0_t;

/*--MeasEpoch_2_1_t : -------------------------------------------------------*/
/* Measurement set of one epoch */

typedef struct
{
  uint8_t        Type;         
  uint8_t        LockTime;     
  uint8_t        CN0;          
  uint8_t        OffsetsMSB;   
  int8_t         CarrierMSB;   
  uint8_t        ObsInfo;      
  uint16_t       CodeOffsetLSB;
  uint16_t       CarrierLSB;   
  uint16_t       DopplerOffsetLSB;
} MeasEpochChannelType2_2_1_t;

typedef struct
{
  uint8_t        RXChannel;    
  uint8_t        Type;         
  uint8_t        SVID;         
  uint8_t        Misc;         
  uint32_t       CodeLSB;      
  int32_t        Doppler;      
  uint16_t       CarrierLSB;   
  int8_t         CarrierMSB;   
  uint8_t        CN0;          
  uint16_t       LockTime;     
  uint8_t        ObsInfo;      
  uint8_t        N_Type2;      
} MeasEpochChannelType1_2_1_t;

#ifndef SBF_MEASEPOCH_2_1_DATA_LENGTH 
#define SBF_MEASEPOCH_2_1_DATA_LENGTH -1 
#endif

#define SBF_MEASEPOCH_2_1_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  /* MeasEpoch Header */
  uint8_t        N;            
  uint8_t        SB1Size;      
  uint8_t        SB2Size;      

  uint8_t        CommonFlags;  
  uint8_t        CumClkJumps;  
  uint8_t        Reserved;     
  uint8_t        Data[SBF_MEASEPOCH_2_1_DATA_LENGTH];
} MeasEpoch_2_1_t;

typedef MeasEpochChannelType2_2_1_t MeasEpochChannelType2_2_t;

typedef MeasEpochChannelType1_2_1_t MeasEpochChannelType1_2_t;

#define SBF_MEASEPOCH_2_DATA_LENGTH SBF_MEASEPOCH_2_1_DATA_LENGTH
typedef MeasEpoch_2_1_t MeasEpoch_2_t;

/*--MeasExtra_1_0_t : -------------------------------------------------------*/
/* Additional info such as observable variance */

typedef struct
{
  uint8_t        RXChannel;    
  uint8_t        Type;         
  int16_t        MPCorr;       
  int16_t        SmoothingCorr;
  uint16_t       CodeVar;      
  uint16_t       CarrierVar;   
  uint16_t       LockTime;     
} MeasExtraChannelSub_1_0_t;

#ifndef SBF_MEASEXTRA_1_0_MEASEXTRACHANNEL_LENGTH 
#define SBF_MEASEXTRA_1_0_MEASEXTRACHANNEL_LENGTH 648 
#endif

#define SBF_MEASEXTRA_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        N;            
  uint8_t        SBSize;       
  float          DopplerVarFactor;
  MeasExtraChannelSub_1_0_t MeasExtraChannel[SBF_MEASEXTRA_1_0_MEASEXTRACHANNEL_LENGTH];
} MeasExtra_1_0_t;

/*--MeasExtra_1_1_t : -------------------------------------------------------*/
/* Additional info such as observable variance */

#ifndef SBF_MEASEXTRACHANNELSUB_1_1__PADDING_LENGTH 
#define SBF_MEASEXTRACHANNELSUB_1_1__PADDING_LENGTH 2 
#endif

typedef struct
{
  uint8_t        RXChannel;    
  uint8_t        Type;         
  int16_t        MPCorr;       
  int16_t        SmoothingCorr;
  uint16_t       CodeVar;      
  uint16_t       CarrierVar;   
  uint16_t       LockTime;     
  uint8_t        CumLossCont;  
  int8_t         CarMPCorr;    

  uint8_t        _padding[SBF_MEASEXTRACHANNELSUB_1_1__PADDING_LENGTH];
} MeasExtraChannelSub_1_1_t;

#ifndef SBF_MEASEXTRA_1_1_MEASEXTRACHANNEL_LENGTH 
#define SBF_MEASEXTRA_1_1_MEASEXTRACHANNEL_LENGTH 648 
#endif

#define SBF_MEASEXTRA_1_1_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        N;            
  uint8_t        SBSize;       
  float          DopplerVarFactor;
  MeasExtraChannelSub_1_1_t MeasExtraChannel[SBF_MEASEXTRA_1_1_MEASEXTRACHANNEL_LENGTH];
} MeasExtra_1_1_t;

/*--MeasExtra_1_2_t : -------------------------------------------------------*/
/* Additional info such as observable variance */

#ifndef SBF_MEASEXTRACHANNELSUB_1_2__PADDING_LENGTH 
#define SBF_MEASEXTRACHANNELSUB_1_2__PADDING_LENGTH 1 
#endif

typedef struct
{
  uint8_t        RXChannel;    
  uint8_t        Type;         
  int16_t        MPCorr;       
  int16_t        SmoothingCorr;
  uint16_t       CodeVar;      
  uint16_t       CarrierVar;   
  uint16_t       LockTime;     
  uint8_t        CumLossCont;  
  int8_t         CarMPCorr;    
  uint8_t        Info;         

  uint8_t        _padding[SBF_MEASEXTRACHANNELSUB_1_2__PADDING_LENGTH];
} MeasExtraChannelSub_1_2_t;

#ifndef SBF_MEASEXTRA_1_2_MEASEXTRACHANNEL_LENGTH 
#define SBF_MEASEXTRA_1_2_MEASEXTRACHANNEL_LENGTH 648 
#endif

#define SBF_MEASEXTRA_1_2_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        N;            
  uint8_t        SBSize;       
  float          DopplerVarFactor;
  MeasExtraChannelSub_1_2_t MeasExtraChannel[SBF_MEASEXTRA_1_2_MEASEXTRACHANNEL_LENGTH];
} MeasExtra_1_2_t;

/*--MeasExtra_1_3_t : -------------------------------------------------------*/
/* Additional info such as observable variance */

typedef struct
{
  uint8_t        RXChannel;    
  uint8_t        Type;         
  int16_t        MPCorr;       
  int16_t        SmoothingCorr;
  uint16_t       CodeVar;      
  uint16_t       CarrierVar;   
  uint16_t       LockTime;     
  uint8_t        CumLossCont;  
  int8_t         CarMPCorr;    
  uint8_t        Info;         
  uint8_t        Misc;         
} MeasExtraChannelSub_1_3_t;

#ifndef SBF_MEASEXTRA_1_3_MEASEXTRACHANNEL_LENGTH 
#define SBF_MEASEXTRA_1_3_MEASEXTRACHANNEL_LENGTH 648 
#endif

#define SBF_MEASEXTRA_1_3_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        N;            
  uint8_t        SBSize;       
  float          DopplerVarFactor;
  MeasExtraChannelSub_1_3_t MeasExtraChannel[SBF_MEASEXTRA_1_3_MEASEXTRACHANNEL_LENGTH];
} MeasExtra_1_3_t;

typedef MeasExtraChannelSub_1_3_t MeasExtraChannelSub_1_t;

#define SBF_MEASEXTRA_1_MEASEXTRACHANNEL_LENGTH SBF_MEASEXTRA_1_3_MEASEXTRACHANNEL_LENGTH
typedef MeasExtra_1_3_t MeasExtra_1_t;

/*--MeasFullRange_1_0_t : ---------------------------------------------------*/
/* Extended-range code and phase measurements */

#define SBF_MEASFULLRANGESUB_1_0_CARRIERMINCODE_PRECISION 3
typedef struct
{
  uint8_t        RxChannel;    
  uint8_t        Type;         
  uint8_t        SVID;         
  uint8_t        FreqNrAnt;    
  SBFDOUBLE      CodeObs;      
  float          CarrierMinCode;
} MeasFullRangeSub_1_0_t;

#ifndef SBF_MEASFULLRANGE_1_0_MEASFULLRANGESUB_LENGTH 
#define SBF_MEASFULLRANGE_1_0_MEASFULLRANGESUB_LENGTH 648 
#endif

#define SBF_MEASFULLRANGE_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        N;            
  uint8_t        SBLength;     
  uint32_t       Reserved;     
  MeasFullRangeSub_1_0_t MeasFullRangeSub[SBF_MEASFULLRANGE_1_0_MEASFULLRANGESUB_LENGTH];
} MeasFullRange_1_0_t;

/*--MeasFullRange_1_1_t : ---------------------------------------------------*/
/* Extended-range code and phase measurements */

#ifndef SBF_MEASFULLRANGESUB_1_1__PADDING_LENGTH 
#define SBF_MEASFULLRANGESUB_1_1__PADDING_LENGTH 1 
#endif

#define SBF_MEASFULLRANGESUB_1_1_CARRIERMINCODE_PRECISION 3
typedef struct
{
  uint8_t        RxChannel;    
  uint8_t        Type;         
  uint8_t        SVID;         
  uint8_t        FreqNrAnt;    
  SBFDOUBLE      CodeObs;      
  float          CarrierMinCode;
  int16_t        CN0;          
  uint8_t        ObsInfo;      

  uint8_t        _padding[SBF_MEASFULLRANGESUB_1_1__PADDING_LENGTH];
} MeasFullRangeSub_1_1_t;

#ifndef SBF_MEASFULLRANGE_1_1_MEASFULLRANGESUB_LENGTH 
#define SBF_MEASFULLRANGE_1_1_MEASFULLRANGESUB_LENGTH 648 
#endif

#define SBF_MEASFULLRANGE_1_1_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        N;            
  uint8_t        SBLength;     
  uint32_t       Reserved;     
  MeasFullRangeSub_1_1_t MeasFullRangeSub[SBF_MEASFULLRANGE_1_1_MEASFULLRANGESUB_LENGTH];
} MeasFullRange_1_1_t;

#define SBF_MEASFULLRANGESUB_1__PADDING_LENGTH SBF_MEASFULLRANGESUB_1_1__PADDING_LENGTH
typedef MeasFullRangeSub_1_1_t MeasFullRangeSub_1_t;

#define SBF_MEASFULLRANGE_1_MEASFULLRANGESUB_LENGTH SBF_MEASFULLRANGE_1_1_MEASFULLRANGESUB_LENGTH
typedef MeasFullRange_1_1_t MeasFullRange_1_t;

/*--Meas3Ranges_1_0_t : -----------------------------------------------------*/
/* Code, phase and CN0 measurements */

typedef struct
{
  uint32_t       BF1;          
  uint32_t       PRLSB;        
  uint16_t       BF2;          
  uint8_t        BF3;          
  int16_t        PRR;          
} M3MasterLong_1_0_t;

typedef struct
{
  uint32_t       BF1;          
  uint32_t       PRLSB;        
  int16_t        PRR;          
} M3MasterShort_1_0_t;

typedef struct
{
  uint32_t       BF1;          
} M3MasterDeltaS_1_0_t;

typedef struct
{
  uint8_t        BF1;          
  uint32_t       BF2;          
} M3MasterDeltaL_1_0_t;

typedef struct
{
  uint32_t       BF1;          
  uint16_t       PRLSBrel;     
  uint8_t        BF3;          
} M3SlaveLong_1_0_t;

typedef struct
{
  uint32_t       BF1;          
  uint8_t        BF2;          
} M3SlaveShort_1_0_t;

typedef struct
{
  uint16_t       BF1;          
  uint8_t        dCarrier;     
} M3SlaveDelta_1_0_t;

#ifndef SBF_M3SATDATA_1_0_SATMASK_LENGTH 
#define SBF_M3SATDATA_1_0_SATMASK_LENGTH 8 
#endif

#ifndef SBF_M3SATDATA_1_0_GLOFNLIST_LENGTH 
#define SBF_M3SATDATA_1_0_GLOFNLIST_LENGTH 8 
#endif

typedef struct
{
  uint8_t        BF1;          
  uint8_t        SatMask[SBF_M3SATDATA_1_0_SATMASK_LENGTH];
  uint8_t        GLOFnList[SBF_M3SATDATA_1_0_GLOFNLIST_LENGTH];
  uint16_t       BDSLongRange; 
  uint8_t        SigExcluded;  
  M3MasterLong_1_0_t M3MasterLong; 
  M3MasterShort_1_0_t M3MasterShort;
  M3MasterDeltaS_1_0_t M3MasterDeltaS;
  M3MasterDeltaL_1_0_t M3MasterDeltaL;
  M3SlaveLong_1_0_t M3SlaveLong;  
  M3SlaveShort_1_0_t M3SlaveShort; 
  M3SlaveDelta_1_0_t M3SlaveDelta; 
} M3SatData_1_0_t;

#ifndef SBF_MEAS3RANGES_1_0_DATA_LENGTH 
#define SBF_MEAS3RANGES_1_0_DATA_LENGTH -1 
#endif

#define SBF_MEAS3RANGES_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        CommonFlags;  
  uint8_t        CumClkJumps;  
  uint16_t       Constellations;
  uint8_t        Misc;         
  uint8_t        Reserved;     
  uint8_t        Data[SBF_MEAS3RANGES_1_0_DATA_LENGTH];
} Meas3Ranges_1_0_t;

typedef M3MasterLong_1_0_t M3MasterLong_1_t;

typedef M3MasterShort_1_0_t M3MasterShort_1_t;

typedef M3MasterDeltaS_1_0_t M3MasterDeltaS_1_t;

typedef M3MasterDeltaL_1_0_t M3MasterDeltaL_1_t;

typedef M3SlaveLong_1_0_t M3SlaveLong_1_t;

typedef M3SlaveShort_1_0_t M3SlaveShort_1_t;

typedef M3SlaveDelta_1_0_t M3SlaveDelta_1_t;

#define SBF_M3SATDATA_1_SATMASK_LENGTH SBF_M3SATDATA_1_0_SATMASK_LENGTH
#define SBF_M3SATDATA_1_GLOFNLIST_LENGTH SBF_M3SATDATA_1_0_GLOFNLIST_LENGTH
typedef M3SatData_1_0_t M3SatData_1_t;

#define SBF_MEAS3RANGES_1_DATA_LENGTH SBF_MEAS3RANGES_1_0_DATA_LENGTH
typedef Meas3Ranges_1_0_t Meas3Ranges_1_t;

/*--Meas3CN0HiRes_1_0_t : ---------------------------------------------------*/
/* Extension of Meas3Ranges containing fractional C/N0 values */

#ifndef SBF_MEAS3CN0HIRES_1_0_CN0HIRES_LENGTH 
#define SBF_MEAS3CN0HIRES_1_0_CN0HIRES_LENGTH 1024 
#endif

#define SBF_MEAS3CN0HIRES_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint16_t       Flags;        
  uint8_t        CN0HiRes[SBF_MEAS3CN0HIRES_1_0_CN0HIRES_LENGTH];
} Meas3CN0HiRes_1_0_t;

#define SBF_MEAS3CN0HIRES_1_CN0HIRES_LENGTH SBF_MEAS3CN0HIRES_1_0_CN0HIRES_LENGTH
typedef Meas3CN0HiRes_1_0_t Meas3CN0HiRes_1_t;

/*--Meas3Doppler_1_0_t : ----------------------------------------------------*/
/* Extension of Meas3Ranges containing Doppler values */

#ifndef SBF_MEAS3DOPPLER_1_0_DATA_LENGTH 
#define SBF_MEAS3DOPPLER_1_0_DATA_LENGTH 2048 
#endif

#define SBF_MEAS3DOPPLER_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint16_t       Flags;        
  uint8_t        Data[SBF_MEAS3DOPPLER_1_0_DATA_LENGTH];
} Meas3Doppler_1_0_t;

#define SBF_MEAS3DOPPLER_1_DATA_LENGTH SBF_MEAS3DOPPLER_1_0_DATA_LENGTH
typedef Meas3Doppler_1_0_t Meas3Doppler_1_t;

/*--Meas3PP_1_0_t : ---------------------------------------------------------*/
/* Extension of Meas3Ranges containing proprietary flags for data post-processing. */

#ifndef SBF_MEAS3PP_1_0_DATA_LENGTH 
#define SBF_MEAS3PP_1_0_DATA_LENGTH 2048 
#endif

#define SBF_MEAS3PP_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint16_t       Flags;        
  uint8_t        Data[SBF_MEAS3PP_1_0_DATA_LENGTH];
} Meas3PP_1_0_t;

#define SBF_MEAS3PP_1_DATA_LENGTH SBF_MEAS3PP_1_0_DATA_LENGTH
typedef Meas3PP_1_0_t Meas3PP_1_t;

/*--Meas3MP_1_0_t : ---------------------------------------------------------*/
/* Extension of Meas3Ranges containing multipath corrections applied by the receiver. */

#ifndef SBF_MEAS3MP_1_0_DATA_LENGTH 
#define SBF_MEAS3MP_1_0_DATA_LENGTH 2048 
#endif

#define SBF_MEAS3MP_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint16_t       Flags;        
  uint8_t        Data[SBF_MEAS3MP_1_0_DATA_LENGTH];
} Meas3MP_1_0_t;

#define SBF_MEAS3MP_1_DATA_LENGTH SBF_MEAS3MP_1_0_DATA_LENGTH
typedef Meas3MP_1_0_t Meas3MP_1_t;

/*--IQCorr_1_0_t : ----------------------------------------------------------*/
/* Real and imaginary post-correlation values */

typedef struct
{
  uint8_t        RXChannel;    
  uint8_t        Type;         
  uint8_t        SVID;         
  uint8_t        CorrIQ_MSB;   
  uint8_t        CorrI_LSB;    
  uint8_t        CorrQ_LSB;    
  uint16_t       Reserved;     /* Reserved for future use */
} IQCorrChannelSub_1_0_t;

#ifndef SBF_IQCORR_1_0_CORRCHANNEL_LENGTH 
#define SBF_IQCORR_1_0_CORRCHANNEL_LENGTH 648 
#endif

#define SBF_IQCORR_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        N;            
  uint8_t        SBSize;       
  uint8_t        CorrDuration; 
  uint8_t        Reserved2;    /* Reserved for future use */
  uint8_t        Reserved[2];  
  IQCorrChannelSub_1_0_t CorrChannel[SBF_IQCORR_1_0_CORRCHANNEL_LENGTH];
} IQCorr_1_0_t;

/*--IQCorr_1_1_t : ----------------------------------------------------------*/
/* Real and imaginary post-correlation values */

typedef struct
{
  uint8_t        RXChannel;    
  uint8_t        Type;         
  uint8_t        SVID;         
  uint8_t        CorrIQ_MSB;   
  uint8_t        CorrI_LSB;    
  uint8_t        CorrQ_LSB;    
  uint16_t       CarrierPhaseLSB;
} IQCorrChannelSub_1_1_t;

#ifndef SBF_IQCORR_1_1_CORRCHANNEL_LENGTH 
#define SBF_IQCORR_1_1_CORRCHANNEL_LENGTH 648 
#endif

#define SBF_IQCORR_1_1_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        N;            
  uint8_t        SBSize;       
  uint8_t        CorrDuration; 
  uint8_t        CumClkJumps;  
  uint8_t        Reserved[2];  
  IQCorrChannelSub_1_1_t CorrChannel[SBF_IQCORR_1_1_CORRCHANNEL_LENGTH];
} IQCorr_1_1_t;

typedef IQCorrChannelSub_1_1_t IQCorrChannelSub_1_t;

#define SBF_IQCORR_1_CORRCHANNEL_LENGTH SBF_IQCORR_1_1_CORRCHANNEL_LENGTH
typedef IQCorr_1_1_t IQCorr_1_t;

/*--ISMR_1_0_t : ------------------------------------------------------------*/
/* Ionospheric scintillation monitor (ISMR) data */

typedef struct
{
  uint8_t        RXChannel;    
  uint8_t        Type;         
  uint8_t        SVID;         
  uint8_t        Reserved;     
  uint16_t       S4;           
  uint16_t       SigmaPhi;     
} ISMRChannel_1_0_t;

#ifndef SBF_ISMR_1_0_ISMRCHANNEL_LENGTH 
#define SBF_ISMR_1_0_ISMRCHANNEL_LENGTH 648 
#endif

#define SBF_ISMR_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        N;            
  uint8_t        SBLength;     
  uint8_t        Reserved[4];  
  ISMRChannel_1_0_t ISMRChannel[SBF_ISMR_1_0_ISMRCHANNEL_LENGTH];
} ISMR_1_0_t;

typedef ISMRChannel_1_0_t ISMRChannel_1_t;

#define SBF_ISMR_1_ISMRCHANNEL_LENGTH SBF_ISMR_1_0_ISMRCHANNEL_LENGTH
typedef ISMR_1_0_t ISMR_1_t;

/*--SQMSamples_1_0_t : ------------------------------------------------------*/
/* Correlation samples for signal quality monitoring */

typedef struct
{
  int16_t        Offset;       
  int16_t        I;            
  int16_t        Q;            
} SQMOneSample_1_0_t;

typedef struct
{
  uint8_t        RxChannel;    
  uint8_t        Type;         
  uint8_t        SVID;         
  uint8_t        N2;           
} SQMChannel_1_0_t;

#ifndef SBF_SQMSAMPLES_1_0_DATA_LENGTH 
#define SBF_SQMSAMPLES_1_0_DATA_LENGTH -1 
#endif

#define SBF_SQMSAMPLES_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        N;            
  uint8_t        SB1Length;    
  uint8_t        SB2Length;    
  uint8_t        Reserved[3];  
  uint8_t        Data[SBF_SQMSAMPLES_1_0_DATA_LENGTH];
} SQMSamples_1_0_t;

typedef SQMOneSample_1_0_t SQMOneSample_1_t;

typedef SQMChannel_1_0_t SQMChannel_1_t;

#define SBF_SQMSAMPLES_1_DATA_LENGTH SBF_SQMSAMPLES_1_0_DATA_LENGTH
typedef SQMSamples_1_0_t SQMSamples_1_t;

/*--EndOfMeas_1_0_t : -------------------------------------------------------*/
/* Measurement epoch marker */

#define SBF_ENDOFMEAS_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          
} EndOfMeas_1_0_t;

typedef EndOfMeas_1_0_t EndOfMeas_1_t;


/*==Navigation Page Blocks===================================================*/

/*--GPSRaw_1_0_t : ----------------------------------------------------------*/
/* GPS CA navigation frame */

#ifndef SBF_GPSRAW_1_0_RAWBITS_LENGTH 
#define SBF_GPSRAW_1_0_RAWBITS_LENGTH 10 
#endif

#define SBF_GPSRAW_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        PRN;          
  uint8_t        Reserved;     
  uint32_t       RawBits[SBF_GPSRAW_1_0_RAWBITS_LENGTH];
} GPSRaw_1_0_t;

#define SBF_GPSRAW_1_RAWBITS_LENGTH SBF_GPSRAW_1_0_RAWBITS_LENGTH
typedef GPSRaw_1_0_t GPSRaw_1_t;

/*--CNAVRaw_1_0_t : ---------------------------------------------------------*/
/* GPS L2C navigation frame */

#ifndef SBF_CNAVRAW_1_0_CNAVBITS_LENGTH 
#define SBF_CNAVRAW_1_0_CNAVBITS_LENGTH 10 
#endif

#define SBF_CNAVRAW_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        PRN;          
  uint8_t        Reserved;     
  uint32_t       CNAVBits[SBF_CNAVRAW_1_0_CNAVBITS_LENGTH];
} CNAVRaw_1_0_t;

#define SBF_CNAVRAW_1_CNAVBITS_LENGTH SBF_CNAVRAW_1_0_CNAVBITS_LENGTH
typedef CNAVRaw_1_0_t CNAVRaw_1_t;

/*--GEORaw_1_0_t : ----------------------------------------------------------*/
/* SBAS L1 navigation frame */

#ifndef SBF_GEORAW_1_0_RAWBITS_LENGTH 
#define SBF_GEORAW_1_0_RAWBITS_LENGTH 8 
#endif

#define SBF_GEORAW_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        PRN;          
  uint8_t        SignalSource; 
  uint32_t       RawBits[SBF_GEORAW_1_0_RAWBITS_LENGTH];
} GEORaw_1_0_t;

#define SBF_GEORAW_1_RAWBITS_LENGTH SBF_GEORAW_1_0_RAWBITS_LENGTH
typedef GEORaw_1_0_t GEORaw_1_t;

/*--GPSRawCA_1_0_t : --------------------------------------------------------*/
/* GPS CA navigation subframe */

#ifndef SBF_GPSRAWCA_1_0_NAVBITS_LENGTH 
#define SBF_GPSRAWCA_1_0_NAVBITS_LENGTH 10 
#endif

#define SBF_GPSRAWCA_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  /* Navigation Header */
  uint8_t        SVID;         
  uint8_t        CRCStatus;    

  uint8_t        ViterbiCnt;   
  uint8_t        Source;       
  uint8_t        FreqNr;       
  uint8_t        RxChannel;    
  uint32_t       NAVBits[SBF_GPSRAWCA_1_0_NAVBITS_LENGTH + 6];
} GPSRawCA_1_0_t;

#define SBF_GPSRAWCA_1_NAVBITS_LENGTH SBF_GPSRAWCA_1_0_NAVBITS_LENGTH
typedef GPSRawCA_1_0_t GPSRawCA_1_t;

/*--GPSRawL2C_1_0_t : -------------------------------------------------------*/
/* GPS L2C navigation frame */

#ifndef SBF_GPSRAWL2C_1_0_NAVBITS_LENGTH 
#define SBF_GPSRAWL2C_1_0_NAVBITS_LENGTH 10 
#endif

#define SBF_GPSRAWL2C_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  /* Navigation Header */
  uint8_t        SVID;         
  uint8_t        CRCStatus;    

  uint8_t        ViterbiCnt;   
  uint8_t        Source;       
  uint8_t        FreqNr;       
  uint8_t        RxChannel;    
  uint32_t       NAVBits[SBF_GPSRAWL2C_1_0_NAVBITS_LENGTH + 6];
} GPSRawL2C_1_0_t;

#define SBF_GPSRAWL2C_1_NAVBITS_LENGTH SBF_GPSRAWL2C_1_0_NAVBITS_LENGTH
typedef GPSRawL2C_1_0_t GPSRawL2C_1_t;

/*--GPSRawL5_1_0_t : --------------------------------------------------------*/
/* GPS L5 navigation frame */

#ifndef SBF_GPSRAWL5_1_0_NAVBITS_LENGTH 
#define SBF_GPSRAWL5_1_0_NAVBITS_LENGTH 10 
#endif

#define SBF_GPSRAWL5_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  /* Navigation Header */
  uint8_t        SVID;         
  uint8_t        CRCStatus;    

  uint8_t        ViterbiCnt;   
  uint8_t        Source;       
  uint8_t        FreqNr;       
  uint8_t        RxChannel;    
  uint32_t       NAVBits[SBF_GPSRAWL5_1_0_NAVBITS_LENGTH + 6];
} GPSRawL5_1_0_t;

#define SBF_GPSRAWL5_1_NAVBITS_LENGTH SBF_GPSRAWL5_1_0_NAVBITS_LENGTH
typedef GPSRawL5_1_0_t GPSRawL5_1_t;

/*--GPSRawL1C_1_0_t : -------------------------------------------------------*/
/* GPS L1C navigation frame */

#ifndef SBF_GPSRAWL1C_1_0_NAVBITS_LENGTH 
#define SBF_GPSRAWL1C_1_0_NAVBITS_LENGTH 57 
#endif

#define SBF_GPSRAWL1C_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        SVID;         
  uint8_t        CRCSF2;       
  uint8_t        CRCSF3;       
  uint8_t        Source;       
  uint8_t        Reserved;     
  uint8_t        RxChannel;    
  uint32_t       NAVBits[SBF_GPSRAWL1C_1_0_NAVBITS_LENGTH];
} GPSRawL1C_1_0_t;

#define SBF_GPSRAWL1C_1_NAVBITS_LENGTH SBF_GPSRAWL1C_1_0_NAVBITS_LENGTH
typedef GPSRawL1C_1_0_t GPSRawL1C_1_t;

/*--GLORawCA_1_0_t : --------------------------------------------------------*/
/* GLONASS CA navigation string */

#ifndef SBF_GLORAWCA_1_0_NAVBITS_LENGTH 
#define SBF_GLORAWCA_1_0_NAVBITS_LENGTH 3 
#endif

#define SBF_GLORAWCA_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  /* Navigation Header */
  uint8_t        SVID;         
  uint8_t        CRCStatus;    

  uint8_t        ViterbiCnt;   
  uint8_t        Source;       
  uint8_t        FreqNr;       
  uint8_t        RxChannel;    
  uint32_t       NAVBits[SBF_GLORAWCA_1_0_NAVBITS_LENGTH + 13];
} GLORawCA_1_0_t;

#define SBF_GLORAWCA_1_NAVBITS_LENGTH SBF_GLORAWCA_1_0_NAVBITS_LENGTH
typedef GLORawCA_1_0_t GLORawCA_1_t;

/*--GALRawFNAV_1_0_t : ------------------------------------------------------*/
/* Galileo F/NAV navigation page */

#ifndef SBF_GALRAWFNAV_1_0_NAVBITS_LENGTH 
#define SBF_GALRAWFNAV_1_0_NAVBITS_LENGTH 8 
#endif

#define SBF_GALRAWFNAV_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  /* Navigation Header */
  uint8_t        SVID;         
  uint8_t        CRCStatus;    

  uint8_t        ViterbiCnt;   
  uint8_t        Source;       
  uint8_t        FreqNr;       
  uint8_t        RxChannel;    
  uint32_t       NAVBits[SBF_GALRAWFNAV_1_0_NAVBITS_LENGTH + 8];
} GALRawFNAV_1_0_t;

#define SBF_GALRAWFNAV_1_NAVBITS_LENGTH SBF_GALRAWFNAV_1_0_NAVBITS_LENGTH
typedef GALRawFNAV_1_0_t GALRawFNAV_1_t;

/*--GALRawINAV_1_0_t : ------------------------------------------------------*/
/* Galileo I/NAV navigation page */

#ifndef SBF_GALRAWINAV_1_0_NAVBITS_LENGTH 
#define SBF_GALRAWINAV_1_0_NAVBITS_LENGTH 8 
#endif

#define SBF_GALRAWINAV_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  /* Navigation Header */
  uint8_t        SVID;         
  uint8_t        CRCStatus;    

  uint8_t        ViterbiCnt;   
  uint8_t        Source;       
  uint8_t        FreqNr;       
  uint8_t        RxChannel;    
  uint32_t       NAVBits[SBF_GALRAWINAV_1_0_NAVBITS_LENGTH + 8];
} GALRawINAV_1_0_t;

#define SBF_GALRAWINAV_1_NAVBITS_LENGTH SBF_GALRAWINAV_1_0_NAVBITS_LENGTH
typedef GALRawINAV_1_0_t GALRawINAV_1_t;

/*--GALRawCNAV_1_0_t : ------------------------------------------------------*/
/* Galileo C/NAV navigation page */

#ifndef SBF_GALRAWCNAV_1_0_NAVBITS_LENGTH 
#define SBF_GALRAWCNAV_1_0_NAVBITS_LENGTH 16 
#endif

#define SBF_GALRAWCNAV_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  /* Navigation Header */
  uint8_t        SVID;         
  uint8_t        CRCStatus;    

  uint8_t        ViterbiCnt;   
  uint8_t        Source;       
  uint8_t        FreqNr;       
  uint8_t        RxChannel;    
  uint32_t       NAVBits[SBF_GALRAWCNAV_1_0_NAVBITS_LENGTH + 0];
} GALRawCNAV_1_0_t;

#define SBF_GALRAWCNAV_1_NAVBITS_LENGTH SBF_GALRAWCNAV_1_0_NAVBITS_LENGTH
typedef GALRawCNAV_1_0_t GALRawCNAV_1_t;

/*--GALRawGNAV_1_0_t : ------------------------------------------------------*/

#ifndef SBF_GALRAWGNAV_1_0_NAVBITS_LENGTH 
#define SBF_GALRAWGNAV_1_0_NAVBITS_LENGTH 6 
#endif

#define SBF_GALRAWGNAV_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  /* Navigation Header */
  uint8_t        SVID;         
  uint8_t        CRCStatus;    

  uint8_t        ViterbiCnt;   
  uint8_t        Source;       
  uint8_t        FreqNr;       
  uint8_t        RxChannel;    
  uint32_t       NAVBits[SBF_GALRAWGNAV_1_0_NAVBITS_LENGTH + 10];
} GALRawGNAV_1_0_t;

#define SBF_GALRAWGNAV_1_NAVBITS_LENGTH SBF_GALRAWGNAV_1_0_NAVBITS_LENGTH
typedef GALRawGNAV_1_0_t GALRawGNAV_1_t;

/*--GALRawGNAVe_1_0_t : -----------------------------------------------------*/

#ifndef SBF_GALRAWGNAVE_1_0_NAVBITS_LENGTH 
#define SBF_GALRAWGNAVE_1_0_NAVBITS_LENGTH 11 
#endif

#define SBF_GALRAWGNAVE_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  /* Navigation Header */
  uint8_t        SVID;         
  uint8_t        CRCStatus;    

  uint8_t        ViterbiCnt;   
  uint8_t        Source;       
  uint8_t        FreqNr;       
  uint8_t        RxChannel;    
  uint32_t       NAVBits[SBF_GALRAWGNAVE_1_0_NAVBITS_LENGTH + 5];
} GALRawGNAVe_1_0_t;

#define SBF_GALRAWGNAVE_1_NAVBITS_LENGTH SBF_GALRAWGNAVE_1_0_NAVBITS_LENGTH
typedef GALRawGNAVe_1_0_t GALRawGNAVe_1_t;

/*--GEORawL1_1_0_t : --------------------------------------------------------*/
/* SBAS L1 navigation message */

#ifndef SBF_GEORAWL1_1_0_NAVBITS_LENGTH 
#define SBF_GEORAWL1_1_0_NAVBITS_LENGTH 8 
#endif

#define SBF_GEORAWL1_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  /* Navigation Header */
  uint8_t        SVID;         
  uint8_t        CRCStatus;    

  uint8_t        ViterbiCnt;   
  uint8_t        Source;       
  uint8_t        FreqNr;       
  uint8_t        RxChannel;    
  uint32_t       NAVBits[SBF_GEORAWL1_1_0_NAVBITS_LENGTH + 8];
} GEORawL1_1_0_t;

#define SBF_GEORAWL1_1_NAVBITS_LENGTH SBF_GEORAWL1_1_0_NAVBITS_LENGTH
typedef GEORawL1_1_0_t GEORawL1_1_t;

/*--GEORawL5_1_0_t : --------------------------------------------------------*/
/* SBAS L5 navigation message */

#ifndef SBF_GEORAWL5_1_0_NAVBITS_LENGTH 
#define SBF_GEORAWL5_1_0_NAVBITS_LENGTH 8 
#endif

#define SBF_GEORAWL5_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  /* Navigation Header */
  uint8_t        SVID;         
  uint8_t        CRCStatus;    

  uint8_t        ViterbiCnt;   
  uint8_t        Source;       
  uint8_t        FreqNr;       
  uint8_t        RxChannel;    
  uint32_t       NAVBits[SBF_GEORAWL5_1_0_NAVBITS_LENGTH + 8];
} GEORawL5_1_0_t;

#define SBF_GEORAWL5_1_NAVBITS_LENGTH SBF_GEORAWL5_1_0_NAVBITS_LENGTH
typedef GEORawL5_1_0_t GEORawL5_1_t;

/*--BDSRaw_1_0_t : ----------------------------------------------------------*/
/* BeiDou navigation page */

#ifndef SBF_BDSRAW_1_0_NAVBITS_LENGTH 
#define SBF_BDSRAW_1_0_NAVBITS_LENGTH 10 
#endif

#define SBF_BDSRAW_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  /* Navigation Header */
  uint8_t        SVID;         
  uint8_t        CRCStatus;    

  uint8_t        ViterbiCnt;   
  uint8_t        Source;       
  uint8_t        Reserved;     
  uint8_t        RxChannel;    
  uint32_t       NAVBits[SBF_BDSRAW_1_0_NAVBITS_LENGTH + 6];
} BDSRaw_1_0_t;

#define SBF_BDSRAW_1_NAVBITS_LENGTH SBF_BDSRAW_1_0_NAVBITS_LENGTH
typedef BDSRaw_1_0_t BDSRaw_1_t;

/*--BDSRawB1C_1_0_t : -------------------------------------------------------*/
/* BeiDou B1C navigation frame */

#ifndef SBF_BDSRAWB1C_1_0_NAVBITS_LENGTH 
#define SBF_BDSRAWB1C_1_0_NAVBITS_LENGTH 57 
#endif

#define SBF_BDSRAWB1C_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        SVID;         
  uint8_t        CRCSF2;       
  uint8_t        CRCSF3;       
  uint8_t        Source;       
  uint8_t        Reserved;     
  uint8_t        RxChannel;    
  uint32_t       NAVBits[SBF_BDSRAWB1C_1_0_NAVBITS_LENGTH];
} BDSRawB1C_1_0_t;

#define SBF_BDSRAWB1C_1_NAVBITS_LENGTH SBF_BDSRAWB1C_1_0_NAVBITS_LENGTH
typedef BDSRawB1C_1_0_t BDSRawB1C_1_t;

/*--BDSRawB2a_1_0_t : -------------------------------------------------------*/
/* BeiDou B2a navigation frame */

#ifndef SBF_BDSRAWB2A_1_0_NAVBITS_LENGTH 
#define SBF_BDSRAWB2A_1_0_NAVBITS_LENGTH 18 
#endif

#define SBF_BDSRAWB2A_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  /* Navigation Header */
  uint8_t        SVID;         
  uint8_t        CRCPassed;    

  uint8_t        ViterbiCnt;   
  uint8_t        Source;       
  uint8_t        Reserved;     
  uint8_t        RxChannel;    
  uint32_t       NAVBits[SBF_BDSRAWB2A_1_0_NAVBITS_LENGTH];
} BDSRawB2a_1_0_t;

#define SBF_BDSRAWB2A_1_NAVBITS_LENGTH SBF_BDSRAWB2A_1_0_NAVBITS_LENGTH
typedef BDSRawB2a_1_0_t BDSRawB2a_1_t;

/*--QZSRawL1CA_1_0_t : ------------------------------------------------------*/
/* QZSS L1 CA navigation frame */

#ifndef SBF_QZSRAWL1CA_1_0_NAVBITS_LENGTH 
#define SBF_QZSRAWL1CA_1_0_NAVBITS_LENGTH 10 
#endif

#define SBF_QZSRAWL1CA_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  /* Navigation Header */
  uint8_t        SVID;         
  uint8_t        CRCPassed;    

  uint8_t        Reserved;     
  uint8_t        Source;       
  uint8_t        Reserved2;    
  uint8_t        RxChannel;    
  uint32_t       NAVBits[SBF_QZSRAWL1CA_1_0_NAVBITS_LENGTH];
} QZSRawL1CA_1_0_t;

#define SBF_QZSRAWL1CA_1_NAVBITS_LENGTH SBF_QZSRAWL1CA_1_0_NAVBITS_LENGTH
typedef QZSRawL1CA_1_0_t QZSRawL1CA_1_t;

/*--QZSRawL2C_1_0_t : -------------------------------------------------------*/
/* QZSS L2C navigation frame */

#ifndef SBF_QZSRAWL2C_1_0_NAVBITS_LENGTH 
#define SBF_QZSRAWL2C_1_0_NAVBITS_LENGTH 10 
#endif

#define SBF_QZSRAWL2C_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  /* Navigation Header */
  uint8_t        SVID;         
  uint8_t        CRCPassed;    

  uint8_t        ViterbiCnt;   
  uint8_t        Source;       
  uint8_t        Reserved;     
  uint8_t        RxChannel;    
  uint32_t       NAVBits[SBF_QZSRAWL2C_1_0_NAVBITS_LENGTH];
} QZSRawL2C_1_0_t;

#define SBF_QZSRAWL2C_1_NAVBITS_LENGTH SBF_QZSRAWL2C_1_0_NAVBITS_LENGTH
typedef QZSRawL2C_1_0_t QZSRawL2C_1_t;

/*--QZSRawL5_1_0_t : --------------------------------------------------------*/
/* QZSS L5 navigation frame */

#ifndef SBF_QZSRAWL5_1_0_NAVBITS_LENGTH 
#define SBF_QZSRAWL5_1_0_NAVBITS_LENGTH 10 
#endif

#define SBF_QZSRAWL5_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  /* Navigation Header */
  uint8_t        SVID;         
  uint8_t        CRCPassed;    

  uint8_t        ViterbiCnt;   
  uint8_t        Source;       
  uint8_t        Reserved;     
  uint8_t        RxChannel;    
  uint32_t       NAVBits[SBF_QZSRAWL5_1_0_NAVBITS_LENGTH];
} QZSRawL5_1_0_t;

#define SBF_QZSRAWL5_1_NAVBITS_LENGTH SBF_QZSRAWL5_1_0_NAVBITS_LENGTH
typedef QZSRawL5_1_0_t QZSRawL5_1_t;

/*--QZSRawL6_1_0_t : --------------------------------------------------------*/
/* QZSS L6 navigation message */

#ifndef SBF_QZSRAWL6_1_0_NAVBITS_LENGTH 
#define SBF_QZSRAWL6_1_0_NAVBITS_LENGTH 63 
#endif

#define SBF_QZSRAWL6_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  /* Navigation Header */
  uint8_t        SVID;         
  uint8_t        CRCPassed;    

  uint16_t       Reserved;     
  uint8_t        Reserved2;    
  uint8_t        RxChannel;    
  uint32_t       NAVBits[SBF_QZSRAWL6_1_0_NAVBITS_LENGTH];
} QZSRawL6_1_0_t;

#define SBF_QZSRAWL6_1_NAVBITS_LENGTH SBF_QZSRAWL6_1_0_NAVBITS_LENGTH
typedef QZSRawL6_1_0_t QZSRawL6_1_t;

/*--QZSRawL1C_1_0_t : -------------------------------------------------------*/
/* QZSS L1C navigation frame */

#ifndef SBF_QZSRAWL1C_1_0_NAVBITS_LENGTH 
#define SBF_QZSRAWL1C_1_0_NAVBITS_LENGTH 57 
#endif

#define SBF_QZSRAWL1C_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        SVID;         
  uint8_t        CRCSF2;       
  uint8_t        CRCSF3;       
  uint8_t        Source;       
  uint8_t        Reserved;     
  uint8_t        RxChannel;    
  uint32_t       NAVBits[SBF_QZSRAWL1C_1_0_NAVBITS_LENGTH];
} QZSRawL1C_1_0_t;

#define SBF_QZSRAWL1C_1_NAVBITS_LENGTH SBF_QZSRAWL1C_1_0_NAVBITS_LENGTH
typedef QZSRawL1C_1_0_t QZSRawL1C_1_t;

/*--QZSRawL1S_1_0_t : -------------------------------------------------------*/
/* QZSS L1S navigation message */

#ifndef SBF_QZSRAWL1S_1_0_NAVBITS_LENGTH 
#define SBF_QZSRAWL1S_1_0_NAVBITS_LENGTH 8 
#endif

#define SBF_QZSRAWL1S_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  /* Navigation Header */
  uint8_t        SVID;         
  uint8_t        CRCPassed;    

  uint8_t        ViterbiCnt;   
  uint8_t        Source;       
  uint8_t        FreqNr;       
  uint8_t        RxChannel;    
  uint32_t       NAVBits[SBF_QZSRAWL1S_1_0_NAVBITS_LENGTH];
} QZSRawL1S_1_0_t;

#define SBF_QZSRAWL1S_1_NAVBITS_LENGTH SBF_QZSRAWL1S_1_0_NAVBITS_LENGTH
typedef QZSRawL1S_1_0_t QZSRawL1S_1_t;

/*--IRNSSRaw_1_0_t : --------------------------------------------------------*/
/* IRNSS subframe */

#ifndef SBF_IRNSSRAW_1_0_NAVBITS_LENGTH 
#define SBF_IRNSSRAW_1_0_NAVBITS_LENGTH 10 
#endif

#define SBF_IRNSSRAW_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  /* Navigation Header */
  uint8_t        SVID;         
  uint8_t        CRCPassed;    

  uint8_t        ViterbiCnt;   
  uint8_t        Source;       
  uint8_t        Reserved;     
  uint8_t        RxChannel;    
  uint32_t       NAVBits[SBF_IRNSSRAW_1_0_NAVBITS_LENGTH];
} IRNSSRaw_1_0_t;

#define SBF_IRNSSRAW_1_NAVBITS_LENGTH SBF_IRNSSRAW_1_0_NAVBITS_LENGTH
typedef IRNSSRaw_1_0_t IRNSSRaw_1_t;

/*--GNSSNavBits_1_0_t : -----------------------------------------------------*/
/* Raw navigation bits during last second */

#ifndef SBF_RAWNAVBITS_1_0_NAVBITS_LENGTH 
#define SBF_RAWNAVBITS_1_0_NAVBITS_LENGTH 16 
#endif

#define SBF_RAWNAVBITS_1_0_TOW_PRECISION 3
typedef struct
{
  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        Type;         
  uint8_t        SVID;         
  uint8_t        FreqNr;       
  uint8_t        RxChannel;    
  uint16_t       NBits;        
  uint32_t       NavBits[SBF_RAWNAVBITS_1_0_NAVBITS_LENGTH];
} RawNavBits_1_0_t;

typedef struct
{
  BlockHeader_t  Header;       

  RawNavBits_1_0_t RawNavBits;   
} GNSSNavBits_1_0_t;

#define SBF_RAWNAVBITS_1_NAVBITS_LENGTH SBF_RAWNAVBITS_1_0_NAVBITS_LENGTH
typedef RawNavBits_1_0_t RawNavBits_1_t;

typedef GNSSNavBits_1_0_t GNSSNavBits_1_t;

/*--GNSSSymbols_1_0_t : -----------------------------------------------------*/
/* Raw navigation symbols */

#ifndef SBF_RAWSYMBOLS_1_0_SYMBOLS_LENGTH 
#define SBF_RAWSYMBOLS_1_0_SYMBOLS_LENGTH 32 
#endif

#define SBF_RAWSYMBOLS_1_0_TOW_PRECISION 3
typedef struct
{
  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        Type;         
  uint8_t        SVID;         
  uint8_t        FreqNr;       
  uint8_t        RxChannel;    
  uint16_t       NSymbols;     
  uint32_t       Symbols[SBF_RAWSYMBOLS_1_0_SYMBOLS_LENGTH];
} RawSymbols_1_0_t;

typedef struct
{
  BlockHeader_t  Header;       

  RawSymbols_1_0_t RawSymbols;   
} GNSSSymbols_1_0_t;

#define SBF_RAWSYMBOLS_1_SYMBOLS_LENGTH SBF_RAWSYMBOLS_1_0_SYMBOLS_LENGTH
typedef RawSymbols_1_0_t RawSymbols_1_t;

typedef GNSSSymbols_1_0_t GNSSSymbols_1_t;


/*==GPS Decoded Message Blocks===============================================*/

/*--GPSNav_1_0_t : ----------------------------------------------------------*/
/* GPS ephemeris and clock */

#define SBF_GPEPH_1_0_TOW_PRECISION 3
#define SBF_GPEPH_1_0_T_GD_PRECISION 10
typedef struct
{
  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        PRN;          
  uint8_t        Reserved;     
  uint16_t       WN;           /* Week number (modulo 1024). [061-070:1] */
  uint8_t        CAorPonL2;    /* C/A code or P code on L2. [071-072:1] */
  uint8_t        URA;          /* User range accuracy index. [073-076:1] */
  uint8_t        health;       /* SV health. [077-082:1] */
  uint8_t        L2DataFlag;   /* L2 P data flag. [091-091:1] */
  uint16_t       IODC;         /* Issue of data clock. [083-084:1] [211-218:1] */
  uint8_t        IODE2;        /* Issue of data eph. frame 2. [061-068:2] */
  uint8_t        IODE3;        /* Issue of data eph. frame 3. [271-278:3] */
  uint8_t        FitIntFlg;    /* fit interval flag [287-287:2] */
  uint8_t        dummy;        /* introduced w.r.t. 32-bits memory alignment */
  float          T_gd;         /* Correction term T_gd (s). [197-204:1] */
  uint32_t       t_oc;         /* Clock correction t_oc (s). [219-234:1] */
  float          a_f2;         /* Clock correction a_f2 (s/s^2). [241-248:1] */
  float          a_f1;         /* Clock correction a_f1 (s/s). [249-264:1] */
  float          a_f0;         /* Clock correction a_f0 (s). [271-292:1] */
  float          C_rs;         /* radius sin ampl (m) [069-084:2] */
  float          DEL_N;        /* mean motion diff (semi-circles/s) [091-106:2] */
  SBFDOUBLE      M_0;          /* Mean Anom (semi-circles) [107-114:2] [121-144:2] */
  float          C_uc;         /* lat cosine ampl (r) [151-166:2] */
  SBFDOUBLE      e;            /* Eccentricity [167-174:2] [181-204:2] */
  float          C_us;         /* Lat sine ampl   (r) [211-226:2] */
  SBFDOUBLE      SQRT_A;       /* SQRT(A) (m^1/2) [227-234:2 241-264:2] */
  uint32_t       t_oe;         /* Reference time of ephemeris (s) [271-286:2] */
  float          C_ic;         /* inclin cos ampl (r) [061-076:3] */
  SBFDOUBLE      OMEGA_0;      /* Right Ascen at TOA (semi-circles) [077-084:3] [091-114:3] */
  float          C_is;         /* inclin sin ampl (r) [121-136:3] */
  SBFDOUBLE      i_0;          /* Orbital Inclination (semi-circles) [137-144:3] [151-174:3] */
  float          C_rc;         /* radius cos ampl (m) [181-196:3] */
  SBFDOUBLE      omega;        /* Argument of Perigee(semi-circle) [197-204:3] [211-234:3] */
  float          OMEGADOT;     /* Rate of Right Ascen(semi-circles/s) [241-264:3] */
  float          IDOT;         /* Rate of inclin (semi-circles/s) [279-292:3] */
  uint16_t       WNt_oc;       /* modified WN to go with t_oc (still modulo 1024) */
  uint16_t       WNt_oe;       /* modified WN to go with t_oe (still modulo 1024) */
} gpEph_1_0_t;

typedef struct
{
  BlockHeader_t  Header;       

  gpEph_1_0_t    Eph;          
} GPSNav_1_0_t;

typedef gpEph_1_0_t gpEph_1_t;

typedef GPSNav_1_0_t GPSNav_1_t;

/*--GPSAlm_1_0_t : ----------------------------------------------------------*/
/* Almanac data for a GPS satellite */

#define SBF_GPALM_1_0_TOW_PRECISION 3
typedef struct
{
  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        PRN;          
  uint8_t        Reserved;     
  float          e;            /* Eccentricity */
  uint32_t       t_oa;         /* Reference time of almanac (s) */
  float          delta_i;      /* Orbital Inclination (semi-circles) */
  float          OMEGADOT;     /* Rate of Right Ascen (r/s) */
  float          SQRT_A;       /* SQRT(A) (m^1/2) */
  float          OMEGA_0;      /* Right Ascen at TOA (semi-circles) */
  float          omega;        /* Argument of Perigee (semi-circles) */
  float          M_0;          /* Mean Anom (semi-circles) */
  float          a_f1;         /* Clock correction a_f1 (s/s). */
  float          a_f0;         /* Clock correction a_f0 (s). */
  uint8_t        WN_a;         /* Reference week number of almanac. */
  uint8_t        config;       /* See GPS SPS Sig. Spec. 2nd Edition 1995 2.4.5.4 p31.
      -bit 00-03: 4bit anti spoofing & configuration. */
  uint8_t        health8;      /* -bit 00-07: 8bit health data in alm. subframes. */
  uint8_t        health6;      /* -bit 08-13: 6bit health data SF4P25 and SF5P25. */
} gpAlm_1_0_t;

typedef struct
{
  BlockHeader_t  Header;       

  gpAlm_1_0_t    Alm;          
} GPSAlm_1_0_t;

typedef gpAlm_1_0_t gpAlm_1_t;

typedef GPSAlm_1_0_t GPSAlm_1_t;

/*--GPSIon_1_0_t : ----------------------------------------------------------*/
/* Ionosphere data from the GPS subframe 5 */

#define SBF_GPION_1_0_TOW_PRECISION 3
typedef struct
{
  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        PRN;          
  uint8_t        Reserved;     
  float          alpha_0;      /* (sec) [069-076:4p18] */
  float          alpha_1;      /* (sec/semicircle) [077-084:4p18] */
  float          alpha_2;      /* (sec/semicircle^2) [091-098:4p18] */
  float          alpha_3;      /* (sec/semicircle^3) [099-106:4p18] */
  float          beta_0;       /* (sec) [107-114:4p18] */
  float          beta_1;       /* (sec/semicircle) [121-128:4p18] */
  float          beta_2;       /* (sec/semicircle^2) [129-136:4p18] */
  float          beta_3;       /* (sec/semicircle^3) [137-144:4p18] */
} gpIon_1_0_t;

typedef struct
{
  BlockHeader_t  Header;       

  gpIon_1_0_t    Ion;          
} GPSIon_1_0_t;

typedef gpIon_1_0_t gpIon_1_t;

typedef GPSIon_1_0_t GPSIon_1_t;

/*--GPSUtc_1_0_t : ----------------------------------------------------------*/
/* GPS-UTC data from GPS subframe 5 */

#define SBF_GPUTC_1_0_TOW_PRECISION 3
#define SBF_GPUTC_1_0_A_1_PRECISION 18
#define SBF_GPUTC_1_0_A_0_PRECISION 16
typedef struct
{
  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        PRN;          
  uint8_t        Reserved;     
  float          A_1;          /* (sec/sec) [151-176:4p18] */
  SBFDOUBLE      A_0;          /* (sec) [181-194:4] [211-218:4] */
  uint32_t       t_ot;         /* (sec) [219-226:4p18] */
  uint8_t        WN_t;         /* (wk) [227-234:4p18] */
  int8_t         DEL_t_LS;     /* (sec) [241-218:4p18] */
  uint8_t        WN_LSF;       /* (wk) [219-226:4p18] */
  uint8_t        DN;           /* (days) [227-234:4p18] */
  int8_t         DEL_t_LSF;    /* (sec) [271-278:4p18] */
} gpUtc_1_0_t;

typedef struct
{
  BlockHeader_t  Header;       

  gpUtc_1_0_t    Utc;          
} GPSUtc_1_0_t;

typedef gpUtc_1_0_t gpUtc_1_t;

typedef GPSUtc_1_0_t GPSUtc_1_t;

/*--GPSCNav_1_0_t : ---------------------------------------------------------*/
/* CNAV Ephemeris data for one satellite. */

#define SBF_GPEPHCNAV_1_0_TOW_PRECISION 3
#define SBF_GPEPHCNAV_1_0_T_GD_PRECISION 3
#define SBF_GPEPHCNAV_1_0_ISC_L1CA_PRECISION 3
#define SBF_GPEPHCNAV_1_0_ISC_L2C_PRECISION 3
#define SBF_GPEPHCNAV_1_0_ISC_L5I5_PRECISION 3
#define SBF_GPEPHCNAV_1_0_ISC_L5Q5_PRECISION 3
typedef struct
{
  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        PRN;          /* PRN [M10/11] */
  uint8_t        Alert;        
  uint16_t       WN;           /* WN [MT10] */
  uint8_t        Health;       /* Health [MT10] */
  int8_t         URA_ED;       /* ED Accuracy Index [MT10] */
  uint32_t       t_op;         /* Data Predict TOW (s) [MT10/30] */
  uint32_t       t_oe;         /* Ephemeris Reference TOW (s) [MT10/11] */
  SBFDOUBLE      A;            /* Semi Major Axis (m) [MT10] */
  SBFDOUBLE      A_DOT;        /* Semi Major Axis Change Rate (m/s) [MT10] */
  float          DELTA_N;      /* Mean Motion Delta (semi-circles/s) [MT10] */
  float          DELTA_N_DOT;  /* Mean Motion Rate Delta (semi-circles/s^2) [MT10] */
  SBFDOUBLE      M_0;          /* Mean anomaly (semi-circles) [MT10] */
  SBFDOUBLE      e;            /* Eccentricity [MT10] */
  SBFDOUBLE      omega;        /* Argument of Perigee (semi-circles) [MT10] */
  SBFDOUBLE      OMEGA_0;      /* Reference Right Ascension Angle (semi-circles) [MT11] */
  float          OMEGADOT;     /* Right Ascension Rate (semi-circles/s) [MT11] */
  SBFDOUBLE      I_0;          /* Inclination Angle (semi-circles) [MT11] */
  float          I_0_DOT;      /* Inclination Angle Rate (semi-circles/s) [MT11] */
  float          C_is;         /* Amplitude is (r) [MT11] */
  float          C_ic;         /* Amplitude ic (r) [MT11] */
  float          C_rs;         /* Amplitude rs (m) [MT11] */
  float          C_rc;         /* Amplitude rc (m) [MT11] */
  float          C_us;         /* Amplitude us (r) [MT11] */
  float          C_uc;         /* Amplitude uc (r) [MT11] */
  uint32_t       t_oc;         /* Clock Reference Time (s) [MT30] */
  int8_t         URA_NED0;     /* NED Accuracy Index [MT30] */
  uint8_t        URA_NED1;     /* NED Accuracy Change Index [MT30] */
  uint8_t        URA_NED2;     /* NED Accuracy Change Rate Index [MT30] */
  uint8_t        WN_op;        /* Data Predict WN [MT30] */
  float          a_f2;         /* Clock Drift Rate Correction Coefficient (s/s^2) [MT30] */
  float          a_f1;         /* Clock Drift Correction Coefficient (s/s) [MT30] */
  SBFDOUBLE      a_f0;         /* Clock Bias Correction Coefficient (s) [MT30] */
  float          T_gd;         /* Tgd (s) [MT30] */
  float          ISC_L1CA;     /* ISC L1CA (s) [MT30] */
  float          ISC_L2C;      /* ISC L2C (s) [MT30] */
  float          ISC_L5I5;     /* ISC L5I5 (s) [MT30] */
  float          ISC_L5Q5;     /* ISC L5Q5 (s) [MT30] */
} gpEphCNAV_1_0_t;

typedef struct
{
  BlockHeader_t  Header;       

  gpEphCNAV_1_0_t Eph;          
} GPSCNav_1_0_t;

typedef gpEphCNAV_1_0_t gpEphCNAV_1_t;

typedef GPSCNav_1_0_t GPSCNav_1_t;


/*==GLONASS Decoded Message Blocks===========================================*/

/*--GLONav_1_0_t : ----------------------------------------------------------*/
/* GLONASS ephemeris and clock */

#define SBF_GLEPH_1_0_TOW_PRECISION 3
#define SBF_GLEPH_1_0_GAMMA_PRECISION 14
#define SBF_GLEPH_1_0_DTAU_PRECISION 11
typedef struct
{
  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        SVID;         /* Slot number + 37 */
  uint8_t        FreqNr;       /* Frequency number + 8 */
  SBFDOUBLE      x;            /* X component of satellite position in PZ-90 (km) */
  SBFDOUBLE      y;            /* Y component of satellite position in PZ-90 (km) */
  SBFDOUBLE      z;            /* Z component of satellite position in PZ-90 (km) */
  float          dx;           /* X component of satellite velocity in PZ-90 (km/s) */
  float          dy;           /* Y component of satellite velocity in PZ-90 (km/s) */
  float          dz;           /* Z component of satellite velocity in PZ-90 (km/s) */
  float          ddx;          /* X component of satellite acceleration in PZ-90 (km/s2) */
  float          ddy;          /* Y component of satellite acceleration in PZ-90 (km/s2) */
  float          ddz;          /* Z component of satellite acceleration in PZ-90 (km/s2) */
  float          gamma;        /* relative deviation of predicted carrier frequency */
  float          tau;          /* corr to nth satellite time rel to GLONASS time t_c */
  float          dtau;         /* time difference between L2 and L1 sub-band (s) */
  uint32_t       t_oe;         /* reference time of GLONASS ephemeris in GPS time frame */
  uint16_t       WNt_oe;       /* reference WN of GLONASS ephemeris in GPS time frame */
  uint8_t        P1;           /* length of applicability interval (min) */
  uint8_t        P2;           /* odd/even flag of t_b */
  uint8_t        E;            /* age of immediate data (days) */
  uint8_t        B;            /* health flag, unhealthy if MSB is set */
  uint16_t       tb;           /* time of day [min] defining middle of validity interval */
  uint8_t        M;            /* GLONASS-M only: GLONASS-M satellite identifier (01, otherwise 00) */
  uint8_t        P;            /* GLONASS-M only: mode of computation of freq/time corr data */
  uint8_t        l;            /* GLONASS-M only: health flag, 0=healthy, 1=unhealthy */
  uint8_t        P4;           /* GLONASS-M only: 'updated' flag of ephemeris data */
  uint16_t       N_T;          /* GLONASS-M only: current day within 4-year interval */
  uint16_t       F_T;          /* GLONASS-M only: predicted user range accuracy at time t_b */
} glEph_1_0_t;

typedef struct
{
  BlockHeader_t  Header;       

  glEph_1_0_t    Eph;          
} GLONav_1_0_t;

typedef glEph_1_0_t glEph_1_t;

typedef GLONav_1_0_t GLONav_1_t;

/*--GLOAlm_1_0_t : ----------------------------------------------------------*/
/* Almanac data for a GLONASS satellite */

#ifndef SBF_GLALM_1_0__PADDING_LENGTH 
#define SBF_GLALM_1_0__PADDING_LENGTH 2 
#endif

#define SBF_GLALM_1_0_TOW_PRECISION 3
typedef struct
{
  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        SVID;         /* Slot number + 37 */
  uint8_t        FreqNr;       /* Frequency number + 8 */
  float          epsilon;      /* eccentricity of satellite at t_ln */
  uint32_t       t_oa;         /* Reference time of almanac in GPS time frame (TOW) */
  float          Delta_i;      /* corr to mean value of incl at t_ln (semi-circle) */
  float          lambda;       /* longitude of first ascending node (semi-circle) */
  float          t_ln;         /* time of first ascending node passage (s) */
  float          omega;        /* argument of perigee at t_ln (semi-circle) */
  float          Delta_T;      /* corr to mean value of T_dr at t_ln (s/orb period) */
  float          dDelta_T;     /* rate of change of T_dr (s/orb period^2) */
  float          tau;          /* coarse value of satellite time corr at t_ln (s) */
  uint8_t        WN_a;         /* Reference week number of almanac in GPS time frame %256 */
  uint8_t        C;            /* general health flag (1 indicates healthy) */
  uint16_t       N;            /* calender day number within 4 year period */
  uint8_t        M;            /* GLONASS-M only: GLONASS-M satellite identifier (01, otherwise 00) */
  uint8_t        N_4;          /* GLONASS-M only: 4 year interval number, starting from 1996 */

  uint8_t        _padding[SBF_GLALM_1_0__PADDING_LENGTH];
} glAlm_1_0_t;

typedef struct
{
  BlockHeader_t  Header;       

  glAlm_1_0_t    Alm;          
} GLOAlm_1_0_t;

#define SBF_GLALM_1__PADDING_LENGTH SBF_GLALM_1_0__PADDING_LENGTH
typedef glAlm_1_0_t glAlm_1_t;

typedef GLOAlm_1_0_t GLOAlm_1_t;

/*--GLOTime_1_0_t : ---------------------------------------------------------*/
/* GLO-UTC, GLO-GPS and GLO-UT1 data */

#define SBF_GLTIME_1_0_TOW_PRECISION 3
#define SBF_GLTIME_1_0_TAU_GPS_PRECISION 9
#define SBF_GLTIME_1_0_TAU_C_PRECISION 9
typedef struct
{
  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        SVID;         /* Slot number + 37 */
  uint8_t        FreqNr;       /* Frequency number + 8 */
  uint8_t        N_4;          
  uint8_t        KP;           /* notification of leap second */
  uint16_t       N;            
  float          tau_GPS;      
  SBFDOUBLE      tau_c;        
  float          B1;           /* diff between UT1 and UTC at start of curr day(s) */
  float          B2;           /* daily change to diff delta UT1 (s/msd) */
} glTime_1_0_t;

typedef struct
{
  BlockHeader_t  Header;       

  glTime_1_0_t   GLOTime;      
} GLOTime_1_0_t;

typedef glTime_1_0_t glTime_1_t;

typedef GLOTime_1_0_t GLOTime_1_t;


/*==Galileo Decoded Message Blocks===========================================*/

/*--GALNav_1_0_t : ----------------------------------------------------------*/
/* Galileo ephemeris, clock, health and BGD */

#define SBF_GAEPH_1_0_TOW_PRECISION 3
#define SBF_GAEPH_1_0_BGD_L1E5A_PRECISION 12
#define SBF_GAEPH_1_0_BGD_L1E5B_PRECISION 12
#define SBF_GAEPH_1_0_BGD_L1AE6A_PRECISION 12
typedef struct
{
  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        SVID;         /* SBF range 71-102 SIS range 1-64 F1/I4/G1 */
  uint8_t        Source;       /* obtained from INAV (2) or FNAV (16) */
  SBFDOUBLE      SQRT_A;       /* SQRT(A) (m^1/2) [F2/I1/G1] */
  SBFDOUBLE      M_0;          /* Mean Anom (semi-circles) [F2/I1/G1] */
  SBFDOUBLE      e;            /* Eccentricity [F2/I1/G1] */
  SBFDOUBLE      i_0;          /* Orbital Inclination (semi-circles) [F3/I2/G2] */
  SBFDOUBLE      omega;        /* Argument of Perigee (semi-circles) [F3/I2/G2] */
  SBFDOUBLE      OMEGA_0;      /* Right Ascen at TOA (semi-circles) [F2/I2/G2] */
  float          OMEGADOT;     /* Rate of Right Ascen(semi-circles/s) [F2/I3/G1] */
  float          IDOT;         /* Rate of inclin (semi-circles/s) [F2/I2/G1] */
  float          DEL_N;        /* mean motion diff (semi-circles/s) [F3/I3/G2] */
  float          C_uc;         /* lat cosine ampl (rad) [F3/I3/G2] */
  float          C_us;         /* Lat sine ampl   (rad) [F3/I3/G3] */
  float          C_rc;         /* radius cos ampl (m) [F3/I3/G3] */
  float          C_rs;         /* radius sin ampl (m) [F3/I3/G3] */
  float          C_ic;         /* inclin cos ampl (r) [F4/I4/G3] */
  float          C_is;         /* inclin sin ampl (r) [F4/I4/G3] */
  uint32_t       t_oe;         /* Reference time of ephemeris (s) [F3/I1/G2] */
  uint32_t       t_oc;         /* Clock correction t_oc (s) [F1/I4/G1] */
  float          a_f2;         /* Clock correction a_f2 (s/s^2) [F1/I4/G1] */
  float          a_f1;         /* Clock correction a_f1 (s/s) [F1/I4/G1] */
  SBFDOUBLE      a_f0;         /* Clock correction a_f0 (s) [F1/I4/G1] */
  uint16_t       WNt_oe;       /* modified WN to go with t_oe */
  uint16_t       WNt_oc;       /* modified WN to go with t_oc */
  uint16_t       IODnav;       /* 0 - 1023 [Fx/Ix/Gx] */
  uint16_t       Health_OSSOL; /* contains DVS,HS for OS an SOL */
  uint8_t        Health_PRS;   /* contains DVS,HS for PRS */
  uint8_t        SISA_L1E5a;   /* Signal In Space Accuracy [F1] */
  uint8_t        SISA_L1E5b;   /* Signal In Space Accuracy [I3] */
  uint8_t        SISA_L1AE6A;  /* Signal In Space Accuracy [G3] */
  float          BGD_L1E5a;    /* Correction term T_gd (s). [F1/I5] */
  float          BGD_L1E5b;    /* Correction term T_gd (s). [I5] */
  float          BGD_L1AE6A;   /* Correction term T_gd (s). [G4] */
  uint8_t        CNAVenc;      /* C/NAV encryption status [I1] */
} gaEph_1_0_t;

typedef struct
{
  BlockHeader_t  Header;       

  gaEph_1_0_t    Eph;          
} GALNav_1_0_t;

typedef gaEph_1_0_t gaEph_1_t;

typedef GALNav_1_0_t GALNav_1_t;

/*--GALAlm_1_0_t : ----------------------------------------------------------*/
/* Almanac data for a Galileo satellite */

#define SBF_GAALM_1_0_TOW_PRECISION 3
typedef struct
{
  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        SVID;         /* SBF range 71-102 SIS range 1-64 F1/I4/G1 */
  uint8_t        Source;       /* bitfield according to sigType */
  float          e;            /* Eccentricity */
  uint32_t       t_oa;         /* Reference time of almanac (s) */
  float          delta_i;      /* Orbital Inclination (semi-circles) */
  float          OMEGADOT;     /* Rate of Right Ascen (r/s) */
  float          SQRT_A;       /* SQRT(A) (m^1/2) */
  float          OMEGA_0;      /* Right Ascen at TOA (semi-circles) */
  float          omega;        /* Argument of Perigee (semi-circles) */
  float          M_0;          /* Mean Anom (semi-circles) */
  float          a_f1;         /* Clock correction a_f1 (s/s). */
  float          a_f0;         /* Clock correction a_f0 (s). */
  uint8_t        WN_a;         /* Reference week number of almanac. */
  uint8_t        SVID_A;       
  uint16_t       health;       
  uint8_t        IODa;         
} gaAlm_1_0_t;

typedef struct
{
  BlockHeader_t  Header;       

  gaAlm_1_0_t    Alm;          
} GALAlm_1_0_t;

typedef gaAlm_1_0_t gaAlm_1_t;

typedef GALAlm_1_0_t GALAlm_1_t;

/*--GALIon_1_0_t : ----------------------------------------------------------*/
/* NeQuick Ionosphere model parameters */

#define SBF_GAION_1_0_TOW_PRECISION 3
typedef struct
{
  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        SVID;         /* SBF range 71-102 SIS range 1-64 F1/I4/G1 */
  uint8_t        Source;       /* bitfield according to sigType */
  float          a_i0;         /* (sec) [F1/I5/C4] */
  float          a_i1;         /* (sec/semicircle) [F1/I5/C4] */
  float          a_i2;         /* (sec/semicircle^2) [F1/I5/C4] */
  uint8_t        StormFlags;   /* 5 bits: 1 for each region [F1/I5/C4] */
} gaIon_1_0_t;

typedef struct
{
  BlockHeader_t  Header;       

  gaIon_1_0_t    Ion;          
} GALIon_1_0_t;

typedef gaIon_1_0_t gaIon_1_t;

typedef GALIon_1_0_t GALIon_1_t;

/*--GALUtc_1_0_t : ----------------------------------------------------------*/
/* GST-UTC data */

#define SBF_GAUTC_1_0_TOW_PRECISION 3
#define SBF_GAUTC_1_0_A_1_PRECISION 3
#define SBF_GAUTC_1_0_A_0_PRECISION 3
typedef struct
{
  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        SVID;         /* SBF range 71-102 SIS range 1-64 [F1/I4/G1] */
  uint8_t        Source;       /* bitfield according to sigType */
  float          A_1;          /* (sec/sec) [F4/I6/C5] */
  SBFDOUBLE      A_0;          /* (sec) [F4/I6/C5] */
  uint32_t       t_ot;         /* (sec) [F4/I6/C5] */
  uint8_t        WN_ot;        /* (wk) [F4/I6/C5] */
  int8_t         DEL_t_LS;     /* (sec) [F4/I6/C5] */
  uint8_t        WN_LSF;       /* (wk) [F4/I6/C5] */
  uint8_t        DN;           /* (days) [F4/I6/C5] */
  int8_t         DEL_t_LSF;    /* (sec) [F4/I6/C5] */
} gaUtc_1_0_t;

typedef struct
{
  BlockHeader_t  Header;       

  gaUtc_1_0_t    Utc;          
} GALUtc_1_0_t;

typedef gaUtc_1_0_t gaUtc_1_t;

typedef GALUtc_1_0_t GALUtc_1_t;

/*--GALGstGps_1_0_t : -------------------------------------------------------*/
/* GST-GPS data */

#define SBF_GAGSTGPS_1_0_TOW_PRECISION 3
typedef struct
{
  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        SVID;         /* SBF range 71-102 SIS range 1-64 [F1/I4/G1] */
  uint8_t        Source;       /* bitfield according to sigType */
  float          A_1G;         /* (sec/sec) [F4/I10/C5] */
  float          A_0G;         /* (sec) [F4/I10/C5] */
  uint32_t       t_oG;         /* (sec) [F4/I10/C5] */
  uint8_t        WN_oG;        /* (wk) [F4/I10/C5] */
} gaGstGps_1_0_t;

typedef struct
{
  BlockHeader_t  Header;       

  gaGstGps_1_0_t GstGps;       
} GALGstGps_1_0_t;

typedef gaGstGps_1_0_t gaGstGps_1_t;

typedef GALGstGps_1_0_t GALGstGps_1_t;

/*--GALSARRLM_1_0_t : -------------------------------------------------------*/
/* Search-and-rescue return link message */

#ifndef SBF_GASARRLM_1_0_RLMBITS_LENGTH 
#define SBF_GASARRLM_1_0_RLMBITS_LENGTH 5 
#endif

#define SBF_GASARRLM_1_0_TOW_PRECISION 3
typedef struct
{
  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        SVID;         
  uint8_t        Source;       /* bitfield according to sigType */
  uint8_t        RLMLength;    
  uint8_t        Reserved[3];  
  uint32_t       RLMBits[SBF_GASARRLM_1_0_RLMBITS_LENGTH];
} gaSARRLM_1_0_t;

typedef struct
{
  BlockHeader_t  Header;       

  gaSARRLM_1_0_t galSARRLM;    
} GALSARRLM_1_0_t;

#define SBF_GASARRLM_1_RLMBITS_LENGTH SBF_GASARRLM_1_0_RLMBITS_LENGTH
typedef gaSARRLM_1_0_t gaSARRLM_1_t;

typedef GALSARRLM_1_0_t GALSARRLM_1_t;


/*==BeiDou Decoded Message Blocks============================================*/

/*--BDSNav_1_0_t : ----------------------------------------------------------*/
/* BeiDou ephemeris and clock */

#define SBF_CMPEPH_1_0_TOW_PRECISION 3
#define SBF_CMPEPH_1_0_T_GD1_PRECISION 10
#define SBF_CMPEPH_1_0_T_GD2_PRECISION 10
typedef struct
{
  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        PRN;          
  uint8_t        Reserved;     
  uint16_t       WN;           
  uint8_t        URA;          
  uint8_t        SatH1;        
  uint8_t        IODC;         
  uint8_t        IODE;         
  uint16_t       Reserved2;    
  float          T_GD1;        
  float          T_GD2;        
  uint32_t       t_oc;         
  float          a_f2;         
  float          a_f1;         
  float          a_f0;         
  float          C_rs;         
  float          DEL_N;        
  SBFDOUBLE      M_0;          
  float          C_uc;         
  SBFDOUBLE      e;            
  float          C_us;         
  SBFDOUBLE      SQRT_A;       
  uint32_t       t_oe;         
  float          C_ic;         
  SBFDOUBLE      OMEGA_0;      
  float          C_is;         
  SBFDOUBLE      i_0;          
  float          C_rc;         
  SBFDOUBLE      omega;        
  float          OMEGADOT;     
  float          IDOT;         
  uint16_t       WNt_oc;       
  uint16_t       WNt_oe;       
} cmpEph_1_0_t;

typedef struct
{
  BlockHeader_t  Header;       

  cmpEph_1_0_t   Eph;          
} BDSNav_1_0_t;

typedef cmpEph_1_0_t cmpEph_1_t;

typedef BDSNav_1_0_t BDSNav_1_t;

/*--BDSIon_1_0_t : ----------------------------------------------------------*/
/* BeiDou Ionospheric delay model parameters */

#define SBF_CMPION_1_0_TOW_PRECISION 3
typedef struct
{
  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        PRN;          
  uint8_t        Reserved;     
  float          alpha_0;      /* (sec) [069-076:4p18] */
  float          alpha_1;      /* (sec/semicircle) [077-084:4p18] */
  float          alpha_2;      /* (sec/semicircle^2) [091-098:4p18] */
  float          alpha_3;      /* (sec/semicircle^3) [099-106:4p18] */
  float          beta_0;       /* (sec) [107-114:4p18] */
  float          beta_1;       /* (sec/semicircle) [121-128:4p18] */
  float          beta_2;       /* (sec/semicircle^2) [129-136:4p18] */
  float          beta_3;       /* (sec/semicircle^3) [137-144:4p18] */
} cmpIon_1_0_t;

typedef struct
{
  BlockHeader_t  Header;       

  cmpIon_1_0_t   Ion;          
} BDSIon_1_0_t;

typedef cmpIon_1_0_t cmpIon_1_t;

typedef BDSIon_1_0_t BDSIon_1_t;

/*--BDSUtc_1_0_t : ----------------------------------------------------------*/
/* BDT-UTC data */

#define SBF_CMPUTC_1_0_TOW_PRECISION 3
#define SBF_CMPUTC_1_0_A_1_PRECISION 18
#define SBF_CMPUTC_1_0_A_0_PRECISION 16
typedef struct
{
  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        PRN;          
  uint8_t        Reserved;     
  float          A_1;          /* (sec/sec) [151-176:4p18] */
  SBFDOUBLE      A_0;          /* (sec) [181-194:4] [211-218:4] */
  int8_t         DEL_t_LS;     /* (sec) [241-218:4p18] */
  uint8_t        WN_LSF;       /* (wk) [219-226:4p18] */
  uint8_t        DN;           
  int8_t         DEL_t_LSF;    /* (sec) [271-278:4p18] */
} cmpUtc_1_0_t;

typedef struct
{
  BlockHeader_t  Header;       

  cmpUtc_1_0_t   Utc;          
} BDSUtc_1_0_t;

typedef cmpUtc_1_0_t cmpUtc_1_t;

typedef BDSUtc_1_0_t BDSUtc_1_t;


/*==QZSS Decoded Message Blocks==============================================*/

/*--QZSNav_1_0_t : ----------------------------------------------------------*/
/* QZSS ephemeris and clock */

#define SBF_QZEPH_1_0_TOW_PRECISION 3
#define SBF_QZEPH_1_0_T_GD_PRECISION 10
typedef struct
{
  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        PRN;          
  uint8_t        Reserved;     
  uint16_t       WN;           /* Week number (modulo 1024). [061-070:1] */
  uint8_t        CAorPonL2;    /* C/A code or P code on L2. [071-072:1] */
  uint8_t        URA;          /* User range accuracy index. [073-076:1] */
  uint8_t        health;       /* SV health. [077-082:1] */
  uint8_t        L2DataFlag;   /* L2 P data flag. [091-091:1] */
  uint16_t       IODC;         /* Issue of data clock. [083-084:1] [211-218:1] */
  uint8_t        IODE2;        /* Issue of data eph. frame 2. [061-068:2] */
  uint8_t        IODE3;        /* Issue of data eph. frame 3. [271-278:3] */
  uint8_t        FitIntFlg;    /* fit interval flag [287-287:2] */
  uint8_t        Reserved2;    /* introduced w.r.t. 32-bits memory alignment */
  float          T_gd;         /* Correction term T_gd (s). [197-204:1] */
  uint32_t       t_oc;         /* Clock correction t_oc (s). [219-234:1] */
  float          a_f2;         /* Clock correction a_f2 (s/s^2). [241-248:1] */
  float          a_f1;         /* Clock correction a_f1 (s/s). [249-264:1] */
  float          a_f0;         /* Clock correction a_f0 (s). [271-292:1] */
  float          C_rs;         /* radius sin ampl (m) [069-084:2] */
  float          DEL_N;        /* mean motion diff (semi-circles/s) [091-106:2] */
  SBFDOUBLE      M_0;          /* Mean Anom (semi-circles) [107-114:2] [121-144:2] */
  float          C_uc;         /* lat cosine ampl (r) [151-166:2] */
  SBFDOUBLE      e;            /* Eccentricity [167-174:2] [181-204:2] */
  float          C_us;         /* Lat sine ampl   (r) [211-226:2] */
  SBFDOUBLE      SQRT_A;       /* SQRT(A) (m^1/2) [227-234:2 241-264:2] */
  uint32_t       t_oe;         /* Reference time of ephemeris (s) [271-286:2] */
  float          C_ic;         /* inclin cos ampl (r) [061-076:3] */
  SBFDOUBLE      OMEGA_0;      /* Right Ascen at TOA (semi-circles) [077-084:3] [091-114:3] */
  float          C_is;         /* inclin sin ampl (r) [121-136:3] */
  SBFDOUBLE      i_0;          /* Orbital Inclination (semi-circles) [137-144:3] [151-174:3] */
  float          C_rc;         /* radius cos ampl (m) [181-196:3] */
  SBFDOUBLE      omega;        /* Argument of Perigee(semi-circle) [197-204:3] [211-234:3] */
  float          OMEGADOT;     /* Rate of Right Ascen(semi-circles/s) [241-264:3] */
  float          IDOT;         /* Rate of inclin (semi-circles/s) [279-292:3] */
  uint16_t       WNt_oc;       /* modified WN to go with t_oc (still modulo 1024) */
  uint16_t       WNt_oe;       /* modified WN to go with t_oe (still modulo 1024) */
} qzEph_1_0_t;

typedef struct
{
  BlockHeader_t  Header;       

  qzEph_1_0_t    Eph;          
} QZSNav_1_0_t;

typedef qzEph_1_0_t qzEph_1_t;

typedef QZSNav_1_0_t QZSNav_1_t;


/*==SBAS L1 Decoded Message Blocks===========================================*/

/*--GEOMT00_1_0_t : ---------------------------------------------------------*/
/* MT00 : SBAS Don't use for safety applications */

#define SBF_RAMT00_1_0_TOW_PRECISION 3
typedef struct
{
  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        PRN;          
} raMT00_1_0_t;

typedef struct
{
  BlockHeader_t  Header;       

  raMT00_1_0_t   GeoMT00;      
} GEOMT00_1_0_t;

typedef raMT00_1_0_t raMT00_1_t;

typedef GEOMT00_1_0_t GEOMT00_1_t;

/*--GEOPRNMask_1_0_t : ------------------------------------------------------*/
/* MT01 : PRN Mask assignments */

#ifndef SBF_RAPRNMASK_1_0_PRNMASK_LENGTH 
#define SBF_RAPRNMASK_1_0_PRNMASK_LENGTH 51 
#endif

#define SBF_RAPRNMASK_1_0_TOW_PRECISION 3
typedef struct
{
  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        PRN;          
  uint8_t        IODP;         
  uint8_t        NbrPRNs;      
  uint8_t        PRNMask[SBF_RAPRNMASK_1_0_PRNMASK_LENGTH];
} raPRNMask_1_0_t;

typedef struct
{
  BlockHeader_t  Header;       

  raPRNMask_1_0_t GeoPRNMask;   
} GEOPRNMask_1_0_t;

#define SBF_RAPRNMASK_1_PRNMASK_LENGTH SBF_RAPRNMASK_1_0_PRNMASK_LENGTH
typedef raPRNMask_1_0_t raPRNMask_1_t;

typedef GEOPRNMask_1_0_t GEOPRNMask_1_t;

/*--GEOFastCorr_1_0_t : -----------------------------------------------------*/
/* MT02-05/24: Fast Corrections */

typedef struct
{
  uint8_t        PRNMaskNo;    
  uint8_t        UDREI;        
  uint8_t        Reserved[2];  
  float          PRC;          
} FastCorr_1_0_t;

#ifndef SBF_RAFASTCORR_1_0_FASTCORR_LENGTH 
#define SBF_RAFASTCORR_1_0_FASTCORR_LENGTH 13 
#endif

#define SBF_RAFASTCORR_1_0_TOW_PRECISION 3
typedef struct
{
  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        PRN;          
  uint8_t        MT;           
  uint8_t        IODP;         
  uint8_t        IODF;         
  uint8_t        N;            
  uint8_t        SBLength;     
  FastCorr_1_0_t FastCorr[SBF_RAFASTCORR_1_0_FASTCORR_LENGTH];
} raFastCorr_1_0_t;

typedef struct
{
  BlockHeader_t  Header;       

  raFastCorr_1_0_t GeoFastCorr;  
} GEOFastCorr_1_0_t;

typedef FastCorr_1_0_t FastCorr_1_t;

#define SBF_RAFASTCORR_1_FASTCORR_LENGTH SBF_RAFASTCORR_1_0_FASTCORR_LENGTH
typedef raFastCorr_1_0_t raFastCorr_1_t;

typedef GEOFastCorr_1_0_t GEOFastCorr_1_t;

/*--GEOIntegrity_1_0_t : ----------------------------------------------------*/
/* MT06 : Integrity information */

#ifndef SBF_RAINTEGRITY_1_0_IODF_LENGTH 
#define SBF_RAINTEGRITY_1_0_IODF_LENGTH 4 
#endif

#ifndef SBF_RAINTEGRITY_1_0_UDREI_LENGTH 
#define SBF_RAINTEGRITY_1_0_UDREI_LENGTH 51 
#endif

#define SBF_RAINTEGRITY_1_0_TOW_PRECISION 3
typedef struct
{
  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        PRN;          
  uint8_t        Reserved;     
  uint8_t        IODF[SBF_RAINTEGRITY_1_0_IODF_LENGTH];
  uint8_t        UDREI[SBF_RAINTEGRITY_1_0_UDREI_LENGTH];
} raIntegrity_1_0_t;

typedef struct
{
  BlockHeader_t  Header;       

  raIntegrity_1_0_t GeoIntegrity; 
} GEOIntegrity_1_0_t;

#define SBF_RAINTEGRITY_1_IODF_LENGTH SBF_RAINTEGRITY_1_0_IODF_LENGTH
#define SBF_RAINTEGRITY_1_UDREI_LENGTH SBF_RAINTEGRITY_1_0_UDREI_LENGTH
typedef raIntegrity_1_0_t raIntegrity_1_t;

typedef GEOIntegrity_1_0_t GEOIntegrity_1_t;

/*--GEOFastCorrDegr_1_0_t : -------------------------------------------------*/
/* MT07 : Fast correction degradation factors */

#ifndef SBF_RAFASTCORRDEGR_1_0_AI_LENGTH 
#define SBF_RAFASTCORRDEGR_1_0_AI_LENGTH 51 
#endif

#define SBF_RAFASTCORRDEGR_1_0_TOW_PRECISION 3
typedef struct
{
  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        PRN;          
  uint8_t        IODP;         
  uint8_t        t_lat;        
  uint8_t        ai[SBF_RAFASTCORRDEGR_1_0_AI_LENGTH];
} raFastCorrDegr_1_0_t;

typedef struct
{
  BlockHeader_t  Header;       

  raFastCorrDegr_1_0_t GeoFastCorrDegr;
} GEOFastCorrDegr_1_0_t;

#define SBF_RAFASTCORRDEGR_1_AI_LENGTH SBF_RAFASTCORRDEGR_1_0_AI_LENGTH
typedef raFastCorrDegr_1_0_t raFastCorrDegr_1_t;

typedef GEOFastCorrDegr_1_0_t GEOFastCorrDegr_1_t;

/*--GEONav_1_0_t : ----------------------------------------------------------*/
/* MT09 : SBAS navigation message */

#define SBF_RAEPH_1_0_TOW_PRECISION 3
typedef struct
{
  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        PRN;          
  uint8_t        Reserved;     
  uint16_t       IODN;         /* Issue of data, 8-bit, cycles from 0 to 255 */
  uint16_t       URA;          /* user range accuracy [0,15] */
  uint32_t       t0;           /* time of day of ephemeris */
  SBFDOUBLE      Xg;           
  SBFDOUBLE      Yg;           
  SBFDOUBLE      Zg;           
  SBFDOUBLE      Xgd;          
  SBFDOUBLE      Ygd;          
  SBFDOUBLE      Zgd;          
  SBFDOUBLE      Xgdd;         
  SBFDOUBLE      Ygdd;         
  SBFDOUBLE      Zgdd;         
  float          aGf0;         /* Clock bias  [s] */
  float          aGf1;         /* Clock drift [s/s] */
} raEph_1_0_t;

typedef struct
{
  BlockHeader_t  Header;       

  raEph_1_0_t    Eph;          
} GEONav_1_0_t;

typedef raEph_1_0_t raEph_1_t;

typedef GEONav_1_0_t GEONav_1_t;

/*--GEODegrFactors_1_0_t : --------------------------------------------------*/
/* MT10 : Degradation factors */

typedef struct
{
  SBFDOUBLE      Brrc;         
  SBFDOUBLE      Cltc_lsb;     
  SBFDOUBLE      Cltc_v1;      
  uint32_t       Iltc_v1;      
  SBFDOUBLE      Cltc_v0;      
  uint32_t       Iltc_v0;      
  SBFDOUBLE      Cgeo_lsb;     
  SBFDOUBLE      Cgeo_v;       
  uint32_t       Igeo;         
  float          Cer;          
  SBFDOUBLE      Ciono_step;   
  uint32_t       Iiono;        
  SBFDOUBLE      Ciono_ramp;   
  uint8_t        RSSudre;      
  uint8_t        RSSiono;      
  uint8_t        Reserved2[2]; 
  SBFDOUBLE      Ccovariance;  
} raDF_1_0_t;

#define SBF_RADEGRFACTORS_1_0_TOW_PRECISION 3
typedef struct
{
  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        PRN;          
  uint8_t        Reserved;     
  raDF_1_0_t     DFactors;     
} raDegrFactors_1_0_t;

typedef struct
{
  BlockHeader_t  Header;       

  raDegrFactors_1_0_t GeoDegrFactors;
} GEODegrFactors_1_0_t;

typedef raDF_1_0_t raDF_1_t;

typedef raDegrFactors_1_0_t raDegrFactors_1_t;

typedef GEODegrFactors_1_0_t GEODegrFactors_1_t;

/*--GEONetworkTime_1_0_t : --------------------------------------------------*/
/* MT12 : SBAS Network Time/UTC offset parameters */

typedef struct
{
  float          A_1;          
  SBFDOUBLE      A_0;          
  uint32_t       t_ot;         
  uint8_t        WN_t;         
  int8_t         DEL_t_LS;     
  uint8_t        WN_LSF;       
  uint8_t        DN;           
  int8_t         DEL_t_LSF;    
  uint8_t        UTC_std;      
  uint16_t       GPS_WN;       
  uint32_t       GPS_TOW;      
  uint8_t        GlonassID;    
} NetworkTimeMsg_1_0_t;

#define SBF_RANETWORKTIME_1_0_TOW_PRECISION 3
typedef struct
{
  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        PRN;          
  uint8_t        Reserved;     
  NetworkTimeMsg_1_0_t NWTMessage;   
} raNetworkTime_1_0_t;

typedef struct
{
  BlockHeader_t  Header;       

  raNetworkTime_1_0_t GeoNetworkTime;
} GEONetworkTime_1_0_t;

typedef NetworkTimeMsg_1_0_t NetworkTimeMsg_1_t;

typedef raNetworkTime_1_0_t raNetworkTime_1_t;

typedef GEONetworkTime_1_0_t GEONetworkTime_1_t;

/*--GEOAlm_1_0_t : ----------------------------------------------------------*/
/* MT17 : SBAS satellite almanac */

#define SBF_RAALM_1_0_TOW_PRECISION 3
typedef struct
{
  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        PRN;          
  uint8_t        Reserved0;    
  uint8_t        DataID;       
  uint8_t        Reserved1;    
  uint16_t       Health;       
  uint32_t       t_oa;         
  SBFDOUBLE      Xg;           
  SBFDOUBLE      Yg;           
  SBFDOUBLE      Zg;           
  SBFDOUBLE      Xgd;          
  SBFDOUBLE      Ygd;          
  SBFDOUBLE      Zgd;          
} raAlm_1_0_t;

typedef struct
{
  BlockHeader_t  Header;       

  raAlm_1_0_t    Alm;          
} GEOAlm_1_0_t;

typedef raAlm_1_0_t raAlm_1_t;

typedef GEOAlm_1_0_t GEOAlm_1_t;

/*--GEOIGPMask_1_0_t : ------------------------------------------------------*/
/* MT18 : Ionospheric grid point mask */

#ifndef SBF_RAIGPMASK_1_0_IGPMASK_LENGTH 
#define SBF_RAIGPMASK_1_0_IGPMASK_LENGTH 201 
#endif

#define SBF_RAIGPMASK_1_0_TOW_PRECISION 3
typedef struct
{
  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        PRN;          
  uint8_t        NbrBands;     
  uint8_t        BandNbr;      
  uint8_t        IODI;         
  uint8_t        NbrIGPs;      
  uint8_t        IGPMask[SBF_RAIGPMASK_1_0_IGPMASK_LENGTH];
} raIGPMask_1_0_t;

typedef struct
{
  BlockHeader_t  Header;       

  raIGPMask_1_0_t GeoIGPMask;   
} GEOIGPMask_1_0_t;

#define SBF_RAIGPMASK_1_IGPMASK_LENGTH SBF_RAIGPMASK_1_0_IGPMASK_LENGTH
typedef raIGPMask_1_0_t raIGPMask_1_t;

typedef GEOIGPMask_1_0_t GEOIGPMask_1_t;

/*--GEOLongTermCorr_1_0_t : -------------------------------------------------*/
/* MT24/25 : Long term satellite error corrections */

typedef struct
{
  uint8_t        VelocityCode; 
  uint8_t        PRNMaskNo;    
  uint8_t        IODP;         
  uint8_t        IODE;         
  float          dx;           
  float          dy;           
  float          dz;           
  float          dxRate;       /* set to 0 if velocity code = 0 */
  float          dyRate;       /* set to 0 if velocity code = 0 */
  float          dzRate;       /* set to 0 if velocity code = 0 */
  float          da_f0;        
  float          da_f1;        /* set to 0 if velocity code = 0 */
  uint32_t       t_oe;         /* set to 0 if velocity code = 0 */
} LTCorr_1_0_t;

#ifndef SBF_RALONGTERMCORR_1_0_LTCORR_LENGTH 
#define SBF_RALONGTERMCORR_1_0_LTCORR_LENGTH 4 
#endif

#define SBF_RALONGTERMCORR_1_0_TOW_PRECISION 3
typedef struct
{
  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        PRN;          
  uint8_t        N;            
  uint8_t        SBLength;     
  uint8_t        Reserved[3];  
  LTCorr_1_0_t   LTCorr[SBF_RALONGTERMCORR_1_0_LTCORR_LENGTH];
} raLongTermCorr_1_0_t;

typedef struct
{
  BlockHeader_t  Header;       

  raLongTermCorr_1_0_t GeoLongTermCorr;
} GEOLongTermCorr_1_0_t;

typedef LTCorr_1_0_t LTCorr_1_t;

#define SBF_RALONGTERMCORR_1_LTCORR_LENGTH SBF_RALONGTERMCORR_1_0_LTCORR_LENGTH
typedef raLongTermCorr_1_0_t raLongTermCorr_1_t;

typedef GEOLongTermCorr_1_0_t GEOLongTermCorr_1_t;

/*--GEOIonoDelay_1_0_t : ----------------------------------------------------*/
/* MT26 : Ionospheric delay corrections */

typedef struct
{
  uint8_t        IGPMaskNo;    
  uint8_t        GIVEI;        
  uint8_t        Reserved[2];  
  float          VerticalDelay;
} IDC_1_0_t;

#ifndef SBF_RAIONODELAY_1_0_IDC_LENGTH 
#define SBF_RAIONODELAY_1_0_IDC_LENGTH 15 
#endif

#define SBF_RAIONODELAY_1_0_TOW_PRECISION 3
typedef struct
{
  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        PRN;          
  uint8_t        BandNbr;      
  uint8_t        IODI;         
  uint8_t        N;            
  uint8_t        SBLength;     
  uint8_t        Reserved;     
  IDC_1_0_t      IDC[SBF_RAIONODELAY_1_0_IDC_LENGTH];
} raIonoDelay_1_0_t;

typedef struct
{
  BlockHeader_t  Header;       

  raIonoDelay_1_0_t GeoIonoDelay; 
} GEOIonoDelay_1_0_t;

typedef IDC_1_0_t IDC_1_t;

#define SBF_RAIONODELAY_1_IDC_LENGTH SBF_RAIONODELAY_1_0_IDC_LENGTH
typedef raIonoDelay_1_0_t raIonoDelay_1_t;

typedef GEOIonoDelay_1_0_t GEOIonoDelay_1_t;

/*--GEOServiceLevel_1_0_t : -------------------------------------------------*/
/* MT27 : SBAS Service Message */

#ifndef SBF_SERVICEREGION_1_0__PADDING_LENGTH 
#define SBF_SERVICEREGION_1_0__PADDING_LENGTH 1 
#endif

typedef struct
{
  int8_t         Latitude1;    /* [-90,90] */
  int8_t         Latitude2;    
  int16_t        Longitude1;   /* [-180,180] */
  int16_t        Longitude2;   
  uint8_t        RegionShape;  

  uint8_t        _padding[SBF_SERVICEREGION_1_0__PADDING_LENGTH];
} ServiceRegion_1_0_t;

#ifndef SBF_RASERVICEMSG_1_0_REGIONS_LENGTH 
#define SBF_RASERVICEMSG_1_0_REGIONS_LENGTH 8 
#endif

typedef struct
{
  uint8_t        IODS;         /* Issue of Data [0,7] */
  uint8_t        nrMessages;   /* Number of Messages [1,8] */
  uint8_t        MessageNR;    /* Message nr [1,8] */
  uint8_t        PriorityCode; /* [0,3] */
  uint8_t        dUDREI_In;    /* Delta UDRE Indicator [0,15] */
  uint8_t        dUDREI_Out;   /* [0,15] */
  uint8_t        N;            /* Number of Regions [0,7] */
  uint8_t        SBLength;     /* for SBF sub-block */
  ServiceRegion_1_0_t Regions[SBF_RASERVICEMSG_1_0_REGIONS_LENGTH];
} raServiceMsg_1_0_t;

#define SBF_RASERVICELEVEL_1_0_TOW_PRECISION 3
typedef struct
{
  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        PRN;          
  uint8_t        Reserved;     
  raServiceMsg_1_0_t ServiceMessage;
} raServiceLevel_1_0_t;

typedef struct
{
  BlockHeader_t  Header;       

  raServiceLevel_1_0_t GeoServiceLevel;
} GEOServiceLevel_1_0_t;

#define SBF_SERVICEREGION_1__PADDING_LENGTH SBF_SERVICEREGION_1_0__PADDING_LENGTH
typedef ServiceRegion_1_0_t ServiceRegion_1_t;

#define SBF_RASERVICEMSG_1_REGIONS_LENGTH SBF_RASERVICEMSG_1_0_REGIONS_LENGTH
typedef raServiceMsg_1_0_t raServiceMsg_1_t;

typedef raServiceLevel_1_0_t raServiceLevel_1_t;

typedef GEOServiceLevel_1_0_t GEOServiceLevel_1_t;

/*--GEOClockEphCovMatrix_1_0_t : --------------------------------------------*/
/* MT28 : Clock-Ephemeris Covariance Matrix */

typedef struct
{
  uint8_t        PRNMaskNo;    
  uint8_t        Reserved[2];  
  uint8_t        ScaleExp;     
  uint16_t       E11;          
  uint16_t       E22;          
  uint16_t       E33;          
  uint16_t       E44;          
  int16_t        E12;          
  int16_t        E13;          
  int16_t        E14;          
  int16_t        E23;          
  int16_t        E24;          
  int16_t        E34;          
} CovMatrix_1_0_t;

#ifndef SBF_RACLOCKEPHCOVMATRIX_1_0_COVMATRIX_LENGTH 
#define SBF_RACLOCKEPHCOVMATRIX_1_0_COVMATRIX_LENGTH 2 
#endif

#define SBF_RACLOCKEPHCOVMATRIX_1_0_TOW_PRECISION 3
typedef struct
{
  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        PRN;          
  uint8_t        IODP;         
  uint8_t        N;            
  uint8_t        SBLength;     
  uint8_t        Reserved[2];  
  CovMatrix_1_0_t CovMatrix[SBF_RACLOCKEPHCOVMATRIX_1_0_COVMATRIX_LENGTH];
} raClockEphCovMatrix_1_0_t;

typedef struct
{
  BlockHeader_t  Header;       

  raClockEphCovMatrix_1_0_t GeoClockEphCovMatrix;
} GEOClockEphCovMatrix_1_0_t;

typedef CovMatrix_1_0_t CovMatrix_1_t;

#define SBF_RACLOCKEPHCOVMATRIX_1_COVMATRIX_LENGTH SBF_RACLOCKEPHCOVMATRIX_1_0_COVMATRIX_LENGTH
typedef raClockEphCovMatrix_1_0_t raClockEphCovMatrix_1_t;

typedef GEOClockEphCovMatrix_1_0_t GEOClockEphCovMatrix_1_t;


/*==SBAS L5 Decoded Message Blocks===========================================*/

/*--SBASL5Nav_1_0_t : -------------------------------------------------------*/
/* DFMC SBAS ephemeris and clock data */

#define SBF_SBSEPH_1_0_TOW_PRECISION 3
typedef struct
{
  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        PRN;          
  uint8_t        IODG;         
  uint8_t        providerID;   
  uint8_t        Reserved[3];  
  float          C_uc;         
  float          C_us;         
  float          IDOT;         
  SBFDOUBLE      omega;        
  SBFDOUBLE      OMEGA_0;      
  SBFDOUBLE      M_0;          
  float          a_f1;         
  float          a_f0;         
  SBFDOUBLE      i_0;          
  SBFDOUBLE      e;            
  SBFDOUBLE      A;            
  uint32_t       t_oe;         
} sbsEph_1_0_t;

typedef struct
{
  BlockHeader_t  Header;       

  sbsEph_1_0_t   Eph;          
} SBASL5Nav_1_0_t;

typedef sbsEph_1_0_t sbsEph_1_t;

typedef SBASL5Nav_1_0_t SBASL5Nav_1_t;

/*--SBASL5Alm_1_0_t : -------------------------------------------------------*/
/* DFMC SBAS almanac data */

#ifndef SBF_SBSALM_1_0__PADDING_LENGTH 
#define SBF_SBSALM_1_0__PADDING_LENGTH 1 
#endif

#define SBF_SBSALM_1_0_TOW_PRECISION 3
typedef struct
{
  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        PRN;          
  uint8_t        providerID;   
  uint16_t       Reserved;     
  uint8_t        BroadcastIndicator;
  float          omega;        
  float          OMEGA_0;      
  float          OMEGADOT;     
  float          M_0;          
  float          i_0;          
  float          e;            
  float          A;            
  uint32_t       t_oa;         

  uint8_t        _padding[SBF_SBSALM_1_0__PADDING_LENGTH];
} sbsAlm_1_0_t;

typedef struct
{
  BlockHeader_t  Header;       

  sbsAlm_1_0_t   Alm;          
} SBASL5Alm_1_0_t;

#define SBF_SBSALM_1__PADDING_LENGTH SBF_SBSALM_1_0__PADDING_LENGTH
typedef sbsAlm_1_0_t sbsAlm_1_t;

typedef SBASL5Alm_1_0_t SBASL5Alm_1_t;


/*==Position, Velocity and Time Blocks=======================================*/

/*--PVTCartesian_1_0_t : ----------------------------------------------------*/
/* PVT in Cartesian coordinates */

#define SBF_PVTCARTESIAN_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        NrSV;         
  uint8_t        Error;        
  uint8_t        Mode;         
  uint8_t        System;       
  uint8_t        Info;         
  uint8_t        SBASprn;      
  SBFDOUBLE      X;            
  SBFDOUBLE      Y;            
  SBFDOUBLE      Z;            
  float          Vx;           
  float          Vy;           
  float          Vz;           
  SBFDOUBLE      RxClkBias;    
  float          RxClkDrift;   
  uint16_t       MeanCorrAge;  
  uint16_t       BaseStationID;
  float          Cog;          
} PVTCartesian_1_0_t;

typedef PVTCartesian_1_0_t PVTCartesian_1_t;

/*--PVTGeodetic_1_0_t : -----------------------------------------------------*/
/* PVT in geodetic coordinates */

#define SBF_PVTGEODETIC_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        NrSV;         
  uint8_t        Error;        
  uint8_t        Mode;         
  uint8_t        System;       
  uint8_t        Info;         
  uint8_t        SBASprn;      
  SBFDOUBLE      Lat;          
  SBFDOUBLE      Lon;          
  SBFDOUBLE      Alt;          
  float          Vn;           
  float          Ve;           
  float          Vu;           
  SBFDOUBLE      RxClkBias;    
  float          RxClkDrift;   
  float          GeoidHeight;  
  uint16_t       MeanCorrAge;  
  uint16_t       BaseStationID;
  float          Cog;          
} PVTGeodetic_1_0_t;

typedef PVTGeodetic_1_0_t PVTGeodetic_1_t;

/*--DOP_1_0_t : -------------------------------------------------------------*/
/* Dilution of precision */

#define SBF_DOP_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        NrSV;         
  uint8_t        Error;        
  uint16_t       PDOP;         
  uint16_t       TDOP;         
  uint16_t       HDOP;         
  uint16_t       VDOP;         
  float          HPL;          
  float          VPL;          
} DOP_1_0_t;

typedef DOP_1_0_t DOP_1_t;

/*--PVTResiduals_1_0_t : ----------------------------------------------------*/
/* Measurement residuals */

#ifndef SBF_PVTRESIDUAL_1_0__PADDING_LENGTH 
#define SBF_PVTRESIDUAL_1_0__PADDING_LENGTH 1 
#endif

typedef struct
{
  int16_t        CACodeRes;    
  int16_t        P1CodeRes;    
  int16_t        P2CodeRes;    
  int16_t        DopplerL1Res; 
  int16_t        DopplerL2Res; 
  uint8_t        PRN;          

  uint8_t        _padding[SBF_PVTRESIDUAL_1_0__PADDING_LENGTH];
} PVTResidual_1_0_t;

#ifndef SBF_PVTRESIDUALS_1_0_SATRESIDUAL_LENGTH 
#define SBF_PVTRESIDUALS_1_0_SATRESIDUAL_LENGTH 72 
#endif

#define SBF_PVTRESIDUALS_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        N;            
  uint8_t        SBSize;       
  PVTResidual_1_0_t SatResidual[SBF_PVTRESIDUALS_1_0_SATRESIDUAL_LENGTH];
} PVTResiduals_1_0_t;

#define SBF_PVTRESIDUAL_1__PADDING_LENGTH SBF_PVTRESIDUAL_1_0__PADDING_LENGTH
typedef PVTResidual_1_0_t PVTResidual_1_t;

#define SBF_PVTRESIDUALS_1_SATRESIDUAL_LENGTH SBF_PVTRESIDUALS_1_0_SATRESIDUAL_LENGTH
typedef PVTResiduals_1_0_t PVTResiduals_1_t;

/*--RAIMStatistics_1_0_t : --------------------------------------------------*/
/* Integrity statistics */

typedef struct
{
  uint8_t        PRN;          
  uint8_t        AntennaID;    
  uint8_t        TestResults;  
  uint8_t        Reserved;     
  uint16_t       UnityRangeW;  
  uint16_t       UnityRrateW;  
  uint16_t       RangeMDB;     
  uint16_t       RrateMDB;     
} RAIMSatData_1_0_t;

#ifndef SBF_RAIMSTATISTICS_1_0_RAIMCHANNEL_LENGTH 
#define SBF_RAIMSTATISTICS_1_0_RAIMCHANNEL_LENGTH 72 
#endif

#define SBF_RAIMSTATISTICS_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        Integrity;    
  uint8_t        Reserved1;    
  float          PositionHERL; 
  float          PositionVERL; 
  float          VelocityHERL; 
  float          VelocityVERL; 
  uint16_t       UnityOverallModelP;
  uint16_t       UnityOverallModelV;
  uint8_t        Reserved2[2]; 
  uint8_t        N;            
  uint8_t        SBSize;       
  RAIMSatData_1_0_t RAIMChannel[SBF_RAIMSTATISTICS_1_0_RAIMCHANNEL_LENGTH];
} RAIMStatistics_1_0_t;

typedef RAIMSatData_1_0_t RAIMSatData_1_t;

#define SBF_RAIMSTATISTICS_1_RAIMCHANNEL_LENGTH SBF_RAIMSTATISTICS_1_0_RAIMCHANNEL_LENGTH
typedef RAIMStatistics_1_0_t RAIMStatistics_1_t;

/*--PVTCartesian_2_0_t : ----------------------------------------------------*/
/* Position, velocity, and time in Cartesian coordinates */

#ifndef SBF_PVTCARTESIAN_2_0__PADDING_LENGTH 
#define SBF_PVTCARTESIAN_2_0__PADDING_LENGTH 3 
#endif

#define SBF_PVTCARTESIAN_2_0_TOW_PRECISION 3
#define SBF_PVTCARTESIAN_2_0_X_PRECISION 3
#define SBF_PVTCARTESIAN_2_0_Y_PRECISION 3
#define SBF_PVTCARTESIAN_2_0_Z_PRECISION 3
#define SBF_PVTCARTESIAN_2_0_UNDULATION_PRECISION 3
#define SBF_PVTCARTESIAN_2_0_VX_PRECISION 3
#define SBF_PVTCARTESIAN_2_0_VY_PRECISION 3
#define SBF_PVTCARTESIAN_2_0_VZ_PRECISION 3
#define SBF_PVTCARTESIAN_2_0_COG_PRECISION 2
#define SBF_PVTCARTESIAN_2_0_RXCLKBIAS_PRECISION 6
#define SBF_PVTCARTESIAN_2_0_RXCLKDRIFT_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        Mode;         
  uint8_t        Error;        
  SBFDOUBLE      X;            
  SBFDOUBLE      Y;            
  SBFDOUBLE      Z;            
  float          Undulation;   
  float          Vx;           
  float          Vy;           
  float          Vz;           
  float          COG;          
  SBFDOUBLE      RxClkBias;    
  float          RxClkDrift;   
  uint8_t        TimeSystem;   
  uint8_t        Datum;        
  uint8_t        NrSV;         
  uint8_t        WACorrInfo;   
  uint16_t       ReferenceId;  
  uint16_t       MeanCorrAge;  
  uint32_t       SignalInfo;   
  uint8_t        AlertFlag;    

  uint8_t        _padding[SBF_PVTCARTESIAN_2_0__PADDING_LENGTH];
} PVTCartesian_2_0_t;

/*--PVTCartesian_2_1_t : ----------------------------------------------------*/
/* Position, velocity, and time in Cartesian coordinates */

#define SBF_PVTCARTESIAN_2_1_TOW_PRECISION 3
#define SBF_PVTCARTESIAN_2_1_X_PRECISION 3
#define SBF_PVTCARTESIAN_2_1_Y_PRECISION 3
#define SBF_PVTCARTESIAN_2_1_Z_PRECISION 3
#define SBF_PVTCARTESIAN_2_1_UNDULATION_PRECISION 3
#define SBF_PVTCARTESIAN_2_1_VX_PRECISION 3
#define SBF_PVTCARTESIAN_2_1_VY_PRECISION 3
#define SBF_PVTCARTESIAN_2_1_VZ_PRECISION 3
#define SBF_PVTCARTESIAN_2_1_COG_PRECISION 2
#define SBF_PVTCARTESIAN_2_1_RXCLKBIAS_PRECISION 6
#define SBF_PVTCARTESIAN_2_1_RXCLKDRIFT_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        Mode;         
  uint8_t        Error;        
  SBFDOUBLE      X;            
  SBFDOUBLE      Y;            
  SBFDOUBLE      Z;            
  float          Undulation;   
  float          Vx;           
  float          Vy;           
  float          Vz;           
  float          COG;          
  SBFDOUBLE      RxClkBias;    
  float          RxClkDrift;   
  uint8_t        TimeSystem;   
  uint8_t        Datum;        
  uint8_t        NrSV;         
  uint8_t        WACorrInfo;   
  uint16_t       ReferenceId;  
  uint16_t       MeanCorrAge;  
  uint32_t       SignalInfo;   
  uint8_t        AlertFlag;    
  uint8_t        NrBases;      
  uint16_t       PPPInfo;      
} PVTCartesian_2_1_t;

/*--PVTCartesian_2_2_t : ----------------------------------------------------*/
/* Position, velocity, and time in Cartesian coordinates */

#ifndef SBF_PVTCARTESIAN_2_2__PADDING_LENGTH 
#define SBF_PVTCARTESIAN_2_2__PADDING_LENGTH 1 
#endif

#define SBF_PVTCARTESIAN_2_2_TOW_PRECISION 3
#define SBF_PVTCARTESIAN_2_2_X_PRECISION 3
#define SBF_PVTCARTESIAN_2_2_Y_PRECISION 3
#define SBF_PVTCARTESIAN_2_2_Z_PRECISION 3
#define SBF_PVTCARTESIAN_2_2_UNDULATION_PRECISION 3
#define SBF_PVTCARTESIAN_2_2_VX_PRECISION 3
#define SBF_PVTCARTESIAN_2_2_VY_PRECISION 3
#define SBF_PVTCARTESIAN_2_2_VZ_PRECISION 3
#define SBF_PVTCARTESIAN_2_2_COG_PRECISION 2
#define SBF_PVTCARTESIAN_2_2_RXCLKBIAS_PRECISION 6
#define SBF_PVTCARTESIAN_2_2_RXCLKDRIFT_PRECISION 3
#define SBF_PVTCARTESIAN_2_2_HACCURACY_PRECISION 2
#define SBF_PVTCARTESIAN_2_2_VACCURACY_PRECISION 2
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        Mode;         
  uint8_t        Error;        
  SBFDOUBLE      X;            
  SBFDOUBLE      Y;            
  SBFDOUBLE      Z;            
  float          Undulation;   
  float          Vx;           
  float          Vy;           
  float          Vz;           
  float          COG;          
  SBFDOUBLE      RxClkBias;    
  float          RxClkDrift;   
  uint8_t        TimeSystem;   
  uint8_t        Datum;        
  uint8_t        NrSV;         
  uint8_t        WACorrInfo;   
  uint16_t       ReferenceId;  
  uint16_t       MeanCorrAge;  
  uint32_t       SignalInfo;   
  uint8_t        AlertFlag;    
  uint8_t        NrBases;      
  uint16_t       PPPInfo;      
  uint16_t       Latency;      
  uint16_t       HAccuracy;    
  uint16_t       VAccuracy;    
  uint8_t        Misc;         

  uint8_t        _padding[SBF_PVTCARTESIAN_2_2__PADDING_LENGTH];
} PVTCartesian_2_2_t;

#define SBF_PVTCARTESIAN_2__PADDING_LENGTH SBF_PVTCARTESIAN_2_2__PADDING_LENGTH
typedef PVTCartesian_2_2_t PVTCartesian_2_t;

/*--PVTGeodetic_2_0_t : -----------------------------------------------------*/
/* Position, velocity, and time in geodetic coordinates */

#ifndef SBF_PVTGEODETIC_2_0__PADDING_LENGTH 
#define SBF_PVTGEODETIC_2_0__PADDING_LENGTH 3 
#endif

#define SBF_PVTGEODETIC_2_0_TOW_PRECISION 3
#define SBF_PVTGEODETIC_2_0_LAT_PRECISION 12
#define SBF_PVTGEODETIC_2_0_LON_PRECISION 12
#define SBF_PVTGEODETIC_2_0_ALT_PRECISION 3
#define SBF_PVTGEODETIC_2_0_UNDULATION_PRECISION 3
#define SBF_PVTGEODETIC_2_0_VN_PRECISION 3
#define SBF_PVTGEODETIC_2_0_VE_PRECISION 3
#define SBF_PVTGEODETIC_2_0_VU_PRECISION 3
#define SBF_PVTGEODETIC_2_0_COG_PRECISION 2
#define SBF_PVTGEODETIC_2_0_RXCLKBIAS_PRECISION 6
#define SBF_PVTGEODETIC_2_0_RXCLKDRIFT_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        Mode;         
  uint8_t        Error;        
  SBFDOUBLE      Lat;          
  SBFDOUBLE      Lon;          
  SBFDOUBLE      Alt;          
  float          Undulation;   
  float          Vn;           
  float          Ve;           
  float          Vu;           
  float          COG;          
  SBFDOUBLE      RxClkBias;    
  float          RxClkDrift;   
  uint8_t        TimeSystem;   
  uint8_t        Datum;        
  uint8_t        NrSV;         
  uint8_t        WACorrInfo;   
  uint16_t       ReferenceId;  
  uint16_t       MeanCorrAge;  
  uint32_t       SignalInfo;   
  uint8_t        AlertFlag;    

  uint8_t        _padding[SBF_PVTGEODETIC_2_0__PADDING_LENGTH];
} PVTGeodetic_2_0_t;

/*--PVTGeodetic_2_1_t : -----------------------------------------------------*/
/* Position, velocity, and time in geodetic coordinates */

#define SBF_PVTGEODETIC_2_1_TOW_PRECISION 3
#define SBF_PVTGEODETIC_2_1_LAT_PRECISION 12
#define SBF_PVTGEODETIC_2_1_LON_PRECISION 12
#define SBF_PVTGEODETIC_2_1_ALT_PRECISION 3
#define SBF_PVTGEODETIC_2_1_UNDULATION_PRECISION 3
#define SBF_PVTGEODETIC_2_1_VN_PRECISION 3
#define SBF_PVTGEODETIC_2_1_VE_PRECISION 3
#define SBF_PVTGEODETIC_2_1_VU_PRECISION 3
#define SBF_PVTGEODETIC_2_1_COG_PRECISION 2
#define SBF_PVTGEODETIC_2_1_RXCLKBIAS_PRECISION 6
#define SBF_PVTGEODETIC_2_1_RXCLKDRIFT_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        Mode;         
  uint8_t        Error;        
  SBFDOUBLE      Lat;          
  SBFDOUBLE      Lon;          
  SBFDOUBLE      Alt;          
  float          Undulation;   
  float          Vn;           
  float          Ve;           
  float          Vu;           
  float          COG;          
  SBFDOUBLE      RxClkBias;    
  float          RxClkDrift;   
  uint8_t        TimeSystem;   
  uint8_t        Datum;        
  uint8_t        NrSV;         
  uint8_t        WACorrInfo;   
  uint16_t       ReferenceId;  
  uint16_t       MeanCorrAge;  
  uint32_t       SignalInfo;   
  uint8_t        AlertFlag;    
  uint8_t        NrBases;      
  uint16_t       PPPInfo;      
} PVTGeodetic_2_1_t;

/*--PVTGeodetic_2_2_t : -----------------------------------------------------*/
/* Position, velocity, and time in geodetic coordinates */

#ifndef SBF_PVTGEODETIC_2_2__PADDING_LENGTH 
#define SBF_PVTGEODETIC_2_2__PADDING_LENGTH 1 
#endif

#define SBF_PVTGEODETIC_2_2_TOW_PRECISION 3
#define SBF_PVTGEODETIC_2_2_LAT_PRECISION 12
#define SBF_PVTGEODETIC_2_2_LON_PRECISION 12
#define SBF_PVTGEODETIC_2_2_ALT_PRECISION 3
#define SBF_PVTGEODETIC_2_2_UNDULATION_PRECISION 3
#define SBF_PVTGEODETIC_2_2_VN_PRECISION 3
#define SBF_PVTGEODETIC_2_2_VE_PRECISION 3
#define SBF_PVTGEODETIC_2_2_VU_PRECISION 3
#define SBF_PVTGEODETIC_2_2_COG_PRECISION 2
#define SBF_PVTGEODETIC_2_2_RXCLKBIAS_PRECISION 6
#define SBF_PVTGEODETIC_2_2_RXCLKDRIFT_PRECISION 3
#define SBF_PVTGEODETIC_2_2_HACCURACY_PRECISION 2
#define SBF_PVTGEODETIC_2_2_VACCURACY_PRECISION 2
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        Mode;         
  uint8_t        Error;        
  SBFDOUBLE      Lat;          
  SBFDOUBLE      Lon;          
  SBFDOUBLE      Alt;          
  float          Undulation;   
  float          Vn;           
  float          Ve;           
  float          Vu;           
  float          COG;          
  SBFDOUBLE      RxClkBias;    
  float          RxClkDrift;   
  uint8_t        TimeSystem;   
  uint8_t        Datum;        
  uint8_t        NrSV;         
  uint8_t        WACorrInfo;   
  uint16_t       ReferenceId;  
  uint16_t       MeanCorrAge;  
  uint32_t       SignalInfo;   
  uint8_t        AlertFlag;    
  uint8_t        NrBases;      
  uint16_t       PPPInfo;      
  uint16_t       Latency;      
  uint16_t       HAccuracy;    
  uint16_t       VAccuracy;    
  uint8_t        Misc;         

  uint8_t        _padding[SBF_PVTGEODETIC_2_2__PADDING_LENGTH];
} PVTGeodetic_2_2_t;

#define SBF_PVTGEODETIC_2__PADDING_LENGTH SBF_PVTGEODETIC_2_2__PADDING_LENGTH
typedef PVTGeodetic_2_2_t PVTGeodetic_2_t;

/*--PosCovCartesian_1_0_t : -------------------------------------------------*/
/* Position covariance matrix (X,Y, Z) */

#define SBF_POSCOVCARTESIAN_1_0_TOW_PRECISION 3
#define SBF_POSCOVCARTESIAN_1_0_COV_XX_PRECISION 8
#define SBF_POSCOVCARTESIAN_1_0_COV_YY_PRECISION 8
#define SBF_POSCOVCARTESIAN_1_0_COV_ZZ_PRECISION 8
#define SBF_POSCOVCARTESIAN_1_0_COV_TT_PRECISION 8
#define SBF_POSCOVCARTESIAN_1_0_COV_XY_PRECISION 8
#define SBF_POSCOVCARTESIAN_1_0_COV_XZ_PRECISION 8
#define SBF_POSCOVCARTESIAN_1_0_COV_XT_PRECISION 8
#define SBF_POSCOVCARTESIAN_1_0_COV_YZ_PRECISION 8
#define SBF_POSCOVCARTESIAN_1_0_COV_YT_PRECISION 8
#define SBF_POSCOVCARTESIAN_1_0_COV_ZT_PRECISION 8
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        Mode;         
  uint8_t        Error;        
  float          Cov_xx;       
  float          Cov_yy;       
  float          Cov_zz;       
  float          Cov_tt;       
  float          Cov_xy;       
  float          Cov_xz;       
  float          Cov_xt;       
  float          Cov_yz;       
  float          Cov_yt;       
  float          Cov_zt;       
} PosCovCartesian_1_0_t;

typedef PosCovCartesian_1_0_t PosCovCartesian_1_t;

/*--PosCovGeodetic_1_0_t : --------------------------------------------------*/
/* Position covariance matrix (Lat, Lon, Alt) */

#define SBF_POSCOVGEODETIC_1_0_TOW_PRECISION 3
#define SBF_POSCOVGEODETIC_1_0_COV_LATLAT_PRECISION 8
#define SBF_POSCOVGEODETIC_1_0_COV_LONLON_PRECISION 8
#define SBF_POSCOVGEODETIC_1_0_COV_ALTALT_PRECISION 8
#define SBF_POSCOVGEODETIC_1_0_COV_TT_PRECISION 8
#define SBF_POSCOVGEODETIC_1_0_COV_LATLON_PRECISION 8
#define SBF_POSCOVGEODETIC_1_0_COV_LATALT_PRECISION 8
#define SBF_POSCOVGEODETIC_1_0_COV_LATT_PRECISION 8
#define SBF_POSCOVGEODETIC_1_0_COV_LONALT_PRECISION 8
#define SBF_POSCOVGEODETIC_1_0_COV_LONT_PRECISION 8
#define SBF_POSCOVGEODETIC_1_0_COV_ALTT_PRECISION 8
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        Mode;         
  uint8_t        Error;        
  float          Cov_LatLat;   
  float          Cov_LonLon;   
  float          Cov_AltAlt;   
  float          Cov_tt;       
  float          Cov_LatLon;   
  float          Cov_LatAlt;   
  float          Cov_Latt;     
  float          Cov_LonAlt;   
  float          Cov_Lont;     
  float          Cov_Altt;     
} PosCovGeodetic_1_0_t;

typedef PosCovGeodetic_1_0_t PosCovGeodetic_1_t;

/*--VelCovCartesian_1_0_t : -------------------------------------------------*/
/* Velocity covariance matrix (X, Y, Z) */

#define SBF_VELCOVCARTESIAN_1_0_TOW_PRECISION 3
#define SBF_VELCOVCARTESIAN_1_0_COV_VXVX_PRECISION 3
#define SBF_VELCOVCARTESIAN_1_0_COV_VYVY_PRECISION 3
#define SBF_VELCOVCARTESIAN_1_0_COV_VZVZ_PRECISION 3
#define SBF_VELCOVCARTESIAN_1_0_COV_DTDT_PRECISION 3
#define SBF_VELCOVCARTESIAN_1_0_COV_VXVY_PRECISION 3
#define SBF_VELCOVCARTESIAN_1_0_COV_VXVZ_PRECISION 3
#define SBF_VELCOVCARTESIAN_1_0_COV_VXDT_PRECISION 3
#define SBF_VELCOVCARTESIAN_1_0_COV_VYVZ_PRECISION 3
#define SBF_VELCOVCARTESIAN_1_0_COV_VYDT_PRECISION 3
#define SBF_VELCOVCARTESIAN_1_0_COV_VZDT_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        Mode;         
  uint8_t        Error;        
  float          Cov_VxVx;     
  float          Cov_VyVy;     
  float          Cov_VzVz;     
  float          Cov_DtDt;     
  float          Cov_VxVy;     
  float          Cov_VxVz;     
  float          Cov_VxDt;     
  float          Cov_VyVz;     
  float          Cov_VyDt;     
  float          Cov_VzDt;     
} VelCovCartesian_1_0_t;

typedef VelCovCartesian_1_0_t VelCovCartesian_1_t;

/*--VelCovGeodetic_1_0_t : --------------------------------------------------*/
/* Velocity covariance matrix (North, East, Up) */

#define SBF_VELCOVGEODETIC_1_0_TOW_PRECISION 3
#define SBF_VELCOVGEODETIC_1_0_COV_VNVN_PRECISION 3
#define SBF_VELCOVGEODETIC_1_0_COV_VEVE_PRECISION 3
#define SBF_VELCOVGEODETIC_1_0_COV_VUVU_PRECISION 3
#define SBF_VELCOVGEODETIC_1_0_COV_DTDT_PRECISION 3
#define SBF_VELCOVGEODETIC_1_0_COV_VNVE_PRECISION 3
#define SBF_VELCOVGEODETIC_1_0_COV_VNVU_PRECISION 3
#define SBF_VELCOVGEODETIC_1_0_COV_VNDT_PRECISION 3
#define SBF_VELCOVGEODETIC_1_0_COV_VEVU_PRECISION 3
#define SBF_VELCOVGEODETIC_1_0_COV_VEDT_PRECISION 3
#define SBF_VELCOVGEODETIC_1_0_COV_VUDT_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        Mode;         
  uint8_t        Error;        
  float          Cov_VnVn;     
  float          Cov_VeVe;     
  float          Cov_VuVu;     
  float          Cov_DtDt;     
  float          Cov_VnVe;     
  float          Cov_VnVu;     
  float          Cov_VnDt;     
  float          Cov_VeVu;     
  float          Cov_VeDt;     
  float          Cov_VuDt;     
} VelCovGeodetic_1_0_t;

typedef VelCovGeodetic_1_0_t VelCovGeodetic_1_t;

/*--DOP_2_0_t : -------------------------------------------------------------*/
/* Dilution of precision */

#define SBF_DOP_2_0_TOW_PRECISION 3
#define SBF_DOP_2_0_HPL_PRECISION 3
#define SBF_DOP_2_0_VPL_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        NrSV;         
  uint8_t        Reserved;     
  uint16_t       PDOP;         
  uint16_t       TDOP;         
  uint16_t       HDOP;         
  uint16_t       VDOP;         
  float          HPL;          
  float          VPL;          
} DOP_2_0_t;

typedef DOP_2_0_t DOP_2_t;

/*--PosCart_1_0_t : ---------------------------------------------------------*/
/* Position, variance and baseline in Cartesian coordinates */

#define SBF_POSCART_1_0_TOW_PRECISION 3
#define SBF_POSCART_1_0_X_PRECISION 3
#define SBF_POSCART_1_0_Y_PRECISION 3
#define SBF_POSCART_1_0_Z_PRECISION 3
#define SBF_POSCART_1_0_BASE2ROVERX_PRECISION 3
#define SBF_POSCART_1_0_BASE2ROVERY_PRECISION 3
#define SBF_POSCART_1_0_BASE2ROVERZ_PRECISION 3
#define SBF_POSCART_1_0_COV_XX_PRECISION 8
#define SBF_POSCART_1_0_COV_YY_PRECISION 8
#define SBF_POSCART_1_0_COV_ZZ_PRECISION 8
#define SBF_POSCART_1_0_COV_XY_PRECISION 8
#define SBF_POSCART_1_0_COV_XZ_PRECISION 8
#define SBF_POSCART_1_0_COV_YZ_PRECISION 8
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        Mode;         
  uint8_t        Error;        
  SBFDOUBLE      X;            
  SBFDOUBLE      Y;            
  SBFDOUBLE      Z;            
  SBFDOUBLE      Base2RoverX;  
  SBFDOUBLE      Base2RoverY;  
  SBFDOUBLE      Base2RoverZ;  
  float          Cov_xx;       
  float          Cov_yy;       
  float          Cov_zz;       
  float          Cov_xy;       
  float          Cov_xz;       
  float          Cov_yz;       
  uint16_t       PDOP;         
  uint16_t       HDOP;         
  uint16_t       VDOP;         
  uint8_t        Misc;         
  uint8_t        Reserved;     
  uint8_t        AlertFlag;    
  uint8_t        Datum;        
  uint8_t        NrSV;         
  uint8_t        WACorrInfo;   
  uint16_t       ReferenceId;  
  uint16_t       MeanCorrAge;  
  uint32_t       SignalInfo;   
} PosCart_1_0_t;

typedef PosCart_1_0_t PosCart_1_t;

/*--PosLocal_1_0_t : --------------------------------------------------------*/
/* Position in a local datum */

#ifndef SBF_POSLOCAL_1_0__PADDING_LENGTH 
#define SBF_POSLOCAL_1_0__PADDING_LENGTH 3 
#endif

#define SBF_POSLOCAL_1_0_TOW_PRECISION 3
#define SBF_POSLOCAL_1_0_LAT_PRECISION 12
#define SBF_POSLOCAL_1_0_LON_PRECISION 12
#define SBF_POSLOCAL_1_0_ALT_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        Mode;         
  uint8_t        Error;        
  SBFDOUBLE      Lat;          
  SBFDOUBLE      Lon;          
  SBFDOUBLE      Alt;          
  uint8_t        Datum;        

  uint8_t        _padding[SBF_POSLOCAL_1_0__PADDING_LENGTH];
} PosLocal_1_0_t;

#define SBF_POSLOCAL_1__PADDING_LENGTH SBF_POSLOCAL_1_0__PADDING_LENGTH
typedef PosLocal_1_0_t PosLocal_1_t;

/*--PosProjected_1_0_t : ----------------------------------------------------*/
/* Plane grid coordinates */

#ifndef SBF_POSPROJECTED_1_0__PADDING_LENGTH 
#define SBF_POSPROJECTED_1_0__PADDING_LENGTH 3 
#endif

#define SBF_POSPROJECTED_1_0_TOW_PRECISION 3
#define SBF_POSPROJECTED_1_0_NORTHING_PRECISION 3
#define SBF_POSPROJECTED_1_0_EASTING_PRECISION 3
#define SBF_POSPROJECTED_1_0_ALT_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        Mode;         
  uint8_t        Error;        
  SBFDOUBLE      Northing;     
  SBFDOUBLE      Easting;      
  SBFDOUBLE      Alt;          
  uint8_t        Datum;        

  uint8_t        _padding[SBF_POSPROJECTED_1_0__PADDING_LENGTH];
} PosProjected_1_0_t;

#define SBF_POSPROJECTED_1__PADDING_LENGTH SBF_POSPROJECTED_1_0__PADDING_LENGTH
typedef PosProjected_1_0_t PosProjected_1_t;

/*--PVTSatCartesian_1_0_t : -------------------------------------------------*/
/* Satellite positions */

#define SBF_SATPOS_1_0_X_PRECISION 3
#define SBF_SATPOS_1_0_Y_PRECISION 3
#define SBF_SATPOS_1_0_Z_PRECISION 3
#define SBF_SATPOS_1_0_VX_PRECISION 3
#define SBF_SATPOS_1_0_VY_PRECISION 3
#define SBF_SATPOS_1_0_VZ_PRECISION 3
typedef struct
{
  uint8_t        SVID;         
  uint8_t        FreqNr;       
  uint16_t       IODE;         
  SBFDOUBLE      x;            
  SBFDOUBLE      y;            
  SBFDOUBLE      z;            
  float          Vx;           
  float          Vy;           
  float          Vz;           
} SatPos_1_0_t;

#ifndef SBF_PVTSATCARTESIAN_1_0_SATPOS_LENGTH 
#define SBF_PVTSATCARTESIAN_1_0_SATPOS_LENGTH 72 
#endif

#define SBF_PVTSATCARTESIAN_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        N;            
  uint8_t        SBSize;       
  SatPos_1_0_t   SatPos[SBF_PVTSATCARTESIAN_1_0_SATPOS_LENGTH];
} PVTSatCartesian_1_0_t;

/*--PVTSatCartesian_1_1_t : -------------------------------------------------*/
/* Satellite positions */

#ifndef SBF_SATPOS_1_1__PADDING_LENGTH 
#define SBF_SATPOS_1_1__PADDING_LENGTH 1 
#endif

#define SBF_SATPOS_1_1_X_PRECISION 3
#define SBF_SATPOS_1_1_Y_PRECISION 3
#define SBF_SATPOS_1_1_Z_PRECISION 3
#define SBF_SATPOS_1_1_VX_PRECISION 3
#define SBF_SATPOS_1_1_VY_PRECISION 3
#define SBF_SATPOS_1_1_VZ_PRECISION 3
typedef struct
{
  uint8_t        SVID;         
  uint8_t        FreqNr;       
  uint16_t       IODE;         
  SBFDOUBLE      x;            
  SBFDOUBLE      y;            
  SBFDOUBLE      z;            
  float          Vx;           
  float          Vy;           
  float          Vz;           
  int16_t        IonoMSB;      
  int16_t        TropoMSB;     
  uint8_t        IonoLSB;      
  uint8_t        TropoLSB;     
  uint8_t        IonoModel;    

  uint8_t        _padding[SBF_SATPOS_1_1__PADDING_LENGTH];
} SatPos_1_1_t;

#ifndef SBF_PVTSATCARTESIAN_1_1_SATPOS_LENGTH 
#define SBF_PVTSATCARTESIAN_1_1_SATPOS_LENGTH 72 
#endif

#define SBF_PVTSATCARTESIAN_1_1_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        N;            
  uint8_t        SBSize;       
  SatPos_1_1_t   SatPos[SBF_PVTSATCARTESIAN_1_1_SATPOS_LENGTH];
} PVTSatCartesian_1_1_t;

#define SBF_SATPOS_1__PADDING_LENGTH SBF_SATPOS_1_1__PADDING_LENGTH
typedef SatPos_1_1_t SatPos_1_t;

#define SBF_PVTSATCARTESIAN_1_SATPOS_LENGTH SBF_PVTSATCARTESIAN_1_1_SATPOS_LENGTH
typedef PVTSatCartesian_1_1_t PVTSatCartesian_1_t;

/*--PVTResiduals_2_0_t : ----------------------------------------------------*/
/* Measurement residuals */

#define SBF_RESIDUALINFOCODE_2_0_RESIDUAL_PRECISION 3
typedef struct
{
  float          Residual;     
  uint16_t       W;            
  uint16_t       MDB;          
} ResidualInfoCode_2_0_t;

#define SBF_RESIDUALINFOPHASE_2_0_RESIDUAL_PRECISION 3
typedef struct
{
  float          Residual;     
  uint16_t       W;            
  uint16_t       MDB;          
} ResidualInfoPhase_2_0_t;

#define SBF_RESIDUALINFODOPPLER_2_0_RESIDUAL_PRECISION 3
typedef struct
{
  float          Residual;     
  uint16_t       W;            
  uint16_t       MDB;          
} ResidualInfoDoppler_2_0_t;

typedef struct
{
  uint8_t        SVID;         
  uint8_t        FreqNr;       
  uint8_t        Type;         
  uint8_t        RefSVID;      
  uint8_t        RefFreqNr;    
  uint8_t        MeasInfo;     
  uint16_t       IODE;         
  uint16_t       CorrAge;      
  uint16_t       Reserved;     /* Reserved for future use */
} SatSignalInfo_2_0_t;

#ifndef SBF_PVTRESIDUALS_2_0_DATA_LENGTH 
#define SBF_PVTRESIDUALS_2_0_DATA_LENGTH -1 
#endif

#define SBF_PVTRESIDUALS_2_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        N;            
  uint8_t        SB1Size;      
  uint8_t        SB2Size;      
  uint8_t        Reserved[3];  
  uint8_t        Data[SBF_PVTRESIDUALS_2_0_DATA_LENGTH];
} PVTResiduals_2_0_t;

/*--PVTResiduals_2_1_t : ----------------------------------------------------*/
/* Measurement residuals */

#define SBF_RESIDUALINFOCODE_2_1_RESIDUAL_PRECISION 3
typedef struct
{
  float          Residual;     
  uint16_t       W;            
  uint16_t       MDB;          
} ResidualInfoCode_2_1_t;

#define SBF_RESIDUALINFOPHASE_2_1_RESIDUAL_PRECISION 3
typedef struct
{
  float          Residual;     
  uint16_t       W;            
  uint16_t       MDB;          
} ResidualInfoPhase_2_1_t;

#define SBF_RESIDUALINFODOPPLER_2_1_RESIDUAL_PRECISION 3
typedef struct
{
  float          Residual;     
  uint16_t       W;            
  uint16_t       MDB;          
} ResidualInfoDoppler_2_1_t;

typedef struct
{
  uint8_t        SVID;         
  uint8_t        FreqNr;       
  uint8_t        Type;         
  uint8_t        RefSVID;      
  uint8_t        RefFreqNr;    
  uint8_t        MeasInfo;     
  uint16_t       IODE;         
  uint16_t       CorrAge;      
  uint16_t       ReferenceID;  
} SatSignalInfo_2_1_t;

#ifndef SBF_PVTRESIDUALS_2_1_DATA_LENGTH 
#define SBF_PVTRESIDUALS_2_1_DATA_LENGTH -1 
#endif

#define SBF_PVTRESIDUALS_2_1_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        N;            
  uint8_t        SB1Size;      
  uint8_t        SB2Size;      
  uint8_t        Reserved[3];  
  uint8_t        Data[SBF_PVTRESIDUALS_2_1_DATA_LENGTH];
} PVTResiduals_2_1_t;

typedef ResidualInfoCode_2_1_t ResidualInfoCode_2_t;

typedef ResidualInfoPhase_2_1_t ResidualInfoPhase_2_t;

typedef ResidualInfoDoppler_2_1_t ResidualInfoDoppler_2_t;

typedef SatSignalInfo_2_1_t SatSignalInfo_2_t;

#define SBF_PVTRESIDUALS_2_DATA_LENGTH SBF_PVTRESIDUALS_2_1_DATA_LENGTH
typedef PVTResiduals_2_1_t PVTResiduals_2_t;

/*--RAIMStatistics_2_0_t : --------------------------------------------------*/
/* Integrity statistics */

#ifndef SBF_RAIMSTATISTICS_2_0__PADDING_LENGTH 
#define SBF_RAIMSTATISTICS_2_0__PADDING_LENGTH 2 
#endif

#define SBF_RAIMSTATISTICS_2_0_TOW_PRECISION 3
#define SBF_RAIMSTATISTICS_2_0_POSITIONHERL_PRECISION 3
#define SBF_RAIMSTATISTICS_2_0_POSITIONVERL_PRECISION 3
#define SBF_RAIMSTATISTICS_2_0_VELOCITYHERL_PRECISION 3
#define SBF_RAIMSTATISTICS_2_0_VELOCITYVERL_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        Integrity;    
  uint8_t        Reserved1;    
  float          PositionHERL; 
  float          PositionVERL; 
  float          VelocityHERL; 
  float          VelocityVERL; 
  uint16_t       UnityOverallModel;

  uint8_t        _padding[SBF_RAIMSTATISTICS_2_0__PADDING_LENGTH];
} RAIMStatistics_2_0_t;

#define SBF_RAIMSTATISTICS_2__PADDING_LENGTH SBF_RAIMSTATISTICS_2_0__PADDING_LENGTH
typedef RAIMStatistics_2_0_t RAIMStatistics_2_t;

/*--GEOCorrections_1_0_t : --------------------------------------------------*/
/* Orbit, Clock and pseudoranges SBAS corrections */

#define SBF_SATCORR_1_0_CORRAGEFC_PRECISION 3
#define SBF_SATCORR_1_0_CORRAGELT_PRECISION 3
#define SBF_SATCORR_1_0_IONOPPLAT_PRECISION 3
#define SBF_SATCORR_1_0_IONOPPLON_PRECISION 3
#define SBF_SATCORR_1_0_SLANTIONO_PRECISION 3
#define SBF_SATCORR_1_0_CORRAGEIONO_PRECISION 3
#define SBF_SATCORR_1_0_VARFLT_PRECISION 3
#define SBF_SATCORR_1_0_VARUIRE_PRECISION 3
#define SBF_SATCORR_1_0_VARAIR_PRECISION 3
#define SBF_SATCORR_1_0_VARTROPO_PRECISION 3
typedef struct
{
  uint8_t        prn;          
  uint8_t        IODE;         
  uint8_t        Reserved[2];  
  float          PRC;          
  float          CorrAgeFC;    
  float          deltaX;       
  float          deltaY;       
  float          deltaZ;       
  float          deltaClock;   
  float          CorrAgeLT;    
  float          IonoPPlat;    
  float          IonoPPlon;    
  float          SlantIono;    
  float          CorrAgeIono;  
  float          VarFLT;       
  float          VarUIRE;      
  float          VarAir;       
  float          VarTropo;     
} SatCorr_1_0_t;

#ifndef SBF_GEOCORRECTIONS_1_0_GEOCORRCHANNEL_LENGTH 
#define SBF_GEOCORRECTIONS_1_0_GEOCORRCHANNEL_LENGTH 72 
#endif

#define SBF_GEOCORRECTIONS_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        N;            
  uint8_t        SBLength;     
  SatCorr_1_0_t  GeoCorrChannel[SBF_GEOCORRECTIONS_1_0_GEOCORRCHANNEL_LENGTH];
} GEOCorrections_1_0_t;

typedef SatCorr_1_0_t SatCorr_1_t;

#define SBF_GEOCORRECTIONS_1_GEOCORRCHANNEL_LENGTH SBF_GEOCORRECTIONS_1_0_GEOCORRCHANNEL_LENGTH
typedef GEOCorrections_1_0_t GEOCorrections_1_t;

/*--BaseVectorCart_1_0_t : --------------------------------------------------*/
/* XYZ relative position and velocity with respect to base(s) */

#define SBF_VECTORINFOCART_1_0_DELTAX_PRECISION 3
#define SBF_VECTORINFOCART_1_0_DELTAY_PRECISION 3
#define SBF_VECTORINFOCART_1_0_DELTAZ_PRECISION 3
#define SBF_VECTORINFOCART_1_0_DELTAXVELOCITY_PRECISION 3
#define SBF_VECTORINFOCART_1_0_DELTAYVELOCITY_PRECISION 3
#define SBF_VECTORINFOCART_1_0_DELTAZVELOCITY_PRECISION 3
#define SBF_VECTORINFOCART_1_0_AZIMUTH_PRECISION 2
#define SBF_VECTORINFOCART_1_0_ELEVATION_PRECISION 2
typedef struct
{
  uint8_t        NrSV;         
  uint8_t        Error;        
  uint8_t        Mode;         
  uint8_t        Misc;         
  SBFDOUBLE      DeltaX;       
  SBFDOUBLE      DeltaY;       
  SBFDOUBLE      DeltaZ;       
  float          DeltaXVelocity;
  float          DeltaYVelocity;
  float          DeltaZVelocity;
  uint16_t       Azimuth;      
  int16_t        Elevation;    
  uint16_t       ReferenceID;  
  uint16_t       CorrAge;      
  uint32_t       SignalInfo;   
} VectorInfoCart_1_0_t;

#ifndef SBF_BASEVECTORCART_1_0_VECTORINFO_LENGTH 
#define SBF_BASEVECTORCART_1_0_VECTORINFO_LENGTH 30 
#endif

#define SBF_BASEVECTORCART_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        N;            
  uint8_t        SBLength;     
  VectorInfoCart_1_0_t VectorInfo[SBF_BASEVECTORCART_1_0_VECTORINFO_LENGTH];
} BaseVectorCart_1_0_t;

typedef VectorInfoCart_1_0_t VectorInfoCart_1_t;

#define SBF_BASEVECTORCART_1_VECTORINFO_LENGTH SBF_BASEVECTORCART_1_0_VECTORINFO_LENGTH
typedef BaseVectorCart_1_0_t BaseVectorCart_1_t;

/*--BaseVectorGeod_1_0_t : --------------------------------------------------*/
/* ENU relative position and velocity with respect to base(s) */

#define SBF_VECTORINFOGEOD_1_0_DELTAEAST_PRECISION 3
#define SBF_VECTORINFOGEOD_1_0_DELTANORTH_PRECISION 3
#define SBF_VECTORINFOGEOD_1_0_DELTAUP_PRECISION 3
#define SBF_VECTORINFOGEOD_1_0_DELTAEASTVELOCITY_PRECISION 3
#define SBF_VECTORINFOGEOD_1_0_DELTANORTHVELOCITY_PRECISION 3
#define SBF_VECTORINFOGEOD_1_0_DELTAUPVELOCITY_PRECISION 3
#define SBF_VECTORINFOGEOD_1_0_AZIMUTH_PRECISION 2
#define SBF_VECTORINFOGEOD_1_0_ELEVATION_PRECISION 2
typedef struct
{
  uint8_t        NrSV;         
  uint8_t        Error;        
  uint8_t        Mode;         
  uint8_t        Misc;         
  SBFDOUBLE      DeltaEast;    
  SBFDOUBLE      DeltaNorth;   
  SBFDOUBLE      DeltaUp;      
  float          DeltaEastVelocity;
  float          DeltaNorthVelocity;
  float          DeltaUpVelocity;
  uint16_t       Azimuth;      
  int16_t        Elevation;    
  uint16_t       ReferenceID;  
  uint16_t       CorrAge;      
  uint32_t       SignalInfo;   
} VectorInfoGeod_1_0_t;

#ifndef SBF_BASEVECTORGEOD_1_0_VECTORINFO_LENGTH 
#define SBF_BASEVECTORGEOD_1_0_VECTORINFO_LENGTH 30 
#endif

#define SBF_BASEVECTORGEOD_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        N;            
  uint8_t        SBLength;     
  VectorInfoGeod_1_0_t VectorInfo[SBF_BASEVECTORGEOD_1_0_VECTORINFO_LENGTH];
} BaseVectorGeod_1_0_t;

typedef VectorInfoGeod_1_0_t VectorInfoGeod_1_t;

#define SBF_BASEVECTORGEOD_1_VECTORINFO_LENGTH SBF_BASEVECTORGEOD_1_0_VECTORINFO_LENGTH
typedef BaseVectorGeod_1_0_t BaseVectorGeod_1_t;

/*--EndOfPVT_1_0_t : --------------------------------------------------------*/
/* PVT epoch marker */

#define SBF_ENDOFPVT_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          
} EndOfPVT_1_0_t;

typedef EndOfPVT_1_0_t EndOfPVT_1_t;

/*--BaseLine_1_0_t : --------------------------------------------------------*/
/* Base-rover vector (deprecated block - not to be used) */

#define SBF_BASELINE_1_0_TOW_PRECISION 3
#define SBF_BASELINE_1_0_EAST_PRECISION 3
#define SBF_BASELINE_1_0_NORTH_PRECISION 3
#define SBF_BASELINE_1_0_UP_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint16_t       BaseStationID;/* 12 bits required RTCM3 */
  SBFDOUBLE      East;         /* 20km cannot be captured with F32_t in mm */
  SBFDOUBLE      North;        /* 20km cannot be captured with F32_t in mm */
  SBFDOUBLE      Up;           /* 20km cannot be captured with F32_t in mm */
} BaseLine_1_0_t;

typedef BaseLine_1_0_t BaseLine_1_t;


/*==INS/GNSS Integrated Blocks===============================================*/

/*--IntPVCart_1_0_t : -------------------------------------------------------*/
/* Integrated PV in Cartesian coordinates */

#define SBF_INTPVCART_1_0_TOW_PRECISION 3
#define SBF_INTPVCART_1_0_X_PRECISION 3
#define SBF_INTPVCART_1_0_Y_PRECISION 3
#define SBF_INTPVCART_1_0_Z_PRECISION 3
#define SBF_INTPVCART_1_0_VX_PRECISION 3
#define SBF_INTPVCART_1_0_VY_PRECISION 3
#define SBF_INTPVCART_1_0_VZ_PRECISION 3
#define SBF_INTPVCART_1_0_COG_PRECISION 2
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        Mode;         
  uint8_t        Error;        
  uint16_t       Info;         
  uint8_t        NrSV;         
  uint8_t        NrAnt;        
  uint8_t        GNSSPVTMode;  
  uint8_t        Datum;        
  uint16_t       GNSSage;      
  SBFDOUBLE      X;            
  SBFDOUBLE      Y;            
  SBFDOUBLE      Z;            
  float          Vx;           
  float          Vy;           
  float          Vz;           
  float          COG;          
} IntPVCart_1_0_t;

typedef IntPVCart_1_0_t IntPVCart_1_t;

/*--IntPVGeod_1_0_t : -------------------------------------------------------*/
/* Integrated PV in Geodetic coordinates */

#define SBF_INTPVGEOD_1_0_TOW_PRECISION 3
#define SBF_INTPVGEOD_1_0_LAT_PRECISION 12
#define SBF_INTPVGEOD_1_0_LON_PRECISION 12
#define SBF_INTPVGEOD_1_0_ALT_PRECISION 3
#define SBF_INTPVGEOD_1_0_VN_PRECISION 3
#define SBF_INTPVGEOD_1_0_VE_PRECISION 3
#define SBF_INTPVGEOD_1_0_VU_PRECISION 3
#define SBF_INTPVGEOD_1_0_COG_PRECISION 2
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        Mode;         
  uint8_t        Error;        
  uint16_t       Info;         
  uint8_t        NrSV;         
  uint8_t        NrAnt;        
  uint8_t        GNSSPVTMode;  
  uint8_t        Datum;        
  uint16_t       GNSSage;      
  SBFDOUBLE      Lat;          
  SBFDOUBLE      Lon;          
  SBFDOUBLE      Alt;          
  float          Vn;           
  float          Ve;           
  float          Vu;           
  float          COG;          
} IntPVGeod_1_0_t;

typedef IntPVGeod_1_0_t IntPVGeod_1_t;

/*--IntPosCovCart_1_0_t : ---------------------------------------------------*/
/* Integrated position covariance matrix (X, Y, Z) */

#define SBF_INTPOSCOVCART_1_0_TOW_PRECISION 3
#define SBF_INTPOSCOVCART_1_0_COV_XX_PRECISION 8
#define SBF_INTPOSCOVCART_1_0_COV_YY_PRECISION 8
#define SBF_INTPOSCOVCART_1_0_COV_ZZ_PRECISION 8
#define SBF_INTPOSCOVCART_1_0_COV_XY_PRECISION 8
#define SBF_INTPOSCOVCART_1_0_COV_XZ_PRECISION 8
#define SBF_INTPOSCOVCART_1_0_COV_YZ_PRECISION 8
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        Mode;         
  uint8_t        Error;        
  float          Cov_xx;       
  float          Cov_yy;       
  float          Cov_zz;       
  float          Cov_xy;       
  float          Cov_xz;       
  float          Cov_yz;       
} IntPosCovCart_1_0_t;

typedef IntPosCovCart_1_0_t IntPosCovCart_1_t;

/*--IntVelCovCart_1_0_t : ---------------------------------------------------*/
/* Integrated velocity covariance matrix (X, Y, Z) */

#define SBF_INTVELCOVCART_1_0_TOW_PRECISION 3
#define SBF_INTVELCOVCART_1_0_COV_VXVX_PRECISION 3
#define SBF_INTVELCOVCART_1_0_COV_VYVY_PRECISION 3
#define SBF_INTVELCOVCART_1_0_COV_VZVZ_PRECISION 3
#define SBF_INTVELCOVCART_1_0_COV_VXVY_PRECISION 3
#define SBF_INTVELCOVCART_1_0_COV_VXVZ_PRECISION 3
#define SBF_INTVELCOVCART_1_0_COV_VYVZ_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        Mode;         
  uint8_t        Error;        
  float          Cov_VxVx;     
  float          Cov_VyVy;     
  float          Cov_VzVz;     
  float          Cov_VxVy;     
  float          Cov_VxVz;     
  float          Cov_VyVz;     
} IntVelCovCart_1_0_t;

typedef IntVelCovCart_1_0_t IntVelCovCart_1_t;

/*--IntPosCovGeod_1_0_t : ---------------------------------------------------*/
/* Integrated position covariance matrix (Lat, Lon, Alt) */

#define SBF_INTPOSCOVGEOD_1_0_TOW_PRECISION 3
#define SBF_INTPOSCOVGEOD_1_0_COV_LATLAT_PRECISION 8
#define SBF_INTPOSCOVGEOD_1_0_COV_LONLON_PRECISION 8
#define SBF_INTPOSCOVGEOD_1_0_COV_ALTALT_PRECISION 8
#define SBF_INTPOSCOVGEOD_1_0_COV_LATLON_PRECISION 8
#define SBF_INTPOSCOVGEOD_1_0_COV_LATALT_PRECISION 8
#define SBF_INTPOSCOVGEOD_1_0_COV_LONALT_PRECISION 8
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        Mode;         
  uint8_t        Error;        
  float          Cov_LatLat;   
  float          Cov_LonLon;   
  float          Cov_AltAlt;   
  float          Cov_LatLon;   
  float          Cov_LatAlt;   
  float          Cov_LonAlt;   
} IntPosCovGeod_1_0_t;

typedef IntPosCovGeod_1_0_t IntPosCovGeod_1_t;

/*--IntVelCovGeod_1_0_t : ---------------------------------------------------*/
/* Integrated velocity covariance matrix (North, East, Up) */

#define SBF_INTVELCOVGEOD_1_0_TOW_PRECISION 3
#define SBF_INTVELCOVGEOD_1_0_COV_VNVN_PRECISION 3
#define SBF_INTVELCOVGEOD_1_0_COV_VEVE_PRECISION 3
#define SBF_INTVELCOVGEOD_1_0_COV_VUVU_PRECISION 3
#define SBF_INTVELCOVGEOD_1_0_COV_VNVE_PRECISION 3
#define SBF_INTVELCOVGEOD_1_0_COV_VNVU_PRECISION 3
#define SBF_INTVELCOVGEOD_1_0_COV_VEVU_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        Mode;         
  uint8_t        Error;        
  float          Cov_VnVn;     
  float          Cov_VeVe;     
  float          Cov_VuVu;     
  float          Cov_VnVe;     
  float          Cov_VnVu;     
  float          Cov_VeVu;     
} IntVelCovGeod_1_0_t;

typedef IntVelCovGeod_1_0_t IntVelCovGeod_1_t;

/*--IntAttEuler_1_0_t : -----------------------------------------------------*/
/* Integrated attitude in Euler angles */

#define SBF_INTATTEULER_1_0_TOW_PRECISION 3
#define SBF_INTATTEULER_1_0_HEADING_PRECISION 6
#define SBF_INTATTEULER_1_0_PITCH_PRECISION 6
#define SBF_INTATTEULER_1_0_ROLL_PRECISION 6
#define SBF_INTATTEULER_1_0_PITCHDOT_PRECISION 3
#define SBF_INTATTEULER_1_0_ROLLDOT_PRECISION 3
#define SBF_INTATTEULER_1_0_HEADINGDOT_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        Mode;         
  uint8_t        Error;        
  uint16_t       Info;         
  uint8_t        NrSV;         
  uint8_t        NrAnt;        
  uint8_t        Reserved;     /* Reserved for future use */
  uint8_t        Datum;        
  uint16_t       GNSSage;      
  float          Heading;      
  float          Pitch;        
  float          Roll;         
  float          PitchDot;     
  float          RollDot;      
  float          HeadingDot;   
} IntAttEuler_1_0_t;

/*--IntAttEuler_1_1_t : -----------------------------------------------------*/
/* Integrated attitude in Euler angles */

#define SBF_INTATTEULER_1_1_TOW_PRECISION 3
#define SBF_INTATTEULER_1_1_HEADING_PRECISION 6
#define SBF_INTATTEULER_1_1_PITCH_PRECISION 6
#define SBF_INTATTEULER_1_1_ROLL_PRECISION 6
#define SBF_INTATTEULER_1_1_PITCHDOT_PRECISION 3
#define SBF_INTATTEULER_1_1_ROLLDOT_PRECISION 3
#define SBF_INTATTEULER_1_1_HEADINGDOT_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        Mode;         
  uint8_t        Error;        
  uint16_t       Info;         
  uint8_t        NrSV;         
  uint8_t        NrAnt;        
  uint8_t        GNSSPVTMode;  
  uint8_t        Datum;        
  uint16_t       GNSSage;      
  float          Heading;      
  float          Pitch;        
  float          Roll;         
  float          PitchDot;     
  float          RollDot;      
  float          HeadingDot;   
} IntAttEuler_1_1_t;

typedef IntAttEuler_1_1_t IntAttEuler_1_t;

/*--IntAttCovEuler_1_0_t : --------------------------------------------------*/
/* Integrated attitude covariance matrix of Euler angles */

#define SBF_INTATTCOVEULER_1_0_TOW_PRECISION 3
#define SBF_INTATTCOVEULER_1_0_COV_HEADHEAD_PRECISION 3
#define SBF_INTATTCOVEULER_1_0_COV_PITCHPITCH_PRECISION 3
#define SBF_INTATTCOVEULER_1_0_COV_ROLLROLL_PRECISION 3
#define SBF_INTATTCOVEULER_1_0_COV_HEADPITCH_PRECISION 3
#define SBF_INTATTCOVEULER_1_0_COV_HEADROLL_PRECISION 3
#define SBF_INTATTCOVEULER_1_0_COV_PITCHROLL_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        Mode;         
  uint8_t        Error;        
  float          Cov_HeadHead; 
  float          Cov_PitchPitch;
  float          Cov_RollRoll; 
  float          Cov_HeadPitch;
  float          Cov_HeadRoll; 
  float          Cov_PitchRoll;
} IntAttCovEuler_1_0_t;

typedef IntAttCovEuler_1_0_t IntAttCovEuler_1_t;

/*--IntPVAAGeod_1_0_t : -----------------------------------------------------*/
/* Integrated position, velocity, acceleration and attitude */

#define SBF_INTPVAAGEOD_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        Mode;         
  uint8_t        Error;        
  uint16_t       Info;         
  uint8_t        GNSSPVTMode;  
  uint8_t        Datum;        
  uint8_t        GNSSage;      
  uint8_t        NrSVAnt;      
  uint8_t        Reserved;     
  uint8_t        PosFine;      
  int32_t        Lat;          
  int32_t        Lon;          
  int32_t        Alt;          
  int32_t        Vn;           
  int32_t        Ve;           
  int32_t        Vu;           
  int16_t        Ax;           
  int16_t        Ay;           
  int16_t        Az;           
  uint16_t       Heading;      
  int16_t        Pitch;        
  int16_t        Roll;         
} IntPVAAGeod_1_0_t;

typedef IntPVAAGeod_1_0_t IntPVAAGeod_1_t;

/*--INSNavCart_1_0_t : ------------------------------------------------------*/
/* INS solution in Cartesian coordinates */

#define SBF_INSNAVCARTPOSSTDDEV_1_0_XSTDDEV_PRECISION 3
#define SBF_INSNAVCARTPOSSTDDEV_1_0_YSTDDEV_PRECISION 3
#define SBF_INSNAVCARTPOSSTDDEV_1_0_ZSTDDEV_PRECISION 3
typedef struct
{
  float          XStdDev;      
  float          YStdDev;      
  float          ZStdDev;      
} INSNavCartPosStdDev_1_0_t;

#define SBF_INSNAVCARTATT_1_0_HEADING_PRECISION 6
#define SBF_INSNAVCARTATT_1_0_PITCH_PRECISION 6
#define SBF_INSNAVCARTATT_1_0_ROLL_PRECISION 6
typedef struct
{
  float          Heading;      
  float          Pitch;        
  float          Roll;         
} INSNavCartAtt_1_0_t;

#define SBF_INSNAVCARTATTSTDDEV_1_0_HEADINGSTDDEV_PRECISION 6
#define SBF_INSNAVCARTATTSTDDEV_1_0_PITCHSTDDEV_PRECISION 6
#define SBF_INSNAVCARTATTSTDDEV_1_0_ROLLSTDDEV_PRECISION 6
typedef struct
{
  float          HeadingStdDev;
  float          PitchStdDev;  
  float          RollStdDev;   
} INSNavCartAttStdDev_1_0_t;

#define SBF_INSNAVCARTVEL_1_0_VX_PRECISION 3
#define SBF_INSNAVCARTVEL_1_0_VY_PRECISION 3
#define SBF_INSNAVCARTVEL_1_0_VZ_PRECISION 3
typedef struct
{
  float          Vx;           
  float          Vy;           
  float          Vz;           
} INSNavCartVel_1_0_t;

#define SBF_INSNAVCARTVELSTDDEV_1_0_VXSTDDEV_PRECISION 3
#define SBF_INSNAVCARTVELSTDDEV_1_0_VYSTDDEV_PRECISION 3
#define SBF_INSNAVCARTVELSTDDEV_1_0_VZSTDDEV_PRECISION 3
typedef struct
{
  float          VxStdDev;     
  float          VyStdDev;     
  float          VzStdDev;     
} INSNavCartVelStdDev_1_0_t;

typedef union
{
  INSNavCartPosStdDev_1_0_t PosStdDev;    
  INSNavCartAtt_1_0_t Att;          
  INSNavCartAttStdDev_1_0_t AttStdDev;    
  INSNavCartVel_1_0_t Vel;          
  INSNavCartVelStdDev_1_0_t VelStdDev;    
} INSNavCartData_1_0_t;

#ifndef SBF_INSNAVCART_1_0_INSNAVCARTDATA_LENGTH 
#define SBF_INSNAVCART_1_0_INSNAVCARTDATA_LENGTH 16 
#endif

#define SBF_INSNAVCART_1_0_TOW_PRECISION 3
#define SBF_INSNAVCART_1_0_X_PRECISION 3
#define SBF_INSNAVCART_1_0_Y_PRECISION 3
#define SBF_INSNAVCART_1_0_Z_PRECISION 3
#define SBF_INSNAVCART_1_0_ACCURACY_PRECISION 2
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        GNSSMode;     
  uint8_t        Error;        
  uint16_t       Info;         
  uint16_t       GNSSAge;      
  SBFDOUBLE      X;            
  SBFDOUBLE      Y;            
  SBFDOUBLE      Z;            
  uint16_t       Accuracy;     
  uint16_t       Latency;      
  uint8_t        Datum;        
  uint8_t        Reserved;     
  uint16_t       SBList;       
  INSNavCartData_1_0_t INSNavCartData[SBF_INSNAVCART_1_0_INSNAVCARTDATA_LENGTH];
} INSNavCart_1_0_t;

typedef INSNavCartPosStdDev_1_0_t INSNavCartPosStdDev_1_t;

typedef INSNavCartAtt_1_0_t INSNavCartAtt_1_t;

typedef INSNavCartAttStdDev_1_0_t INSNavCartAttStdDev_1_t;

typedef INSNavCartVel_1_0_t INSNavCartVel_1_t;

typedef INSNavCartVelStdDev_1_0_t INSNavCartVelStdDev_1_t;

typedef INSNavCartData_1_0_t INSNavCartData_1_t;

#define SBF_INSNAVCART_1_INSNAVCARTDATA_LENGTH SBF_INSNAVCART_1_0_INSNAVCARTDATA_LENGTH
typedef INSNavCart_1_0_t INSNavCart_1_t;

/*--INSNavGeod_1_0_t : ------------------------------------------------------*/
/* INS solution in Geodetic coordinates */

#define SBF_INSNAVGEODPOSSTDDEV_1_0_LATITUDESTDDEV_PRECISION 3
#define SBF_INSNAVGEODPOSSTDDEV_1_0_LONGITUDESTDDEV_PRECISION 3
#define SBF_INSNAVGEODPOSSTDDEV_1_0_HEIGHTSTDDEV_PRECISION 3
typedef struct
{
  float          LatitudeStdDev;
  float          LongitudeStdDev;
  float          HeightStdDev; 
} INSNavGeodPosStdDev_1_0_t;

#define SBF_INSNAVGEODATT_1_0_HEADING_PRECISION 6
#define SBF_INSNAVGEODATT_1_0_PITCH_PRECISION 6
#define SBF_INSNAVGEODATT_1_0_ROLL_PRECISION 6
typedef struct
{
  float          Heading;      
  float          Pitch;        
  float          Roll;         
} INSNavGeodAtt_1_0_t;

#define SBF_INSNAVGEODATTSTDDEV_1_0_HEADINGSTDDEV_PRECISION 6
#define SBF_INSNAVGEODATTSTDDEV_1_0_PITCHSTDDEV_PRECISION 6
#define SBF_INSNAVGEODATTSTDDEV_1_0_ROLLSTDDEV_PRECISION 6
typedef struct
{
  float          HeadingStdDev;
  float          PitchStdDev;  
  float          RollStdDev;   
} INSNavGeodAttStdDev_1_0_t;

#define SBF_INSNAVGEODVEL_1_0_VE_PRECISION 3
#define SBF_INSNAVGEODVEL_1_0_VN_PRECISION 3
#define SBF_INSNAVGEODVEL_1_0_VU_PRECISION 3
typedef struct
{
  float          Ve;           
  float          Vn;           
  float          Vu;           
} INSNavGeodVel_1_0_t;

#define SBF_INSNAVGEODVELSTDDEV_1_0_VESTDDEV_PRECISION 3
#define SBF_INSNAVGEODVELSTDDEV_1_0_VNSTDDEV_PRECISION 3
#define SBF_INSNAVGEODVELSTDDEV_1_0_VUSTDDEV_PRECISION 3
typedef struct
{
  float          VeStdDev;     
  float          VnStdDev;     
  float          VuStdDev;     
} INSNavGeodVelStdDev_1_0_t;

typedef union
{
  INSNavGeodPosStdDev_1_0_t PosStdDev;    
  INSNavGeodAtt_1_0_t Att;          
  INSNavGeodAttStdDev_1_0_t AttStdDev;    
  INSNavGeodVel_1_0_t Vel;          
  INSNavGeodVelStdDev_1_0_t VelStdDev;    
} INSNavGeodData_1_0_t;

#ifndef SBF_INSNAVGEOD_1_0_INSNAVGEODDATA_LENGTH 
#define SBF_INSNAVGEOD_1_0_INSNAVGEODDATA_LENGTH 16 
#endif

#define SBF_INSNAVGEOD_1_0_TOW_PRECISION 3
#define SBF_INSNAVGEOD_1_0_LATITUDE_PRECISION 12
#define SBF_INSNAVGEOD_1_0_LONGITUDE_PRECISION 12
#define SBF_INSNAVGEOD_1_0_HEIGHT_PRECISION 3
#define SBF_INSNAVGEOD_1_0_UNDULATION_PRECISION 3
#define SBF_INSNAVGEOD_1_0_ACCURACY_PRECISION 2
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        GNSSMode;     
  uint8_t        Error;        
  uint16_t       Info;         
  uint16_t       GNSSAge;      
  SBFDOUBLE      Latitude;     
  SBFDOUBLE      Longitude;    
  SBFDOUBLE      Height;       
  float          Undulation;   
  uint16_t       Accuracy;     
  uint16_t       Latency;      
  uint8_t        Datum;        
  uint8_t        Reserved;     
  uint16_t       SBList;       
  INSNavGeodData_1_0_t INSNavGeodData[SBF_INSNAVGEOD_1_0_INSNAVGEODDATA_LENGTH];
} INSNavGeod_1_0_t;

typedef INSNavGeodPosStdDev_1_0_t INSNavGeodPosStdDev_1_t;

typedef INSNavGeodAtt_1_0_t INSNavGeodAtt_1_t;

typedef INSNavGeodAttStdDev_1_0_t INSNavGeodAttStdDev_1_t;

typedef INSNavGeodVel_1_0_t INSNavGeodVel_1_t;

typedef INSNavGeodVelStdDev_1_0_t INSNavGeodVelStdDev_1_t;

typedef INSNavGeodData_1_0_t INSNavGeodData_1_t;

#define SBF_INSNAVGEOD_1_INSNAVGEODDATA_LENGTH SBF_INSNAVGEOD_1_0_INSNAVGEODDATA_LENGTH
typedef INSNavGeod_1_0_t INSNavGeod_1_t;


/*==GNSS Attitude Blocks=====================================================*/

/*--AttEuler_1_0_t : --------------------------------------------------------*/
/* GNSS attitude expressed as Euler angles */

#define SBF_ATTEULER_1_0_TOW_PRECISION 3
#define SBF_ATTEULER_1_0_HEADING_PRECISION 6
#define SBF_ATTEULER_1_0_PITCH_PRECISION 6
#define SBF_ATTEULER_1_0_ROLL_PRECISION 6
#define SBF_ATTEULER_1_0_PITCHDOT_PRECISION 3
#define SBF_ATTEULER_1_0_ROLLDOT_PRECISION 3
#define SBF_ATTEULER_1_0_HEADINGDOT_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        NRSV;         
  uint8_t        Error;        
  uint16_t       Mode;         
  uint16_t       Reserved;     
  float          Heading;      
  float          Pitch;        
  float          Roll;         
  float          PitchDot;     
  float          RollDot;      
  float          HeadingDot;   
} AttEuler_1_0_t;

typedef AttEuler_1_0_t AttEuler_1_t;

/*--AttCovEuler_1_0_t : -----------------------------------------------------*/
/* Covariance matrix of attitude */

#define SBF_ATTCOVEULER_1_0_TOW_PRECISION 3
#define SBF_ATTCOVEULER_1_0_COV_HEADHEAD_PRECISION 3
#define SBF_ATTCOVEULER_1_0_COV_PITCHPITCH_PRECISION 3
#define SBF_ATTCOVEULER_1_0_COV_ROLLROLL_PRECISION 3
#define SBF_ATTCOVEULER_1_0_COV_HEADPITCH_PRECISION 3
#define SBF_ATTCOVEULER_1_0_COV_HEADROLL_PRECISION 3
#define SBF_ATTCOVEULER_1_0_COV_PITCHROLL_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        Reserved;     
  uint8_t        Error;        
  float          Cov_HeadHead; 
  float          Cov_PitchPitch;
  float          Cov_RollRoll; 
  float          Cov_HeadPitch;
  float          Cov_HeadRoll; 
  float          Cov_PitchRoll;
} AttCovEuler_1_0_t;

typedef AttCovEuler_1_0_t AttCovEuler_1_t;

/*--AuxAntPositions_1_0_t : -------------------------------------------------*/
/* Relative position and velocity estimates of auxiliary antennas */

#define SBF_AUXANTPOSITIONSUB_1_0_DELTAEAST_PRECISION 3
#define SBF_AUXANTPOSITIONSUB_1_0_DELTANORTH_PRECISION 3
#define SBF_AUXANTPOSITIONSUB_1_0_DELTAUP_PRECISION 3
#define SBF_AUXANTPOSITIONSUB_1_0_EASTVELOCITY_PRECISION 3
#define SBF_AUXANTPOSITIONSUB_1_0_NORTHVELOCITY_PRECISION 3
#define SBF_AUXANTPOSITIONSUB_1_0_UPVELOCITY_PRECISION 3
typedef struct
{
  uint8_t        NRSV;         
  uint8_t        Error;        
  uint8_t        AmbiguityType;
  uint8_t        AuxAntID;     
  SBFDOUBLE      DeltaEast;    
  SBFDOUBLE      DeltaNorth;   
  SBFDOUBLE      DeltaUp;      
  SBFDOUBLE      EastVelocity; 
  SBFDOUBLE      NorthVelocity;
  SBFDOUBLE      UpVelocity;   
} AuxAntPositionSub_1_0_t;

#ifndef SBF_AUXANTPOSITIONS_1_0_AUXANTPOSITIONS_LENGTH 
#define SBF_AUXANTPOSITIONS_1_0_AUXANTPOSITIONS_LENGTH 4 
#endif

#define SBF_AUXANTPOSITIONS_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        NbrAuxAntennas;
  uint8_t        SBSize;       
  AuxAntPositionSub_1_0_t AuxAntPositions[SBF_AUXANTPOSITIONS_1_0_AUXANTPOSITIONS_LENGTH];
} AuxAntPositions_1_0_t;

typedef AuxAntPositionSub_1_0_t AuxAntPositionSub_1_t;

#define SBF_AUXANTPOSITIONS_1_AUXANTPOSITIONS_LENGTH SBF_AUXANTPOSITIONS_1_0_AUXANTPOSITIONS_LENGTH
typedef AuxAntPositions_1_0_t AuxAntPositions_1_t;

/*--EndOfAtt_1_0_t : --------------------------------------------------------*/
/* GNSS attitude epoch marker */

#define SBF_ENDOFATT_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          
} EndOfAtt_1_0_t;

typedef EndOfAtt_1_0_t EndOfAtt_1_t;

/*--AttQuat_1_0_t : ---------------------------------------------------------*/

#define SBF_ATTQUAT_1_0_TOW_PRECISION 3
#define SBF_ATTQUAT_1_0_Q1_PRECISION 3
#define SBF_ATTQUAT_1_0_Q2_PRECISION 3
#define SBF_ATTQUAT_1_0_Q3_PRECISION 3
#define SBF_ATTQUAT_1_0_Q4_PRECISION 3
#define SBF_ATTQUAT_1_0_PITCHDOT_PRECISION 3
#define SBF_ATTQUAT_1_0_ROLLDOT_PRECISION 3
#define SBF_ATTQUAT_1_0_HEADINGDOT_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        NRSV;         
  uint8_t        Error;        
  uint16_t       Mode;         
  uint16_t       Reserved;     
  float          q1;           
  float          q2;           
  float          q3;           
  float          q4;           
  float          PitchDot;     
  float          RollDot;      
  float          HeadingDot;   
} AttQuat_1_0_t;

typedef AttQuat_1_0_t AttQuat_1_t;

/*--AttCovQuat_1_0_t : ------------------------------------------------------*/

#define SBF_ATTCOVQUAT_1_0_TOW_PRECISION 3
#define SBF_ATTCOVQUAT_1_0_COV_Q1Q1_PRECISION 3
#define SBF_ATTCOVQUAT_1_0_COV_Q2Q2_PRECISION 3
#define SBF_ATTCOVQUAT_1_0_COV_Q3Q3_PRECISION 3
#define SBF_ATTCOVQUAT_1_0_COV_Q4Q4_PRECISION 3
#define SBF_ATTCOVQUAT_1_0_COV_Q1Q2_PRECISION 3
#define SBF_ATTCOVQUAT_1_0_COV_Q1Q3_PRECISION 3
#define SBF_ATTCOVQUAT_1_0_COV_Q1Q4_PRECISION 3
#define SBF_ATTCOVQUAT_1_0_COV_Q2Q3_PRECISION 3
#define SBF_ATTCOVQUAT_1_0_COV_Q2Q4_PRECISION 3
#define SBF_ATTCOVQUAT_1_0_COV_Q3Q4_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        Reserved;     
  uint8_t        Error;        
  float          Cov_q1q1;     
  float          Cov_q2q2;     
  float          Cov_q3q3;     
  float          Cov_q4q4;     
  float          Cov_q1q2;     
  float          Cov_q1q3;     
  float          Cov_q1q4;     
  float          Cov_q2q3;     
  float          Cov_q2q4;     
  float          Cov_q3q4;     
} AttCovQuat_1_0_t;

typedef AttCovQuat_1_0_t AttCovQuat_1_t;


/*==Receiver Time Blocks=====================================================*/

/*--ReceiverTime_1_0_t : ----------------------------------------------------*/
/* Current receiver and UTC time */

#ifndef SBF_RECEIVERTIME_1_0__PADDING_LENGTH 
#define SBF_RECEIVERTIME_1_0__PADDING_LENGTH 2 
#endif

#define SBF_RECEIVERTIME_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  int8_t         UTCYear;      
  int8_t         UTCMonth;     
  int8_t         UTCDay;       
  int8_t         UTCHour;      
  int8_t         UTCMin;       
  int8_t         UTCSec;       
  int8_t         DeltaLS;      
  uint8_t        SyncLevel;    

  uint8_t        _padding[SBF_RECEIVERTIME_1_0__PADDING_LENGTH];
} ReceiverTime_1_0_t;

#define SBF_RECEIVERTIME_1__PADDING_LENGTH SBF_RECEIVERTIME_1_0__PADDING_LENGTH
typedef ReceiverTime_1_0_t ReceiverTime_1_t;

/*--xPPSOffset_1_0_t : ------------------------------------------------------*/
/* Offset of the xPPS pulse with respect to GNSS time */

#define SBF_PPSDATA_1_0_TOW_PRECISION 3
typedef struct
{
  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        SyncAge;      
  uint8_t        Source;       
  float          Offset;       
} PPSData_1_0_t;

typedef struct
{
  BlockHeader_t  Header;       

  PPSData_1_0_t  PPSData;      
} xPPSOffset_1_0_t;

typedef PPSData_1_0_t PPSData_1_t;

typedef xPPSOffset_1_0_t xPPSOffset_1_t;

/*--SysTimeOffset_1_0_t : ---------------------------------------------------*/
/* Time offset between different constellations */

#ifndef SBF_TIMEOFFSETSUB_1_0__PADDING_LENGTH 
#define SBF_TIMEOFFSETSUB_1_0__PADDING_LENGTH 3 
#endif

#define SBF_TIMEOFFSETSUB_1_0_OFFSET_PRECISION 3
typedef struct
{
  uint8_t        TimeSystem;   
  float          Offset;       

  uint8_t        _padding[SBF_TIMEOFFSETSUB_1_0__PADDING_LENGTH];
} TimeOffsetSub_1_0_t;

#ifndef SBF_SYSTIMEOFFSET_1_0_TIMEOFFSET_LENGTH 
#define SBF_SYSTIMEOFFSET_1_0_TIMEOFFSET_LENGTH 11 
#endif

#define SBF_SYSTIMEOFFSET_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        RefTimeSystem;
  uint8_t        N;            
  uint8_t        SBLength;     
  uint8_t        Reserved2;    /* Reserved for future use */
  uint8_t        Reserved3[2]; /* Reserved for future use */
  TimeOffsetSub_1_0_t TimeOffset[SBF_SYSTIMEOFFSET_1_0_TIMEOFFSET_LENGTH];
} SysTimeOffset_1_0_t;

/*--SysTimeOffset_1_1_t : ---------------------------------------------------*/
/* Time offset between different constellations */

#define SBF_TIMEOFFSETSUB_1_1_OFFSET_PRECISION 3
typedef struct
{
  uint8_t        TimeSystem;   
  uint8_t        Mode;         
  uint8_t        Reserved[2];  
  float          Offset;       
} TimeOffsetSub_1_1_t;

#ifndef SBF_SYSTIMEOFFSET_1_1_TIMEOFFSET_LENGTH 
#define SBF_SYSTIMEOFFSET_1_1_TIMEOFFSET_LENGTH 11 
#endif

#define SBF_SYSTIMEOFFSET_1_1_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        RefTimeSystem;
  uint8_t        N;            
  uint8_t        SBLength;     
  uint8_t        Alert_flags;  
  uint8_t        Reserved[2];  
  TimeOffsetSub_1_1_t TimeOffset[SBF_SYSTIMEOFFSET_1_1_TIMEOFFSET_LENGTH];
} SysTimeOffset_1_1_t;

typedef TimeOffsetSub_1_1_t TimeOffsetSub_1_t;

#define SBF_SYSTIMEOFFSET_1_TIMEOFFSET_LENGTH SBF_SYSTIMEOFFSET_1_1_TIMEOFFSET_LENGTH
typedef SysTimeOffset_1_1_t SysTimeOffset_1_t;


/*==External Event Blocks====================================================*/

/*--ExtEvent_1_0_t : --------------------------------------------------------*/
/* Time at the instant of an external event */

#define SBF_TIMERDATA_1_0_TOW_PRECISION 3
#define SBF_TIMERDATA_1_0_RXCLKBIAS_PRECISION 9
typedef struct
{
  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        Source;       
  uint8_t        Polarity;     
  float          Offset;       
  SBFDOUBLE      RxClkBias;    
} TimerData_1_0_t;

typedef struct
{
  BlockHeader_t  Header;       

  TimerData_1_0_t TimerData;    
} ExtEvent_1_0_t;

/*--ExtEvent_1_1_t : --------------------------------------------------------*/
/* Time at the instant of an external event */

#define SBF_TIMERDATA_1_1_TOW_PRECISION 3
#define SBF_TIMERDATA_1_1_RXCLKBIAS_PRECISION 9
typedef struct
{
  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        Source;       
  uint8_t        Polarity;     
  float          Offset;       
  SBFDOUBLE      RxClkBias;    
  uint16_t       PVTAge;       
} TimerData_1_1_t;

typedef struct
{
  BlockHeader_t  Header;       

  TimerData_1_1_t TimerData;    
} ExtEvent_1_1_t;

typedef TimerData_1_1_t TimerData_1_t;

typedef ExtEvent_1_1_t ExtEvent_1_t;

/*--ExtEventPVTCartesian_1_0_t : --------------------------------------------*/
/* Cartesian position at the instant of an event */

#ifndef SBF_EXTEVENTPVTCARTESIAN_1_0__PADDING_LENGTH 
#define SBF_EXTEVENTPVTCARTESIAN_1_0__PADDING_LENGTH 3 
#endif

#define SBF_EXTEVENTPVTCARTESIAN_1_0_TOW_PRECISION 3
#define SBF_EXTEVENTPVTCARTESIAN_1_0_X_PRECISION 3
#define SBF_EXTEVENTPVTCARTESIAN_1_0_Y_PRECISION 3
#define SBF_EXTEVENTPVTCARTESIAN_1_0_Z_PRECISION 3
#define SBF_EXTEVENTPVTCARTESIAN_1_0_UNDULATION_PRECISION 3
#define SBF_EXTEVENTPVTCARTESIAN_1_0_VX_PRECISION 3
#define SBF_EXTEVENTPVTCARTESIAN_1_0_VY_PRECISION 3
#define SBF_EXTEVENTPVTCARTESIAN_1_0_VZ_PRECISION 3
#define SBF_EXTEVENTPVTCARTESIAN_1_0_COG_PRECISION 2
#define SBF_EXTEVENTPVTCARTESIAN_1_0_RXCLKBIAS_PRECISION 6
#define SBF_EXTEVENTPVTCARTESIAN_1_0_RXCLKDRIFT_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        Mode;         
  uint8_t        Error;        
  SBFDOUBLE      X;            
  SBFDOUBLE      Y;            
  SBFDOUBLE      Z;            
  float          Undulation;   
  float          Vx;           
  float          Vy;           
  float          Vz;           
  float          COG;          
  SBFDOUBLE      RxClkBias;    
  float          RxClkDrift;   
  uint8_t        TimeSystem;   
  uint8_t        Datum;        
  uint8_t        NrSV;         
  uint8_t        WACorrInfo;   
  uint16_t       ReferenceId;  
  uint16_t       MeanCorrAge;  
  uint32_t       SignalInfo;   
  uint8_t        AlertFlag;    

  uint8_t        _padding[SBF_EXTEVENTPVTCARTESIAN_1_0__PADDING_LENGTH];
} ExtEventPVTCartesian_1_0_t;

/*--ExtEventPVTCartesian_1_1_t : --------------------------------------------*/
/* Cartesian position at the instant of an event */

#define SBF_EXTEVENTPVTCARTESIAN_1_1_TOW_PRECISION 3
#define SBF_EXTEVENTPVTCARTESIAN_1_1_X_PRECISION 3
#define SBF_EXTEVENTPVTCARTESIAN_1_1_Y_PRECISION 3
#define SBF_EXTEVENTPVTCARTESIAN_1_1_Z_PRECISION 3
#define SBF_EXTEVENTPVTCARTESIAN_1_1_UNDULATION_PRECISION 3
#define SBF_EXTEVENTPVTCARTESIAN_1_1_VX_PRECISION 3
#define SBF_EXTEVENTPVTCARTESIAN_1_1_VY_PRECISION 3
#define SBF_EXTEVENTPVTCARTESIAN_1_1_VZ_PRECISION 3
#define SBF_EXTEVENTPVTCARTESIAN_1_1_COG_PRECISION 2
#define SBF_EXTEVENTPVTCARTESIAN_1_1_RXCLKBIAS_PRECISION 6
#define SBF_EXTEVENTPVTCARTESIAN_1_1_RXCLKDRIFT_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        Mode;         
  uint8_t        Error;        
  SBFDOUBLE      X;            
  SBFDOUBLE      Y;            
  SBFDOUBLE      Z;            
  float          Undulation;   
  float          Vx;           
  float          Vy;           
  float          Vz;           
  float          COG;          
  SBFDOUBLE      RxClkBias;    
  float          RxClkDrift;   
  uint8_t        TimeSystem;   
  uint8_t        Datum;        
  uint8_t        NrSV;         
  uint8_t        WACorrInfo;   
  uint16_t       ReferenceId;  
  uint16_t       MeanCorrAge;  
  uint32_t       SignalInfo;   
  uint8_t        AlertFlag;    
  uint8_t        NrBases;      
  uint16_t       PPPInfo;      
} ExtEventPVTCartesian_1_1_t;

/*--ExtEventPVTCartesian_1_2_t : --------------------------------------------*/
/* Cartesian position at the instant of an event */

#ifndef SBF_EXTEVENTPVTCARTESIAN_1_2__PADDING_LENGTH 
#define SBF_EXTEVENTPVTCARTESIAN_1_2__PADDING_LENGTH 1 
#endif

#define SBF_EXTEVENTPVTCARTESIAN_1_2_TOW_PRECISION 3
#define SBF_EXTEVENTPVTCARTESIAN_1_2_X_PRECISION 3
#define SBF_EXTEVENTPVTCARTESIAN_1_2_Y_PRECISION 3
#define SBF_EXTEVENTPVTCARTESIAN_1_2_Z_PRECISION 3
#define SBF_EXTEVENTPVTCARTESIAN_1_2_UNDULATION_PRECISION 3
#define SBF_EXTEVENTPVTCARTESIAN_1_2_VX_PRECISION 3
#define SBF_EXTEVENTPVTCARTESIAN_1_2_VY_PRECISION 3
#define SBF_EXTEVENTPVTCARTESIAN_1_2_VZ_PRECISION 3
#define SBF_EXTEVENTPVTCARTESIAN_1_2_COG_PRECISION 2
#define SBF_EXTEVENTPVTCARTESIAN_1_2_RXCLKBIAS_PRECISION 6
#define SBF_EXTEVENTPVTCARTESIAN_1_2_RXCLKDRIFT_PRECISION 3
#define SBF_EXTEVENTPVTCARTESIAN_1_2_HACCURACY_PRECISION 2
#define SBF_EXTEVENTPVTCARTESIAN_1_2_VACCURACY_PRECISION 2
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        Mode;         
  uint8_t        Error;        
  SBFDOUBLE      X;            
  SBFDOUBLE      Y;            
  SBFDOUBLE      Z;            
  float          Undulation;   
  float          Vx;           
  float          Vy;           
  float          Vz;           
  float          COG;          
  SBFDOUBLE      RxClkBias;    
  float          RxClkDrift;   
  uint8_t        TimeSystem;   
  uint8_t        Datum;        
  uint8_t        NrSV;         
  uint8_t        WACorrInfo;   
  uint16_t       ReferenceId;  
  uint16_t       MeanCorrAge;  
  uint32_t       SignalInfo;   
  uint8_t        AlertFlag;    
  uint8_t        NrBases;      
  uint16_t       PPPInfo;      
  uint16_t       Latency;      
  uint16_t       HAccuracy;    
  uint16_t       VAccuracy;    
  uint8_t        Misc;         

  uint8_t        _padding[SBF_EXTEVENTPVTCARTESIAN_1_2__PADDING_LENGTH];
} ExtEventPVTCartesian_1_2_t;

#define SBF_EXTEVENTPVTCARTESIAN_1__PADDING_LENGTH SBF_EXTEVENTPVTCARTESIAN_1_2__PADDING_LENGTH
typedef ExtEventPVTCartesian_1_2_t ExtEventPVTCartesian_1_t;

/*--ExtEventPVTGeodetic_1_0_t : ---------------------------------------------*/
/* Geodetic position at the instant of an event */

#ifndef SBF_EXTEVENTPVTGEODETIC_1_0__PADDING_LENGTH 
#define SBF_EXTEVENTPVTGEODETIC_1_0__PADDING_LENGTH 3 
#endif

#define SBF_EXTEVENTPVTGEODETIC_1_0_TOW_PRECISION 3
#define SBF_EXTEVENTPVTGEODETIC_1_0_LAT_PRECISION 12
#define SBF_EXTEVENTPVTGEODETIC_1_0_LON_PRECISION 12
#define SBF_EXTEVENTPVTGEODETIC_1_0_ALT_PRECISION 3
#define SBF_EXTEVENTPVTGEODETIC_1_0_UNDULATION_PRECISION 3
#define SBF_EXTEVENTPVTGEODETIC_1_0_VN_PRECISION 3
#define SBF_EXTEVENTPVTGEODETIC_1_0_VE_PRECISION 3
#define SBF_EXTEVENTPVTGEODETIC_1_0_VU_PRECISION 3
#define SBF_EXTEVENTPVTGEODETIC_1_0_COG_PRECISION 2
#define SBF_EXTEVENTPVTGEODETIC_1_0_RXCLKBIAS_PRECISION 6
#define SBF_EXTEVENTPVTGEODETIC_1_0_RXCLKDRIFT_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        Mode;         
  uint8_t        Error;        
  SBFDOUBLE      Lat;          
  SBFDOUBLE      Lon;          
  SBFDOUBLE      Alt;          
  float          Undulation;   
  float          Vn;           
  float          Ve;           
  float          Vu;           
  float          COG;          
  SBFDOUBLE      RxClkBias;    
  float          RxClkDrift;   
  uint8_t        TimeSystem;   
  uint8_t        Datum;        
  uint8_t        NrSV;         
  uint8_t        WACorrInfo;   
  uint16_t       ReferenceId;  
  uint16_t       MeanCorrAge;  
  uint32_t       SignalInfo;   
  uint8_t        AlertFlag;    

  uint8_t        _padding[SBF_EXTEVENTPVTGEODETIC_1_0__PADDING_LENGTH];
} ExtEventPVTGeodetic_1_0_t;

/*--ExtEventPVTGeodetic_1_1_t : ---------------------------------------------*/
/* Geodetic position at the instant of an event */

#define SBF_EXTEVENTPVTGEODETIC_1_1_TOW_PRECISION 3
#define SBF_EXTEVENTPVTGEODETIC_1_1_LAT_PRECISION 12
#define SBF_EXTEVENTPVTGEODETIC_1_1_LON_PRECISION 12
#define SBF_EXTEVENTPVTGEODETIC_1_1_ALT_PRECISION 3
#define SBF_EXTEVENTPVTGEODETIC_1_1_UNDULATION_PRECISION 3
#define SBF_EXTEVENTPVTGEODETIC_1_1_VN_PRECISION 3
#define SBF_EXTEVENTPVTGEODETIC_1_1_VE_PRECISION 3
#define SBF_EXTEVENTPVTGEODETIC_1_1_VU_PRECISION 3
#define SBF_EXTEVENTPVTGEODETIC_1_1_COG_PRECISION 2
#define SBF_EXTEVENTPVTGEODETIC_1_1_RXCLKBIAS_PRECISION 6
#define SBF_EXTEVENTPVTGEODETIC_1_1_RXCLKDRIFT_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        Mode;         
  uint8_t        Error;        
  SBFDOUBLE      Lat;          
  SBFDOUBLE      Lon;          
  SBFDOUBLE      Alt;          
  float          Undulation;   
  float          Vn;           
  float          Ve;           
  float          Vu;           
  float          COG;          
  SBFDOUBLE      RxClkBias;    
  float          RxClkDrift;   
  uint8_t        TimeSystem;   
  uint8_t        Datum;        
  uint8_t        NrSV;         
  uint8_t        WACorrInfo;   
  uint16_t       ReferenceId;  
  uint16_t       MeanCorrAge;  
  uint32_t       SignalInfo;   
  uint8_t        AlertFlag;    
  uint8_t        NrBases;      
  uint16_t       PPPInfo;      
} ExtEventPVTGeodetic_1_1_t;

/*--ExtEventPVTGeodetic_1_2_t : ---------------------------------------------*/
/* Geodetic position at the instant of an event */

#ifndef SBF_EXTEVENTPVTGEODETIC_1_2__PADDING_LENGTH 
#define SBF_EXTEVENTPVTGEODETIC_1_2__PADDING_LENGTH 1 
#endif

#define SBF_EXTEVENTPVTGEODETIC_1_2_TOW_PRECISION 3
#define SBF_EXTEVENTPVTGEODETIC_1_2_LAT_PRECISION 12
#define SBF_EXTEVENTPVTGEODETIC_1_2_LON_PRECISION 12
#define SBF_EXTEVENTPVTGEODETIC_1_2_ALT_PRECISION 3
#define SBF_EXTEVENTPVTGEODETIC_1_2_UNDULATION_PRECISION 3
#define SBF_EXTEVENTPVTGEODETIC_1_2_VN_PRECISION 3
#define SBF_EXTEVENTPVTGEODETIC_1_2_VE_PRECISION 3
#define SBF_EXTEVENTPVTGEODETIC_1_2_VU_PRECISION 3
#define SBF_EXTEVENTPVTGEODETIC_1_2_COG_PRECISION 2
#define SBF_EXTEVENTPVTGEODETIC_1_2_RXCLKBIAS_PRECISION 6
#define SBF_EXTEVENTPVTGEODETIC_1_2_RXCLKDRIFT_PRECISION 3
#define SBF_EXTEVENTPVTGEODETIC_1_2_HACCURACY_PRECISION 2
#define SBF_EXTEVENTPVTGEODETIC_1_2_VACCURACY_PRECISION 2
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        Mode;         
  uint8_t        Error;        
  SBFDOUBLE      Lat;          
  SBFDOUBLE      Lon;          
  SBFDOUBLE      Alt;          
  float          Undulation;   
  float          Vn;           
  float          Ve;           
  float          Vu;           
  float          COG;          
  SBFDOUBLE      RxClkBias;    
  float          RxClkDrift;   
  uint8_t        TimeSystem;   
  uint8_t        Datum;        
  uint8_t        NrSV;         
  uint8_t        WACorrInfo;   
  uint16_t       ReferenceId;  
  uint16_t       MeanCorrAge;  
  uint32_t       SignalInfo;   
  uint8_t        AlertFlag;    
  uint8_t        NrBases;      
  uint16_t       PPPInfo;      
  uint16_t       Latency;      
  uint16_t       HAccuracy;    
  uint16_t       VAccuracy;    
  uint8_t        Misc;         

  uint8_t        _padding[SBF_EXTEVENTPVTGEODETIC_1_2__PADDING_LENGTH];
} ExtEventPVTGeodetic_1_2_t;

#define SBF_EXTEVENTPVTGEODETIC_1__PADDING_LENGTH SBF_EXTEVENTPVTGEODETIC_1_2__PADDING_LENGTH
typedef ExtEventPVTGeodetic_1_2_t ExtEventPVTGeodetic_1_t;

/*--ExtEventBaseVectCart_1_0_t : --------------------------------------------*/
/* XYZ relative position with respect to base(s) at the instant of an event */

#define SBF_EXTEVENTVECTORINFOCART_1_0_DELTAX_PRECISION 3
#define SBF_EXTEVENTVECTORINFOCART_1_0_DELTAY_PRECISION 3
#define SBF_EXTEVENTVECTORINFOCART_1_0_DELTAZ_PRECISION 3
#define SBF_EXTEVENTVECTORINFOCART_1_0_DELTAVX_PRECISION 3
#define SBF_EXTEVENTVECTORINFOCART_1_0_DELTAVY_PRECISION 3
#define SBF_EXTEVENTVECTORINFOCART_1_0_DELTAVZ_PRECISION 3
#define SBF_EXTEVENTVECTORINFOCART_1_0_AZIMUTH_PRECISION 2
#define SBF_EXTEVENTVECTORINFOCART_1_0_ELEVATION_PRECISION 2
typedef struct
{
  uint8_t        NrSV;         
  uint8_t        Error;        
  uint8_t        Mode;         
  uint8_t        Misc;         
  SBFDOUBLE      DeltaX;       
  SBFDOUBLE      DeltaY;       
  SBFDOUBLE      DeltaZ;       
  float          DeltaVx;      
  float          DeltaVy;      
  float          DeltaVz;      
  uint16_t       Azimuth;      
  int16_t        Elevation;    
  uint16_t       ReferenceID;  
  uint16_t       CorrAge;      
  uint32_t       SignalInfo;   
} ExtEventVectorInfoCart_1_0_t;

#ifndef SBF_EXTEVENTBASEVECTCART_1_0_EXTEVENTVECTORINFOCART_LENGTH 
#define SBF_EXTEVENTBASEVECTCART_1_0_EXTEVENTVECTORINFOCART_LENGTH 30 
#endif

#define SBF_EXTEVENTBASEVECTCART_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        N;            
  uint8_t        SBLength;     
  ExtEventVectorInfoCart_1_0_t ExtEventVectorInfoCart[SBF_EXTEVENTBASEVECTCART_1_0_EXTEVENTVECTORINFOCART_LENGTH];
} ExtEventBaseVectCart_1_0_t;

typedef ExtEventVectorInfoCart_1_0_t ExtEventVectorInfoCart_1_t;

#define SBF_EXTEVENTBASEVECTCART_1_EXTEVENTVECTORINFOCART_LENGTH SBF_EXTEVENTBASEVECTCART_1_0_EXTEVENTVECTORINFOCART_LENGTH
typedef ExtEventBaseVectCart_1_0_t ExtEventBaseVectCart_1_t;

/*--ExtEventBaseVectGeod_1_0_t : --------------------------------------------*/
/* ENU relative position with respect to base(s) at the instant of an event */

#define SBF_EXTEVENTVECTORINFOGEOD_1_0_DELTAEAST_PRECISION 3
#define SBF_EXTEVENTVECTORINFOGEOD_1_0_DELTANORTH_PRECISION 3
#define SBF_EXTEVENTVECTORINFOGEOD_1_0_DELTAUP_PRECISION 3
#define SBF_EXTEVENTVECTORINFOGEOD_1_0_DELTAVE_PRECISION 3
#define SBF_EXTEVENTVECTORINFOGEOD_1_0_DELTAVN_PRECISION 3
#define SBF_EXTEVENTVECTORINFOGEOD_1_0_DELTAVU_PRECISION 3
#define SBF_EXTEVENTVECTORINFOGEOD_1_0_AZIMUTH_PRECISION 2
#define SBF_EXTEVENTVECTORINFOGEOD_1_0_ELEVATION_PRECISION 2
typedef struct
{
  uint8_t        NrSV;         
  uint8_t        Error;        
  uint8_t        Mode;         
  uint8_t        Misc;         
  SBFDOUBLE      DeltaEast;    
  SBFDOUBLE      DeltaNorth;   
  SBFDOUBLE      DeltaUp;      
  float          DeltaVe;      
  float          DeltaVn;      
  float          DeltaVu;      
  uint16_t       Azimuth;      
  int16_t        Elevation;    
  uint16_t       ReferenceID;  
  uint16_t       CorrAge;      
  uint32_t       SignalInfo;   
} ExtEventVectorInfoGeod_1_0_t;

#ifndef SBF_EXTEVENTBASEVECTGEOD_1_0_EXTEVENTVECTORINFOGEOD_LENGTH 
#define SBF_EXTEVENTBASEVECTGEOD_1_0_EXTEVENTVECTORINFOGEOD_LENGTH 30 
#endif

#define SBF_EXTEVENTBASEVECTGEOD_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        N;            
  uint8_t        SBLength;     
  ExtEventVectorInfoGeod_1_0_t ExtEventVectorInfoGeod[SBF_EXTEVENTBASEVECTGEOD_1_0_EXTEVENTVECTORINFOGEOD_LENGTH];
} ExtEventBaseVectGeod_1_0_t;

typedef ExtEventVectorInfoGeod_1_0_t ExtEventVectorInfoGeod_1_t;

#define SBF_EXTEVENTBASEVECTGEOD_1_EXTEVENTVECTORINFOGEOD_LENGTH SBF_EXTEVENTBASEVECTGEOD_1_0_EXTEVENTVECTORINFOGEOD_LENGTH
typedef ExtEventBaseVectGeod_1_0_t ExtEventBaseVectGeod_1_t;

/*--ExtEventINSNavCart_1_0_t : ----------------------------------------------*/
/* INS solution in Cartesian coordinates at the instant of an event */

#define SBF_EXTEVENTINSNAVCARTPOSSTDDEV_1_0_XSTDDEV_PRECISION 3
#define SBF_EXTEVENTINSNAVCARTPOSSTDDEV_1_0_YSTDDEV_PRECISION 3
#define SBF_EXTEVENTINSNAVCARTPOSSTDDEV_1_0_ZSTDDEV_PRECISION 3
typedef struct
{
  float          XStdDev;      
  float          YStdDev;      
  float          ZStdDev;      
} ExtEventINSNavCartPosStdDev_1_0_t;

#define SBF_EXTEVENTINSNAVCARTATT_1_0_HEADING_PRECISION 6
#define SBF_EXTEVENTINSNAVCARTATT_1_0_PITCH_PRECISION 6
#define SBF_EXTEVENTINSNAVCARTATT_1_0_ROLL_PRECISION 6
typedef struct
{
  float          Heading;      
  float          Pitch;        
  float          Roll;         
} ExtEventINSNavCartAtt_1_0_t;

#define SBF_EXTEVENTINSNAVCARTATTSTDDEV_1_0_HEADINGSTDDEV_PRECISION 6
#define SBF_EXTEVENTINSNAVCARTATTSTDDEV_1_0_PITCHSTDDEV_PRECISION 6
#define SBF_EXTEVENTINSNAVCARTATTSTDDEV_1_0_ROLLSTDDEV_PRECISION 6
typedef struct
{
  float          HeadingStdDev;
  float          PitchStdDev;  
  float          RollStdDev;   
} ExtEventINSNavCartAttStdDev_1_0_t;

#define SBF_EXTEVENTINSNAVCARTVEL_1_0_VX_PRECISION 3
#define SBF_EXTEVENTINSNAVCARTVEL_1_0_VY_PRECISION 3
#define SBF_EXTEVENTINSNAVCARTVEL_1_0_VZ_PRECISION 3
typedef struct
{
  float          Vx;           
  float          Vy;           
  float          Vz;           
} ExtEventINSNavCartVel_1_0_t;

#define SBF_EXTEVENTINSNAVCARTVELSTDDEV_1_0_VXSTDDEV_PRECISION 3
#define SBF_EXTEVENTINSNAVCARTVELSTDDEV_1_0_VYSTDDEV_PRECISION 3
#define SBF_EXTEVENTINSNAVCARTVELSTDDEV_1_0_VZSTDDEV_PRECISION 3
typedef struct
{
  float          VxStdDev;     
  float          VyStdDev;     
  float          VzStdDev;     
} ExtEventINSNavCartVelStdDev_1_0_t;

typedef union
{
  ExtEventINSNavCartPosStdDev_1_0_t PosStdDev;    
  ExtEventINSNavCartAtt_1_0_t Att;          
  ExtEventINSNavCartAttStdDev_1_0_t AttStdDev;    
  ExtEventINSNavCartVel_1_0_t Vel;          
  ExtEventINSNavCartVelStdDev_1_0_t VelStdDev;    
} ExtEventINSNavCartData_1_0_t;

#ifndef SBF_EXTEVENTINSNAVCART_1_0_EXTEVENTINSNAVCARTDATA_LENGTH 
#define SBF_EXTEVENTINSNAVCART_1_0_EXTEVENTINSNAVCARTDATA_LENGTH 16 
#endif

#define SBF_EXTEVENTINSNAVCART_1_0_TOW_PRECISION 3
#define SBF_EXTEVENTINSNAVCART_1_0_X_PRECISION 3
#define SBF_EXTEVENTINSNAVCART_1_0_Y_PRECISION 3
#define SBF_EXTEVENTINSNAVCART_1_0_Z_PRECISION 3
#define SBF_EXTEVENTINSNAVCART_1_0_ACCURACY_PRECISION 2
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        GNSSMode;     
  uint8_t        Error;        
  uint16_t       Info;         
  uint16_t       GNSSAge;      
  SBFDOUBLE      X;            
  SBFDOUBLE      Y;            
  SBFDOUBLE      Z;            
  uint16_t       Accuracy;     
  uint16_t       Reserved2;    
  uint8_t        Datum;        
  uint8_t        Reserved;     
  uint16_t       SBList;       
  ExtEventINSNavCartData_1_0_t ExtEventINSNavCartData[SBF_EXTEVENTINSNAVCART_1_0_EXTEVENTINSNAVCARTDATA_LENGTH];
} ExtEventINSNavCart_1_0_t;

typedef ExtEventINSNavCartPosStdDev_1_0_t ExtEventINSNavCartPosStdDev_1_t;

typedef ExtEventINSNavCartAtt_1_0_t ExtEventINSNavCartAtt_1_t;

typedef ExtEventINSNavCartAttStdDev_1_0_t ExtEventINSNavCartAttStdDev_1_t;

typedef ExtEventINSNavCartVel_1_0_t ExtEventINSNavCartVel_1_t;

typedef ExtEventINSNavCartVelStdDev_1_0_t ExtEventINSNavCartVelStdDev_1_t;

typedef ExtEventINSNavCartData_1_0_t ExtEventINSNavCartData_1_t;

#define SBF_EXTEVENTINSNAVCART_1_EXTEVENTINSNAVCARTDATA_LENGTH SBF_EXTEVENTINSNAVCART_1_0_EXTEVENTINSNAVCARTDATA_LENGTH
typedef ExtEventINSNavCart_1_0_t ExtEventINSNavCart_1_t;

/*--ExtEventINSNavGeod_1_0_t : ----------------------------------------------*/
/* INS solution in Geodetic coordinates at the instant of an event */

#define SBF_EXTEVENTINSNAVGEODPOSSTDDEV_1_0_LATITUDESTDDEV_PRECISION 3
#define SBF_EXTEVENTINSNAVGEODPOSSTDDEV_1_0_LONGITUDESTDDEV_PRECISION 3
#define SBF_EXTEVENTINSNAVGEODPOSSTDDEV_1_0_HEIGHTSTDDEV_PRECISION 3
typedef struct
{
  float          LatitudeStdDev;
  float          LongitudeStdDev;
  float          HeightStdDev; 
} ExtEventINSNavGeodPosStdDev_1_0_t;

#define SBF_EXTEVENTINSNAVGEODATT_1_0_HEADING_PRECISION 6
#define SBF_EXTEVENTINSNAVGEODATT_1_0_PITCH_PRECISION 6
#define SBF_EXTEVENTINSNAVGEODATT_1_0_ROLL_PRECISION 6
typedef struct
{
  float          Heading;      
  float          Pitch;        
  float          Roll;         
} ExtEventINSNavGeodAtt_1_0_t;

#define SBF_EXTEVENTINSNAVGEODATTSTDDEV_1_0_HEADINGSTDDEV_PRECISION 6
#define SBF_EXTEVENTINSNAVGEODATTSTDDEV_1_0_PITCHSTDDEV_PRECISION 6
#define SBF_EXTEVENTINSNAVGEODATTSTDDEV_1_0_ROLLSTDDEV_PRECISION 6
typedef struct
{
  float          HeadingStdDev;
  float          PitchStdDev;  
  float          RollStdDev;   
} ExtEventINSNavGeodAttStdDev_1_0_t;

#define SBF_EXTEVENTINSNAVGEODVEL_1_0_VE_PRECISION 3
#define SBF_EXTEVENTINSNAVGEODVEL_1_0_VN_PRECISION 3
#define SBF_EXTEVENTINSNAVGEODVEL_1_0_VU_PRECISION 3
typedef struct
{
  float          Ve;           
  float          Vn;           
  float          Vu;           
} ExtEventINSNavGeodVel_1_0_t;

#define SBF_EXTEVENTINSNAVGEODVELSTDDEV_1_0_VESTDDEV_PRECISION 3
#define SBF_EXTEVENTINSNAVGEODVELSTDDEV_1_0_VNSTDDEV_PRECISION 3
#define SBF_EXTEVENTINSNAVGEODVELSTDDEV_1_0_VUSTDDEV_PRECISION 3
typedef struct
{
  float          VeStdDev;     
  float          VnStdDev;     
  float          VuStdDev;     
} ExtEventINSNavGeodVelStdDev_1_0_t;

typedef union
{
  ExtEventINSNavGeodPosStdDev_1_0_t PosStdDev;    
  ExtEventINSNavGeodAtt_1_0_t Att;          
  ExtEventINSNavGeodAttStdDev_1_0_t AttStdDev;    
  ExtEventINSNavGeodVel_1_0_t Vel;          
  ExtEventINSNavGeodVelStdDev_1_0_t VelStdDev;    
} ExtEventINSNavGeodData_1_0_t;

#ifndef SBF_EXTEVENTINSNAVGEOD_1_0_EXTEVENTINSNAVGEODDATA_LENGTH 
#define SBF_EXTEVENTINSNAVGEOD_1_0_EXTEVENTINSNAVGEODDATA_LENGTH 16 
#endif

#define SBF_EXTEVENTINSNAVGEOD_1_0_TOW_PRECISION 3
#define SBF_EXTEVENTINSNAVGEOD_1_0_LATITUDE_PRECISION 12
#define SBF_EXTEVENTINSNAVGEOD_1_0_LONGITUDE_PRECISION 12
#define SBF_EXTEVENTINSNAVGEOD_1_0_HEIGHT_PRECISION 3
#define SBF_EXTEVENTINSNAVGEOD_1_0_UNDULATION_PRECISION 3
#define SBF_EXTEVENTINSNAVGEOD_1_0_ACCURACY_PRECISION 2
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        GNSSMode;     
  uint8_t        Error;        
  uint16_t       Info;         
  uint16_t       GNSSAge;      
  SBFDOUBLE      Latitude;     
  SBFDOUBLE      Longitude;    
  SBFDOUBLE      Height;       
  float          Undulation;   
  uint16_t       Accuracy;     
  uint16_t       Reserved2;    
  uint8_t        Datum;        
  uint8_t        Reserved;     
  uint16_t       SBList;       
  ExtEventINSNavGeodData_1_0_t ExtEventINSNavGeodData[SBF_EXTEVENTINSNAVGEOD_1_0_EXTEVENTINSNAVGEODDATA_LENGTH];
} ExtEventINSNavGeod_1_0_t;

typedef ExtEventINSNavGeodPosStdDev_1_0_t ExtEventINSNavGeodPosStdDev_1_t;

typedef ExtEventINSNavGeodAtt_1_0_t ExtEventINSNavGeodAtt_1_t;

typedef ExtEventINSNavGeodAttStdDev_1_0_t ExtEventINSNavGeodAttStdDev_1_t;

typedef ExtEventINSNavGeodVel_1_0_t ExtEventINSNavGeodVel_1_t;

typedef ExtEventINSNavGeodVelStdDev_1_0_t ExtEventINSNavGeodVelStdDev_1_t;

typedef ExtEventINSNavGeodData_1_0_t ExtEventINSNavGeodData_1_t;

#define SBF_EXTEVENTINSNAVGEOD_1_EXTEVENTINSNAVGEODDATA_LENGTH SBF_EXTEVENTINSNAVGEOD_1_0_EXTEVENTINSNAVGEODDATA_LENGTH
typedef ExtEventINSNavGeod_1_0_t ExtEventINSNavGeod_1_t;


/*==Differential Correction Blocks===========================================*/

/*--DiffCorrIn_1_0_t : ------------------------------------------------------*/
/* Incoming RTCM or  CMR message */

#ifndef SBF_DIFFCORRIN_1_0_FRAME_LENGTH 
#define SBF_DIFFCORRIN_1_0_FRAME_LENGTH 4096 
#endif

#define SBF_DIFFCORRIN_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        Mode;         
  uint8_t        Source;       
  uint8_t        Frame[SBF_DIFFCORRIN_1_0_FRAME_LENGTH];
} DiffCorrIn_1_0_t;

#define SBF_DIFFCORRIN_1_FRAME_LENGTH SBF_DIFFCORRIN_1_0_FRAME_LENGTH
typedef DiffCorrIn_1_0_t DiffCorrIn_1_t;

/*--BaseStation_1_0_t : -----------------------------------------------------*/
/* Base station coordinates */

#define SBF_BASESTATION_1_0_TOW_PRECISION 3
#define SBF_BASESTATION_1_0_X_L1PHASECENTER_PRECISION 3
#define SBF_BASESTATION_1_0_Y_L1PHASECENTER_PRECISION 3
#define SBF_BASESTATION_1_0_Z_L1PHASECENTER_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint16_t       BaseStationID;
  uint8_t        BaseType;     
  uint8_t        Source;       
  uint8_t        Datum;        
  uint8_t        Reserved;     
  SBFDOUBLE      X_L1PhaseCenter;
  SBFDOUBLE      Y_L1PhaseCenter;
  SBFDOUBLE      Z_L1PhaseCenter;
} BaseStation_1_0_t;

typedef BaseStation_1_0_t BaseStation_1_t;

/*--RTCMDatum_1_0_t : -------------------------------------------------------*/
/* Datum information from the RTK service provider */

#ifndef SBF_RTCMDATUM_1_0_SOURCECRS_LENGTH 
#define SBF_RTCMDATUM_1_0_SOURCECRS_LENGTH 32 
#endif

#ifndef SBF_RTCMDATUM_1_0_TARGETCRS_LENGTH 
#define SBF_RTCMDATUM_1_0_TARGETCRS_LENGTH 32 
#endif

#define SBF_RTCMDATUM_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  char           SourceCRS[SBF_RTCMDATUM_1_0_SOURCECRS_LENGTH];
  char           TargetCRS[SBF_RTCMDATUM_1_0_TARGETCRS_LENGTH];
  uint8_t        Datum;        
  uint8_t        HeightType;   
  uint8_t        QualityInd;   
} RTCMDatum_1_0_t;

#define SBF_RTCMDATUM_1_SOURCECRS_LENGTH SBF_RTCMDATUM_1_0_SOURCECRS_LENGTH
#define SBF_RTCMDATUM_1_TARGETCRS_LENGTH SBF_RTCMDATUM_1_0_TARGETCRS_LENGTH
typedef RTCMDatum_1_0_t RTCMDatum_1_t;

/*--BaseLink_1_0_t : --------------------------------------------------------*/

#define SBF_BASELINK_1_0_TOW_PRECISION 3
#define SBF_BASELINK_1_0_AGEOFLASTMSG_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        CorrAvailable;
  uint8_t        Reserved;     
  uint32_t       NrBytesReceived;
  uint32_t       NrBytesAccepted;
  uint32_t       NrMessagesReceived;
  uint32_t       NrMessagesAccepted;
  float          AgeOfLastMsg; 
} BaseLink_1_0_t;

typedef BaseLink_1_0_t BaseLink_1_t;


/*==L-Band Demodulator Blocks================================================*/

/*--LBandReceiverStatus_1_0_t : ---------------------------------------------*/

#define SBF_LBANDRECEIVERSTATUS_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        CPULoad;      
  uint8_t        Reserved;     
  uint32_t       UpTime;       
  uint32_t       RxStatus;     
  uint32_t       RxError;      
} LBandReceiverStatus_1_0_t;

typedef LBandReceiverStatus_1_0_t LBandReceiverStatus_1_t;

/*--LBandTrackerStatus_1_0_t : ----------------------------------------------*/
/* Status of the L-band signal tracking */

#define SBF_TRACKDATA_1_0_FREQOFFSET_PRECISION 3
#define SBF_TRACKDATA_1_0_CN0_PRECISION 2
typedef struct
{
  uint32_t       Frequency;    
  uint16_t       BaudRate;     
  uint16_t       ServiceID;    
  float          FreqOffset;   
  uint16_t       CN0;          
  int16_t        AvgPower;     
  int8_t         AGCGain;      
  uint8_t        Mode;         
  uint8_t        Status;       
  uint8_t        Reserved;     /* Reserved for future use */
} TrackData_1_0_t;

#ifndef SBF_LBANDTRACKERSTATUS_1_0_TRACKDATA_LENGTH 
#define SBF_LBANDTRACKERSTATUS_1_0_TRACKDATA_LENGTH 2 
#endif

#define SBF_LBANDTRACKERSTATUS_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        N;            
  uint8_t        SBSize;       
  TrackData_1_0_t TrackData[SBF_LBANDTRACKERSTATUS_1_0_TRACKDATA_LENGTH];
} LBandTrackerStatus_1_0_t;

/*--LBandTrackerStatus_1_1_t : ----------------------------------------------*/
/* Status of the L-band signal tracking */

#ifndef SBF_TRACKDATA_1_1__PADDING_LENGTH 
#define SBF_TRACKDATA_1_1__PADDING_LENGTH 2 
#endif

#define SBF_TRACKDATA_1_1_FREQOFFSET_PRECISION 3
#define SBF_TRACKDATA_1_1_CN0_PRECISION 2
typedef struct
{
  uint32_t       Frequency;    
  uint16_t       BaudRate;     
  uint16_t       ServiceID;    
  float          FreqOffset;   
  uint16_t       CN0;          
  int16_t        AvgPower;     
  int8_t         AGCGain;      
  uint8_t        Mode;         
  uint8_t        Status;       
  uint8_t        Reserved;     /* Reserved for future use */
  uint16_t       LockTime;     

  uint8_t        _padding[SBF_TRACKDATA_1_1__PADDING_LENGTH];
} TrackData_1_1_t;

#ifndef SBF_LBANDTRACKERSTATUS_1_1_TRACKDATA_LENGTH 
#define SBF_LBANDTRACKERSTATUS_1_1_TRACKDATA_LENGTH 2 
#endif

#define SBF_LBANDTRACKERSTATUS_1_1_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        N;            
  uint8_t        SBSize;       
  TrackData_1_1_t TrackData[SBF_LBANDTRACKERSTATUS_1_1_TRACKDATA_LENGTH];
} LBandTrackerStatus_1_1_t;

/*--LBandTrackerStatus_1_2_t : ----------------------------------------------*/
/* Status of the L-band signal tracking */

#ifndef SBF_TRACKDATA_1_2__PADDING_LENGTH 
#define SBF_TRACKDATA_1_2__PADDING_LENGTH 2 
#endif

#define SBF_TRACKDATA_1_2_FREQOFFSET_PRECISION 3
#define SBF_TRACKDATA_1_2_CN0_PRECISION 2
typedef struct
{
  uint32_t       Frequency;    
  uint16_t       BaudRate;     
  uint16_t       ServiceID;    
  float          FreqOffset;   
  uint16_t       CN0;          
  int16_t        AvgPower;     
  int8_t         AGCGain;      
  uint8_t        Mode;         
  uint8_t        Status;       
  uint8_t        SVID;         
  uint16_t       LockTime;     

  uint8_t        _padding[SBF_TRACKDATA_1_2__PADDING_LENGTH];
} TrackData_1_2_t;

#ifndef SBF_LBANDTRACKERSTATUS_1_2_TRACKDATA_LENGTH 
#define SBF_LBANDTRACKERSTATUS_1_2_TRACKDATA_LENGTH 2 
#endif

#define SBF_LBANDTRACKERSTATUS_1_2_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        N;            
  uint8_t        SBSize;       
  TrackData_1_2_t TrackData[SBF_LBANDTRACKERSTATUS_1_2_TRACKDATA_LENGTH];
} LBandTrackerStatus_1_2_t;

/*--LBandTrackerStatus_1_3_t : ----------------------------------------------*/
/* Status of the L-band signal tracking */

#ifndef SBF_TRACKDATA_1_3__PADDING_LENGTH 
#define SBF_TRACKDATA_1_3__PADDING_LENGTH 1 
#endif

#define SBF_TRACKDATA_1_3_FREQOFFSET_PRECISION 3
#define SBF_TRACKDATA_1_3_CN0_PRECISION 2
typedef struct
{
  uint32_t       Frequency;    
  uint16_t       BaudRate;     
  uint16_t       ServiceID;    
  float          FreqOffset;   
  uint16_t       CN0;          
  int16_t        AvgPower;     
  int8_t         AGCGain;      
  uint8_t        Mode;         
  uint8_t        Status;       
  uint8_t        SVID;         
  uint16_t       LockTime;     
  uint8_t        Source;       

  uint8_t        _padding[SBF_TRACKDATA_1_3__PADDING_LENGTH];
} TrackData_1_3_t;

#ifndef SBF_LBANDTRACKERSTATUS_1_3_TRACKDATA_LENGTH 
#define SBF_LBANDTRACKERSTATUS_1_3_TRACKDATA_LENGTH 2 
#endif

#define SBF_LBANDTRACKERSTATUS_1_3_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        N;            
  uint8_t        SBSize;       
  TrackData_1_3_t TrackData[SBF_LBANDTRACKERSTATUS_1_3_TRACKDATA_LENGTH];
} LBandTrackerStatus_1_3_t;

#define SBF_TRACKDATA_1__PADDING_LENGTH SBF_TRACKDATA_1_3__PADDING_LENGTH
typedef TrackData_1_3_t TrackData_1_t;

#define SBF_LBANDTRACKERSTATUS_1_TRACKDATA_LENGTH SBF_LBANDTRACKERSTATUS_1_3_TRACKDATA_LENGTH
typedef LBandTrackerStatus_1_3_t LBandTrackerStatus_1_t;

/*--LBAS1DecoderStatus_1_0_t : ----------------------------------------------*/
/* Status of the LBAS1 L-band service */

#define SBF_LBAS1DECODERSTATUS_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        Reserved[2];  
  uint8_t        Status;       
  uint8_t        Access;       
  uint8_t        GeoGatingMode;
  uint8_t        GeoGatingStatus;
  uint32_t       Event;        
} LBAS1DecoderStatus_1_0_t;

/*--LBAS1DecoderStatus_1_1_t : ----------------------------------------------*/
/* Status of the LBAS1 L-band service */

#ifndef SBF_LBAS1DECODERSTATUS_1_1_PAC_LENGTH 
#define SBF_LBAS1DECODERSTATUS_1_1_PAC_LENGTH 20 
#endif

#define SBF_LBAS1DECODERSTATUS_1_1_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        Reserved[2];  
  uint8_t        Status;       
  uint8_t        Access;       
  uint8_t        GeoGatingMode;
  uint8_t        GeoGatingStatus;
  uint32_t       Event;        
  uint32_t       LeaseTime;    
  uint32_t       LeaseRemaining;
  int32_t        LocalAreaLat; 
  int32_t        LocalAreaLon; 
  uint16_t       LocalAreaRadius;
  uint8_t        LocalAreaStatus;
  uint8_t        Reserved1;    
  int8_t         SubscrEndYear;
  int8_t         SubscrEndMonth;
  int8_t         SubscrEndDay; 
  int8_t         SubscrEndHour;
  char           PAC[SBF_LBAS1DECODERSTATUS_1_1_PAC_LENGTH];
} LBAS1DecoderStatus_1_1_t;

/*--LBAS1DecoderStatus_1_2_t : ----------------------------------------------*/
/* Status of the LBAS1 L-band service */

#ifndef SBF_LBAS1DECODERSTATUS_1_2_PAC_LENGTH 
#define SBF_LBAS1DECODERSTATUS_1_2_PAC_LENGTH 20 
#endif

#ifndef SBF_LBAS1DECODERSTATUS_1_2__PADDING_LENGTH 
#define SBF_LBAS1DECODERSTATUS_1_2__PADDING_LENGTH 2 
#endif

#define SBF_LBAS1DECODERSTATUS_1_2_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        Reserved[2];  
  uint8_t        Status;       
  uint8_t        Access;       
  uint8_t        GeoGatingMode;
  uint8_t        GeoGatingStatus;
  uint32_t       Event;        
  uint32_t       LeaseTime;    
  uint32_t       LeaseRemaining;
  int32_t        LocalAreaLat; 
  int32_t        LocalAreaLon; 
  uint16_t       LocalAreaRadius;
  uint8_t        LocalAreaStatus;
  uint8_t        Reserved1;    
  int8_t         SubscrEndYear;
  int8_t         SubscrEndMonth;
  int8_t         SubscrEndDay; 
  int8_t         SubscrEndHour;
  char           PAC[SBF_LBAS1DECODERSTATUS_1_2_PAC_LENGTH];
  uint8_t        VelocityLimit;
  uint8_t        SpeedGatingStatus;

  uint8_t        _padding[SBF_LBAS1DECODERSTATUS_1_2__PADDING_LENGTH];
} LBAS1DecoderStatus_1_2_t;

#define SBF_LBAS1DECODERSTATUS_1_PAC_LENGTH SBF_LBAS1DECODERSTATUS_1_2_PAC_LENGTH
#define SBF_LBAS1DECODERSTATUS_1__PADDING_LENGTH SBF_LBAS1DECODERSTATUS_1_2__PADDING_LENGTH
typedef LBAS1DecoderStatus_1_2_t LBAS1DecoderStatus_1_t;

/*--LBAS1Messages_1_0_t : ---------------------------------------------------*/
/* LBAS1over-the-air message */

#ifndef SBF_LBAS1MESSAGES_1_0_MESSAGE_LENGTH 
#define SBF_LBAS1MESSAGES_1_0_MESSAGE_LENGTH 512 
#endif

#define SBF_LBAS1MESSAGES_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint16_t       MessageLen;   
  char           Message[SBF_LBAS1MESSAGES_1_0_MESSAGE_LENGTH];
} LBAS1Messages_1_0_t;

#define SBF_LBAS1MESSAGES_1_MESSAGE_LENGTH SBF_LBAS1MESSAGES_1_0_MESSAGE_LENGTH
typedef LBAS1Messages_1_0_t LBAS1Messages_1_t;

/*--LBandBeams_1_0_t : ------------------------------------------------------*/
/* L-band satellite/beam information */

#ifndef SBF_BEAMINFO_1_0_SATNAME_LENGTH 
#define SBF_BEAMINFO_1_0_SATNAME_LENGTH 9 
#endif

typedef struct
{
  uint8_t        SVID;         
  char           SatName[SBF_BEAMINFO_1_0_SATNAME_LENGTH];
  int16_t        SatLongitude; 
  uint32_t       BeamFreq;     
} BeamInfo_1_0_t;

#ifndef SBF_LBANDBEAMS_1_0_BEAMDATA_LENGTH 
#define SBF_LBANDBEAMS_1_0_BEAMDATA_LENGTH 13 
#endif

#define SBF_LBANDBEAMS_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        N;            
  uint8_t        SBLength;     
  BeamInfo_1_0_t BeamData[SBF_LBANDBEAMS_1_0_BEAMDATA_LENGTH];
} LBandBeams_1_0_t;

#define SBF_BEAMINFO_1_SATNAME_LENGTH SBF_BEAMINFO_1_0_SATNAME_LENGTH
typedef BeamInfo_1_0_t BeamInfo_1_t;

#define SBF_LBANDBEAMS_1_BEAMDATA_LENGTH SBF_LBANDBEAMS_1_0_BEAMDATA_LENGTH
typedef LBandBeams_1_0_t LBandBeams_1_t;

/*--LBandRaw_1_0_t : --------------------------------------------------------*/
/* L-Band raw user data */

#ifndef SBF_LBANDRAW_1_0_USERDATA_LENGTH 
#define SBF_LBANDRAW_1_0_USERDATA_LENGTH 2048 
#endif

#define SBF_LBANDRAW_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint16_t       N;            
  uint32_t       Frequency;    
  uint8_t        UserData[SBF_LBANDRAW_1_0_USERDATA_LENGTH];
} LBandRaw_1_0_t;

#define SBF_LBANDRAW_1_USERDATA_LENGTH SBF_LBANDRAW_1_0_USERDATA_LENGTH
typedef LBandRaw_1_0_t LBandRaw_1_t;

/*--FugroStatus_1_0_t : -----------------------------------------------------*/
/* Fugro Status Information */

#define SBF_FUGROSTATUS_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        Reserved[2];  
  uint32_t       Status;       
  int32_t        SubStartingTime;
  int32_t        SubExpirationTime;
  int32_t        SubHourGlass; 
  uint32_t       SubscribedMode;
  uint32_t       SubCurrentMode;
  uint32_t       SubLinkVector;
  uint32_t       CRCGoodCount; 
  uint32_t       CRCBadCount;  
} FugroStatus_1_0_t;

typedef FugroStatus_1_0_t FugroStatus_1_t;


/*==External Sensor Blocks===================================================*/

/*--ExtSensorMeas_1_0_t : ---------------------------------------------------*/
/* Measurement set of external sensors of one epoch */

#define SBF_EXTSENSORMEASACCELERATION_1_0_ACCELERATIONX_PRECISION 3
#define SBF_EXTSENSORMEASACCELERATION_1_0_ACCELERATIONY_PRECISION 3
#define SBF_EXTSENSORMEASACCELERATION_1_0_ACCELERATIONZ_PRECISION 3
typedef struct
{
  SBFDOUBLE      AccelerationX;
  SBFDOUBLE      AccelerationY;
  SBFDOUBLE      AccelerationZ;
} ExtSensorMeasAcceleration_1_0_t;

#define SBF_EXTSENSORMEASANGULARRATE_1_0_ANGULARRATEX_PRECISION 3
#define SBF_EXTSENSORMEASANGULARRATE_1_0_ANGULARRATEY_PRECISION 3
#define SBF_EXTSENSORMEASANGULARRATE_1_0_ANGULARRATEZ_PRECISION 3
typedef struct
{
  SBFDOUBLE      AngularRateX; 
  SBFDOUBLE      AngularRateY; 
  SBFDOUBLE      AngularRateZ; 
} ExtSensorMeasAngularRate_1_0_t;

typedef struct
{
  int16_t        SensorTemperature;
  uint8_t        Reserved[22]; 
} ExtSensorMeasInfo_1_0_t;

typedef struct
{
  SBFDOUBLE      ZeroVelocityFlag;
  uint8_t        Reserved[16]; 
} ExtSensorMeasZeroVelocityFlag_1_0_t;

typedef union
{
  ExtSensorMeasAcceleration_1_0_t Acceleration; 
  ExtSensorMeasAngularRate_1_0_t AngularRate;  
  ExtSensorMeasInfo_1_0_t Info;         
  ExtSensorMeasZeroVelocityFlag_1_0_t ZeroVelocityFlag;
} ExtSensorMeasData_1_0_t;

typedef struct
{
  uint8_t        Source;       
  uint8_t        SensorModel;  
  uint8_t        Type;         
  uint8_t        ObsInfo;      
  ExtSensorMeasData_1_0_t ExtSensorMeasData;
} ExtSensorMeasSet_1_0_t;

#ifndef SBF_EXTSENSORMEAS_1_0_EXTSENSORMEAS_LENGTH 
#define SBF_EXTSENSORMEAS_1_0_EXTSENSORMEAS_LENGTH 4 
#endif

#define SBF_EXTSENSORMEAS_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        N;            
  uint8_t        SBSize;       
  ExtSensorMeasSet_1_0_t ExtSensorMeas[SBF_EXTSENSORMEAS_1_0_EXTSENSORMEAS_LENGTH];
} ExtSensorMeas_1_0_t;

typedef ExtSensorMeasAcceleration_1_0_t ExtSensorMeasAcceleration_1_t;

typedef ExtSensorMeasAngularRate_1_0_t ExtSensorMeasAngularRate_1_t;

typedef ExtSensorMeasInfo_1_0_t ExtSensorMeasInfo_1_t;

typedef ExtSensorMeasZeroVelocityFlag_1_0_t ExtSensorMeasZeroVelocityFlag_1_t;

typedef ExtSensorMeasData_1_0_t ExtSensorMeasData_1_t;

typedef ExtSensorMeasSet_1_0_t ExtSensorMeasSet_1_t;

#define SBF_EXTSENSORMEAS_1_EXTSENSORMEAS_LENGTH SBF_EXTSENSORMEAS_1_0_EXTSENSORMEAS_LENGTH
typedef ExtSensorMeas_1_0_t ExtSensorMeas_1_t;

/*--ExtSensorStatus_1_0_t : -------------------------------------------------*/
/* Overall status of external sensors */

#ifndef SBF_EXTSENSORSTATUS_1_0_STATUSBITS_LENGTH 
#define SBF_EXTSENSORSTATUS_1_0_STATUSBITS_LENGTH 60 
#endif

#define SBF_EXTSENSORSTATUS_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        Source;       
  uint8_t        SensorModel;  
  uint8_t        StatusType;   
  uint8_t        Reserved[3];  
  uint8_t        StatusBits[SBF_EXTSENSORSTATUS_1_0_STATUSBITS_LENGTH];
} ExtSensorStatus_1_0_t;

#define SBF_EXTSENSORSTATUS_1_STATUSBITS_LENGTH SBF_EXTSENSORSTATUS_1_0_STATUSBITS_LENGTH
typedef ExtSensorStatus_1_0_t ExtSensorStatus_1_t;

/*--ExtSensorSetup_1_0_t : --------------------------------------------------*/
/* General information about the setup of external sensors */

typedef struct
{
  uint8_t        Source;       
  uint8_t        SensorModel;  
  uint16_t       MeasTypes;    
} OneSensor_1_0_t;

#ifndef SBF_EXTSENSORSETUP_1_0_EXTSENSORSETUP_LENGTH 
#define SBF_EXTSENSORSETUP_1_0_EXTSENSORSETUP_LENGTH 4 
#endif

#define SBF_EXTSENSORSETUP_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        N;            
  uint8_t        SBLength;     
  OneSensor_1_0_t ExtSensorSetup[SBF_EXTSENSORSETUP_1_0_EXTSENSORSETUP_LENGTH];
} ExtSensorSetup_1_0_t;

/*--ExtSensorSetup_1_1_t : --------------------------------------------------*/
/* General information about the setup of external sensors */

#ifndef SBF_ONESENSOR_1_1__PADDING_LENGTH 
#define SBF_ONESENSOR_1_1__PADDING_LENGTH 2 
#endif

typedef struct
{
  uint8_t        Source;       
  uint8_t        SensorModel;  
  uint16_t       MeasTypes;    
  uint8_t        LeverArmSource;
  uint8_t        Reserved1;    

  uint8_t        _padding[SBF_ONESENSOR_1_1__PADDING_LENGTH];
} OneSensor_1_1_t;

#ifndef SBF_EXTSENSORSETUP_1_1_EXTSENSORSETUP_LENGTH 
#define SBF_EXTSENSORSETUP_1_1_EXTSENSORSETUP_LENGTH 4 
#endif

#define SBF_EXTSENSORSETUP_1_1_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        N;            
  uint8_t        SBLength;     
  OneSensor_1_1_t ExtSensorSetup[SBF_EXTSENSORSETUP_1_1_EXTSENSORSETUP_LENGTH];
} ExtSensorSetup_1_1_t;

/*--ExtSensorSetup_1_2_t : --------------------------------------------------*/
/* General information about the setup of external sensors */

typedef struct
{
  uint8_t        Source;       
  uint8_t        SensorModel;  
  uint16_t       MeasTypes;    
  uint8_t        LeverArmSource;
  uint8_t        Reserved1;    
  int16_t        LeverArmX;    
  int16_t        LeverArmY;    
  int16_t        LeverArmZ;    
  uint16_t       LeverArmStdX; 
  uint16_t       LeverArmStdY; 
  uint16_t       LeverArmStdZ; 
  uint16_t       Reserved2;    
} OneSensor_1_2_t;

#ifndef SBF_EXTSENSORSETUP_1_2_EXTSENSORSETUP_LENGTH 
#define SBF_EXTSENSORSETUP_1_2_EXTSENSORSETUP_LENGTH 4 
#endif

#define SBF_EXTSENSORSETUP_1_2_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        N;            
  uint8_t        SBLength;     
  OneSensor_1_2_t ExtSensorSetup[SBF_EXTSENSORSETUP_1_2_EXTSENSORSETUP_LENGTH];
} ExtSensorSetup_1_2_t;

typedef OneSensor_1_2_t OneSensor_1_t;

#define SBF_EXTSENSORSETUP_1_EXTSENSORSETUP_LENGTH SBF_EXTSENSORSETUP_1_2_EXTSENSORSETUP_LENGTH
typedef ExtSensorSetup_1_2_t ExtSensorSetup_1_t;

/*--ExtSensorStatus_2_0_t : -------------------------------------------------*/
/* Overall status of external sensors */

#ifndef SBF_EXTSENSORSTATUS_2_0_DATA_LENGTH 
#define SBF_EXTSENSORSTATUS_2_0_DATA_LENGTH 4 
#endif

#define SBF_EXTSENSORSTATUS_2_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        Source;       
  uint8_t        SensorModel;  
  uint8_t        Data[SBF_EXTSENSORSTATUS_2_0_DATA_LENGTH];
} ExtSensorStatus_2_0_t;

#define SBF_EXTSENSORSTATUS_2_DATA_LENGTH SBF_EXTSENSORSTATUS_2_0_DATA_LENGTH
typedef ExtSensorStatus_2_0_t ExtSensorStatus_2_t;

/*--ExtSensorInfo_1_0_t : ---------------------------------------------------*/
/* Configuration information of external sensors */

#ifndef SBF_EXTSENSORINFO_1_0_DATA_LENGTH 
#define SBF_EXTSENSORINFO_1_0_DATA_LENGTH 60 
#endif

#define SBF_EXTSENSORINFO_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        Source;       
  uint8_t        SensorModel;  
  uint8_t        Data[SBF_EXTSENSORINFO_1_0_DATA_LENGTH];
} ExtSensorInfo_1_0_t;

#define SBF_EXTSENSORINFO_1_DATA_LENGTH SBF_EXTSENSORINFO_1_0_DATA_LENGTH
typedef ExtSensorInfo_1_0_t ExtSensorInfo_1_t;

/*--IMUSetup_1_0_t : --------------------------------------------------------*/
/* General information about the setup of the IMU */

#define SBF_IMUSETUP_1_0_TOW_PRECISION 3
#define SBF_IMUSETUP_1_0_ANTLEVERARMX_PRECISION 3
#define SBF_IMUSETUP_1_0_ANTLEVERARMY_PRECISION 3
#define SBF_IMUSETUP_1_0_ANTLEVERARMZ_PRECISION 3
#define SBF_IMUSETUP_1_0_THETAX_PRECISION 2
#define SBF_IMUSETUP_1_0_THETAY_PRECISION 2
#define SBF_IMUSETUP_1_0_THETAZ_PRECISION 2
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        Reserved;     
  uint8_t        SerialPort;   
  float          AntLeverArmX; 
  float          AntLeverArmY; 
  float          AntLeverArmZ; 
  float          ThetaX;       
  float          ThetaY;       
  float          ThetaZ;       
} IMUSetup_1_0_t;

typedef IMUSetup_1_0_t IMUSetup_1_t;


/*==Status Blocks============================================================*/

/*--ReceiverStatus_1_0_t : --------------------------------------------------*/
/* Overall status information of the receiver */

typedef struct
{
  uint8_t        L1Magn;       
  uint8_t        L2Magn;       
  uint8_t        AGCSettings;  
  uint8_t        AttenuatorSettings;
} AGCData_1_0_t;

#define SBF_RECEIVERSTATUS_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        CPULoad;      
  uint8_t        Reserved;     
  uint32_t       UpTime;       
  uint32_t       RxStatus;     
  AGCData_1_0_t  AGCData;      
} ReceiverStatus_1_0_t;

typedef AGCData_1_0_t AGCData_1_t;

typedef ReceiverStatus_1_0_t ReceiverStatus_1_t;

/*--TrackingStatus_1_0_t : --------------------------------------------------*/
/* Status of the tracking for all receiver channels */

#ifndef SBF_TRACKINGSTATUSCHANNEL_1_0__PADDING_LENGTH 
#define SBF_TRACKINGSTATUSCHANNEL_1_0__PADDING_LENGTH 3 
#endif

typedef struct
{
  uint8_t        RxChannel;    
  uint8_t        SVID;         
  uint8_t        AttitudeStatus;
  uint8_t        Status;       
  int16_t        Azimuth;      
  int8_t         Elevation;    
  uint8_t        Health;       
  int8_t         ElevChange;   

  uint8_t        _padding[SBF_TRACKINGSTATUSCHANNEL_1_0__PADDING_LENGTH];
} TrackingStatusChannel_1_0_t;

#ifndef SBF_TRACKINGSTATUS_1_0_CHANNELDATA_LENGTH 
#define SBF_TRACKINGSTATUS_1_0_CHANNELDATA_LENGTH 72 
#endif

#define SBF_TRACKINGSTATUS_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        N;            
  uint8_t        SBSize;       
  TrackingStatusChannel_1_0_t ChannelData[SBF_TRACKINGSTATUS_1_0_CHANNELDATA_LENGTH];
} TrackingStatus_1_0_t;

#define SBF_TRACKINGSTATUSCHANNEL_1__PADDING_LENGTH SBF_TRACKINGSTATUSCHANNEL_1_0__PADDING_LENGTH
typedef TrackingStatusChannel_1_0_t TrackingStatusChannel_1_t;

#define SBF_TRACKINGSTATUS_1_CHANNELDATA_LENGTH SBF_TRACKINGSTATUS_1_0_CHANNELDATA_LENGTH
typedef TrackingStatus_1_0_t TrackingStatus_1_t;

/*--ChannelStatus_1_0_t : ---------------------------------------------------*/
/* Status of the tracking for all receiver channels */

typedef struct
{
  uint8_t        Antenna;      
  uint8_t        ReservedA;    
  uint16_t       TrackingStatus;
  uint16_t       PVTStatus;    
  uint16_t       PVTInfo;      
} ChannelStateInfo_1_0_t;

typedef struct
{
  uint8_t        SVID;         
  uint8_t        FreqNr;       
  uint8_t        Reserved1[2]; 
  uint16_t       Az_RiseSet;   
  uint16_t       HealthStatus; 
  int8_t         Elev;         
  uint8_t        N2;           
  uint8_t        Channel;      
  uint8_t        Reserved2;    
} ChannelSatInfo_1_0_t;

#ifndef SBF_CHANNELSTATUS_1_0_DATA_LENGTH 
#define SBF_CHANNELSTATUS_1_0_DATA_LENGTH -1 
#endif

#define SBF_CHANNELSTATUS_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        N;            
  uint8_t        SB1Size;      
  uint8_t        SB2Size;      
  uint8_t        Reserved[3];  
  uint8_t        Data[SBF_CHANNELSTATUS_1_0_DATA_LENGTH];
} ChannelStatus_1_0_t;

typedef ChannelStateInfo_1_0_t ChannelStateInfo_1_t;

typedef ChannelSatInfo_1_0_t ChannelSatInfo_1_t;

#define SBF_CHANNELSTATUS_1_DATA_LENGTH SBF_CHANNELSTATUS_1_0_DATA_LENGTH
typedef ChannelStatus_1_0_t ChannelStatus_1_t;

/*--ReceiverStatus_2_0_t : --------------------------------------------------*/
/* Overall status information of the receiver */

typedef struct
{
  uint8_t        FrontendID;   
  int8_t         Gain;         
  uint8_t        SampleVar;    
  uint8_t        BlankingStat; 
} AGCState_2_0_t;

#ifndef SBF_RECEIVERSTATUS_2_0_AGCSTATE_LENGTH 
#define SBF_RECEIVERSTATUS_2_0_AGCSTATE_LENGTH 18 
#endif

#define SBF_RECEIVERSTATUS_2_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        CPULoad;      
  uint8_t        ExtError;     
  uint32_t       UpTime;       
  uint32_t       RxStatus;     
  uint32_t       RxError;      
  uint8_t        N;            
  uint8_t        SBSize;       
  uint8_t        CmdCount;     
  uint8_t        Temperature;  
  AGCState_2_0_t AGCState[SBF_RECEIVERSTATUS_2_0_AGCSTATE_LENGTH];
} ReceiverStatus_2_0_t;

/*--ReceiverStatus_2_1_t : --------------------------------------------------*/
/* Overall status information of the receiver */

typedef struct
{
  uint8_t        FrontendID;   
  int8_t         Gain;         
  uint8_t        SampleVar;    
  uint8_t        BlankingStat; 
} AGCState_2_1_t;

#ifndef SBF_RECEIVERSTATUS_2_1_AGCSTATE_LENGTH 
#define SBF_RECEIVERSTATUS_2_1_AGCSTATE_LENGTH 18 
#endif

#define SBF_RECEIVERSTATUS_2_1_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        CPULoad;      
  uint8_t        ExtError;     
  uint32_t       UpTime;       
  uint32_t       RxStatus;     
  uint32_t       RxError;      
  uint8_t        N;            
  uint8_t        SBSize;       
  uint8_t        CmdCount;     
  uint8_t        Temperature;  
  AGCState_2_1_t AGCState[SBF_RECEIVERSTATUS_2_1_AGCSTATE_LENGTH];
} ReceiverStatus_2_1_t;

typedef AGCState_2_1_t AGCState_2_t;

#define SBF_RECEIVERSTATUS_2_AGCSTATE_LENGTH SBF_RECEIVERSTATUS_2_1_AGCSTATE_LENGTH
typedef ReceiverStatus_2_1_t ReceiverStatus_2_t;

/*--SatVisibility_1_0_t : ---------------------------------------------------*/
/* Azimuth/elevation of visible satellites */

#define SBF_SATINFO_1_0_AZIMUTH_PRECISION 2
#define SBF_SATINFO_1_0_ELEVATION_PRECISION 2
typedef struct
{
  uint8_t        SVID;         
  uint8_t        FreqNr;       
  uint16_t       Azimuth;      
  int16_t        Elevation;    
  uint8_t        RiseSet;      
  uint8_t        SatelliteInfo;
} SatInfo_1_0_t;

#ifndef SBF_SATVISIBILITY_1_0_SATINFO_LENGTH 
#define SBF_SATVISIBILITY_1_0_SATINFO_LENGTH 100 
#endif

#define SBF_SATVISIBILITY_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        N;            
  uint8_t        SBLength;     
  SatInfo_1_0_t  SatInfo[SBF_SATVISIBILITY_1_0_SATINFO_LENGTH];
} SatVisibility_1_0_t;

typedef SatInfo_1_0_t SatInfo_1_t;

#define SBF_SATVISIBILITY_1_SATINFO_LENGTH SBF_SATVISIBILITY_1_0_SATINFO_LENGTH
typedef SatVisibility_1_0_t SatVisibility_1_t;

/*--InputLink_1_0_t : -------------------------------------------------------*/
/* Statistics on input streams */

typedef struct
{
  uint8_t        CD;           
  uint8_t        Type;         
  uint16_t       AgeOfLastMessage;
  uint32_t       NrBytesReceived;
  uint32_t       NrBytesAccepted;
  uint32_t       NrMsgReceived;
  uint32_t       NrMsgAccepted;
} InputStatsSub_1_0_t;

#ifndef SBF_INPUTLINK_1_0_DATA_LENGTH 
#define SBF_INPUTLINK_1_0_DATA_LENGTH -1 
#endif

#define SBF_INPUTLINK_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        N;            
  uint8_t        SBLength;     
  uint8_t        Data[SBF_INPUTLINK_1_0_DATA_LENGTH];
} InputLink_1_0_t;

typedef InputStatsSub_1_0_t InputStatsSub_1_t;

#define SBF_INPUTLINK_1_DATA_LENGTH SBF_INPUTLINK_1_0_DATA_LENGTH
typedef InputLink_1_0_t InputLink_1_t;

/*--OutputLink_1_0_t : ------------------------------------------------------*/
/* Statistics on output streams */

#ifndef SBF_OUTPUTTYPESUB_1_0__PADDING_LENGTH 
#define SBF_OUTPUTTYPESUB_1_0__PADDING_LENGTH 2 
#endif

typedef struct
{
  uint8_t        Type;         
  uint8_t        Percentage;   

  uint8_t        _padding[SBF_OUTPUTTYPESUB_1_0__PADDING_LENGTH];
} OutputTypeSub_1_0_t;

typedef struct
{
  uint8_t        CD;           
  uint8_t        N2;           
  uint16_t       AllowedRate;  
  uint32_t       NrBytesProduced;
  uint32_t       NrBytesSent;  
} OutputStatsSub_1_0_t;

#ifndef SBF_OUTPUTLINK_1_0_DATA_LENGTH 
#define SBF_OUTPUTLINK_1_0_DATA_LENGTH -1 
#endif

#define SBF_OUTPUTLINK_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        N1;           
  uint8_t        SB1Length;    
  uint8_t        SB2Length;    
  uint8_t        Reserved[3];  
  uint8_t        Data[SBF_OUTPUTLINK_1_0_DATA_LENGTH];
} OutputLink_1_0_t;

/*--OutputLink_1_1_t : ------------------------------------------------------*/
/* Statistics on output streams */

typedef struct
{
  uint8_t        Type;         
  uint8_t        Percentage;   
} OutputTypeSub_1_1_t;

typedef struct
{
  uint8_t        CD;           
  uint8_t        N2;           
  uint16_t       AllowedRate;  
  uint32_t       NrBytesProduced;
  uint32_t       NrBytesSent;  
  uint8_t        NrClients;    
  uint8_t        Reserved[3];  
} OutputStatsSub_1_1_t;

#ifndef SBF_OUTPUTLINK_1_1_DATA_LENGTH 
#define SBF_OUTPUTLINK_1_1_DATA_LENGTH -1 
#endif

#define SBF_OUTPUTLINK_1_1_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        N1;           
  uint8_t        SB1Length;    
  uint8_t        SB2Length;    
  uint8_t        Reserved[3];  
  uint8_t        Data[SBF_OUTPUTLINK_1_1_DATA_LENGTH];
} OutputLink_1_1_t;

typedef OutputTypeSub_1_1_t OutputTypeSub_1_t;

typedef OutputStatsSub_1_1_t OutputStatsSub_1_t;

#define SBF_OUTPUTLINK_1_DATA_LENGTH SBF_OUTPUTLINK_1_1_DATA_LENGTH
typedef OutputLink_1_1_t OutputLink_1_t;

/*--NTRIPClientStatus_1_0_t : -----------------------------------------------*/
/* NTRIP client connection status */

#ifndef SBF_NTRIPCLIENTCONNECTION_1_0__PADDING_LENGTH 
#define SBF_NTRIPCLIENTCONNECTION_1_0__PADDING_LENGTH 1 
#endif

typedef struct
{
  uint8_t        CDIndex;      
  uint8_t        Status;       
  uint8_t        ErrorCode;    

  uint8_t        _padding[SBF_NTRIPCLIENTCONNECTION_1_0__PADDING_LENGTH];
} NTRIPClientConnection_1_0_t;

#ifndef SBF_NTRIPCLIENTSTATUS_1_0_NTRIPCLIENTCONNECTION_LENGTH 
#define SBF_NTRIPCLIENTSTATUS_1_0_NTRIPCLIENTCONNECTION_LENGTH 5 
#endif

#define SBF_NTRIPCLIENTSTATUS_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        N;            
  uint8_t        SBLength;     
  NTRIPClientConnection_1_0_t NTRIPClientConnection[SBF_NTRIPCLIENTSTATUS_1_0_NTRIPCLIENTCONNECTION_LENGTH];
} NTRIPClientStatus_1_0_t;

#define SBF_NTRIPCLIENTCONNECTION_1__PADDING_LENGTH SBF_NTRIPCLIENTCONNECTION_1_0__PADDING_LENGTH
typedef NTRIPClientConnection_1_0_t NTRIPClientConnection_1_t;

#define SBF_NTRIPCLIENTSTATUS_1_NTRIPCLIENTCONNECTION_LENGTH SBF_NTRIPCLIENTSTATUS_1_0_NTRIPCLIENTCONNECTION_LENGTH
typedef NTRIPClientStatus_1_0_t NTRIPClientStatus_1_t;

/*--IPStatus_1_0_t : --------------------------------------------------------*/
/* IP address, gateway and MAC address of Ethernet interface */

#ifndef SBF_IPSTATUS_1_0_MACADDRESS_LENGTH 
#define SBF_IPSTATUS_1_0_MACADDRESS_LENGTH 6 
#endif

#ifndef SBF_IPSTATUS_1_0_IPADDRESS_LENGTH 
#define SBF_IPSTATUS_1_0_IPADDRESS_LENGTH 16 
#endif

#ifndef SBF_IPSTATUS_1_0_GATEWAY_LENGTH 
#define SBF_IPSTATUS_1_0_GATEWAY_LENGTH 16 
#endif

#define SBF_IPSTATUS_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        MACAddress[SBF_IPSTATUS_1_0_MACADDRESS_LENGTH];
  uint8_t        IPAddress[SBF_IPSTATUS_1_0_IPADDRESS_LENGTH];
  uint8_t        Gateway[SBF_IPSTATUS_1_0_GATEWAY_LENGTH];
  uint8_t        Netmask;      
  uint8_t        Reserved[3];  
} IPStatus_1_0_t;

/*--IPStatus_1_1_t : --------------------------------------------------------*/
/* IP address, gateway and MAC address of Ethernet interface */

#ifndef SBF_IPSTATUS_1_1_MACADDRESS_LENGTH 
#define SBF_IPSTATUS_1_1_MACADDRESS_LENGTH 6 
#endif

#ifndef SBF_IPSTATUS_1_1_IPADDRESS_LENGTH 
#define SBF_IPSTATUS_1_1_IPADDRESS_LENGTH 16 
#endif

#ifndef SBF_IPSTATUS_1_1_GATEWAY_LENGTH 
#define SBF_IPSTATUS_1_1_GATEWAY_LENGTH 16 
#endif

#ifndef SBF_IPSTATUS_1_1_HOSTNAME_LENGTH 
#define SBF_IPSTATUS_1_1_HOSTNAME_LENGTH 32 
#endif

#define SBF_IPSTATUS_1_1_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        MACAddress[SBF_IPSTATUS_1_1_MACADDRESS_LENGTH];
  uint8_t        IPAddress[SBF_IPSTATUS_1_1_IPADDRESS_LENGTH];
  uint8_t        Gateway[SBF_IPSTATUS_1_1_GATEWAY_LENGTH];
  uint8_t        Netmask;      
  uint8_t        Reserved[3];  
  char           HostName[SBF_IPSTATUS_1_1_HOSTNAME_LENGTH];
} IPStatus_1_1_t;

#define SBF_IPSTATUS_1_MACADDRESS_LENGTH SBF_IPSTATUS_1_1_MACADDRESS_LENGTH
#define SBF_IPSTATUS_1_IPADDRESS_LENGTH SBF_IPSTATUS_1_1_IPADDRESS_LENGTH
#define SBF_IPSTATUS_1_GATEWAY_LENGTH SBF_IPSTATUS_1_1_GATEWAY_LENGTH
#define SBF_IPSTATUS_1_HOSTNAME_LENGTH SBF_IPSTATUS_1_1_HOSTNAME_LENGTH
typedef IPStatus_1_1_t IPStatus_1_t;

/*--WiFiAPStatus_1_0_t : ----------------------------------------------------*/
/* WiFi status in access point mode */

#ifndef SBF_WIFICLIENT_1_0_CLIENTHOSTNAME_LENGTH 
#define SBF_WIFICLIENT_1_0_CLIENTHOSTNAME_LENGTH 32 
#endif

#ifndef SBF_WIFICLIENT_1_0_CLIENTMACADDRESS_LENGTH 
#define SBF_WIFICLIENT_1_0_CLIENTMACADDRESS_LENGTH 6 
#endif

#ifndef SBF_WIFICLIENT_1_0_CLIENTIPADDRESS_LENGTH 
#define SBF_WIFICLIENT_1_0_CLIENTIPADDRESS_LENGTH 16 
#endif

#ifndef SBF_WIFICLIENT_1_0__PADDING_LENGTH 
#define SBF_WIFICLIENT_1_0__PADDING_LENGTH 2 
#endif

typedef struct
{
  char           ClientHostName[SBF_WIFICLIENT_1_0_CLIENTHOSTNAME_LENGTH];
  uint8_t        ClientMACAddress[SBF_WIFICLIENT_1_0_CLIENTMACADDRESS_LENGTH];
  uint8_t        ClientIPAddress[SBF_WIFICLIENT_1_0_CLIENTIPADDRESS_LENGTH];

  uint8_t        _padding[SBF_WIFICLIENT_1_0__PADDING_LENGTH];
} WiFiClient_1_0_t;

#ifndef SBF_WIFIAPSTATUS_1_0_APIPADDRESS_LENGTH 
#define SBF_WIFIAPSTATUS_1_0_APIPADDRESS_LENGTH 16 
#endif

#ifndef SBF_WIFIAPSTATUS_1_0_WIFICLIENT_LENGTH 
#define SBF_WIFIAPSTATUS_1_0_WIFICLIENT_LENGTH 20 
#endif

#define SBF_WIFIAPSTATUS_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        N;            
  uint8_t        SBLength;     
  uint8_t        APIPAddress[SBF_WIFIAPSTATUS_1_0_APIPADDRESS_LENGTH];
  uint8_t        Mode;         
  uint8_t        Hotspot;      
  uint8_t        Reserved[2];  
  WiFiClient_1_0_t WiFiClient[SBF_WIFIAPSTATUS_1_0_WIFICLIENT_LENGTH];
} WiFiAPStatus_1_0_t;

#define SBF_WIFICLIENT_1_CLIENTHOSTNAME_LENGTH SBF_WIFICLIENT_1_0_CLIENTHOSTNAME_LENGTH
#define SBF_WIFICLIENT_1_CLIENTMACADDRESS_LENGTH SBF_WIFICLIENT_1_0_CLIENTMACADDRESS_LENGTH
#define SBF_WIFICLIENT_1_CLIENTIPADDRESS_LENGTH SBF_WIFICLIENT_1_0_CLIENTIPADDRESS_LENGTH
#define SBF_WIFICLIENT_1__PADDING_LENGTH SBF_WIFICLIENT_1_0__PADDING_LENGTH
typedef WiFiClient_1_0_t WiFiClient_1_t;

#define SBF_WIFIAPSTATUS_1_APIPADDRESS_LENGTH SBF_WIFIAPSTATUS_1_0_APIPADDRESS_LENGTH
#define SBF_WIFIAPSTATUS_1_WIFICLIENT_LENGTH SBF_WIFIAPSTATUS_1_0_WIFICLIENT_LENGTH
typedef WiFiAPStatus_1_0_t WiFiAPStatus_1_t;

/*--WiFiClientStatus_1_0_t : ------------------------------------------------*/
/* WiFi status in client mode */

#ifndef SBF_WIFICLIENTSTATUS_1_0_SSID_AP_LENGTH 
#define SBF_WIFICLIENTSTATUS_1_0_SSID_AP_LENGTH 32 
#endif

#ifndef SBF_WIFICLIENTSTATUS_1_0_IPADDRESS_LENGTH 
#define SBF_WIFICLIENTSTATUS_1_0_IPADDRESS_LENGTH 16 
#endif

#ifndef SBF_WIFICLIENTSTATUS_1_0__PADDING_LENGTH 
#define SBF_WIFICLIENTSTATUS_1_0__PADDING_LENGTH 2 
#endif

#define SBF_WIFICLIENTSTATUS_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  char           SSID_AP[SBF_WIFICLIENTSTATUS_1_0_SSID_AP_LENGTH];
  uint8_t        IPAddress[SBF_WIFICLIENTSTATUS_1_0_IPADDRESS_LENGTH];
  uint8_t        Reserved[1];  
  int8_t         SigLevel;     
  uint8_t        Status;       
  uint8_t        ErrorCode;    

  uint8_t        _padding[SBF_WIFICLIENTSTATUS_1_0__PADDING_LENGTH];
} WiFiClientStatus_1_0_t;

#define SBF_WIFICLIENTSTATUS_1_SSID_AP_LENGTH SBF_WIFICLIENTSTATUS_1_0_SSID_AP_LENGTH
#define SBF_WIFICLIENTSTATUS_1_IPADDRESS_LENGTH SBF_WIFICLIENTSTATUS_1_0_IPADDRESS_LENGTH
#define SBF_WIFICLIENTSTATUS_1__PADDING_LENGTH SBF_WIFICLIENTSTATUS_1_0__PADDING_LENGTH
typedef WiFiClientStatus_1_0_t WiFiClientStatus_1_t;

/*--CellularStatus_1_0_t : --------------------------------------------------*/
/* Cellular status */

#ifndef SBF_CELLULARSTATUS_1_0_OPERATORNAME_LENGTH 
#define SBF_CELLULARSTATUS_1_0_OPERATORNAME_LENGTH 20 
#endif

#ifndef SBF_CELLULARSTATUS_1_0__PADDING_LENGTH 
#define SBF_CELLULARSTATUS_1_0__PADDING_LENGTH 2 
#endif

#define SBF_CELLULARSTATUS_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        ConnectionType;
  int8_t         RSSI;         
  char           OperatorName[SBF_CELLULARSTATUS_1_0_OPERATORNAME_LENGTH];
  uint8_t        Status;       
  uint8_t        ErrorCode;    

  uint8_t        _padding[SBF_CELLULARSTATUS_1_0__PADDING_LENGTH];
} CellularStatus_1_0_t;

/*--CellularStatus_1_1_t : --------------------------------------------------*/
/* Cellular status */

#ifndef SBF_CELLULARSTATUS_1_1_OPERATORNAME_LENGTH 
#define SBF_CELLULARSTATUS_1_1_OPERATORNAME_LENGTH 20 
#endif

#define SBF_CELLULARSTATUS_1_1_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        ConnectionType;
  int8_t         RSSI;         
  char           OperatorName[SBF_CELLULARSTATUS_1_1_OPERATORNAME_LENGTH];
  uint8_t        Status;       
  uint8_t        ErrorCode;    
  uint8_t        DataCall;     
  uint8_t        Info;         
} CellularStatus_1_1_t;

#define SBF_CELLULARSTATUS_1_OPERATORNAME_LENGTH SBF_CELLULARSTATUS_1_1_OPERATORNAME_LENGTH
typedef CellularStatus_1_1_t CellularStatus_1_t;

/*--BluetoothStatus_1_0_t : -------------------------------------------------*/
/* Bluetooth status */

#ifndef SBF_BTDEVICE_1_0_DEVICENAME_LENGTH 
#define SBF_BTDEVICE_1_0_DEVICENAME_LENGTH 30 
#endif

#ifndef SBF_BTDEVICE_1_0__PADDING_LENGTH 
#define SBF_BTDEVICE_1_0__PADDING_LENGTH 1 
#endif

typedef struct
{
  char           DeviceName[SBF_BTDEVICE_1_0_DEVICENAME_LENGTH];
  uint8_t        Flags;        

  uint8_t        _padding[SBF_BTDEVICE_1_0__PADDING_LENGTH];
} BTDevice_1_0_t;

#ifndef SBF_BLUETOOTHSTATUS_1_0_BTDEVICE_LENGTH 
#define SBF_BLUETOOTHSTATUS_1_0_BTDEVICE_LENGTH 20 
#endif

#define SBF_BLUETOOTHSTATUS_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        N;            
  uint8_t        SBLength;     
  uint8_t        Mode;         
  uint8_t        Reserved[3];  
  BTDevice_1_0_t BTDevice[SBF_BLUETOOTHSTATUS_1_0_BTDEVICE_LENGTH];
} BluetoothStatus_1_0_t;

#define SBF_BTDEVICE_1_DEVICENAME_LENGTH SBF_BTDEVICE_1_0_DEVICENAME_LENGTH
#define SBF_BTDEVICE_1__PADDING_LENGTH SBF_BTDEVICE_1_0__PADDING_LENGTH
typedef BTDevice_1_0_t BTDevice_1_t;

#define SBF_BLUETOOTHSTATUS_1_BTDEVICE_LENGTH SBF_BLUETOOTHSTATUS_1_0_BTDEVICE_LENGTH
typedef BluetoothStatus_1_0_t BluetoothStatus_1_t;

/*--DynDNSStatus_1_0_t : ----------------------------------------------------*/
/* DynDNS status */

#define SBF_DYNDNSSTATUS_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        Status;       
  uint8_t        ErrorCode;    
} DynDNSStatus_1_0_t;

/*--DynDNSStatus_1_1_t : ----------------------------------------------------*/
/* DynDNS status */

#ifndef SBF_DYNDNSSTATUS_1_1_IPADDRESS_LENGTH 
#define SBF_DYNDNSSTATUS_1_1_IPADDRESS_LENGTH 16 
#endif

#define SBF_DYNDNSSTATUS_1_1_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        Status;       
  uint8_t        ErrorCode;    
  uint8_t        IPAddress[SBF_DYNDNSSTATUS_1_1_IPADDRESS_LENGTH];
} DynDNSStatus_1_1_t;

#define SBF_DYNDNSSTATUS_1_IPADDRESS_LENGTH SBF_DYNDNSSTATUS_1_1_IPADDRESS_LENGTH
typedef DynDNSStatus_1_1_t DynDNSStatus_1_t;

/*--BatteryStatus_1_0_t : ---------------------------------------------------*/
/* Battery status */

typedef struct
{
  uint8_t        ChargeLevel;  
  uint8_t        Status;       
  uint16_t       RemainingTime;
} Battery_1_0_t;

#ifndef SBF_BATTERYSTATUS_1_0_BATTERY_LENGTH 
#define SBF_BATTERYSTATUS_1_0_BATTERY_LENGTH 4 
#endif

#define SBF_BATTERYSTATUS_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        N;            
  uint8_t        SBLength;     
  uint8_t        ExtSupply;    
  uint8_t        Reserved[3];  
  Battery_1_0_t  Battery[SBF_BATTERYSTATUS_1_0_BATTERY_LENGTH];
} BatteryStatus_1_0_t;

/*--BatteryStatus_1_1_t : ---------------------------------------------------*/
/* Battery status */

typedef struct
{
  uint8_t        ChargeLevel;  
  uint8_t        Status;       
  uint16_t       RemainingTime;
  uint16_t       Voltage;      
  int16_t        Current;      
} Battery_1_1_t;

#ifndef SBF_BATTERYSTATUS_1_1_BATTERY_LENGTH 
#define SBF_BATTERYSTATUS_1_1_BATTERY_LENGTH 4 
#endif

#define SBF_BATTERYSTATUS_1_1_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        N;            
  uint8_t        SBLength;     
  uint8_t        ExtSupply;    
  uint8_t        Reserved[3];  
  Battery_1_1_t  Battery[SBF_BATTERYSTATUS_1_1_BATTERY_LENGTH];
} BatteryStatus_1_1_t;

/*--BatteryStatus_1_2_t : ---------------------------------------------------*/
/* Battery status */

typedef struct
{
  uint8_t        ChargeLevel;  
  uint8_t        Status;       
  uint16_t       RemainingTime;
  uint16_t       Voltage;      
  int16_t        Current;      
  int8_t         Temperature;  
  uint8_t        Reserved[3];  
} Battery_1_2_t;

#ifndef SBF_BATTERYSTATUS_1_2_BATTERY_LENGTH 
#define SBF_BATTERYSTATUS_1_2_BATTERY_LENGTH 4 
#endif

#define SBF_BATTERYSTATUS_1_2_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        N;            
  uint8_t        SBLength;     
  uint8_t        ExtSupply;    
  uint8_t        Reserved[3];  
  Battery_1_2_t  Battery[SBF_BATTERYSTATUS_1_2_BATTERY_LENGTH];
} BatteryStatus_1_2_t;

typedef Battery_1_2_t Battery_1_t;

#define SBF_BATTERYSTATUS_1_BATTERY_LENGTH SBF_BATTERYSTATUS_1_2_BATTERY_LENGTH
typedef BatteryStatus_1_2_t BatteryStatus_1_t;

/*--PowerStatus_1_0_t : -----------------------------------------------------*/
/* Power supply source and voltage */

#define SBF_POWERSTATUS_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint16_t       PowerInfo;    
} PowerStatus_1_0_t;

typedef PowerStatus_1_0_t PowerStatus_1_t;

/*--QualityInd_1_0_t : ------------------------------------------------------*/
/* Quality indicators */

#ifndef SBF_QUALITYIND_1_0_INDICATORS_LENGTH 
#define SBF_QUALITYIND_1_0_INDICATORS_LENGTH 40 
#endif

#define SBF_QUALITYIND_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        N;            
  uint8_t        Reserved;     
  uint16_t       Indicators[SBF_QUALITYIND_1_0_INDICATORS_LENGTH];
} QualityInd_1_0_t;

#define SBF_QUALITYIND_1_INDICATORS_LENGTH SBF_QUALITYIND_1_0_INDICATORS_LENGTH
typedef QualityInd_1_0_t QualityInd_1_t;

/*--DiskStatus_1_0_t : ------------------------------------------------------*/
/* Internal logging status */

#ifndef SBF_DISKDATA_1_0__PADDING_LENGTH 
#define SBF_DISKDATA_1_0__PADDING_LENGTH 3 
#endif

typedef struct
{
  uint8_t        DiskID;       
  uint8_t        Status;       
  uint16_t       DiskUsageMSB; 
  uint32_t       DiskUsageLSB; 
  uint32_t       DiskSize;     
  uint8_t        CreateDeleteCount;

  uint8_t        _padding[SBF_DISKDATA_1_0__PADDING_LENGTH];
} DiskData_1_0_t;

#ifndef SBF_DISKSTATUS_1_0_DISKDATA_LENGTH 
#define SBF_DISKSTATUS_1_0_DISKDATA_LENGTH 2 
#endif

#define SBF_DISKSTATUS_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        N;            
  uint8_t        SBLength;     
  uint8_t        Reserved[4];  
  DiskData_1_0_t DiskData[SBF_DISKSTATUS_1_0_DISKDATA_LENGTH];
} DiskStatus_1_0_t;

/*--DiskStatus_1_1_t : ------------------------------------------------------*/
/* Internal logging status */

#ifndef SBF_DISKDATA_1_1__PADDING_LENGTH 
#define SBF_DISKDATA_1_1__PADDING_LENGTH 2 
#endif

typedef struct
{
  uint8_t        DiskID;       
  uint8_t        Status;       
  uint16_t       DiskUsageMSB; 
  uint32_t       DiskUsageLSB; 
  uint32_t       DiskSize;     
  uint8_t        CreateDeleteCount;
  uint8_t        Error;        

  uint8_t        _padding[SBF_DISKDATA_1_1__PADDING_LENGTH];
} DiskData_1_1_t;

#ifndef SBF_DISKSTATUS_1_1_DISKDATA_LENGTH 
#define SBF_DISKSTATUS_1_1_DISKDATA_LENGTH 2 
#endif

#define SBF_DISKSTATUS_1_1_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        N;            
  uint8_t        SBLength;     
  uint8_t        Reserved[4];  
  DiskData_1_1_t DiskData[SBF_DISKSTATUS_1_1_DISKDATA_LENGTH];
} DiskStatus_1_1_t;

#define SBF_DISKDATA_1__PADDING_LENGTH SBF_DISKDATA_1_1__PADDING_LENGTH
typedef DiskData_1_1_t DiskData_1_t;

#define SBF_DISKSTATUS_1_DISKDATA_LENGTH SBF_DISKSTATUS_1_1_DISKDATA_LENGTH
typedef DiskStatus_1_1_t DiskStatus_1_t;

/*--LogStatus_1_0_t : -------------------------------------------------------*/
/* Log sessions status */

typedef struct
{
  uint8_t        Type;         
  uint8_t        ErrorCode;    
  uint8_t        RetryQueueSize;
  uint8_t        NrFailedTransfers;
} FTPPushStatus_1_0_t;

#ifndef SBF_LOGSESSION_1_0_FTPPUSHSTATUS_LENGTH 
#define SBF_LOGSESSION_1_0_FTPPUSHSTATUS_LENGTH 6 
#endif

typedef struct
{
  uint8_t        SessionID;    
  uint8_t        SessionStatus;
  uint8_t        N2;           
  uint8_t        Reserved;     
  FTPPushStatus_1_0_t FTPPushStatus[SBF_LOGSESSION_1_0_FTPPUSHSTATUS_LENGTH];
} LogSession_1_0_t;

#ifndef SBF_LOGSTATUS_1_0_LOGSESSIONS_LENGTH 
#define SBF_LOGSTATUS_1_0_LOGSESSIONS_LENGTH 8 
#endif

#define SBF_LOGSTATUS_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        N1;           
  uint8_t        SB1Length;    
  uint8_t        SB2Length;    
  uint8_t        Reserved[3];  
  LogSession_1_0_t LogSessions[SBF_LOGSTATUS_1_0_LOGSESSIONS_LENGTH];
} LogStatus_1_0_t;

typedef FTPPushStatus_1_0_t FTPPushStatus_1_t;

#define SBF_LOGSESSION_1_FTPPUSHSTATUS_LENGTH SBF_LOGSESSION_1_0_FTPPUSHSTATUS_LENGTH
typedef LogSession_1_0_t LogSession_1_t;

#define SBF_LOGSTATUS_1_LOGSESSIONS_LENGTH SBF_LOGSTATUS_1_0_LOGSESSIONS_LENGTH
typedef LogStatus_1_0_t LogStatus_1_t;

/*--UHFStatus_1_0_t : -------------------------------------------------------*/
/* UHF status */

#ifndef SBF_UHFDATA_1_0__PADDING_LENGTH 
#define SBF_UHFDATA_1_0__PADDING_LENGTH 1 
#endif

typedef struct
{
  uint32_t       Frequency;    
  uint8_t        Channel;      
  uint8_t        Bandwidth;    
  int8_t         RSSI;         

  uint8_t        _padding[SBF_UHFDATA_1_0__PADDING_LENGTH];
} UHFData_1_0_t;

#ifndef SBF_UHFSTATUS_1_0_UHFDATA_LENGTH 
#define SBF_UHFSTATUS_1_0_UHFDATA_LENGTH 20 
#endif

#define SBF_UHFSTATUS_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        N;            
  uint8_t        SBLength;     
  uint8_t        UHFID;        
  uint8_t        Status;       
  uint8_t        ErrorCode;    
  uint8_t        Reserved;     
  UHFData_1_0_t  UHFData[SBF_UHFSTATUS_1_0_UHFDATA_LENGTH];
} UHFStatus_1_0_t;

#define SBF_UHFDATA_1__PADDING_LENGTH SBF_UHFDATA_1_0__PADDING_LENGTH
typedef UHFData_1_0_t UHFData_1_t;

#define SBF_UHFSTATUS_1_UHFDATA_LENGTH SBF_UHFSTATUS_1_0_UHFDATA_LENGTH
typedef UHFStatus_1_0_t UHFStatus_1_t;

/*--RFStatus_1_0_t : --------------------------------------------------------*/
/* Radio-frequency interference mitigation status */

#ifndef SBF_RFBAND_1_0__PADDING_LENGTH 
#define SBF_RFBAND_1_0__PADDING_LENGTH 1 
#endif

typedef struct
{
  uint32_t       Frequency;    
  uint16_t       Bandwidth;    
  uint8_t        Info;         

  uint8_t        _padding[SBF_RFBAND_1_0__PADDING_LENGTH];
} RFBand_1_0_t;

#ifndef SBF_RFSTATUS_1_0_RFBAND_LENGTH 
#define SBF_RFSTATUS_1_0_RFBAND_LENGTH 10 
#endif

#define SBF_RFSTATUS_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        N;            
  uint8_t        SBLength;     
  uint32_t       Reserved;     
  RFBand_1_0_t   RFBand[SBF_RFSTATUS_1_0_RFBAND_LENGTH];
} RFStatus_1_0_t;

#define SBF_RFBAND_1__PADDING_LENGTH SBF_RFBAND_1_0__PADDING_LENGTH
typedef RFBand_1_0_t RFBand_1_t;

#define SBF_RFSTATUS_1_RFBAND_LENGTH SBF_RFSTATUS_1_0_RFBAND_LENGTH
typedef RFStatus_1_0_t RFStatus_1_t;

/*--RIMSHealth_1_0_t : ------------------------------------------------------*/
/* Health status of the receiver */

#ifndef SBF_RIMSHEALTH_1_0__PADDING_LENGTH 
#define SBF_RIMSHEALTH_1_0__PADDING_LENGTH 1 
#endif

#define SBF_RIMSHEALTH_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        Reserved[2];  
  uint32_t       RxError;      
  uint32_t       Degraded;     
  uint16_t       RxInVoltage;  
  uint16_t       RxInCurrent;  
  uint16_t       AntInVoltage; 
  uint16_t       AntInCurrent; 
  int16_t        Temperature;  
  uint8_t        MemoryLoad;   

  uint8_t        _padding[SBF_RIMSHEALTH_1_0__PADDING_LENGTH];
} RIMSHealth_1_0_t;

#define SBF_RIMSHEALTH_1__PADDING_LENGTH SBF_RIMSHEALTH_1_0__PADDING_LENGTH
typedef RIMSHealth_1_0_t RIMSHealth_1_t;


/*==Miscellaneous Blocks=====================================================*/

/*--ReceiverSetup_1_0_t : ---------------------------------------------------*/
/* General information about the receiver installation */

#ifndef SBF_RECEIVERSETUP_1_0_MARKERNAME_LENGTH 
#define SBF_RECEIVERSETUP_1_0_MARKERNAME_LENGTH 60 
#endif

#ifndef SBF_RECEIVERSETUP_1_0_MARKERNUMBER_LENGTH 
#define SBF_RECEIVERSETUP_1_0_MARKERNUMBER_LENGTH 20 
#endif

#ifndef SBF_RECEIVERSETUP_1_0_OBSERVER_LENGTH 
#define SBF_RECEIVERSETUP_1_0_OBSERVER_LENGTH 20 
#endif

#ifndef SBF_RECEIVERSETUP_1_0_AGENCY_LENGTH 
#define SBF_RECEIVERSETUP_1_0_AGENCY_LENGTH 40 
#endif

#ifndef SBF_RECEIVERSETUP_1_0_RXSERIALNBR_LENGTH 
#define SBF_RECEIVERSETUP_1_0_RXSERIALNBR_LENGTH 20 
#endif

#ifndef SBF_RECEIVERSETUP_1_0_RXNAME_LENGTH 
#define SBF_RECEIVERSETUP_1_0_RXNAME_LENGTH 20 
#endif

#ifndef SBF_RECEIVERSETUP_1_0_RXVERSION_LENGTH 
#define SBF_RECEIVERSETUP_1_0_RXVERSION_LENGTH 20 
#endif

#ifndef SBF_RECEIVERSETUP_1_0_ANTSERIALNBR_LENGTH 
#define SBF_RECEIVERSETUP_1_0_ANTSERIALNBR_LENGTH 20 
#endif

#ifndef SBF_RECEIVERSETUP_1_0_ANTTYPE_LENGTH 
#define SBF_RECEIVERSETUP_1_0_ANTTYPE_LENGTH 20 
#endif

#define SBF_RECEIVERSETUP_1_0_TOW_PRECISION 3
#define SBF_RECEIVERSETUP_1_0_DELTAH_PRECISION 7
#define SBF_RECEIVERSETUP_1_0_DELTAE_PRECISION 7
#define SBF_RECEIVERSETUP_1_0_DELTAN_PRECISION 7
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        Reserved[2];  
  char           MarkerName[SBF_RECEIVERSETUP_1_0_MARKERNAME_LENGTH];
  char           MarkerNumber[SBF_RECEIVERSETUP_1_0_MARKERNUMBER_LENGTH];
  char           Observer[SBF_RECEIVERSETUP_1_0_OBSERVER_LENGTH];
  char           Agency[SBF_RECEIVERSETUP_1_0_AGENCY_LENGTH];
  char           RxSerialNbr[SBF_RECEIVERSETUP_1_0_RXSERIALNBR_LENGTH];
  char           RxName[SBF_RECEIVERSETUP_1_0_RXNAME_LENGTH];
  char           RxVersion[SBF_RECEIVERSETUP_1_0_RXVERSION_LENGTH];
  char           AntSerialNbr[SBF_RECEIVERSETUP_1_0_ANTSERIALNBR_LENGTH];
  char           AntType[SBF_RECEIVERSETUP_1_0_ANTTYPE_LENGTH];
  float          deltaH;       /* [m] */
  float          deltaE;       /* [m] */
  float          deltaN;       /* [m] */
} ReceiverSetup_1_0_t;

/*--ReceiverSetup_1_1_t : ---------------------------------------------------*/
/* General information about the receiver installation */

#ifndef SBF_RECEIVERSETUP_1_1_MARKERNAME_LENGTH 
#define SBF_RECEIVERSETUP_1_1_MARKERNAME_LENGTH 60 
#endif

#ifndef SBF_RECEIVERSETUP_1_1_MARKERNUMBER_LENGTH 
#define SBF_RECEIVERSETUP_1_1_MARKERNUMBER_LENGTH 20 
#endif

#ifndef SBF_RECEIVERSETUP_1_1_OBSERVER_LENGTH 
#define SBF_RECEIVERSETUP_1_1_OBSERVER_LENGTH 20 
#endif

#ifndef SBF_RECEIVERSETUP_1_1_AGENCY_LENGTH 
#define SBF_RECEIVERSETUP_1_1_AGENCY_LENGTH 40 
#endif

#ifndef SBF_RECEIVERSETUP_1_1_RXSERIALNBR_LENGTH 
#define SBF_RECEIVERSETUP_1_1_RXSERIALNBR_LENGTH 20 
#endif

#ifndef SBF_RECEIVERSETUP_1_1_RXNAME_LENGTH 
#define SBF_RECEIVERSETUP_1_1_RXNAME_LENGTH 20 
#endif

#ifndef SBF_RECEIVERSETUP_1_1_RXVERSION_LENGTH 
#define SBF_RECEIVERSETUP_1_1_RXVERSION_LENGTH 20 
#endif

#ifndef SBF_RECEIVERSETUP_1_1_ANTSERIALNBR_LENGTH 
#define SBF_RECEIVERSETUP_1_1_ANTSERIALNBR_LENGTH 20 
#endif

#ifndef SBF_RECEIVERSETUP_1_1_ANTTYPE_LENGTH 
#define SBF_RECEIVERSETUP_1_1_ANTTYPE_LENGTH 20 
#endif

#ifndef SBF_RECEIVERSETUP_1_1_MARKERTYPE_LENGTH 
#define SBF_RECEIVERSETUP_1_1_MARKERTYPE_LENGTH 20 
#endif

#define SBF_RECEIVERSETUP_1_1_TOW_PRECISION 3
#define SBF_RECEIVERSETUP_1_1_DELTAH_PRECISION 7
#define SBF_RECEIVERSETUP_1_1_DELTAE_PRECISION 7
#define SBF_RECEIVERSETUP_1_1_DELTAN_PRECISION 7
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        Reserved[2];  
  char           MarkerName[SBF_RECEIVERSETUP_1_1_MARKERNAME_LENGTH];
  char           MarkerNumber[SBF_RECEIVERSETUP_1_1_MARKERNUMBER_LENGTH];
  char           Observer[SBF_RECEIVERSETUP_1_1_OBSERVER_LENGTH];
  char           Agency[SBF_RECEIVERSETUP_1_1_AGENCY_LENGTH];
  char           RxSerialNbr[SBF_RECEIVERSETUP_1_1_RXSERIALNBR_LENGTH];
  char           RxName[SBF_RECEIVERSETUP_1_1_RXNAME_LENGTH];
  char           RxVersion[SBF_RECEIVERSETUP_1_1_RXVERSION_LENGTH];
  char           AntSerialNbr[SBF_RECEIVERSETUP_1_1_ANTSERIALNBR_LENGTH];
  char           AntType[SBF_RECEIVERSETUP_1_1_ANTTYPE_LENGTH];
  float          deltaH;       /* [m] */
  float          deltaE;       /* [m] */
  float          deltaN;       /* [m] */
  char           MarkerType[SBF_RECEIVERSETUP_1_1_MARKERTYPE_LENGTH];
} ReceiverSetup_1_1_t;

/*--ReceiverSetup_1_2_t : ---------------------------------------------------*/
/* General information about the receiver installation */

#ifndef SBF_RECEIVERSETUP_1_2_MARKERNAME_LENGTH 
#define SBF_RECEIVERSETUP_1_2_MARKERNAME_LENGTH 60 
#endif

#ifndef SBF_RECEIVERSETUP_1_2_MARKERNUMBER_LENGTH 
#define SBF_RECEIVERSETUP_1_2_MARKERNUMBER_LENGTH 20 
#endif

#ifndef SBF_RECEIVERSETUP_1_2_OBSERVER_LENGTH 
#define SBF_RECEIVERSETUP_1_2_OBSERVER_LENGTH 20 
#endif

#ifndef SBF_RECEIVERSETUP_1_2_AGENCY_LENGTH 
#define SBF_RECEIVERSETUP_1_2_AGENCY_LENGTH 40 
#endif

#ifndef SBF_RECEIVERSETUP_1_2_RXSERIALNBR_LENGTH 
#define SBF_RECEIVERSETUP_1_2_RXSERIALNBR_LENGTH 20 
#endif

#ifndef SBF_RECEIVERSETUP_1_2_RXNAME_LENGTH 
#define SBF_RECEIVERSETUP_1_2_RXNAME_LENGTH 20 
#endif

#ifndef SBF_RECEIVERSETUP_1_2_RXVERSION_LENGTH 
#define SBF_RECEIVERSETUP_1_2_RXVERSION_LENGTH 20 
#endif

#ifndef SBF_RECEIVERSETUP_1_2_ANTSERIALNBR_LENGTH 
#define SBF_RECEIVERSETUP_1_2_ANTSERIALNBR_LENGTH 20 
#endif

#ifndef SBF_RECEIVERSETUP_1_2_ANTTYPE_LENGTH 
#define SBF_RECEIVERSETUP_1_2_ANTTYPE_LENGTH 20 
#endif

#ifndef SBF_RECEIVERSETUP_1_2_MARKERTYPE_LENGTH 
#define SBF_RECEIVERSETUP_1_2_MARKERTYPE_LENGTH 20 
#endif

#ifndef SBF_RECEIVERSETUP_1_2_GNSSFWVERSION_LENGTH 
#define SBF_RECEIVERSETUP_1_2_GNSSFWVERSION_LENGTH 40 
#endif

#define SBF_RECEIVERSETUP_1_2_TOW_PRECISION 3
#define SBF_RECEIVERSETUP_1_2_DELTAH_PRECISION 7
#define SBF_RECEIVERSETUP_1_2_DELTAE_PRECISION 7
#define SBF_RECEIVERSETUP_1_2_DELTAN_PRECISION 7
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        Reserved[2];  
  char           MarkerName[SBF_RECEIVERSETUP_1_2_MARKERNAME_LENGTH];
  char           MarkerNumber[SBF_RECEIVERSETUP_1_2_MARKERNUMBER_LENGTH];
  char           Observer[SBF_RECEIVERSETUP_1_2_OBSERVER_LENGTH];
  char           Agency[SBF_RECEIVERSETUP_1_2_AGENCY_LENGTH];
  char           RxSerialNbr[SBF_RECEIVERSETUP_1_2_RXSERIALNBR_LENGTH];
  char           RxName[SBF_RECEIVERSETUP_1_2_RXNAME_LENGTH];
  char           RxVersion[SBF_RECEIVERSETUP_1_2_RXVERSION_LENGTH];
  char           AntSerialNbr[SBF_RECEIVERSETUP_1_2_ANTSERIALNBR_LENGTH];
  char           AntType[SBF_RECEIVERSETUP_1_2_ANTTYPE_LENGTH];
  float          deltaH;       /* [m] */
  float          deltaE;       /* [m] */
  float          deltaN;       /* [m] */
  char           MarkerType[SBF_RECEIVERSETUP_1_2_MARKERTYPE_LENGTH];
  char           GNSSFWVersion[SBF_RECEIVERSETUP_1_2_GNSSFWVERSION_LENGTH];
} ReceiverSetup_1_2_t;

/*--ReceiverSetup_1_3_t : ---------------------------------------------------*/
/* General information about the receiver installation */

#ifndef SBF_RECEIVERSETUP_1_3_MARKERNAME_LENGTH 
#define SBF_RECEIVERSETUP_1_3_MARKERNAME_LENGTH 60 
#endif

#ifndef SBF_RECEIVERSETUP_1_3_MARKERNUMBER_LENGTH 
#define SBF_RECEIVERSETUP_1_3_MARKERNUMBER_LENGTH 20 
#endif

#ifndef SBF_RECEIVERSETUP_1_3_OBSERVER_LENGTH 
#define SBF_RECEIVERSETUP_1_3_OBSERVER_LENGTH 20 
#endif

#ifndef SBF_RECEIVERSETUP_1_3_AGENCY_LENGTH 
#define SBF_RECEIVERSETUP_1_3_AGENCY_LENGTH 40 
#endif

#ifndef SBF_RECEIVERSETUP_1_3_RXSERIALNBR_LENGTH 
#define SBF_RECEIVERSETUP_1_3_RXSERIALNBR_LENGTH 20 
#endif

#ifndef SBF_RECEIVERSETUP_1_3_RXNAME_LENGTH 
#define SBF_RECEIVERSETUP_1_3_RXNAME_LENGTH 20 
#endif

#ifndef SBF_RECEIVERSETUP_1_3_RXVERSION_LENGTH 
#define SBF_RECEIVERSETUP_1_3_RXVERSION_LENGTH 20 
#endif

#ifndef SBF_RECEIVERSETUP_1_3_ANTSERIALNBR_LENGTH 
#define SBF_RECEIVERSETUP_1_3_ANTSERIALNBR_LENGTH 20 
#endif

#ifndef SBF_RECEIVERSETUP_1_3_ANTTYPE_LENGTH 
#define SBF_RECEIVERSETUP_1_3_ANTTYPE_LENGTH 20 
#endif

#ifndef SBF_RECEIVERSETUP_1_3_MARKERTYPE_LENGTH 
#define SBF_RECEIVERSETUP_1_3_MARKERTYPE_LENGTH 20 
#endif

#ifndef SBF_RECEIVERSETUP_1_3_GNSSFWVERSION_LENGTH 
#define SBF_RECEIVERSETUP_1_3_GNSSFWVERSION_LENGTH 40 
#endif

#ifndef SBF_RECEIVERSETUP_1_3_PRODUCTNAME_LENGTH 
#define SBF_RECEIVERSETUP_1_3_PRODUCTNAME_LENGTH 40 
#endif

#define SBF_RECEIVERSETUP_1_3_TOW_PRECISION 3
#define SBF_RECEIVERSETUP_1_3_DELTAH_PRECISION 7
#define SBF_RECEIVERSETUP_1_3_DELTAE_PRECISION 7
#define SBF_RECEIVERSETUP_1_3_DELTAN_PRECISION 7
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        Reserved[2];  
  char           MarkerName[SBF_RECEIVERSETUP_1_3_MARKERNAME_LENGTH];
  char           MarkerNumber[SBF_RECEIVERSETUP_1_3_MARKERNUMBER_LENGTH];
  char           Observer[SBF_RECEIVERSETUP_1_3_OBSERVER_LENGTH];
  char           Agency[SBF_RECEIVERSETUP_1_3_AGENCY_LENGTH];
  char           RxSerialNbr[SBF_RECEIVERSETUP_1_3_RXSERIALNBR_LENGTH];
  char           RxName[SBF_RECEIVERSETUP_1_3_RXNAME_LENGTH];
  char           RxVersion[SBF_RECEIVERSETUP_1_3_RXVERSION_LENGTH];
  char           AntSerialNbr[SBF_RECEIVERSETUP_1_3_ANTSERIALNBR_LENGTH];
  char           AntType[SBF_RECEIVERSETUP_1_3_ANTTYPE_LENGTH];
  float          deltaH;       /* [m] */
  float          deltaE;       /* [m] */
  float          deltaN;       /* [m] */
  char           MarkerType[SBF_RECEIVERSETUP_1_3_MARKERTYPE_LENGTH];
  char           GNSSFWVersion[SBF_RECEIVERSETUP_1_3_GNSSFWVERSION_LENGTH];
  char           ProductName[SBF_RECEIVERSETUP_1_3_PRODUCTNAME_LENGTH];
} ReceiverSetup_1_3_t;

/*--ReceiverSetup_1_4_t : ---------------------------------------------------*/
/* General information about the receiver installation */

#ifndef SBF_RECEIVERSETUP_1_4_MARKERNAME_LENGTH 
#define SBF_RECEIVERSETUP_1_4_MARKERNAME_LENGTH 60 
#endif

#ifndef SBF_RECEIVERSETUP_1_4_MARKERNUMBER_LENGTH 
#define SBF_RECEIVERSETUP_1_4_MARKERNUMBER_LENGTH 20 
#endif

#ifndef SBF_RECEIVERSETUP_1_4_OBSERVER_LENGTH 
#define SBF_RECEIVERSETUP_1_4_OBSERVER_LENGTH 20 
#endif

#ifndef SBF_RECEIVERSETUP_1_4_AGENCY_LENGTH 
#define SBF_RECEIVERSETUP_1_4_AGENCY_LENGTH 40 
#endif

#ifndef SBF_RECEIVERSETUP_1_4_RXSERIALNBR_LENGTH 
#define SBF_RECEIVERSETUP_1_4_RXSERIALNBR_LENGTH 20 
#endif

#ifndef SBF_RECEIVERSETUP_1_4_RXNAME_LENGTH 
#define SBF_RECEIVERSETUP_1_4_RXNAME_LENGTH 20 
#endif

#ifndef SBF_RECEIVERSETUP_1_4_RXVERSION_LENGTH 
#define SBF_RECEIVERSETUP_1_4_RXVERSION_LENGTH 20 
#endif

#ifndef SBF_RECEIVERSETUP_1_4_ANTSERIALNBR_LENGTH 
#define SBF_RECEIVERSETUP_1_4_ANTSERIALNBR_LENGTH 20 
#endif

#ifndef SBF_RECEIVERSETUP_1_4_ANTTYPE_LENGTH 
#define SBF_RECEIVERSETUP_1_4_ANTTYPE_LENGTH 20 
#endif

#ifndef SBF_RECEIVERSETUP_1_4_MARKERTYPE_LENGTH 
#define SBF_RECEIVERSETUP_1_4_MARKERTYPE_LENGTH 20 
#endif

#ifndef SBF_RECEIVERSETUP_1_4_GNSSFWVERSION_LENGTH 
#define SBF_RECEIVERSETUP_1_4_GNSSFWVERSION_LENGTH 40 
#endif

#ifndef SBF_RECEIVERSETUP_1_4_PRODUCTNAME_LENGTH 
#define SBF_RECEIVERSETUP_1_4_PRODUCTNAME_LENGTH 40 
#endif

#ifndef SBF_RECEIVERSETUP_1_4_STATIONCODE_LENGTH 
#define SBF_RECEIVERSETUP_1_4_STATIONCODE_LENGTH 10 
#endif

#ifndef SBF_RECEIVERSETUP_1_4_COUNTRYCODE_LENGTH 
#define SBF_RECEIVERSETUP_1_4_COUNTRYCODE_LENGTH 3 
#endif

#ifndef SBF_RECEIVERSETUP_1_4_RESERVED1_LENGTH 
#define SBF_RECEIVERSETUP_1_4_RESERVED1_LENGTH 21 
#endif

#define SBF_RECEIVERSETUP_1_4_TOW_PRECISION 3
#define SBF_RECEIVERSETUP_1_4_DELTAH_PRECISION 7
#define SBF_RECEIVERSETUP_1_4_DELTAE_PRECISION 7
#define SBF_RECEIVERSETUP_1_4_DELTAN_PRECISION 7
#define SBF_RECEIVERSETUP_1_4_LATITUDE_PRECISION 12
#define SBF_RECEIVERSETUP_1_4_LONGITUDE_PRECISION 12
#define SBF_RECEIVERSETUP_1_4_HEIGHT_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        Reserved[2];  
  char           MarkerName[SBF_RECEIVERSETUP_1_4_MARKERNAME_LENGTH];
  char           MarkerNumber[SBF_RECEIVERSETUP_1_4_MARKERNUMBER_LENGTH];
  char           Observer[SBF_RECEIVERSETUP_1_4_OBSERVER_LENGTH];
  char           Agency[SBF_RECEIVERSETUP_1_4_AGENCY_LENGTH];
  char           RxSerialNbr[SBF_RECEIVERSETUP_1_4_RXSERIALNBR_LENGTH];
  char           RxName[SBF_RECEIVERSETUP_1_4_RXNAME_LENGTH];
  char           RxVersion[SBF_RECEIVERSETUP_1_4_RXVERSION_LENGTH];
  char           AntSerialNbr[SBF_RECEIVERSETUP_1_4_ANTSERIALNBR_LENGTH];
  char           AntType[SBF_RECEIVERSETUP_1_4_ANTTYPE_LENGTH];
  float          deltaH;       /* [m] */
  float          deltaE;       /* [m] */
  float          deltaN;       /* [m] */
  char           MarkerType[SBF_RECEIVERSETUP_1_4_MARKERTYPE_LENGTH];
  char           GNSSFWVersion[SBF_RECEIVERSETUP_1_4_GNSSFWVERSION_LENGTH];
  char           ProductName[SBF_RECEIVERSETUP_1_4_PRODUCTNAME_LENGTH];
  SBFDOUBLE      Latitude;     
  SBFDOUBLE      Longitude;    
  float          Height;       
  char           StationCode[SBF_RECEIVERSETUP_1_4_STATIONCODE_LENGTH];
  uint8_t        MonumentIdx;  
  uint8_t        ReceiverIdx;  
  char           CountryCode[SBF_RECEIVERSETUP_1_4_COUNTRYCODE_LENGTH];
  char           Reserved1[SBF_RECEIVERSETUP_1_4_RESERVED1_LENGTH];
} ReceiverSetup_1_4_t;

#define SBF_RECEIVERSETUP_1_MARKERNAME_LENGTH SBF_RECEIVERSETUP_1_4_MARKERNAME_LENGTH
#define SBF_RECEIVERSETUP_1_MARKERNUMBER_LENGTH SBF_RECEIVERSETUP_1_4_MARKERNUMBER_LENGTH
#define SBF_RECEIVERSETUP_1_OBSERVER_LENGTH SBF_RECEIVERSETUP_1_4_OBSERVER_LENGTH
#define SBF_RECEIVERSETUP_1_AGENCY_LENGTH SBF_RECEIVERSETUP_1_4_AGENCY_LENGTH
#define SBF_RECEIVERSETUP_1_RXSERIALNBR_LENGTH SBF_RECEIVERSETUP_1_4_RXSERIALNBR_LENGTH
#define SBF_RECEIVERSETUP_1_RXNAME_LENGTH SBF_RECEIVERSETUP_1_4_RXNAME_LENGTH
#define SBF_RECEIVERSETUP_1_RXVERSION_LENGTH SBF_RECEIVERSETUP_1_4_RXVERSION_LENGTH
#define SBF_RECEIVERSETUP_1_ANTSERIALNBR_LENGTH SBF_RECEIVERSETUP_1_4_ANTSERIALNBR_LENGTH
#define SBF_RECEIVERSETUP_1_ANTTYPE_LENGTH SBF_RECEIVERSETUP_1_4_ANTTYPE_LENGTH
#define SBF_RECEIVERSETUP_1_MARKERTYPE_LENGTH SBF_RECEIVERSETUP_1_4_MARKERTYPE_LENGTH
#define SBF_RECEIVERSETUP_1_GNSSFWVERSION_LENGTH SBF_RECEIVERSETUP_1_4_GNSSFWVERSION_LENGTH
#define SBF_RECEIVERSETUP_1_PRODUCTNAME_LENGTH SBF_RECEIVERSETUP_1_4_PRODUCTNAME_LENGTH
#define SBF_RECEIVERSETUP_1_STATIONCODE_LENGTH SBF_RECEIVERSETUP_1_4_STATIONCODE_LENGTH
#define SBF_RECEIVERSETUP_1_COUNTRYCODE_LENGTH SBF_RECEIVERSETUP_1_4_COUNTRYCODE_LENGTH
#define SBF_RECEIVERSETUP_1_RESERVED1_LENGTH SBF_RECEIVERSETUP_1_4_RESERVED1_LENGTH
typedef ReceiverSetup_1_4_t ReceiverSetup_1_t;

/*--RxComponents_1_0_t : ----------------------------------------------------*/
/* Information on various receiver components */

#ifndef SBF_COMPONENT_1_0_NAME_LENGTH 
#define SBF_COMPONENT_1_0_NAME_LENGTH 40 
#endif

#ifndef SBF_COMPONENT_1_0_SERIALNUMBER_LENGTH 
#define SBF_COMPONENT_1_0_SERIALNUMBER_LENGTH 20 
#endif

#ifndef SBF_COMPONENT_1_0_FWVERSION_LENGTH 
#define SBF_COMPONENT_1_0_FWVERSION_LENGTH 40 
#endif

#ifndef SBF_COMPONENT_1_0_MACADDRESS_LENGTH 
#define SBF_COMPONENT_1_0_MACADDRESS_LENGTH 6 
#endif

#ifndef SBF_COMPONENT_1_0__PADDING_LENGTH 
#define SBF_COMPONENT_1_0__PADDING_LENGTH 2 
#endif

typedef struct
{
  uint8_t        Type;         
  uint8_t        CPULoad;      
  uint8_t        Reserved[2];  
  char           Name[SBF_COMPONENT_1_0_NAME_LENGTH];
  char           SerialNumber[SBF_COMPONENT_1_0_SERIALNUMBER_LENGTH];
  char           FWVersion[SBF_COMPONENT_1_0_FWVERSION_LENGTH];
  uint8_t        MACAddress[SBF_COMPONENT_1_0_MACADDRESS_LENGTH];

  uint8_t        _padding[SBF_COMPONENT_1_0__PADDING_LENGTH];
} Component_1_0_t;

#ifndef SBF_RXCOMPONENTS_1_0_COMPONENT_LENGTH 
#define SBF_RXCOMPONENTS_1_0_COMPONENT_LENGTH 20 
#endif

#define SBF_RXCOMPONENTS_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        N;            
  uint8_t        SBLength;     
  uint8_t        Reserved[4];  
  Component_1_0_t Component[SBF_RXCOMPONENTS_1_0_COMPONENT_LENGTH];
} RxComponents_1_0_t;

#define SBF_COMPONENT_1_NAME_LENGTH SBF_COMPONENT_1_0_NAME_LENGTH
#define SBF_COMPONENT_1_SERIALNUMBER_LENGTH SBF_COMPONENT_1_0_SERIALNUMBER_LENGTH
#define SBF_COMPONENT_1_FWVERSION_LENGTH SBF_COMPONENT_1_0_FWVERSION_LENGTH
#define SBF_COMPONENT_1_MACADDRESS_LENGTH SBF_COMPONENT_1_0_MACADDRESS_LENGTH
#define SBF_COMPONENT_1__PADDING_LENGTH SBF_COMPONENT_1_0__PADDING_LENGTH
typedef Component_1_0_t Component_1_t;

#define SBF_RXCOMPONENTS_1_COMPONENT_LENGTH SBF_RXCOMPONENTS_1_0_COMPONENT_LENGTH
typedef RxComponents_1_0_t RxComponents_1_t;

/*--RxMessage_1_0_t : -------------------------------------------------------*/
/* Receiver message */

#ifndef SBF_RXMESSAGE_1_0_RESERVED2_LENGTH 
#define SBF_RXMESSAGE_1_0_RESERVED2_LENGTH 2 
#endif

#ifndef SBF_RXMESSAGE_1_0_MESSAGE_LENGTH 
#define SBF_RXMESSAGE_1_0_MESSAGE_LENGTH 2000 
#endif

#define SBF_RXMESSAGE_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        Type;         
  uint8_t        Severity;     
  uint32_t       MessageID;    
  uint16_t       StringLn;     
  uint8_t        Reserved2[SBF_RXMESSAGE_1_0_RESERVED2_LENGTH];
  char           Message[SBF_RXMESSAGE_1_0_MESSAGE_LENGTH];
} RxMessage_1_0_t;

#define SBF_RXMESSAGE_1_RESERVED2_LENGTH SBF_RXMESSAGE_1_0_RESERVED2_LENGTH
#define SBF_RXMESSAGE_1_MESSAGE_LENGTH SBF_RXMESSAGE_1_0_MESSAGE_LENGTH
typedef RxMessage_1_0_t RxMessage_1_t;

/*--Commands_1_0_t : --------------------------------------------------------*/
/* Commands entered by the user */

#ifndef SBF_COMMANDS_1_0_CMDDATA_LENGTH 
#define SBF_COMMANDS_1_0_CMDDATA_LENGTH 2048 
#endif

#define SBF_COMMANDS_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        Reserved[2];  
  uint8_t        CmdData[SBF_COMMANDS_1_0_CMDDATA_LENGTH];
} Commands_1_0_t;

#define SBF_COMMANDS_1_CMDDATA_LENGTH SBF_COMMANDS_1_0_CMDDATA_LENGTH
typedef Commands_1_0_t Commands_1_t;

/*--Comment_1_0_t : ---------------------------------------------------------*/
/* Comment entered by the user */

#ifndef SBF_COMMENT_1_0_COMMENT_LENGTH 
#define SBF_COMMENT_1_0_COMMENT_LENGTH 120 
#endif

#define SBF_COMMENT_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint16_t       CommentLn;    
  char           Comment[SBF_COMMENT_1_0_COMMENT_LENGTH];
} Comment_1_0_t;

#define SBF_COMMENT_1_COMMENT_LENGTH SBF_COMMENT_1_0_COMMENT_LENGTH
typedef Comment_1_0_t Comment_1_t;

/*--BBSamples_1_0_t : -------------------------------------------------------*/
/* Baseband samples */

#ifndef SBF_BBSAMPLESDATA_1_0_SAMPLES_LENGTH 
#define SBF_BBSAMPLESDATA_1_0_SAMPLES_LENGTH 2048 
#endif

#define SBF_BBSAMPLESDATA_1_0_TOW_PRECISION 3
typedef struct
{
  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint16_t       N;            
  uint8_t        Info;         
  uint8_t        Reserved[3];  
  uint32_t       SampleFreq;   
  uint32_t       LOFreq;       
  uint16_t       Samples[SBF_BBSAMPLESDATA_1_0_SAMPLES_LENGTH];
} BBSamplesData_1_0_t;

typedef struct
{
  BlockHeader_t  Header;       

  BBSamplesData_1_0_t BBSamplesData;
} BBSamples_1_0_t;

#define SBF_BBSAMPLESDATA_1_SAMPLES_LENGTH SBF_BBSAMPLESDATA_1_0_SAMPLES_LENGTH
typedef BBSamplesData_1_0_t BBSamplesData_1_t;

typedef BBSamples_1_0_t BBSamples_1_t;

/*--ASCIIIn_1_0_t : ---------------------------------------------------------*/
/* ASCII input from external sensor */

#ifndef SBF_ASCIIIN_1_0_SENSORMODEL_LENGTH 
#define SBF_ASCIIIN_1_0_SENSORMODEL_LENGTH 20 
#endif

#ifndef SBF_ASCIIIN_1_0_SENSORTYPE_LENGTH 
#define SBF_ASCIIIN_1_0_SENSORTYPE_LENGTH 20 
#endif

#ifndef SBF_ASCIIIN_1_0_ASCIISTRING_LENGTH 
#define SBF_ASCIIIN_1_0_ASCIISTRING_LENGTH 2000 
#endif

#define SBF_ASCIIIN_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        CD;           
  uint8_t        Reserved1[3]; 
  uint16_t       StringLn;     
  char           SensorModel[SBF_ASCIIIN_1_0_SENSORMODEL_LENGTH];
  char           SensorType[SBF_ASCIIIN_1_0_SENSORTYPE_LENGTH];
  uint8_t        Reserved2[20];
  char           ASCIIString[SBF_ASCIIIN_1_0_ASCIISTRING_LENGTH];
} ASCIIIn_1_0_t;

#define SBF_ASCIIIN_1_SENSORMODEL_LENGTH SBF_ASCIIIN_1_0_SENSORMODEL_LENGTH
#define SBF_ASCIIIN_1_SENSORTYPE_LENGTH SBF_ASCIIIN_1_0_SENSORTYPE_LENGTH
#define SBF_ASCIIIN_1_ASCIISTRING_LENGTH SBF_ASCIIIN_1_0_ASCIISTRING_LENGTH
typedef ASCIIIn_1_0_t ASCIIIn_1_t;

/*--EncapsulatedOutput_1_0_t : ----------------------------------------------*/
/* SBF encapsulation of non-SBF messages */

#ifndef SBF_ENCAPSULATEDOUTPUT_1_0_PAYLOAD_LENGTH 
#define SBF_ENCAPSULATEDOUTPUT_1_0_PAYLOAD_LENGTH 4096 
#endif

#define SBF_ENCAPSULATEDOUTPUT_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        Mode;         
  uint8_t        Reserved;     
  uint16_t       N;            
  uint16_t       ReservedId;   
  uint8_t        Payload[SBF_ENCAPSULATEDOUTPUT_1_0_PAYLOAD_LENGTH];
} EncapsulatedOutput_1_0_t;

#define SBF_ENCAPSULATEDOUTPUT_1_PAYLOAD_LENGTH SBF_ENCAPSULATEDOUTPUT_1_0_PAYLOAD_LENGTH
typedef EncapsulatedOutput_1_0_t EncapsulatedOutput_1_t;


/*==TUR Specific Blocks======================================================*/

/*--TURPVTSatCorrections_1_0_t : --------------------------------------------*/
/* Satellite range corrections */

typedef struct
{
  uint8_t        Type;         
  uint8_t        Reserved[3];  
  float          ClockBias;    
  float          ClockDrift;   
} SatClkCorrInfo_1_0_t;

typedef struct
{
  uint8_t        SVID;         
  uint8_t        FreqNr;       
  uint8_t        N2;           
  uint8_t        Reserved;     
  float          TropoDelay;   
  float          IonoDelay;    
  float          RelCorr;      
  float          PRC;          
} SatCorrInfo_1_0_t;

#ifndef SBF_TURPVTSATCORRECTIONS_1_0_DATA_LENGTH 
#define SBF_TURPVTSATCORRECTIONS_1_0_DATA_LENGTH -1 
#endif

#define SBF_TURPVTSATCORRECTIONS_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        N;            
  uint8_t        SB1Length;    
  uint8_t        SB2Length;    
  uint8_t        Reserved[3];  
  uint8_t        Data[SBF_TURPVTSATCORRECTIONS_1_0_DATA_LENGTH];
} TURPVTSatCorrections_1_0_t;

typedef SatClkCorrInfo_1_0_t SatClkCorrInfo_1_t;

typedef SatCorrInfo_1_0_t SatCorrInfo_1_t;

#define SBF_TURPVTSATCORRECTIONS_1_DATA_LENGTH SBF_TURPVTSATCORRECTIONS_1_0_DATA_LENGTH
typedef TURPVTSatCorrections_1_0_t TURPVTSatCorrections_1_t;

/*--TURHPCAInfo_1_0_t : -----------------------------------------------------*/
/* HMI Probability information */

#ifndef SBF_SATHPCA_1_0__PADDING_LENGTH 
#define SBF_SATHPCA_1_0__PADDING_LENGTH 1 
#endif

#define SBF_SATHPCA_1_0_HORDEV_PRECISION 3
#define SBF_SATHPCA_1_0_VERDEV_PRECISION 3
#define SBF_SATHPCA_1_0_HMIPROB_PRECISION 3
typedef struct
{
  uint8_t        SVID;         
  uint8_t        info;         
  uint8_t        SISA;         
  uint8_t        SISMA;        
  float          HorDev;       
  float          VerDev;       
  SBFDOUBLE      HMIProb;      
  uint16_t       NavAge;       
  uint8_t        FreqNr;       

  uint8_t        _padding[SBF_SATHPCA_1_0__PADDING_LENGTH];
} SatHpca_1_0_t;

#ifndef SBF_TURHPCAINFO_1_0_HPCADATA_LENGTH 
#define SBF_TURHPCAINFO_1_0_HPCADATA_LENGTH 72 
#endif

#define SBF_TURHPCAINFO_1_0_TOW_PRECISION 3
#define SBF_TURHPCAINFO_1_0_HMIPROB_PRECISION 3
#define SBF_TURHPCAINFO_1_0_HORDEV_PRECISION 3
#define SBF_TURHPCAINFO_1_0_VERDEV_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        N;            
  uint8_t        SBSize;       
  uint8_t        NWA_I;        
  uint8_t        NrSVc;        
  uint8_t        NrLink;       
  uint8_t        Reserved;     
  SBFDOUBLE      HMIProb;      
  float          HorDev;       
  float          VerDev;       
  SatHpca_1_0_t  HPCAData[SBF_TURHPCAINFO_1_0_HPCADATA_LENGTH];
} TURHPCAInfo_1_0_t;

#define SBF_SATHPCA_1__PADDING_LENGTH SBF_SATHPCA_1_0__PADDING_LENGTH
typedef SatHpca_1_0_t SatHpca_1_t;

#define SBF_TURHPCAINFO_1_HPCADATA_LENGTH SBF_TURHPCAINFO_1_0_HPCADATA_LENGTH
typedef TURHPCAInfo_1_0_t TURHPCAInfo_1_t;

/*--CorrPeakSample_1_0_t : --------------------------------------------------*/
/* Real-time samples of the correlation peak function */

typedef struct
{
  int16_t        Offset;       
  int8_t         I;            
  int8_t         Q;            
} CorrSample_1_0_t;

#ifndef SBF_CORRPEAKSAMPLE_1_0_CORRSAMPLEDATA_LENGTH 
#define SBF_CORRPEAKSAMPLE_1_0_CORRSAMPLEDATA_LENGTH 11 
#endif

#define SBF_CORRPEAKSAMPLE_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        N;            
  uint8_t        SBSize;       
  uint8_t        RxChannel;    
  uint8_t        Type;         
  uint8_t        SVID;         
  uint8_t        Reserved;     
  CorrSample_1_0_t CorrSampleData[SBF_CORRPEAKSAMPLE_1_0_CORRSAMPLEDATA_LENGTH];
} CorrPeakSample_1_0_t;

typedef CorrSample_1_0_t CorrSample_1_t;

#define SBF_CORRPEAKSAMPLE_1_CORRSAMPLEDATA_LENGTH SBF_CORRPEAKSAMPLE_1_0_CORRSAMPLEDATA_LENGTH
typedef CorrPeakSample_1_0_t CorrPeakSample_1_t;

/*--CorrValues_1_0_t : ------------------------------------------------------*/
/* Raw correlation values */

typedef struct
{
  uint8_t        dT;           
  uint8_t        Type;         
  uint16_t       CorrInfo;     
  int16_t        I;            
  int16_t        Q;            
} CorrValue_1_0_t;

#ifndef SBF_CORRVALUES_1_0_CORRELATIONVALUES_LENGTH 
#define SBF_CORRVALUES_1_0_CORRELATIONVALUES_LENGTH 2000 
#endif

#ifndef SBF_CORRVALUES_1_0__PADDING_LENGTH 
#define SBF_CORRVALUES_1_0__PADDING_LENGTH 2 
#endif

#define SBF_CORRVALUES_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint16_t       N;            
  uint8_t        SBLength;     
  uint8_t        RxChannel;    
  uint8_t        SVID;         
  uint8_t        Reserved[3];  
  CorrValue_1_0_t CorrelationValues[SBF_CORRVALUES_1_0_CORRELATIONVALUES_LENGTH];

  uint8_t        _padding[SBF_CORRVALUES_1_0__PADDING_LENGTH];
} CorrValues_1_0_t;

typedef CorrValue_1_0_t CorrValue_1_t;

#define SBF_CORRVALUES_1_CORRELATIONVALUES_LENGTH SBF_CORRVALUES_1_0_CORRELATIONVALUES_LENGTH
#define SBF_CORRVALUES_1__PADDING_LENGTH SBF_CORRVALUES_1_0__PADDING_LENGTH
typedef CorrValues_1_0_t CorrValues_1_t;

/*--TURStatus_1_0_t : -------------------------------------------------------*/
/* TUR-specific status information */

#define SBF_TURSTATUS_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  int8_t         RxTemp;       
  int8_t         AntTemp;      
} TURStatus_1_0_t;

/*--TURStatus_1_1_t : -------------------------------------------------------*/
/* TUR-specific status information */

#define SBF_TURSTATUS_1_1_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  int8_t         RxTemp;       
  int8_t         AntTemp;      
  uint32_t       EventCountTB1;
  uint32_t       EventCountTB2;
  uint32_t       EventCountTB3;
  uint32_t       StartIFL_TOW; 
  uint16_t       StartIFL_WNc; 
  uint8_t        StartIFL_Status;
  uint8_t        Reserved1;    
  uint32_t       StopIFL_TOW;  
  uint16_t       StopIFL_WNc;  
  uint8_t        StopIFL_Status;
  uint8_t        Reserved2;    
} TURStatus_1_1_t;

/*--TURStatus_1_2_t : -------------------------------------------------------*/
/* TUR-specific status information */

#define SBF_TURSTATUS_1_2_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  int8_t         RxTemp;       
  int8_t         AntTemp;      
  uint32_t       EventCountTB1;
  uint32_t       EventCountTB2;
  uint32_t       EventCountTB3;
  uint32_t       StartIFL_TOW; 
  uint16_t       StartIFL_WNc; 
  uint8_t        StartIFL_Status;
  uint8_t        Reserved1;    
  uint32_t       StopIFL_TOW;  
  uint16_t       StopIFL_WNc;  
  uint8_t        StopIFL_Status;
  uint8_t        Reserved2;    
  int16_t        I_E5;         
  int16_t        V_E5;         
  int16_t        I_E5a;        
  int16_t        V_E5a;        
  int16_t        I_E5b;        
  int16_t        V_E5b;        
  int16_t        I_E6;         
  int16_t        V_E6;         
  int16_t        I_L1;         
  int16_t        V_L1;         
  int16_t        I_12V_Ant;    
  int16_t        V_12V_Ant;    
  int16_t        I_12V;        
  int16_t        V_12V;        
  int16_t        I_5V;         
  int16_t        V_5V;         
  int16_t        I_5V_OCXO;    
  int16_t        V_5V_OCXO;    
} TURStatus_1_2_t;

typedef TURStatus_1_2_t TURStatus_1_t;

/*--GALIntegrity_1_0_t : ----------------------------------------------------*/
/* Galileo integrity data */

#ifndef SBF_GAINTEGRITY_1_0_INTEGRITYSVI_LENGTH 
#define SBF_GAINTEGRITY_1_0_INTEGRITYSVI_LENGTH 36 
#endif

#define SBF_GAINTEGRITY_1_0_TOW_PRECISION 3
typedef struct
{
  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        SVID;         /* SBF range 71-102 SIS range 1-64 [F1/I4/G1] */
  uint8_t        Source;       /* bitfield according to sigType */
  uint32_t       ToC;          /* Time of Computation in sec of week */
  uint8_t        RegionStatus; /* 8-bit region status */
  uint8_t        Authenticated;/* 1 : succeeded, 0 otherwise */
  uint8_t        CleverWord;   
  uint8_t        UpdateEvent;  
  uint8_t        IntegritySVi[SBF_GAINTEGRITY_1_0_INTEGRITYSVI_LENGTH];/* Bit 0-3 : IF bits
           Bit 4-6 : SNF info
           Bit   7 : reserved */
} gaIntegrity_1_0_t;

typedef struct
{
  BlockHeader_t  Header;       

  gaIntegrity_1_0_t galIntegrity; 
} GALIntegrity_1_0_t;

#define SBF_GAINTEGRITY_1_INTEGRITYSVI_LENGTH SBF_GAINTEGRITY_1_0_INTEGRITYSVI_LENGTH
typedef gaIntegrity_1_0_t gaIntegrity_1_t;

typedef GALIntegrity_1_0_t GALIntegrity_1_t;

/*--GALNavMonitor_1_0_t : ---------------------------------------------------*/
/* Monitoring navigation data per Galileo satellite. */

#ifndef SBF_GALNAVMONITOR_1_0__PADDING_LENGTH 
#define SBF_GALNAVMONITOR_1_0__PADDING_LENGTH 3 
#endif

#define SBF_GALNAVMONITOR_1_0_TOW_PRECISION 3
#define SBF_GALNAVMONITOR_1_0_POSITIONDIFF_PRECISION 3
#define SBF_GALNAVMONITOR_1_0_TIMECORRDIFF_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        SVID;         
  uint8_t        Source;       
  float          PositionDiff; 
  float          TimeCorrDiff; 
  uint16_t       IODNavCurr;   
  uint16_t       IODNavPrev;   
  uint8_t        Flags;        

  uint8_t        _padding[SBF_GALNAVMONITOR_1_0__PADDING_LENGTH];
} GALNavMonitor_1_0_t;

#define SBF_GALNAVMONITOR_1__PADDING_LENGTH SBF_GALNAVMONITOR_1_0__PADDING_LENGTH
typedef GALNavMonitor_1_0_t GALNavMonitor_1_t;

/*--TURFormat_1_0_t : -------------------------------------------------------*/

#ifndef SBF_TURFORMATMODE1RGCREQUEST_1_0__PADDING_LENGTH 
#define SBF_TURFORMATMODE1RGCREQUEST_1_0__PADDING_LENGTH 2 
#endif

typedef struct
{
  uint8_t        uCodeGen1;    
  uint8_t        uCodeGen2;    
  uint8_t        uCodeGen3;    
  uint8_t        uSatNo;       
  uint8_t        uFreq;        
  uint8_t        uChipRate;    
  uint16_t       uWeekNr;      
  uint16_t       uTOW;         
  uint16_t       uPeriod;      
  uint16_t       uLatency;     

  uint8_t        _padding[SBF_TURFORMATMODE1RGCREQUEST_1_0__PADDING_LENGTH];
} TURFormatMode1RGCrequest_1_0_t;

#ifndef SBF_TURFORMATRANGINGCODEREADY_1_0__PADDING_LENGTH 
#define SBF_TURFORMATRANGINGCODEREADY_1_0__PADDING_LENGTH 2 
#endif

typedef struct
{
  uint8_t        uCodeGen;     
  uint8_t        uPadding;     

  uint8_t        _padding[SBF_TURFORMATRANGINGCODEREADY_1_0__PADDING_LENGTH];
} TURFormatRangingCodeReady_1_0_t;

typedef struct
{
  uint8_t        uCodeGen;     
  uint8_t        uStartParam;  
  uint8_t        uStep;        
  uint8_t        uLength;      
  uint8_t        uSatNo;       
  uint8_t        uFreq;        
  uint8_t        uChipRate;    
  uint16_t       uWeekNr;      
  uint16_t       uTOW;         
  uint8_t        uM1parameter; 
} TURFormatMode2RGCrequest_1_0_t;

#ifndef SBF_TURFORMATGETMODE2TIME_1_0__PADDING_LENGTH 
#define SBF_TURFORMATGETMODE2TIME_1_0__PADDING_LENGTH 2 
#endif

typedef struct
{
  uint8_t        uCodeGen;     
  uint8_t        uM2hit;       
  uint8_t        uSatNo;       
  uint8_t        uFreq;        
  uint8_t        uChipRate;    
  uint16_t       uWeekNr;      
  uint16_t       uTOW;         
  uint8_t        uPadding;     

  uint8_t        _padding[SBF_TURFORMATGETMODE2TIME_1_0__PADDING_LENGTH];
} TURFormatGetMode2Time_1_0_t;

#ifndef SBF_TURFORMATMODE2TIMEREPLY_1_0__PADDING_LENGTH 
#define SBF_TURFORMATMODE2TIMEREPLY_1_0__PADDING_LENGTH 2 
#endif

typedef struct
{
  uint16_t       uRefCount;    
  uint32_t       uIndex;       

  uint8_t        _padding[SBF_TURFORMATMODE2TIMEREPLY_1_0__PADDING_LENGTH];
} TURFormatMode2TimeReply_1_0_t;

#ifndef SBF_TURFORMATRANGINGCODEREQUEST_1_0__PADDING_LENGTH 
#define SBF_TURFORMATRANGINGCODEREQUEST_1_0__PADDING_LENGTH 3 
#endif

typedef struct
{
  uint8_t        uCodeGen;     
  uint8_t        uSatNo;       
  uint8_t        uFreq;        
  uint8_t        uChipRate;    
  uint16_t       uWeekNr;      
  uint16_t       uTOW;         
  uint32_t       uBlock;       
  uint8_t        uM1Parameter; 

  uint8_t        _padding[SBF_TURFORMATRANGINGCODEREQUEST_1_0__PADDING_LENGTH];
} TURFormatRangingCodeRequest_1_0_t;

#ifndef SBF_TURFORMATENCRYPTEDDATA_1_0_UDATA_LENGTH 
#define SBF_TURFORMATENCRYPTEDDATA_1_0_UDATA_LENGTH 41 
#endif

#ifndef SBF_TURFORMATENCRYPTEDDATA_1_0__PADDING_LENGTH 
#define SBF_TURFORMATENCRYPTEDDATA_1_0__PADDING_LENGTH 2 
#endif

typedef struct
{
  uint8_t        uSatNo;       
  uint8_t        uFreq;        
  uint16_t       uWeekNr;      
  uint32_t       uTOW;         
  uint8_t        uData[SBF_TURFORMATENCRYPTEDDATA_1_0_UDATA_LENGTH];
  uint8_t        uPadding;     

  uint8_t        _padding[SBF_TURFORMATENCRYPTEDDATA_1_0__PADDING_LENGTH];
} TURFormatEncryptedData_1_0_t;

#ifndef SBF_TURFORMATDECRYPTEDDATA_1_0_UDATA_LENGTH 
#define SBF_TURFORMATDECRYPTEDDATA_1_0_UDATA_LENGTH 21 
#endif

#ifndef SBF_TURFORMATDECRYPTEDDATA_1_0__PADDING_LENGTH 
#define SBF_TURFORMATDECRYPTEDDATA_1_0__PADDING_LENGTH 1 
#endif

typedef struct
{
  uint8_t        uRefCount;    
  uint8_t        uData[SBF_TURFORMATDECRYPTEDDATA_1_0_UDATA_LENGTH];
  uint8_t        uPadding;     

  uint8_t        _padding[SBF_TURFORMATDECRYPTEDDATA_1_0__PADDING_LENGTH];
} TURFormatDecryptedData_1_0_t;

typedef struct
{
  int32_t        iLatitude;    
  int32_t        iLongitude;   
  uint32_t       uVelocity;    
  int32_t        iBearing;     
  int32_t        iAltitude;    
  uint16_t       uWeekNumber;  
  uint32_t       uTimeOfWeek;  
  uint16_t       uMillisecond; 
  uint16_t       uUTCOffeset;  
  uint8_t        uTimeType;    
  uint8_t        uPadding;     
} TURFormatPVT_1_0_t;

#ifndef SBF_TURFORMATDENIAL_1_0__PADDING_LENGTH 
#define SBF_TURFORMATDENIAL_1_0__PADDING_LENGTH 2 
#endif

typedef struct
{
  uint8_t        uDenialFlag;  
  uint8_t        uPadding;     

  uint8_t        _padding[SBF_TURFORMATDENIAL_1_0__PADDING_LENGTH];
} TURFormatDenial_1_0_t;

typedef struct
{
  uint8_t        Format;       
  uint8_t        EventType;    
  uint16_t       WN;           
  uint32_t       TOW;          
} TURFormatEventLog_1_0_t;

typedef struct
{
  uint16_t       uSerialNumberMSB;
  uint32_t       uSerialNumberLSB;
  uint16_t       uSMVersionMajor;
  uint16_t       uSMVersionMinor1;
  uint16_t       uSMVersionMinor2;
} TURFormatVersionResponse_1_0_t;

#ifndef SBF_TURFORMATSTATUSRESPONSE_1_0__PADDING_LENGTH 
#define SBF_TURFORMATSTATUSRESPONSE_1_0__PADDING_LENGTH 2 
#endif

typedef struct
{
  uint8_t        uStatus;      
  uint8_t        uPadding;     

  uint8_t        _padding[SBF_TURFORMATSTATUSRESPONSE_1_0__PADDING_LENGTH];
} TURFormatStatusResponse_1_0_t;

#ifndef SBF_TURFORMATOPTIONA_1_0__PADDING_LENGTH 
#define SBF_TURFORMATOPTIONA_1_0__PADDING_LENGTH 2 
#endif

typedef struct
{
  uint16_t       uWN1;         
  uint32_t       uTOW1;        
  uint8_t        uOptionA_1;   
  uint16_t       uWN2;         
  uint32_t       uTOW2;        
  uint8_t        uOptionA_2;   

  uint8_t        _padding[SBF_TURFORMATOPTIONA_1_0__PADDING_LENGTH];
} TURFormatOptionA_1_0_t;

#ifndef SBF_TURFORMATOPTIONB_1_0_UMASK1_LENGTH 
#define SBF_TURFORMATOPTIONB_1_0_UMASK1_LENGTH 5 
#endif

#ifndef SBF_TURFORMATOPTIONB_1_0_UMASK2_LENGTH 
#define SBF_TURFORMATOPTIONB_1_0_UMASK2_LENGTH 5 
#endif

typedef struct
{
  uint8_t        uMask1[SBF_TURFORMATOPTIONB_1_0_UMASK1_LENGTH];
  uint16_t       uWN1;         
  uint32_t       uTOW1;        
  uint8_t        uOptionB_1;   
  uint8_t        uMask2[SBF_TURFORMATOPTIONB_1_0_UMASK2_LENGTH];
  uint16_t       uWN2;         
  uint32_t       uTOW2;        
  uint8_t        uOptionB_2;   
} TURFormatOptionB_1_0_t;

#define SBF_TURFORMAT_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        Reserved[2];  
  uint8_t        Type;         
  uint8_t        Payload_Length;
  uint16_t       Message_Count;
  TURFormatMode1RGCrequest_1_0_t Message_Mode_1_RCG_Request;
  TURFormatRangingCodeReady_1_0_t Message_Ranging_Code_Ready;
  TURFormatMode2RGCrequest_1_0_t Message_Mode_2_RCG_Request;
  TURFormatGetMode2Time_1_0_t Message_Get_Mode_2_Time;
  TURFormatMode2TimeReply_1_0_t Message_Mode_2_Time_Reply;
  TURFormatRangingCodeRequest_1_0_t Message_Ranging_Code_Request;
  TURFormatEncryptedData_1_0_t Message_Encrypted_Data;
  TURFormatDecryptedData_1_0_t Message_Decrypted_Data;
  TURFormatPVT_1_0_t Message_PVT;  
  TURFormatDenial_1_0_t Message_Denial;
  TURFormatEventLog_1_0_t Message_Event_Log;
  TURFormatVersionResponse_1_0_t Message_Version_Response;
  TURFormatStatusResponse_1_0_t Message_Status_Response;
  TURFormatOptionA_1_0_t OptionA;      
  TURFormatOptionB_1_0_t OptionB;      
} TURFormat_1_0_t;

#define SBF_TURFORMATMODE1RGCREQUEST_1__PADDING_LENGTH SBF_TURFORMATMODE1RGCREQUEST_1_0__PADDING_LENGTH
typedef TURFormatMode1RGCrequest_1_0_t TURFormatMode1RGCrequest_1_t;

#define SBF_TURFORMATRANGINGCODEREADY_1__PADDING_LENGTH SBF_TURFORMATRANGINGCODEREADY_1_0__PADDING_LENGTH
typedef TURFormatRangingCodeReady_1_0_t TURFormatRangingCodeReady_1_t;

typedef TURFormatMode2RGCrequest_1_0_t TURFormatMode2RGCrequest_1_t;

#define SBF_TURFORMATGETMODE2TIME_1__PADDING_LENGTH SBF_TURFORMATGETMODE2TIME_1_0__PADDING_LENGTH
typedef TURFormatGetMode2Time_1_0_t TURFormatGetMode2Time_1_t;

#define SBF_TURFORMATMODE2TIMEREPLY_1__PADDING_LENGTH SBF_TURFORMATMODE2TIMEREPLY_1_0__PADDING_LENGTH
typedef TURFormatMode2TimeReply_1_0_t TURFormatMode2TimeReply_1_t;

#define SBF_TURFORMATRANGINGCODEREQUEST_1__PADDING_LENGTH SBF_TURFORMATRANGINGCODEREQUEST_1_0__PADDING_LENGTH
typedef TURFormatRangingCodeRequest_1_0_t TURFormatRangingCodeRequest_1_t;

#define SBF_TURFORMATENCRYPTEDDATA_1_UDATA_LENGTH SBF_TURFORMATENCRYPTEDDATA_1_0_UDATA_LENGTH
#define SBF_TURFORMATENCRYPTEDDATA_1__PADDING_LENGTH SBF_TURFORMATENCRYPTEDDATA_1_0__PADDING_LENGTH
typedef TURFormatEncryptedData_1_0_t TURFormatEncryptedData_1_t;

#define SBF_TURFORMATDECRYPTEDDATA_1_UDATA_LENGTH SBF_TURFORMATDECRYPTEDDATA_1_0_UDATA_LENGTH
#define SBF_TURFORMATDECRYPTEDDATA_1__PADDING_LENGTH SBF_TURFORMATDECRYPTEDDATA_1_0__PADDING_LENGTH
typedef TURFormatDecryptedData_1_0_t TURFormatDecryptedData_1_t;

typedef TURFormatPVT_1_0_t TURFormatPVT_1_t;

#define SBF_TURFORMATDENIAL_1__PADDING_LENGTH SBF_TURFORMATDENIAL_1_0__PADDING_LENGTH
typedef TURFormatDenial_1_0_t TURFormatDenial_1_t;

typedef TURFormatEventLog_1_0_t TURFormatEventLog_1_t;

typedef TURFormatVersionResponse_1_0_t TURFormatVersionResponse_1_t;

#define SBF_TURFORMATSTATUSRESPONSE_1__PADDING_LENGTH SBF_TURFORMATSTATUSRESPONSE_1_0__PADDING_LENGTH
typedef TURFormatStatusResponse_1_0_t TURFormatStatusResponse_1_t;

#define SBF_TURFORMATOPTIONA_1__PADDING_LENGTH SBF_TURFORMATOPTIONA_1_0__PADDING_LENGTH
typedef TURFormatOptionA_1_0_t TURFormatOptionA_1_t;

#define SBF_TURFORMATOPTIONB_1_UMASK1_LENGTH SBF_TURFORMATOPTIONB_1_0_UMASK1_LENGTH
#define SBF_TURFORMATOPTIONB_1_UMASK2_LENGTH SBF_TURFORMATOPTIONB_1_0_UMASK2_LENGTH
typedef TURFormatOptionB_1_0_t TURFormatOptionB_1_t;

typedef TURFormat_1_0_t TURFormat_1_t;

/*--CalibrationValues_1_0_t : -----------------------------------------------*/
/* Calibration values */

typedef struct
{
  uint8_t        SignalType;   
  uint8_t        Reserved[3];  
  int32_t        ManualValue;  
  int32_t        MeasuredValue;
} CalibrationInfoType_1_0_t;

#ifndef SBF_CALIBRATIONVALUES_1_0_CALIBRATIONINFO_LENGTH 
#define SBF_CALIBRATIONVALUES_1_0_CALIBRATIONINFO_LENGTH 32 
#endif

#define SBF_CALIBRATIONVALUES_1_0_TOW_PRECISION 3
#define SBF_CALIBRATIONVALUES_1_0_TOW_LAST_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        Flags;        
  uint8_t        Reserved;     
  uint32_t       TOW_last;     
  uint16_t       WNc_last;     
  uint8_t        N;            
  uint8_t        SBLength;     
  CalibrationInfoType_1_0_t CalibrationInfo[SBF_CALIBRATIONVALUES_1_0_CALIBRATIONINFO_LENGTH];
} CalibrationValues_1_0_t;

typedef CalibrationInfoType_1_0_t CalibrationInfoType_1_t;

#define SBF_CALIBRATIONVALUES_1_CALIBRATIONINFO_LENGTH SBF_CALIBRATIONVALUES_1_0_CALIBRATIONINFO_LENGTH
typedef CalibrationValues_1_0_t CalibrationValues_1_t;

/*--MultipathMonitor_1_0_t : ------------------------------------------------*/
/* MultipathMonitorSub blocks */

typedef struct
{
  uint8_t        RxChannel;    
  uint8_t        Type;         
  uint8_t        SVID;         
  uint8_t        Reserved;     
  float          MPEstimate;   
} MultipathMonitorSubType_1_0_t;

#ifndef SBF_MULTIPATHMONITOR_1_0_MULTIPATHMONITORSUB_LENGTH 
#define SBF_MULTIPATHMONITOR_1_0_MULTIPATHMONITORSUB_LENGTH 72 
#endif

#define SBF_MULTIPATHMONITOR_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        N;            
  uint8_t        SBLength;     
  MultipathMonitorSubType_1_0_t MultipathMonitorSub[SBF_MULTIPATHMONITOR_1_0_MULTIPATHMONITORSUB_LENGTH];
} MultipathMonitor_1_0_t;

typedef MultipathMonitorSubType_1_0_t MultipathMonitorSubType_1_t;

#define SBF_MULTIPATHMONITOR_1_MULTIPATHMONITORSUB_LENGTH SBF_MULTIPATHMONITOR_1_0_MULTIPATHMONITORSUB_LENGTH
typedef MultipathMonitor_1_0_t MultipathMonitor_1_t;


/*==PinPoint-GIS RX==========================================================*/

/*--GISAction_1_0_t : -------------------------------------------------------*/
/* PinPoint-GIS RX Action */

#ifndef SBF_GISACTION_1_0_COMMENT_LENGTH 
#define SBF_GISACTION_1_0_COMMENT_LENGTH 250 
#endif

#ifndef SBF_GISACTION_1_0__PADDING_LENGTH 
#define SBF_GISACTION_1_0__PADDING_LENGTH 2 
#endif

#define SBF_GISACTION_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint16_t       CommentLn;    
  uint32_t       ItemIDMSB;    
  uint32_t       ItemIDLSB;    
  uint8_t        Action;       
  uint8_t        Trigger;      
  uint8_t        Database;     
  uint8_t        Reserved;     
  char           Comment[SBF_GISACTION_1_0_COMMENT_LENGTH];

  uint8_t        _padding[SBF_GISACTION_1_0__PADDING_LENGTH];
} GISAction_1_0_t;

#define SBF_GISACTION_1_COMMENT_LENGTH SBF_GISACTION_1_0_COMMENT_LENGTH
#define SBF_GISACTION_1__PADDING_LENGTH SBF_GISACTION_1_0__PADDING_LENGTH
typedef GISAction_1_0_t GISAction_1_t;

/*--GISStatus_1_0_t : -------------------------------------------------------*/
/* Status of the different PinPoint-GIS collection databases */

typedef struct
{
  uint8_t        Database;     
  uint8_t        OnlineStatus; 
  uint8_t        Error;        
  uint8_t        Reserved;     
  uint32_t       NrItems;      
  uint32_t       NrNotSync;    
} DatabaseStatus_1_0_t;

#ifndef SBF_GISSTATUS_1_0_DATABASESTATUS_LENGTH 
#define SBF_GISSTATUS_1_0_DATABASESTATUS_LENGTH 3 
#endif

#define SBF_GISSTATUS_1_0_TOW_PRECISION 3
typedef struct
{
  BlockHeader_t  Header;       

  /* Time Header */
  uint32_t       TOW;          
  uint16_t       WNc;          

  uint8_t        N;            
  uint8_t        SBLength;     
  DatabaseStatus_1_0_t DatabaseStatus[SBF_GISSTATUS_1_0_DATABASESTATUS_LENGTH];
} GISStatus_1_0_t;

typedef DatabaseStatus_1_0_t DatabaseStatus_1_t;

#define SBF_GISSTATUS_1_DATABASESTATUS_LENGTH SBF_GISSTATUS_1_0_DATABASESTATUS_LENGTH
typedef GISStatus_1_0_t GISStatus_1_t;


// compatibility definitions follow

#define sbfnr_GALSar_1  sbfnr_GALSARRLM_1
#define sbfid_GALSar_1_0  sbfid_GALSARRLM_1_0

#define sbfnr_PvtSatCartesian_1  sbfnr_PVTSatCartesian_1
#define sbfid_PvtSatCartesian_1_0  sbfid_PVTSatCartesian_1_0
#define sbfid_PvtSatCartesian_1_1  sbfid_PVTSatCartesian_1_1

#define sbfnr_GeoCorrections_1  sbfnr_GEOCorrections_1
#define sbfid_GeoCorrections_1_0  sbfid_GEOCorrections_1_0

#define sbfnr_LBAS1Message_1   sbfnr_LBAS1Messages_1
#define sbfid_LBAS1Message_1_0   sbfid_LBAS1Messages_1_0

#define sbfnr_LbandBeams_1  sbfnr_LBandBeams_1
#define sbfid_LbandBeams_1_0  sbfid_LBandBeams_1_0

/*==SBF-BLOCKS definition====================================================*/


/*--Header-VoidBlock definition ---------------------------------------------*/

typedef BlockHeader_t HeaderBlock_t;
typedef TimeHeader_t HeaderAndTimeBlock_t;

typedef HeaderBlock_t VoidBlock_t;


/*==Measurement Blocks=======================================================*/

/*--GenMeasEpoch_1_0_t : ----------------------------------------------------*/
#define MAX_MEASEPOCH_DATA SBF_GENMEASEPOCH_1_0_DATA_LENGTH


/*--MeasExtra_1_0_t : -------------------------------------------------------*/
#define MAXSB_MEASEXTRA SBF_MEASEXTRA_1_0_MEASEXTRACHANNEL_LENGTH

typedef MeasExtraChannelSub_1_0_t  MeasExtraChannel_1_0_t;


/*--MeasExtra_1_1_t : -------------------------------------------------------*/
typedef MeasExtraChannelSub_1_1_t  MeasExtraChannel_1_1_t;


/*--MeasExtra_1_2_t : -------------------------------------------------------*/
typedef MeasExtraChannelSub_1_2_t  MeasExtraChannel_1_2_t;

/*--MeasExtra_1_3_t : -------------------------------------------------------*/
typedef MeasExtraChannelSub_1_3_t  MeasExtraChannel_1_3_t;

typedef MeasExtraChannelSub_1_t  MeasExtraChannel_1_t;


/*--IQCorr_1_0_t : ----------------------------------------------------------*/
#define MAXSB_IQCORR  SBF_IQCORR_1_0_CORRCHANNEL_LENGTH

typedef IQCorrChannelSub_1_0_t CorrChannel_1_0_t;


/*--IQCorr_1_1_t : ----------------------------------------------------------*/
typedef IQCorrChannelSub_1_1_t CorrChannel_1_1_t;

typedef IQCorrChannelSub_1_t CorrChannel_1_t;


/*==Navigation Page Blocks===================================================*/

/*--GPSRaw_1_0_t : ----------------------------------------------------------*/
#define GPS_NR_OF_RAW_BITS   SBF_GPSRAW_1_0_RAWBITS_LENGTH


/*--CNAVRaw_1_0_t : ---------------------------------------------------------*/
#define CNAV_NR_OF_RAW_BITS  SBF_CNAVRAW_1_0_CNAVBITS_LENGTH


/*--GEORaw_1_0_t : ----------------------------------------------------------*/
#define GEO_NR_OF_RAW_BITS   SBF_GEORAW_1_0_RAWBITS_LENGTH


/*--GPSRawCA_1_0_t : --------------------------------------------------------*/
#define NB_NAVBITS_WORDS_GPSCA    SBF_GPSRAWCA_1_0_NAVBITS_LENGTH

typedef GPSRawCA_1_0_t NavBits_1_0_t;
typedef NavBits_1_0_t NavBits_1_t;

/*--GPSRawL2C_1_0_t : -------------------------------------------------------*/
#define NB_NAVBITS_WORDS_GPSL2C  SBF_GPSRAWL2C_1_0_NAVBITS_LENGTH


/*--GLORawCA_1_0_t : --------------------------------------------------------*/
#define NB_NAVBITS_WORDS_GLOCA  SBF_GLORAWCA_1_0_NAVBITS_LENGTH


/*--GALRawFNAV_1_0_t : ------------------------------------------------------*/
#define NB_NAVBITS_WORDS_GALFNAV  SBF_GALRAWFNAV_1_0_NAVBITS_LENGTH


/*--GALRawINAV_1_0_t : ------------------------------------------------------*/
#define NB_NAVBITS_WORDS_GALINAV  SBF_GALRAWINAV_1_0_NAVBITS_LENGTH


/*--GALRawCNAV_1_0_t : ------------------------------------------------------*/
#define NB_NAVBITS_WORDS_GALCNAV  SBF_GALRAWCNAV_1_0_NAVBITS_LENGTH


/*--GALRawGNAV_1_0_t : ------------------------------------------------------*/
#define NB_NAVBITS_WORDS_GALGNAV  SBF_GALRAWGNAV_1_0_NAVBITS_LENGTH


/*--GALRawGNAVe_1_0_t : ------------------------------------------------------*/
#define NB_NAVBITS_WORDS_GALGNAVE SBF_GALRAWGNAVE_1_0_NAVBITS_LENGTH


/*--GEORawL1_1_0_t : --------------------------------------------------------*/
#define NB_NAVBITS_WORDS_GEOL1  SBF_GEORAWL1_1_0_NAVBITS_LENGTH


/*--CMPRaw_1_0_t : ----------------------------------------------------------*/
#define NB_NAVBITS_WORDS_CMP  SBF_BDSRAW_1_0_NAVBITS_LENGTH

/* For backwards compatibility after renaming CMP to BDS */
typedef BDSRaw_1_0_t CMPRaw_1_0_t;
typedef BDSRaw_1_t   CMPRaw_1_t;  
typedef BDSNav_1_0_t CMPNav_1_0_t;
typedef BDSNav_1_t   CMPNav_1_t;  
#define SBF_CMPRAW_1_NAVBITS_LENGTH   SBF_BDSRAW_1_NAVBITS_LENGTH
#define SBF_CMPRAW_1_0_NAVBITS_LENGTH SBF_BDSRAW_1_0_NAVBITS_LENGTH

#define sbfnr_CMPRaw_1   sbfnr_BDSRaw_1
#define sbfid_CMPRaw_1_0 sbfid_BDSRaw_1_0
#define sbfnr_CMPNav_1   sbfnr_BDSNav_1
#define sbfid_CMPNav_1_0 sbfid_BDSNav_1_0

/*==Galileo Decoded Message Blocks===========================================*/


/*--GALSARRLM_1_0_t : -------------------------------------------------------*/
#define MAX_GAL_SAR_RLM_DATA  SBF_GASARRLM_1_0_RLMBITS_LENGTH


/*==SBAS Decoded Message Blocks==============================================*/

/*--GEOPRNMask_1_0_t : ------------------------------------------------------*/
#define MAXSB_SBAS_NRCORRSAT SBF_RAPRNMASK_1_0_PRNMASK_LENGTH
// same as SBF_GEOINTEGRITY_1_0_UDREI_LENGTH
// same as SBF_GEOFASTCORRDEGR_1_0_AI_LENGTH

/*--GEOIntegrity_1_0_t : ----------------------------------------------------*/
#define MAXSB_SBAS_NRIODF  SBF_RAINTEGRITY_1_0_IODF_LENGTH


/*--GEOServiceLevel_1_0_t : -------------------------------------------------*/
typedef ServiceRegion_1_0_t  raServiceRegion_1_0_t;
typedef ServiceRegion_1_t  raServiceRegion_1_t;


/*==Position, Velocity and Time Blocks=======================================*/


/*--PVTSatCartesian_1_0_t : -------------------------------------------------*/
#define MAXSB_SATPOS  SBF_PVTSATCARTESIAN_1_0_SATPOS_LENGTH


/*--PVTResiduals_1_0_t : ----------------------------------------------------*/
#define MAXSB_SATRESIDUAL SBF_PVTRESIDUALS_1_0_SATRESIDUAL_LENGTH

typedef PVTResidual_1_0_t  SatResidual_1_0_t;
typedef PVTResidual_1_t  SatResidual_1_t;


/*--PVTResiduals_2_0_t : ----------------------------------------------------*/
#define MAX_PVTRES_DATA  SBF_PVTRESIDUALS_2_0_DATA_LENGTH

typedef ResidualInfoCode_2_0_t SatResInfo_2_0_t;
// same as ResidualInfoPhase_2_0_t and ResidualInfoDoppler_2_0_t


/*--PVTResiduals_2_1_t : ----------------------------------------------------*/
typedef ResidualInfoCode_2_1_t SatResInfo_2_1_t;
// same as ResidualInfoPhase_2_1_t and ResidualInfoDoppler_2_1_t
typedef SatResInfo_2_1_t    SatResInfo_2_t;


/*--RAIMStatistics_1_0_t : --------------------------------------------------*/
#define MAXSB_RAIM  SBF_RAIMSTATISTICS_1_0_RAIMCHANNEL_LENGTH

typedef RAIMSatData_1_0_t  RAIMStatChannel_1_0_t;
typedef RAIMSatData_1_t  RAIMStatChannel_1_t;


/*--GEOCorrections_1_0_t : --------------------------------------------------*/
#define MAXSB_GEOCORRCHANNEL  SBF_GEOCORRECTIONS_1_0_GEOCORRCHANNEL_LENGTH

typedef SatCorr_1_0_t  GeoCorrChannel_1_0_t;
typedef SatCorr_1_t  GeoCorrChannel_1_t;
typedef GEOCorrections_1_0_t GeoCorrections_1_0_t;
typedef GEOCorrections_1_t   GeoCorrections_1_t;


/*--BaseVectorCart_1_0_t : --------------------------------------------------*/
#define MAXSB_VECTORINFO SBF_BASEVECTORCART_1_0_VECTORINFO_LENGTH
// same as SBF_BASEVECTORGEOD_1_0_VECTORINFOGEOD_LENGTH


/*==INS/GNSS Integrated Blocks===============================================*/


/*--IntPVCart_1_0_t : -------------------------------------------------------*/
typedef IntPVCart_1_0_t  intPVCart_1_0_t;
typedef IntPVCart_1_t  intPVCart_1_t;


/*--IntPVGeod_1_0_t : -------------------------------------------------------*/
typedef IntPVGeod_1_0_t  intPVGeod_1_0_t;
typedef IntPVGeod_1_t  intPVGeod_1_t;


/*--IntPosCovCart_1_0_t : ---------------------------------------------------*/
typedef IntPosCovCart_1_0_t  intPosCovCart_1_0_t;
typedef IntPosCovCart_1_t  intPosCovCart_1_t;


/*--IntVelCovCart_1_0_t : ---------------------------------------------------*/
typedef IntVelCovCart_1_0_t  intVelCovCart_1_0_t;
typedef IntVelCovCart_1_t  intVelCovCart_1_t;


/*--IntPosCovGeod_1_0_t : ---------------------------------------------------*/
typedef IntPosCovGeod_1_0_t  intPosCovGeod_1_0_t;
typedef IntPosCovGeod_1_t  intPosCovGeod_1_t;


/*--IntVelCovGeod_1_0_t : ---------------------------------------------------*/
typedef IntVelCovGeod_1_0_t  intVelCovGeod_1_0_t;
typedef IntVelCovGeod_1_t  intVelCovGeod_1_t;


/*--IntAttEuler_1_0_t : -----------------------------------------------------*/
typedef IntAttEuler_1_0_t  intAttEuler_1_0_t;
typedef IntAttEuler_1_t  intAttEuler_1_t;


/*--IntAttCovEuler_1_0_t : --------------------------------------------------*/
typedef IntAttCovEuler_1_0_t  intAttCovEuler_1_0_t;
typedef IntAttCovEuler_1_t  intAttCovEuler_1_t;


/*--IntPVAAGeod_1_0_t : -----------------------------------------------------*/
typedef IntPVAAGeod_1_0_t  intPVAAGeod_1_0_t;
typedef IntPVAAGeod_1_t  intPVAAGeod_1_t;


/*==GNSS Attitude Blocks=====================================================*/


/*--AuxAntPositions_1_0_t : -------------------------------------------------*/
typedef AuxAntPositionSub_1_0_t  AuxAntPosData_1_0_t;
typedef AuxAntPositionSub_1_t  AuxAntPosData_1_t;


/*==Differential Correction Blocks===========================================*/


/*--RTCMDatum_1_0_t : -------------------------------------------------------*/
#define RTCMDATUM_CRS_NAME_LENGTH SBF_RTCMDATUM_1_0_SOURCECRS_LENGTH
// same as SBF_RTCMDATUM_1_0_TARGETCRS_LENGTH


/*==L-Band Demodulator Blocks================================================*/


/*--LBandTrackerStatus_1_0_t : ----------------------------------------------*/
#define MAX_NROF_LBANDTRACKERDATA  SBF_LBANDTRACKERSTATUS_1_0_TRACKDATA_LENGTH

typedef TrackData_1_0_t  LBandTrackData_1_0_t;


/*--LBandTrackerStatus_1_1_t : ----------------------------------------------*/
typedef TrackData_1_1_t  LBandTrackData_1_1_t;


/*--LBandTrackerStatus_1_1_2 : ----------------------------------------------*/
typedef TrackData_1_2_t  LBandTrackData_1_2_t;


/*--LBandTrackerStatus_1_1_3 : ----------------------------------------------*/
typedef TrackData_1_3_t  LBandTrackData_1_3_t;
typedef TrackData_1_t  LBandTrackData_1_t;


/*--LBAS1DecoderStatus_1_1_t : ----------------------------------------------*/
#define LBAS1_PAC_LENGTH  SBF_LBAS1DECODERSTATUS_1_1_PAC_LENGTH


/*--LBAS1Message_1_0_t : ----------------------------------------------------*/
#define MAX_LBAS1MSG_LEN  SBF_LBAS1MESSAGES_1_0_MESSAGE_LENGTH

typedef LBAS1Messages_1_0_t LBAS1Message_1_0_t;
typedef LBAS1Messages_1_t  LBAS1Message_1_t;


/*--LBandBeams_1_0_t : ------------------------------------------------------*/
#define MAX_LBANDSAT  SBF_BEAMINFO_1_0_SATNAME_LENGTH

typedef BeamInfo_1_0_t  LBandBeamInfo_1_0_t;
typedef BeamInfo_1_t  LBandBeamInfo_1_t;


/*==External Sensor Blocks===================================================*/


/*--ExtSensorMeas_1_0_t : ---------------------------------------------------*/
#define MAXSB_EXTSENSORMEAS  SBF_EXTSENSORMEAS_1_0_EXTSENSORMEAS_LENGTH

typedef ExtSensorMeasSet_1_0_t  ExtSensorMeasSB_1_0_t;
typedef ExtSensorMeasSet_1_t  ExtSensorMeasSB_1_t;


/*--ExtSensorStatus_1_0_t : -------------------------------------------------*/
#define EXTSENSORSTATUS_STATUSBITS_SIZE  SBF_EXTSENSORSTATUS_1_0_STATUSBITS_LENGTH


/*--ExtSensorSetup_1_0_t : --------------------------------------------------*/
#define MAXSB_EXTSENSORSETUP  SBF_EXTSENSORSETUP_1_0_EXTSENSORSETUP_LENGTH


/*--ExtSensorStatus_2_0_t : -------------------------------------------------*/
#define EXTSENSORSTATUS2_STATUSBITS_SIZE  SBF_EXTSENSORSTATUS_2_0_STATUSBITS_LENGTH


/*--ExtSensorInfo_1_0_t : ---------------------------------------------------*/
#define EXTSENSORINFO_STATUSBITS_SIZE  SBF_EXTSENSORINFO_1_0_STATUSBITS_LENGTH


typedef OneSensor_1_0_t  ExtSensorSetupData_1_0_t;


/*--ExtSensorSetup_1_1_t : --------------------------------------------------*/
typedef OneSensor_1_1_t  ExtSensorSetupData_1_1_t;
typedef OneSensor_1_t    ExtSensorSetupData_1_t;

/*--ExtSensorSetup_1_2_t : --------------------------------------------------*/
typedef OneSensor_1_2_t  ExtSensorSetupData_1_2_t;

/*==Status Blocks============================================================*/


/*--TrackingStatus_1_0_t : --------------------------------------------------*/
#define MAXSB_TRACKINGSTATUS  SBF_TRACKINGSTATUS_1_0_CHANNELDATA_LENGTH


/*--ChannelStatus_1_0_t : ---------------------------------------------------*/
#define MAXSB_CHANNELSTS  SBF_CHANNELSTATUS_1_0_DATA_LENGTH


/*--ReceiverStatus_2_0_t : --------------------------------------------------*/
#define MAXSB_AGCSTATE SBF_RECEIVERSTATUS_2_0_AGCSTATE_LENGTH


/*--SatVisibility_1_0_t : ---------------------------------------------------*/
#define MAXSB_SATVISIBILITY  SBF_SATVISIBILITY_1_0_SATINFO_LENGTH


/*--InputLink_1_0_t : -------------------------------------------------------*/
#define MAX_INPUTSTATS_DATA  SBF_INPUTLINK_1_0_DATA_LENGTH

typedef InputStatsSub_1_0_t  InputStats_1_0_t;
typedef InputStatsSub_1_t  InputStats_1_t;


/*--OutputLink_1_0_t : ------------------------------------------------------*/
#define MAX_OUTPUTSTATS_DATA_1_0  SBF_OUTPUTLINK_1_0_DATA_LENGTH

typedef OutputTypeSub_1_0_t  OutputTypes_1_0_t;
typedef OutputStatsSub_1_0_t  OutputStats_1_0_t;


/*--OutputLink_1_1_t : ------------------------------------------------------*/
#define MAX_OUTPUTSTATS_DATA_1_1  SBF_OUTPUTLINK_1_1_DATA_LENGTH

typedef OutputTypeSub_1_1_t  OutputTypes_1_1_t;
typedef OutputStatsSub_1_1_t  OutputStats_1_1_t;
typedef OutputTypeSub_1_t  OutputTypes_1_t;
typedef OutputStatsSub_1_t  OutputStats_1_t;


/*--IPStatus_1_0_t : --------------------------------------------------------*/
#define MAC_ADDR_LENGTH  SBF_IPSTATUS_1_0_MACADDRESS_LENGTH
#define IP_ADDR_LENGTH  SBF_IPSTATUS_1_0_IPADDRESS_LENGTH
// same as SBF_IPSTATUS_1_0_GATEWAY_LENGTH


/*==Miscellaneous Blocks=====================================================*/


/*--ReceiverSetup_1_0_t : ---------------------------------------------------*/
#define MARKERNAME_LENGTH  SBF_RECEIVERSETUP_1_0_MARKERNAME_LENGTH
#define MARKERNBR_LENGTH  SBF_RECEIVERSETUP_1_0_MARKERNUMBER_LENGTH
#define OBSERVER_LENGTH  SBF_RECEIVERSETUP_1_0_OBSERVER_LENGTH
#define AGENCY_LENGTH  SBF_RECEIVERSETUP_1_0_AGENCY_LENGTH
#define RXSERIALNBR_LENGTH SBF_RECEIVERSETUP_1_0_RXSERIALNBR_LENGTH
#define RXNAME_LENGTH  SBF_RECEIVERSETUP_1_0_RXNAME_LENGTH
#define RXVERSION_LENGTH  SBF_RECEIVERSETUP_1_0_RXVERSION_LENGTH
#define ANTSERIALNBR_LENGTH  SBF_RECEIVERSETUP_1_0_ANTSERIALNBR_LENGTH
#define ANTTYPE_LENGTH  SBF_RECEIVERSETUP_1_0_ANTTYPE_LENGTH


/*--ReceiverSetup_1_1_t : ---------------------------------------------------*/
#define MARKERTYPE_LENGTH  SBF_RECEIVERSETUP_1_1_MARKERTYPE_LENGTH


/*--ReceiverSetup_1_2_t : ---------------------------------------------------*/
#define GNSSFWVERSION_LENGTH  SBF_RECEIVERSETUP_1_2_GNSSFWVERSION_LENGTH


/*--ReceiverSetup_1_3_t : ---------------------------------------------------*/
#define PRODUCTNAME_LENGTH  SBF_RECEIVERSETUP_1_3_PRODUCTNAME_LENGTH


/*--ReceiverSetup_1_4_t : ---------------------------------------------------*/
#define STATIONCODE_LENGTH  SBF_RECEIVERSETUP_1_4_STATIONCODE_LENGTH
#define COUNTRYCODE_LENGTH  SBF_RECEIVERSETUP_1_4_COUNTRYCODE_LENGTH


/*--Commands_1_0_t : --------------------------------------------------------*/
#define COMMANDS_LENGTH  SBF_COMMANDS_1_0_CMDDATA_LENGTH


/*--Comment_1_0_t : ---------------------------------------------------------*/
#define COMMENT_LENGTH  SBF_COMMENT_1_0_COMMENT_LENGTH


/*--BBSamples_1_0_t : -------------------------------------------------------*/
#define MAX_BBSAMPLES  SBF_BBSAMPLESDATA_1_0_SAMPLES_LENGTH


/*--ASCIIIn_1_0_t : ---------------------------------------------------------*/
#define ASCII_STRING_LENGTH  SBF_ASCIIIN_1_0_ASCIISTRING_LENGTH


/*==TUR Specific Blocks======================================================*/


/*--TURPVTSatCorrections_1_0_t : ---------------------------------------------*/
#define MAX_PVT_SATCORRECTIONS_DATA  SBF_TURPVTSATCORRECTIONS_1_0_DATA_LENGTH


/*--TURHPCAInfo_1_0_t : -----------------------------------------------------*/
#define MAXSB_SATHPCA  SBF_TURHPCAINFO_1_0_HPCADATA_LENGTH

typedef SatHpca_1_0_t  SatHPCA_1_0_t;
typedef SatHpca_1_t  SatHPCA_1_t;


/*--CorrPeakSample_1_0_t : --------------------------------------------------*/
#define MAXSB_CORRSAMPLE  SBF_CORRPEAKSAMPLE_1_0_CORRSAMPLEDATA_LENGTH


/*--GALIntegrity_1_0_t : ----------------------------------------------------*/
#define MAX_GALINTEGRITY  SBF_GAINTEGRITY_1_0_INTEGRITYSVI_LENGTH


/*--SysTimeOffset_1_0_t : ---------------------------------------------------*/
#define MAXSB_TIMEOFFSET  SBF_SYSTIMEOFFSET_1_0_TIMEOFFSET_LENGTH

typedef TimeOffsetSub_1_0_t  TimeOffset_1_0_t;

/*--SysTimeOffset_1_1_t : ---------------------------------------------------*/
#define MAXSB_TIMEOFFSET  SBF_SYSTIMEOFFSET_1_0_TIMEOFFSET_LENGTH

typedef TimeOffsetSub_1_1_t  TimeOffset_1_1_t;
typedef TimeOffsetSub_1_t  TimeOffset_1_t;


/*==Other Blocks=============================================================*/

/*--sbsAlm_1_0_t : -----------------------------------------------------*/
typedef sbsAlm_1_0_t SbasL5Alm_t;
typedef sbsEph_1_0_t SbasL5Eph_t;

/*--GALRawGNAVe_1_0_t : -----------------------------------------------------*/
typedef GALRawGNAVe_1_0_t GALRawGNAVencrypted_1_0_t;

/*--TrackerDatat_1_0_t : ----------------------------------------------------*/
#define SBF_TRACKERDATA_MAXSIZE  SBF_TRACKERDATA_1_0_DATA_LENGTH
#define SBF_TRACKERDATA_MAXMSGS  SBF_TRACKERDATA_1_0_SUBMSGINDEX_LENGTH

/* These typedefs are added for backwards compatibility reasons.
   Please do not use them in new development.
 */
typedef MeasEpochChannelType2_1_0_t MeasEpochChannelType2_t;
typedef MeasEpochChannelType1_1_0_t MeasEpochChannelType1_t;
typedef MeasExtraChannel_1_0_t MeasExtraChannel_t;
typedef gpEph_1_0_t gpEphLNAV_t;
typedef gpEphCNAV_1_0_t gpEphCNAV_t;
typedef gpAlm_1_0_t gpAlm_t;
typedef gpIon_1_0_t gpIon_t;
typedef gpUtc_1_0_t gpUtc_t;
typedef glEph_1_0_t glEph_t;
typedef glAlm_1_0_t glAlm_t;
typedef glTime_1_0_t glTime_t;
typedef gaEph_1_0_t galEph_t;
typedef gaAlm_1_0_t galAlm_t;
typedef gaIon_1_0_t galIon_t;
typedef gaUtc_1_0_t galUtc_t;
typedef gaGstGps_1_0_t galGstGps_t;
typedef cmpEph_1_0_t cmpEph_t;
typedef cmpIon_1_0_t cmpIon_t;
typedef cmpUtc_1_0_t cmpUtc_t;
typedef raMT00_1_0_t GeoMT00_t;
typedef raPRNMask_1_0_t GeoPRNMask_t;
typedef FastCorr_1_0_t FastCorr_t;
typedef raFastCorr_1_0_t GeoFastCorr_t;
typedef raIntegrity_1_0_t GeoIntegrity_t;
typedef raFastCorrDegr_1_0_t GeoFastCorrDegr_t;
typedef raEph_1_0_t raEph_t;
typedef raDF_1_0_t raDF_t;
typedef raDegrFactors_1_0_t GeoDegrFactors_t;
typedef NetworkTimeMsg_1_0_t raNetworkTime_t;
typedef raNetworkTime_1_0_t GeoNetworkTime_t;
typedef raAlm_1_0_t raAlm_t;
typedef raIGPMask_1_0_t GeoIGPMask_t;
typedef LTCorr_1_0_t LTCorr_t;
typedef raLongTermCorr_1_0_t GeoLongTermCorr_t;
typedef IDC_1_0_t IDC_t;
typedef raIonoDelay_1_0_t GeoIonoDelay_t;
typedef raServiceRegion_1_0_t raServiceRegion_t;
typedef raServiceMsg_1_0_t raServiceLevel_t;
typedef raServiceLevel_1_0_t GeoServiceLevel_t;
typedef CovMatrix_1_0_t CovMatrix_t;
typedef raClockEphCovMatrix_1_0_t GeoClockEphCovMatrix_t;
typedef SatPos_1_0_t SatPos_t;
typedef SatResidual_1_0_t SatResidual_t;
typedef SatSignalInfo_2_0_t SatSignalInfo_t;
typedef SatResInfo_2_0_t SatResInfo_t;
typedef RAIMStatChannel_1_0_t RAIMStatChannel_t;
typedef GeoCorrChannel_1_0_t GeoCorrChannel_t;
typedef PPSData_1_0_t PPSData_t;
typedef TimerData_1_0_t TimerData_t;
typedef TrackingStatusChannel_1_0_t TrackingStatusChannel_t;
typedef ChannelSatInfo_1_0_t ChannelSatInfo_t;
typedef ChannelStateInfo_1_0_t ChannelStateInfo_t;
typedef AGCData_1_0_t AGCData_t;
typedef AGCState_2_0_t AGCState_t;
typedef SatInfo_1_0_t SatInfo_t;
typedef SatCorrInfo_1_0_t SatCorrInfo_t;
typedef SatClkCorrInfo_1_0_t SatClkCorrInfo_t;
typedef SatHPCA_1_0_t SatHPCA_t;
typedef CorrSample_1_0_t CorrSample_t;
typedef gaIntegrity_1_0_t galIntegrity_t;
typedef gaSARRLM_1_0_t galSARRLM_t;
typedef TimeOffset_1_1_t TimeOffset_t;
typedef ExtSensorMeasSB_1_0_t ExtSensorMeasSB_t;

/* put the packing alignment back to what it was */
#pragma pack(pop)

#endif
/* End of "ifndef SBFDEF_H" */
