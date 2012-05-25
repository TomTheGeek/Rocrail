/*
 Rocrail - Model Railroad Software

 Copyright (C) Rob Versluis <r.j.versluis@rocrail.net>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 3
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "donkey.h"

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/defs.h"
#endif

#include "rocview/public/guiapp.h"
#include "rocview/wrapper/public/Gui.h"
#include "rocview/res/icons.hpp"

#include "rocs/public/trace.h"
#include "rocs/public/file.h"
#include "rocs/public/system.h"
#include "rocs/public/str.h"
#include "rocs/public/strtok.h"


DonKey::DonKey( wxWindow* parent, iONode ini )
:
DonKeyGen( parent )
{
  m_Ini = ini;
  initLabels();
  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);
  initValues();
}

void DonKey::OnSplash( wxCommandEvent& event ) {
  OnLoadKey(event);
}
void DonKey::OnLoadKey( wxCommandEvent& event )
{
  wxFileDialog* fdlg = new wxFileDialog(this, _T("Donation Key"),
      _T("."), _T(""), _T("DonKey files (*.dat)|*.dat"), wxFD_OPEN);
      
  if( fdlg->ShowModal() == wxID_OK ) {
    TraceOp.trc( "donkey", TRCLEVEL_INFO, __LINE__, 9999, "Loading %s...", (const char*)fdlg->GetPath().mb_str(wxConvUTF8) );
    if( FileOp.exist(fdlg->GetPath().mb_str(wxConvUTF8)) ) {
      iOFile f = FileOp.inst( fdlg->GetPath().mb_str(wxConvUTF8), OPEN_READONLY );
      char* buffer = (char*)allocMem( FileOp.size( f ) +1 );
      FileOp.read( f, buffer, FileOp.size( f ) );
      FileOp.base.del( f );
      iOStrTok tok = StrTokOp.inst( buffer, ';' );
      if( StrTokOp.hasMoreTokens(tok))
        wGui.setdoneml( m_Ini, StrTokOp.nextToken(tok) );
      if( StrTokOp.hasMoreTokens(tok))
        wGui.setdonkey( m_Ini, StrTokOp.nextToken(tok) );
      StrTokOp.base.del( tok );

    if( SystemOp.isExpired(SystemOp.decode(StrOp.strToByte(wGui.getdonkey(m_Ini)), StrOp.len(wGui.getdonkey(m_Ini))/2, wGui.getdoneml(m_Ini)), NULL, NULL) )
      wxMessageDialog( this, wxGetApp().getMsg("expireddonkey"), _T("Rocrail"), wxOK ).ShowModal();
    }
    initValues();
  }
}

void DonKey::OnDonateNow( wxCommandEvent& event )
{
  wxLaunchDefaultBrowser(wxGetApp().getMsg("donatelink"), wxBROWSER_NEW_WINDOW );
}

void DonKey::OnOK( wxCommandEvent& event )
{
  EndModal( 0 );
}

void DonKey::initLabels() {
  m_Splash->SetBitmapLabel( *_img_rocrail_logo );

  m_labEmail->SetLabel( wxGetApp().getMsg( "email" ) );
  m_labExpDate->SetLabel( wxGetApp().getMsg( "expdate" ) );
  m_LoadKey->SetLabel( wxGetApp().getMenu( "loadkey" ) );
  m_Donate->SetLabel( wxGetApp().getMsg( "donatenow" ) );
  m_stdButtonOK->SetLabel( wxGetApp().getMsg( "ok" ) );
}

void DonKey::initValues() {
  char* expdate = NULL;
  long expdays = 0;
  if( SystemOp.isExpired(SystemOp.decode(StrOp.strToByte(wGui.getdonkey(m_Ini)), StrOp.len(wGui.getdonkey(m_Ini))/2, wGui.getdoneml(m_Ini)), &expdate, &expdays) ) {
    m_DonateText->SetValue( wxGetApp().getMsg( "donatekey" ) );
  }
  else {
    //m_LoadKey->Enable(false);
    //m_Donate->Enable(false);
    m_DonateText->SetValue( wxGetApp().getMsg( "donatethanks" ) );
    m_stdButtonOK->SetDefault();
    if( expdays < 8 && expdays >= 0 ) {
      m_ExpDate->SetBackgroundColour(*wxRED);
    }
  }
  m_Email->SetValue( wxString( wGui.getdoneml( m_Ini ),wxConvUTF8) );
  if( expdate != NULL ) {
    m_ExpDate->SetValue(wxString( expdate,wxConvUTF8));
    StrOp.free(expdate);
  }
}

