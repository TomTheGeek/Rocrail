///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __locowidgetgen__
#define __locowidgetgen__

#include <wx/string.h>
#include <wx/button.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/statbmp.h>
#include <wx/stattext.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class LocoWidgetGen
///////////////////////////////////////////////////////////////////////////////
class LocoWidgetGen : public wxPanel 
{
	private:
	
	protected:
		wxButton* m_Stop;
		wxStaticBitmap* m_LocoImage;
		wxStaticText* m_ID;
		wxStaticText* m_Block;
		wxStaticLine* m_staticline1;
		wxStaticText* m_Speed;
		wxStaticLine* m_staticline2;
		wxStaticText* m_Mode;
		wxStaticLine* m_staticline3;
		wxStaticText* m_Destination;
		
		// Virtual event handlers, overide them in your derived class
		virtual void onMouseWheel( wxMouseEvent& event ) { event.Skip(); }
		virtual void onStop( wxCommandEvent& event ) { event.Skip(); }
		virtual void onImageLeftDown( wxMouseEvent& event ) { event.Skip(); }
		virtual void onThrottle( wxMouseEvent& event ) { event.Skip(); }
		virtual void onPopUp( wxMouseEvent& event ) { event.Skip(); }
		virtual void onSpeedUp( wxMouseEvent& event ) { event.Skip(); }
		virtual void onSpeedDown( wxMouseEvent& event ) { event.Skip(); }
		
	
	public:
		
		LocoWidgetGen( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 260,-1 ), long style = 0 );
		~LocoWidgetGen();
	
};

#endif //__locowidgetgen__
