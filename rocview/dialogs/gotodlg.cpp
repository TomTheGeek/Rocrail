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
#include "gotodlg.h"

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/defs.h"
#endif



#include "rocrail/wrapper/public/Plan.h"
#include "rocrail/wrapper/public/Block.h"
#include "rocrail/wrapper/public/SelTab.h"
#include "rocrail/wrapper/public/Turntable.h"
#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/Item.h"

#include "rocview/public/guiapp.h"
#include "rocs/public/strtok.h"



GotoDlg::GotoDlg( wxWindow* parent, const char* lcid ):gotodlggen( parent )
{
  SetTitle(wxGetApp().getMsg( "gotoblock" ));

  // Buttons
  m_stdButtonsOK->SetLabel( wxGetApp().getMsg( "ok" ) );
  m_stdButtonsCancel->SetLabel( wxGetApp().getMsg( "cancel" ) );
  m_Blocks->SetLabel( wxGetApp().getMsg( "blocks" ) );
  m_Stages->SetLabel( wxGetApp().getMsg( "stagingblock" ) );
  m_FiddleYards->SetLabel( wxGetApp().getMsg( "seltab" ) );
  m_Turntables->SetLabel( wxGetApp().getMsg( "turntable" ) );

  m_LocoID = lcid;
  m_Props = NULL;
  initList(m_List, this, false, false, true);
  m_Blocks->SetValue(true);
  m_Stages->SetValue(true);
  m_FiddleYards->SetValue(true);
  m_Turntables->SetValue(true);
  initIndex();
}

void GotoDlg::initIndex() {
  iONode list = NodeOp.inst("list", NULL, ELEMENT_NODE);
  iONode model = wxGetApp().getModel();
  if( model != NULL ) {
    iONode bklist = wPlan.getbklist( model );
    if( m_Blocks->IsChecked() && bklist != NULL ) {
      int cnt = NodeOp.getChildCnt(bklist);
      for( int i = 0; i < cnt; i++ )
        NodeOp.addChild(list, NodeOp.getChild(bklist, i));
    }
    iONode sblist = wPlan.getsblist( model );
    if( m_Stages->IsChecked() && sblist != NULL ) {
      int cnt = NodeOp.getChildCnt(sblist);
      for( int i = 0; i < cnt; i++ )
        NodeOp.addChild(list, NodeOp.getChild(sblist, i));
    }
    iONode fylist = wPlan.getseltablist( model );
    if( m_FiddleYards->IsChecked() && fylist != NULL ) {
      int cnt = NodeOp.getChildCnt(fylist);
      for( int i = 0; i < cnt; i++ ) {
        iONode fy = NodeOp.getChild(fylist, i);
        if( wSelTab.ismanager(fy) )
          NodeOp.addChild(list, fy);
      }
    }
    iONode ttlist = wPlan.getttlist( model );
    if( m_Turntables->IsChecked() && ttlist != NULL ) {
      int cnt = NodeOp.getChildCnt(ttlist);
      for( int i = 0; i < cnt; i++ ) {
        iONode tt = NodeOp.getChild(ttlist, i);
        if( wTurntable.isembeddedblock(tt) )
          NodeOp.addChild(list, tt);
      }
    }
  }
  fillIndex(list);
}

void GotoDlg::onColClick( wxListEvent& event )
{
  sortOnColumn(event.GetColumn());
}

void GotoDlg::onSelected( wxListEvent& event )
{
  m_Props = getSelection(event.GetIndex());
}

void GotoDlg::onBlocks( wxCommandEvent& event )
{
  initIndex();
}

void GotoDlg::onStages( wxCommandEvent& event )
{
  initIndex();
}

void GotoDlg::onFYs( wxCommandEvent& event )
{
  initIndex();
}

void GotoDlg::onTTs( wxCommandEvent& event )
{
  initIndex();
}

void GotoDlg::onCancel( wxCommandEvent& event )
{
  EndModal( 0 );
}

void GotoDlg::onOK( wxCommandEvent& event )
{
  EndModal( wxID_OK );
}


void GotoDlg::onAdd2Trip( wxCommandEvent& event ) {
  if( m_Props != NULL && m_LocoID != NULL) {
    /* add block to trip */
    iONode cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
    wLoc.setid( cmd, m_LocoID );
    wLoc.setcmd( cmd, wLoc.addblock2trip );
    wLoc.setblockid( cmd, wItem.getid( m_Props ) );
    wxGetApp().sendToRocrail( cmd );
    cmd->base.del(cmd);
  }
}

