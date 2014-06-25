///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep 12 2010)
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
		wxButton* m_Reset;
		wxStdDialogButtonSizer* m_StdButtons;
		wxButton* m_StdButtonsOK;
		wxButton* m_StdButtonsHelp;
		
		// Virtual event handlers, overide them in your derived class
		virtual void onClose( wxCloseEvent& event ) { event.Skip(); }
		virtual void onDrag( wxListEvent& event ) { event.Skip(); }
		virtual void onColClick( wxListEvent& event ) { event.Skip(); }
		virtual void onListSelected( wxListEvent& event ) { event.Skip(); }
		virtual void onRefresh( wxCommandEvent& event ) { event.Skip(); }
		virtual void onReset( wxCommandEvent& event ) { event.Skip(); }
		virtual void onHelp( wxCommandEvent& event ) { event.Skip(); }
		virtual void onOK( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		SensorEventsGen( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Sensor Events"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~SensorEventsGen();
	
};

#endif //__sensoreventsgen__
