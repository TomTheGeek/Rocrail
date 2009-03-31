///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 17 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "fxdlggen.h"

///////////////////////////////////////////////////////////////////////////

fxdlggen::fxdlggen( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxVERTICAL );
	
	wxString m_FunctionChoices[] = { wxT("F0 Fwd"), wxT("F0 Rev"), wxT("F1"), wxT("F2"), wxT("F3"), wxT("F4"), wxT("F5"), wxT("F6") };
	int m_FunctionNChoices = sizeof( m_FunctionChoices ) / sizeof( wxString );
	m_Function = new wxRadioBox( this, wxID_ANY, wxT("Function"), wxDefaultPosition, wxDefaultSize, m_FunctionNChoices, m_FunctionChoices, 1, wxRA_SPECIFY_ROWS );
	m_Function->SetSelection( 0 );
	bSizer8->Add( m_Function, 0, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxHORIZONTAL );
	
	wxString m_GenerationChoices[] = { wxT("Normal"), wxT("Random flicker"), wxT("Mars light"), wxT("Flashing head light"), wxT("Single pulse strobe"), wxT("Double pulse strobe"), wxT("Rotary beacon simulation"), wxT("Gyralite"), wxT("Rule 17 dimmable headlight"), wxT("FRED"), wxT("Right ditch light"), wxT("Left ditch light") };
	int m_GenerationNChoices = sizeof( m_GenerationChoices ) / sizeof( wxString );
	m_Generation = new wxRadioBox( this, wxID_ANY, wxT("Effect"), wxDefaultPosition, wxDefaultSize, m_GenerationNChoices, m_GenerationChoices, 1, wxRA_SPECIFY_COLS );
	m_Generation->SetSelection( 4 );
	bSizer9->Add( m_Generation, 0, wxALL, 5 );
	
	wxString m_WorkChoices[] = { wxT("Forward direction phase A"), wxT("Reverse direction phase B"), wxT("Non directional phase A"), wxT("Non directional phase B"), wxT("Forward direction, F0 on, phase A"), wxT("Reverse direction, F0 on, phase B"), wxT("Forwards ditch light or Rule 17"), wxT("Reverse ditch light or Rule 17"), wxT("Speed=0, non directional, phase A"), wxT("Speed>0, non directional, phase B"), wxT("Ditch off phase A"), wxT("Ditch off phase B") };
	int m_WorkNChoices = sizeof( m_WorkChoices ) / sizeof( wxString );
	m_Work = new wxRadioBox( this, wxID_ANY, wxT("Activation"), wxDefaultPosition, wxDefaultSize, m_WorkNChoices, m_WorkChoices, 1, wxRA_SPECIFY_COLS );
	m_Work->SetSelection( 2 );
	bSizer9->Add( m_Work, 0, wxALL, 5 );
	
	bSizer8->Add( bSizer9, 1, wxEXPAND, 5 );
	
	m_sdButtons = new wxStdDialogButtonSizer();
	m_sdButtonsOK = new wxButton( this, wxID_OK );
	m_sdButtons->AddButton( m_sdButtonsOK );
	m_sdButtonsCancel = new wxButton( this, wxID_CANCEL );
	m_sdButtons->AddButton( m_sdButtonsCancel );
	m_sdButtons->Realize();
	bSizer8->Add( m_sdButtons, 0, wxEXPAND|wxALL, 5 );
	
	this->SetSizer( bSizer8 );
	this->Layout();
	bSizer8->Fit( this );
	
	// Connect Events
	m_Function->Connect( wxEVT_COMMAND_RADIOBOX_SELECTED, wxCommandEventHandler( fxdlggen::onFunction ), NULL, this );
	m_sdButtonsCancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( fxdlggen::onCancel ), NULL, this );
	m_sdButtonsOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( fxdlggen::onOK ), NULL, this );
}

fxdlggen::~fxdlggen()
{
	// Disconnect Events
	m_Function->Disconnect( wxEVT_COMMAND_RADIOBOX_SELECTED, wxCommandEventHandler( fxdlggen::onFunction ), NULL, this );
	m_sdButtonsCancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( fxdlggen::onCancel ), NULL, this );
	m_sdButtonsOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( fxdlggen::onOK ), NULL, this );
}
