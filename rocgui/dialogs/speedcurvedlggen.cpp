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
	
	m_Step15 = new wxTextCtrl( this, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 40,-1 ), wxTE_CENTRE );
	m_Step15->SetMaxLength( 3 ); 
	bSizer3->Add( m_Step15, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP, 5 );
	
	m_SliderStep15 = new wxSlider( this, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxSize( -1,100 ), wxSL_INVERSE|wxSL_VERTICAL );
	bSizer3->Add( m_SliderStep15, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, wxT("15"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	bSizer3->Add( m_staticText1, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM, 5 );
	
	bSizer2->Add( bSizer3, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer31;
	bSizer31 = new wxBoxSizer( wxVERTICAL );
	
	m_Step16 = new wxTextCtrl( this, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 40,-1 ), wxTE_CENTRE );
	m_Step16->SetMaxLength( 3 ); 
	bSizer31->Add( m_Step16, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP, 5 );
	
	m_SliderStep16 = new wxSlider( this, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxSize( -1,100 ), wxSL_INVERSE|wxSL_VERTICAL );
	bSizer31->Add( m_SliderStep16, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText11 = new wxStaticText( this, wxID_ANY, wxT("16"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText11->Wrap( -1 );
	bSizer31->Add( m_staticText11, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM, 5 );
	
	bSizer2->Add( bSizer31, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer32;
	bSizer32 = new wxBoxSizer( wxVERTICAL );
	
	m_Step17 = new wxTextCtrl( this, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 40,-1 ), wxTE_CENTRE );
	m_Step17->SetMaxLength( 3 ); 
	bSizer32->Add( m_Step17, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP, 5 );
	
	m_SliderStep17 = new wxSlider( this, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxSize( -1,100 ), wxSL_INVERSE|wxSL_VERTICAL );
	bSizer32->Add( m_SliderStep17, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText12 = new wxStaticText( this, wxID_ANY, wxT("17"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText12->Wrap( -1 );
	bSizer32->Add( m_staticText12, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM, 5 );
	
	bSizer2->Add( bSizer32, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer33;
	bSizer33 = new wxBoxSizer( wxVERTICAL );
	
	m_Step18 = new wxTextCtrl( this, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 40,-1 ), wxTE_CENTRE );
	m_Step18->SetMaxLength( 3 ); 
	bSizer33->Add( m_Step18, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP, 5 );
	
	m_SliderStep18 = new wxSlider( this, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxSize( -1,100 ), wxSL_INVERSE|wxSL_VERTICAL );
	bSizer33->Add( m_SliderStep18, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText13 = new wxStaticText( this, wxID_ANY, wxT("18"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText13->Wrap( -1 );
	bSizer33->Add( m_staticText13, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM, 5 );
	
	bSizer2->Add( bSizer33, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer34;
	bSizer34 = new wxBoxSizer( wxVERTICAL );
	
	m_Step19 = new wxTextCtrl( this, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 40,-1 ), wxTE_CENTRE );
	m_Step19->SetMaxLength( 3 ); 
	bSizer34->Add( m_Step19, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP, 5 );
	
	m_SliderStep19 = new wxSlider( this, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxSize( -1,100 ), wxSL_INVERSE|wxSL_VERTICAL );
	bSizer34->Add( m_SliderStep19, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText14 = new wxStaticText( this, wxID_ANY, wxT("19"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText14->Wrap( -1 );
	bSizer34->Add( m_staticText14, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM, 5 );
	
	bSizer2->Add( bSizer34, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer35;
	bSizer35 = new wxBoxSizer( wxVERTICAL );
	
	m_Step20 = new wxTextCtrl( this, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 40,-1 ), wxTE_CENTRE );
	m_Step20->SetMaxLength( 3 ); 
	bSizer35->Add( m_Step20, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP, 5 );
	
	m_SliderStep20 = new wxSlider( this, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxSize( -1,100 ), wxSL_INVERSE|wxSL_VERTICAL );
	bSizer35->Add( m_SliderStep20, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText15 = new wxStaticText( this, wxID_ANY, wxT("20"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText15->Wrap( -1 );
	bSizer35->Add( m_staticText15, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM, 5 );
	
	bSizer2->Add( bSizer35, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer36;
	bSizer36 = new wxBoxSizer( wxVERTICAL );
	
	m_Step21 = new wxTextCtrl( this, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 40,-1 ), wxTE_CENTRE );
	m_Step21->SetMaxLength( 3 ); 
	bSizer36->Add( m_Step21, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP, 5 );
	
	m_SliderStep21 = new wxSlider( this, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxSize( -1,100 ), wxSL_INVERSE|wxSL_VERTICAL );
	bSizer36->Add( m_SliderStep21, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText16 = new wxStaticText( this, wxID_ANY, wxT("21"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText16->Wrap( -1 );
	bSizer36->Add( m_staticText16, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM, 5 );
	
	bSizer2->Add( bSizer36, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer37;
	bSizer37 = new wxBoxSizer( wxVERTICAL );
	
	m_Step22 = new wxTextCtrl( this, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 40,-1 ), wxTE_CENTRE );
	m_Step22->SetMaxLength( 3 ); 
	bSizer37->Add( m_Step22, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP, 5 );
	
	m_SliderStep22 = new wxSlider( this, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxSize( -1,100 ), wxSL_INVERSE|wxSL_VERTICAL );
	bSizer37->Add( m_SliderStep22, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText17 = new wxStaticText( this, wxID_ANY, wxT("22"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText17->Wrap( -1 );
	bSizer37->Add( m_staticText17, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM, 5 );
	
	bSizer2->Add( bSizer37, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer38;
	bSizer38 = new wxBoxSizer( wxVERTICAL );
	
	m_Step23 = new wxTextCtrl( this, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 40,-1 ), wxTE_CENTRE );
	m_Step23->SetMaxLength( 3 ); 
	bSizer38->Add( m_Step23, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP, 5 );
	
	m_SliderStep23 = new wxSlider( this, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxSize( -1,100 ), wxSL_INVERSE|wxSL_VERTICAL );
	bSizer38->Add( m_SliderStep23, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText18 = new wxStaticText( this, wxID_ANY, wxT("23"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText18->Wrap( -1 );
	bSizer38->Add( m_staticText18, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM, 5 );
	
	bSizer2->Add( bSizer38, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer39;
	bSizer39 = new wxBoxSizer( wxVERTICAL );
	
	m_Step24 = new wxTextCtrl( this, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 40,-1 ), wxTE_CENTRE );
	m_Step24->SetMaxLength( 3 ); 
	bSizer39->Add( m_Step24, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP, 5 );
	
	m_SliderStep24 = new wxSlider( this, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxSize( -1,100 ), wxSL_INVERSE|wxSL_VERTICAL );
	bSizer39->Add( m_SliderStep24, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText19 = new wxStaticText( this, wxID_ANY, wxT("24"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText19->Wrap( -1 );
	bSizer39->Add( m_staticText19, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM, 5 );
	
	bSizer2->Add( bSizer39, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer310;
	bSizer310 = new wxBoxSizer( wxVERTICAL );
	
	m_Step25 = new wxTextCtrl( this, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 40,-1 ), wxTE_CENTRE );
	m_Step25->SetMaxLength( 3 ); 
	bSizer310->Add( m_Step25, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP, 5 );
	
	m_SliderStep25 = new wxSlider( this, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxSize( -1,100 ), wxSL_INVERSE|wxSL_VERTICAL );
	bSizer310->Add( m_SliderStep25, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText110 = new wxStaticText( this, wxID_ANY, wxT("25"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText110->Wrap( -1 );
	bSizer310->Add( m_staticText110, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM, 5 );
	
	bSizer2->Add( bSizer310, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer311;
	bSizer311 = new wxBoxSizer( wxVERTICAL );
	
	m_Step26 = new wxTextCtrl( this, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 40,-1 ), wxTE_CENTRE );
	m_Step26->SetMaxLength( 3 ); 
	bSizer311->Add( m_Step26, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP, 5 );
	
	m_SliderStep26 = new wxSlider( this, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxSize( -1,100 ), wxSL_INVERSE|wxSL_VERTICAL );
	bSizer311->Add( m_SliderStep26, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText111 = new wxStaticText( this, wxID_ANY, wxT("26"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText111->Wrap( -1 );
	bSizer311->Add( m_staticText111, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM, 5 );
	
	bSizer2->Add( bSizer311, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer312;
	bSizer312 = new wxBoxSizer( wxVERTICAL );
	
	m_Step27 = new wxTextCtrl( this, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 40,-1 ), wxTE_CENTRE );
	m_Step27->SetMaxLength( 3 ); 
	bSizer312->Add( m_Step27, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP, 5 );
	
	m_SliderStep27 = new wxSlider( this, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxSize( -1,100 ), wxSL_INVERSE|wxSL_VERTICAL );
	bSizer312->Add( m_SliderStep27, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText112 = new wxStaticText( this, wxID_ANY, wxT("27"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText112->Wrap( -1 );
	bSizer312->Add( m_staticText112, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM, 5 );
	
	bSizer2->Add( bSizer312, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer313;
	bSizer313 = new wxBoxSizer( wxVERTICAL );
	
	m_Step28 = new wxTextCtrl( this, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 40,-1 ), wxTE_CENTRE );
	m_Step28->SetMaxLength( 3 ); 
	bSizer313->Add( m_Step28, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP, 5 );
	
	m_SliderStep28 = new wxSlider( this, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxSize( -1,100 ), wxSL_INVERSE|wxSL_VERTICAL );
	bSizer313->Add( m_SliderStep28, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText113 = new wxStaticText( this, wxID_ANY, wxT("28"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText113->Wrap( -1 );
	bSizer313->Add( m_staticText113, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM, 5 );
	
	bSizer2->Add( bSizer313, 1, wxEXPAND, 5 );
	
	bSizer1->Add( bSizer2, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer21;
	bSizer21 = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* bSizer314;
	bSizer314 = new wxBoxSizer( wxVERTICAL );
	
	m_Step1 = new wxTextCtrl( this, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 40,-1 ), wxTE_CENTRE );
	m_Step1->SetMaxLength( 3 ); 
	bSizer314->Add( m_Step1, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP, 5 );
	
	m_SliderStep1 = new wxSlider( this, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxSize( -1,100 ), wxSL_INVERSE|wxSL_VERTICAL );
	bSizer314->Add( m_SliderStep1, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText114 = new wxStaticText( this, wxID_ANY, wxT("1"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText114->Wrap( -1 );
	bSizer314->Add( m_staticText114, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM, 5 );
	
	bSizer21->Add( bSizer314, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer315;
	bSizer315 = new wxBoxSizer( wxVERTICAL );
	
	m_Step2 = new wxTextCtrl( this, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 40,-1 ), wxTE_CENTRE );
	m_Step2->SetMaxLength( 3 ); 
	bSizer315->Add( m_Step2, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP, 5 );
	
	m_SliderStep2 = new wxSlider( this, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxSize( -1,100 ), wxSL_INVERSE|wxSL_VERTICAL );
	bSizer315->Add( m_SliderStep2, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText115 = new wxStaticText( this, wxID_ANY, wxT("2"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText115->Wrap( -1 );
	bSizer315->Add( m_staticText115, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM, 5 );
	
	bSizer21->Add( bSizer315, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer321;
	bSizer321 = new wxBoxSizer( wxVERTICAL );
	
	m_Step3 = new wxTextCtrl( this, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 40,-1 ), wxTE_CENTRE );
	m_Step3->SetMaxLength( 3 ); 
	bSizer321->Add( m_Step3, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP, 5 );
	
	m_SliderStep3 = new wxSlider( this, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxSize( -1,100 ), wxSL_INVERSE|wxSL_VERTICAL );
	bSizer321->Add( m_SliderStep3, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText121 = new wxStaticText( this, wxID_ANY, wxT("3"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText121->Wrap( -1 );
	bSizer321->Add( m_staticText121, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM, 5 );
	
	bSizer21->Add( bSizer321, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer331;
	bSizer331 = new wxBoxSizer( wxVERTICAL );
	
	m_Step4 = new wxTextCtrl( this, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 40,-1 ), wxTE_CENTRE );
	m_Step4->SetMaxLength( 3 ); 
	bSizer331->Add( m_Step4, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP, 5 );
	
	m_SliderStep4 = new wxSlider( this, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxSize( -1,100 ), wxSL_INVERSE|wxSL_VERTICAL );
	bSizer331->Add( m_SliderStep4, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText131 = new wxStaticText( this, wxID_ANY, wxT("4"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText131->Wrap( -1 );
	bSizer331->Add( m_staticText131, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM, 5 );
	
	bSizer21->Add( bSizer331, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer341;
	bSizer341 = new wxBoxSizer( wxVERTICAL );
	
	m_Step5 = new wxTextCtrl( this, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 40,-1 ), wxTE_CENTRE );
	m_Step5->SetMaxLength( 3 ); 
	bSizer341->Add( m_Step5, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP, 5 );
	
	m_SliderStep5 = new wxSlider( this, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxSize( -1,100 ), wxSL_INVERSE|wxSL_VERTICAL );
	bSizer341->Add( m_SliderStep5, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText141 = new wxStaticText( this, wxID_ANY, wxT("5"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText141->Wrap( -1 );
	bSizer341->Add( m_staticText141, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM, 5 );
	
	bSizer21->Add( bSizer341, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer351;
	bSizer351 = new wxBoxSizer( wxVERTICAL );
	
	m_Step6 = new wxTextCtrl( this, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 40,-1 ), wxTE_CENTRE );
	m_Step6->SetMaxLength( 3 ); 
	bSizer351->Add( m_Step6, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP, 5 );
	
	m_SliderStep6 = new wxSlider( this, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxSize( -1,100 ), wxSL_INVERSE|wxSL_VERTICAL );
	bSizer351->Add( m_SliderStep6, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText151 = new wxStaticText( this, wxID_ANY, wxT("6"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText151->Wrap( -1 );
	bSizer351->Add( m_staticText151, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM, 5 );
	
	bSizer21->Add( bSizer351, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer361;
	bSizer361 = new wxBoxSizer( wxVERTICAL );
	
	m_Step7 = new wxTextCtrl( this, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 40,-1 ), wxTE_CENTRE );
	m_Step7->SetMaxLength( 3 ); 
	bSizer361->Add( m_Step7, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP, 5 );
	
	m_SliderStep7 = new wxSlider( this, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxSize( -1,100 ), wxSL_INVERSE|wxSL_VERTICAL );
	bSizer361->Add( m_SliderStep7, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText161 = new wxStaticText( this, wxID_ANY, wxT("7"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText161->Wrap( -1 );
	bSizer361->Add( m_staticText161, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM, 5 );
	
	bSizer21->Add( bSizer361, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer371;
	bSizer371 = new wxBoxSizer( wxVERTICAL );
	
	m_Step8 = new wxTextCtrl( this, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 40,-1 ), wxTE_CENTRE );
	m_Step8->SetMaxLength( 3 ); 
	bSizer371->Add( m_Step8, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP, 5 );
	
	m_SliderStep8 = new wxSlider( this, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxSize( -1,100 ), wxSL_INVERSE|wxSL_VERTICAL );
	bSizer371->Add( m_SliderStep8, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText171 = new wxStaticText( this, wxID_ANY, wxT("8"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText171->Wrap( -1 );
	bSizer371->Add( m_staticText171, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM, 5 );
	
	bSizer21->Add( bSizer371, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer381;
	bSizer381 = new wxBoxSizer( wxVERTICAL );
	
	m_Step9 = new wxTextCtrl( this, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 40,-1 ), wxTE_CENTRE );
	m_Step9->SetMaxLength( 3 ); 
	bSizer381->Add( m_Step9, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP, 5 );
	
	m_SliderStep9 = new wxSlider( this, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxSize( -1,100 ), wxSL_INVERSE|wxSL_VERTICAL );
	bSizer381->Add( m_SliderStep9, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText181 = new wxStaticText( this, wxID_ANY, wxT("9"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText181->Wrap( -1 );
	bSizer381->Add( m_staticText181, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM, 5 );
	
	bSizer21->Add( bSizer381, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer391;
	bSizer391 = new wxBoxSizer( wxVERTICAL );
	
	m_Step10 = new wxTextCtrl( this, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 40,-1 ), wxTE_CENTRE );
	m_Step10->SetMaxLength( 3 ); 
	bSizer391->Add( m_Step10, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP, 5 );
	
	m_SliderStep10 = new wxSlider( this, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxSize( -1,100 ), wxSL_INVERSE|wxSL_VERTICAL );
	bSizer391->Add( m_SliderStep10, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText191 = new wxStaticText( this, wxID_ANY, wxT("10"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText191->Wrap( -1 );
	bSizer391->Add( m_staticText191, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM, 5 );
	
	bSizer21->Add( bSizer391, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer3101;
	bSizer3101 = new wxBoxSizer( wxVERTICAL );
	
	m_Step11 = new wxTextCtrl( this, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 40,-1 ), wxTE_CENTRE );
	m_Step11->SetMaxLength( 3 ); 
	bSizer3101->Add( m_Step11, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP, 5 );
	
	m_SliderStep11 = new wxSlider( this, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxSize( -1,100 ), wxSL_INVERSE|wxSL_VERTICAL );
	bSizer3101->Add( m_SliderStep11, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText1101 = new wxStaticText( this, wxID_ANY, wxT("11"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1101->Wrap( -1 );
	bSizer3101->Add( m_staticText1101, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM, 5 );
	
	bSizer21->Add( bSizer3101, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer3111;
	bSizer3111 = new wxBoxSizer( wxVERTICAL );
	
	m_Step12 = new wxTextCtrl( this, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 40,-1 ), wxTE_CENTRE );
	m_Step12->SetMaxLength( 3 ); 
	bSizer3111->Add( m_Step12, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP, 5 );
	
	m_SliderStep12 = new wxSlider( this, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxSize( -1,100 ), wxSL_INVERSE|wxSL_VERTICAL );
	bSizer3111->Add( m_SliderStep12, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText1111 = new wxStaticText( this, wxID_ANY, wxT("12"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1111->Wrap( -1 );
	bSizer3111->Add( m_staticText1111, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM, 5 );
	
	bSizer21->Add( bSizer3111, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer3121;
	bSizer3121 = new wxBoxSizer( wxVERTICAL );
	
	m_Step13 = new wxTextCtrl( this, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 40,-1 ), wxTE_CENTRE );
	m_Step13->SetMaxLength( 3 ); 
	bSizer3121->Add( m_Step13, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP, 5 );
	
	m_SliderStep13 = new wxSlider( this, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxSize( -1,100 ), wxSL_INVERSE|wxSL_VERTICAL );
	bSizer3121->Add( m_SliderStep13, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText1121 = new wxStaticText( this, wxID_ANY, wxT("13"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1121->Wrap( -1 );
	bSizer3121->Add( m_staticText1121, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM, 5 );
	
	bSizer21->Add( bSizer3121, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer3131;
	bSizer3131 = new wxBoxSizer( wxVERTICAL );
	
	m_Step14 = new wxTextCtrl( this, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize( 40,-1 ), wxTE_CENTRE );
	m_Step14->SetMaxLength( 3 ); 
	bSizer3131->Add( m_Step14, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP, 5 );
	
	m_SliderStep14 = new wxSlider( this, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxSize( -1,100 ), wxSL_INVERSE|wxSL_VERTICAL );
	bSizer3131->Add( m_SliderStep14, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText1131 = new wxStaticText( this, wxID_ANY, wxT("14"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1131->Wrap( -1 );
	bSizer3131->Add( m_staticText1131, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM, 5 );
	
	bSizer21->Add( bSizer3131, 1, wxEXPAND, 5 );
	
	bSizer1->Add( bSizer21, 1, wxEXPAND, 5 );
	
	m_staticline2 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer1->Add( m_staticline2, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer71;
	bSizer71 = new wxBoxSizer( wxHORIZONTAL );
	
	m_Linearize = new wxButton( this, wxID_ANY, wxT("Linearize"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer71->Add( m_Linearize, 0, wxALL, 5 );
	
	m_Logarithmize = new wxButton( this, wxID_ANY, wxT("Logarithmize"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer71->Add( m_Logarithmize, 0, wxALL, 5 );
	
	bSizer1->Add( bSizer71, 0, wxEXPAND, 5 );
	
	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer1->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );
	
	m_stdButton = new wxStdDialogButtonSizer();
	m_stdButtonOK = new wxButton( this, wxID_OK );
	m_stdButton->AddButton( m_stdButtonOK );
	m_stdButtonCancel = new wxButton( this, wxID_CANCEL );
	m_stdButton->AddButton( m_stdButtonCancel );
	m_stdButton->Realize();
	bSizer1->Add( m_stdButton, 0, wxEXPAND|wxALL, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	bSizer1->Fit( this );
	
	// Connect Events
	m_Step15->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( speedcurvedlggen::onStep ), NULL, this );
	m_SliderStep15->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep15->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep15->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep15->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep15->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep15->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep15->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep15->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep15->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_Step16->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( speedcurvedlggen::onStep ), NULL, this );
	m_SliderStep16->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep16->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep16->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep16->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep16->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep16->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep16->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep16->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep16->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_Step17->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( speedcurvedlggen::onStep ), NULL, this );
	m_SliderStep17->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep17->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep17->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep17->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep17->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep17->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep17->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep17->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep17->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_Step18->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( speedcurvedlggen::onStep ), NULL, this );
	m_SliderStep18->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep18->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep18->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep18->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep18->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep18->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep18->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep18->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep18->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_Step19->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( speedcurvedlggen::onStep ), NULL, this );
	m_SliderStep19->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep19->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep19->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep19->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep19->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep19->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep19->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep19->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep19->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_Step20->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( speedcurvedlggen::onStep ), NULL, this );
	m_SliderStep20->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep20->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep20->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep20->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep20->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep20->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep20->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep20->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep20->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_Step21->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( speedcurvedlggen::onStep ), NULL, this );
	m_SliderStep21->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep21->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep21->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep21->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep21->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep21->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep21->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep21->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep21->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_Step22->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( speedcurvedlggen::onStep ), NULL, this );
	m_SliderStep22->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep22->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep22->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep22->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep22->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep22->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep22->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep22->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep22->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_Step23->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( speedcurvedlggen::onStep ), NULL, this );
	m_SliderStep23->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep23->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep23->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep23->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep23->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep23->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep23->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep23->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep23->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_Step24->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( speedcurvedlggen::onStep ), NULL, this );
	m_SliderStep24->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep24->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep24->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep24->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep24->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep24->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep24->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep24->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep24->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_Step25->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( speedcurvedlggen::onStep ), NULL, this );
	m_SliderStep25->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep25->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep25->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep25->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep25->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep25->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep25->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep25->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep25->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_Step26->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( speedcurvedlggen::onStep ), NULL, this );
	m_SliderStep26->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep26->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep26->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep26->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep26->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep26->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep26->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep26->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep26->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_Step27->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( speedcurvedlggen::onStep ), NULL, this );
	m_SliderStep27->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep27->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep27->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep27->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep27->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep27->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep27->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep27->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep27->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_Step28->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( speedcurvedlggen::onStep ), NULL, this );
	m_SliderStep28->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep28->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep28->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep28->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep28->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep28->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep28->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep28->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep28->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_Step1->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( speedcurvedlggen::onStep ), NULL, this );
	m_SliderStep1->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep1->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep1->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep1->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep1->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep1->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep1->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep1->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep1->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_Step2->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( speedcurvedlggen::onStep ), NULL, this );
	m_SliderStep2->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep2->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep2->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep2->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep2->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep2->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep2->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep2->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep2->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_Step3->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( speedcurvedlggen::onStep ), NULL, this );
	m_SliderStep3->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep3->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep3->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep3->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep3->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep3->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep3->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep3->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep3->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_Step4->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( speedcurvedlggen::onStep ), NULL, this );
	m_SliderStep4->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep4->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep4->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep4->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep4->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep4->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep4->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep4->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep4->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_Step5->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( speedcurvedlggen::onStep ), NULL, this );
	m_SliderStep5->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep5->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep5->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep5->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep5->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep5->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep5->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep5->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep5->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_Step6->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( speedcurvedlggen::onStep ), NULL, this );
	m_SliderStep6->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep6->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep6->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep6->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep6->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep6->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep6->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep6->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep6->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_Step7->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( speedcurvedlggen::onStep ), NULL, this );
	m_SliderStep7->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep7->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep7->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep7->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep7->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep7->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep7->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep7->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep7->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_Step8->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( speedcurvedlggen::onStep ), NULL, this );
	m_SliderStep8->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep8->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep8->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep8->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep8->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep8->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep8->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep8->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep8->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_Step9->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( speedcurvedlggen::onStep ), NULL, this );
	m_SliderStep9->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep9->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep9->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep9->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep9->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep9->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep9->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep9->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep9->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_Step10->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( speedcurvedlggen::onStep ), NULL, this );
	m_SliderStep10->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep10->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep10->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep10->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep10->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep10->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep10->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep10->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep10->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_Step11->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( speedcurvedlggen::onStep ), NULL, this );
	m_SliderStep11->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep11->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep11->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep11->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep11->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep11->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep11->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep11->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep11->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_Step12->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( speedcurvedlggen::onStep ), NULL, this );
	m_SliderStep12->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep12->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep12->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep12->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep12->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep12->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep12->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep12->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep12->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_Step13->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( speedcurvedlggen::onStep ), NULL, this );
	m_SliderStep13->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep13->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep13->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep13->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep13->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep13->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep13->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep13->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep13->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_Step14->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( speedcurvedlggen::onStep ), NULL, this );
	m_SliderStep14->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep14->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep14->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep14->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep14->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep14->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep14->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep14->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep14->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_Linearize->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( speedcurvedlggen::onLinearize ), NULL, this );
	m_Logarithmize->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( speedcurvedlggen::onLogarithmize ), NULL, this );
	m_stdButtonCancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( speedcurvedlggen::onCancel ), NULL, this );
	m_stdButtonOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( speedcurvedlggen::onOK ), NULL, this );
}

speedcurvedlggen::~speedcurvedlggen()
{
	// Disconnect Events
	m_Step15->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( speedcurvedlggen::onStep ), NULL, this );
	m_SliderStep15->Disconnect( wxEVT_SCROLL_TOP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep15->Disconnect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep15->Disconnect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep15->Disconnect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep15->Disconnect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep15->Disconnect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep15->Disconnect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep15->Disconnect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep15->Disconnect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_Step16->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( speedcurvedlggen::onStep ), NULL, this );
	m_SliderStep16->Disconnect( wxEVT_SCROLL_TOP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep16->Disconnect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep16->Disconnect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep16->Disconnect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep16->Disconnect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep16->Disconnect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep16->Disconnect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep16->Disconnect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep16->Disconnect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_Step17->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( speedcurvedlggen::onStep ), NULL, this );
	m_SliderStep17->Disconnect( wxEVT_SCROLL_TOP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep17->Disconnect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep17->Disconnect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep17->Disconnect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep17->Disconnect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep17->Disconnect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep17->Disconnect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep17->Disconnect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep17->Disconnect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_Step18->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( speedcurvedlggen::onStep ), NULL, this );
	m_SliderStep18->Disconnect( wxEVT_SCROLL_TOP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep18->Disconnect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep18->Disconnect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep18->Disconnect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep18->Disconnect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep18->Disconnect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep18->Disconnect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep18->Disconnect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep18->Disconnect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_Step19->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( speedcurvedlggen::onStep ), NULL, this );
	m_SliderStep19->Disconnect( wxEVT_SCROLL_TOP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep19->Disconnect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep19->Disconnect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep19->Disconnect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep19->Disconnect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep19->Disconnect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep19->Disconnect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep19->Disconnect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep19->Disconnect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_Step20->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( speedcurvedlggen::onStep ), NULL, this );
	m_SliderStep20->Disconnect( wxEVT_SCROLL_TOP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep20->Disconnect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep20->Disconnect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep20->Disconnect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep20->Disconnect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep20->Disconnect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep20->Disconnect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep20->Disconnect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep20->Disconnect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_Step21->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( speedcurvedlggen::onStep ), NULL, this );
	m_SliderStep21->Disconnect( wxEVT_SCROLL_TOP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep21->Disconnect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep21->Disconnect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep21->Disconnect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep21->Disconnect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep21->Disconnect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep21->Disconnect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep21->Disconnect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep21->Disconnect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_Step22->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( speedcurvedlggen::onStep ), NULL, this );
	m_SliderStep22->Disconnect( wxEVT_SCROLL_TOP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep22->Disconnect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep22->Disconnect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep22->Disconnect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep22->Disconnect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep22->Disconnect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep22->Disconnect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep22->Disconnect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep22->Disconnect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_Step23->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( speedcurvedlggen::onStep ), NULL, this );
	m_SliderStep23->Disconnect( wxEVT_SCROLL_TOP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep23->Disconnect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep23->Disconnect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep23->Disconnect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep23->Disconnect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep23->Disconnect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep23->Disconnect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep23->Disconnect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep23->Disconnect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_Step24->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( speedcurvedlggen::onStep ), NULL, this );
	m_SliderStep24->Disconnect( wxEVT_SCROLL_TOP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep24->Disconnect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep24->Disconnect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep24->Disconnect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep24->Disconnect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep24->Disconnect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep24->Disconnect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep24->Disconnect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep24->Disconnect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_Step25->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( speedcurvedlggen::onStep ), NULL, this );
	m_SliderStep25->Disconnect( wxEVT_SCROLL_TOP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep25->Disconnect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep25->Disconnect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep25->Disconnect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep25->Disconnect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep25->Disconnect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep25->Disconnect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep25->Disconnect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep25->Disconnect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_Step26->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( speedcurvedlggen::onStep ), NULL, this );
	m_SliderStep26->Disconnect( wxEVT_SCROLL_TOP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep26->Disconnect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep26->Disconnect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep26->Disconnect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep26->Disconnect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep26->Disconnect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep26->Disconnect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep26->Disconnect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep26->Disconnect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_Step27->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( speedcurvedlggen::onStep ), NULL, this );
	m_SliderStep27->Disconnect( wxEVT_SCROLL_TOP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep27->Disconnect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep27->Disconnect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep27->Disconnect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep27->Disconnect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep27->Disconnect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep27->Disconnect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep27->Disconnect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep27->Disconnect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_Step28->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( speedcurvedlggen::onStep ), NULL, this );
	m_SliderStep28->Disconnect( wxEVT_SCROLL_TOP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep28->Disconnect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep28->Disconnect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep28->Disconnect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep28->Disconnect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep28->Disconnect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep28->Disconnect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep28->Disconnect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep28->Disconnect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_Step1->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( speedcurvedlggen::onStep ), NULL, this );
	m_SliderStep1->Disconnect( wxEVT_SCROLL_TOP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep1->Disconnect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep1->Disconnect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep1->Disconnect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep1->Disconnect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep1->Disconnect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep1->Disconnect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep1->Disconnect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep1->Disconnect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_Step2->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( speedcurvedlggen::onStep ), NULL, this );
	m_SliderStep2->Disconnect( wxEVT_SCROLL_TOP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep2->Disconnect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep2->Disconnect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep2->Disconnect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep2->Disconnect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep2->Disconnect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep2->Disconnect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep2->Disconnect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep2->Disconnect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_Step3->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( speedcurvedlggen::onStep ), NULL, this );
	m_SliderStep3->Disconnect( wxEVT_SCROLL_TOP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep3->Disconnect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep3->Disconnect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep3->Disconnect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep3->Disconnect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep3->Disconnect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep3->Disconnect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep3->Disconnect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep3->Disconnect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_Step4->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( speedcurvedlggen::onStep ), NULL, this );
	m_SliderStep4->Disconnect( wxEVT_SCROLL_TOP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep4->Disconnect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep4->Disconnect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep4->Disconnect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep4->Disconnect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep4->Disconnect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep4->Disconnect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep4->Disconnect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep4->Disconnect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_Step5->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( speedcurvedlggen::onStep ), NULL, this );
	m_SliderStep5->Disconnect( wxEVT_SCROLL_TOP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep5->Disconnect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep5->Disconnect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep5->Disconnect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep5->Disconnect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep5->Disconnect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep5->Disconnect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep5->Disconnect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep5->Disconnect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_Step6->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( speedcurvedlggen::onStep ), NULL, this );
	m_SliderStep6->Disconnect( wxEVT_SCROLL_TOP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep6->Disconnect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep6->Disconnect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep6->Disconnect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep6->Disconnect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep6->Disconnect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep6->Disconnect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep6->Disconnect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep6->Disconnect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_Step7->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( speedcurvedlggen::onStep ), NULL, this );
	m_SliderStep7->Disconnect( wxEVT_SCROLL_TOP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep7->Disconnect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep7->Disconnect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep7->Disconnect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep7->Disconnect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep7->Disconnect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep7->Disconnect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep7->Disconnect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep7->Disconnect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_Step8->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( speedcurvedlggen::onStep ), NULL, this );
	m_SliderStep8->Disconnect( wxEVT_SCROLL_TOP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep8->Disconnect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep8->Disconnect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep8->Disconnect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep8->Disconnect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep8->Disconnect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep8->Disconnect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep8->Disconnect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep8->Disconnect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_Step9->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( speedcurvedlggen::onStep ), NULL, this );
	m_SliderStep9->Disconnect( wxEVT_SCROLL_TOP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep9->Disconnect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep9->Disconnect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep9->Disconnect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep9->Disconnect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep9->Disconnect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep9->Disconnect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep9->Disconnect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep9->Disconnect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_Step10->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( speedcurvedlggen::onStep ), NULL, this );
	m_SliderStep10->Disconnect( wxEVT_SCROLL_TOP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep10->Disconnect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep10->Disconnect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep10->Disconnect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep10->Disconnect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep10->Disconnect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep10->Disconnect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep10->Disconnect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep10->Disconnect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_Step11->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( speedcurvedlggen::onStep ), NULL, this );
	m_SliderStep11->Disconnect( wxEVT_SCROLL_TOP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep11->Disconnect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep11->Disconnect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep11->Disconnect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep11->Disconnect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep11->Disconnect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep11->Disconnect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep11->Disconnect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep11->Disconnect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_Step12->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( speedcurvedlggen::onStep ), NULL, this );
	m_SliderStep12->Disconnect( wxEVT_SCROLL_TOP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep12->Disconnect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep12->Disconnect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep12->Disconnect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep12->Disconnect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep12->Disconnect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep12->Disconnect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep12->Disconnect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep12->Disconnect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_Step13->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( speedcurvedlggen::onStep ), NULL, this );
	m_SliderStep13->Disconnect( wxEVT_SCROLL_TOP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep13->Disconnect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep13->Disconnect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep13->Disconnect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep13->Disconnect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep13->Disconnect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep13->Disconnect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep13->Disconnect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep13->Disconnect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_Step14->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( speedcurvedlggen::onStep ), NULL, this );
	m_SliderStep14->Disconnect( wxEVT_SCROLL_TOP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep14->Disconnect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep14->Disconnect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep14->Disconnect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep14->Disconnect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep14->Disconnect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep14->Disconnect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep14->Disconnect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_SliderStep14->Disconnect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( speedcurvedlggen::onSlider ), NULL, this );
	m_Linearize->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( speedcurvedlggen::onLinearize ), NULL, this );
	m_Logarithmize->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( speedcurvedlggen::onLogarithmize ), NULL, this );
	m_stdButtonCancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( speedcurvedlggen::onCancel ), NULL, this );
	m_stdButtonOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( speedcurvedlggen::onOK ), NULL, this );
}
