//===============================================================================
//
// OpenDCC - BiDiB
//
// Copyright (c) 2010 Wolfgang Kufer
//
//-------------------------------------------------------------------------------
//
// file:      bidib_messages.h
// author:    Wolfgang Kufer
// contact:   kufer@gmx.de
// webpage:   http://www.opendcc.de
// history:   2010-12-01 V0.00 kw  start
//            2010-12-07 V0.01 kw  added BIDIB_PKT_MAGIC, MSB_BM_ACCESSORY, ...
//            2011-02-22       kw  added feature-codes for booster class
//            2011-03-01       kw  update error codes, added MSG_SYS_GET_ERROR
//            2011-03-30       kw  added full bytes for system messages
//            2011-04-11       kw  added MSG_BM_BLOCK_CV
//            2011-07-23       kw  added Booster features
//            2011-01-07 V0.02 kw  added local messages in own block, 
//                                 added MSG_BM_MIRROR_OCC, MSG_BM_MIRROR_FREE
//                                 moved MSG_BM_GET_ADDR_RANGE
//            2012-02-13 V0.03 kw  added Local Ping, removed BIDIBUS_LOGON_ACK
//                                 added BIDIBUS_BUSY
//            2012-03-06       kw  added messages for switch/light control, 
//                                       and command station
// 
//===============================================================================
//
// purpose:   common header for bidib protocol
//            (This should be used in any BiDiB implementation)
//
//===============================================================================
//
// content:   0. Defines for bidib general
//            1. Defines for downstream messages
//            2. Defines for upstream messages
//            3. type defines
//            4. feature codes
//            5. error codes
//            6. Defines for serial protocol / BiDiBus
//
//===============================================================================
// 
// known problems: messages for programming are not yet defined
//
//===============================================================================


#ifndef __BIDIB_MESSAGES_H__
#define __BIDIB_MESSAGES_H__

//===============================================================================
//
// 0. Defines for serial protocol / bidib general
//
//===============================================================================

//                              Mainversion   Subversion
#define BIDIB_VERSION           (0 * 256 +    3)

#define BIDIB_SYS_MAGIC         0xAFFE

//===============================================================================
//
// 1. Defines for downstream messages
//
//===============================================================================

#define MSG_DSTRM  0x00

//-- system messages
#define MSG_DSYS                (MSG_DSTRM + 0x00)
#define MSG_SYS_GET_MAGIC       (MSG_DSYS + 0x01)       // these must stay here
#define MSG_SYS_GET_P_VERSION   (MSG_DSYS + 0x02)
#define MSG_SYS_ENABLE          (MSG_DSYS + 0x03)
#define MSG_SYS_DISABLE         (MSG_DSYS + 0x04)
#define MSG_SYS_GET_UNIQUE_ID   (MSG_DSYS + 0x05)
#define MSG_SYS_GET_SW_VERSION  (MSG_DSYS + 0x06)
#define MSG_SYS_PING            (MSG_DSYS + 0x07)
#define MSG_SYS_IDENTIFY        (MSG_DSYS + 0x08)
#define MSG_SYS_RESET           (MSG_DSYS + 0x09)
#define MSG_GET_NODE_TAB        (MSG_DSYS + 0x0a)
#define MSG_GET_PKT_CAPACITY    (MSG_DSYS + 0x0b)
#define MSG_NODE_CHANGED_ACK    (MSG_DSYS + 0x0c)
#define MSG_SYS_GET_ERROR       (MSG_DSYS + 0x0d)

//-- feature and user config messages
#define MSG_DFC                 (MSG_DSTRM + 0x10)
#define MSG_FEATURE_GETALL      (MSG_DFC + 0x00)
#define MSG_FEATURE_GET         (MSG_DFC + 0x01)
#define MSG_FEATURE_SET         (MSG_DFC + 0x02)
#define MSG_VENDOR_ENABLE       (MSG_DFC + 0x03)
#define MSG_VENDOR_DISABLE      (MSG_DFC + 0x04)
#define MSG_VENDOR_SET          (MSG_DFC + 0x05)
#define MSG_VENDOR_GET          (MSG_DFC + 0x06)

//-- occupancy messages
#define MSG_DBM                 (MSG_DSTRM + 0x20)
#define MSG_BM_GET_RANGE        (MSG_DBM + 0x00)
#define MSG_BM_MIRROR_MULTIPLE  (MSG_DBM + 0x01)
#define MSG_BM_MIRROR_OCC       (MSG_DBM + 0x02)
#define MSG_BM_MIRROR_FREE      (MSG_DBM + 0x03)
#define MSG_BM_ADDR_GET_RANGE   (MSG_DBM + 0x04)

//-- booster messages
#define MSG_DBST                (MSG_DSTRM + 0x30)
#define MSG_BOOST_OFF           (MSG_DBST + 0x00)
#define MSG_BOOST_ON            (MSG_DBST + 0x01)

//-- switch/light control messages
#define MSG_DLC                 (MSG_DSTRM + 0x40)
#define MSG_LC_OUTPUT           (MSG_DLC + 0x00)
#define MSG_LC_CONFIG_SET       (MSG_DLC + 0x01)
#define MSG_LC_CONFIG_GET       (MSG_DLC + 0x02)
#define MSG_LC_KEY_QUERY        (MSG_DLC + 0x03)

//-- macro messages
#define MSG_DMAC                (MSG_DSTRM + 0x48)
#define MSG_LC_MACRO_HANDLE     (MSG_DMAC + 0x00)
#define MSG_LC_MACRO_SET        (MSG_DMAC + 0x01)
#define MSG_LC_MACRO_GET        (MSG_DMAC + 0x02)

//-- dcc gen messages
#define MSG_DGEN                (MSG_DSTRM + 0x60)
#define  MSG_CS_ALLOCATE        (MSG_DGEN + 0x00)
#define  MSG_CS_CONNECT         (MSG_DGEN + 0x01)
#define  MSG_CS_SET_STATE       (MSG_DGEN + 0x02)
#define  MSG_CS_GET_STATE       (MSG_DGEN + 0x03)    // könnte ev. entfallen
#define  MSG_CS_DRIVE           (MSG_DGEN + 0x04)
#define  MSG_CS_ACCESSORY       (MSG_DGEN + 0x05)

/*
#define MSG_PRG_METHOD
#define MSG_PRG_CV_WRITE
#define MSG_PRG_CV_BLOCK_WRITE
#define MSG_PRG_CV_READ 
#define MSG_PRG_CV_BLOCK_READ 
*/

//-- local message
#define MSG_DLOCAL              (MSG_DSTRM + 0x70)                           // only locally used
#define MSG_LOGON_ACK           (MSG_DSTRM + MSG_DLOCAL + 0x00) 
#define MSG_LOCAL_PING          (MSG_DSTRM + MSG_DLOCAL + 0x01)

//===============================================================================
//
// 2. Defines for upstream messages
//
//===============================================================================

#define MSG_USTRM  0x80

//-- system messages
#define MSG_USYS                (MSG_USTRM +  0x00)
#define MSG_SYS_MAGIC           (MSG_USYS + 0x01)     
#define MSG_SYS_PONG            (MSG_USYS + 0x02)
#define MSG_SYS_P_VERSION       (MSG_USYS + 0x03)
#define MSG_SYS_UNIQUE_ID       (MSG_USYS + 0x04)
#define MSG_SYS_SW_VERSION      (MSG_USYS + 0x05)
#define MSG_SYS_ERROR           (MSG_USYS + 0x06)
#define MSG_SYS_IDENTIFY_STATE  (MSG_USYS + 0x07)
#define MSG_NODE_TAB_NA         (MSG_USYS + 0x08)
#define MSG_NODE_TAB            (MSG_USYS + 0x09)
#define MSG_PKT_CAPACITY        (MSG_USYS + 0x0a)
#define MSG_NODE_NA             (MSG_USYS + 0x0b)
#define MSG_NODE_LOST           (MSG_USYS + 0x0c)
#define MSG_NODE_NEW            (MSG_USYS + 0x0d)

//-- feature and user config messages
#define MSG_UFC                 (MSG_USTRM +  0x10)
#define MSG_FEATURE             (MSG_UFC + 0x00)
#define MSG_FEATURE_NA          (MSG_UFC + 0x01)
#define MSG_FEATURE_COUNT       (MSG_UFC + 0x02)
#define MSG_VENDOR              (MSG_UFC + 0x03)
#define MSG_VENDOR_ACK          (MSG_UFC + 0x04)

//-- occupancy messages
#define MSG_UBM                 (MSG_USTRM +  0x20)
#define MSG_BM_OCC              (MSG_UBM + 0x00)
#define MSG_BM_FREE             (MSG_UBM + 0x01)
#define MSG_BM_MULTIPLE         (MSG_UBM + 0x02)
#define MSG_BM_ADDRESS          (MSG_UBM + 0x03)
#define MSG_BM_ACCESSORY        (MSG_UBM + 0x04)
#define MSG_BM_CV               (MSG_UBM + 0x05)
#define MSG_BM_SPEED            (MSG_UBM + 0x06)
#define MSG_BM_CURRENT          (MSG_UBM + 0x07)
#define MSG_BM_BLOCK_CV         (MSG_UBM + 0x08)

//-- booster messages
#define MSG_UBST                (MSG_USTRM +  0x30)
#define MSG_BOOST_STAT          (MSG_UBST + 0x00)    // short, temperatur, ...
#define MSG_BOOST_CURRENT       (MSG_UBST + 0x01)

//-- switch/light control messages
#define MSG_ULC                 (MSG_USTRM +  0x40)
#define MSG_LC_STAT             (MSG_ULC + 0x00)
#define MSG_LC_NA               (MSG_ULC + 0x01)
#define MSG_LC_CONFIG           (MSG_ULC + 0x02)
#define MSG_LC_KEY              (MSG_ULC + 0x03)


//-- macro messages
#define MSG_UMAC                (MSG_USTRM +  0x48)
#define MSG_LC_MACRO_STATE      (MSG_UMAC + 0x00)
#define MSG_LC_MACRO            (MSG_UMAC + 0x01)

//-- dcc control messages
#define MSG_UGEN                (MSG_USTRM +  0x60)
#define  MSG_CS_ALLOC_ACK       (MSG_UGEN + 0x00)    // noch genauer zu klären
#define  MSG_CS_STATE           (MSG_UGEN + 0x01)
#define  MSG_CS_DRIVE_ACK       (MSG_UGEN + 0x02)
#define  MSG_CS_ACCESSORY_ACK   (MSG_UGEN + 0x03)

/*
MSG_PRG_CV_STAT
*/

//-- local message
#define MSG_ULOCAL              (MSG_USTRM +  0x70)                           // only locally used
#define MSG_LOGON               (MSG_ULOCAL + 0x00) 
#define MSG_LOCAL_PONG          (MSG_ULOCAL + 0x01)   // only locally used

//===============================================================================
//
// 3. type defines
//
//===============================================================================

typedef struct
  {
    union
      {
        struct
          {
            unsigned char class_switch: 1;
            unsigned char class_booster: 1;
            unsigned char class_prog: 1;
            unsigned char class_dcc_acc: 1;
            unsigned char class_dcc_lok: 1;
            unsigned char class_ui: 1;
            unsigned char class_occupancy: 1;
            unsigned char class_bridge: 1;
          };
        unsigned char  class_id;
      };
    unsigned char  classx_id;
    unsigned char  dcc_vendor;
    union
      {
        struct
          {
            unsigned int   product_id;
            unsigned int   product_serial;
          };
        unsigned long vendor32;
      };
  } t_bidib_unique_id;

typedef enum
  { BIDIB_MACRO_OFF     = 0,
    BIDIB_MACRO_RUN     = 1,
    BIDIB_MACRO_SAVE    = 254,
    BIDIB_MACRO_DELETE  = 255,
  } t_bidib_macro_state;

//===============================================================================
//
// 4. feature codes
//
//===============================================================================

#define FEATURE_BM_SIZE                     0
#define FEATURE_BM_ON                       1
#define FEATURE_BM_SECACK_AVAILABLE         2
#define FEATURE_BM_SECACK_ON                3
#define FEATURE_BM_CURMEAS_AVAILABLE        4
#define FEATURE_BM_CURMEAS_INTERVAL         5
#define FEATURE_BM_DC_MEAS_AVAILABLE        6
#define FEATURE_BM_DC_MEAS_ON               7
#define FEATURE_BM_ADDR_DETECT_AVAILABLE    8
#define FEATURE_BM_ADDR_DETECT_ON           9
#define FEATURE_BM_ADDR_AND_DIR            10
#define FEATURE_BM_ISTSPEED_AVAILABLE      11
#define FEATURE_BM_ISTSPEED_ON             12
#define FEATURE_BM_CV_AVAILABLE            13
#define FEATURE_BM_CV_ON                   14
//-- booster
#define FEATURE_BST_VOLT_ADJUSTABLE        15
#define FEATURE_BST_VOLT                   16
#define FEATURE_BST_CUTOUT_AVAIALABLE      17
#define FEATURE_BST_CUTOUT_ON              18
#define FEATURE_BST_TURNOFF_TIME           19
#define FEATURE_BST_INRUSH_TURNOFF_TIME    20
#define FEATURE_BST_AMPERE_ADJUSTABLE      21
#define FEATURE_BST_AMPERE                 22
//-- control
#define FEATURE_CTRL_CHANNEL_COUNT         50   // numbers to be reviewed!
#define FEATURE_CTRL_KEY_COUNT             51
#define FEATURE_CTRL_MAC1_SAVE             52
#define FEATURE_CTRL_MAC1_COUNT            53
#define FEATURE_CTRL_MAC1_SIZE             54
//-- dcc gen
#define FEATURE_GEN_SPYMODE                100  // numbers to be reviewed!
#define FEATURE_GEN_WATCHDOG               101  // 
#define FEATURE_GEN_DRIVE_ACK              102  // 
#define FEATURE_GEN_SWITCH_ACK             103  // 
#define FEATURE_GEN_LOK_DB_SIZE            104  // 
#define FEATURE_GEN_LOK_DB_STRING          105  // 


//===============================================================================
//
// 5. error codes
//
//===============================================================================

#define BIDIB_ERR_NONE      0x00    // void
#define BIDIB_ERR_TXT       0x01    // general text error
#define BIDIB_ERR_CRC       0x02    // received crc was errornous
#define BIDIB_ERR_SIZE      0x03    //
#define BIDIB_ERR_SEQUENCE  0x04    // sequence was wrong
#define BIDIB_ERR_PARAMETER 0x05    // parameter out of range
#define BIDIB_ERR_BUS       0x10    // Bus Fault, capacity exceeded
#define BIDIB_ERR_ADDRSTACK 0x11    // Address Stack, 4 bytes follow 
#define BIDIB_ERR_IDDOUBLE  0x12    // Double ID, 7 bytes follow 
#define BIDIB_ERR_HW        0x20    // self test failed


//===============================================================================
//
// 6. System Messages, Serial Link, BiDiBus
//
//===============================================================================

// 6.a) serial Link

#define BIDIB_PKT_MAGIC         0xFE            // frame delimiter for serial link
#define BIDIB_PKT_ESCAPE        0xFD

// 6.b) defines für BiDiBus, system messages
// (system messages are 9 bits, bit8 is set (1), bits 0..7 do have even parity)
#define BIDIBUS_SYS_MSG         0x40            // System Part of BiDiBus

#define BIDIBUS_POWER_UP        0x7F            // Bus Reset
#define BIDIBUS_POWER_UP_par    0xFF            // Bus Reset (including parity)
#define BIDIBUS_LOGON           0x7E            // Logon Prompt
#define BIDIBUS_LOGON_par       0x7E            // Logon Prompt (including parity)
#define BIDIBUS_BUSY            0x7D            // Interface Busy
#define BIDIBUS_BUSY_par        0x7D            // Interface Busy (including parity)

// from Node
#define BIDIBUS_NODE_READY      0
#define BIDIBUS_NODE_BUSY       1

#endif // __BIDIB_MESSAGES_H__
