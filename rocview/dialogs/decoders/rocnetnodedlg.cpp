/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2013 Rob Versluis, Rocrail.net

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

#include "rocnetnodedlg.h"
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/defs.h"
#endif


#include "rocview/public/guiapp.h"

#include "rocrail/wrapper/public/RocRail.h"
#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/Program.h"
#include "rocrail/wrapper/public/RocNet.h"
#include "rocrail/wrapper/public/RocNetNode.h"
#include "rocs/public/strtok.h"
#include "rocutils/public/vendors.h"

RocnetNodeDlg::RocnetNodeDlg( wxWindow* parent, iONode ini )
  :rocnetnodegen( parent )
{
  m_Ini = ini;
  m_Digint = NULL;
  m_Props = NULL;
  m_PortGroup = 0;
  __initVendors();

  initLabels();
  initNodeList();
}

void RocnetNodeDlg::onRocnetWrite( wxCommandEvent& event ) {
  if( m_Props == NULL )
    return;

  iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  wProgram.setmodid(cmd, wRocNetNode.getid(m_Props));
  wProgram.setcmd( cmd, wProgram.nnreq );
  wProgram.setvalue(cmd, m_ID->GetValue() );
  wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
  wProgram.setlntype(cmd, wProgram.lntype_rocnet);
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);
}


void RocnetNodeDlg::initPorts() {
  wxSpinCtrl* l_IO[] = { NULL, m_IO1, m_IO2, m_IO3, m_IO4, m_IO5, m_IO6, m_IO7, m_IO8};
  wxRadioBox* l_Type[] = {NULL, m_Type1, m_Type2, m_Type3, m_Type4, m_Type5, m_Type6, m_Type7, m_Type8};
  wxSpinCtrl* l_Delay[] = { NULL, m_Delay1, m_Delay2, m_Delay3, m_Delay4, m_Delay5, m_Delay6, m_Delay7, m_Delay8};
  wxStaticText* l_labPort[] = { NULL, m_labPort1, m_labPort2, m_labPort3, m_labPort4, m_labPort5, m_labPort6, m_labPort7, m_labPort8 };
  for( int i = 1; i < 9; i++ ) {
    l_labPort[i]->SetLabel( wxString::Format(wxT("%d"),i + m_PortGroup*8) );
    l_IO[i]->SetValue(0);
    l_Type[i]->SetSelection(0);
    l_Delay[i]->SetValue(0);
  }
}

void RocnetNodeDlg::onPortPrev( wxCommandEvent& event )
{
  if( m_PortGroup > 0 ) {
    m_PortGroup--;
    initPorts();
  }
}

void RocnetNodeDlg::onPortNext( wxCommandEvent& event )
{
  if( m_Props == NULL )
    return;
  int nrio = wRocNetNode.getnrio(m_Props);
  if(nrio > (m_PortGroup+1) * 8) {
    m_PortGroup++;
    initPorts();
  }
}

void RocnetNodeDlg::onPortRead( wxCommandEvent& event ) {
  if( m_Props == NULL )
    return;

  iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  wProgram.setmodid(cmd, wRocNetNode.getid(m_Props));
  wProgram.setcmd( cmd, wProgram.nvget );
  wProgram.setval1(cmd, 1 + m_PortGroup*8 ); // range
  wProgram.setval2(cmd, 8 + m_PortGroup*8 );
  wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
  wProgram.setlntype(cmd, wProgram.lntype_rocnet);
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);
}

void RocnetNodeDlg::onPortWrite( wxCommandEvent& event )
{
// TODO: Implement onPortWrite
}

void RocnetNodeDlg::onOK( wxCommandEvent& event ) {
  EndModal( wxID_OK );
}

void RocnetNodeDlg::onClose( wxCloseEvent& event ) {
  EndModal( wxID_OK );
}


void RocnetNodeDlg::initLabels() {
  m_NodeList->InsertColumn(0, wxGetApp().getMsg( "id" ), wxLIST_FORMAT_RIGHT );
  m_NodeList->InsertColumn(1, wxGetApp().getMsg( "vendor" ), wxLIST_FORMAT_LEFT );
  m_NodeList->InsertColumn(2, wxGetApp().getMsg( "product" ), wxLIST_FORMAT_LEFT );
  m_NodeList->InsertColumn(3, wxGetApp().getMsg( "version" ), wxLIST_FORMAT_LEFT );
  m_NodeList->InsertColumn(4, wxT("I/O"), wxLIST_FORMAT_RIGHT );

  iONode l_RocrailIni = wxGetApp().getFrame()->getRocrailIni();
  if( l_RocrailIni != NULL ) {
    iONode digint = wRocRail.getdigint(l_RocrailIni);
    while( digint != NULL ) {
      iONode rocnet = wDigInt.getrocnet(digint);
      if( rocnet != NULL ) {
        m_IID->SetValue( wxString( wDigInt.getiid(digint), wxConvUTF8) );
        m_Digint = digint;
        break;
      }
      digint = wRocRail.nextdigint(l_RocrailIni, digint);
    }
  }

}


void RocnetNodeDlg::initNodeList() {
  m_NodeList->DeleteAllItems();

  if( m_Digint == NULL )
    return;

  int index = 0;
  iONode rnnode = wRocNet.getrocnetnode(m_Digint);
  while( rnnode != NULL ) {
    m_NodeList->InsertItem( index, wxString::Format(_T("%d"), wRocNetNode.getid(rnnode)));
    m_NodeList->SetItem( index, 1, wxString( m_Vendor[wRocNetNode.getvendor(rnnode)&0xFF],wxConvUTF8) );
    m_NodeList->SetItem( index, 2, wxString(wRocNetNode.getclass(rnnode),wxConvUTF8));
    m_NodeList->SetItem( index, 3, wxString(wRocNetNode.getversion(rnnode),wxConvUTF8));
    m_NodeList->SetItem( index, 4, wxString::Format(_T("%d"), wRocNetNode.getnrio(rnnode)));
    m_NodeList->SetItemPtrData(index, (wxUIntPtr)rnnode);
    index++;
    rnnode = wRocNet.nextrocnetnode(m_Digint, rnnode);
  }
  m_NodeList->SetColumnWidth(0, wxLIST_AUTOSIZE);
  m_NodeList->SetColumnWidth(1, wxLIST_AUTOSIZE);
  m_NodeList->SetColumnWidth(2, wxLIST_AUTOSIZE);
  m_NodeList->SetColumnWidth(3, wxLIST_AUTOSIZE);
  m_NodeList->SetColumnWidth(4, wxLIST_AUTOSIZE);

  if(m_NodeList->GetItemCount() > 0 ) {
    m_NodeList->SetItemState(0, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
    m_Props = (iONode)m_NodeList->GetItemData(0);

  }

}

void RocnetNodeDlg::onIndexSelected( wxListEvent& event ) {
  int index = event.GetIndex();
  m_Props = (iONode)m_NodeList->GetItemData(index);
  if( m_Props != NULL ) {
    initValues();
    SetTitle(wxT("RocNetNode: ") + wxString::Format(_T("%d"), wRocNetNode.getid(m_Props) ) );
  }
  else
    TraceOp.trc( "rocnetnode", TRCLEVEL_INFO, __LINE__, 9999, "no selection..." );

}

void RocnetNodeDlg::initValues() {
  m_ID->SetValue( wRocNetNode.getid(m_Props) );
  m_VendorName->SetValue( wxString( m_Vendor[wRocNetNode.getvendor(m_Props)&0xFF],wxConvUTF8) );
  m_ProductName->SetValue( wxString(wRocNetNode.getclass(m_Props),wxConvUTF8) );
  m_Version->SetValue( wxString(wRocNetNode.getversion(m_Props),wxConvUTF8) );
}

void RocnetNodeDlg::event(iONode node) {
  char* s = NodeOp.toEscString(node);
  TraceOp.trc( "rocnetnode", TRCLEVEL_INFO, __LINE__, 9999, "event: \n%s", s );
  StrOp.free(s);
  if( StrOp.equals( wProgram.name(), NodeOp.getName(node)) && wProgram.getlntype(node) == wProgram.lntype_rocnet ) {
    wxSpinCtrl* l_IO[] = { NULL, m_IO1, m_IO2, m_IO3, m_IO4, m_IO5, m_IO6, m_IO7, m_IO8};
    wxRadioBox* l_Type[] = {NULL, m_Type1, m_Type2, m_Type3, m_Type4, m_Type5, m_Type6, m_Type7, m_Type8};
    wxSpinCtrl* l_Delay[] = { NULL, m_Delay1, m_Delay2, m_Delay3, m_Delay4, m_Delay5, m_Delay6, m_Delay7, m_Delay8};

    char key[32] = {'\0'};
    if( wProgram.getcmd(node) == wProgram.nvget ) {
      for( int i = 0; i < 8; i++ ) {
        StrOp.fmtb(key, "val%d", 1 + i*4);
        int port = NodeOp.getInt( node, key, 0);
        StrOp.fmtb(key, "val%d", 2 + i*4);
        int ionr = NodeOp.getInt( node, key, 0);
        StrOp.fmtb(key, "val%d", 3 + i*4);
        int type = NodeOp.getInt( node, key, 0);
        StrOp.fmtb(key, "val%d", 4 + i*4);
        int delay = NodeOp.getInt( node, key, 0);

        if( port > 0 + m_PortGroup*8 && (port-m_PortGroup*8) < 9) {
          TraceOp.trc( "rocnetnode", TRCLEVEL_INFO, __LINE__, 9999, "set ionr[%d]=%d", port, ionr );
          l_IO[port-m_PortGroup*8]->SetValue(ionr);
          TraceOp.trc( "rocnetnode", TRCLEVEL_INFO, __LINE__, 9999, "set delay[%d]=%d", port, delay );
          l_Delay[port-m_PortGroup*8]->SetValue(delay);
          l_Type[port-m_PortGroup*8]->SetSelection(type);
        }
      }
    }
  }
}

