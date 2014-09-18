///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "variabledlggen.h"

///////////////////////////////////////////////////////////////////////////

VariableDlgGen::VariableDlgGen( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_VarBook = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_IndexPanel = new wxPanel( m_VarBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );
	
	m_VarList = new wxListCtrl( m_IndexPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_HRULES|wxLC_REPORT|wxLC_SINGLE_SEL );
	bSizer2->Add( m_VarList, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );
	
	m_New = new wxButton( m_IndexPanel, wxID_ANY, wxT("New"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_New, 0, wxALL, 5 );
	
	m_Delete = new wxButton( m_IndexPanel, wxID_ANY, wxT("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_Delete, 0, wxALL, 5 );
	
	m_Doc = new wxButton( m_IndexPanel, wxID_ANY, wxT("Documentation"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_Doc, 0, wxALL, 5 );
	
	bSizer2->Add( bSizer3, 0, wxEXPAND, 5 );
	
	m_IndexPanel->SetSizer( bSizer2 );
	m_IndexPanel->Layout();
	bSizer2->Fit( m_IndexPanel );
	m_VarBook->AddPage( m_IndexPanel, wxT("Index"), true );
	m_GeneralPanel = new wxPanel( m_VarBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_labID = new wxStaticText( m_GeneralPanel, wxID_ANY, wxT("ID"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labID->Wrap( -1 );
	fgSizer1->Add( m_labID, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_ID = new wxTextCtrl( m_GeneralPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_ID, 0, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_labGroup = new wxStaticText( m_GeneralPanel, wxID_ANY, wxT("Group"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labGroup->Wrap( -1 );
	fgSizer1->Add( m_labGroup, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_Group = new wxTextCtrl( m_GeneralPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_Group, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	m_labDesc = new wxStaticText( m_GeneralPanel, wxID_ANY, wxT("Description"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labDesc->Wrap( -1 );
	fgSizer1->Add( m_labDesc, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_Desc = new wxTextCtrl( m_GeneralPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_Desc, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	bSizer4->Add( fgSizer1, 0, wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer2->AddGrowableCol( 1 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_labMin = new wxStaticText( m_GeneralPanel, wxID_ANY, wxT("Min. value"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labMin->Wrap( -1 );
	fgSizer2->Add( m_labMin, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_MinValue = new wxSpinCtrl( m_GeneralPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 120,-1 ), wxSP_ARROW_KEYS, -100000, 100000, 0 );
	fgSizer2->Add( m_MinValue, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_labMax = new wxStaticText( m_GeneralPanel, wxID_ANY, wxT("Max. value"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labMax->Wrap( -1 );
	fgSizer2->Add( m_labMax, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_MaxValue = new wxSpinCtrl( m_GeneralPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 120,-1 ), wxSP_ARROW_KEYS, -100000, 100000, 0 );
	fgSizer2->Add( m_MaxValue, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_labText = new wxStaticText( m_GeneralPanel, wxID_ANY, wxT("Text"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labText->Wrap( -1 );
	fgSizer2->Add( m_labText, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_Text = new wxTextCtrl( m_GeneralPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_Text, 0, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_labValue = new wxStaticText( m_GeneralPanel, wxID_ANY, wxT("Value"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labValue->Wrap( -1 );
	fgSizer2->Add( m_labValue, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_Value = new wxSpinCtrl( m_GeneralPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 120,-1 ), wxSP_ARROW_KEYS, -100000, 100000, 0 );
	fgSizer2->Add( m_Value, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	bSizer4->Add( fgSizer2, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxHORIZONTAL );
	
	m_Actions = new wxButton( m_GeneralPanel, wxID_ANY, wxT("Actions..."), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer5->Add( m_Actions, 0, wxALL, 5 );
	
	bSizer4->Add( bSizer5, 0, wxEXPAND, 5 );
	
	m_GeneralPanel->SetSizer( bSizer4 );
	m_GeneralPanel->Layout();
	bSizer4->Fit( m_GeneralPanel );
	m_VarBook->AddPage( m_GeneralPanel, wxT("General"), false );
	
	bSizer1->Add( m_VarBook, 0, wxEXPAND | wxALL, 5 );
	
	m_StandardButtons = new wxStdDialogButtonSizer();
	m_StandardButtonsOK = new wxButton( this, wxID_OK );
	m_StandardButtons->AddButton( m_StandardButtonsOK );
	m_StandardButtonsApply = new wxButton( this, wxID_APPLY );
	m_StandardButtons->AddButton( m_StandardButtonsApply );
	m_StandardButtonsCancel = new wxButton( this, wxID_CANCEL );
	m_StandardButtons->AddButton( m_StandardButtonsCancel );
	m_StandardButtonsHelp = new wxButton( this, wxID_HELP );
	m_StandardButtons->AddButton( m_StandardButtonsHelp );
	m_StandardButtons->Realize();
	bSizer1->Add( m_StandardButtons, 0, wxEXPAND|wxALL, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	bSizer1->Fit( this );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( VariableDlgGen::onClose ) );
	m_VarList->Connect( wxEVT_COMMAND_LIST_COL_CLICK, wxListEventHandler( VariableDlgGen::onListCol ), NULL, this );
	m_VarList->Connect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( VariableDlgGen::onListSelected ), NULL, this );
	m_New->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( VariableDlgGen::onNew ), NULL, this );
	m_Delete->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( VariableDlgGen::onDelete ), NULL, this );
	m_Doc->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( VariableDlgGen::onDoc ), NULL, this );
	m_Actions->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( VariableDlgGen::onActions ), NULL, this );
	m_StandardButtonsApply->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( VariableDlgGen::onApply ), NULL, this );
	m_StandardButtonsCancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( VariableDlgGen::onCancel ), NULL, this );
	m_StandardButtonsHelp->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( VariableDlgGen::onHelp ), NULL, this );
	m_StandardButtonsOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( VariableDlgGen::onOK ), NULL, this );
}

VariableDlgGen::~VariableDlgGen()
{
	// Disconnect Events
	this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( VariableDlgGen::onClose ) );
	m_VarList->Disconnect( wxEVT_COMMAND_LIST_COL_CLICK, wxListEventHandler( VariableDlgGen::onListCol ), NULL, this );
	m_VarList->Disconnect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( VariableDlgGen::onListSelected ), NULL, this );
	m_New->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( VariableDlgGen::onNew ), NULL, this );
	m_Delete->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( VariableDlgGen::onDelete ), NULL, this );
	m_Doc->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( VariableDlgGen::onDoc ), NULL, this );
	m_Actions->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( VariableDlgGen::onActions ), NULL, this );
	m_StandardButtonsApply->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( VariableDlgGen::onApply ), NULL, this );
	m_StandardButtonsCancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( VariableDlgGen::onCancel ), NULL, this );
	m_StandardButtonsHelp->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( VariableDlgGen::onHelp ), NULL, this );
	m_StandardButtonsOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( VariableDlgGen::onOK ), NULL, this );
	
}
