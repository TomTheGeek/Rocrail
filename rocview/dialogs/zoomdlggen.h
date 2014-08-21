///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __zoomdlggen__
#define __zoomdlggen__

#include <wx/string.h>
#include <wx/combobox.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/stattext.h>
#include <wx/sizer.h>
#include <wx/slider.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class ZoomDlgGen
///////////////////////////////////////////////////////////////////////////////
class ZoomDlgGen : public wxDialog 
{
	private:
	
	protected:
		wxComboBox* m_ZoomValue;
		wxStaticText* m_labPercent;
		wxSlider* m_ZoomSlider;
		wxStdDialogButtonSizer* m_sdbSizer1;
		wxButton* m_sdbSizer1OK;
		wxButton* m_sdbSizer1Cancel;
		wxButton* m_sdbSizer1Help;
		
		// Virtual event handlers, overide them in your derived class
		virtual void onZoomSelect( wxCommandEvent& event ) { event.Skip(); }
		virtual void onZoomEnter( wxCommandEvent& event ) { event.Skip(); }
		virtual void onZoomThumb( wxScrollEvent& event ) { event.Skip(); }
		virtual void onZoomRelease( wxScrollEvent& event ) { event.Skip(); }
		virtual void onCancel( wxCommandEvent& event ) { event.Skip(); }
		virtual void onHelp( wxCommandEvent& event ) { event.Skip(); }
		virtual void onOK( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		ZoomDlgGen( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
		~ZoomDlgGen();
	
};

#endif //__zoomdlggen__
