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

#include "rocnetdlg.h"
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/defs.h"
#endif


#include "rocview/public/guiapp.h"
#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/RocNet.h"

RocNetDlg::RocNetDlg( wxWindow* parent, iONode props ):rocnetdlggen( parent ) {

  m_Props = props;

  iONode rnini = wDigInt.getrocnet(m_Props);

  if( rnini == NULL ) {
    rnini = NodeOp.inst( wRocNet.name(), m_Props, ELEMENT_NODE );
    NodeOp.addChild( m_Props, rnini );
  }

  initLabels();
  initValues();
}


void RocNetDlg::initLabels() {
  m_labIID->SetLabel( wxGetApp().getMsg( "iid" ) );
  m_labDevice->SetLabel( wxGetApp().getMsg( "port" ) );
  m_BPS->SetLabel(wxGetApp().getMsg( "bps" ));
  m_Sublib->SetLabel(wxGetApp().getMsg( "sublib" ));
  m_Address->SetLabel(wxGetApp().getMsg( "address" ));
  m_Port->SetLabel(wxGetApp().getMsg( "port" ));

}


void RocNetDlg::initValues() {
  iONode rnini = wDigInt.getrocnet(m_Props);

  // General
  m_IID->SetValue( wxString( wDigInt.getiid( m_Props ), wxConvUTF8 ) );
  m_RNID->SetValue( wRocNet.getid( rnini ) );
  m_Device->SetValue( wxString( wDigInt.getdevice( m_Props ), wxConvUTF8 ) );

  if( wDigInt.getbps( m_Props ) == 9600 )
    m_BPS->SetSelection(0);
  else if( wDigInt.getbps( m_Props ) == 38400 )
    m_BPS->SetSelection(2);
  else if( wDigInt.getbps( m_Props ) == 57600 )
    m_BPS->SetSelection(3);
  else if( wDigInt.getbps( m_Props ) == 115200 )
    m_BPS->SetSelection(4);
  else
    m_BPS->SetSelection(1); // default 19200

  if( StrOp.equals( wDigInt.sublib_udp, wDigInt.getsublib(m_Props) )) {
    m_Sublib->SetSelection(0);
    m_BPS->Enable(false);
    m_Device->Enable(false);
    m_Address->Enable(true);
    m_Port->Enable(true);
  }
  else {
    // Serial
    m_Sublib->SetSelection(1);
    m_BPS->Enable(true);
    m_Device->Enable(true);
    m_Address->Enable(false);
    m_Port->Enable(false);
  }

  m_CRC->SetValue( wRocNet.iscrc(rnini) ? true:false);
  m_Address->SetValue( wxString( wRocNet.getaddr( rnini ), wxConvUTF8 ) );
  m_Port->SetValue( wRocNet.getport( rnini ) );

}


void RocNetDlg::evaluate() {
  iONode rnini = wDigInt.getrocnet(m_Props);

  wDigInt.setiid( m_Props, m_IID->GetValue().mb_str(wxConvUTF8) );
  wRocNet.setid( rnini, m_RNID->GetValue() );
  wDigInt.setdevice( m_Props, m_Device->GetValue().mb_str(wxConvUTF8) );
  wRocNet.setaddr( rnini, m_Address->GetValue().mb_str(wxConvUTF8) );
  wRocNet.setport( rnini, m_Port->GetValue() );

  if( m_Sublib->GetSelection() == 0 )
    wDigInt.setsublib(m_Props, wDigInt.sublib_udp );
  else
    wDigInt.setsublib(m_Props, wDigInt.sublib_serial );

  if( m_BPS->GetSelection() == 0 )
    wDigInt.setbps( m_Props, 9600 );
  else if( m_BPS->GetSelection() == 1 )
    wDigInt.setbps( m_Props, 19200 );
  else if( m_BPS->GetSelection() == 2 )
    wDigInt.setbps( m_Props, 38400 );
  else if( m_BPS->GetSelection() == 3 )
    wDigInt.setbps( m_Props, 57600 );
  else if( m_BPS->GetSelection() == 4 )
    wDigInt.setbps( m_Props, 115200 );

  wRocNet.setcrc(m_Props, m_CRC->IsChecked()?True:False);
}



void RocNetDlg::OnSublib( wxCommandEvent& event ) {
  if( m_Sublib->GetSelection() == 0 ) {
    m_BPS->Enable(false);
    m_Device->Enable(false);
    m_Address->Enable(true);
    m_Port->Enable(true);
    return;
  }

  if( m_Sublib->GetSelection() == 1 ) {
    m_BPS->Enable(true);
    m_Device->Enable(true);
    m_Address->Enable(false);
    m_Port->Enable(false);
    return;
  }
}

void RocNetDlg::OnCancel( wxCommandEvent& event ) {
  EndModal( wxID_CANCEL );
}

void RocNetDlg::OnOK( wxCommandEvent& event ) {
  evaluate();
  EndModal( wxID_OK );
}
