///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "zoomdlggen.h"

///////////////////////////////////////////////////////////////////////////

ZoomDlgGen::ZoomDlgGen( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_ZoomValue = new wxComboBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 100,-1 ), 0, NULL, 0 ); 
	fgSizer1->Add( m_ZoomValue, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_labPercent = new wxStaticText( this, wxID_ANY, wxT("%"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labPercent->Wrap( -1 );
	fgSizer1->Add( m_labPercent, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	bSizer1->Add( fgSizer1, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_ZoomSlider = new wxSlider( this, wxID_ANY, 100, 25, 200, wxDefaultPosition, wxDefaultSize, wxSL_AUTOTICKS|wxSL_BOTTOM|wxSL_HORIZONTAL|wxSL_LABELS );
	bSizer1->Add( m_ZoomSlider, 0, wxALL|wxEXPAND, 5 );
	
	m_sdbSizer1 = new wxStdDialogButtonSizer();
	m_sdbSizer1OK = new wxButton( this, wxID_OK );
	m_sdbSizer1->AddButton( m_sdbSizer1OK );
	m_sdbSizer1Cancel = new wxButton( this, wxID_CANCEL );
	m_sdbSizer1->AddButton( m_sdbSizer1Cancel );
	m_sdbSizer1Help = new wxButton( this, wxID_HELP );
	m_sdbSizer1->AddButton( m_sdbSizer1Help );
	m_sdbSizer1->Realize();
	bSizer1->Add( m_sdbSizer1, 0, wxALL|wxALIGN_RIGHT, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	bSizer1->Fit( this );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_ZoomValue->Connect( wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler( ZoomDlgGen::onZoomSelect ), NULL, this );
	m_ZoomValue->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( ZoomDlgGen::onZoomEnter ), NULL, this );
	m_ZoomSlider->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( ZoomDlgGen::onZoomThumb ), NULL, this );
	m_ZoomSlider->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( ZoomDlgGen::onZoomThumb ), NULL, this );
	m_ZoomSlider->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( ZoomDlgGen::onZoomThumb ), NULL, this );
	m_ZoomSlider->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( ZoomDlgGen::onZoomThumb ), NULL, this );
	m_ZoomSlider->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( ZoomDlgGen::onZoomThumb ), NULL, this );
	m_ZoomSlider->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( ZoomDlgGen::onZoomThumb ), NULL, this );
	m_ZoomSlider->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( ZoomDlgGen::onZoomThumb ), NULL, this );
	m_ZoomSlider->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( ZoomDlgGen::onZoomThumb ), NULL, this );
	m_ZoomSlider->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( ZoomDlgGen::onZoomThumb ), NULL, this );
	m_ZoomSlider->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( ZoomDlgGen::onZoomThumb ), NULL, this );
	m_ZoomSlider->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( ZoomDlgGen::onZoomThumb ), NULL, this );
	m_ZoomSlider->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( ZoomDlgGen::onZoomThumb ), NULL, this );
	m_ZoomSlider->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( ZoomDlgGen::onZoomThumb ), NULL, this );
	m_ZoomSlider->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( ZoomDlgGen::onZoomRelease ), NULL, this );
	m_ZoomSlider->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( ZoomDlgGen::onZoomThumb ), NULL, this );
	m_sdbSizer1Cancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ZoomDlgGen::onCancel ), NULL, this );
	m_sdbSizer1Help->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ZoomDlgGen::onHelp ), NULL, this );
	m_sdbSizer1OK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ZoomDlgGen::onOK ), NULL, this );
}

ZoomDlgGen::~ZoomDlgGen()
{
	// Disconnect Events
	m_ZoomValue->Disconnect( wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler( ZoomDlgGen::onZoomSelect ), NULL, this );
	m_ZoomValue->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( ZoomDlgGen::onZoomEnter ), NULL, this );
	m_ZoomSlider->Disconnect( wxEVT_SCROLL_TOP, wxScrollEventHandler( ZoomDlgGen::onZoomThumb ), NULL, this );
	m_ZoomSlider->Disconnect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( ZoomDlgGen::onZoomThumb ), NULL, this );
	m_ZoomSlider->Disconnect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( ZoomDlgGen::onZoomThumb ), NULL, this );
	m_ZoomSlider->Disconnect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( ZoomDlgGen::onZoomThumb ), NULL, this );
	m_ZoomSlider->Disconnect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( ZoomDlgGen::onZoomThumb ), NULL, this );
	m_ZoomSlider->Disconnect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( ZoomDlgGen::onZoomThumb ), NULL, this );
	m_ZoomSlider->Disconnect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( ZoomDlgGen::onZoomThumb ), NULL, this );
	m_ZoomSlider->Disconnect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( ZoomDlgGen::onZoomThumb ), NULL, this );
	m_ZoomSlider->Disconnect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( ZoomDlgGen::onZoomThumb ), NULL, this );
	m_ZoomSlider->Disconnect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( ZoomDlgGen::onZoomThumb ), NULL, this );
	m_ZoomSlider->Disconnect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( ZoomDlgGen::onZoomThumb ), NULL, this );
	m_ZoomSlider->Disconnect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( ZoomDlgGen::onZoomThumb ), NULL, this );
	m_ZoomSlider->Disconnect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( ZoomDlgGen::onZoomThumb ), NULL, this );
	m_ZoomSlider->Disconnect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( ZoomDlgGen::onZoomRelease ), NULL, this );
	m_ZoomSlider->Disconnect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( ZoomDlgGen::onZoomThumb ), NULL, this );
	m_sdbSizer1Cancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ZoomDlgGen::onCancel ), NULL, this );
	m_sdbSizer1Help->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ZoomDlgGen::onHelp ), NULL, this );
	m_sdbSizer1OK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ZoomDlgGen::onOK ), NULL, this );
	
}
