///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "throttledlggen.h"

///////////////////////////////////////////////////////////////////////////

ThrottleDlgGen::ThrottleDlgGen( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer21;
	bSizer21 = new wxBoxSizer( wxVERTICAL );
	
	m_LocoImage = new wxBitmapButton( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 196,-1 ), wxBU_AUTODRAW );
	m_LocoImage->SetMaxSize( wxSize( -1,70 ) );
	
	m_LocoImage->SetMaxSize( wxSize( -1,70 ) );
	
	bSizer21->Add( m_LocoImage, 1, wxALIGN_CENTER_HORIZONTAL|wxALL|wxEXPAND, 2 );
	
	wxFlexGridSizer* fgSizer18;
	fgSizer18 = new wxFlexGridSizer( 0, 3, 0, 0 );
	fgSizer18->AddGrowableCol( 2 );
	fgSizer18->SetFlexibleDirection( wxBOTH );
	fgSizer18->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxBoxSizer* bSizer23;
	bSizer23 = new wxBoxSizer( wxVERTICAL );
	
	m_Speed = new LEDButton(this, wxT("0"), 60, 25, false, true);
	bSizer23->Add( m_Speed, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 1 );
	
	m_SpeedSlider = new Slider(this, 40, 125);
	bSizer23->Add( m_SpeedSlider, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 1 );
	
	m_SwitchAddr = new LEDButton(this, wxT("1"), 60, 25);
	bSizer23->Add( m_SwitchAddr, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxHORIZONTAL );
	
	m_BinState = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 70,25 ), wxSP_ARROW_KEYS, 0, 65535, 0 );
	bSizer5->Add( m_BinState, 0, wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	bSizer23->Add( bSizer5, 0, wxEXPAND, 5 );
	
	fgSizer18->Add( bSizer23, 0, wxEXPAND, 5 );
	
	m_staticline5 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL );
	fgSizer18->Add( m_staticline5, 0, wxEXPAND | wxALL, 2 );
	
	wxBoxSizer* bSizer24;
	bSizer24 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer17;
	fgSizer17 = new wxFlexGridSizer( 0, 3, 0, 0 );
	fgSizer17->AddGrowableCol( 0 );
	fgSizer17->AddGrowableCol( 1 );
	fgSizer17->AddGrowableCol( 2 );
	fgSizer17->SetFlexibleDirection( wxBOTH );
	fgSizer17->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_ALL );
	
	m_F1 = new LEDButton(this, wxT("F1"), 50, 25);
	fgSizer17->Add( m_F1, 0, wxALL|wxEXPAND, 1 );
	
	m_F2 = new LEDButton(this, wxT("F2"), 50, 25);
	fgSizer17->Add( m_F2, 0, wxALL|wxEXPAND, 1 );
	
	m_F3 = new LEDButton(this, wxT("F3"), 50, 25);
	fgSizer17->Add( m_F3, 0, wxALL|wxEXPAND, 1 );
	
	m_F4 = new LEDButton(this, wxT("F4"), 50, 25);
	fgSizer17->Add( m_F4, 0, wxALL|wxEXPAND, 1 );
	
	m_F5 = new LEDButton(this, wxT("F5"), 50, 25);
	fgSizer17->Add( m_F5, 0, wxALL|wxEXPAND, 1 );
	
	m_F6 = new LEDButton(this, wxT("F6"), 50, 25);
	fgSizer17->Add( m_F6, 0, wxALL|wxEXPAND, 1 );
	
	m_F7 = new LEDButton(this, wxT("F7"), 50, 25);
	fgSizer17->Add( m_F7, 0, wxALL|wxEXPAND, 1 );
	
	m_F8 = new LEDButton(this, wxT("F8"), 50, 25);
	fgSizer17->Add( m_F8, 0, wxALL|wxEXPAND, 1 );
	
	m_F9 = new LEDButton(this, wxT("F9"), 50, 25);
	fgSizer17->Add( m_F9, 0, wxALL|wxEXPAND, 1 );
	
	m_F10 = new LEDButton(this, wxT("F10"), 50, 25);
	fgSizer17->Add( m_F10, 0, wxALL|wxEXPAND, 1 );
	
	m_F11 = new LEDButton(this, wxT("F11"), 50, 25);
	fgSizer17->Add( m_F11, 0, wxALL|wxEXPAND, 1 );
	
	m_F12 = new LEDButton(this, wxT("F12"), 50, 25);
	fgSizer17->Add( m_F12, 0, wxALL|wxEXPAND, 1 );
	
	m_F13 = new LEDButton(this, wxT("F13"), 50, 25);
	fgSizer17->Add( m_F13, 0, wxALL|wxEXPAND, 1 );
	
	m_F14 = new LEDButton(this, wxT("F14"), 50, 25);
	fgSizer17->Add( m_F14, 0, wxALL|wxEXPAND, 1 );
	
	m_F0 = new LEDButton(this, wxT("Lights"), 50, 25);
	fgSizer17->Add( m_F0, 0, wxALL|wxEXPAND, 1 );
	
	bSizer24->Add( fgSizer17, 0, wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer19;
	fgSizer19 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer19->AddGrowableCol( 1 );
	fgSizer19->SetFlexibleDirection( wxBOTH );
	fgSizer19->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxBoxSizer* bSizer25;
	bSizer25 = new wxBoxSizer( wxVERTICAL );
	
	m_FG = new LEDButton(this, wxT("FG"), 82, 25, false);
	bSizer25->Add( m_FG, 0, wxALL|wxEXPAND, 1 );
	
	m_Stop = new LEDButton(this, wxT("STOP"), 82, 25, false);
	bSizer25->Add( m_Stop, 0, wxALL|wxEXPAND, 1 );
	
	fgSizer19->Add( bSizer25, 0, wxEXPAND, 5 );
	
	m_Dir = new LEDButton(this, wxT(">>"), 70, 52, false);
	fgSizer19->Add( m_Dir, 0, wxALL|wxEXPAND, 1 );
	
	bSizer24->Add( fgSizer19, 0, wxEXPAND, 5 );
	
	m_Break = new LEDButton(this, wxT("BREAK"), 154, 25, false);
	bSizer24->Add( m_Break, 0, wxALL|wxEXPAND, 1 );
	
	fgSizer18->Add( bSizer24, 0, wxEXPAND, 5 );
	
	bSizer21->Add( fgSizer18, 0, wxBOTTOM|wxRIGHT|wxLEFT|wxEXPAND, 3 );
	
	this->SetSizer( bSizer21 );
	this->Layout();
	bSizer21->Fit( this );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( ThrottleDlgGen::onClose ) );
	m_BinState->Connect( wxEVT_LEFT_DCLICK, wxMouseEventHandler( ThrottleDlgGen::onBinState ), NULL, this );
}

ThrottleDlgGen::~ThrottleDlgGen()
{
	// Disconnect Events
	this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( ThrottleDlgGen::onClose ) );
	m_BinState->Disconnect( wxEVT_LEFT_DCLICK, wxMouseEventHandler( ThrottleDlgGen::onBinState ), NULL, this );
	
}
