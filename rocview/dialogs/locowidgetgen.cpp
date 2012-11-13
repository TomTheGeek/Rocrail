///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "locowidgetgen.h"

///////////////////////////////////////////////////////////////////////////

LocoWidgetGen::LocoWidgetGen( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	this->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ) );
	this->SetMinSize( wxSize( 260,-1 ) );
	this->SetMaxSize( wxSize( 260,-1 ) );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 0, 3, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_Stop = new wxButton( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 25,25 ), 0 );
	fgSizer1->Add( m_Stop, 0, wxALIGN_BOTTOM|wxALL, 1 );
	
	m_LocoImage = new wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 120,30 ), 0 );
	fgSizer1->Add( m_LocoImage, 0, wxALIGN_BOTTOM|wxTOP|wxRIGHT, 1 );
	
	wxFlexGridSizer* fgSizer3;
	fgSizer3 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer3->AddGrowableCol( 0 );
	fgSizer3->AddGrowableCol( 1 );
	fgSizer3->SetFlexibleDirection( wxBOTH );
	fgSizer3->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_ID = new wxStaticText( this, wxID_ANY, wxT("ID=NS2404"), wxDefaultPosition, wxDefaultSize, 0 );
	m_ID->Wrap( -1 );
	fgSizer3->Add( m_ID, 0, wxEXPAND|wxALIGN_BOTTOM, 5 );
	
	fgSizer1->Add( fgSizer3, 0, wxALIGN_BOTTOM, 0 );
	
	bSizer2->Add( fgSizer1, 0, 0, 0 );
	
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 0, 7, 0, 0 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_Block = new wxStaticText( this, wxID_ANY, wxT("B=12"), wxDefaultPosition, wxDefaultSize, 0 );
	m_Block->Wrap( -1 );
	fgSizer2->Add( m_Block, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 2 );
	
	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL );
	fgSizer2->Add( m_staticline1, 0, wxEXPAND|wxRIGHT|wxLEFT, 5 );
	
	m_Speed = new wxStaticText( this, wxID_ANY, wxT("V=80"), wxDefaultPosition, wxDefaultSize, 0 );
	m_Speed->Wrap( -1 );
	fgSizer2->Add( m_Speed, 0, wxALIGN_CENTER_VERTICAL, 2 );
	
	m_staticline2 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL );
	fgSizer2->Add( m_staticline2, 0, wxRIGHT|wxLEFT|wxEXPAND, 5 );
	
	m_Mode = new wxStaticText( this, wxID_ANY, wxT("D=15"), wxDefaultPosition, wxDefaultSize, 0 );
	m_Mode->Wrap( -1 );
	fgSizer2->Add( m_Mode, 0, wxALIGN_CENTER_VERTICAL, 2 );
	
	m_staticline3 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL );
	fgSizer2->Add( m_staticline3, 0, wxEXPAND|wxRIGHT|wxLEFT, 5 );
	
	m_Destination = new wxStaticText( this, wxID_ANY, wxT("D=56"), wxDefaultPosition, wxDefaultSize, 0 );
	m_Destination->Wrap( -1 );
	fgSizer2->Add( m_Destination, 0, wxALIGN_CENTER_VERTICAL, 2 );
	
	bSizer2->Add( fgSizer2, 0, 0, 0 );
	
	bSizer1->Add( bSizer2, 0, 0, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	
	// Connect Events
	this->Connect( wxEVT_MIDDLE_UP, wxMouseEventHandler( LocoWidgetGen::onMouseWheel ) );
	this->Connect( wxEVT_MOUSEWHEEL, wxMouseEventHandler( LocoWidgetGen::onMouseWheel ) );
	m_Stop->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LocoWidgetGen::onStop ), NULL, this );
	m_LocoImage->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( LocoWidgetGen::onImageLeftDown ), NULL, this );
	m_LocoImage->Connect( wxEVT_LEFT_UP, wxMouseEventHandler( LocoWidgetGen::onThrottle ), NULL, this );
	m_LocoImage->Connect( wxEVT_MIDDLE_UP, wxMouseEventHandler( LocoWidgetGen::onMouseWheel ), NULL, this );
	m_LocoImage->Connect( wxEVT_MOUSEWHEEL, wxMouseEventHandler( LocoWidgetGen::onMouseWheel ), NULL, this );
	m_LocoImage->Connect( wxEVT_RIGHT_UP, wxMouseEventHandler( LocoWidgetGen::onPopUp ), NULL, this );
	m_Speed->Connect( wxEVT_LEFT_UP, wxMouseEventHandler( LocoWidgetGen::onSpeedUp ), NULL, this );
	m_Speed->Connect( wxEVT_RIGHT_UP, wxMouseEventHandler( LocoWidgetGen::onSpeedDown ), NULL, this );
}

LocoWidgetGen::~LocoWidgetGen()
{
	// Disconnect Events
	this->Disconnect( wxEVT_MIDDLE_UP, wxMouseEventHandler( LocoWidgetGen::onMouseWheel ) );
	this->Disconnect( wxEVT_MOUSEWHEEL, wxMouseEventHandler( LocoWidgetGen::onMouseWheel ) );
	m_Stop->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( LocoWidgetGen::onStop ), NULL, this );
	m_LocoImage->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( LocoWidgetGen::onImageLeftDown ), NULL, this );
	m_LocoImage->Disconnect( wxEVT_LEFT_UP, wxMouseEventHandler( LocoWidgetGen::onThrottle ), NULL, this );
	m_LocoImage->Disconnect( wxEVT_MIDDLE_UP, wxMouseEventHandler( LocoWidgetGen::onMouseWheel ), NULL, this );
	m_LocoImage->Disconnect( wxEVT_MOUSEWHEEL, wxMouseEventHandler( LocoWidgetGen::onMouseWheel ), NULL, this );
	m_LocoImage->Disconnect( wxEVT_RIGHT_UP, wxMouseEventHandler( LocoWidgetGen::onPopUp ), NULL, this );
	m_Speed->Disconnect( wxEVT_LEFT_UP, wxMouseEventHandler( LocoWidgetGen::onSpeedUp ), NULL, this );
	m_Speed->Disconnect( wxEVT_RIGHT_UP, wxMouseEventHandler( LocoWidgetGen::onSpeedDown ), NULL, this );
	
}
