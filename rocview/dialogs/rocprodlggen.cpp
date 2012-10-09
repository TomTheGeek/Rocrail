///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "rocprodlggen.h"

///////////////////////////////////////////////////////////////////////////

RocProDlgGen::RocProDlgGen( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer( wxVERTICAL );
	
	m_DecTree = new wxTreeCtrl( this, wxID_ANY, wxDefaultPosition, wxSize( -1,300 ), wxTR_DEFAULT_STYLE|wxRAISED_BORDER );
	bSizer10->Add( m_DecTree, 1, wxALL|wxEXPAND, 5 );
	
	m_Info = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( -1,100 ), wxTE_MULTILINE|wxTE_READONLY );
	bSizer10->Add( m_Info, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	bSizer8->Add( bSizer10, 1, wxEXPAND, 5 );
	
	m_staticline2 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL );
	bSizer8->Add( m_staticline2, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer8;
	fgSizer8 = new wxFlexGridSizer( 0, 3, 0, 0 );
	fgSizer8->AddGrowableCol( 1 );
	fgSizer8->SetFlexibleDirection( wxBOTH );
	fgSizer8->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_labFile = new wxStaticText( this, wxID_ANY, wxT("Decoder"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labFile->Wrap( -1 );
	fgSizer8->Add( m_labFile, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_DecFile = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 200,-1 ), wxTE_READONLY );
	fgSizer8->Add( m_DecFile, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	m_DecFileOpen = new wxButton( this, wxID_ANY, wxT("Open..."), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer8->Add( m_DecFileOpen, 0, wxALL, 5 );
	
	bSizer9->Add( fgSizer8, 0, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer3;
	sbSizer3 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Value") ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizer9;
	fgSizer9 = new wxFlexGridSizer( 0, 10, 0, 0 );
	fgSizer9->SetFlexibleDirection( wxBOTH );
	fgSizer9->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_Nr = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 50,-1 ), wxTE_CENTRE|wxTE_READONLY );
	fgSizer9->Add( m_Nr, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_Value = new wxSpinCtrl( this, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 80,-1 ), wxSP_ARROW_KEYS, 0, 255, 0 );
	fgSizer9->Add( m_Value, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_Bit7 = new wxCheckBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer9->Add( m_Bit7, 0, wxTOP|wxBOTTOM|wxLEFT|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_Bit6 = new wxCheckBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer9->Add( m_Bit6, 0, wxTOP|wxBOTTOM|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_Bit5 = new wxCheckBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer9->Add( m_Bit5, 0, wxTOP|wxBOTTOM|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_Bit4 = new wxCheckBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer9->Add( m_Bit4, 0, wxTOP|wxBOTTOM|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_Bit3 = new wxCheckBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer9->Add( m_Bit3, 0, wxTOP|wxBOTTOM|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_Bit2 = new wxCheckBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer9->Add( m_Bit2, 0, wxTOP|wxBOTTOM|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_Bit1 = new wxCheckBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer9->Add( m_Bit1, 0, wxTOP|wxBOTTOM|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_Bit0 = new wxCheckBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer9->Add( m_Bit0, 0, wxTOP|wxBOTTOM|wxALIGN_CENTER_VERTICAL, 5 );
	
	sbSizer3->Add( fgSizer9, 1, wxEXPAND, 5 );
	
	bSizer9->Add( sbSizer3, 0, wxEXPAND, 5 );
	
	bSizer8->Add( bSizer9, 1, wxEXPAND, 5 );
	
	bSizer7->Add( bSizer8, 1, wxEXPAND, 5 );
	
	m_sdbSizer2 = new wxStdDialogButtonSizer();
	m_sdbSizer2OK = new wxButton( this, wxID_OK );
	m_sdbSizer2->AddButton( m_sdbSizer2OK );
	m_sdbSizer2->Realize();
	bSizer7->Add( m_sdbSizer2, 0, wxEXPAND|wxALL, 5 );
	
	this->SetSizer( bSizer7 );
	this->Layout();
	bSizer7->Fit( this );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( RocProDlgGen::onClose ) );
	m_DecTree->Connect( wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler( RocProDlgGen::onTreeSelChanged ), NULL, this );
	m_DecFileOpen->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( RocProDlgGen::onOpen ), NULL, this );
	m_sdbSizer2OK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( RocProDlgGen::onOK ), NULL, this );
}

RocProDlgGen::~RocProDlgGen()
{
	// Disconnect Events
	this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( RocProDlgGen::onClose ) );
	m_DecTree->Disconnect( wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler( RocProDlgGen::onTreeSelChanged ), NULL, this );
	m_DecFileOpen->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( RocProDlgGen::onOpen ), NULL, this );
	m_sdbSizer2OK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( RocProDlgGen::onOK ), NULL, this );
	
}
