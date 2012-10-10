///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "rocprodlggen.h"

///////////////////////////////////////////////////////////////////////////

RocProDlgGen::RocProDlgGen( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer( wxVERTICAL );
	
	m_DecTree = new wxTreeCtrl( this, wxID_ANY, wxDefaultPosition, wxSize( -1,300 ), wxTR_DEFAULT_STYLE|wxRAISED_BORDER );
	bSizer10->Add( m_DecTree, 1, wxALL|wxEXPAND, 5 );
	
	m_Info = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( -1,100 ), wxTE_MULTILINE|wxTE_READONLY );
	bSizer10->Add( m_Info, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	bSizer8->Add( bSizer10, 1, wxEXPAND, 5 );
	
	m_staticline2 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL );
	bSizer8->Add( m_staticline2, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxVERTICAL );
	
	m_LocoImage = new wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer9->Add( m_LocoImage, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_LocoList = new wxComboBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	bSizer9->Add( m_LocoList, 0, wxALL|wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer8;
	fgSizer8 = new wxFlexGridSizer( 0, 4, 0, 0 );
	fgSizer8->AddGrowableCol( 1 );
	fgSizer8->SetFlexibleDirection( wxBOTH );
	fgSizer8->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_labFile = new wxStaticText( this, wxID_ANY, wxT("Decoder"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labFile->Wrap( -1 );
	fgSizer8->Add( m_labFile, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_DecFile = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 200,-1 ), wxTE_READONLY );
	fgSizer8->Add( m_DecFile, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	m_DecFileOpen = new wxButton( this, wxID_ANY, wxT("Open..."), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer8->Add( m_DecFileOpen, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL|wxTOP|wxBOTTOM|wxRIGHT, 5 );
	
	m_SaveAs = new wxButton( this, wxID_ANY, wxT("Save..."), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer8->Add( m_SaveAs, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL|wxTOP|wxBOTTOM|wxRIGHT, 5 );
	
	bSizer9->Add( fgSizer8, 0, wxEXPAND|wxTOP, 5 );
	
	wxStaticBoxSizer* sbSizer3;
	sbSizer3 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("CV") ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizer9;
	fgSizer9 = new wxFlexGridSizer( 0, 10, 0, 0 );
	fgSizer9->SetFlexibleDirection( wxBOTH );
	fgSizer9->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_labNr = new wxStaticText( this, wxID_ANY, wxT("Nr"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labNr->Wrap( -1 );
	fgSizer9->Add( m_labNr, 0, wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_labValue = new wxStaticText( this, wxID_ANY, wxT("Value"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labValue->Wrap( -1 );
	fgSizer9->Add( m_labValue, 0, wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_staticText4 = new wxStaticText( this, wxID_ANY, wxT("7"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	fgSizer9->Add( m_staticText4, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL|wxTOP|wxLEFT, 5 );
	
	m_staticText5 = new wxStaticText( this, wxID_ANY, wxT("6"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	fgSizer9->Add( m_staticText5, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL|wxTOP, 5 );
	
	m_staticText6 = new wxStaticText( this, wxID_ANY, wxT("5"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText6->Wrap( -1 );
	fgSizer9->Add( m_staticText6, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL|wxTOP, 5 );
	
	m_staticText7 = new wxStaticText( this, wxID_ANY, wxT("4"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText7->Wrap( -1 );
	fgSizer9->Add( m_staticText7, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL|wxTOP, 5 );
	
	m_staticText8 = new wxStaticText( this, wxID_ANY, wxT("3"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText8->Wrap( -1 );
	fgSizer9->Add( m_staticText8, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL|wxTOP, 5 );
	
	m_staticText9 = new wxStaticText( this, wxID_ANY, wxT("2"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText9->Wrap( -1 );
	fgSizer9->Add( m_staticText9, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL|wxTOP, 5 );
	
	m_staticText10 = new wxStaticText( this, wxID_ANY, wxT("1"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText10->Wrap( -1 );
	fgSizer9->Add( m_staticText10, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL|wxTOP, 5 );
	
	m_staticText11 = new wxStaticText( this, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText11->Wrap( -1 );
	fgSizer9->Add( m_staticText11, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL|wxTOP, 5 );
	
	m_Nr = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 50,-1 ), wxTE_CENTRE|wxTE_READONLY );
	fgSizer9->Add( m_Nr, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_Value = new wxSpinCtrl( this, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 80,-1 ), wxSP_ARROW_KEYS|wxSP_WRAP, 0, 255, 0 );
	fgSizer9->Add( m_Value, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_Bit7 = new wxCheckBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer9->Add( m_Bit7, 0, wxTOP|wxBOTTOM|wxLEFT|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_Bit6 = new wxCheckBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer9->Add( m_Bit6, 0, wxTOP|wxBOTTOM|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_Bit5 = new wxCheckBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer9->Add( m_Bit5, 0, wxTOP|wxBOTTOM|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_Bit4 = new wxCheckBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer9->Add( m_Bit4, 0, wxTOP|wxBOTTOM|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_Bit3 = new wxCheckBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer9->Add( m_Bit3, 0, wxTOP|wxBOTTOM|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_Bit2 = new wxCheckBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer9->Add( m_Bit2, 0, wxTOP|wxBOTTOM|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_Bit1 = new wxCheckBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer9->Add( m_Bit1, 0, wxTOP|wxBOTTOM|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_Bit0 = new wxCheckBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer9->Add( m_Bit0, 0, wxTOP|wxBOTTOM|wxALIGN_CENTER_VERTICAL, 5 );
	
	sbSizer3->Add( fgSizer9, 1, wxEXPAND, 5 );
	
	m_ValueSlider = new wxSlider( this, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL );
	sbSizer3->Add( m_ValueSlider, 0, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxHORIZONTAL );
	
	m_ReadCV = new wxButton( this, wxID_ANY, wxT("Read"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer5->Add( m_ReadCV, 0, wxALL, 5 );
	
	m_WriteCV = new wxButton( this, wxID_ANY, wxT("Write"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer5->Add( m_WriteCV, 0, wxALL, 5 );
	
	m_POM = new wxCheckBox( this, wxID_ANY, wxT("PoM"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer5->Add( m_POM, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	sbSizer3->Add( bSizer5, 0, 0, 5 );
	
	bSizer9->Add( sbSizer3, 0, wxEXPAND|wxALL, 5 );
	
	wxStaticBoxSizer* m_Special;
	m_Special = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Special") ), wxVERTICAL );
	
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxHORIZONTAL );
	
	m_VCurve = new wxButton( this, wxID_ANY, wxT("V Curve..."), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer6->Add( m_VCurve, 0, wxALL, 5 );
	
	m_CV29 = new wxButton( this, wxID_ANY, wxT("Config..."), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer6->Add( m_CV29, 0, wxALL, 5 );
	
	m_Special->Add( bSizer6, 1, wxEXPAND, 5 );
	
	bSizer9->Add( m_Special, 0, wxEXPAND|wxALL, 5 );
	
	bSizer8->Add( bSizer9, 1, wxEXPAND, 5 );
	
	bSizer7->Add( bSizer8, 1, wxEXPAND, 5 );
	
	m_sdbSizer2 = new wxStdDialogButtonSizer();
	m_sdbSizer2OK = new wxButton( this, wxID_OK );
	m_sdbSizer2->AddButton( m_sdbSizer2OK );
	m_sdbSizer2->Realize();
	bSizer7->Add( m_sdbSizer2, 0, wxEXPAND|wxALL, 5 );
	
	this->SetSizer( bSizer7 );
	this->Layout();
	bSizer7->Fit( this );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( RocProDlgGen::onClose ) );
	m_DecTree->Connect( wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler( RocProDlgGen::onTreeSelChanged ), NULL, this );
	m_LocoList->Connect( wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler( RocProDlgGen::onLocoList ), NULL, this );
	m_DecFileOpen->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( RocProDlgGen::onOpen ), NULL, this );
	m_SaveAs->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( RocProDlgGen::onSaveAs ), NULL, this );
	m_Value->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( RocProDlgGen::onValue ), NULL, this );
	m_Value->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( RocProDlgGen::onValueText ), NULL, this );
	m_Bit7->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( RocProDlgGen::onBit ), NULL, this );
	m_Bit6->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( RocProDlgGen::onBit ), NULL, this );
	m_Bit5->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( RocProDlgGen::onBit ), NULL, this );
	m_Bit4->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( RocProDlgGen::onBit ), NULL, this );
	m_Bit3->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( RocProDlgGen::onBit ), NULL, this );
	m_Bit2->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( RocProDlgGen::onBit ), NULL, this );
	m_Bit1->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( RocProDlgGen::onBit ), NULL, this );
	m_Bit0->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( RocProDlgGen::onBit ), NULL, this );
	m_ValueSlider->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( RocProDlgGen::onValueSlider ), NULL, this );
	m_ReadCV->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( RocProDlgGen::onRead ), NULL, this );
	m_WriteCV->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( RocProDlgGen::onWrite ), NULL, this );
	m_VCurve->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( RocProDlgGen::onVCurve ), NULL, this );
	m_CV29->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( RocProDlgGen::onConfig ), NULL, this );
	m_sdbSizer2OK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( RocProDlgGen::onOK ), NULL, this );
}

RocProDlgGen::~RocProDlgGen()
{
	// Disconnect Events
	this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( RocProDlgGen::onClose ) );
	m_DecTree->Disconnect( wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler( RocProDlgGen::onTreeSelChanged ), NULL, this );
	m_LocoList->Disconnect( wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler( RocProDlgGen::onLocoList ), NULL, this );
	m_DecFileOpen->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( RocProDlgGen::onOpen ), NULL, this );
	m_SaveAs->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( RocProDlgGen::onSaveAs ), NULL, this );
	m_Value->Disconnect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( RocProDlgGen::onValue ), NULL, this );
	m_Value->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( RocProDlgGen::onValueText ), NULL, this );
	m_Bit7->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( RocProDlgGen::onBit ), NULL, this );
	m_Bit6->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( RocProDlgGen::onBit ), NULL, this );
	m_Bit5->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( RocProDlgGen::onBit ), NULL, this );
	m_Bit4->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( RocProDlgGen::onBit ), NULL, this );
	m_Bit3->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( RocProDlgGen::onBit ), NULL, this );
	m_Bit2->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( RocProDlgGen::onBit ), NULL, this );
	m_Bit1->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( RocProDlgGen::onBit ), NULL, this );
	m_Bit0->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( RocProDlgGen::onBit ), NULL, this );
	m_ValueSlider->Disconnect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( RocProDlgGen::onValueSlider ), NULL, this );
	m_ReadCV->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( RocProDlgGen::onRead ), NULL, this );
	m_WriteCV->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( RocProDlgGen::onWrite ), NULL, this );
	m_VCurve->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( RocProDlgGen::onVCurve ), NULL, this );
	m_CV29->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( RocProDlgGen::onConfig ), NULL, this );
	m_sdbSizer2OK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( RocProDlgGen::onOK ), NULL, this );
	
}
