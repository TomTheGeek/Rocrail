///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 17 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __fxdlggen__
#define __fxdlggen__

#include <wx/string.h>
#include <wx/radiobox.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class fxdlggen
///////////////////////////////////////////////////////////////////////////////
class fxdlggen : public wxDialog 
{
	private:
	
	protected:
		wxRadioBox* m_Function;
		wxRadioBox* m_Generation;
		wxRadioBox* m_Work;
		wxStdDialogButtonSizer* m_sdButtons;
		wxButton* m_sdButtonsOK;
		wxButton* m_sdButtonsCancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void onCancel( wxCommandEvent& event ){ event.Skip(); }
		virtual void onOK( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		fxdlggen( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("FX Setup"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
		~fxdlggen();
	
};

#endif //__fxdlggen__
