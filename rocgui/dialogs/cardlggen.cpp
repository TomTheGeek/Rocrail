///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 21 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "cardlggen.h"

///////////////////////////////////////////////////////////////////////////

CarDlg::CarDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_CarBook = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_IndexPanel = new wxPanel( m_CarBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );
	
	m_CarImage = new wxBitmapButton( m_IndexPanel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 280,80 ), wxBU_AUTODRAW );
	bSizer2->Add( m_CarImage, 0, wxALL, 5 );
	
	m_CarList = new wxListBox( m_IndexPanel, wxID_ANY, wxDefaultPosition, wxSize( -1,200 ), 0, NULL, 0 ); 
	bSizer2->Add( m_CarList, 0, wxALL|wxEXPAND, 5 );
	
	m_IndexPanel->SetSizer( bSizer2 );
	m_IndexPanel->Layout();
	bSizer2->Fit( m_IndexPanel );
	m_CarBook->AddPage( m_IndexPanel, wxT("Index"), false );
	m_GeneralPanel = new wxPanel( m_CarBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_labID = new wxStaticText( m_GeneralPanel, wxID_ANY, wxT("ID"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labID->Wrap( -1 );
	fgSizer1->Add( m_labID, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_ID = new wxTextCtrl( m_GeneralPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_ID, 0, wxALL|wxEXPAND, 5 );
	
	bSizer3->Add( fgSizer1, 1, wxEXPAND, 5 );
	
	m_GeneralPanel->SetSizer( bSizer3 );
	m_GeneralPanel->Layout();
	bSizer3->Fit( m_GeneralPanel );
	m_CarBook->AddPage( m_GeneralPanel, wxT("General"), false );
	
	bSizer1->Add( m_CarBook, 1, wxEXPAND | wxALL, 5 );
	
	m_sdbSizer1 = new wxStdDialogButtonSizer();
	m_sdbSizer1OK = new wxButton( this, wxID_OK );
	m_sdbSizer1->AddButton( m_sdbSizer1OK );
	m_sdbSizer1Apply = new wxButton( this, wxID_APPLY );
	m_sdbSizer1->AddButton( m_sdbSizer1Apply );
	m_sdbSizer1Cancel = new wxButton( this, wxID_CANCEL );
	m_sdbSizer1->AddButton( m_sdbSizer1Cancel );
	m_sdbSizer1->Realize();
	bSizer1->Add( m_sdbSizer1, 0, wxALIGN_RIGHT|wxALL, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	bSizer1->Fit( this );
	
	// Connect Events
	m_CarImage->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CarDlg::onCarImage ), NULL, this );
	m_CarList->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( CarDlg::onCarList ), NULL, this );
	m_sdbSizer1Apply->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CarDlg::onApply ), NULL, this );
	m_sdbSizer1Cancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CarDlg::onCancel ), NULL, this );
	m_sdbSizer1OK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CarDlg::onOK ), NULL, this );
}

CarDlg::~CarDlg()
{
	// Disconnect Events
	m_CarImage->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CarDlg::onCarImage ), NULL, this );
	m_CarList->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( CarDlg::onCarList ), NULL, this );
	m_sdbSizer1Apply->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CarDlg::onApply ), NULL, this );
	m_sdbSizer1Cancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CarDlg::onCancel ), NULL, this );
	m_sdbSizer1OK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CarDlg::onOK ), NULL, this );
}
