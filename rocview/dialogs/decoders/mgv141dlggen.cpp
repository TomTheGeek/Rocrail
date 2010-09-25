///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 17 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "mgv141dlggen.h"

///////////////////////////////////////////////////////////////////////////

mgv141gen::mgv141gen( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 2, 4, 0, 0 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, wxT("Unit address"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	fgSizer1->Add( m_staticText1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_UnitHigh = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 80,-1 ), wxSP_ARROW_KEYS, 0, 999, 0 );
	fgSizer1->Add( m_UnitHigh, 0, wxALL, 5 );
	
	m_UnitLow = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 80,-1 ), wxSP_ARROW_KEYS, 0, 999, 0 );
	fgSizer1->Add( m_UnitLow, 0, wxALL, 5 );
	
	m_UnitSet = new wxButton( this, wxID_ANY, wxT("Set"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_UnitSet, 0, wxALL, 5 );
	
	bSizer1->Add( fgSizer1, 1, wxEXPAND, 5 );
	
	wxStaticBoxSizer* m_CounterAddressesBox;
	m_CounterAddressesBox = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Counter Addresses") ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 2, 4, 0, 0 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_lab1 = new wxStaticText( this, wxID_ANY, wxT("1"), wxDefaultPosition, wxDefaultSize, 0 );
	m_lab1->Wrap( -1 );
	fgSizer2->Add( m_lab1, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );
	
	m_CounterAddress1 = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 100,-1 ), wxSP_ARROW_KEYS, 0, 9999, 0 );
	fgSizer2->Add( m_CounterAddress1, 0, wxALL, 5 );
	
	m_lab2 = new wxStaticText( this, wxID_ANY, wxT("2"), wxDefaultPosition, wxDefaultSize, 0 );
	m_lab2->Wrap( -1 );
	fgSizer2->Add( m_lab2, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );
	
	m_CounterAddress2 = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 100,-1 ), wxSP_ARROW_KEYS, 0, 10, 0 );
	fgSizer2->Add( m_CounterAddress2, 0, wxALL, 5 );
	
	m_lab3 = new wxStaticText( this, wxID_ANY, wxT("3"), wxDefaultPosition, wxDefaultSize, 0 );
	m_lab3->Wrap( -1 );
	fgSizer2->Add( m_lab3, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_CounterAddress3 = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 100,-1 ), wxSP_ARROW_KEYS, 0, 9999, 0 );
	fgSizer2->Add( m_CounterAddress3, 0, wxALL, 5 );
	
	m_lab4 = new wxStaticText( this, wxID_ANY, wxT("4"), wxDefaultPosition, wxDefaultSize, 0 );
	m_lab4->Wrap( -1 );
	fgSizer2->Add( m_lab4, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_CounterAddress4 = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 100,-1 ), wxSP_ARROW_KEYS, 0, 9999, 0 );
	fgSizer2->Add( m_CounterAddress4, 0, wxALL, 5 );
	
	m_lab5 = new wxStaticText( this, wxID_ANY, wxT("5"), wxDefaultPosition, wxDefaultSize, 0 );
	m_lab5->Wrap( -1 );
	fgSizer2->Add( m_lab5, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_CounterAddress5 = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 100,-1 ), wxSP_ARROW_KEYS, 0, 9999, 0 );
	fgSizer2->Add( m_CounterAddress5, 0, wxALL, 5 );
	
	m_lab6 = new wxStaticText( this, wxID_ANY, wxT("6"), wxDefaultPosition, wxDefaultSize, 0 );
	m_lab6->Wrap( -1 );
	fgSizer2->Add( m_lab6, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );
	
	m_CounterAddress6 = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 100,-1 ), wxSP_ARROW_KEYS, 0, 9999, 0 );
	fgSizer2->Add( m_CounterAddress6, 0, wxALL, 5 );
	
	m_lab7 = new wxStaticText( this, wxID_ANY, wxT("7"), wxDefaultPosition, wxDefaultSize, 0 );
	m_lab7->Wrap( -1 );
	fgSizer2->Add( m_lab7, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_CounterAddress7 = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 100,-1 ), wxSP_ARROW_KEYS, 0, 9999, 0 );
	fgSizer2->Add( m_CounterAddress7, 0, wxALL, 5 );
	
	m_lab8 = new wxStaticText( this, wxID_ANY, wxT("8"), wxDefaultPosition, wxDefaultSize, 0 );
	m_lab8->Wrap( -1 );
	fgSizer2->Add( m_lab8, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_CounterAddress8 = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 100,-1 ), wxSP_ARROW_KEYS, 0, 9999, 0 );
	fgSizer2->Add( m_CounterAddress8, 0, wxALL, 5 );
	
	m_CounterAddressesBox->Add( fgSizer2, 1, wxEXPAND, 5 );
	
	bSizer1->Add( m_CounterAddressesBox, 0, wxEXPAND|wxRIGHT|wxLEFT, 5 );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	m_WriteAll = new wxButton( this, wxID_ANY, wxT("Write"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_WriteAll, 0, wxALL, 5 );
	
	m_ReadAll = new wxButton( this, wxID_ANY, wxT("Read"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_ReadAll, 0, wxALL, 5 );
	
	bSizer1->Add( bSizer2, 0, 0, 5 );
	
	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer1->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );
	
	m_StdButtons = new wxStdDialogButtonSizer();
	m_StdButtonsOK = new wxButton( this, wxID_OK );
	m_StdButtons->AddButton( m_StdButtonsOK );
	m_StdButtonsCancel = new wxButton( this, wxID_CANCEL );
	m_StdButtons->AddButton( m_StdButtonsCancel );
	m_StdButtons->Realize();
	bSizer1->Add( m_StdButtons, 1, wxEXPAND, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	bSizer1->Fit( this );
	
	// Connect Events
	m_UnitSet->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mgv141gen::onUnitSet ), NULL, this );
	m_WriteAll->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mgv141gen::onWriteAll ), NULL, this );
	m_ReadAll->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mgv141gen::onReadAll ), NULL, this );
	m_StdButtonsCancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mgv141gen::onCancel ), NULL, this );
	m_StdButtonsOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mgv141gen::onOK ), NULL, this );
}

mgv141gen::~mgv141gen()
{
	// Disconnect Events
	m_UnitSet->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mgv141gen::onUnitSet ), NULL, this );
	m_WriteAll->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mgv141gen::onWriteAll ), NULL, this );
	m_ReadAll->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mgv141gen::onReadAll ), NULL, this );
	m_StdButtonsCancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mgv141gen::onCancel ), NULL, this );
	m_StdButtonsOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mgv141gen::onOK ), NULL, this );
}
