///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 17 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "dcc232dlggen.h"

///////////////////////////////////////////////////////////////////////////

dcc232gen::dcc232gen( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer2->AddGrowableCol( 1 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_labIID = new wxStaticText( this, wxID_ANY, wxT("IID"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labIID->Wrap( -1 );
	fgSizer2->Add( m_labIID, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_IID = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_IID, 0, wxALL|wxEXPAND, 5 );
	
	bSizer1->Add( fgSizer2, 1, wxEXPAND|wxRIGHT|wxLEFT, 5 );
	
	wxStaticBoxSizer* m_PortBox;
	m_PortBox = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Port") ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 1, 2, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_labDevice = new wxStaticText( this, wxID_ANY, wxT("Device"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labDevice->Wrap( -1 );
	fgSizer1->Add( m_labDevice, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_Device = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 200,-1 ), 0 );
	fgSizer1->Add( m_Device, 0, wxALL, 5 );
	
	m_labOffset = new wxStaticText( this, wxID_ANY, wxT("Offset"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labOffset->Wrap( -1 );
	fgSizer1->Add( m_labOffset, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_Offset = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_Offset, 0, wxALL, 5 );
	
	m_PortBox->Add( fgSizer1, 1, wxEXPAND, 5 );
	
	bSizer1->Add( m_PortBox, 0, wxEXPAND|wxTOP|wxRIGHT|wxLEFT, 5 );
	
	wxStaticBoxSizer* m_SlotBox;
	m_SlotBox = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Slot Management") ), wxVERTICAL );
	
	m_AutoPurge = new wxCheckBox( this, wxID_ANY, wxT("Auto purging"), wxDefaultPosition, wxDefaultSize, 0 );
	
	m_SlotBox->Add( m_AutoPurge, 0, wxALL, 5 );
	
	wxFlexGridSizer* fgSizer3;
	fgSizer3 = new wxFlexGridSizer( 2, 3, 0, 0 );
	fgSizer3->SetFlexibleDirection( wxBOTH );
	fgSizer3->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_labPurgeTime = new wxStaticText( this, wxID_ANY, wxT("Purge time"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labPurgeTime->Wrap( -1 );
	fgSizer3->Add( m_labPurgeTime, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_PurgeTime = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 100, 10 );
	fgSizer3->Add( m_PurgeTime, 0, wxALL, 5 );
	
	m_labPurgetimeSec = new wxStaticText( this, wxID_ANY, wxT("seconds"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labPurgetimeSec->Wrap( -1 );
	fgSizer3->Add( m_labPurgetimeSec, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM|wxRIGHT, 5 );
	
	m_SlotBox->Add( fgSizer3, 1, wxEXPAND, 5 );
	
	bSizer1->Add( m_SlotBox, 0, wxEXPAND|wxTOP|wxRIGHT|wxLEFT, 5 );
	
	wxStaticBoxSizer* m_ShortCutBox;
	m_ShortCutBox = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Shortcut detection") ), wxVERTICAL );
	
	m_ShortcutDetection = new wxCheckBox( this, wxID_ANY, wxT("Activated"), wxDefaultPosition, wxDefaultSize, 0 );
	
	m_ShortCutBox->Add( m_ShortcutDetection, 0, wxALL, 5 );
	
	wxFlexGridSizer* fgSizer4;
	fgSizer4 = new wxFlexGridSizer( 2, 3, 0, 0 );
	fgSizer4->SetFlexibleDirection( wxBOTH );
	fgSizer4->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_labShortcutDelay = new wxStaticText( this, wxID_ANY, wxT("Delay"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labShortcutDelay->Wrap( -1 );
	fgSizer4->Add( m_labShortcutDelay, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_ShortcutDelay = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 10, 10000, 1000 );
	fgSizer4->Add( m_ShortcutDelay, 0, wxALL, 5 );
	
	m_labShortcutDelayMs = new wxStaticText( this, wxID_ANY, wxT("ms"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labShortcutDelayMs->Wrap( -1 );
	fgSizer4->Add( m_labShortcutDelayMs, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM|wxRIGHT, 5 );
	
	m_ShortCutBox->Add( fgSizer4, 1, wxEXPAND, 5 );
	
	bSizer1->Add( m_ShortCutBox, 0, wxEXPAND|wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_DefaultButtons = new wxStdDialogButtonSizer();
	m_DefaultButtonsOK = new wxButton( this, wxID_OK );
	m_DefaultButtons->AddButton( m_DefaultButtonsOK );
	m_DefaultButtonsCancel = new wxButton( this, wxID_CANCEL );
	m_DefaultButtons->AddButton( m_DefaultButtonsCancel );
	m_DefaultButtons->Realize();
	bSizer1->Add( m_DefaultButtons, 1, wxEXPAND|wxALL, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	bSizer1->Fit( this );
}

dcc232gen::~dcc232gen()
{
}
