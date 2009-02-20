///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 21 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __operatordlggen__
#define __operatordlggen__

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
#include <wx/statline.h>
#include <wx/bmpbuttn.h>
#include <wx/combobox.h>
#include <wx/notebook.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class operatordlggen
///////////////////////////////////////////////////////////////////////////////
class operatordlggen : public wxDialog 
{
	private:
	
	protected:
		wxNotebook* m_OperatorBook;
		wxPanel* m_IndexPanel;
		wxListBox* m_OperatorList;
		wxButton* m_NewOperator;
		wxButton* m_DelOperator;
		wxPanel* m_ControlPanel;
		wxStaticText* m_labOperator;
		wxTextCtrl* m_Operator;
		wxStaticLine* m_staticline1;
		wxBitmapButton* m_LocoImage;
		wxStaticText* m_labLocoID;
		wxComboBox* m_LocoID;
		wxStaticText* m_labLocation;
		wxComboBox* m_Location;
		wxButton* m_Reserve;
		wxStaticText* m_labGoto;
		wxComboBox* m_Goto;
		wxButton* m_Run;
		wxPanel* m_ConsistPanel;
		wxBitmapButton* m_CarImage;
		wxListBox* m_CarList;
		wxButton* m_AddCar;
		wxButton* m_LeaveCar;
		wxButton* m_ShowCar;
		wxButton* m_ShowWaybill;
		wxStdDialogButtonSizer* m_sdbSizer1;
		wxButton* m_sdbSizer1OK;
		wxButton* m_sdbSizer1Apply;
		wxButton* m_sdbSizer1Cancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void onOperatorList( wxCommandEvent& event ){ event.Skip(); }
		virtual void onNewOperator( wxCommandEvent& event ){ event.Skip(); }
		virtual void onDelOperator( wxCommandEvent& event ){ event.Skip(); }
		virtual void onLocoImage( wxCommandEvent& event ){ event.Skip(); }
		virtual void onLocomotiveCombo( wxCommandEvent& event ){ event.Skip(); }
		virtual void onReserve( wxCommandEvent& event ){ event.Skip(); }
		virtual void onRun( wxCommandEvent& event ){ event.Skip(); }
		virtual void onCarImage( wxCommandEvent& event ){ event.Skip(); }
		virtual void onAddCar( wxCommandEvent& event ){ event.Skip(); }
		virtual void onLeaveCar( wxCommandEvent& event ){ event.Skip(); }
		virtual void onCarCard( wxCommandEvent& event ){ event.Skip(); }
		virtual void onWayBill( wxCommandEvent& event ){ event.Skip(); }
		virtual void onApply( wxCommandEvent& event ){ event.Skip(); }
		virtual void onCancel( wxCommandEvent& event ){ event.Skip(); }
		virtual void onOK( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		operatordlggen( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Operator"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
		~operatordlggen();
	
};

#endif //__operatordlggen__
