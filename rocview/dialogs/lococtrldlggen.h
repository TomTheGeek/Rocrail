///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __lococtrldlggen__
#define __lococtrldlggen__

#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/bmpbuttn.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/button.h>
#include <wx/textctrl.h>
#include <wx/panel.h>
#include "../public/slider.h"
#include <wx/sizer.h>
#include "../public/ledbutton.h"
#include <wx/combobox.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class LocoCtrlDlgGen
///////////////////////////////////////////////////////////////////////////////
class LocoCtrlDlgGen : public wxDialog 
{
	private:
	
	protected:
		wxBitmapButton* m_LocoImage;
		wxTextCtrl* m_LocoSpeed;
		Slider* m_LocoSlider;
		LEDButton* m_LB1;
		wxButton* m_F2;
		wxButton* m_F3;
		wxButton* m_F4;
		wxButton* m_F5;
		wxButton* m_F6;
		wxButton* m_F7;
		wxButton* m_F8;
		wxButton* m_F9;
		wxButton* m_F10;
		wxButton* m_F11;
		wxButton* m_F12;
		wxButton* m_FN;
		wxButton* m_F13;
		wxButton* m_F14;
		wxButton* m_FG;
		wxBitmapButton* m_Dir;
		wxComboBox* m_LocoList;
		wxButton* m_Cancel;
		wxButton* m_Stop;
		wxButton* m_Break;
		
		// Virtual event handlers, overide them in your derived class
		virtual void onLB1( wxMouseEvent& event ) { event.Skip(); }
		
	
	public:
		
		LocoCtrlDlgGen( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Loc Control"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX );
		~LocoCtrlDlgGen();
	
};

#endif //__lococtrldlggen__
