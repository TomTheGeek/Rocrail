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
//            2012-12-03 V0.09 kw  added FEATURE_BST_CURMEAS_INTERVAL
//                             kw  added t_bidib_cs_accessory
//            2012-12-21       kw  added MSG_BOOST_QUERY, MSG_CS_BIN_STATES
//                                 removed MSG_CS_CONNECT, MSG_CS_GET_STATE
//            2013-01-15       kw  added MSG_CS_POM and MSG_CS_POM_ACK, MSG_PRG_CV_STAT moved
//            2013-01-21           added FEATURE_GEN_NOTIFY_DRIVE_MANUAL
//            2013-02-10 V0.09 kw  added addtional BST_STATE values
//            2013-02-21 V0.10 kw  added MSG_BOOST_DIAGNOSTIC, BIDIB_ERR_SUBPAKET
//            2013-03-25 V0.11 kw  added MSG_LOGON_REJECTED
//            2013-06-04       kw  added FEATURE_CTRL_STRETCH_DIMM
//            2013-06-17       kw  added FEATURE_GEN_WATCHDOG, BIDIB_ERR_OVERRUN
//                                 added useful defines for Accessory States
//            2013-07-10 V0.12 kw  added BIDIB_BOOT_MAGIC
//            2013-09-26 V0.13 kw  added MSG_LC_OUTPUT_QUERY, added FEATURE_RELEVANT_PID_BITS
//                                       FEATURE_CTRL_PORT_QUERY_AVAILABLE
//            2013-10-04 V0.14 kw  added BIDIB_MSYS_ACC_OKAY_QIN0 ..1, BIDIB_MSYS_ACC_OKAY_NF
//            2013-10-06       kw  added FEATURE_SPORT_CONFIG_AVAILABLE, added Ctrltype BACKLIGHT
//                                 added FEATURE_CTRL_BACKLIGHT_COUNT, BIDIB_OUTTYPE_BACKLIGHT
//            2013-12-15 V0.15 kw  added FEATURE_STRING_SIZE, MSG_STRING_GET, _STRING_SET _STRING
//            2014-01-22 V0.16 kw  added MSG_BM_DYN_STATE, MSG_CS_PROG, MSG_CS_PROG_STATE, FEATURE_BM_DYN_STATE_INTERVAL
//            2014-02-07           added MSG_ACCESSORY_NOTIFY
//            2014-02-08 V0.17 kw  fixed some typos; removed MSG_PRG_CV_*; moved MSG_CS_PROG
//                                 added BIDIB_CS_PROG_QUERY, removed t_bidib_macro_state
//                                 added FEATURE_GEN_POM_REPEAT
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
//            8. Booster and Command Station Handling (useful defines)
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

#define BIDIB_SYS_MAGIC         0xAFFE                  // full featured BiDiB-Node
#define BIDIB_BOOT_MAGIC        0xB00D                  // reduced Node, bootloader only

//===============================================================================
//
// 1. Defines for Downstream Messages
//
//===============================================================================
//*// = broadcast messages, a interface must forward this to subnodes
//      a node must not answer these messages, if not connected
#define MSG_DSTRM  0x00

//-- system messages                                    // Parameters
#define MSG_DSYS                (MSG_DSTRM + 0x00)
#define MSG_SYS_GET_MAGIC       (MSG_DSYS + 0x01)       // - // these must stay here
#define MSG_SYS_GET_P_VERSION   (MSG_DSYS + 0x02)       // - // these must stay here
#define MSG_SYS_ENABLE          (MSG_DSYS + 0x03)    //*// -
#define MSG_SYS_DISABLE         (MSG_DSYS + 0x04)    //*// -
#define MSG_SYS_GET_UNIQUE_ID   (MSG_DSYS + 0x05)       // -
#define MSG_SYS_GET_SW_VERSION  (MSG_DSYS + 0x06)       // -
#define MSG_SYS_PING            (MSG_DSYS + 0x07)       // 1:dat
#define MSG_SYS_IDENTIFY        (MSG_DSYS + 0x08)       // 1:id_state
#define MSG_SYS_RESET           (MSG_DSYS + 0x09)    //*// -
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
#define MSG_SYS_CLOCK           (MSG_DFC + 0x08)     //*// 1:TCODE0, 2:TCODE1, 3:TCODE2, 4:TCODE3
#define MSG_STRING_GET          (MSG_DFC + 0x09)        // 1:Nspace, 2:ID
#define MSG_STRING_SET          (MSG_DFC + 0x0a)        // 1:Nspace, 2:ID, 3:Strsize, 4...n: string 

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
#define MSG_BOOST_OFF           (MSG_DBST + 0x00)    //*// 1:bc or node
#define MSG_BOOST_ON            (MSG_DBST + 0x01)    //*// 1:bc or node
#define MSG_BOOST_QUERY         (MSG_DBST + 0x02)       // -

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
#define MSG_LC_OUTPUT_QUERY     (MSG_DLC + 0x04)        // 1:type, 2:port

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
#define  MSG_CS_SET_STATE       (MSG_DGEN + 0x02)
#define  MSG_CS_DRIVE           (MSG_DGEN + 0x04)       // 1:addrl, 2:addrh, 3:format, 4:active, 5:speed, 6:1-4, 7:5-12, 8:13-20, 9:21-28
#define  MSG_CS_ACCESSORY       (MSG_DGEN + 0x05)       // 1:addrl, 2:addrh, 3:data(aspect), 4:time_l, 5:time_h
#define  MSG_CS_BIN_STATE       (MSG_DGEN + 0x06)       // 1:addrl, 2:addrh, 3:bin_statl, 4:bin_stath
#define  MSG_CS_POM             (MSG_DGEN + 0x07)       // 1..4:addr, 5:MID, 6:opcode, 7:cv_l, 8:cv_h, 9:cv_x, 10..13: data
                                                        // 1:did[0], 2:did[1], 3:did[2], 4:did[4]
//-- service mode
#define  MSG_CS_PROG            (MSG_DGEN + 0x0F)       // 1:opcode, 2:cv_l, 3:cv_h, 4: data

//-- local message
#define MSG_DLOCAL              (MSG_DSTRM + 0x70)      // only locally used
#define MSG_LOGON_ACK           (MSG_DLOCAL + 0x00)     // 1:node_addr, 2..8:unique_id
#define MSG_LOCAL_PING          (MSG_DLOCAL + 0x01)
#define MSG_LOGON_REJECTED      (MSG_DLOCAL + 0x02)     // 1..7:unique_id

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
#define MSG_STRING              (MSG_UFC + 0x05)        // 1:Nspace, 2:ID, 3:Strsize, 4...n: string 

//-- occupancy messages
#define MSG_UBM                 (MSG_USTRM +  0x20)
#define MSG_BM_OCC              (MSG_UBM + 0x00)        // 1:mnum
#define MSG_BM_FREE             (MSG_UBM + 0x01)        // 1:mnum
#define MSG_BM_MULTIPLE         (MSG_UBM + 0x02)        // 1:base, 2:size; 3..n:data
#define MSG_BM_ADDRESS          (MSG_UBM + 0x03)        // 1:mnum, [2,3:addr_l, addr_h]
#define MSG_BM_ACCESSORY        (MSG_UBM + 0x04)        //
#define MSG_BM_CV               (MSG_UBM + 0x05)        // 1:addr_l, 2:addr_h, 3:cv_addr_l, 4:cv_addr_h, 5:cv_dat
#define MSG_BM_SPEED            (MSG_UBM + 0x06)        // 1:addr_l, 2:addr_h, 3:speed_l, 4:speed_h (from loco)
#define MSG_BM_CURRENT          (MSG_UBM + 0x07)        // 1:mnum, 2:current
#define MSG_BM_BLOCK_CV         (MSG_UBM + 0x08)        //
#define MSG_BM_CONFIDENCE       (MSG_UBM + 0x09)        // 1:void, 2:freeze, 3:signal
#define MSG_BM_DYN_STATE        (MSG_UBM + 0x0a)        // 1:mnum, 2:addr_l, 3:addr_h, 4:dyn_num, 5:value (from loco)

//-- booster messages
#define MSG_UBST                (MSG_USTRM +  0x30)
#define MSG_BOOST_STAT          (MSG_UBST + 0x00)       // 1:state (see defines below)
#define MSG_BOOST_CURRENT       (MSG_UBST + 0x01)       // 1:current
#define MSG_BOOST_DIAGNOSTIC    (MSG_UBST + 0x02)       // [1:enum, 2:value],[3:enum, 4:value] ...
#define MSG_NEW_DECODER         (MSG_UBST + 0x03)       // 1:mnum, 2: dec_vid, 3,4,5,6:dec_uid
#define MSG_ID_SEARCH_ACK       (MSG_UBST + 0x04)       // 1:mnum, 2: s_vid, 3,4,5,6:s_uid[0..3],  7: dec_vid, 8,9,10,11:dec_uid
#define MSG_ADDR_CHANGE_ACK     (MSG_UBST + 0x05)       // 1:mnum, 2: dec_vid, 3,4,5,6:dec_uid, 7:addr_l, 8:addr_h

//-- accessory control messages
#define MSG_UACC                (MSG_USTRM + 0x38)
#define MSG_ACCESSORY_STATE     (MSG_UACC + 0x00)       // 1:port, 2:aspect, 3:total, 4:execute, 5:wait (Quittung)
#define MSG_ACCESSORY_PARA      (MSG_UACC + 0x01)       // 1:anum, 2:para_num, 3..n: data
#define MSG_ACCESSORY_NOTIFY    (MSG_UACC + 0x02)       // 1:port, 2:aspect, 3:total, 4:execute, 5:wait (Spontan)

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
#define MSG_CS_ACCESSORY_ACK    (MSG_UGEN + 0x03)       // 1:addrl, 2:addrh, 3:data 
#define MSG_CS_POM_ACK          (MSG_UGEN + 0x04)       // 1:addrl, 2:addrh, 3:addrxl, 4:addrxh, 5:mid, 6:data 
#define MSG_CS_DRIVE_MANUAL     (MSG_UGEN + 0x05)       // 1:addrl, 2:addrh, 3:format, 4:active, 5:speed, 6:1-4, 7:5-12, 8:13-20, 9:21-28
#define MSG_CS_DRIVE_EVENT      (MSG_UGEN + 0x06)       // 1:addrl, 2:addrh, 3:eventtype, Parameters
#define MSG_CS_ACCESSORY_MANUAL (MSG_UGEN + 0x07)       // 1:addrl, 2:addrh, 3:data 

//-- service mode
#define MSG_CS_PROG_STATE       (MSG_UGEN + 0x0F)       // 1: state, 2:time, 3:cv_l, 4:cv_h, 5:data

//-- local message
#define MSG_ULOCAL              (MSG_USTRM +  0x70)     // only locally used
#define MSG_LOGON               (MSG_ULOCAL + 0x00)
#define MSG_LOCAL_PONG          (MSG_ULOCAL + 0x01)     // only locally used

//===============================================================================
//
// 3. Type Defines
//    (useful defines for access to data structures in BiDiB)
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

// typedef for control operations - execute
typedef struct
  {
    unsigned char type;                  // BIDIB_OUTTYPE_*
    unsigned char portnum;               // out number, 0 ... n
    unsigned char portstat;              // state of this output
  }  t_bidib_port;

// typedef for control operations - config
typedef struct
  {
    unsigned char portnum;
    unsigned char portmode;             // operation mode of port - default state?
    unsigned char pulstime;             // Holdtime for outputs
    unsigned char reserved1;            // 
    unsigned char reserved2;            // 
  } t_bidib_sport_cfg;                  // for Switch PORTs

typedef struct
  {
    unsigned char portnum;
    unsigned char brightness_off;       // Brightness in state OFF, range 0..255
    unsigned char brightness_on;        // Brightness in state ON, range 0..255
    unsigned char dimm_off;             // time for dimming towards OFF: 0=fast ... 255=slow
    unsigned char dimm_on;              // time for dimming towards ON: 0=fast ... 255=slow
  } t_bidib_lport_cfg;                  // for Light PORTs

typedef struct
  {
    unsigned char portnum;
    unsigned char dimm_off;             // time for dimming towards OFF: 0=fast ... 255=slow
    unsigned char dimm_on;              // time for dimming towards ON: 0=fast ... 255=slow
    unsigned char channel;              // mapping to physical channel
    unsigned char reserved0;            // 
  } t_bidib_backlport_cfg;              // for BACKLIGHT

typedef struct
  {
    unsigned char portnum;
    unsigned char adjust_low;
    unsigned char adjust_high;
    unsigned char speed;
    unsigned char reserved0;
  } t_bidib_servo_cfg;                  // for Servos
  
typedef struct 
  {
    unsigned char fb_state;             // state of the execution
    unsigned char error_code;           // 0 or error code
  } t_bidib_accessory_state;            // for accessory state messages

typedef struct                              //  t_bidib_cs_accessory
  {
    union
      {
        struct
          {
            unsigned char addrl;            // low byte of addr
            unsigned char addrh;            // high byte of addr
          };
        unsigned int  addr;                 // true dcc address (start with 0)
      };
    union
      {
        struct
          {
            unsigned char aspect: 5;        // the desired aspect
            unsigned char activate: 1;      // output (coil) state (only if control_mode == 0)
            unsigned char control_mode: 1;  // 0: direct coil control; 1:aspect mode
            unsigned char ext_accessory: 1; // 0: classic dcc; 1:extended accessory control
          };
        unsigned char  control;
      };
    unsigned char  time;
  } t_bidib_cs_accessory;

typedef struct                              //  t_bidib_bin_states
  {
    union
      {
        struct
          {
            unsigned char addrl;            // low byte of addr
            unsigned char addrh;            // high byte of addr
          };
        unsigned int  addr;                 // true dcc address (start with 0)
      };
    union
      {
        struct
          {
            unsigned char bin_numl;         // low byte of state  (this is little endian, DCC is big endian)
            unsigned char bin_numh;         // high byte of state
          };
        unsigned int  bin_num;
      };
    unsigned char data;
  } t_bidib_bin_state;

typedef struct                              // t_bidib_cs_drive
  {
    union
      {
        struct
          {
            unsigned char addrl;            // low byte of addr
            unsigned char addrh;            // high byte of addr
          };
        unsigned int  addr;                 // true dcc address (start with 0)
      };
    unsigned char format;                   // BIDIB_CS_DRIVE_FORMAT_DCC14, _DCC28, _DCC128
    unsigned char active;                   // BIDIB_CS_DRIVE_SPEED_BIT,
                                            // BIDIB_CS_DRIVE_F1F4_BIT     (1<<1)
                                            // BIDIB_CS_DRIVE_F5F8_BIT     (1<<2)
                                            // BIDIB_CS_DRIVE_F9F12_BIT    (1<<3)
                                            // BIDIB_CS_DRIVE_F13F20_BIT   (1<<4)
                                            // BIDIB_CS_DRIVE_F21F28_BIT   (1<<5)
    unsigned char speed;                    // like DCC, MSB=1:forward, MSB=0:revers, speed=1: ESTOP
    union
      {
        struct
          {
            unsigned char f4_f1: 4;         // functions f4..f1
            unsigned char light: 1;         // f0
          };
        unsigned char  f4_f0;
      };
    union
      {
        struct
          {
            unsigned char f8_f5: 4;         // functions f8..f5
            unsigned char f12_f9: 4;        // functions f12..f9
          };
        unsigned char  f12_f5;
      };
    union
      {
        struct
          {
            unsigned char f16_f13: 4;       // functions f16..f13
            unsigned char f20_f17: 4;       // functions f20..f17
          };
        unsigned char  f20_f13;
      };
    union
      {
        struct
          {
            unsigned char f24_f21: 4;       // functions f24..f21
            unsigned char f28_f25: 4;       // functions f28..f25
          };
        unsigned char  f28_f21;
      };
  } t_bidib_cs_drive;

typedef struct                              // t_bidib_cs_pom
  {
    union
      {
        struct
          {
            union
              {
                struct
                  {
                    unsigned char addrl;    // low byte of addr
                    unsigned char addrh;    // high byte of addr
                  };
                unsigned int  addr;         // true dcc address (start with 0)
              };
            unsigned char addrxl;           // 0 for normal POM
            unsigned char addrxh;           // 0 for normal POM
            unsigned char mid;              // manufactorer ID: 0 for normal POM, else VendorID like DCC
          };
        unsigned char did[5];                 // true dcc address (start with 0)
      };
    unsigned char opcode;                   // 0=RdBlock, 1=RdByte, 2=WrBit, 3=WrByte
                                            // 80=XRdBlock, 81=XRdByte, 82=XWrBit, 83=XWrByte
                                            // see below: BIDIB_CS_POM_RD_BLOCK ...
    union
      {
        struct
          {
            unsigned char cv_addrl;         // low byte of cv addr
            unsigned char cv_addrh;         // high byte of cv addr
          };
        unsigned int  cv_addr;              // true cv address (start with 0)
      };
    unsigned char cv_addrx;                 //
    unsigned char data;
  } t_bidib_cs_pom;

typedef struct                              // t_bidib_cs_pom
  {
    unsigned char opcode;                   // 0=Break, 1=RdByte, 2=WrBit, 3=WrByte
    union
      {
        struct
          {
            unsigned char cv_addrl;         // low byte of cv addr
            unsigned char cv_addrh;         // high byte of cv addr
          };
        unsigned int  cv_addr;              // true cv address (start with 0)
      };
    unsigned char data;
  } t_bidib_cs_prog;

typedef struct                              // t_bidib_cs_pom
  {
    unsigned char result;                   // 
    unsigned char time;                   // 
    union
      {
        struct
          {
            unsigned char cv_addrl;         // low byte of cv addr
            unsigned char cv_addrh;         // high byte of cv addr
          };
        unsigned int  cv_addr;              // true cv address (start with 0)
      };
    unsigned char data;
  } t_bidib_cs_prog_state;

//===============================================================================
//
// 4. Feature Codes
//
//===============================================================================

#define FEATURE_BM_SIZE                     0   // number of occupancy detectors
#define FEATURE_BM_ON                       1   // occupancy detection on/off
#define FEATURE_BM_SECACK_AVAILABLE         2   // secure ack available
#define FEATURE_BM_SECACK_ON                3   // secure ack on/off
#define FEATURE_BM_CURMEAS_AVAILABLE        4   // occupancy detectors with current measurement results
#define FEATURE_BM_CURMEAS_INTERVAL         5   // interval for current measurements
#define FEATURE_BM_DC_MEAS_AVAILABLE        6   // (dc) measurements available, even if track voltage is off
#define FEATURE_BM_DC_MEAS_ON               7   // dc measurement enabled
#define FEATURE_BM_ADDR_DETECT_AVAILABLE    8   // detector ic capable to detect loco address
#define FEATURE_BM_ADDR_DETECT_ON           9   // address detection enabled
#define FEATURE_BM_ADDR_AND_DIR            10   // address detection contains direction
#define FEATURE_BM_ISTSPEED_AVAILABLE      11   // speed messages enabled
#define FEATURE_BM_ISTSPEED_INTERVAL       12   // speed update interval
#define FEATURE_BM_CV_AVAILABLE            13   // CV readback available
#define FEATURE_BM_CV_ON                   14   // CV readback enabled
//-- booster
#define FEATURE_BST_VOLT_ADJUSTABLE        15   // booster output voltage is adjustable
#define FEATURE_BST_VOLT                   16   // booster output voltage setting (unit: V)
#define FEATURE_BST_CUTOUT_AVAIALABLE      17   // booster can do cutout for railcom
#define FEATURE_BST_CUTOUT_ON              18   // cutout is enabled
#define FEATURE_BST_TURNOFF_TIME           19   // time in ms until booster turns off in case of a short
#define FEATURE_BST_INRUSH_TURNOFF_TIME    20   // time in ms until booster turns off in case of a short after the first power up
#define FEATURE_BST_AMPERE_ADJUSTABLE      21   // booster output current is adjustable
#define FEATURE_BST_AMPERE                 22   // booster output current value (special coding)
#define FEATURE_BST_CURMEAS_INTERVAL       23   // current update interval
#define FEATURE_BST_CV_AVAILABLE           24   // CV readback available
#define FEATURE_BST_CV_ON                  25   // CV readback enabled
#define FEATURE_BST_INHIBIT_AUTOSTART      26   // 1: Booster does no automatic BOOST_ON when DCC at input wakes up.
#define FEATURE_BST_INHIBIT_LOCAL_ONOFF    27   // 1: Booster annouces local STOP/GO key stroke only, no local action

//-- booster/occupancy2
#define FEATURE_BM_DYN_STATE_INTERVAL      28   // transmit interval of MSG_BM_DYN_STATE (unit 100ms)


//-- accessory
#define FEATURE_ACCESSORY_COUNT            40   // number of objects
#define FEATURE_ACCESSORY_SURVEILLED       41   // 1: annouce if operated outside bidib
#define FEATURE_ACCESSORY_MACROMAPPED      42   // 1..n: no of accessory aspects are mapped to macros

//-- control
#define FEATURE_CTRL_INPUT_COUNT           50   // number of inputs for keys
#define FEATURE_CTRL_INPUT_NOTIFY          51   // 1: report a keystroke to host
#define FEATURE_CTRL_SPORT_COUNT           52   // number of switch ports (direct controlled)
#define FEATURE_CTRL_LPORT_COUNT           53   // number of light ports (direct controlled)
#define FEATURE_CTRL_SERVO_COUNT           54   // number of servo ports (direct controlled)
#define FEATURE_CTRL_SOUND_COUNT           55   // number of sound ports (direct controlled)
#define FEATURE_CTRL_MOTOR_COUNT           56   // number of motor ports (direct controlled)
#define FEATURE_CTRL_ANALOG_COUNT          57   // number of analog ports (direct controlled)
#define FEATURE_CTRL_STRETCH_DIMM          58   // additional time stretch for dimming (for LPORT)
#define FEATURE_CTRL_BACKLIGHT_COUNT       59   // number of backlight ports (intensity direct controlled)
#define FEATURE_CTRL_MAC_LEVEL             60   // supported macro level
#define FEATURE_CTRL_MAC_SAVE              61   // number of permanent storage places for macros
#define FEATURE_CTRL_MAC_COUNT             62   // number of macros
#define FEATURE_CTRL_MAC_SIZE              63   // length of each macro (entries)
#define FEATURE_CTRL_MAC_START_MAN         64   // (local) manual control of macros enabled
#define FEATURE_CTRL_MAC_START_DCC         65   // (local) dcc control of macros enabled
#define FEATURE_CTRL_PORT_QUERY_AVAILABLE  66   // 1: node will answer to MSG_LC_OUTPUT_QUERY
#define FEATURE_SPORT_CONFIG_AVAILABLE     67   // 1: node has possibility to configure SPORTs

//-- dcc gen
#define FEATURE_GEN_SPYMODE                100  // 1: watch bidib handsets
#define FEATURE_GEN_WATCHDOG               101  // 0: no watchdog, 1: permanent update of MSG_CS_SET_STATE required, unit 100ms
#define FEATURE_GEN_DRIVE_ACK              102  // not used
#define FEATURE_GEN_SWITCH_ACK             103  // not used
#define FEATURE_GEN_LOK_DB_SIZE            104  //
#define FEATURE_GEN_LOK_DB_STRING          105  //
#define FEATURE_GEN_POM_REPEAT             106  // supported service modes
#define FEATURE_GEN_DRIVE_BUS              107  // 1: this node drive the dcc bus.
#define FEATURE_GEN_LOK_LOST_DETECT        108  // 1: command station annouces lost loco
#define FEATURE_GEN_NOTIFY_DRIVE_MANUAL    109  // 1: dcc gen reports manual operation

#define FEATURE_STRING_SIZE                252  // length of user strings, 0:n.a (default); allowed 8..24
#define FEATURE_RELEVANT_PID_BITS          253  // how many bits of 'vendor32' are relevant for PID (default 16, LSB aligned)
#define FEATURE_FW_UPDATE_MODE             254  // 0: no fw-update, 1: intel hex (max. 10 byte / record)
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
#define BIDIB_ERR_SUBPAKET                0x15  // Message in Subsystem Paket Size Error
#define BIDIB_ERR_OVERRUN                 0x16  // Message buffer in downstream overrun, messages lost.
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
// 8. Booster and Command Station Handling (useful defines)
//
//===============================================================================

#define BIDIB_BST_STATE_OFF           0x00  // Booster turned off
#define BIDIB_BST_STATE_OFF_SHORT     0x01  // Booster is off, output shortend
#define BIDIB_BST_STATE_OFF_HOT       0x02  // Booster off and too hot
#define BIDIB_BST_STATE_OFF_NOPOWER   0x03  // Booster has no mains
#define BIDIB_BST_STATE_OFF_GO_REQ    0x04  // Booster off and local go request is present
#define BIDIB_BST_STATE_OFF_HERE      0x05  // Booster off (was turned off by a local key)
#define BIDIB_BST_STATE_OFF_NO_DCC    0x06  // Booster is off (no DCC input)
#define BIDIB_BST_STATE_ON            0x80  // Booster on
#define BIDIB_BST_STATE_ON_LIMIT      0x81  // Booster on and critical current flows
#define BIDIB_BST_STATE_ON_HOT        0x82  // Booster on and is getting hot
#define BIDIB_BST_STATE_ON_STOP_REQ   0x83  // Booster on and a local stop request is present
#define BIDIB_BST_STATE_ON_HERE       0x84  // Booster on (was turned on by a local key)

#define BIDIB_BST_DIAG_I              0x00  // Current
#define BIDIB_BST_DIAG_V              0x01  // Voltage
#define BIDIB_BST_DIAG_T              0x02  // Temperatur

#define BIDIB_CS_STATE_OFF            0x00  // no DCC, DCC-line is static, not toggling
#define BIDIB_CS_STATE_STOP           0x01  // DCC, all speed setting = 0
#define BIDIB_CS_STATE_SOFTSTOP       0x02  // DCC, soft stop is progress
#define BIDIB_CS_STATE_GO             0x03  // DCC on (must be repeated if watchdog is on)
#define BIDIB_CS_STATE_GO_IGN_WD      0x04  // DCC on (watchdog ignored)
#define BIDIB_CS_STATE_PROG           0x08  // in Programming Mode (ready for commands)
#define BIDIB_CS_STATE_PROGBUSY       0x09  // in Programming Mode (busy)
#define BIDIB_CS_STATE_BUSY           0x0D  // busy
#define BIDIB_CS_STATE_QUERY          0xFF


#define BIDIB_CS_DRIVE_FORMAT_DCC14      0
#define BIDIB_CS_DRIVE_FORMAT_DCC28      2
#define BIDIB_CS_DRIVE_FORMAT_DCC128     3

#define BIDIB_CS_DRIVE_SPEED_BIT    (1<<0)
#define BIDIB_CS_DRIVE_F1F4_BIT     (1<<1)  // also FL
#define BIDIB_CS_DRIVE_F0F4_BIT     (1<<1)  // additional define, it is the same bit
#define BIDIB_CS_DRIVE_F5F8_BIT     (1<<2)
#define BIDIB_CS_DRIVE_F9F12_BIT    (1<<3)
#define BIDIB_CS_DRIVE_F13F20_BIT   (1<<4)
#define BIDIB_CS_DRIVE_F21F28_BIT   (1<<5)

#define BIDIB_CS_POM_RD_BLOCK             0
#define BIDIB_CS_POM_RD_BYTE              1
#define BIDIB_CS_POM_WR_BIT               2
#define BIDIB_CS_POM_WR_BYTE              3
#define BIDIB_CS_xPOM_RD_BLOCK         0x80
#define BIDIB_CS_xPOM_RD_BYTE          0x81
#define BIDIB_CS_xPOM_WR_BIT           0x82
#define BIDIB_CS_xPOM_WR_BYTE          0x83

#define BIDIB_CS_PROG_BREAK             0   // service mode commands (MSG_CS_PROG)
#define BIDIB_CS_PROG_QUERY             1
#define BIDIB_CS_PROG_RD_BYTE           2
#define BIDIB_CS_PROG_RDWR_BIT          3
#define BIDIB_CS_PROG_WR_BYTE           4

#define BIDIB_CS_PROG_START            0x00  // service mode answer (MSG_CS_PROG_STATE)
#define BIDIB_CS_PROG_RUNNING          0x01  // generell rule: MSB: 0: running, 1: finished
#define BIDIB_CS_PROG_OKAY             0x80  //               Bit6: 0: okay,    1: fail
#define BIDIB_CS_PROG_STOPPED          0xC0
#define BIDIB_CS_PROG_NO_LOCO          0xC1
#define BIDIB_CS_PROG_NO_ANSWER        0xC2
#define BIDIB_CS_PROG_SHORT            0xC3
#define BIDIB_CS_PROG_VERIFY_FAILED    0xC4

//===============================================================================
//
// 9. IO-Control and Macro (useful defines)
//
//===============================================================================

// Accessory parameter
#define BIDIB_ACCESSORY_PARA_MACROMAP  253   // following data defines a mapping
#define BIDIB_ACCESSORY_SWITCH_TIME    254   //

// Accessory states
#define BIDIB_ACC_STATE_DONE               0   // done, no feedback
#define BIDIB_ACC_STATE_WAIT               1   // plus time like railcom spec
#define BIDIB_ACC_STATE_NO_FB_AVAILABLE 0x02   // np feedback available
#define BIDIB_ACC_STATE_ERROR           0x80   // always if error, + error code
#define BIDIB_ACC_STATE_ERROR_MORE      0x40   // more errors are present
#define BIDIB_ACC_STATE_ERROR_NONE      0x00   // no (more) errors
#define BIDIB_ACC_STATE_ERROR_VOID      0x01   // no processing possilbe, illegal aspect
#define BIDIB_ACC_STATE_ERROR_CURRENT   0x02   // current comsumption to high
#define BIDIB_ACC_STATE_ERROR_LOWPOWER  0x03   // supply too low
#define BIDIB_ACC_STATE_ERROR_FUSE      0x04   // fuse blown
#define BIDIB_ACC_STATE_ERROR_TEMP      0x05   // temp too high
#define BIDIB_ACC_STATE_ERROR_POSITION  0x06   // feedback error
#define BIDIB_ACC_STATE_ERROR_MAN_OP    0x07   // manually operated
#define BIDIB_ACC_STATE_ERROR_BULB      0x10   // bulb blown
#define BIDIB_ACC_STATE_ERROR_SERVO     0x20   // servo broken
#define BIDIB_ACC_STATE_ERROR_SELFTEST  0x3F   // internal error

// Macro / Switch Pointparameters
// type codes
#define BIDIB_OUTTYPE_SPORT          0     // standard port
#define BIDIB_OUTTYPE_LPORT          1     // light port
#define BIDIB_OUTTYPE_SERVO          2
#define BIDIB_OUTTYPE_SOUND          3
#define BIDIB_OUTTYPE_MOTOR          4
#define BIDIB_OUTTYPE_ANALOG         5
#define BIDIB_OUTTYPE_BACKLIGHT      6

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

#define BIDIB_MSYS_ACC_OKAY_QIN1    243     // query input for 'pressed / activated' and send okay to accessory-module, if pressed, else send nok. (not waiting)
#define BIDIB_MSYS_ACC_OKAY_QIN0    242     // query input for 'released' and send okay to accessory-module, if pressed, else send nok. (not waiting)
#define BIDIB_MSYS_ACC_OKAY_NF      241     // send okay to accessory-module, no feedback available


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
