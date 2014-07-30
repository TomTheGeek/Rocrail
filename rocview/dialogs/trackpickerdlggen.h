///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __trackpickerdlggen__
#define __trackpickerdlggen__

#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/font.h>
#include <wx/grid.h>
#include <wx/gdicmn.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/listbook.h>
#include <wx/listctrl.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class TrackPickerDlgGen
///////////////////////////////////////////////////////////////////////////////
class TrackPickerDlgGen : public wxDialog 
{
	private:
	
	protected:
		wxListbook* m_TrackBook;
		wxPanel* m_PageTrack;
		wxGrid* m_GridTrack;
		wxPanel* m_PageSwitch;
		wxGrid* m_GridSwitch;
		wxPanel* m_PageSignal;
		wxGrid* m_GridSignal;
		wxPanel* m_PageBlock;
		wxGrid* m_GridBlock;
		
		// Virtual event handlers, overide them in your derived class
		virtual void onTrackCellLeftClick( wxGridEvent& event ) { event.Skip(); }
		
	
	public:
		
		TrackPickerDlgGen( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Track Picker"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~TrackPickerDlgGen();
	
};

#endif //__trackpickerdlggen__
