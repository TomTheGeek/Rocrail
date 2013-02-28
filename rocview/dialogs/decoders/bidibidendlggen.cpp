///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "bidibidendlggen.h"

///////////////////////////////////////////////////////////////////////////

BidibIdentDlgGen::BidibIdentDlgGen( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxVERTICAL );
	
	m_Notebook = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_IndexPanel = new wxPanel( m_Notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	m_Tree = new wxTreeCtrl( m_IndexPanel, wxID_ANY, wxDefaultPosition, wxSize( 200,-1 ), wxTR_DEFAULT_STYLE|wxTR_SINGLE|wxSUNKEN_BORDER );
	bSizer2->Add( m_Tree, 0, wxALL|wxEXPAND, 5 );
	
	m_staticline1 = new wxStaticLine( m_IndexPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL );
	bSizer2->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	m_BiDiBlogo = new wxStaticBitmap( m_IndexPanel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_BiDiBlogo, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
	
	wxFlexGridSizer* fgSizer8;
	fgSizer8 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer8->AddGrowableCol( 1 );
	fgSizer8->SetFlexibleDirection( wxBOTH );
	fgSizer8->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_labPath = new wxStaticText( m_IndexPanel, wxID_ANY, wxT("Path"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labPath->Wrap( -1 );
	fgSizer8->Add( m_labPath, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_Path = new wxTextCtrl( m_IndexPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	fgSizer8->Add( m_Path, 0, wxEXPAND|wxALL, 5 );
	
	m_labVendor = new wxStaticText( m_IndexPanel, wxID_ANY, wxT("Vendor"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labVendor->Wrap( -1 );
	fgSizer8->Add( m_labVendor, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxFlexGridSizer* fgSizer16;
	fgSizer16 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer16->AddGrowableCol( 1 );
	fgSizer16->SetFlexibleDirection( wxBOTH );
	fgSizer16->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_VID = new wxTextCtrl( m_IndexPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 60,-1 ), 0 );
	fgSizer16->Add( m_VID, 0, wxALL, 5 );
	
	m_VendorName = new wxTextCtrl( m_IndexPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( -1,-1 ), wxTE_READONLY );
	fgSizer16->Add( m_VendorName, 0, wxEXPAND|wxALL, 5 );
	
	fgSizer8->Add( fgSizer16, 1, wxEXPAND, 5 );
	
	m_labUID = new wxStaticText( m_IndexPanel, wxID_ANY, wxT("Unique-ID"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labUID->Wrap( -1 );
	fgSizer8->Add( m_labUID, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxFlexGridSizer* fgSizer6;
	fgSizer6 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer6->SetFlexibleDirection( wxBOTH );
	fgSizer6->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_UIDX = new wxTextCtrl( m_IndexPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 100,-1 ), wxTE_READONLY );
	fgSizer6->Add( m_UIDX, 0, wxALL, 5 );
	
	m_UID = new wxTextCtrl( m_IndexPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 100,-1 ), wxTE_READONLY );
	fgSizer6->Add( m_UID, 0, wxEXPAND|wxALL, 5 );
	
	fgSizer8->Add( fgSizer6, 0, wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_labClass = new wxStaticText( m_IndexPanel, wxID_ANY, wxT("Class"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labClass->Wrap( -1 );
	fgSizer8->Add( m_labClass, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_Class = new wxTextCtrl( m_IndexPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 200,-1 ), wxTE_READONLY );
	fgSizer8->Add( m_Class, 0, wxEXPAND|wxALL, 5 );
	
	m_labVersion = new wxStaticText( m_IndexPanel, wxID_ANY, wxT("Version"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labVersion->Wrap( -1 );
	fgSizer8->Add( m_labVersion, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_Version = new wxTextCtrl( m_IndexPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_CENTRE|wxTE_READONLY );
	fgSizer8->Add( m_Version, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	bSizer4->Add( fgSizer8, 0, wxEXPAND, 5 );
	
	bSizer2->Add( bSizer4, 1, wxEXPAND, 5 );
	
	m_IndexPanel->SetSizer( bSizer2 );
	m_IndexPanel->Layout();
	bSizer2->Fit( m_IndexPanel );
	m_Notebook->AddPage( m_IndexPanel, wxT("Index"), false );
	m_FeaturesPanel = new wxPanel( m_Notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxHORIZONTAL );
	
	m_FeatureList = new wxListBox( m_FeaturesPanel, wxID_ANY, wxDefaultPosition, wxSize( 200,-1 ), 0, NULL, wxLB_HSCROLL ); 
	bSizer8->Add( m_FeatureList, 0, wxALL|wxEXPAND, 5 );
	
	m_staticline2 = new wxStaticLine( m_FeaturesPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL );
	bSizer8->Add( m_staticline2, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer3;
	fgSizer3 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer3->AddGrowableCol( 1 );
	fgSizer3->SetFlexibleDirection( wxBOTH );
	fgSizer3->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_labIID = new wxStaticText( m_FeaturesPanel, wxID_ANY, wxT("IID"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labIID->Wrap( -1 );
	fgSizer3->Add( m_labIID, 0, wxALL, 5 );
	
	m_IID = new wxTextCtrl( m_FeaturesPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer3->Add( m_IID, 0, wxALL|wxEXPAND, 5 );
	
	m_staticline3 = new wxStaticLine( m_FeaturesPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	fgSizer3->Add( m_staticline3, 0, wxEXPAND | wxALL, 5 );
	
	m_staticline4 = new wxStaticLine( m_FeaturesPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	fgSizer3->Add( m_staticline4, 0, wxEXPAND | wxALL, 5 );
	
	m_labFeature = new wxStaticText( m_FeaturesPanel, wxID_ANY, wxT("Feature"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labFeature->Wrap( -1 );
	fgSizer3->Add( m_labFeature, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_Feature = new wxSpinCtrl( m_FeaturesPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 120,-1 ), wxSP_ARROW_KEYS, 0, 255, 0 );
	fgSizer3->Add( m_Feature, 0, wxALL, 5 );
	
	m_labFeatureValue = new wxStaticText( m_FeaturesPanel, wxID_ANY, wxT("Value"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labFeatureValue->Wrap( -1 );
	fgSizer3->Add( m_labFeatureValue, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_FeatureValue = new wxSpinCtrl( m_FeaturesPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 120,-1 ), wxSP_ARROW_KEYS, 0, 255, 0 );
	fgSizer3->Add( m_FeatureValue, 0, wxALL, 5 );
	
	bSizer9->Add( fgSizer3, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer71;
	bSizer71 = new wxBoxSizer( wxHORIZONTAL );
	
	m_FeaturesGet = new wxButton( m_FeaturesPanel, wxID_ANY, wxT("Get"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer71->Add( m_FeaturesGet, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_FeatureSet = new wxButton( m_FeaturesPanel, wxID_ANY, wxT("Set"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer71->Add( m_FeatureSet, 0, wxALL, 5 );
	
	bSizer9->Add( bSizer71, 0, wxALIGN_RIGHT, 5 );
	
	bSizer8->Add( bSizer9, 1, wxEXPAND, 5 );
	
	bSizer6->Add( bSizer8, 1, wxEXPAND, 5 );
	
	m_FeaturesPanel->SetSizer( bSizer6 );
	m_FeaturesPanel->Layout();
	bSizer6->Fit( m_FeaturesPanel );
	m_Notebook->AddPage( m_FeaturesPanel, wxT("Features"), false );
	m_AccessoryPanel = new wxPanel( m_Notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer18;
	bSizer18 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer19;
	bSizer19 = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* bSizer20;
	bSizer20 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* m_AccessoryPortBox;
	m_AccessoryPortBox = new wxStaticBoxSizer( new wxStaticBox( m_AccessoryPanel, wxID_ANY, wxT("Port") ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizer10;
	fgSizer10 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer10->SetFlexibleDirection( wxBOTH );
	fgSizer10->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_labAccessoryPort = new wxStaticText( m_AccessoryPanel, wxID_ANY, wxT("Number"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labAccessoryPort->Wrap( -1 );
	fgSizer10->Add( m_labAccessoryPort, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );
	
	m_AccessoryPort = new wxSpinCtrl( m_AccessoryPanel, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 80,-1 ), wxSP_ARROW_KEYS, 0, 255, 0 );
	fgSizer10->Add( m_AccessoryPort, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_AccessoryPortBox->Add( fgSizer10, 0, 0, 5 );
	
	wxFlexGridSizer* fgSizer11;
	fgSizer11 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer11->SetFlexibleDirection( wxBOTH );
	fgSizer11->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_AccessoryOn = new wxButton( m_AccessoryPanel, wxID_ANY, wxT("ON"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer11->Add( m_AccessoryOn, 0, wxALL, 5 );
	
	m_AccessoryOff = new wxButton( m_AccessoryPanel, wxID_ANY, wxT("OFF"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer11->Add( m_AccessoryOff, 0, wxALL, 5 );
	
	m_AccessoryPortBox->Add( fgSizer11, 0, 0, 5 );
	
	bSizer20->Add( m_AccessoryPortBox, 0, wxEXPAND, 5 );
	
	wxStaticBoxSizer* mAccessoryOptionsBox;
	mAccessoryOptionsBox = new wxStaticBoxSizer( new wxStaticBox( m_AccessoryPanel, wxID_ANY, wxT("Options") ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizer12;
	fgSizer12 = new wxFlexGridSizer( 0, 3, 0, 0 );
	fgSizer12->SetFlexibleDirection( wxBOTH );
	fgSizer12->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_labAccessorySwitchTime = new wxStaticText( m_AccessoryPanel, wxID_ANY, wxT("Switch time"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labAccessorySwitchTime->Wrap( -1 );
	fgSizer12->Add( m_labAccessorySwitchTime, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_AccessorySwitchTime = new wxSpinCtrl( m_AccessoryPanel, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 80,-1 ), wxSP_ARROW_KEYS, 0, 127, 0 );
	fgSizer12->Add( m_AccessorySwitchTime, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_AccessorySwitchTimeSeconds = new wxCheckBox( m_AccessoryPanel, wxID_ANY, wxT("seconds"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer12->Add( m_AccessorySwitchTimeSeconds, 0, wxTOP|wxBOTTOM|wxALIGN_CENTER_VERTICAL, 5 );
	
	mAccessoryOptionsBox->Add( fgSizer12, 0, 0, 5 );
	
	wxFlexGridSizer* fgSizer13;
	fgSizer13 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer13->SetFlexibleDirection( wxBOTH );
	fgSizer13->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_AccessoryReadOptions = new wxButton( m_AccessoryPanel, wxID_ANY, wxT("Read"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer13->Add( m_AccessoryReadOptions, 0, wxALL, 5 );
	
	m_AccessoryWriteOptions = new wxButton( m_AccessoryPanel, wxID_ANY, wxT("Write"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer13->Add( m_AccessoryWriteOptions, 0, wxALL, 5 );
	
	mAccessoryOptionsBox->Add( fgSizer13, 0, 0, 5 );
	
	bSizer20->Add( mAccessoryOptionsBox, 0, wxEXPAND|wxTOP, 5 );
	
	bSizer19->Add( bSizer20, 0, 0, 5 );
	
	wxStaticBoxSizer* m_AccessoryMacroMapBox;
	m_AccessoryMacroMapBox = new wxStaticBoxSizer( new wxStaticBox( m_AccessoryPanel, wxID_ANY, wxT("Macro map") ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizer14;
	fgSizer14 = new wxFlexGridSizer( 0, 6, 0, 0 );
	fgSizer14->SetFlexibleDirection( wxBOTH );
	fgSizer14->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText28 = new wxStaticText( m_AccessoryPanel, wxID_ANY, wxT("1"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText28->Wrap( -1 );
	fgSizer14->Add( m_staticText28, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxLEFT, 5 );
	
	m_AccessoryMacro1 = new wxSpinCtrl( m_AccessoryPanel, wxID_ANY, wxT("255"), wxDefaultPosition, wxSize( 80,-1 ), wxSP_ARROW_KEYS, 0, 255, 255 );
	fgSizer14->Add( m_AccessoryMacro1, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT|wxLEFT, 5 );
	
	m_staticText29 = new wxStaticText( m_AccessoryPanel, wxID_ANY, wxT("2"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText29->Wrap( -1 );
	fgSizer14->Add( m_staticText29, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT, 5 );
	
	m_AccessoryMacro2 = new wxSpinCtrl( m_AccessoryPanel, wxID_ANY, wxT("255"), wxDefaultPosition, wxSize( 80,-1 ), wxSP_ARROW_KEYS, 0, 255, 0 );
	fgSizer14->Add( m_AccessoryMacro2, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT|wxLEFT, 5 );
	
	m_staticText281 = new wxStaticText( m_AccessoryPanel, wxID_ANY, wxT("3"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText281->Wrap( -1 );
	fgSizer14->Add( m_staticText281, 0, wxLEFT|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );
	
	m_AccessoryMacro3 = new wxSpinCtrl( m_AccessoryPanel, wxID_ANY, wxT("255"), wxDefaultPosition, wxSize( 80,-1 ), wxSP_ARROW_KEYS, 0, 255, 0 );
	fgSizer14->Add( m_AccessoryMacro3, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT|wxLEFT, 5 );
	
	m_staticText2811 = new wxStaticText( m_AccessoryPanel, wxID_ANY, wxT("4"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2811->Wrap( -1 );
	fgSizer14->Add( m_staticText2811, 0, wxLEFT|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );
	
	m_AccessoryMacro4 = new wxSpinCtrl( m_AccessoryPanel, wxID_ANY, wxT("255"), wxDefaultPosition, wxSize( 80,-1 ), wxSP_ARROW_KEYS, 0, 255, 0 );
	fgSizer14->Add( m_AccessoryMacro4, 0, wxRIGHT|wxLEFT|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_staticText28111 = new wxStaticText( m_AccessoryPanel, wxID_ANY, wxT("5"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText28111->Wrap( -1 );
	fgSizer14->Add( m_staticText28111, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT, 5 );
	
	m_AccessoryMacro5 = new wxSpinCtrl( m_AccessoryPanel, wxID_ANY, wxT("255"), wxDefaultPosition, wxSize( 80,-1 ), wxSP_ARROW_KEYS, 0, 255, 0 );
	fgSizer14->Add( m_AccessoryMacro5, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT|wxLEFT, 5 );
	
	m_staticText281111 = new wxStaticText( m_AccessoryPanel, wxID_ANY, wxT("6"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText281111->Wrap( -1 );
	fgSizer14->Add( m_staticText281111, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT, 5 );
	
	m_AccessoryMacro6 = new wxSpinCtrl( m_AccessoryPanel, wxID_ANY, wxT("255"), wxDefaultPosition, wxSize( 80,-1 ), wxSP_ARROW_KEYS, 0, 255, 0 );
	fgSizer14->Add( m_AccessoryMacro6, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT|wxLEFT, 5 );
	
	m_staticText282 = new wxStaticText( m_AccessoryPanel, wxID_ANY, wxT("7"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText282->Wrap( -1 );
	fgSizer14->Add( m_staticText282, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT, 5 );
	
	m_AccessoryMacro7 = new wxSpinCtrl( m_AccessoryPanel, wxID_ANY, wxT("255"), wxDefaultPosition, wxSize( 80,-1 ), wxSP_ARROW_KEYS, 0, 255, 0 );
	fgSizer14->Add( m_AccessoryMacro7, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT|wxLEFT, 5 );
	
	m_staticText283 = new wxStaticText( m_AccessoryPanel, wxID_ANY, wxT("8"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText283->Wrap( -1 );
	fgSizer14->Add( m_staticText283, 0, wxALIGN_RIGHT|wxLEFT|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_AccessoryMacro8 = new wxSpinCtrl( m_AccessoryPanel, wxID_ANY, wxT("255"), wxDefaultPosition, wxSize( 80,-1 ), wxSP_ARROW_KEYS, 0, 255, 0 );
	fgSizer14->Add( m_AccessoryMacro8, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT|wxLEFT, 5 );
	
	m_staticText284 = new wxStaticText( m_AccessoryPanel, wxID_ANY, wxT("9"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText284->Wrap( -1 );
	fgSizer14->Add( m_staticText284, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT, 5 );
	
	m_AccessoryMacro9 = new wxSpinCtrl( m_AccessoryPanel, wxID_ANY, wxT("255"), wxDefaultPosition, wxSize( 80,-1 ), wxSP_ARROW_KEYS, 0, 255, 0 );
	fgSizer14->Add( m_AccessoryMacro9, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT|wxLEFT, 5 );
	
	m_staticText285 = new wxStaticText( m_AccessoryPanel, wxID_ANY, wxT("10"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText285->Wrap( -1 );
	fgSizer14->Add( m_staticText285, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT, 5 );
	
	m_AccessoryMacro10 = new wxSpinCtrl( m_AccessoryPanel, wxID_ANY, wxT("255"), wxDefaultPosition, wxSize( 80,-1 ), wxSP_ARROW_KEYS, 0, 255, 0 );
	fgSizer14->Add( m_AccessoryMacro10, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT|wxLEFT, 5 );
	
	m_staticText286 = new wxStaticText( m_AccessoryPanel, wxID_ANY, wxT("11"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText286->Wrap( -1 );
	fgSizer14->Add( m_staticText286, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT, 5 );
	
	m_AccessoryMacro11 = new wxSpinCtrl( m_AccessoryPanel, wxID_ANY, wxT("255"), wxDefaultPosition, wxSize( 80,-1 ), wxSP_ARROW_KEYS, 0, 255, 0 );
	fgSizer14->Add( m_AccessoryMacro11, 0, wxRIGHT|wxLEFT|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_staticText287 = new wxStaticText( m_AccessoryPanel, wxID_ANY, wxT("12"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText287->Wrap( -1 );
	fgSizer14->Add( m_staticText287, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT, 5 );
	
	m_AccessoryMacro12 = new wxSpinCtrl( m_AccessoryPanel, wxID_ANY, wxT("255"), wxDefaultPosition, wxSize( 80,-1 ), wxSP_ARROW_KEYS, 0, 255, 0 );
	fgSizer14->Add( m_AccessoryMacro12, 0, wxRIGHT|wxLEFT|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_staticText288 = new wxStaticText( m_AccessoryPanel, wxID_ANY, wxT("13"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText288->Wrap( -1 );
	fgSizer14->Add( m_staticText288, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT, 5 );
	
	m_AccessoryMacro13 = new wxSpinCtrl( m_AccessoryPanel, wxID_ANY, wxT("255"), wxDefaultPosition, wxSize( 80,-1 ), wxSP_ARROW_KEYS, 0, 255, 0 );
	fgSizer14->Add( m_AccessoryMacro13, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT|wxLEFT, 5 );
	
	m_staticText289 = new wxStaticText( m_AccessoryPanel, wxID_ANY, wxT("14"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText289->Wrap( -1 );
	fgSizer14->Add( m_staticText289, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT, 5 );
	
	m_AccessoryMacro14 = new wxSpinCtrl( m_AccessoryPanel, wxID_ANY, wxT("255"), wxDefaultPosition, wxSize( 80,-1 ), wxSP_ARROW_KEYS, 0, 255, 0 );
	fgSizer14->Add( m_AccessoryMacro14, 0, wxRIGHT|wxLEFT|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_staticText2810 = new wxStaticText( m_AccessoryPanel, wxID_ANY, wxT("15"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2810->Wrap( -1 );
	fgSizer14->Add( m_staticText2810, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT, 5 );
	
	m_AccessoryMacro15 = new wxSpinCtrl( m_AccessoryPanel, wxID_ANY, wxT("255"), wxDefaultPosition, wxSize( 80,-1 ), wxSP_ARROW_KEYS, 0, 255, 0 );
	fgSizer14->Add( m_AccessoryMacro15, 0, wxRIGHT|wxLEFT|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_staticText2812 = new wxStaticText( m_AccessoryPanel, wxID_ANY, wxT("16"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2812->Wrap( -1 );
	fgSizer14->Add( m_staticText2812, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT, 5 );
	
	m_AccessoryMacro16 = new wxSpinCtrl( m_AccessoryPanel, wxID_ANY, wxT("255"), wxDefaultPosition, wxSize( 80,-1 ), wxSP_ARROW_KEYS, 0, 255, 0 );
	fgSizer14->Add( m_AccessoryMacro16, 0, wxRIGHT|wxLEFT|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_AccessoryMacroMapBox->Add( fgSizer14, 1, wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer15;
	fgSizer15 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer15->SetFlexibleDirection( wxBOTH );
	fgSizer15->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_AccessoryReadMacroMap = new wxButton( m_AccessoryPanel, wxID_ANY, wxT("Read"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer15->Add( m_AccessoryReadMacroMap, 0, wxALL, 5 );
	
	m_AccessoryWriteMacroMap = new wxButton( m_AccessoryPanel, wxID_ANY, wxT("Write"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer15->Add( m_AccessoryWriteMacroMap, 0, wxALL, 5 );
	
	m_AccessoryMacroMapBox->Add( fgSizer15, 0, 0, 5 );
	
	bSizer19->Add( m_AccessoryMacroMapBox, 0, wxLEFT, 5 );
	
	bSizer18->Add( bSizer19, 0, wxEXPAND, 5 );
	
	m_AccessoryPanel->SetSizer( bSizer18 );
	m_AccessoryPanel->Layout();
	bSizer18->Fit( m_AccessoryPanel );
	m_Notebook->AddPage( m_AccessoryPanel, wxT("Accessory"), false );
	m_ServoPanel = new wxPanel( m_Notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer( wxHORIZONTAL );
	
	wxFlexGridSizer* fgSizer4;
	fgSizer4 = new wxFlexGridSizer( 0, 4, 0, 0 );
	fgSizer4->AddGrowableRow( 1 );
	fgSizer4->SetFlexibleDirection( wxBOTH );
	fgSizer4->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_labServoLeft = new wxStaticText( m_ServoPanel, wxID_ANY, wxT("L"), wxDefaultPosition, wxSize( -1,-1 ), 0 );
	m_labServoLeft->Wrap( -1 );
	fgSizer4->Add( m_labServoLeft, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_labServoRight = new wxStaticText( m_ServoPanel, wxID_ANY, wxT("R"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labServoRight->Wrap( -1 );
	fgSizer4->Add( m_labServoRight, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_labServoSpeed = new wxStaticText( m_ServoPanel, wxID_ANY, wxT("V"), wxDefaultPosition, wxSize( -1,-1 ), 0 );
	m_labServoSpeed->Wrap( -1 );
	fgSizer4->Add( m_labServoSpeed, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_labServoReserved = new wxStaticText( m_ServoPanel, wxID_ANY, wxT("S"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labServoReserved->Wrap( -1 );
	fgSizer4->Add( m_labServoReserved, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_ServoLeft = new wxSlider( m_ServoPanel, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxDefaultSize, wxSL_AUTOTICKS|wxSL_INVERSE|wxSL_VERTICAL );
	fgSizer4->Add( m_ServoLeft, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	m_ServoRight = new wxSlider( m_ServoPanel, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxDefaultSize, wxSL_AUTOTICKS|wxSL_INVERSE|wxSL_VERTICAL );
	fgSizer4->Add( m_ServoRight, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	m_ServoSpeed = new wxSlider( m_ServoPanel, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxDefaultSize, wxSL_AUTOTICKS|wxSL_INVERSE|wxSL_VERTICAL );
	fgSizer4->Add( m_ServoSpeed, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	m_ServoReserved = new wxSlider( m_ServoPanel, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxDefaultSize, wxSL_AUTOTICKS|wxSL_INVERSE|wxSL_VERTICAL );
	fgSizer4->Add( m_ServoReserved, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	m_ConfigL = new wxSpinCtrl( m_ServoPanel, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 60,-1 ), wxSP_ARROW_KEYS, 0, 255, 0 );
	fgSizer4->Add( m_ConfigL, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_ConfigR = new wxSpinCtrl( m_ServoPanel, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 60,-1 ), wxSP_ARROW_KEYS, 0, 255, 0 );
	fgSizer4->Add( m_ConfigR, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_ConfigV = new wxSpinCtrl( m_ServoPanel, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 60,-1 ), wxSP_ARROW_KEYS, 0, 255, 0 );
	fgSizer4->Add( m_ConfigV, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_ConfigS = new wxSpinCtrl( m_ServoPanel, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 60,-1 ), wxSP_ARROW_KEYS, 0, 255, 0 );
	fgSizer4->Add( m_ConfigS, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	bSizer10->Add( fgSizer4, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer( wxVERTICAL );
	
	wxString m_PortTypeChoices[] = { wxT("Standard"), wxT("Light"), wxT("Servo"), wxT("Sound"), wxT("Motor"), wxT("Analog"), wxT("Macro") };
	int m_PortTypeNChoices = sizeof( m_PortTypeChoices ) / sizeof( wxString );
	m_PortType = new wxRadioBox( m_ServoPanel, wxID_ANY, wxT("Type"), wxDefaultPosition, wxDefaultSize, m_PortTypeNChoices, m_PortTypeChoices, 2, wxRA_SPECIFY_COLS );
	m_PortType->SetSelection( 5 );
	bSizer11->Add( m_PortType, 0, wxALL, 5 );
	
	wxFlexGridSizer* fgSizer5;
	fgSizer5 = new wxFlexGridSizer( 0, 3, 0, 0 );
	fgSizer5->SetFlexibleDirection( wxBOTH );
	fgSizer5->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_labServoPort = new wxStaticText( m_ServoPanel, wxID_ANY, wxT("Port"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labServoPort->Wrap( -1 );
	fgSizer5->Add( m_labServoPort, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM|wxLEFT, 5 );
	
	m_ServoPort = new wxSpinCtrl( m_ServoPanel, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 80,-1 ), wxSP_ARROW_KEYS, 0, 127, 0 );
	fgSizer5->Add( m_ServoPort, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM|wxRIGHT, 5 );
	
	m_ServoGet = new wxButton( m_ServoPanel, wxID_ANY, wxT("Get"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer5->Add( m_ServoGet, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM|wxRIGHT, 5 );
	
	bSizer11->Add( fgSizer5, 0, 0, 5 );
	
	wxStaticBoxSizer* m_ServoTestBox;
	m_ServoTestBox = new wxStaticBoxSizer( new wxStaticBox( m_ServoPanel, wxID_ANY, wxT("Test") ), wxVERTICAL );
	
	wxBoxSizer* bSizer12;
	bSizer12 = new wxBoxSizer( wxHORIZONTAL );
	
	m_ServoLeftTest = new wxButton( m_ServoPanel, wxID_ANY, wxT("ON"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer12->Add( m_ServoLeftTest, 0, wxALL, 5 );
	
	m_ServoRightTest = new wxButton( m_ServoPanel, wxID_ANY, wxT("OFF"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer12->Add( m_ServoRightTest, 0, wxALL, 5 );
	
	m_ServoTestBox->Add( bSizer12, 0, 0, 5 );
	
	bSizer11->Add( m_ServoTestBox, 0, 0, 5 );
	
	bSizer10->Add( bSizer11, 0, wxALIGN_BOTTOM, 5 );
	
	m_ServoPanel->SetSizer( bSizer10 );
	m_ServoPanel->Layout();
	bSizer10->Fit( m_ServoPanel );
	m_Notebook->AddPage( m_ServoPanel, wxT("Port Setup"), false );
	m_MacroPanel = new wxPanel( m_Notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer13;
	bSizer13 = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* bSizer14;
	bSizer14 = new wxBoxSizer( wxVERTICAL );
	
	m_labMacroList = new wxStaticText( m_MacroPanel, wxID_ANY, wxT("Macro:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labMacroList->Wrap( -1 );
	bSizer14->Add( m_labMacroList, 0, wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_MacroList = new wxListBox( m_MacroPanel, wxID_ANY, wxDefaultPosition, wxSize( 100,-1 ), 0, NULL, wxLB_SINGLE ); 
	bSizer14->Add( m_MacroList, 1, wxALL|wxEXPAND, 5 );
	
	bSizer13->Add( bSizer14, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer15;
	bSizer15 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer81;
	fgSizer81 = new wxFlexGridSizer( 0, 10, 0, 0 );
	fgSizer81->SetFlexibleDirection( wxBOTH );
	fgSizer81->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_labMacroSlowdown = new wxStaticText( m_MacroPanel, wxID_ANY, wxT("Slowdown"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labMacroSlowdown->Wrap( -1 );
	fgSizer81->Add( m_labMacroSlowdown, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5 );
	
	m_MacroSlowdown = new wxSpinCtrl( m_MacroPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 80,-1 ), wxSP_ARROW_KEYS, 0, 255, 0 );
	fgSizer81->Add( m_MacroSlowdown, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, 5 );
	
	m_labCycles = new wxStaticText( m_MacroPanel, wxID_ANY, wxT("Cycles"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labCycles->Wrap( -1 );
	fgSizer81->Add( m_labCycles, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_MacroCycles = new wxSpinCtrl( m_MacroPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 80,-1 ), wxSP_ARROW_KEYS, 0, 250, 0 );
	fgSizer81->Add( m_MacroCycles, 0, wxRIGHT|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_labMacroTime = new wxStaticText( m_MacroPanel, wxID_ANY, wxT("Time"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labMacroTime->Wrap( -1 );
	fgSizer81->Add( m_labMacroTime, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5 );
	
	m_MacroHours = new wxSpinCtrl( m_MacroPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 60,-1 ), wxSP_ARROW_KEYS, 0, 24, 0 );
	fgSizer81->Add( m_MacroHours, 0, wxALIGN_CENTER_VERTICAL, 5 );
	
	m_staticText23 = new wxStaticText( m_MacroPanel, wxID_ANY, wxT(":"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText23->Wrap( -1 );
	fgSizer81->Add( m_staticText23, 0, wxALIGN_CENTER_VERTICAL, 5 );
	
	m_MacroMinutes = new wxSpinCtrl( m_MacroPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 60,-1 ), wxSP_ARROW_KEYS, 0, 62, 0 );
	fgSizer81->Add( m_MacroMinutes, 0, wxALIGN_CENTER_VERTICAL, 5 );
	
	m_staticText26 = new wxStaticText( m_MacroPanel, wxID_ANY, wxT("-"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText26->Wrap( -1 );
	fgSizer81->Add( m_staticText26, 0, wxALIGN_CENTER_VERTICAL, 5 );
	
	m_MacroWDay = new wxSpinCtrl( m_MacroPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 60,-1 ), wxSP_ARROW_KEYS, 0, 127, 0 );
	fgSizer81->Add( m_MacroWDay, 0, wxALIGN_CENTER_VERTICAL, 5 );
	
	bSizer15->Add( fgSizer81, 0, wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer9;
	fgSizer9 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer9->AddGrowableCol( 0 );
	fgSizer9->AddGrowableRow( 0 );
	fgSizer9->SetFlexibleDirection( wxBOTH );
	fgSizer9->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_MacroLines = new wxGrid( m_MacroPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL );
	
	// Grid
	m_MacroLines->CreateGrid( 1, 4 );
	m_MacroLines->EnableEditing( false );
	m_MacroLines->EnableGridLines( true );
	m_MacroLines->EnableDragGridSize( false );
	m_MacroLines->SetMargins( 0, 0 );
	
	// Columns
	m_MacroLines->AutoSizeColumns();
	m_MacroLines->EnableDragColMove( false );
	m_MacroLines->EnableDragColSize( true );
	m_MacroLines->SetColLabelSize( 20 );
	m_MacroLines->SetColLabelAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );
	
	// Rows
	m_MacroLines->EnableDragRowSize( false );
	m_MacroLines->SetRowLabelSize( 40 );
	m_MacroLines->SetRowLabelAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );
	
	// Label Appearance
	
	// Cell Defaults
	m_MacroLines->SetDefaultCellAlignment( wxALIGN_LEFT, wxALIGN_TOP );
	fgSizer9->Add( m_MacroLines, 1, wxEXPAND|wxTOP|wxRIGHT|wxLEFT, 5 );
	
	wxStaticBoxSizer* m_MacroStartBox;
	m_MacroStartBox = new wxStaticBoxSizer( new wxStaticBox( m_MacroPanel, wxID_ANY, wxT("Start every:") ), wxVERTICAL );
	
	m_MacroStartDaily = new wxCheckBox( m_MacroPanel, wxID_ANY, wxT("Day"), wxDefaultPosition, wxDefaultSize, 0 );
	m_MacroStartBox->Add( m_MacroStartDaily, 0, wxRIGHT|wxLEFT, 5 );
	
	m_MacroStartHourly = new wxCheckBox( m_MacroPanel, wxID_ANY, wxT("Hour"), wxDefaultPosition, wxDefaultSize, 0 );
	m_MacroStartBox->Add( m_MacroStartHourly, 0, wxRIGHT|wxLEFT, 5 );
	
	m_MacroStart30 = new wxCheckBox( m_MacroPanel, wxID_ANY, wxT("30 minutes"), wxDefaultPosition, wxDefaultSize, 0 );
	m_MacroStartBox->Add( m_MacroStart30, 0, wxRIGHT|wxLEFT, 5 );
	
	m_MacroStart15 = new wxCheckBox( m_MacroPanel, wxID_ANY, wxT("15 minutes"), wxDefaultPosition, wxDefaultSize, 0 );
	m_MacroStartBox->Add( m_MacroStart15, 0, wxRIGHT|wxLEFT, 5 );
	
	m_MacroStart1 = new wxCheckBox( m_MacroPanel, wxID_ANY, wxT("minute"), wxDefaultPosition, wxDefaultSize, 0 );
	m_MacroStartBox->Add( m_MacroStart1, 0, wxRIGHT|wxLEFT, 5 );
	
	fgSizer9->Add( m_MacroStartBox, 0, 0, 5 );
	
	bSizer15->Add( fgSizer9, 1, wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer7;
	fgSizer7 = new wxFlexGridSizer( 0, 8, 0, 0 );
	fgSizer7->SetFlexibleDirection( wxBOTH );
	fgSizer7->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_labMacroDelay = new wxStaticText( m_MacroPanel, wxID_ANY, wxT("Delay"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labMacroDelay->Wrap( -1 );
	fgSizer7->Add( m_labMacroDelay, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM|wxLEFT, 5 );
	
	m_MacroDelay = new wxSpinCtrl( m_MacroPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 80,-1 ), wxSP_ARROW_KEYS, 0, 255, 0 );
	fgSizer7->Add( m_MacroDelay, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM|wxRIGHT, 5 );
	
	m_labMacroType = new wxStaticText( m_MacroPanel, wxID_ANY, wxT("Type"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labMacroType->Wrap( -1 );
	fgSizer7->Add( m_labMacroType, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM|wxLEFT, 5 );
	
	m_MacroType = new wxComboBox( m_MacroPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 100,-1 ), 0, NULL, 0 ); 
	fgSizer7->Add( m_MacroType, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM|wxRIGHT, 5 );
	
	m_labMacroPort = new wxStaticText( m_MacroPanel, wxID_ANY, wxT("Port"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labMacroPort->Wrap( -1 );
	fgSizer7->Add( m_labMacroPort, 0, wxTOP|wxBOTTOM|wxLEFT|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_MacroPort = new wxSpinCtrl( m_MacroPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 80,-1 ), wxSP_ARROW_KEYS, 0, 127, 0 );
	fgSizer7->Add( m_MacroPort, 0, wxTOP|wxBOTTOM|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_labMacroValue = new wxStaticText( m_MacroPanel, wxID_ANY, wxT("Value"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labMacroValue->Wrap( -1 );
	fgSizer7->Add( m_labMacroValue, 0, wxTOP|wxBOTTOM|wxLEFT|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_MacroValue = new wxSpinCtrl( m_MacroPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 80,-1 ), wxSP_ARROW_KEYS, 0, 255, 0 );
	fgSizer7->Add( m_MacroValue, 0, wxTOP|wxBOTTOM|wxRIGHT|wxALIGN_CENTER_VERTICAL, 5 );
	
	bSizer15->Add( fgSizer7, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer16;
	bSizer16 = new wxBoxSizer( wxHORIZONTAL );
	
	m_MacroApply = new wxButton( m_MacroPanel, wxID_ANY, wxT("Apply line"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer16->Add( m_MacroApply, 0, wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	m_MacroReload = new wxButton( m_MacroPanel, wxID_ANY, wxT("Read"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer16->Add( m_MacroReload, 0, wxBOTTOM|wxLEFT, 5 );
	
	m_MacroSave = new wxButton( m_MacroPanel, wxID_ANY, wxT("Write"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer16->Add( m_MacroSave, 0, wxBOTTOM|wxRIGHT, 5 );
	
	m_MacroExport = new wxButton( m_MacroPanel, wxID_ANY, wxT("Export..."), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer16->Add( m_MacroExport, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxLEFT, 5 );
	
	m_MacroImport = new wxButton( m_MacroPanel, wxID_ANY, wxT("Import..."), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer16->Add( m_MacroImport, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxRIGHT, 5 );
	
	bSizer15->Add( bSizer16, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer17;
	bSizer17 = new wxBoxSizer( wxHORIZONTAL );
	
	m_MacroSaveMacro = new wxButton( m_MacroPanel, wxID_ANY, wxT("Save"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer17->Add( m_MacroSaveMacro, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_MacroDeleteMacro = new wxButton( m_MacroPanel, wxID_ANY, wxT("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer17->Add( m_MacroDeleteMacro, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_MacroRestoreMacro = new wxButton( m_MacroPanel, wxID_ANY, wxT("Restore"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer17->Add( m_MacroRestoreMacro, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_MacroTest = new wxButton( m_MacroPanel, wxID_ANY, wxT("Test"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer17->Add( m_MacroTest, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	bSizer15->Add( bSizer17, 0, 0, 5 );
	
	bSizer13->Add( bSizer15, 1, wxEXPAND, 5 );
	
	m_MacroPanel->SetSizer( bSizer13 );
	m_MacroPanel->Layout();
	bSizer13->Fit( m_MacroPanel );
	m_Notebook->AddPage( m_MacroPanel, wxT("Macro"), false );
	m_UpdatePanel = new wxPanel( m_Notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer111;
	bSizer111 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer51;
	fgSizer51 = new wxFlexGridSizer( 0, 3, 0, 0 );
	fgSizer51->AddGrowableCol( 1 );
	fgSizer51->SetFlexibleDirection( wxBOTH );
	fgSizer51->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_labUpdateFile = new wxStaticText( m_UpdatePanel, wxID_ANY, wxT("File"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labUpdateFile->Wrap( -1 );
	fgSizer51->Add( m_labUpdateFile, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_UpdateFile = new wxTextCtrl( m_UpdatePanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer51->Add( m_UpdateFile, 0, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_OpenUpdateFile = new wxButton( m_UpdatePanel, wxID_ANY, wxT("..."), wxDefaultPosition, wxSize( 50,-1 ), 0 );
	fgSizer51->Add( m_OpenUpdateFile, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	bSizer111->Add( fgSizer51, 0, wxEXPAND, 5 );
	
	m_labUpdateFilePreview = new wxStaticText( m_UpdatePanel, wxID_ANY, wxT("Preview (first 50 lines)"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labUpdateFilePreview->Wrap( -1 );
	bSizer111->Add( m_labUpdateFilePreview, 0, wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_HEXFileText = new wxTextCtrl( m_UpdatePanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY );
	bSizer111->Add( m_HEXFileText, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer121;
	bSizer121 = new wxBoxSizer( wxHORIZONTAL );
	
	m_UpdateStart = new wxButton( m_UpdatePanel, wxID_ANY, wxT("Start"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer121->Add( m_UpdateStart, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	bSizer111->Add( bSizer121, 0, wxALIGN_RIGHT, 5 );
	
	m_UpdatePanel->SetSizer( bSizer111 );
	m_UpdatePanel->Layout();
	bSizer111->Fit( m_UpdatePanel );
	m_Notebook->AddPage( m_UpdatePanel, wxT("Update"), false );
	
	bSizer7->Add( m_Notebook, 1, wxEXPAND|wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_stdButtons = new wxStdDialogButtonSizer();
	m_stdButtonsOK = new wxButton( this, wxID_OK );
	m_stdButtons->AddButton( m_stdButtonsOK );
	m_stdButtonsCancel = new wxButton( this, wxID_CANCEL );
	m_stdButtons->AddButton( m_stdButtonsCancel );
	m_stdButtons->Realize();
	bSizer7->Add( m_stdButtons, 0, wxALIGN_CENTER_VERTICAL|wxEXPAND|wxALIGN_RIGHT|wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	this->SetSizer( bSizer7 );
	this->Layout();
	bSizer7->Fit( this );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_Tree->Connect( wxEVT_COMMAND_TREE_BEGIN_DRAG, wxTreeEventHandler( BidibIdentDlgGen::onBeginDrag ), NULL, this );
	m_Tree->Connect( wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler( BidibIdentDlgGen::onItemActivated ), NULL, this );
	m_Tree->Connect( wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK, wxTreeEventHandler( BidibIdentDlgGen::onItemRightClick ), NULL, this );
	m_Tree->Connect( wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler( BidibIdentDlgGen::onTreeSelChanged ), NULL, this );
	m_FeatureList->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( BidibIdentDlgGen::onFeatureSelect ), NULL, this );
	m_FeaturesGet->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BidibIdentDlgGen::onFeaturesGet ), NULL, this );
	m_FeatureSet->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BidibIdentDlgGen::onFeatureSet ), NULL, this );
	m_AccessoryOn->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BidibIdentDlgGen::onAccessoryOnTest ), NULL, this );
	m_AccessoryOff->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BidibIdentDlgGen::onAccessoryOffTest ), NULL, this );
	m_AccessoryReadOptions->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BidibIdentDlgGen::onAccessoryReadOptions ), NULL, this );
	m_AccessoryWriteOptions->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BidibIdentDlgGen::onAccessoryWriteOptions ), NULL, this );
	m_AccessoryReadMacroMap->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BidibIdentDlgGen::onAccessoryReadMacroMap ), NULL, this );
	m_AccessoryWriteMacroMap->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BidibIdentDlgGen::onAccessoryWriteMacroMap ), NULL, this );
	m_ServoLeft->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( BidibIdentDlgGen::onServoLeft ), NULL, this );
	m_ServoRight->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( BidibIdentDlgGen::onServoRight ), NULL, this );
	m_ServoSpeed->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( BidibIdentDlgGen::onServoSpeed ), NULL, this );
	m_ServoReserved->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( BidibIdentDlgGen::onServoReserved ), NULL, this );
	m_ConfigL->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( BidibIdentDlgGen::onConfigL ), NULL, this );
	m_ConfigL->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( BidibIdentDlgGen::onConfigLtxt ), NULL, this );
	m_ConfigR->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( BidibIdentDlgGen::onConfigR ), NULL, this );
	m_ConfigR->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( BidibIdentDlgGen::onConfigRtxt ), NULL, this );
	m_ConfigV->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( BidibIdentDlgGen::onConfigV ), NULL, this );
	m_ConfigV->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( BidibIdentDlgGen::onConfigVtxt ), NULL, this );
	m_ConfigS->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( BidibIdentDlgGen::onConfigS ), NULL, this );
	m_ConfigS->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( BidibIdentDlgGen::onConfigStxt ), NULL, this );
	m_PortType->Connect( wxEVT_COMMAND_RADIOBOX_SELECTED, wxCommandEventHandler( BidibIdentDlgGen::onPortType ), NULL, this );
	m_ServoPort->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( BidibIdentDlgGen::onServoPort ), NULL, this );
	m_ServoGet->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BidibIdentDlgGen::onServoGet ), NULL, this );
	m_ServoLeftTest->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BidibIdentDlgGen::onServoLeftTest ), NULL, this );
	m_ServoRightTest->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BidibIdentDlgGen::onServoRightTest ), NULL, this );
	m_MacroList->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( BidibIdentDlgGen::onMacroList ), NULL, this );
	m_MacroLines->Connect( wxEVT_GRID_CELL_LEFT_CLICK, wxGridEventHandler( BidibIdentDlgGen::onMacroLineSelected ), NULL, this );
	m_MacroStart30->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( BidibIdentDlgGen::onMacroEveryMinute ), NULL, this );
	m_MacroStart15->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( BidibIdentDlgGen::onMacroEveryMinute ), NULL, this );
	m_MacroStart1->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( BidibIdentDlgGen::onMacroEveryMinute ), NULL, this );
	m_MacroApply->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BidibIdentDlgGen::onMacroApply ), NULL, this );
	m_MacroReload->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BidibIdentDlgGen::onMacroReload ), NULL, this );
	m_MacroSave->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BidibIdentDlgGen::onMacroSave ), NULL, this );
	m_MacroExport->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BidibIdentDlgGen::onMacroExport ), NULL, this );
	m_MacroImport->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BidibIdentDlgGen::onMacroImport ), NULL, this );
	m_MacroSaveMacro->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BidibIdentDlgGen::onMacroSaveMacro ), NULL, this );
	m_MacroDeleteMacro->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BidibIdentDlgGen::onMacroDeleteMacro ), NULL, this );
	m_MacroRestoreMacro->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BidibIdentDlgGen::onMacroRestoreMacro ), NULL, this );
	m_MacroTest->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BidibIdentDlgGen::onMacroTest ), NULL, this );
	m_OpenUpdateFile->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BidibIdentDlgGen::onSelectUpdateFile ), NULL, this );
	m_UpdateStart->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BidibIdentDlgGen::onUpdateStart ), NULL, this );
	m_stdButtonsCancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BidibIdentDlgGen::onCancel ), NULL, this );
	m_stdButtonsOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BidibIdentDlgGen::onOK ), NULL, this );
}

BidibIdentDlgGen::~BidibIdentDlgGen()
{
	// Disconnect Events
	m_Tree->Disconnect( wxEVT_COMMAND_TREE_BEGIN_DRAG, wxTreeEventHandler( BidibIdentDlgGen::onBeginDrag ), NULL, this );
	m_Tree->Disconnect( wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler( BidibIdentDlgGen::onItemActivated ), NULL, this );
	m_Tree->Disconnect( wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK, wxTreeEventHandler( BidibIdentDlgGen::onItemRightClick ), NULL, this );
	m_Tree->Disconnect( wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler( BidibIdentDlgGen::onTreeSelChanged ), NULL, this );
	m_FeatureList->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( BidibIdentDlgGen::onFeatureSelect ), NULL, this );
	m_FeaturesGet->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BidibIdentDlgGen::onFeaturesGet ), NULL, this );
	m_FeatureSet->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BidibIdentDlgGen::onFeatureSet ), NULL, this );
	m_AccessoryOn->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BidibIdentDlgGen::onAccessoryOnTest ), NULL, this );
	m_AccessoryOff->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BidibIdentDlgGen::onAccessoryOffTest ), NULL, this );
	m_AccessoryReadOptions->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BidibIdentDlgGen::onAccessoryReadOptions ), NULL, this );
	m_AccessoryWriteOptions->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BidibIdentDlgGen::onAccessoryWriteOptions ), NULL, this );
	m_AccessoryReadMacroMap->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BidibIdentDlgGen::onAccessoryReadMacroMap ), NULL, this );
	m_AccessoryWriteMacroMap->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BidibIdentDlgGen::onAccessoryWriteMacroMap ), NULL, this );
	m_ServoLeft->Disconnect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( BidibIdentDlgGen::onServoLeft ), NULL, this );
	m_ServoRight->Disconnect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( BidibIdentDlgGen::onServoRight ), NULL, this );
	m_ServoSpeed->Disconnect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( BidibIdentDlgGen::onServoSpeed ), NULL, this );
	m_ServoReserved->Disconnect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( BidibIdentDlgGen::onServoReserved ), NULL, this );
	m_ConfigL->Disconnect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( BidibIdentDlgGen::onConfigL ), NULL, this );
	m_ConfigL->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( BidibIdentDlgGen::onConfigLtxt ), NULL, this );
	m_ConfigR->Disconnect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( BidibIdentDlgGen::onConfigR ), NULL, this );
	m_ConfigR->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( BidibIdentDlgGen::onConfigRtxt ), NULL, this );
	m_ConfigV->Disconnect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( BidibIdentDlgGen::onConfigV ), NULL, this );
	m_ConfigV->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( BidibIdentDlgGen::onConfigVtxt ), NULL, this );
	m_ConfigS->Disconnect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( BidibIdentDlgGen::onConfigS ), NULL, this );
	m_ConfigS->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( BidibIdentDlgGen::onConfigStxt ), NULL, this );
	m_PortType->Disconnect( wxEVT_COMMAND_RADIOBOX_SELECTED, wxCommandEventHandler( BidibIdentDlgGen::onPortType ), NULL, this );
	m_ServoPort->Disconnect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( BidibIdentDlgGen::onServoPort ), NULL, this );
	m_ServoGet->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BidibIdentDlgGen::onServoGet ), NULL, this );
	m_ServoLeftTest->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BidibIdentDlgGen::onServoLeftTest ), NULL, this );
	m_ServoRightTest->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BidibIdentDlgGen::onServoRightTest ), NULL, this );
	m_MacroList->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( BidibIdentDlgGen::onMacroList ), NULL, this );
	m_MacroLines->Disconnect( wxEVT_GRID_CELL_LEFT_CLICK, wxGridEventHandler( BidibIdentDlgGen::onMacroLineSelected ), NULL, this );
	m_MacroStart30->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( BidibIdentDlgGen::onMacroEveryMinute ), NULL, this );
	m_MacroStart15->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( BidibIdentDlgGen::onMacroEveryMinute ), NULL, this );
	m_MacroStart1->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( BidibIdentDlgGen::onMacroEveryMinute ), NULL, this );
	m_MacroApply->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BidibIdentDlgGen::onMacroApply ), NULL, this );
	m_MacroReload->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BidibIdentDlgGen::onMacroReload ), NULL, this );
	m_MacroSave->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BidibIdentDlgGen::onMacroSave ), NULL, this );
	m_MacroExport->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BidibIdentDlgGen::onMacroExport ), NULL, this );
	m_MacroImport->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BidibIdentDlgGen::onMacroImport ), NULL, this );
	m_MacroSaveMacro->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BidibIdentDlgGen::onMacroSaveMacro ), NULL, this );
	m_MacroDeleteMacro->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BidibIdentDlgGen::onMacroDeleteMacro ), NULL, this );
	m_MacroRestoreMacro->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BidibIdentDlgGen::onMacroRestoreMacro ), NULL, this );
	m_MacroTest->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BidibIdentDlgGen::onMacroTest ), NULL, this );
	m_OpenUpdateFile->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BidibIdentDlgGen::onSelectUpdateFile ), NULL, this );
	m_UpdateStart->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BidibIdentDlgGen::onUpdateStart ), NULL, this );
	m_stdButtonsCancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BidibIdentDlgGen::onCancel ), NULL, this );
	m_stdButtonsOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BidibIdentDlgGen::onOK ), NULL, this );
	
}
