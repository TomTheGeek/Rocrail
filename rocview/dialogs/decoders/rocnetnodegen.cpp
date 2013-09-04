///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "rocnetnodegen.h"

///////////////////////////////////////////////////////////////////////////

rocnetnodegen::rocnetnodegen( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );
	
	m_notebook2 = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_IndexPanel = new wxPanel( m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer( wxVERTICAL );
	
	m_NodeList = new wxListCtrl( m_IndexPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_HRULES|wxLC_REPORT|wxLC_SINGLE_SEL );
	bSizer10->Add( m_NodeList, 1, wxALL|wxEXPAND, 5 );
	
	m_IndexPanel->SetSizer( bSizer10 );
	m_IndexPanel->Layout();
	bSizer10->Fit( m_IndexPanel );
	m_notebook2->AddPage( m_IndexPanel, wxT("Index"), false );
	m_RocNetPanel = new wxPanel( m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer5;
	fgSizer5 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer5->AddGrowableCol( 1 );
	fgSizer5->SetFlexibleDirection( wxBOTH );
	fgSizer5->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_labIID = new wxStaticText( m_RocNetPanel, wxID_ANY, wxT("IID"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labIID->Wrap( -1 );
	fgSizer5->Add( m_labIID, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_IID = new wxTextCtrl( m_RocNetPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer5->Add( m_IID, 0, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_labID = new wxStaticText( m_RocNetPanel, wxID_ANY, wxT("ID"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labID->Wrap( -1 );
	fgSizer5->Add( m_labID, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );
	
	m_ID = new wxSpinCtrl( m_RocNetPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 100,-1 ), wxSP_ARROW_KEYS, 0, 65535, 0 );
	fgSizer5->Add( m_ID, 0, wxALL, 5 );
	
	m_labVendor = new wxStaticText( m_RocNetPanel, wxID_ANY, wxT("Vendor"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labVendor->Wrap( -1 );
	fgSizer5->Add( m_labVendor, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );
	
	m_VendorName = new wxTextCtrl( m_RocNetPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	fgSizer5->Add( m_VendorName, 0, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_labProduct = new wxStaticText( m_RocNetPanel, wxID_ANY, wxT("Product"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labProduct->Wrap( -1 );
	fgSizer5->Add( m_labProduct, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_ProductName = new wxTextCtrl( m_RocNetPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	fgSizer5->Add( m_ProductName, 0, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_labVersion = new wxStaticText( m_RocNetPanel, wxID_ANY, wxT("Version"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labVersion->Wrap( -1 );
	fgSizer5->Add( m_labVersion, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_Version = new wxTextCtrl( m_RocNetPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	fgSizer5->Add( m_Version, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	bSizer6->Add( fgSizer5, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxHORIZONTAL );
	
	m_RocnetWrite = new wxButton( m_RocNetPanel, wxID_ANY, wxT("Write"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer9->Add( m_RocnetWrite, 0, wxALL, 5 );
	
	bSizer6->Add( bSizer9, 0, wxALIGN_RIGHT, 5 );
	
	m_RocNetPanel->SetSizer( bSizer6 );
	m_RocNetPanel->Layout();
	bSizer6->Fit( m_RocNetPanel );
	m_notebook2->AddPage( m_RocNetPanel, wxT("RocNet"), false );
	m_NodeOptions = new wxPanel( m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer91;
	bSizer91 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* m_RocNetOptionBox;
	m_RocNetOptionBox = new wxStaticBoxSizer( new wxStaticBox( m_NodeOptions, wxID_ANY, wxT("Options") ), wxVERTICAL );
	
	m_SecAck = new wxCheckBox( m_NodeOptions, wxID_ANY, wxT("Acknowledge sensor events"), wxDefaultPosition, wxDefaultSize, 0 );
	m_RocNetOptionBox->Add( m_SecAck, 0, wxALL, 5 );
	
	wxString m_IOTypeChoices[] = { wxT("I2C 0"), wxT("I2C 1"), wxT("GPIO") };
	int m_IOTypeNChoices = sizeof( m_IOTypeChoices ) / sizeof( wxString );
	m_IOType = new wxRadioBox( m_NodeOptions, wxID_ANY, wxT("I/O Type"), wxDefaultPosition, wxDefaultSize, m_IOTypeNChoices, m_IOTypeChoices, 1, wxRA_SPECIFY_ROWS );
	m_IOType->SetSelection( 1 );
	m_RocNetOptionBox->Add( m_IOType, 0, wxALL, 5 );
	
	bSizer91->Add( m_RocNetOptionBox, 0, wxEXPAND|wxALL, 5 );
	
	wxBoxSizer* bSizer92;
	bSizer92 = new wxBoxSizer( wxHORIZONTAL );
	
	m_NodeOptionsRead = new wxButton( m_NodeOptions, wxID_ANY, wxT("Read"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer92->Add( m_NodeOptionsRead, 0, wxALL, 5 );
	
	m_NodeOptionsWrite = new wxButton( m_NodeOptions, wxID_ANY, wxT("Write"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer92->Add( m_NodeOptionsWrite, 0, wxALL, 5 );
	
	bSizer91->Add( bSizer92, 0, wxALIGN_RIGHT, 5 );
	
	m_NodeOptions->SetSizer( bSizer91 );
	m_NodeOptions->Layout();
	bSizer91->Fit( m_NodeOptions );
	m_notebook2->AddPage( m_NodeOptions, wxT("Options"), true );
	m_PortSetupPanel = new wxPanel( m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer6;
	fgSizer6 = new wxFlexGridSizer( 0, 4, 0, 0 );
	fgSizer6->SetFlexibleDirection( wxBOTH );
	fgSizer6->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_labPort = new wxStaticText( m_PortSetupPanel, wxID_ANY, wxT("Port"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labPort->Wrap( -1 );
	fgSizer6->Add( m_labPort, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_BOTTOM|wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_labIO = new wxStaticText( m_PortSetupPanel, wxID_ANY, wxT("I/O"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labIO->Wrap( -1 );
	fgSizer6->Add( m_labIO, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_BOTTOM|wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_labType = new wxStaticText( m_PortSetupPanel, wxID_ANY, wxT("Type"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labType->Wrap( -1 );
	fgSizer6->Add( m_labType, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP|wxRIGHT|wxLEFT|wxALIGN_BOTTOM, 5 );
	
	m_labDelay = new wxStaticText( m_PortSetupPanel, wxID_ANY, wxT("Delay"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labDelay->Wrap( -1 );
	fgSizer6->Add( m_labDelay, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP|wxRIGHT|wxLEFT|wxALIGN_BOTTOM, 5 );
	
	m_labPort1 = new wxStaticText( m_PortSetupPanel, wxID_ANY, wxT("1"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labPort1->Wrap( -1 );
	fgSizer6->Add( m_labPort1, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_IO1 = new wxSpinCtrl( m_PortSetupPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 70,-1 ), wxSP_ARROW_KEYS, 0, 255, 0 );
	fgSizer6->Add( m_IO1, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	wxString m_Type1Choices[] = { wxT("O"), wxT("I") };
	int m_Type1NChoices = sizeof( m_Type1Choices ) / sizeof( wxString );
	m_Type1 = new wxRadioBox( m_PortSetupPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, m_Type1NChoices, m_Type1Choices, 1, wxRA_SPECIFY_ROWS );
	m_Type1->SetSelection( 0 );
	fgSizer6->Add( m_Type1, 0, wxLEFT|wxRIGHT|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_Delay1 = new wxSpinCtrl( m_PortSetupPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 100,-1 ), wxSP_ARROW_KEYS, 0, 65535, 0 );
	fgSizer6->Add( m_Delay1, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_labPort2 = new wxStaticText( m_PortSetupPanel, wxID_ANY, wxT("2"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labPort2->Wrap( -1 );
	fgSizer6->Add( m_labPort2, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_IO2 = new wxSpinCtrl( m_PortSetupPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 70,-1 ), wxSP_ARROW_KEYS, 0, 255, 0 );
	fgSizer6->Add( m_IO2, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	wxString m_Type2Choices[] = { wxT("O"), wxT("I") };
	int m_Type2NChoices = sizeof( m_Type2Choices ) / sizeof( wxString );
	m_Type2 = new wxRadioBox( m_PortSetupPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, m_Type2NChoices, m_Type2Choices, 1, wxRA_SPECIFY_ROWS );
	m_Type2->SetSelection( 1 );
	fgSizer6->Add( m_Type2, 0, wxALIGN_CENTER_HORIZONTAL|wxRIGHT|wxLEFT, 5 );
	
	m_Delay2 = new wxSpinCtrl( m_PortSetupPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 100,-1 ), wxSP_ARROW_KEYS, 0, 65535, 0 );
	fgSizer6->Add( m_Delay2, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_labPort3 = new wxStaticText( m_PortSetupPanel, wxID_ANY, wxT("3"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labPort3->Wrap( -1 );
	fgSizer6->Add( m_labPort3, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_IO3 = new wxSpinCtrl( m_PortSetupPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 70,-1 ), wxSP_ARROW_KEYS, 0, 255, 0 );
	fgSizer6->Add( m_IO3, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxString m_Type3Choices[] = { wxT("O"), wxT("I") };
	int m_Type3NChoices = sizeof( m_Type3Choices ) / sizeof( wxString );
	m_Type3 = new wxRadioBox( m_PortSetupPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, m_Type3NChoices, m_Type3Choices, 1, wxRA_SPECIFY_ROWS );
	m_Type3->SetSelection( 1 );
	fgSizer6->Add( m_Type3, 0, wxALIGN_CENTER_HORIZONTAL|wxRIGHT|wxLEFT, 5 );
	
	m_Delay3 = new wxSpinCtrl( m_PortSetupPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 100,-1 ), wxSP_ARROW_KEYS, 0, 65535, 0 );
	fgSizer6->Add( m_Delay3, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_labPort4 = new wxStaticText( m_PortSetupPanel, wxID_ANY, wxT("4"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labPort4->Wrap( -1 );
	fgSizer6->Add( m_labPort4, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_IO4 = new wxSpinCtrl( m_PortSetupPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 70,-1 ), wxSP_ARROW_KEYS, 0, 255, 0 );
	fgSizer6->Add( m_IO4, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxString m_Type4Choices[] = { wxT("O"), wxT("I") };
	int m_Type4NChoices = sizeof( m_Type4Choices ) / sizeof( wxString );
	m_Type4 = new wxRadioBox( m_PortSetupPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, m_Type4NChoices, m_Type4Choices, 1, wxRA_SPECIFY_ROWS );
	m_Type4->SetSelection( 1 );
	fgSizer6->Add( m_Type4, 0, wxALIGN_CENTER_HORIZONTAL|wxRIGHT|wxLEFT, 5 );
	
	m_Delay4 = new wxSpinCtrl( m_PortSetupPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 100,-1 ), wxSP_ARROW_KEYS, 0, 65535, 0 );
	fgSizer6->Add( m_Delay4, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_labPort5 = new wxStaticText( m_PortSetupPanel, wxID_ANY, wxT("5"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labPort5->Wrap( -1 );
	fgSizer6->Add( m_labPort5, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_IO5 = new wxSpinCtrl( m_PortSetupPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 70,-1 ), wxSP_ARROW_KEYS, 0, 255, 0 );
	fgSizer6->Add( m_IO5, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxString m_Type5Choices[] = { wxT("O"), wxT("I") };
	int m_Type5NChoices = sizeof( m_Type5Choices ) / sizeof( wxString );
	m_Type5 = new wxRadioBox( m_PortSetupPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, m_Type5NChoices, m_Type5Choices, 1, wxRA_SPECIFY_ROWS );
	m_Type5->SetSelection( 1 );
	fgSizer6->Add( m_Type5, 0, wxALIGN_CENTER_HORIZONTAL|wxRIGHT|wxLEFT, 5 );
	
	m_Delay5 = new wxSpinCtrl( m_PortSetupPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 100,-1 ), wxSP_ARROW_KEYS, 0, 65535, 0 );
	fgSizer6->Add( m_Delay5, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_labPort6 = new wxStaticText( m_PortSetupPanel, wxID_ANY, wxT("6"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labPort6->Wrap( -1 );
	fgSizer6->Add( m_labPort6, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_IO6 = new wxSpinCtrl( m_PortSetupPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 70,-1 ), wxSP_ARROW_KEYS, 0, 255, 0 );
	fgSizer6->Add( m_IO6, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxString m_Type6Choices[] = { wxT("O"), wxT("I") };
	int m_Type6NChoices = sizeof( m_Type6Choices ) / sizeof( wxString );
	m_Type6 = new wxRadioBox( m_PortSetupPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, m_Type6NChoices, m_Type6Choices, 1, wxRA_SPECIFY_ROWS );
	m_Type6->SetSelection( 1 );
	fgSizer6->Add( m_Type6, 0, wxALIGN_CENTER_HORIZONTAL|wxRIGHT|wxLEFT, 5 );
	
	m_Delay6 = new wxSpinCtrl( m_PortSetupPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 100,-1 ), wxSP_ARROW_KEYS, 0, 65535, 0 );
	fgSizer6->Add( m_Delay6, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_labPort7 = new wxStaticText( m_PortSetupPanel, wxID_ANY, wxT("7"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labPort7->Wrap( -1 );
	fgSizer6->Add( m_labPort7, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_IO7 = new wxSpinCtrl( m_PortSetupPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 70,-1 ), wxSP_ARROW_KEYS, 0, 255, 0 );
	fgSizer6->Add( m_IO7, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxString m_Type7Choices[] = { wxT("O"), wxT("I") };
	int m_Type7NChoices = sizeof( m_Type7Choices ) / sizeof( wxString );
	m_Type7 = new wxRadioBox( m_PortSetupPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, m_Type7NChoices, m_Type7Choices, 1, wxRA_SPECIFY_ROWS );
	m_Type7->SetSelection( 1 );
	fgSizer6->Add( m_Type7, 0, wxALIGN_CENTER_HORIZONTAL|wxRIGHT|wxLEFT, 5 );
	
	m_Delay7 = new wxSpinCtrl( m_PortSetupPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 100,-1 ), wxSP_ARROW_KEYS, 0, 65535, 0 );
	fgSizer6->Add( m_Delay7, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_labPort8 = new wxStaticText( m_PortSetupPanel, wxID_ANY, wxT("8"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labPort8->Wrap( -1 );
	fgSizer6->Add( m_labPort8, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_IO8 = new wxSpinCtrl( m_PortSetupPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 70,-1 ), wxSP_ARROW_KEYS, 0, 255, 0 );
	fgSizer6->Add( m_IO8, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxString m_Type8Choices[] = { wxT("O"), wxT("I") };
	int m_Type8NChoices = sizeof( m_Type8Choices ) / sizeof( wxString );
	m_Type8 = new wxRadioBox( m_PortSetupPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, m_Type8NChoices, m_Type8Choices, 1, wxRA_SPECIFY_ROWS );
	m_Type8->SetSelection( 1 );
	fgSizer6->Add( m_Type8, 0, wxALIGN_CENTER_HORIZONTAL|wxRIGHT|wxLEFT, 5 );
	
	m_Delay8 = new wxSpinCtrl( m_PortSetupPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 100,-1 ), wxSP_ARROW_KEYS, 0, 65535, 0 );
	fgSizer6->Add( m_Delay8, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5 );
	
	bSizer7->Add( fgSizer6, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxHORIZONTAL );
	
	m_PortPrev = new wxButton( m_PortSetupPanel, wxID_ANY, wxT("<"), wxDefaultPosition, wxSize( 50,-1 ), 0 );
	bSizer8->Add( m_PortPrev, 0, wxALL, 5 );
	
	m_PortNext = new wxButton( m_PortSetupPanel, wxID_ANY, wxT(">"), wxDefaultPosition, wxSize( 50,-1 ), 0 );
	bSizer8->Add( m_PortNext, 0, wxALL, 5 );
	
	m_PortRead = new wxButton( m_PortSetupPanel, wxID_ANY, wxT("Read"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer8->Add( m_PortRead, 0, wxALL, 5 );
	
	m_PortWrite = new wxButton( m_PortSetupPanel, wxID_ANY, wxT("Write"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer8->Add( m_PortWrite, 0, wxALL, 5 );
	
	bSizer7->Add( bSizer8, 0, 0, 5 );
	
	m_PortSetupPanel->SetSizer( bSizer7 );
	m_PortSetupPanel->Layout();
	bSizer7->Fit( m_PortSetupPanel );
	m_notebook2->AddPage( m_PortSetupPanel, wxT("Port Setup"), false );
	
	bSizer5->Add( m_notebook2, 0, wxEXPAND | wxALL, 5 );
	
	m_StdButton = new wxStdDialogButtonSizer();
	m_StdButtonOK = new wxButton( this, wxID_OK );
	m_StdButton->AddButton( m_StdButtonOK );
	m_StdButton->Realize();
	bSizer5->Add( m_StdButton, 0, wxALL|wxALIGN_RIGHT, 5 );
	
	this->SetSizer( bSizer5 );
	this->Layout();
	bSizer5->Fit( this );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( rocnetnodegen::onClose ) );
	m_NodeList->Connect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( rocnetnodegen::onIndexSelected ), NULL, this );
	m_NodeList->Connect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( rocnetnodegen::onIndexSelected ), NULL, this );
	m_RocnetWrite->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( rocnetnodegen::onRocnetWrite ), NULL, this );
	m_NodeOptionsRead->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( rocnetnodegen::onNodeOptionsRead ), NULL, this );
	m_NodeOptionsWrite->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( rocnetnodegen::onNodeOptionsWrite ), NULL, this );
	m_PortPrev->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( rocnetnodegen::onPortPrev ), NULL, this );
	m_PortNext->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( rocnetnodegen::onPortNext ), NULL, this );
	m_PortRead->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( rocnetnodegen::onPortRead ), NULL, this );
	m_PortWrite->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( rocnetnodegen::onPortWrite ), NULL, this );
	m_StdButtonOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( rocnetnodegen::onOK ), NULL, this );
}

rocnetnodegen::~rocnetnodegen()
{
	// Disconnect Events
	this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( rocnetnodegen::onClose ) );
	m_NodeList->Disconnect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( rocnetnodegen::onIndexSelected ), NULL, this );
	m_NodeList->Disconnect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( rocnetnodegen::onIndexSelected ), NULL, this );
	m_RocnetWrite->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( rocnetnodegen::onRocnetWrite ), NULL, this );
	m_NodeOptionsRead->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( rocnetnodegen::onNodeOptionsRead ), NULL, this );
	m_NodeOptionsWrite->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( rocnetnodegen::onNodeOptionsWrite ), NULL, this );
	m_PortPrev->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( rocnetnodegen::onPortPrev ), NULL, this );
	m_PortNext->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( rocnetnodegen::onPortNext ), NULL, this );
	m_PortRead->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( rocnetnodegen::onPortRead ), NULL, this );
	m_PortWrite->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( rocnetnodegen::onPortWrite ), NULL, this );
	m_StdButtonOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( rocnetnodegen::onOK ), NULL, this );
	
}
