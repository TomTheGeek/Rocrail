///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Dec 21 2009)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __cbusdlggen__
#define __cbusdlggen__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/spinctrl.h>
#include <wx/sizer.h>
#include <wx/radiobox.h>
#include <wx/checkbox.h>
#include <wx/statline.h>
#include <wx/statbox.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class cbusdlggen
///////////////////////////////////////////////////////////////////////////////
class cbusdlggen : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_labIID;
		wxTextCtrl* m_IID;
		wxStaticText* m_labCANID;
		wxSpinCtrl* m_CANID;
		wxRadioBox* m_Sublib;
		wxCheckBox* m_CTS;
		wxStaticText* m_labDevice;
		wxTextCtrl* m_Device;
		wxStaticText* m_labHost;
		wxTextCtrl* m_Host;
		wxStaticText* m_labPort;
		wxSpinCtrl* m_Port;
		wxStaticLine* m_staticline1;
		wxStaticBoxSizer* m_OptionsBox;
		wxCheckBox* m_ShortEvents;
		wxCheckBox* m_FonFof;
		wxStaticText* m_labSOD;
		wxSpinCtrl* m_SOD;
		wxStaticText* m_labSwTime;
		wxSpinCtrl* m_SwTime;
		wxStaticText* m_labPurgetime;
		wxSpinCtrl* m_Purgetime;
		wxStaticText* m_labLoaderTime;
		wxSpinCtrl* m_LoaderTime;
		wxStdDialogButtonSizer* m_StdButton;
		wxButton* m_StdButtonOK;
		wxButton* m_StdButtonCancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnSublib( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCancel( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnOK( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		cbusdlggen( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("MERG CBUS"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
		~cbusdlggen();
	
};

#endif //__cbusdlggen__
