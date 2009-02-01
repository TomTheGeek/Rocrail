///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 21 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "speedcurvedlggen.h"

///////////////////////////////////////////////////////////////////////////

speedcurvedlggen::speedcurvedlggen( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	m_textCtrl1 = new wxTextCtrl( this, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 40,-1 ), wxTE_CENTRE );
	bSizer3->Add( m_textCtrl1, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP, 5 );
	
	m_slider1 = new wxSlider( this, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxSize( -1,100 ), wxSL_INVERSE|wxSL_VERTICAL );
	bSizer3->Add( m_slider1, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, wxT("15"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	bSizer3->Add( m_staticText1, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM, 5 );
	
	bSizer2->Add( bSizer3, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer31;
	bSizer31 = new wxBoxSizer( wxVERTICAL );
	
	m_textCtrl11 = new wxTextCtrl( this, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 40,-1 ), wxTE_CENTRE );
	bSizer31->Add( m_textCtrl11, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP, 5 );
	
	m_slider11 = new wxSlider( this, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxSize( -1,100 ), wxSL_INVERSE|wxSL_VERTICAL );
	bSizer31->Add( m_slider11, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText11 = new wxStaticText( this, wxID_ANY, wxT("16"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText11->Wrap( -1 );
	bSizer31->Add( m_staticText11, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM, 5 );
	
	bSizer2->Add( bSizer31, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer32;
	bSizer32 = new wxBoxSizer( wxVERTICAL );
	
	m_textCtrl12 = new wxTextCtrl( this, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 40,-1 ), wxTE_CENTRE );
	bSizer32->Add( m_textCtrl12, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP, 5 );
	
	m_slider12 = new wxSlider( this, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxSize( -1,100 ), wxSL_INVERSE|wxSL_VERTICAL );
	bSizer32->Add( m_slider12, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText12 = new wxStaticText( this, wxID_ANY, wxT("17"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText12->Wrap( -1 );
	bSizer32->Add( m_staticText12, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM, 5 );
	
	bSizer2->Add( bSizer32, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer33;
	bSizer33 = new wxBoxSizer( wxVERTICAL );
	
	m_textCtrl13 = new wxTextCtrl( this, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 40,-1 ), wxTE_CENTRE );
	bSizer33->Add( m_textCtrl13, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP, 5 );
	
	m_slider13 = new wxSlider( this, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxSize( -1,100 ), wxSL_INVERSE|wxSL_VERTICAL );
	bSizer33->Add( m_slider13, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText13 = new wxStaticText( this, wxID_ANY, wxT("18"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText13->Wrap( -1 );
	bSizer33->Add( m_staticText13, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM, 5 );
	
	bSizer2->Add( bSizer33, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer34;
	bSizer34 = new wxBoxSizer( wxVERTICAL );
	
	m_textCtrl14 = new wxTextCtrl( this, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 40,-1 ), wxTE_CENTRE );
	bSizer34->Add( m_textCtrl14, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP, 5 );
	
	m_slider14 = new wxSlider( this, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxSize( -1,100 ), wxSL_INVERSE|wxSL_VERTICAL );
	bSizer34->Add( m_slider14, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText14 = new wxStaticText( this, wxID_ANY, wxT("19"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText14->Wrap( -1 );
	bSizer34->Add( m_staticText14, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM, 5 );
	
	bSizer2->Add( bSizer34, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer35;
	bSizer35 = new wxBoxSizer( wxVERTICAL );
	
	m_textCtrl15 = new wxTextCtrl( this, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 40,-1 ), wxTE_CENTRE );
	bSizer35->Add( m_textCtrl15, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP, 5 );
	
	m_slider15 = new wxSlider( this, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxSize( -1,100 ), wxSL_INVERSE|wxSL_VERTICAL );
	bSizer35->Add( m_slider15, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText15 = new wxStaticText( this, wxID_ANY, wxT("20"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText15->Wrap( -1 );
	bSizer35->Add( m_staticText15, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM, 5 );
	
	bSizer2->Add( bSizer35, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer36;
	bSizer36 = new wxBoxSizer( wxVERTICAL );
	
	m_textCtrl16 = new wxTextCtrl( this, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 40,-1 ), wxTE_CENTRE );
	bSizer36->Add( m_textCtrl16, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP, 5 );
	
	m_slider16 = new wxSlider( this, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxSize( -1,100 ), wxSL_INVERSE|wxSL_VERTICAL );
	bSizer36->Add( m_slider16, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText16 = new wxStaticText( this, wxID_ANY, wxT("21"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText16->Wrap( -1 );
	bSizer36->Add( m_staticText16, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM, 5 );
	
	bSizer2->Add( bSizer36, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer37;
	bSizer37 = new wxBoxSizer( wxVERTICAL );
	
	m_textCtrl17 = new wxTextCtrl( this, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 40,-1 ), wxTE_CENTRE );
	bSizer37->Add( m_textCtrl17, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP, 5 );
	
	m_slider17 = new wxSlider( this, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxSize( -1,100 ), wxSL_INVERSE|wxSL_VERTICAL );
	bSizer37->Add( m_slider17, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText17 = new wxStaticText( this, wxID_ANY, wxT("22"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText17->Wrap( -1 );
	bSizer37->Add( m_staticText17, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM, 5 );
	
	bSizer2->Add( bSizer37, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer38;
	bSizer38 = new wxBoxSizer( wxVERTICAL );
	
	m_textCtrl18 = new wxTextCtrl( this, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 40,-1 ), wxTE_CENTRE );
	bSizer38->Add( m_textCtrl18, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP, 5 );
	
	m_slider18 = new wxSlider( this, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxSize( -1,100 ), wxSL_INVERSE|wxSL_VERTICAL );
	bSizer38->Add( m_slider18, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText18 = new wxStaticText( this, wxID_ANY, wxT("23"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText18->Wrap( -1 );
	bSizer38->Add( m_staticText18, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM, 5 );
	
	bSizer2->Add( bSizer38, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer39;
	bSizer39 = new wxBoxSizer( wxVERTICAL );
	
	m_textCtrl19 = new wxTextCtrl( this, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 40,-1 ), wxTE_CENTRE );
	bSizer39->Add( m_textCtrl19, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP, 5 );
	
	m_slider19 = new wxSlider( this, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxSize( -1,100 ), wxSL_INVERSE|wxSL_VERTICAL );
	bSizer39->Add( m_slider19, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText19 = new wxStaticText( this, wxID_ANY, wxT("24"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText19->Wrap( -1 );
	bSizer39->Add( m_staticText19, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM, 5 );
	
	bSizer2->Add( bSizer39, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer310;
	bSizer310 = new wxBoxSizer( wxVERTICAL );
	
	m_textCtrl110 = new wxTextCtrl( this, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 40,-1 ), wxTE_CENTRE );
	bSizer310->Add( m_textCtrl110, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP, 5 );
	
	m_slider110 = new wxSlider( this, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxSize( -1,100 ), wxSL_INVERSE|wxSL_VERTICAL );
	bSizer310->Add( m_slider110, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText110 = new wxStaticText( this, wxID_ANY, wxT("25"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText110->Wrap( -1 );
	bSizer310->Add( m_staticText110, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM, 5 );
	
	bSizer2->Add( bSizer310, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer311;
	bSizer311 = new wxBoxSizer( wxVERTICAL );
	
	m_textCtrl111 = new wxTextCtrl( this, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 40,-1 ), wxTE_CENTRE );
	bSizer311->Add( m_textCtrl111, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP, 5 );
	
	m_slider111 = new wxSlider( this, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxSize( -1,100 ), wxSL_INVERSE|wxSL_VERTICAL );
	bSizer311->Add( m_slider111, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText111 = new wxStaticText( this, wxID_ANY, wxT("26"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText111->Wrap( -1 );
	bSizer311->Add( m_staticText111, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM, 5 );
	
	bSizer2->Add( bSizer311, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer312;
	bSizer312 = new wxBoxSizer( wxVERTICAL );
	
	m_textCtrl112 = new wxTextCtrl( this, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 40,-1 ), wxTE_CENTRE );
	bSizer312->Add( m_textCtrl112, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP, 5 );
	
	m_slider112 = new wxSlider( this, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxSize( -1,100 ), wxSL_INVERSE|wxSL_VERTICAL );
	bSizer312->Add( m_slider112, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText112 = new wxStaticText( this, wxID_ANY, wxT("27"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText112->Wrap( -1 );
	bSizer312->Add( m_staticText112, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM, 5 );
	
	bSizer2->Add( bSizer312, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer313;
	bSizer313 = new wxBoxSizer( wxVERTICAL );
	
	m_textCtrl113 = new wxTextCtrl( this, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 40,-1 ), wxTE_CENTRE );
	bSizer313->Add( m_textCtrl113, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP, 5 );
	
	m_slider113 = new wxSlider( this, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxSize( -1,100 ), wxSL_INVERSE|wxSL_VERTICAL );
	bSizer313->Add( m_slider113, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText113 = new wxStaticText( this, wxID_ANY, wxT("28"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText113->Wrap( -1 );
	bSizer313->Add( m_staticText113, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM, 5 );
	
	bSizer2->Add( bSizer313, 1, wxEXPAND, 5 );
	
	bSizer1->Add( bSizer2, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer21;
	bSizer21 = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* bSizer314;
	bSizer314 = new wxBoxSizer( wxVERTICAL );
	
	m_textCtrl114 = new wxTextCtrl( this, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 40,-1 ), wxTE_CENTRE );
	bSizer314->Add( m_textCtrl114, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP, 5 );
	
	m_slider114 = new wxSlider( this, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxSize( -1,100 ), wxSL_INVERSE|wxSL_VERTICAL );
	bSizer314->Add( m_slider114, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText114 = new wxStaticText( this, wxID_ANY, wxT("1"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText114->Wrap( -1 );
	bSizer314->Add( m_staticText114, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM, 5 );
	
	bSizer21->Add( bSizer314, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer315;
	bSizer315 = new wxBoxSizer( wxVERTICAL );
	
	m_textCtrl115 = new wxTextCtrl( this, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 40,-1 ), wxTE_CENTRE );
	bSizer315->Add( m_textCtrl115, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP, 5 );
	
	m_slider115 = new wxSlider( this, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxSize( -1,100 ), wxSL_INVERSE|wxSL_VERTICAL );
	bSizer315->Add( m_slider115, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText115 = new wxStaticText( this, wxID_ANY, wxT("2"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText115->Wrap( -1 );
	bSizer315->Add( m_staticText115, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM, 5 );
	
	bSizer21->Add( bSizer315, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer321;
	bSizer321 = new wxBoxSizer( wxVERTICAL );
	
	m_textCtrl121 = new wxTextCtrl( this, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 40,-1 ), wxTE_CENTRE );
	bSizer321->Add( m_textCtrl121, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP, 5 );
	
	m_slider121 = new wxSlider( this, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxSize( -1,100 ), wxSL_INVERSE|wxSL_VERTICAL );
	bSizer321->Add( m_slider121, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText121 = new wxStaticText( this, wxID_ANY, wxT("3"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText121->Wrap( -1 );
	bSizer321->Add( m_staticText121, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM, 5 );
	
	bSizer21->Add( bSizer321, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer331;
	bSizer331 = new wxBoxSizer( wxVERTICAL );
	
	m_textCtrl131 = new wxTextCtrl( this, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 40,-1 ), wxTE_CENTRE );
	bSizer331->Add( m_textCtrl131, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP, 5 );
	
	m_slider131 = new wxSlider( this, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxSize( -1,100 ), wxSL_INVERSE|wxSL_VERTICAL );
	bSizer331->Add( m_slider131, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText131 = new wxStaticText( this, wxID_ANY, wxT("4"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText131->Wrap( -1 );
	bSizer331->Add( m_staticText131, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM, 5 );
	
	bSizer21->Add( bSizer331, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer341;
	bSizer341 = new wxBoxSizer( wxVERTICAL );
	
	m_textCtrl141 = new wxTextCtrl( this, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 40,-1 ), wxTE_CENTRE );
	bSizer341->Add( m_textCtrl141, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP, 5 );
	
	m_slider141 = new wxSlider( this, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxSize( -1,100 ), wxSL_INVERSE|wxSL_VERTICAL );
	bSizer341->Add( m_slider141, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText141 = new wxStaticText( this, wxID_ANY, wxT("5"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText141->Wrap( -1 );
	bSizer341->Add( m_staticText141, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM, 5 );
	
	bSizer21->Add( bSizer341, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer351;
	bSizer351 = new wxBoxSizer( wxVERTICAL );
	
	m_textCtrl151 = new wxTextCtrl( this, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 40,-1 ), wxTE_CENTRE );
	bSizer351->Add( m_textCtrl151, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP, 5 );
	
	m_slider151 = new wxSlider( this, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxSize( -1,100 ), wxSL_INVERSE|wxSL_VERTICAL );
	bSizer351->Add( m_slider151, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText151 = new wxStaticText( this, wxID_ANY, wxT("6"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText151->Wrap( -1 );
	bSizer351->Add( m_staticText151, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM, 5 );
	
	bSizer21->Add( bSizer351, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer361;
	bSizer361 = new wxBoxSizer( wxVERTICAL );
	
	m_textCtrl161 = new wxTextCtrl( this, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 40,-1 ), wxTE_CENTRE );
	bSizer361->Add( m_textCtrl161, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP, 5 );
	
	m_slider161 = new wxSlider( this, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxSize( -1,100 ), wxSL_INVERSE|wxSL_VERTICAL );
	bSizer361->Add( m_slider161, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText161 = new wxStaticText( this, wxID_ANY, wxT("7"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText161->Wrap( -1 );
	bSizer361->Add( m_staticText161, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM, 5 );
	
	bSizer21->Add( bSizer361, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer371;
	bSizer371 = new wxBoxSizer( wxVERTICAL );
	
	m_textCtrl171 = new wxTextCtrl( this, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 40,-1 ), wxTE_CENTRE );
	bSizer371->Add( m_textCtrl171, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP, 5 );
	
	m_slider171 = new wxSlider( this, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxSize( -1,100 ), wxSL_INVERSE|wxSL_VERTICAL );
	bSizer371->Add( m_slider171, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText171 = new wxStaticText( this, wxID_ANY, wxT("8"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText171->Wrap( -1 );
	bSizer371->Add( m_staticText171, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM, 5 );
	
	bSizer21->Add( bSizer371, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer381;
	bSizer381 = new wxBoxSizer( wxVERTICAL );
	
	m_textCtrl181 = new wxTextCtrl( this, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 40,-1 ), wxTE_CENTRE );
	bSizer381->Add( m_textCtrl181, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP, 5 );
	
	m_slider181 = new wxSlider( this, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxSize( -1,100 ), wxSL_INVERSE|wxSL_VERTICAL );
	bSizer381->Add( m_slider181, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText181 = new wxStaticText( this, wxID_ANY, wxT("9"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText181->Wrap( -1 );
	bSizer381->Add( m_staticText181, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM, 5 );
	
	bSizer21->Add( bSizer381, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer391;
	bSizer391 = new wxBoxSizer( wxVERTICAL );
	
	m_textCtrl191 = new wxTextCtrl( this, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 40,-1 ), wxTE_CENTRE );
	bSizer391->Add( m_textCtrl191, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP, 5 );
	
	m_slider191 = new wxSlider( this, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxSize( -1,100 ), wxSL_INVERSE|wxSL_VERTICAL );
	bSizer391->Add( m_slider191, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText191 = new wxStaticText( this, wxID_ANY, wxT("10"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText191->Wrap( -1 );
	bSizer391->Add( m_staticText191, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM, 5 );
	
	bSizer21->Add( bSizer391, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer3101;
	bSizer3101 = new wxBoxSizer( wxVERTICAL );
	
	m_textCtrl1101 = new wxTextCtrl( this, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 40,-1 ), wxTE_CENTRE );
	bSizer3101->Add( m_textCtrl1101, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP, 5 );
	
	m_slider1101 = new wxSlider( this, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxSize( -1,100 ), wxSL_INVERSE|wxSL_VERTICAL );
	bSizer3101->Add( m_slider1101, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText1101 = new wxStaticText( this, wxID_ANY, wxT("11"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1101->Wrap( -1 );
	bSizer3101->Add( m_staticText1101, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM, 5 );
	
	bSizer21->Add( bSizer3101, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer3111;
	bSizer3111 = new wxBoxSizer( wxVERTICAL );
	
	m_textCtrl1111 = new wxTextCtrl( this, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 40,-1 ), wxTE_CENTRE );
	bSizer3111->Add( m_textCtrl1111, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP, 5 );
	
	m_slider1111 = new wxSlider( this, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxSize( -1,100 ), wxSL_INVERSE|wxSL_VERTICAL );
	bSizer3111->Add( m_slider1111, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText1111 = new wxStaticText( this, wxID_ANY, wxT("12"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1111->Wrap( -1 );
	bSizer3111->Add( m_staticText1111, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM, 5 );
	
	bSizer21->Add( bSizer3111, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer3121;
	bSizer3121 = new wxBoxSizer( wxVERTICAL );
	
	m_textCtrl1121 = new wxTextCtrl( this, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 40,-1 ), wxTE_CENTRE );
	bSizer3121->Add( m_textCtrl1121, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP, 5 );
	
	m_slider1121 = new wxSlider( this, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxSize( -1,100 ), wxSL_INVERSE|wxSL_VERTICAL );
	bSizer3121->Add( m_slider1121, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText1121 = new wxStaticText( this, wxID_ANY, wxT("13"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1121->Wrap( -1 );
	bSizer3121->Add( m_staticText1121, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM, 5 );
	
	bSizer21->Add( bSizer3121, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer3131;
	bSizer3131 = new wxBoxSizer( wxVERTICAL );
	
	m_textCtrl1131 = new wxTextCtrl( this, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 40,-1 ), wxTE_CENTRE );
	bSizer3131->Add( m_textCtrl1131, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP, 5 );
	
	m_slider1131 = new wxSlider( this, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxSize( -1,100 ), wxSL_INVERSE|wxSL_VERTICAL );
	bSizer3131->Add( m_slider1131, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText1131 = new wxStaticText( this, wxID_ANY, wxT("14"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1131->Wrap( -1 );
	bSizer3131->Add( m_staticText1131, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM, 5 );
	
	bSizer21->Add( bSizer3131, 1, wxEXPAND, 5 );
	
	bSizer1->Add( bSizer21, 1, wxEXPAND, 5 );
	
	m_staticline2 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer1->Add( m_staticline2, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer71;
	bSizer71 = new wxBoxSizer( wxHORIZONTAL );
	
	m_button1 = new wxButton( this, wxID_ANY, wxT("Read"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer71->Add( m_button1, 0, wxALL, 5 );
	
	m_button2 = new wxButton( this, wxID_ANY, wxT("Write"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer71->Add( m_button2, 0, wxALL, 5 );
	
	m_button3 = new wxButton( this, wxID_ANY, wxT("Linearize"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer71->Add( m_button3, 0, wxALL, 5 );
	
	bSizer1->Add( bSizer71, 0, wxEXPAND, 5 );
	
	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer1->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );
	
	m_sdbSizer1 = new wxStdDialogButtonSizer();
	m_sdbSizer1OK = new wxButton( this, wxID_OK );
	m_sdbSizer1->AddButton( m_sdbSizer1OK );
	m_sdbSizer1->Realize();
	bSizer1->Add( m_sdbSizer1, 0, wxEXPAND, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	bSizer1->Fit( this );
}

speedcurvedlggen::~speedcurvedlggen()
{
}
