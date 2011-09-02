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
	
	m_NoteBook = new wxNotebook( this, wxID_CBUSNODE_BOOK, wxDefaultPosition, wxDefaultSize, 0 );
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
	
	bSizer3->Add( fgSizer2, 0, wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer51;
	fgSizer51 = new wxFlexGridSizer( 0, 4, 0, 0 );
	fgSizer51->AddGrowableCol( 1 );
	fgSizer51->SetFlexibleDirection( wxBOTH );
	fgSizer51->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_labNodeType = new wxStaticText( m_NodeNumberPanel, wxID_ANY, wxT("Type"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labNodeType->Wrap( -1 );
	fgSizer51->Add( m_labNodeType, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );
	
	m_NodeType = new wxTextCtrl( m_NodeNumberPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	fgSizer51->Add( m_NodeType, 0, wxALL|wxEXPAND, 5 );
	
	m_NodeManuNr = new wxSpinCtrl( m_NodeNumberPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 255, 0 );
	m_NodeManuNr->Enable( false );
	
	fgSizer51->Add( m_NodeManuNr, 0, wxALL, 5 );
	
	m_NodeTypeNr = new wxSpinCtrl( m_NodeNumberPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 255, 0 );
	m_NodeTypeNr->Enable( false );
	
	fgSizer51->Add( m_NodeTypeNr, 0, wxALL, 5 );
	
	bSizer3->Add( fgSizer51, 0, wxEXPAND, 5 );
	
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
	
	m_IndexList = new wxListBox( m_IndexPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	m_IndexList->SetMinSize( wxSize( -1,150 ) );
	
	bSizer5->Add( m_IndexList, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxHORIZONTAL );
	
	m_IndexDelete = new wxButton( m_IndexPanel, wxID_ANY, wxT("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer6->Add( m_IndexDelete, 0, wxALL, 5 );
	
	bSizer5->Add( bSizer6, 0, wxEXPAND, 5 );
	
	m_IndexPanel->SetSizer( bSizer5 );
	m_IndexPanel->Layout();
	bSizer5->Fit( m_IndexPanel );
	m_NoteBook->AddPage( m_IndexPanel, wxT("Index"), false );
	m_VarPanel = new wxPanel( m_NoteBook, wxID_CBUS_VAR, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	m_VarList = new wxListBox( m_VarPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_SINGLE ); 
	m_VarList->SetMinSize( wxSize( -1,150 ) );
	
	bSizer4->Add( m_VarList, 1, wxALL|wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer5;
	fgSizer5 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer5->SetFlexibleDirection( wxBOTH );
	fgSizer5->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_labVarIndex = new wxStaticText( m_VarPanel, wxID_ANY, wxT("Index"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labVarIndex->Wrap( -1 );
	fgSizer5->Add( m_labVarIndex, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_labVarValue = new wxStaticText( m_VarPanel, wxID_ANY, wxT("Value"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labVarValue->Wrap( -1 );
	fgSizer5->Add( m_labVarValue, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_VarIndex = new wxSpinCtrl( m_VarPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 255, 0 );
	fgSizer5->Add( m_VarIndex, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	wxBoxSizer* bSizer121;
	bSizer121 = new wxBoxSizer( wxVERTICAL );
	
	m_VarValue = new wxSpinCtrl( m_VarPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 255, 0 );
	bSizer121->Add( m_VarValue, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxHORIZONTAL );
	
	m_VarBit7 = new wxCheckBox( m_VarPanel, wxID_ANY, wxT("7"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer8->Add( m_VarBit7, 0, 0, 5 );
	
	m_VarBit6 = new wxCheckBox( m_VarPanel, wxID_ANY, wxT("6"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer8->Add( m_VarBit6, 0, 0, 5 );
	
	m_VarBit5 = new wxCheckBox( m_VarPanel, wxID_ANY, wxT("5"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer8->Add( m_VarBit5, 0, 0, 5 );
	
	m_VarBit4 = new wxCheckBox( m_VarPanel, wxID_ANY, wxT("4"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer8->Add( m_VarBit4, 0, 0, 5 );
	
	m_VarBit3 = new wxCheckBox( m_VarPanel, wxID_ANY, wxT("3"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer8->Add( m_VarBit3, 0, 0, 5 );
	
	m_VarBit2 = new wxCheckBox( m_VarPanel, wxID_ANY, wxT("2"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer8->Add( m_VarBit2, 0, 0, 5 );
	
	m_VarBit1 = new wxCheckBox( m_VarPanel, wxID_ANY, wxT("1"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer8->Add( m_VarBit1, 0, 0, 5 );
	
	m_VarBit0 = new wxCheckBox( m_VarPanel, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer8->Add( m_VarBit0, 0, 0, 5 );
	
	bSizer121->Add( bSizer8, 0, wxEXPAND, 5 );
	
	fgSizer5->Add( bSizer121, 1, wxEXPAND, 5 );
	
	bSizer4->Add( fgSizer5, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxHORIZONTAL );
	
	m_VarGet = new wxButton( m_VarPanel, wxID_ANY, wxT("Get"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer9->Add( m_VarGet, 0, wxALL, 5 );
	
	m_VarSet = new wxButton( m_VarPanel, wxID_ANY, wxT("Set"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer9->Add( m_VarSet, 0, wxALL, 5 );
	
	bSizer4->Add( bSizer9, 0, wxEXPAND, 5 );
	
	m_VarPanel->SetSizer( bSizer4 );
	m_VarPanel->Layout();
	bSizer4->Fit( m_VarPanel );
	m_NoteBook->AddPage( m_VarPanel, wxT("Variables"), false );
	m_EventsPanel = new wxPanel( m_NoteBook, wxID_CBUS_EVENTSPANEL, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer( wxVERTICAL );
	
	m_EventList = new wxListBox( m_EventsPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_SINGLE ); 
	m_EventList->SetMinSize( wxSize( -1,150 ) );
	
	bSizer10->Add( m_EventList, 1, wxALL|wxEXPAND, 5 );
	
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
	
	wxBoxSizer* bSizer12;
	bSizer12 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer6;
	fgSizer6 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer6->SetFlexibleDirection( wxBOTH );
	fgSizer6->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_EventVar = new wxSpinCtrl( m_EventsPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 255, 0 );
	fgSizer6->Add( m_EventVar, 0, wxALIGN_CENTER_HORIZONTAL|wxRIGHT|wxLEFT, 5 );
	
	m_EvtGetVar = new wxButton( m_EventsPanel, wxID_ANY, wxT("Get"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer6->Add( m_EvtGetVar, 0, wxRIGHT, 5 );
	
	bSizer12->Add( fgSizer6, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	wxBoxSizer* bSizer13;
	bSizer13 = new wxBoxSizer( wxHORIZONTAL );
	
	m_EVBit7 = new wxCheckBox( m_EventsPanel, wxID_ANY, wxT("7"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer13->Add( m_EVBit7, 0, 0, 5 );
	
	m_EVBit6 = new wxCheckBox( m_EventsPanel, wxID_ANY, wxT("6"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer13->Add( m_EVBit6, 0, 0, 5 );
	
	m_EVBit5 = new wxCheckBox( m_EventsPanel, wxID_ANY, wxT("5"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer13->Add( m_EVBit5, 0, 0, 5 );
	
	m_EVBit4 = new wxCheckBox( m_EventsPanel, wxID_ANY, wxT("4"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer13->Add( m_EVBit4, 0, 0, 5 );
	
	m_EVBit3 = new wxCheckBox( m_EventsPanel, wxID_ANY, wxT("3"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer13->Add( m_EVBit3, 0, 0, 5 );
	
	m_EVBit2 = new wxCheckBox( m_EventsPanel, wxID_ANY, wxT("2"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer13->Add( m_EVBit2, 0, 0, 5 );
	
	m_EVBit1 = new wxCheckBox( m_EventsPanel, wxID_ANY, wxT("1"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer13->Add( m_EVBit1, 0, 0, 5 );
	
	m_EVBit0 = new wxCheckBox( m_EventsPanel, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer13->Add( m_EVBit0, 0, 0, 5 );
	
	bSizer12->Add( bSizer13, 1, wxEXPAND, 5 );
	
	fgSizer4->Add( bSizer12, 0, wxEXPAND, 5 );
	
	bSizer10->Add( fgSizer4, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer( wxHORIZONTAL );
	
	m_EventGetAll = new wxButton( m_EventsPanel, wxID_ANY, wxT("Get all"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer11->Add( m_EventGetAll, 0, wxALL, 5 );
	
	m_EventAdd = new wxButton( m_EventsPanel, wxID_ANY, wxT("Add"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer11->Add( m_EventAdd, 0, wxALL, 5 );
	
	m_EventDelete = new wxButton( m_EventsPanel, wxID_ANY, wxT("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer11->Add( m_EventDelete, 0, wxTOP|wxBOTTOM|wxLEFT, 5 );
	
	m_EvtClearAll = new wxButton( m_EventsPanel, wxID_ANY, wxT("Clear all"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer11->Add( m_EvtClearAll, 0, wxTOP|wxBOTTOM|wxRIGHT, 5 );
	
	m_EvtLearn = new wxButton( m_EventsPanel, wxID_ANY, wxT("Learn"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer11->Add( m_EvtLearn, 0, wxTOP|wxBOTTOM|wxLEFT, 5 );
	
	m_EvtUnlearn = new wxButton( m_EventsPanel, wxID_ANY, wxT("Unlearn"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer11->Add( m_EvtUnlearn, 0, wxTOP|wxBOTTOM|wxRIGHT, 5 );
	
	bSizer10->Add( bSizer11, 0, wxEXPAND, 5 );
	
	m_EventsPanel->SetSizer( bSizer10 );
	m_EventsPanel->Layout();
	bSizer10->Fit( m_EventsPanel );
	m_NoteBook->AddPage( m_EventsPanel, wxT("Events"), false );
	m_FirmwarePanel = new wxPanel( m_NoteBook, wxID_CBUS_NODEFIRMWAREPANEL, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer131;
	bSizer131 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer7;
	fgSizer7 = new wxFlexGridSizer( 0, 3, 0, 0 );
	fgSizer7->AddGrowableCol( 1 );
	fgSizer7->SetFlexibleDirection( wxBOTH );
	fgSizer7->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_HexFile = new wxButton( m_FirmwarePanel, wxID_ANY, wxT("HEX File..."), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer7->Add( m_HexFile, 0, wxALL, 5 );
	
	m_HEXFileName = new wxTextCtrl( m_FirmwarePanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	fgSizer7->Add( m_HEXFileName, 0, wxALL|wxEXPAND, 5 );
	
	m_HEXFileSend = new wxButton( m_FirmwarePanel, wxID_ANY, wxT("Send"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer7->Add( m_HEXFileSend, 0, wxALL, 5 );
	
	bSizer131->Add( fgSizer7, 0, wxEXPAND, 5 );
	
	m_HEXFileText = new wxTextCtrl( m_FirmwarePanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxTE_MULTILINE|wxTE_READONLY );
	m_HEXFileText->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 90, false, wxT("Monospace") ) );
	
	bSizer131->Add( m_HEXFileText, 1, wxALL|wxEXPAND, 5 );
	
	m_staticline1 = new wxStaticLine( m_FirmwarePanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer131->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer15;
	bSizer15 = new wxBoxSizer( wxHORIZONTAL );
	
	m_BootMode = new wxButton( m_FirmwarePanel, wxID_ANY, wxT("Boot mode"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer15->Add( m_BootMode, 0, wxALL, 5 );
	
	m_ResetBoot = new wxButton( m_FirmwarePanel, wxID_ANY, wxT("Reset"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer15->Add( m_ResetBoot, 0, wxALL, 5 );
	
	bSizer131->Add( bSizer15, 0, wxEXPAND, 5 );
	
	m_FirmwarePanel->SetSizer( bSizer131 );
	m_FirmwarePanel->Layout();
	bSizer131->Fit( m_FirmwarePanel );
	m_NoteBook->AddPage( m_FirmwarePanel, wxT("Firmware"), true );
	
	bSizer1->Add( m_NoteBook, 1, wxEXPAND | wxALL, 5 );
	
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
	m_SetNodeNumber->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onSetNodeNumber ), NULL, this );
	m_IndexList->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( cbusnodedlggen::onIndexSelect ), NULL, this );
	m_IndexDelete->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onIndexDelete ), NULL, this );
	m_VarList->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( cbusnodedlggen::onVarSelect ), NULL, this );
	m_VarValue->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( cbusnodedlggen::onVarValue ), NULL, this );
	m_VarBit7->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( cbusnodedlggen::onVarBit ), NULL, this );
	m_VarBit6->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( cbusnodedlggen::onVarBit ), NULL, this );
	m_VarBit5->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( cbusnodedlggen::onVarBit ), NULL, this );
	m_VarBit4->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( cbusnodedlggen::onVarBit ), NULL, this );
	m_VarBit3->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( cbusnodedlggen::onVarBit ), NULL, this );
	m_VarBit2->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( cbusnodedlggen::onVarBit ), NULL, this );
	m_VarBit1->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( cbusnodedlggen::onVarBit ), NULL, this );
	m_VarBit0->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( cbusnodedlggen::onVarBit ), NULL, this );
	m_VarGet->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onVarGet ), NULL, this );
	m_VarSet->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onVarSet ), NULL, this );
	m_EventList->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( cbusnodedlggen::onEventSelect ), NULL, this );
	m_EventVar->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( cbusnodedlggen::onEV ), NULL, this );
	m_EvtGetVar->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onEvtGetVar ), NULL, this );
	m_EVBit7->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( cbusnodedlggen::onEVBit ), NULL, this );
	m_EVBit6->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( cbusnodedlggen::onEVBit ), NULL, this );
	m_EVBit5->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( cbusnodedlggen::onEVBit ), NULL, this );
	m_EVBit4->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( cbusnodedlggen::onEVBit ), NULL, this );
	m_EVBit3->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( cbusnodedlggen::onEVBit ), NULL, this );
	m_EVBit2->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( cbusnodedlggen::onEVBit ), NULL, this );
	m_EVBit1->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( cbusnodedlggen::onEVBit ), NULL, this );
	m_EVBit0->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( cbusnodedlggen::onEVBit ), NULL, this );
	m_EventGetAll->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onEventGetAll ), NULL, this );
	m_EventAdd->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onEventAdd ), NULL, this );
	m_EventDelete->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onEventDelete ), NULL, this );
	m_EvtClearAll->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onEvtClearAll ), NULL, this );
	m_EvtLearn->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onLearn ), NULL, this );
	m_EvtUnlearn->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onUnlearn ), NULL, this );
	m_HexFile->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onHexFile ), NULL, this );
	m_HEXFileSend->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onHEXFileSend ), NULL, this );
	m_BootMode->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onBootmode ), NULL, this );
	m_ResetBoot->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onResetBoot ), NULL, this );
	m_stdButtonCancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onCancel ), NULL, this );
	m_stdButtonOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onOK ), NULL, this );
}

cbusnodedlggen::~cbusnodedlggen()
{
	// Disconnect Events
	m_SetNodeNumber->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onSetNodeNumber ), NULL, this );
	m_IndexList->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( cbusnodedlggen::onIndexSelect ), NULL, this );
	m_IndexDelete->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onIndexDelete ), NULL, this );
	m_VarList->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( cbusnodedlggen::onVarSelect ), NULL, this );
	m_VarValue->Disconnect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( cbusnodedlggen::onVarValue ), NULL, this );
	m_VarBit7->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( cbusnodedlggen::onVarBit ), NULL, this );
	m_VarBit6->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( cbusnodedlggen::onVarBit ), NULL, this );
	m_VarBit5->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( cbusnodedlggen::onVarBit ), NULL, this );
	m_VarBit4->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( cbusnodedlggen::onVarBit ), NULL, this );
	m_VarBit3->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( cbusnodedlggen::onVarBit ), NULL, this );
	m_VarBit2->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( cbusnodedlggen::onVarBit ), NULL, this );
	m_VarBit1->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( cbusnodedlggen::onVarBit ), NULL, this );
	m_VarBit0->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( cbusnodedlggen::onVarBit ), NULL, this );
	m_VarGet->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onVarGet ), NULL, this );
	m_VarSet->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onVarSet ), NULL, this );
	m_EventList->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( cbusnodedlggen::onEventSelect ), NULL, this );
	m_EventVar->Disconnect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( cbusnodedlggen::onEV ), NULL, this );
	m_EvtGetVar->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onEvtGetVar ), NULL, this );
	m_EVBit7->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( cbusnodedlggen::onEVBit ), NULL, this );
	m_EVBit6->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( cbusnodedlggen::onEVBit ), NULL, this );
	m_EVBit5->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( cbusnodedlggen::onEVBit ), NULL, this );
	m_EVBit4->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( cbusnodedlggen::onEVBit ), NULL, this );
	m_EVBit3->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( cbusnodedlggen::onEVBit ), NULL, this );
	m_EVBit2->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( cbusnodedlggen::onEVBit ), NULL, this );
	m_EVBit1->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( cbusnodedlggen::onEVBit ), NULL, this );
	m_EVBit0->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( cbusnodedlggen::onEVBit ), NULL, this );
	m_EventGetAll->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onEventGetAll ), NULL, this );
	m_EventAdd->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onEventAdd ), NULL, this );
	m_EventDelete->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onEventDelete ), NULL, this );
	m_EvtClearAll->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onEvtClearAll ), NULL, this );
	m_EvtLearn->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onLearn ), NULL, this );
	m_EvtUnlearn->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onUnlearn ), NULL, this );
	m_HexFile->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onHexFile ), NULL, this );
	m_HEXFileSend->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onHEXFileSend ), NULL, this );
	m_BootMode->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onBootmode ), NULL, this );
	m_ResetBoot->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onResetBoot ), NULL, this );
	m_stdButtonCancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onCancel ), NULL, this );
	m_stdButtonOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onOK ), NULL, this );
}
