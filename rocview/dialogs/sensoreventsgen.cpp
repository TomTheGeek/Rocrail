///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep 12 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "sensoreventsgen.h"

///////////////////////////////////////////////////////////////////////////

SensorEventsGen::SensorEventsGen( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_EventList = new wxListCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_HRULES|wxLC_REPORT|wxLC_SINGLE_SEL );
	m_EventList->SetMinSize( wxSize( 400,200 ) );
	
	bSizer1->Add( m_EventList, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	m_Refresh = new wxButton( this, wxID_ANY, wxT("Refresh"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_Refresh, 0, wxALL, 5 );
	
	m_Reset = new wxButton( this, wxID_ANY, wxT("Reset"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_Reset, 0, wxALL, 5 );
	
	bSizer1->Add( bSizer2, 0, 0, 5 );
	
	m_StdButtons = new wxStdDialogButtonSizer();
	m_StdButtonsOK = new wxButton( this, wxID_OK );
	m_StdButtons->AddButton( m_StdButtonsOK );
	m_StdButtonsHelp = new wxButton( this, wxID_HELP );
	m_StdButtons->AddButton( m_StdButtonsHelp );
	m_StdButtons->Realize();
	bSizer1->Add( m_StdButtons, 0, wxALL|wxALIGN_RIGHT|wxEXPAND, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	bSizer1->Fit( this );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( SensorEventsGen::onClose ) );
	m_EventList->Connect( wxEVT_COMMAND_LIST_BEGIN_DRAG, wxListEventHandler( SensorEventsGen::onDrag ), NULL, this );
	m_EventList->Connect( wxEVT_COMMAND_LIST_COL_CLICK, wxListEventHandler( SensorEventsGen::onColClick ), NULL, this );
	m_EventList->Connect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( SensorEventsGen::onListSelected ), NULL, this );
	m_Refresh->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SensorEventsGen::onRefresh ), NULL, this );
	m_Reset->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SensorEventsGen::onReset ), NULL, this );
	m_StdButtonsHelp->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SensorEventsGen::onHelp ), NULL, this );
	m_StdButtonsOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SensorEventsGen::onOK ), NULL, this );
}

SensorEventsGen::~SensorEventsGen()
{
	// Disconnect Events
	this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( SensorEventsGen::onClose ) );
	m_EventList->Disconnect( wxEVT_COMMAND_LIST_BEGIN_DRAG, wxListEventHandler( SensorEventsGen::onDrag ), NULL, this );
	m_EventList->Disconnect( wxEVT_COMMAND_LIST_COL_CLICK, wxListEventHandler( SensorEventsGen::onColClick ), NULL, this );
	m_EventList->Disconnect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( SensorEventsGen::onListSelected ), NULL, this );
	m_Refresh->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SensorEventsGen::onRefresh ), NULL, this );
	m_Reset->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SensorEventsGen::onReset ), NULL, this );
	m_StdButtonsHelp->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SensorEventsGen::onHelp ), NULL, this );
	m_StdButtonsOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SensorEventsGen::onOK ), NULL, this );
	
}
