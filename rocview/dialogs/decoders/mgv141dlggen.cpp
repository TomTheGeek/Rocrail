///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Dec 21 2009)
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
	
	m_NoteBook = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_QueryPanel = new wxPanel( m_NoteBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	m_AddressList = new wxListBox( m_QueryPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	bSizer4->Add( m_AddressList, 1, wxALL|wxEXPAND, 5 );
	
	m_Query = new wxButton( m_QueryPanel, wxID_ANY, wxT("Query"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_Query, 0, wxALL, 5 );
	
	m_QueryPanel->SetSizer( bSizer4 );
	m_QueryPanel->Layout();
	bSizer4->Fit( m_QueryPanel );
	m_NoteBook->AddPage( m_QueryPanel, wxT("Query"), false );
	m_SetupPanel = new wxPanel( m_NoteBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer3;
	fgSizer3 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer3->AddGrowableCol( 1 );
	fgSizer3->SetFlexibleDirection( wxBOTH );
	fgSizer3->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_labIID = new wxStaticText( m_SetupPanel, wxID_ANY, wxT("IID"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labIID->Wrap( -1 );
	fgSizer3->Add( m_labIID, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );
	
	m_IID = new wxTextCtrl( m_SetupPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer3->Add( m_IID, 0, wxALL|wxEXPAND, 5 );
	
	bSizer3->Add( fgSizer3, 0, wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 0, 4, 0, 0 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText1 = new wxStaticText( m_SetupPanel, wxID_ANY, wxT("Unit address"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	fgSizer1->Add( m_staticText1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_UnitHigh = new wxSpinCtrl( m_SetupPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 80,-1 ), wxSP_ARROW_KEYS, 0, 999, 0 );
	fgSizer1->Add( m_UnitHigh, 0, wxALL, 5 );
	
	m_UnitLow = new wxSpinCtrl( m_SetupPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 80,-1 ), wxSP_ARROW_KEYS, 0, 999, 0 );
	fgSizer1->Add( m_UnitLow, 0, wxALL, 5 );
	
	m_UnitSet = new wxButton( m_SetupPanel, wxID_ANY, wxT("Set"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_UnitSet, 0, wxALL, 5 );
	
	bSizer3->Add( fgSizer1, 1, wxEXPAND, 5 );
	
	wxStaticBoxSizer* m_CounterAddressesBox;
	m_CounterAddressesBox = new wxStaticBoxSizer( new wxStaticBox( m_SetupPanel, wxID_ANY, wxT("Counter Addresses") ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 0, 4, 0, 0 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_lab1 = new wxStaticText( m_SetupPanel, wxID_ANY, wxT("1"), wxDefaultPosition, wxDefaultSize, 0 );
	m_lab1->Wrap( -1 );
	fgSizer2->Add( m_lab1, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );
	
	m_CounterAddress1 = new wxSpinCtrl( m_SetupPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 100,-1 ), wxSP_ARROW_KEYS, 0, 9999, 0 );
	fgSizer2->Add( m_CounterAddress1, 0, wxALL, 5 );
	
	m_lab2 = new wxStaticText( m_SetupPanel, wxID_ANY, wxT("2"), wxDefaultPosition, wxDefaultSize, 0 );
	m_lab2->Wrap( -1 );
	fgSizer2->Add( m_lab2, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );
	
	m_CounterAddress2 = new wxSpinCtrl( m_SetupPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 100,-1 ), wxSP_ARROW_KEYS, 0, 9999, 0 );
	fgSizer2->Add( m_CounterAddress2, 0, wxALL, 5 );
	
	m_lab3 = new wxStaticText( m_SetupPanel, wxID_ANY, wxT("3"), wxDefaultPosition, wxDefaultSize, 0 );
	m_lab3->Wrap( -1 );
	fgSizer2->Add( m_lab3, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_CounterAddress3 = new wxSpinCtrl( m_SetupPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 100,-1 ), wxSP_ARROW_KEYS, 0, 9999, 0 );
	fgSizer2->Add( m_CounterAddress3, 0, wxALL, 5 );
	
	m_lab4 = new wxStaticText( m_SetupPanel, wxID_ANY, wxT("4"), wxDefaultPosition, wxDefaultSize, 0 );
	m_lab4->Wrap( -1 );
	fgSizer2->Add( m_lab4, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_CounterAddress4 = new wxSpinCtrl( m_SetupPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 100,-1 ), wxSP_ARROW_KEYS, 0, 9999, 0 );
	fgSizer2->Add( m_CounterAddress4, 0, wxALL, 5 );
	
	m_lab5 = new wxStaticText( m_SetupPanel, wxID_ANY, wxT("5"), wxDefaultPosition, wxDefaultSize, 0 );
	m_lab5->Wrap( -1 );
	fgSizer2->Add( m_lab5, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_CounterAddress5 = new wxSpinCtrl( m_SetupPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 100,-1 ), wxSP_ARROW_KEYS, 0, 9999, 0 );
	fgSizer2->Add( m_CounterAddress5, 0, wxALL, 5 );
	
	m_lab6 = new wxStaticText( m_SetupPanel, wxID_ANY, wxT("6"), wxDefaultPosition, wxDefaultSize, 0 );
	m_lab6->Wrap( -1 );
	fgSizer2->Add( m_lab6, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );
	
	m_CounterAddress6 = new wxSpinCtrl( m_SetupPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 100,-1 ), wxSP_ARROW_KEYS, 0, 9999, 0 );
	fgSizer2->Add( m_CounterAddress6, 0, wxALL, 5 );
	
	m_lab7 = new wxStaticText( m_SetupPanel, wxID_ANY, wxT("7"), wxDefaultPosition, wxDefaultSize, 0 );
	m_lab7->Wrap( -1 );
	fgSizer2->Add( m_lab7, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_CounterAddress7 = new wxSpinCtrl( m_SetupPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 100,-1 ), wxSP_ARROW_KEYS, 0, 9999, 0 );
	fgSizer2->Add( m_CounterAddress7, 0, wxALL, 5 );
	
	m_lab8 = new wxStaticText( m_SetupPanel, wxID_ANY, wxT("8"), wxDefaultPosition, wxDefaultSize, 0 );
	m_lab8->Wrap( -1 );
	fgSizer2->Add( m_lab8, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_CounterAddress8 = new wxSpinCtrl( m_SetupPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 100,-1 ), wxSP_ARROW_KEYS, 0, 9999, 0 );
	fgSizer2->Add( m_CounterAddress8, 0, wxALL, 5 );
	
	m_CounterAddressesBox->Add( fgSizer2, 1, wxEXPAND, 5 );
	
	bSizer3->Add( m_CounterAddressesBox, 0, wxEXPAND|wxRIGHT|wxLEFT, 5 );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	m_WriteAll = new wxButton( m_SetupPanel, wxID_ANY, wxT("Write"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_WriteAll, 0, wxALL, 5 );
	
	m_ReadAll = new wxButton( m_SetupPanel, wxID_ANY, wxT("Read"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_ReadAll, 0, wxALL, 5 );
	
	bSizer3->Add( bSizer2, 0, 0, 5 );
	
	m_SetupPanel->SetSizer( bSizer3 );
	m_SetupPanel->Layout();
	bSizer3->Fit( m_SetupPanel );
	m_NoteBook->AddPage( m_SetupPanel, wxT("Setup"), true );
	
	bSizer1->Add( m_NoteBook, 0, wxEXPAND | wxALL, 5 );
	
	m_StdButtons = new wxStdDialogButtonSizer();
	m_StdButtonsOK = new wxButton( this, wxID_OK );
	m_StdButtons->AddButton( m_StdButtonsOK );
	m_StdButtons->Realize();
	bSizer1->Add( m_StdButtons, 1, wxEXPAND|wxALL, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	bSizer1->Fit( this );
	
	// Connect Events
	m_AddressList->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( mgv141gen::onUnitSelected ), NULL, this );
	m_AddressList->Connect( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler( mgv141gen::onUnitSelected ), NULL, this );
	m_Query->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mgv141gen::onQuery ), NULL, this );
	m_UnitSet->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mgv141gen::onUnitSet ), NULL, this );
	m_WriteAll->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mgv141gen::onWriteAll ), NULL, this );
	m_ReadAll->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mgv141gen::onReadAll ), NULL, this );
	m_StdButtonsOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mgv141gen::onOK ), NULL, this );
}

mgv141gen::~mgv141gen()
{
	// Disconnect Events
	m_AddressList->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( mgv141gen::onUnitSelected ), NULL, this );
	m_AddressList->Disconnect( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler( mgv141gen::onUnitSelected ), NULL, this );
	m_Query->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mgv141gen::onQuery ), NULL, this );
	m_UnitSet->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mgv141gen::onUnitSet ), NULL, this );
	m_WriteAll->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mgv141gen::onWriteAll ), NULL, this );
	m_ReadAll->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mgv141gen::onReadAll ), NULL, this );
	m_StdButtonsOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( mgv141gen::onOK ), NULL, this );
}
