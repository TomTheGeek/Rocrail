///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 17 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __mgvdlggen__
#define __mgvdlggen__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/spinctrl.h>
#include <wx/button.h>
#include <wx/panel.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/radiobox.h>
#include <wx/slider.h>
#include <wx/checkbox.h>
#include <wx/notebook.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class mgvdlggen
///////////////////////////////////////////////////////////////////////////////
class mgvdlggen : public wxDialog 
{
	private:
	
	protected:
		wxNotebook* m_MGVBook;
		wxPanel* m_Setup;
		wxStaticText* m_labIID;
		wxTextCtrl* m_IID;
		wxStaticLine* m_staticline2;
		wxStaticText* m_staticText14;
		wxStaticText* m_labAddress;
		wxStaticText* m_labPort;
		wxStaticText* m_labOne;
		wxSpinCtrl* m_Addr1;
		wxSpinCtrl* m_Port1;
		wxStaticText* m_labTwo;
		wxSpinCtrl* m_Addr2;
		wxSpinCtrl* m_Port2;
		wxStaticText* m_labThree;
		wxSpinCtrl* m_Addr3;
		wxSpinCtrl* m_Port3;
		wxStaticText* m_labFour;
		wxSpinCtrl* m_Addr4;
		wxSpinCtrl* m_Port4;
		wxStaticLine* m_staticline3;
		wxStaticText* m_labProgramming;
		wxButton* m_ResetBits;
		wxButton* m_ProgrammingOn;
		wxButton* m_ProgrammingOff;
		wxPanel* m_Servo;
		wxRadioBox* m_ServoSelection;
		wxStaticText* m_labServoLeft;
		wxStaticText* m_labServoRight;
		wxStaticText* m_labServoSpeed;
		wxSlider* m_ServoLeftAng;
		wxSlider* m_ServoRightAng;
		wxSlider* m_ServoSpeed;
		wxButton* m_SetLeftAngle;
		wxButton* m_SetRightAngle;
		wxButton* m_SetSpeed;
		wxStaticLine* m_staticline4;
		wxCheckBox* m_ServoRelay;
		wxStdDialogButtonSizer* m_StdButton;
		wxButton* m_StdButtonOK;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnResetBits( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnProgrammingOn( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnProgrammingOff( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnServoSelect( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnServoLeftAngle( wxScrollEvent& event ){ event.Skip(); }
		virtual void OnServoRightAngle( wxScrollEvent& event ){ event.Skip(); }
		virtual void OnServoSpeed( wxScrollEvent& event ){ event.Skip(); }
		virtual void onSetLeftAngle( wxCommandEvent& event ){ event.Skip(); }
		virtual void onSetRightAngle( wxCommandEvent& event ){ event.Skip(); }
		virtual void onSetSpeed( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnServoRelay( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnOK( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		mgvdlggen( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("MGV136 Servo"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
		~mgvdlggen();
	
};

#endif //__mgvdlggen__
