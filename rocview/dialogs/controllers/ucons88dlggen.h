///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep 12 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __ucons88dlggen__
#define __ucons88dlggen__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/spinctrl.h>
#include <wx/statbox.h>
#include <wx/panel.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class ucons88dlggen
///////////////////////////////////////////////////////////////////////////////
class ucons88dlggen : public wxDialog 
{
	private:
	
	protected:
		wxPanel* m_General;
		wxStaticText* m_labIID;
		wxTextCtrl* m_IID;
		wxStaticBoxSizer* m_DeviceBox;
		wxStaticText* m_labHost;
		wxTextCtrl* m_Host;
		wxStaticText* m_labPort;
		wxSpinCtrl* m_Port;
		wxStaticBoxSizer* m_S88BusBox;
		wxStaticText* m_labBus1;
		wxSpinCtrl* m_B1;
		wxStaticText* m_labBus2;
		wxSpinCtrl* m_B2;
		wxStaticText* m_labBus3;
		wxSpinCtrl* m_B3;
		wxStdDialogButtonSizer* m_StdButton;
		wxButton* m_StdButtonOK;
		wxButton* m_StdButtonCancel;
		wxButton* m_StdButtonHelp;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnCancel( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnHelp( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnOK( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		ucons88dlggen( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("µCon-S88-Master Setup"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 315,430 ), long style = wxDEFAULT_DIALOG_STYLE );
		~ucons88dlggen();
	
};

#endif //__ucons88dlggen__
