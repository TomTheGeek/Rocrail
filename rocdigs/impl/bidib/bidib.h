//===============================================================================
//
// OpenDCC - BiDiB
//
// Copyright (c) 2010 Wolfgang Kufer
//
//-------------------------------------------------------------------------------
//
// file:      bidib_messages.c
// author:    Wolfgang Kufer
// contact:   kufer@gmx.de
// webpage:   http://www.opendcc.de
// history:   2010-12-01 V0.01 kw  start
//            2010-12-07 V0.02 kw  added BIDIB_PKT_MAGIC, MSB_BM_ACCESSORY, ...
//
//===============================================================================
//
// purpose:   common header for bidib protocol
//            (This should be used in any BiDiB implementation)
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
#define BIDIB_VERSION           (0 * 256 +    1)

#define BIDIB_PKT_MAGIC         0xFE
#define BIDIB_PKT_ESCAPE        0xFD
#define BIDIB_SYS_MAGIC         0xAFFE


//===============================================================================
//
// 1. Defines for downstream messages
//
//===============================================================================

#define MSG_DSTRM  0x00

//-- system messages
#define MSG_DSYS                 0x00
#define MSG_SYS_GET_MAGIC       (MSG_DSTRM + MSG_DSYS + 0x01)       // these must stay here
#define MSG_SYS_GET_P_VERSION   (MSG_DSTRM + MSG_DSYS + 0x02)
#define MSG_SYS_ENABLE          (MSG_DSTRM + MSG_DSYS + 0x03)
#define MSG_SYS_DISABLE         (MSG_DSTRM + MSG_DSYS + 0x04)
#define MSG_SYS_GET_UNIQUE_ID   (MSG_DSTRM + MSG_DSYS + 0x05)
#define MSG_SYS_GET_SW_VERSION  (MSG_DSTRM + MSG_DSYS + 0x06)
#define MSG_SYS_PING            (MSG_DSTRM + MSG_DSYS + 0x07)
#define MSG_SYS_IDENTIFY        (MSG_DSTRM + MSG_DSYS + 0x08)
#define MSG_SYS_RESET           (MSG_DSTRM + MSG_DSYS + 0x09)
#define MSG_GET_NODE_TAB        (MSG_DSTRM + MSG_DSYS + 0x0a)
#define MSG_GET_PKT_CAPACITY    (MSG_DSTRM + MSG_DSYS + 0x0b)
#define MSG_NODE_CHANGED_ACK    (MSG_DSTRM + MSG_DSYS + 0x0c)
#define MSG_LOGON_ACK           (MSG_DSTRM + MSG_DSYS + 0x0d)

//-- feature and user config messages
#define MSG_DFC                  0x10
#define MSG_FEATURE_GETALL      (MSG_DSTRM + MSG_DFC + 0x00)
#define MSG_FEATURE_GET         (MSG_DSTRM + MSG_DFC + 0x01)
#define MSG_FEATURE_SET         (MSG_DSTRM + MSG_DFC + 0x02)
#define MSG_VENDOR_ENABLE       (MSG_DSTRM + MSG_DFC + 0x03)
#define MSG_VENDOR_DISABLE      (MSG_DSTRM + MSG_DFC + 0x04)
#define MSG_VENDOR_SET          (MSG_DSTRM + MSG_DFC + 0x05)
#define MSG_VENDOR_GET          (MSG_DSTRM + MSG_DFC + 0x06)

//-- occupancy messages
#define MSG_DBM                  0x20
#define MSG_BM_GET_RANGE        (MSG_DSTRM + MSG_DBM + 0x00)
#define MSG_BM_MIRROR           (MSG_DSTRM + MSG_DBM + 0x01)
#define MSG_BM_ADDR_GET_RANGE   (MSG_DSTRM + MSG_DBM + 0x02)

//-- booster messages
#define MSG_BOOST                0x30
#define MSG_BOOST_ON            (MSG_DSTRM + MSG_BOOST + 0x00)
#define MSG_BOOST_OFF           (MSG_DSTRM + MSG_BOOST + 0x01)

//===============================================================================
//
// 2. Defines for upstream messages
//
//===============================================================================

#define MSG_USTRM  0x80

//-- system messages
#define MSG_USYS                 0x00
#define MSG_SYS_MAGIC           (MSG_USTRM + MSG_USYS + 0x01)
#define MSG_SYS_PONG            (MSG_USTRM + MSG_USYS + 0x02)
#define MSG_SYS_P_VERSION       (MSG_USTRM + MSG_USYS + 0x03)
#define MSG_SYS_UNIQUE_ID       (MSG_USTRM + MSG_USYS + 0x04)
#define MSG_SYS_SW_VERSION      (MSG_USTRM + MSG_USYS + 0x05)
#define MSG_SYS_ERROR           (MSG_USTRM + MSG_USYS + 0x06)
#define MSG_SYS_IDENTIFY_STATE  (MSG_USTRM + MSG_USYS + 0x07)
#define MSG_NODE_TAB_NA         (MSG_USTRM + MSG_USYS + 0x08)
#define MSG_NODE_TAB            (MSG_USTRM + MSG_USYS + 0x09)
#define MSG_PKT_CAPACITY        (MSG_USTRM + MSG_USYS + 0x0a)
#define MSG_NODE_NA             (MSG_USTRM + MSG_USYS + 0x0b)
#define MSG_NODE_LOST           (MSG_USTRM + MSG_USYS + 0x0c)
#define MSG_NODE_NEW            (MSG_USTRM + MSG_USYS + 0x0d)
#define MSG_LOGON               (MSG_USTRM + MSG_USYS + 0x0e)

//-- feature and user config messages
#define MSG_UFC                  0x10
#define MSG_FEATURE             (MSG_USTRM + MSG_UFC + 0x00)
#define MSG_FEATURE_NA          (MSG_USTRM + MSG_UFC + 0x01)
#define MSG_FEATURE_COUNT       (MSG_USTRM + MSG_UFC + 0x02)
#define MSG_VENDOR              (MSG_USTRM + MSG_UFC + 0x03)
#define MSG_VENDOR_ACK          (MSG_USTRM + MSG_UFC + 0x04)

//-- occupancy messages
#define MSG_UBM                  0x20
#define MSG_BM_OCC              (MSG_USTRM + MSG_UBM + 0x00)
#define MSG_BM_FREE             (MSG_USTRM + MSG_UBM + 0x01)
#define MSG_BM_MULTIPLE         (MSG_USTRM + MSG_UBM + 0x02)
#define MSG_BM_ADDRESS          (MSG_USTRM + MSG_UBM + 0x03)
#define MSG_BM_ACCESSORY        (MSG_USTRM + MSG_UBM + 0x04)
#define MSG_BM_CV               (MSG_USTRM + MSG_UBM + 0x05)
#define MSG_BM_SPEED            (MSG_USTRM + MSG_UBM + 0x06)
#define MSG_BM_CURRENT          (MSG_USTRM + MSG_UBM + 0x07)


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
  } t_unique_id;


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


//===============================================================================
//
// 5. error codes
//
//===============================================================================

#define BIDIB_ERR_TXT       0x00    // general text error
#define BIDIB_ERR_CRC       0x01    // received crc was errornous
#define BIDIB_ERR_SIZE      0x02    //
#define BIDIB_ERR_SEQUENCE  0x03    // sequence was wrong
#define BIDIB_ERR_PARAMETER 0x04    // parameter out of range
#define BIDIB_ERR_BUS       0x10    // Bus Fault
#define BIDIB_ERR_HW        0x11    // self test failed




#endif // __BIDIB_MESSAGES_H__
