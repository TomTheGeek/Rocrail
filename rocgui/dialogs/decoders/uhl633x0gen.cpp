///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 17 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "uhl633x0gen.h"

///////////////////////////////////////////////////////////////////////////

uhl633x0dlggen::uhl633x0dlggen( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	this->SetExtraStyle( wxWS_EX_BLOCK_EVENTS );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_MainPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_labModAddr = new wxStaticText( m_MainPanel, wxID_ANY, wxT("Moduleaddress"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labModAddr->Wrap( -1 );
	fgSizer1->Add( m_labModAddr, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5 );
	
	m_ModAddr = new wxTextCtrl( m_MainPanel, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
	m_ModAddr->SetMaxLength( 5 ); 
	fgSizer1->Add( m_ModAddr, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_labRepAddr = new wxStaticText( m_MainPanel, wxID_ANY, wxT("Reportaddress"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labRepAddr->Wrap( -1 );
	fgSizer1->Add( m_labRepAddr, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5 );
	
	m_RepAddr = new wxTextCtrl( m_MainPanel, wxID_ANY, wxT("1017"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
	m_RepAddr->SetMaxLength( 4 ); 
	fgSizer1->Add( m_RepAddr, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	bSizer2->Add( fgSizer1, 0, wxEXPAND|wxALL, 5 );
	
	wxStaticBoxSizer* m_ConfigSizer;
	m_ConfigSizer = new wxStaticBoxSizer( new wxStaticBox( m_MainPanel, wxID_ANY, wxT("Configuration") ), wxVERTICAL );
	
	m_AutoAddressing = new wxCheckBox( m_MainPanel, wxID_ANY, wxT("Automatic Addressing"), wxDefaultPosition, wxDefaultSize, 0 );
	
	m_ConfigSizer->Add( m_AutoAddressing, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP, 5 );
	
	m_IndividualDelays = new wxCheckBox( m_MainPanel, wxID_ANY, wxT("Individual delays"), wxDefaultPosition, wxDefaultSize, 0 );
	
	m_ConfigSizer->Add( m_IndividualDelays, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT, 5 );
	
	m_ReportAtPowerOn = new wxCheckBox( m_MainPanel, wxID_ANY, wxT("Report at poweron"), wxDefaultPosition, wxDefaultSize, 0 );
	
	m_ConfigSizer->Add( m_ReportAtPowerOn, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM, 5 );
	
	bSizer2->Add( m_ConfigSizer, 0, wxEXPAND|wxALL, 5 );
	
	wxString m_ModuleTypeChoices[] = { wxT("63340"), wxT("63350") };
	int m_ModuleTypeNChoices = sizeof( m_ModuleTypeChoices ) / sizeof( wxString );
	m_ModuleType = new wxRadioBox( m_MainPanel, wxID_ANY, wxT("Module type"), wxDefaultPosition, wxDefaultSize, m_ModuleTypeNChoices, m_ModuleTypeChoices, 1, wxRA_SPECIFY_ROWS );
	m_ModuleType->SetSelection( 0 );
	bSizer2->Add( m_ModuleType, 0, wxALL, 5 );
	
	wxStaticBoxSizer* m_InputSizer;
	m_InputSizer = new wxStaticBoxSizer( new wxStaticBox( m_MainPanel, wxID_ANY, wxT("Inputs") ), wxVERTICAL );
	
	wxString m_SettingChoices[] = { wxT("Address"), wxT("On delay"), wxT("Off delay") };
	int m_SettingNChoices = sizeof( m_SettingChoices ) / sizeof( wxString );
	m_Setting = new wxRadioBox( m_MainPanel, wxID_ANY, wxT("Set"), wxDefaultPosition, wxDefaultSize, m_SettingNChoices, m_SettingChoices, 1, wxRA_SPECIFY_ROWS );
	m_Setting->SetSelection( 0 );
	m_InputSizer->Add( m_Setting, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
	
	wxFlexGridSizer* m_InputSettingSizer;
	m_InputSettingSizer = new wxFlexGridSizer( 4, 4, 0, 0 );
	m_InputSettingSizer->SetFlexibleDirection( wxBOTH );
	m_InputSettingSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_Input1 = new wxSpinCtrl( m_MainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 60,-1 ), wxSP_ARROW_KEYS, 1, 2048, 1 );
	m_InputSettingSizer->Add( m_Input1, 0, 0, 5 );
	
	m_Input2 = new wxSpinCtrl( m_MainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 60,-1 ), wxSP_ARROW_KEYS, 1, 2048, 1 );
	m_InputSettingSizer->Add( m_Input2, 0, 0, 5 );
	
	m_Input3 = new wxSpinCtrl( m_MainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 60,-1 ), wxSP_ARROW_KEYS, 1, 2048, 1 );
	m_InputSettingSizer->Add( m_Input3, 0, 0, 5 );
	
	m_Input4 = new wxSpinCtrl( m_MainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 60,-1 ), wxSP_ARROW_KEYS, 1, 2048, 1 );
	m_InputSettingSizer->Add( m_Input4, 0, 0, 5 );
	
	m_Input5 = new wxSpinCtrl( m_MainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 60,-1 ), wxSP_ARROW_KEYS, 1, 2048, 1 );
	m_InputSettingSizer->Add( m_Input5, 0, wxBOTTOM, 5 );
	
	m_Input6 = new wxSpinCtrl( m_MainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 60,-1 ), wxSP_ARROW_KEYS, 1, 2048, 1 );
	m_InputSettingSizer->Add( m_Input6, 0, wxBOTTOM, 5 );
	
	m_Input7 = new wxSpinCtrl( m_MainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 60,-1 ), wxSP_ARROW_KEYS, 1, 2048, 1 );
	m_InputSettingSizer->Add( m_Input7, 0, wxBOTTOM, 5 );
	
	m_Input8 = new wxSpinCtrl( m_MainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 60,-1 ), wxSP_ARROW_KEYS, 1, 2048, 1 );
	m_InputSettingSizer->Add( m_Input8, 0, wxBOTTOM, 5 );
	
	m_Input9 = new wxSpinCtrl( m_MainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 60,-1 ), wxSP_ARROW_KEYS, 1, 2048, 1 );
	m_InputSettingSizer->Add( m_Input9, 0, wxTOP, 5 );
	
	m_Input10 = new wxSpinCtrl( m_MainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 60,-1 ), wxSP_ARROW_KEYS, 1, 2048, 1 );
	m_InputSettingSizer->Add( m_Input10, 0, wxTOP, 5 );
	
	m_Input11 = new wxSpinCtrl( m_MainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 60,-1 ), wxSP_ARROW_KEYS, 1, 2048, 1 );
	m_InputSettingSizer->Add( m_Input11, 0, wxTOP, 5 );
	
	m_Input12 = new wxSpinCtrl( m_MainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 60,-1 ), wxSP_ARROW_KEYS, 1, 2048, 1 );
	m_InputSettingSizer->Add( m_Input12, 0, wxTOP, 5 );
	
	m_Input13 = new wxSpinCtrl( m_MainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 60,-1 ), wxSP_ARROW_KEYS, 1, 2048, 1 );
	m_InputSettingSizer->Add( m_Input13, 0, 0, 5 );
	
	m_Input14 = new wxSpinCtrl( m_MainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 60,-1 ), wxSP_ARROW_KEYS, 1, 2048, 1 );
	m_InputSettingSizer->Add( m_Input14, 0, 0, 5 );
	
	m_Input15 = new wxSpinCtrl( m_MainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 60,-1 ), wxSP_ARROW_KEYS, 1, 2048, 1 );
	m_InputSettingSizer->Add( m_Input15, 0, 0, 5 );
	
	m_Input16 = new wxSpinCtrl( m_MainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 60,-1 ), wxSP_ARROW_KEYS, 1, 2048, 1 );
	m_InputSettingSizer->Add( m_Input16, 0, 0, 5 );
	
	m_InputSizer->Add( m_InputSettingSizer, 0, 0, 5 );
	
	bSizer2->Add( m_InputSizer, 0, wxEXPAND|wxALL, 5 );
	
	m_MainPanel->SetSizer( bSizer2 );
	m_MainPanel->Layout();
	bSizer2->Fit( m_MainPanel );
	bSizer1->Add( m_MainPanel, 0, wxEXPAND|wxALL, 5 );
	
	m_stdButton = new wxStdDialogButtonSizer();
	m_stdButtonOK = new wxButton( this, wxID_OK );
	m_stdButton->AddButton( m_stdButtonOK );
	m_stdButtonCancel = new wxButton( this, wxID_CANCEL );
	m_stdButton->AddButton( m_stdButtonCancel );
	m_stdButton->Realize();
	bSizer1->Add( m_stdButton, 0, wxALIGN_RIGHT|wxALL, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	bSizer1->Fit( this );
	
	this->Centre( wxBOTH );
}

uhl633x0dlggen::~uhl633x0dlggen()
{
}
