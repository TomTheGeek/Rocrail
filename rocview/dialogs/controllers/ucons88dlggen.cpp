///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep 12 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "ucons88dlggen.h"

///////////////////////////////////////////////////////////////////////////

ucons88dlggen::ucons88dlggen( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );
	
	m_General = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer5;
	fgSizer5 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer5->AddGrowableCol( 1 );
	fgSizer5->SetFlexibleDirection( wxBOTH );
	fgSizer5->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_labIID = new wxStaticText( m_General, wxID_ANY, wxT("IID"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labIID->Wrap( -1 );
	fgSizer5->Add( m_labIID, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_IID = new wxTextCtrl( m_General, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer5->Add( m_IID, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND|wxLEFT|wxRIGHT|wxTOP, 5 );
	
	bSizer6->Add( fgSizer5, 0, wxEXPAND, 5 );
	
	m_DeviceBox = new wxStaticBoxSizer( new wxStaticBox( m_General, wxID_ANY, wxT("Device") ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizer4;
	fgSizer4 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer4->AddGrowableCol( 1 );
	fgSizer4->SetFlexibleDirection( wxHORIZONTAL );
	fgSizer4->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_labHost = new wxStaticText( m_General, wxID_ANY, wxT("Host"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labHost->Wrap( -1 );
	fgSizer4->Add( m_labHost, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_Host = new wxTextCtrl( m_General, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer4->Add( m_Host, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP|wxEXPAND, 5 );
	
	m_labPort = new wxStaticText( m_General, wxID_ANY, wxT("Port"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labPort->Wrap( -1 );
	fgSizer4->Add( m_labPort, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_Port = new wxSpinCtrl( m_General, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 100,-1 ), wxSP_ARROW_KEYS, 0, 65535, 0 );
	fgSizer4->Add( m_Port, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5 );
	
	m_DeviceBox->Add( fgSizer4, 0, wxEXPAND, 5 );
	
	bSizer6->Add( m_DeviceBox, 0, wxALL|wxEXPAND, 5 );
	
	m_S88BusBox = new wxStaticBoxSizer( new wxStaticBox( m_General, wxID_ANY, wxT("S88 Bus Config") ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizer3;
	fgSizer3 = new wxFlexGridSizer( 3, 2, 0, 0 );
	fgSizer3->AddGrowableCol( 1 );
	fgSizer3->SetFlexibleDirection( wxHORIZONTAL );
	fgSizer3->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_labBus1 = new wxStaticText( m_General, wxID_ANY, wxT("Bus 1 Length"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labBus1->Wrap( -1 );
	fgSizer3->Add( m_labBus1, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxTOP|wxLEFT, 5 );
	
	m_B1 = new wxSpinCtrl( m_General, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 16, 0 );
	fgSizer3->Add( m_B1, 0, wxALIGN_CENTER_VERTICAL|wxEXPAND|wxLEFT|wxRIGHT|wxTOP, 5 );
	
	m_labBus2 = new wxStaticText( m_General, wxID_ANY, wxT("Bus 2 Length"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labBus2->Wrap( -1 );
	fgSizer3->Add( m_labBus2, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxLEFT|wxTOP, 5 );
	
	m_B2 = new wxSpinCtrl( m_General, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 16, 0 );
	fgSizer3->Add( m_B2, 0, wxALIGN_CENTER_VERTICAL|wxEXPAND|wxLEFT|wxRIGHT|wxTOP, 5 );
	
	m_labBus3 = new wxStaticText( m_General, wxID_ANY, wxT("Bus 3 Length"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labBus3->Wrap( -1 );
	fgSizer3->Add( m_labBus3, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxBOTTOM|wxLEFT|wxTOP, 5 );
	
	m_B3 = new wxSpinCtrl( m_General, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 16, 0 );
	fgSizer3->Add( m_B3, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 5 );
	
	m_S88BusBox->Add( fgSizer3, 0, wxEXPAND, 5 );
	
	bSizer6->Add( m_S88BusBox, 0, wxALL|wxEXPAND, 5 );
	
	m_General->SetSizer( bSizer6 );
	m_General->Layout();
	bSizer6->Fit( m_General );
	bSizer5->Add( m_General, 1, wxEXPAND | wxALL, 5 );
	
	m_StdButton = new wxStdDialogButtonSizer();
	m_StdButtonOK = new wxButton( this, wxID_OK );
	m_StdButton->AddButton( m_StdButtonOK );
	m_StdButtonCancel = new wxButton( this, wxID_CANCEL );
	m_StdButton->AddButton( m_StdButtonCancel );
	m_StdButtonHelp = new wxButton( this, wxID_HELP );
	m_StdButton->AddButton( m_StdButtonHelp );
	m_StdButton->Realize();
	bSizer5->Add( m_StdButton, 0, wxALL|wxALIGN_RIGHT|wxEXPAND, 5 );
	
	this->SetSizer( bSizer5 );
	this->Layout();
	
	// Connect Events
	m_StdButtonCancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ucons88dlggen::OnCancel ), NULL, this );
	m_StdButtonHelp->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ucons88dlggen::OnHelp ), NULL, this );
	m_StdButtonOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ucons88dlggen::OnOK ), NULL, this );
}

ucons88dlggen::~ucons88dlggen()
{
	// Disconnect Events
	m_StdButtonCancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ucons88dlggen::OnCancel ), NULL, this );
	m_StdButtonHelp->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ucons88dlggen::OnHelp ), NULL, this );
	m_StdButtonOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ucons88dlggen::OnOK ), NULL, this );
	
}
