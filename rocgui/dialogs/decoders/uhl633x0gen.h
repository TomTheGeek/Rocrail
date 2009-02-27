///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 17 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __uhl633x0gen__
#define __uhl633x0gen__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/checkbox.h>
#include <wx/statbox.h>
#include <wx/radiobox.h>
#include <wx/spinctrl.h>
#include <wx/panel.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class uhl633x0dlggen
///////////////////////////////////////////////////////////////////////////////
class uhl633x0dlggen : public wxDialog 
{
	private:
	
	protected:
		wxPanel* m_MainPanel;
		wxStaticText* m_labModAddr;
		wxTextCtrl* m_ModAddr;
		wxStaticText* m_labRepAddr;
		wxTextCtrl* m_RepAddr;
		wxCheckBox* m_AutoAddressing;
		wxCheckBox* m_IndividualDelays;
		wxCheckBox* m_ReportAtPowerOn;
		wxRadioBox* m_ModuleType;
		wxRadioBox* m_Setting;
		wxSpinCtrl* m_Input1;
		wxSpinCtrl* m_Input2;
		wxSpinCtrl* m_Input3;
		wxSpinCtrl* m_Input4;
		wxSpinCtrl* m_Input5;
		wxSpinCtrl* m_Input6;
		wxSpinCtrl* m_Input7;
		wxSpinCtrl* m_Input8;
		wxSpinCtrl* m_Input9;
		wxSpinCtrl* m_Input10;
		wxSpinCtrl* m_Input11;
		wxSpinCtrl* m_Input12;
		wxSpinCtrl* m_Input13;
		wxSpinCtrl* m_Input14;
		wxSpinCtrl* m_Input15;
		wxSpinCtrl* m_Input16;
		wxStdDialogButtonSizer* m_stdButton;
		wxButton* m_stdButtonOK;
		wxButton* m_stdButtonCancel;
	
	public:
		uhl633x0dlggen( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Uhlenbrock 63350"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX );
		~uhl633x0dlggen();
	
};

#endif //__uhl633x0gen__
