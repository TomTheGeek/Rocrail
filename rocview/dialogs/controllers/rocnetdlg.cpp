/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2014 Rob Versluis, Rocrail.net

 


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
#include "rocrail/wrapper/public/RocNetNode.h"
#include "rocs/public/strtok.h"
#include "rocutils/public/vendors.h"

#include "rocview/dialogs/decoders/rocnetnodedlg.h"

RocNetDlg::RocNetDlg( wxWindow* parent, iONode props, const char* devices ):rocnetdlggen( parent ) {

  m_Props = props;
  m_Devices = devices;

  iONode rnini = wDigInt.getrocnet(m_Props);

  if( rnini == NULL ) {
    rnini = NodeOp.inst( wRocNet.name(), m_Props, ELEMENT_NODE );
    NodeOp.addChild( m_Props, rnini );
  }
  __initVendors();

  initLabels();

  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);
  GetSizer()->Layout();

  initValues();
}


void RocNetDlg::initLabels() {
  m_labIID->SetLabel( wxGetApp().getMsg( "iid" ) );
  m_labDevice->SetLabel( wxGetApp().getMsg( "port" ) );
  m_BPS->SetLabel(wxGetApp().getMsg( "bps" ));
  m_Sublib->SetLabel(wxGetApp().getMsg( "sublib" ));
  m_Address->SetLabel(wxGetApp().getMsg( "address" ));
  m_Port->SetLabel(wxGetApp().getMsg( "port" ));
  m_Sack->SetLabel(wxGetApp().getMsg( "secureack" ));

  m_NodeList->InsertColumn(0, wxGetApp().getMsg( "id" ), wxLIST_FORMAT_RIGHT );
  m_NodeList->InsertColumn(1, wxGetApp().getMsg( "vendor" ), wxLIST_FORMAT_LEFT );
  m_NodeList->InsertColumn(2, wxGetApp().getMsg( "product" ), wxLIST_FORMAT_LEFT );
  m_NodeList->InsertColumn(3, wxGetApp().getMsg( "revision" ), wxLIST_FORMAT_RIGHT );
  m_NodeList->InsertColumn(4, wxT("I/O"), wxLIST_FORMAT_RIGHT );
  m_NodeList->InsertColumn(5, wxT("Sub IP"), wxLIST_FORMAT_CENTER);

}


void RocNetDlg::initValues() {
  iONode rnini = wDigInt.getrocnet(m_Props);

  // General
  m_IID->SetValue( wxString( wDigInt.getiid( m_Props ), wxConvUTF8 ) );
  m_RNID->SetValue( wRocNet.getid( rnini ) );
  m_Device->SetValue( wxString( wDigInt.getdevice( m_Props ), wxConvUTF8 ) );
  if( m_Devices != NULL ) {
    iOStrTok tok = StrTokOp.inst(m_Devices, ',');
    while( StrTokOp.hasMoreTokens(tok) ) {
      m_Device->Append( wxString( StrTokOp.nextToken(tok), wxConvUTF8 ) );
    }
    StrTokOp.base.del(tok);
  }

  if( wDigInt.getbps( m_Props ) == 9600 )
    m_BPS->SetSelection(0);
  else if( wDigInt.getbps( m_Props ) == 38400 )
    m_BPS->SetSelection(2);
  else if( wDigInt.getbps( m_Props ) == 57600 )
    m_BPS->SetSelection(3);
  else if( wDigInt.getbps( m_Props ) == 115200 )
    m_BPS->SetSelection(4);
  else if( wDigInt.getbps( m_Props ) == 125000 )
    m_BPS->SetSelection(5);
  else
    m_BPS->SetSelection(1); // default 19200

  if( StrOp.equals( wDigInt.sublib_tcp, wDigInt.getsublib(m_Props) )) {
    m_Sublib->SetSelection(2);
    m_BPS->Enable(false);
    m_Device->Enable(false);
    m_Address->Enable(true);
    m_Port->Enable(true);
  }
  else  if( StrOp.equals( wDigInt.sublib_serial, wDigInt.getsublib(m_Props) )) {
    // Serial
    m_Sublib->SetSelection(1);
    m_BPS->Enable(true);
    m_Device->Enable(true);
    m_Address->Enable(false);
    m_Port->Enable(false);
  }
  else {
    m_Sublib->SetSelection(0);
    m_BPS->Enable(false);
    m_Device->Enable(false);
    m_Address->Enable(true);
    m_Port->Enable(true);
  }

  m_CRC->SetValue( wRocNet.iscrc(rnini) ? true:false);
  m_Sack->SetValue( wRocNet.issack(rnini) ? true:false);
  m_Watchdog->SetValue( wRocNet.iswd(rnini) ? true:false);
  m_Address->SetValue( wxString( wRocNet.getaddr( rnini ), wxConvUTF8 ) );
  m_Port->SetValue( wRocNet.getport( rnini ) );

  initNodeList();
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
  else if( m_Sublib->GetSelection() == 2 )
    wDigInt.setsublib(m_Props, wDigInt.sublib_tcp );
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
  else if( m_BPS->GetSelection() == 5 )
    wDigInt.setbps( m_Props, 125000 );

  wRocNet.setcrc(m_Props, m_CRC->IsChecked()?True:False);
  wRocNet.setwd(m_Props, m_Watchdog->IsChecked()?True:False);
  wRocNet.setsack(m_Props, m_Sack->IsChecked()?True:False);
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

void RocNetDlg::onNodeListSelected( wxListEvent& event ) {
  wxCommandEvent menuevent( wxEVT_COMMAND_MENU_SELECTED, ME_RocNet );
  wxPostEvent( wxGetApp().getFrame(), menuevent );
}

void RocNetDlg::initNodeList() {
  m_NodeList->DeleteAllItems();
  int index = 0;
  iONode rnnode = wRocNet.getrocnetnode(m_Props);
  while( rnnode != NULL ) {
    m_NodeList->InsertItem( index, wxString::Format(_T("%d"), wRocNetNode.getid(rnnode)));
    m_NodeList->SetItem( index, 1, wxString( m_Vendor[wRocNetNode.getvendor(rnnode)&0xFF],wxConvUTF8) );
    m_NodeList->SetItem( index, 2, wxString(wRocNetNode.getclass(rnnode),wxConvUTF8));
    m_NodeList->SetItem( index, 3, wxString::Format(_T("%d"), wRocNetNode.getrevision(rnnode)));
    m_NodeList->SetItem( index, 4, wxString::Format(_T("%d"), wRocNetNode.getnrio(rnnode)));
    m_NodeList->SetItem( index, 5, wxString::Format(_T("%d.%d"), wRocNetNode.getsubip(rnnode)/256, wRocNetNode.getsubip(rnnode)%256));
    m_NodeList->SetItemPtrData(index, (wxUIntPtr)rnnode);
    index++;
    rnnode = wRocNet.nextrocnetnode(m_Props, rnnode);
  }
  m_NodeList->SetColumnWidth(0, wxLIST_AUTOSIZE);
  m_NodeList->SetColumnWidth(1, wxLIST_AUTOSIZE);
  m_NodeList->SetColumnWidth(2, wxLIST_AUTOSIZE);
  m_NodeList->SetColumnWidth(3, wxLIST_AUTOSIZE);
  m_NodeList->SetColumnWidth(4, wxLIST_AUTOSIZE);

}

