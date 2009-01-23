///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 21 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __waybilldlggen__
#define __waybilldlggen__

#include <wx/string.h>
#include <wx/listbox.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/notebook.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class waybillgen
///////////////////////////////////////////////////////////////////////////////
class waybillgen : public wxDialog 
{
	private:
	
	protected:
		wxNotebook* m_WaybillBook;
		wxPanel* m_IndexPanel;
		wxListBox* m_WaybillList;
		wxButton* m_NewWaybill;
		wxButton* m_DeleteWaybill;
		wxPanel* m_GeneralPanel;
		wxStaticText* m_labID;
		wxTextCtrl* m_ID;
		wxStaticText* m_labShipper;
		wxTextCtrl* m_Shipper;
		wxStaticText* m_labShipperAddr;
		wxTextCtrl* m_ShipperAddr;
		wxStaticText* m_labConsignee;
		wxTextCtrl* m_Consignee;
		wxStaticText* m_labConsigneeAddr;
		wxTextCtrl* m_ConsigneeAddr;
		wxStaticText* m_labCargo;
		wxTextCtrl* m_Cargo;
		wxPanel* m_RoutingPanel;
		wxStdDialogButtonSizer* m_sdButton;
		wxButton* m_sdButtonOK;
		wxButton* m_sdButtonApply;
		wxButton* m_sdButtonCancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void onWaybillSelect( wxCommandEvent& event ){ event.Skip(); }
		virtual void onNewWaybill( wxCommandEvent& event ){ event.Skip(); }
		virtual void onDeleteWaybill( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		waybillgen( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Waybill Table"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
		~waybillgen();
	
};

#endif //__waybilldlggen__
