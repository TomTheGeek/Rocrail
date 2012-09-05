///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "toursdlggen.h"

///////////////////////////////////////////////////////////////////////////

toursdlggen::toursdlggen( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );
	
	m_TourBook = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_IndexPanel = new wxPanel( m_TourBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );
	
	m_TourList = new wxListBox( m_IndexPanel, wxID_ANY, wxDefaultPosition, wxSize( -1,180 ), 0, NULL, 0 ); 
	bSizer6->Add( m_TourList, 0, wxALL|wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer4;
	fgSizer4 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer4->AddGrowableCol( 1 );
	fgSizer4->SetFlexibleDirection( wxBOTH );
	fgSizer4->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_labTourID = new wxStaticText( m_IndexPanel, wxID_ANY, wxT("ID"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labTourID->Wrap( -1 );
	fgSizer4->Add( m_labTourID, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );
	
	m_ID = new wxTextCtrl( m_IndexPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer4->Add( m_ID, 0, wxALL|wxEXPAND, 5 );
	
	bSizer6->Add( fgSizer4, 0, wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 0, 3, 0, 0 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_ShowAll = new wxCheckBox( m_IndexPanel, wxID_ANY, wxT("Show all"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_ShowAll, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_labStartBlock = new wxStaticText( m_IndexPanel, wxID_ANY, wxT("Start block"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labStartBlock->Wrap( -1 );
	fgSizer2->Add( m_labStartBlock, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxTOP|wxBOTTOM|wxLEFT, 5 );
	
	m_StartBlockID = new wxTextCtrl( m_IndexPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_StartBlockID->Enable( false );
	
	fgSizer2->Add( m_StartBlockID, 0, wxALL|wxEXPAND, 5 );
	
	m_Recycle = new wxCheckBox( m_IndexPanel, wxID_ANY, wxT("Recycle"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_Recycle, 0, wxBOTTOM|wxRIGHT|wxLEFT|wxALIGN_CENTER_VERTICAL, 5 );
	
	bSizer6->Add( fgSizer2, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxHORIZONTAL );
	
	m_AddTour = new wxButton( m_IndexPanel, wxID_ANY, wxT("Add"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer7->Add( m_AddTour, 0, wxALL, 5 );
	
	m_ModifyTour = new wxButton( m_IndexPanel, wxID_ANY, wxT("Modify"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer7->Add( m_ModifyTour, 0, wxALL, 5 );
	
	m_DeleteTour = new wxButton( m_IndexPanel, wxID_ANY, wxT("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer7->Add( m_DeleteTour, 0, wxALL, 5 );
	
	bSizer6->Add( bSizer7, 1, wxEXPAND, 5 );
	
	m_IndexPanel->SetSizer( bSizer6 );
	m_IndexPanel->Layout();
	bSizer6->Fit( m_IndexPanel );
	m_TourBook->AddPage( m_IndexPanel, wxT("Index"), true );
	m_SchedulesPanel = new wxPanel( m_TourBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxVERTICAL );
	
	m_EntryList = new wxListBox( m_SchedulesPanel, wxID_ANY, wxDefaultPosition, wxSize( -1,-1 ), 0, NULL, 0 ); 
	bSizer9->Add( m_EntryList, 1, wxALL|wxEXPAND, 5 );
	
	m_ScheduleList = new wxComboBox( m_SchedulesPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	bSizer9->Add( m_ScheduleList, 0, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer( wxHORIZONTAL );
	
	m_EntryAdd = new wxButton( m_SchedulesPanel, wxID_ANY, wxT("Add"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer10->Add( m_EntryAdd, 0, wxALL, 5 );
	
	m_EntryDelete = new wxButton( m_SchedulesPanel, wxID_ANY, wxT("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer10->Add( m_EntryDelete, 0, wxALL, 5 );
	
	bSizer9->Add( bSizer10, 0, wxEXPAND, 5 );
	
	m_SchedulesPanel->SetSizer( bSizer9 );
	m_SchedulesPanel->Layout();
	bSizer9->Fit( m_SchedulesPanel );
	m_TourBook->AddPage( m_SchedulesPanel, wxT("Schedules"), false );
	
	bSizer5->Add( m_TourBook, 1, wxEXPAND | wxALL, 5 );
	
	m_DefaultButtons = new wxStdDialogButtonSizer();
	m_DefaultButtonsOK = new wxButton( this, wxID_OK );
	m_DefaultButtons->AddButton( m_DefaultButtonsOK );
	m_DefaultButtonsApply = new wxButton( this, wxID_APPLY );
	m_DefaultButtons->AddButton( m_DefaultButtonsApply );
	m_DefaultButtonsCancel = new wxButton( this, wxID_CANCEL );
	m_DefaultButtons->AddButton( m_DefaultButtonsCancel );
	m_DefaultButtons->Realize();
	bSizer5->Add( m_DefaultButtons, 0, wxEXPAND|wxALL, 5 );
	
	this->SetSizer( bSizer5 );
	this->Layout();
	bSizer5->Fit( this );
	
	// Connect Events
	m_TourList->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( toursdlggen::onTourSelect ), NULL, this );
	m_TourList->Connect( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler( toursdlggen::onTourSelect ), NULL, this );
	m_ShowAll->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( toursdlggen::onShowAll ), NULL, this );
	m_AddTour->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( toursdlggen::onAddTour ), NULL, this );
	m_ModifyTour->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( toursdlggen::onModifyTour ), NULL, this );
	m_DeleteTour->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( toursdlggen::onDeleteTour ), NULL, this );
	m_EntryList->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( toursdlggen::onEntrySelect ), NULL, this );
	m_ScheduleList->Connect( wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler( toursdlggen::onScheduleSelect ), NULL, this );
	m_EntryAdd->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( toursdlggen::onEntryAdd ), NULL, this );
	m_EntryDelete->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( toursdlggen::onEntryDelete ), NULL, this );
	m_DefaultButtonsApply->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( toursdlggen::onApply ), NULL, this );
	m_DefaultButtonsCancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( toursdlggen::onCancel ), NULL, this );
	m_DefaultButtonsOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( toursdlggen::onOK ), NULL, this );
}

toursdlggen::~toursdlggen()
{
	// Disconnect Events
	m_TourList->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( toursdlggen::onTourSelect ), NULL, this );
	m_TourList->Disconnect( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler( toursdlggen::onTourSelect ), NULL, this );
	m_ShowAll->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( toursdlggen::onShowAll ), NULL, this );
	m_AddTour->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( toursdlggen::onAddTour ), NULL, this );
	m_ModifyTour->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( toursdlggen::onModifyTour ), NULL, this );
	m_DeleteTour->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( toursdlggen::onDeleteTour ), NULL, this );
	m_EntryList->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( toursdlggen::onEntrySelect ), NULL, this );
	m_ScheduleList->Disconnect( wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler( toursdlggen::onScheduleSelect ), NULL, this );
	m_EntryAdd->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( toursdlggen::onEntryAdd ), NULL, this );
	m_EntryDelete->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( toursdlggen::onEntryDelete ), NULL, this );
	m_DefaultButtonsApply->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( toursdlggen::onApply ), NULL, this );
	m_DefaultButtonsCancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( toursdlggen::onCancel ), NULL, this );
	m_DefaultButtonsOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( toursdlggen::onOK ), NULL, this );
	
}
