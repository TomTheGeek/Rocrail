///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __cmdrecordergen__
#define __cmdrecordergen__

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
#include <wx/stattext.h>
#include <wx/sizer.h>
#include <wx/listbox.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class CmdRecorderGen
///////////////////////////////////////////////////////////////////////////////
class CmdRecorderGen : public wxDialog 
{
	private:
	
	protected:
		wxBitmapButton* m_Record;
		wxBitmapButton* m_Stop;
		wxBitmapButton* m_Pause;
		wxBitmapButton* m_Play;
		wxStaticText* m_labStatus;
		wxListBox* m_CmdList;
		wxTextCtrl* m_Cmd;
		wxButton* m_Modify;
		wxButton* m_Delete;
		wxButton* m_Insert;
		wxButton* m_Clear;
		wxButton* m_Export;
		wxButton* m_Import;
		wxCheckBox* m_Stamp;
		wxButton* m_CreateRoute;
		wxStdDialogButtonSizer* m_StdButtons;
		wxButton* m_StdButtonsOK;
		wxButton* m_StdButtonsHelp;
		
		// Virtual event handlers, overide them in your derived class
		virtual void onClose( wxCloseEvent& event ) { event.Skip(); }
		virtual void onRecord( wxCommandEvent& event ) { event.Skip(); }
		virtual void onStop( wxCommandEvent& event ) { event.Skip(); }
		virtual void onPause( wxCommandEvent& event ) { event.Skip(); }
		virtual void onPlay( wxCommandEvent& event ) { event.Skip(); }
		virtual void onCmdSelected( wxCommandEvent& event ) { event.Skip(); }
		virtual void onModify( wxCommandEvent& event ) { event.Skip(); }
		virtual void onDelete( wxCommandEvent& event ) { event.Skip(); }
		virtual void onInsert( wxCommandEvent& event ) { event.Skip(); }
		virtual void onClear( wxCommandEvent& event ) { event.Skip(); }
		virtual void onExport( wxCommandEvent& event ) { event.Skip(); }
		virtual void onImport( wxCommandEvent& event ) { event.Skip(); }
		virtual void onCreateRoute( wxCommandEvent& event ) { event.Skip(); }
		virtual void onHelp( wxCommandEvent& event ) { event.Skip(); }
		virtual void onOK( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		CmdRecorderGen( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Command Recorder"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~CmdRecorderGen();
	
};

#endif //__cmdrecordergen__
