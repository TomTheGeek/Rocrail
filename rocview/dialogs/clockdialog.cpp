/** ------------------------------------------------------------
  * Module:
  * Object:
  * ------------------------------------------------------------
  * $Source: /cvsroot/rojav/rocgui/dialogs/rocgui-dialogs.pjd,v $
  * $Author: robvrs $
  * $Date: 2006/02/22 14:10:57 $
  * $Revision: 1.63 $
  * $Name:  $
  */
#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "clockdialog.h"
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

#include "clockdialog.h"

////@begin XPM images
////@end XPM images

#include "rocview/public/guiapp.h"

/*!
 * ClockDialog type definition
 */

IMPLEMENT_DYNAMIC_CLASS( ClockDialog, wxDialog )


/*!
 * ClockDialog event table definition
 */

BEGIN_EVENT_TABLE( ClockDialog, wxDialog )

////@begin ClockDialog event table entries
    EVT_BUTTON( wxID_OK, ClockDialog::OnOkClick )

    EVT_BUTTON( wxID_CANCEL, ClockDialog::OnCancelClick )

////@end ClockDialog event table entries

END_EVENT_TABLE()


/*!
 * ClockDialog constructors
 */

ClockDialog::ClockDialog()
{
    Init();
}

ClockDialog::ClockDialog( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    Create(parent, -1, wxGetApp().getMsg("clock") );
}

void ClockDialog::setClock( int divider, int hours, int minutes ) {
  m_Divider->SetValue(divider);
  m_Hour->SetValue(hours);
  m_Minute->SetValue(minutes);
}

void ClockDialog::getClock( int* divider, int* hours, int* minutes ) {
  *divider = m_Divider->GetValue();
  *hours = m_Hour->GetValue();
  *minutes = m_Minute->GetValue();
  
}


/*!
 * ClockDialog creator
 */

bool ClockDialog::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin ClockDialog creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end ClockDialog creation
    return true;
}


/*!
 * ClockDialog destructor
 */

ClockDialog::~ClockDialog()
{
////@begin ClockDialog destruction
////@end ClockDialog destruction
}


/*!
 * Member initialisation
 */

void ClockDialog::Init()
{
////@begin ClockDialog member initialisation
    m_ClockBox = NULL;
    m_labDevider = NULL;
    m_Divider = NULL;
    m_labHour = NULL;
    m_Hour = NULL;
    m_labMinute = NULL;
    m_Minute = NULL;
    m_OK = NULL;
    m_Cancel = NULL;
////@end ClockDialog member initialisation
}


/*!
 * Control creation for ClockDialog
 */

void ClockDialog::CreateControls()
{    
////@begin ClockDialog content construction
    ClockDialog* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    wxStaticBox* itemStaticBoxSizer3Static = new wxStaticBox(itemDialog1, wxID_ANY, _("Clock Service"));
    m_ClockBox = new wxStaticBoxSizer(itemStaticBoxSizer3Static, wxHORIZONTAL);
    itemBoxSizer2->Add(m_ClockBox, 0, wxGROW|wxALL, 5);

    m_labDevider = new wxStaticText( itemDialog1, wxID_ANY, _("divider"), wxDefaultPosition, wxDefaultSize, 0 );
    m_ClockBox->Add(m_labDevider, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    m_Divider = new wxSpinCtrl( itemDialog1, wxID_ANY, _T("1"), wxDefaultPosition, wxSize(70, -1), wxSP_ARROW_KEYS, 1, 10, 1 );
    m_ClockBox->Add(m_Divider, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT|wxTOP|wxBOTTOM, 5);

    m_labHour = new wxStaticText( itemDialog1, wxID_ANY, _("hour"), wxDefaultPosition, wxDefaultSize, 0 );
    m_ClockBox->Add(m_labHour, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    m_Hour = new wxSpinCtrl( itemDialog1, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(70, -1), wxSP_ARROW_KEYS, 0, 23, 0 );
    m_ClockBox->Add(m_Hour, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT|wxTOP|wxBOTTOM, 5);

    m_labMinute = new wxStaticText( itemDialog1, wxID_ANY, _("minute"), wxDefaultPosition, wxDefaultSize, 0 );
    m_ClockBox->Add(m_labMinute, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    m_Minute = new wxSpinCtrl( itemDialog1, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(70, -1), wxSP_ARROW_KEYS, 0, 59, 0 );
    m_ClockBox->Add(m_Minute, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT|wxTOP|wxBOTTOM, 5);

    wxStdDialogButtonSizer* itemStdDialogButtonSizer10 = new wxStdDialogButtonSizer;

    itemBoxSizer2->Add(itemStdDialogButtonSizer10, 0, wxALIGN_RIGHT|wxALL, 5);
    m_OK = new wxButton( itemDialog1, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer10->AddButton(m_OK);

    m_Cancel = new wxButton( itemDialog1, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer10->AddButton(m_Cancel);

    itemStdDialogButtonSizer10->Realize();

////@end ClockDialog content construction
}


/*!
 * Should we show tooltips?
 */

bool ClockDialog::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap ClockDialog::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin ClockDialog bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end ClockDialog bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon ClockDialog::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin ClockDialog icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end ClockDialog icon retrieval
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
 */

void ClockDialog::OnCancelClick( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL in ClockDialog.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL in ClockDialog. 
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
 */

void ClockDialog::OnOkClick( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK in ClockDialog.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK in ClockDialog. 
}

