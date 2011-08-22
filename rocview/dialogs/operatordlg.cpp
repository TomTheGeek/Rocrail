/*
 Rocrail - Model Railroad Software

 Copyright (C) Rob Versluis <r.j.versluis@rocrail.net>

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
#include "operatordlg.h"

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/defs.h"
#endif

#include "rocview/public/guiapp.h"

#include "rocview/dialogs/locdialog.h"
#include "rocview/dialogs/cardlg.h"
#include "rocview/dialogs/waybilldlg.h"

#include "rocrail/wrapper/public/ModelCmd.h"
#include "rocrail/wrapper/public/Plan.h"
#include "rocrail/wrapper/public/Item.h"
#include "rocrail/wrapper/public/Car.h"
#include "rocrail/wrapper/public/CarList.h"
#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/LocList.h"
#include "rocrail/wrapper/public/Operator.h"
#include "rocrail/wrapper/public/OperatorList.h"
#include "rocrail/wrapper/public/Block.h"
#include "rocrail/wrapper/public/Location.h"
#include "rocrail/wrapper/public/LocationList.h"
#include "rocrail/wrapper/public/Waybill.h"

#include "rocview/wrapper/public/Gui.h"

#include "rocs/public/trace.h"
#include "rocs/public/strtok.h"
#include "rocs/public/system.h"

#include "rocview/xpm/nopict.xpm"


OperatorDlg::OperatorDlg( wxWindow* parent, iONode p_Props )
  :operatordlggen( parent )
{
  TraceOp.trc( "opdlg", TRCLEVEL_INFO, __LINE__, 9999, "cardlg" );
  m_TabAlign = wxGetApp().getTabAlign();
  m_Props    = p_Props;
  m_bSave    = true;
  initLabels();
  initIndex();

  m_IndexPanel->GetSizer()->Layout();
  m_ControlPanel->GetSizer()->Layout();
  m_ConsistPanel->GetSizer()->Layout();

  m_OperatorBook->Fit();

  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);

  m_OperatorList->SetFocus();

  m_OperatorBook->Connect( wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( OperatorDlg::onSetPage ), NULL, this );
  m_SetPage = 0;

  m_ControlPanel->Enable(false);
  m_ConsistPanel->Enable(false);

  if( m_Props != NULL ) {
    initValues();
    m_SetPage = 1;
  }
  wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, -1 );
  wxPostEvent( m_OperatorBook, event );

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


void OperatorDlg::onSetPage(wxCommandEvent& event) {
  TraceOp.trc( "opdlg", TRCLEVEL_INFO, __LINE__, 9999, "set page to %d", m_SetPage );
  m_OperatorBook->SetSelection( m_SetPage );
}


void OperatorDlg::initLocos() {
  m_LocoID->Clear();

  iONode model = wxGetApp().getModel();
  if( model != NULL ) {
    iONode lclist = wPlan.getlclist( model );
    if( lclist != NULL ) {
      iOList list = ListOp.inst();
      int cnt = NodeOp.getChildCnt( lclist );
      for( int i = 0; i < cnt; i++ ) {
        iONode lc = NodeOp.getChild( lclist, i );
        const char* id = wLoc.getid( lc );
        if( id != NULL ) {
          ListOp.add(list, (obj)lc);
        }
      }

      ListOp.sort(list, &__sortID);
      cnt = ListOp.size( list );
      for( int i = 0; i < cnt; i++ ) {
        iONode lc = (iONode)ListOp.get( list, i );
        const char* id = wLoc.getid( lc );
        m_LocoID->Append( wxString(id,wxConvUTF8), lc );
      }
      /* clean up the temp. list */
      ListOp.base.del(list);

      if( m_Props != NULL ) {
        m_LocoID->SetStringSelection( wxString(wOperator.getlcid( m_Props ),wxConvUTF8) );
        iONode lc = wxGetApp().getFrame()->findLoc(wOperator.getlcid( m_Props ));
        if( lc != NULL ) {
          iONode bk = wxGetApp().getFrame()->findBlock4Loc(wOperator.getlcid( m_Props ), NULL);
          if( bk != NULL ) {
            m_Location->SetStringSelection( wxString(wBlock.getid( bk ),wxConvUTF8) );
          }
          else {
            m_Location->SetStringSelection( wxString(wLoc.getblockid( lc ),wxConvUTF8) );
          }
        }
      }
      else
        TraceOp.trc( "opdlg", TRCLEVEL_INFO, __LINE__, 9999, "no selection" );
    }

  }

  wxCommandEvent event( 0, 0 );
  onLocomotiveCombo(event);

}


static int __sortStr(obj* _a, obj* _b)
{
    const char* a = (const char*)*_a;
    const char* b = (const char*)*_b;
    return strcmp( a, b );
}

void OperatorDlg::initLocationCombo() {
  TraceOp.trc( "opdlg", TRCLEVEL_INFO, __LINE__, 9999, "init location combos" );
  m_Location->Clear();
  m_Location->Append( _T(""), (void*)NULL );
  m_Goto->Clear();
  m_Goto->Append( _T(""), (void*)NULL );

  iONode model = wxGetApp().getModel();
  iOList locations = ListOp.inst();
  iOList gotos     = ListOp.inst();

  if( model != NULL ) {
    iONode locationlist = wPlan.getlocationlist( model );
    if( locationlist != NULL ) {
      int cnt = NodeOp.getChildCnt( locationlist );
      for( int i = 0; i < cnt; i++ ) {
        iONode location = NodeOp.getChild( locationlist, i );
        const char* id = wLocation.getid( location );
        if( id != NULL ) {
          ListOp.add(gotos, (obj)id);
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
          ListOp.add(gotos, (obj)id);
          ListOp.add(locations, (obj)id);
        }
      }
    }

    if( ListOp.size(gotos) > 0 ) {
      ListOp.sort(gotos, &__sortStr);
      int cnt = ListOp.size( gotos );
      for( int i = 0; i < cnt; i++ ) {
        const char* id = (const char*)ListOp.get( gotos, i );
        m_Goto->Append( wxString(id,wxConvUTF8) );
      }
    }
    if( ListOp.size(locations) > 0 ) {
      ListOp.sort(locations, &__sortStr);
      int cnt = ListOp.size( locations );
      for( int i = 0; i < cnt; i++ ) {
        const char* id = (const char*)ListOp.get( locations, i );
        m_Location->Append( wxString(id,wxConvUTF8) );
      }
    }

  }
  /* clean up the temp. list */
  ListOp.base.del(gotos);
  ListOp.base.del(locations);
}


void OperatorDlg::initLabels() {

  initLocationCombo();
}


void OperatorDlg::onOperatorList( wxCommandEvent& event ) {
  if( m_OperatorList->GetSelection() != wxNOT_FOUND ) {
    m_Props = (iONode)m_OperatorList->GetClientData(m_OperatorList->GetSelection());
    if( m_Props != NULL )
      initValues();
    else
      TraceOp.trc( "opdlg", TRCLEVEL_INFO, __LINE__, 9999, "no selection..." );
  }
}


void OperatorDlg::evaluate() {
  if( m_Props == NULL )
    return;

  TraceOp.trc( "opdlg", TRCLEVEL_INFO, __LINE__, 9999, "Evaluate %s", wOperator.getid( m_Props ) );

  // evaluate General
  wItem.setprev_id( m_Props, wItem.getid(m_Props) );
  wOperator.setid( m_Props, m_Operator->GetValue().mb_str(wxConvUTF8) );
  wOperator.setlcid( m_Props, m_LocoID->GetStringSelection().mb_str(wxConvUTF8) );

  int carcnt = m_CarList->GetCount();
  char* consist = NULL;
  TraceOp.trc( "opdlg", TRCLEVEL_INFO, __LINE__, 9999, "carids[%d]", carcnt );
  for( int i = 0; i < carcnt; i++ ) {
    iONode car = (iONode)m_CarList->GetClientData(i);
    TraceOp.trc( "opdlg", TRCLEVEL_INFO, __LINE__, 9999, "adding carid [%s]", wCar.getid(car) );
    if( consist == NULL ) {
      consist = StrOp.cat(consist, wCar.getid(car));
    }
    else {
      consist = StrOp.cat(consist, ",");
      consist = StrOp.cat(consist, wCar.getid(car));
    }
  }
  if( consist == NULL )
    wOperator.setcarids(m_Props, "");
  else
    wOperator.setcarids(m_Props, consist);

  TraceOp.trc( "opdlg", TRCLEVEL_INFO, __LINE__, 9999, "carids: [%s]", wOperator.getcarids(m_Props) );
}


void OperatorDlg::initIndex() {
  TraceOp.trc( "opdlg", TRCLEVEL_INFO, __LINE__, 9999, "initIndex" );
  iONode l_Props = m_Props;

  SetTitle(wxGetApp().getMsg( "operatortable" ));

  m_OperatorList->Clear();

  iONode model = wxGetApp().getModel();
  if( model != NULL ) {
    iONode operatorlist = wPlan.getoperatorlist( model );
    if( operatorlist != NULL ) {
      iOList list = ListOp.inst();
      int cnt = NodeOp.getChildCnt( operatorlist );
      for( int i = 0; i < cnt; i++ ) {
        iONode op = NodeOp.getChild( operatorlist, i );
        const char* id = wOperator.getid( op );
        if( id != NULL ) {
          ListOp.add(list, (obj)op);
        }
      }

      ListOp.sort(list, &__sortID);
      cnt = ListOp.size( list );
      for( int i = 0; i < cnt; i++ ) {
        iONode op = (iONode)ListOp.get( list, i );
        const char* id = wOperator.getid( op );
        m_OperatorList->Append( wxString(id,wxConvUTF8), op );
      }
      /* clean up the temp. list */
      ListOp.base.del(list);

      if( l_Props != NULL ) {
        m_OperatorList->SetStringSelection( wxString(wOperator.getid( l_Props ),wxConvUTF8) );
        m_OperatorList->SetFirstItem( wxString(wOperator.getid( l_Props ),wxConvUTF8) );
        m_Props = l_Props;
        char* title = StrOp.fmt( "%s %s", (const char*)wxGetApp().getMsg("operator").mb_str(wxConvUTF8), wOperator.getid( m_Props ) );
        SetTitle( wxString(title,wxConvUTF8) );
        StrOp.free( title );
      }
      else
        TraceOp.trc( "opdlg", TRCLEVEL_INFO, __LINE__, 9999, "no selection" );
    }

  }


}


void OperatorDlg::initValues() {
  if( m_Props == NULL ) {
    TraceOp.trc( "opdlg", TRCLEVEL_DEBUG, __LINE__, 9999, "no operator selected" );
    return;
  }
  m_ControlPanel->Enable(true);
  m_ConsistPanel->Enable(true);

  char* title = StrOp.fmt( "%s %s", (const char*)wxGetApp().getMsg("operator").mb_str(wxConvUTF8), wOperator.getid( m_Props ) );
  SetTitle( wxString(title,wxConvUTF8) );
  StrOp.free( title );

  TraceOp.trc( "opdlg", TRCLEVEL_INFO, __LINE__, 9999, "initValues for operator [%s]", wOperator.getid( m_Props ) );

  m_Operator->SetValue( wxString(wOperator.getid( m_Props ),wxConvUTF8) );

  initLocos();
  initConsist();
}


void OperatorDlg::onCarList( wxCommandEvent& event ){
  if( m_CarList->GetSelection() != wxNOT_FOUND ) {
    iONode car = (iONode)m_CarList->GetClientData(m_CarList->GetSelection());
    if( car != NULL && wCar.getimage( car ) != NULL ) {
      wxBitmapType bmptype = wxBITMAP_TYPE_XPM;
      if( StrOp.endsWithi( wCar.getimage( car ), ".gif" ) )
        bmptype = wxBITMAP_TYPE_GIF;
      else if( StrOp.endsWithi( wCar.getimage( car ), ".png" ) )
        bmptype = wxBITMAP_TYPE_PNG;

      const char* imagepath = wGui.getimagepath(wxGetApp().getIni());
      static char pixpath[256];
      StrOp.fmtb( pixpath, "%s%c%s", imagepath, SystemOp.getFileSeparator(), FileOp.ripPath( wCar.getimage( car ) ) );

      if( FileOp.exist(pixpath)) {
        TraceOp.trc( "opdlg", TRCLEVEL_INFO, __LINE__, 9999, "picture [%s]", pixpath );
        m_CarImage->SetBitmapLabel( wxBitmap(wxString(pixpath,wxConvUTF8), bmptype) );
      }
      else {
        TraceOp.trc( "opdlg", TRCLEVEL_WARNING, __LINE__, 9999, "picture [%s] not found", pixpath );
        m_CarImage->SetBitmapLabel( wxBitmap(nopict_xpm) );
      }
      m_CarImage->SetToolTip(wxString(wCar.getroadname( car ),wxConvUTF8));


    }
    else {
      m_CarImage->SetBitmapLabel( wxBitmap(nopict_xpm) );
    }
    m_CarImage->Refresh();
  }
}

void OperatorDlg::initConsist() {
  m_CarList->Clear();

  if( m_Props == NULL )
    return;

  const char* carids = wOperator.getcarids(m_Props);
  iOStrTok strtok = StrTokOp.inst( carids, ',' );
  while( StrTokOp.hasMoreTokens( strtok ) ) {
    const char* carid  = StrTokOp.nextToken( strtok );
    iONode car = wxGetApp().getFrame()->findCar( carid );
    if( car != NULL ) {
      const char* id = wCar.getid( car );
      m_CarList->Append( wxString(id,wxConvUTF8) + wxString(_T(": ")) + wxString(wCar.getwaybills(car), wxConvUTF8), car );
    }
  }
}


void OperatorDlg::onNewOperator( wxCommandEvent& event ) {
  int i = m_OperatorList->FindString( _T("NEW") );
  if( i == wxNOT_FOUND ) {
    m_OperatorList->Append( _T("NEW") );
    iONode model = wxGetApp().getModel();
    if( model != NULL ) {
      iONode operatorlist = wPlan.getoperatorlist( model );
      if( operatorlist == NULL ) {
        operatorlist = NodeOp.inst( wOperatorList.name(), model, ELEMENT_NODE );
        NodeOp.addChild( model, operatorlist );
      }
      if( operatorlist != NULL ) {
        iONode op = NodeOp.inst( wOperator.name(), operatorlist, ELEMENT_NODE );
        NodeOp.addChild( operatorlist, op );
        wOperator.setid( op, "NEW" );
        m_Props = op;
        initValues();
      }
    }
  }
  m_OperatorList->SetStringSelection( _T("NEW") );
  m_OperatorList->SetFirstItem( _T("NEW") );
}


void OperatorDlg::onDelOperator( wxCommandEvent& event ) {
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
    iONode operatorlist = wPlan.getoperatorlist( model );
    if( operatorlist != NULL ) {
      NodeOp.removeChild( operatorlist, m_Props );
      m_Props = NULL;
    }
  }

  initIndex();
}


void OperatorDlg::OnCopy( wxCommandEvent& event ){
  if( m_Props != NULL ) {
    iONode model = wxGetApp().getModel();
    if( model != NULL ) {
      iONode operatorlist = wPlan.getoperatorlist( model );
      if( operatorlist == NULL ) {
        operatorlist = NodeOp.inst( wOperatorList.name(), model, ELEMENT_NODE );
        NodeOp.addChild( model, operatorlist );
      }

      if( operatorlist != NULL ) {
        iONode lccopy = (iONode)NodeOp.base.clone( m_Props );
        char* id = StrOp.fmt( "%s (copy)", wOperator.getid(lccopy));
        wOperator.setid(lccopy, id);
        StrOp.free(id);
        NodeOp.addChild( operatorlist, lccopy );
        initIndex();
      }

    }
  }
}


void OperatorDlg::onLocoImage( wxCommandEvent& event ) {
  if( m_LocoID->GetSelection() != wxNOT_FOUND ) {
    iONode lc = (iONode)((wxItemContainer*)m_LocoID)->GetClientData(m_LocoID->GetSelection());
    if( lc != NULL ) {
      LocDialog* dlg = new LocDialog(this, lc, false );
      if( wxID_OK == dlg->ShowModal() ) {
        /* Notify Notebook. */
      }
      dlg->Destroy();
    }
  }
}


void OperatorDlg::onLocomotiveCombo( wxCommandEvent& event ) {
  iONode lc = wxGetApp().getFrame()->findLoc( m_LocoID->GetStringSelection().mb_str(wxConvUTF8) );

  if( lc != NULL && wLoc.getimage( lc ) != NULL && StrOp.len(wLoc.getimage( lc )) > 0  ) {
    wxBitmapType bmptype = wxBITMAP_TYPE_XPM;
    if( StrOp.endsWithi( wLoc.getimage( lc ), ".gif" ) )
      bmptype = wxBITMAP_TYPE_GIF;
    else if( StrOp.endsWithi( wLoc.getimage( lc ), ".png" ) )
      bmptype = wxBITMAP_TYPE_PNG;
    else {
      m_LocoImage->SetBitmapLabel( wxBitmap(nopict_xpm) );
      m_LocoImage->Refresh();
      return;
    }

    const char* imagepath = wGui.getimagepath(wxGetApp().getIni());
    static char pixpath[256];
    StrOp.fmtb( pixpath, "%s%c%s", imagepath, SystemOp.getFileSeparator(), FileOp.ripPath( wLoc.getimage( lc ) ) );

    if( FileOp.exist(pixpath)) {
      TraceOp.trc( "opdlg", TRCLEVEL_INFO, __LINE__, 9999, "picture [%s]", pixpath );
      m_LocoImage->SetBitmapLabel( wxBitmap(wxString(pixpath,wxConvUTF8), bmptype) );
    }
    else {
      TraceOp.trc( "opdlg", TRCLEVEL_WARNING, __LINE__, 9999, "picture [%s] not found", pixpath );
      m_LocoImage->SetBitmapLabel( wxBitmap(nopict_xpm) );
    }
    m_LocoImage->SetToolTip(wxString(wLoc.getdesc( m_Props ),wxConvUTF8));


  }
  else {
    m_LocoImage->SetBitmapLabel( wxBitmap(nopict_xpm) );
  }
  m_LocoImage->Refresh();

}


void OperatorDlg::onReserve( wxCommandEvent& event ) {
  /* Inform RocRail... */
  iONode cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
  wLoc.setid( cmd, wOperator.getlcid( m_Props ) );
  wLoc.setcmd( cmd, wLoc.block );
  wLoc.setblockid( cmd, m_Location->GetStringSelection().mb_str(wxConvUTF8) );
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);

}


void OperatorDlg::onRun( wxCommandEvent& event ) {
  iONode cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
  wLoc.setid( cmd, wOperator.getlcid( m_Props ) );
  wLoc.setcmd( cmd, wLoc.gotoblock );
  wLoc.setblockid( cmd, m_Goto->GetStringSelection().mb_str(wxConvUTF8) );
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);

  cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
  wLoc.setid( cmd, wOperator.getlcid( m_Props ) );
  wLoc.setcmd( cmd, wLoc.go );
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);
}


void OperatorDlg::onGotoMan( wxCommandEvent& event ) {
  iONode cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
  wLoc.setid( cmd, wOperator.getlcid( m_Props ) );
  wLoc.setcmd( cmd, wLoc.gotoblock );
  wLoc.setblockid( cmd, m_Goto->GetStringSelection().mb_str(wxConvUTF8) );
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);

  cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
  wLoc.setid( cmd, wOperator.getlcid( m_Props ) );
  wLoc.setcmd( cmd, wLoc.gomanual );
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);
}


void OperatorDlg::onCarImage( wxCommandEvent& event ) {
  onCarCard(event);
}


void OperatorDlg::onAddCar( wxCommandEvent& event ) {
  CarDlg* dlg = new CarDlg(this, NULL, false );
  if( wxID_OK == dlg->ShowModal() ) {
    /* Notify Notebook. */
    iONode car = dlg->getSelectedCar();
    if( car != NULL ) {
      const char* id = wCar.getid( car );
      m_CarList->Append( wxString(id,wxConvUTF8) + wxString(_T(": ")) + wxString(wCar.getwaybills(car), wxConvUTF8), car );
    }
  }
  dlg->Destroy();

}


void OperatorDlg::onLeaveCar( wxCommandEvent& event ) {
  if( m_CarList->GetSelection() != wxNOT_FOUND ) {
    int action = wxMessageDialog( this, wxGetApp().getMsg("removewarning"), _T("Rocrail"), wxYES_NO | wxICON_EXCLAMATION ).ShowModal();
    if( action == wxID_NO )
      return;
    m_CarList->Delete(m_CarList->GetSelection());
    m_CarImage->SetBitmapLabel( wxBitmap(nopict_xpm) );
    m_CarImage->Refresh();
    evaluate();
    initConsist();
  }
}


void OperatorDlg::onCarCard( wxCommandEvent& event ) {
  if( m_CarList->GetSelection() != wxNOT_FOUND ) {
    iONode car = (iONode)m_CarList->GetClientData(m_CarList->GetSelection());
    if( car != NULL ) {
      CarDlg* dlg = new CarDlg(this, car, false );
      if( wxID_OK == dlg->ShowModal() ) {
        /* Notify Notebook. */
      }
      dlg->Destroy();
    }
  }
}


void OperatorDlg::onWayBill( wxCommandEvent& event ) {
  if( m_CarList->GetSelection() != wxNOT_FOUND ) {
    iONode car = (iONode)m_CarList->GetClientData(m_CarList->GetSelection());
    if( car != NULL ) {
      iONode waybill = NULL;
      const char* waybills = wCar.getwaybills(car);

      if( waybills != NULL && StrOp.len(waybills) > 0 ) {
        /* TODO: get waybill node for the first entry in the list */
        iOStrTok strtok = StrTokOp.inst( waybills, ',' );
        while( StrTokOp.hasMoreTokens( strtok ) ) {
          const char* billid  = StrTokOp.nextToken( strtok );
          waybill = wxGetApp().getFrame()->findWaybill( billid );
          break;
        }
      }

      WaybillDlg* dlg = new WaybillDlg(this, waybill, false );
      if( wxID_OK == dlg->ShowModal() ) {
        /* Notify Notebook. */
        iONode waybill = dlg->getSelectedWaybill();
        if( waybill != NULL ) {
          wCar.setwaybills( car, wWaybill.getid(waybill) );
          initConsist();
        }
      }
      dlg->Destroy();
    }
  }

}


void OperatorDlg::onApply( wxCommandEvent& event ) {
  if( m_Props == NULL )
    return;

  evaluate();
  if( !wxGetApp().isStayOffline() ) {
    /* Notify RocRail. */
    iONode cmd = NodeOp.inst( wModelCmd.name(), NULL, ELEMENT_NODE );
    wModelCmd.setcmd( cmd, wModelCmd.modify );
    NodeOp.addChild( cmd, (iONode)m_Props->base.clone( m_Props ) );
    wxGetApp().sendToRocrail( cmd );
    cmd->base.del(cmd);
  }
  initIndex();
}


void OperatorDlg::onCancel( wxCommandEvent& event ) {
  //EndModal( 0 );
  Destroy();
}


void OperatorDlg::onOK( wxCommandEvent& event ) {
  if( m_bSave )
    onApply(event);
  //EndModal( wxID_OK );
  Destroy();
}

