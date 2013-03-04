/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2012 Rob Versluis, Rocrail.net

 Without an official permission commercial use is not permitted.
 Forking this project is not permitted.

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

#include "rocview/wrapper/public/Gui.h"
#include "rocview/wrapper/public/PowerCtrl.h"

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
  m_SelectedRow = -1;
  initLabels();
  initValues(NULL);

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

  iONode ini = wxGetApp().getIni();
  iONode powerctrl = wGui.getpowerctrl(ini);
  if( powerctrl != NULL ) {
    if( wPowerCtrl.getcx(powerctrl) > 0 && wPowerCtrl.getcy(powerctrl) > 0 ) {
      SetSize(wPowerCtrl.getx(powerctrl), wPowerCtrl.gety(powerctrl), wPowerCtrl.getcx(powerctrl), wPowerCtrl.getcy(powerctrl));
    }
    else
      SetSize(wPowerCtrl.getx(powerctrl), wPowerCtrl.gety(powerctrl));
  }

}


void PowerCtrlDlg::initLabels() {
  m_Boosters->AutoSizeColumns();
  m_Boosters->AutoSizeRows();

  m_Boosters->SetColLabelValue(0, wxGetApp().getMsg("boosterid") );
  m_Boosters->SetColLabelValue(1, wxGetApp().getMsg("shortcut") );
  m_Boosters->SetColLabelValue(2, wxGetApp().getMsg("trackpower") );
  m_Boosters->SetColLabelValue(3, wxGetApp().getMsg("powerdistrict") );
  m_Boosters->SetColLabelValue(4, wxT("mA") );
  m_Boosters->SetColLabelValue(5, wxT("V") );
  m_Boosters->SetColLabelValue(6, wxT("°C") );
}


void PowerCtrlDlg::initValues(iONode event) {
  m_Boosters->DeleteRows(0,m_Boosters->GetNumberRows());
  MapOp.clear(m_BoosterMap);

  if( event != NULL ) {
    TraceOp.trc( "pwrctrl", TRCLEVEL_INFO, __LINE__, 9999, "event from booster %08X", wBooster.getuid(event) );
  }

  iONode model = wxGetApp().getModel();
  if( model != NULL ) {
    iONode boosterlist = wPlan.getboosterlist( model );
    if( boosterlist != NULL ) {
      iONode booster = wBoosterList.getbooster( boosterlist );
      while( booster != NULL ) {
        if( event != NULL && StrOp.equals( wBooster.name(), NodeOp.getName(event))) {
          if( wBooster.getuid(booster) > 0 && wBooster.getuid(event) > 0 && wBooster.getuid(booster) == wBooster.getuid(event) ) {
            wBooster.setload(booster, wBooster.getload(event));
            wBooster.setvolt(booster, wBooster.getvolt(event));
            wBooster.settemp(booster, wBooster.gettemp(event));
            wBooster.setpower(booster, wBooster.ispower(event));
          }
        }

        MapOp.put(m_BoosterMap, wBooster.getid( booster ), (obj)booster );
        m_Boosters->AppendRows();
        m_Boosters->SetCellValue(m_Boosters->GetNumberRows()-1, 0, wxString(wBooster.getid( booster ),wxConvUTF8) );
        m_Boosters->SetCellValue(m_Boosters->GetNumberRows()-1, 1, wBooster.isshortcut(booster)?wxGetApp().getMsg("yes"):wxGetApp().getMsg("no") );
        m_Boosters->SetCellValue(m_Boosters->GetNumberRows()-1, 2, wBooster.ispower(booster)?wxGetApp().getMsg("on"):wxGetApp().getMsg("off") );
        m_Boosters->SetCellValue(m_Boosters->GetNumberRows()-1, 3, wxString(wBooster.getdistrict( booster ),wxConvUTF8) );
        m_Boosters->SetCellValue(m_Boosters->GetNumberRows()-1, 4, wxString::Format(_T("%d"), wBooster.getload(booster)) );
        m_Boosters->SetCellValue(m_Boosters->GetNumberRows()-1, 5, wxString::Format(_T("%d.%d"), wBooster.getvolt(booster)/1000, (wBooster.getvolt(booster)%1000)/100)  );
        m_Boosters->SetCellValue(m_Boosters->GetNumberRows()-1, 6, wxString::Format(_T("%d"), wBooster.gettemp(booster)) );
        m_Boosters->SetReadOnly( m_Boosters->GetNumberRows()-1, 0, true );
        m_Boosters->SetReadOnly( m_Boosters->GetNumberRows()-1, 1, true );
        m_Boosters->SetReadOnly( m_Boosters->GetNumberRows()-1, 2, true );
        m_Boosters->SetReadOnly( m_Boosters->GetNumberRows()-1, 3, true );
        m_Boosters->SetReadOnly( m_Boosters->GetNumberRows()-1, 4, true );
        m_Boosters->SetReadOnly( m_Boosters->GetNumberRows()-1, 5, true );
        m_Boosters->SetReadOnly( m_Boosters->GetNumberRows()-1, 6, true );

        int row = m_Boosters->GetNumberRows()-1;
        m_Boosters->SetCellBackgroundColour( row, 1,
            wBooster.isshortcut(booster)?wxColour(240,200,200):wxColour(200,240,200));
        m_Boosters->SetCellBackgroundColour( row, 2,
            wBooster.ispower(booster)?wxColour(200,240,200):wxColour(240,200,200));


        booster = wBoosterList.nextbooster( boosterlist, booster );
      };
    }
  }

  if( m_SelectedRow != -1 ) {
    m_Boosters->SelectRow(m_SelectedRow);
  }
  else {
    m_On->Enable( false );
    m_Off->Enable( false );
  }
}


void PowerCtrlDlg::onCellLeftClick( wxGridEvent& event ) {
  m_SelectedRow = event.GetRow();
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
  initValues(event);
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
  //EndModal( wxID_OK );
  int x,y;
  GetPosition(&x,&y);
  int cx,cy;
  GetSize(&cx,&cy);

  iONode ini = wxGetApp().getIni();
  iONode powerctrl = wGui.getpowerctrl(ini);
  if( powerctrl == NULL ) {
    powerctrl = NodeOp.inst(wPowerCtrl.name(), ini, ELEMENT_NODE);
    NodeOp.addChild(ini, powerctrl);
  }
  wPowerCtrl.setx(powerctrl,x);
  wPowerCtrl.sety(powerctrl,y);
  wPowerCtrl.setcx(powerctrl,cx);
  wPowerCtrl.setcy(powerctrl,cy);

  wxGetApp().getFrame()->resetPowerCtrlRef();
  Destroy();
}

void PowerCtrlDlg::onClose( wxCloseEvent& event ) {
}


void PowerCtrlDlg::onSize( wxSizeEvent& event ) {
  powerctrlgen::OnSize( event );
}
