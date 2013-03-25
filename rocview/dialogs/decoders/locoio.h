/*
 Copyright (C) 2002-2012 Rob Versluis, Rocrail.net

 Without an official permission commercial use is not permitted.
 Forking this project is not permitted.
 */
#ifndef _LOCOIO_H_
#define _LOCOIO_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "locoio.h"
#endif

/*!
 * Includes
 */

////@begin includes
#include "wx/notebook.h"
#include "wx/spinctrl.h"
#include "wx/statline.h"
////@end includes
#include "wx/timer.h"

#include "rocs/public/node.h"
#include "rocs/public/queue.h"
#include "rocs/public/file.h"

/*!
 * Forward declarations
 */

////@begin forward declarations
class wxSpinCtrl;
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_LOCOIO 10003
#define wxID_LOCOIO_LOW_ADDR 10027
#define ID_BUTTON_GET_GEN 10018
#define ID_BUTTON_SET_GEN 10019
#define wxID_LOCOIO_SUB_ADDR 10028
#define ID_BUTTON_RESET_LOCOIO 10006
#define ID_LN_ADDRESSLIST 10007
#define ID_QUEURY_ADDRESSES 10008
#define ID_LOCOIO_REPORT 10013
#define ID_EASYTEST1 10014
#define ID_EASYTEST2 10015
#define ID_EASYTEST3 10023
#define ID_EASYTEST4 10024
#define ID_EASYTEST5 10025
#define ID_EASYTEST6 10026
#define ID_EASYTEST7 10029
#define ID_EASYTEST8 10030
#define ID_EASYTEST9 10031
#define ID_EASYTEST10 10032
#define ID_EASYTEST11 10033
#define ID_EASYTEST12 10034
#define ID_EASYTEST13 10035
#define ID_EASYTEST14 10036
#define ID_EASYTEST15 10037
#define ID_EASYTEST16 10038
#define ID_EASYGETALL 10004
#define ID_EASYSETALL 10005
#define ID_BUTTON_EASY_SAVE 10009
#define ID_BUTTON_EASY_READ 10010
#define ID_LOCOIO_BOOSTER 10002
#define ID_BUTTON_PORT_GET 10016
#define ID_BUTTON_PORT_SET 10017
#define ID_LOCOIO_TYPE 10022
#define ID_BUTTON_OPC_GET 10000
#define ID_BUTTON_OPC_SET 10001
#define ID_BUTTON_MP_READ 10011
#define ID_BUTTON_MP_WRITE 10012
#define ID_BUTTON_SERVO_PORT_GET 10020
#define ID_BUTTON_SERVO_PORT_SET 10021
#define SYMBOL_LOCOIO_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxTAB_TRAVERSAL
#define SYMBOL_LOCOIO_TITLE _("LocoIO")
#define SYMBOL_LOCOIO_IDNAME ID_LOCOIO
#define SYMBOL_LOCOIO_SIZE wxDefaultSize
#define SYMBOL_LOCOIO_POSITION wxDefaultPosition
////@end control identifiers

#define ME_SVTimer 10099

/*!
 * LocoIO class declaration
 */

class LocoIO: public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( LocoIO )
    DECLARE_EVENT_TABLE()
    void initLabels();
    void sendPacket();
    void OnTimer(wxTimerEvent& event);
    void easyGetPort( int port, int* conf, int* val1, int* val2 );
    void OnEasyTypeSelected();

    int m_iLowAddress;
    int m_iSubAddress;
    iOQueue m_Queue;
    iONode m_SendedCmd;
    iOFile m_ReportFile;
    int m_iReportIdx;
    bool m_bReporting;
    wxTimer* m_Timer;
    int m_TabAlign;
    int m_SV[256];
    bool m_EasyFlip[17];

public:
    /// Constructors
    LocoIO();
    LocoIO( wxWindow* parent, wxWindowID id = SYMBOL_LOCOIO_IDNAME, const wxString& caption = SYMBOL_LOCOIO_TITLE, const wxPoint& pos = SYMBOL_LOCOIO_POSITION, const wxSize& size = SYMBOL_LOCOIO_SIZE, long style = SYMBOL_LOCOIO_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_LOCOIO_IDNAME, const wxString& caption = SYMBOL_LOCOIO_TITLE, const wxPoint& pos = SYMBOL_LOCOIO_POSITION, const wxSize& size = SYMBOL_LOCOIO_SIZE, long style = SYMBOL_LOCOIO_STYLE );

    /// Destructor
    ~LocoIO();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

////@begin LocoIO event handler declarations

    /// wxEVT_COMMAND_SPINCTRL_UPDATED event handler for wxID_LOCOIO_LOW_ADDR
    void OnLocoioLowAddrUpdated( wxSpinEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_GET_GEN
    void OnButtonGetGenClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_SET_GEN
    void OnButtonSetGenClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_SPINCTRL_UPDATED event handler for wxID_LOCOIO_SUB_ADDR
    void OnLocoioSubAddrUpdated( wxSpinEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_RESET_LOCOIO
    void OnButtonResetLocoioClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_LISTBOX_SELECTED event handler for ID_LN_ADDRESSLIST
    void OnLnAddresslistDoubleClicked( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_QUEURY_ADDRESSES
    void OnQueuryAddressesClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_LOCOIO_REPORT
    void OnLocoIOReport( wxCommandEvent& event );

    /// wxEVT_COMMAND_RADIOBOX_SELECTED event handler for wxID_ANY
    void OnEasyTypeSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_EASYTEST1
    void OnEasytestClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_EASYGETALL
    void OnEasygetallClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_EASYSETALL
    void OnEasysetallClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_EASY_SAVE
    void OnButtonEasySaveClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_EASY_READ
    void OnButtonEasyReadClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_LOCOIO_BOOSTER
    void OnBoosterClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_SPINCTRL_UPDATED event handler for wxID_ANY
    void OnAnyUpdated( wxSpinEvent& event );

    /// wxEVT_SCROLL_LINEUP event handler for wxID_ANY
    void OnAnyUp( wxSpinEvent& event );

    /// wxEVT_SCROLL_LINEDOWN event handler for wxID_ANY
    void OnAnyDown( wxSpinEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_PORT_GET
    void OnButtonPortGetClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_PORT_SET
    void OnButtonPortSetClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_RADIOBOX_SELECTED event handler for ID_LOCOIO_TYPE
    void OnLocoioTypeSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_OPC_GET
    void OnButtonOpcGetClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_OPC_SET
    void OnButtonOpcSetClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_MP_READ
    void OnButtonMpReadClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_MP_WRITE
    void OnButtonMpWriteClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_SERVO_PORT_GET
    void OnButtonServoPortGetClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_SERVO_PORT_SET
    void OnButtonServoPortSetClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
    void OnCancelClick( wxCommandEvent& event );

////@end LocoIO event handler declarations

////@begin LocoIO member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end LocoIO member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin LocoIO member variables
    wxPanel* m_GeneralPanel;
    wxStaticBox* m_AddressBox;
    wxStaticText* m_labIID;
    wxTextCtrl* m_IID;
    wxStaticText* m_labLowAddr;
    wxSpinCtrl* m_LowAddr;
    wxButton* m_GetGen;
    wxButton* m_SetGen;
    wxStaticText* m_labSubAddr;
    wxSpinCtrl* m_SubAddr;
    wxStaticBox* m_SetupBox;
    wxCheckBox* m_PortRefresh;
    wxCheckBox* m_FilpButtonCode;
    wxCheckBox* m_4PosServo;
    wxCheckBox* m_Port_5_12_Servos;
    wxStaticText* m_labFlashFreq;
    wxSpinCtrl* m_FlashFreq;
    wxStaticBox* m_VersionBox;
    wxStaticText* m_labVersion;
    wxTextCtrl* m_Version;
    wxStaticText* m_labModConfig;
    wxTextCtrl* m_ModConfig;
    wxPanel* m_AddressesPanel;
    wxListBox* m_AddressList;
    wxButton* m_QueryAddresses;
    wxButton* m_Report;
    wxPanel* m_EasyPanel;
    wxStaticText* m_labEasyPort;
    wxStaticText* m_labEasyAddress;
    wxStaticText* m_labEasyTestL;
    wxSpinCtrl* m_EasyAddr1;
    wxRadioBox* m_EasyType1;
    wxCheckBox* m_EasyContact1;
    wxButton* m_EasyTest1;
    wxSpinCtrl* m_EasyAddr2;
    wxRadioBox* m_EasyType2;
    wxCheckBox* m_EasyContact2;
    wxButton* m_EasyTest2;
    wxSpinCtrl* m_EasyAddr3;
    wxRadioBox* m_EasyType3;
    wxCheckBox* m_EasyContact3;
    wxButton* m_EasyTest3;
    wxSpinCtrl* m_EasyAddr4;
    wxRadioBox* m_EasyType4;
    wxCheckBox* m_EasyContact4;
    wxButton* m_EasyTest4;
    wxSpinCtrl* m_EasyAddr5;
    wxRadioBox* m_EasyType5;
    wxCheckBox* m_EasyContact5;
    wxButton* m_EasyTest5;
    wxSpinCtrl* m_EasyAddr6;
    wxRadioBox* m_EasyType6;
    wxCheckBox* m_EasyContact6;
    wxButton* m_EasyTest6;
    wxSpinCtrl* m_EasyAddr7;
    wxRadioBox* m_EasyType7;
    wxCheckBox* m_EasyContact7;
    wxButton* m_EasyTest7;
    wxSpinCtrl* m_EasyAddr8;
    wxRadioBox* m_EasyType8;
    wxCheckBox* m_EasyContact8;
    wxButton* m_EasyTest8;
    wxStaticText* m_labEasyPort2;
    wxStaticText* m_labEasyAddress2;
    wxStaticText* m_labEasyTestR;
    wxSpinCtrl* m_EasyAddr9;
    wxRadioBox* m_EasyType9;
    wxCheckBox* m_EasyContact9;
    wxButton* m_EasyTest9;
    wxSpinCtrl* m_EasyAddr10;
    wxRadioBox* m_EasyType10;
    wxCheckBox* m_EasyContact10;
    wxButton* m_EasyTest10;
    wxSpinCtrl* m_EasyAddr11;
    wxRadioBox* m_EasyType11;
    wxCheckBox* m_EasyContact11;
    wxButton* m_EasyTest11;
    wxSpinCtrl* m_EasyAddr12;
    wxRadioBox* m_EasyType12;
    wxCheckBox* m_EasyContact12;
    wxButton* m_EasyTest12;
    wxSpinCtrl* m_EasyAddr13;
    wxRadioBox* m_EasyType13;
    wxCheckBox* m_EasyContact13;
    wxButton* m_EasyTest13;
    wxSpinCtrl* m_EasyAddr14;
    wxRadioBox* m_EasyType14;
    wxCheckBox* m_EasyContact14;
    wxButton* m_EasyTest14;
    wxSpinCtrl* m_EasyAddr15;
    wxRadioBox* m_EasyType15;
    wxCheckBox* m_EasyContact15;
    wxButton* m_EasyTest15;
    wxSpinCtrl* m_EasyAddr16;
    wxRadioBox* m_EasyType16;
    wxCheckBox* m_EasyContact16;
    wxButton* m_EasyTest16;
    wxButton* m_EasyGetAll;
    wxButton* m_EasySetAll;
    wxButton* m_EasySaveAll;
    wxButton* m_EasyReadAll;
    wxCheckBox* m_Booster;
    wxPanel* m_IOPanel;
    wxStaticText* m_labPort;
    wxSpinCtrl* m_Port;
    wxButton* m_PortGet;
    wxButton* m_PortSet;
    wxStaticText* m_labPortAddr;
    wxSpinCtrl* m_PortAddr;
    wxRadioBox* m_SensorContact;
    wxRadioBox* m_PortType;
    wxStaticBox* m_PortSetupBox;
    wxStaticBox* m_OutputSetupBox;
    wxCheckBox* m_LowAtStartup;
    wxCheckBox* m_HardwareReset;
    wxCheckBox* m_PulseContact;
    wxCheckBox* m_Flash;
    wxCheckBox* m_Multi;
    wxCheckBox* m_BlockDetector;
    wxStaticBox* m_RegisterBox;
    wxStaticText* m_labConfig;
    wxTextCtrl* m_Config;
    wxTextCtrl* m_ConfigHex;
    wxStaticText* m_labVal1;
    wxTextCtrl* m_Val1;
    wxTextCtrl* m_Val1Hex;
    wxStaticText* m_labVal2;
    wxTextCtrl* m_Val2;
    wxTextCtrl* m_Val2Hex;
    wxStaticBox* m_SensorSetupBox;
    wxCheckBox* m_SensorActiveLow;
    wxCheckBox* m_SensorDelay;
    wxCheckBox* m_SensorTurnout;
    wxCheckBox* m_SensorPulse;
    wxRadioBox* m_SensorMessage;
    wxRadioBox* m_InputType;
    wxPanel* m_OPCPanel;
    wxStaticText* m_labOPCPort;
    wxSpinCtrl* m_OPCPort;
    wxButton* m_OPCGet;
    wxButton* m_OPCSet;
    wxStaticText* m_labOPC;
    wxSpinCtrl* m_OPC;
    wxStaticText* m_labARG1;
    wxSpinCtrl* m_ARG1;
    wxStaticText* m_labARG2;
    wxSpinCtrl* m_ARG2;
    wxPanel* m_MultiPortPanel;
    wxCheckBox* m_MP1;
    wxCheckBox* m_MP2;
    wxCheckBox* m_MP3;
    wxCheckBox* m_MP4;
    wxCheckBox* m_MP5;
    wxCheckBox* m_MP6;
    wxCheckBox* m_MP7;
    wxCheckBox* m_MP8;
    wxCheckBox* m_MP9;
    wxCheckBox* m_MP10;
    wxCheckBox* m_MP11;
    wxCheckBox* m_MP12;
    wxCheckBox* m_MP13;
    wxCheckBox* m_MP14;
    wxCheckBox* m_MP15;
    wxCheckBox* m_MP16;
    wxButton* m_MPRead;
    wxButton* m_MPWrite;
    wxStaticText* m_labServoPort;
    wxSpinCtrl* m_ServoPort;
    wxButton* m_ServoPortGet;
    wxButton* m_ServoPortSet;
    wxStaticBox* m_ServoBox;
    wxStaticText* m_labPos1;
    wxSpinCtrl* m_Pos1;
    wxStaticText* m_labPos2;
    wxSpinCtrl* m_Pos2;
    wxStaticText* m_labV;
    wxSpinCtrl* m_V;
    wxButton* m_Cancel;
////@end LocoIO member variables
    
    void event( iONode event );
    void evaluateEvent( int type, int lowaddr, int subaddr, int sv, int val, int ver );
    void evaluatePort( int sv, int val );
    void evaluateServo( int sv, int val );
    void evaluateOPC( int sv, int val );
    int makeSV0();
    int makeSVConfigPort();
    int makeSVParam1Port();
    int makeSVParam2Port();
    void make3Packet( int sv_offset, int port, int val1, int val2, int val3, bool sv_set, bool sendnow=true );
    void queryAddresses();
};

#endif
    // _LOCOIO_H_
