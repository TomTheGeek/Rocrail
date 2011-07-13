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
		wxSpinCtrl* m_NodeTypeNr;
		wxStaticText* m_labNumber;
		wxSpinCtrl* m_NodeNumber;
		wxButton* m_SetNodeNumber;
		wxPanel* m_IndexPanel;
		wxListBox* m_IndexList;
		wxButton* m_IndexDelete;
		wxPanel* m_VarPanel;
		wxListBox* m_VarList;
		wxStaticText* m_labVarIndex;
		wxStaticText* m_labVarValue;
		wxSpinCtrl* m_VarIndex;
		wxSpinCtrl* m_VarValue;
		wxCheckBox* m_VarBit7;
		wxCheckBox* m_VarBit6;
		wxCheckBox* m_VarBit5;
		wxCheckBox* m_VarBit4;
		wxCheckBox* m_VarBit3;
		wxCheckBox* m_VarBit2;
		wxCheckBox* m_VarBit1;
		wxCheckBox* m_VarBit0;
		wxButton* m_VarGet;
		wxButton* m_VarSet;
		wxPanel* m_EventsPanel;
		wxListBox* m_EventList;
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
		wxButton* m_EvtClearAll;
		wxButton* m_EvtLearn;
		wxButton* m_EvtUnlearn;
		wxStdDialogButtonSizer* m_sdbSizer1;
		wxButton* m_sdbSizer1OK;
		wxButton* m_sdbSizer1Cancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void onSetNodeNumber( wxCommandEvent& event ) { event.Skip(); }
		virtual void onIndexSelect( wxCommandEvent& event ) { event.Skip(); }
		virtual void onIndexDelete( wxCommandEvent& event ) { event.Skip(); }
		virtual void onVarSelect( wxCommandEvent& event ) { event.Skip(); }
		virtual void onVarValue( wxSpinEvent& event ) { event.Skip(); }
		virtual void onVarBit( wxCommandEvent& event ) { event.Skip(); }
		virtual void onVarGet( wxCommandEvent& event ) { event.Skip(); }
		virtual void onVarSet( wxCommandEvent& event ) { event.Skip(); }
		virtual void onEventSelect( wxCommandEvent& event ) { event.Skip(); }
		virtual void onEV( wxSpinEvent& event ) { event.Skip(); }
		virtual void onEVBit( wxCommandEvent& event ) { event.Skip(); }
		virtual void onEventGetAll( wxCommandEvent& event ) { event.Skip(); }
		virtual void onEventAdd( wxCommandEvent& event ) { event.Skip(); }
		virtual void onEventDelete( wxCommandEvent& event ) { event.Skip(); }
		virtual void onEvtClearAll( wxCommandEvent& event ) { event.Skip(); }
		virtual void onLearn( wxCommandEvent& event ) { event.Skip(); }
		virtual void onUnlearn( wxCommandEvent& event ) { event.Skip(); }
		virtual void onCancel( wxCommandEvent& event ) { event.Skip(); }
		virtual void onOK( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		cbusnodedlggen( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("CBUS Node"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
		~cbusnodedlggen();
	
};

#endif //__cbusnodedlggen__
