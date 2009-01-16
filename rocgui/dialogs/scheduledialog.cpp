/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2007 - Rob Versluis <r.j.versluis@rocrail.net>

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
////@end includes

#include "scheduledialog.h"
#include "actionsctrldlg.h"

////@begin XPM images
////@end XPM images

#include "rocgui/public/guiapp.h"
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
    EVT_LISTBOX( ID_LISTBOX_SCHEDULE_LIST, ScheduleDialog::OnListboxScheduleListSelected )

    EVT_CHECKBOX( ID_CHECKBOX_SCHEDULES_SHOW_ALL, ScheduleDialog::OnCheckboxSchedulesShowAllClick )

    EVT_BUTTON( ID_BUTTON_SCHEDULE_NEW, ScheduleDialog::OnButtonScheduleNewClick )

    EVT_BUTTON( ID_BUTTON_SCHEDULE_DELETE, ScheduleDialog::OnButtonScheduleDeleteClick )

    EVT_BUTTON( ID_BUTTON_SC_DOC, ScheduleDialog::OnButtonScDocClick )

    EVT_TEXT( ID_TEXTCTRL_SCHEDULE_ID, ScheduleDialog::OnTextctrlScheduleIdUpdated )

    EVT_GRID_CELL_LEFT_CLICK( ScheduleDialog::OnCellLeftClick )
    EVT_GRID_CELL_RIGHT_CLICK( ScheduleDialog::OnCellRightClick )
    EVT_GRID_LABEL_LEFT_CLICK( ScheduleDialog::OnLabelLeftClick )
    EVT_GRID_LABEL_RIGHT_CLICK( ScheduleDialog::OnLabelRightClick )

    EVT_BUTTON( wxID_BUTTON_SCHEDULE_ADD_LOCATION, ScheduleDialog::OnAddLocationClick )

    EVT_BUTTON( wxID_BUTTON_SCHEDULE_ADD_BLOCK, ScheduleDialog::OnAddBlockClick )

    EVT_BUTTON( wxID_BUTTON_SCHEDULE_REMOVE_DESTINATION, ScheduleDialog::OnRemoveDestinationClick )

    EVT_BUTTON( wxID_BUTTON_SCHEDULE_MODIFY_DESTINATION, ScheduleDialog::OnModifyDestinationClick )

    EVT_BUTTON( wxID_BUTTON_SCHEDULE_ENTRY_ACTIONS, ScheduleDialog::OnButtonScheduleEntryActionsClick )

    EVT_BUTTON( ID_SCHEDULE_ACTIONS, ScheduleDialog::OnScheduleActionsClick )

    EVT_BUTTON( wxID_OK, ScheduleDialog::OnOkClick )

    EVT_BUTTON( wxID_CANCEL, ScheduleDialog::OnCancelClick )

    EVT_BUTTON( wxID_APPLY, ScheduleDialog::OnApplyClick )

////@end ScheduleDialog event table entries

END_EVENT_TABLE()

/*!
 * ScheduleDialog constructors
 */

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
  m_ShowAll->SetValue((startblock==NULL)?true:false);
  m_ShowAll->Enable((startblock!=NULL)?true:false);
  m_StartBlockID->SetValue((startblock==NULL)?_T(""):wxString(startblock,wxConvUTF8));

  m_EntryPropsList = ListOp.inst();

  initLabels();
  initIndex();

  if( m_Props != NULL ) {
    initSchedule();
    wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, ID_PANEL_SCHEDULE_DESTINATIONS );
    wxPostEvent( this, event );
  }

  m_Index->GetSizer()->Layout();
  m_Destinations->GetSizer()->Layout();
  m_NoteBook->Fit();
  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);
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
      ListOp.sort(list, &__sortStr);
      cnt = ListOp.size( list );
      for( int i = 0; i < cnt; i++ ) {
        const char* id = (const char*)ListOp.get( list, i );
        m_Block->Append( wxString(id,wxConvUTF8) );
      }
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
  m_New->SetLabel( wxGetApp().getMsg( "new" ) );
  m_Delete->SetLabel( wxGetApp().getMsg( "delete" ) );
  m_ShowAll->SetLabel( wxGetApp().getMsg( "showall" ) );
  m_labStartBlock->SetLabel( wxGetApp().getMsg( "startblock" ) );
  m_Doc->SetLabel( wxGetApp().getMsg( "doc_report" ) );

  // Detail
  m_LabelID->SetLabel( wxGetApp().getMsg( "id" ) );
  m_RelativeTime->SetLabel( wxGetApp().getMsg( "relativetime" ) );
  m_LabelLocation->SetLabel( wxGetApp().getMsg( "location" ) );
  m_LabelBlock->SetLabel( wxGetApp().getMsg( "block" ) );
  m_labHour->SetLabel( wxGetApp().getMsg( "hour" ) );
  m_labMinute->SetLabel( wxGetApp().getMsg( "minute" ) );
  m_EntryDetails->SetLabel( wxGetApp().getMsg( "details" ) );
  m_EntrySwap->SetLabel( wxGetApp().getMsg( "swapplacing" ) );

  m_AddLocation->SetLabel( wxGetApp().getMsg( "add" ) );
  m_AddBlock->SetLabel( wxGetApp().getMsg( "add" ) );
  m_RemoveDestination->SetLabel( wxGetApp().getMsg( "delete" ) );
  m_ModifyDestination->SetLabel( wxGetApp().getMsg( "modify" ) );
  m_EntryActions->SetLabel( wxGetApp().getMsg( "actions" )+_T("...") );

  m_labDeparture->SetLabel( wxGetApp().getMsg( "departure" ) );
  m_Actions->SetLabel( wxGetApp().getMsg( "actions" )+_T("...") );

  // Buttons
  m_OK->SetLabel( wxGetApp().getMsg( "ok" ) );
  m_Cancel->SetLabel( wxGetApp().getMsg( "cancel" ) );
  m_Apply->SetLabel( wxGetApp().getMsg( "apply" ) );

  m_Entries->SetLabelValue(wxHORIZONTAL, wxGetApp().getMsg("location"), 0);
  m_Entries->SetLabelValue(wxHORIZONTAL, wxGetApp().getMsg("block"), 1);
  m_Entries->SetLabelValue(wxHORIZONTAL, wxGetApp().getMsg("time"), 2);
  m_Entries->SetEditable(FALSE);
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
    return strcmp( idA, idB );
}


void ScheduleDialog::initIndex() {
  TraceOp.trc( "scdlg", TRCLEVEL_INFO, __LINE__, 9999, "InitIndex" );

  iONode l_Props = m_Props;
  m_List->Clear();

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

  	  ListOp.sort(list, &__sortID);
      cnt = ListOp.size( list );
      for( int i = 0; i < cnt; i++ ) {
        iONode sc = (iONode)ListOp.get( list, i );
        const char* id = wItem.getid(sc);
  		  TraceOp.trc( "scdlg", TRCLEVEL_INFO, __LINE__, 9999, "schedule %s", id );
        m_List->Append( wxString(id,wxConvUTF8), sc );
        m_ScheduleAction->Append( wxString(id,wxConvUTF8) );
  		}

      ListOp.sort(listAll, &__sortID);
      cnt = ListOp.size( listAll );
      for( int i = 0; i < cnt; i++ ) {
        iONode sc = (iONode)ListOp.get( listAll, i );
        const char* id = wItem.getid(sc);
        TraceOp.trc( "scdlg", TRCLEVEL_INFO, __LINE__, 9999, "schedule %s", id );
        //m_LocSchedule->Append( wxString(id,wxConvUTF8) );
      }

      if( l_Props != NULL ) {
        m_List->SetStringSelection( wxString(wSchedule.getid( l_Props ),wxConvUTF8) );
        m_List->SetFirstItem( wxString(wSchedule.getid( l_Props ),wxConvUTF8) );
        m_Props = l_Props;
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

}


void ScheduleDialog::initSchedule() {
  if( m_Props == NULL )
    return;

  char* title = StrOp.fmt( "%s %s", (const char*)wxGetApp().getMsg("schedule").mb_str(wxConvUTF8), wSchedule.getid( m_Props ) );
  SetTitle( wxString(title,wxConvUTF8) );
  StrOp.free( title );

  m_ID->SetValue(wxString(wSchedule.getid( m_Props ),wxConvUTF8));
  m_RelativeTime->SetValue(wSchedule.isrelativetime( m_Props )?true:false);


  // Entries
  m_Entries->DeleteRows(0,m_Entries->GetNumberRows());
  ListOp.clear( m_EntryPropsList );
  m_SelectedRow = -1;


  iONode scentry = wSchedule.getscentry( m_Props );
  while( scentry != NULL ) {
    m_Entries->AppendRows();
    ListOp.add( m_EntryPropsList, (obj)scentry );

    m_Entries->SetCellValue(m_Entries->GetNumberRows()-1, 0, wxString(wScheduleEntry.getlocation( scentry ),wxConvUTF8 ) );
    m_Entries->SetCellValue(m_Entries->GetNumberRows()-1, 1, wxString(wScheduleEntry.getblock( scentry ),wxConvUTF8 ) );

    char* hour = StrOp.fmt( "%02d:%02d", wScheduleEntry.gethour( scentry ), wScheduleEntry.getminute( scentry ) );
    m_Entries->SetCellValue(m_Entries->GetNumberRows()-1, 2, wxString(hour,wxConvUTF8 ) );
    StrOp.free( hour );


    m_Entries->SetReadOnly(m_Entries->GetNumberRows()-1, 0, true );
    m_Entries->SetReadOnly(m_Entries->GetNumberRows()-1, 1, true );
    m_Entries->SetReadOnly(m_Entries->GetNumberRows()-1, 2, true );

    scentry = wSchedule.nextscentry( m_Props, scentry );
  };
  m_Entries->AutoSize();
  m_Entries->FitInside();
  //m_Entries->UpdateDimensions();
  m_Destinations->GetSizer()->Layout();

  m_RemoveDestination->Enable( false );
  m_ModifyDestination->Enable( false );
  m_EntryActions->Enable( false );

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

}


void ScheduleDialog::evaluate() {
  if( m_Props == NULL )
    return;

  wItem.setprev_id( m_Props, wItem.getid(m_Props) );

  wSchedule.setid( m_Props, m_ID->GetValue().mb_str(wxConvUTF8) );
  wSchedule.setrelativetime( m_Props, m_RelativeTime->IsChecked()?True:False );

  wSchedule.setscaction( m_Props, m_ScheduleAction->GetValue().mb_str(wxConvUTF8) );

  if( m_SelectedRow == -1 )
    return;

  iONode entry = (iONode)ListOp.get( m_EntryPropsList, m_SelectedRow );

  if( entry != NULL ) {
    wScheduleEntry.setlocation( entry, m_Location->GetValue().mb_str(wxConvUTF8) );
    wScheduleEntry.setblock( entry, m_Block->GetValue().mb_str(wxConvUTF8) );
    wScheduleEntry.sethour( entry, m_Hour->GetValue() );
    wScheduleEntry.setminute( entry, m_Minute->GetValue() );

  }

  initSchedule();
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
    m_Destinations = NULL;
    m_LabelID = NULL;
    m_ID = NULL;
    m_RelativeTime = NULL;
    m_Entries = NULL;
    m_LabelLocation = NULL;
    m_LabelBlock = NULL;
    m_Location = NULL;
    m_Block = NULL;
    m_AddLocation = NULL;
    m_AddBlock = NULL;
    m_RemoveDestination = NULL;
    m_ModifyDestination = NULL;
    m_EntryActions = NULL;
    m_labDeparture = NULL;
    m_labHour = NULL;
    m_labMinute = NULL;
    m_Hour = NULL;
    m_Minute = NULL;
    m_EntryDetails = NULL;
    m_EntrySwap = NULL;
    m_ScheduleActions = NULL;
    m_ScheduleBox = NULL;
    m_ScheduleAction = NULL;
    m_Actions = NULL;
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

    wxArrayString m_ListStrings;
    m_List = new wxListBox( m_Index, ID_LISTBOX_SCHEDULE_LIST, wxDefaultPosition, wxDefaultSize, m_ListStrings, wxLB_SINGLE|wxLB_ALWAYS_SB );
    itemBoxSizer5->Add(m_List, 1, wxGROW|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer7 = new wxFlexGridSizer(1, 3, 0, 0);
    itemFlexGridSizer7->AddGrowableCol(2);
    itemBoxSizer5->Add(itemFlexGridSizer7, 0, wxGROW|wxALL, 5);
    m_ShowAll = new wxCheckBox( m_Index, ID_CHECKBOX_SCHEDULES_SHOW_ALL, _("Show all"), wxDefaultPosition, wxDefaultSize, 0 );
    m_ShowAll->SetValue(true);
    itemFlexGridSizer7->Add(m_ShowAll, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labStartBlock = new wxStaticText( m_Index, wxID_ANY, _("StartBlock"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer7->Add(m_labStartBlock, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_StartBlockID = new wxTextCtrl( m_Index, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
    m_StartBlockID->Enable(false);
    itemFlexGridSizer7->Add(m_StartBlockID, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer11 = new wxFlexGridSizer(1, 3, 0, 0);
    itemBoxSizer5->Add(itemFlexGridSizer11, 0, wxGROW|wxALL, 5);
    m_New = new wxButton( m_Index, ID_BUTTON_SCHEDULE_NEW, _("New"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer11->Add(m_New, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Delete = new wxButton( m_Index, ID_BUTTON_SCHEDULE_DELETE, _("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer11->Add(m_Delete, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Doc = new wxButton( m_Index, ID_BUTTON_SC_DOC, _("Doc"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer11->Add(m_Doc, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_NoteBook->AddPage(m_Index, _("Index"));

    m_Destinations = new wxPanel( m_NoteBook, ID_PANEL_SCHEDULE_DESTINATIONS, wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer16 = new wxBoxSizer(wxVERTICAL);
    m_Destinations->SetSizer(itemBoxSizer16);

    wxFlexGridSizer* itemFlexGridSizer17 = new wxFlexGridSizer(1, 2, 0, 0);
    itemFlexGridSizer17->AddGrowableRow(1);
    itemFlexGridSizer17->AddGrowableCol(1);
    itemBoxSizer16->Add(itemFlexGridSizer17, 0, wxGROW|wxALL, 5);
    m_LabelID = new wxStaticText( m_Destinations, wxID_STATIC_SCHEDULE_ID, _("ID"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer17->Add(m_LabelID, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    m_ID = new wxTextCtrl( m_Destinations, ID_TEXTCTRL_SCHEDULE_ID, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer17->Add(m_ID, 1, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_RelativeTime = new wxCheckBox( m_Destinations, wxID_ANY, _("Relative time"), wxDefaultPosition, wxDefaultSize, 0 );
    m_RelativeTime->SetValue(false);
    itemBoxSizer16->Add(m_RelativeTime, 0, wxALIGN_LEFT|wxALL, 5);

    m_Entries = new wxGrid( m_Destinations, ID_GRID_SCHEDULE, wxDefaultPosition, wxSize(-1, 150), wxSUNKEN_BORDER|wxVSCROLL );
    m_Entries->SetDefaultColSize(50);
    m_Entries->SetDefaultRowSize(25);
    m_Entries->SetColLabelSize(25);
    m_Entries->SetRowLabelSize(50);
    m_Entries->CreateGrid(1, 3, wxGrid::wxGridSelectRows);
    itemBoxSizer16->Add(m_Entries, 2, wxGROW|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer22 = new wxFlexGridSizer(2, 2, 0, 0);
    itemBoxSizer16->Add(itemFlexGridSizer22, 0, wxGROW|wxALL, 5);
    m_LabelLocation = new wxStaticText( m_Destinations, ID_STATICTEXT_SCHEDULE_FROM, _("Location"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer22->Add(m_LabelLocation, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    m_LabelBlock = new wxStaticText( m_Destinations, ID_STATICTEXT_SCHEDULE_TO, _("Block"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer22->Add(m_LabelBlock, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    wxArrayString m_LocationStrings;
    m_Location = new wxComboBox( m_Destinations, ID_COMBOBOX_SCHEDULE_FROM_LOCATION, _T(""), wxDefaultPosition, wxDefaultSize, m_LocationStrings, wxCB_READONLY );
    itemFlexGridSizer22->Add(m_Location, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    wxArrayString m_BlockStrings;
    m_Block = new wxComboBox( m_Destinations, ID_COMBOBOX_SCHEDULE_TO_LOCATION, _T(""), wxDefaultPosition, wxDefaultSize, m_BlockStrings, wxCB_READONLY );
    itemFlexGridSizer22->Add(m_Block, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_AddLocation = new wxButton( m_Destinations, wxID_BUTTON_SCHEDULE_ADD_LOCATION, _("Add"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer22->Add(m_AddLocation, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_AddBlock = new wxButton( m_Destinations, wxID_BUTTON_SCHEDULE_ADD_BLOCK, _("Add"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer22->Add(m_AddBlock, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    wxBoxSizer* itemBoxSizer29 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer16->Add(itemBoxSizer29, 0, wxGROW|wxALL, 5);
    wxBoxSizer* itemBoxSizer30 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer29->Add(itemBoxSizer30, 0, wxALIGN_CENTER_VERTICAL, 5);
    m_RemoveDestination = new wxButton( m_Destinations, wxID_BUTTON_SCHEDULE_REMOVE_DESTINATION, _("Remove"), wxDefaultPosition, wxDefaultSize, 0 );
    m_RemoveDestination->SetDefault();
    itemBoxSizer30->Add(m_RemoveDestination, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP, 5);

    m_ModifyDestination = new wxButton( m_Destinations, wxID_BUTTON_SCHEDULE_MODIFY_DESTINATION, _("Modify"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer30->Add(m_ModifyDestination, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT, 5);

    m_EntryActions = new wxButton( m_Destinations, wxID_BUTTON_SCHEDULE_ENTRY_ACTIONS, _("Actions..."), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer30->Add(m_EntryActions, 0, wxALIGN_CENTER_HORIZONTAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    wxFlexGridSizer* itemFlexGridSizer34 = new wxFlexGridSizer(2, 1, 0, 0);
    itemBoxSizer29->Add(itemFlexGridSizer34, 0, wxALIGN_TOP|wxALL, 5);
    m_labDeparture = new wxStaticText( m_Destinations, wxID_ANY, _("Departure"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer34->Add(m_labDeparture, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    wxFlexGridSizer* itemFlexGridSizer36 = new wxFlexGridSizer(2, 2, 0, 0);
    itemFlexGridSizer34->Add(itemFlexGridSizer36, 0, wxALIGN_LEFT|wxALIGN_TOP, 5);
    m_labHour = new wxStaticText( m_Destinations, wxID_ANY, _("hour"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer36->Add(m_labHour, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    m_labMinute = new wxStaticText( m_Destinations, wxID_ANY, _("minute"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer36->Add(m_labMinute, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    m_Hour = new wxSpinCtrl( m_Destinations, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(50, -1), wxSP_ARROW_KEYS, 0, 23, 0 );
    itemFlexGridSizer36->Add(m_Hour, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_Minute = new wxSpinCtrl( m_Destinations, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(50, -1), wxSP_ARROW_KEYS, 0, 59, 0 );
    itemFlexGridSizer36->Add(m_Minute, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_EntryDetails = new wxStaticBox(m_Destinations, wxID_ANY, _("Details"));
    wxStaticBoxSizer* itemStaticBoxSizer41 = new wxStaticBoxSizer(m_EntryDetails, wxVERTICAL);
    itemBoxSizer29->Add(itemStaticBoxSizer41, 0, wxALIGN_TOP|wxALL, 5);
    m_EntrySwap = new wxCheckBox( m_Destinations, wxID_ANY, _("Swap placing"), wxDefaultPosition, wxDefaultSize, 0 );
    m_EntrySwap->SetValue(false);
    itemStaticBoxSizer41->Add(m_EntrySwap, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    m_NoteBook->AddPage(m_Destinations, _("Destinations"));

    m_ScheduleActions = new wxPanel( m_NoteBook, ID_PANEL_SCHEDULES_ACTIONS, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer44 = new wxBoxSizer(wxVERTICAL);
    m_ScheduleActions->SetSizer(itemBoxSizer44);

    m_ScheduleBox = new wxStaticBox(m_ScheduleActions, wxID_ANY, _("Schedule"));
    wxStaticBoxSizer* itemStaticBoxSizer45 = new wxStaticBoxSizer(m_ScheduleBox, wxHORIZONTAL);
    itemBoxSizer44->Add(itemStaticBoxSizer45, 0, wxGROW|wxALL, 5);
    wxArrayString m_ScheduleActionStrings;
    m_ScheduleAction = new wxComboBox( m_ScheduleActions, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, m_ScheduleActionStrings, wxCB_DROPDOWN );
    itemStaticBoxSizer45->Add(m_ScheduleAction, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Actions = new wxButton( m_ScheduleActions, ID_SCHEDULE_ACTIONS, _("Actions..."), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer44->Add(m_Actions, 0, wxALIGN_LEFT|wxALL, 5);

    m_NoteBook->AddPage(m_ScheduleActions, _("Actions"));

    itemBoxSizer2->Add(m_NoteBook, 1, wxGROW|wxALL, 5);

    wxStdDialogButtonSizer* itemStdDialogButtonSizer48 = new wxStdDialogButtonSizer;

    itemBoxSizer2->Add(itemStdDialogButtonSizer48, 0, wxGROW|wxALL, 5);
    m_OK = new wxButton( itemDialog1, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer48->AddButton(m_OK);

    m_Cancel = new wxButton( itemDialog1, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer48->AddButton(m_Cancel);

    m_Apply = new wxButton( itemDialog1, wxID_APPLY, _("&Apply"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer48->AddButton(m_Apply);

    itemStdDialogButtonSizer48->Realize();

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


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_SCHEDULE_NEW
 */

void ScheduleDialog::OnButtonScheduleNewClick( wxCommandEvent& event )
{
  int i = m_List->FindString( _T("NEW") );
  if( i == wxNOT_FOUND ) {
    m_List->Append( _T("NEW") );
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
  m_List->SetStringSelection( _T("NEW") );
  m_List->SetFirstItem( _T("NEW") );
  initSchedule();
}


/*!
 * wxEVT_COMMAND_LISTBOX_SELECTED event handler for ID_LISTBOX_SCHEDULE_LIST
 */

void ScheduleDialog::OnListboxScheduleListSelected( wxCommandEvent& event )
{
  m_Props = (iONode)m_List->GetClientData(m_List->GetSelection() );
  initSchedule();
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_APPLY
 */

void ScheduleDialog::OnApplyClick( wxCommandEvent& event )
{
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

  iONode scentry = NodeOp.inst( wScheduleEntry.name(), m_Props, ELEMENT_NODE );
  wScheduleEntry.setlocation( scentry, m_Location->GetStringSelection().mb_str(wxConvUTF8) );
  wScheduleEntry.sethour( scentry, m_Hour->GetValue() );
  wScheduleEntry.setminute( scentry, m_Minute->GetValue() );
  wScheduleEntry.setswap( scentry, m_EntrySwap->IsChecked()?True:False );
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

  iONode scentry = NodeOp.inst( wScheduleEntry.name(), m_Props, ELEMENT_NODE );
  wScheduleEntry.setblock( scentry, m_Block->GetStringSelection().mb_str(wxConvUTF8) );
  wScheduleEntry.sethour( scentry, m_Hour->GetValue() );
  wScheduleEntry.setminute( scentry, m_Minute->GetValue() );
  wScheduleEntry.setswap( scentry, m_EntrySwap->IsChecked()?True:False );
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
}

