/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2012 Rob Versluis, Rocrail.net

 Without an official permission commercial use is not permitted.
 Forking this project is not permitted.

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

#include "mgvdlg.h"

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/defs.h"
#endif

#include "rocview/public/guiapp.h"
#include "rocrail/wrapper/public/Switch.h"

#include "rocview/wrapper/public/Gui.h"
#include "rocview/wrapper/public/MGV.h"
#include "rocview/wrapper/public/MGVServo.h"

#include "rocs/public/trace.h"
#include "rocs/public/system.h"

MGVDlg::MGVDlg( wxWindow* parent ):mgvdlggen( parent ) {
  m_bProgrammingMode = false;
  m_MGVmap = MapOp.inst();
  m_MGVServo = NULL;
  initLabels();

  m_Setup->GetSizer()->Layout();
  m_Servo->GetSizer()->Layout();

  m_MGVBook->Fit();

  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);

  initValues();

  //m_ResetBits->Enable(true);
  m_ProgrammingOn->Enable(true);
  m_ProgrammingOff->Enable(false);
  m_Servo->Enable(false);

  iONode mgv = wGui.getmgv( wxGetApp().getIni() );
  if( mgv == NULL ) {
    mgv = NodeOp.inst(wMGV.name(), wxGetApp().getIni(), ELEMENT_NODE);
    NodeOp.addChild(wxGetApp().getIni(), mgv);
  }

  m_IID->SetValue(wxString(wMGV.getiid(mgv),wxConvUTF8));
  m_Addr1->SetValue(wMGV.getaddr1(mgv));
  m_Addr2->SetValue(wMGV.getaddr2(mgv));
  m_Addr3->SetValue(wMGV.getaddr3(mgv));
  m_Addr4->SetValue(wMGV.getaddr4(mgv));
  m_Port1->SetValue(wMGV.getport1(mgv));
  m_Port2->SetValue(wMGV.getport2(mgv));
  m_Port3->SetValue(wMGV.getport3(mgv));
  m_Port4->SetValue(wMGV.getport4(mgv));


  iONode mgvservo = wMGV.getmgvservo(mgv);
  while( mgvservo != NULL ) {
    char key[80];
    StrOp.fmtb(key, "%d-%d", wMGVServo.getaddr(mgvservo), wMGVServo.getport(mgvservo));
    MapOp.put(m_MGVmap, key, (obj)mgvservo);
    mgvservo = wMGV.nextmgvservo(mgv, mgvservo);
  };
}

void MGVDlg::SetBit(int addr, int port, bool on) {
  /* switch command with following parameters */
  /* Address-Port, Direction = on, Output = true */
  iONode cmd = NodeOp.inst( wSwitch.name(), NULL, ELEMENT_NODE );
  wSwitch.setiid(cmd, m_IID->GetValue().mb_str(wxConvUTF8));
  wSwitch.setaddr1( cmd, addr );
  wSwitch.setport1( cmd, port );
  wSwitch.setcmd( cmd, on ? wSwitch.straight:wSwitch.turnout );
  wxGetApp().sendToRocrail( cmd );
  NodeOp.base.del(cmd);
}

/* Send a 3-bits nibble. */
void MGVDlg::SendNibble(int value)
{
    /* Lower b4 first */
    SetBit(m_Addr4->GetValue(), m_Port4->GetValue(), false);

    /* Set b1 */
    SetBit(m_Addr1->GetValue(), m_Port1->GetValue(), (value & 0x01) != 0);
    /* Set b2 */
    SetBit(m_Addr2->GetValue(), m_Port2->GetValue(), (value & 0x02) != 0);
    /* Set b3 */
    SetBit(m_Addr3->GetValue(), m_Port3->GetValue(), (value & 0x04) != 0);

    /* Raise B4 to transmit */
    SetBit(m_Addr4->GetValue(), m_Port4->GetValue(), true);

    /* Wait a while */
    ThreadOp.sleep(20);

    /* Lower B4 to avoid duplication */
    SetBit(m_Addr4->GetValue(), m_Port4->GetValue(), false);

    /* Wait a while */
    ThreadOp.sleep(5);
}




void MGVDlg::initLabels() {
  m_MGVBook->SetPageText( 0, wxGetApp().getMsg( "setup" ) );
  m_MGVBook->SetPageText( 1, wxGetApp().getMsg( "servo" ) );

  // Setup
  m_labAddress->SetLabel( wxGetApp().getMsg( "address" ) );
  m_labPort->SetLabel( wxGetApp().getMsg( "port" ) );
  m_labProgramming->SetLabel( wxGetApp().getMsg( "programming" ) );
  m_ProgrammingOn->SetLabel( wxGetApp().getMsg( "on" ).MakeUpper() );
  m_ProgrammingOff->SetLabel( wxGetApp().getMsg( "off" ).MakeUpper() );
  //m_ResetBits->SetLabel( wxGetApp().getMsg( "reset" ) );
  m_Addr1->SetValue(0);
  m_Addr2->SetValue(0);
  m_Addr3->SetValue(0);
  m_Addr4->SetValue(0);
  m_Port1->SetValue(0);
  m_Port2->SetValue(0);
  m_Port3->SetValue(0);
  m_Port4->SetValue(0);

  // Servo
  m_ServoSelection->SetLabel( wxGetApp().getMsg( "servo" ) );
  m_labServoLeft->SetLabel( wxGetApp().getMsg( "left" ) );
  m_labServoRight->SetLabel( wxGetApp().getMsg( "right" ) );
  m_labServoSpeed->SetLabel( wxGetApp().getMsg( "speed" ) );
  m_ServoRelay->SetLabel( wxGetApp().getMsg( "polarizationrelay" ) );

  // Buttons
  m_StdButtonOK->SetLabel( wxGetApp().getMsg( "ok" ) );
}

void MGVDlg::initValues() {
  if( m_MGVServo != NULL ) {
    m_ServoLeftAng->SetValue(wMGVServo.getleftangle(m_MGVServo));
    m_ServoRightAng->SetValue(wMGVServo.getrightangle(m_MGVServo));
    m_ServoSpeed->SetValue(wMGVServo.getspeed(m_MGVServo));
    m_ServoRelay->SetValue(wMGVServo.isrelay(m_MGVServo)?true:false);
  }
  else {
    m_ServoLeftAng->SetValue(50);
    m_ServoRightAng->SetValue(50);
    m_ServoSpeed->SetValue(1);
    m_ServoRelay->SetValue(false);
  }
}

void MGVDlg::evaluate() {
  if( m_MGVServo != NULL ) {
    wMGVServo.setleftangle(m_MGVServo, m_ServoLeftAng->GetValue());
    wMGVServo.setrightangle(m_MGVServo, m_ServoRightAng->GetValue());
    wMGVServo.setspeed(m_MGVServo, m_ServoSpeed->GetValue());
    wMGVServo.setrelay(m_MGVServo, m_ServoRelay->IsChecked()?True:False);
  }
}

void MGVDlg::enableSetup(bool enable) {
  m_ProgrammingOn->Enable(enable);
  //m_ResetBits->Enable(enable);
  m_ProgrammingOff->Enable(!enable);
  m_Addr1->Enable(enable);
  m_Addr2->Enable(enable);
  m_Addr3->Enable(enable);
  m_Addr4->Enable(enable);
  m_Port1->Enable(enable);
  m_Port2->Enable(enable);
  m_Port3->Enable(enable);
  m_Port4->Enable(enable);
}


void MGVDlg::OnResetBits( wxCommandEvent& event ) {
  m_ProgrammingOn->Enable(true);
  m_ProgrammingOff->Enable(true);

  SetBit(m_Addr4->GetValue(), m_Port4->GetValue(), false);
  SetBit(m_Addr3->GetValue(), m_Port3->GetValue(), false);
  SetBit(m_Addr2->GetValue(), m_Port2->GetValue(), false);
  SetBit(m_Addr1->GetValue(), m_Port1->GetValue(), false);
}


void MGVDlg::OnProgrammingOn( wxCommandEvent& event ){
  enableSetup(false);
  EnterProgrammingMode();
  m_ProgrammingOff->Enable(true);
  ThreadOp.sleep(100);
  m_Servo->Enable(true);

  OnServoSelect();
}

void MGVDlg::EnterProgrammingMode(){
  m_bProgrammingMode = true;
  // Turn all bits off (Reset did this already!?)
  SetBit(m_Addr4->GetValue(), m_Port4->GetValue(), false);
  SetBit(m_Addr3->GetValue(), m_Port3->GetValue(), false);
  SetBit(m_Addr2->GetValue(), m_Port2->GetValue(), false);
  SetBit(m_Addr1->GetValue(), m_Port1->GetValue(), false);
  ThreadOp.sleep(1000);

  // Turn bit 1 on
  SetBit(m_Addr1->GetValue(), m_Port1->GetValue(), true);
  ThreadOp.sleep(6);
  // Turn bit 2 on
  SetBit(m_Addr2->GetValue(), m_Port2->GetValue(), true);
  ThreadOp.sleep(6);
  // Turn bit 3 on
  SetBit(m_Addr3->GetValue(), m_Port3->GetValue(), true);
  ThreadOp.sleep(6);
  // Turn bit 4 on
  SetBit(m_Addr4->GetValue(), m_Port4->GetValue(), true);
  ThreadOp.sleep(6);
  // Turn bit 1 off
  SetBit(m_Addr1->GetValue(), m_Port1->GetValue(), false);
  ThreadOp.sleep(6);
  // Turn bit 2 off
  SetBit(m_Addr2->GetValue(), m_Port2->GetValue(), false);
  ThreadOp.sleep(6);
  // Turn bit 3 off
  SetBit(m_Addr3->GetValue(), m_Port3->GetValue(), false);
  ThreadOp.sleep(6);
  // Turn bit 4 off
  SetBit(m_Addr4->GetValue(), m_Port4->GetValue(), false);
  ThreadOp.sleep(6);
}

void MGVDlg::ExitProgrammingMode(){
  SendNibble(0x07); /* Command */

  /* Reset all 4 bits */
  SetBit(m_Addr4->GetValue(), m_Port4->GetValue(), false);
  SetBit(m_Addr3->GetValue(), m_Port3->GetValue(), false);
  SetBit(m_Addr2->GetValue(), m_Port2->GetValue(), false);
  SetBit(m_Addr1->GetValue(), m_Port1->GetValue(), false);
  m_bProgrammingMode = false;
}

void MGVDlg::OnProgrammingOff( wxCommandEvent& event ){
  enableSetup(true);
  m_Servo->Enable(false);
  ExitProgrammingMode();
}

void MGVDlg::OnServoSelect(){
  int l_iServo = m_ServoSelection->GetSelection();
  int addr = 0;
  int port = 0;
  m_MGVServo = NULL;

  char key[80];
  if( l_iServo == 0 ) {
    addr = m_Addr1->GetValue(); port = m_Port1->GetValue();
  }
  else if( l_iServo == 1 ) {
    addr = m_Addr2->GetValue(); port = m_Port2->GetValue();
  }
  else if( l_iServo == 2 ) {
    addr = m_Addr3->GetValue(); port = m_Port3->GetValue();
  }
  else if( l_iServo == 3 ) {
    addr = m_Addr4->GetValue(); port = m_Port4->GetValue();
  }

  StrOp.fmtb(key, "%d-%d", addr, port);

  m_MGVServo = (iONode)MapOp.get(m_MGVmap, key);
  if( m_MGVServo == NULL ) {
    m_MGVServo = NodeOp.inst(wMGVServo.name(), wGui.getmgv( wxGetApp().getIni() ), ELEMENT_NODE );
    wMGVServo.setleftangle(m_MGVServo, 50);
    wMGVServo.setrightangle(m_MGVServo, 50);
    wMGVServo.setspeed(m_MGVServo, 1);
    wMGVServo.setaddr(m_MGVServo, addr );
    wMGVServo.setport(m_MGVServo, port );

    NodeOp.addChild(wGui.getmgv( wxGetApp().getIni() ), m_MGVServo);
    MapOp.put(m_MGVmap, key, (obj)m_MGVServo);
  }
  initValues();


  SendNibble(0x01); /* Command */
  SendNibble(l_iServo); /* Turnout encoded as 0-3 */
}

void MGVDlg::OnServoSelect( wxCommandEvent& event ){
  OnServoSelect();
}

void MGVDlg::OnServoLeftAngle( wxScrollEvent& event ){
  setServoLeftAngle();
}

void MGVDlg::setServoLeftAngle(){
  int l_iAngle = m_ServoLeftAng->GetValue();
  evaluate();
  SendNibble(0x02); /* Command */
  SendNibble((l_iAngle >> 1) & 0x07); /* value bit 4,3,2 */
  SendNibble((l_iAngle >> 4) & 0x07); /* value bit 7,6,5 */
}

void MGVDlg::OnServoRightAngle( wxScrollEvent& event ){
  setServoRightAngle();
}

void MGVDlg::setServoRightAngle(){
  int l_iAngle = m_ServoRightAng->GetValue();
  evaluate();
  SendNibble(0x03); /* Command */
  SendNibble((l_iAngle >> 1) & 0x07); /* value bit 4,3,2 */
  SendNibble((l_iAngle >> 4) & 0x07); /* value bit 7,6,5 */
}

static int SPEED_MASKS[] = { 0x01, 0x05, 0x0D, 0x0F };

void MGVDlg::OnServoSpeed( wxScrollEvent& event ){
  setServoSpeed();
}

void MGVDlg::setServoSpeed(){
  int l_iSpeed = m_ServoSpeed->GetValue();
  evaluate();
  SendNibble(0x05); /* Command */
  SendNibble(SPEED_MASKS[l_iSpeed - 1] & 0x07); /* LSB */
  SendNibble((SPEED_MASKS[l_iSpeed - 1] >> 3) & 0x07); /* MSB */
}

void MGVDlg::OnServoRelay( wxCommandEvent& event ){
  evaluate();
  SendNibble(0x04); /* Command */
  SendNibble(m_ServoRelay->IsChecked() ? 0 : 1);
}

void MGVDlg::OnOK( wxCommandEvent& event ){
  if(m_bProgrammingMode) {
    ExitProgrammingMode();
  }

  iONode mgv = wGui.getmgv( wxGetApp().getIni() );

  wMGV.setiid(mgv, m_IID->GetValue().mb_str(wxConvUTF8));
  wMGV.setaddr1(mgv, m_Addr1->GetValue());
  wMGV.setaddr2(mgv, m_Addr2->GetValue());
  wMGV.setaddr3(mgv, m_Addr3->GetValue());
  wMGV.setaddr4(mgv, m_Addr4->GetValue());
  wMGV.setport1(mgv, m_Port1->GetValue());
  wMGV.setport2(mgv, m_Port2->GetValue());
  wMGV.setport3(mgv, m_Port3->GetValue());
  wMGV.setport4(mgv, m_Port4->GetValue());

  MapOp.base.del(m_MGVmap);
  EndModal( wxID_OK );
}



void MGVDlg::onSetLeftAngle( wxCommandEvent& event ){
  setServoLeftAngle();
}


void MGVDlg::onSetRightAngle( wxCommandEvent& event ) {
  setServoRightAngle();
}


void MGVDlg::onSetSpeed( wxCommandEvent& event ) {
  setServoSpeed();
}
