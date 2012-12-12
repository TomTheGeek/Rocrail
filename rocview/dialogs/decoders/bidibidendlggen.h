///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __bidibidendlggen__
#define __bidibidendlggen__

#include <wx/treectrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/statline.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/statbmp.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/listbox.h>
#include <wx/spinctrl.h>
#include <wx/button.h>
#include <wx/slider.h>
#include <wx/radiobox.h>
#include <wx/statbox.h>
#include <wx/grid.h>
#include <wx/checkbox.h>
#include <wx/combobox.h>
#include <wx/notebook.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class BidibIdentDlgGen
///////////////////////////////////////////////////////////////////////////////
class BidibIdentDlgGen : public wxDialog 
{
	private:
	
	protected:
		wxNotebook* m_Notebook;
		wxPanel* m_IndexPanel;
		wxTreeCtrl* m_Tree;
		wxStaticLine* m_staticline1;
		wxStaticBitmap* m_BiDiBlogo;
		wxStaticText* m_labPath;
		wxTextCtrl* m_Path;
		wxStaticText* m_labUID;
		wxTextCtrl* m_UID;
		wxTextCtrl* m_UIDX;
		wxStaticText* m_labVendor;
		wxTextCtrl* m_VendorName;
		wxStaticText* m_labClass;
		wxTextCtrl* m_Class;
		wxStaticText* m_labVersion;
		wxTextCtrl* m_Version;
		wxPanel* m_FeaturesPanel;
		wxListBox* m_FeatureList;
		wxStaticLine* m_staticline2;
		wxStaticText* m_labIID;
		wxTextCtrl* m_IID;
		wxStaticLine* m_staticline3;
		wxStaticLine* m_staticline4;
		wxStaticText* m_labFeature;
		wxSpinCtrl* m_Feature;
		wxStaticText* m_labFeatureValue;
		wxSpinCtrl* m_FeatureValue;
		wxButton* m_FeaturesGet;
		wxButton* m_FeatureSet;
		wxPanel* m_ServoPanel;
		wxStaticText* m_labServoLeft;
		wxStaticText* m_labServoRight;
		wxStaticText* m_labServoSpeed;
		wxStaticText* m_labServoReserved;
		wxSlider* m_ServoLeft;
		wxSlider* m_ServoRight;
		wxSlider* m_ServoSpeed;
		wxSlider* m_ServoReserved;
		wxSpinCtrl* m_ConfigL;
		wxSpinCtrl* m_ConfigR;
		wxSpinCtrl* m_ConfigV;
		wxSpinCtrl* m_ConfigS;
		wxRadioBox* m_PortType;
		wxStaticText* m_labServoPort;
		wxSpinCtrl* m_ServoPort;
		wxButton* m_ServoGet;
		wxButton* m_ServoLeftTest;
		wxButton* m_ServoRightTest;
		wxPanel* m_MacroPanel;
		wxStaticText* m_labMacroList;
		wxListBox* m_MacroList;
		wxStaticText* m_labMacroSlowdown;
		wxSpinCtrl* m_MacroSlowdown;
		wxStaticText* m_labCycles;
		wxSpinCtrl* m_MacroCycles;
		wxStaticText* m_labMacroTime;
		wxSpinCtrl* m_MacroHours;
		wxStaticText* m_staticText23;
		wxSpinCtrl* m_MacroMinutes;
		wxStaticText* m_staticText26;
		wxSpinCtrl* m_MacroWDay;
		wxGrid* m_MacroLines;
		wxCheckBox* m_MacroStartDaily;
		wxCheckBox* m_MacroStartHourly;
		wxCheckBox* m_MacroStart30;
		wxCheckBox* m_MacroStart15;
		wxCheckBox* m_MacroStart1;
		wxStaticText* m_labMacroDelay;
		wxSpinCtrl* m_MacroDelay;
		wxStaticText* m_labMacroType;
		wxComboBox* m_MacroType;
		wxStaticText* m_labMacroPort;
		wxSpinCtrl* m_MacroPort;
		wxStaticText* m_labMacroValue;
		wxSpinCtrl* m_MacroValue;
		wxButton* m_MacroApply;
		wxButton* m_MacroReload;
		wxButton* m_MacroSave;
		wxButton* m_MacroExport;
		wxButton* m_MacroImport;
		wxButton* m_MacroSaveMacro;
		wxButton* m_MacroDeleteMacro;
		wxButton* m_MacroRestoreMacro;
		wxPanel* m_UpdatePanel;
		wxStaticText* m_labUpdateFile;
		wxTextCtrl* m_UpdateFile;
		wxButton* m_OpenUpdateFile;
		wxStaticText* m_labUpdateFilePreview;
		wxTextCtrl* m_HEXFileText;
		wxButton* m_UpdateStart;
		wxStdDialogButtonSizer* m_stdButtons;
		wxButton* m_stdButtonsOK;
		wxButton* m_stdButtonsCancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void onBeginDrag( wxTreeEvent& event ) { event.Skip(); }
		virtual void onItemActivated( wxTreeEvent& event ) { event.Skip(); }
		virtual void onItemRightClick( wxTreeEvent& event ) { event.Skip(); }
		virtual void onTreeSelChanged( wxTreeEvent& event ) { event.Skip(); }
		virtual void onFeatureSelect( wxCommandEvent& event ) { event.Skip(); }
		virtual void onFeaturesGet( wxCommandEvent& event ) { event.Skip(); }
		virtual void onFeatureSet( wxCommandEvent& event ) { event.Skip(); }
		virtual void onServoLeft( wxScrollEvent& event ) { event.Skip(); }
		virtual void onServoRight( wxScrollEvent& event ) { event.Skip(); }
		virtual void onServoSpeed( wxScrollEvent& event ) { event.Skip(); }
		virtual void onServoReserved( wxScrollEvent& event ) { event.Skip(); }
		virtual void onConfigL( wxSpinEvent& event ) { event.Skip(); }
		virtual void onConfigLtxt( wxCommandEvent& event ) { event.Skip(); }
		virtual void onConfigR( wxSpinEvent& event ) { event.Skip(); }
		virtual void onConfigRtxt( wxCommandEvent& event ) { event.Skip(); }
		virtual void onConfigV( wxSpinEvent& event ) { event.Skip(); }
		virtual void onConfigVtxt( wxCommandEvent& event ) { event.Skip(); }
		virtual void onConfigS( wxSpinEvent& event ) { event.Skip(); }
		virtual void onConfigStxt( wxCommandEvent& event ) { event.Skip(); }
		virtual void onPortType( wxCommandEvent& event ) { event.Skip(); }
		virtual void onServoPort( wxSpinEvent& event ) { event.Skip(); }
		virtual void onServoGet( wxCommandEvent& event ) { event.Skip(); }
		virtual void onServoLeftTest( wxCommandEvent& event ) { event.Skip(); }
		virtual void onServoRightTest( wxCommandEvent& event ) { event.Skip(); }
		virtual void onMacroList( wxCommandEvent& event ) { event.Skip(); }
		virtual void onMacroLineSelected( wxGridEvent& event ) { event.Skip(); }
		virtual void onMacroEveryMinute( wxCommandEvent& event ) { event.Skip(); }
		virtual void onMacroApply( wxCommandEvent& event ) { event.Skip(); }
		virtual void onMacroReload( wxCommandEvent& event ) { event.Skip(); }
		virtual void onMacroSave( wxCommandEvent& event ) { event.Skip(); }
		virtual void onMacroExport( wxCommandEvent& event ) { event.Skip(); }
		virtual void onMacroImport( wxCommandEvent& event ) { event.Skip(); }
		virtual void onMacroSaveMacro( wxCommandEvent& event ) { event.Skip(); }
		virtual void onMacroDeleteMacro( wxCommandEvent& event ) { event.Skip(); }
		virtual void onMacroRestoreMacro( wxCommandEvent& event ) { event.Skip(); }
		virtual void onSelectUpdateFile( wxCommandEvent& event ) { event.Skip(); }
		virtual void onUpdateStart( wxCommandEvent& event ) { event.Skip(); }
		virtual void onCancel( wxCommandEvent& event ) { event.Skip(); }
		virtual void onOK( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		BidibIdentDlgGen( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("BiDiB Notifier"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~BidibIdentDlgGen();
	
};

#endif //__bidibidendlggen__
