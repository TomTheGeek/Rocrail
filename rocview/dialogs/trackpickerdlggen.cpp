///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "trackpickerdlggen.h"

///////////////////////////////////////////////////////////////////////////

TrackPickerDlgGen::TrackPickerDlgGen( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_TrackBook = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_PageTrack = new wxPanel( m_TrackBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );
	
	m_GridTrack = new wxGrid( m_PageTrack, wxID_ANY, wxDefaultPosition, wxSize( 128,-1 ), 0 );
	
	// Grid
	m_GridTrack->CreateGrid( 5, 1 );
	m_GridTrack->EnableEditing( false );
	m_GridTrack->EnableGridLines( false );
	m_GridTrack->EnableDragGridSize( false );
	m_GridTrack->SetMargins( 0, 0 );
	
	// Columns
	m_GridTrack->AutoSizeColumns();
	m_GridTrack->EnableDragColMove( false );
	m_GridTrack->EnableDragColSize( true );
	m_GridTrack->SetColLabelSize( 0 );
	m_GridTrack->SetColLabelAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );
	
	// Rows
	m_GridTrack->AutoSizeRows();
	m_GridTrack->EnableDragRowSize( true );
	m_GridTrack->SetRowLabelSize( 0 );
	m_GridTrack->SetRowLabelAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );
	
	// Label Appearance
	
	// Cell Defaults
	m_GridTrack->SetDefaultCellAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );
	bSizer2->Add( m_GridTrack, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxEXPAND, 5 );
	
	m_PageTrack->SetSizer( bSizer2 );
	m_PageTrack->Layout();
	bSizer2->Fit( m_PageTrack );
	m_TrackBook->AddPage( m_PageTrack, wxT("Track"), true );
	
	bSizer1->Add( m_TrackBook, 1, wxEXPAND | wxALL, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	bSizer1->Fit( this );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_GridTrack->Connect( wxEVT_GRID_CELL_LEFT_CLICK, wxGridEventHandler( TrackPickerDlgGen::onTrackCellLeftClick ), NULL, this );
}

TrackPickerDlgGen::~TrackPickerDlgGen()
{
	// Disconnect Events
	m_GridTrack->Disconnect( wxEVT_GRID_CELL_LEFT_CLICK, wxGridEventHandler( TrackPickerDlgGen::onTrackCellLeftClick ), NULL, this );
	
}
