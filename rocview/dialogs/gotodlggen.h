///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __gotodlggen__
#define __gotodlggen__

#include <wx/listctrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/checkbox.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class gotodlggen
///////////////////////////////////////////////////////////////////////////////
class gotodlggen : public wxDialog 
{
	private:
	
	protected:
		wxListCtrl* m_List;
		wxCheckBox* m_Blocks;
		wxCheckBox* m_Stages;
		wxCheckBox* m_FiddleYards;
		wxCheckBox* m_Turntables;
		wxStdDialogButtonSizer* m_stdButtons;
		wxButton* m_stdButtonsOK;
		wxButton* m_stdButtonsCancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void onColClick( wxListEvent& event ) { event.Skip(); }
		virtual void onSelected( wxListEvent& event ) { event.Skip(); }
		virtual void onBlocks( wxCommandEvent& event ) { event.Skip(); }
		virtual void onStages( wxCommandEvent& event ) { event.Skip(); }
		virtual void onFYs( wxCommandEvent& event ) { event.Skip(); }
		virtual void onTTs( wxCommandEvent& event ) { event.Skip(); }
		virtual void onCancel( wxCommandEvent& event ) { event.Skip(); }
		virtual void onOK( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		gotodlggen( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Go to"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
		~gotodlggen();
	
};

#endif //__gotodlggen__
