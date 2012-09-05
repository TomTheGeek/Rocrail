///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __toursdlggen__
#define __toursdlggen__

#include <wx/string.h>
#include <wx/listbox.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/checkbox.h>
#include <wx/button.h>
#include <wx/panel.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/combobox.h>
#include <wx/notebook.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class toursdlggen
///////////////////////////////////////////////////////////////////////////////
class toursdlggen : public wxDialog 
{
	private:
	
	protected:
		wxNotebook* m_TourBook;
		wxPanel* m_IndexPanel;
		wxListBox* m_TourList;
		wxStaticText* m_labTourID;
		wxTextCtrl* m_ID;
		wxCheckBox* m_ShowAll;
		wxStaticText* m_labStartBlock;
		wxTextCtrl* m_StartBlockID;
		wxCheckBox* m_Recycle;
		wxButton* m_AddTour;
		wxButton* m_ModifyTour;
		wxButton* m_DeleteTour;
		wxPanel* m_SchedulesPanel;
		wxListBox* m_EntryList;
		wxComboBox* m_ScheduleList;
		wxButton* m_EntryAdd;
		wxButton* m_EntryDelete;
		wxStdDialogButtonSizer* m_DefaultButtons;
		wxButton* m_DefaultButtonsOK;
		wxButton* m_DefaultButtonsApply;
		wxButton* m_DefaultButtonsCancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void onTourSelect( wxCommandEvent& event ) { event.Skip(); }
		virtual void onShowAll( wxCommandEvent& event ) { event.Skip(); }
		virtual void onAddTour( wxCommandEvent& event ) { event.Skip(); }
		virtual void onModifyTour( wxCommandEvent& event ) { event.Skip(); }
		virtual void onDeleteTour( wxCommandEvent& event ) { event.Skip(); }
		virtual void onEntrySelect( wxCommandEvent& event ) { event.Skip(); }
		virtual void onScheduleSelect( wxCommandEvent& event ) { event.Skip(); }
		virtual void onEntryAdd( wxCommandEvent& event ) { event.Skip(); }
		virtual void onEntryDelete( wxCommandEvent& event ) { event.Skip(); }
		virtual void onApply( wxCommandEvent& event ) { event.Skip(); }
		virtual void onCancel( wxCommandEvent& event ) { event.Skip(); }
		virtual void onOK( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		toursdlggen( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Tours"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
		~toursdlggen();
	
};

#endif //__toursdlggen__
