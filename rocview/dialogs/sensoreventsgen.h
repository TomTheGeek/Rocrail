///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __sensoreventsgen__
#define __sensoreventsgen__

#include <wx/listctrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class SensorEventsGen
///////////////////////////////////////////////////////////////////////////////
class SensorEventsGen : public wxDialog 
{
	private:
	
	protected:
		wxListCtrl* m_EventList;
		wxButton* m_Refresh;
		wxStdDialogButtonSizer* m_StdButtons;
		wxButton* m_StdButtonsOK;
		
		// Virtual event handlers, overide them in your derived class
		virtual void onRefresh( wxCommandEvent& event ) { event.Skip(); }
		virtual void onOK( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		SensorEventsGen( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Sensor Events"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~SensorEventsGen();
	
};

#endif //__sensoreventsgen__
