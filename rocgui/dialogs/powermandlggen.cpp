///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 17 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "powermandlggen.h"

///////////////////////////////////////////////////////////////////////////

powermandlggen::powermandlggen( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	m_labBoosters = new wxStaticText( this, wxID_ANY, wxT("Boosters"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labBoosters->Wrap( -1 );
	bSizer3->Add( m_labBoosters, 0, wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_Boosters = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxSize( -1,100 ), 0, NULL, wxLB_ALWAYS_SB ); 
	bSizer3->Add( m_Boosters, 0, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer61;
	bSizer61 = new wxBoxSizer( wxHORIZONTAL );
	
	m_labID = new wxStaticText( this, wxID_ANY, wxT("ID"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labID->Wrap( -1 );
	bSizer61->Add( m_labID, 0, wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_ID = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer61->Add( m_ID, 1, wxRIGHT|wxLEFT, 5 );
	
	bSizer3->Add( bSizer61, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxHORIZONTAL );
	
	m_labDistrict = new wxStaticText( this, wxID_ANY, wxT("District"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labDistrict->Wrap( -1 );
	bSizer7->Add( m_labDistrict, 0, wxTOP|wxBOTTOM|wxLEFT, 5 );
	
	m_District = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer7->Add( m_District, 1, wxRIGHT, 5 );
	
	bSizer3->Add( bSizer7, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxHORIZONTAL );
	
	m_AddBooster = new wxButton( this, wxID_ANY, wxT("Add"), wxDefaultPosition, wxSize( -1,30 ), 0 );
	bSizer8->Add( m_AddBooster, 0, wxALL, 5 );
	
	m_DelBooster = new wxButton( this, wxID_ANY, wxT("Delete"), wxDefaultPosition, wxSize( -1,30 ), 0 );
	bSizer8->Add( m_DelBooster, 0, wxALL, 5 );
	
	bSizer3->Add( bSizer8, 1, 0, 5 );
	
	bSizer2->Add( bSizer3, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	m_labModules = new wxStaticText( this, wxID_ANY, wxT("Modules"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labModules->Wrap( -1 );
	bSizer4->Add( m_labModules, 0, wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_Modules = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxSize( -1,100 ), 0, NULL, wxLB_ALWAYS_SB ); 
	bSizer4->Add( m_Modules, 0, wxALL|wxEXPAND, 5 );
	
	m_ModulesCombo = new wxComboBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	bSizer4->Add( m_ModulesCombo, 0, wxEXPAND|wxRIGHT|wxLEFT, 5 );
	
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxHORIZONTAL );
	
	m_AddModule = new wxButton( this, wxID_ANY, wxT("Add"), wxDefaultPosition, wxSize( -1,30 ), 0 );
	bSizer9->Add( m_AddModule, 0, wxALL, 5 );
	
	m_DelModule = new wxButton( this, wxID_ANY, wxT("Delete"), wxDefaultPosition, wxSize( -1,30 ), 0 );
	bSizer9->Add( m_DelModule, 0, wxALL, 5 );
	
	bSizer4->Add( bSizer9, 0, wxEXPAND, 5 );
	
	bSizer2->Add( bSizer4, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );
	
	m_labBlocks = new wxStaticText( this, wxID_ANY, wxT("Blocks"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labBlocks->Wrap( -1 );
	bSizer5->Add( m_labBlocks, 0, wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_Blocks = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxSize( -1,100 ), 0, NULL, wxLB_ALWAYS_SB ); 
	bSizer5->Add( m_Blocks, 0, wxALL|wxEXPAND, 5 );
	
	m_BlocksCombo = new wxComboBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	bSizer5->Add( m_BlocksCombo, 0, wxEXPAND|wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer( wxHORIZONTAL );
	
	m_AddBlock = new wxButton( this, wxID_ANY, wxT("Add"), wxDefaultPosition, wxSize( -1,30 ), 0 );
	bSizer10->Add( m_AddBlock, 0, wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	m_DelBlock = new wxButton( this, wxID_ANY, wxT("Delete"), wxDefaultPosition, wxSize( -1,30 ), 0 );
	bSizer10->Add( m_DelBlock, 0, wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	bSizer5->Add( bSizer10, 0, wxEXPAND, 5 );
	
	bSizer2->Add( bSizer5, 1, wxEXPAND, 5 );
	
	bSizer6->Add( bSizer2, 1, wxEXPAND, 5 );
	
	m_sdbSizer1 = new wxStdDialogButtonSizer();
	m_sdbSizer1OK = new wxButton( this, wxID_OK );
	m_sdbSizer1->AddButton( m_sdbSizer1OK );
	m_sdbSizer1Apply = new wxButton( this, wxID_APPLY );
	m_sdbSizer1->AddButton( m_sdbSizer1Apply );
	m_sdbSizer1Cancel = new wxButton( this, wxID_CANCEL );
	m_sdbSizer1->AddButton( m_sdbSizer1Cancel );
	m_sdbSizer1->Realize();
	bSizer6->Add( m_sdbSizer1, 0, wxALIGN_RIGHT|wxEXPAND|wxALL, 5 );
	
	this->SetSizer( bSizer6 );
	this->Layout();
	bSizer6->Fit( this );
	
	// Connect Events
	m_AddBooster->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( powermandlggen::OnAddBooster ), NULL, this );
	m_DelBooster->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( powermandlggen::OnDelBooster ), NULL, this );
	m_AddModule->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( powermandlggen::OnAddModule ), NULL, this );
	m_DelModule->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( powermandlggen::OnDelModule ), NULL, this );
	m_AddBlock->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( powermandlggen::OnAddBlock ), NULL, this );
	m_DelBlock->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( powermandlggen::OnDelBlock ), NULL, this );
	m_sdbSizer1Apply->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( powermandlggen::OnApply ), NULL, this );
	m_sdbSizer1Cancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( powermandlggen::OnCancel ), NULL, this );
	m_sdbSizer1OK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( powermandlggen::OnOK ), NULL, this );
}

powermandlggen::~powermandlggen()
{
	// Disconnect Events
	m_AddBooster->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( powermandlggen::OnAddBooster ), NULL, this );
	m_DelBooster->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( powermandlggen::OnDelBooster ), NULL, this );
	m_AddModule->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( powermandlggen::OnAddModule ), NULL, this );
	m_DelModule->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( powermandlggen::OnDelModule ), NULL, this );
	m_AddBlock->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( powermandlggen::OnAddBlock ), NULL, this );
	m_DelBlock->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( powermandlggen::OnDelBlock ), NULL, this );
	m_sdbSizer1Apply->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( powermandlggen::OnApply ), NULL, this );
	m_sdbSizer1Cancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( powermandlggen::OnCancel ), NULL, this );
	m_sdbSizer1OK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( powermandlggen::OnOK ), NULL, this );
}
