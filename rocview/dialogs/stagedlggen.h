///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __stagedlggen__
#define __stagedlggen__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/spinctrl.h>
#include <wx/combobox.h>
#include <wx/button.h>
#include <wx/panel.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/listbox.h>
#include <wx/checkbox.h>
#include <wx/radiobox.h>
#include <wx/statbox.h>
#include <wx/notebook.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////

#define ID_STAGEBOOK 1000

///////////////////////////////////////////////////////////////////////////////
/// Class stagedlggen
///////////////////////////////////////////////////////////////////////////////
class stagedlggen : public wxDialog 
{
	private:
	
	protected:
		wxNotebook* m_Notebook;
		wxPanel* m_General;
		wxStaticText* m_labID;
		wxTextCtrl* m_ID;
		wxStaticText* m_labDescription;
		wxTextCtrl* m_Description;
		wxStaticLine* m_staticline1;
		wxStaticText* m_labSectionLength;
		wxSpinCtrl* m_SectionLength;
		wxStaticText* m_labTrainGap;
		wxSpinCtrl* m_TrainGap;
		wxStaticText* m_labEnterSensor;
		wxComboBox* m_EnterSensor;
		wxStaticText* m_labEnterSignal;
		wxComboBox* m_EnterSignal;
		wxStaticText* m_labExitSignal;
		wxComboBox* m_ExitSignal;
		wxButton* m_Actions;
		wxPanel* m_Sections;
		wxListBox* m_SectionList;
		wxStaticText* m_labSectionID;
		wxTextCtrl* m_SectionID;
		wxStaticText* m_labSectionSensor;
		wxComboBox* m_SectionSensor;
		wxStaticText* m_labOwnSectionLength;
		wxSpinCtrl* m_OwnSectionLength;
		wxStaticText* m_labOccSensor;
		wxComboBox* m_OccSensor;
		wxStaticText* m_labSectionLocoId;
		wxComboBox* m_SectionLocoId;
		wxButton* m_AddSection;
		wxButton* m_ModifySection;
		wxButton* m_DeleteSection;
		wxButton* m_SetLoco;
		wxButton* m_FreeSection;
		wxButton* m_FreeAll;
		wxPanel* m_Details;
		wxStaticText* m_labDepartDelay;
		wxSpinCtrl* m_DepartDelay;
		wxCheckBox* m_SuitsWell;
		wxCheckBox* m_InAtLen;
		wxRadioBox* m_WaitType;
		wxStaticText* m_labRandomMin;
		wxSpinCtrl* m_RandomMin;
		wxStaticText* m_labRandomMax;
		wxSpinCtrl* m_RandomMax;
		wxStaticText* m_labFixed;
		wxSpinCtrl* m_Fixed;
		wxRadioBox* m_ArriveSpeed;
		wxSpinCtrl* m_ArriveSpeedPercent;
		wxRadioBox* m_ExitSpeed;
		wxStdDialogButtonSizer* m_stdButton;
		wxButton* m_stdButtonOK;
		wxButton* m_stdButtonCancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void onActions( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSectionList( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSectionAdd( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSectionModify( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSectionDelete( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSetLoco( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnFreeSection( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnFreeAll( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCancel( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnOK( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		wxStaticBoxSizer* m_WaitBox;
		wxStaticBoxSizer* m_SpeedBox;
		
		stagedlggen( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Staging Block"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
		~stagedlggen();
	
};

#endif //__stagedlggen__
