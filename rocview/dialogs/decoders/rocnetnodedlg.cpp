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

void RocnetNodeDlg::onPortPrev( wxCommandEvent& event )
{
// TODO: Implement onPortPrev
}

void RocnetNodeDlg::onPortNext( wxCommandEvent& event )
{
// TODO: Implement onPortNext
}

void RocnetNodeDlg::onPortRead( wxCommandEvent& event )
{
// TODO: Implement onPortRead
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

