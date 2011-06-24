///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Dec 21 2009)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "cbusdlggen.h"

///////////////////////////////////////////////////////////////////////////

cbusdlggen::cbusdlggen( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer4;
	fgSizer4 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer4->AddGrowableCol( 1 );
	fgSizer4->SetFlexibleDirection( wxBOTH );
	fgSizer4->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_labIID = new wxStaticText( this, wxID_ANY, wxT("IID"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labIID->Wrap( -1 );
	fgSizer4->Add( m_labIID, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );
	
	m_IID = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer4->Add( m_IID, 0, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_labCANID = new wxStaticText( this, wxID_ANY, wxT("CANID"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labCANID->Wrap( -1 );
	fgSizer4->Add( m_labCANID, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );
	
	m_CANID = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 100,-1 ), wxSP_ARROW_KEYS, 0, 127, 0 );
	fgSizer4->Add( m_CANID, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	bSizer8->Add( fgSizer4, 0, wxEXPAND, 5 );
	
	wxString m_SublibChoices[] = { wxT("USB"), wxT("Serial"), wxT("TCP/IP") };
	int m_SublibNChoices = sizeof( m_SublibChoices ) / sizeof( wxString );
	m_Sublib = new wxRadioBox( this, wxID_ANY, wxT("Type"), wxDefaultPosition, wxDefaultSize, m_SublibNChoices, m_SublibChoices, 1, wxRA_SPECIFY_ROWS );
	m_Sublib->SetSelection( 1 );
	bSizer8->Add( m_Sublib, 0, wxALL|wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer5;
	fgSizer5 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer5->AddGrowableCol( 1 );
	fgSizer5->SetFlexibleDirection( wxBOTH );
	fgSizer5->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_labDevice = new wxStaticText( this, wxID_ANY, wxT("Device"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labDevice->Wrap( -1 );
	fgSizer5->Add( m_labDevice, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_Device = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( -1,-1 ), 0 );
	fgSizer5->Add( m_Device, 1, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	bSizer8->Add( fgSizer5, 1, wxEXPAND, 5 );
	
	wxStaticBoxSizer* m_OptionsBox;
	m_OptionsBox = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Options") ), wxVERTICAL );
	
	m_ShortEvents = new wxCheckBox( this, wxID_ANY, wxT("Short events"), wxDefaultPosition, wxDefaultSize, 0 );
	m_ShortEvents->SetValue(true); 
	m_OptionsBox->Add( m_ShortEvents, 0, wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_FonFof = new wxCheckBox( this, wxID_ANY, wxT("FONFOF"), wxDefaultPosition, wxDefaultSize, 0 );
	m_OptionsBox->Add( m_FonFof, 0, wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	wxFlexGridSizer* fgSizer41;
	fgSizer41 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer41->SetFlexibleDirection( wxBOTH );
	fgSizer41->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_labSOD = new wxStaticText( this, wxID_ANY, wxT("SoD"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labSOD->Wrap( -1 );
	fgSizer41->Add( m_labSOD, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );
	
	m_SOD = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 100,-1 ), wxSP_ARROW_KEYS, 0, 65535, 0 );
	fgSizer41->Add( m_SOD, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_labSwTime = new wxStaticText( this, wxID_ANY, wxT("Switch time"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labSwTime->Wrap( -1 );
	fgSizer41->Add( m_labSwTime, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );
	
	m_SwTime = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 100,-1 ), wxSP_ARROW_KEYS, 0, 10000, 0 );
	fgSizer41->Add( m_SwTime, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_labPurgetime = new wxStaticText( this, wxID_ANY, wxT("Purge time"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labPurgetime->Wrap( -1 );
	fgSizer41->Add( m_labPurgetime, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_Purgetime = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 100,-1 ), wxSP_ARROW_KEYS, 20, 60, 20 );
	fgSizer41->Add( m_Purgetime, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_OptionsBox->Add( fgSizer41, 1, wxEXPAND, 5 );
	
	bSizer8->Add( m_OptionsBox, 0, wxEXPAND|wxALL, 5 );
	
	bSizer6->Add( bSizer8, 1, 0, 5 );
	
	m_StdButton = new wxStdDialogButtonSizer();
	m_StdButtonOK = new wxButton( this, wxID_OK );
	m_StdButton->AddButton( m_StdButtonOK );
	m_StdButtonCancel = new wxButton( this, wxID_CANCEL );
	m_StdButton->AddButton( m_StdButtonCancel );
	m_StdButton->Realize();
	bSizer6->Add( m_StdButton, 0, wxEXPAND|wxALL, 5 );
	
	this->SetSizer( bSizer6 );
	this->Layout();
	bSizer6->Fit( this );
	
	// Connect Events
	m_Sublib->Connect( wxEVT_COMMAND_RADIOBOX_SELECTED, wxCommandEventHandler( cbusdlggen::OnSublib ), NULL, this );
	m_StdButtonCancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusdlggen::OnCancel ), NULL, this );
	m_StdButtonOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusdlggen::OnOK ), NULL, this );
}

cbusdlggen::~cbusdlggen()
{
	// Disconnect Events
	m_Sublib->Disconnect( wxEVT_COMMAND_RADIOBOX_SELECTED, wxCommandEventHandler( cbusdlggen::OnSublib ), NULL, this );
	m_StdButtonCancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusdlggen::OnCancel ), NULL, this );
	m_StdButtonOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusdlggen::OnOK ), NULL, this );
}
