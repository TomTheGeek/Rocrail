///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 17 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "basenotebook.h"

#include "operatordlggen.h"

///////////////////////////////////////////////////////////////////////////

operatordlggen::operatordlggen( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_OperatorBook = new basenotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_IndexPanel = new wxPanel( m_OperatorBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );
	
	m_OperatorList = new wxListBox( m_IndexPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	bSizer5->Add( m_OperatorList, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxHORIZONTAL );
	
	m_NewOperator = new wxButton( m_IndexPanel, wxID_ANY, wxT("New"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer6->Add( m_NewOperator, 0, wxALL, 5 );
	
	m_DelOperator = new wxButton( m_IndexPanel, wxID_ANY, wxT("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer6->Add( m_DelOperator, 0, wxALL, 5 );
	
	bSizer5->Add( bSizer6, 0, wxEXPAND, 5 );
	
	m_IndexPanel->SetSizer( bSizer5 );
	m_IndexPanel->Layout();
	bSizer5->Fit( m_IndexPanel );
	m_OperatorBook->AddPage( m_IndexPanel, wxT("Index"), false );
	m_ControlPanel = new wxPanel( m_OperatorBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer3;
	fgSizer3 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer3->AddGrowableCol( 1 );
	fgSizer3->SetFlexibleDirection( wxBOTH );
	fgSizer3->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_labOperator = new wxStaticText( m_ControlPanel, wxID_ANY, wxT("Name"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labOperator->Wrap( -1 );
	fgSizer3->Add( m_labOperator, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_Operator = new wxTextCtrl( m_ControlPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer3->Add( m_Operator, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	bSizer4->Add( fgSizer3, 0, wxEXPAND, 5 );
	
	m_staticline1 = new wxStaticLine( m_ControlPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer4->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );
	
	m_LocoImage = new wxBitmapButton( m_ControlPanel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( -1,80 ), wxBU_AUTODRAW );
	bSizer4->Add( m_LocoImage, 0, wxEXPAND|wxTOP|wxRIGHT|wxLEFT, 5 );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_labLocoID = new wxStaticText( m_ControlPanel, wxID_ANY, wxT("Locomotive"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labLocoID->Wrap( -1 );
	fgSizer1->Add( m_labLocoID, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );
	
	m_LocoID = new wxComboBox( m_ControlPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY ); 
	fgSizer1->Add( m_LocoID, 0, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	bSizer4->Add( fgSizer1, 0, wxEXPAND, 5 );
	
	wxStaticBoxSizer* m_labLocation;
	m_labLocation = new wxStaticBoxSizer( new wxStaticBox( m_ControlPanel, wxID_ANY, wxT("Location") ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 2, 3, 0, 0 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_Location = new wxComboBox( m_ControlPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY ); 
	fgSizer2->Add( m_Location, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_Reserve = new wxButton( m_ControlPanel, wxID_ANY, wxT("Reserve"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_Reserve, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_labLocation->Add( fgSizer2, 0, wxEXPAND, 5 );
	
	bSizer4->Add( m_labLocation, 0, wxEXPAND, 5 );
	
	wxStaticBoxSizer* m_labGoto;
	m_labGoto = new wxStaticBoxSizer( new wxStaticBox( m_ControlPanel, wxID_ANY, wxT("Goto") ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizer4;
	fgSizer4 = new wxFlexGridSizer( 1, 3, 0, 0 );
	fgSizer4->SetFlexibleDirection( wxBOTH );
	fgSizer4->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_Goto = new wxComboBox( m_ControlPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY ); 
	fgSizer4->Add( m_Goto, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxVERTICAL );
	
	m_Run = new wxButton( m_ControlPanel, wxID_ANY, wxT("Run"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer7->Add( m_Run, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT|wxLEFT, 5 );
	
	m_GotoMan = new wxButton( m_ControlPanel, wxID_ANY, wxT("Manually"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer7->Add( m_GotoMan, 0, wxRIGHT|wxLEFT, 5 );
	
	fgSizer4->Add( bSizer7, 0, 0, 5 );
	
	m_labGoto->Add( fgSizer4, 0, wxEXPAND, 5 );
	
	bSizer4->Add( m_labGoto, 0, wxEXPAND, 5 );
	
	m_ControlPanel->SetSizer( bSizer4 );
	m_ControlPanel->Layout();
	bSizer4->Fit( m_ControlPanel );
	m_OperatorBook->AddPage( m_ControlPanel, wxT("Control"), true );
	m_ConsistPanel = new wxPanel( m_OperatorBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );
	
	m_CarImage = new wxBitmapButton( m_ConsistPanel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( -1,80 ), wxBU_AUTODRAW );
	bSizer2->Add( m_CarImage, 0, wxEXPAND|wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_CarList = new wxListBox( m_ConsistPanel, wxID_ANY, wxDefaultPosition, wxSize( -1,200 ), 0, NULL, 0 ); 
	bSizer2->Add( m_CarList, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );
	
	m_AddCar = new wxButton( m_ConsistPanel, wxID_ANY, wxT("Add"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_AddCar, 0, wxTOP|wxBOTTOM|wxLEFT, 5 );
	
	m_LeaveCar = new wxButton( m_ConsistPanel, wxID_ANY, wxT("Leave"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_LeaveCar, 0, wxTOP|wxBOTTOM, 5 );
	
	m_ShowCar = new wxButton( m_ConsistPanel, wxID_ANY, wxT("Car card"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_ShowCar, 0, wxTOP|wxBOTTOM, 5 );
	
	m_ShowWaybill = new wxButton( m_ConsistPanel, wxID_ANY, wxT("Waybill"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_ShowWaybill, 0, wxTOP|wxBOTTOM|wxRIGHT, 5 );
	
	bSizer2->Add( bSizer3, 0, wxEXPAND, 5 );
	
	m_ConsistPanel->SetSizer( bSizer2 );
	m_ConsistPanel->Layout();
	bSizer2->Fit( m_ConsistPanel );
	m_OperatorBook->AddPage( m_ConsistPanel, wxT("Consist"), false );
	
	bSizer1->Add( m_OperatorBook, 1, wxEXPAND | wxALL, 5 );
	
	m_sdbSizer1 = new wxStdDialogButtonSizer();
	m_sdbSizer1OK = new wxButton( this, wxID_OK );
	m_sdbSizer1->AddButton( m_sdbSizer1OK );
	m_sdbSizer1Apply = new wxButton( this, wxID_APPLY );
	m_sdbSizer1->AddButton( m_sdbSizer1Apply );
	m_sdbSizer1Cancel = new wxButton( this, wxID_CANCEL );
	m_sdbSizer1->AddButton( m_sdbSizer1Cancel );
	m_sdbSizer1->Realize();
	bSizer1->Add( m_sdbSizer1, 0, wxEXPAND|wxALL, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	bSizer1->Fit( this );
	
	// Connect Events
	m_OperatorList->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( operatordlggen::onOperatorList ), NULL, this );
	m_NewOperator->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( operatordlggen::onNewOperator ), NULL, this );
	m_DelOperator->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( operatordlggen::onDelOperator ), NULL, this );
	m_LocoImage->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( operatordlggen::onLocoImage ), NULL, this );
	m_LocoID->Connect( wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler( operatordlggen::onLocomotiveCombo ), NULL, this );
	m_Reserve->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( operatordlggen::onReserve ), NULL, this );
	m_Run->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( operatordlggen::onRun ), NULL, this );
	m_GotoMan->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( operatordlggen::onGotoMan ), NULL, this );
	m_CarImage->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( operatordlggen::onCarImage ), NULL, this );
	m_CarList->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( operatordlggen::onCarList ), NULL, this );
	m_AddCar->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( operatordlggen::onAddCar ), NULL, this );
	m_LeaveCar->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( operatordlggen::onLeaveCar ), NULL, this );
	m_ShowCar->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( operatordlggen::onCarCard ), NULL, this );
	m_ShowWaybill->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( operatordlggen::onWayBill ), NULL, this );
	m_sdbSizer1Apply->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( operatordlggen::onApply ), NULL, this );
	m_sdbSizer1Cancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( operatordlggen::onCancel ), NULL, this );
	m_sdbSizer1OK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( operatordlggen::onOK ), NULL, this );
}

operatordlggen::~operatordlggen()
{
	// Disconnect Events
	m_OperatorList->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( operatordlggen::onOperatorList ), NULL, this );
	m_NewOperator->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( operatordlggen::onNewOperator ), NULL, this );
	m_DelOperator->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( operatordlggen::onDelOperator ), NULL, this );
	m_LocoImage->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( operatordlggen::onLocoImage ), NULL, this );
	m_LocoID->Disconnect( wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler( operatordlggen::onLocomotiveCombo ), NULL, this );
	m_Reserve->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( operatordlggen::onReserve ), NULL, this );
	m_Run->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( operatordlggen::onRun ), NULL, this );
	m_GotoMan->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( operatordlggen::onGotoMan ), NULL, this );
	m_CarImage->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( operatordlggen::onCarImage ), NULL, this );
	m_CarList->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( operatordlggen::onCarList ), NULL, this );
	m_AddCar->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( operatordlggen::onAddCar ), NULL, this );
	m_LeaveCar->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( operatordlggen::onLeaveCar ), NULL, this );
	m_ShowCar->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( operatordlggen::onCarCard ), NULL, this );
	m_ShowWaybill->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( operatordlggen::onWayBill ), NULL, this );
	m_sdbSizer1Apply->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( operatordlggen::onApply ), NULL, this );
	m_sdbSizer1Cancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( operatordlggen::onCancel ), NULL, this );
	m_sdbSizer1OK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( operatordlggen::onOK ), NULL, this );
}
