///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __donkeygen__
#define __donkeygen__

#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/statbmp.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/textctrl.h>
#include <wx/stattext.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class DonKeyGen
///////////////////////////////////////////////////////////////////////////////
class DonKeyGen : public wxDialog 
{
	private:
	
	protected:
		wxStaticBitmap* m_Splash;
		wxTextCtrl* m_DonateText;
		wxStaticText* m_labEmail;
		wxTextCtrl* m_Email;
		wxStaticText* m_labExpDate;
		wxTextCtrl* m_ExpDate;
		wxButton* m_LoadKey;
		wxButton* m_Donate;
		wxStdDialogButtonSizer* m_stdButton;
		wxButton* m_stdButtonOK;
		wxButton* m_stdButtonHelp;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnSplash( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnLoadKey( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnDonateNow( wxCommandEvent& event ) { event.Skip(); }
		virtual void onHelp( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnOK( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		DonKeyGen( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
		~DonKeyGen();
	
};

#endif //__donkeygen__
