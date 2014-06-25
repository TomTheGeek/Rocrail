///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep 12 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __channeltunegen__
#define __channeltunegen__

#include <wx/string.h>
#include <wx/spinctrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/radiobox.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/slider.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class ChannelTuneDlgGen
///////////////////////////////////////////////////////////////////////////////
class ChannelTuneDlgGen : public wxDialog 
{
	private:
	
	protected:
		wxStaticBoxSizer* m_RangeBox;
		wxSpinCtrl* m_MaxRange;
		wxSpinCtrl* m_MinRange;
		wxRadioBox* m_RangePreset;
		wxStaticText* m_labOffPos;
		wxSlider* m_OffPos;
		wxTextCtrl* m_OffPosValue;
		wxStaticText* m_labOnPos;
		wxSlider* m_OnPos;
		wxTextCtrl* m_OnPosValue;
		wxStdDialogButtonSizer* m_StdButton;
		wxButton* m_StdButtonOK;
		wxButton* m_StdButtonHelp;
		
		// Virtual event handlers, overide them in your derived class
		virtual void onMaxRange( wxSpinEvent& event ) { event.Skip(); }
		virtual void onMinRange( wxSpinEvent& event ) { event.Skip(); }
		virtual void onPreset( wxCommandEvent& event ) { event.Skip(); }
		virtual void onOffPos( wxScrollEvent& event ) { event.Skip(); }
		virtual void onOffPosRelease( wxScrollEvent& event ) { event.Skip(); }
		virtual void onOnPos( wxScrollEvent& event ) { event.Skip(); }
		virtual void onOnPosRelease( wxScrollEvent& event ) { event.Skip(); }
		virtual void onHelp( wxCommandEvent& event ) { event.Skip(); }
		virtual void onOK( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		ChannelTuneDlgGen( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Channel Finetuning"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
		~ChannelTuneDlgGen();
	
};

#endif //__channeltunegen__
