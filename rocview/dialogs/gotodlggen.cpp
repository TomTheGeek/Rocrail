///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "gotodlggen.h"

///////////////////////////////////////////////////////////////////////////

gotodlggen::gotodlggen( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxVERTICAL );
	
	m_List = new wxListCtrl( this, wxID_ANY, wxDefaultPosition, wxSize( -1,200 ), wxLC_HRULES|wxLC_REPORT|wxLC_SINGLE_SEL );
	bSizer7->Add( m_List, 0, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxHORIZONTAL );
	
	m_Blocks = new wxCheckBox( this, wxID_ANY, wxT("Blocks"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer8->Add( m_Blocks, 0, wxALL, 5 );
	
	m_Stages = new wxCheckBox( this, wxID_ANY, wxT("Stages"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer8->Add( m_Stages, 0, wxALL, 5 );
	
	m_FiddleYards = new wxCheckBox( this, wxID_ANY, wxT("FiddleYards"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer8->Add( m_FiddleYards, 0, wxALL, 5 );
	
	m_Turntables = new wxCheckBox( this, wxID_ANY, wxT("Turntables"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer8->Add( m_Turntables, 0, wxALL, 5 );
	
	bSizer7->Add( bSizer8, 0, 0, 5 );
	
	m_stdButtons = new wxStdDialogButtonSizer();
	m_stdButtonsOK = new wxButton( this, wxID_OK );
	m_stdButtons->AddButton( m_stdButtonsOK );
	m_stdButtonsCancel = new wxButton( this, wxID_CANCEL );
	m_stdButtons->AddButton( m_stdButtonsCancel );
	m_stdButtons->Realize();
	bSizer7->Add( m_stdButtons, 1, wxEXPAND|wxALL, 5 );
	
	this->SetSizer( bSizer7 );
	this->Layout();
	bSizer7->Fit( this );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_List->Connect( wxEVT_COMMAND_LIST_COL_CLICK, wxListEventHandler( gotodlggen::onColClick ), NULL, this );
	m_List->Connect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( gotodlggen::onSelected ), NULL, this );
	m_Blocks->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( gotodlggen::onBlocks ), NULL, this );
	m_Stages->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( gotodlggen::onStages ), NULL, this );
	m_FiddleYards->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( gotodlggen::onFYs ), NULL, this );
	m_Turntables->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( gotodlggen::onTTs ), NULL, this );
	m_stdButtonsCancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( gotodlggen::onCancel ), NULL, this );
	m_stdButtonsOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( gotodlggen::onOK ), NULL, this );
}

gotodlggen::~gotodlggen()
{
	// Disconnect Events
	m_List->Disconnect( wxEVT_COMMAND_LIST_COL_CLICK, wxListEventHandler( gotodlggen::onColClick ), NULL, this );
	m_List->Disconnect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( gotodlggen::onSelected ), NULL, this );
	m_Blocks->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( gotodlggen::onBlocks ), NULL, this );
	m_Stages->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( gotodlggen::onStages ), NULL, this );
	m_FiddleYards->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( gotodlggen::onFYs ), NULL, this );
	m_Turntables->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( gotodlggen::onTTs ), NULL, this );
	m_stdButtonsCancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( gotodlggen::onCancel ), NULL, this );
	m_stdButtonsOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( gotodlggen::onOK ), NULL, this );
	
}
