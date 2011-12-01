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
#include "toursdlg.h"

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/defs.h"
#endif

#include "rocview/public/guiapp.h"

#include "rocrail/wrapper/public/ModelCmd.h"
#include "rocrail/wrapper/public/Plan.h"
#include "rocrail/wrapper/public/Item.h"
#include "rocrail/wrapper/public/Tour.h"
#include "rocrail/wrapper/public/TourList.h"
#include "rocrail/wrapper/public/Schedule.h"
#include "rocrail/wrapper/public/ScheduleEntry.h"
#include "rocrail/wrapper/public/Location.h"

#include "rocs/public/trace.h"
#include "rocs/public/list.h"
#include "rocs/public/str.h"
#include "rocs/public/strtok.h"

ToursDlg::ToursDlg( wxWindow* parent )
  :toursdlggen( parent )
{
  m_Props = NULL;
  m_bSave = true;
}


ToursDlg::ToursDlg( wxWindow* parent, iONode tour, bool save, const char* startblock )
  :toursdlggen( parent )
{
  m_Props = tour;
  m_bSave = save;
  m_StartBlock = startblock;
  m_StartBlockID->SetValue((startblock==NULL)?_T(""):wxString(startblock,wxConvUTF8));

  initLabels();
  initIndex();
  initScheduleCombo();

  m_IndexPanel->GetSizer()->Layout();
  m_SchedulesPanel->GetSizer()->Layout();

  m_TourBook->Fit();

  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);

  m_TourList->SetFocus();

  if( m_Props != NULL ) {
    initValues();
  }

}



void ToursDlg::initLabels() {
  TraceOp.trc( "tourdlg", TRCLEVEL_INFO, __LINE__, 9999, "initLabels" );
  SetTitle(wxGetApp().getMsg( "tourtable" ));
  m_TourBook->SetPageText( 0, wxGetApp().getMsg( "index" ) );
  m_TourBook->SetPageText( 1, wxGetApp().getMsg( "scheduletable" ) );

  // Index
  m_labTourID->SetLabel( wxGetApp().getMsg( "id" ) );
  m_AddTour->SetLabel( wxGetApp().getMsg( "new" ) );
  m_ModifyTour->SetLabel( wxGetApp().getMsg( "modify" ) );
  m_DeleteTour->SetLabel( wxGetApp().getMsg( "delete" ) );
  m_labStartBlock->SetLabel( wxGetApp().getMsg( "startblock" ) );
  m_ShowAll->SetLabel( wxGetApp().getMsg( "showall" ) );

  // Schedules
  m_EntryAdd->SetLabel( wxGetApp().getMsg( "add" ) );
  m_EntryDelete->SetLabel( wxGetApp().getMsg( "delete" ) );

  // Buttons
  m_DefaultButtonsOK->SetLabel( wxGetApp().getMsg( "ok" ) );
  m_DefaultButtonsCancel->SetLabel( wxGetApp().getMsg( "cancel" ) );
  m_DefaultButtonsApply->SetLabel( wxGetApp().getMsg( "apply" ) );
  m_DefaultButtonsApply->Enable( m_bSave );

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


void ToursDlg::initIndex() {
  TraceOp.trc( "tourdlg", TRCLEVEL_INFO, __LINE__, 9999, "initIndex" );
  iONode l_Props = m_Props;

  m_TourList->Clear();

  iONode model = wxGetApp().getModel();
  if( model != NULL ) {
    iONode tourlist = wPlan.gettourlist( model );
    if( tourlist != NULL ) {
      iOList list = ListOp.inst();
      int cnt = NodeOp.getChildCnt( tourlist );
      for( int i = 0; i < cnt; i++ ) {
        iONode tour = NodeOp.getChild( tourlist, i );
        const char* id = wTour.getid( tour );
        if( id != NULL ) {
          ListOp.add(list, (obj)tour);
        }
      }

      ListOp.sort(list, &__sortID);
      cnt = ListOp.size( list );
      for( int i = 0; i < cnt; i++ ) {
        iONode tour = (iONode)ListOp.get( list, i );
        const char* id = wTour.getid( tour );
        if( m_StartBlock == NULL || m_ShowAll->IsChecked() || isFirst(tour, m_StartBlock) )
          m_TourList->Append( wxString(id,wxConvUTF8), tour );
      }
      /* clean up the temp. list */
      ListOp.base.del(list);

      if( l_Props != NULL ) {
        m_TourList->SetStringSelection( wxString(wTour.getid( l_Props ),wxConvUTF8) );
        m_TourList->SetFirstItem( wxString(wTour.getid( l_Props ),wxConvUTF8) );
        m_Props = l_Props;
        char* title = StrOp.fmt( "%s %s", (const char*)wxGetApp().getMsg("tour").mb_str(wxConvUTF8), wTour.getid( m_Props ) );
        SetTitle( wxString(title,wxConvUTF8) );
        StrOp.free( title );
      }
      else
        TraceOp.trc( "tourdlg", TRCLEVEL_INFO, __LINE__, 9999, "no selection" );
    }

  }

}

static int __sortStr(obj* _a, obj* _b)
{
    const char* a = (const char*)*_a;
    const char* b = (const char*)*_b;
    return strcmp( a, b );
}

void ToursDlg::initScheduleCombo() {
  m_ScheduleList->Clear();
  m_ScheduleList->Append( _T(""), (void*)NULL );

  iONode model = wxGetApp().getModel();
  iOList list = ListOp.inst();

  if( model != NULL ) {
    iONode sclist = wPlan.getsclist( model );
    if( sclist != NULL ) {
      int cnt = NodeOp.getChildCnt( sclist );
      for( int i = 0; i < cnt; i++ ) {
        iONode sc = NodeOp.getChild( sclist, i );
        const char* id = wSchedule.getid( sc );
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
        m_ScheduleList->Append( wxString(id,wxConvUTF8) );
      }
    }
  }
  /* clean up the temp. list */
  ListOp.base.del(list);
}

bool ToursDlg::isFirst(iONode tour, const char* startblock) {
  bool isFirst = false;
  TraceOp.trc( "tourdlg", TRCLEVEL_INFO, __LINE__, 9999, "check if block %s is first in tour...", startblock );

  iONode model = wxGetApp().getModel();

  if( model != NULL ) {

    /* check if the block is in a location */
    const char* locationID = NULL;
    iONode locationList = wPlan.getlocationlist(model);
    if( locationList != NULL ) {
      int cnt = NodeOp.getChildCnt( locationList );
      TraceOp.trc( "tourdlg", TRCLEVEL_INFO, __LINE__, 9999, "%d locations...", cnt );
      for( int i = 0; i < cnt; i++ ) {
        iONode location = NodeOp.getChild( locationList, i );

        iOStrTok blocks = StrTokOp.inst( wLocation.getblocks( location ), ',' );
        const char* id = NULL;
        while( StrTokOp.hasMoreTokens( blocks ) ) {
          id = StrTokOp.nextToken( blocks );
          TraceOp.trc( "tourdlg", TRCLEVEL_INFO, __LINE__, 9999, "check if block [%s] is member location [%s]...", id, wLocation.getid( location ) );
          if( StrOp.equals( id, startblock) ) {
            TraceOp.trc( "tourdlg", TRCLEVEL_INFO, __LINE__, 9999, "block [%s] is member of location [%s]", id, wLocation.getid( location ) );
            locationID = wLocation.getid( location );
            i = cnt;
            break;
          }
        };

      }
    }

    char* scid = NULL;
    TraceOp.trc( "tourdlg", TRCLEVEL_INFO, __LINE__, 9999, "schedules in tour: %s", wTour.getschedules(tour) );
    iOStrTok tok = StrTokOp.inst(wTour.getschedules(tour), ',');
    if( StrTokOp.hasMoreTokens(tok) ) {
      scid = StrOp.dup(StrTokOp.nextToken(tok));
      TraceOp.trc( "tourdlg", TRCLEVEL_INFO, __LINE__, 9999, "first schedule in tour: %s", scid!=NULL?scid:"?" );
    }
    StrTokOp.base.del(tok);

    iONode sclist = wPlan.getsclist( model );
    if( sclist != NULL && scid != NULL ) {
      int cnt = NodeOp.getChildCnt( sclist );
      TraceOp.trc( "tourdlg", TRCLEVEL_INFO, __LINE__, 9999, "%d schedules...", cnt );
      for( int i = 0; i < cnt; i++ ) {
        iONode sc = NodeOp.getChild( sclist, i );
        const char* id = wSchedule.getid( sc );
        TraceOp.trc( "tourdlg", TRCLEVEL_INFO, __LINE__, 9999, "check if schedule %s == %s", id, scid );
        if( id != NULL && StrOp.equals(id, scid) ) {
          if( startblock != NULL ) {
            /* check if the schedule start block matches */
            iONode scentry = wSchedule.getscentry( sc );
            TraceOp.trc( "tourdlg", TRCLEVEL_INFO, __LINE__, 9999, "check schedule entry: %d of %s", i, id );
            if( scentry != NULL && StrOp.equals( startblock, wScheduleEntry.getblock( scentry ) ) ) {
              isFirst = true;
            }
            else if( scentry != NULL && locationID != NULL && StrOp.equals( locationID, wScheduleEntry.getblock( scentry ) ) ) {
              isFirst = true;
            }
          }
          else {
            TraceOp.trc( "tourdlg", TRCLEVEL_WARNING, __LINE__, 9999, "start block is not set" );
          }
          break;
        }
     }
    }

    if( scid != NULL )
      StrOp.free(scid);

  }


  return isFirst;
}


void ToursDlg::initValues() {
  if( m_Props == NULL ) {
    TraceOp.trc( "tourdlg", TRCLEVEL_DEBUG, __LINE__, 9999, "no tour selected" );
    return;
  }

  char* title = StrOp.fmt( "%s %s", (const char*)wxGetApp().getMsg("tour").mb_str(wxConvUTF8), wTour.getid( m_Props ) );
  SetTitle( wxString(title,wxConvUTF8) );
  StrOp.free( title );

  // Index
  m_ID->SetValue( wxString(wTour.getid( m_Props ),wxConvUTF8) );

  // Schedules
  m_EntryList->Clear();
  iOStrTok tok = StrTokOp.inst(wTour.getschedules(m_Props), ',');
  while( StrTokOp.hasMoreTokens(tok) ) {
    const char* scid = StrTokOp.nextToken(tok);
    m_EntryList->Append( wxString(scid,wxConvUTF8) );
  }

  StrTokOp.base.del(tok);

}


bool ToursDlg::evaluate() {
  if( m_Props == NULL ) {
    return false;
  }

  if( m_ID->GetValue().Len() == 0 ) {
    wxMessageDialog( this, wxGetApp().getMsg("invalidid"), _T("Rocrail"), wxOK | wxICON_ERROR ).ShowModal();
    m_ID->SetValue( wxString(wItem.getid( m_Props ),wxConvUTF8) );
    return false;
  }
  wItem.setprev_id( m_Props, wItem.getid(m_Props) );

  int cnt = m_EntryList->GetCount();
  TraceOp.trc( "tourdlg", TRCLEVEL_INFO, __LINE__, 9999, "schedules in list: %d", cnt );
  if( cnt > 0 ) {
    char* ids = StrOp.dup("");
    for( int i = 0; i < cnt; i++ ) {
      char* p = ids;
      if( StrOp.len(ids) == 0 )
        ids = StrOp.fmt( "%s", (const char*)m_EntryList->GetString(i).mb_str(wxConvUTF8) );
      else
        ids = StrOp.fmt( "%s,%s", ids, (const char*)m_EntryList->GetString(i).mb_str(wxConvUTF8) );
      StrOp.free( p );
    }
    TraceOp.trc( "tourdlg", TRCLEVEL_INFO, __LINE__, 9999, "schedules in tour: \"%s\"", ids );
    wTour.setschedules( m_Props, ids );
    StrOp.free( ids );
  }
  else
    wTour.setschedules( m_Props, "" );

  wTour.setid( m_Props, m_ID->GetValue().mb_str(wxConvUTF8) );

  return true;
}


void ToursDlg::onApply( wxCommandEvent& event ) {
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


void ToursDlg::onModifyTour( wxCommandEvent& event ) {
  onApply(event);
}


void ToursDlg::onDeleteTour( wxCommandEvent& event ) {
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
    iONode tourlist = wPlan.gettourlist( model );
    if( tourlist != NULL ) {
      NodeOp.removeChild( tourlist, m_Props );
      m_Props = NULL;
    }
  }

  initIndex();

}


void ToursDlg::onAddTour( wxCommandEvent& event ) {
  int i = m_TourList->FindString( _T("NEW") );
  if( i == wxNOT_FOUND ) {
    m_TourList->Append( _T("NEW") );
    iONode model = wxGetApp().getModel();
    if( model != NULL ) {
      iONode tourlist = wPlan.gettourlist( model );
      if( tourlist == NULL ) {
        tourlist = NodeOp.inst( wTourList.name(), model, ELEMENT_NODE );
        NodeOp.addChild( model, tourlist );
      }

      if( tourlist != NULL ) {
        iONode tour = NodeOp.inst( wTour.name(), tourlist, ELEMENT_NODE );
        NodeOp.addChild( tourlist, tour );
        wTour.setid( tour, "NEW" );
        m_Props = tour;
      }
    }
  }
  m_TourList->SetStringSelection( _T("NEW") );
  m_TourList->SetFirstItem( _T("NEW") );
  initValues();

}

void ToursDlg::onTourSelect( wxCommandEvent& event ) {
  if( m_TourList->GetSelection() != wxNOT_FOUND ) {
    m_Props = (iONode)m_TourList->GetClientData(m_TourList->GetSelection());
    if( m_Props != NULL )
      initValues();
    else
      TraceOp.trc( "tourdlg", TRCLEVEL_INFO, __LINE__, 9999, "no selection..." );
  }

}

void ToursDlg::onEntryAdd( wxCommandEvent& event ) {
  int sel = m_ScheduleList->GetSelection();
  if( sel != wxNOT_FOUND ) {
    m_EntryList->Append(m_ScheduleList->GetStringSelection());
  }
}


void ToursDlg::onEntryDelete( wxCommandEvent& event ) {
  int sel = m_EntryList->GetSelection();
  if( sel != wxNOT_FOUND ) {
    m_EntryList->Delete(sel);
  }
}


void ToursDlg::onCancel( wxCommandEvent& event ){
  EndModal( 0 );
}


void ToursDlg::onOK( wxCommandEvent& event ){
  if( m_bSave )
    onApply(event);
  EndModal( wxID_OK );
}

void ToursDlg::onShowAll( wxCommandEvent& event ){
  initIndex();
}


