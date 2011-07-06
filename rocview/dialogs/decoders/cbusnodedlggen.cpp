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
	m_NodeNumberPanel = new wxPanel( m_NoteBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer2->AddGrowableCol( 1 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_labNodeType = new wxStaticText( m_NodeNumberPanel, wxID_ANY, wxT("Type"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labNodeType->Wrap( -1 );
	fgSizer2->Add( m_labNodeType, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );
	
	m_NodeType = new wxTextCtrl( m_NodeNumberPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	fgSizer2->Add( m_NodeType, 0, wxALL|wxEXPAND, 5 );
	
	bSizer3->Add( fgSizer2, 1, wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 0, 3, 0, 0 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_labNumber = new wxStaticText( m_NodeNumberPanel, wxID_ANY, wxT("Number"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labNumber->Wrap( -1 );
	fgSizer1->Add( m_labNumber, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_NodeNumber = new wxSpinCtrl( m_NodeNumberPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 100,-1 ), wxSP_ARROW_KEYS, 0, 10, 0 );
	fgSizer1->Add( m_NodeNumber, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_SetNodeNumber = new wxButton( m_NodeNumberPanel, wxID_ANY, wxT("Set"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_SetNodeNumber, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	bSizer3->Add( fgSizer1, 1, wxEXPAND, 5 );
	
	m_NodeNumberPanel->SetSizer( bSizer3 );
	m_NodeNumberPanel->Layout();
	bSizer3->Fit( m_NodeNumberPanel );
	m_NoteBook->AddPage( m_NodeNumberPanel, wxT("Node"), false );
	
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
	m_sdbSizer1OK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onOK ), NULL, this );
}

cbusnodedlggen::~cbusnodedlggen()
{
	// Disconnect Events
	m_sdbSizer1OK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( cbusnodedlggen::onOK ), NULL, this );
}
