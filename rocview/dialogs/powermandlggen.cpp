///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "basenotebook.h"

#include "powermandlggen.h"

///////////////////////////////////////////////////////////////////////////

powermandlggen::powermandlggen( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );
	
	m_BoosterBook = new basenotebook( this, ID_BOOSTERBOOK, wxDefaultPosition, wxDefaultSize, 0 );
	m_IndexPanel = new wxPanel( m_BoosterBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	m_BoosterList = new wxListBox( m_IndexPanel, wxID_ANY, wxDefaultPosition, wxSize( -1,100 ), 0, NULL, wxLB_ALWAYS_SB ); 
	bSizer3->Add( m_BoosterList, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxHORIZONTAL );
	
	m_AddBooster = new wxButton( m_IndexPanel, wxID_ANY, wxT("Add"), wxDefaultPosition, wxSize( -1,30 ), 0 );
	bSizer8->Add( m_AddBooster, 0, wxALL, 5 );
	
	m_DelBooster = new wxButton( m_IndexPanel, wxID_ANY, wxT("Delete"), wxDefaultPosition, wxSize( -1,30 ), 0 );
	bSizer8->Add( m_DelBooster, 0, wxALL, 5 );
	
	bSizer3->Add( bSizer8, 0, 0, 5 );
	
	m_IndexPanel->SetSizer( bSizer3 );
	m_IndexPanel->Layout();
	bSizer3->Fit( m_IndexPanel );
	m_BoosterBook->AddPage( m_IndexPanel, wxT("Index"), false );
	m_GeneralPanel = new wxPanel( m_BoosterBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer14;
	bSizer14 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer61;
	bSizer61 = new wxBoxSizer( wxHORIZONTAL );
	
	m_labID = new wxStaticText( m_GeneralPanel, wxID_ANY, wxT("ID"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labID->Wrap( -1 );
	bSizer61->Add( m_labID, 0, wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_ID = new wxTextCtrl( m_GeneralPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer61->Add( m_ID, 1, wxRIGHT|wxLEFT, 5 );
	
	bSizer14->Add( bSizer61, 0, wxEXPAND|wxTOP|wxBOTTOM, 5 );
	
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxHORIZONTAL );
	
	m_labDistrict = new wxStaticText( m_GeneralPanel, wxID_ANY, wxT("District"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labDistrict->Wrap( -1 );
	bSizer7->Add( m_labDistrict, 0, wxTOP|wxBOTTOM|wxLEFT, 5 );
	
	m_District = new wxTextCtrl( m_GeneralPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer7->Add( m_District, 1, wxRIGHT, 5 );
	
	bSizer14->Add( bSizer7, 0, wxEXPAND|wxTOP|wxBOTTOM, 5 );
	
	wxStaticBoxSizer* m_IObox;
	m_IObox = new wxStaticBoxSizer( new wxStaticBox( m_GeneralPanel, wxID_ANY, wxT("I/O") ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_labShortcutSensor = new wxStaticText( m_GeneralPanel, wxID_ANY, wxT("Shortcut sensor"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labShortcutSensor->Wrap( -1 );
	fgSizer1->Add( m_labShortcutSensor, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_ShortcutSensor = new wxComboBox( m_GeneralPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY ); 
	fgSizer1->Add( m_ShortcutSensor, 0, wxALL|wxEXPAND, 5 );
	
	m_labPowerSensor = new wxStaticText( m_GeneralPanel, wxID_ANY, wxT("Power sensor"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labPowerSensor->Wrap( -1 );
	fgSizer1->Add( m_labPowerSensor, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_PowerSensor = new wxComboBox( m_GeneralPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY ); 
	fgSizer1->Add( m_PowerSensor, 0, wxALL|wxEXPAND, 5 );
	
	m_labPowerOutput = new wxStaticText( m_GeneralPanel, wxID_ANY, wxT("Power output"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labPowerOutput->Wrap( -1 );
	fgSizer1->Add( m_labPowerOutput, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_PowerOutput = new wxComboBox( m_GeneralPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY ); 
	fgSizer1->Add( m_PowerOutput, 0, wxALL|wxEXPAND, 5 );
	
	m_labUID = new wxStaticText( m_GeneralPanel, wxID_ANY, wxT("UID"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labUID->Wrap( -1 );
	fgSizer1->Add( m_labUID, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_UID = new wxTextCtrl( m_GeneralPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_UID, 0, wxALL|wxEXPAND, 5 );
	
	m_IObox->Add( fgSizer1, 1, wxEXPAND, 5 );
	
	bSizer14->Add( m_IObox, 0, wxEXPAND|wxALL, 5 );
	
	m_GeneralPanel->SetSizer( bSizer14 );
	m_GeneralPanel->Layout();
	bSizer14->Fit( m_GeneralPanel );
	m_BoosterBook->AddPage( m_GeneralPanel, wxT("Generel"), true );
	m_ModulesPanel = new wxPanel( m_BoosterBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	m_ModuleList = new wxListBox( m_ModulesPanel, wxID_ANY, wxDefaultPosition, wxSize( -1,100 ), 0, NULL, wxLB_ALWAYS_SB ); 
	bSizer4->Add( m_ModuleList, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer12;
	bSizer12 = new wxBoxSizer( wxHORIZONTAL );
	
	m_labModuleID = new wxStaticText( m_ModulesPanel, wxID_ANY, wxT("Module ID"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labModuleID->Wrap( -1 );
	bSizer12->Add( m_labModuleID, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_ModuleID = new wxTextCtrl( m_ModulesPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer12->Add( m_ModuleID, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	bSizer4->Add( bSizer12, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxHORIZONTAL );
	
	m_AddModule = new wxButton( m_ModulesPanel, wxID_ANY, wxT("Add"), wxDefaultPosition, wxSize( -1,30 ), 0 );
	bSizer9->Add( m_AddModule, 0, wxALL, 5 );
	
	m_DelModule = new wxButton( m_ModulesPanel, wxID_ANY, wxT("Delete"), wxDefaultPosition, wxSize( -1,30 ), 0 );
	bSizer9->Add( m_DelModule, 0, wxALL, 5 );
	
	bSizer4->Add( bSizer9, 0, wxEXPAND, 5 );
	
	m_ModulesPanel->SetSizer( bSizer4 );
	m_ModulesPanel->Layout();
	bSizer4->Fit( m_ModulesPanel );
	m_BoosterBook->AddPage( m_ModulesPanel, wxT("Modules"), false );
	m_BlocksPanel = new wxPanel( m_BoosterBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );
	
	m_BlockList = new wxListBox( m_BlocksPanel, wxID_ANY, wxDefaultPosition, wxSize( -1,100 ), 0, NULL, wxLB_ALWAYS_SB ); 
	bSizer5->Add( m_BlockList, 1, wxALL|wxEXPAND, 5 );
	
	m_BlocksCombo = new wxComboBox( m_BlocksPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	bSizer5->Add( m_BlocksCombo, 0, wxEXPAND|wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer( wxHORIZONTAL );
	
	m_AddBlock = new wxButton( m_BlocksPanel, wxID_ANY, wxT("Add"), wxDefaultPosition, wxSize( -1,30 ), 0 );
	bSizer10->Add( m_AddBlock, 0, wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	m_DelBlock = new wxButton( m_BlocksPanel, wxID_ANY, wxT("Delete"), wxDefaultPosition, wxSize( -1,30 ), 0 );
	bSizer10->Add( m_DelBlock, 0, wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	bSizer5->Add( bSizer10, 0, wxEXPAND, 5 );
	
	m_BlocksPanel->SetSizer( bSizer5 );
	m_BlocksPanel->Layout();
	bSizer5->Fit( m_BlocksPanel );
	m_BoosterBook->AddPage( m_BlocksPanel, wxT("Blocks"), false );
	m_DetailsPanel = new wxPanel( m_BoosterBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer15;
	bSizer15 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* m_ShortcutDetails;
	m_ShortcutDetails = new wxStaticBoxSizer( new wxStaticBox( m_DetailsPanel, wxID_ANY, wxT("Shortcut") ), wxVERTICAL );
	
	m_StopDistrictLocos = new wxCheckBox( m_DetailsPanel, wxID_ANY, wxT("Stop all locos in this district"), wxDefaultPosition, wxDefaultSize, 0 );
	m_ShortcutDetails->Add( m_StopDistrictLocos, 0, wxALL, 5 );
	
	m_PowerOffAll = new wxCheckBox( m_DetailsPanel, wxID_ANY, wxT("Power off all other boosters"), wxDefaultPosition, wxDefaultSize, 0 );
	m_ShortcutDetails->Add( m_PowerOffAll, 0, wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	m_RetryPowerOn = new wxCheckBox( m_DetailsPanel, wxID_ANY, wxT("Retry power on every second"), wxDefaultPosition, wxDefaultSize, 0 );
	m_ShortcutDetails->Add( m_RetryPowerOn, 0, wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	bSizer15->Add( m_ShortcutDetails, 0, wxEXPAND|wxALL, 5 );
	
	m_Actions = new wxButton( m_DetailsPanel, wxID_ANY, wxT("Actions"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer15->Add( m_Actions, 0, wxALL, 5 );
	
	m_DetailsPanel->SetSizer( bSizer15 );
	m_DetailsPanel->Layout();
	bSizer15->Fit( m_DetailsPanel );
	m_BoosterBook->AddPage( m_DetailsPanel, wxT("Details"), false );
	
	bSizer6->Add( m_BoosterBook, 1, wxEXPAND | wxALL, 5 );
	
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
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_BoosterList->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( powermandlggen::onBoosterSelect ), NULL, this );
	m_AddBooster->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( powermandlggen::OnAddBooster ), NULL, this );
	m_DelBooster->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( powermandlggen::OnDelBooster ), NULL, this );
	m_ModuleList->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( powermandlggen::onModuleSelect ), NULL, this );
	m_AddModule->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( powermandlggen::OnAddModule ), NULL, this );
	m_DelModule->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( powermandlggen::OnDelModule ), NULL, this );
	m_BlockList->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( powermandlggen::onBlockSelect ), NULL, this );
	m_AddBlock->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( powermandlggen::OnAddBlock ), NULL, this );
	m_DelBlock->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( powermandlggen::OnDelBlock ), NULL, this );
	m_Actions->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( powermandlggen::onActions ), NULL, this );
	m_sdbSizer1Apply->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( powermandlggen::OnApply ), NULL, this );
	m_sdbSizer1Cancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( powermandlggen::OnCancel ), NULL, this );
	m_sdbSizer1OK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( powermandlggen::OnOK ), NULL, this );
}

powermandlggen::~powermandlggen()
{
	// Disconnect Events
	m_BoosterList->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( powermandlggen::onBoosterSelect ), NULL, this );
	m_AddBooster->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( powermandlggen::OnAddBooster ), NULL, this );
	m_DelBooster->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( powermandlggen::OnDelBooster ), NULL, this );
	m_ModuleList->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( powermandlggen::onModuleSelect ), NULL, this );
	m_AddModule->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( powermandlggen::OnAddModule ), NULL, this );
	m_DelModule->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( powermandlggen::OnDelModule ), NULL, this );
	m_BlockList->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( powermandlggen::onBlockSelect ), NULL, this );
	m_AddBlock->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( powermandlggen::OnAddBlock ), NULL, this );
	m_DelBlock->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( powermandlggen::OnDelBlock ), NULL, this );
	m_Actions->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( powermandlggen::onActions ), NULL, this );
	m_sdbSizer1Apply->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( powermandlggen::OnApply ), NULL, this );
	m_sdbSizer1Cancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( powermandlggen::OnCancel ), NULL, this );
	m_sdbSizer1OK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( powermandlggen::OnOK ), NULL, this );
	
}
