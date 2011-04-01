///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Dec 21 2009)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "mgvdlggen.h"

///////////////////////////////////////////////////////////////////////////

mgvdlggen::mgvdlggen( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );
	
	m_MGVBook = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_Setup = new wxPanel( m_MGVBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer3;
	fgSizer3 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer3->AddGrowableCol( 1 );
	fgSizer3->SetFlexibleDirection( wxBOTH );
	fgSizer3->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_labIID = new wxStaticText( m_Setup, wxID_ANY, wxT("IID"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labIID->Wrap( -1 );
	fgSizer3->Add( m_labIID, 0, wxALL, 5 );
	
	m_IID = new wxTextCtrl( m_Setup, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer3->Add( m_IID, 0, wxALL|wxEXPAND, 5 );
	
	bSizer7->Add( fgSizer3, 0, wxEXPAND, 5 );
	
	m_staticline2 = new wxStaticLine( m_Setup, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer7->Add( m_staticline2, 0, wxEXPAND | wxALL, 5 );
	
	wxFlexGridSizer* fgSizer4;
	fgSizer4 = new wxFlexGridSizer( 5, 3, 0, 0 );
	fgSizer4->SetFlexibleDirection( wxBOTH );
	fgSizer4->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText14 = new wxStaticText( m_Setup, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText14->Wrap( -1 );
	fgSizer4->Add( m_staticText14, 0, wxALL, 5 );
	
	m_labAddress = new wxStaticText( m_Setup, wxID_ANY, wxT("Address"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labAddress->Wrap( -1 );
	fgSizer4->Add( m_labAddress, 0, wxALL, 5 );
	
	m_labPort = new wxStaticText( m_Setup, wxID_ANY, wxT("Port"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labPort->Wrap( -1 );
	fgSizer4->Add( m_labPort, 0, wxALL, 5 );
	
	m_labOne = new wxStaticText( m_Setup, wxID_ANY, wxT("1"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labOne->Wrap( -1 );
	fgSizer4->Add( m_labOne, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_Addr1 = new wxSpinCtrl( m_Setup, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 100,-1 ), wxSP_ARROW_KEYS, 0, 9999, 0 );
	fgSizer4->Add( m_Addr1, 0, wxALL, 5 );
	
	m_Port1 = new wxSpinCtrl( m_Setup, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 100,-1 ), wxSP_ARROW_KEYS, 0, 9999, 0 );
	fgSizer4->Add( m_Port1, 0, wxALL, 5 );
	
	m_labTwo = new wxStaticText( m_Setup, wxID_ANY, wxT("2"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labTwo->Wrap( -1 );
	fgSizer4->Add( m_labTwo, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	m_Addr2 = new wxSpinCtrl( m_Setup, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 100,-1 ), wxSP_ARROW_KEYS, 0, 9999, 0 );
	fgSizer4->Add( m_Addr2, 0, wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	m_Port2 = new wxSpinCtrl( m_Setup, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 100,-1 ), wxSP_ARROW_KEYS, 0, 9999, 0 );
	fgSizer4->Add( m_Port2, 0, wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	m_labThree = new wxStaticText( m_Setup, wxID_ANY, wxT("3"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labThree->Wrap( -1 );
	fgSizer4->Add( m_labThree, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	m_Addr3 = new wxSpinCtrl( m_Setup, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 100,-1 ), wxSP_ARROW_KEYS, 0, 9999, 0 );
	fgSizer4->Add( m_Addr3, 0, wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	m_Port3 = new wxSpinCtrl( m_Setup, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 100,-1 ), wxSP_ARROW_KEYS, 0, 9999, 0 );
	fgSizer4->Add( m_Port3, 0, wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	m_labFour = new wxStaticText( m_Setup, wxID_ANY, wxT("4"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labFour->Wrap( -1 );
	fgSizer4->Add( m_labFour, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	m_Addr4 = new wxSpinCtrl( m_Setup, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 100,-1 ), wxSP_ARROW_KEYS, 0, 9999, 0 );
	fgSizer4->Add( m_Addr4, 0, wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	m_Port4 = new wxSpinCtrl( m_Setup, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 100,-1 ), wxSP_ARROW_KEYS, 0, 9999, 0 );
	fgSizer4->Add( m_Port4, 0, wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	bSizer7->Add( fgSizer4, 0, wxEXPAND, 5 );
	
	m_staticline3 = new wxStaticLine( m_Setup, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer7->Add( m_staticline3, 0, wxEXPAND | wxALL, 5 );
	
	m_labProgramming = new wxStaticText( m_Setup, wxID_ANY, wxT("Programming:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labProgramming->Wrap( -1 );
	bSizer7->Add( m_labProgramming, 0, wxALL, 5 );
	
	wxFlexGridSizer* fgSizer5;
	fgSizer5 = new wxFlexGridSizer( 2, 3, 0, 0 );
	fgSizer5->SetFlexibleDirection( wxBOTH );
	fgSizer5->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_ProgrammingOn = new wxButton( m_Setup, wxID_ANY, wxT("ON"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer5->Add( m_ProgrammingOn, 0, wxALL, 5 );
	
	m_ProgrammingOff = new wxButton( m_Setup, wxID_ANY, wxT("OFF"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer5->Add( m_ProgrammingOff, 0, wxALL, 5 );
	
	bSizer7->Add( fgSizer5, 0, wxEXPAND, 5 );
	
	m_Setup->SetSizer( bSizer7 );
	m_Setup->Layout();
	bSizer7->Fit( m_Setup );
	m_MGVBook->AddPage( m_Setup, wxT("Setup"), true );
	m_Servo = new wxPanel( m_MGVBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxHORIZONTAL );
	
	wxString m_ServoSelectionChoices[] = { wxT("1"), wxT("2"), wxT("3"), wxT("4") };
	int m_ServoSelectionNChoices = sizeof( m_ServoSelectionChoices ) / sizeof( wxString );
	m_ServoSelection = new wxRadioBox( m_Servo, wxID_ANY, wxT("Servo"), wxDefaultPosition, wxDefaultSize, m_ServoSelectionNChoices, m_ServoSelectionChoices, 1, wxRA_SPECIFY_COLS );
	m_ServoSelection->SetSelection( 0 );
	bSizer8->Add( m_ServoSelection, 0, wxALL, 5 );
	
	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer6;
	fgSizer6 = new wxFlexGridSizer( 3, 3, 0, 0 );
	fgSizer6->AddGrowableRow( 1 );
	fgSizer6->SetFlexibleDirection( wxVERTICAL );
	fgSizer6->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_labServoLeft = new wxStaticText( m_Servo, wxID_ANY, wxT("Left"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labServoLeft->Wrap( -1 );
	fgSizer6->Add( m_labServoLeft, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_labServoRight = new wxStaticText( m_Servo, wxID_ANY, wxT("Right"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labServoRight->Wrap( -1 );
	fgSizer6->Add( m_labServoRight, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_labServoSpeed = new wxStaticText( m_Servo, wxID_ANY, wxT("Speed"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labServoSpeed->Wrap( -1 );
	fgSizer6->Add( m_labServoSpeed, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_ServoLeftAng = new wxSlider( m_Servo, wxID_ANY, 50, 1, 100, wxDefaultPosition, wxDefaultSize, wxSL_AUTOTICKS|wxSL_INVERSE|wxSL_VERTICAL );
	fgSizer6->Add( m_ServoLeftAng, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxEXPAND, 5 );
	
	m_ServoRightAng = new wxSlider( m_Servo, wxID_ANY, 50, 1, 100, wxDefaultPosition, wxDefaultSize, wxSL_AUTOTICKS|wxSL_INVERSE|wxSL_VERTICAL );
	fgSizer6->Add( m_ServoRightAng, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxEXPAND, 5 );
	
	m_ServoSpeed = new wxSlider( m_Servo, wxID_ANY, 1, 1, 4, wxDefaultPosition, wxDefaultSize, wxSL_AUTOTICKS|wxSL_INVERSE|wxSL_VERTICAL );
	fgSizer6->Add( m_ServoSpeed, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxEXPAND, 5 );
	
	m_SetLeftAngle = new wxButton( m_Servo, wxID_ANY, wxT("Set"), wxDefaultPosition, wxSize( 60,-1 ), 0 );
	fgSizer6->Add( m_SetLeftAngle, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_SetRightAngle = new wxButton( m_Servo, wxID_ANY, wxT("Set"), wxDefaultPosition, wxSize( 60,-1 ), 0 );
	fgSizer6->Add( m_SetRightAngle, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_SetSpeed = new wxButton( m_Servo, wxID_ANY, wxT("Set"), wxDefaultPosition, wxSize( 60,-1 ), 0 );
	fgSizer6->Add( m_SetSpeed, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	bSizer11->Add( fgSizer6, 1, wxEXPAND, 5 );
	
	m_staticline4 = new wxStaticLine( m_Servo, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer11->Add( m_staticline4, 0, wxEXPAND | wxALL, 5 );
	
	m_ServoRelay = new wxCheckBox( m_Servo, wxID_ANY, wxT("Polarisation relay"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer11->Add( m_ServoRelay, 0, wxALL, 5 );
	
	bSizer8->Add( bSizer11, 1, wxEXPAND, 5 );
	
	m_Servo->SetSizer( bSizer8 );
	m_Servo->Layout();
	bSizer8->Fit( m_Servo );
	m_MGVBook->AddPage( m_Servo, wxT("Servo"), false );
	
	bSizer6->Add( m_MGVBook, 0, wxEXPAND | wxALL, 5 );
	
	m_StdButton = new wxStdDialogButtonSizer();
	m_StdButtonOK = new wxButton( this, wxID_OK );
	m_StdButton->AddButton( m_StdButtonOK );
	m_StdButton->Realize();
	bSizer6->Add( m_StdButton, 1, wxEXPAND|wxALL|wxALIGN_RIGHT, 5 );
	
	this->SetSizer( bSizer6 );
	this->Layout();
	bSizer6->Fit( this );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_ProgrammingOn->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mgvdlggen::OnProgrammingOn ), NULL, this );
	m_ProgrammingOff->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mgvdlggen::OnProgrammingOff ), NULL, this );
	m_ServoSelection->Connect( wxEVT_COMMAND_RADIOBOX_SELECTED, wxCommandEventHandler( mgvdlggen::OnServoSelect ), NULL, this );
	m_ServoLeftAng->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( mgvdlggen::OnServoLeftAngle ), NULL, this );
	m_ServoRightAng->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( mgvdlggen::OnServoRightAngle ), NULL, this );
	m_ServoSpeed->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( mgvdlggen::OnServoSpeed ), NULL, this );
	m_SetLeftAngle->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mgvdlggen::onSetLeftAngle ), NULL, this );
	m_SetRightAngle->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mgvdlggen::onSetRightAngle ), NULL, this );
	m_SetSpeed->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mgvdlggen::onSetSpeed ), NULL, this );
	m_ServoRelay->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( mgvdlggen::OnServoRelay ), NULL, this );
	m_StdButtonOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mgvdlggen::OnOK ), NULL, this );
}

mgvdlggen::~mgvdlggen()
{
	// Disconnect Events
	m_ProgrammingOn->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mgvdlggen::OnProgrammingOn ), NULL, this );
	m_ProgrammingOff->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mgvdlggen::OnProgrammingOff ), NULL, this );
	m_ServoSelection->Disconnect( wxEVT_COMMAND_RADIOBOX_SELECTED, wxCommandEventHandler( mgvdlggen::OnServoSelect ), NULL, this );
	m_ServoLeftAng->Disconnect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( mgvdlggen::OnServoLeftAngle ), NULL, this );
	m_ServoRightAng->Disconnect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( mgvdlggen::OnServoRightAngle ), NULL, this );
	m_ServoSpeed->Disconnect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( mgvdlggen::OnServoSpeed ), NULL, this );
	m_SetLeftAngle->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mgvdlggen::onSetLeftAngle ), NULL, this );
	m_SetRightAngle->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mgvdlggen::onSetRightAngle ), NULL, this );
	m_SetSpeed->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mgvdlggen::onSetSpeed ), NULL, this );
	m_ServoRelay->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( mgvdlggen::OnServoRelay ), NULL, this );
	m_StdButtonOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mgvdlggen::OnOK ), NULL, this );
}
