///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 17 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "bidibdlggen.h"

///////////////////////////////////////////////////////////////////////////

bidibdlggen::bidibdlggen( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );
	
	m_Notebook = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_General = new wxPanel( m_Notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer5;
	fgSizer5 = new wxFlexGridSizer( 1, 2, 0, 0 );
	fgSizer5->AddGrowableCol( 1 );
	fgSizer5->SetFlexibleDirection( wxBOTH );
	fgSizer5->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_labIID = new wxStaticText( m_General, wxID_ANY, wxT("IID"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labIID->Wrap( -1 );
	fgSizer5->Add( m_labIID, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );
	
	m_IID = new wxTextCtrl( m_General, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer5->Add( m_IID, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	m_labDevice = new wxStaticText( m_General, wxID_ANY, wxT("Device"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labDevice->Wrap( -1 );
	fgSizer5->Add( m_labDevice, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );
	
	m_Device = new wxTextCtrl( m_General, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 200,-1 ), 0 );
	fgSizer5->Add( m_Device, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	bSizer6->Add( fgSizer5, 0, wxEXPAND, 5 );
	
	wxString m_BPSChoices[] = { wxT("19600"), wxT("115200"), wxT("1M") };
	int m_BPSNChoices = sizeof( m_BPSChoices ) / sizeof( wxString );
	m_BPS = new wxRadioBox( m_General, wxID_ANY, wxT("BPS"), wxDefaultPosition, wxDefaultSize, m_BPSNChoices, m_BPSChoices, 1, wxRA_SPECIFY_ROWS );
	m_BPS->SetSelection( 0 );
	bSizer6->Add( m_BPS, 0, wxALL, 5 );
	
	wxString m_SubLibChoices[] = { wxT("Serial") };
	int m_SubLibNChoices = sizeof( m_SubLibChoices ) / sizeof( wxString );
	m_SubLib = new wxRadioBox( m_General, wxID_ANY, wxT("Sub Library"), wxDefaultPosition, wxDefaultSize, m_SubLibNChoices, m_SubLibChoices, 1, wxRA_SPECIFY_ROWS );
	m_SubLib->SetSelection( 0 );
	bSizer6->Add( m_SubLib, 0, wxALL, 5 );
	
	m_General->SetSizer( bSizer6 );
	m_General->Layout();
	bSizer6->Fit( m_General );
	m_Notebook->AddPage( m_General, wxT("General"), true );
	m_Options = new wxPanel( m_Notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* m_SecAckBox;
	m_SecAckBox = new wxStaticBoxSizer( new wxStaticBox( m_Options, wxID_ANY, wxT("Secure ACK") ), wxVERTICAL );
	
	m_SecAck = new wxCheckBox( m_Options, wxID_ANY, wxT("Enable"), wxDefaultPosition, wxDefaultSize, 0 );
	
	m_SecAckBox->Add( m_SecAck, 0, wxALL, 5 );
	
	wxFlexGridSizer* fgSizer6;
	fgSizer6 = new wxFlexGridSizer( 1, 3, 0, 0 );
	fgSizer6->SetFlexibleDirection( wxBOTH );
	fgSizer6->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_labSecAckInt = new wxStaticText( m_Options, wxID_ANY, wxT("Interval"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labSecAckInt->Wrap( -1 );
	fgSizer6->Add( m_labSecAckInt, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_SecAckInt = new wxSpinCtrl( m_Options, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 80,-1 ), wxSP_ARROW_KEYS, 0, 100, 20 );
	fgSizer6->Add( m_SecAckInt, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_labMs10 = new wxStaticText( m_Options, wxID_ANY, wxT("* 10ms"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labMs10->Wrap( -1 );
	fgSizer6->Add( m_labMs10, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_SecAckBox->Add( fgSizer6, 1, wxEXPAND, 5 );
	
	bSizer7->Add( m_SecAckBox, 0, wxEXPAND, 5 );
	
	m_Options->SetSizer( bSizer7 );
	m_Options->Layout();
	bSizer7->Fit( m_Options );
	m_Notebook->AddPage( m_Options, wxT("Options"), false );
	m_Nodes = new wxPanel( m_Notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxVERTICAL );
	
	m_NodeList = new wxListBox( m_Nodes, wxID_ANY, wxDefaultPosition, wxSize( -1,140 ), 0, NULL, wxLB_SINGLE ); 
	bSizer8->Add( m_NodeList, 0, wxALL|wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer7;
	fgSizer7 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer7->SetFlexibleDirection( wxBOTH );
	fgSizer7->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_labUID = new wxStaticText( m_Nodes, wxID_ANY, wxT("UID"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labUID->Wrap( -1 );
	fgSizer7->Add( m_labUID, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );
	
	m_UID = new wxTextCtrl( m_Nodes, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 120,-1 ), 0 );
	fgSizer7->Add( m_UID, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_labOffset = new wxStaticText( m_Nodes, wxID_ANY, wxT("Offset"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labOffset->Wrap( -1 );
	fgSizer7->Add( m_labOffset, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );
	
	m_Offset = new wxSpinCtrl( m_Nodes, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 120,-1 ), wxSP_ARROW_KEYS, 0, 10000, 0 );
	fgSizer7->Add( m_Offset, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	bSizer8->Add( fgSizer7, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxHORIZONTAL );
	
	m_AddNode = new wxButton( m_Nodes, wxID_ANY, wxT("Add"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer9->Add( m_AddNode, 0, wxALL, 5 );
	
	m_ModifyNode = new wxButton( m_Nodes, wxID_ANY, wxT("Modify"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer9->Add( m_ModifyNode, 0, wxALL, 5 );
	
	m_DeleteNode = new wxButton( m_Nodes, wxID_ANY, wxT("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer9->Add( m_DeleteNode, 0, wxALL, 5 );
	
	bSizer8->Add( bSizer9, 0, wxEXPAND|wxALIGN_RIGHT, 5 );
	
	m_Nodes->SetSizer( bSizer8 );
	m_Nodes->Layout();
	bSizer8->Fit( m_Nodes );
	m_Notebook->AddPage( m_Nodes, wxT("Nodes"), false );
	
	bSizer5->Add( m_Notebook, 0, wxEXPAND | wxALL, 5 );
	
	m_StdButton = new wxStdDialogButtonSizer();
	m_StdButtonOK = new wxButton( this, wxID_OK );
	m_StdButton->AddButton( m_StdButtonOK );
	m_StdButtonCancel = new wxButton( this, wxID_CANCEL );
	m_StdButton->AddButton( m_StdButtonCancel );
	m_StdButton->Realize();
	bSizer5->Add( m_StdButton, 0, wxALL|wxALIGN_RIGHT, 5 );
	
	this->SetSizer( bSizer5 );
	this->Layout();
	bSizer5->Fit( this );
	
	// Connect Events
	m_NodeList->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( bidibdlggen::OnNodeList ), NULL, this );
	m_AddNode->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( bidibdlggen::OnAddNode ), NULL, this );
	m_ModifyNode->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( bidibdlggen::OnModifyNode ), NULL, this );
	m_DeleteNode->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( bidibdlggen::OnDeleteNode ), NULL, this );
	m_StdButtonCancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( bidibdlggen::OnCancel ), NULL, this );
	m_StdButtonOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( bidibdlggen::OnOK ), NULL, this );
}

bidibdlggen::~bidibdlggen()
{
	// Disconnect Events
	m_NodeList->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( bidibdlggen::OnNodeList ), NULL, this );
	m_AddNode->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( bidibdlggen::OnAddNode ), NULL, this );
	m_ModifyNode->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( bidibdlggen::OnModifyNode ), NULL, this );
	m_DeleteNode->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( bidibdlggen::OnDeleteNode ), NULL, this );
	m_StdButtonCancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( bidibdlggen::OnCancel ), NULL, this );
	m_StdButtonOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( bidibdlggen::OnOK ), NULL, this );
}
