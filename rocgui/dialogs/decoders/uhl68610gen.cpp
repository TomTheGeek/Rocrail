///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 17 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "uhl68610gen.h"

///////////////////////////////////////////////////////////////////////////

uhl68610dlggen::uhl68610dlggen( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_labAddr1 = new wxStaticText( this, wxID_ANY, wxT("Address 1"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labAddr1->Wrap( -1 );
	fgSizer1->Add( m_labAddr1, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );
	
	m_Addr1 = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 4095, 1 );
	fgSizer1->Add( m_Addr1, 0, wxALL, 5 );
	
	m_labAddr2 = new wxStaticText( this, wxID_ANY, wxT("Address 2"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labAddr2->Wrap( -1 );
	fgSizer1->Add( m_labAddr2, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );
	
	m_Addr2 = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 4095, 2 );
	fgSizer1->Add( m_Addr2, 0, wxALL, 5 );
	
	bSizer1->Add( fgSizer1, 1, wxEXPAND, 5 );
	
	wxString m_ReportingChoices[] = { wxT("Double"), wxT("Single") };
	int m_ReportingNChoices = sizeof( m_ReportingChoices ) / sizeof( wxString );
	m_Reporting = new wxRadioBox( this, wxID_ANY, wxT("Reporting"), wxDefaultPosition, wxDefaultSize, m_ReportingNChoices, m_ReportingChoices, 1, wxRA_SPECIFY_ROWS );
	m_Reporting->SetSelection( 0 );
	bSizer1->Add( m_Reporting, 0, wxALL|wxEXPAND, 5 );
	
	wxString m_FormatChoices[] = { wxT("none"), wxT("Lissy"), wxT("Transponder Exit"), wxT("Transponder Enter") };
	int m_FormatNChoices = sizeof( m_FormatChoices ) / sizeof( wxString );
	m_Format = new wxRadioBox( this, wxID_ANY, wxT("Reporting Format"), wxDefaultPosition, wxDefaultSize, m_FormatNChoices, m_FormatChoices, 1, wxRA_SPECIFY_COLS );
	m_Format->SetSelection( 0 );
	bSizer1->Add( m_Format, 0, wxALL|wxEXPAND, 5 );
	
	wxString m_ScaleChoices[] = { wxT("I"), wxT("0"), wxT("H0"), wxT("TT"), wxT("N"), wxT("Z") };
	int m_ScaleNChoices = sizeof( m_ScaleChoices ) / sizeof( wxString );
	m_Scale = new wxRadioBox( this, wxID_ANY, wxT("Scale"), wxDefaultPosition, wxDefaultSize, m_ScaleNChoices, m_ScaleChoices, 1, wxRA_SPECIFY_ROWS );
	m_Scale->SetSelection( 0 );
	bSizer1->Add( m_Scale, 0, wxALL, 5 );
	
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_labGap = new wxStaticText( this, wxID_ANY, wxT("Sensor gap"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labGap->Wrap( -1 );
	fgSizer2->Add( m_labGap, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );
	
	m_Gap = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 100, 0 );
	fgSizer2->Add( m_Gap, 0, wxALL, 5 );
	
	bSizer1->Add( fgSizer2, 0, wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer3;
	fgSizer3 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer3->SetFlexibleDirection( wxBOTH );
	fgSizer3->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_Read = new wxButton( this, wxID_ANY, wxT("Read"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer3->Add( m_Read, 0, wxALL, 5 );
	
	m_Write = new wxButton( this, wxID_ANY, wxT("Write"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer3->Add( m_Write, 0, wxALL, 5 );
	
	bSizer1->Add( fgSizer3, 0, wxEXPAND, 5 );
	
	m_sdbSizer1 = new wxStdDialogButtonSizer();
	m_sdbSizer1OK = new wxButton( this, wxID_OK );
	m_sdbSizer1->AddButton( m_sdbSizer1OK );
	m_sdbSizer1Cancel = new wxButton( this, wxID_CANCEL );
	m_sdbSizer1->AddButton( m_sdbSizer1Cancel );
	m_sdbSizer1->Realize();
	bSizer1->Add( m_sdbSizer1, 0, wxEXPAND, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	bSizer1->Fit( this );
	
	// Connect Events
	m_Reporting->Connect( wxEVT_COMMAND_RADIOBOX_SELECTED, wxCommandEventHandler( uhl68610dlggen::onReporting ), NULL, this );
	m_Read->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( uhl68610dlggen::onRead ), NULL, this );
	m_Write->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( uhl68610dlggen::onWrite ), NULL, this );
	m_sdbSizer1Cancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( uhl68610dlggen::onCancel ), NULL, this );
	m_sdbSizer1OK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( uhl68610dlggen::onOK ), NULL, this );
}

uhl68610dlggen::~uhl68610dlggen()
{
	// Disconnect Events
	m_Reporting->Disconnect( wxEVT_COMMAND_RADIOBOX_SELECTED, wxCommandEventHandler( uhl68610dlggen::onReporting ), NULL, this );
	m_Read->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( uhl68610dlggen::onRead ), NULL, this );
	m_Write->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( uhl68610dlggen::onWrite ), NULL, this );
	m_sdbSizer1Cancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( uhl68610dlggen::onCancel ), NULL, this );
	m_sdbSizer1OK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( uhl68610dlggen::onOK ), NULL, this );
}
