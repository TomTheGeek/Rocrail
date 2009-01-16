/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2007 - Rob Versluis <r.j.versluis@rocrail.net>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
#if defined(__GNUG__) && !defined(__APPLE__)
#pragma implementation "infodialog.h"
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
#include "rocs/public/str.h"
#include "rocs/public/trace.h"
#include "rocgui/wrapper/public/Gui.h"

#include "infodialog.h"

////@begin XPM images
////@end XPM images
#include "rocgui/xpm/rocrail-logo-net.xpm"
#include "rocgui/public/guiapp.h"

/*!
 * InfoDialog type definition
 */

IMPLEMENT_DYNAMIC_CLASS( InfoDialog, wxDialog )

/*!
 * InfoDialog event table definition
 */

BEGIN_EVENT_TABLE( InfoDialog, wxDialog )

////@begin InfoDialog event table entries
    EVT_BUTTON( ID_BITMAPBUTTON_INFO_SPLASH, InfoDialog::OnBitmapbuttonInfoSplashClick )

////@end InfoDialog event table entries

END_EVENT_TABLE()

/*!
 * InfoDialog constructors
 */

InfoDialog::InfoDialog( )
{
}

InfoDialog::InfoDialog( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
  Create(parent, id, caption, pos, size, style);

  m_Splash->SetBitmapLabel( wxIcon(rocrail_logo_net_xpm) );

  char* str = StrOp.fmt("%d.%d.%d",
              wGui.vmajor, wGui.vminor, wGui.patch );
  m_Version->SetLabel( wxString(str,wxConvUTF8) );
  StrOp.free( str );

  str = StrOp.fmt("\'%s\' %s",
              wGui.releasename, wGui.releasesuffix );
  m_Name->SetLabel( wxString(str,wxConvUTF8) );
  StrOp.free( str );

  str = StrOp.fmt("%s svn %d", wGui.buildDate, wxGetApp().getSvn());
  m_Build->SetLabel( wxString(str,wxConvUTF8) );
  StrOp.free( str );

  m_Home->SetLabel( _T("http://www.rocrail.net") );
  m_Support->SetLabel( _T("Copyright 2002-2009 Rob Versluis, License: GPL") );
  //m_ThanksLine->SetLabel( wxGetApp().getMsg( "license" ) );
  m_ThanksLine->SetLabel( _T("") );

  m_Thanks->AppendText( _T("The name Rocrail and the associated logo is our trademark and is officially registered in Germany with number 302008050592.") );
  m_Thanks->AppendText( _T("\n\n") );

  m_Thanks->AppendText( _T("This program is free software; you can redistribute it and/or ") );
  m_Thanks->AppendText( _T("modify it under the terms of the GNU General Public License ") );
  m_Thanks->AppendText( _T("as published by the Free Software Foundation; either version 2 ") );
  m_Thanks->AppendText( _T("of the License, or (at your option) any later version.\n\n") );

  m_Thanks->AppendText( _T("This program is distributed in the hope that it will be useful, ") );
  m_Thanks->AppendText( _T("but WITHOUT ANY WARRANTY; without even the implied warranty of ") );
  m_Thanks->AppendText( _T("MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the ") );
  m_Thanks->AppendText( _T("GNU General Public License for more details.\n\n") );

  m_Thanks->AppendText( _T("You should have received a copy of the GNU General Public License ") );
  m_Thanks->AppendText( _T("along with this program; if not, write to the Free Software ") );
  m_Thanks->AppendText( _T("Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.") );

  m_Thanks->ShowPosition(0);

  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);
  Centre();
}

/*!
 * InfoDialog creator
 */

bool InfoDialog::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin InfoDialog member initialisation
    m_Splash = NULL;
    m_Version = NULL;
    m_Name = NULL;
    m_Build = NULL;
    m_Home = NULL;
    m_Support = NULL;
    m_ThanksLine = NULL;
    m_Thanks = NULL;
////@end InfoDialog member initialisation

////@begin InfoDialog creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end InfoDialog creation
    return true;
}

/*!
 * Control creation for InfoDialog
 */

void InfoDialog::CreateControls()
{
////@begin InfoDialog content construction
    InfoDialog* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    m_Splash = new wxBitmapButton( itemDialog1, ID_BITMAPBUTTON_INFO_SPLASH, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
    itemBoxSizer2->Add(m_Splash, 0, wxGROW|wxALL, 5);

    m_Version = new wxStaticText( itemDialog1, wxID_STATIC_INFO_VERSION, _("version"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE );
    itemBoxSizer2->Add(m_Version, 0, wxALIGN_CENTER_HORIZONTAL|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    m_Name = new wxStaticText( itemDialog1, wxID_STATIC_INFO_NAME, _("name"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE );
    itemBoxSizer2->Add(m_Name, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxADJUST_MINSIZE, 5);

    m_Build = new wxStaticText( itemDialog1, wxID_STATIC_INFO_BUILD, _("build"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE );
    itemBoxSizer2->Add(m_Build, 0, wxALIGN_CENTER_HORIZONTAL|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    m_Home = new wxStaticText( itemDialog1, wxID_STATIC_INFO_HOME, _("home"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE );
    itemBoxSizer2->Add(m_Home, 0, wxALIGN_CENTER_HORIZONTAL|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    m_Support = new wxStaticText( itemDialog1, wxID_STATIC_INFO_SUPPORT, _("support"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE );
    itemBoxSizer2->Add(m_Support, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxADJUST_MINSIZE, 5);

    m_ThanksLine = new wxStaticText( itemDialog1, WX_STATICTEXT_INFO_THANKS, _("Special Thanks to:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer2->Add(m_ThanksLine, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    m_Thanks = new wxTextCtrl( itemDialog1, ID_TEXTCTRL_INFO_THANKS, _T(""), wxDefaultPosition, wxSize(-1, 100), wxTE_MULTILINE|wxTE_READONLY );
    itemBoxSizer2->Add(m_Thanks, 0, wxGROW|wxLEFT|wxRIGHT|wxBOTTOM, 5);

////@end InfoDialog content construction
}

/*!
 * Should we show tooltips?
 */

bool InfoDialog::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap InfoDialog::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin InfoDialog bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end InfoDialog bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon InfoDialog::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin InfoDialog icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end InfoDialog icon retrieval
}
/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BITMAPBUTTON_INFO_SPLASH
 */

void InfoDialog::OnBitmapbuttonInfoSplashClick( wxCommandEvent& event )
{
  EndModal( wxID_OK );
}


