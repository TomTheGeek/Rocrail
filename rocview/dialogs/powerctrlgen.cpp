///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "powerctrlgen.h"

///////////////////////////////////////////////////////////////////////////

powerctrlgen::powerctrlgen( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer( wxVERTICAL );
	
	m_Boosters = new wxGrid( this, wxID_ANY, wxDefaultPosition, wxSize( -1,-1 ), 0 );
	
	// Grid
	m_Boosters->CreateGrid( 1, 7 );
	m_Boosters->EnableEditing( false );
	m_Boosters->EnableGridLines( true );
	m_Boosters->EnableDragGridSize( false );
	m_Boosters->SetMargins( 0, 0 );
	
	// Columns
	m_Boosters->AutoSizeColumns();
	m_Boosters->EnableDragColMove( false );
	m_Boosters->EnableDragColSize( true );
	m_Boosters->SetColLabelSize( 30 );
	m_Boosters->SetColLabelAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );
	
	// Rows
	m_Boosters->AutoSizeRows();
	m_Boosters->EnableDragRowSize( true );
	m_Boosters->SetRowLabelSize( 1 );
	m_Boosters->SetRowLabelAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );
	
	// Label Appearance
	
	// Cell Defaults
	m_Boosters->SetDefaultCellAlignment( wxALIGN_LEFT, wxALIGN_TOP );
	m_Boosters->SetMinSize( wxSize( 440,80 ) );
	
	bSizer11->Add( m_Boosters, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* bSizer12;
	bSizer12 = new wxBoxSizer( wxHORIZONTAL );
	
	m_On = new wxButton( this, wxID_ANY, wxT("On"), wxDefaultPosition, wxSize( -1,30 ), 0 );
	bSizer12->Add( m_On, 0, wxALL, 5 );
	
	m_Off = new wxButton( this, wxID_ANY, wxT("Off"), wxDefaultPosition, wxSize( -1,30 ), 0 );
	bSizer12->Add( m_Off, 0, wxALL, 5 );
	
	bSizer3->Add( bSizer12, 1, wxEXPAND, 5 );
	
	m_sdbSizer2 = new wxStdDialogButtonSizer();
	m_sdbSizer2OK = new wxButton( this, wxID_OK );
	m_sdbSizer2->AddButton( m_sdbSizer2OK );
	m_sdbSizer2->Realize();
	bSizer3->Add( m_sdbSizer2, 0, wxALL|wxALIGN_RIGHT, 5 );
	
	bSizer11->Add( bSizer3, 0, wxEXPAND, 5 );
	
	this->SetSizer( bSizer11 );
	this->Layout();
	bSizer11->Fit( this );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( powerctrlgen::onClose ) );
	this->Connect( wxEVT_SIZE, wxSizeEventHandler( powerctrlgen::onSize ) );
	m_Boosters->Connect( wxEVT_GRID_CELL_LEFT_CLICK, wxGridEventHandler( powerctrlgen::onCellLeftClick ), NULL, this );
	m_Boosters->Connect( wxEVT_GRID_CELL_RIGHT_CLICK, wxGridEventHandler( powerctrlgen::onCellRightClick ), NULL, this );
	m_On->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( powerctrlgen::OnOn ), NULL, this );
	m_Off->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( powerctrlgen::OnOff ), NULL, this );
	m_sdbSizer2OK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( powerctrlgen::OnOK ), NULL, this );
}

powerctrlgen::~powerctrlgen()
{
	// Disconnect Events
	this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( powerctrlgen::onClose ) );
	this->Disconnect( wxEVT_SIZE, wxSizeEventHandler( powerctrlgen::onSize ) );
	m_Boosters->Disconnect( wxEVT_GRID_CELL_LEFT_CLICK, wxGridEventHandler( powerctrlgen::onCellLeftClick ), NULL, this );
	m_Boosters->Disconnect( wxEVT_GRID_CELL_RIGHT_CLICK, wxGridEventHandler( powerctrlgen::onCellRightClick ), NULL, this );
	m_On->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( powerctrlgen::OnOn ), NULL, this );
	m_Off->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( powerctrlgen::OnOff ), NULL, this );
	m_sdbSizer2OK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( powerctrlgen::OnOK ), NULL, this );
	
}
