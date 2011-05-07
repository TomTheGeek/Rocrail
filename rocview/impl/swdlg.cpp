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
// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------
#ifdef __GNUG__
    #pragma implementation "rocgui.cpp"
    #pragma interface "rocgui.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "rocview/public/guiapp.h"
#include "rocview/public/swdlg.h"
#include "rocview/public/base.h"

#include "rocs/public/str.h"

#include "rocrail/wrapper/public/Plan.h"
#include "rocrail/wrapper/public/Switch.h"
#include "rocview/wrapper/public/Gui.h"
#include "rocview/wrapper/public/SwCtrl.h"

BEGIN_EVENT_TABLE(SwCtrlDlg, wxDialog)
    EVT_BUTTON(-1, SwCtrlDlg::OnButton)
    EVT_SPINCTRL(-1, SwCtrlDlg::OnSpinButton)
    EVT_CLOSE(SwCtrlDlg::OnClose)
END_EVENT_TABLE()



// ----------------------------------------------------------------------------
// LcDlg
// ----------------------------------------------------------------------------

SwCtrlDlg::SwCtrlDlg(wxWindow *parent)
             : wxDialog(parent, -1, wxString( wxGetApp().getMsg("swctrl") ),
             wxDefaultPosition, wxDefaultSize,
             wxDEFAULT_DIALOG_STYLE )
{

  wxBoxSizer  *sizer1 = new wxBoxSizer(wxVERTICAL);
  wxGridSizer *sizer2 = new wxGridSizer(0,2,5,5);

  m_Pin1Green = new wxButton( this, -1, _T("1 ||") );
  m_Pin1Red   = new wxButton( this, -1, _T("1 //") );
  m_Pin2Green = new wxButton( this, -1, _T("2 ||") );
  m_Pin2Red   = new wxButton( this, -1, _T("2 //") );
  m_Pin3Green = new wxButton( this, -1, _T("3 ||") );
  m_Pin3Red   = new wxButton( this, -1, _T("3 //") );
  m_Pin4Green = new wxButton( this, -1, _T("4 ||") );
  m_Pin4Red   = new wxButton( this, -1, _T("4 //") );

  m_Pin1Green->SetBackgroundColour( Base::getGreen() );
  m_Pin1Red->SetBackgroundColour( Base::getRed() );
  m_Pin2Green->SetBackgroundColour( Base::getGreen() );
  m_Pin2Red->SetBackgroundColour( Base::getRed() );
  m_Pin3Green->SetBackgroundColour( Base::getGreen() );
  m_Pin3Red->SetBackgroundColour( Base::getRed() );
  m_Pin4Green->SetBackgroundColour( Base::getGreen() );
  m_Pin4Red->SetBackgroundColour( Base::getRed() );

  m_labIID = new wxStaticText( this, -1, _("IID"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
  m_IID = new wxTextCtrl( this, -1, _T(""), wxDefaultPosition, wxDefaultSize, 0 );

  iONode swctrl = wGui.getswctrl( wxGetApp().getIni() );
  if( swctrl == NULL ) {
    swctrl = NodeOp.inst(wSwCtrl.name(), wxGetApp().getIni(), ELEMENT_NODE);
    NodeOp.addChild(wxGetApp().getIni(), swctrl);
  }

  m_IID->SetValue(wxString(wSwCtrl.getiid(swctrl),wxConvUTF8));
  m_Unit = wSwCtrl.getmodule(swctrl);

  m_UnitSpin = new wxSpinCtrl( this, wxID_ANY, _T("0"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 256, 0 );
  m_UnitSpin->SetToolTip( wxGetApp().getTip("decoder") );
  m_Quit = new wxButton( this, -1, wxGetApp().getMsg("cancel") );
  m_Quit->SetToolTip( wxGetApp().getTip("cancel") );

  m_UnitSpin->SetValue( m_Unit );

  sizer2->Add( m_Pin1Green, 0, wxALIGN_CENTER | wxEXPAND | wxALL, 2);
  sizer2->Add( m_Pin1Red  , 0, wxALIGN_CENTER | wxEXPAND | wxALL, 2);
  sizer2->Add( m_Pin2Green, 0, wxALIGN_CENTER | wxEXPAND | wxALL, 2);
  sizer2->Add( m_Pin2Red  , 0, wxALIGN_CENTER | wxEXPAND | wxALL, 2);
  sizer2->Add( m_Pin3Green, 0, wxALIGN_CENTER | wxEXPAND | wxALL, 2);
  sizer2->Add( m_Pin3Red  , 0, wxALIGN_CENTER | wxEXPAND | wxALL, 2);
  sizer2->Add( m_Pin4Green, 0, wxALIGN_CENTER | wxEXPAND | wxALL, 2);
  sizer2->Add( m_Pin4Red  , 0, wxALIGN_CENTER | wxEXPAND | wxALL, 2);

  sizer2->Add( m_labIID , 0, wxALIGN_BOTTOM | wxEXPAND | wxALL, 2);
  sizer2->Add( m_IID, 0, wxALIGN_CENTER | wxEXPAND | wxALL, 2);

  wxStaticText* unitlabel = new wxStaticText( this, -1, wxGetApp().getMsg("decoder"), wxPoint(0,0), wxDefaultSize, wxALIGN_RIGHT );
  sizer2->Add( unitlabel , 0, wxALIGN_BOTTOM | wxEXPAND | wxALL, 2);
  sizer2->Add( m_UnitSpin, 0, wxALIGN_CENTER | wxEXPAND | wxALL, 2);

  sizer1->Add( sizer2 , 0, wxALIGN_CENTER | wxEXPAND | wxALL, 0 );
  //sizer1->Add( m_UnitSpin , 0, wxALIGN_CENTER | wxEXPAND | wxALL, 5 );
  sizer1->Add( m_Quit , 0, wxALIGN_CENTER | wxEXPAND | wxALL, 2 );

  SetAutoLayout(TRUE);
  SetSizer(sizer1);

  sizer1->SetSizeHints(this);
  sizer1->Fit(this);

  m_Quit->SetDefault();
  setTooltips();
  
	Centre();
}

void SwCtrlDlg::setTooltips(void) {
  int unit = m_UnitSpin->GetValue();
  
  int pada = (unit-1) * 4;

  m_Pin1Green->SetToolTip( wxString::Format(_T("pada=%d"), pada+1));
  m_Pin1Red->SetToolTip( wxString::Format(_T("pada=%d"), pada+1));
  m_Pin2Green->SetToolTip( wxString::Format(_T("pada=%d"), pada+2));
  m_Pin2Red->SetToolTip( wxString::Format(_T("pada=%d"), pada+2));
  m_Pin3Green->SetToolTip( wxString::Format(_T("pada=%d"), pada+3));
  m_Pin3Red->SetToolTip( wxString::Format(_T("pada=%d"), pada+3));
  m_Pin4Green->SetToolTip( wxString::Format(_T("pada=%d"), pada+4));
  m_Pin4Red->SetToolTip( wxString::Format(_T("pada=%d"), pada+4));
}


void SwCtrlDlg::OnSpinButton(wxSpinEvent& event) {
  setTooltips();
}


void SwCtrlDlg::OnButton(wxCommandEvent& event)
{
  int pin = 0;
  const char* cmd = wSwitch.straight;

  m_Unit = m_UnitSpin->GetValue();
  
  if ( event.GetEventObject() == m_Quit ) {
    iONode swctrl = wGui.getswctrl( wxGetApp().getIni() );

    wSwCtrl.setiid(swctrl, m_IID->GetValue().mb_str(wxConvUTF8));
    wSwCtrl.setmodule(swctrl, m_UnitSpin->GetValue());
    Destroy();
    //EndModal(0);
  }
  else if( event.GetEventObject() == m_Pin1Green ) {
    pin = 1;
    cmd = wSwitch.straight;
  }
  else if( event.GetEventObject() == m_Pin1Red ) {
    pin = 1;
    cmd = wSwitch.turnout;
  }
  else if( event.GetEventObject() == m_Pin2Green ) {
    pin = 2;
    cmd = wSwitch.straight;
  }
  else if( event.GetEventObject() == m_Pin2Red ) {
    pin = 2;
    cmd = wSwitch.turnout;
  }
  else if( event.GetEventObject() == m_Pin3Green ) {
    pin = 3;
    cmd = wSwitch.straight;
  }
  else if( event.GetEventObject() == m_Pin3Red ) {
    pin = 3;
    cmd = wSwitch.turnout;
  }
  else if( event.GetEventObject() == m_Pin4Green ) {
    pin = 4;
    cmd = wSwitch.straight;
  }
  else if( event.GetEventObject() == m_Pin4Red ) {
    pin = 4;
    cmd = wSwitch.turnout;
  }
  else
  {
    event.Skip();
  }

  if( pin != 0 ) {
    iONode swcmd = NodeOp.inst( wSwitch.name(), NULL, ELEMENT_NODE );
    wSwitch.setaddr1( swcmd, m_Unit );
    wSwitch.setport1( swcmd, pin );
    wSwitch.setiid( swcmd, m_IID->GetValue().mb_str(wxConvUTF8) );
    wSwitch.setcmd( swcmd, cmd );

    // test Motorola
    wSwitch.setprot( swcmd, wSwitch.prot_M );
    wxGetApp().sendToRocrail( swcmd );
    
    // test DCC
    wSwitch.setprot( swcmd, wSwitch.prot_N );
    wxGetApp().sendToRocrail( swcmd );
    
    swcmd->base.del( swcmd );
  }
}


void SwCtrlDlg::OnClose(wxCloseEvent& event) {
  iONode swctrl = wGui.getswctrl( wxGetApp().getIni() );

  wSwCtrl.setiid(swctrl, m_IID->GetValue().mb_str(wxConvUTF8));
  wSwCtrl.setmodule(swctrl, m_UnitSpin->GetValue());
  Destroy();
}

