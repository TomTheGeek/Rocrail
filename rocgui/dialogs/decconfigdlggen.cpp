///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 21 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "decconfigdlggen.h"

///////////////////////////////////////////////////////////////////////////

decconfigdlggen::decconfigdlggen( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_Direction = new wxCheckBox( this, wxID_ANY, wxT("Invert locomotive direction"), wxDefaultPosition, wxDefaultSize, 0 );
	
	bSizer1->Add( m_Direction, 0, wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_SpeedSteps = new wxCheckBox( this, wxID_ANY, wxT("28/128 Speed steps"), wxDefaultPosition, wxDefaultSize, 0 );
	
	bSizer1->Add( m_SpeedSteps, 0, wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_DCOperation = new wxCheckBox( this, wxID_ANY, wxT("DC Operation support"), wxDefaultPosition, wxDefaultSize, 0 );
	
	bSizer1->Add( m_DCOperation, 0, wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_UserSpeedCurve = new wxCheckBox( this, wxID_ANY, wxT("User defined speed curve"), wxDefaultPosition, wxDefaultSize, 0 );
	
	bSizer1->Add( m_UserSpeedCurve, 0, wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_ExtAddressing = new wxCheckBox( this, wxID_ANY, wxT("Extended addressing"), wxDefaultPosition, wxDefaultSize, 0 );
	
	bSizer1->Add( m_ExtAddressing, 0, wxALL, 5 );
	
	m_stdButton = new wxStdDialogButtonSizer();
	m_stdButtonOK = new wxButton( this, wxID_OK );
	m_stdButton->AddButton( m_stdButtonOK );
	m_stdButtonCancel = new wxButton( this, wxID_CANCEL );
	m_stdButton->AddButton( m_stdButtonCancel );
	m_stdButton->Realize();
	bSizer1->Add( m_stdButton, 0, wxEXPAND|wxALL, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	bSizer1->Fit( this );
	
	// Connect Events
	m_stdButtonCancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( decconfigdlggen::onCancel ), NULL, this );
	m_stdButtonOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( decconfigdlggen::onOK ), NULL, this );
}

decconfigdlggen::~decconfigdlggen()
{
	// Disconnect Events
	m_stdButtonCancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( decconfigdlggen::onCancel ), NULL, this );
	m_stdButtonOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( decconfigdlggen::onOK ), NULL, this );
}
