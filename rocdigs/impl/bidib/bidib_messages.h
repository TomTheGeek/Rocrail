//===============================================================================
//
// Common Header for BiDiB Implementations / Reference
// (c) http://www.bidib.org
// (see www.bidib.org for licence terms)
//
//-------------------------------------------------------------------------------
//
// file:      bidib_messages.h
// author:    Wolfgang Kufer
// contact:   kufer@gmx.de
// webpage:   http://www.bidib.org
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
//            2012-03-14       kw  added messages for switch/light control, 
//                                       and command station
//            2012-03-18 V0.04 kw  added FEATURE_GETNEXT, moved following messages
//                             kw  changed NODETAB-Handling
//                                 --> update of all firmware/software required!
//            2012-04-18 V0.05 kw  added defines for Switch / Output / Makros
//            2012-05-14 V0.06 kw  added defines for firmware update
//            2012-07-10       kw  added BIDIB_ERR_SUBTIME, BIDIB_ERR_SUBCRC
//            2012-07-30       kw  added BIDIB_MACRO_RESTORE, MSG_SYS_CLOCK
//            2012-09-14 V0.07 kw  added MSG_BM_CONFIDENCE, MSG_BM_GET_CONFIDENCE
//            2012-09-25 V0.07 kw  added MSG_LC_WAIT
//            2012-10-12 V0.08 kw  added MSG_ACCESSORY_*, CLASS bit accessory (update)
//                                       FEATURE_BM_ISTSPEED_INTERVAL instead _ON
// 
//===============================================================================
//
// purpose:   common header for bidib protocol
//            (This should be used in any BiDiB implementation)
//
//===============================================================================
//
// content:   0. Defines for serial protocol / bidib general
//            1. Defines for Downstream Messages
//            2. Defines for Upstream Messages
//            3. type defines
//            4. Feature Codes
//            5. Error Codes
//            6. FW Update (useful defines)
//            7. System Messages, Serial Link, BiDiBus
//            8. Command Station Handling (useful defines)
//            9. IO-Control and Macro (useful defines)
//
//===============================================================================
// 
// known problems: messages for programming are not yet defined
//
//===============================================================================
// 
// Generell rules for BiDiB
// a) BiDiB is based on bytes. Whenever longer objects are referred (int, long)
//    LSByte is transmitted first. 

#ifndef __BIDIB_MESSAGES_H__
#define __BIDIB_MESSAGES_H__

//===============================================================================
//
// 0. Defines for serial protocol / bidib general
//
//===============================================================================

//                              Mainversion   Subversion
#define BIDIB_VERSION           (0 * 256 +    5)

#define BIDIB_SYS_MAGIC         0xAFFE

//===============================================================================
//
// 1. Defines for Downstream Messages
//
//===============================================================================

#define MSG_DSTRM  0x00

//-- system messages                                    // Parameters
#define MSG_DSYS                (MSG_DSTRM + 0x00)
#define MSG_SYS_GET_MAGIC       (MSG_DSYS + 0x01)       // - // these must stay here
#define MSG_SYS_GET_P_VERSION   (MSG_DSYS + 0x02)       // - // these must stay here
#define MSG_SYS_ENABLE          (MSG_DSYS + 0x03)       // -
#define MSG_SYS_DISABLE         (MSG_DSYS + 0x04)       // -
#define MSG_SYS_GET_UNIQUE_ID   (MSG_DSYS + 0x05)       // -
#define MSG_SYS_GET_SW_VERSION  (MSG_DSYS + 0x06)       // -
#define MSG_SYS_PING            (MSG_DSYS + 0x07)       // 1:dat
#define MSG_SYS_IDENTIFY        (MSG_DSYS + 0x08)       // 1:id_state
#define MSG_SYS_RESET           (MSG_DSYS + 0x09)       // -
#define MSG_GET_PKT_CAPACITY    (MSG_DSYS + 0x0a)       // -
#define MSG_NODETAB_GETALL      (MSG_DSYS + 0x0b)       // -
#define MSG_NODETAB_GETNEXT     (MSG_DSYS + 0x0c)       // -
#define MSG_NODE_CHANGED_ACK    (MSG_DSYS + 0x0d)       // 1:nodetab_version
#define MSG_SYS_GET_ERROR       (MSG_DSYS + 0x0e)       // -
#define MSG_FW_UPDATE_OP        (MSG_DSYS + 0x0f)       // 1:opcode, 2..n parameters

//-- feature and user config messages
#define MSG_DFC                 (MSG_DSTRM + 0x10)
#define MSG_FEATURE_GETALL      (MSG_DFC + 0x00)        // -
#define MSG_FEATURE_GETNEXT     (MSG_DFC + 0x01)        // -
#define MSG_FEATURE_GET         (MSG_DFC + 0x02)        // 1:feature_num
#define MSG_FEATURE_SET         (MSG_DFC + 0x03)        // 1:feature_num, 2:feature_val
#define MSG_VENDOR_ENABLE       (MSG_DFC + 0x04)        // 1-7: unique-id of node
#define MSG_VENDOR_DISABLE      (MSG_DFC + 0x05)        // -
#define MSG_VENDOR_SET          (MSG_DFC + 0x06)        // V_NAME,V_VALUE
#define MSG_VENDOR_GET          (MSG_DFC + 0x07)        // V_NAME
#define MSG_SYS_CLOCK           (MSG_DFC + 0x08)        // 1:TCODE0, 2:TCODE1, 3:TCODE2, 4:TCODE3

//-- occupancy messages
#define MSG_DBM                 (MSG_DSTRM + 0x20)
#define MSG_BM_GET_RANGE        (MSG_DBM + 0x00)        // 1:start, 2:end
#define MSG_BM_MIRROR_MULTIPLE  (MSG_DBM + 0x01)        // 1:start, 2:size, 3..n:data
#define MSG_BM_MIRROR_OCC       (MSG_DBM + 0x02)        // 1:mnum
#define MSG_BM_MIRROR_FREE      (MSG_DBM + 0x03)        // 1:mnum
#define MSG_BM_ADDR_GET_RANGE   (MSG_DBM + 0x04)        // 1:start, 2:end
#define MSG_BM_GET_CONFIDENCE   (MSG_DBM + 0x05)        // -

//-- booster messages
#define MSG_DBST                (MSG_DSTRM + 0x30)
#define MSG_BOOST_OFF           (MSG_DBST + 0x00)       // -
#define MSG_BOOST_ON            (MSG_DBST + 0x01)       // -

//-- accessory control messages
#define MSG_DACC                (MSG_DSTRM + 0x38)
#define MSG_ACCESSORY_SET       (MSG_DACC + 0x00)       // 1:anum, 2:aspect
#define MSG_ACCESSORY_GET       (MSG_DACC + 0x01)       // 1:anum
#define MSG_ACCESSORY_PARA_SET  (MSG_DACC + 0x02)       // 1:anum, 2:para_num, 3..n: data 
#define MSG_ACCESSORY_PARA_GET  (MSG_DACC + 0x03)       // 1:anum, 2:para_num

//-- switch/light/servo control messages
#define MSG_DLC                 (MSG_DSTRM + 0x40)
#define MSG_LC_OUTPUT           (MSG_DLC + 0x00)        // 1:type, 2:port, 3:state
#define MSG_LC_CONFIG_SET       (MSG_DLC + 0x01)        // 1:type, 2:port, 3:off_val, 4:on_val, 5:dimm_off, 6:dimm_on
#define MSG_LC_CONFIG_GET       (MSG_DLC + 0x02)        // 1:type, 2:port
#define MSG_LC_KEY_QUERY        (MSG_DLC + 0x03)        // 1:port

//-- macro messages
#define MSG_DMAC                (MSG_DSTRM + 0x48)
#define MSG_LC_MACRO_HANDLE     (MSG_DMAC + 0x00)       // 1:macro, 2:opcode
#define MSG_LC_MACRO_SET        (MSG_DMAC + 0x01)       // 1:macro, 2:item, 3:delay, 4:lstate,  5:lvalue 
#define MSG_LC_MACRO_GET        (MSG_DMAC + 0x02)       // 1:macro, 2:item
#define MSG_LC_MACRO_PARA_SET   (MSG_DMAC + 0x03)       // 1:macro, 2:para_idx, 3,4,5,6:value
#define MSG_LC_MACRO_PARA_GET   (MSG_DMAC + 0x04)       // 1:macro, 2:para_idx

//-- dcc gen messages
#define MSG_DGEN                (MSG_DSTRM + 0x60)
#define  MSG_CS_ALLOCATE        (MSG_DGEN + 0x00)
#define  MSG_CS_CONNECT         (MSG_DGEN + 0x01)
#define  MSG_CS_SET_STATE       (MSG_DGEN + 0x02)
#define  MSG_CS_GET_STATE       (MSG_DGEN + 0x03)       // could be obsolete
#define  MSG_CS_DRIVE           (MSG_DGEN + 0x04)       // 1:addrl, 2:addrh, 3:format, 4:active, 5:speed, 6:1-4, 7:5-12, 8:13-20, 9:21-28
#define  MSG_CS_ACCESSORY       (MSG_DGEN + 0x05)
#define  MSG_PRG_CV_WRITE       (MSG_DGEN + 0x07)       // 1:method, 2:CV_ADDR_L, 3:CV_ADDR_H, 4:CV_DAT
#define  MSG_PRG_CV_BLOCKWRITE  (MSG_DGEN + 0x08)
#define  MSG_PRG_CV_READ        (MSG_DGEN + 0x09)       // 1:method, 2:CV_ADDR_L, 3:CV_ADDR_H
#define  MSG_PRG_CV_BLOCKREAD   (MSG_DGEN + 0x0A)


//-- local message
#define MSG_DLOCAL              (MSG_DSTRM + 0x70)                           // only locally used
#define MSG_LOGON_ACK           (MSG_DSTRM + MSG_DLOCAL + 0x00) 
#define MSG_LOCAL_PING          (MSG_DSTRM + MSG_DLOCAL + 0x01)

//===============================================================================
//
// 2. Defines for Upstream Messages
//
//===============================================================================

#define MSG_USTRM  0x80

//-- system messages
#define MSG_USYS                (MSG_USTRM +  0x00)
#define MSG_SYS_MAGIC           (MSG_USYS + 0x01)       // 1:0xFE 2:0xAF
#define MSG_SYS_PONG            (MSG_USYS + 0x02)       // 1:mirrored dat
#define MSG_SYS_P_VERSION       (MSG_USYS + 0x03)       // 1:proto-ver_l, 2:proto-ver_h
#define MSG_SYS_UNIQUE_ID       (MSG_USYS + 0x04)       // 1:class, 2:classx, 3:vid, 4..7:pid+uid    
#define MSG_SYS_SW_VERSION      (MSG_USYS + 0x05)       // 1:sw-ver_l, 2:sw_-ver_h, 3:sw-ver_u
#define MSG_SYS_ERROR           (MSG_USYS + 0x06)       // 1:err_code, 2:msg
#define MSG_SYS_IDENTIFY_STATE  (MSG_USYS + 0x07)       // 1:state
#define MSG_NODETAB_COUNT       (MSG_USYS + 0x08)       // 1:length
#define MSG_NODETAB             (MSG_USYS + 0x09)       // 1:version, 2:local num, 3..9: unique
#define MSG_PKT_CAPACITY        (MSG_USYS + 0x0a)       // 1:capacity
#define MSG_NODE_NA             (MSG_USYS + 0x0b)       // 1:node
#define MSG_NODE_LOST           (MSG_USYS + 0x0c)       // 1:node
#define MSG_NODE_NEW            (MSG_USYS + 0x0d)       // 1:version, 2:local num, 3..9: unique
#define MSG_STALL               (MSG_USYS + 0x0e)       // 1:state
#define MSG_FW_UPDATE_STAT      (MSG_USYS + 0x0f)       // 1:stat, 2:timeout

//-- feature and user config messages
#define MSG_UFC                 (MSG_USTRM +  0x10)
#define MSG_FEATURE             (MSG_UFC + 0x00)        // 1:feature_num, 2:data
#define MSG_FEATURE_NA          (MSG_UFC + 0x01)        // 1:feature_num
#define MSG_FEATURE_COUNT       (MSG_UFC + 0x02)        // 1:count
#define MSG_VENDOR              (MSG_UFC + 0x03)        // 1..n: length,'string',length,'value'
#define MSG_VENDOR_ACK          (MSG_UFC + 0x04)        // 1:ack

//-- occupancy messages
#define MSG_UBM                 (MSG_USTRM +  0x20)
#define MSG_BM_OCC              (MSG_UBM + 0x00)        // 1:mnum
#define MSG_BM_FREE             (MSG_UBM + 0x01)        // 1:mnum
#define MSG_BM_MULTIPLE         (MSG_UBM + 0x02)        // 1:base, 2:size; 3..n:data
#define MSG_BM_ADDRESS          (MSG_UBM + 0x03)        // 1:mnum, [2,3:addr_l, addr_h]
#define MSG_BM_ACCESSORY        (MSG_UBM + 0x04)        //
#define MSG_BM_CV               (MSG_UBM + 0x05)        // 1:addr_l, 2:addr_h, 3:cv_addr_l, 4:cv_addr_h, 5:cv_dat
#define MSG_BM_SPEED            (MSG_UBM + 0x06)        // 1:addr_l, 2:addr_h, 3:speed_l, 4:speed_h
#define MSG_BM_CURRENT          (MSG_UBM + 0x07)        // 1:mnum, 2:current
#define MSG_BM_BLOCK_CV         (MSG_UBM + 0x08)        //
#define MSG_BM_CONFIDENCE       (MSG_UBM + 0x09)        // 1:void, 2:freeze, 3:signal

//-- booster messages
#define MSG_UBST                (MSG_USTRM +  0x30)
#define MSG_BOOST_STAT          (MSG_UBST + 0x00)       // 1:state
#define MSG_BOOST_CURRENT       (MSG_UBST + 0x01)       // 1:current
#define MSG_NEW_DECODER         (MSG_UBST + 0x02)       // 1:mnum, 2: dec_vid, 3,4,5,6:dec_uid    
#define MSG_ID_SEARCH_ACK       (MSG_UBST + 0x03)       // 1:mnum, 2: s_vid, 3,4,5,6:s_uid,  7: dec_vid, 8,9,10,11:dec_uid  
#define MSG_ADDR_CHANGE_ACK     (MSG_UBST + 0x04)       // 1:mnum, 2: dec_vid, 3,4,5,6:dec_uid, 7:addr_l, 8:addr_h

//-- accessory control messages
#define MSG_UACC                (MSG_USTRM + 0x38)
#define MSG_ACCESSORY_STATE     (MSG_UACC + 0x00)       // 1:port, 2:aspect, 3:total, 4:execute, 5:wait
#define MSG_ACCESSORY_PARA      (MSG_UACC + 0x01)       // 1:anum, 2:para_num, 3..n: data

//-- switch/light control messages
#define MSG_ULC                 (MSG_USTRM +  0x40)
#define MSG_LC_STAT             (MSG_ULC + 0x00)        // 1:type, 2:port, 3:state
#define MSG_LC_NA               (MSG_ULC + 0x01)        // 1:type, 2:port
#define MSG_LC_CONFIG           (MSG_ULC + 0x02)        // 1:type, 2:port, 3:off_val, 4:on_val, 5:dimm_off, 6:dimm_on
#define MSG_LC_KEY              (MSG_ULC + 0x03)        // 1:port, 2:state
#define MSG_LC_WAIT             (MSG_ULC + 0x04)        // 1:type, 2:port, 3:time

//-- macro messages
#define MSG_UMAC                (MSG_USTRM +  0x48)
#define MSG_LC_MACRO_STATE      (MSG_UMAC + 0x00)
#define MSG_LC_MACRO            (MSG_UMAC + 0x01)
#define MSG_LC_MACRO_PARA       (MSG_UMAC + 0x02)

//-- dcc control messages
#define MSG_UGEN                (MSG_USTRM +  0x60)
#define MSG_CS_ALLOC_ACK        (MSG_UGEN + 0x00)       // noch genauer zu klaeren
#define MSG_CS_STATE            (MSG_UGEN + 0x01)
#define MSG_CS_DRIVE_ACK        (MSG_UGEN + 0x02)
#define MSG_CS_ACCESSORY_ACK    (MSG_UGEN + 0x03)
#define MSG_PRG_CV_STAT         (MSG_UGEN + 0x04)       // 1: PRG_STATE, 2:PRG_DATA 

//-- local message
#define MSG_ULOCAL              (MSG_USTRM +  0x70)     // only locally used
#define MSG_LOGON               (MSG_ULOCAL + 0x00) 
#define MSG_LOCAL_PONG          (MSG_ULOCAL + 0x01)     // only locally used

//===============================================================================
//
// 3. Type Defines
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
            unsigned char class_accessory: 1;
            unsigned char class_dcc_prog: 1;
            unsigned char class_dcc_main: 1;
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

// typedef for control operations
typedef struct
  {
    unsigned char type;                  // BIDIB_OUTTYPE_*
    unsigned char portnum;               // out number, 0 ... n
    unsigned char portstat;              // state of this output
  }  t_bidib_port;

typedef struct
  {
    unsigned char portnum;
    unsigned char brightness_off;       // Brightness in state OFF, range 0..255
    unsigned char brightness_on;        // Brightness in state ON, range 0..255
    unsigned char dimm_off;             // time for dimming towards OFF: 0=fast ... 255=slow
    unsigned char dimm_on;              // time for dimming towards ON: 0=fast ... 255=slow
  } t_bidib_lport_cfg;

typedef struct
  {
    unsigned char portnum;
    unsigned char adjust_low;
    unsigned char adjust_high;
    unsigned char speed;
    unsigned char reserved0;
  } t_bidib_servo_cfg;

//===============================================================================
//
// 4. Feature Codes
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
#define FEATURE_BM_ISTSPEED_INTERVAL       12
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
#define FEATURE_BST_CURMEAS_INTERVAL       23
#define FEATURE_BST_CV_AVAILABLE           24
#define FEATURE_BST_CV_ON                  25
#define FEATURE_BST_INHIBIT_AUTOSTART      26
#define FEATURE_BST_INHIBIT_LOCAL_ONOFF    27

//-- accessory
#define FEATURE_ACCESSORY_COUNT            40   // number of objects
#define FEATURE_ACCESSORY_SURVEILLED       41   // 1: annouce if operated outside bidib
#define FEATURE_ACCESSORY_MACROMAPPED      42   // 1: accessory aspects are mapped to macros

//-- control
#define FEATURE_CTRL_INPUT_COUNT           50
#define FEATURE_CTRL_INPUT_NOTIFY          51   // 1: report to host
#define FEATURE_CTRL_SPORT_COUNT           52
#define FEATURE_CTRL_LPORT_COUNT           53
#define FEATURE_CTRL_SERVO_COUNT           54
#define FEATURE_CTRL_SOUND_COUNT           55
#define FEATURE_CTRL_MOTOR_COUNT           56
#define FEATURE_CTRL_ANALOG_COUNT          57
#define FEATURE_CTRL_MAC_LEVEL             60
#define FEATURE_CTRL_MAC_SAVE              61
#define FEATURE_CTRL_MAC_COUNT             62
#define FEATURE_CTRL_MAC_SIZE              63
#define FEATURE_CTRL_MAC_START_MAN         64
#define FEATURE_CTRL_MAC_START_DCC         65

//-- dcc gen
#define FEATURE_GEN_SPYMODE                100  // 1: watch bidib handsets
#define FEATURE_GEN_WATCHDOG               101  // watchdog (MSG_CS_ALLOCATE)
#define FEATURE_GEN_DRIVE_ACK              102  //  
#define FEATURE_GEN_SWITCH_ACK             103  // 
#define FEATURE_GEN_LOK_DB_SIZE            104  // 
#define FEATURE_GEN_LOK_DB_STRING          105  // 
#define FEATURE_GEN_SERVICE_MODES          106  // 
#define FEATURE_GEN_DRIVE_BUS              107  // 1: this node drive the dcc bus. 
#define FEATURE_GEN_LOK_LOST_DETECT        108
#define FEATURE_GEN_NOTIFY_DRIVE_MANUAL    109

#define FEATURE_FW_UPDATE_MODE             254  // 0: no fw-update, 1: intel hex
#define FEATURE_EXTENSION                  255  // 1: reserved for future expansion

//===============================================================================
//
// 5. Error Codes
//
//===============================================================================

#define BIDIB_ERR_NONE                    0x00  // void
#define BIDIB_ERR_TXT                     0x01  // general text error
#define BIDIB_ERR_CRC                     0x02  // received crc was errornous
#define BIDIB_ERR_SIZE                    0x03  //
#define BIDIB_ERR_SEQUENCE                0x04  // sequence was wrong
#define BIDIB_ERR_PARAMETER               0x05  // parameter out of range
#define BIDIB_ERR_BUS                     0x10  // Bus Fault, capacity exceeded
#define BIDIB_ERR_ADDRSTACK               0x11  // Address Stack, 4 bytes follow 
#define BIDIB_ERR_IDDOUBLE                0x12  // Double ID, 7 bytes follow 
#define BIDIB_ERR_SUBCRC                  0x13  // Message in Subsystem had crc error, 1 byte with node addr follow 
#define BIDIB_ERR_SUBTIME                 0x14  // Message in Subsystem timed out
#define BIDIB_ERR_HW                      0x20  // self test failed

//===============================================================================
//
// 6. FW Update (useful defines)
//
//===============================================================================

#define BIDIB_MSG_FW_UPDATE_OP_ENTER      0x00  // node should enter update mode
#define BIDIB_MSG_FW_UPDATE_OP_EXIT       0x01  // node should leave update mode
#define BIDIB_MSG_FW_UPDATE_OP_SETDEST    0x02  // set destination memory
#define BIDIB_MSG_FW_UPDATE_OP_DATA       0x03  // data chunk
#define BIDIB_MSG_FW_UPDATE_OP_DONE       0x04  // end of data
                                          
#define BIDIB_MSG_FW_UPDATE_STAT_READY      0   // ready
#define BIDIB_MSG_FW_UPDATE_STAT_EXIT       1   // exit ack'd
#define BIDIB_MSG_FW_UPDATE_STAT_DATA       2   // waiting for data
#define BIDIB_MSG_FW_UPDATE_STAT_ERROR    255   // there was an error

#define BIDIB_FW_UPDATE_ERROR_NO_DEST       1   // destination not yet set
#define BIDIB_FW_UPDATE_ERROR_RECORD        2   // error in hex record type
#define BIDIB_FW_UPDATE_ERROR_ADDR          3   // record out of range
#define BIDIB_FW_UPDATE_ERROR_CHECKSUM      4   // checksum error on record
#define BIDIB_FW_UPDATE_ERROR_SIZE          5   // size error
#define BIDIB_FW_UPDATE_ERROR_APPCRC        6   // crc error on application, cant start


//===============================================================================
//
// 7. System Messages, Serial Link, BiDiBus
//
//===============================================================================

// 6.a) Serial Link

#define BIDIB_PKT_MAGIC                  0xFE   // frame delimiter for serial link
#define BIDIB_PKT_ESCAPE                 0xFD   

// 6.b) defines for BiDiBus, system messages
// (system messages are 9 bits, bit8 is set (1), bits 0..7 do have even parity)
#define BIDIBUS_SYS_MSG                  0x40   // System Part of BiDiBus

#define BIDIBUS_POWER_UPx                0x7F   // formerly Bus Reset (now reserved)
#define BIDIBUS_POWER_UPx_par            0xFF   // formerly Bus Reset (including parity)
#define BIDIBUS_LOGON                    0x7E   // Logon Prompt
#define BIDIBUS_LOGON_par                0x7E   // Logon Prompt (including parity)
#define BIDIBUS_BUSY                     0x7D   // Interface Busy
#define BIDIBUS_BUSY_par                 0x7D   // Interface Busy (including parity)

// from Node
#define BIDIBUS_NODE_READY              0
#define BIDIBUS_NODE_BUSY               1

//===============================================================================
//
// 8. Command Station Handling (useful defines)
//
//===============================================================================

#define BIDIB_CS_STATE_OFF          0x00
#define BIDIB_CS_STATE_STOP         0x01
#define BIDIB_CS_STATE_SOFTSTOP     0x02
#define BIDIB_CS_STATE_SHORT        0x03
#define BIDIB_CS_STATE_GO           0x10
#define BIDIB_CS_STATE_PROG         0x80
#define BIDIB_CS_STATE_PROGBUSY     0x81
#define BIDIB_CS_STATE_BUSY         0xF0

#define BIDIB_CS_DRIVE_FORMAT_DCC14      0 
#define BIDIB_CS_DRIVE_FORMAT_DCC28      2 
#define BIDIB_CS_DRIVE_FORMAT_DCC128     3 

#define BIDIB_CS_DRIVE_SPEED_BIT    (1<<0)
#define BIDIB_CS_DRIVE_F1F4_BIT     (1<<1)
#define BIDIB_CS_DRIVE_F5F8_BIT     (1<<2)
#define BIDIB_CS_DRIVE_F9F12_BIT    (1<<3)
#define BIDIB_CS_DRIVE_F13F20_BIT   (1<<4)
#define BIDIB_CS_DRIVE_F21F28_BIT   (1<<5)

//===============================================================================
//
// 9. IO-Control and Macro (useful defines)
//
//===============================================================================

// Accessory
#define BIDIB_ACCESSORY_PARA_MACROMAP  253   // following data defines a mapping
#define BIDIB_ACCESSORY_SWITCH_TIME    254   // 

// Macro / Switch Pointparameters
// type codes
#define BIDIB_OUTTYPE_SPORT          0     // standard port
#define BIDIB_OUTTYPE_LPORT          1     // light port
#define BIDIB_OUTTYPE_SERVO          2     
#define BIDIB_OUTTYPE_SOUND          3     
#define BIDIB_OUTTYPE_MOTOR          4     
#define BIDIB_OUTTYPE_ANALOG         5     

// control codes  - limited to one nibble, here for PORTs

#define BIDIB_PORT_TURN_OFF          0      // for standard
#define BIDIB_PORT_TURN_ON           1      // for standard
#define BIDIB_PORT_DIMM_OFF          2
#define BIDIB_PORT_DIMM_ON           3
#define BIDIB_PORT_TURN_ON_NEON      4
#define BIDIB_PORT_BLINK_A           5
#define BIDIB_PORT_BLINK_B           6
#define BIDIB_PORT_FLASH_A           7
#define BIDIB_PORT_FLASH_B           8
#define BIDIB_PORT_DOUBLE_FLASH      9
#define BIDIB_PORT_QUERY            15

// Macro Global States
#define BIDIB_MACRO_OFF             0x00
#define BIDIB_MACRO_START           0x01
#define BIDIB_MACRO_RUNNING         0x02 
#define BIDIB_MACRO_RESTORE         0xFC    // 252  
#define BIDIB_MACRO_SAVE            0xFD    // 253
#define BIDIB_MACRO_DELETE          0xFE
#define BIDIB_MACRO_NOTEXIST        0xFF

// Macro System Commands (Level 2)
// These are opcodes inside a macro-syscommand of level 2 
#define BIDIB_MSYS_END_OF_MACRO     255     // end of macro (EOF)
#define BIDIB_MSYS_START_MACRO      254     // start a macro
#define BIDIB_MSYS_STOP_MACRO       253     // stop a macro
#define BIDIB_MSYS_BEGIN_CRITCAL    252     // current macro will ignore stop requests
#define BIDIB_MSYS_END_CRITCAL      251     // current macro can be stopped by a stop (default)

#define BIDIB_MSYS_FLAG_QUERY       250     // query flag and pause as long as flag is not set
#define BIDIB_MSYS_FLAG_SET         249     // set flag
#define BIDIB_MSYS_FLAG_CLEAR       248     // reset flag

#define BIDIB_MSYS_INPUT_QUERY1     247     // query input for 'pressed / activated'
#define BIDIB_MSYS_INPUT_QUERY0     246     // query input for 'released'
#define BIDIB_MSYS_DELAY_RANDOM     245     // make a random delay
#define BIDIB_MSYS_DELAY_FIXED      244     // make a fixed delay



// Macro global parameters
#define BIDIB_MACRO_PARA_SLOWDOWN   0x01      
#define BIDIB_MACRO_PARA_REPEAT     0x02    // 0=forever, 1=once, 2..250 n times  
#define BIDIB_MACRO_PARA_START_CLK  0x03    // TCODE defines Startpoint  



// here additional run parameters are to be defined. like:
// start condition: from DCC, DCC addr low, DCC addr high
//                  from system clock: time
//                  from input
// stop condition:


#endif // __BIDIB_MESSAGES_H__
