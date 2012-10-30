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

#include "bidibdlg.h"
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/defs.h"
#endif


#include "rocview/public/guiapp.h"

#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/BiDiB.h"
#include "rocrail/wrapper/public/BiDiBnode.h"

#include "rocutils/public/vendors.h"

#include "rocs/public/strtok.h"

BidibDlg::BidibDlg( wxWindow* parent, iONode props, const char* devices )
  :bidibdlggen( parent )
{
  m_Props = props;
  m_Devices = devices;
  __initVendors();
  initLabels();
  initValues();

  m_General->GetSizer()->Layout();
  m_Options->GetSizer()->Layout();
  m_Nodes->GetSizer()->Layout();

  m_Notebook->Fit();

  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);

}


void BidibDlg::initLabels() {
  // Tabs
  m_Notebook->SetPageText( 0, wxGetApp().getMsg( "general" ) );
  m_Notebook->SetPageText( 1, wxGetApp().getMsg( "options" ) );
  m_Notebook->SetPageText( 2, wxGetApp().getMsg( "nodes" ) );

  // General
  m_labIID->SetLabel( wxGetApp().getMsg( "iid" ) );
  m_labDevice->SetLabel( wxGetApp().getMsg( "port" ) );
  m_BPS->SetLabel(wxGetApp().getMsg( "bps" ));
  m_SubLib->SetLabel(wxGetApp().getMsg( "sublib" ));

  // Options
  m_SecAck->SetLabel(wxGetApp().getMsg( "enable" ));
  m_labSecAckInt->SetLabel(wxGetApp().getMsg( "interval" ));

  // Nodes

  // Buttons
  m_StdButtonOK->SetLabel( wxGetApp().getMsg( "ok" ) );
  m_StdButtonCancel->SetLabel( wxGetApp().getMsg( "cancel" ) );

}


void BidibDlg::initValues() {
  if( m_Props == NULL )
    return;

  // General
  m_IID->SetValue( wxString( wDigInt.getiid( m_Props ), wxConvUTF8 ) );
  m_Device->SetValue( wxString( wDigInt.getdevice( m_Props ), wxConvUTF8 ) );

  if( m_Devices != NULL ) {
    iOStrTok tok = StrTokOp.inst(m_Devices, ',');
    while( StrTokOp.hasMoreTokens(tok) ) {
      m_Device->Append( wxString( StrTokOp.nextToken(tok), wxConvUTF8 ) );
    }
    StrTokOp.base.del(tok);
  }

  if( wDigInt.getbps( m_Props ) == 19200 )
    m_BPS->SetSelection(0);
  else if( wDigInt.getbps( m_Props ) == 1000000 )
    m_BPS->SetSelection(2);
  else
    m_BPS->SetSelection(1);

  m_SubLib->SetSelection(0);

  // Options
  iONode bidib = wDigInt.getbidib(m_Props);
  if( bidib == NULL ) {
    bidib = NodeOp.inst( wBiDiB.name(), m_Props, ELEMENT_NODE );
    NodeOp.addChild( m_Props, bidib );
  }
  m_SecAck->SetValue( wBiDiB.issecAck( bidib ) ? true:false );
  m_SecAckInt->SetValue( wBiDiB.getsecAckInt( bidib ) );

  // Nodes
  initNodes();
}


void BidibDlg::initNodes() {
  if( m_Props == NULL )
    return;

  TraceOp.trc( "bidibdlg", TRCLEVEL_INFO, __LINE__, 9999, "init node list..." );
  iONode bidib = wDigInt.getbidib(m_Props);

  m_NodeList->Clear();
  iONode node = wBiDiB.getbidibnode(bidib);
  while( node != NULL ) {
    char uid[256];
    StrOp.fmtb( uid, "%08X, %s, %s",
        wBiDiBnode.getuid(node), wBiDiBnode.getpath(node), wBiDiBnode.getclass(node) );

    m_NodeList->Append( wxString(uid,wxConvUTF8), node );
    node = wBiDiB.nextbidibnode(bidib, node);
  }
}


void BidibDlg::evaluate() {
  if( m_Props == NULL )
    return;

  // General
  wDigInt.setiid( m_Props, m_IID->GetValue().mb_str(wxConvUTF8) );
  wDigInt.setdevice( m_Props, m_Device->GetValue().mb_str(wxConvUTF8) );

  if( m_BPS->GetSelection() == 0 )
    wDigInt.setbps( m_Props, 19200 );
  else if( m_BPS->GetSelection() == 1 )
    wDigInt.setbps( m_Props, 115200 );
  else
    wDigInt.setbps( m_Props, 1000000 );

  wDigInt.setsublib( m_Props, wDigInt.sublib_serial);

  // Options
  iONode bidib = wDigInt.getbidib(m_Props);
  wBiDiB.setsecAck( bidib, m_SecAck->IsChecked() ? True:False );
  wBiDiB.setsecAckInt( bidib, m_SecAckInt->GetValue() );
}


void BidibDlg::OnNodeList( wxCommandEvent& event ) {
  if( m_Props == NULL )
    return;
  TraceOp.trc( "bidibdlg", TRCLEVEL_INFO, __LINE__, 9999, "select node..." );

  if( m_NodeList->GetSelection() != wxNOT_FOUND ) {
    iONode node = (iONode)m_NodeList->GetClientData(m_NodeList->GetSelection());
    if( node != NULL ) {
    }
    else
      TraceOp.trc( "bidibdlg", TRCLEVEL_INFO, __LINE__, 9999, "no selection..." );
  }
  else
    TraceOp.trc( "bidibdlg", TRCLEVEL_INFO, __LINE__, 9999, "no selection..." );
}


void BidibDlg::OnCancel( wxCommandEvent& event ) {
  EndModal( wxID_CANCEL );
}


void BidibDlg::OnOK( wxCommandEvent& event ) {
  evaluate();
  EndModal( wxID_OK );
}

void BidibDlg::onConfigureNodes( wxCommandEvent& event ) {
  wxCommandEvent menuevent( wxEVT_COMMAND_MENU_SELECTED, ME_BiDiB );
  wxPostEvent( wxGetApp().getFrame(), menuevent );
}

