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

#include "rocrail/wrapper/public/Plan.h"
#include "rocrail/wrapper/public/Item.h"
#include "rocrail/wrapper/public/Tour.h"
#include "rocrail/wrapper/public/TourList.h"
#include "rocrail/wrapper/public/Schedule.h"

#include "rocs/public/trace.h"
#include "rocs/public/list.h"

ToursDlg::ToursDlg( wxWindow* parent )
  :toursdlggen( parent )
{
  m_Props = NULL;
  m_bSave = true;
}


ToursDlg::ToursDlg( wxWindow* parent, iONode tour, bool save )
  :toursdlggen( parent )
{
  m_Props = tour;
  m_bSave = save;

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
  m_TourBook->SetPageText( 1, wxGetApp().getMsg( "schedules" ) );

  // Index
  m_labTourID->SetLabel( wxGetApp().getMsg( "id" ) );
  m_AddTour->SetLabel( wxGetApp().getMsg( "add" ) );
  m_ModifyTour->SetLabel( wxGetApp().getMsg( "modify" ) );
  m_DeleteTour->SetLabel( wxGetApp().getMsg( "delete" ) );

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

}
