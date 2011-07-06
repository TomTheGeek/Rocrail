///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Dec 21 2009)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __cbusnodedlggen__
#define __cbusnodedlggen__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/spinctrl.h>
#include <wx/button.h>
#include <wx/panel.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/notebook.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class cbusnodedlggen
///////////////////////////////////////////////////////////////////////////////
class cbusnodedlggen : public wxDialog 
{
	private:
	
	protected:
		wxNotebook* m_NoteBook;
		wxPanel* m_NodeNumberPanel;
		wxStaticText* m_labNodeType;
		wxTextCtrl* m_NodeType;
		wxStaticText* m_labNumber;
		wxSpinCtrl* m_NodeNumber;
		wxButton* m_SetNodeNumber;
		wxStdDialogButtonSizer* m_sdbSizer1;
		wxButton* m_sdbSizer1OK;
		
		// Virtual event handlers, overide them in your derived class
		virtual void onOK( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		cbusnodedlggen( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("CBUS Node"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
		~cbusnodedlggen();
	
};

#endif //__cbusnodedlggen__
