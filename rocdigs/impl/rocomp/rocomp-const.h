#ifndef ROCOMP_CONST_H
#define ROCOMP_CONST_H

/* USB-Communication Header
   [DataLength | Header | Data n ]
   Fixed packet length is 64 byte. No CRC.
 */
#define USB_NOSEND                        0x00
#define USB_FIRMWARE_INFO                 0x01
#define USB_RF_SENDDATA                   0x03
#define USB_RF_SENDSTATE                  0x04
#define USB_RF_RECEIVEDDATA               0x05
#define USB_RF_ACK                        0x06
#define USB_GET_SERIALNUMBER              0x10
#define USB_GET_SETTINGS                  0x12
#define USB_SET_SETTINGS                  0x13
#define USB_GET_DCCSETTINGS               0x16
#define USB_SET_DCCSETTINGS               0x17
#define USB_GET_CODE                      0x18
#define USB_GET_WIRELESSMODULEVERSION     0x20
#define USB_XPRESSNET                     0x40
#define USB_SETAUTOINFORMFLAGS            0x50
#define USB_GETAUTOINFORMFLAGS            0x51
#define USB_RMBUS_DATACHANGED             0x80
#define USB_RMBUS_GETDATA                 0x81
#define USB_RMBUS_PROGRAMMODULE           0x82
#define USB_SYSTEMSTATE_DATACHANGED       0x84
#define USB_SYSTEMSTATE_GETDATA           0x85
#define USB_NETWORKCHILDREN_DATACHANGED   0x86
#define USB_NETWORKCHILDREN_GETDATA       0x87
#define USB_RAILCOM_DATACHANGED           0x88
#define USB_RAILCOM_GETDATA               0x89

// TCentralState
#define csEmergencyStop           0x01  // Der Nothalt ist eingeschaltet
#define csTrackVoltageOff         0x02  // Die Gleisspannung ist abgeschaltet
#define csShortCircuit            0x04  // Kurschluss am Gleisausgang
#define csNotDefined              0x08  // Nicht definiert bzw. unbekannt
#define csAutoMode                0x10  // Der Automatische Startmodus ist aktiv
#define csProgrammingModeActive   0x20  // Der Programmiermodus ist aktiv
#define csColdStart               0x40  // Kaltstart
#define csRamError                0x80  // RAM Fehler in der Zentrale

// TSpeedSteps
#define ss14                        0x00
#define ss28                        0x01
#define ss128                       0x02

// TStreetCommand
#define scGet                       0x10
#define scGetNext                   0x11
#define scGetNextEmpty              0x12
#define scGetPrev                   0x13
#define scGetPrevEmpty              0x14
#define scDelete                    0x20
#define scDeleteAll                 0x21
#define scSwitch                    0x30

// TStreetState
#define ssUnknown                   0x00
#define ssValid                     0x01
#define ssInValid                   0x02
#define ssSwitching                 0x03
#define ssChanged                   0x80

// TDTRState
#define dsErrorSameAddresses        0x00
#define dsErrorDifferentSpeedSteps  0x01
#define dsErrorSpeedNotZero         0x02
#define dsErrorAlreadyInDTR         0x03
#define dsErrorNotInDTR             0x04
#define dsErrorMemoryFull           0x05
#define dsErrorUnknown              0x10
#define dsOk                        0x20

// TDTRCommand
#define dcBuildDTR                  0x10
#define dcReleaseDTR                0x11
#define dcGetDTRInfos               0x12
#define dcSetDTRDirection           0x13
#define dcReleaseAllDTR             0x14

// TProgrammingResult
#define prUnknown                   0x00
#define prUserAbort                 0x01
#define prOk                        0x02
#define prShortCircuit              0x03
#define prNothingFound              0x04
#define prNotSupported              0x05

// TProgrammingType
#define ptNothing                   0x00
#define ptBitOnly                   0x01
#define ptByteOnly                  0x02
#define ptBoth                      0x03

// TWirelessLostMode
#define wlmTrackPowerOff            0x00
#define wlmEmergencyStop            0x01
#define wlmNothing                  0x02

// TTurnoutState
#define tsNone                      0x00
#define tsStraight                  0x01
#define tsBranch                    0x02
#define tsBoth                      0x03

// TLedOptions
#define loNone                      0x00
#define loSinglePulse               0x01
#define loSynchronize               0x02


// TAutoInformFlags
#define aiXPressNet           0x00000001
#define aiRMFeedBack          0x00000002
#define aiRailCom             0x00000004
#define aiMMUpdate            0x00000008
#define aiSystemState         0x00000100
#define aiWireless            0x00000200
#define aiWirelessConnect     0x00000400

#endif
