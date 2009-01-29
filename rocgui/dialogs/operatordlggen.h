///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 21 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __operatordlggen__
#define __operatordlggen__

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
#include <wx/combobox.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/listbox.h>
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
		wxPanel* m_LocoPanel;
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
	
	public:
		operatordlggen( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Operator"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
		~operatordlggen();
	
};

#endif //__operatordlggen__
