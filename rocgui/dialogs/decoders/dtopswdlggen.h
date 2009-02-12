///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 21 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __dtopswdlggen__
#define __dtopswdlggen__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/spinctrl.h>
#include <wx/combobox.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/checklst.h>
#include <wx/button.h>
#include <wx/statline.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class dtopswdlg
///////////////////////////////////////////////////////////////////////////////
class dtopswdlg : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_labBoardNumber;
		wxSpinCtrl* m_BoardNumber;
		wxStaticText* m_labBoardType;
		wxComboBox* m_BoardType;
		wxStaticText* m_labOpSw;
		wxCheckListBox* m_OpSwList;
		wxButton* m_ReadAll;
		wxButton* m_WriteAll;
		wxStaticLine* m_staticline1;
		wxStdDialogButtonSizer* m_sdbSizer1;
		wxButton* m_sdbSizer1OK;
		
		// Virtual event handlers, overide them in your derived class
		virtual void onBoardType( wxCommandEvent& event ){ event.Skip(); }
		virtual void onReadAll( wxCommandEvent& event ){ event.Skip(); }
		virtual void onWriteAll( wxCommandEvent& event ){ event.Skip(); }
		virtual void onOK( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		dtopswdlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("DT Option Switches"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
		~dtopswdlg();
	
};

#endif //__dtopswdlggen__
