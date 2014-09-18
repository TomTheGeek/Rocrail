///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __variabledlggen__
#define __variabledlggen__

#include <wx/listctrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/spinctrl.h>
#include <wx/notebook.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class VariableDlgGen
///////////////////////////////////////////////////////////////////////////////
class VariableDlgGen : public wxDialog 
{
	private:
	
	protected:
		wxNotebook* m_VarBook;
		wxPanel* m_IndexPanel;
		wxListCtrl* m_VarList;
		wxButton* m_New;
		wxButton* m_Delete;
		wxButton* m_Doc;
		wxPanel* m_GeneralPanel;
		wxStaticText* m_labID;
		wxTextCtrl* m_ID;
		wxStaticText* m_labGroup;
		wxTextCtrl* m_Group;
		wxStaticText* m_labDesc;
		wxTextCtrl* m_Desc;
		wxStaticText* m_labMin;
		wxSpinCtrl* m_MinValue;
		wxStaticText* m_labMax;
		wxSpinCtrl* m_MaxValue;
		wxStaticText* m_labText;
		wxTextCtrl* m_Text;
		wxStaticText* m_labValue;
		wxSpinCtrl* m_Value;
		wxButton* m_Actions;
		wxStdDialogButtonSizer* m_StandardButtons;
		wxButton* m_StandardButtonsOK;
		wxButton* m_StandardButtonsApply;
		wxButton* m_StandardButtonsCancel;
		wxButton* m_StandardButtonsHelp;
		
		// Virtual event handlers, overide them in your derived class
		virtual void onClose( wxCloseEvent& event ) { event.Skip(); }
		virtual void onListCol( wxListEvent& event ) { event.Skip(); }
		virtual void onListSelected( wxListEvent& event ) { event.Skip(); }
		virtual void onNew( wxCommandEvent& event ) { event.Skip(); }
		virtual void onDelete( wxCommandEvent& event ) { event.Skip(); }
		virtual void onDoc( wxCommandEvent& event ) { event.Skip(); }
		virtual void onActions( wxCommandEvent& event ) { event.Skip(); }
		virtual void onApply( wxCommandEvent& event ) { event.Skip(); }
		virtual void onCancel( wxCommandEvent& event ) { event.Skip(); }
		virtual void onHelp( wxCommandEvent& event ) { event.Skip(); }
		virtual void onOK( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		VariableDlgGen( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Variable"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxCLOSE_BOX|wxDEFAULT_DIALOG_STYLE );
		~VariableDlgGen();
	
};

#endif //__variabledlggen__
