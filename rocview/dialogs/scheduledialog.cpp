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
#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "scheduledialog.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

////@begin includes
#include "wx/imaglist.h"
#include "schedulegraph.h"
////@end includes

#include "scheduledialog.h"
#include "actionsctrldlg.h"

////@begin XPM images
////@end XPM images

#include "rocview/public/guiapp.h"
#include "rocrail/wrapper/public/Schedule.h"
#include "rocrail/wrapper/public/ScheduleEntry.h"
#include "rocrail/wrapper/public/ScheduleList.h"
#include "rocrail/wrapper/public/Block.h"
#include "rocrail/wrapper/public/Item.h"
#include "rocrail/wrapper/public/Plan.h"
#include "rocrail/wrapper/public/ModelCmd.h"
#include "rocrail/wrapper/public/Location.h"
#include "rocrail/wrapper/public/LocationList.h"
#include "rocrail/wrapper/public/Output.h"
#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/SelTab.h"
#include "rocrail/wrapper/public/Turntable.h"
#include "rocrail/wrapper/public/Stage.h"

#include "rocs/public/strtok.h"

/*!
 * ScheduleDialog type definition
 */

IMPLEMENT_DYNAMIC_CLASS( ScheduleDialog, wxDialog )

/*!
 * ScheduleDialog event table definition
 */

BEGIN_EVENT_TABLE( ScheduleDialog, wxDialog )

////@begin ScheduleDialog event table entries
    EVT_LIST_ITEM_SELECTED( ID_LISTBOX_SCHEDULE_LIST, ScheduleDialog::OnListboxScheduleListSelected )
    EVT_LIST_COL_CLICK( ID_LISTBOX_SCHEDULE_LIST, ScheduleDialog::OnListboxScheduleListColLeftClick )
    EVT_CHECKBOX( ID_CHECKBOX_SCHEDULES_SHOW_ALL, ScheduleDialog::OnCheckboxSchedulesShowAllClick )
    EVT_BUTTON( ID_BUTTON_SCHEDULE_NEW, ScheduleDialog::OnButtonScheduleNewClick )
    EVT_BUTTON( ID_BUTTON_SCHEDULE_DELETE, ScheduleDialog::OnButtonScheduleDeleteClick )
    EVT_BUTTON( ID_BUTTON_SC_DOC, ScheduleDialog::OnButtonScDocClick )
    EVT_BUTTON( ID_BUTTON_SCHEDULE_COPY, ScheduleDialog::OnButtonScheduleCopyClick )
    EVT_TEXT( ID_TEXTCTRL_SCHEDULE_ID, ScheduleDialog::OnTextctrlScheduleIdUpdated )
    EVT_SPINCTRL( ID_SCHEDULE_FROMHOUR, ScheduleDialog::OnScheduleFromhourUpdated )
    EVT_RADIOBOX( ID_SC_TIMEPROCESSING, ScheduleDialog::OnScTimeprocessingSelected )
    EVT_GRID_CELL_LEFT_CLICK( ScheduleDialog::OnCellLeftClick )
    EVT_GRID_CELL_RIGHT_CLICK( ScheduleDialog::OnCellRightClick )
    EVT_GRID_LABEL_LEFT_CLICK( ScheduleDialog::OnLabelLeftClick )
    EVT_GRID_LABEL_RIGHT_CLICK( ScheduleDialog::OnLabelRightClick )
    EVT_BUTTON( wxID_BUTTON_SCHEDULE_ADD_LOCATION, ScheduleDialog::OnAddLocationClick )
    EVT_BUTTON( wxID_BUTTON_SCHEDULE_ADD_BLOCK, ScheduleDialog::OnAddBlockClick )
    EVT_BUTTON( wxID_BUTTON_SCHEDULE_ENTRY_ACTIONS, ScheduleDialog::OnButtonScheduleEntryActionsClick )
    EVT_BUTTON( wxID_BUTTON_SCHEDULE_REMOVE_DESTINATION, ScheduleDialog::OnRemoveDestinationClick )
    EVT_BUTTON( wxID_BUTTON_SCHEDULE_MODIFY_DESTINATION, ScheduleDialog::OnModifyDestinationClick )
    EVT_BUTTON( ID_DESTUP, ScheduleDialog::OnDestupClick )
    EVT_BUTTON( ID_DESTDOWN, ScheduleDialog::OnDestdownClick )
    EVT_BUTTON( ID_SCHEDULE_ACTIONS, ScheduleDialog::OnScheduleActionsClick )
    EVT_BUTTON( ID_BUTTON_GRAPH_GEN, ScheduleDialog::OnButtonGraphGenClick )
    EVT_BUTTON( wxID_OK, ScheduleDialog::OnOkClick )
    EVT_BUTTON( wxID_CANCEL, ScheduleDialog::OnCancelClick )
    EVT_BUTTON( wxID_APPLY, ScheduleDialog::OnApplyClick )
    EVT_BUTTON( wxID_HELP, ScheduleDialog::OnHelpClick )
////@end ScheduleDialog event table entries

END_EVENT_TABLE()

/*!
 * ScheduleDialog constructors
 */

static bool m_bSortInverse = false;

ScheduleDialog::ScheduleDialog( )
{
}

ScheduleDialog::~ScheduleDialog()
{
  ListOp.base.del(m_EntryPropsList);
}

ScheduleDialog::ScheduleDialog( wxWindow* parent, iONode p_Props, bool save, const char* startblock )
{
  m_TabAlign = wxGetApp().getTabAlign();
  Create(parent, -1, wxGetApp().getMsg("scheduletable"));
  m_Props = p_Props;
  m_bSave = save;
  m_StartBlock = startblock;
  m_SelectedRow = -1;
  m_SortCol = 0;
  m_bSortInverse = false;

  m_ShowAll->SetValue((startblock==NULL)?true:false);
  m_ShowAll->Enable((startblock!=NULL)?true:false);
  m_StartBlockID->SetValue((startblock==NULL)?_T(""):wxString(startblock,wxConvUTF8));

  m_EntryPropsList = ListOp.inst();

  initLabels();
  m_Index->GetSizer()->Layout();
  m_Destinations->GetSizer()->Layout();
  m_NoteBook->Fit();
  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);
  initIndex();

  m_Apply->Enable(m_bSave);

  if( p_Props != NULL ) {
    initSchedule();
    wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, ID_PANEL_SCHEDULE_DESTINATIONS );
    wxPostEvent( this, event );
    m_NoteBook->SetSelection(1);
  }
  else {
    m_NoteBook->SetSelection(0);
  }

}
/* comparator for sorting by id: */
static int __sortStr(obj* _a, obj* _b)
{
    const char* a = (const char*)*_a;
    const char* b = (const char*)*_b;
    return strcmp( a, b );
}

void ScheduleDialog::initBlockCombo() {
  m_Block->Clear();
  m_Block->Append( _T(""), (void*)NULL );

  iONode model = wxGetApp().getModel();
  iOList list = ListOp.inst();

  if( model != NULL ) {
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

    iONode seltablist = wPlan.getseltablist( model );
    if( seltablist != NULL ) {
      int cnt = NodeOp.getChildCnt( seltablist );
      for( int i = 0; i < cnt; i++ ) {
        iONode fy = NodeOp.getChild( seltablist, i );
        const char* id = wSelTab.getid( fy );
        if( id != NULL ) {
          ListOp.add(list, (obj)id);
        }
      }
    }

    iONode ttlist = wPlan.getttlist( model );
    if( ttlist != NULL ) {
      int cnt = NodeOp.getChildCnt( ttlist );
      for( int i = 0; i < cnt; i++ ) {
        iONode tt = NodeOp.getChild( ttlist, i );
        const char* id = wTurntable.getid( tt );
        if( id != NULL && wTurntable.isembeddedblock(tt) ) {
          ListOp.add(list, (obj)id);
        }
      }
    }

    iONode sblist = wPlan.getsblist( model );
    if( sblist != NULL ) {
      int cnt = NodeOp.getChildCnt( sblist );
      for( int i = 0; i < cnt; i++ ) {
        iONode sb = NodeOp.getChild( sblist, i );
        const char* id = wStage.getid( sb );
        ListOp.add(list, (obj)id);
      }
    }

    ListOp.sort(list, &__sortStr);
    int cnt = ListOp.size( list );
    for( int i = 0; i < cnt; i++ ) {
      const char* id = (const char*)ListOp.get( list, i );
      m_Block->Append( wxString(id,wxConvUTF8) );
    }

  }
  /* clean up the temp. list */
  ListOp.base.del(list);
}

void ScheduleDialog::initLocationCombo() {
  m_Location->Clear();
  m_Location->Append( _T(""), (void*)NULL );

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
      ListOp.sort(list, &__sortStr);
      cnt = ListOp.size( list );
      for( int i = 0; i < cnt; i++ ) {
        const char* id = (const char*)ListOp.get( list, i );
        m_Location->Append( wxString(id,wxConvUTF8) );
      }
    }
  }
  /* clean up the temp. list */
  ListOp.base.del(list);
}
void ScheduleDialog::initLabels() {
  m_NoteBook->SetPageText( 0, wxGetApp().getMsg( "index" ) );
  m_NoteBook->SetPageText( 1, wxGetApp().getMsg( "destinations" ) );
  m_NoteBook->SetPageText( 2, wxGetApp().getMsg( "followup" ) );

  //m_Entries->SetRowLabelSize(0);
  wxFont* font = new wxFont( m_Entries->GetDefaultCellFont() );
  font->SetPointSize( (int)(font->GetPointSize() - 1 ) );
  m_Entries->SetSelectionMode(wxGrid::wxGridSelectRows);
  m_Entries->SetDefaultCellFont( *font );
  m_Entries->SetDefaultRowSize(20);
  m_Entries->SetColLabelSize(20);

  // Index
  m_List->InsertColumn(0, wxGetApp().getMsg( "id" ), wxLIST_FORMAT_LEFT );
  m_List->InsertColumn(1, wxGetApp().getMsg( "start" ), wxLIST_FORMAT_LEFT );
  m_List->InsertColumn(2, wxGetApp().getMsg( "destination" ), wxLIST_FORMAT_LEFT );
  m_List->InsertColumn(3, wxGetApp().getMsg( "followup" ), wxLIST_FORMAT_LEFT );

  m_New->SetLabel( wxGetApp().getMsg( "new" ) );
  m_Delete->SetLabel( wxGetApp().getMsg( "delete" ) );
  m_ShowAll->SetLabel( wxGetApp().getMsg( "showall" ) );
  m_labStartBlock->SetLabel( wxGetApp().getMsg( "startblock" ) );
  m_Doc->SetLabel( wxGetApp().getMsg( "doc_report" ) );
  m_CopySchedule->SetLabel( wxGetApp().getMsg( "copy" ) );

  // Detail
  m_LabelID->SetLabel( wxGetApp().getMsg( "id" ) );
  m_labTimeFrame->SetLabel( wxGetApp().getMsg( "timeframe" ) );
  m_TimeProcessing->SetLabel( wxGetApp().getMsg( "timeprocessing" ) );
  m_TimeProcessing->SetString( 0, wxGetApp().getMsg( "real" ) );
  m_TimeProcessing->SetString( 1, wxGetApp().getMsg( "relative" ) );
  m_TimeProcessing->SetString( 2, wxGetApp().getMsg( "hourly" ) );
  m_labCycle->SetLabel( wxGetApp().getMsg( "recycle" ) );

  m_LabelLocation->SetLabel( wxGetApp().getMsg( "location" ) );
  m_LabelBlock->SetLabel( wxGetApp().getMsg( "block" ) );
  m_labHour->SetLabel( wxGetApp().getMsg( "hour" ) );
  m_labMinute->SetLabel( wxGetApp().getMsg( "minute" ) );
  m_labMaxDelay->SetLabel( wxGetApp().getMsg( "maxdelay" ) );
  m_EntryDetails->SetLabel( wxGetApp().getMsg( "details" ) );
  m_EntrySwap->SetLabel( wxGetApp().getMsg( "swapplacing" ) );
  m_Free2Go->SetLabel( wxGetApp().getMsg( "free2go" ) );

  m_AddLocation->SetLabel( wxGetApp().getMsg( "add" ) );
  m_AddBlock->SetLabel( wxGetApp().getMsg( "add" ) );
  m_RemoveDestination->SetLabel( wxGetApp().getMsg( "delete" ) );
  m_ModifyDestination->SetLabel( wxGetApp().getMsg( "modify" ) );
  m_EntryActions->SetLabel( wxGetApp().getMsg( "actions" )+_T("...") );

  m_labDeparture->SetLabel( wxGetApp().getMsg( "departure" ) );
  m_Actions->SetLabel( wxGetApp().getMsg( "actions" )+_T("...") );
  m_labInDelay->SetLabel( wxGetApp().getMsg( "indelay" ) );
  m_DestUp->SetLabel( wxGetApp().getMsg( "up" ) );
  m_DestDown->SetLabel( wxGetApp().getMsg( "down" ) );

  // Buttons
  m_OK->SetLabel( wxGetApp().getMsg( "ok" ) );
  m_Cancel->SetLabel( wxGetApp().getMsg( "cancel" ) );
  m_Apply->SetLabel( wxGetApp().getMsg( "apply" ) );

  m_Entries->SetColLabelValue(0, wxGetApp().getMsg("location"));
  m_Entries->SetColLabelValue(1, wxGetApp().getMsg("block"));
  m_Entries->SetColLabelValue(2, wxGetApp().getMsg("time"));
  m_Entries->SetColLabelValue(3, wxGetApp().getMsg("actions"));
  m_Entries->SetColLabelValue(4, wxGetApp().getMsg("free"));
  m_Entries->EnableEditing(false);
  m_Entries->AutoSizeColumns();
  m_Entries->AutoSizeRows();

  // Initialize Comboboxes
  initBlockCombo();
  initLocationCombo();

  // Actions
  m_ScheduleBox->SetLabel(wxGetApp().getMsg("schedule") );

}

/* comparator for sorting by id: */
static int __sortID(obj* _a, obj* _b)
{
    iONode a = (iONode)*_a;
    iONode b = (iONode)*_b;
    const char* idA = wItem.getid( a );
    const char* idB = wItem.getid( b );
    return m_bSortInverse ? strcmp( idB, idA ):strcmp( idA, idB );
}
/* comparator for sorting by scaction: */
static int __sortFollowup(obj* _a, obj* _b)
{
    iONode a = (iONode)*_a;
    iONode b = (iONode)*_b;
    const char* idA = wSchedule.getscaction( a );
    const char* idB = wSchedule.getscaction( b );
    return m_bSortInverse ? strcmp( idB, idA ):strcmp( idA, idB );
}
static int __sortStart(obj* _a, obj* _b)
{
    iONode a = (iONode)*_a;
    iONode b = (iONode)*_b;
    const char* idA = ScheduleDialog::getStart( a );
    const char* idB = ScheduleDialog::getStart( b );
    return m_bSortInverse ? strcmp( idB, idA ):strcmp( idA, idB );
}
static int __sortEnd(obj* _a, obj* _b)
{
    iONode a = (iONode)*_a;
    iONode b = (iONode)*_b;
    const char* idA = ScheduleDialog::getEnd( a );
    const char* idB = ScheduleDialog::getEnd( b );
    return m_bSortInverse ? strcmp( idB, idA ):strcmp( idA, idB );
}


void ScheduleDialog::initIndex() {
  TraceOp.trc( "scdlg", TRCLEVEL_INFO, __LINE__, 9999, "InitIndex" );

  m_List->DeleteAllItems();

  m_ScheduleAction->Clear();
  m_ScheduleAction->Append( _T("") );

  iONode model = wxGetApp().getModel();
  iOList list = ListOp.inst();
  iOList listAll = ListOp.inst();


  if( model != NULL ) {

    /* check if the block is in a location */
    const char* locationID = NULL;
    iONode locationList = wPlan.getlocationlist(model);
    if( locationList != NULL ) {
      int cnt = NodeOp.getChildCnt( locationList );
      TraceOp.trc( "scdlg", TRCLEVEL_INFO, __LINE__, 9999, "%d locations...", cnt );
      for( int i = 0; i < cnt; i++ ) {
        iONode location = NodeOp.getChild( locationList, i );

        iOStrTok blocks = StrTokOp.inst( wLocation.getblocks( location ), ',' );
        const char* id = NULL;
        while( StrTokOp.hasMoreTokens( blocks ) ) {
          id = StrTokOp.nextToken( blocks );
          TraceOp.trc( "scdlg", TRCLEVEL_INFO, __LINE__, 9999, "check if block [%s] is member location [%s]...", id, wLocation.getid( location ) );
          if( StrOp.equals( id, m_StartBlock) ) {
            TraceOp.trc( "scdlg", TRCLEVEL_INFO, __LINE__, 9999, "block [%s] is member of location [%s]", id, wLocation.getid( location ) );
            locationID = wLocation.getid( location );
            i = cnt;
            break;
          }
        };

      }
    }



    iONode sclist = wPlan.getsclist( model );
    if( sclist != NULL ) {
      int cnt = NodeOp.getChildCnt( sclist );
      TraceOp.trc( "scdlg", TRCLEVEL_INFO, __LINE__, 9999, "%d schedules...", cnt );
      for( int i = 0; i < cnt; i++ ) {
        iONode sc = NodeOp.getChild( sclist, i );
        const char* id = wSchedule.getid( sc );
        if( id != NULL ) {
          ListOp.add(listAll, (obj)sc);
          if( m_ShowAll->IsChecked() || m_StartBlock == NULL )
            ListOp.add(list, (obj)sc);
          else {
            /* check if the schedule start block matches */
            iONode scentry = wSchedule.getscentry( sc );
            if( scentry != NULL && StrOp.equals( m_StartBlock, wScheduleEntry.getblock( scentry ) ) ) {
              ListOp.add(list, (obj)sc);
            }
            else if( scentry != NULL && locationID != NULL && StrOp.equals( locationID, wScheduleEntry.getlocation( scentry ) ) ) {
              ListOp.add(list, (obj)sc);
            }
          }
        }
	    }

      if( m_SortCol == 1 )
        ListOp.sort(list, &__sortStart);
      else if( m_SortCol == 2 )
        ListOp.sort(list, &__sortEnd);
      else if( m_SortCol == 3 )
        ListOp.sort(list, &__sortFollowup);
      else
  	    ListOp.sort(list, &__sortID);

      cnt = ListOp.size( list );
      for( int i = 0; i < cnt; i++ ) {
        iONode sc = (iONode)ListOp.get( list, i );
        const char* id = wItem.getid(sc);
  		  TraceOp.trc( "scdlg", TRCLEVEL_INFO, __LINE__, 9999, "schedule %s", id );
        m_List->InsertItem( i, wxString(id,wxConvUTF8) );
        m_List->SetItem( i, 1, wxString(getStart( sc ), wxConvUTF8) );
        m_List->SetItem( i, 2, wxString(getEnd( sc ), wxConvUTF8) );
        m_List->SetItem( i, 3, wxString(wSchedule.getscaction( sc ), wxConvUTF8) );
        m_List->SetItemPtrData(i, (wxUIntPtr)sc);
  		}
      // resize
      for( int n = 0; n < 4; n++ ) {
        m_List->SetColumnWidth(n, wxLIST_AUTOSIZE_USEHEADER);
        int autoheadersize = m_List->GetColumnWidth(n);
        m_List->SetColumnWidth(n, wxLIST_AUTOSIZE);
        int autosize = m_List->GetColumnWidth(n);
        if(autoheadersize > autosize )
          m_List->SetColumnWidth(n, wxLIST_AUTOSIZE_USEHEADER);
      }


      if( m_SortCol == 1 )
        ListOp.sort(listAll, &__sortStart);
      else if( m_SortCol == 2 )
        ListOp.sort(listAll, &__sortEnd);
      else if( m_SortCol == 3 )
        ListOp.sort(listAll, &__sortFollowup);
      else
        ListOp.sort(listAll, &__sortID);

      cnt = ListOp.size( listAll );
      for( int i = 0; i < cnt; i++ ) {
        iONode sc = (iONode)ListOp.get( listAll, i );
        const char* id = wItem.getid(sc);
        TraceOp.trc( "scdlg", TRCLEVEL_INFO, __LINE__, 9999, "schedule %s", id );
        //m_LocSchedule->Append( wxString(id,wxConvUTF8) );
        m_ScheduleAction->Append( wxString(id,wxConvUTF8) );
      }

      if( m_Props != NULL ) {
        setSelection(wSchedule.getid(m_Props));
      }
      else if(m_List->GetItemCount() > 0 ) {
        m_List->SetItemState(0, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
        m_Props = (iONode)m_List->GetItemData(0);
      }

    }
  }
  /* clean up the temp. list */
  ListOp.base.del(list);
  ListOp.base.del(listAll);

  SetTitle( wxGetApp().getMsg("scheduletable") );

  m_RemoveDestination->Enable( false );
  m_ModifyDestination->Enable( false );
  m_EntryActions->Enable( false );
  m_DestUp->Enable( false );
  m_DestDown->Enable( false );

}

const char* ScheduleDialog::getStart( iONode sc ) {
  iONode scentry = wSchedule.getscentry( sc );
  if( scentry != NULL ) {
    if( wScheduleEntry.getblock(scentry) != NULL && StrOp.len(wScheduleEntry.getblock(scentry)) > 0 )
      return wScheduleEntry.getblock(scentry);
    if( wScheduleEntry.getlocation(scentry) != NULL && StrOp.len(wScheduleEntry.getlocation(scentry)) > 0 )
      return wScheduleEntry.getlocation(scentry);
  }
  return "";
}


const char* ScheduleDialog::getEnd( iONode sc ) {
  iONode scentry = wSchedule.getscentry( sc );
  iONode node = scentry;
  while( scentry != NULL ) {
    scentry = wSchedule.nextscentry( sc, scentry );
    if( scentry != NULL )
      node = scentry;
  };
  scentry = node;
  if( scentry != NULL ) {
    if( wScheduleEntry.getblock(scentry) != NULL && StrOp.len(wScheduleEntry.getblock(scentry)) > 0 )
      return wScheduleEntry.getblock(scentry);
    if( wScheduleEntry.getlocation(scentry) != NULL && StrOp.len(wScheduleEntry.getlocation(scentry)) > 0 )
      return wScheduleEntry.getlocation(scentry);
  }
  return "";
}

void ScheduleDialog::setSelection(const char* ID) {
  int size = m_List->GetItemCount();
  for( int index = 0; index < size; index++ ) {
    iONode node = (iONode)m_List->GetItemData(index);
    if( StrOp.equals( ID, wSchedule.getid(node) ) ) {
      m_List->SetItemState(index, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
      break;
    }
  }
}




void ScheduleDialog::initSchedule() {
  if( m_Props == NULL )
    return;

  char* title = StrOp.fmt( "%s %s", (const char*)wxGetApp().getMsg("schedule").mb_str(wxConvUTF8), wSchedule.getid( m_Props ) );
  SetTitle( wxString(title,wxConvUTF8) );
  StrOp.free( title );

  m_ID->SetValue(wxString(wSchedule.getid( m_Props ),wxConvUTF8));
  m_TimeFrame->SetValue(wSchedule.gettimeframe( m_Props ));
  m_TimeProcessing->SetSelection(wSchedule.gettimeprocessing( m_Props ));
  m_TimeFrame->Enable(m_TimeProcessing->GetSelection() == wSchedule.time_hourly);
  m_Cycle->SetValue(wSchedule.getcycles( m_Props ));
  m_MaxDelay->SetValue(wSchedule.getmaxdelay( m_Props ));

  m_FromHour->SetValue(wSchedule.getfromhour( m_Props ));
  m_FromHour->Enable(m_TimeProcessing->GetSelection() == wSchedule.time_hourly);
  m_ToHour->SetValue(wSchedule.gettohour( m_Props ));
  m_ToHour->Enable(m_TimeProcessing->GetSelection() == wSchedule.time_hourly);

  // Entries
  if( m_Entries->GetNumberRows() > 0 )
    m_Entries->DeleteRows(0,m_Entries->GetNumberRows());
  ListOp.clear( m_EntryPropsList );


  iONode scentry = wSchedule.getscentry( m_Props );
  while( scentry != NULL ) {
    m_Entries->AppendRows();
    ListOp.add( m_EntryPropsList, (obj)scentry );

    m_Entries->SetCellValue(m_Entries->GetNumberRows()-1, 0, wxString(wScheduleEntry.getlocation( scentry ),wxConvUTF8 ) );
    m_Entries->SetCellValue(m_Entries->GetNumberRows()-1, 1, wxString(wScheduleEntry.getblock( scentry ),wxConvUTF8 ) );

    char* hour = StrOp.fmt( "%02d:%02d", wScheduleEntry.gethour( scentry ), wScheduleEntry.getminute( scentry ) );
    m_Entries->SetCellValue(m_Entries->GetNumberRows()-1, 2, wxString(hour,wxConvUTF8 ) );
    StrOp.free( hour );

    bool hasActions = wScheduleEntry.getactionctrl(scentry) != NULL ? true:false;
    bool swap = wScheduleEntry.isswap(scentry) ? true:false;
    char* action = StrOp.fmt( "%s%s", hasActions ? "X ":"", swap ? wxGetApp().getCMsg("swapplacing"):"");
    m_Entries->SetCellValue(m_Entries->GetNumberRows()-1, 3, wxString(action,wxConvUTF8 ) );
    StrOp.free(action);
    m_Entries->SetCellValue(m_Entries->GetNumberRows()-1, 4, wScheduleEntry.isfree2go(scentry) ? wxGetApp().getMsg( "free" ):_T("") );

    m_Entries->SetReadOnly(m_Entries->GetNumberRows()-1, 0, true );
    m_Entries->SetReadOnly(m_Entries->GetNumberRows()-1, 1, true );
    m_Entries->SetReadOnly(m_Entries->GetNumberRows()-1, 2, true );
    m_Entries->SetReadOnly(m_Entries->GetNumberRows()-1, 3, true );
    m_Entries->SetReadOnly(m_Entries->GetNumberRows()-1, 4, true );

    scentry = wSchedule.nextscentry( m_Props, scentry );
  };
  m_Entries->AutoSizeColumns();
  m_Entries->FitInside();
  m_Destinations->GetSizer()->Layout();

  if( m_SelectedRow != -1 ) {
    m_Entries->SelectRow(m_SelectedRow);
    initEntry(m_SelectedRow);
  }
  else {
    m_RemoveDestination->Enable( false );
    m_ModifyDestination->Enable( false );
    m_EntryActions->Enable( false );
    m_DestUp->Enable( false );
    m_DestDown->Enable( false );
  }
  initActions();

}


void ScheduleDialog::initActions() {

  m_ScheduleAction->SetStringSelection( wxString(wSchedule.getscaction( m_Props ),wxConvUTF8) );

}

void ScheduleDialog::initEntry(int row) {
  if( m_Props == NULL )
    return;

  iONode entry = (iONode)ListOp.get( m_EntryPropsList, row );
  if( entry == NULL )
    return;

  m_Location->SetStringSelection(wxString(wScheduleEntry.getlocation( entry ),wxConvUTF8));
  m_Block->SetStringSelection(wxString(wScheduleEntry.getblock( entry ),wxConvUTF8));

  m_Hour->SetValue( wScheduleEntry.gethour( entry ) );
  m_Minute->SetValue( wScheduleEntry.getminute( entry ) );
  m_EntrySwap->SetValue( wScheduleEntry.isswap( entry ) );
  m_Free2Go->SetValue( wScheduleEntry.isfree2go( entry ) );
  m_InDelay->SetValue( wScheduleEntry.getindelay( entry ) );
  m_Free2Go->SetValue( wScheduleEntry.isfree2go( entry ) );

}


bool ScheduleDialog::evaluate() {
  if( m_Props == NULL ) {
    return false;
  }

  if( m_ID->GetValue().Len() == 0 ) {
    wxMessageDialog( this, wxGetApp().getMsg("invalidid"), _T("Rocrail"), wxOK | wxICON_ERROR ).ShowModal();
    m_ID->SetValue( wxString(wSchedule.getid( m_Props ),wxConvUTF8) );
    return false;
  }

  wItem.setprev_id( m_Props, wItem.getid(m_Props) );

  wSchedule.setid( m_Props, m_ID->GetValue().mb_str(wxConvUTF8) );
  wSchedule.settimeframe( m_Props, m_TimeFrame->GetValue() );
  wSchedule.settimeprocessing( m_Props, m_TimeProcessing->GetSelection() );
  wSchedule.setfromhour( m_Props, m_FromHour->GetValue() );
  wSchedule.settohour( m_Props, m_ToHour->GetValue() );
  wSchedule.setcycles( m_Props, m_Cycle->GetValue() );
  wSchedule.setmaxdelay( m_Props, m_MaxDelay->GetValue() );

  wSchedule.setscaction( m_Props, m_ScheduleAction->GetValue().mb_str(wxConvUTF8) );

  if( m_SelectedRow == -1 )
    return true;

  iONode entry = (iONode)ListOp.get( m_EntryPropsList, m_SelectedRow );

  if( entry != NULL ) {
    wScheduleEntry.setlocation( entry, m_Location->GetValue().mb_str(wxConvUTF8) );
    wScheduleEntry.setblock( entry, m_Block->GetValue().mb_str(wxConvUTF8) );
    wScheduleEntry.sethour( entry, m_Hour->GetValue() );
    wScheduleEntry.setminute( entry, m_Minute->GetValue() );
    wScheduleEntry.setindelay( entry, m_InDelay->GetValue() );

  }

  initSchedule();

  return true;
}


/*!
 * ScheduleDialog creator
 */

bool ScheduleDialog::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin ScheduleDialog member initialisation
    m_Props = NULL;
    m_NoteBook = NULL;
    m_Index = NULL;
    m_List = NULL;
    m_ShowAll = NULL;
    m_labStartBlock = NULL;
    m_StartBlockID = NULL;
    m_New = NULL;
    m_Delete = NULL;
    m_Doc = NULL;
    m_CopySchedule = NULL;
    m_Destinations = NULL;
    m_LabelID = NULL;
    m_ID = NULL;
    m_labTimeFrame = NULL;
    m_TimeFrame = NULL;
    m_labFromHour = NULL;
    m_FromHour = NULL;
    m_labToHour = NULL;
    m_ToHour = NULL;
    m_labCycle = NULL;
    m_Cycle = NULL;
    m_labMaxDelay = NULL;
    m_MaxDelay = NULL;
    m_TimeProcessing = NULL;
    m_Entries = NULL;
    m_LabelLocation = NULL;
    m_LabelBlock = NULL;
    m_Location = NULL;
    m_Block = NULL;
    m_AddLocation = NULL;
    m_AddBlock = NULL;
    m_labDeparture = NULL;
    m_labHour = NULL;
    m_labMinute = NULL;
    m_Hour = NULL;
    m_Minute = NULL;
    m_EntryDetails = NULL;
    m_EntrySwap = NULL;
    m_Free2Go = NULL;
    m_labInDelay = NULL;
    m_InDelay = NULL;
    m_EntryActions = NULL;
    m_RemoveDestination = NULL;
    m_ModifyDestination = NULL;
    m_DestUp = NULL;
    m_DestDown = NULL;
    m_ScheduleActions = NULL;
    m_ScheduleBox = NULL;
    m_ScheduleAction = NULL;
    m_Actions = NULL;
    m_Graph = NULL;
    m_GraphGen = NULL;
    m_OK = NULL;
    m_Cancel = NULL;
    m_Apply = NULL;
////@end ScheduleDialog member initialisation

////@begin ScheduleDialog creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end ScheduleDialog creation
    return true;
}

/*!
 * Control creation for ScheduleDialog
 */

void ScheduleDialog::CreateControls()
{
////@begin ScheduleDialog content construction
    ScheduleDialog* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    m_NoteBook = new wxNotebook( itemDialog1, ID_NOTEBOOK_SCHEDULE, wxDefaultPosition, wxDefaultSize, m_TabAlign );

    m_Index = new wxPanel( m_NoteBook, ID_DIALOG_SCHEDULE_INDEX, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxVERTICAL);
    m_Index->SetSizer(itemBoxSizer5);

    m_List = new wxListCtrl( m_Index, ID_LISTBOX_SCHEDULE_LIST, wxDefaultPosition, wxSize(100, 100), wxLC_REPORT|wxLC_SINGLE_SEL|wxLC_HRULES );
    itemBoxSizer5->Add(m_List, 1, wxGROW|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer7 = new wxFlexGridSizer(0, 3, 0, 0);
    itemBoxSizer5->Add(itemFlexGridSizer7, 0, wxGROW|wxALL, 5);
    m_ShowAll = new wxCheckBox( m_Index, ID_CHECKBOX_SCHEDULES_SHOW_ALL, _("Show all"), wxDefaultPosition, wxDefaultSize, 0 );
    m_ShowAll->SetValue(true);
    itemFlexGridSizer7->Add(m_ShowAll, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labStartBlock = new wxStaticText( m_Index, wxID_ANY, _("StartBlock"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer7->Add(m_labStartBlock, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_StartBlockID = new wxTextCtrl( m_Index, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
    m_StartBlockID->Enable(false);
    itemFlexGridSizer7->Add(m_StartBlockID, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    itemFlexGridSizer7->AddGrowableCol(2);

    wxFlexGridSizer* itemFlexGridSizer11 = new wxFlexGridSizer(0, 4, 0, 0);
    itemBoxSizer5->Add(itemFlexGridSizer11, 0, wxGROW|wxALL, 5);
    m_New = new wxButton( m_Index, ID_BUTTON_SCHEDULE_NEW, _("New"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer11->Add(m_New, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Delete = new wxButton( m_Index, ID_BUTTON_SCHEDULE_DELETE, _("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer11->Add(m_Delete, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Doc = new wxButton( m_Index, ID_BUTTON_SC_DOC, _("Doc"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer11->Add(m_Doc, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_CopySchedule = new wxButton( m_Index, ID_BUTTON_SCHEDULE_COPY, _("Copy"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer11->Add(m_CopySchedule, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_NoteBook->AddPage(m_Index, _("Index"));

    m_Destinations = new wxPanel( m_NoteBook, ID_PANEL_SCHEDULE_DESTINATIONS, wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer17 = new wxBoxSizer(wxHORIZONTAL);
    m_Destinations->SetSizer(itemBoxSizer17);

    wxBoxSizer* itemBoxSizer18 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer17->Add(itemBoxSizer18, 1, wxALIGN_TOP|wxALL, 5);
    wxFlexGridSizer* itemFlexGridSizer19 = new wxFlexGridSizer(0, 2, 0, 0);
    itemBoxSizer18->Add(itemFlexGridSizer19, 0, wxGROW|wxALL, 5);
    m_LabelID = new wxStaticText( m_Destinations, wxID_STATIC_SCHEDULE_ID, _("ID"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer19->Add(m_LabelID, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    m_ID = new wxTextCtrl( m_Destinations, ID_TEXTCTRL_SCHEDULE_ID, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer19->Add(m_ID, 1, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labTimeFrame = new wxStaticText( m_Destinations, wxID_ANY, _("Delay time frame"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer19->Add(m_labTimeFrame, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_TimeFrame = new wxSpinCtrl( m_Destinations, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(70, -1), wxSP_ARROW_KEYS, 0, 30, 0 );
    itemFlexGridSizer19->Add(m_TimeFrame, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_labFromHour = new wxStaticText( m_Destinations, wxID_ANY, _("From hour"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer19->Add(m_labFromHour, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_FromHour = new wxSpinCtrl( m_Destinations, ID_SCHEDULE_FROMHOUR, _T("0"), wxDefaultPosition, wxSize(70, -1), wxSP_ARROW_KEYS, 0, 23, 0 );
    itemFlexGridSizer19->Add(m_FromHour, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_labToHour = new wxStaticText( m_Destinations, wxID_ANY, _("To hour"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer19->Add(m_labToHour, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_ToHour = new wxSpinCtrl( m_Destinations, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(70, -1), wxSP_ARROW_KEYS, 0, 23, 0 );
    itemFlexGridSizer19->Add(m_ToHour, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_labCycle = new wxStaticText( m_Destinations, wxID_ANY, _("Recycle"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer19->Add(m_labCycle, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_Cycle = new wxSpinCtrl( m_Destinations, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(70, -1), wxSP_ARROW_KEYS, 0, 100, 0 );
    itemFlexGridSizer19->Add(m_Cycle, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_labMaxDelay = new wxStaticText( m_Destinations, wxID_ANY, _("Max. delay"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer19->Add(m_labMaxDelay, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_MaxDelay = new wxSpinCtrl( m_Destinations, wxID_ANY, _T("60"), wxDefaultPosition, wxSize(90, -1), wxSP_ARROW_KEYS, 0, 1440, 60 );
    itemFlexGridSizer19->Add(m_MaxDelay, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    itemFlexGridSizer19->AddGrowableRow(1);
    itemFlexGridSizer19->AddGrowableCol(1);

    wxArrayString m_TimeProcessingStrings;
    m_TimeProcessingStrings.Add(_("&Real"));
    m_TimeProcessingStrings.Add(_("&Relative"));
    m_TimeProcessingStrings.Add(_("&Hourly"));
    m_TimeProcessing = new wxRadioBox( m_Destinations, ID_SC_TIMEPROCESSING, _("Time processing"), wxDefaultPosition, wxDefaultSize, m_TimeProcessingStrings, 1, wxRA_SPECIFY_ROWS );
    m_TimeProcessing->SetSelection(0);
    itemBoxSizer18->Add(m_TimeProcessing, 0, wxALIGN_LEFT|wxALL, 5);

    wxStaticLine* itemStaticLine33 = new wxStaticLine( m_Destinations, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL );
    itemBoxSizer17->Add(itemStaticLine33, 0, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer34 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer17->Add(itemBoxSizer34, 0, wxGROW|wxLEFT|wxRIGHT, 5);
    m_Entries = new wxGrid( m_Destinations, ID_GRID_SCHEDULE, wxDefaultPosition, wxSize(-1, 200), wxSUNKEN_BORDER|wxVSCROLL );
    m_Entries->SetDefaultColSize(50);
    m_Entries->SetDefaultRowSize(25);
    m_Entries->SetColLabelSize(25);
    m_Entries->SetRowLabelSize(50);
    m_Entries->CreateGrid(1, 5, wxGrid::wxGridSelectRows);
    itemBoxSizer34->Add(m_Entries, 1, wxGROW|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer36 = new wxFlexGridSizer(0, 2, 0, 0);
    itemBoxSizer34->Add(itemFlexGridSizer36, 0, wxGROW|wxALL, 5);
    m_LabelLocation = new wxStaticText( m_Destinations, ID_STATICTEXT_SCHEDULE_FROM, _("Location"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer36->Add(m_LabelLocation, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxADJUST_MINSIZE, 5);

    m_LabelBlock = new wxStaticText( m_Destinations, ID_STATICTEXT_SCHEDULE_TO, _("Block"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer36->Add(m_LabelBlock, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxADJUST_MINSIZE, 5);

    wxArrayString m_LocationStrings;
    m_Location = new wxComboBox( m_Destinations, ID_COMBOBOX_SCHEDULE_FROM_LOCATION, wxEmptyString, wxDefaultPosition, wxDefaultSize, m_LocationStrings, wxCB_READONLY );
    itemFlexGridSizer36->Add(m_Location, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    wxArrayString m_BlockStrings;
    m_Block = new wxComboBox( m_Destinations, ID_COMBOBOX_SCHEDULE_TO_LOCATION, wxEmptyString, wxDefaultPosition, wxDefaultSize, m_BlockStrings, wxCB_READONLY );
    itemFlexGridSizer36->Add(m_Block, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_AddLocation = new wxButton( m_Destinations, wxID_BUTTON_SCHEDULE_ADD_LOCATION, _("Add"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer36->Add(m_AddLocation, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_AddBlock = new wxButton( m_Destinations, wxID_BUTTON_SCHEDULE_ADD_BLOCK, _("Add"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer36->Add(m_AddBlock, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    itemFlexGridSizer36->AddGrowableCol(0);
    itemFlexGridSizer36->AddGrowableCol(1);

    wxBoxSizer* itemBoxSizer43 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer34->Add(itemBoxSizer43, 0, wxGROW, 5);
    wxFlexGridSizer* itemFlexGridSizer44 = new wxFlexGridSizer(0, 1, 0, 0);
    itemBoxSizer43->Add(itemFlexGridSizer44, 0, wxALIGN_TOP|wxLEFT|wxRIGHT, 5);
    m_labDeparture = new wxStaticBox(m_Destinations, wxID_ANY, _("Departure"));
    wxStaticBoxSizer* itemStaticBoxSizer45 = new wxStaticBoxSizer(m_labDeparture, wxVERTICAL);
    itemFlexGridSizer44->Add(itemStaticBoxSizer45, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);
    wxFlexGridSizer* itemFlexGridSizer46 = new wxFlexGridSizer(0, 2, 0, 0);
    itemStaticBoxSizer45->Add(itemFlexGridSizer46, 0, wxGROW, 5);
    m_labHour = new wxStaticText( m_Destinations, wxID_ANY, _("hour"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer46->Add(m_labHour, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxADJUST_MINSIZE, 5);

    m_labMinute = new wxStaticText( m_Destinations, wxID_ANY, _("minute"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer46->Add(m_labMinute, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxADJUST_MINSIZE, 5);

    m_Hour = new wxSpinCtrl( m_Destinations, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(70, -1), wxSP_ARROW_KEYS, 0, 23, 0 );
    itemFlexGridSizer46->Add(m_Hour, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_Minute = new wxSpinCtrl( m_Destinations, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(70, -1), wxSP_ARROW_KEYS, 0, 59, 0 );
    itemFlexGridSizer46->Add(m_Minute, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_EntryDetails = new wxStaticBox(m_Destinations, wxID_ANY, _("Details"));
    wxStaticBoxSizer* itemStaticBoxSizer51 = new wxStaticBoxSizer(m_EntryDetails, wxVERTICAL);
    itemBoxSizer43->Add(itemStaticBoxSizer51, 0, wxALIGN_TOP|wxLEFT|wxRIGHT, 5);
    m_EntrySwap = new wxCheckBox( m_Destinations, wxID_ANY, _("Swap placing"), wxDefaultPosition, wxDefaultSize, 0 );
    m_EntrySwap->SetValue(false);
    itemStaticBoxSizer51->Add(m_EntrySwap, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP, 5);

    m_Free2Go = new wxCheckBox( m_Destinations, wxID_ANY, _("Free before start"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Free2Go->SetValue(false);
    itemStaticBoxSizer51->Add(m_Free2Go, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP, 5);

    wxFlexGridSizer* itemFlexGridSizer54 = new wxFlexGridSizer(0, 2, 0, 0);
    itemStaticBoxSizer51->Add(itemFlexGridSizer54, 0, wxGROW, 5);
    m_labInDelay = new wxStaticText( m_Destinations, wxID_ANY, _("IN delay"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer54->Add(m_labInDelay, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_InDelay = new wxSpinCtrl( m_Destinations, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(80, -1), wxSP_ARROW_KEYS, 0, 10000, 0 );
    itemFlexGridSizer54->Add(m_InDelay, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_EntryActions = new wxButton( m_Destinations, wxID_BUTTON_SCHEDULE_ENTRY_ACTIONS, _("Actions..."), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticBoxSizer51->Add(m_EntryActions, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    wxBoxSizer* itemBoxSizer58 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer34->Add(itemBoxSizer58, 0, wxALIGN_LEFT, 5);
    m_RemoveDestination = new wxButton( m_Destinations, wxID_BUTTON_SCHEDULE_REMOVE_DESTINATION, _("Remove"), wxDefaultPosition, wxDefaultSize, 0 );
    m_RemoveDestination->SetDefault();
    itemBoxSizer58->Add(m_RemoveDestination, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_ModifyDestination = new wxButton( m_Destinations, wxID_BUTTON_SCHEDULE_MODIFY_DESTINATION, _("Modify"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer58->Add(m_ModifyDestination, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_DestUp = new wxButton( m_Destinations, ID_DESTUP, _("Up"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer58->Add(m_DestUp, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_DestDown = new wxButton( m_Destinations, ID_DESTDOWN, _("Down"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer58->Add(m_DestDown, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_NoteBook->AddPage(m_Destinations, _("Destinations"));

    m_ScheduleActions = new wxPanel( m_NoteBook, ID_PANEL_SCHEDULES_ACTIONS, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer64 = new wxBoxSizer(wxVERTICAL);
    m_ScheduleActions->SetSizer(itemBoxSizer64);

    m_ScheduleBox = new wxStaticBox(m_ScheduleActions, wxID_ANY, _("Schedule"));
    wxStaticBoxSizer* itemStaticBoxSizer65 = new wxStaticBoxSizer(m_ScheduleBox, wxHORIZONTAL);
    itemBoxSizer64->Add(itemStaticBoxSizer65, 0, wxGROW|wxALL, 5);
    wxArrayString m_ScheduleActionStrings;
    m_ScheduleAction = new wxComboBox( m_ScheduleActions, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, m_ScheduleActionStrings, wxCB_DROPDOWN );
    itemStaticBoxSizer65->Add(m_ScheduleAction, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Actions = new wxButton( m_ScheduleActions, ID_SCHEDULE_ACTIONS, _("Actions..."), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer64->Add(m_Actions, 0, wxALIGN_LEFT|wxALL, 5);

    m_NoteBook->AddPage(m_ScheduleActions, _("Actions"));

    wxPanel* itemPanel68 = new wxPanel( m_NoteBook, ID_PANEL_SCHEDULE_GRAPH, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer69 = new wxBoxSizer(wxVERTICAL);
    itemPanel68->SetSizer(itemBoxSizer69);

    m_Graph = new ScheduleGraph( itemPanel68, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    m_Graph->SetBackgroundColour(wxColour(255, 255, 255));
    itemBoxSizer69->Add(m_Graph, 1, wxGROW|wxALL, 10);

    wxBoxSizer* itemBoxSizer71 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer69->Add(itemBoxSizer71, 0, wxALIGN_RIGHT|wxALL, 5);
    m_GraphGen = new wxButton( itemPanel68, ID_BUTTON_GRAPH_GEN, _("Generate"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer71->Add(m_GraphGen, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_NoteBook->AddPage(itemPanel68, _("Graph"));

    itemBoxSizer2->Add(m_NoteBook, 1, wxGROW|wxALL, 5);

    wxStdDialogButtonSizer* itemStdDialogButtonSizer73 = new wxStdDialogButtonSizer;

    itemBoxSizer2->Add(itemStdDialogButtonSizer73, 0, wxGROW|wxALL, 5);
    m_OK = new wxButton( itemDialog1, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer73->AddButton(m_OK);

    m_Cancel = new wxButton( itemDialog1, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer73->AddButton(m_Cancel);

    m_Apply = new wxButton( itemDialog1, wxID_APPLY, _("&Apply"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer73->AddButton(m_Apply);

    wxButton* itemButton77 = new wxButton( itemDialog1, wxID_HELP, _("&Help"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer73->AddButton(itemButton77);

    itemStdDialogButtonSizer73->Realize();

////@end ScheduleDialog content construction
}

/*!
 * Should we show tooltips?
 */

bool ScheduleDialog::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap ScheduleDialog::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin ScheduleDialog bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end ScheduleDialog bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon ScheduleDialog::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin ScheduleDialog icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end ScheduleDialog icon retrieval
}
/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_SCHEDULE_ADD
 */

void ScheduleDialog::OnButtonScheduleDeleteClick( wxCommandEvent& event )
{
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
    iONode sclist = wPlan.getsclist( model );
    if( sclist != NULL ) {
      NodeOp.removeChild( sclist, m_Props );
      m_Props = NULL;
    }
  }

  initIndex();
}


int ScheduleDialog::findID( const char* ID ) {
  int size = m_List->GetItemCount();
  for( int index = 0; index < size; index++ ) {
    iONode node = (iONode)m_List->GetItemData(index);
    if( StrOp.equals( ID, wSchedule.getid(node) ) ) {
      return index;
    }
  }
  return wxNOT_FOUND;
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_SCHEDULE_NEW
 */

void ScheduleDialog::OnButtonScheduleNewClick( wxCommandEvent& event )
{
  int i = findID("NEW");
  if( i == wxNOT_FOUND ) {
    iONode model = wxGetApp().getModel();
    if( model != NULL ) {
      iONode sclist = wPlan.getsclist( model );
      if( sclist == NULL ) {
        sclist = NodeOp.inst( wScheduleList.name(), model, ELEMENT_NODE );
        NodeOp.addChild( model, sclist );
      }

      if( sclist != NULL ) {
        iONode sc = NodeOp.inst( wSchedule.name(), sclist, ELEMENT_NODE );
        NodeOp.addChild( sclist, sc );
        wSchedule.setid( sc, "NEW" );
        m_Props = sc;
        initIndex();
      }
    }
  }
  initIndex();
  setSelection(wSchedule.getid(m_Props));
  initSchedule();
}


/*!
 * wxEVT_COMMAND_LISTBOX_SELECTED event handler for ID_LISTBOX_SCHEDULE_LIST
 */

void ScheduleDialog::OnListboxScheduleListSelected( wxListEvent& event )
{
  int index = event.GetIndex();
  m_Props = (iONode)m_List->GetItemData(index);
  m_SelectedRow = -1;
  initSchedule();
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_APPLY
 */

void ScheduleDialog::OnApplyClick( wxCommandEvent& event )
{
  TraceOp.trc( "scdlg", TRCLEVEL_INFO, __LINE__, 9999, "apply" );
  if( m_Props == NULL ) {
    TraceOp.trc( "scdlg", TRCLEVEL_WARNING, __LINE__, 9999, "no valid properties" );
    return;
  }

  if( !evaluate() ) {
    TraceOp.trc( "scdlg", TRCLEVEL_WARNING, __LINE__, 9999, "error on evaluate" );
    return;
  }

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
  initSchedule();
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
 */

void ScheduleDialog::OnOkClick( wxCommandEvent& event )
{
  if( m_bSave )
    OnApplyClick(event);
  EndModal( wxID_OK );
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
 */

void ScheduleDialog::OnCancelClick( wxCommandEvent& event )
{
  EndModal( 0 );
}


/*!
 * wxEVT_GRID_CELL_LEFT_CLICK event handler for ID_GRID
 */

void ScheduleDialog::OnCellLeftClick( wxGridEvent& event )
{
  m_SelectedRow = event.GetRow();
  m_Entries->SelectRow(event.GetRow());
  m_RemoveDestination->Enable( true );
  m_ModifyDestination->Enable( true );
  m_EntryActions->Enable( true );
  m_DestUp->Enable( true );
  m_DestDown->Enable( true );
  initEntry( event.GetRow() );
}


/*!
 * wxEVT_GRID_CELL_RIGHT_CLICK event handler for ID_GRID
 */

void ScheduleDialog::OnCellRightClick( wxGridEvent& event )
{
  m_Entries->SelectRow(event.GetRow());
  m_RemoveDestination->Enable( true );
  m_ModifyDestination->Enable( true );
  m_EntryActions->Enable( true );
}


/*!
 * wxEVT_GRID_LABEL_LEFT_CLICK event handler for ID_GRID_SCHEDULE
 */

void ScheduleDialog::OnLabelLeftClick( wxGridEvent& event )
{
  OnCellLeftClick( event );
}

/*!
 * wxEVT_GRID_LABEL_RIGHT_CLICK event handler for ID_GRID_SCHEDULE
 */

void ScheduleDialog::OnLabelRightClick( wxGridEvent& event )
{
  OnCellRightClick( event );
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_BUTTON_SCHEDULE_ADD_LOCATION
 */

void ScheduleDialog::OnAddLocationClick( wxCommandEvent& event )
{
  if( m_Props == NULL )
    return;

  wSchedule.setid( m_Props, m_ID->GetValue().mb_str(wxConvUTF8) );

  iONode scentry = NodeOp.inst( wScheduleEntry.name(), m_Props, ELEMENT_NODE );
  wScheduleEntry.setlocation( scentry, m_Location->GetStringSelection().mb_str(wxConvUTF8) );
  wScheduleEntry.sethour( scentry, m_Hour->GetValue() );
  wScheduleEntry.setminute( scentry, m_Minute->GetValue() );
  wScheduleEntry.setswap( scentry, m_EntrySwap->IsChecked()?True:False );
  wScheduleEntry.setfree2go( scentry, m_Free2Go->IsChecked()?True:False );
  wScheduleEntry.setindelay( scentry, m_InDelay->GetValue() );
  NodeOp.addChild( m_Props, scentry );
  initSchedule();

}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_BUTTON_SCHEDULE_ADD_BLOCK
 */

void ScheduleDialog::OnAddBlockClick( wxCommandEvent& event )
{
  if( m_Props == NULL )
    return;

  wSchedule.setid( m_Props, m_ID->GetValue().mb_str(wxConvUTF8) );

  iONode scentry = NodeOp.inst( wScheduleEntry.name(), m_Props, ELEMENT_NODE );
  wScheduleEntry.setblock( scentry, m_Block->GetStringSelection().mb_str(wxConvUTF8) );
  wScheduleEntry.sethour( scentry, m_Hour->GetValue() );
  wScheduleEntry.setminute( scentry, m_Minute->GetValue() );
  wScheduleEntry.setswap( scentry, m_EntrySwap->IsChecked()?True:False );
  wScheduleEntry.setfree2go( scentry, m_Free2Go->IsChecked()?True:False );
  wScheduleEntry.setindelay( scentry, m_InDelay->GetValue() );
  NodeOp.addChild( m_Props, scentry );
  initSchedule();

}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_BUTTON_SCHEDULE_REMOVE_DESTINATION
 */

void ScheduleDialog::OnRemoveDestinationClick( wxCommandEvent& event )
{
  iONode entry = (iONode)ListOp.get( m_EntryPropsList, m_SelectedRow );

  if( entry != NULL ) {
    ListOp.removeObj( m_EntryPropsList, (obj)entry );
    NodeOp.removeChild( m_Props, entry );
    initSchedule();
  }


}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_BUTTON_SCHEDULE_MODIFY_DESTINATION
 */

void ScheduleDialog::OnModifyDestinationClick( wxCommandEvent& event )
{
  iONode entry = (iONode)ListOp.get( m_EntryPropsList, m_SelectedRow );

  if( entry != NULL ) {
    wScheduleEntry.setlocation( entry, m_Location->GetStringSelection().mb_str(wxConvUTF8) );
    wScheduleEntry.setblock( entry, m_Block->GetStringSelection().mb_str(wxConvUTF8) );
    wScheduleEntry.sethour( entry, m_Hour->GetValue() );
    wScheduleEntry.setminute( entry, m_Minute->GetValue() );
    wScheduleEntry.setswap( entry, m_EntrySwap->IsChecked()?True:False );
    wScheduleEntry.setfree2go( entry, m_Free2Go->IsChecked()?True:False );
    wScheduleEntry.setindelay( entry, m_InDelay->GetValue() );
    initSchedule();
  }


}

/*!
 * wxEVT_COMMAND_TEXT_UPDATED event handler for ID_TEXTCTRL_SCHEDULE_ID
 */

void ScheduleDialog::OnTextctrlScheduleIdUpdated( wxCommandEvent& event )
{
  if( m_Props == NULL )
    return;

}


/*!
 * wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CHECKBOX_SCHEDULES_SHOW_ALL
 */

void ScheduleDialog::OnCheckboxSchedulesShowAllClick( wxCommandEvent& event )
{
  initIndex();
}


void ScheduleDialog::OnButtonScheduleEntryActionsClick( wxCommandEvent& event )
{
  if( m_SelectedRow == -1 )
    return;

  iONode entry = (iONode)ListOp.get( m_EntryPropsList, m_SelectedRow );

  if( entry != NULL ) {
    ActionsCtrlDlg*  dlg = new ActionsCtrlDlg(this, entry );
    dlg->ShowModal();
    dlg->Destroy();
    initSchedule();
  }
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_SC_DOC
 */

void ScheduleDialog::OnButtonScDocClick( wxCommandEvent& event )

{
  doDoc( event, "schedules");
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_SCHEDULE_ACTIONS
 */

void ScheduleDialog::OnScheduleActionsClick( wxCommandEvent& event )
{
  if( m_Props == NULL )
    return;

  ActionsCtrlDlg*  dlg = new ActionsCtrlDlg(this, m_Props );

  if( wxID_OK == dlg->ShowModal() ) {
    // TODO: inform
  }

  dlg->Destroy();

  initSchedule();
}


/*!
 * wxEVT_COMMAND_RADIOBOX_SELECTED event handler for ID_SC_TIMEPROCESSING
 */

void ScheduleDialog::OnScTimeprocessingSelected( wxCommandEvent& event )
{
  m_TimeFrame->Enable(m_TimeProcessing->GetSelection() == wSchedule.time_hourly);
  m_FromHour->Enable(m_TimeProcessing->GetSelection() == wSchedule.time_hourly);
  m_ToHour->Enable(m_TimeProcessing->GetSelection() == wSchedule.time_hourly);
  m_Cycle->Enable(m_TimeProcessing->GetSelection() != wSchedule.time_hourly);
}


/*!
 * wxEVT_COMMAND_SPINCTRL_UPDATED event handler for ID_SCHEDULE_FROMHOUR
 */

void ScheduleDialog::OnScheduleFromhourUpdated( wxSpinEvent& event )
{
  if( m_FromHour->GetValue() > m_ToHour->GetValue() )
    m_ToHour->SetValue(m_FromHour->GetValue());
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_DESTUP
 */

void ScheduleDialog::OnDestupClick( wxCommandEvent& event ) {
  if( m_Props == NULL )
    return;

  int size = ListOp.size(m_EntryPropsList);
  if( size == 0 )
    return;

  if( m_SelectedRow == -1 || m_SelectedRow == 0 )
    return;

  iONode entry = (iONode)ListOp.get( m_EntryPropsList, m_SelectedRow );
  if( entry == NULL )
    return;

  ListOp.insert( m_EntryPropsList, m_SelectedRow-1, (obj)entry);
  ListOp.remove( m_EntryPropsList, m_SelectedRow+1 );
  m_SelectedRow--;

  iONode scentry = wSchedule.getscentry( m_Props );
  while( scentry != NULL ) {
    NodeOp.removeChild(m_Props, scentry);
    scentry = wSchedule.getscentry( m_Props );
  }

  size = ListOp.size(m_EntryPropsList);
  for( int i = 0; i < size; i++ ) {
    NodeOp.addChild(m_Props, (iONode)ListOp.get(m_EntryPropsList, i));
  }

  initSchedule();
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_DESTDOWN
 */

void ScheduleDialog::OnDestdownClick( wxCommandEvent& event ) {
  if( m_Props == NULL )
    return;

  int size = ListOp.size(m_EntryPropsList);

  if( size == 0 )
    return;

  if( m_SelectedRow == -1 || m_SelectedRow == (size - 1) )
    return;

  iONode entry = (iONode)ListOp.get( m_EntryPropsList, m_SelectedRow );
  if( entry == NULL )
    return;

  ListOp.insert( m_EntryPropsList, m_SelectedRow+2, (obj)entry);
  ListOp.remove( m_EntryPropsList, m_SelectedRow );
  m_SelectedRow++;

  iONode scentry = wSchedule.getscentry( m_Props );
  while( scentry != NULL ) {
    NodeOp.removeChild(m_Props, scentry);
    scentry = wSchedule.getscentry( m_Props );
  }

  size = ListOp.size(m_EntryPropsList);
  for( int i = 0; i < size; i++ ) {
    NodeOp.addChild(m_Props, (iONode)ListOp.get(m_EntryPropsList, i));
  }

  initSchedule();
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_SCHEDULE_COPY
 */

void ScheduleDialog::OnButtonScheduleCopyClick( wxCommandEvent& event )
{
  if( m_Props != NULL ) {
    iONode model = wxGetApp().getModel();
    if( model != NULL ) {
      iONode sclist = wPlan.getsclist( model );
      if( sclist == NULL ) {
        sclist = NodeOp.inst( wScheduleList.name(), model, ELEMENT_NODE );
        NodeOp.addChild( model, sclist );
      }

      if( sclist != NULL ) {
        iONode sccopy = (iONode)NodeOp.base.clone( m_Props );
        char* id = StrOp.fmt( "%s (copy)", wSchedule.getid(sccopy));
        wSchedule.setid(sccopy, id);
        StrOp.free(id);
        NodeOp.addChild( sclist, sccopy );
        initIndex();
      }

    }
  }
}


/*!
 * wxEVT_COMMAND_LIST_COL_CLICK event handler for ID_LISTBOX_SCHEDULE_LIST
 */

void ScheduleDialog::OnListboxScheduleListColLeftClick( wxListEvent& event )
{
  int SortCol = event.GetColumn();
  if( SortCol == m_SortCol)
    m_bSortInverse = !m_bSortInverse;
  m_SortCol = SortCol;
  initIndex();
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_GRAPH_GEN
 */

void ScheduleDialog::OnButtonGraphGenClick( wxCommandEvent& event )
{
  if( m_Props != NULL ) {
    m_Graph->setSchedule(m_Props);
  }
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_HELP
 */

void ScheduleDialog::OnHelpClick( wxCommandEvent& event )
{
  switch( m_NoteBook->GetSelection() ) {
  case 0: wxGetApp().openLink( "schedules-index" ); break;
  case 1: wxGetApp().openLink( "schedules-dest" ); break;
  case 2: wxGetApp().openLink( "schedules-actions" ); break;
  default: wxGetApp().openLink( "schedules" ); break;
  }
}

