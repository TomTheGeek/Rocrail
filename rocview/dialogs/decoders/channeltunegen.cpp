///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep 12 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "channeltunegen.h"

///////////////////////////////////////////////////////////////////////////

ChannelTuneDlgGen::ChannelTuneDlgGen( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer20;
	bSizer20 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer21;
	bSizer21 = new wxBoxSizer( wxHORIZONTAL );
	
	m_RangeBox = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Range") ), wxVERTICAL );
	
	m_MaxRange = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 80,-1 ), wxSP_ARROW_KEYS, 0, 4095, 4095 );
	m_RangeBox->Add( m_MaxRange, 0, wxALL, 5 );
	
	m_MinRange = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 80,-1 ), wxSP_ARROW_KEYS, 0, 4095, 0 );
	m_RangeBox->Add( m_MinRange, 0, wxALL, 5 );
	
	wxString m_RangePresetChoices[] = { wxT("Servo"), wxT("Maximal"), wxT("Manual") };
	int m_RangePresetNChoices = sizeof( m_RangePresetChoices ) / sizeof( wxString );
	m_RangePreset = new wxRadioBox( this, wxID_ANY, wxT("Preset"), wxDefaultPosition, wxDefaultSize, m_RangePresetNChoices, m_RangePresetChoices, 1, wxRA_SPECIFY_COLS );
	m_RangePreset->SetSelection( 0 );
	m_RangeBox->Add( m_RangePreset, 0, wxALL, 5 );
	
	bSizer21->Add( m_RangeBox, 1, wxEXPAND|wxALL, 5 );
	
	wxBoxSizer* bSizer22;
	bSizer22 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer23;
	bSizer23 = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* bSizer24;
	bSizer24 = new wxBoxSizer( wxVERTICAL );
	
	m_labOffPos = new wxStaticText( this, wxID_ANY, wxT("Off"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labOffPos->Wrap( -1 );
	bSizer24->Add( m_labOffPos, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
	
	m_OffPos = new wxSlider( this, wxID_ANY, 2050, 0, 4095, wxDefaultPosition, wxDefaultSize, wxSL_INVERSE|wxSL_VERTICAL );
	bSizer24->Add( m_OffPos, 1, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
	
	m_OffPosValue = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 60,-1 ), wxTE_CENTRE|wxTE_READONLY );
	bSizer24->Add( m_OffPosValue, 0, wxTOP|wxBOTTOM|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	bSizer23->Add( bSizer24, 1, wxEXPAND|wxRIGHT|wxLEFT, 5 );
	
	wxBoxSizer* bSizer26;
	bSizer26 = new wxBoxSizer( wxVERTICAL );
	
	m_labOnPos = new wxStaticText( this, wxID_ANY, wxT("On"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labOnPos->Wrap( -1 );
	bSizer26->Add( m_labOnPos, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
	
	m_OnPos = new wxSlider( this, wxID_ANY, 2050, 0, 4095, wxDefaultPosition, wxDefaultSize, wxSL_INVERSE|wxSL_VERTICAL );
	bSizer26->Add( m_OnPos, 1, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
	
	m_OnPosValue = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 60,-1 ), wxTE_CENTRE|wxTE_READONLY );
	bSizer26->Add( m_OnPosValue, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP|wxBOTTOM, 5 );
	
	bSizer23->Add( bSizer26, 1, wxEXPAND|wxRIGHT, 5 );
	
	bSizer22->Add( bSizer23, 1, 0, 5 );
	
	bSizer21->Add( bSizer22, 0, wxEXPAND, 5 );
	
	bSizer20->Add( bSizer21, 1, wxEXPAND, 5 );
	
	m_StdButton = new wxStdDialogButtonSizer();
	m_StdButtonOK = new wxButton( this, wxID_OK );
	m_StdButton->AddButton( m_StdButtonOK );
	m_StdButtonHelp = new wxButton( this, wxID_HELP );
	m_StdButton->AddButton( m_StdButtonHelp );
	m_StdButton->Realize();
	bSizer20->Add( m_StdButton, 0, wxALL|wxALIGN_RIGHT|wxEXPAND, 5 );
	
	this->SetSizer( bSizer20 );
	this->Layout();
	bSizer20->Fit( this );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_MaxRange->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( ChannelTuneDlgGen::onMaxRange ), NULL, this );
	m_MinRange->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( ChannelTuneDlgGen::onMinRange ), NULL, this );
	m_RangePreset->Connect( wxEVT_COMMAND_RADIOBOX_SELECTED, wxCommandEventHandler( ChannelTuneDlgGen::onPreset ), NULL, this );
	m_OffPos->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( ChannelTuneDlgGen::onOffPos ), NULL, this );
	m_OffPos->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( ChannelTuneDlgGen::onOffPos ), NULL, this );
	m_OffPos->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( ChannelTuneDlgGen::onOffPos ), NULL, this );
	m_OffPos->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( ChannelTuneDlgGen::onOffPos ), NULL, this );
	m_OffPos->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( ChannelTuneDlgGen::onOffPos ), NULL, this );
	m_OffPos->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( ChannelTuneDlgGen::onOffPos ), NULL, this );
	m_OffPos->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( ChannelTuneDlgGen::onOffPos ), NULL, this );
	m_OffPos->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( ChannelTuneDlgGen::onOffPos ), NULL, this );
	m_OffPos->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( ChannelTuneDlgGen::onOffPos ), NULL, this );
	m_OffPos->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( ChannelTuneDlgGen::onOffPosRelease ), NULL, this );
	m_OnPos->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( ChannelTuneDlgGen::onOnPos ), NULL, this );
	m_OnPos->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( ChannelTuneDlgGen::onOnPos ), NULL, this );
	m_OnPos->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( ChannelTuneDlgGen::onOnPos ), NULL, this );
	m_OnPos->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( ChannelTuneDlgGen::onOnPos ), NULL, this );
	m_OnPos->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( ChannelTuneDlgGen::onOnPos ), NULL, this );
	m_OnPos->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( ChannelTuneDlgGen::onOnPos ), NULL, this );
	m_OnPos->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( ChannelTuneDlgGen::onOnPos ), NULL, this );
	m_OnPos->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( ChannelTuneDlgGen::onOnPos ), NULL, this );
	m_OnPos->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( ChannelTuneDlgGen::onOnPos ), NULL, this );
	m_OnPos->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( ChannelTuneDlgGen::onOnPosRelease ), NULL, this );
	m_StdButtonHelp->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ChannelTuneDlgGen::onHelp ), NULL, this );
	m_StdButtonOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ChannelTuneDlgGen::onOK ), NULL, this );
}

ChannelTuneDlgGen::~ChannelTuneDlgGen()
{
	// Disconnect Events
	m_MaxRange->Disconnect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( ChannelTuneDlgGen::onMaxRange ), NULL, this );
	m_MinRange->Disconnect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( ChannelTuneDlgGen::onMinRange ), NULL, this );
	m_RangePreset->Disconnect( wxEVT_COMMAND_RADIOBOX_SELECTED, wxCommandEventHandler( ChannelTuneDlgGen::onPreset ), NULL, this );
	m_OffPos->Disconnect( wxEVT_SCROLL_TOP, wxScrollEventHandler( ChannelTuneDlgGen::onOffPos ), NULL, this );
	m_OffPos->Disconnect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( ChannelTuneDlgGen::onOffPos ), NULL, this );
	m_OffPos->Disconnect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( ChannelTuneDlgGen::onOffPos ), NULL, this );
	m_OffPos->Disconnect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( ChannelTuneDlgGen::onOffPos ), NULL, this );
	m_OffPos->Disconnect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( ChannelTuneDlgGen::onOffPos ), NULL, this );
	m_OffPos->Disconnect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( ChannelTuneDlgGen::onOffPos ), NULL, this );
	m_OffPos->Disconnect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( ChannelTuneDlgGen::onOffPos ), NULL, this );
	m_OffPos->Disconnect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( ChannelTuneDlgGen::onOffPos ), NULL, this );
	m_OffPos->Disconnect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( ChannelTuneDlgGen::onOffPos ), NULL, this );
	m_OffPos->Disconnect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( ChannelTuneDlgGen::onOffPosRelease ), NULL, this );
	m_OnPos->Disconnect( wxEVT_SCROLL_TOP, wxScrollEventHandler( ChannelTuneDlgGen::onOnPos ), NULL, this );
	m_OnPos->Disconnect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( ChannelTuneDlgGen::onOnPos ), NULL, this );
	m_OnPos->Disconnect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( ChannelTuneDlgGen::onOnPos ), NULL, this );
	m_OnPos->Disconnect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( ChannelTuneDlgGen::onOnPos ), NULL, this );
	m_OnPos->Disconnect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( ChannelTuneDlgGen::onOnPos ), NULL, this );
	m_OnPos->Disconnect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( ChannelTuneDlgGen::onOnPos ), NULL, this );
	m_OnPos->Disconnect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( ChannelTuneDlgGen::onOnPos ), NULL, this );
	m_OnPos->Disconnect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( ChannelTuneDlgGen::onOnPos ), NULL, this );
	m_OnPos->Disconnect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( ChannelTuneDlgGen::onOnPos ), NULL, this );
	m_OnPos->Disconnect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( ChannelTuneDlgGen::onOnPosRelease ), NULL, this );
	m_StdButtonHelp->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ChannelTuneDlgGen::onHelp ), NULL, this );
	m_StdButtonOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ChannelTuneDlgGen::onOK ), NULL, this );
	
}
