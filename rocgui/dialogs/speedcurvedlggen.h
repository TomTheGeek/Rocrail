///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 21 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __speedcurvedlggen__
#define __speedcurvedlggen__

#include <wx/string.h>
#include <wx/textctrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/slider.h>
#include <wx/stattext.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class speedcurvedlggen
///////////////////////////////////////////////////////////////////////////////
class speedcurvedlggen : public wxDialog 
{
	private:
	
	protected:
		wxTextCtrl* m_Step15;
		wxSlider* m_SliderStep15;
		wxStaticText* m_staticText1;
		wxTextCtrl* m_Step16;
		wxSlider* m_SliderStep16;
		wxStaticText* m_staticText11;
		wxTextCtrl* m_Step17;
		wxSlider* m_SliderStep17;
		wxStaticText* m_staticText12;
		wxTextCtrl* m_Step18;
		wxSlider* m_SliderStep18;
		wxStaticText* m_staticText13;
		wxTextCtrl* m_Step19;
		wxSlider* m_SliderStep19;
		wxStaticText* m_staticText14;
		wxTextCtrl* m_Step20;
		wxSlider* m_SliderStep20;
		wxStaticText* m_staticText15;
		wxTextCtrl* m_Step21;
		wxSlider* m_SliderStep21;
		wxStaticText* m_staticText16;
		wxTextCtrl* m_Step22;
		wxSlider* m_SliderStep22;
		wxStaticText* m_staticText17;
		wxTextCtrl* m_Step23;
		wxSlider* m_SliderStep23;
		wxStaticText* m_staticText18;
		wxTextCtrl* m_Step24;
		wxSlider* m_SliderStep24;
		wxStaticText* m_staticText19;
		wxTextCtrl* m_Step25;
		wxSlider* m_SliderStep25;
		wxStaticText* m_staticText110;
		wxTextCtrl* m_Step26;
		wxSlider* m_SliderStep26;
		wxStaticText* m_staticText111;
		wxTextCtrl* m_Step27;
		wxSlider* m_SliderStep27;
		wxStaticText* m_staticText112;
		wxTextCtrl* m_Step28;
		wxSlider* m_SliderStep28;
		wxStaticText* m_staticText113;
		wxTextCtrl* m_Step1;
		wxSlider* m_SliderStep1;
		wxStaticText* m_staticText114;
		wxTextCtrl* m_Step2;
		wxSlider* m_SliderStep2;
		wxStaticText* m_staticText115;
		wxTextCtrl* m_Step3;
		wxSlider* m_SliderStep3;
		wxStaticText* m_staticText121;
		wxTextCtrl* m_Step4;
		wxSlider* m_SliderStep4;
		wxStaticText* m_staticText131;
		wxTextCtrl* m_Step5;
		wxSlider* m_SliderStep5;
		wxStaticText* m_staticText141;
		wxTextCtrl* m_Step6;
		wxSlider* m_SliderStep6;
		wxStaticText* m_staticText151;
		wxTextCtrl* m_Step7;
		wxSlider* m_SliderStep7;
		wxStaticText* m_staticText161;
		wxTextCtrl* m_Step8;
		wxSlider* m_SliderStep8;
		wxStaticText* m_staticText171;
		wxTextCtrl* m_Step9;
		wxSlider* m_SliderStep9;
		wxStaticText* m_staticText181;
		wxTextCtrl* m_Step10;
		wxSlider* m_SliderStep10;
		wxStaticText* m_staticText191;
		wxTextCtrl* m_Step11;
		wxSlider* m_SliderStep11;
		wxStaticText* m_staticText1101;
		wxTextCtrl* m_Step12;
		wxSlider* m_SliderStep12;
		wxStaticText* m_staticText1111;
		wxTextCtrl* m_Step13;
		wxSlider* m_SliderStep13;
		wxStaticText* m_staticText1121;
		wxTextCtrl* m_Step14;
		wxSlider* m_SliderStep14;
		wxStaticText* m_staticText1131;
		wxStaticLine* m_staticline2;
		wxButton* m_Linearize;
		wxButton* m_Logarithmize;
		wxStaticLine* m_staticline1;
		wxStdDialogButtonSizer* m_stdButton;
		wxButton* m_stdButtonOK;
		wxButton* m_stdButtonCancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void onStep( wxCommandEvent& event ){ event.Skip(); }
		virtual void onSlider( wxScrollEvent& event ){ event.Skip(); }
		virtual void onLinearize( wxCommandEvent& event ){ event.Skip(); }
		virtual void onLogarithmize( wxCommandEvent& event ){ event.Skip(); }
		virtual void onCancel( wxCommandEvent& event ){ event.Skip(); }
		virtual void onOK( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		speedcurvedlggen( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Speed Curve"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
		~speedcurvedlggen();
	
};

#endif //__speedcurvedlggen__
