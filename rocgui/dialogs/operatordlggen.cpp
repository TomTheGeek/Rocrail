///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 21 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "operatordlggen.h"

///////////////////////////////////////////////////////////////////////////

operatordlggen::operatordlggen( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_OperatorBook = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
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
	
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 2, 3, 0, 0 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_labLocation = new wxStaticText( m_ControlPanel, wxID_ANY, wxT("Location"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labLocation->Wrap( -1 );
	fgSizer2->Add( m_labLocation, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_Location = new wxComboBox( m_ControlPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY ); 
	fgSizer2->Add( m_Location, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_Reserve = new wxButton( m_ControlPanel, wxID_ANY, wxT("Reserve"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_Reserve, 0, wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_labGoto = new wxStaticText( m_ControlPanel, wxID_ANY, wxT("Goto"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labGoto->Wrap( -1 );
	fgSizer2->Add( m_labGoto, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	m_Goto = new wxComboBox( m_ControlPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY ); 
	fgSizer2->Add( m_Goto, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	m_Run = new wxButton( m_ControlPanel, wxID_ANY, wxT("Run"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_Run, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	bSizer4->Add( fgSizer2, 0, wxEXPAND, 5 );
	
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
	m_NewOperator->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( operatordlggen::onNewOperator ), NULL, this );
	m_DelOperator->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( operatordlggen::onDelOperator ), NULL, this );
}

operatordlggen::~operatordlggen()
{
	// Disconnect Events
	m_NewOperator->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( operatordlggen::onNewOperator ), NULL, this );
	m_DelOperator->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( operatordlggen::onDelOperator ), NULL, this );
}
