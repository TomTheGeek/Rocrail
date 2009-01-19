///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 21 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __cardlggen__
#define __cardlggen__

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
#include <wx/listbox.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/statline.h>
#include <wx/radiobox.h>
#include <wx/combobox.h>
#include <wx/notebook.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class CarDlg
///////////////////////////////////////////////////////////////////////////////
class CarDlg : public wxDialog 
{
	private:
	
	protected:
		wxNotebook* m_CarBook;
		wxPanel* m_IndexPanel;
		wxBitmapButton* m_CarImage;
		wxListBox* m_CarList;
		wxPanel* m_GeneralPanel;
		wxStaticText* m_labID;
		wxTextCtrl* m_ID;
		wxStaticText* m_labCode;
		wxTextCtrl* m_Code;
		wxStaticText* m_labRoadname;
		wxTextCtrl* m_Roadname;
		wxStaticText* m_labColor;
		wxTextCtrl* m_Color;
		wxStaticLine* m_staticline1;
		wxRadioBox* m_Era;
		wxPanel* m_DetailsPanel;
		wxRadioBox* m_Type;
		wxRadioBox* m_SubType;
		wxStaticText* m_labLength;
		wxTextCtrl* m_Length;
		wxPanel* m_WaybillPanel;
		wxListBox* m_WaybillList;
		wxComboBox* m_WaybillSelection;
		wxButton* m_AddWaybill;
		wxButton* m_DeleteWaybill;
		wxStdDialogButtonSizer* m_stdButton;
		wxButton* m_stdButtonOK;
		wxButton* m_stdButtonApply;
		wxButton* m_stdButtonCancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void onCarImage( wxCommandEvent& event ){ event.Skip(); }
		virtual void onCarList( wxCommandEvent& event ){ event.Skip(); }
		virtual void onTypeSelect( wxCommandEvent& event ){ event.Skip(); }
		virtual void onWaybillList( wxCommandEvent& event ){ event.Skip(); }
		virtual void onAddWaybill( wxCommandEvent& event ){ event.Skip(); }
		virtual void onDeleteWaybill( wxCommandEvent& event ){ event.Skip(); }
		virtual void onApply( wxCommandEvent& event ){ event.Skip(); }
		virtual void onCancel( wxCommandEvent& event ){ event.Skip(); }
		virtual void onOK( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		CarDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Car Table"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
		~CarDlg();
	
};

#endif //__cardlggen__
