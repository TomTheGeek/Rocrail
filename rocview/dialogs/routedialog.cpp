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
#if defined(__GNUG__) && !defined(__APPLE__)
#pragma implementation "routedialog.h"
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
////@end includes

#include "routedialog.h"
#include "actionsctrldlg.h"

////@begin XPM images
////@end XPM images

#include "rocview/public/guiapp.h"
#include "rocrail/wrapper/public/Route.h"
#include "rocrail/wrapper/public/RouteList.h"
#include "rocrail/wrapper/public/Switch.h"
#include "rocrail/wrapper/public/SwitchCmd.h"
#include "rocrail/wrapper/public/Output.h"
#include "rocrail/wrapper/public/Block.h"
#include "rocrail/wrapper/public/Item.h"
#include "rocrail/wrapper/public/Plan.h"
#include "rocrail/wrapper/public/ModelCmd.h"
#include "rocrail/wrapper/public/Turntable.h"
#include "rocrail/wrapper/public/SelTab.h"
#include "rocrail/wrapper/public/FeedbackEvent.h"
#include "rocrail/wrapper/public/Feedback.h"
#include "rocrail/wrapper/public/PermInclude.h"
#include "rocrail/wrapper/public/PermExclude.h"
#include "rocrail/wrapper/public/Loc.h"

/*!
 * RouteDialog type definition
 */

IMPLEMENT_DYNAMIC_CLASS( RouteDialog, wxDialog )

/*!
 * RouteDialog event table definition
 */

BEGIN_EVENT_TABLE( RouteDialog, wxDialog )

////@begin RouteDialog event table entries
    EVT_LISTBOX( ID_LISTBOX_ST, RouteDialog::OnListboxStSelected )

    EVT_BUTTON( ID_BUTTON_ST_NEW, RouteDialog::OnButtonStNewClick )

    EVT_BUTTON( ID_BUTTON_ST_DELETE_ROUTE, RouteDialog::OnButtonStDeleteRouteClick )

    EVT_BUTTON( ID_BUTTON_ST_TEST, RouteDialog::OnButtonStTestClick )

    EVT_BUTTON( ID_BUTTON_ST_DOC, RouteDialog::OnButtonStDocClick )

    EVT_RADIOBOX( ID_ROUTE_VELOCITY, RouteDialog::OnRouteVelocitySelected )

    EVT_BUTTON( ID_ROUTE_ACTIONS, RouteDialog::OnRouteActionsClick )

    EVT_LISTBOX( ID_LISTBOX_COMMANDS, RouteDialog::OnListboxCommandsSelected )

    EVT_BUTTON( ID_BUTTON_ST_DELETE, RouteDialog::OnButtonTurnoutDeleteClick )

    EVT_BUTTON( ID_BUTTON_ST_MODIFY, RouteDialog::OnButtonTurnoutModifyClick )

    EVT_COMBOBOX( ID_COMBOBOX_ST_SWITCH_ID, RouteDialog::OnComboboxStSwitchIdSelected )

    EVT_BUTTON( ID_BUTTON_ST_ADD, RouteDialog::OnButtonTurnoutAddClick )

    EVT_LISTBOX( ID_LIST_ROUTE_SENSORS, RouteDialog::OnListRouteSensorsSelected )

    EVT_BUTTON( ID_BUTTON_ROUTES_ADD_SENSOR, RouteDialog::OnButtonRoutesAddSensorClick )

    EVT_BUTTON( ID_BUTTON_ROUTES_DEL_SENSOR, RouteDialog::OnButtonRoutesDelSensorClick )

    EVT_BUTTON( wxID_CANCEL, RouteDialog::OnCancelClick )

    EVT_BUTTON( wxID_OK, RouteDialog::OnOkClick )

    EVT_BUTTON( wxID_APPLY, RouteDialog::OnApplyClick )

////@end RouteDialog event table entries
    EVT_MENU( ID_PANEL_ST_GENERAL, RouteDialog::OnSelectPage )

END_EVENT_TABLE()

/*!
 * RouteDialog constructors
 */


/* comparator for sorting by id: */
static int __sortStr(obj* _a, obj* _b)
{
    const char* a = (const char*)*_a;
    const char* b = (const char*)*_b;
    return strcmp( a, b );
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

RouteDialog::RouteDialog()
{
}

RouteDialog::RouteDialog( wxWindow* parent, iONode p_Props )
{
  m_TabAlign = wxGetApp().getTabAlign();
  Create(parent, -1, wxGetApp().getMsg("routes") );
  m_Props = p_Props;

  initLabels();

  m_IndexPanel->GetSizer()->Layout();
  m_GeneralPanel->GetSizer()->Layout();
  m_CommandPanel->GetSizer()->Layout();
  m_SensorPanel->GetSizer()->Layout();
  m_PermissionsPanel->GetSizer()->Layout();
  m_LocationPanel->GetSizer()->Layout();
  m_WiringPanel->GetSizer()->Layout();


  m_Notebook->Fit();
  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);

  initIndex();

  if( m_Props != NULL ) {
    initValues();
    //m_Notebook->SetSelection( 1 );
    wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, ID_PANEL_ST_GENERAL );
    wxPostEvent( this, event );
  }

}

void RouteDialog::OnSelectPage(wxCommandEvent& event) {
  m_Notebook->SetSelection( 1 );
}


void RouteDialog::initLabels() {
  m_Notebook->SetPageText( 0, wxGetApp().getMsg( "index" ) );
  m_Notebook->SetPageText( 1, wxGetApp().getMsg( "general" ) );
  m_Notebook->SetPageText( 2, wxGetApp().getMsg( "turnouttable" ) );
  m_Notebook->SetPageText( 3, wxGetApp().getMsg( "sensors" ) );
  m_Notebook->SetPageText( 4, wxGetApp().getMsg( "permissions" ) );
  m_Notebook->SetPageText( 5, wxGetApp().getMsg( "location" ) );
  m_Notebook->SetPageText( 6, wxGetApp().getMsg( "wiring" ) );

  // Index
  m_New->SetLabel( wxGetApp().getMsg( "new" ) );
  m_DeleteRoute->SetLabel( wxGetApp().getMsg( "delete" ) );
  m_Test->SetLabel( wxGetApp().getMsg( "test" ) );
  m_Doc->SetLabel( wxGetApp().getMsg( "doc_report" ) );

  // General
  m_LabelId->SetLabel( wxGetApp().getMsg( "id" ) );
  m_LabelBlockA->SetLabel( wxGetApp().getMsg( "fromblock" ) );
  m_LabelBlockB->SetLabel( wxGetApp().getMsg( "toblock" ) );
  m_Dir->SetLabel( wxGetApp().getMsg( "usage" ) );
  m_Dir->SetString( 0, wxGetApp().getMsg( "fromto" ) );
  m_Dir->SetString( 1, wxGetApp().getMsg( "dirall" ) );
  m_RunDir->SetLabel( wxGetApp().getMsg( "rundirection" ) );
  m_RunDir->SetString( 0, wxGetApp().getMsg( "forwards" ) );
  m_RunDir->SetString( 1, wxGetApp().getMsg( "reverse" ) );
  m_labCrossingBlock->SetLabel( wxGetApp().getMsg( "crossingblocks" ) );
  m_BlockC->SetToolTip( wxGetApp().getTip( "crossingblocks" ) );
  m_labModID->SetLabel( wxGetApp().getMsg( "modid" ) );

  m_FromSignals->SetLabel( wxGetApp().getMsg( "fromsignals" ) );
  m_FromSignals->SetString( 0, wxGetApp().getMsg( "forwards" ) );
  m_FromSignals->SetString( 1, wxGetApp().getMsg( "reverse" ) );
  m_ToSignals->SetLabel( wxGetApp().getMsg( "tosignals" ) );
  m_ToSignals->SetString( 0, wxGetApp().getMsg( "forwards" ) );
  m_ToSignals->SetString( 1, wxGetApp().getMsg( "reverse" ) );



  m_Speed->SetLabel( wxGetApp().getMsg( "speed" ) );
  m_Speed->SetString( 0, wxGetApp().getMsg( "block" ) );
  m_Speed->SetString( 1, wxGetApp().getMsg( "min" ) );
  m_Speed->SetString( 2, wxGetApp().getMsg( "mid" ) );
  m_Speed->SetString( 3, wxGetApp().getMsg( "cruise" ) );
  m_Speed->SetString( 4, wxGetApp().getMsg( "max" ) );
  m_Speed->SetString( 5, _T("%") );
  m_ReduceV->SetLabel( wxGetApp().getMsg( "reducespeed" ) );

  m_DetailsBox->SetLabel( wxGetApp().getMsg( "options" ) );
  m_Swap->SetLabel( wxGetApp().getMsg( "swapplacing" ) );
  m_SwapPost->SetLabel( wxGetApp().getMsg( "swapplacingpost" ) );
  m_CrossingblockSignals->SetLabel( wxGetApp().getMsg( "crossingblocksignals" ) );
  m_Actions->SetLabel( wxGetApp().getMsg( "actions" )+_T("...") );
  m_Show->SetLabel( wxGetApp().getMsg( "show" ) );
  m_Manual->SetLabel( wxGetApp().getMsg( "manual" ) );

  // Commands
  m_Add->SetLabel( wxGetApp().getMsg( "add" ) );
  m_Delete->SetLabel( wxGetApp().getMsg( "delete" ) );
  m_Modify->SetLabel( wxGetApp().getMsg( "modify" ) );
  m_LabelSwitchId->SetLabel( wxGetApp().getMsg( "turnoutid" ) );
  m_labTrackNumber->SetLabel( wxGetApp().getMsg( "tracknr" ) );
  m_SwitchCmd->SetLabel( wxGetApp().getMsg( "command" ) );
  m_SwitchCmd->SetString( 0, wxGetApp().getMsg( "straight" ) );
  m_SwitchCmd->SetString( 1, wxGetApp().getMsg( "thrown" ) );
  m_SwitchCmd->SetString( 2, wxGetApp().getMsg( "left" ) );
  m_SwitchCmd->SetString( 3, wxGetApp().getMsg( "right" ) );
  m_SwitchCmd->SetString( 4, wxGetApp().getMsg( "track" ) );
  m_Lock->SetLabel( wxGetApp().getMsg( "lock" ) );

  // Sensors
  m_AddSensor->SetLabel( wxGetApp().getMsg( "add" ) );
  m_DelSensor->SetLabel( wxGetApp().getMsg( "delete" ) );

  iONode model = wxGetApp().getModel();
  iOList list = ListOp.inst();
  if( model != NULL ) {
    iONode fblist = wPlan.getfblist( model );
    if( fblist != NULL ) {
      int cnt = NodeOp.getChildCnt( fblist );
      for( int i = 0; i < cnt; i++ ) {
        iONode fb = NodeOp.getChild( fblist, i );
        const char* id = wFeedback.getid( fb );
        if( id != NULL ) {
          ListOp.add(list, (obj)id);
        }
      }
      ListOp.sort(list, &__sortStr);
      cnt = ListOp.size( list );
      for( int i = 0; i < cnt; i++ ) {
        const char* id = (const char*)ListOp.get( list, i );
        m_SensorCombo->Append( wxString(id,wxConvUTF8) );
      }
    }
  }
  /* clean up the temp. list */
  ListOp.base.del(list);

  // Permissions
  m_labInclude->SetLabel( wxGetApp().getMsg( "include" ) );
  m_labExclude->SetLabel( wxGetApp().getMsg( "exclude" ) );

  m_PermType->SetString( 0, wxGetApp().getMsg( "all" ) );
  m_PermType->SetString( 1, wxGetApp().getMsg( "none" ) );
  m_PermType->SetString( 2, wxGetApp().getMsg( "goods" ) );
  m_PermType->SetString( 3, wxGetApp().getMsg( "local" ) );
  m_PermType->SetString( 4, wxGetApp().getMsg( "mixed" ) );
  m_PermType->SetString( 5, wxGetApp().getMsg( "cleaning" ) );
  m_PermType->SetString( 6, wxGetApp().getMsg( "ice" ) );
  m_PermType->SetString( 7, wxGetApp().getMsg( "post" ) );

  // Initialize sorted Loco Permission List
  initLocPermissionList();

  m_labMaxLen->SetLabel( wxGetApp().getMsg( "maxtrainlen" ) );

  // Location
  m_LabelX->SetLabel( wxGetApp().getMsg( "x" ) );
  m_LabelY->SetLabel( wxGetApp().getMsg( "y" ) );
  m_LabelZ->SetLabel( wxGetApp().getMsg( "z" ) );
  m_ori->SetLabel( wxGetApp().getMsg( "orientation" ) );
  m_ori->SetString( 0, wxGetApp().getMsg( "north" ) );
  m_ori->SetString( 1, wxGetApp().getMsg( "east" ) );
  m_ori->SetString( 2, wxGetApp().getMsg( "south" ) );
  m_ori->SetString( 3, wxGetApp().getMsg( "west" ) );

  // Wiring
  m_labCTCbutton->SetLabel( wxGetApp().getMsg( "button" ) );
  m_labCTCIID->SetLabel( wxGetApp().getMsg( "iid" ) );
  m_labCTCAddr->SetLabel( wxGetApp().getMsg( "address" ) );


  // Buttons
  m_OK->SetLabel( wxGetApp().getMsg( "ok" ) );
  m_Cancel->SetLabel( wxGetApp().getMsg( "cancel" ) );
  m_Apply->SetLabel( wxGetApp().getMsg( "apply" ) );
}


void RouteDialog::initLocPermissionList() {
  iONode model = wxGetApp().getModel();
  iOList list = ListOp.inst();

  if( model != NULL ) {
    iONode lclist = wPlan.getlclist( model );
    if( lclist != NULL ) {
      int cnt = NodeOp.getChildCnt( lclist );
      for( int i = 0; i < cnt; i++ ) {
        iONode lc = NodeOp.getChild( lclist, i );
        const char* id = wLoc.getid( lc );
        if( id != NULL ) {
          ListOp.add(list, (obj)id);
    }
    }
      ListOp.sort(list, &__sortStr);
      cnt = ListOp.size( list );
      for( int i = 0; i < cnt; i++ ) {
        const char* id = (const char*)ListOp.get( list, i );
        m_IncludeList->Append( wxString(id,wxConvUTF8) );
        m_ExcludeList->Append( wxString(id,wxConvUTF8) );
      }
    }
  }
  /* clean up the temp. list */
  ListOp.base.del(list);
}



void RouteDialog::initIndex() {
  TraceOp.trc( "app", TRCLEVEL_INFO, __LINE__, 9999, "InitIndex" );
  iONode l_Props = m_Props;
  m_List->Clear();
  iONode model = wxGetApp().getModel();
  if( model != NULL ) {
    iONode stlist = wPlan.getstlist( model );
    if( stlist != NULL ) {
      int cnt = NodeOp.getChildCnt( stlist );
      for( int i = 0; i < cnt; i++ ) {
        iONode st = NodeOp.getChild( stlist, i );
        const char* id = wRoute.getid( st );
        if( id != NULL ) {
          m_List->Append( wxString(id,wxConvUTF8) );
        }
      }
      if( l_Props != NULL ) {
        m_List->SetStringSelection( wxString(wRoute.getid( l_Props ),wxConvUTF8) );
        m_List->SetFirstItem( wxString(wRoute.getid( l_Props ),wxConvUTF8) );
        m_Props = l_Props;
        char* title = StrOp.fmt( "%s %s", (const char*)wxGetApp().getMsg("route").mb_str(wxConvUTF8), wRoute.getid( l_Props ) );
        SetTitle( wxString(title,wxConvUTF8) );
        StrOp.free( title );

      }

    }
  }
}


void RouteDialog::initBlockCombos() {
  m_BlockA->Clear();
  m_BlockB->Clear();
  m_BlockA->Append( _T("<undef>"), (void*)NULL );
  m_BlockB->Append( _T("<undef>"), (void*)NULL );

  iONode model = wxGetApp().getModel();
  iOList list = ListOp.inst();

  if( model != NULL ) {
    iONode bklist = wPlan.getbklist( model );
    iONode seltablist = wPlan.getseltablist( model );
    if( bklist != NULL ) {
      int cnt = NodeOp.getChildCnt( bklist );
      for( int i = 0; i < cnt; i++ ) {
        iONode bk = NodeOp.getChild( bklist, i );
        const char* id = wBlock.getid( bk );
        if( id != NULL ) {
          ListOp.add(list, (obj)id);
        }
      }

      if(seltablist != NULL) {
        cnt = NodeOp.getChildCnt( seltablist );
        for( int i = 0; i < cnt; i++ ) {
          iONode seltab = NodeOp.getChild( seltablist, i );
          const char* id = wSelTab.getid( seltab );
          if( id != NULL ) {
            ListOp.add(list, (obj)id);
          }
        }
      }

      ListOp.sort(list, &__sortStr);
      cnt = ListOp.size( list );
      for( int i = 0; i < cnt; i++ ) {
        const char* id = (const char*)ListOp.get( list, i );
        m_BlockA->Append( wxString(id,wxConvUTF8) );
        m_BlockB->Append( wxString(id,wxConvUTF8) );
      }
    }
		m_BlockA->Append( wxString( wRoute.point_en,wxConvUTF8) );
		m_BlockA->Append( wxString( wRoute.point_es,wxConvUTF8) );
		m_BlockA->Append( wxString( wRoute.point_ne,wxConvUTF8) );
		m_BlockA->Append( wxString( wRoute.point_nw,wxConvUTF8) );
		m_BlockA->Append( wxString( wRoute.point_se,wxConvUTF8) );
		m_BlockA->Append( wxString( wRoute.point_sw,wxConvUTF8) );
		m_BlockA->Append( wxString( wRoute.point_wn,wxConvUTF8) );
		m_BlockA->Append( wxString( wRoute.point_ws,wxConvUTF8) );
		
		// for single track modules
		m_BlockA->Append( wxString( wRoute.point_e,wxConvUTF8) );
		m_BlockA->Append( wxString( wRoute.point_n,wxConvUTF8) );
		m_BlockA->Append( wxString( wRoute.point_s,wxConvUTF8) );
		m_BlockA->Append( wxString( wRoute.point_w,wxConvUTF8) );
		
		m_BlockB->Append( wxString( wRoute.point_en,wxConvUTF8) );
		m_BlockB->Append( wxString( wRoute.point_es,wxConvUTF8) );
		m_BlockB->Append( wxString( wRoute.point_ne,wxConvUTF8) );
		m_BlockB->Append( wxString( wRoute.point_nw,wxConvUTF8) );
		m_BlockB->Append( wxString( wRoute.point_se,wxConvUTF8) );
		m_BlockB->Append( wxString( wRoute.point_sw,wxConvUTF8) );
		m_BlockB->Append( wxString( wRoute.point_wn,wxConvUTF8) );
		m_BlockB->Append( wxString( wRoute.point_ws,wxConvUTF8) );
		
		// for single track modules
		m_BlockB->Append( wxString( wRoute.point_e,wxConvUTF8) );
		m_BlockB->Append( wxString( wRoute.point_n,wxConvUTF8) );
		m_BlockB->Append( wxString( wRoute.point_s,wxConvUTF8) );
		m_BlockB->Append( wxString( wRoute.point_w,wxConvUTF8) );
		
  }
  /* clean up the temp. list */
  ListOp.base.del(list);
}

void RouteDialog::initSwitchCombo() {
  m_SwitchId->Clear();
  m_SwitchId->Append( _T("<undef>"), (void*)NULL );

  iONode model = wxGetApp().getModel();
  iOList list = ListOp.inst();

  if( model != NULL ) {
    iONode swlist = wPlan.getswlist( model );
    if( swlist != NULL ) {
      int cnt = NodeOp.getChildCnt( swlist );
      for( int i = 0; i < cnt; i++ ) {
        iONode sw = NodeOp.getChild( swlist, i );
        const char* id = wSwitch.getid( sw );
        if( id != NULL ) {
          ListOp.add(list, (obj)sw);
		    }
	    }
    }
    iONode ttlist = wPlan.getttlist( model );
    if( ttlist != NULL ) {
      int cnt = NodeOp.getChildCnt( ttlist );
      for( int i = 0; i < cnt; i++ ) {
        iONode tt = NodeOp.getChild( ttlist, i );
        const char* id = wItem.getid( tt );
        if( id != NULL ) {
          ListOp.add(list, (obj)tt);
        }
      }
    }
    iONode seltablist = wPlan.getseltablist( model );
    if( seltablist != NULL ) {
      int cnt = NodeOp.getChildCnt( seltablist );
      for( int i = 0; i < cnt; i++ ) {
        iONode seltab = NodeOp.getChild( seltablist, i );
        const char* id = wItem.getid( seltab );
        if( id != NULL ) {
          ListOp.add(list, (obj)seltab);
        }
      }
    }

    ListOp.sort(list, &__sortID);
    int cnt = ListOp.size( list );
    for( int i = 0; i < cnt; i++ ) {
      iONode sw = (iONode)ListOp.get( list, i );
      const char* id = wItem.getid(sw);
      m_SwitchId->Append( wxString(id,wxConvUTF8), sw );
    }

  }
  /* clean up the temp. list */
  ListOp.base.del(list);
}

void RouteDialog::initValues() {
  char* title = StrOp.fmt( "%s %s", (const char*)wxGetApp().getMsg("route").mb_str(wxConvUTF8), wRoute.getid( m_Props ) );
  SetTitle( wxString(title,wxConvUTF8) );
  StrOp.free( title );

  m_Id->SetValue( wxString(wRoute.getid( m_Props ),wxConvUTF8) );

  // Initialize Block Comboboxes
  initBlockCombos();

//  iONode model = wxGetApp().getModel();

  m_BlockA->SetStringSelection( wRoute.getbka( m_Props ) == NULL ?
                                _T("<undef>"):wxString(wRoute.getbka( m_Props ),wxConvUTF8)  );
  m_BlockB->SetStringSelection( wRoute.getbkb( m_Props ) == NULL ?
                                _T("<undef>"):wxString(wRoute.getbkb( m_Props ),wxConvUTF8)  );
  m_BlockC->SetValue( wxString(wRoute.getbkc( m_Props ),wxConvUTF8)  );

  m_ModID->SetValue( wxString(wRoute.getmodid( m_Props ),wxConvUTF8)  );

  int dir = wRoute.isdir( m_Props ) ? 0:1;
  m_Dir->SetSelection(dir);
  dir = wRoute.islcdir( m_Props ) ? 0:1;
  m_RunDir->SetSelection(dir);

  m_FromSignals->SetSelection(wRoute.getsga( m_Props ));
  m_ToSignals->SetSelection(wRoute.getsgb( m_Props ));

  int speed = 0; // none
  if( StrOp.equals( wBlock.min, wRoute.getspeed( m_Props ) ) )
    speed = 1;
  else if( StrOp.equals( wBlock.mid, wRoute.getspeed( m_Props ) ) )
    speed = 2;
  else if( StrOp.equals( wBlock.cruise, wRoute.getspeed( m_Props ) ) )
    speed = 3;
  else if( StrOp.equals( wBlock.max, wRoute.getspeed( m_Props ) ) )
    speed = 4;
  else if( StrOp.equals( wBlock.percent, wRoute.getspeed( m_Props ) ) )
    speed = 5;
  m_Speed->SetSelection( speed );
  char * str;
  str = StrOp.fmt( "%d", wRoute.getspeedpercent(m_Props) );
  m_SpeedPercent->SetValue( wxString(str,wxConvUTF8) ); StrOp.free( str );

  m_ReduceV->SetValue(wRoute.isreduceV( m_Props ));

  m_Swap->SetValue(wRoute.isswap( m_Props ));

  m_SwapPost->SetValue(wRoute.isswappost( m_Props ));
  m_CrossingblockSignals->SetValue(wRoute.iscrossingblocksignals( m_Props ));
  m_Show->SetValue(wRoute.isshow( m_Props ));
  m_Manual->SetValue(wRoute.ismanual( m_Props ));

  initCommands();

  // Initialize sorted Switch Combo
  initSwitchCombo();

  m_SwitchCmd->SetSelection(1);
  m_SwitchCmd->Enable( 2, false );
  m_SwitchCmd->Enable( 3, false );
  m_SwitchCmd->Enable( 4, false );

  m_Lock->SetValue(true);

  // Sensors
  m_SensorList->Clear();
  iONode fb = wRoute.getfbevent( m_Props );
  while( fb != NULL ) {
    m_SensorList->Append( wxString(wFeedbackEvent.getid( fb ),wxConvUTF8), fb );
    fb = wRoute.nextfbevent( m_Props, fb );
  };

  // Permissions

  // remove selections:
  wxArrayInt ai;
  int cnt = m_ExcludeList->GetSelections(ai);
  for( int i = 0; i < cnt; i++ ) {
    int idx = ai.Item(i);
    m_ExcludeList->Deselect(idx);
  }
  cnt = m_IncludeList->GetSelections(ai);
  for( int i = 0; i < cnt; i++ ) {
    int idx = ai.Item(i);
    m_IncludeList->Deselect(idx);
  }

  // set selections:
  iONode excl = wRoute.getexcl( m_Props );
  while( excl != NULL ) {
    int nr = m_ExcludeList->FindString( wxString(wPermExclude.getid(excl),wxConvUTF8) );
    if( nr != wxNOT_FOUND ) {
      m_ExcludeList->Select(nr);
    }
    excl = wRoute.nextexcl( m_Props, excl );
  };

  iONode incl = wRoute.getincl( m_Props );
  while( incl != NULL ) {
    int nr = m_IncludeList->FindString( wxString(wPermInclude.getid(incl),wxConvUTF8) );
    if( nr != wxNOT_FOUND ) {
      m_IncludeList->Select(nr);
    }
    incl = wRoute.nextincl( m_Props, incl );
  };

  int cargo = 0;
  if( StrOp.equals( wLoc.cargo_all, wRoute.gettypeperm( m_Props ) ) )
    cargo = 0;
  else if( StrOp.equals( wLoc.cargo_none, wRoute.gettypeperm( m_Props ) ) )
    cargo = 1;
  else if( StrOp.equals( wLoc.cargo_goods, wRoute.gettypeperm( m_Props ) ) )
    cargo = 2;
  else if( StrOp.equals( wLoc.cargo_person, wRoute.gettypeperm( m_Props ) ) )
    cargo = 3;
  else if( StrOp.equals( wLoc.cargo_mixed, wRoute.gettypeperm( m_Props ) ) )
    cargo = 4;
  else if( StrOp.equals( wLoc.cargo_cleaning, wRoute.gettypeperm( m_Props ) ) )
    cargo = 5;
  else if( StrOp.equals( wLoc.cargo_ice, wRoute.gettypeperm( m_Props ) ) )
    cargo = 6;
  else if( StrOp.equals( wLoc.cargo_post, wRoute.gettypeperm( m_Props ) ) )
    cargo = 7;
  m_PermType->SetSelection( cargo );

  m_MaxLen->SetValue(wRoute.getmaxlen(m_Props));
  
  // Location
  char* val = StrOp.fmt( "%d", wRoute.getx( m_Props ) );
  m_x->SetValue( wxString(val,wxConvUTF8) ); StrOp.free( val );
  val = StrOp.fmt( "%d", wRoute.gety( m_Props ) );
  m_y->SetValue( wxString(val,wxConvUTF8) ); StrOp.free( val );
  val = StrOp.fmt( "%d", wRoute.getz( m_Props ) );
  m_z->SetValue( wxString(val,wxConvUTF8) ); StrOp.free( val );
  if( StrOp.equals( wItem.north, wRoute.getori( m_Props ) ) )
    m_ori->SetSelection( 0 );
  else if( StrOp.equals( wItem.east, wRoute.getori( m_Props ) ) )
    m_ori->SetSelection( 1 );
  else if( StrOp.equals( wItem.south, wRoute.getori( m_Props ) ) )
    m_ori->SetSelection( 2 );
  else
    m_ori->SetSelection( 3 );
  
  // Wiring
  m_CTCIID1->SetValue( wxString(wRoute.getctciid1(m_Props),wxConvUTF8));
  m_CTCIID2->SetValue( wxString(wRoute.getctciid2(m_Props),wxConvUTF8));
  m_CTCIID3->SetValue( wxString(wRoute.getctciid3(m_Props),wxConvUTF8));
  m_CTCAddr1->SetValue( wRoute.getctcaddr1(m_Props));
  m_CTCAddr2->SetValue( wRoute.getctcaddr2(m_Props));
  m_CTCAddr3->SetValue( wRoute.getctcaddr3(m_Props));



}

void RouteDialog::initCommands() {
  m_Commands->Clear();
  iONode swcmd = wRoute.getswcmd( m_Props );
  while( swcmd != NULL ) {
    char* str = StrOp.fmt( "%s %s %d %s",
        wSwitchCmd.getid( swcmd ), wSwitchCmd.getcmd(swcmd), wSwitchCmd.gettrack(swcmd), wSwitchCmd.islock(swcmd)?"lock":"" );
    m_Commands->Append( wxString(str,wxConvUTF8), swcmd );
    StrOp.free( str );
    swcmd = wRoute.nextswcmd( m_Props, swcmd );
  }
  m_Delete->Enable( false );
  m_Modify->Enable( false );

}


bool RouteDialog::evaluate() {
  if( m_Props == NULL )
    return false;

  if( m_Id->GetValue().Len() == 0 ) {
    wxMessageDialog( this, wxGetApp().getMsg("invalidid"), _T("Rocrail"), wxOK | wxICON_ERROR ).ShowModal();
    m_Id->SetValue( wxString(wRoute.getid( m_Props ),wxConvUTF8) );
    return false;
  }

  wItem.setprev_id( m_Props, wItem.getid(m_Props) );
  wRoute.setid( m_Props, m_Id->GetValue().mb_str(wxConvUTF8) );
  if( StrOp.equals( "<undef>", m_BlockA->GetStringSelection().mb_str(wxConvUTF8) ) )
    wRoute.setbka( m_Props, "" );
  else
    wRoute.setbka( m_Props, m_BlockA->GetStringSelection().mb_str(wxConvUTF8) );

  if( StrOp.equals( "<undef>", m_BlockB->GetStringSelection().mb_str(wxConvUTF8) ) )
    wRoute.setbkb( m_Props, "" );
  else
    wRoute.setbkb( m_Props, m_BlockB->GetStringSelection().mb_str(wxConvUTF8) );

  wRoute.setbkc( m_Props, m_BlockC->GetValue().mb_str(wxConvUTF8) );

  wRoute.setmodid( m_Props, m_ModID->GetValue().mb_str(wxConvUTF8) );

  wRoute.setdir( m_Props, m_Dir->GetSelection() == 0 ? True:False );
  wRoute.setlcdir( m_Props, m_RunDir->GetSelection() == 0 ? True:False );

  wRoute.setsga( m_Props, m_FromSignals->GetSelection() );
  wRoute.setsgb( m_Props, m_ToSignals->GetSelection() );


  if( m_Speed->GetSelection() == 1 )
    wRoute.setspeed( m_Props, wBlock.min );
  else if( m_Speed->GetSelection() == 2 )
    wRoute.setspeed( m_Props, wBlock.mid );
  else if( m_Speed->GetSelection() == 3 )
    wRoute.setspeed( m_Props, wBlock.cruise );
  else if( m_Speed->GetSelection() == 4 )
    wRoute.setspeed( m_Props, wBlock.max );
  else if( m_Speed->GetSelection() == 5 ) {
    wRoute.setspeed( m_Props, wBlock.percent );
    wRoute.setspeedpercent( m_Props, m_SpeedPercent->GetValue() );
  }
  else
    wRoute.setspeed( m_Props, wRoute.V_none );

  wRoute.setreduceV( m_Props, m_ReduceV->IsChecked()?True:False);

  wRoute.setswap( m_Props, m_Swap->IsChecked()?True:False);

  wRoute.setswappost( m_Props, m_SwapPost->IsChecked()?True:False);

  wRoute.setcrossingblocksignals( m_Props, m_CrossingblockSignals->IsChecked()?True:False);
  wRoute.setshow( m_Props, m_Show->IsChecked()?True:False);
  wRoute.setmanual( m_Props, m_Manual->IsChecked()?True:False);

  // Permissions
  // remove all excl and incl childnodes:
  iONode incl = wRoute.getincl( m_Props );
  while( incl != NULL ) {
    NodeOp.removeChild( m_Props, incl );
    NodeOp.base.del(incl);
    incl = wRoute.getincl( m_Props );
  };
  iONode excl = wRoute.getexcl( m_Props );
  while( excl != NULL ) {
    NodeOp.removeChild( m_Props, excl );
    NodeOp.base.del(excl);
    excl = wRoute.getexcl( m_Props );
  };

  wxArrayInt ai;
  int cnt = m_ExcludeList->GetSelections(ai);
  for( int i = 0; i < cnt; i++ ) {
    int idx = ai.Item(i);
    excl = NodeOp.inst( wPermExclude.name(), m_Props, ELEMENT_NODE );
    wPermExclude.setid( excl, m_ExcludeList->GetString(idx).mb_str(wxConvUTF8) );
    NodeOp.addChild( m_Props, excl );
  }
  cnt = m_IncludeList->GetSelections(ai);
  for( int i = 0; i < cnt; i++ ) {
    int idx = ai.Item(i);
    incl = NodeOp.inst( wPermInclude.name(), m_Props, ELEMENT_NODE );
    wPermInclude.setid( incl, m_IncludeList->GetString(idx).mb_str(wxConvUTF8) );
    NodeOp.addChild( m_Props, incl );
  }

  int cargo = m_PermType->GetSelection();
  if( cargo == 0 )
    wRoute.settypeperm( m_Props, wLoc.cargo_all );
  else if( cargo == 1 )
    wRoute.settypeperm( m_Props, wLoc.cargo_none );
  else if( cargo == 2 )
    wRoute.settypeperm( m_Props, wLoc.cargo_goods );
  else if( cargo == 3 )
    wRoute.settypeperm( m_Props, wLoc.cargo_person );
  else if( cargo == 4 )
    wRoute.settypeperm( m_Props, wLoc.cargo_mixed );
  else if( cargo == 5 )
    wRoute.settypeperm( m_Props, wLoc.cargo_cleaning );
  else if( cargo == 6 )
    wRoute.settypeperm( m_Props, wLoc.cargo_ice );
  else if( cargo == 7 )
    wRoute.settypeperm( m_Props, wLoc.cargo_post );

  wRoute.setmaxlen(m_Props,m_MaxLen->GetValue());

  // Location
  wRoute.setx( m_Props, atoi( m_x->GetValue().mb_str(wxConvUTF8) ) );
  wRoute.sety( m_Props, atoi( m_y->GetValue().mb_str(wxConvUTF8) ) );
  wRoute.setz( m_Props, atoi( m_z->GetValue().mb_str(wxConvUTF8) ) );
  int ori = m_ori->GetSelection();
  if( ori == 0 )
    wRoute.setori( m_Props, wItem.north );
  else if( ori == 1 )
    wRoute.setori( m_Props, wItem.east );
  else if( ori == 2 )
    wRoute.setori( m_Props, wItem.south );
  else if( ori == 3 )
    wRoute.setori( m_Props, wItem.west );


  // Wiring
  wRoute.setctciid1(m_Props, m_CTCIID1->GetValue().mb_str(wxConvUTF8));
  wRoute.setctciid2(m_Props, m_CTCIID2->GetValue().mb_str(wxConvUTF8));
  wRoute.setctciid3(m_Props, m_CTCIID3->GetValue().mb_str(wxConvUTF8));
  wRoute.setctcaddr1(m_Props, m_CTCAddr1->GetValue());
  wRoute.setctcaddr2(m_Props, m_CTCAddr2->GetValue());
  wRoute.setctcaddr3(m_Props, m_CTCAddr3->GetValue());

  return true;
}

/*!
 * RouteDialog creator
 */

bool RouteDialog::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin RouteDialog member initialisation
    m_Notebook = NULL;
    m_IndexPanel = NULL;
    m_List = NULL;
    m_New = NULL;
    m_DeleteRoute = NULL;
    m_Test = NULL;
    m_Doc = NULL;
    m_GeneralPanel = NULL;
    m_LabelId = NULL;
    m_Id = NULL;
    m_LabelBlockA = NULL;
    m_BlockA = NULL;
    m_LabelBlockB = NULL;
    m_BlockB = NULL;
    m_labCrossingBlock = NULL;
    m_BlockC = NULL;
    m_labModID = NULL;
    m_ModID = NULL;
    m_Speed = NULL;
    m_SpeedPercent = NULL;
    m_ReduceV = NULL;
    m_Dir = NULL;
    m_FromSignals = NULL;
    m_RunDir = NULL;
    m_ToSignals = NULL;
    m_DetailsBox = NULL;
    m_Swap = NULL;
    m_SwapPost = NULL;
    m_CrossingblockSignals = NULL;
    m_Show = NULL;
    m_Manual = NULL;
    m_Actions = NULL;
    m_CommandPanel = NULL;
    m_Commands = NULL;
    m_Delete = NULL;
    m_Modify = NULL;
    m_LabelSwitchId = NULL;
    m_SwitchId = NULL;
    m_labTrackNumber = NULL;
    m_TrackNumber = NULL;
    m_Add = NULL;
    m_SwitchCmd = NULL;
    m_Lock = NULL;
    m_SensorPanel = NULL;
    m_SensorList = NULL;
    m_SensorCombo = NULL;
    m_AddSensor = NULL;
    m_DelSensor = NULL;
    m_PermissionsPanel = NULL;
    m_labInclude = NULL;
    m_IncludeList = NULL;
    m_labExclude = NULL;
    m_ExcludeList = NULL;
    m_PermType = NULL;
    m_labMaxLen = NULL;
    m_MaxLen = NULL;
    m_LocationPanel = NULL;
    m_LabelX = NULL;
    m_x = NULL;
    m_LabelY = NULL;
    m_y = NULL;
    m_LabelZ = NULL;
    m_z = NULL;
    m_ori = NULL;
    m_WiringPanel = NULL;
    m_CTCBox = NULL;
    m_labCTCbutton = NULL;
    m_labCTCIID = NULL;
    m_labCTCAddr = NULL;
    m_labCTCButton1 = NULL;
    m_CTCIID1 = NULL;
    m_CTCAddr1 = NULL;
    m_labCTCButton2 = NULL;
    m_CTCIID2 = NULL;
    m_CTCAddr2 = NULL;
    m_labCTCButton3 = NULL;
    m_CTCIID3 = NULL;
    m_CTCAddr3 = NULL;
    m_Cancel = NULL;
    m_OK = NULL;
    m_Apply = NULL;
////@end RouteDialog member initialisation

////@begin RouteDialog creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end RouteDialog creation
    return true;
}

/*!
 * Control creation for RouteDialog
 */

void RouteDialog::CreateControls()
{
////@begin RouteDialog content construction
    RouteDialog* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    m_Notebook = new wxNotebook( itemDialog1, ID_NOTEBOOK_STREET, wxDefaultPosition, wxDefaultSize, m_TabAlign );

    m_IndexPanel = new wxPanel( m_Notebook, ID_PANEL_ST_INDEX, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxVERTICAL);
    m_IndexPanel->SetSizer(itemBoxSizer5);

    wxArrayString m_ListStrings;
    m_List = new wxListBox( m_IndexPanel, ID_LISTBOX_ST, wxDefaultPosition, wxDefaultSize, m_ListStrings, wxLB_SINGLE|wxLB_ALWAYS_SB|wxLB_SORT );
    itemBoxSizer5->Add(m_List, 1, wxGROW|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer7 = new wxFlexGridSizer(2, 4, 0, 0);
    itemBoxSizer5->Add(itemFlexGridSizer7, 0, wxGROW|wxALL, 5);
    m_New = new wxButton( m_IndexPanel, ID_BUTTON_ST_NEW, _("New"), wxDefaultPosition, wxSize(80, -1), 0 );
    itemFlexGridSizer7->Add(m_New, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 5);

    m_DeleteRoute = new wxButton( m_IndexPanel, ID_BUTTON_ST_DELETE_ROUTE, _("Delete"), wxDefaultPosition, wxSize(80, -1), 0 );
    itemFlexGridSizer7->Add(m_DeleteRoute, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 5);

    m_Test = new wxButton( m_IndexPanel, ID_BUTTON_ST_TEST, _("Test"), wxDefaultPosition, wxSize(80, -1), 0 );
    itemFlexGridSizer7->Add(m_Test, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 5);

    m_Doc = new wxButton( m_IndexPanel, ID_BUTTON_ST_DOC, _("Doc"), wxDefaultPosition, wxSize(80, -1), 0 );
    itemFlexGridSizer7->Add(m_Doc, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 5);

    m_Notebook->AddPage(m_IndexPanel, _("Index"));

    m_GeneralPanel = new wxPanel( m_Notebook, ID_PANEL_ST_GENERAL, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer13 = new wxBoxSizer(wxVERTICAL);
    m_GeneralPanel->SetSizer(itemBoxSizer13);

    wxFlexGridSizer* itemFlexGridSizer14 = new wxFlexGridSizer(2, 2, 0, 0);
    itemFlexGridSizer14->AddGrowableCol(1);
    itemBoxSizer13->Add(itemFlexGridSizer14, 0, wxGROW|wxALL, 5);
    m_LabelId = new wxStaticText( m_GeneralPanel, wxID_STATIC_ST_ID, _("Id"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer14->Add(m_LabelId, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Id = new wxTextCtrl( m_GeneralPanel, ID_TEXTCTRL_ST_ID, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer14->Add(m_Id, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_LabelBlockA = new wxStaticText( m_GeneralPanel, wxID_STATIC_ST_BLOCKA, _("from block"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer14->Add(m_LabelBlockA, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxArrayString m_BlockAStrings;
    m_BlockA = new wxComboBox( m_GeneralPanel, ID_COMBOBOX_ST_BLOCKA, wxEmptyString, wxDefaultPosition, wxDefaultSize, m_BlockAStrings, wxCB_READONLY );
    itemFlexGridSizer14->Add(m_BlockA, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_LabelBlockB = new wxStaticText( m_GeneralPanel, wxID_STATIC_BLOCKB, _("to block"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer14->Add(m_LabelBlockB, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxArrayString m_BlockBStrings;
    m_BlockB = new wxComboBox( m_GeneralPanel, ID_COMBOBOX_ST_BLOCKB, wxEmptyString, wxDefaultPosition, wxDefaultSize, m_BlockBStrings, wxCB_READONLY );
    itemFlexGridSizer14->Add(m_BlockB, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labCrossingBlock = new wxStaticText( m_GeneralPanel, wxID_STATIC_BLOCKC, _("crossing block"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer14->Add(m_labCrossingBlock, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_BlockC = new wxTextCtrl( m_GeneralPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer14->Add(m_BlockC, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labModID = new wxStaticText( m_GeneralPanel, wxID_ANY, _("Module ID"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer14->Add(m_labModID, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_ModID = new wxTextCtrl( m_GeneralPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer14->Add(m_ModID, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer25 = new wxFlexGridSizer(3, 2, 0, 0);
    itemBoxSizer13->Add(itemFlexGridSizer25, 0, wxGROW|wxLEFT|wxRIGHT, 5);
    wxArrayString m_SpeedStrings;
    m_SpeedStrings.Add(_("&None"));
    m_SpeedStrings.Add(_("&Min"));
    m_SpeedStrings.Add(_("&Mid"));
    m_SpeedStrings.Add(_("&Cruise"));
    m_SpeedStrings.Add(_("&Max"));
    m_SpeedStrings.Add(_("&%"));
    m_Speed = new wxRadioBox( m_GeneralPanel, ID_ROUTE_VELOCITY, _("Speed"), wxDefaultPosition, wxDefaultSize, m_SpeedStrings, 1, wxRA_SPECIFY_ROWS );
    m_Speed->SetSelection(0);
    itemFlexGridSizer25->Add(m_Speed, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 5);

    m_SpeedPercent = new wxSpinCtrl( m_GeneralPanel, wxID_ANY, _T("80"), wxDefaultPosition, wxSize(70, -1), wxSP_ARROW_KEYS, 0, 100, 80 );
    itemFlexGridSizer25->Add(m_SpeedPercent, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_BOTTOM|wxALL, 5);

    m_ReduceV = new wxCheckBox( m_GeneralPanel, wxID_ANY, _("Reduce speed"), wxDefaultPosition, wxDefaultSize, 0 );
    m_ReduceV->SetValue(false);
    itemFlexGridSizer25->Add(m_ReduceV, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    wxBoxSizer* itemBoxSizer29 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer13->Add(itemBoxSizer29, 0, wxGROW|wxRIGHT, 5);
    wxFlexGridSizer* itemFlexGridSizer30 = new wxFlexGridSizer(0, 2, 0, 0);
    itemBoxSizer29->Add(itemFlexGridSizer30, 0, wxGROW|wxALL, 5);
    wxArrayString m_DirStrings;
    m_DirStrings.Add(_("&from-to"));
    m_DirStrings.Add(_("&both"));
    m_Dir = new wxRadioBox( m_GeneralPanel, ID_RADIOBOX_DIR, _("Usage"), wxDefaultPosition, wxDefaultSize, m_DirStrings, 1, wxRA_SPECIFY_ROWS );
    m_Dir->SetSelection(0);
    itemFlexGridSizer30->Add(m_Dir, 0, wxALIGN_LEFT|wxALIGN_TOP|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    wxArrayString m_FromSignalsStrings;
    m_FromSignalsStrings.Add(_("&forward"));
    m_FromSignalsStrings.Add(_("&reverse"));
    m_FromSignals = new wxRadioBox( m_GeneralPanel, wxID_ANY, _("From signal pair"), wxDefaultPosition, wxDefaultSize, m_FromSignalsStrings, 1, wxRA_SPECIFY_ROWS );
    m_FromSignals->SetSelection(0);
    itemFlexGridSizer30->Add(m_FromSignals, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    wxArrayString m_RunDirStrings;
    m_RunDirStrings.Add(_("&forwards"));
    m_RunDirStrings.Add(_("&reverse"));
    m_RunDir = new wxRadioBox( m_GeneralPanel, wxID_ANY, _("Run direction"), wxDefaultPosition, wxDefaultSize, m_RunDirStrings, 1, wxRA_SPECIFY_ROWS );
    m_RunDir->SetSelection(0);
    itemFlexGridSizer30->Add(m_RunDir, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    wxArrayString m_ToSignalsStrings;
    m_ToSignalsStrings.Add(_("&forward"));
    m_ToSignalsStrings.Add(_("&reverse"));
    m_ToSignals = new wxRadioBox( m_GeneralPanel, wxID_ANY, _("To signal pair"), wxDefaultPosition, wxDefaultSize, m_ToSignalsStrings, 1, wxRA_SPECIFY_ROWS );
    m_ToSignals->SetSelection(0);
    itemFlexGridSizer30->Add(m_ToSignals, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_DetailsBox = new wxStaticBox(m_GeneralPanel, wxID_ANY, _("Details"));
    wxStaticBoxSizer* itemStaticBoxSizer35 = new wxStaticBoxSizer(m_DetailsBox, wxHORIZONTAL);
    itemBoxSizer13->Add(itemStaticBoxSizer35, 0, wxGROW|wxLEFT|wxRIGHT, 5);
    wxFlexGridSizer* itemFlexGridSizer36 = new wxFlexGridSizer(0, 2, 0, 0);
    itemStaticBoxSizer35->Add(itemFlexGridSizer36, 1, wxGROW|wxALL, 5);
    m_Swap = new wxCheckBox( m_GeneralPanel, wxID_ANY, _("Swap loco placing"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Swap->SetValue(false);
    itemFlexGridSizer36->Add(m_Swap, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_SwapPost = new wxCheckBox( m_GeneralPanel, wxID_ANY, _("Swap loco placing post route"), wxDefaultPosition, wxDefaultSize, 0 );
    m_SwapPost->SetValue(false);
    itemFlexGridSizer36->Add(m_SwapPost, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_CrossingblockSignals = new wxCheckBox( m_GeneralPanel, wxID_ANY, _("Crossing block signals"), wxDefaultPosition, wxDefaultSize, 0 );
    m_CrossingblockSignals->SetValue(false);
    itemFlexGridSizer36->Add(m_CrossingblockSignals, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Show = new wxCheckBox( m_GeneralPanel, wxID_ANY, _("Visible"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Show->SetValue(false);
    itemFlexGridSizer36->Add(m_Show, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Manual = new wxCheckBox( m_GeneralPanel, wxID_ANY, _("Manual controlled"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Manual->SetValue(false);
    itemFlexGridSizer36->Add(m_Manual, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Actions = new wxButton( m_GeneralPanel, ID_ROUTE_ACTIONS, _("Actions..."), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer36->Add(m_Actions, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_Notebook->AddPage(m_GeneralPanel, _("General"));

    m_CommandPanel = new wxPanel( m_Notebook, ID_PANEL_ST_COMMANDS, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer44 = new wxBoxSizer(wxVERTICAL);
    m_CommandPanel->SetSizer(itemBoxSizer44);

    wxArrayString m_CommandsStrings;
    m_Commands = new wxListBox( m_CommandPanel, ID_LISTBOX_COMMANDS, wxDefaultPosition, wxDefaultSize, m_CommandsStrings, wxLB_SINGLE|wxLB_ALWAYS_SB );
    itemBoxSizer44->Add(m_Commands, 1, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer46 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer44->Add(itemBoxSizer46, 0, wxGROW|wxALL, 5);
    m_Delete = new wxButton( m_CommandPanel, ID_BUTTON_ST_DELETE, _("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer46->Add(m_Delete, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Modify = new wxButton( m_CommandPanel, ID_BUTTON_ST_MODIFY, _("Modify"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer46->Add(m_Modify, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer49 = new wxFlexGridSizer(2, 2, 0, 0);
    itemFlexGridSizer49->AddGrowableCol(0);
    itemBoxSizer44->Add(itemFlexGridSizer49, 1, wxGROW|wxALL, 5);
    wxBoxSizer* itemBoxSizer50 = new wxBoxSizer(wxVERTICAL);
    itemFlexGridSizer49->Add(itemBoxSizer50, 1, wxGROW|wxALIGN_TOP|wxALL, 5);
    m_LabelSwitchId = new wxStaticText( m_CommandPanel, wxID_STATIC, _("turnoutID"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer50->Add(m_LabelSwitchId, 0, wxALIGN_LEFT|wxALL, 5);

    wxArrayString m_SwitchIdStrings;
    m_SwitchId = new wxComboBox( m_CommandPanel, ID_COMBOBOX_ST_SWITCH_ID, wxEmptyString, wxDefaultPosition, wxSize(120, -1), m_SwitchIdStrings, wxCB_DROPDOWN );
    itemBoxSizer50->Add(m_SwitchId, 0, wxGROW|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer53 = new wxFlexGridSizer(1, 2, 0, 0);
    itemBoxSizer50->Add(itemFlexGridSizer53, 0, wxGROW|wxBOTTOM, 5);
    m_labTrackNumber = new wxStaticText( m_CommandPanel, wxID_ANY, _("Track number"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer53->Add(m_labTrackNumber, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_TrackNumber = new wxSpinCtrl( m_CommandPanel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(70, -1), wxSP_ARROW_KEYS, 0, 47, 0 );
    itemFlexGridSizer53->Add(m_TrackNumber, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Add = new wxButton( m_CommandPanel, ID_BUTTON_ST_ADD, _("Add"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer50->Add(m_Add, 0, wxALIGN_LEFT|wxALL, 5);

    wxBoxSizer* itemBoxSizer57 = new wxBoxSizer(wxVERTICAL);
    itemFlexGridSizer49->Add(itemBoxSizer57, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_TOP, 5);
    wxArrayString m_SwitchCmdStrings;
    m_SwitchCmdStrings.Add(_("&straight"));
    m_SwitchCmdStrings.Add(_("&turnout(left)"));
    m_SwitchCmdStrings.Add(_("&right(3-way)"));
    m_SwitchCmdStrings.Add(_("&crossing"));
    m_SwitchCmdStrings.Add(_("&track"));
    m_SwitchCmd = new wxRadioBox( m_CommandPanel, ID_RADIOBOX_ST_SW_CMD, _("Command"), wxDefaultPosition, wxDefaultSize, m_SwitchCmdStrings, 1, wxRA_SPECIFY_COLS );
    m_SwitchCmd->SetSelection(0);
    itemBoxSizer57->Add(m_SwitchCmd, 0, wxALIGN_LEFT|wxALL, 5);

    m_Lock = new wxCheckBox( m_CommandPanel, wxID_ANY, _("Lock"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Lock->SetValue(false);
    itemBoxSizer57->Add(m_Lock, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_Notebook->AddPage(m_CommandPanel, _("Turnouts"));

    m_SensorPanel = new wxPanel( m_Notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer61 = new wxBoxSizer(wxVERTICAL);
    m_SensorPanel->SetSizer(itemBoxSizer61);

    wxArrayString m_SensorListStrings;
    m_SensorList = new wxListBox( m_SensorPanel, ID_LIST_ROUTE_SENSORS, wxDefaultPosition, wxDefaultSize, m_SensorListStrings, wxLB_SINGLE|wxLB_ALWAYS_SB );
    itemBoxSizer61->Add(m_SensorList, 1, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer63 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer61->Add(itemBoxSizer63, 0, wxGROW|wxALL, 5);
    wxArrayString m_SensorComboStrings;
    m_SensorCombo = new wxComboBox( m_SensorPanel, ID_COMBOBOX_ROUTES_SENSORS, wxEmptyString, wxDefaultPosition, wxSize(140, -1), m_SensorComboStrings, wxCB_DROPDOWN );
    itemBoxSizer63->Add(m_SensorCombo, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT|wxTOP|wxBOTTOM, 5);

    m_AddSensor = new wxButton( m_SensorPanel, ID_BUTTON_ROUTES_ADD_SENSOR, _("Add"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer63->Add(m_AddSensor, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 5);

    m_DelSensor = new wxButton( m_SensorPanel, ID_BUTTON_ROUTES_DEL_SENSOR, _("Remove"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer63->Add(m_DelSensor, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 5);

    m_Notebook->AddPage(m_SensorPanel, _("Sensors"));

    m_PermissionsPanel = new wxPanel( m_Notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer68 = new wxBoxSizer(wxVERTICAL);
    m_PermissionsPanel->SetSizer(itemBoxSizer68);

    m_labInclude = new wxStaticText( m_PermissionsPanel, wxID_ANY, _("Include"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer68->Add(m_labInclude, 0, wxALIGN_LEFT|wxALL, 5);

    wxArrayString m_IncludeListStrings;
    m_IncludeList = new wxListBox( m_PermissionsPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_IncludeListStrings, wxLB_MULTIPLE );
    itemBoxSizer68->Add(m_IncludeList, 1, wxGROW|wxALL, 5);

    m_labExclude = new wxStaticText( m_PermissionsPanel, wxID_ANY, _("Exclude"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer68->Add(m_labExclude, 0, wxALIGN_LEFT|wxALL, 5);

    wxArrayString m_ExcludeListStrings;
    m_ExcludeList = new wxListBox( m_PermissionsPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_ExcludeListStrings, wxLB_MULTIPLE );
    itemBoxSizer68->Add(m_ExcludeList, 1, wxGROW|wxALL, 5);

    wxArrayString m_PermTypeStrings;
    m_PermTypeStrings.Add(_("&All"));
    m_PermTypeStrings.Add(_("&None"));
    m_PermTypeStrings.Add(_("&Goods"));
    m_PermTypeStrings.Add(_("&Local"));
    m_PermTypeStrings.Add(_("&Mixed"));
    m_PermTypeStrings.Add(_("&Cleaning"));
    m_PermTypeStrings.Add(_("&ICE"));
    m_PermTypeStrings.Add(_("&Post"));
    m_PermType = new wxRadioBox( m_PermissionsPanel, wxID_ANY, _("Type"), wxDefaultPosition, wxDefaultSize, m_PermTypeStrings, 2, wxRA_SPECIFY_ROWS );
    m_PermType->SetSelection(0);
    itemBoxSizer68->Add(m_PermType, 0, wxGROW|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer74 = new wxFlexGridSizer(0, 2, 0, 0);
    itemFlexGridSizer74->AddGrowableCol(1);
    itemBoxSizer68->Add(itemFlexGridSizer74, 0, wxGROW|wxALL, 5);
    m_labMaxLen = new wxStaticText( m_PermissionsPanel, wxID_ANY, _("Max. train length"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer74->Add(m_labMaxLen, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_MaxLen = new wxSpinCtrl( m_PermissionsPanel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(90, -1), wxSP_ARROW_KEYS, 0, 10000, 0 );
    itemFlexGridSizer74->Add(m_MaxLen, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Notebook->AddPage(m_PermissionsPanel, _("Persmissions"));

    m_LocationPanel = new wxPanel( m_Notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer78 = new wxBoxSizer(wxHORIZONTAL);
    m_LocationPanel->SetSizer(itemBoxSizer78);

    wxFlexGridSizer* itemFlexGridSizer79 = new wxFlexGridSizer(2, 2, 0, 0);
    itemBoxSizer78->Add(itemFlexGridSizer79, 0, wxALIGN_TOP|wxALL, 5);
    m_LabelX = new wxStaticText( m_LocationPanel, wxID_ANY, _("x"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer79->Add(m_LabelX, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_x = new wxTextCtrl( m_LocationPanel, wxID_ANY, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer79->Add(m_x, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_LabelY = new wxStaticText( m_LocationPanel, wxID_ANY, _("y"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer79->Add(m_LabelY, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_y = new wxTextCtrl( m_LocationPanel, wxID_ANY, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer79->Add(m_y, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_LabelZ = new wxStaticText( m_LocationPanel, wxID_ANY, _("z"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer79->Add(m_LabelZ, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_z = new wxTextCtrl( m_LocationPanel, wxID_ANY, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer79->Add(m_z, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxArrayString m_oriStrings;
    m_oriStrings.Add(_("&north"));
    m_oriStrings.Add(_("&east"));
    m_oriStrings.Add(_("&south"));
    m_oriStrings.Add(_("&west"));
    m_ori = new wxRadioBox( m_LocationPanel, wxID_ANY, _("Orientation"), wxDefaultPosition, wxDefaultSize, m_oriStrings, 1, wxRA_SPECIFY_COLS );
    m_ori->SetSelection(0);
    itemBoxSizer78->Add(m_ori, 0, wxALIGN_TOP|wxALL, 5);

    m_Notebook->AddPage(m_LocationPanel, _("Location"));

    m_WiringPanel = new wxPanel( m_Notebook, ID_ROUTE_WIRING, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer88 = new wxBoxSizer(wxVERTICAL);
    m_WiringPanel->SetSizer(itemBoxSizer88);

    m_CTCBox = new wxStaticBox(m_WiringPanel, wxID_ANY, _("CTC"));
    wxStaticBoxSizer* itemStaticBoxSizer89 = new wxStaticBoxSizer(m_CTCBox, wxVERTICAL);
    itemBoxSizer88->Add(itemStaticBoxSizer89, 0, wxGROW|wxALL, 5);
    wxFlexGridSizer* itemFlexGridSizer90 = new wxFlexGridSizer(4, 3, 0, 0);
    itemStaticBoxSizer89->Add(itemFlexGridSizer90, 0, wxGROW|wxALL, 5);
    m_labCTCbutton = new wxStaticText( m_WiringPanel, wxID_ANY, _("Button"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer90->Add(m_labCTCbutton, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labCTCIID = new wxStaticText( m_WiringPanel, wxID_ANY, _("IID"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer90->Add(m_labCTCIID, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labCTCAddr = new wxStaticText( m_WiringPanel, wxID_ANY, _("Address"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer90->Add(m_labCTCAddr, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labCTCButton1 = new wxStaticText( m_WiringPanel, wxID_STATIC, _("1"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer90->Add(m_labCTCButton1, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_CTCIID1 = new wxTextCtrl( m_WiringPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer90->Add(m_CTCIID1, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_CTCAddr1 = new wxSpinCtrl( m_WiringPanel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(100, -1), wxSP_ARROW_KEYS, 0, 10000, 0 );
    itemFlexGridSizer90->Add(m_CTCAddr1, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labCTCButton2 = new wxStaticText( m_WiringPanel, wxID_ANY, _("2"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer90->Add(m_labCTCButton2, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_CTCIID2 = new wxTextCtrl( m_WiringPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer90->Add(m_CTCIID2, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_CTCAddr2 = new wxSpinCtrl( m_WiringPanel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(100, -1), wxSP_ARROW_KEYS, 0, 10000, 0 );
    itemFlexGridSizer90->Add(m_CTCAddr2, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labCTCButton3 = new wxStaticText( m_WiringPanel, wxID_ANY, _("3"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer90->Add(m_labCTCButton3, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_CTCIID3 = new wxTextCtrl( m_WiringPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer90->Add(m_CTCIID3, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_CTCAddr3 = new wxSpinCtrl( m_WiringPanel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(100, -1), wxSP_ARROW_KEYS, 0, 10000, 0 );
    itemFlexGridSizer90->Add(m_CTCAddr3, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Notebook->AddPage(m_WiringPanel, _("Wiring"));

    itemBoxSizer2->Add(m_Notebook, 1, wxGROW|wxALL, 5);

    wxStdDialogButtonSizer* itemStdDialogButtonSizer103 = new wxStdDialogButtonSizer;

    itemBoxSizer2->Add(itemStdDialogButtonSizer103, 0, wxALIGN_RIGHT|wxALL, 5);
    m_Cancel = new wxButton( itemDialog1, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer103->AddButton(m_Cancel);

    m_OK = new wxButton( itemDialog1, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    m_OK->SetDefault();
    itemStdDialogButtonSizer103->AddButton(m_OK);

    m_Apply = new wxButton( itemDialog1, wxID_APPLY, _("&Apply"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer103->AddButton(m_Apply);

    itemStdDialogButtonSizer103->Realize();

////@end RouteDialog content construction
}

/*!
 * Should we show tooltips?
 */

bool RouteDialog::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap RouteDialog::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin RouteDialog bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end RouteDialog bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon RouteDialog::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin RouteDialog icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end RouteDialog icon retrieval
}
/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_ST_ADD
 */

void RouteDialog::OnButtonTurnoutAddClick( wxCommandEvent& event )
{
  iONode swcmd = NodeOp.inst( wSwitchCmd.name(), m_Props, ELEMENT_NODE );

  wSwitchCmd.setid(swcmd, m_SwitchId->GetStringSelection().mb_str(wxConvUTF8) );

  int dir = m_SwitchCmd->GetSelection();
  switch( dir ) {
    case 0:
      wSwitchCmd.setcmd( swcmd, wSwitch.straight );
      break;
    case 1:
      wSwitchCmd.setcmd( swcmd, wSwitch.turnout );
      break;
    case 2:
      wSwitchCmd.setcmd( swcmd, wSwitch.left );
      break;
    case 3:
      wSwitchCmd.setcmd( swcmd, wSwitch.right );
      break;
    case 4:
      wSwitchCmd.setcmd( swcmd, wSwitchCmd.cmd_track );
      wSwitchCmd.settrack( swcmd, m_TrackNumber->GetValue() );
      break;
  }

  wSwitchCmd.setlock( swcmd, m_Lock->IsChecked() ? True:False );

  NodeOp.addChild( m_Props, swcmd );

  char* str = StrOp.fmt( "%s %s %d %s",
      wSwitchCmd.getid( swcmd ), wSwitchCmd.getcmd(swcmd), wSwitchCmd.gettrack(swcmd), wSwitchCmd.islock(swcmd)?"lock":"" );
  m_Commands->Append( wxString(str,wxConvUTF8), swcmd );
  StrOp.free( str );

}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_ST_DELETE
 */

void RouteDialog::OnButtonTurnoutDeleteClick( wxCommandEvent& event )
{
  iONode swcmd = (iONode)m_Commands->GetClientData( m_Commands->GetSelection() );
  if( swcmd != NULL ) {
    m_Commands->Delete( m_Commands->GetSelection() );
    NodeOp.removeChild( m_Props, swcmd );
  }
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
 */

void RouteDialog::OnCancelClick( wxCommandEvent& event )
{
  EndModal( 0 );
}

/*!
 * wxEVT_COMMAND_LISTBOX_SELECTED event handler for ID_LISTBOX_COMMANDS
 */

void RouteDialog::OnListboxCommandsSelected( wxCommandEvent& event )
{
  iONode swcmd = (iONode)m_Commands->GetClientData( m_Commands->GetSelection() );
  m_TrackNumber->Enable(false);
  if( swcmd != NULL ) {
    m_SwitchId->SetStringSelection( wxString(wSwitchCmd.getid(swcmd),wxConvUTF8));
    int dir = 0;
    if( StrOp.equals( wSwitch.straight, wSwitchCmd.getcmd(swcmd) ) )
      dir = 0;
    else if( StrOp.equals( wSwitch.turnout, wSwitchCmd.getcmd(swcmd) ) )
      dir = 1;
    else if( StrOp.equals( wSwitch.left, wSwitchCmd.getcmd(swcmd) ) )
      dir = 2;
    else if( StrOp.equals( wSwitch.right, wSwitchCmd.getcmd(swcmd) ) )
      dir = 3;
    else if( StrOp.equals( wSwitchCmd.cmd_track, wSwitchCmd.getcmd(swcmd) ) ) {
      dir = 4;
      m_TrackNumber->SetValue(wSwitchCmd.gettrack(swcmd));
      m_TrackNumber->Enable(true);
    }
    m_SwitchCmd->SetSelection(dir);

    m_Lock->Enable(true);
    m_Lock->SetValue(wSwitchCmd.islock( swcmd )?true:false);

    int swidx = m_SwitchId->FindString( wxString(wSwitchCmd.getid(swcmd),wxConvUTF8) );
    if( swidx != wxNOT_FOUND ) {
      iONode sw = (iONode)m_SwitchId->GetClientData( swidx );
      if( StrOp.equals( wSwitch.threeway, wSwitch.gettype( sw ) ) ) {
        m_SwitchCmd->Enable( 0, true );
        m_SwitchCmd->Enable( 1, false );
        m_SwitchCmd->Enable( 2, true );
        m_SwitchCmd->Enable( 3, true );
        m_SwitchCmd->Enable( 4, false );
      }
      else if( StrOp.equals( wSwitch.dcrossing, wSwitch.gettype( sw ) ) ) {
        m_SwitchCmd->Enable( 0, true );
        m_SwitchCmd->Enable( 1, true );
        m_SwitchCmd->Enable( 2, true );
        m_SwitchCmd->Enable( 3, true );
        m_SwitchCmd->Enable( 4, false );
      }
      else if( StrOp.equals( wTurntable.name(), NodeOp.getName( sw ) ) ||
               StrOp.equals( wSelTab.name(), NodeOp.getName( sw ) ) ) {
        m_SwitchCmd->Enable( 0, false );
        m_SwitchCmd->Enable( 1, false );
        m_SwitchCmd->Enable( 2, false );
        m_SwitchCmd->Enable( 3, false );
        m_SwitchCmd->Enable( 4, true );
        m_Lock->Enable(false);
      }
      else {
        m_SwitchCmd->Enable( 0, true );
        m_SwitchCmd->Enable( 1, true );
        m_SwitchCmd->Enable( 2, false );
        m_SwitchCmd->Enable( 3, false );
        m_SwitchCmd->Enable( 4, false );
      }
    }

    m_Delete->Enable( true );
    m_Modify->Enable( true );

  }
  else {
    m_SwitchId->SetSelection(0);
  }
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
 */

void RouteDialog::OnOkClick( wxCommandEvent& event )
{
  OnApplyClick(event);
  EndModal( wxID_OK );
}


/*!
 * wxEVT_COMMAND_LISTBOX_SELECTED event handler for ID_LISTBOX_FB
 */

void RouteDialog::OnListboxStSelected( wxCommandEvent& event )
{
  iONode model = wxGetApp().getModel();
  if( model != NULL ) {
    iONode stlist = wPlan.getstlist( model );
    if( stlist != NULL ) {
      int cnt = NodeOp.getChildCnt( stlist );
      for( int i = 0; i < cnt; i++ ) {
        iONode st = NodeOp.getChild( stlist, i );
        const char* id = wRoute.getid( st );
        if( id != NULL && StrOp.equals( id, m_List->GetStringSelection().mb_str(wxConvUTF8) ) ) {
          m_Props = st;
          initValues();
          break;
        }
      }
    }
  }
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_FB_NEW
 */

void RouteDialog::OnButtonStNewClick( wxCommandEvent& event )
{
  int i = m_List->FindString( _T("NEW") );
  if( i == wxNOT_FOUND ) {
    m_List->Append( _T("NEW") );
    iONode model = wxGetApp().getModel();
    if( model != NULL ) {
      iONode stlist = wPlan.getstlist( model );
      if( stlist == NULL ) {
        stlist = NodeOp.inst( wRouteList.name(), model, ELEMENT_NODE );
        NodeOp.addChild( model, stlist );
      }

      if( stlist != NULL ) {
        iONode st = NodeOp.inst( wRoute.name(), stlist, ELEMENT_NODE );
        NodeOp.addChild( stlist, st );
        wRoute.setid( st, "NEW" );
        m_Props = st;
        initValues();
      }
    }
  }
  m_List->SetStringSelection( _T("NEW") );
  m_List->SetFirstItem( _T("NEW") );
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_FB_DELETE
 */

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_ST_DELETE_ROUTE
 */

void RouteDialog::OnButtonStDeleteRouteClick( wxCommandEvent& event )
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
    iONode stlist = wPlan.getstlist( model );
    if( stlist != NULL ) {
      NodeOp.removeChild( stlist, m_Props );
      m_Props = NULL;
    }
  }

  initIndex();
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_APPLY
 */

void RouteDialog::OnApplyClick( wxCommandEvent& event )
{
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
  initIndex();
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_ST_MODIFY
 */

void RouteDialog::OnButtonTurnoutModifyClick( wxCommandEvent& event )
{
  iONode swcmd = (iONode)m_Commands->GetClientData( m_Commands->GetSelection() );
  if( swcmd != NULL ) {
    wSwitchCmd.setid(swcmd, m_SwitchId->GetStringSelection().mb_str(wxConvUTF8) );
    wSwitchCmd.setlock( swcmd, m_Lock->IsChecked() ? True:False );

    int dir = m_SwitchCmd->GetSelection();
    switch( dir ) {
      case 0:
        wSwitchCmd.setcmd( swcmd, wSwitch.straight );
        break;
      case 1:
        wSwitchCmd.setcmd( swcmd, wSwitch.turnout );
        break;
      case 2:
        wSwitchCmd.setcmd( swcmd, wSwitch.left );
        break;
      case 3:
        wSwitchCmd.setcmd( swcmd, wSwitch.right );
        break;
      case 4:
        wSwitchCmd.setcmd( swcmd, wSwitchCmd.cmd_track );
        wSwitchCmd.settrack( swcmd, m_TrackNumber->GetValue() );
        break;
    }

    initCommands();
  }
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_ST_TEST
 */

void RouteDialog::OnButtonStTestClick( wxCommandEvent& event )
{
  if( m_Props == NULL )
    return;

  iONode cmd = NodeOp.inst( wRoute.name(), NULL, ELEMENT_NODE );
  wRoute.setcmd( cmd, wRoute.test );
  wRoute.setid( cmd, wRoute.getid( m_Props ) );
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);
}



/*!
 * wxEVT_COMMAND_COMBOBOX_SELECTED event handler for ID_COMBOBOX_ST_SWITCH_ID
 */

void RouteDialog::OnComboboxStSwitchIdSelected( wxCommandEvent& event )
{
  int swidx = m_SwitchId->GetSelection();
  if( swidx != wxNOT_FOUND ) {
    iONode sw = (iONode)m_SwitchId->GetClientData( swidx );
    if( sw == NULL ) {
      m_Add->Enable( false );
      return;
    }
    m_Add->Enable( true );
    m_SwitchCmd->SetSelection(0);
    m_SwitchCmd->Enable( 0, false );
    m_SwitchCmd->Enable( 1, false );
    m_SwitchCmd->Enable( 2, false );
    m_SwitchCmd->Enable( 3, false );
    m_SwitchCmd->Enable( 4, false );
    m_TrackNumber->Enable(false);
    if( StrOp.equals( wTurntable.name(), NodeOp.getName( sw ) ) ||
        StrOp.equals( wSelTab.name(), NodeOp.getName( sw ) )) {
      m_SwitchCmd->Enable( 4, true );
      m_SwitchCmd->SetSelection(4);
      m_TrackNumber->Enable(true);
    }
    else if( StrOp.equals( wSwitch.threeway, wSwitch.gettype( sw ) ) ) {
      m_SwitchCmd->Enable( 0, true );
      m_SwitchCmd->Enable( 1, true );
      m_SwitchCmd->Enable( 2, true );
    }
    else if( StrOp.equals( wSwitch.dcrossing, wSwitch.gettype( sw ) ) ) {
      m_SwitchCmd->Enable( 0, true );
      m_SwitchCmd->Enable( 1, true );
      m_SwitchCmd->Enable( 2, true );
      m_SwitchCmd->Enable( 3, true );
    }
    else {
      m_SwitchCmd->Enable( 0, true );
      m_SwitchCmd->Enable( 1, true );
    }

  }
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_ST_DOC
 */

void RouteDialog::OnButtonStDocClick( wxCommandEvent& event )
{
  doDoc( event, "streets");
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_ROUTES_ADD_SENSOR
 */

void RouteDialog::OnButtonRoutesAddSensorClick( wxCommandEvent& event )
{
  if( m_Props == NULL )
    return;

  iONode fb = wRoute.getfbevent( m_Props );
  while( fb != NULL ) {
    if( StrOp.equals( wFeedbackEvent.getid(fb), m_SensorCombo->GetStringSelection().mb_str(wxConvUTF8) ) ) {
      // allready in list...
      return;
    }

    fb = wRoute.nextfbevent( m_Props, fb );
  };
  m_SensorList->Append( m_SensorCombo->GetStringSelection() );
  iONode fbevent = NodeOp.inst( wFeedbackEvent.name(), m_Props, ELEMENT_NODE );
  wFeedbackEvent.setid( fbevent, m_SensorCombo->GetStringSelection().mb_str(wxConvUTF8) );
  NodeOp.addChild( m_Props, fbevent );
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_ROUTES_DEL_SENSOR
 */

void RouteDialog::OnButtonRoutesDelSensorClick( wxCommandEvent& event )
{
  if( m_Props == NULL )
    return;

  iONode fb = wRoute.getfbevent( m_Props );
  while( fb != NULL ) {
    if( StrOp.equals( wFeedbackEvent.getid(fb), m_SensorList->GetStringSelection().mb_str(wxConvUTF8) ) ) {
      // in list...
      break;
    }

    fb = wRoute.nextfbevent( m_Props, fb );
  };

  if( fb != NULL ) {
    NodeOp.removeChild( m_Props, fb );
  }

  m_SensorList->Delete( m_SensorList->GetSelection() );
}


/*!
 * wxEVT_COMMAND_LISTBOX_SELECTED event handler for ID_LIST_ROUTE_SENSORS
 */

void RouteDialog::OnListRouteSensorsSelected( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_LISTBOX_SELECTED event handler for ID_LIST_ROUTE_SENSORS in RouteDialog.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_COMMAND_LISTBOX_SELECTED event handler for ID_LIST_ROUTE_SENSORS in RouteDialog.
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_ROUTE_ACTIONS
 */

void RouteDialog::OnRouteActionsClick( wxCommandEvent& event )
{
  if( m_Props == NULL )
    return;

  ActionsCtrlDlg*  dlg = new ActionsCtrlDlg(this, m_Props );

  if( wxID_OK == dlg->ShowModal() ) {
    // TODO: inform
  }

  dlg->Destroy();
}


/*!
 * wxEVT_COMMAND_RADIOBOX_SELECTED event handler for ID_ROUTE_VELOCITY
 */

void RouteDialog::OnRouteVelocitySelected( wxCommandEvent& event )
{
  if( m_Speed->GetSelection() == 5 ) {
    // V_%
    m_ReduceV->SetValue(false);
    m_ReduceV->Enable(false);
  }
  else {
    m_ReduceV->Enable(true);
  }
}

