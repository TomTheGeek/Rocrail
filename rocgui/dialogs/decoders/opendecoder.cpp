/////////////////////////////////////////////////////////////////////////////
// Name:        opendecoder.cpp
// Purpose:     
// Author:      
// Modified by: 
// Created:     Sat 31 May 2008 07:25:32 CEST
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "opendecoder.h"
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

#include "opendecoder.h"

////@begin XPM images
////@end XPM images


/*!
 * OpenDecoderDlg type definition
 */

IMPLEMENT_DYNAMIC_CLASS( OpenDecoderDlg, wxDialog )


/*!
 * OpenDecoderDlg event table definition
 */

BEGIN_EVENT_TABLE( OpenDecoderDlg, wxDialog )

////@begin OpenDecoderDlg event table entries
    EVT_BUTTON( ID_BUTTON_GEN_GET, OpenDecoderDlg::OnButtonGenGetClick )

    EVT_BUTTON( ID_BUTTON_GEN_SET, OpenDecoderDlg::OnButtonGenSetClick )

    EVT_BUTTON( wxID_CANCEL, OpenDecoderDlg::OnCancelClick )

////@end OpenDecoderDlg event table entries

END_EVENT_TABLE()


/*!
 * OpenDecoderDlg constructors
 */

OpenDecoderDlg::OpenDecoderDlg()
{
    Init();
}

OpenDecoderDlg::OpenDecoderDlg( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    Create(parent, id, caption, pos, size, style);
}


/*!
 * OpenDecoderDlg creator
 */

bool OpenDecoderDlg::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin OpenDecoderDlg creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end OpenDecoderDlg creation
    return true;
}


/*!
 * OpenDecoderDlg destructor
 */

OpenDecoderDlg::~OpenDecoderDlg()
{
////@begin OpenDecoderDlg destruction
////@end OpenDecoderDlg destruction
}


/*!
 * Member initialisation
 */

void OpenDecoderDlg::Init()
{
////@begin OpenDecoderDlg member initialisation
    m_GenerelPanel = NULL;
    m_labAddress = NULL;
    m_Address = NULL;
    m_labVersion = NULL;
    m_Version = NULL;
    m_labVendor = NULL;
    m_Vendor = NULL;
    m_GenGet = NULL;
    m_GenSet = NULL;
    m_ConfigPanel = NULL;
    m_TimePanel = NULL;
    m_Cancel = NULL;
////@end OpenDecoderDlg member initialisation
}


/*!
 * Control creation for OpenDecoderDlg
 */

void OpenDecoderDlg::CreateControls()
{    
////@begin OpenDecoderDlg content construction
    OpenDecoderDlg* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    wxNotebook* itemNotebook3 = new wxNotebook( itemDialog1, ID_NOTEBOOK_DCC, wxDefaultPosition, wxDefaultSize, wxNB_DEFAULT );

    m_GenerelPanel = new wxPanel( itemNotebook3, ID_PANEL_DCC_GEN, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxVERTICAL);
    m_GenerelPanel->SetSizer(itemBoxSizer5);

    wxFlexGridSizer* itemFlexGridSizer6 = new wxFlexGridSizer(2, 2, 0, 0);
    itemBoxSizer5->Add(itemFlexGridSizer6, 1, wxGROW|wxALL, 5);
    m_labAddress = new wxStaticText( m_GenerelPanel, wxID_ANY, _("Address"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(m_labAddress, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Address = new wxSpinCtrl( m_GenerelPanel, wxID_ANY, _T("1"), wxDefaultPosition, wxSize(80, -1), wxSP_ARROW_KEYS, 1, 511, 1 );
    itemFlexGridSizer6->Add(m_Address, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labVersion = new wxStaticText( m_GenerelPanel, wxID_ANY, _("Version"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(m_labVersion, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Version = new wxTextCtrl( m_GenerelPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
    m_Version->Enable(false);
    itemFlexGridSizer6->Add(m_Version, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labVendor = new wxStaticText( m_GenerelPanel, wxID_ANY, _("Vendor"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(m_labVendor, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Vendor = new wxTextCtrl( m_GenerelPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
    m_Vendor->Enable(false);
    itemFlexGridSizer6->Add(m_Vendor, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer13 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer5->Add(itemBoxSizer13, 0, wxGROW|wxALL, 5);
    m_GenGet = new wxButton( m_GenerelPanel, ID_BUTTON_GEN_GET, _("Get"), wxDefaultPosition, wxSize(60, 30), 0 );
    itemBoxSizer13->Add(m_GenGet, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_GenSet = new wxButton( m_GenerelPanel, ID_BUTTON_GEN_SET, _("Set"), wxDefaultPosition, wxSize(60, 30), 0 );
    itemBoxSizer13->Add(m_GenSet, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    itemNotebook3->AddPage(m_GenerelPanel, _("Generel"));

    m_ConfigPanel = new wxPanel( itemNotebook3, ID_PANEL_DCC_CONFIG, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );

    itemNotebook3->AddPage(m_ConfigPanel, _("Config"));

    m_TimePanel = new wxPanel( itemNotebook3, ID_PANEL_DCC_TIME, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );

    itemNotebook3->AddPage(m_TimePanel, _("Time"));

    itemBoxSizer2->Add(itemNotebook3, 1, wxGROW|wxALL, 5);

    wxStdDialogButtonSizer* itemStdDialogButtonSizer18 = new wxStdDialogButtonSizer;

    itemBoxSizer2->Add(itemStdDialogButtonSizer18, 0, wxALIGN_RIGHT|wxALL, 5);
    m_Cancel = new wxButton( itemDialog1, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer18->AddButton(m_Cancel);

    itemStdDialogButtonSizer18->Realize();

////@end OpenDecoderDlg content construction
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_GEN_GET
 */

void OpenDecoderDlg::OnButtonGenGetClick( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_GEN_GET in OpenDecoderDlg.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_GEN_GET in OpenDecoderDlg. 
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_GEN_SET
 */

void OpenDecoderDlg::OnButtonGenSetClick( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_GEN_SET in OpenDecoderDlg.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_GEN_SET in OpenDecoderDlg. 
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
 */

void OpenDecoderDlg::OnCancelClick( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL in OpenDecoderDlg.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL in OpenDecoderDlg. 
}


/*!
 * Should we show tooltips?
 */

bool OpenDecoderDlg::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap OpenDecoderDlg::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin OpenDecoderDlg bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end OpenDecoderDlg bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon OpenDecoderDlg::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin OpenDecoderDlg icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end OpenDecoderDlg icon retrieval
}
