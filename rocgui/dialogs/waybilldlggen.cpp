///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 17 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "basenotebook.h"

#include "waybilldlggen.h"

///////////////////////////////////////////////////////////////////////////

waybillgen::waybillgen( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_WaybillBook = new basenotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_IndexPanel = new wxPanel( m_WaybillBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );
	
	m_WaybillList = new wxListBox( m_IndexPanel, wxID_ANY, wxDefaultPosition, wxSize( -1,200 ), 0, NULL, 0 ); 
	bSizer2->Add( m_WaybillList, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );
	
	m_NewWaybill = new wxButton( m_IndexPanel, wxID_ANY, wxT("New"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_NewWaybill, 0, wxALL, 5 );
	
	m_DeleteWaybill = new wxButton( m_IndexPanel, wxID_ANY, wxT("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_DeleteWaybill, 0, wxALL, 5 );
	
	bSizer2->Add( bSizer3, 0, 0, 5 );
	
	m_IndexPanel->SetSizer( bSizer2 );
	m_IndexPanel->Layout();
	bSizer2->Fit( m_IndexPanel );
	m_WaybillBook->AddPage( m_IndexPanel, wxT("Index"), false );
	m_GeneralPanel = new wxPanel( m_WaybillBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_labID = new wxStaticText( m_GeneralPanel, wxID_ANY, wxT("ID"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labID->Wrap( -1 );
	fgSizer1->Add( m_labID, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );
	
	m_ID = new wxTextCtrl( m_GeneralPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_ID, 0, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_labShipper = new wxStaticText( m_GeneralPanel, wxID_ANY, wxT("Shipper"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labShipper->Wrap( -1 );
	fgSizer1->Add( m_labShipper, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_Shipper = new wxTextCtrl( m_GeneralPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_Shipper, 0, wxALIGN_CENTER_VERTICAL|wxEXPAND|wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_labOrigin = new wxStaticText( m_GeneralPanel, wxID_ANY, wxT("Origin"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labOrigin->Wrap( -1 );
	fgSizer1->Add( m_labOrigin, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	m_Origin = new wxComboBox( m_GeneralPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_DROPDOWN|wxCB_READONLY ); 
	fgSizer1->Add( m_Origin, 0, wxBOTTOM|wxRIGHT|wxLEFT|wxEXPAND, 5 );
	
	m_labConsignee = new wxStaticText( m_GeneralPanel, wxID_ANY, wxT("Consignee"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labConsignee->Wrap( -1 );
	fgSizer1->Add( m_labConsignee, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_Consignee = new wxTextCtrl( m_GeneralPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_Consignee, 0, wxALIGN_CENTER_VERTICAL|wxEXPAND|wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_labDestination = new wxStaticText( m_GeneralPanel, wxID_ANY, wxT("Destination"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labDestination->Wrap( -1 );
	fgSizer1->Add( m_labDestination, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	m_Destination = new wxComboBox( m_GeneralPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_DROPDOWN|wxCB_READONLY ); 
	fgSizer1->Add( m_Destination, 0, wxBOTTOM|wxRIGHT|wxLEFT|wxEXPAND, 5 );
	
	m_labCommodity = new wxStaticText( m_GeneralPanel, wxID_ANY, wxT("Commodity"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labCommodity->Wrap( -1 );
	fgSizer1->Add( m_labCommodity, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_Commodity = new wxTextCtrl( m_GeneralPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE );
	fgSizer1->Add( m_Commodity, 0, wxEXPAND|wxALIGN_CENTER_VERTICAL|wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_labCartype = new wxStaticText( m_GeneralPanel, wxID_ANY, wxT("Car type"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labCartype->Wrap( -1 );
	fgSizer1->Add( m_labCartype, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5 );
	
	m_Cartype = new wxComboBox( m_GeneralPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	fgSizer1->Add( m_Cartype, 0, wxALL|wxEXPAND, 5 );
	
	bSizer4->Add( fgSizer1, 1, wxEXPAND, 5 );
	
	wxString m_StatusChoices[] = { wxT("waiting"), wxT("shipping"), wxT("delivered") };
	int m_StatusNChoices = sizeof( m_StatusChoices ) / sizeof( wxString );
	m_Status = new wxRadioBox( m_GeneralPanel, wxID_ANY, wxT("Status"), wxDefaultPosition, wxDefaultSize, m_StatusNChoices, m_StatusChoices, 1, wxRA_SPECIFY_ROWS );
	m_Status->SetSelection( 2 );
	bSizer4->Add( m_Status, 0, wxALL|wxEXPAND, 5 );
	
	m_GeneralPanel->SetSizer( bSizer4 );
	m_GeneralPanel->Layout();
	bSizer4->Fit( m_GeneralPanel );
	m_WaybillBook->AddPage( m_GeneralPanel, wxT("General"), true );
	m_RoutingPanel = new wxPanel( m_WaybillBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer2->AddGrowableCol( 1 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	bSizer5->Add( fgSizer2, 1, wxEXPAND, 5 );
	
	m_RoutingPanel->SetSizer( bSizer5 );
	m_RoutingPanel->Layout();
	bSizer5->Fit( m_RoutingPanel );
	m_WaybillBook->AddPage( m_RoutingPanel, wxT("Routing"), false );
	
	bSizer1->Add( m_WaybillBook, 1, wxEXPAND | wxALL, 5 );
	
	m_stdButton = new wxStdDialogButtonSizer();
	m_stdButtonOK = new wxButton( this, wxID_OK );
	m_stdButton->AddButton( m_stdButtonOK );
	m_stdButtonApply = new wxButton( this, wxID_APPLY );
	m_stdButton->AddButton( m_stdButtonApply );
	m_stdButtonCancel = new wxButton( this, wxID_CANCEL );
	m_stdButton->AddButton( m_stdButtonCancel );
	m_stdButton->Realize();
	bSizer1->Add( m_stdButton, 0, wxALIGN_RIGHT|wxALL, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	bSizer1->Fit( this );
	
	// Connect Events
	m_WaybillList->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( waybillgen::onWaybillList ), NULL, this );
	m_NewWaybill->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( waybillgen::onNewWaybill ), NULL, this );
	m_DeleteWaybill->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( waybillgen::onDeleteWaybill ), NULL, this );
	m_stdButtonApply->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( waybillgen::onApply ), NULL, this );
	m_stdButtonCancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( waybillgen::onCancel ), NULL, this );
	m_stdButtonOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( waybillgen::onOK ), NULL, this );
}

waybillgen::~waybillgen()
{
	// Disconnect Events
	m_WaybillList->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( waybillgen::onWaybillList ), NULL, this );
	m_NewWaybill->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( waybillgen::onNewWaybill ), NULL, this );
	m_DeleteWaybill->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( waybillgen::onDeleteWaybill ), NULL, this );
	m_stdButtonApply->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( waybillgen::onApply ), NULL, this );
	m_stdButtonCancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( waybillgen::onCancel ), NULL, this );
	m_stdButtonOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( waybillgen::onOK ), NULL, this );
}
