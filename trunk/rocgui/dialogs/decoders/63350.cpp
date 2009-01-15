/////////////////////////////////////////////////////////////////////////////
// Name:        63350.cpp
// Purpose:     
// Author:      
// Modified by: 
// Created:     Sat 28 Apr 2007 06:55:50 CEST
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "63350.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

////@begin includes
////@end includes

#include "63350.h"

////@begin XPM images
////@end XPM images

/*!
 * Uhl_63350_Dlg type definition
 */

IMPLEMENT_DYNAMIC_CLASS( Uhl_63350_Dlg, wxDialog )

/*!
 * Uhl_63350_Dlg event table definition
 */

BEGIN_EVENT_TABLE( Uhl_63350_Dlg, wxDialog )

////@begin Uhl_63350_Dlg event table entries
    EVT_RADIOBOX( ID_RADIOBOX_63350_CONFIGURE, Uhl_63350_Dlg::OnRadiobox63350ConfigureSelected )

////@end Uhl_63350_Dlg event table entries

END_EVENT_TABLE()

/*!
 * Uhl_63350_Dlg constructors
 */

Uhl_63350_Dlg::Uhl_63350_Dlg()
{
    Init();
}

Uhl_63350_Dlg::Uhl_63350_Dlg( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    Create(parent, id, caption, pos, size, style);
}

/*!
 * 63350 creator
 */

bool Uhl_63350_Dlg::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin Uhl_63350_Dlg creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end Uhl_63350_Dlg creation
    return true;
}

/*!
 * 63350 destructor
 */

Uhl_63350_Dlg::~Uhl_63350_Dlg()
{
////@begin Uhl_63350_Dlg destruction
////@end Uhl_63350_Dlg destruction
}

/*!
 * Member initialisation 
 */

void Uhl_63350_Dlg::Init()
{
////@begin Uhl_63350_Dlg member initialisation
    m_labModuleAddress = NULL;
    m_ModuleAddress = NULL;
    m_labReportAddress = NULL;
    m_ReportAddress = NULL;
    m_BoxConfig = NULL;
    m_AutoAddress = NULL;
    m_IndividualDelays = NULL;
    m_ReportAtPoweron = NULL;
    m_Configure = NULL;
    m_Input1 = NULL;
    m_Input2 = NULL;
    m_Input3 = NULL;
    m_Input4 = NULL;
    m_Input5 = NULL;
    m_Input6 = NULL;
    m_Input7 = NULL;
    m_Input8 = NULL;
    m_Input9 = NULL;
    m_Input10 = NULL;
    m_Input11 = NULL;
    m_Input12 = NULL;
    m_Input13 = NULL;
    m_Input14 = NULL;
    m_Input15 = NULL;
    m_Input16 = NULL;
////@end Uhl_63350_Dlg member initialisation
}

/*!
 * Control creation for 63350
 */

void Uhl_63350_Dlg::CreateControls()
{    
////@begin Uhl_63350_Dlg content construction
    Uhl_63350_Dlg* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    wxPanel* itemPanel3 = new wxPanel( itemDialog1, ID_PANEL1, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    itemBoxSizer2->Add(itemPanel3, 0, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxVERTICAL);
    itemPanel3->SetSizer(itemBoxSizer4);

    wxFlexGridSizer* itemFlexGridSizer5 = new wxFlexGridSizer(2, 2, 0, 0);
    itemBoxSizer4->Add(itemFlexGridSizer5, 0, wxGROW|wxALL, 5);

    m_labModuleAddress = new wxStaticText( itemPanel3, wxID_ANY, _("Moduleaddress"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer5->Add(m_labModuleAddress, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    m_ModuleAddress = new wxTextCtrl( itemPanel3, wxID_ANY, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    m_ModuleAddress->SetMaxLength(5);
    itemFlexGridSizer5->Add(m_ModuleAddress, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labReportAddress = new wxStaticText( itemPanel3, wxID_ANY, _("Reportaddress"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer5->Add(m_labReportAddress, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    m_ReportAddress = new wxTextCtrl( itemPanel3, wxID_ANY, _("1017"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    m_ReportAddress->SetMaxLength(4);
    itemFlexGridSizer5->Add(m_ReportAddress, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_BoxConfig = new wxStaticBox(itemPanel3, wxID_ANY, _("Configuration"));
    wxStaticBoxSizer* itemStaticBoxSizer10 = new wxStaticBoxSizer(m_BoxConfig, wxVERTICAL);
    itemBoxSizer4->Add(itemStaticBoxSizer10, 0, wxGROW|wxALL, 5);

    m_AutoAddress = new wxCheckBox( itemPanel3, wxID_ANY, _("Automatic Addressing"), wxDefaultPosition, wxDefaultSize, 0 );
    m_AutoAddress->SetValue(false);
    itemStaticBoxSizer10->Add(m_AutoAddress, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP, 5);

    m_IndividualDelays = new wxCheckBox( itemPanel3, wxID_ANY, _("Individual delays"), wxDefaultPosition, wxDefaultSize, 0 );
    m_IndividualDelays->SetValue(false);
    itemStaticBoxSizer10->Add(m_IndividualDelays, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT, 5);

    m_ReportAtPoweron = new wxCheckBox( itemPanel3, wxID_ANY, _("Report at poweron"), wxDefaultPosition, wxDefaultSize, 0 );
    m_ReportAtPoweron->SetValue(false);
    itemStaticBoxSizer10->Add(m_ReportAtPoweron, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    wxStaticBox* itemStaticBoxSizer14Static = new wxStaticBox(itemPanel3, wxID_ANY, _("Inputs"));
    wxStaticBoxSizer* itemStaticBoxSizer14 = new wxStaticBoxSizer(itemStaticBoxSizer14Static, wxVERTICAL);
    itemBoxSizer4->Add(itemStaticBoxSizer14, 0, wxGROW|wxALL, 5);

    wxString m_ConfigureStrings[] = {
        _("&Address"),
        _("&On delay"),
        _("&Off delay")
    };
    m_Configure = new wxRadioBox( itemPanel3, ID_RADIOBOX_63350_CONFIGURE, _("Configure"), wxDefaultPosition, wxDefaultSize, 3, m_ConfigureStrings, 1, wxRA_SPECIFY_ROWS );
    m_Configure->SetSelection(0);
    itemStaticBoxSizer14->Add(m_Configure, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer16 = new wxFlexGridSizer(4, 4, 0, 0);
    itemStaticBoxSizer14->Add(itemFlexGridSizer16, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    m_Input1 = new wxTextCtrl( itemPanel3, wxID_ANY, _("0"), wxDefaultPosition, wxSize(50, -1), wxTE_CENTRE );
    itemFlexGridSizer16->Add(m_Input1, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP, 5);

    m_Input2 = new wxTextCtrl( itemPanel3, wxID_ANY, _("0"), wxDefaultPosition, wxSize(50, -1), wxTE_CENTRE );
    itemFlexGridSizer16->Add(m_Input2, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxTOP, 5);

    m_Input3 = new wxTextCtrl( itemPanel3, wxID_ANY, _("0"), wxDefaultPosition, wxSize(50, -1), wxTE_CENTRE );
    itemFlexGridSizer16->Add(m_Input3, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxTOP, 5);

    m_Input4 = new wxTextCtrl( itemPanel3, wxID_ANY, _("0"), wxDefaultPosition, wxSize(50, -1), wxTE_CENTRE );
    itemFlexGridSizer16->Add(m_Input4, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxRIGHT|wxTOP, 5);

    m_Input5 = new wxTextCtrl( itemPanel3, wxID_ANY, _("0"), wxDefaultPosition, wxSize(50, -1), wxTE_CENTRE );
    itemFlexGridSizer16->Add(m_Input5, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT, 5);

    m_Input6 = new wxTextCtrl( itemPanel3, wxID_ANY, _("0"), wxDefaultPosition, wxSize(50, -1), wxTE_CENTRE );
    itemFlexGridSizer16->Add(m_Input6, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    m_Input7 = new wxTextCtrl( itemPanel3, wxID_ANY, _("0"), wxDefaultPosition, wxSize(50, -1), wxTE_CENTRE );
    itemFlexGridSizer16->Add(m_Input7, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    m_Input8 = new wxTextCtrl( itemPanel3, wxID_ANY, _("0"), wxDefaultPosition, wxSize(50, -1), wxTE_CENTRE );
    itemFlexGridSizer16->Add(m_Input8, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxRIGHT, 5);

    m_Input9 = new wxTextCtrl( itemPanel3, wxID_ANY, _("0"), wxDefaultPosition, wxSize(50, -1), wxTE_CENTRE );
    itemFlexGridSizer16->Add(m_Input9, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT, 5);

    m_Input10 = new wxTextCtrl( itemPanel3, wxID_ANY, _("0"), wxDefaultPosition, wxSize(50, -1), wxTE_CENTRE );
    itemFlexGridSizer16->Add(m_Input10, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    m_Input11 = new wxTextCtrl( itemPanel3, wxID_ANY, _("0"), wxDefaultPosition, wxSize(50, -1), wxTE_CENTRE );
    itemFlexGridSizer16->Add(m_Input11, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    m_Input12 = new wxTextCtrl( itemPanel3, wxID_ANY, _("0"), wxDefaultPosition, wxSize(50, -1), wxTE_CENTRE );
    itemFlexGridSizer16->Add(m_Input12, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxRIGHT, 5);

    m_Input13 = new wxTextCtrl( itemPanel3, wxID_ANY, _("0"), wxDefaultPosition, wxSize(50, -1), wxTE_CENTRE );
    itemFlexGridSizer16->Add(m_Input13, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxBOTTOM, 5);

    m_Input14 = new wxTextCtrl( itemPanel3, wxID_ANY, _("0"), wxDefaultPosition, wxSize(50, -1), wxTE_CENTRE );
    itemFlexGridSizer16->Add(m_Input14, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxBOTTOM, 5);

    m_Input15 = new wxTextCtrl( itemPanel3, wxID_ANY, _("0"), wxDefaultPosition, wxSize(50, -1), wxTE_CENTRE );
    itemFlexGridSizer16->Add(m_Input15, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxBOTTOM, 5);

    m_Input16 = new wxTextCtrl( itemPanel3, wxID_ANY, _("0"), wxDefaultPosition, wxSize(50, -1), wxTE_CENTRE );
    itemFlexGridSizer16->Add(m_Input16, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxRIGHT|wxBOTTOM, 5);

    wxStdDialogButtonSizer* itemStdDialogButtonSizer33 = new wxStdDialogButtonSizer;

    itemBoxSizer2->Add(itemStdDialogButtonSizer33, 0, wxALIGN_RIGHT|wxALL, 5);
    wxButton* itemButton34 = new wxButton( itemDialog1, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer33->AddButton(itemButton34);

    wxButton* itemButton35 = new wxButton( itemDialog1, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer33->AddButton(itemButton35);

    itemStdDialogButtonSizer33->Realize();

////@end Uhl_63350_Dlg content construction
}

/*!
 * Should we show tooltips?
 */

bool Uhl_63350_Dlg::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap Uhl_63350_Dlg::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin Uhl_63350_Dlg bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end Uhl_63350_Dlg bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon Uhl_63350_Dlg::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin Uhl_63350_Dlg icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end Uhl_63350_Dlg icon retrieval
}

/*!
 * wxEVT_COMMAND_RADIOBOX_SELECTED event handler for ID_RADIOBOX1
 */

void Uhl_63350_Dlg::OnRadiobox63350ConfigureSelected( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_RADIOBOX_SELECTED event handler for ID_RADIOBOX1 in Uhl_63350_Dlg.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_COMMAND_RADIOBOX_SELECTED event handler for ID_RADIOBOX1 in Uhl_63350_Dlg. 
}

