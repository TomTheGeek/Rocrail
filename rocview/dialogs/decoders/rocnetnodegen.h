///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __rocnetnodegen__
#define __rocnetnodegen__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/spinctrl.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/panel.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/radiobox.h>
#include <wx/notebook.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class rocnetnodegen
///////////////////////////////////////////////////////////////////////////////
class rocnetnodegen : public wxDialog 
{
	private:
	
	protected:
		wxNotebook* m_notebook2;
		wxPanel* m_RocNetPanel;
		wxStaticText* m_labID;
		wxSpinCtrl* m_ID;
		wxStaticText* m_labVendor;
		wxTextCtrl* m_Vendor;
		wxStaticText* m_labProduct;
		wxTextCtrl* m_Product;
		wxButton* m_RocnetWrite;
		wxPanel* m_PortSetupPanel;
		wxStaticText* m_labPort;
		wxStaticText* m_labIO;
		wxStaticText* m_labType;
		wxStaticText* m_labDelay;
		wxStaticText* m_labPort1;
		wxSpinCtrl* m_IO1;
		wxRadioBox* m_Type1;
		wxSpinCtrl* m_Delay1;
		wxStaticText* m_labPort2;
		wxSpinCtrl* m_IO2;
		wxRadioBox* m_Type2;
		wxSpinCtrl* m_Delay2;
		wxStaticText* m_labPort3;
		wxSpinCtrl* m_IO3;
		wxRadioBox* m_Type3;
		wxSpinCtrl* m_Delay3;
		wxStaticText* m_labPort4;
		wxSpinCtrl* m_IO4;
		wxRadioBox* m_Type4;
		wxSpinCtrl* m_Delay4;
		wxStaticText* m_labPort5;
		wxSpinCtrl* m_IO5;
		wxRadioBox* m_Type5;
		wxSpinCtrl* m_Delay5;
		wxStaticText* m_labPort6;
		wxSpinCtrl* m_IO6;
		wxRadioBox* m_Type6;
		wxSpinCtrl* m_Delay6;
		wxStaticText* m_labPort7;
		wxSpinCtrl* m_IO7;
		wxRadioBox* m_Type7;
		wxSpinCtrl* m_Delay7;
		wxStaticText* m_labPort8;
		wxSpinCtrl* m_IO8;
		wxRadioBox* m_Type8;
		wxSpinCtrl* m_Delay8;
		wxButton* m_PortPrev;
		wxButton* m_PortNext;
		wxButton* m_PortRead;
		wxButton* m_PortWrite;
		wxStdDialogButtonSizer* m_StdButton;
		wxButton* m_StdButtonOK;
		
		// Virtual event handlers, overide them in your derived class
		virtual void onClose( wxCloseEvent& event ) { event.Skip(); }
		virtual void onRocnetWrite( wxCommandEvent& event ) { event.Skip(); }
		virtual void onPortPrev( wxCommandEvent& event ) { event.Skip(); }
		virtual void onPortNext( wxCommandEvent& event ) { event.Skip(); }
		virtual void onPortRead( wxCommandEvent& event ) { event.Skip(); }
		virtual void onPortWrite( wxCommandEvent& event ) { event.Skip(); }
		virtual void onOK( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		rocnetnodegen( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("RocNetNode"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~rocnetnodegen();
	
};

#endif //__rocnetnodegen__
