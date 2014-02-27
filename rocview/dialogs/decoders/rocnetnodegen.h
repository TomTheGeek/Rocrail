///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __rocnetnodegen__
#define __rocnetnodegen__

#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/statbmp.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/sizer.h>
#include <wx/listctrl.h>
#include <wx/button.h>
#include <wx/panel.h>
#include <wx/stattext.h>
#include <wx/treectrl.h>
#include <wx/statline.h>
#include <wx/textctrl.h>
#include <wx/spinctrl.h>
#include <wx/checkbox.h>
#include <wx/radiobox.h>
#include <wx/statbox.h>
#include <wx/tglbtn.h>
#include <wx/grid.h>
#include <wx/notebook.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class rocnetnodegen
///////////////////////////////////////////////////////////////////////////////
class rocnetnodegen : public wxDialog 
{
	private:
	
	protected:
		wxNotebook* m_NodeBook;
		wxPanel* m_IndexPanel;
		wxStaticBitmap* m_RocNetLogo;
		wxStaticBitmap* m_GCALogo;
		wxListCtrl* m_NodeList;
		wxButton* m_Shutdown;
		wxButton* m_ShutdownAll;
		wxButton* m_Show;
		wxButton* m_Query;
		wxButton* m_Report;
		wxButton* m_Shell;
		wxPanel* m_RocNetPanel;
		wxStaticText* m_labNodemap;
		wxTreeCtrl* m_NodeTree;
		wxStaticLine* m_staticline1;
		wxStaticText* m_labIID;
		wxTextCtrl* m_IID;
		wxStaticText* m_labID;
		wxSpinCtrl* m_ID;
		wxStaticText* m_labLocation;
		wxSpinCtrl* m_Location;
		wxStaticText* m_labVendor;
		wxTextCtrl* m_VendorName;
		wxStaticText* m_labProduct;
		wxTextCtrl* m_ProductName;
		wxStaticText* m_labIO;
		wxTextCtrl* m_IO;
		wxStaticText* m_labSubIP;
		wxTextCtrl* m_SubIP;
		wxStaticText* m_labVersion;
		wxTextCtrl* m_Version;
		wxStaticText* m_labNickname;
		wxTextCtrl* m_Nickname;
		wxButton* m_RocnetWrite;
		wxPanel* m_OptionsPanel;
		wxStaticBoxSizer* m_RocNetOptionBox;
		wxCheckBox* m_SecAck;
		wxCheckBox* m_UsePB;
		wxCheckBox* m_RFID;
		wxCheckBox* m_ADCSensor;
		wxCheckBox* m_Mobile;
		wxRadioBox* m_IOType;
		wxRadioBox* m_DCCType;
		wxRadioBox* m_DCCDevice;
		wxStaticBoxSizer* m_TraceLevelsBox;
		wxCheckBox* m_TraceLevelInfo;
		wxCheckBox* m_TraceLevelMonitor;
		wxStaticLine* m_staticline2;
		wxStaticBoxSizer* m_I2CScan;
		wxStaticText* m_labI2CScan20;
		wxTextCtrl* m_I2CScan20;
		wxStaticText* m_labI2CScan40;
		wxTextCtrl* m_I2CScan40;
		wxStaticText* m_labI2CScan30;
		wxTextCtrl* m_I2CScan30;
		wxStaticText* m_labI2CScan48;
		wxTextCtrl* m_I2CScan48;
		wxButton* m_NodeOptionsRead;
		wxButton* m_NodeOptionsWrite;
		wxPanel* m_PortSetupPanel;
		wxStaticText* m_labPort;
		wxStaticText* m_labType;
		wxStaticText* m_labDelay;
		wxStaticText* m_labBlink;
		wxStaticText* m_labPortInv;
		wxStaticText* m_labPortToggle;
		wxStaticText* m_labPortTest;
		wxStaticText* m_labPortEventID;
		wxStaticText* m_labPortEventPort;
		wxStaticText* m_labPort1;
		wxRadioBox* m_Type1;
		wxSpinCtrl* m_Delay1;
		wxCheckBox* m_Blink1;
		wxCheckBox* m_PortInv1;
		wxCheckBox* m_PortToggle1;
		wxButton* m_PortTest1;
		wxSpinCtrl* m_PortEventID1;
		wxSpinCtrl* m_PortEventPort1;
		wxStaticText* m_labPort2;
		wxRadioBox* m_Type2;
		wxSpinCtrl* m_Delay2;
		wxCheckBox* m_Blink2;
		wxCheckBox* m_PortInv2;
		wxCheckBox* m_PortToggle2;
		wxButton* m_PortTest2;
		wxSpinCtrl* m_PortEventID2;
		wxSpinCtrl* m_PortEventPort2;
		wxStaticText* m_labPort3;
		wxRadioBox* m_Type3;
		wxSpinCtrl* m_Delay3;
		wxCheckBox* m_Blink3;
		wxCheckBox* m_PortInv3;
		wxCheckBox* m_PortToggle3;
		wxButton* m_PortTest3;
		wxSpinCtrl* m_PortEventID3;
		wxSpinCtrl* m_PortEventPort3;
		wxStaticText* m_labPort4;
		wxRadioBox* m_Type4;
		wxSpinCtrl* m_Delay4;
		wxCheckBox* m_Blink4;
		wxCheckBox* m_PortInv4;
		wxCheckBox* m_PortToggle4;
		wxButton* m_PortTest4;
		wxSpinCtrl* m_PortEventID4;
		wxSpinCtrl* m_PortEventPort4;
		wxStaticText* m_labPort5;
		wxRadioBox* m_Type5;
		wxSpinCtrl* m_Delay5;
		wxCheckBox* m_Blink5;
		wxCheckBox* m_PortInv5;
		wxCheckBox* m_PortToggle5;
		wxButton* m_PortTest5;
		wxSpinCtrl* m_PortEventID5;
		wxSpinCtrl* m_PortEventPort5;
		wxStaticText* m_labPort6;
		wxRadioBox* m_Type6;
		wxSpinCtrl* m_Delay6;
		wxCheckBox* m_Blink6;
		wxCheckBox* m_PortInv6;
		wxCheckBox* m_PortToggle6;
		wxButton* m_PortTest6;
		wxSpinCtrl* m_PortEventID6;
		wxSpinCtrl* m_PortEventPort6;
		wxStaticText* m_labPort7;
		wxRadioBox* m_Type7;
		wxSpinCtrl* m_Delay7;
		wxCheckBox* m_Blink7;
		wxCheckBox* m_PortInv7;
		wxCheckBox* m_PortToggle7;
		wxButton* m_PortTest7;
		wxSpinCtrl* m_PortEventID7;
		wxSpinCtrl* m_PortEventPort7;
		wxStaticText* m_labPort8;
		wxRadioBox* m_Type8;
		wxSpinCtrl* m_Delay8;
		wxCheckBox* m_Blink8;
		wxCheckBox* m_PortInv8;
		wxCheckBox* m_PortToggle8;
		wxButton* m_PortTest8;
		wxSpinCtrl* m_PortEventID8;
		wxSpinCtrl* m_PortEventPort8;
		wxStaticText* m_PortStatus;
		wxButton* m_PortPrev;
		wxButton* m_PortNext;
		wxButton* m_PortRemove;
		wxButton* m_PortRead;
		wxButton* m_PortWrite;
		wxPanel* m_PWMSetupPanel;
		wxStaticText* m_labChannel;
		wxStaticText* m_labOffPos;
		wxStaticText* m_labOnPos;
		wxStaticText* m_labOffSteps;
		wxStaticText* m_labOnSteps;
		wxStaticText* m_labChannelBlink;
		wxStaticText* m_labChannelReport;
		wxStaticText* m_labChannelServo;
		wxStaticText* m_labChannelDelay;
		wxStaticText* m_labChannelTune;
		wxStaticText* m_labChannelTest;
		wxStaticText* m_labChannel1;
		wxSpinCtrl* m_OffPos1;
		wxSpinCtrl* m_OnPos1;
		wxSpinCtrl* m_OffSteps1;
		wxSpinCtrl* m_OnSteps1;
		wxCheckBox* m_ChannelBlink1;
		wxCheckBox* m_ChannelReport1;
		wxCheckBox* m_ChannelServo1;
		wxSpinCtrl* m_ChannelDelay1;
		wxButton* m_ChannelTune1;
		wxToggleButton* m_ChannelTest1;
		wxStaticText* m_labChannel2;
		wxSpinCtrl* m_OffPos2;
		wxSpinCtrl* m_OnPos2;
		wxSpinCtrl* m_OffSteps2;
		wxSpinCtrl* m_OnSteps2;
		wxCheckBox* m_ChannelBlink2;
		wxCheckBox* m_ChannelReport2;
		wxCheckBox* m_ChannelServo2;
		wxSpinCtrl* m_ChannelDelay2;
		wxButton* m_ChannelTune2;
		wxToggleButton* m_ChannelTest2;
		wxStaticText* m_labChannel3;
		wxSpinCtrl* m_OffPos3;
		wxSpinCtrl* m_OnPos3;
		wxSpinCtrl* m_OffSteps3;
		wxSpinCtrl* m_OnSteps3;
		wxCheckBox* m_ChannelBlink3;
		wxCheckBox* m_ChannelReport3;
		wxCheckBox* m_ChannelServo3;
		wxSpinCtrl* m_ChannelDelay3;
		wxButton* m_ChannelTune3;
		wxToggleButton* m_ChannelTest3;
		wxStaticText* m_labChannel4;
		wxSpinCtrl* m_OffPos4;
		wxSpinCtrl* m_OnPos4;
		wxSpinCtrl* m_OffSteps4;
		wxSpinCtrl* m_OnSteps4;
		wxCheckBox* m_ChannelBlink4;
		wxCheckBox* m_ChannelReport4;
		wxCheckBox* m_ChannelServo4;
		wxSpinCtrl* m_ChannelDelay4;
		wxButton* m_ChannelTune4;
		wxToggleButton* m_ChannelTest4;
		wxStaticText* m_labChannel5;
		wxSpinCtrl* m_OffPos5;
		wxSpinCtrl* m_OnPos5;
		wxSpinCtrl* m_OffSteps5;
		wxSpinCtrl* m_OnSteps5;
		wxCheckBox* m_ChannelBlink5;
		wxCheckBox* m_ChannelReport5;
		wxCheckBox* m_ChannelServo5;
		wxSpinCtrl* m_ChannelDelay5;
		wxButton* m_ChannelTune5;
		wxToggleButton* m_ChannelTest5;
		wxStaticText* m_labChannel6;
		wxSpinCtrl* m_OffPos6;
		wxSpinCtrl* m_OnPos6;
		wxSpinCtrl* m_OffSteps6;
		wxSpinCtrl* m_OnSteps6;
		wxCheckBox* m_ChannelBlink6;
		wxCheckBox* m_ChannelReport6;
		wxCheckBox* m_ChannelServo6;
		wxSpinCtrl* m_ChannelDelay6;
		wxButton* m_ChannelTune6;
		wxToggleButton* m_ChannelTest6;
		wxStaticText* m_labChannel7;
		wxSpinCtrl* m_OffPos7;
		wxSpinCtrl* m_OnPos7;
		wxSpinCtrl* m_OffSteps7;
		wxSpinCtrl* m_OnSteps7;
		wxCheckBox* m_ChannelBlink7;
		wxCheckBox* m_ChannelReport7;
		wxCheckBox* m_ChannelServo7;
		wxSpinCtrl* m_ChannelDelay7;
		wxButton* m_ChannelTune7;
		wxToggleButton* m_ChannelTest7;
		wxStaticText* m_labChannel8;
		wxSpinCtrl* m_OffPos8;
		wxSpinCtrl* m_OnPos8;
		wxSpinCtrl* m_OffSteps8;
		wxSpinCtrl* m_OnSteps8;
		wxCheckBox* m_ChannelBlink8;
		wxCheckBox* m_ChannelReport8;
		wxCheckBox* m_ChannelServo8;
		wxSpinCtrl* m_ChannelDelay8;
		wxButton* m_ChannelTune8;
		wxToggleButton* m_ChannelTest8;
		wxStaticText* m_ChannelStatus;
		wxButton* m_ChannelPrev;
		wxButton* m_ChannelNext;
		wxButton* m_ChannelRemove;
		wxButton* m_ChannelRead;
		wxButton* m_ChannelWrite;
		wxPanel* m_MacroPanel;
		wxStaticText* m_labMacroNr;
		wxSpinCtrl* m_MacroNr;
		wxGrid* m_MacroLines;
		wxButton* m_MacroExport;
		wxButton* m_MacroImport;
		wxButton* m_MacroGet;
		wxButton* m_MacroSet;
		wxPanel* m_UpdatePanel;
		wxStaticText* m_labUpdate;
		wxTextCtrl* m_UpdateRevision;
		wxButton* m_Update;
		wxCheckBox* m_UpdateOffline;
		wxStaticText* m_labRevisionInfo;
		wxTextCtrl* m_RevisionInfo;
		wxStdDialogButtonSizer* m_StdButton;
		wxButton* m_StdButtonOK;
		
		// Virtual event handlers, overide them in your derived class
		virtual void onClose( wxCloseEvent& event ) { event.Skip(); }
		virtual void onRocNetLogo( wxMouseEvent& event ) { event.Skip(); }
		virtual void onGCALogo( wxMouseEvent& event ) { event.Skip(); }
		virtual void onBeginListDrag( wxListEvent& event ) { event.Skip(); }
		virtual void onListColClick( wxListEvent& event ) { event.Skip(); }
		virtual void onIndexSelected( wxListEvent& event ) { event.Skip(); }
		virtual void onShutdown( wxCommandEvent& event ) { event.Skip(); }
		virtual void onShutdownAll( wxCommandEvent& event ) { event.Skip(); }
		virtual void onShow( wxCommandEvent& event ) { event.Skip(); }
		virtual void onQuery( wxCommandEvent& event ) { event.Skip(); }
		virtual void onReport( wxCommandEvent& event ) { event.Skip(); }
		virtual void onShell( wxCommandEvent& event ) { event.Skip(); }
		virtual void onBeginDrag( wxTreeEvent& event ) { event.Skip(); }
		virtual void onItemActivated( wxTreeEvent& event ) { event.Skip(); }
		virtual void onTreeItemRightClick( wxTreeEvent& event ) { event.Skip(); }
		virtual void onTreeSelChanged( wxTreeEvent& event ) { event.Skip(); }
		virtual void onRocnetWrite( wxCommandEvent& event ) { event.Skip(); }
		virtual void onIOType( wxCommandEvent& event ) { event.Skip(); }
		virtual void onNodeOptionsRead( wxCommandEvent& event ) { event.Skip(); }
		virtual void onNodeOptionsWrite( wxCommandEvent& event ) { event.Skip(); }
		virtual void onPort1Drag( wxMouseEvent& event ) { event.Skip(); }
		virtual void onPortTest( wxCommandEvent& event ) { event.Skip(); }
		virtual void onPortPrev( wxCommandEvent& event ) { event.Skip(); }
		virtual void onPortNext( wxCommandEvent& event ) { event.Skip(); }
		virtual void onPortRemove( wxCommandEvent& event ) { event.Skip(); }
		virtual void onPortRead( wxCommandEvent& event ) { event.Skip(); }
		virtual void onPortWrite( wxCommandEvent& event ) { event.Skip(); }
		virtual void onChannelDrag( wxMouseEvent& event ) { event.Skip(); }
		virtual void onChannelTune( wxCommandEvent& event ) { event.Skip(); }
		virtual void onChannelTest( wxCommandEvent& event ) { event.Skip(); }
		virtual void onChannelPrev( wxCommandEvent& event ) { event.Skip(); }
		virtual void onChannelNext( wxCommandEvent& event ) { event.Skip(); }
		virtual void onChannelRemove( wxCommandEvent& event ) { event.Skip(); }
		virtual void onChannelRead( wxCommandEvent& event ) { event.Skip(); }
		virtual void onChannelWrite( wxCommandEvent& event ) { event.Skip(); }
		virtual void onMacroNumber( wxSpinEvent& event ) { event.Skip(); }
		virtual void onMacroLineChange( wxGridEvent& event ) { event.Skip(); }
		virtual void onMacroExport( wxCommandEvent& event ) { event.Skip(); }
		virtual void onMacroImport( wxCommandEvent& event ) { event.Skip(); }
		virtual void onMacroGet( wxCommandEvent& event ) { event.Skip(); }
		virtual void onMacroSet( wxCommandEvent& event ) { event.Skip(); }
		virtual void onNewRevisionNumber( wxCommandEvent& event ) { event.Skip(); }
		virtual void onUpdate( wxCommandEvent& event ) { event.Skip(); }
		virtual void onOK( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		rocnetnodegen( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("RocNetNode"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~rocnetnodegen();
	
};

#endif //__rocnetnodegen__
