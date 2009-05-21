///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 17 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __uhl68610gen__
#define __uhl68610gen__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/spinctrl.h>
#include <wx/sizer.h>
#include <wx/radiobox.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class uhl68610dlggen
///////////////////////////////////////////////////////////////////////////////
class uhl68610dlggen : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_labAddr1;
		wxSpinCtrl* m_Addr1;
		wxStaticText* m_labAddr2;
		wxSpinCtrl* m_Addr2;
		wxRadioBox* m_Reporting;
		wxRadioBox* m_Format;
		wxRadioBox* m_Scale;
		wxStaticText* m_labGap;
		wxSpinCtrl* m_Gap;
		wxButton* m_Read;
		wxButton* m_Write;
		wxStdDialogButtonSizer* m_sdbSizer1;
		wxButton* m_sdbSizer1OK;
		wxButton* m_sdbSizer1Cancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void onReporting( wxCommandEvent& event ){ event.Skip(); }
		virtual void onRead( wxCommandEvent& event ){ event.Skip(); }
		virtual void onWrite( wxCommandEvent& event ){ event.Skip(); }
		virtual void onCancel( wxCommandEvent& event ){ event.Skip(); }
		virtual void onOK( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		uhl68610dlggen( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Uhlenbrock 68610"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
		~uhl68610dlggen();
	
};

#endif //__uhl68610gen__
