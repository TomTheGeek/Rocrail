///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 17 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __powermandlggen__
#define __powermandlggen__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/listbox.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/combobox.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class powermandlggen
///////////////////////////////////////////////////////////////////////////////
class powermandlggen : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_labBoosters;
		wxListBox* m_Boosters;
		wxStaticText* m_labID;
		wxTextCtrl* m_ID;
		wxStaticText* m_labDistrict;
		wxTextCtrl* m_District;
		wxButton* m_AddBooster;
		wxButton* m_DelBooster;
		wxStaticText* m_labModules;
		wxListBox* m_Modules;
		wxComboBox* m_ModulesCombo;
		wxButton* m_AddModule;
		wxButton* m_DelModule;
		wxStaticText* m_labBlocks;
		wxListBox* m_Blocks;
		wxComboBox* m_BlocksCombo;
		wxButton* m_AddBlock;
		wxButton* m_DelBlock;
		wxStdDialogButtonSizer* m_sdbSizer1;
		wxButton* m_sdbSizer1OK;
		wxButton* m_sdbSizer1Apply;
		wxButton* m_sdbSizer1Cancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnAddBooster( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnDelBooster( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnAddModule( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnDelModule( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnAddBlock( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnDelBlock( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnApply( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnCancel( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnOK( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		powermandlggen( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Power districts setup"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
		~powermandlggen();
	
};

#endif //__powermandlggen__
