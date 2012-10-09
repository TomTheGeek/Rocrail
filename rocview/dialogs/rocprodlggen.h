///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __rocprodlggen__
#define __rocprodlggen__

#include <wx/treectrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/stattext.h>
#include <wx/button.h>
#include <wx/spinctrl.h>
#include <wx/checkbox.h>
#include <wx/statbox.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class RocProDlgGen
///////////////////////////////////////////////////////////////////////////////
class RocProDlgGen : public wxDialog 
{
	private:
	
	protected:
		wxTreeCtrl* m_DecTree;
		wxTextCtrl* m_Info;
		wxStaticLine* m_staticline2;
		wxStaticText* m_labFile;
		wxTextCtrl* m_DecFile;
		wxButton* m_DecFileOpen;
		wxTextCtrl* m_Nr;
		wxSpinCtrl* m_Value;
		wxCheckBox* m_Bit7;
		wxCheckBox* m_Bit6;
		wxCheckBox* m_Bit5;
		wxCheckBox* m_Bit4;
		wxCheckBox* m_Bit3;
		wxCheckBox* m_Bit2;
		wxCheckBox* m_Bit1;
		wxCheckBox* m_Bit0;
		wxStdDialogButtonSizer* m_sdbSizer2;
		wxButton* m_sdbSizer2OK;
		
		// Virtual event handlers, overide them in your derived class
		virtual void onClose( wxCloseEvent& event ) { event.Skip(); }
		virtual void onTreeSelChanged( wxTreeEvent& event ) { event.Skip(); }
		virtual void onOpen( wxCommandEvent& event ) { event.Skip(); }
		virtual void onOK( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		RocProDlgGen( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("RocPro"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~RocProDlgGen();
	
};

#endif //__rocprodlggen__
