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
#include "waybilldlg.h"

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/defs.h"
#endif

#include "rocview/public/guiapp.h"
#include "rocrail/wrapper/public/ModelCmd.h"
#include "rocrail/wrapper/public/Plan.h"
#include "rocrail/wrapper/public/Waybill.h"
#include "rocrail/wrapper/public/WaybillList.h"
#include "rocrail/wrapper/public/Item.h"
#include "rocrail/wrapper/public/Location.h"
#include "rocrail/wrapper/public/LocationList.h"
#include "rocrail/wrapper/public/Block.h"
#include "rocrail/wrapper/public/Car.h"

#include "rocview/wrapper/public/Gui.h"

#include "rocs/public/trace.h"
#include "rocs/public/system.h"


WaybillDlg::WaybillDlg( wxWindow* parent, iONode p_Props, bool save )
  :waybillgen( parent )
{
  TraceOp.trc( "waybilldlg", TRCLEVEL_INFO, __LINE__, 9999, "waybilldlg" );
  m_TabAlign = wxGetApp().getTabAlign();
  m_Props    = p_Props;
  m_bSave    = save;
  initLabels();
  initIndex();

  m_IndexPanel->GetSizer()->Layout();
  m_GeneralPanel->GetSizer()->Layout();
  m_RoutingPanel->GetSizer()->Layout();
  m_RoutingPanel->Show(false);

  m_WaybillBook->Fit();

  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);

  m_WaybillList->SetFocus();

  m_WaybillBook->Connect( wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( WaybillDlg::onSetPage ), NULL, this );
  m_SetPage = 0;

  if( m_Props != NULL ) {
    initValues();
    m_SetPage = 1;
  }
  wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED );
  wxPostEvent( m_WaybillBook, event );

}

void WaybillDlg::onSetPage(wxCommandEvent& event) {
  TraceOp.trc( "waybilldlg", TRCLEVEL_INFO, __LINE__, 9999, "set page to %d", m_SetPage );
  m_WaybillBook->SetSelection( m_SetPage );
}

static int __sortStr(obj* _a, obj* _b)
{
    const char* a = (const char*)*_a;
    const char* b = (const char*)*_b;
    return strcmp( a, b );
}

void WaybillDlg::initLocationCombo() {
  m_Origin->Clear();
  m_Origin->Append( _T(""), (void*)NULL );
  m_Destination->Clear();
  m_Destination->Append( _T(""), (void*)NULL );

  iONode model = wxGetApp().getModel();
  iOList list = ListOp.inst();

  if( model != NULL ) {
    iONode locationlist = wPlan.getlocationlist( model );
    if( locationlist != NULL ) {
      int cnt = NodeOp.getChildCnt( locationlist );
      for( int i = 0; i < cnt; i++ ) {
        iONode location = NodeOp.getChild( locationlist, i );
        const char* id = wLocation.getid( location );
        if( id != NULL ) {
          ListOp.add(list, (obj)id);
        }
      }
    }

    iONode bklist = wPlan.getbklist( model );
    if( bklist != NULL ) {
      int cnt = NodeOp.getChildCnt( bklist );
      for( int i = 0; i < cnt; i++ ) {
        iONode bk = NodeOp.getChild( bklist, i );
        const char* id = wBlock.getid( bk );
        if( id != NULL ) {
          ListOp.add(list, (obj)id);
        }
      }
    }

    if( ListOp.size(list) > 0 ) {
      ListOp.sort(list, &__sortStr);
      int cnt = ListOp.size( list );
      for( int i = 0; i < cnt; i++ ) {
        const char* id = (const char*)ListOp.get( list, i );
        m_Origin->Append( wxString(id,wxConvUTF8) );
        m_Destination->Append( wxString(id,wxConvUTF8) );
      }
    }
  }
  /* clean up the temp. list */
  ListOp.base.del(list);
}


void WaybillDlg::initLabels() {
  TraceOp.trc( "waybilldlg", TRCLEVEL_INFO, __LINE__, 9999, "initLabels" );
  SetTitle(wxGetApp().getMsg( "waybilltable" ));
  m_WaybillBook->SetPageText( 0, wxGetApp().getMsg( "index" ) );
  m_WaybillBook->SetPageText( 1, wxGetApp().getMsg( "general" ) );
  m_WaybillBook->SetPageText( 2, wxGetApp().getMsg( "routing" ) );

  // Index
  m_NewWaybill->SetLabel( wxGetApp().getMsg( "new" ) );
  m_DeleteWaybill->SetLabel( wxGetApp().getMsg( "delete" ) );
  m_Copy->SetLabel( wxGetApp().getMsg( "copy" ) );

  // General
  m_labID->SetLabel( wxGetApp().getMsg( "id" ) );
  m_labShipper->SetLabel( wxGetApp().getMsg( "shipper" ) );
  m_labOrigin->SetLabel( wxGetApp().getMsg( "origin" ) );
  m_labConsignee->SetLabel( wxGetApp().getMsg( "consignee" ) );
  m_labDestination->SetLabel( wxGetApp().getMsg( "destination" ) );
  m_labCommodity->SetLabel( wxGetApp().getMsg( "commodity" ) );
  m_Status->SetLabel( wxGetApp().getMsg( "status" ) );
  m_Status->SetString( 0, wxGetApp().getMsg( "waiting" ) );
  m_Status->SetString( 1, wxGetApp().getMsg( "shipping" ) );
  m_Status->SetString( 2, wxGetApp().getMsg( "delivered" ) );
  m_labCartype->SetLabel( wxGetApp().getMsg( "cartype" ) );
  initLocationCombo();

  m_Cartype->Append( wxGetApp().getMsg( wCar.freight_boxcar ), (void*)wCar.freight_boxcar );
  m_Cartype->Append( wxGetApp().getMsg( wCar.freight_gondola ), (void*)wCar.freight_gondola );
  m_Cartype->Append( wxGetApp().getMsg( wCar.freight_flatcar ), (void*)wCar.freight_flatcar );
  m_Cartype->Append( wxGetApp().getMsg( wCar.freight_reefer ), (void*)wCar.freight_reefer );
  m_Cartype->Append( wxGetApp().getMsg( wCar.freight_stockcar ), (void*)wCar.freight_stockcar );
  m_Cartype->Append( wxGetApp().getMsg( wCar.freight_tankcar ), (void*)wCar.freight_tankcar );
  m_Cartype->Append( wxGetApp().getMsg( wCar.freight_wellcar ), (void*)wCar.freight_wellcar );
  m_Cartype->Append( wxGetApp().getMsg( wCar.freight_hopper ), (void*)wCar.freight_hopper );
  m_Cartype->Append( wxGetApp().getMsg( wCar.freight_caboose ), (void*)wCar.freight_caboose );
  m_Cartype->Append( wxGetApp().getMsg( wCar.freight_autorack ), (void*)wCar.freight_autorack );
  m_Cartype->Append( wxGetApp().getMsg( wCar.freight_autocarrier ), (void*)wCar.freight_autocarrier );
  m_Cartype->Append( wxGetApp().getMsg( wCar.freight_logdumpcar ), (void*)wCar.freight_logdumpcar );
  m_Cartype->Append( wxGetApp().getMsg( wCar.freight_coilcar ), (void*)wCar.freight_coilcar );


  // Buttons
  m_stdButtonOK->SetLabel( wxGetApp().getMsg( "ok" ) );
  m_stdButtonCancel->SetLabel( wxGetApp().getMsg( "cancel" ) );
  m_stdButtonApply->SetLabel( wxGetApp().getMsg( "apply" ) );
  m_stdButtonApply->Enable( m_bSave );
}


/* comparator for sorting by id: */
static int __sortID(obj* _a, obj* _b)
{
    iONode a = (iONode)*_a;
    iONode b = (iONode)*_b;
    const char* idA = wItem.getid( a );
    const char* idB = wItem.getid( b );
    return strcmp( idA, idB );
}


void WaybillDlg::initIndex(){
  TraceOp.trc( "waybilldlg", TRCLEVEL_INFO, __LINE__, 9999, "initIndex" );
  SetTitle(wxGetApp().getMsg( "waybilltable" ));

  m_WaybillList->Clear();

  iONode model = wxGetApp().getModel();
  if( model != NULL ) {
    iONode waybilllist = wPlan.getwaybilllist( model );
    if( waybilllist != NULL ) {
      iOList list = ListOp.inst();
      int cnt = NodeOp.getChildCnt( waybilllist );
      for( int i = 0; i < cnt; i++ ) {
        iONode waybill = NodeOp.getChild( waybilllist, i );
        const char* id = wWaybill.getid( waybill );
        if( id != NULL ) {
          ListOp.add(list, (obj)waybill);
        }
      }

      ListOp.sort(list, &__sortID);
      cnt = ListOp.size( list );
      for( int i = 0; i < cnt; i++ ) {
        iONode waybill = (iONode)ListOp.get( list, i );
        const char* id = wWaybill.getid( waybill );
        m_WaybillList->Append( wxString(id,wxConvUTF8), waybill );
      }
      /* clean up the temp. list */
      ListOp.base.del(list);

      if( m_Props != NULL ) {
        m_WaybillList->SetStringSelection( wxString(wWaybill.getid( m_Props ),wxConvUTF8) );
        m_WaybillList->SetFirstItem( wxString(wWaybill.getid( m_Props ),wxConvUTF8) );
        char* title = StrOp.fmt( "%s %s", (const char*)wxGetApp().getMsg("waybill").mb_str(wxConvUTF8), wWaybill.getid( m_Props ) );
        SetTitle( wxString(title,wxConvUTF8) );
        StrOp.free( title );
      }
      else
        TraceOp.trc( "waybilldlg", TRCLEVEL_INFO, __LINE__, 9999, "no selection" );
    }

  }

}



void WaybillDlg::initValues() {
  if( m_Props == NULL ) {
    TraceOp.trc( "waybilldlg", TRCLEVEL_DEBUG, __LINE__, 9999, "no waybill selected" );
    return;
  }

  char* title = StrOp.fmt( "%s %s", (const char*)wxGetApp().getMsg("waybill").mb_str(wxConvUTF8), wWaybill.getid( m_Props ) );
  SetTitle( wxString(title,wxConvUTF8) );
  StrOp.free( title );

  TraceOp.trc( "waybilldlg", TRCLEVEL_INFO, __LINE__, 9999, "initValues for waybill [%s]", wWaybill.getid( m_Props ) );

  // init General
  m_ID->SetValue( wxString(wWaybill.getid( m_Props ),wxConvUTF8) );
  m_Shipper->SetValue( wxString(wWaybill.getshipper( m_Props ),wxConvUTF8) );
  m_Origin->SetStringSelection( wxString(wWaybill.getorigin( m_Props ),wxConvUTF8) );
  m_Consignee->SetValue( wxString(wWaybill.getconsignee( m_Props ),wxConvUTF8) );
  m_Destination->SetStringSelection( wxString(wWaybill.getdestination( m_Props ),wxConvUTF8) );
  m_Commodity->SetValue( wxString(wWaybill.getcommodity( m_Props ),wxConvUTF8) );

  if( StrOp.len( wWaybill.getcartype( m_Props ) ) > 0 )
    m_Cartype->SetStringSelection( wxGetApp().getMsg( wWaybill.getcartype( m_Props ) ) );

  // init Status
  if( StrOp.equals( wWaybill.status_waiting, wWaybill.getstatus( m_Props) ) )
    m_Status->SetSelection(0);
  else if( StrOp.equals( wWaybill.status_shipping, wWaybill.getstatus( m_Props) ) )
    m_Status->SetSelection(1);
  else
    m_Status->SetSelection(2);

  m_Routing->SetValue(wxString(wWaybill.getrouting( m_Props ),wxConvUTF8));

}


bool WaybillDlg::evaluate(){
  if( m_Props == NULL )
    return false;

  if( m_ID->GetValue().Len() == 0 ) {
    wxMessageDialog( this, wxGetApp().getMsg("invalidid"), _T("Rocrail"), wxOK | wxICON_ERROR ).ShowModal();
    m_ID->SetValue( wxString(wWaybill.getid( m_Props ),wxConvUTF8) );
    return false;
  }


  TraceOp.trc( "waybilldlg", TRCLEVEL_INFO, __LINE__, 9999, "Evaluate %s", wWaybill.getid( m_Props ) );

  // evaluate General
  wItem.setprev_id( m_Props, wItem.getid(m_Props) );
  wWaybill.setid( m_Props, m_ID->GetValue().mb_str(wxConvUTF8) );

  wWaybill.setshipper( m_Props, m_Shipper->GetValue().mb_str(wxConvUTF8) );
  wWaybill.setorigin( m_Props, m_Origin->GetStringSelection().mb_str(wxConvUTF8) );
  wWaybill.setconsignee( m_Props, m_Consignee->GetValue().mb_str(wxConvUTF8) );
  wWaybill.setdestination( m_Props, m_Destination->GetStringSelection().mb_str(wxConvUTF8) );
  wWaybill.setcommodity( m_Props, m_Commodity->GetValue().mb_str(wxConvUTF8) );
  if( m_Cartype->GetSelection() != wxNOT_FOUND )
    wWaybill.setcartype( m_Props, (char*)((wxItemContainer*)m_Cartype)->GetClientData( m_Cartype->GetSelection()) );


  if( m_Status->GetSelection() == 0 )
    wWaybill.setstatus( m_Props, wWaybill.status_waiting );
  else if( m_Status->GetSelection() == 1 )
    wWaybill.setstatus( m_Props, wWaybill.status_shipping );
  else
    wWaybill.setstatus( m_Props, wWaybill.status_delivered );

  wWaybill.setrouting( m_Props, m_Routing->GetValue().mb_str(wxConvUTF8));

  return true;
}


void WaybillDlg::onWaybillList( wxCommandEvent& event ){
  if( m_WaybillList->GetSelection() != wxNOT_FOUND ) {
    m_Props = (iONode)m_WaybillList->GetClientData(m_WaybillList->GetSelection());
    if( m_Props != NULL )
      initValues();
    else
      TraceOp.trc( "waybilldlg", TRCLEVEL_INFO, __LINE__, 9999, "no selection..." );
  }
}


iONode WaybillDlg::getSelectedWaybill() {
  return m_Props;
}



void WaybillDlg::onNewWaybill( wxCommandEvent& event ){
  int i = m_WaybillList->FindString( _T("NEW") );
  if( i == wxNOT_FOUND ) {
    m_WaybillList->Append( _T("NEW") );
    iONode model = wxGetApp().getModel();
    if( model != NULL ) {
      iONode waybilllist = wPlan.getwaybilllist( model );
      if( waybilllist == NULL ) {
        waybilllist = NodeOp.inst( wWaybillList.name(), model, ELEMENT_NODE );
        NodeOp.addChild( model, waybilllist );
      }
      if( waybilllist != NULL ) {
        iONode waybill = NodeOp.inst( wWaybill.name(), waybilllist, ELEMENT_NODE );
        NodeOp.addChild( waybilllist, waybill );
        wWaybill.setid( waybill, "NEW" );
        m_Props = waybill;
        initValues();
      }
    }
  }
  m_WaybillList->SetStringSelection( _T("NEW") );
  m_WaybillList->SetFirstItem( _T("NEW") );
}


void WaybillDlg::OnCopy( wxCommandEvent& event ){
  if( m_Props != NULL ) {
    iONode model = wxGetApp().getModel();
    if( model != NULL ) {
      iONode waybilllist = wPlan.getwaybilllist( model );
      if( waybilllist == NULL ) {
        waybilllist = NodeOp.inst( wWaybillList.name(), model, ELEMENT_NODE );
        NodeOp.addChild( model, waybilllist );
      }

      if( waybilllist != NULL ) {
        iONode lccopy = (iONode)NodeOp.base.clone( m_Props );
        char* id = StrOp.fmt( "%s (copy)", wWaybill.getid(lccopy));
        wWaybill.setid(lccopy, id);
        StrOp.free(id);
        NodeOp.addChild( waybilllist, lccopy );
        initIndex();
      }

    }
  }
}


void WaybillDlg::onDeleteWaybill( wxCommandEvent& event ){
  if( m_Props == NULL )
    return;

  int action = wxMessageDialog( this, wxGetApp().getMsg("removewarning"), _T("Rocrail"), wxYES_NO | wxICON_EXCLAMATION ).ShowModal();
  if( action == wxID_NO )
    return;

  wxGetApp().pushUndoItem( (iONode)NodeOp.base.clone( m_Props ) );
  /* Notify RocRail. */
  iONode cmd = NodeOp.inst( wModelCmd.name(), NULL, ELEMENT_NODE );
  wModelCmd.setcmd( cmd, wModelCmd.remove );
  NodeOp.addChild( cmd, (iONode)m_Props->base.clone( m_Props ) );
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);

  iONode model = wxGetApp().getModel();
  if( model != NULL ) {
    iONode waybilllist = wPlan.getwaybilllist( model );
    if( waybilllist != NULL ) {
      NodeOp.removeChild( waybilllist, m_Props );
      m_Props = NULL;
    }
  }

  initIndex();
}


void WaybillDlg::onApply( wxCommandEvent& event ){
  if( m_Props == NULL )
    return;

  if( !evaluate() )
    return;
  if( !wxGetApp().isStayOffline() ) {
    /* Notify RocRail. */
    iONode cmd = NodeOp.inst( wModelCmd.name(), NULL, ELEMENT_NODE );
    wModelCmd.setcmd( cmd, wModelCmd.modify );
    NodeOp.addChild( cmd, (iONode)m_Props->base.clone( m_Props ) );
    wxGetApp().sendToRocrail( cmd );
    cmd->base.del(cmd);
  }
  else {
    wxGetApp().setLocalModelModified(true);
  }
  initIndex();
}


void WaybillDlg::onCancel( wxCommandEvent& event ){
  EndModal( 0 );
}


void WaybillDlg::onOK( wxCommandEvent& event ){
  if( m_bSave )
    onApply(event);
  EndModal( wxID_OK );
}



