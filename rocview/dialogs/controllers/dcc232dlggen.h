///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 17 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __dcc232dlggen__
#define __dcc232dlggen__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/checkbox.h>
#include <wx/spinctrl.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class dcc232gen
///////////////////////////////////////////////////////////////////////////////
class dcc232gen : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_labIID;
		wxTextCtrl* m_IID;
		wxStaticBoxSizer* m_PortBox;
		wxStaticText* m_labDevice;
		wxTextCtrl* m_Device;
		wxStaticBoxSizer* m_SlotBox;
		wxCheckBox* m_AutoPurge;
		wxStaticText* m_labPurgeTime;
		wxSpinCtrl* m_PurgeTime;
		wxStaticBoxSizer* m_ShortCutBox;
		wxCheckBox* m_ShortcutDetection;
		wxStaticText* m_labShortcutDelay;
		wxSpinCtrl* m_ShortcutDelay;
		wxStaticText* m_labShortcutDelayMs;
		wxStdDialogButtonSizer* m_DefaultButtons;
		wxButton* m_DefaultButtonsOK;
		wxButton* m_DefaultButtonsCancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void onCancel( wxCommandEvent& event ){ event.Skip(); }
		virtual void onOK( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		dcc232gen( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("DCC232"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
		~dcc232gen();
	
};

#endif //__dcc232dlggen__
