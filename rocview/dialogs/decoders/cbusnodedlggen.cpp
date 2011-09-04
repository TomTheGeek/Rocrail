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
	m_NoteBook->AddPage( m_FirmwarePanel, wxT("Firmware"), false );
	m_CANGC2Panel = new wxPanel( m_NoteBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer151;
	bSizer151 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer17;
	bSizer17 = new wxBoxSizer( wxHORIZONTAL );
	
	wxFlexGridSizer* fgSizer8;
	fgSizer8 = new wxFlexGridSizer( 0, 9, 0, 0 );
	fgSizer8->SetFlexibleDirection( wxBOTH );
	fgSizer8->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_lab1GC2Port = new wxStaticText( m_CANGC2Panel, wxID_ANY, wxT("Port"), wxDefaultPosition, wxDefaultSize, 0 );
	m_lab1GC2Port->Wrap( -1 );
	fgSizer8->Add( m_lab1GC2Port, 0, wxTOP|wxBOTTOM|wxLEFT, 5 );
	
	m_lab1GC2EvtNN = new wxStaticText( m_CANGC2Panel, wxID_ANY, wxT("Node Nr."), wxDefaultPosition, wxDefaultSize, 0 );
	m_lab1GC2EvtNN->Wrap( -1 );
	fgSizer8->Add( m_lab1GC2EvtNN, 0, wxALL, 5 );
	
	m_lab1GC2EvtAddr = new wxStaticText( m_CANGC2Panel, wxID_ANY, wxT("Address"), wxDefaultPosition, wxDefaultSize, 0 );
	m_lab1GC2EvtAddr->Wrap( -1 );
	fgSizer8->Add( m_lab1GC2EvtAddr, 0, wxALL, 5 );
	
	m_Lab1GC2Input = new wxStaticText( m_CANGC2Panel, wxID_ANY, wxT("Input"), wxDefaultPosition, wxDefaultSize, 0 );
	m_Lab1GC2Input->Wrap( -1 );
	fgSizer8->Add( m_Lab1GC2Input, 0, wxTOP|wxBOTTOM|wxLEFT, 5 );
	
	m_lab1GC2Sensor = new wxStaticText( m_CANGC2Panel, wxID_ANY, wxT("Block"), wxDefaultPosition, wxDefaultSize, 0 );
	m_lab1GC2Sensor->Wrap( -1 );
	fgSizer8->Add( m_lab1GC2Sensor, 0, wxTOP|wxBOTTOM|wxLEFT, 5 );
	
	m_lab1GC2Switch = new wxStaticText( m_CANGC2Panel, wxID_ANY, wxT("Switch"), wxDefaultPosition, wxDefaultSize, 0 );
	m_lab1GC2Switch->Wrap( -1 );
	fgSizer8->Add( m_lab1GC2Switch, 0, wxTOP|wxBOTTOM|wxLEFT, 5 );
	
	m_lab1GC2Pulse = new wxStaticText( m_CANGC2Panel, wxID_ANY, wxT("Pulse"), wxDefaultPosition, wxDefaultSize, 0 );
	m_lab1GC2Pulse->Wrap( -1 );
	fgSizer8->Add( m_lab1GC2Pulse, 0, wxTOP|wxBOTTOM|wxLEFT, 5 );
	
	m_lab1C2 = new wxStaticText( m_CANGC2Panel, wxID_ANY, wxT("C2"), wxDefaultPosition, wxDefaultSize, 0 );
	m_lab1C2->Wrap( -1 );
	fgSizer8->Add( m_lab1C2, 0, wxALL, 5 );
	
	m_lab1GC2Test = new wxStaticText( m_CANGC2Panel, wxID_ANY, wxT("Test"), wxDefaultPosition, wxDefaultSize, 0 );
	m_lab1GC2Test->Wrap( -1 );
	fgSizer8->Add( m_lab1GC2Test, 0, wxALL, 5 );
	
	m_labGC2Port1 = new wxStaticText( m_CANGC2Panel, wxID_ANY, wxT("1"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labGC2Port1->Wrap( -1 );
	fgSizer8->Add( m_labGC2Port1, 0, wxTOP|wxLEFT, 5 );
	
	m_GC2EvtNN1 = new wxSpinCtrl( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 80,-1 ), wxSP_ARROW_KEYS, 0, 65535, 0 );
	fgSizer8->Add( m_GC2EvtNN1, 0, wxLEFT, 5 );
	
	m_GC2EvtAddr1 = new wxSpinCtrl( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 80,-1 ), wxSP_ARROW_KEYS, 0, 65535, 0 );
	fgSizer8->Add( m_GC2EvtAddr1, 0, wxLEFT, 5 );
	
	m_GC2Input1 = new wxRadioButton( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	fgSizer8->Add( m_GC2Input1, 0, wxLEFT, 5 );
	
	m_GC2Block1 = new wxRadioButton( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer8->Add( m_GC2Block1, 0, wxLEFT, 5 );
	
	m_GC2Switch1 = new wxRadioButton( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer8->Add( m_GC2Switch1, 0, wxLEFT, 5 );
	
	m_GC2Pulse1 = new wxRadioButton( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer8->Add( m_GC2Pulse1, 0, wxLEFT, 5 );
	
	m_GC2c21 = new wxCheckBox( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer8->Add( m_GC2c21, 0, wxLEFT, 5 );
	
	m_GC2Test1 = new wxButton( m_CANGC2Panel, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 30,-1 ), 0 );
	fgSizer8->Add( m_GC2Test1, 0, wxLEFT, 5 );
	
	m_labGC2Port2 = new wxStaticText( m_CANGC2Panel, wxID_ANY, wxT("2"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labGC2Port2->Wrap( -1 );
	fgSizer8->Add( m_labGC2Port2, 0, wxTOP|wxLEFT, 5 );
	
	m_GC2EvtNN2 = new wxSpinCtrl( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 80,-1 ), wxSP_ARROW_KEYS, 0, 65535, 0 );
	fgSizer8->Add( m_GC2EvtNN2, 0, wxLEFT, 5 );
	
	m_GC2EvtAddr2 = new wxSpinCtrl( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 80,-1 ), wxSP_ARROW_KEYS, 0, 65535, 0 );
	fgSizer8->Add( m_GC2EvtAddr2, 0, wxLEFT, 5 );
	
	m_GC2Input2 = new wxRadioButton( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	fgSizer8->Add( m_GC2Input2, 0, wxLEFT, 5 );
	
	m_GC2Block2 = new wxRadioButton( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer8->Add( m_GC2Block2, 0, wxLEFT, 5 );
	
	m_GC2Switch2 = new wxRadioButton( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer8->Add( m_GC2Switch2, 0, wxLEFT, 5 );
	
	m_GC2Pulse2 = new wxRadioButton( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer8->Add( m_GC2Pulse2, 0, wxLEFT, 5 );
	
	m_GC2c22 = new wxCheckBox( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer8->Add( m_GC2c22, 0, wxLEFT, 5 );
	
	m_GC2Test2 = new wxButton( m_CANGC2Panel, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 30,-1 ), 0 );
	fgSizer8->Add( m_GC2Test2, 0, wxLEFT, 5 );
	
	m_labGC2Port3 = new wxStaticText( m_CANGC2Panel, wxID_ANY, wxT("3"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labGC2Port3->Wrap( -1 );
	fgSizer8->Add( m_labGC2Port3, 0, wxTOP|wxLEFT, 5 );
	
	m_GC2EvtNN3 = new wxSpinCtrl( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 80,-1 ), wxSP_ARROW_KEYS, 0, 65535, 0 );
	fgSizer8->Add( m_GC2EvtNN3, 0, wxLEFT, 5 );
	
	m_GC2EvtAddr3 = new wxSpinCtrl( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 80,-1 ), wxSP_ARROW_KEYS, 0, 65535, 0 );
	fgSizer8->Add( m_GC2EvtAddr3, 0, wxLEFT, 5 );
	
	m_GC2Input3 = new wxRadioButton( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	fgSizer8->Add( m_GC2Input3, 0, wxLEFT, 5 );
	
	m_GC2Block3 = new wxRadioButton( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer8->Add( m_GC2Block3, 0, wxLEFT, 5 );
	
	m_GC2Switch3 = new wxRadioButton( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer8->Add( m_GC2Switch3, 0, wxLEFT, 5 );
	
	m_GC2Pulse3 = new wxRadioButton( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer8->Add( m_GC2Pulse3, 0, wxLEFT, 5 );
	
	m_GC2c23 = new wxCheckBox( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer8->Add( m_GC2c23, 0, wxLEFT, 5 );
	
	m_GC2Test3 = new wxButton( m_CANGC2Panel, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 30,-1 ), 0 );
	fgSizer8->Add( m_GC2Test3, 0, wxLEFT, 5 );
	
	m_labGC2Port4 = new wxStaticText( m_CANGC2Panel, wxID_ANY, wxT("4"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labGC2Port4->Wrap( -1 );
	fgSizer8->Add( m_labGC2Port4, 0, wxTOP|wxLEFT, 5 );
	
	m_GC2EvtNN4 = new wxSpinCtrl( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 80,-1 ), wxSP_ARROW_KEYS, 0, 65535, 0 );
	fgSizer8->Add( m_GC2EvtNN4, 0, wxLEFT, 5 );
	
	m_GC2EvtAddr4 = new wxSpinCtrl( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 80,-1 ), wxSP_ARROW_KEYS, 0, 65535, 0 );
	fgSizer8->Add( m_GC2EvtAddr4, 0, wxLEFT, 5 );
	
	m_GC2Input4 = new wxRadioButton( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	fgSizer8->Add( m_GC2Input4, 0, wxLEFT, 5 );
	
	m_GC2Block4 = new wxRadioButton( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer8->Add( m_GC2Block4, 0, wxLEFT, 5 );
	
	m_GC2Switch4 = new wxRadioButton( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer8->Add( m_GC2Switch4, 0, wxLEFT, 5 );
	
	m_GC2Pulse4 = new wxRadioButton( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer8->Add( m_GC2Pulse4, 0, wxLEFT, 5 );
	
	m_GC2c24 = new wxCheckBox( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer8->Add( m_GC2c24, 0, wxLEFT, 5 );
	
	m_GC2Test4 = new wxButton( m_CANGC2Panel, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 30,-1 ), 0 );
	fgSizer8->Add( m_GC2Test4, 0, wxLEFT, 5 );
	
	m_labGC2Port5 = new wxStaticText( m_CANGC2Panel, wxID_ANY, wxT("5"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labGC2Port5->Wrap( -1 );
	fgSizer8->Add( m_labGC2Port5, 0, wxTOP|wxLEFT, 5 );
	
	m_GC2EvtNN5 = new wxSpinCtrl( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 80,-1 ), wxSP_ARROW_KEYS, 0, 65535, 0 );
	fgSizer8->Add( m_GC2EvtNN5, 0, wxLEFT, 5 );
	
	m_GC2EvtAddr5 = new wxSpinCtrl( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 80,-1 ), wxSP_ARROW_KEYS, 0, 65535, 0 );
	fgSizer8->Add( m_GC2EvtAddr5, 0, wxLEFT, 5 );
	
	m_GC2Input5 = new wxRadioButton( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	fgSizer8->Add( m_GC2Input5, 0, wxLEFT, 5 );
	
	m_GC2Block5 = new wxRadioButton( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer8->Add( m_GC2Block5, 0, wxLEFT, 5 );
	
	m_GC2Switch5 = new wxRadioButton( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer8->Add( m_GC2Switch5, 0, wxLEFT, 5 );
	
	m_GC2Pulse5 = new wxRadioButton( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer8->Add( m_GC2Pulse5, 0, wxLEFT, 5 );
	
	m_GC2c25 = new wxCheckBox( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer8->Add( m_GC2c25, 0, wxLEFT, 5 );
	
	m_GC2Test5 = new wxButton( m_CANGC2Panel, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 30,-1 ), 0 );
	fgSizer8->Add( m_GC2Test5, 0, wxLEFT, 5 );
	
	m_labGC2Port6 = new wxStaticText( m_CANGC2Panel, wxID_ANY, wxT("6"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labGC2Port6->Wrap( -1 );
	fgSizer8->Add( m_labGC2Port6, 0, wxTOP|wxLEFT, 5 );
	
	m_GC2EvtNN6 = new wxSpinCtrl( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 80,-1 ), wxSP_ARROW_KEYS, 0, 65535, 0 );
	fgSizer8->Add( m_GC2EvtNN6, 0, wxLEFT, 5 );
	
	m_GC2EvtAddr6 = new wxSpinCtrl( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 80,-1 ), wxSP_ARROW_KEYS, 0, 65535, 0 );
	fgSizer8->Add( m_GC2EvtAddr6, 0, wxLEFT, 5 );
	
	m_GC2Input6 = new wxRadioButton( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	fgSizer8->Add( m_GC2Input6, 0, wxLEFT, 5 );
	
	m_GC2Block6 = new wxRadioButton( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer8->Add( m_GC2Block6, 0, wxLEFT, 5 );
	
	m_GC2Switch6 = new wxRadioButton( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer8->Add( m_GC2Switch6, 0, wxLEFT, 5 );
	
	m_GC2Pulse6 = new wxRadioButton( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer8->Add( m_GC2Pulse6, 0, wxLEFT, 5 );
	
	m_GC2c26 = new wxCheckBox( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer8->Add( m_GC2c26, 0, wxLEFT, 5 );
	
	m_GC2Test6 = new wxButton( m_CANGC2Panel, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 30,-1 ), 0 );
	fgSizer8->Add( m_GC2Test6, 0, wxLEFT, 5 );
	
	m_labGC2Port7 = new wxStaticText( m_CANGC2Panel, wxID_ANY, wxT("7"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labGC2Port7->Wrap( -1 );
	fgSizer8->Add( m_labGC2Port7, 0, wxTOP|wxLEFT, 5 );
	
	m_GC2EvtNN7 = new wxSpinCtrl( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 80,-1 ), wxSP_ARROW_KEYS, 0, 65535, 0 );
	fgSizer8->Add( m_GC2EvtNN7, 0, wxLEFT, 5 );
	
	m_GC2EvtAddr7 = new wxSpinCtrl( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 80,-1 ), wxSP_ARROW_KEYS, 0, 65535, 0 );
	fgSizer8->Add( m_GC2EvtAddr7, 0, wxLEFT, 5 );
	
	m_GC2Input7 = new wxRadioButton( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	fgSizer8->Add( m_GC2Input7, 0, wxLEFT, 5 );
	
	m_GC2Block7 = new wxRadioButton( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer8->Add( m_GC2Block7, 0, wxLEFT, 5 );
	
	m_GC2Switch7 = new wxRadioButton( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer8->Add( m_GC2Switch7, 0, wxLEFT, 5 );
	
	m_GC2Pulse7 = new wxRadioButton( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer8->Add( m_GC2Pulse7, 0, wxLEFT, 5 );
	
	m_GC2c27 = new wxCheckBox( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer8->Add( m_GC2c27, 0, wxLEFT, 5 );
	
	m_GC2Test7 = new wxButton( m_CANGC2Panel, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 30,-1 ), 0 );
	fgSizer8->Add( m_GC2Test7, 0, wxLEFT, 5 );
	
	m_labGC2Port8 = new wxStaticText( m_CANGC2Panel, wxID_ANY, wxT("8"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labGC2Port8->Wrap( -1 );
	fgSizer8->Add( m_labGC2Port8, 0, wxTOP|wxLEFT, 5 );
	
	m_GC2EvtNN8 = new wxSpinCtrl( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 80,-1 ), wxSP_ARROW_KEYS, 0, 65535, 0 );
	fgSizer8->Add( m_GC2EvtNN8, 0, wxLEFT, 5 );
	
	m_GC2EvtAddr8 = new wxSpinCtrl( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 80,-1 ), wxSP_ARROW_KEYS, 0, 65535, 0 );
	fgSizer8->Add( m_GC2EvtAddr8, 0, wxLEFT, 5 );
	
	m_GC2Input8 = new wxRadioButton( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	fgSizer8->Add( m_GC2Input8, 0, wxLEFT, 5 );
	
	m_GC2Block8 = new wxRadioButton( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer8->Add( m_GC2Block8, 0, wxLEFT, 5 );
	
	m_GC2Switch8 = new wxRadioButton( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer8->Add( m_GC2Switch8, 0, wxLEFT, 5 );
	
	m_GC2Pulse8 = new wxRadioButton( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer8->Add( m_GC2Pulse8, 0, wxLEFT, 5 );
	
	m_GC2c28 = new wxCheckBox( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer8->Add( m_GC2c28, 0, wxLEFT, 5 );
	
	m_GC2Test8 = new wxButton( m_CANGC2Panel, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 30,-1 ), 0 );
	fgSizer8->Add( m_GC2Test8, 0, wxLEFT, 5 );
	
	bSizer17->Add( fgSizer8, 1, wxEXPAND, 5 );
	
	m_staticline2 = new wxStaticLine( m_CANGC2Panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL );
	bSizer17->Add( m_staticline2, 0, wxEXPAND | wxALL, 5 );
	
	wxFlexGridSizer* fgSizer81;
	fgSizer81 = new wxFlexGridSizer( 0, 9, 0, 0 );
	fgSizer81->SetFlexibleDirection( wxBOTH );
	fgSizer81->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_lab2GC2Port = new wxStaticText( m_CANGC2Panel, wxID_ANY, wxT("Port"), wxDefaultPosition, wxDefaultSize, 0 );
	m_lab2GC2Port->Wrap( -1 );
	fgSizer81->Add( m_lab2GC2Port, 0, wxTOP|wxBOTTOM|wxLEFT, 5 );
	
	m_lab2GC2EvtNN = new wxStaticText( m_CANGC2Panel, wxID_ANY, wxT("Node Nr."), wxDefaultPosition, wxDefaultSize, 0 );
	m_lab2GC2EvtNN->Wrap( -1 );
	fgSizer81->Add( m_lab2GC2EvtNN, 0, wxALL, 5 );
	
	m_lab2GC2EvtAddr = new wxStaticText( m_CANGC2Panel, wxID_ANY, wxT("Address"), wxDefaultPosition, wxDefaultSize, 0 );
	m_lab2GC2EvtAddr->Wrap( -1 );
	fgSizer81->Add( m_lab2GC2EvtAddr, 0, wxALL, 5 );
	
	m_Lab2GC2Input = new wxStaticText( m_CANGC2Panel, wxID_ANY, wxT("Input"), wxDefaultPosition, wxDefaultSize, 0 );
	m_Lab2GC2Input->Wrap( -1 );
	fgSizer81->Add( m_Lab2GC2Input, 0, wxTOP|wxBOTTOM|wxLEFT, 5 );
	
	m_lab2GC2Sensor = new wxStaticText( m_CANGC2Panel, wxID_ANY, wxT("Block"), wxDefaultPosition, wxDefaultSize, 0 );
	m_lab2GC2Sensor->Wrap( -1 );
	fgSizer81->Add( m_lab2GC2Sensor, 0, wxTOP|wxBOTTOM|wxLEFT, 5 );
	
	m_lab2GC2Switch = new wxStaticText( m_CANGC2Panel, wxID_ANY, wxT("Switch"), wxDefaultPosition, wxDefaultSize, 0 );
	m_lab2GC2Switch->Wrap( -1 );
	fgSizer81->Add( m_lab2GC2Switch, 0, wxTOP|wxBOTTOM|wxLEFT, 5 );
	
	m_lab2GC2Pulse = new wxStaticText( m_CANGC2Panel, wxID_ANY, wxT("Pulse"), wxDefaultPosition, wxDefaultSize, 0 );
	m_lab2GC2Pulse->Wrap( -1 );
	fgSizer81->Add( m_lab2GC2Pulse, 0, wxTOP|wxBOTTOM|wxLEFT, 5 );
	
	m_lab2C2 = new wxStaticText( m_CANGC2Panel, wxID_ANY, wxT("C2"), wxDefaultPosition, wxDefaultSize, 0 );
	m_lab2C2->Wrap( -1 );
	fgSizer81->Add( m_lab2C2, 0, wxALL, 5 );
	
	m_lab2GC2Test = new wxStaticText( m_CANGC2Panel, wxID_ANY, wxT("Test"), wxDefaultPosition, wxDefaultSize, 0 );
	m_lab2GC2Test->Wrap( -1 );
	fgSizer81->Add( m_lab2GC2Test, 0, wxALL, 5 );
	
	m_labGC2Port9 = new wxStaticText( m_CANGC2Panel, wxID_ANY, wxT("9"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labGC2Port9->Wrap( -1 );
	fgSizer81->Add( m_labGC2Port9, 0, wxTOP|wxLEFT, 5 );
	
	m_GC2EvtNN9 = new wxSpinCtrl( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 80,-1 ), wxSP_ARROW_KEYS, 0, 65535, 0 );
	fgSizer81->Add( m_GC2EvtNN9, 0, wxLEFT, 5 );
	
	m_GC2EvtAddr9 = new wxSpinCtrl( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 80,-1 ), wxSP_ARROW_KEYS, 0, 65535, 0 );
	fgSizer81->Add( m_GC2EvtAddr9, 0, wxLEFT, 5 );
	
	m_GC2Input9 = new wxRadioButton( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	fgSizer81->Add( m_GC2Input9, 0, wxLEFT, 5 );
	
	m_GC2Block9 = new wxRadioButton( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer81->Add( m_GC2Block9, 0, wxLEFT, 5 );
	
	m_GC2Switch9 = new wxRadioButton( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer81->Add( m_GC2Switch9, 0, wxLEFT, 5 );
	
	m_GC2Pulse9 = new wxRadioButton( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer81->Add( m_GC2Pulse9, 0, wxLEFT, 5 );
	
	m_GC2c29 = new wxCheckBox( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer81->Add( m_GC2c29, 0, wxLEFT, 5 );
	
	m_GC2Test9 = new wxButton( m_CANGC2Panel, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 30,-1 ), 0 );
	fgSizer81->Add( m_GC2Test9, 0, wxLEFT, 5 );
	
	m_labGC2Port10 = new wxStaticText( m_CANGC2Panel, wxID_ANY, wxT("10"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labGC2Port10->Wrap( -1 );
	fgSizer81->Add( m_labGC2Port10, 0, wxTOP|wxLEFT, 5 );
	
	m_GC2EvtNN10 = new wxSpinCtrl( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 80,-1 ), wxSP_ARROW_KEYS, 0, 65535, 0 );
	fgSizer81->Add( m_GC2EvtNN10, 0, wxLEFT, 5 );
	
	m_GC2EvtAddr10 = new wxSpinCtrl( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 80,-1 ), wxSP_ARROW_KEYS, 0, 65535, 0 );
	fgSizer81->Add( m_GC2EvtAddr10, 0, wxLEFT, 5 );
	
	m_GC2Input10 = new wxRadioButton( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	fgSizer81->Add( m_GC2Input10, 0, wxLEFT, 5 );
	
	m_GC2Block10 = new wxRadioButton( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer81->Add( m_GC2Block10, 0, wxLEFT, 5 );
	
	m_GC2Switch10 = new wxRadioButton( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer81->Add( m_GC2Switch10, 0, wxLEFT, 5 );
	
	m_GC2Pulse10 = new wxRadioButton( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer81->Add( m_GC2Pulse10, 0, wxLEFT, 5 );
	
	m_GC2c210 = new wxCheckBox( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer81->Add( m_GC2c210, 0, wxLEFT, 5 );
	
	m_GC2Test10 = new wxButton( m_CANGC2Panel, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 30,-1 ), 0 );
	fgSizer81->Add( m_GC2Test10, 0, wxLEFT, 5 );
	
	m_labGC2Port11 = new wxStaticText( m_CANGC2Panel, wxID_ANY, wxT("11"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labGC2Port11->Wrap( -1 );
	fgSizer81->Add( m_labGC2Port11, 0, wxTOP|wxLEFT, 5 );
	
	m_GC2EvtNN11 = new wxSpinCtrl( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 80,-1 ), wxSP_ARROW_KEYS, 0, 65535, 0 );
	fgSizer81->Add( m_GC2EvtNN11, 0, wxLEFT, 5 );
	
	m_GC2EvtAddr11 = new wxSpinCtrl( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 80,-1 ), wxSP_ARROW_KEYS, 0, 65535, 0 );
	fgSizer81->Add( m_GC2EvtAddr11, 0, wxLEFT, 5 );
	
	m_GC2Input11 = new wxRadioButton( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	fgSizer81->Add( m_GC2Input11, 0, wxLEFT, 5 );
	
	m_GC2Block11 = new wxRadioButton( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer81->Add( m_GC2Block11, 0, wxLEFT, 5 );
	
	m_GC2Switch11 = new wxRadioButton( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer81->Add( m_GC2Switch11, 0, wxLEFT, 5 );
	
	m_GC2Pulse11 = new wxRadioButton( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer81->Add( m_GC2Pulse11, 0, wxLEFT, 5 );
	
	m_GC2c211 = new wxCheckBox( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer81->Add( m_GC2c211, 0, wxLEFT, 5 );
	
	m_GC2Test11 = new wxButton( m_CANGC2Panel, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 30,-1 ), 0 );
	fgSizer81->Add( m_GC2Test11, 0, wxLEFT, 5 );
	
	m_labGC2Port12 = new wxStaticText( m_CANGC2Panel, wxID_ANY, wxT("12"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labGC2Port12->Wrap( -1 );
	fgSizer81->Add( m_labGC2Port12, 0, wxTOP|wxLEFT, 5 );
	
	m_GC2EvtNN12 = new wxSpinCtrl( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 80,-1 ), wxSP_ARROW_KEYS, 0, 65535, 0 );
	fgSizer81->Add( m_GC2EvtNN12, 0, wxLEFT, 5 );
	
	m_GC2EvtAddr12 = new wxSpinCtrl( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 80,-1 ), wxSP_ARROW_KEYS, 0, 65535, 0 );
	fgSizer81->Add( m_GC2EvtAddr12, 0, wxLEFT, 5 );
	
	m_GC2Input12 = new wxRadioButton( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	fgSizer81->Add( m_GC2Input12, 0, wxLEFT, 5 );
	
	m_GC2Block12 = new wxRadioButton( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer81->Add( m_GC2Block12, 0, wxLEFT, 5 );
	
	m_GC2Switch12 = new wxRadioButton( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer81->Add( m_GC2Switch12, 0, wxLEFT, 5 );
	
	m_GC2Pulse12 = new wxRadioButton( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer81->Add( m_GC2Pulse12, 0, wxLEFT, 5 );
	
	m_GC2c212 = new wxCheckBox( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer81->Add( m_GC2c212, 0, wxLEFT, 5 );
	
	m_GC2Test12 = new wxButton( m_CANGC2Panel, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 30,-1 ), 0 );
	fgSizer81->Add( m_GC2Test12, 0, wxLEFT, 5 );
	
	m_labGC2Port13 = new wxStaticText( m_CANGC2Panel, wxID_ANY, wxT("13"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labGC2Port13->Wrap( -1 );
	fgSizer81->Add( m_labGC2Port13, 0, wxTOP|wxLEFT, 5 );
	
	m_GC2EvtNN13 = new wxSpinCtrl( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 80,-1 ), wxSP_ARROW_KEYS, 0, 65535, 0 );
	fgSizer81->Add( m_GC2EvtNN13, 0, wxLEFT, 5 );
	
	m_GC2EvtAddr13 = new wxSpinCtrl( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 80,-1 ), wxSP_ARROW_KEYS, 0, 65535, 0 );
	fgSizer81->Add( m_GC2EvtAddr13, 0, wxLEFT, 5 );
	
	m_GC2Input13 = new wxRadioButton( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	fgSizer81->Add( m_GC2Input13, 0, wxLEFT, 5 );
	
	m_GC2Block13 = new wxRadioButton( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer81->Add( m_GC2Block13, 0, wxLEFT, 5 );
	
	m_GC2Switch13 = new wxRadioButton( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer81->Add( m_GC2Switch13, 0, wxLEFT, 5 );
	
	m_GC2Pulse13 = new wxRadioButton( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer81->Add( m_GC2Pulse13, 0, wxLEFT, 5 );
	
	m_GC2c213 = new wxCheckBox( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer81->Add( m_GC2c213, 0, wxLEFT, 5 );
	
	m_GC2Test13 = new wxButton( m_CANGC2Panel, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 30,-1 ), 0 );
	fgSizer81->Add( m_GC2Test13, 0, wxLEFT, 5 );
	
	m_labGC2Port14 = new wxStaticText( m_CANGC2Panel, wxID_ANY, wxT("14"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labGC2Port14->Wrap( -1 );
	fgSizer81->Add( m_labGC2Port14, 0, wxTOP|wxLEFT, 5 );
	
	m_GC2EvtNN14 = new wxSpinCtrl( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 80,-1 ), wxSP_ARROW_KEYS, 0, 65535, 0 );
	fgSizer81->Add( m_GC2EvtNN14, 0, wxLEFT, 5 );
	
	m_GC2EvtAddr14 = new wxSpinCtrl( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 80,-1 ), wxSP_ARROW_KEYS, 0, 65535, 0 );
	fgSizer81->Add( m_GC2EvtAddr14, 0, wxLEFT, 5 );
	
	m_GC2Input14 = new wxRadioButton( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	fgSizer81->Add( m_GC2Input14, 0, wxLEFT, 5 );
	
	m_GC2Block14 = new wxRadioButton( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer81->Add( m_GC2Block14, 0, wxLEFT, 5 );
	
	m_GC2Switch14 = new wxRadioButton( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer81->Add( m_GC2Switch14, 0, wxLEFT, 5 );
	
	m_GC2Pulse14 = new wxRadioButton( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer81->Add( m_GC2Pulse14, 0, wxLEFT, 5 );
	
	m_GC2c214 = new wxCheckBox( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer81->Add( m_GC2c214, 0, wxLEFT, 5 );
	
	m_GC2Test14 = new wxButton( m_CANGC2Panel, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 30,-1 ), 0 );
	fgSizer81->Add( m_GC2Test14, 0, wxLEFT, 5 );
	
	m_labGC2Port15 = new wxStaticText( m_CANGC2Panel, wxID_ANY, wxT("15"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labGC2Port15->Wrap( -1 );
	fgSizer81->Add( m_labGC2Port15, 0, wxTOP|wxLEFT, 5 );
	
	m_GC2EvtNN15 = new wxSpinCtrl( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 80,-1 ), wxSP_ARROW_KEYS, 0, 65535, 0 );
	fgSizer81->Add( m_GC2EvtNN15, 0, wxLEFT, 5 );
	
	m_GC2EvtAddr15 = new wxSpinCtrl( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 80,-1 ), wxSP_ARROW_KEYS, 0, 65535, 0 );
	fgSizer81->Add( m_GC2EvtAddr15, 0, wxLEFT, 5 );
	
	m_GC2Input15 = new wxRadioButton( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	fgSizer81->Add( m_GC2Input15, 0, wxLEFT, 5 );
	
	m_GC2Block15 = new wxRadioButton( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer81->Add( m_GC2Block15, 0, wxLEFT, 5 );
	
	m_GC2Switch15 = new wxRadioButton( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer81->Add( m_GC2Switch15, 0, wxLEFT, 5 );
	
	m_GC2Pulse15 = new wxRadioButton( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer81->Add( m_GC2Pulse15, 0, wxLEFT, 5 );
	
	m_GC2c215 = new wxCheckBox( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer81->Add( m_GC2c215, 0, wxLEFT, 5 );
	
	m_GC2Test15 = new wxButton( m_CANGC2Panel, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 30,-1 ), 0 );
	fgSizer81->Add( m_GC2Test15, 0, wxLEFT, 5 );
	
	m_labGC2Port16 = new wxStaticText( m_CANGC2Panel, wxID_ANY, wxT("16"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labGC2Port16->Wrap( -1 );
	fgSizer81->Add( m_labGC2Port16, 0, wxTOP|wxLEFT, 5 );
	
	m_GC2EvtNN16 = new wxSpinCtrl( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 80,-1 ), wxSP_ARROW_KEYS, 0, 65535, 0 );
	fgSizer81->Add( m_GC2EvtNN16, 0, wxLEFT, 5 );
	
	m_GC2EvtAddr16 = new wxSpinCtrl( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 80,-1 ), wxSP_ARROW_KEYS, 0, 65535, 0 );
	fgSizer81->Add( m_GC2EvtAddr16, 0, wxLEFT, 5 );
	
	m_GC2Input16 = new wxRadioButton( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	fgSizer81->Add( m_GC2Input16, 0, wxLEFT, 5 );
	
	m_GC2Block16 = new wxRadioButton( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer81->Add( m_GC2Block16, 0, wxLEFT, 5 );
	
	m_GC2Switch16 = new wxRadioButton( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer81->Add( m_GC2Switch16, 0, wxLEFT, 5 );
	
	m_GC2Pulse16 = new wxRadioButton( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer81->Add( m_GC2Pulse16, 0, wxLEFT, 5 );
	
	m_GC2c216 = new wxCheckBox( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer81->Add( m_GC2c216, 0, wxLEFT, 5 );
	
	m_GC2Test16 = new wxButton( m_CANGC2Panel, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 30,-1 ), 0 );
	fgSizer81->Add( m_GC2Test16, 0, wxLEFT, 5 );
	
	bSizer17->Add( fgSizer81, 1, wxEXPAND, 5 );
	
	bSizer151->Add( bSizer17, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer16;
	bSizer16 = new wxBoxSizer( wxHORIZONTAL );
	
	m_GC2GetAll = new wxButton( m_CANGC2Panel, wxID_ANY, wxT("Get all"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer16->Add( m_GC2GetAll, 0, wxTOP|wxBOTTOM|wxLEFT, 5 );
	
	m_GC2SetAll = new wxButton( m_CANGC2Panel, wxID_ANY, wxT("Set all"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer16->Add( m_GC2SetAll, 0, wxTOP|wxBOTTOM|wxLEFT, 5 );
	
	m_GC2Set = new wxButton( m_CANGC2Panel, wxID_ANY, wxT("Set"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer16->Add( m_GC2Set, 0, wxTOP|wxBOTTOM|wxLEFT, 5 );
	
	m_GC2SaveOutput = new wxCheckBox( m_CANGC2Panel, wxID_ANY, wxT("Save output state"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer16->Add( m_GC2SaveOutput, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_GC2ShortEvents = new wxCheckBox( m_CANGC2Panel, wxID_ANY, wxT("Short events"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer16->Add( m_GC2ShortEvents, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxFlexGridSizer* fgSizer13;
	fgSizer13 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer13->AddGrowableCol( 1 );
	fgSizer13->SetFlexibleDirection( wxBOTH );
	fgSizer13->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_labSOD = new wxStaticText( m_CANGC2Panel, wxID_ANY, wxT("SoD:"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	m_labSOD->Wrap( -1 );
	fgSizer13->Add( m_labSOD, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );
	
	m_GC2SOD = new wxSpinCtrl( m_CANGC2Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 80,-1 ), wxSP_ARROW_KEYS, 0, 65535, 0 );
	fgSizer13->Add( m_GC2SOD, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	bSizer16->Add( fgSizer13, 1, wxALIGN_CENTER_VERTICAL, 5 );
	
	bSizer151->Add( bSizer16, 0, wxEXPAND, 5 );
	
	m_CANGC2Panel->SetSizer( bSizer151 );
	m_CANGC2Panel->Layout();
	bSizer151->Fit( m_CANGC2Panel );
	m_NoteBook->AddPage( m_CANGC2Panel, wxT("CAN-GC2"), true );
	
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
	m_GC2Test1->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onGC2Test ), NULL, this );
	m_GC2Test2->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onGC2Test ), NULL, this );
	m_GC2Test3->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onGC2Test ), NULL, this );
	m_GC2Test4->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onGC2Test ), NULL, this );
	m_GC2Test5->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onGC2Test ), NULL, this );
	m_GC2Test6->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onGC2Test ), NULL, this );
	m_GC2Test7->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onGC2Test ), NULL, this );
	m_GC2Test8->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onGC2Test ), NULL, this );
	m_GC2Test9->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onGC2Test ), NULL, this );
	m_GC2Test10->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onGC2Test ), NULL, this );
	m_GC2Test11->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onGC2Test ), NULL, this );
	m_GC2Test12->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onGC2Test ), NULL, this );
	m_GC2Test13->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onGC2Test ), NULL, this );
	m_GC2Test14->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onGC2Test ), NULL, this );
	m_GC2Test15->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onGC2Test ), NULL, this );
	m_GC2Test16->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onGC2Test ), NULL, this );
	m_GC2GetAll->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onGC2GetAll ), NULL, this );
	m_GC2SetAll->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onGC2SetAll ), NULL, this );
	m_GC2Set->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onGC2Set ), NULL, this );
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
	m_GC2Test1->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onGC2Test ), NULL, this );
	m_GC2Test2->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onGC2Test ), NULL, this );
	m_GC2Test3->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onGC2Test ), NULL, this );
	m_GC2Test4->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onGC2Test ), NULL, this );
	m_GC2Test5->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onGC2Test ), NULL, this );
	m_GC2Test6->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onGC2Test ), NULL, this );
	m_GC2Test7->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onGC2Test ), NULL, this );
	m_GC2Test8->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onGC2Test ), NULL, this );
	m_GC2Test9->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onGC2Test ), NULL, this );
	m_GC2Test10->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onGC2Test ), NULL, this );
	m_GC2Test11->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onGC2Test ), NULL, this );
	m_GC2Test12->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onGC2Test ), NULL, this );
	m_GC2Test13->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onGC2Test ), NULL, this );
	m_GC2Test14->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onGC2Test ), NULL, this );
	m_GC2Test15->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onGC2Test ), NULL, this );
	m_GC2Test16->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onGC2Test ), NULL, this );
	m_GC2GetAll->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onGC2GetAll ), NULL, this );
	m_GC2SetAll->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onGC2SetAll ), NULL, this );
	m_GC2Set->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onGC2Set ), NULL, this );
	m_stdButtonCancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onCancel ), NULL, this );
	m_stdButtonOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onOK ), NULL, this );
}
