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
#include <wx/listbox.h>
#include <wx/checkbox.h>
#include <wx/notebook.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////

#define wxID_CBUSNODE_BOOK 1000
#define wxID_CBUSNODE_NUMBER 1001
#define wxID_CBUSNODE_INDEX 1002
#define wxID_CBUS_VAR 1003
#define wxID_CBUS_EVENTSPANEL 1004

///////////////////////////////////////////////////////////////////////////////
/// Class cbusnodedlggen
///////////////////////////////////////////////////////////////////////////////
class cbusnodedlggen : public wxDialog 
{
	private:
	
	protected:
		wxNotebook* m_NoteBook;
		wxPanel* m_NodeNumberPanel;
		wxStaticText* m_labIID;
		wxTextCtrl* m_IID;
		wxStaticText* m_labNodeType;
		wxTextCtrl* m_NodeType;
		wxStaticText* m_labNumber;
		wxSpinCtrl* m_NodeNumber;
		wxButton* m_SetNodeNumber;
		wxPanel* m_IndexPanel;
		wxListBox* m_listBox1;
		wxButton* m_IndexDelete;
		wxPanel* m_VarPanel;
		wxStaticText* m_labACE8CNV1;
		wxCheckBox* m_ACE8CInput0;
		wxCheckBox* m_ACE8CInput1;
		wxCheckBox* m_ACE8CInput2;
		wxCheckBox* m_ACE8CInput3;
		wxCheckBox* m_ACE8CInput4;
		wxCheckBox* m_ACE8CInput5;
		wxCheckBox* m_ACE8CInput6;
		wxCheckBox* m_ACE8CInput7;
		wxButton* m_ACE8CGet;
		wxButton* m_ACE8CSet;
		wxPanel* m_EventsPanel;
		wxListBox* m_EventsList;
		wxStaticText* m_labEventNode;
		wxStaticText* m_labEventAddr;
		wxStaticText* m_labEventIndex;
		wxStaticText* m_labEventVar;
		wxSpinCtrl* m_EventNodeNr;
		wxSpinCtrl* m_EventAddress;
		wxSpinCtrl* m_EventIndex;
		wxSpinCtrl* m_EventVar;
		wxCheckBox* m_EVBit7;
		wxCheckBox* m_EVBit6;
		wxCheckBox* m_EVBit5;
		wxCheckBox* m_EVBit4;
		wxCheckBox* m_EVBit3;
		wxCheckBox* m_EVBit2;
		wxCheckBox* m_EVBit1;
		wxCheckBox* m_EVBit0;
		wxButton* m_EventGetAll;
		wxButton* m_EventAdd;
		wxButton* m_EventDelete;
		wxStdDialogButtonSizer* m_sdbSizer1;
		wxButton* m_sdbSizer1OK;
		
		// Virtual event handlers, overide them in your derived class
		virtual void onSetNodeNumber( wxCommandEvent& event ) { event.Skip(); }
		virtual void onACE8CGet( wxCommandEvent& event ) { event.Skip(); }
		virtual void onACE8CSet( wxCommandEvent& event ) { event.Skip(); }
		virtual void onEventSelect( wxCommandEvent& event ) { event.Skip(); }
		virtual void onEV( wxSpinEvent& event ) { event.Skip(); }
		virtual void onEVBit( wxCommandEvent& event ) { event.Skip(); }
		virtual void onEventGetAll( wxCommandEvent& event ) { event.Skip(); }
		virtual void onEventAdd( wxCommandEvent& event ) { event.Skip(); }
		virtual void onEventDelete( wxCommandEvent& event ) { event.Skip(); }
		virtual void onOK( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		cbusnodedlggen( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("CBUS Node"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
		~cbusnodedlggen();
	
};

#endif //__cbusnodedlggen__
