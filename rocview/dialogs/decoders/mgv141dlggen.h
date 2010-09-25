///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 17 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __mgv141dlggen__
#define __mgv141dlggen__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/spinctrl.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/statline.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class mgv141gen
///////////////////////////////////////////////////////////////////////////////
class mgv141gen : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText1;
		wxSpinCtrl* m_UnitHigh;
		wxSpinCtrl* m_UnitLow;
		wxButton* m_UnitSet;
		wxStaticText* m_lab1;
		wxSpinCtrl* m_CounterAddress1;
		wxStaticText* m_lab2;
		wxSpinCtrl* m_CounterAddress2;
		wxStaticText* m_lab3;
		wxSpinCtrl* m_CounterAddress3;
		wxStaticText* m_lab4;
		wxSpinCtrl* m_CounterAddress4;
		wxStaticText* m_lab5;
		wxSpinCtrl* m_CounterAddress5;
		wxStaticText* m_lab6;
		wxSpinCtrl* m_CounterAddress6;
		wxStaticText* m_lab7;
		wxSpinCtrl* m_CounterAddress7;
		wxStaticText* m_lab8;
		wxSpinCtrl* m_CounterAddress8;
		wxButton* m_WriteAll;
		wxButton* m_ReadAll;
		wxStaticLine* m_staticline1;
		wxStdDialogButtonSizer* m_StdButtons;
		wxButton* m_StdButtonsOK;
		wxButton* m_StdButtonsCancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void onUnitSet( wxCommandEvent& event ){ event.Skip(); }
		virtual void onWriteAll( wxCommandEvent& event ){ event.Skip(); }
		virtual void onReadAll( wxCommandEvent& event ){ event.Skip(); }
		virtual void onCancel( wxCommandEvent& event ){ event.Skip(); }
		virtual void onOK( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		mgv141gen( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Wheel Counter"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
		~mgv141gen();
	
};

#endif //__mgv141dlggen__
