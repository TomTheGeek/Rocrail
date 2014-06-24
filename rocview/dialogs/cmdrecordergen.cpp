///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "cmdrecordergen.h"

///////////////////////////////////////////////////////////////////////////

CmdRecorderGen::CmdRecorderGen( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	m_Record = new wxBitmapButton( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizer2->Add( m_Record, 0, wxALL|wxEXPAND, 5 );
	
	m_Stop = new wxBitmapButton( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizer2->Add( m_Stop, 0, wxALL|wxEXPAND, 5 );
	
	m_Pause = new wxBitmapButton( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizer2->Add( m_Pause, 0, wxALL|wxEXPAND, 5 );
	
	m_Play = new wxBitmapButton( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bSizer2->Add( m_Play, 0, wxALL|wxEXPAND, 5 );
	
	m_labStatus = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_labStatus->Wrap( -1 );
	bSizer2->Add( m_labStatus, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	bSizer1->Add( bSizer2, 0, wxEXPAND, 5 );
	
	m_CmdList = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	m_CmdList->SetMinSize( wxSize( -1,100 ) );
	
	bSizer1->Add( m_CmdList, 1, wxALL|wxEXPAND, 5 );
	
	m_Cmd = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer1->Add( m_Cmd, 0, wxALL|wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 0, 4, 0, 0 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_Modify = new wxButton( this, wxID_ANY, wxT("Modify"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_Modify, 0, wxALL, 5 );
	
	m_Delete = new wxButton( this, wxID_ANY, wxT("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_Delete, 0, wxALL, 5 );
	
	m_Insert = new wxButton( this, wxID_ANY, wxT("Insert"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_Insert, 0, wxALL, 5 );
	
	m_Clear = new wxButton( this, wxID_ANY, wxT("Clear"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_Clear, 0, wxALL, 5 );
	
	m_Export = new wxButton( this, wxID_ANY, wxT("Export..."), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_Export, 0, wxALL, 5 );
	
	m_Import = new wxButton( this, wxID_ANY, wxT("Import..."), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_Import, 0, wxALL, 5 );
	
	m_Stamp = new wxCheckBox( this, wxID_ANY, wxT("Stamp"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_Stamp, 0, wxALL, 5 );
	
	m_CreateRoute = new wxButton( this, wxID_ANY, wxT("Route"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_CreateRoute, 0, wxALL, 5 );
	
	bSizer1->Add( fgSizer1, 0, 0, 5 );
	
	m_StdButtons = new wxStdDialogButtonSizer();
	m_StdButtonsOK = new wxButton( this, wxID_OK );
	m_StdButtons->AddButton( m_StdButtonsOK );
	m_StdButtonsHelp = new wxButton( this, wxID_HELP );
	m_StdButtons->AddButton( m_StdButtonsHelp );
	m_StdButtons->Realize();
	bSizer1->Add( m_StdButtons, 0, wxALIGN_RIGHT|wxALL|wxEXPAND, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	bSizer1->Fit( this );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( CmdRecorderGen::onClose ) );
	m_Record->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CmdRecorderGen::onRecord ), NULL, this );
	m_Stop->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CmdRecorderGen::onStop ), NULL, this );
	m_Pause->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CmdRecorderGen::onPause ), NULL, this );
	m_Play->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CmdRecorderGen::onPlay ), NULL, this );
	m_CmdList->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( CmdRecorderGen::onCmdSelected ), NULL, this );
	m_Modify->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CmdRecorderGen::onModify ), NULL, this );
	m_Delete->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CmdRecorderGen::onDelete ), NULL, this );
	m_Insert->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CmdRecorderGen::onInsert ), NULL, this );
	m_Clear->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CmdRecorderGen::onClear ), NULL, this );
	m_Export->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CmdRecorderGen::onExport ), NULL, this );
	m_Import->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CmdRecorderGen::onImport ), NULL, this );
	m_CreateRoute->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CmdRecorderGen::onCreateRoute ), NULL, this );
	m_StdButtonsHelp->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CmdRecorderGen::onHelp ), NULL, this );
	m_StdButtonsOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CmdRecorderGen::onOK ), NULL, this );
}

CmdRecorderGen::~CmdRecorderGen()
{
	// Disconnect Events
	this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( CmdRecorderGen::onClose ) );
	m_Record->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CmdRecorderGen::onRecord ), NULL, this );
	m_Stop->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CmdRecorderGen::onStop ), NULL, this );
	m_Pause->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CmdRecorderGen::onPause ), NULL, this );
	m_Play->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CmdRecorderGen::onPlay ), NULL, this );
	m_CmdList->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( CmdRecorderGen::onCmdSelected ), NULL, this );
	m_Modify->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CmdRecorderGen::onModify ), NULL, this );
	m_Delete->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CmdRecorderGen::onDelete ), NULL, this );
	m_Insert->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CmdRecorderGen::onInsert ), NULL, this );
	m_Clear->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CmdRecorderGen::onClear ), NULL, this );
	m_Export->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CmdRecorderGen::onExport ), NULL, this );
	m_Import->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CmdRecorderGen::onImport ), NULL, this );
	m_CreateRoute->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CmdRecorderGen::onCreateRoute ), NULL, this );
	m_StdButtonsHelp->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CmdRecorderGen::onHelp ), NULL, this );
	m_StdButtonsOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CmdRecorderGen::onOK ), NULL, this );
	
}
