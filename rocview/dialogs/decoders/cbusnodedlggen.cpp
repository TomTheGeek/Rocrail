///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Dec 21 2009)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "cbusnodedlggen.h"

///////////////////////////////////////////////////////////////////////////

cbusnodedlggen::cbusnodedlggen( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_NoteBook = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_NodeNumberPanel = new wxPanel( m_NoteBook, wxID_CBUSNODE_NUMBER, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer2->AddGrowableCol( 1 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_labIID = new wxStaticText( m_NodeNumberPanel, wxID_ANY, wxT("IID"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labIID->Wrap( -1 );
	fgSizer2->Add( m_labIID, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );
	
	m_IID = new wxTextCtrl( m_NodeNumberPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_IID, 0, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_labNodeType = new wxStaticText( m_NodeNumberPanel, wxID_ANY, wxT("Type"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labNodeType->Wrap( -1 );
	fgSizer2->Add( m_labNodeType, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );
	
	m_NodeType = new wxTextCtrl( m_NodeNumberPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	fgSizer2->Add( m_NodeType, 0, wxALL|wxEXPAND, 5 );
	
	bSizer3->Add( fgSizer2, 0, wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 0, 3, 0, 0 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_labNumber = new wxStaticText( m_NodeNumberPanel, wxID_ANY, wxT("Number"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labNumber->Wrap( -1 );
	fgSizer1->Add( m_labNumber, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_NodeNumber = new wxSpinCtrl( m_NodeNumberPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 100,-1 ), wxSP_ARROW_KEYS, 0, 65535, 0 );
	fgSizer1->Add( m_NodeNumber, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_SetNodeNumber = new wxButton( m_NodeNumberPanel, wxID_ANY, wxT("Set"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_SetNodeNumber, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	bSizer3->Add( fgSizer1, 0, wxEXPAND, 5 );
	
	m_NodeNumberPanel->SetSizer( bSizer3 );
	m_NodeNumberPanel->Layout();
	bSizer3->Fit( m_NodeNumberPanel );
	m_NoteBook->AddPage( m_NodeNumberPanel, wxT("Node"), false );
	m_IndexPanel = new wxPanel( m_NoteBook, wxID_CBUSNODE_INDEX, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );
	
	m_listBox1 = new wxListBox( m_IndexPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	m_listBox1->SetMinSize( wxSize( -1,150 ) );
	
	bSizer5->Add( m_listBox1, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxHORIZONTAL );
	
	m_IndexDelete = new wxButton( m_IndexPanel, wxID_ANY, wxT("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer6->Add( m_IndexDelete, 0, wxALL, 5 );
	
	bSizer5->Add( bSizer6, 0, wxEXPAND, 5 );
	
	m_IndexPanel->SetSizer( bSizer5 );
	m_IndexPanel->Layout();
	bSizer5->Fit( m_IndexPanel );
	m_NoteBook->AddPage( m_IndexPanel, wxT("Index"), false );
	m_ACC8Panel = new wxPanel( m_NoteBook, wxID_CBUS_ACC8, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer31;
	bSizer31 = new wxBoxSizer( wxVERTICAL );
	
	m_ACC8Panel->SetSizer( bSizer31 );
	m_ACC8Panel->Layout();
	bSizer31->Fit( m_ACC8Panel );
	m_NoteBook->AddPage( m_ACC8Panel, wxT("ACC8"), false );
	m_ACE8CPanel = new wxPanel( m_NoteBook, wxID_CBUS_ACE8C, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer3;
	fgSizer3 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer3->SetFlexibleDirection( wxBOTH );
	fgSizer3->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_labACE8CNV1 = new wxStaticText( m_ACE8CPanel, wxID_ANY, wxT("NV#1"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labACE8CNV1->Wrap( -1 );
	fgSizer3->Add( m_labACE8CNV1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxHORIZONTAL );
	
	m_ACE8CInput0 = new wxCheckBox( m_ACE8CPanel, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer8->Add( m_ACE8CInput0, 0, wxTOP|wxBOTTOM|wxRIGHT, 5 );
	
	m_ACE8CInput1 = new wxCheckBox( m_ACE8CPanel, wxID_ANY, wxT("1"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer8->Add( m_ACE8CInput1, 0, wxTOP|wxBOTTOM|wxRIGHT, 5 );
	
	m_ACE8CInput2 = new wxCheckBox( m_ACE8CPanel, wxID_ANY, wxT("2"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer8->Add( m_ACE8CInput2, 0, wxTOP|wxBOTTOM|wxRIGHT, 5 );
	
	m_ACE8CInput3 = new wxCheckBox( m_ACE8CPanel, wxID_ANY, wxT("3"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer8->Add( m_ACE8CInput3, 0, wxTOP|wxBOTTOM|wxRIGHT, 5 );
	
	m_ACE8CInput4 = new wxCheckBox( m_ACE8CPanel, wxID_ANY, wxT("4"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer8->Add( m_ACE8CInput4, 0, wxTOP|wxBOTTOM|wxRIGHT, 5 );
	
	m_ACE8CInput5 = new wxCheckBox( m_ACE8CPanel, wxID_ANY, wxT("5"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer8->Add( m_ACE8CInput5, 0, wxTOP|wxBOTTOM|wxRIGHT, 5 );
	
	m_ACE8CInput6 = new wxCheckBox( m_ACE8CPanel, wxID_ANY, wxT("6"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer8->Add( m_ACE8CInput6, 0, wxTOP|wxBOTTOM|wxRIGHT, 5 );
	
	m_ACE8CInput7 = new wxCheckBox( m_ACE8CPanel, wxID_ANY, wxT("7"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer8->Add( m_ACE8CInput7, 0, wxTOP|wxBOTTOM|wxRIGHT, 5 );
	
	fgSizer3->Add( bSizer8, 0, wxEXPAND, 5 );
	
	bSizer4->Add( fgSizer3, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxHORIZONTAL );
	
	m_ACE8CGet = new wxButton( m_ACE8CPanel, wxID_ANY, wxT("Get"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer9->Add( m_ACE8CGet, 0, wxALL, 5 );
	
	m_ACE8CSet = new wxButton( m_ACE8CPanel, wxID_ANY, wxT("Set"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer9->Add( m_ACE8CSet, 0, wxALL, 5 );
	
	bSizer4->Add( bSizer9, 0, wxEXPAND, 5 );
	
	m_ACE8CPanel->SetSizer( bSizer4 );
	m_ACE8CPanel->Layout();
	bSizer4->Fit( m_ACE8CPanel );
	m_NoteBook->AddPage( m_ACE8CPanel, wxT("ACE8C"), false );
	m_CMDPanel = new wxPanel( m_NoteBook, wxID_CBUS_CMD, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxVERTICAL );
	
	m_CMDPanel->SetSizer( bSizer7 );
	m_CMDPanel->Layout();
	bSizer7->Fit( m_CMDPanel );
	m_NoteBook->AddPage( m_CMDPanel, wxT("CMD"), true );
	m_EventsPanel = new wxPanel( m_NoteBook, wxID_CBUS_EVENTSPANEL, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer( wxVERTICAL );
	
	m_EventsList = new wxListBox( m_EventsPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_SINGLE ); 
	bSizer10->Add( m_EventsList, 1, wxALL|wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer4;
	fgSizer4 = new wxFlexGridSizer( 0, 4, 0, 0 );
	fgSizer4->SetFlexibleDirection( wxBOTH );
	fgSizer4->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_labEventNode = new wxStaticText( m_EventsPanel, wxID_ANY, wxT("Node#"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labEventNode->Wrap( -1 );
	fgSizer4->Add( m_labEventNode, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_labEventAddr = new wxStaticText( m_EventsPanel, wxID_ANY, wxT("Address"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labEventAddr->Wrap( -1 );
	fgSizer4->Add( m_labEventAddr, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_labEventIndex = new wxStaticText( m_EventsPanel, wxID_ANY, wxT("Index"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labEventIndex->Wrap( -1 );
	fgSizer4->Add( m_labEventIndex, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_labEventVar = new wxStaticText( m_EventsPanel, wxID_ANY, wxT("Variable"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labEventVar->Wrap( -1 );
	fgSizer4->Add( m_labEventVar, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_EventNodeNr = new wxSpinCtrl( m_EventsPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 65535, 0 );
	fgSizer4->Add( m_EventNodeNr, 0, wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	m_EventAddress = new wxSpinCtrl( m_EventsPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 65535, 0 );
	fgSizer4->Add( m_EventAddress, 0, wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	m_EventIndex = new wxSpinCtrl( m_EventsPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 32, 0 );
	fgSizer4->Add( m_EventIndex, 0, wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	m_EventVar = new wxSpinCtrl( m_EventsPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 65535, 0 );
	fgSizer4->Add( m_EventVar, 0, wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	bSizer10->Add( fgSizer4, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer( wxHORIZONTAL );
	
	m_EventGetAll = new wxButton( m_EventsPanel, wxID_ANY, wxT("Get all"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer11->Add( m_EventGetAll, 0, wxALL, 5 );
	
	m_EventAdd = new wxButton( m_EventsPanel, wxID_ANY, wxT("Add"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer11->Add( m_EventAdd, 0, wxALL, 5 );
	
	m_EventDelete = new wxButton( m_EventsPanel, wxID_ANY, wxT("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer11->Add( m_EventDelete, 0, wxALL, 5 );
	
	bSizer10->Add( bSizer11, 0, wxEXPAND, 5 );
	
	m_EventsPanel->SetSizer( bSizer10 );
	m_EventsPanel->Layout();
	bSizer10->Fit( m_EventsPanel );
	m_NoteBook->AddPage( m_EventsPanel, wxT("Events"), false );
	
	bSizer1->Add( m_NoteBook, 1, wxEXPAND | wxALL, 5 );
	
	m_sdbSizer1 = new wxStdDialogButtonSizer();
	m_sdbSizer1OK = new wxButton( this, wxID_OK );
	m_sdbSizer1->AddButton( m_sdbSizer1OK );
	m_sdbSizer1->Realize();
	bSizer1->Add( m_sdbSizer1, 0, wxEXPAND|wxALL, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	bSizer1->Fit( this );
	
	// Connect Events
	m_SetNodeNumber->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onSetNodeNumber ), NULL, this );
	m_ACE8CGet->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onACE8CGet ), NULL, this );
	m_ACE8CSet->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onACE8CSet ), NULL, this );
	m_EventsList->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( cbusnodedlggen::onEventSelect ), NULL, this );
	m_EventGetAll->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onEventGetAll ), NULL, this );
	m_EventAdd->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onEventAdd ), NULL, this );
	m_EventDelete->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onEventDelete ), NULL, this );
	m_sdbSizer1OK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onOK ), NULL, this );
}

cbusnodedlggen::~cbusnodedlggen()
{
	// Disconnect Events
	m_SetNodeNumber->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onSetNodeNumber ), NULL, this );
	m_ACE8CGet->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onACE8CGet ), NULL, this );
	m_ACE8CSet->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onACE8CSet ), NULL, this );
	m_EventsList->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( cbusnodedlggen::onEventSelect ), NULL, this );
	m_EventGetAll->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onEventGetAll ), NULL, this );
	m_EventAdd->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onEventAdd ), NULL, this );
	m_EventDelete->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onEventDelete ), NULL, this );
	m_sdbSizer1OK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onOK ), NULL, this );
}
