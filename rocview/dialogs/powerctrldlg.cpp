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

#include "powerctrldlg.h"

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/defs.h"
#endif

#include "rocview/public/guiapp.h"
#include "rocrail/wrapper/public/ModelCmd.h"
#include "rocrail/wrapper/public/Plan.h"
#include "rocrail/wrapper/public/PwrCmd.h"
#include "rocrail/wrapper/public/Booster.h"
#include "rocrail/wrapper/public/BoosterList.h"

#include "rocs/public/trace.h"



PowerCtrlDlg::PowerCtrlDlg( wxWindow* parent ):powerctrlgen( parent )
{
  m_SelBooster = NULL;
  m_BoosterMap = MapOp.inst();
  initLabels();
  initValues();

  GetSizer()->Layout();
  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);

/*
  wxFont* font = new wxFont( m_TracksGrid->GetDefaultCellFont() );
  font->SetPointSize( (int)(font->GetPointSize() - 1 ) );
  m_Boosters->SetDefaultCellFont( *font );
*/
  m_Boosters->AutoSizeColumns();
  m_Boosters->AutoSizeRows();

  m_Boosters->AutoSize();
  m_Boosters->FitInside();
  m_Boosters->UpdateDimensions();
  GetSizer()->Layout();

}


void PowerCtrlDlg::initLabels() {
  m_Boosters->AutoSizeColumns();
  m_Boosters->AutoSizeRows();

  m_Boosters->SetColLabelValue(0, wxGetApp().getMsg("boosterid") );
  m_Boosters->SetColLabelValue(1, wxGetApp().getMsg("shortcut") );
  m_Boosters->SetColLabelValue(2, wxGetApp().getMsg("trackpower") );
  m_Boosters->SetColLabelValue(3, wxGetApp().getMsg("powerdistrict") );
}


void PowerCtrlDlg::initValues() {
  m_Boosters->DeleteRows(0,m_Boosters->GetNumberRows());
  MapOp.clear(m_BoosterMap);

  iONode model = wxGetApp().getModel();
  if( model != NULL ) {
    iONode boosterlist = wPlan.getboosterlist( model );
    if( boosterlist != NULL ) {
      iONode booster = wBoosterList.getbooster( boosterlist );
      while( booster != NULL ) {
        MapOp.put(m_BoosterMap, wBooster.getid( booster ), (obj)booster );
        m_Boosters->AppendRows();
        m_Boosters->SetCellValue(m_Boosters->GetNumberRows()-1, 0, wxString(wBooster.getid( booster ),wxConvUTF8) );
        m_Boosters->SetCellValue(m_Boosters->GetNumberRows()-1, 1, wBooster.isshortcut(booster)?wxGetApp().getMsg("yes"):wxGetApp().getMsg("no") );
        m_Boosters->SetCellValue(m_Boosters->GetNumberRows()-1, 2, wBooster.ispower(booster)?wxGetApp().getMsg("on"):wxGetApp().getMsg("off") );
        m_Boosters->SetCellValue(m_Boosters->GetNumberRows()-1, 3, wxString(wBooster.getdistrict( booster ),wxConvUTF8) );
        m_Boosters->SetReadOnly( m_Boosters->GetNumberRows()-1, 0, true );
        m_Boosters->SetReadOnly( m_Boosters->GetNumberRows()-1, 1, true );
        m_Boosters->SetReadOnly( m_Boosters->GetNumberRows()-1, 2, true );
        m_Boosters->SetReadOnly( m_Boosters->GetNumberRows()-1, 3, true );

        booster = wBoosterList.nextbooster( boosterlist, booster );
      };
    }
  }

  m_On->Enable( false );
  m_Off->Enable( false );
}


void PowerCtrlDlg::onCellLeftClick( wxGridEvent& event ) {
  m_Boosters->SelectRow(event.GetRow());
  m_On->Enable( true );
  m_Off->Enable( true );

  wxString str = m_Boosters->GetCellValue( event.GetRow(), 0 );
  m_SelBooster = (iONode)MapOp.get(m_BoosterMap, (const char*)str.mb_str(wxConvUTF8) );
  if( m_SelBooster != NULL )
    TraceOp.trc( "pwrctrl", TRCLEVEL_INFO, __LINE__, 9999, "booster %s selected", wBooster.getid(m_SelBooster) );
}


void PowerCtrlDlg::onCellRightClick( wxGridEvent& event ) {
  onCellLeftClick(event);
}


void PowerCtrlDlg::powerEvent(iONode event) {
  initValues();
}


void PowerCtrlDlg::OnOn( wxCommandEvent& event )
{
	if( m_SelBooster != NULL ) {
	  iONode pwrcmd = NodeOp.inst( wPwrCmd.name(), NULL, ELEMENT_NODE );
	  wPwrCmd.setid(pwrcmd, wBooster.getid(m_SelBooster));
    wPwrCmd.setcmd(pwrcmd, wPwrCmd.on);
    TraceOp.trc( "pwrctrl", TRCLEVEL_INFO, __LINE__, 9999, "sending on cmd for booster %s", wBooster.getid(m_SelBooster) );
    wxGetApp().sendToRocrail( pwrcmd );
	}
}

void PowerCtrlDlg::OnOff( wxCommandEvent& event )
{
  if( m_SelBooster != NULL ) {
    iONode pwrcmd = NodeOp.inst( wPwrCmd.name(), NULL, ELEMENT_NODE );
    wPwrCmd.setid(pwrcmd, wBooster.getid(m_SelBooster));
    wPwrCmd.setcmd(pwrcmd, wPwrCmd.off);
    TraceOp.trc( "pwrctrl", TRCLEVEL_INFO, __LINE__, 9999, "sending off cmd for booster %s", wBooster.getid(m_SelBooster) );
    wxGetApp().sendToRocrail( pwrcmd );
  }
}

void PowerCtrlDlg::OnOK( wxCommandEvent& event )
{
  EndModal( wxID_OK );
}
