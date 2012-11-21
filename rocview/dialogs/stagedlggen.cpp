///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "stagedlggen.h"

///////////////////////////////////////////////////////////////////////////

stagedlggen::stagedlggen( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );
	
	m_Notebook = new wxNotebook( this, ID_STAGEBOOK, wxDefaultPosition, wxDefaultSize, 0 );
	m_General = new wxPanel( m_Notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer4;
	fgSizer4 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer4->AddGrowableCol( 1 );
	fgSizer4->SetFlexibleDirection( wxBOTH );
	fgSizer4->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_labID = new wxStaticText( m_General, wxID_ANY, wxT("ID"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labID->Wrap( -1 );
	fgSizer4->Add( m_labID, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_ID = new wxTextCtrl( m_General, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer4->Add( m_ID, 0, wxALL|wxEXPAND, 5 );
	
	m_labDescription = new wxStaticText( m_General, wxID_ANY, wxT("Description"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labDescription->Wrap( -1 );
	fgSizer4->Add( m_labDescription, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );
	
	m_Description = new wxTextCtrl( m_General, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer4->Add( m_Description, 0, wxALL|wxEXPAND, 5 );
	
	bSizer7->Add( fgSizer4, 0, wxEXPAND, 5 );
	
	m_staticline1 = new wxStaticLine( m_General, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer7->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );
	
	wxFlexGridSizer* fgSizer6;
	fgSizer6 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer6->AddGrowableCol( 1 );
	fgSizer6->SetFlexibleDirection( wxBOTH );
	fgSizer6->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_labSectionLength = new wxStaticText( m_General, wxID_ANY, wxT("Section length"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labSectionLength->Wrap( -1 );
	fgSizer6->Add( m_labSectionLength, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );
	
	m_SectionLength = new wxSpinCtrl( m_General, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 100,-1 ), wxSP_ARROW_KEYS, 0, 999, 0 );
	fgSizer6->Add( m_SectionLength, 0, wxALL, 5 );
	
	m_labTrainGap = new wxStaticText( m_General, wxID_ANY, wxT("Train gap"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labTrainGap->Wrap( -1 );
	fgSizer6->Add( m_labTrainGap, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_TrainGap = new wxSpinCtrl( m_General, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 100,-1 ), wxSP_ARROW_KEYS, 0, 99, 0 );
	fgSizer6->Add( m_TrainGap, 0, wxALL, 5 );
	
	m_labEnterSensor = new wxStaticText( m_General, wxID_ANY, wxT("Enter"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labEnterSensor->Wrap( -1 );
	fgSizer6->Add( m_labEnterSensor, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_EnterSensor = new wxComboBox( m_General, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	fgSizer6->Add( m_EnterSensor, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	m_labEnterSignal = new wxStaticText( m_General, wxID_ANY, wxT("Enter signal"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labEnterSignal->Wrap( -1 );
	fgSizer6->Add( m_labEnterSignal, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_EnterSignal = new wxComboBox( m_General, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	fgSizer6->Add( m_EnterSignal, 0, wxALL|wxEXPAND, 5 );
	
	m_labExitSignal = new wxStaticText( m_General, wxID_ANY, wxT("Exit signal"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labExitSignal->Wrap( -1 );
	fgSizer6->Add( m_labExitSignal, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_ExitSignal = new wxComboBox( m_General, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	fgSizer6->Add( m_ExitSignal, 0, wxALL|wxEXPAND, 5 );
	
	bSizer7->Add( fgSizer6, 0, wxEXPAND, 5 );
	
	m_Actions = new wxButton( m_General, wxID_ANY, wxT("Actions..."), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer7->Add( m_Actions, 0, wxALL, 5 );
	
	m_General->SetSizer( bSizer7 );
	m_General->Layout();
	bSizer7->Fit( m_General );
	m_Notebook->AddPage( m_General, wxT("General"), false );
	m_Sections = new wxPanel( m_Notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxVERTICAL );
	
	m_SectionList = new wxListBox( m_Sections, wxID_ANY, wxDefaultPosition, wxSize( -1,120 ), 0, NULL, 0 ); 
	bSizer8->Add( m_SectionList, 1, wxALL|wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer5;
	fgSizer5 = new wxFlexGridSizer( 0, 4, 0, 0 );
	fgSizer5->AddGrowableCol( 3 );
	fgSizer5->SetFlexibleDirection( wxBOTH );
	fgSizer5->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_labSectionID = new wxStaticText( m_Sections, wxID_ANY, wxT("ID"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labSectionID->Wrap( -1 );
	fgSizer5->Add( m_labSectionID, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_SectionID = new wxTextCtrl( m_Sections, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer5->Add( m_SectionID, 0, wxALL, 5 );
	
	m_labSectionSensor = new wxStaticText( m_Sections, wxID_ANY, wxT("Sensor"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labSectionSensor->Wrap( -1 );
	fgSizer5->Add( m_labSectionSensor, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );
	
	m_SectionSensor = new wxComboBox( m_Sections, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_DROPDOWN|wxCB_READONLY|wxCB_SIMPLE ); 
	fgSizer5->Add( m_SectionSensor, 0, wxALL|wxEXPAND, 5 );
	
	m_labOwnSectionLength = new wxStaticText( m_Sections, wxID_ANY, wxT("Length"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labOwnSectionLength->Wrap( -1 );
	fgSizer5->Add( m_labOwnSectionLength, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );
	
	m_OwnSectionLength = new wxSpinCtrl( m_Sections, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 100,-1 ), wxSP_ARROW_KEYS, 0, 1000, 0 );
	fgSizer5->Add( m_OwnSectionLength, 0, wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	m_labOccSensor = new wxStaticText( m_Sections, wxID_ANY, wxT("Occupancy"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labOccSensor->Wrap( -1 );
	fgSizer5->Add( m_labOccSensor, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );
	
	m_OccSensor = new wxComboBox( m_Sections, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_DROPDOWN|wxCB_READONLY|wxCB_SIMPLE ); 
	fgSizer5->Add( m_OccSensor, 0, wxALL|wxEXPAND, 5 );
	
	bSizer8->Add( fgSizer5, 0, wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer41;
	fgSizer41 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer41->AddGrowableCol( 1 );
	fgSizer41->SetFlexibleDirection( wxBOTH );
	fgSizer41->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_labSectionLocoId = new wxStaticText( m_Sections, wxID_ANY, wxT("Loco ID"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labSectionLocoId->Wrap( -1 );
	fgSizer41->Add( m_labSectionLocoId, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );
	
	m_SectionLocoId = new wxComboBox( m_Sections, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	fgSizer41->Add( m_SectionLocoId, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	bSizer8->Add( fgSizer41, 0, wxEXPAND, 5 );
	
	wxBoxSizer* m_SectionButtons;
	m_SectionButtons = new wxBoxSizer( wxHORIZONTAL );
	
	m_AddSection = new wxButton( m_Sections, wxID_ANY, wxT("Add"), wxDefaultPosition, wxDefaultSize, 0 );
	m_SectionButtons->Add( m_AddSection, 0, wxALL, 5 );
	
	m_ModifySection = new wxButton( m_Sections, wxID_ANY, wxT("Modify"), wxDefaultPosition, wxDefaultSize, 0 );
	m_SectionButtons->Add( m_ModifySection, 0, wxALL, 5 );
	
	m_DeleteSection = new wxButton( m_Sections, wxID_ANY, wxT("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
	m_SectionButtons->Add( m_DeleteSection, 0, wxALL, 5 );
	
	m_SetLoco = new wxButton( m_Sections, wxID_ANY, wxT("Set"), wxDefaultPosition, wxDefaultSize, 0 );
	m_SectionButtons->Add( m_SetLoco, 0, wxALL, 5 );
	
	m_FreeSection = new wxButton( m_Sections, wxID_ANY, wxT("Free"), wxDefaultPosition, wxDefaultSize, 0 );
	m_SectionButtons->Add( m_FreeSection, 0, wxALL, 5 );
	
	m_FreeAll = new wxButton( m_Sections, wxID_ANY, wxT("Free all"), wxDefaultPosition, wxDefaultSize, 0 );
	m_SectionButtons->Add( m_FreeAll, 0, wxALL, 5 );
	
	bSizer8->Add( m_SectionButtons, 0, wxEXPAND, 5 );
	
	m_Sections->SetSizer( bSizer8 );
	m_Sections->Layout();
	bSizer8->Fit( m_Sections );
	m_Notebook->AddPage( m_Sections, wxT("Sections"), false );
	m_Details = new wxPanel( m_Notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer51;
	fgSizer51 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer51->AddGrowableCol( 1 );
	fgSizer51->SetFlexibleDirection( wxBOTH );
	fgSizer51->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_labDepartDelay = new wxStaticText( m_Details, wxID_ANY, wxT("Depart delay"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labDepartDelay->Wrap( -1 );
	fgSizer51->Add( m_labDepartDelay, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );
	
	m_DepartDelay = new wxSpinCtrl( m_Details, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 100,-1 ), wxSP_ARROW_KEYS, 0, 100, 0 );
	fgSizer51->Add( m_DepartDelay, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_SuitsWell = new wxCheckBox( m_Details, wxID_ANY, wxT("Best choice"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer51->Add( m_SuitsWell, 0, wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	m_InAtLen = new wxCheckBox( m_Details, wxID_ANY, wxT("IN event on train length"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer51->Add( m_InAtLen, 0, wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	bSizer5->Add( fgSizer51, 0, 0, 5 );
	
	m_WaitBox = new wxStaticBoxSizer( new wxStaticBox( m_Details, wxID_ANY, wxT("Wait") ), wxVERTICAL );
	
	wxBoxSizer* bSizer61;
	bSizer61 = new wxBoxSizer( wxHORIZONTAL );
	
	wxString m_WaitTypeChoices[] = { wxT("Random"), wxT("Fixed"), wxT("Loco"), wxT("No") };
	int m_WaitTypeNChoices = sizeof( m_WaitTypeChoices ) / sizeof( wxString );
	m_WaitType = new wxRadioBox( m_Details, wxID_ANY, wxT("Type"), wxDefaultPosition, wxDefaultSize, m_WaitTypeNChoices, m_WaitTypeChoices, 1, wxRA_SPECIFY_COLS );
	m_WaitType->SetSelection( 0 );
	bSizer61->Add( m_WaitType, 0, wxALL, 5 );
	
	wxFlexGridSizer* fgSizer7;
	fgSizer7 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer7->AddGrowableCol( 1 );
	fgSizer7->SetFlexibleDirection( wxBOTH );
	fgSizer7->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_labRandomMin = new wxStaticText( m_Details, wxID_ANY, wxT("Random min."), wxDefaultPosition, wxDefaultSize, 0 );
	m_labRandomMin->Wrap( -1 );
	fgSizer7->Add( m_labRandomMin, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_RandomMin = new wxSpinCtrl( m_Details, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 100,-1 ), wxSP_ARROW_KEYS, 0, 1000, 0 );
	fgSizer7->Add( m_RandomMin, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_labRandomMax = new wxStaticText( m_Details, wxID_ANY, wxT("Random max."), wxDefaultPosition, wxDefaultSize, 0 );
	m_labRandomMax->Wrap( -1 );
	fgSizer7->Add( m_labRandomMax, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );
	
	m_RandomMax = new wxSpinCtrl( m_Details, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 100,-1 ), wxSP_ARROW_KEYS, 0, 10000, 0 );
	fgSizer7->Add( m_RandomMax, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_labFixed = new wxStaticText( m_Details, wxID_ANY, wxT("Fixed"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labFixed->Wrap( -1 );
	fgSizer7->Add( m_labFixed, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );
	
	m_Fixed = new wxSpinCtrl( m_Details, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 100,-1 ), wxSP_ARROW_KEYS, 0, 1000, 0 );
	fgSizer7->Add( m_Fixed, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	bSizer61->Add( fgSizer7, 1, wxEXPAND, 5 );
	
	m_WaitBox->Add( bSizer61, 1, wxEXPAND, 5 );
	
	bSizer5->Add( m_WaitBox, 0, wxEXPAND, 5 );
	
	m_SpeedBox = new wxStaticBoxSizer( new wxStaticBox( m_Details, wxID_ANY, wxT("Speed") ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizer71;
	fgSizer71 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer71->SetFlexibleDirection( wxBOTH );
	fgSizer71->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxString m_ArriveSpeedChoices[] = { wxT("min"), wxT("%") };
	int m_ArriveSpeedNChoices = sizeof( m_ArriveSpeedChoices ) / sizeof( wxString );
	m_ArriveSpeed = new wxRadioBox( m_Details, wxID_ANY, wxT("Arrive"), wxDefaultPosition, wxDefaultSize, m_ArriveSpeedNChoices, m_ArriveSpeedChoices, 1, wxRA_SPECIFY_ROWS );
	m_ArriveSpeed->SetSelection( 0 );
	fgSizer71->Add( m_ArriveSpeed, 0, wxALL, 5 );
	
	m_ArriveSpeedPercent = new wxSpinCtrl( m_Details, wxID_ANY, wxT("10"), wxDefaultPosition, wxSize( 80,-1 ), wxSP_ARROW_KEYS, 0, 100, 10 );
	fgSizer71->Add( m_ArriveSpeedPercent, 0, wxALIGN_BOTTOM|wxTOP|wxBOTTOM|wxRIGHT, 5 );
	
	m_SpeedBox->Add( fgSizer71, 0, 0, 5 );
	
	wxString m_ExitSpeedChoices[] = { wxT("min"), wxT("mid"), wxT("cruise"), wxT("max") };
	int m_ExitSpeedNChoices = sizeof( m_ExitSpeedChoices ) / sizeof( wxString );
	m_ExitSpeed = new wxRadioBox( m_Details, wxID_ANY, wxT("Departure"), wxDefaultPosition, wxDefaultSize, m_ExitSpeedNChoices, m_ExitSpeedChoices, 1, wxRA_SPECIFY_ROWS );
	m_ExitSpeed->SetSelection( 0 );
	m_SpeedBox->Add( m_ExitSpeed, 0, wxALL, 5 );
	
	bSizer5->Add( m_SpeedBox, 0, wxEXPAND, 5 );
	
	m_Details->SetSizer( bSizer5 );
	m_Details->Layout();
	bSizer5->Fit( m_Details );
	m_Notebook->AddPage( m_Details, wxT("Details"), true );
	
	bSizer6->Add( m_Notebook, 1, wxEXPAND | wxALL, 5 );
	
	m_stdButton = new wxStdDialogButtonSizer();
	m_stdButtonOK = new wxButton( this, wxID_OK );
	m_stdButton->AddButton( m_stdButtonOK );
	m_stdButtonCancel = new wxButton( this, wxID_CANCEL );
	m_stdButton->AddButton( m_stdButtonCancel );
	m_stdButton->Realize();
	bSizer6->Add( m_stdButton, 0, wxEXPAND|wxALL, 5 );
	
	this->SetSizer( bSizer6 );
	this->Layout();
	bSizer6->Fit( this );
	
	// Connect Events
	m_Actions->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( stagedlggen::onActions ), NULL, this );
	m_SectionList->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( stagedlggen::OnSectionList ), NULL, this );
	m_AddSection->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( stagedlggen::OnSectionAdd ), NULL, this );
	m_ModifySection->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( stagedlggen::OnSectionModify ), NULL, this );
	m_DeleteSection->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( stagedlggen::OnSectionDelete ), NULL, this );
	m_SetLoco->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( stagedlggen::OnSetLoco ), NULL, this );
	m_FreeSection->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( stagedlggen::OnFreeSection ), NULL, this );
	m_FreeAll->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( stagedlggen::OnFreeAll ), NULL, this );
	m_stdButtonCancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( stagedlggen::OnCancel ), NULL, this );
	m_stdButtonOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( stagedlggen::OnOK ), NULL, this );
}

stagedlggen::~stagedlggen()
{
	// Disconnect Events
	m_Actions->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( stagedlggen::onActions ), NULL, this );
	m_SectionList->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( stagedlggen::OnSectionList ), NULL, this );
	m_AddSection->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( stagedlggen::OnSectionAdd ), NULL, this );
	m_ModifySection->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( stagedlggen::OnSectionModify ), NULL, this );
	m_DeleteSection->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( stagedlggen::OnSectionDelete ), NULL, this );
	m_SetLoco->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( stagedlggen::OnSetLoco ), NULL, this );
	m_FreeSection->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( stagedlggen::OnFreeSection ), NULL, this );
	m_FreeAll->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( stagedlggen::OnFreeAll ), NULL, this );
	m_stdButtonCancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( stagedlggen::OnCancel ), NULL, this );
	m_stdButtonOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( stagedlggen::OnOK ), NULL, this );
	
}
