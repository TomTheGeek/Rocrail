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
#pragma implementation "blockdialog.h"
#endif


#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#else
// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
#endif

////@begin includes
////@end includes

#include "blockdialog.h"
#include "locdialog.h"
#include "feedbackdialog.h"
#include "switchdialog.h"
#include "routedialog.h"

////@begin XPM images
////@end XPM images

#include "rocrail/wrapper/public/Plan.h"
#include "rocrail/wrapper/public/Block.h"
#include "rocrail/wrapper/public/BlockList.h"
#include "rocrail/wrapper/public/Feedback.h"
#include "rocrail/wrapper/public/Switch.h"
#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/Item.h"
#include "rocrail/wrapper/public/SysCmd.h"
#include "rocrail/wrapper/public/FunCmd.h"
#include "rocrail/wrapper/public/ModelCmd.h"
#include "rocrail/wrapper/public/Route.h"
#include "rocrail/wrapper/public/FeedbackEvent.h"
#include "rocrail/wrapper/public/Switch.h"
#include "rocrail/wrapper/public/BlockInclude.h"
#include "rocrail/wrapper/public/BlockExclude.h"
#include "rocrail/wrapper/public/Signal.h"
#include "rocrail/wrapper/public/Turntable.h"

#include "rocgui/public/guiapp.h"
#include "rocs/public/strtok.h"

/*!
 * BlockDialog type definition
 */

IMPLEMENT_DYNAMIC_CLASS( BlockDialog, wxDialog )

/*!
 * BlockDialog event table definition
 */

BEGIN_EVENT_TABLE( BlockDialog, wxDialog )

////@begin BlockDialog event table entries
    EVT_LISTBOX( ID_LISTBOX_BLOCKS, BlockDialog::OnListboxBlocksSelected )

    EVT_BUTTON( ID_BUTTON_BLOCK_NEW, BlockDialog::OnButtonBlockNewClick )

    EVT_BUTTON( ID_BUTTON_BLOCK_DELETE, BlockDialog::OnButtonBlockDeleteClick )

    EVT_BUTTON( ID_BUTTON_BK_LOCPROPS, BlockDialog::OnButtonBkLocpropsClick )

    EVT_CHECKBOX( wxID_BLOCK_WAIT, BlockDialog::OnBlockWaitClick )

    EVT_LISTBOX( ID_LISTBOX_BLOCK_TURNOUTS, BlockDialog::OnListboxBlockTurnoutsSelected )

    EVT_BUTTON( ID_BUTTON_BLOCK_ADD_TURNOUT, BlockDialog::OnButtonBlockAddTurnoutClick )

    EVT_BUTTON( ID_BUTTON_BLOCK_REMOVE_TURNOUT, BlockDialog::OnButtonBlockRemoveTurnoutClick )

    EVT_BUTTON( ID_BUTTON_BK_SIGNAL, BlockDialog::OnButtonBkSignalClick )

    EVT_BUTTON( ID_BUTTON_BK_WSIGNAL, BlockDialog::OnButtonBkWsignalClick )

    EVT_BUTTON( ID_BUTTON_BK_SIGNAL_R, BlockDialog::OnButtonBkSignalRClick )

    EVT_BUTTON( ID_BUTTON_BK_WSIGNAL_R, BlockDialog::OnButtonBkWsignalRClick )

    EVT_RADIOBOX( ID_RADIOBOX_BK_WAIT, BlockDialog::OnRadioboxBkWaitSelected )

    EVT_LISTBOX( ID_LISTBOX_BK_ROUTES, BlockDialog::OnListboxBkRoutesSelected )

    EVT_COMBOBOX( ID_COMBOBOX_BLOCK_SENSOR_ID1, BlockDialog::OnBlockSensor )

    EVT_COMBOBOX( ID_COMBOBOX_BLOCK_SENSOR_ACTION1, BlockDialog::OnBlockSensor )

    EVT_CHECKBOX( ID_CHECKBOX_BLOCK_SENSOR_END1, BlockDialog::OnBlockSensor )

    EVT_BUTTON( ID_BUTTON_BLOCKS_SENSOR_PROPS1, BlockDialog::OnSensorProps )

    EVT_COMBOBOX( ID_COMBOBOX_BLOCK_SENSOR_ID2, BlockDialog::OnBlockSensor )

    EVT_COMBOBOX( ID_COMBOBOX_BLOCK_SENSOR_ACTION2, BlockDialog::OnBlockSensor )

    EVT_CHECKBOX( ID_CHECKBOX_BLOCK_SENSOR_END2, BlockDialog::OnBlockSensor )

    EVT_BUTTON( ID_BUTTON_BLOCK_SENSOR_PROPS2, BlockDialog::OnSensorProps )

    EVT_COMBOBOX( ID_COMBOBOX_BLOCK_SENSOR_ID3, BlockDialog::OnBlockSensor )

    EVT_COMBOBOX( ID_COMBOBOX_BLOCK_SENSOR_ACTION3, BlockDialog::OnBlockSensor )

    EVT_CHECKBOX( ID_CHECKBOX_BLOCK_SENSOR_END3, BlockDialog::OnBlockSensor )

    EVT_BUTTON( ID_BUTTON_BLOCKS_SENSOR_PROPS3, BlockDialog::OnSensorProps )

    EVT_COMBOBOX( ID_COMBOBOX_BLOCK_SENSOR_ID4, BlockDialog::OnBlockSensor )

    EVT_COMBOBOX( ID_COMBOBOX_BLOCK_SENSOR_ACTION4, BlockDialog::OnBlockSensor )

    EVT_CHECKBOX( ID_CHECKBOX_BLOCK_SENSOR_END4, BlockDialog::OnBlockSensor )

    EVT_BUTTON( ID_BUTTON_BLOCKS_SENSOR_PROPS4, BlockDialog::OnSensorProps )

    EVT_COMBOBOX( ID_COMBOBOX_BLOCK_SENSOR_ID5, BlockDialog::OnBlockSensor )

    EVT_COMBOBOX( ID_COMBOBOX_BLOCK_SENSOR_ACTION5, BlockDialog::OnBlockSensor )

    EVT_CHECKBOX( ID_CHECKBOX_BLOCK_SENSOR_END5, BlockDialog::OnBlockSensor )

    EVT_BUTTON( ID_BUTTON_BLOCKS_SENSOR_PROPS5, BlockDialog::OnSensorProps )

    EVT_BUTTON( ID_BUTTON1, BlockDialog::OnButtonBkRoutePropsClick )

    EVT_BUTTON( ID_BUTTON2, BlockDialog::OnButtonBkRouteTestClick )

    EVT_BUTTON( wxID_CANCEL, BlockDialog::OnCancelClick )

    EVT_BUTTON( wxID_APPLY, BlockDialog::OnApplyClick )

    EVT_BUTTON( wxID_OK, BlockDialog::OnOkClick )

////@end BlockDialog event table entries
    EVT_MENU( ID_PANEL_BK_GENERAL, BlockDialog::OnSelectPage )

END_EVENT_TABLE()

/*!
 * BlockDialog constructors
 */

BlockDialog::BlockDialog( )
{
}

BlockDialog::BlockDialog( wxWindow* parent, iONode p_Props, bool save )
{
  m_TabAlign = wxGetApp().getTabAlign();
  Create(parent, -1, wxGetApp().getMsg("blocktable") );
  m_Props = p_Props;
  m_bSave = save;
  initLabels();

  initIndex();
  if( m_Props != NULL ) {
    initValues();
    //m_Notebook->SetSelection( 1 );
    wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, ID_PANEL_BK_GENERAL );
    wxPostEvent( this, event );
  }

  m_IndexPanel->GetSizer()->Layout();
  m_General_Panel->GetSizer()->Layout();
  m_LocationPanel->GetSizer()->Layout();
  m_PanelWirering->GetSizer()->Layout();
  m_RoutesPanel->GetSizer()->Layout();
  m_PanelDetails->GetSizer()->Layout();
  m_Interface->GetSizer()->Layout();

  m_Notebook->Fit();

  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);
}

void BlockDialog::OnSelectPage(wxCommandEvent& event) {
  m_Notebook->SetSelection( 1 );
}

/* comparator for sorting by id: */
static int __sortStr(obj* _a, obj* _b)
{
    const char* a = (const char*)*_a;
    const char* b = (const char*)*_b;
    return strcmp( a, b );
}

void BlockDialog::initSignalCombos() {
  m_Signal->Append( _T("-") );
  m_WSignal->Append( _T("-") );
  m_SignalR->Append( _T("-") );
  m_WSignalR->Append( _T("-") );

  iONode model = wxGetApp().getModel();
  iOList list = ListOp.inst();

  if( model != NULL ) {
    iONode sglist = wPlan.getsglist( model );
    if( sglist != NULL ) {
      int cnt = NodeOp.getChildCnt( sglist );
      for( int i = 0; i < cnt; i++ ) {
        iONode sg = NodeOp.getChild( sglist, i );
        const char* id = wSignal.getid( sg );
        if( id != NULL ) {
          ListOp.add(list, (obj)id);
		}
	  }
      ListOp.sort(list, &__sortStr);
      cnt = ListOp.size( list );
      for( int i = 0; i < cnt; i++ ) {
        const char* id = (const char*)ListOp.get( list, i );
          m_Signal->Append( wxString(id,wxConvUTF8) );
          m_WSignal->Append( wxString(id,wxConvUTF8) );
          m_SignalR->Append( wxString(id,wxConvUTF8) );
          m_WSignalR->Append( wxString(id,wxConvUTF8) );
      }
    }
  }
  /* clean up the temp. list */
  ListOp.base.del(list);
}
void BlockDialog::initSwitchCombo() {
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
          ListOp.add(list, (obj)id);
		}
	  }
      ListOp.sort(list, &__sortStr);
      cnt = ListOp.size( list );
      for( int i = 0; i < cnt; i++ ) {
        const char* id = (const char*)ListOp.get( list, i );
        m_ChoiceTurnouts->Append( wxString(id,wxConvUTF8) );
      }
    }
  }
  /* clean up the temp. list */
  ListOp.base.del(list);
}

void BlockDialog::initTurntableCombo() {
  iONode model = wxGetApp().getModel();
  iOList list = ListOp.inst();

  m_TurntableID->Append( _T("-") );

  if( model != NULL ) {
    iONode ttlist = wPlan.getttlist( model );
    if( ttlist != NULL ) {
      int cnt = NodeOp.getChildCnt( ttlist );
      for( int i = 0; i < cnt; i++ ) {
        iONode tt = NodeOp.getChild( ttlist, i );
        const char* id = wItem.getid( tt );
        if( id != NULL ) {
          ListOp.add(list, (obj)id);
    }
    }
      ListOp.sort(list, &__sortStr);
      cnt = ListOp.size( list );
      for( int i = 0; i < cnt; i++ ) {
        const char* id = (const char*)ListOp.get( list, i );
        m_TurntableID->Append( wxString(id,wxConvUTF8) );
      }
    }
  }
  /* clean up the temp. list */
  ListOp.base.del(list);
}

void BlockDialog::initLocPermissionList() {
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


void BlockDialog::initLabels() {
  m_Notebook->SetPageText( 0, wxGetApp().getMsg( "index" ) );
  m_Notebook->SetPageText( 1, wxGetApp().getMsg( "general" ) );
  m_Notebook->SetPageText( 2, wxGetApp().getMsg( "location" ) );
  m_Notebook->SetPageText( 3, wxGetApp().getMsg( "signals" ) );
  m_Notebook->SetPageText( 4, wxGetApp().getMsg( "details" ) );
  m_Notebook->SetPageText( 5, wxGetApp().getMsg( "routes" ) );
  m_Notebook->SetPageText( 6, wxGetApp().getMsg( "interface" ) );
  m_Notebook->SetPageText( 7, wxGetApp().getMsg( "permissions" ) );

  // Index
  m_New->SetLabel( wxGetApp().getMsg( "new" ) );
  m_Delete->SetLabel( wxGetApp().getMsg( "delete" ) );

  // General
  m_label_ID->SetLabel( wxGetApp().getMsg( "id" ) );
  m_Label_Description->SetLabel( wxGetApp().getMsg( "description" ) );
  m_Label_Length->SetLabel( wxGetApp().getMsg( "length" ) );
  m_LabelLocID->SetLabel( wxGetApp().getMsg( "locid" ) );
  m_labTurntableID->SetLabel( wxGetApp().getMsg( "turntableid" ) );
  m_Commuter->SetLabel( wxGetApp().getMsg( "commuter" ) );
  m_Commuter->SetString( 0, wxGetApp().getMsg( "no" ) );
  m_Commuter->SetString( 1, wxGetApp().getMsg( "yes" ) );
  m_Commuter->SetString( 2, wxGetApp().getMsg( "only" ) );
  m_Configuration->GetStaticBox()->SetLabel( wxGetApp().getMsg( "configuration" ) );
  m_Electrified->SetLabel( wxGetApp().getMsg( "electrified" ) );
  m_Station->SetLabel( wxGetApp().getMsg( "station" ) );
  m_Closed->SetLabel( wxGetApp().getMsg( "outoforder" ) );
  //m_Critical->SetLabel( wxGetApp().getMsg( "criticalsection" ) );
  m_GoManual->SetLabel( wxGetApp().getMsg( "gomanual" ) );
  m_AcceptGhost->SetLabel( wxGetApp().getMsg( "acceptghost" ) );
  m_Swapplacing->SetLabel( wxGetApp().getMsg( "terminalstation" ) );
  m_WaitFlag->SetLabel( wxGetApp().getMsg( "wait" ) );
  m_Road->SetLabel( wxGetApp().getMsg( "road" ) );
  m_AllowChgDir->SetLabel( wxGetApp().getMsg( "allowchgdir" ) );

  initTurntableCombo();

  // Location
  m_LabelX->SetLabel( wxGetApp().getMsg( "x" ) );
  m_LabelY->SetLabel( wxGetApp().getMsg( "y" ) );
  m_LabelZ->SetLabel( wxGetApp().getMsg( "z" ) );
  m_labOverlappedTurnouts->SetLabel(wxGetApp().getMsg( "overlappedturnouts" ) );
  m_AddTurnout->SetLabel( wxGetApp().getMsg( "add" ) );
  m_RemoveTurnout->SetLabel( wxGetApp().getMsg( "delete" ) );
  m_Location->SetLabel( wxGetApp().getMsg( "location" ) );

  // Initialize sorted Switch Combo
  initSwitchCombo();
  /*
  m_ori->SetLabel( wxGetApp().getMsg( "orientation" ) );
  m_ori->SetString( 0, wxGetApp().getMsg( "north" ) );
  m_ori->SetString( 1, wxGetApp().getMsg( "east" ) );
  m_ori->SetString( 2, wxGetApp().getMsg( "south" ) );
  m_ori->SetString( 3, wxGetApp().getMsg( "west" ) );
  */

  // Signals
  m_ForwardSignalsBox->SetLabel( wxGetApp().getMsg( "forwardsignals" ) );
  m_ReverseSignalsBox->SetLabel( wxGetApp().getMsg( "reversesignals" ) );
  m_LabelSignal->SetLabel( wxGetApp().getMsg( "signal" ) );
  m_LabelWSignal->SetLabel( wxGetApp().getMsg( "distant_signal" ) );
  m_LabelSignalR->SetLabel( wxGetApp().getMsg( "signal" ) );
  m_LabelWSignalR->SetLabel( wxGetApp().getMsg( "distant_signal" ) );

  // Initialize sorted Signal Combos
  initSignalCombos();

  // Details
  m_SpeedBox->SetLabel( wxGetApp().getMsg( "speed" ) );
  m_Speed->SetLabel( wxGetApp().getMsg( "arrive" ) );
  m_Speed->SetString( 0, wxGetApp().getMsg( "min" ) );
  m_Speed->SetString( 1, wxGetApp().getMsg( "mid" ) );
  m_Speed->SetString( 2, wxGetApp().getMsg( "cruise" ) );
  m_Speed->SetString( 3, wxGetApp().getMsg( "max" ) );
  m_Speed->SetString( 4, _T("%") );
  m_ExitSpeed->SetLabel( wxGetApp().getMsg( "departure" ) );
  m_ExitSpeed->SetString( 0, wxGetApp().getMsg( "min" ) );
  m_ExitSpeed->SetString( 1, wxGetApp().getMsg( "mid" ) );
  m_ExitSpeed->SetString( 2, wxGetApp().getMsg( "cruise" ) );
  m_ExitSpeed->SetString( 3, wxGetApp().getMsg( "max" ) );

  m_Type->SetLabel( wxGetApp().getMsg( "type" ) );
  m_Type->SetString( 0, wxGetApp().getMsg( "none" ) );
  m_Type->SetString( 1, wxGetApp().getMsg( "local" ) );
  m_Type->SetString( 2, wxGetApp().getMsg( "ice" ) );
  m_Type->SetString( 3, wxGetApp().getMsg( "goods" ) );
  m_Type->SetString( 4, wxGetApp().getMsg( "shunting" ) );
  m_Type->SetString( 5, wxGetApp().getMsg( "turntable" ) );
  m_Incline->SetLabel( wxGetApp().getMsg( "incline" ) );
  m_Incline->SetString( 0, wxGetApp().getMsg( "none" ) );
  m_Incline->SetString( 1, wxGetApp().getMsg( "up" ) );
  m_Incline->SetString( 2, wxGetApp().getMsg( "down" ) );
  m_Wait->SetLabel( wxGetApp().getMsg( "wait" ) );
  m_Wait->SetString( 0, wxGetApp().getMsg( "random" ) );
  m_Wait->SetString( 1, wxGetApp().getMsg( "fixed" ) );
  m_Wait->SetString( 2, wxGetApp().getMsg( "loc" ) );
  m_Wait->SetString( 3, wxGetApp().getMsg( "no" ) );
  m_WaitDetails->GetStaticBox()->SetLabel( wxGetApp().getMsg( "waitdetails" ) );
  m_LabelRandomMin->SetLabel( wxGetApp().getMsg( "randommin" ) );
  m_LabelRandomMax->SetLabel( wxGetApp().getMsg( "randommax" ) );
  m_LabelFixed->SetLabel( wxGetApp().getMsg( "fixed" ) );

  m_Measurement->SetLabel( wxGetApp().getMsg( "measurement" ) );
  m_labScale->SetLabel( wxGetApp().getMsg( "scale" ) );
  m_labDistance->SetLabel( wxGetApp().getMsg( "distance" ) );

  // Routes
  m_RouteProps->SetLabel( wxGetApp().getMsg( "properties" ) );
  m_RouteTest->SetLabel( wxGetApp().getMsg( "test" ) );

  // Route Sensors
  m_LabelSensorIDs->SetLabel( wxGetApp().getMsg( "id" ) );
  m_LabelSensorActions->SetLabel( wxGetApp().getMsg( "event" ) );
  m_LabelSensorEnd->SetLabel( wxGetApp().getMsg( "endpulse" ) );
  m_LabelGhost->SetLabel( wxGetApp().getMsg( "ghost" ) );
  m_labTimer->SetLabel( wxGetApp().getMsg( "eventtimer" ) );
  initSensorCombos();

  // Interface
  m_Labeliid->SetLabel( wxGetApp().getMsg( "iid" ) );
  m_LabelAddress->SetLabel( wxGetApp().getMsg( "address" ) );
  m_LabelPort->SetLabel( wxGetApp().getMsg( "port" ) );
  m_labTD->SetLabel( wxGetApp().getMsg( "trackdriver" ) );
  m_TD->SetLabel( wxGetApp().getTip( "trackdriver" ) );

  // Permissions
  m_labInclude->SetLabel( wxGetApp().getMsg( "include" ) );
  m_labExclude->SetLabel( wxGetApp().getMsg( "exclude" ) );

  // Initialize sorted Loco Permission List
  initLocPermissionList();

  // Buttons
  m_OK->SetLabel( wxGetApp().getMsg( "ok" ) );
  m_Cancel->SetLabel( wxGetApp().getMsg( "cancel" ) );
  m_Apply->SetLabel( wxGetApp().getMsg( "apply" ) );
}


void BlockDialog::initIndex() {
  TraceOp.trc( "blockdlg", TRCLEVEL_INFO, __LINE__, 9999, "InitIndex" );
  iONode l_Props = m_Props;
  m_List->Clear();
  iONode model = wxGetApp().getModel();
  if( model != NULL ) {
    iONode bklist = wPlan.getbklist( model );
    if( bklist != NULL ) {
      int cnt = NodeOp.getChildCnt( bklist );
      for( int i = 0; i < cnt; i++ ) {
        iONode bk = NodeOp.getChild( bklist, i );
        const char* id = wBlock.getid( bk );
        if( id != NULL ) {
          m_List->Append( wxString(id,wxConvUTF8) );
        }
      }
      if( l_Props != NULL ) {
        m_List->SetStringSelection( wxString(wBlock.getid( l_Props ),wxConvUTF8) );
        m_List->SetFirstItem( wxString(wBlock.getid( l_Props ),wxConvUTF8) );
        m_Props = l_Props;
      }

    }
  }
}


void BlockDialog::initValues() {
  char* title = StrOp.fmt( "%s %s", (const char*)wxGetApp().getMsg("block").mb_str(wxConvUTF8), wBlock.getid( m_Props ) );
  SetTitle( wxString(title,wxConvUTF8) );
  StrOp.free( title );

  // General
  m_ID->SetValue( wxString(wBlock.getid( m_Props ),wxConvUTF8) );
  m_Description->SetValue( wxString(wBlock.getdesc( m_Props ),wxConvUTF8) );
  wxString val; val.Printf( _T("%d"), wBlock.getlen( m_Props ) );
  m_Length->SetValue( val );
  m_LocID->Disable();
  if( wBlock.getlocid( m_Props ) != NULL )
    m_LocID->SetValue( wxString(wBlock.getlocid( m_Props ),wxConvUTF8) );
  else
    m_LocID->SetValue( _T("") );

  if( wBlock.getttid( m_Props ) != NULL && StrOp.len(wBlock.getttid( m_Props )) > 0 )
    m_TurntableID->SetStringSelection( wxString(wBlock.getttid( m_Props ),wxConvUTF8) );
  else
    m_TurntableID->SetSelection(0);

  if( StrOp.equals( "no", wBlock.getcommuter( m_Props ) ) )
    m_Commuter->SetSelection(0);
  else if( StrOp.equals( "yes", wBlock.getcommuter( m_Props ) ) )
    m_Commuter->SetSelection(1);
  else if( StrOp.equals( "only", wBlock.getcommuter( m_Props ) ) )
    m_Commuter->SetSelection(2);
  else
    m_Commuter->SetSelection(0);

  m_Electrified->SetValue( wBlock.iselectrified(m_Props) );
  m_Station->SetValue( wBlock.isstation(m_Props) );
  if( StrOp.equals( wBlock.closed, wBlock.getstate( m_Props ) ) )
    m_Closed->SetValue( true );
  else
    m_Closed->SetValue( false );
  //m_Critical->SetValue( wBlock.iscritical(m_Props) );
  m_GoManual->SetValue( wBlock.isgomanual(m_Props) );
  m_AcceptGhost->SetValue( wBlock.isacceptghost(m_Props) );
  m_Swapplacing->SetValue( wBlock.isterminalstation(m_Props) );
  m_WaitFlag->SetValue( wBlock.iswait(m_Props) );
  m_Road->SetValue( wItem.isroad(m_Props) );
  m_AllowChgDir->SetValue( wBlock.isallowchgdir(m_Props) );

  // Location
  char* cval = StrOp.fmt( "%d", wBlock.getx( m_Props ) );
  m_x->SetValue( wxString(cval,wxConvUTF8) ); StrOp.free( cval );
  cval = StrOp.fmt( "%d", wBlock.gety( m_Props ) );
  m_y->SetValue( wxString(cval,wxConvUTF8) ); StrOp.free( cval );
  cval = StrOp.fmt( "%d", wBlock.getz( m_Props ) );
  m_z->SetValue( wxString(cval,wxConvUTF8) ); StrOp.free( cval );
  m_Ori->SetValue( wxString(wItem.getori( m_Props ),wxConvUTF8) );

  m_TurnoutList->Clear();
  iOStrTok tok = StrTokOp.inst( wBlock.getturnouts( m_Props ), ',' );
  while( StrTokOp.hasMoreTokens( tok ) )  {
    const char* id = StrTokOp.nextToken( tok );
    if( StrOp.len( id ) > 0 )
      m_TurnoutList->Append( wxString(id,wxConvUTF8) );
  }


  /*
  if( StrOp.equals( wItem.north, wBlock.getori( m_Props ) ) )
    m_ori->SetSelection( 0 );
  else if( StrOp.equals( wItem.east, wBlock.getori( m_Props ) ) || wBlock.getori( m_Props ) == NULL )
    m_ori->SetSelection( 1 );
  else if( StrOp.equals( wItem.south, wBlock.getori( m_Props ) ) )
    m_ori->SetSelection( 2 );
  else if( StrOp.equals( wItem.west, wBlock.getori( m_Props ) ) )
    m_ori->SetSelection( 3 );
  */

  // Signals
  m_Signal->SetSelection(0);
  m_WSignal->SetSelection(0);
  m_SignalR->SetSelection(0);
  m_WSignalR->SetSelection(0);

  if( wBlock.getsignal( m_Props ) != NULL && StrOp.len( wBlock.getsignal( m_Props ) ) > 0 )
    m_Signal->SetStringSelection( wxString(wBlock.getsignal(m_Props),wxConvUTF8) );
  if( wBlock.getwsignal( m_Props ) != NULL && StrOp.len( wBlock.getwsignal( m_Props ) ) > 0 )
    m_WSignal->SetStringSelection( wxString(wBlock.getwsignal(m_Props),wxConvUTF8) );

  if( wBlock.getsignalR( m_Props ) != NULL && StrOp.len( wBlock.getsignalR( m_Props ) ) > 0 )
    m_SignalR->SetStringSelection( wxString(wBlock.getsignalR(m_Props),wxConvUTF8) );
  if( wBlock.getwsignalR( m_Props ) != NULL && StrOp.len( wBlock.getwsignalR( m_Props ) ) > 0 )
    m_WSignalR->SetStringSelection( wxString(wBlock.getwsignalR(m_Props),wxConvUTF8) );

  // Details
  int speed = 0;
  if( StrOp.equals( wBlock.min, wBlock.getspeed( m_Props ) ) )
    speed = 0;
  else if( StrOp.equals( wBlock.mid, wBlock.getspeed( m_Props ) ) )
    speed = 1;
  else if( StrOp.equals( wBlock.cruise, wBlock.getspeed( m_Props ) ) )
    speed = 2;
  else if( StrOp.equals( wBlock.max, wBlock.getspeed( m_Props ) ) )
    speed = 3;
  else if( StrOp.equals( wBlock.percent, wBlock.getspeed( m_Props ) ) )
    speed = 4;
  m_Speed->SetSelection( speed );

  speed = 0;
  if( StrOp.equals( wBlock.min, wBlock.getexitspeed( m_Props ) ) )
    speed = 0;
  else if( StrOp.equals( wBlock.mid, wBlock.getexitspeed( m_Props ) ) )
    speed = 1;
  else if( StrOp.equals( wBlock.cruise, wBlock.getexitspeed( m_Props ) ) )
    speed = 2;
  else if( StrOp.equals( wBlock.max, wBlock.getexitspeed( m_Props ) ) )
    speed = 3;
  m_ExitSpeed->SetSelection( speed );

  char * str;
  str = StrOp.fmt( "%d", wBlock.getspeedpercent(m_Props) );
  m_SpeedPercent->SetValue( wxString(str,wxConvUTF8) ); StrOp.free( str );

  int type = 0;
  if( StrOp.equals( wBlock.type_none, wBlock.gettype( m_Props ) ) )
    type = 0;
  if( StrOp.equals( wBlock.type_local, wBlock.gettype( m_Props ) ) )
    type = 1;
  else if( StrOp.equals( wBlock.type_ice, wBlock.gettype( m_Props ) ) )
    type = 2;
  else if( StrOp.equals( wBlock.type_goods, wBlock.gettype( m_Props ) ) )
    type = 3;
  else if( StrOp.equals( wBlock.type_shunting, wBlock.gettype( m_Props ) ) )
    type = 4;
  else if( StrOp.equals( wBlock.type_turntable, wBlock.gettype( m_Props ) ) )
    type = 5;
  m_Type->SetSelection( type );

  int incline = 0;
  if( wBlock.incline_none == wBlock.getincline( m_Props ) )
    incline = 0;
  else if( wBlock.incline_up == wBlock.getincline( m_Props ) )
    incline = 1;
  else if( wBlock.incline_down == wBlock.getincline( m_Props ) )
    incline = 2;
  m_Incline->SetSelection( incline );

  int wait = 0;
  if( StrOp.equals( wBlock.wait_random, wBlock.getwaitmode( m_Props ) ) )
    wait = 0;
  else if( StrOp.equals( wBlock.wait_fixed, wBlock.getwaitmode( m_Props ) ) )
    wait = 1;
  else if( StrOp.equals( wBlock.wait_loc, wBlock.getwaitmode( m_Props ) ) )
    wait = 2;
  if( !wBlock.iswait(m_Props) )
    wait = 3;
  m_Wait->SetSelection( wait );

  val.Printf( _T("%d"), wBlock.getminwaittime( m_Props ) );
  m_RandomMin->SetValue( val );
  val.Printf( _T("%d"), wBlock.getmaxwaittime( m_Props ) );
  m_RandomMax->SetValue( val );
  val.Printf( _T("%d"), wBlock.getwaittime( m_Props ) );
  m_Fixed->SetValue( val );

  val.Printf( _T("%d"), wBlock.getmvscale( m_Props ) );
  m_Scale->SetValue( val );
  val.Printf( _T("%d"), wBlock.getmvdistance( m_Props ) );
  m_Distance->SetValue( val );
  m_MPH->SetValue(wBlock.ismvmph( m_Props ));


  val.Printf( _T("%d"), wBlock.getevttimer( m_Props ) );
  m_Timer->SetValue( val );


  // Routes
  m_Routes->Clear();
  m_Routes->Append( wxString(wFeedbackEvent.from_all,wxConvUTF8), (void*)NULL );
  m_Routes->Append( wxString(wFeedbackEvent.from_all_reverse,wxConvUTF8), (void*)NULL );
  iONode model = wxGetApp().getModel();
  if( model != NULL ) {
    iONode stlist = wPlan.getstlist( model );
    if( stlist != NULL ) {
      int cnt = NodeOp.getChildCnt( stlist );
      for( int i = 0; i < cnt; i++ ) {
        iONode st = NodeOp.getChild( stlist, i );
        const char* id = wRoute.getid( st );
        const char* bka = wRoute.getbka( st );
        const char* bkb = wRoute.getbkb( st );
        if( bkb != NULL && StrOp.equals( bkb, wBlock.getid( m_Props ) ) ||
            bka != NULL && StrOp.equals( bka, wBlock.getid( m_Props ) ) && !wRoute.isdir(st) ) {
          char* str = StrOp.fmt( "%s = from \"%s\" to \"%s\"", id, bka, bkb );
          m_Routes->Append( wxString(str,wxConvUTF8), st );
          StrOp.free( str );
        }
      }
    }
  }

  // Interface
  m_iid->SetValue( wBlock.getiid( m_Props )==NULL?_T(""):wxString(wBlock.getiid( m_Props ),wxConvUTF8) );
  str = StrOp.fmt( "%d", wBlock.getaddr(m_Props) );
  m_Address->SetValue( wxString(str,wxConvUTF8) ); StrOp.free( str );
  str = StrOp.fmt( "%d", wBlock.getport(m_Props) );
  m_Port->SetValue( wxString(str,wxConvUTF8) ); StrOp.free( str );
  m_TD->SetValue( wBlock.istd(m_Props) );

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
  iONode excl = wBlock.getexcl( m_Props );
  while( excl != NULL ) {
    int nr = m_ExcludeList->FindString( wxString(wBlockExclude.getid(excl),wxConvUTF8) );
    if( nr != wxNOT_FOUND ) {
      m_ExcludeList->Select(nr);
    }
    excl = wBlock.nextexcl( m_Props, excl );
  };

  iONode incl = wBlock.getincl( m_Props );
  while( incl != NULL ) {
    int nr = m_IncludeList->FindString( wxString(wBlockInclude.getid(incl),wxConvUTF8) );
    if( nr != wxNOT_FOUND ) {
      m_IncludeList->Select(nr);
    }
    incl = wBlock.nextincl( m_Props, incl );
  };

}

bool BlockDialog::evaluate() {
  if( m_Props == NULL )
    return false;

  if( m_ID->GetValue().Len() == 0 ) {
    wxMessageDialog( this, wxGetApp().getMsg("invalidid"), _T("Rocrail"), wxOK | wxICON_ERROR ).ShowModal();
    m_ID->SetValue( wxString(wBlock.getid( m_Props ),wxConvUTF8) );
    return false;
  }
  // General
  wItem.setprev_id( m_Props, wItem.getid(m_Props) );
  wBlock.setid( m_Props, m_ID->GetValue().mb_str(wxConvUTF8) );
  wBlock.setdesc ( m_Props, m_Description->GetValue().mb_str(wxConvUTF8) );
  wBlock.setlen( m_Props, atoi( m_Length->GetValue().mb_str(wxConvUTF8) ) );

  wBlock.setelectrified( m_Props, m_Electrified->GetValue()?True:False );
  wBlock.setstation    ( m_Props, m_Station->GetValue()?True:False );
  wBlock.setstate      ( m_Props, m_Closed->GetValue()?wBlock.closed:wBlock.open );
  //wBlock.setcritical   ( m_Props, m_Critical->GetValue()?True:False );
  wBlock.setgomanual   ( m_Props, m_GoManual->GetValue()?True:False );
  wBlock.setacceptghost( m_Props, m_AcceptGhost->GetValue()?True:False );
  wBlock.setterminalstation( m_Props, m_Swapplacing->GetValue()?True:False );
  wBlock.setwait( m_Props, m_WaitFlag->GetValue()?True:False );
  wItem.setroad( m_Props, m_Road->IsChecked()?True:False );
  wBlock.setallowchgdir( m_Props, m_AllowChgDir->GetValue()?True:False );

  if( m_Commuter->GetSelection() == 0 )
    wBlock.setcommuter( m_Props, "no" );
  else if( m_Commuter->GetSelection() == 1 )
    wBlock.setcommuter( m_Props, "yes" );
  else if( m_Commuter->GetSelection() == 2 )
    wBlock.setcommuter( m_Props, "only" );

  // Location
  wBlock.setx( m_Props, atoi( m_x->GetValue().mb_str(wxConvUTF8) ) );
  wBlock.sety( m_Props, atoi( m_y->GetValue().mb_str(wxConvUTF8) ) );
  wBlock.setz( m_Props, atoi( m_z->GetValue().mb_str(wxConvUTF8) ) );

  int cnt = m_TurnoutList->GetCount();
  char* dst = NULL;
  for( int i = 0; i < cnt; i++ ) {
    if( i > 0 )
      dst = StrOp.cat( dst, "," );
    dst = StrOp.cat( dst, m_TurnoutList->GetString(i).mb_str(wxConvUTF8) );
  }
  wBlock.setturnouts( m_Props, dst==NULL ? "":dst );

  /*
  int ori = m_ori->GetSelection();
  if( ori == 0 )
    wBlock.setori( m_Props, wItem.north );
  else if( ori == 1 )
    wBlock.setori( m_Props, wItem.east );
  else if( ori == 2 )
    wBlock.setori( m_Props, wItem.south );
  else if( ori == 3 )
    wBlock.setori( m_Props, wItem.west );
  */

  // Turntable
  if( StrOp.equals( "-", m_TurntableID->GetStringSelection().mb_str(wxConvUTF8) ) )
    wBlock.setttid( m_Props, "" );
  else
    wBlock.setttid( m_Props, m_TurntableID->GetStringSelection().mb_str(wxConvUTF8) );

  // Signals
  if( StrOp.equals( "-", m_Signal->GetStringSelection().mb_str(wxConvUTF8) ) )
    wBlock.setsignal( m_Props, "" );
  else
    wBlock.setsignal( m_Props, m_Signal->GetStringSelection().mb_str(wxConvUTF8) );

  if( StrOp.equals( "-", m_WSignal->GetStringSelection().mb_str(wxConvUTF8) ) )
    wBlock.setwsignal( m_Props, "" );
  else
    wBlock.setwsignal( m_Props, m_WSignal->GetStringSelection().mb_str(wxConvUTF8) );

  if( StrOp.equals( "-", m_SignalR->GetStringSelection().mb_str(wxConvUTF8) ) )
    wBlock.setsignalR( m_Props, "" );
  else
    wBlock.setsignalR( m_Props, m_SignalR->GetStringSelection().mb_str(wxConvUTF8) );

  if( StrOp.equals( "-", m_WSignalR->GetStringSelection().mb_str(wxConvUTF8) ) )
    wBlock.setwsignalR( m_Props, "" );
  else
    wBlock.setwsignalR( m_Props, m_WSignalR->GetStringSelection().mb_str(wxConvUTF8) );

  // Details
  if( m_Speed->GetSelection() == 0 )
    wBlock.setspeed( m_Props, wBlock.min );
  else if( m_Speed->GetSelection() == 1 )
    wBlock.setspeed( m_Props, wBlock.mid );
  else if( m_Speed->GetSelection() == 2 )
    wBlock.setspeed( m_Props, wBlock.cruise );
  else if( m_Speed->GetSelection() == 3 )
    wBlock.setspeed( m_Props, wBlock.max );
  else if( m_Speed->GetSelection() == 4 ) {
    wBlock.setspeed( m_Props, wBlock.percent );
    wBlock.setspeedpercent( m_Props, m_SpeedPercent->GetValue() );
  }
  if( m_ExitSpeed->GetSelection() == 0 )
    wBlock.setexitspeed( m_Props, wBlock.min );
  else if( m_ExitSpeed->GetSelection() == 1 )
    wBlock.setexitspeed( m_Props, wBlock.mid );
  else if( m_ExitSpeed->GetSelection() == 2 )
    wBlock.setexitspeed( m_Props, wBlock.cruise );
  else if( m_ExitSpeed->GetSelection() == 3 )
    wBlock.setexitspeed( m_Props, wBlock.max );

  if( m_Type->GetSelection() == 0 )
    wBlock.settype( m_Props, wBlock.type_none );
  else if( m_Type->GetSelection() == 1 )
    wBlock.settype( m_Props, wBlock.type_local );
  else if( m_Type->GetSelection() == 2 )
    wBlock.settype( m_Props, wBlock.type_ice );
  else if( m_Type->GetSelection() == 3 )
    wBlock.settype( m_Props, wBlock.type_goods );
  else if( m_Type->GetSelection() == 4 )
    wBlock.settype( m_Props, wBlock.type_shunting );
  else if( m_Type->GetSelection() == 5 )
    wBlock.settype( m_Props, wBlock.type_turntable );

  if( m_Incline->GetSelection() == 0 )
    wBlock.setincline( m_Props, wBlock.incline_none );
  else if( m_Incline->GetSelection() == 1 )
    wBlock.setincline( m_Props, wBlock.incline_up );
  else if( m_Incline->GetSelection() == 2 )
    wBlock.setincline( m_Props, wBlock.incline_down );

  if( m_Wait->GetSelection() == 0 )
    wBlock.setwaitmode( m_Props, wBlock.wait_random );
  else if( m_Wait->GetSelection() == 1 )
    wBlock.setwaitmode( m_Props, wBlock.wait_fixed );
  else if( m_Wait->GetSelection() == 2 )
    wBlock.setwaitmode( m_Props, wBlock.wait_loc );
  else
    wBlock.setwait( m_Props, False );


  wBlock.setminwaittime( m_Props, atoi( m_RandomMin->GetValue().mb_str(wxConvUTF8) ) );
  wBlock.setmaxwaittime( m_Props, atoi( m_RandomMax->GetValue().mb_str(wxConvUTF8) ) );
  wBlock.setwaittime( m_Props, atoi( m_Fixed->GetValue().mb_str(wxConvUTF8) ) );

  wBlock.setmvscale( m_Props, atoi( m_Scale->GetValue().mb_str(wxConvUTF8) ) );
  wBlock.setmvdistance( m_Props, atoi( m_Distance->GetValue().mb_str(wxConvUTF8) ) );
  wBlock.setmvmph( m_Props, m_MPH->IsChecked()?True:False );

  wBlock.setevttimer( m_Props, atoi( m_Timer->GetValue().mb_str(wxConvUTF8) ) );

  // Interface
  wBlock.setiid( m_Props, m_iid->GetValue().mb_str(wxConvUTF8) );
  wBlock.setaddr( m_Props, atoi( m_Address->GetValue().mb_str(wxConvUTF8) ) );
  wBlock.setport( m_Props, atoi( m_Port->GetValue().mb_str(wxConvUTF8) ) );
  wBlock.settd( m_Props, m_TD->GetValue()?True:False );

  // Permissions
  // remove all excl and incl childnodes:
  iONode incl = wBlock.getincl( m_Props );
  while( incl != NULL ) {
    NodeOp.removeChild( m_Props, incl );
    NodeOp.base.del(incl);
    incl = wBlock.getincl( m_Props );
  };
  iONode excl = wBlock.getexcl( m_Props );
  while( excl != NULL ) {
    NodeOp.removeChild( m_Props, excl );
    NodeOp.base.del(excl);
    excl = wBlock.getexcl( m_Props );
  };

  wxArrayInt ai;
  cnt = m_ExcludeList->GetSelections(ai);
  for( int i = 0; i < cnt; i++ ) {
    int idx = ai.Item(i);
    excl = NodeOp.inst( wBlockExclude.name(), m_Props, ELEMENT_NODE );
    wBlockExclude.setid( excl, m_ExcludeList->GetString(idx).mb_str(wxConvUTF8) );
    NodeOp.addChild( m_Props, excl );
  }
  cnt = m_IncludeList->GetSelections(ai);
  for( int i = 0; i < cnt; i++ ) {
    int idx = ai.Item(i);
    incl = NodeOp.inst( wBlockInclude.name(), m_Props, ELEMENT_NODE );
    wBlockInclude.setid( incl, m_IncludeList->GetString(idx).mb_str(wxConvUTF8) );
    NodeOp.addChild( m_Props, incl );
  }


  // remove un used events
  iONode fb = wBlock.getfbevent( m_Props );
  iOList delList = ListOp.inst();
  while( fb != NULL ) {
    TraceOp.trc( "blockdlg", TRCLEVEL_INFO, __LINE__, 9999, "check usaged of fbevent from %s...", wFeedbackEvent.getfrom( fb ) );
    bool hasRoute = false;

    int cnt = m_Routes->GetCount();
    for( int i = 0; i < cnt; i++ ) {
      iONode st = (iONode)m_Routes->GetClientData(i);
      if( st == NULL )
        continue;

      const char* fromID = wRoute.getbka( st );
      if( fromID == NULL )
        continue;

      TraceOp.trc( "blockdlg", TRCLEVEL_INFO, __LINE__, 9999, "fromID %s", fromID );
      if( StrOp.equals( wFeedbackEvent.from_all, wFeedbackEvent.getfrom( fb ) ) ||
          StrOp.equals( wFeedbackEvent.from_all_reverse, wFeedbackEvent.getfrom( fb ) ) ||
          StrOp.equals( fromID, wFeedbackEvent.getfrom( fb ) ) )
      {
        hasRoute = true;
        break;
      }
      else if( !wRoute.isdir(st) && StrOp.equals( wRoute.getbkb( st ), wFeedbackEvent.getfrom( fb ) ) ) {
        hasRoute = true;
        break;
      }
    }

    if( !hasRoute ) {
      char* msg = StrOp.fmt( wxGetApp().getMsg("unusedfbevent").mb_str(wxConvUTF8), wFeedbackEvent.getfrom( fb ) );

      int rc = wxMessageDialog( this, wxString(msg,wxConvUTF8),
          wxGetApp().getMsg("blocktable"), wxYES_NO | wxICON_QUESTION ).ShowModal();
      StrOp.free(msg);
      if( rc == wxID_YES ) {
        // remove the fbevent
        ListOp.add( delList, (obj)fb );
      }
    }

    fb = wBlock.nextfbevent( m_Props, fb );
  };

  if( ListOp.size(delList) > 0 ) {
    fb = (iONode)ListOp.first( delList );
    while( fb != NULL ) {
      NodeOp.removeChild( m_Props, fb );
      NodeOp.base.del(fb);
      fb = (iONode)ListOp.next( delList );
    };
  }
  ListOp.base.del(delList);

  return true;
}

/*!
 * BlockDialog creator
 */

bool BlockDialog::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin BlockDialog member initialisation
    m_Notebook = NULL;
    m_IndexPanel = NULL;
    m_List = NULL;
    m_New = NULL;
    m_Delete = NULL;
    m_General_Panel = NULL;
    m_label_ID = NULL;
    m_ID = NULL;
    m_Label_Description = NULL;
    m_Description = NULL;
    m_Label_Length = NULL;
    m_Length = NULL;
    m_LabelLocID = NULL;
    m_LocID = NULL;
    m_LocProps = NULL;
    m_labTurntableID = NULL;
    m_TurntableID = NULL;
    m_Commuter = NULL;
    m_Configuration = NULL;
    m_Electrified = NULL;
    m_Station = NULL;
    m_Closed = NULL;
    m_WaitFlag = NULL;
    m_GoManual = NULL;
    m_AcceptGhost = NULL;
    m_Swapplacing = NULL;
    m_Road = NULL;
    m_AllowChgDir = NULL;
    m_LocationPanel = NULL;
    m_labOverlappedTurnouts = NULL;
    m_TurnoutList = NULL;
    m_ChoiceTurnouts = NULL;
    m_AddTurnout = NULL;
    m_RemoveTurnout = NULL;
    m_Location = NULL;
    m_LabelX = NULL;
    m_x = NULL;
    m_LabelY = NULL;
    m_y = NULL;
    m_LabelZ = NULL;
    m_z = NULL;
    m_Ori = NULL;
    m_PanelWirering = NULL;
    m_ForwardSignalsBox = NULL;
    m_LabelSignal = NULL;
    m_Signal = NULL;
    m_SignalProps = NULL;
    m_LabelWSignal = NULL;
    m_WSignal = NULL;
    m_WSignalProps = NULL;
    m_ReverseSignalsBox = NULL;
    m_LabelSignalR = NULL;
    m_SignalR = NULL;
    m_SignalPropsR = NULL;
    m_LabelWSignalR = NULL;
    m_WSignalR = NULL;
    m_WSignalPropsR = NULL;
    m_PanelDetails = NULL;
    m_Type = NULL;
    m_Incline = NULL;
    m_Wait = NULL;
    m_WaitDetails = NULL;
    m_LabelRandomMin = NULL;
    m_RandomMin = NULL;
    m_LabelRandomMax = NULL;
    m_RandomMax = NULL;
    m_LabelFixed = NULL;
    m_Fixed = NULL;
    m_Measurement = NULL;
    m_labScale = NULL;
    m_Scale = NULL;
    m_labDistance = NULL;
    m_Distance = NULL;
    m_MPH = NULL;
    m_SpeedBox = NULL;
    m_Speed = NULL;
    m_SpeedPercent = NULL;
    m_ExitSpeed = NULL;
    m_RoutesPanel = NULL;
    m_Routes = NULL;
    m_LabelSensorsFromBlock = NULL;
    m_LabelSensorIDs = NULL;
    m_LabelSensorActions = NULL;
    m_LabelSensorEnd = NULL;
    m_LabelGhost = NULL;
    m_LabelProps = NULL;
    m_SensorID1 = NULL;
    m_SensorAction1 = NULL;
    m_SensorEnd1 = NULL;
    m_Ghost1 = NULL;
    m_SensorProps1 = NULL;
    m_SensorID2 = NULL;
    m_SensorAction2 = NULL;
    m_SensorEnd2 = NULL;
    m_Ghost2 = NULL;
    m_SensorProps2 = NULL;
    m_SensorID3 = NULL;
    m_SensorAction3 = NULL;
    m_SensorEnd3 = NULL;
    m_Ghost3 = NULL;
    m_SensorProps3 = NULL;
    m_SensorID4 = NULL;
    m_SensorAction4 = NULL;
    m_SensorEnd4 = NULL;
    m_Ghost4 = NULL;
    m_SensorProps4 = NULL;
    m_SensorID5 = NULL;
    m_SensorAction5 = NULL;
    m_SensorEnd5 = NULL;
    m_Ghost5 = NULL;
    m_SensorProps5 = NULL;
    m_labTimer = NULL;
    m_Timer = NULL;
    m_RouteProps = NULL;
    m_RouteTest = NULL;
    m_Interface = NULL;
    m_Labeliid = NULL;
    m_iid = NULL;
    m_LabelAddress = NULL;
    m_Address = NULL;
    m_LabelPort = NULL;
    m_Port = NULL;
    m_labTD = NULL;
    m_TD = NULL;
    m_PermissionsPanel = NULL;
    m_labInclude = NULL;
    m_IncludeList = NULL;
    m_labExclude = NULL;
    m_ExcludeList = NULL;
    m_Cancel = NULL;
    m_Apply = NULL;
    m_OK = NULL;
////@end BlockDialog member initialisation

////@begin BlockDialog creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end BlockDialog creation
    return true;
}

/*!
 * Control creation for BlockDialog
 */

void BlockDialog::CreateControls()
{
////@begin BlockDialog content construction
    BlockDialog* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    m_Notebook = new wxNotebook( itemDialog1, ID_NOTEBOOK_BLOCKS, wxDefaultPosition, wxDefaultSize, wxNB_DEFAULT|m_TabAlign );

    m_IndexPanel = new wxPanel( m_Notebook, ID_PANEL_INDEX_BLOCKS, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxVERTICAL);
    m_IndexPanel->SetSizer(itemBoxSizer5);

    wxArrayString m_ListStrings;
    m_List = new wxListBox( m_IndexPanel, ID_LISTBOX_BLOCKS, wxDefaultPosition, wxDefaultSize, m_ListStrings, wxLB_SINGLE|wxLB_ALWAYS_SB|wxLB_SORT );
    itemBoxSizer5->Add(m_List, 1, wxGROW|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer7 = new wxFlexGridSizer(2, 2, 0, 0);
    itemBoxSizer5->Add(itemFlexGridSizer7, 0, wxGROW|wxALL, 5);
    m_New = new wxButton( m_IndexPanel, ID_BUTTON_BLOCK_NEW, _("New"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer7->Add(m_New, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Delete = new wxButton( m_IndexPanel, ID_BUTTON_BLOCK_DELETE, _("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer7->Add(m_Delete, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Notebook->AddPage(m_IndexPanel, _("Index"));

    m_General_Panel = new wxPanel( m_Notebook, ID_PANEL_BK_GENERAL, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer11 = new wxBoxSizer(wxVERTICAL);
    m_General_Panel->SetSizer(itemBoxSizer11);

    wxFlexGridSizer* itemFlexGridSizer12 = new wxFlexGridSizer(0, 2, 0, 0);
    itemFlexGridSizer12->AddGrowableCol(1);
    itemBoxSizer11->Add(itemFlexGridSizer12, 0, wxGROW|wxALL, 5);
    m_label_ID = new wxStaticText( m_General_Panel, wxID_STATIC_ID_BLOCK, _("ID:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer12->Add(m_label_ID, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    m_ID = new wxTextCtrl( m_General_Panel, ID_TEXTCTRL_ID_BLOCK, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer12->Add(m_ID, 1, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Label_Description = new wxStaticText( m_General_Panel, wxID_STATIC_DESCRIPTION_BLOCK, _("Description:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer12->Add(m_Label_Description, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    m_Description = new wxTextCtrl( m_General_Panel, ID_TEXTCTRL_DESCRIPTION_BLOCK, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer12->Add(m_Description, 1, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Label_Length = new wxStaticText( m_General_Panel, wxID_STATIC_LENGTH_BLOCK, _("Length:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer12->Add(m_Label_Length, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    m_Length = new wxTextCtrl( m_General_Panel, ID_TEXTCTRL_LENGTH_BLOCK, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer12->Add(m_Length, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer19 = new wxFlexGridSizer(1, 3, 0, 0);
    itemFlexGridSizer19->AddGrowableCol(1);
    itemBoxSizer11->Add(itemFlexGridSizer19, 0, wxGROW|wxALL, 5);
    m_LabelLocID = new wxStaticText( m_General_Panel, wxID_STATIC_BK_LOCID, _("LocID:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer19->Add(m_LabelLocID, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    m_LocID = new wxTextCtrl( m_General_Panel, ID_TEXTCTRL_BK_LOCID, _T(""), wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
    itemFlexGridSizer19->Add(m_LocID, 2, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_LocProps = new wxButton( m_General_Panel, ID_BUTTON_BK_LOCPROPS, _("..."), wxDefaultPosition, wxSize(25, -1), 0 );
    itemFlexGridSizer19->Add(m_LocProps, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    m_labTurntableID = new wxStaticText( m_General_Panel, wxID_ANY, _("Turntable ID:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer19->Add(m_labTurntableID, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxArrayString m_TurntableIDStrings;
    m_TurntableID = new wxChoice( m_General_Panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_TurntableIDStrings, 0 );
    itemFlexGridSizer19->Add(m_TurntableID, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer25 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer11->Add(itemBoxSizer25, 0, wxALIGN_LEFT|wxALL, 5);
    wxArrayString m_CommuterStrings;
    m_CommuterStrings.Add(_("&no"));
    m_CommuterStrings.Add(_("&yes"));
    m_CommuterStrings.Add(_("&only"));
    m_Commuter = new wxRadioBox( m_General_Panel, ID_RADIOBOX_BK_COMMUTER, _("Commuter train"), wxDefaultPosition, wxDefaultSize, m_CommuterStrings, 1, wxRA_SPECIFY_ROWS );
    m_Commuter->SetSelection(0);
    itemBoxSizer25->Add(m_Commuter, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticBox* itemStaticBoxSizer27Static = new wxStaticBox(m_General_Panel, wxID_ANY_BK_CONFIG, _("Configuration"));
    m_Configuration = new wxStaticBoxSizer(itemStaticBoxSizer27Static, wxVERTICAL);
    itemBoxSizer11->Add(m_Configuration, 0, wxGROW|wxALL, 5);
    wxBoxSizer* itemBoxSizer28 = new wxBoxSizer(wxHORIZONTAL);
    m_Configuration->Add(itemBoxSizer28, 0, wxALIGN_LEFT|wxALL, 5);
    wxBoxSizer* itemBoxSizer29 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer28->Add(itemBoxSizer29, 0, wxGROW|wxALL, 5);
    m_Electrified = new wxCheckBox( m_General_Panel, ID_CHECKBOX_BK_ELECTRIFIED, _("Electrified"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    m_Electrified->SetValue(false);
    itemBoxSizer29->Add(m_Electrified, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT, 5);

    m_Station = new wxCheckBox( m_General_Panel, ID_CHECKBOX_BK_STATION, _("Station"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    m_Station->SetValue(false);
    itemBoxSizer29->Add(m_Station, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT, 5);

    m_Closed = new wxCheckBox( m_General_Panel, ID_CHECKBOX_BK_CLOSED, _("Closed"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    m_Closed->SetValue(false);
    itemBoxSizer29->Add(m_Closed, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT, 5);

    m_WaitFlag = new wxCheckBox( m_General_Panel, wxID_BLOCK_WAIT, _("Wait"), wxDefaultPosition, wxDefaultSize, 0 );
    m_WaitFlag->SetValue(false);
    itemBoxSizer29->Add(m_WaitFlag, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT, 5);

    wxBoxSizer* itemBoxSizer34 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer28->Add(itemBoxSizer34, 0, wxGROW|wxALL, 5);
    m_GoManual = new wxCheckBox( m_General_Panel, ID_CHECKBOX_BK_GOMANUAL, _("Allow half-automatic"), wxDefaultPosition, wxDefaultSize, 0 );
    m_GoManual->SetValue(false);
    itemBoxSizer34->Add(m_GoManual, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT, 5);

    m_AcceptGhost = new wxCheckBox( m_General_Panel, ID_CHECKBOX_BK_ACCGHOST, _("Accept Ghosttrain"), wxDefaultPosition, wxDefaultSize, 0 );
    m_AcceptGhost->SetValue(false);
    itemBoxSizer34->Add(m_AcceptGhost, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT, 5);

    m_Swapplacing = new wxCheckBox( m_General_Panel, wxID_ANY, _("Terminal station"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Swapplacing->SetValue(false);
    itemBoxSizer34->Add(m_Swapplacing, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT, 5);

    m_Road = new wxCheckBox( m_General_Panel, wxID_ANY, _("Road"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Road->SetValue(false);
    itemBoxSizer34->Add(m_Road, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT, 5);

    m_AllowChgDir = new wxCheckBox( m_General_Panel, wxID_ANY, _("allowchgdir"), wxDefaultPosition, wxDefaultSize, 0 );
    m_AllowChgDir->SetValue(false);
    itemBoxSizer34->Add(m_AllowChgDir, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT, 5);

    m_Notebook->AddPage(m_General_Panel, _("General"));

    m_LocationPanel = new wxPanel( m_Notebook, ID_PANEL_BK_LOCATION, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer41 = new wxBoxSizer(wxVERTICAL);
    m_LocationPanel->SetSizer(itemBoxSizer41);

    m_labOverlappedTurnouts = new wxStaticBox(m_LocationPanel, wxID_ANY, _("Overlapped Turnouts"));
    wxStaticBoxSizer* itemStaticBoxSizer42 = new wxStaticBoxSizer(m_labOverlappedTurnouts, wxVERTICAL);
    itemBoxSizer41->Add(itemStaticBoxSizer42, 1, wxGROW|wxALL, 5);
    wxArrayString m_TurnoutListStrings;
    m_TurnoutList = new wxListBox( m_LocationPanel, ID_LISTBOX_BLOCK_TURNOUTS, wxDefaultPosition, wxDefaultSize, m_TurnoutListStrings, wxLB_SINGLE );
    itemStaticBoxSizer42->Add(m_TurnoutList, 1, wxGROW|wxALL, 5);

    wxArrayString m_ChoiceTurnoutsStrings;
    m_ChoiceTurnouts = new wxChoice( m_LocationPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_ChoiceTurnoutsStrings, 0 );
    itemStaticBoxSizer42->Add(m_ChoiceTurnouts, 0, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer45 = new wxBoxSizer(wxHORIZONTAL);
    itemStaticBoxSizer42->Add(itemBoxSizer45, 0, wxALIGN_LEFT|wxALL, 5);
    m_AddTurnout = new wxButton( m_LocationPanel, ID_BUTTON_BLOCK_ADD_TURNOUT, _("Add"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer45->Add(m_AddTurnout, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_RemoveTurnout = new wxButton( m_LocationPanel, ID_BUTTON_BLOCK_REMOVE_TURNOUT, _("Remove"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer45->Add(m_RemoveTurnout, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Location = new wxStaticBox(m_LocationPanel, wxID_ANY, _("Location"));
    wxStaticBoxSizer* itemStaticBoxSizer48 = new wxStaticBoxSizer(m_Location, wxVERTICAL);
    itemBoxSizer41->Add(itemStaticBoxSizer48, 0, wxALIGN_LEFT|wxALL, 5);
    wxFlexGridSizer* itemFlexGridSizer49 = new wxFlexGridSizer(2, 4, 0, 0);
    itemStaticBoxSizer48->Add(itemFlexGridSizer49, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
    m_LabelX = new wxStaticText( m_LocationPanel, wxID_ANY, _("x"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer49->Add(m_LabelX, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    m_x = new wxTextCtrl( m_LocationPanel, ID_TEXTCTRL, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer49->Add(m_x, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_LabelY = new wxStaticText( m_LocationPanel, wxID_ANY, _("y"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer49->Add(m_LabelY, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    m_y = new wxTextCtrl( m_LocationPanel, ID_TEXTCTRL1, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer49->Add(m_y, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_LabelZ = new wxStaticText( m_LocationPanel, wxID_ANY, _("z"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer49->Add(m_LabelZ, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    m_z = new wxTextCtrl( m_LocationPanel, ID_TEXTCTRL2, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer49->Add(m_z, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText56 = new wxStaticText( m_LocationPanel, wxID_ANY, _("ori"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer49->Add(itemStaticText56, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Ori = new wxTextCtrl( m_LocationPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
    itemFlexGridSizer49->Add(m_Ori, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Notebook->AddPage(m_LocationPanel, _("Location"));

    m_PanelWirering = new wxPanel( m_Notebook, ID_PANEL_BLOCK_WIRERING, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer59 = new wxBoxSizer(wxVERTICAL);
    m_PanelWirering->SetSizer(itemBoxSizer59);

    m_ForwardSignalsBox = new wxStaticBox(m_PanelWirering, wxID_ANY, _("Forward Signals"));
    wxStaticBoxSizer* itemStaticBoxSizer60 = new wxStaticBoxSizer(m_ForwardSignalsBox, wxVERTICAL);
    itemBoxSizer59->Add(itemStaticBoxSizer60, 0, wxGROW|wxALL, 5);
    wxFlexGridSizer* itemFlexGridSizer61 = new wxFlexGridSizer(2, 3, 0, 0);
    itemFlexGridSizer61->AddGrowableCol(1);
    itemStaticBoxSizer60->Add(itemFlexGridSizer61, 0, wxGROW|wxALL, 5);
    m_LabelSignal = new wxStaticText( m_PanelWirering, ID_STATICTEXT_BK_SIGNAL, _("Signal"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer61->Add(m_LabelSignal, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxArrayString m_SignalStrings;
    m_Signal = new wxComboBox( m_PanelWirering, ID_COMBOBOX_BK_SIGNAL, _T(""), wxDefaultPosition, wxDefaultSize, m_SignalStrings, wxCB_READONLY );
    itemFlexGridSizer61->Add(m_Signal, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_SignalProps = new wxButton( m_PanelWirering, ID_BUTTON_BK_SIGNAL, _("..."), wxDefaultPosition, wxSize(25, -1), 0 );
    itemFlexGridSizer61->Add(m_SignalProps, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_LabelWSignal = new wxStaticText( m_PanelWirering, wxID_STATIC_BK_WSIGNAL, _("Warning Signal"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer61->Add(m_LabelWSignal, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxArrayString m_WSignalStrings;
    m_WSignal = new wxComboBox( m_PanelWirering, ID_COMBOBOX_BK_WSIGNAL, _T(""), wxDefaultPosition, wxDefaultSize, m_WSignalStrings, wxCB_READONLY );
    itemFlexGridSizer61->Add(m_WSignal, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_WSignalProps = new wxButton( m_PanelWirering, ID_BUTTON_BK_WSIGNAL, _("..."), wxDefaultPosition, wxSize(25, -1), 0 );
    itemFlexGridSizer61->Add(m_WSignalProps, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_ReverseSignalsBox = new wxStaticBox(m_PanelWirering, wxID_ANY, _("Reverse Signals"));
    wxStaticBoxSizer* itemStaticBoxSizer68 = new wxStaticBoxSizer(m_ReverseSignalsBox, wxVERTICAL);
    itemBoxSizer59->Add(itemStaticBoxSizer68, 0, wxGROW|wxALL, 5);
    wxFlexGridSizer* itemFlexGridSizer69 = new wxFlexGridSizer(2, 3, 0, 0);
    itemFlexGridSizer69->AddGrowableCol(1);
    itemStaticBoxSizer68->Add(itemFlexGridSizer69, 0, wxGROW|wxALL, 5);
    m_LabelSignalR = new wxStaticText( m_PanelWirering, wxID_ANY, _("Signal"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer69->Add(m_LabelSignalR, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxArrayString m_SignalRStrings;
    m_SignalR = new wxComboBox( m_PanelWirering, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, m_SignalRStrings, wxCB_READONLY );
    itemFlexGridSizer69->Add(m_SignalR, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_SignalPropsR = new wxButton( m_PanelWirering, ID_BUTTON_BK_SIGNAL_R, _("..."), wxDefaultPosition, wxSize(25, -1), 0 );
    itemFlexGridSizer69->Add(m_SignalPropsR, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_LabelWSignalR = new wxStaticText( m_PanelWirering, wxID_ANY, _("Warning Signal"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer69->Add(m_LabelWSignalR, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxArrayString m_WSignalRStrings;
    m_WSignalR = new wxComboBox( m_PanelWirering, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, m_WSignalRStrings, wxCB_READONLY );
    itemFlexGridSizer69->Add(m_WSignalR, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_WSignalPropsR = new wxButton( m_PanelWirering, ID_BUTTON_BK_WSIGNAL_R, _("..."), wxDefaultPosition, wxSize(25, -1), 0 );
    itemFlexGridSizer69->Add(m_WSignalPropsR, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Notebook->AddPage(m_PanelWirering, _("Signals"));

    m_PanelDetails = new wxPanel( m_Notebook, ID_PANEL_BK_DETAILS, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer77 = new wxBoxSizer(wxVERTICAL);
    m_PanelDetails->SetSizer(itemBoxSizer77);

    wxFlexGridSizer* itemFlexGridSizer78 = new wxFlexGridSizer(1, 3, 0, 0);
    itemBoxSizer77->Add(itemFlexGridSizer78, 0, wxGROW|wxALL|wxADJUST_MINSIZE, 5);
    wxArrayString m_TypeStrings;
    m_TypeStrings.Add(_("&none"));
    m_TypeStrings.Add(_("&local"));
    m_TypeStrings.Add(_("&ice"));
    m_TypeStrings.Add(_("&goods"));
    m_TypeStrings.Add(_("&shunting"));
    m_TypeStrings.Add(_("&turntable"));
    m_Type = new wxRadioBox( m_PanelDetails, ID_RADIOBOX_BK_TYPE, _("Type"), wxDefaultPosition, wxDefaultSize, m_TypeStrings, 2, wxRA_SPECIFY_COLS );
    m_Type->SetSelection(0);
    itemFlexGridSizer78->Add(m_Type, 0, wxGROW|wxALIGN_TOP|wxALL, 5);

    wxArrayString m_InclineStrings;
    m_InclineStrings.Add(_("&none"));
    m_InclineStrings.Add(_("&up"));
    m_InclineStrings.Add(_("&down"));
    m_Incline = new wxRadioBox( m_PanelDetails, ID_RADIOBOX_BK_INCLINE, _("Incline"), wxDefaultPosition, wxDefaultSize, m_InclineStrings, 1, wxRA_SPECIFY_COLS );
    m_Incline->SetSelection(0);
    itemFlexGridSizer78->Add(m_Incline, 0, wxGROW|wxALIGN_TOP|wxALL, 5);

    wxArrayString m_WaitStrings;
    m_WaitStrings.Add(_("&random"));
    m_WaitStrings.Add(_("&fixed"));
    m_WaitStrings.Add(_("&loc"));
    m_WaitStrings.Add(_("&no"));
    m_Wait = new wxRadioBox( m_PanelDetails, ID_RADIOBOX_BK_WAIT, _("Wait"), wxDefaultPosition, wxDefaultSize, m_WaitStrings, 1, wxRA_SPECIFY_COLS );
    m_Wait->SetSelection(0);
    itemFlexGridSizer78->Add(m_Wait, 0, wxGROW|wxALIGN_TOP|wxALL, 5);

    wxBoxSizer* itemBoxSizer82 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer77->Add(itemBoxSizer82, 0, wxALIGN_LEFT|wxALL, 5);
    wxStaticBox* itemStaticBoxSizer83Static = new wxStaticBox(m_PanelDetails, wxID_ANY, _("Wait details"));
    m_WaitDetails = new wxStaticBoxSizer(itemStaticBoxSizer83Static, wxHORIZONTAL);
    itemBoxSizer82->Add(m_WaitDetails, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    wxFlexGridSizer* itemFlexGridSizer84 = new wxFlexGridSizer(2, 2, 0, 0);
    m_WaitDetails->Add(itemFlexGridSizer84, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    m_LabelRandomMin = new wxStaticText( m_PanelDetails, wxID_STATIC_BK_RANDOM_MIN, _("Radom min:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer84->Add(m_LabelRandomMin, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    m_RandomMin = new wxTextCtrl( m_PanelDetails, ID_TEXTCTRL_BK_RANDOM_MIN, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer84->Add(m_RandomMin, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5);

    m_LabelRandomMax = new wxStaticText( m_PanelDetails, wxID_STATIC_BK_RANDOM_MAX, _("Random max:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer84->Add(m_LabelRandomMax, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxADJUST_MINSIZE, 5);

    m_RandomMax = new wxTextCtrl( m_PanelDetails, ID_TEXTCTRL_BK_RANDOM_MAX, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer84->Add(m_RandomMax, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_LabelFixed = new wxStaticText( m_PanelDetails, wxID_STATIC_BK_FIXED, _("Fixed:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer84->Add(m_LabelFixed, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM|wxADJUST_MINSIZE, 5);

    m_Fixed = new wxTextCtrl( m_PanelDetails, ID_TEXTCTRL_BK_FIXED, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer84->Add(m_Fixed, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_Measurement = new wxStaticBox(m_PanelDetails, wxID_ANY, _("Measurement"));
    wxStaticBoxSizer* itemStaticBoxSizer91 = new wxStaticBoxSizer(m_Measurement, wxVERTICAL);
    itemBoxSizer82->Add(itemStaticBoxSizer91, 0, wxALIGN_TOP|wxALL, 5);
    wxFlexGridSizer* itemFlexGridSizer92 = new wxFlexGridSizer(2, 2, 0, 0);
    itemStaticBoxSizer91->Add(itemFlexGridSizer92, 0, wxALIGN_CENTER_HORIZONTAL|wxLEFT|wxRIGHT|wxTOP, 5);
    m_labScale = new wxStaticText( m_PanelDetails, wxID_ANY, _("Scale"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer92->Add(m_labScale, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5);

    m_Scale = new wxTextCtrl( m_PanelDetails, wxID_ANY, _("87"), wxDefaultPosition, wxSize(60, -1), wxTE_CENTRE );
    itemFlexGridSizer92->Add(m_Scale, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5);

    m_labDistance = new wxStaticText( m_PanelDetails, wxID_ANY, _("Distance"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer92->Add(m_labDistance, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_Distance = new wxTextCtrl( m_PanelDetails, wxID_ANY, _("0"), wxDefaultPosition, wxSize(60, -1), wxTE_CENTRE );
    itemFlexGridSizer92->Add(m_Distance, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_MPH = new wxCheckBox( m_PanelDetails, wxID_ANY, _("MPH"), wxDefaultPosition, wxDefaultSize, 0 );
    m_MPH->SetValue(false);
    itemStaticBoxSizer91->Add(m_MPH, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_SpeedBox = new wxStaticBox(m_PanelDetails, wxID_ANY, _("Speed"));
    wxStaticBoxSizer* itemStaticBoxSizer98 = new wxStaticBoxSizer(m_SpeedBox, wxHORIZONTAL);
    itemBoxSizer77->Add(itemStaticBoxSizer98, 0, wxALIGN_LEFT|wxALL, 5);
    wxFlexGridSizer* itemFlexGridSizer99 = new wxFlexGridSizer(2, 3, 0, 0);
    itemStaticBoxSizer98->Add(itemFlexGridSizer99, 0, wxGROW|wxALL, 5);
    wxArrayString m_SpeedStrings;
    m_SpeedStrings.Add(_("&min"));
    m_SpeedStrings.Add(_("&mid"));
    m_SpeedStrings.Add(_("cruise"));
    m_SpeedStrings.Add(_("&max"));
    m_SpeedStrings.Add(_("&%"));
    m_Speed = new wxRadioBox( m_PanelDetails, ID_RADIOBOX_BK_SPEED, _("enter"), wxDefaultPosition, wxDefaultSize, m_SpeedStrings, 1, wxRA_SPECIFY_ROWS );
    m_Speed->SetSelection(0);
    itemFlexGridSizer99->Add(m_Speed, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_SpeedPercent = new wxSpinCtrl( m_PanelDetails, ID_SPINCTRL1, _T("80"), wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 100, 80 );
    itemFlexGridSizer99->Add(m_SpeedPercent, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText102 = new wxStaticText( m_PanelDetails, wxID_STATIC, _("%"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer99->Add(itemStaticText102, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxRIGHT|wxTOP|wxBOTTOM, 5);

    wxArrayString m_ExitSpeedStrings;
    m_ExitSpeedStrings.Add(_("&min"));
    m_ExitSpeedStrings.Add(_("&mid"));
    m_ExitSpeedStrings.Add(_("&cruise"));
    m_ExitSpeedStrings.Add(_("&max"));
    m_ExitSpeed = new wxRadioBox( m_PanelDetails, wxID_ANY, _("exit"), wxDefaultPosition, wxDefaultSize, m_ExitSpeedStrings, 1, wxRA_SPECIFY_ROWS );
    m_ExitSpeed->SetSelection(0);
    itemFlexGridSizer99->Add(m_ExitSpeed, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_Notebook->AddPage(m_PanelDetails, _("Details"));

    m_RoutesPanel = new wxPanel( m_Notebook, ID_PANEL_BK_ROUTES, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer105 = new wxBoxSizer(wxVERTICAL);
    m_RoutesPanel->SetSizer(itemBoxSizer105);

    wxArrayString m_RoutesStrings;
    m_Routes = new wxListBox( m_RoutesPanel, ID_LISTBOX_BK_ROUTES, wxDefaultPosition, wxDefaultSize, m_RoutesStrings, wxLB_SINGLE|wxLB_ALWAYS_SB );
    itemBoxSizer105->Add(m_Routes, 1, wxGROW|wxALL, 5);

    m_LabelSensorsFromBlock = new wxStaticText( m_RoutesPanel, wxID_STATIC_SENSORS_TITLE, _("..."), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer105->Add(m_LabelSensorsFromBlock, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    wxFlexGridSizer* itemFlexGridSizer108 = new wxFlexGridSizer(2, 5, 0, 0);
    itemBoxSizer105->Add(itemFlexGridSizer108, 0, wxGROW|wxALL, 5);
    m_LabelSensorIDs = new wxStaticText( m_RoutesPanel, ID_STATICTEXT_BLOCK_SENSORID, _("Sensor ID:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer108->Add(m_LabelSensorIDs, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxTOP|wxADJUST_MINSIZE, 2);

    m_LabelSensorActions = new wxStaticText( m_RoutesPanel, ID_STATICTEXT_BLOCK_SENSOR_ACTION, _("Event:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer108->Add(m_LabelSensorActions, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxTOP|wxADJUST_MINSIZE, 2);

    m_LabelSensorEnd = new wxStaticText( m_RoutesPanel, wxID_STATIC_BLOCK_SENSOR_END, _("End:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer108->Add(m_LabelSensorEnd, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 2);

    m_LabelGhost = new wxStaticText( m_RoutesPanel, wxID_STATIC_BLOCK_GHOST, _("Ghost:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer108->Add(m_LabelGhost, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxADJUST_MINSIZE, 2);

    m_LabelProps = new wxStaticText( m_RoutesPanel, ID_STATICTEXT_BLOCK_SENSOR_FREE, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer108->Add(m_LabelProps, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxTOP|wxADJUST_MINSIZE, 5);

    wxArrayString m_SensorID1Strings;
    m_SensorID1 = new wxComboBox( m_RoutesPanel, ID_COMBOBOX_BLOCK_SENSOR_ID1, _T(""), wxDefaultPosition, wxSize(120, -1), m_SensorID1Strings, wxCB_READONLY );
    itemFlexGridSizer108->Add(m_SensorID1, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 2);

    wxArrayString m_SensorAction1Strings;
    m_SensorAction1 = new wxComboBox( m_RoutesPanel, ID_COMBOBOX_BLOCK_SENSOR_ACTION1, _T(""), wxDefaultPosition, wxSize(120, -1), m_SensorAction1Strings, wxCB_READONLY );
    itemFlexGridSizer108->Add(m_SensorAction1, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_SensorEnd1 = new wxCheckBox( m_RoutesPanel, ID_CHECKBOX_BLOCK_SENSOR_END1, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_SensorEnd1->SetValue(false);
    itemFlexGridSizer108->Add(m_SensorEnd1, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_Ghost1 = new wxCheckBox( m_RoutesPanel, ID_CHECKBOX_BLOCK_GHOST1, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_Ghost1->SetValue(false);
    itemFlexGridSizer108->Add(m_Ghost1, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_SensorProps1 = new wxButton( m_RoutesPanel, ID_BUTTON_BLOCKS_SENSOR_PROPS1, _("..."), wxDefaultPosition, wxSize(25, 25), 0 );
    itemFlexGridSizer108->Add(m_SensorProps1, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 2);

    wxArrayString m_SensorID2Strings;
    m_SensorID2 = new wxComboBox( m_RoutesPanel, ID_COMBOBOX_BLOCK_SENSOR_ID2, _T(""), wxDefaultPosition, wxSize(120, -1), m_SensorID2Strings, wxCB_READONLY );
    itemFlexGridSizer108->Add(m_SensorID2, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 2);

    wxArrayString m_SensorAction2Strings;
    m_SensorAction2 = new wxComboBox( m_RoutesPanel, ID_COMBOBOX_BLOCK_SENSOR_ACTION2, _T(""), wxDefaultPosition, wxSize(120, -1), m_SensorAction2Strings, wxCB_READONLY );
    itemFlexGridSizer108->Add(m_SensorAction2, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_SensorEnd2 = new wxCheckBox( m_RoutesPanel, ID_CHECKBOX_BLOCK_SENSOR_END2, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_SensorEnd2->SetValue(false);
    itemFlexGridSizer108->Add(m_SensorEnd2, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_Ghost2 = new wxCheckBox( m_RoutesPanel, ID_CHECKBOX_BLOCK_GHOST2, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_Ghost2->SetValue(false);
    itemFlexGridSizer108->Add(m_Ghost2, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_SensorProps2 = new wxButton( m_RoutesPanel, ID_BUTTON_BLOCK_SENSOR_PROPS2, _("..."), wxDefaultPosition, wxSize(25, 25), 0 );
    itemFlexGridSizer108->Add(m_SensorProps2, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 2);

    wxArrayString m_SensorID3Strings;
    m_SensorID3 = new wxComboBox( m_RoutesPanel, ID_COMBOBOX_BLOCK_SENSOR_ID3, _T(""), wxDefaultPosition, wxSize(120, -1), m_SensorID3Strings, wxCB_READONLY );
    itemFlexGridSizer108->Add(m_SensorID3, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 2);

    wxArrayString m_SensorAction3Strings;
    m_SensorAction3 = new wxComboBox( m_RoutesPanel, ID_COMBOBOX_BLOCK_SENSOR_ACTION3, _T(""), wxDefaultPosition, wxSize(120, -1), m_SensorAction3Strings, wxCB_READONLY );
    itemFlexGridSizer108->Add(m_SensorAction3, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_SensorEnd3 = new wxCheckBox( m_RoutesPanel, ID_CHECKBOX_BLOCK_SENSOR_END3, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_SensorEnd3->SetValue(false);
    itemFlexGridSizer108->Add(m_SensorEnd3, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_Ghost3 = new wxCheckBox( m_RoutesPanel, ID_CHECKBOX_BLOCK_GHOST3, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_Ghost3->SetValue(false);
    itemFlexGridSizer108->Add(m_Ghost3, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_SensorProps3 = new wxButton( m_RoutesPanel, ID_BUTTON_BLOCKS_SENSOR_PROPS3, _("..."), wxDefaultPosition, wxSize(25, 25), 0 );
    itemFlexGridSizer108->Add(m_SensorProps3, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 2);

    wxArrayString m_SensorID4Strings;
    m_SensorID4 = new wxComboBox( m_RoutesPanel, ID_COMBOBOX_BLOCK_SENSOR_ID4, _T(""), wxDefaultPosition, wxSize(120, -1), m_SensorID4Strings, wxCB_READONLY );
    itemFlexGridSizer108->Add(m_SensorID4, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 2);

    wxArrayString m_SensorAction4Strings;
    m_SensorAction4 = new wxComboBox( m_RoutesPanel, ID_COMBOBOX_BLOCK_SENSOR_ACTION4, _T(""), wxDefaultPosition, wxSize(120, -1), m_SensorAction4Strings, wxCB_READONLY );
    itemFlexGridSizer108->Add(m_SensorAction4, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_SensorEnd4 = new wxCheckBox( m_RoutesPanel, ID_CHECKBOX_BLOCK_SENSOR_END4, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_SensorEnd4->SetValue(false);
    itemFlexGridSizer108->Add(m_SensorEnd4, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_Ghost4 = new wxCheckBox( m_RoutesPanel, ID_CHECKBOX_BLOCK_GHOST4, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_Ghost4->SetValue(false);
    itemFlexGridSizer108->Add(m_Ghost4, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_SensorProps4 = new wxButton( m_RoutesPanel, ID_BUTTON_BLOCKS_SENSOR_PROPS4, _("..."), wxDefaultPosition, wxSize(25, 25), 0 );
    itemFlexGridSizer108->Add(m_SensorProps4, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 2);

    wxArrayString m_SensorID5Strings;
    m_SensorID5 = new wxComboBox( m_RoutesPanel, ID_COMBOBOX_BLOCK_SENSOR_ID5, _T(""), wxDefaultPosition, wxSize(120, -1), m_SensorID5Strings, wxCB_READONLY );
    itemFlexGridSizer108->Add(m_SensorID5, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 2);

    wxArrayString m_SensorAction5Strings;
    m_SensorAction5 = new wxComboBox( m_RoutesPanel, ID_COMBOBOX_BLOCK_SENSOR_ACTION5, _T(""), wxDefaultPosition, wxSize(120, -1), m_SensorAction5Strings, wxCB_READONLY );
    itemFlexGridSizer108->Add(m_SensorAction5, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_SensorEnd5 = new wxCheckBox( m_RoutesPanel, ID_CHECKBOX_BLOCK_SENSOR_END5, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_SensorEnd5->SetValue(false);
    itemFlexGridSizer108->Add(m_SensorEnd5, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_Ghost5 = new wxCheckBox( m_RoutesPanel, ID_CHECKBOX_BLOCK_GHOST5, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_Ghost5->SetValue(false);
    itemFlexGridSizer108->Add(m_Ghost5, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_SensorProps5 = new wxButton( m_RoutesPanel, ID_BUTTON_BLOCKS_SENSOR_PROPS5, _("..."), wxDefaultPosition, wxSize(25, 25), 0 );
    itemFlexGridSizer108->Add(m_SensorProps5, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 2);

    wxBoxSizer* itemBoxSizer139 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer105->Add(itemBoxSizer139, 0, wxGROW|wxALL, 5);
    m_labTimer = new wxStaticText( m_RoutesPanel, wxID_STATIC_BK_TIMER, _("Event timer"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer139->Add(m_labTimer, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    m_Timer = new wxTextCtrl( m_RoutesPanel, ID_TEXTCTRL_BK_TIMER, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    m_Timer->SetMaxLength(5);
    itemBoxSizer139->Add(m_Timer, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer142 = new wxFlexGridSizer(1, 3, 0, 0);
    itemFlexGridSizer142->AddGrowableCol(0);
    itemBoxSizer105->Add(itemFlexGridSizer142, 0, wxGROW|wxALL, 5);
    m_RouteProps = new wxButton( m_RoutesPanel, ID_BUTTON1, _("Properties..."), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer142->Add(m_RouteProps, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_RouteTest = new wxButton( m_RoutesPanel, ID_BUTTON2, _("Test"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer142->Add(m_RouteTest, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Notebook->AddPage(m_RoutesPanel, _("Routes"));

    m_Interface = new wxPanel( m_Notebook, ID_PANEL_BK_INTERFACE, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxFlexGridSizer* itemFlexGridSizer146 = new wxFlexGridSizer(2, 2, 0, 0);
    itemFlexGridSizer146->AddGrowableCol(1);
    m_Interface->SetSizer(itemFlexGridSizer146);

    m_Labeliid = new wxStaticText( m_Interface, wxID_STATIC_BK_IID, _("iid"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer146->Add(m_Labeliid, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    m_iid = new wxTextCtrl( m_Interface, ID_TEXTCTRL_BK_IID, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer146->Add(m_iid, 1, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_LabelAddress = new wxStaticText( m_Interface, wxID_STATIC_BK_ADDRESS, _("address"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer146->Add(m_LabelAddress, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    m_Address = new wxTextCtrl( m_Interface, ID_TEXTCTRL_BK_ADDRESS, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer146->Add(m_Address, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_LabelPort = new wxStaticText( m_Interface, wxID_STATIC_BK_PORT, _("port"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer146->Add(m_LabelPort, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    m_Port = new wxTextCtrl( m_Interface, ID_TEXTCTRL_BK_PORT, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer146->Add(m_Port, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labTD = new wxStaticText( m_Interface, wxID_STATIC_BK_TD, _("TD"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer146->Add(m_labTD, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    m_TD = new wxCheckBox( m_Interface, ID_CHECKBOX_BKTD, _("(TrackDriver)"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    m_TD->SetValue(false);
    itemFlexGridSizer146->Add(m_TD, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Notebook->AddPage(m_Interface, _("Interface"));

    m_PermissionsPanel = new wxPanel( m_Notebook, ID__PANEL_BK_PERMISSIONS, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer156 = new wxBoxSizer(wxVERTICAL);
    m_PermissionsPanel->SetSizer(itemBoxSizer156);

    m_labInclude = new wxStaticText( m_PermissionsPanel, wxID_ANY, _("Include"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer156->Add(m_labInclude, 0, wxALIGN_LEFT|wxALL|wxADJUST_MINSIZE, 5);

    wxArrayString m_IncludeListStrings;
    m_IncludeList = new wxListBox( m_PermissionsPanel, ID_LISTBOX_INCLUDE, wxDefaultPosition, wxDefaultSize, m_IncludeListStrings, wxLB_MULTIPLE );
    itemBoxSizer156->Add(m_IncludeList, 1, wxGROW|wxALL, 5);

    m_labExclude = new wxStaticText( m_PermissionsPanel, wxID_ANY, _("Exclude"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer156->Add(m_labExclude, 0, wxALIGN_LEFT|wxALL|wxADJUST_MINSIZE, 5);

    wxArrayString m_ExcludeListStrings;
    m_ExcludeList = new wxListBox( m_PermissionsPanel, ID_LISTBOX_EXCLUDE, wxDefaultPosition, wxDefaultSize, m_ExcludeListStrings, wxLB_MULTIPLE );
    itemBoxSizer156->Add(m_ExcludeList, 1, wxGROW|wxALL, 5);

    m_Notebook->AddPage(m_PermissionsPanel, _("Persmissions"));

    itemBoxSizer2->Add(m_Notebook, 0, wxGROW|wxALL, 5);

    wxStdDialogButtonSizer* itemStdDialogButtonSizer161 = new wxStdDialogButtonSizer;

    itemBoxSizer2->Add(itemStdDialogButtonSizer161, 0, wxALIGN_RIGHT|wxALL, 5);
    m_Cancel = new wxButton( itemDialog1, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer161->AddButton(m_Cancel);

    m_Apply = new wxButton( itemDialog1, wxID_APPLY, _("&Apply"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer161->AddButton(m_Apply);

    m_OK = new wxButton( itemDialog1, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    m_OK->SetDefault();
    itemStdDialogButtonSizer161->AddButton(m_OK);

    itemStdDialogButtonSizer161->Realize();

////@end BlockDialog content construction
}

/*!
 * wxEVT_COMMAND_LISTBOX_SELECTED event handler for ID_LISTBOX_BLOCKS
 */

void BlockDialog::OnListboxBlocksSelected( wxCommandEvent& event )
{
  iONode model = wxGetApp().getModel();
  if( model != NULL ) {
    iONode bklist = wPlan.getbklist( model );
    if( bklist != NULL ) {
      int cnt = NodeOp.getChildCnt( bklist );
      for( int i = 0; i < cnt; i++ ) {
        iONode bk = NodeOp.getChild( bklist, i );
        const char* id = wBlock.getid( bk );
        if( id != NULL && StrOp.equals( id, m_List->GetStringSelection().mb_str(wxConvUTF8) ) ) {
          m_Props = bk;
          initValues();
          break;
        }
      }
    }
  }
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_BLOCK_NEW
 */

void BlockDialog::OnButtonBlockNewClick( wxCommandEvent& event )
{
  int i = m_List->FindString( _T("NEW") );
  if( i == wxNOT_FOUND ) {
    m_List->Append( _T("NEW") );
    iONode model = wxGetApp().getModel();
    if( model != NULL ) {
      iONode bklist = wPlan.getbklist( model );
      if( bklist == NULL ) {
        bklist = NodeOp.inst( wBlockList.name(), model, ELEMENT_NODE );
        NodeOp.addChild( model, bklist );
      }
      if( bklist != NULL ) {
        iONode bk = NodeOp.inst( wBlock.name(), bklist, ELEMENT_NODE );
        NodeOp.addChild( bklist, bk );
        wBlock.setid( bk, "NEW" );
        m_Props = bk;
        initValues();
      }
    }
  }
  m_List->SetStringSelection( _T("NEW") );
  m_List->SetFirstItem( _T("NEW") );
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_BLOCK_DELETE
 */

void BlockDialog::OnButtonBlockDeleteClick( wxCommandEvent& event )
{
  if( m_Props != NULL ) {
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
      iONode bklist = wPlan.getbklist( model );
      if( bklist != NULL ) {
        NodeOp.removeChild( bklist, m_Props );
        m_Props = NULL;
      }
    }

    initIndex();
  }
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
 */

void BlockDialog::OnCancelClick( wxCommandEvent& event )
{
  EndModal( 0 );
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_APPLY
 */

void BlockDialog::OnApplyClick( wxCommandEvent& event )
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
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
 */

void BlockDialog::OnOkClick( wxCommandEvent& event )
{
  if( m_bSave )
    OnApplyClick(event);
  EndModal( wxID_OK );
}

/*!
 * Should we show tooltips?
 */

bool BlockDialog::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap BlockDialog::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin BlockDialog bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end BlockDialog bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon BlockDialog::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin BlockDialog icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end BlockDialog icon retrieval
}
/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_BK_LOCPROPS
 */

void BlockDialog::OnButtonBkLocpropsClick( wxCommandEvent& event )
{
  const char* locid = wBlock.getlocid( m_Props );
  iONode model = wxGetApp().getModel();

  if( model != NULL ) {
    iONode lclist = wPlan.getlclist( model );
    if( lclist != NULL ) {
      int cnt = NodeOp.getChildCnt( lclist );
      for( int i = 0; i < cnt; i++ ) {
        iONode lc = NodeOp.getChild( lclist, i );
        const char* id = wLoc.getid( lc );

        if( id != NULL && StrOp.equals( locid, id ) ) {
          LocDialog*  propDlg = new LocDialog(this, lc );
          if( wxID_OK == propDlg->ShowModal() ) {
            /* Notify RocRail. */
            iONode cmd = NodeOp.inst( wModelCmd.name(), NULL, ELEMENT_NODE );
            wModelCmd.setcmd( cmd, wModelCmd.modify );
            NodeOp.addChild( cmd, (iONode)lc->base.clone( lc ) );
            wxGetApp().sendToRocrail( cmd );
            cmd->base.del(cmd);
          }
          break;
        }
      }
    }
  }
}


void BlockDialog::fbProps( const char* fbid ) {
  iONode model = wxGetApp().getModel();

  if( model != NULL ) {
    iONode fblist = wPlan.getfblist( model );
    if( fblist != NULL ) {
      int cnt = NodeOp.getChildCnt( fblist );
      for( int i = 0; i < cnt; i++ ) {
        iONode fb = NodeOp.getChild( fblist, i );
        const char* id = wFeedback.getid( fb );

        if( id != NULL && StrOp.equals( fbid, id ) ) {
          FeedbackDialog* propDlg = new FeedbackDialog(this, fb );
          if( wxID_OK == propDlg->ShowModal() ) {
            /* Notify RocRail. */
            iONode cmd = NodeOp.inst( wModelCmd.name(), NULL, ELEMENT_NODE );
            wModelCmd.setcmd( cmd, wModelCmd.modify );
            NodeOp.addChild( cmd, (iONode)NodeOp.base.clone( fb ) );
            wxGetApp().sendToRocrail( cmd );
            cmd->base.del(cmd);
          }
          break;
        }
      }
    }
  }
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_BK_SIGNAL
 */

void BlockDialog::OnButtonBkSignalRClick( wxCommandEvent& event )
{
  const char* swid = wBlock.getsignal( m_Props );
  if( swid == NULL || StrOp.len( swid ) == 0 )
    return;

  iONode model = wxGetApp().getModel();

  if( model != NULL ) {
    iONode swlist = wPlan.getswlist( model );
    if( swlist != NULL ) {
      int cnt = NodeOp.getChildCnt( swlist );
      for( int i = 0; i < cnt; i++ ) {
        iONode sw = NodeOp.getChild( swlist, i );
        const char* id = wSwitch.getid( sw );

        if( id != NULL && StrOp.equals( swid, id ) ) {
          SwitchDialog* propDlg = new SwitchDialog(this, sw );
          if( wxID_OK == propDlg->ShowModal() ) {
            /* Notify RocRail. */
            iONode cmd = NodeOp.inst( wModelCmd.name(), NULL, ELEMENT_NODE );
            wModelCmd.setcmd( cmd, wModelCmd.modify );
            NodeOp.addChild( cmd, (iONode)NodeOp.base.clone( sw ) );
            wxGetApp().sendToRocrail( cmd );
            cmd->base.del(cmd);
          }
          break;
        }
      }
    }
  }
}


/*!
 * wxEVT_COMMAND_LISTBOX_SELECTED event handler for ID_LISTBOX_COMMANDS
 */

void BlockDialog::OnListboxBkRoutesSelected( wxCommandEvent& event )
{
  initSensors();
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_ST_ADD
 */

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_BK_ROUTE_PROPS
 */

void BlockDialog::OnButtonBkRoutePropsClick( wxCommandEvent& event )
{
  int idx = m_Routes->GetSelection();
  if( idx != wxNOT_FOUND ) {
    iONode st = (iONode)m_Routes->GetClientData(idx);
    if( st != NULL ) {
      RouteDialog* propDlg = new RouteDialog(this, st );
      if( wxID_OK == propDlg->ShowModal() ) {
        /* Notify RocRail. */
        iONode cmd = NodeOp.inst( wModelCmd.name(), NULL, ELEMENT_NODE );
        wModelCmd.setcmd( cmd, wModelCmd.modify );
        NodeOp.addChild( cmd, (iONode)NodeOp.base.clone( st ) );
        wxGetApp().sendToRocrail( cmd );
        cmd->base.del(cmd);
      }
    }
  }
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_BK_ROUTE_TEST
 */

void BlockDialog::OnButtonBkRouteTestClick( wxCommandEvent& event )
{
  int idx = m_Routes->GetSelection();
  if( idx != wxNOT_FOUND ) {
    iONode st = (iONode)m_Routes->GetClientData(idx);
    if( st != NULL ) {
      iONode cmd = NodeOp.inst( wRoute.name(), NULL, ELEMENT_NODE );
      wRoute.setcmd( cmd, wRoute.test );
      wRoute.setid( cmd, wRoute.getid( st ) );
      wxGetApp().sendToRocrail( cmd );
      cmd->base.del(cmd);
    }
  }
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_BK_WSIGNAL
 */

void BlockDialog::OnButtonBkWsignalRClick( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_BK_WSIGNAL in BlockDialog.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_BK_WSIGNAL in BlockDialog.
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_BLOCK_FEEDBACKS
 */


void BlockDialog::initSensorCombos() {
  wxComboBox* ids[5] = {m_SensorID1,m_SensorID2,m_SensorID3,m_SensorID4,m_SensorID5};
  wxComboBox* acts[5] = {m_SensorAction1,m_SensorAction2,m_SensorAction3,m_SensorAction4,m_SensorAction5};
  wxCheckBox* end[5] = {m_SensorEnd1,m_SensorEnd2,m_SensorEnd3,m_SensorEnd4,m_SensorEnd5};
  wxCheckBox* ghost[5] = {m_Ghost1,m_Ghost2,m_Ghost3,m_Ghost4,m_Ghost5};

  for( int i = 0; i < 5; i++ ) {
    ids[i]->Clear();
    acts[i]->Clear();
    ids[i]->Append( wxString("-",wxConvUTF8) );
    acts[i]->Append( wxString("-",wxConvUTF8) );

    end[i]->SetValue( false );
    ghost[i]->SetValue( false );

    acts[i]->Append( wxString(wFeedbackEvent.enter_event,wxConvUTF8) );
    acts[i]->Append( wxString(wFeedbackEvent.enter2route_event,wxConvUTF8) );
    acts[i]->Append( wxString(wFeedbackEvent.enter2in_event,wxConvUTF8) );
    acts[i]->Append( wxString(wFeedbackEvent.in_event,wxConvUTF8) );
    acts[i]->Append( wxString(wFeedbackEvent.exit_event,wxConvUTF8) );
    acts[i]->Append( wxString(wFeedbackEvent.pre2in_event,wxConvUTF8) );
    acts[i]->Append( wxString(wFeedbackEvent.occupied_event,wxConvUTF8) );
    acts[i]->Append( wxString(wFeedbackEvent.ident_event,wxConvUTF8) );
    acts[i]->Append( wxString(wFeedbackEvent.shortin_event,wxConvUTF8) );
  }

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
        m_SensorID1->Append( wxString(id,wxConvUTF8) );
        m_SensorID2->Append( wxString(id,wxConvUTF8) );
        m_SensorID3->Append( wxString(id,wxConvUTF8) );
        m_SensorID4->Append( wxString(id,wxConvUTF8) );
        m_SensorID5->Append( wxString(id,wxConvUTF8) );
      }
    }
  }
  /* clean up the temp. list */
  ListOp.base.del(list);
}

void BlockDialog::initSensors() {
  TraceOp.trc( "blockdlg", TRCLEVEL_INFO, __LINE__, 9999, "initSensors" );

  wxComboBox* ids[5] = {m_SensorID1,m_SensorID2,m_SensorID3,m_SensorID4,m_SensorID5};
  wxComboBox* acts[5] = {m_SensorAction1,m_SensorAction2,m_SensorAction3,m_SensorAction4,m_SensorAction5};
  wxCheckBox* end[5] = {m_SensorEnd1,m_SensorEnd2,m_SensorEnd3,m_SensorEnd4,m_SensorEnd5};
  wxCheckBox* ghost[5] = {m_Ghost1,m_Ghost2,m_Ghost3,m_Ghost4,m_Ghost5};

  for( int i = 0; i < 5; i++ ) {
    ids[i]->SetStringSelection( wxString("-",wxConvUTF8) );
    acts[i]->SetStringSelection( wxString("-",wxConvUTF8) );
    end[i]->SetValue( false );
    m_fbEvents[i] = NULL;
  }

  iONode st = NULL;
  const char* routeID = NULL;
  int idx = m_Routes->GetSelection();
  if( idx != wxNOT_FOUND ) {
    st = (iONode)m_Routes->GetClientData(idx);

    if( StrOp.equals( wFeedbackEvent.from_all, m_Routes->GetStringSelection().mb_str(wxConvUTF8) ) ) {
      routeID = wFeedbackEvent.from_all;
    }
    else if( StrOp.equals( wFeedbackEvent.from_all_reverse, m_Routes->GetStringSelection().mb_str(wxConvUTF8) ) ) {
      routeID = wFeedbackEvent.from_all_reverse;
    }
    else if( st == NULL ) {
      TraceOp.trc( "blockdlg", TRCLEVEL_WARNING, __LINE__, 9999,
          "route [%s] has no properties...", (const char*)m_Routes->GetStringSelection().mb_str(wxConvUTF8) );
      return;
    }
    else {
      routeID = wRoute.getid(st);
    }
  }

  TraceOp.trc( "blockdlg", TRCLEVEL_INFO, __LINE__, 9999, "route [%s]", routeID );

  if( st == NULL ) {
    /* all */
    m_FromBlockID = routeID;
  }
  else if( StrOp.equals( wBlock.getid( m_Props ), wRoute.getbkb( st ) ) ) {
    m_FromBlockID = wRoute.getbka( st );
  }
  else if( !wRoute.isdir(st) ) {
    m_FromBlockID = wRoute.getbkb( st );
  }
  else {
    m_LabelSensorsFromBlock->SetLabel( _T("") );
    return;
  }

  TraceOp.trc( "blockdlg", TRCLEVEL_INFO, __LINE__, 9999, "FromBlockID [%s]", m_FromBlockID );



  char* s = StrOp.fmt( wxGetApp().getMsg("sensorfromblock").mb_str(wxConvUTF8), m_FromBlockID );
  m_LabelSensorsFromBlock->SetLabel( wxString( s ,wxConvUTF8) );
  StrOp.free(s);

  iONode fb = wBlock.getfbevent( m_Props );
  idx = 0;
  while( fb != NULL && idx < 5 ) {
    TraceOp.trc( "blockdlg", TRCLEVEL_INFO, __LINE__, 9999, "fbIndex %d", idx );
    if( StrOp.equals( m_FromBlockID, wFeedbackEvent.getfrom( fb ) ) ) {
      ids[idx]->SetStringSelection( wxString(wFeedbackEvent.getid( fb ),wxConvUTF8) );
      acts[idx]->SetStringSelection( wxString(wFeedbackEvent.getaction( fb ),wxConvUTF8) );
      end[idx]->SetValue( wFeedbackEvent.isendpuls( fb ) );
      ghost[idx]->SetValue( wFeedbackEvent.isghostdetection( fb ) );

      m_fbEvents[idx] = fb;
      idx++;
    }
    fb = wBlock.nextfbevent( m_Props, fb );
  };

}


void BlockDialog::evaluateSensors() {
  TraceOp.trc( "blockdlg", TRCLEVEL_INFO, __LINE__, 9999, "evaluateSensors" );
  if( m_Props == NULL || m_Routes->GetSelection()== wxNOT_FOUND )
    return;

  wxComboBox* ids[5] = {m_SensorID1,m_SensorID2,m_SensorID3,m_SensorID4,m_SensorID5};
  wxComboBox* acts[5] = {m_SensorAction1,m_SensorAction2,m_SensorAction3,m_SensorAction4,m_SensorAction5};
  wxCheckBox* end[5] = {m_SensorEnd1,m_SensorEnd2,m_SensorEnd3,m_SensorEnd4,m_SensorEnd5};
  wxCheckBox* ghost[5] = {m_Ghost1,m_Ghost2,m_Ghost3,m_Ghost4,m_Ghost5};

  for( int i = 0; i < 5; i++ ) {
    if( StrOp.equals( "-", ids[i]->GetValue().mb_str(wxConvUTF8) ) &&
        !StrOp.equals( wFeedbackEvent.enter2route_event, acts[i]->GetValue().mb_str(wxConvUTF8) ) &&
        m_fbEvents[i] != NULL )
      {
      NodeOp.removeChild( m_Props, m_fbEvents[i] );
      NodeOp.base.del( m_fbEvents[i] );
      m_fbEvents[i] = NULL;
    }
    else if( !StrOp.equals( "-", ids[i]->GetValue().mb_str(wxConvUTF8) ) ||
             StrOp.equals( wFeedbackEvent.enter2route_event, acts[i]->GetValue().mb_str(wxConvUTF8) ) )
      {
      if( m_fbEvents[i] == NULL ) {
        TraceOp.trc( "blockdlg", TRCLEVEL_INFO, __LINE__, 9999, "m_fbEvents[%d] == NULL", i );
        m_fbEvents[i] = NodeOp.inst( wFeedbackEvent.name(), m_Props, ELEMENT_NODE );
        NodeOp.addChild( m_Props, m_fbEvents[i] );
      }
      if( StrOp.equals( "-", ids[i]->GetValue().mb_str(wxConvUTF8) ) )
        wFeedbackEvent.setid( m_fbEvents[i], "" );
      else
        wFeedbackEvent.setid( m_fbEvents[i], ids[i]->GetValue().mb_str(wxConvUTF8) );

      wFeedbackEvent.setaction( m_fbEvents[i], acts[i]->GetValue().mb_str(wxConvUTF8) );
      wFeedbackEvent.setfrom( m_fbEvents[i], m_FromBlockID );
      wFeedbackEvent.setendpuls( m_fbEvents[i], end[i]->GetValue()?True:False );
      wFeedbackEvent.setghostdetection( m_fbEvents[i], ghost[i]->GetValue()?True:False );
    }

  }
}

void BlockDialog::OnBlockSensor( wxCommandEvent& event )
{
  evaluateSensors();
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_BLOCKS_SENSOR_PROPS1
 */

void BlockDialog::OnSensorProps( wxCommandEvent& event )
{
  TraceOp.trc( "blockdlg", TRCLEVEL_INFO, __LINE__, 9999, "OnSensorProps" );

  if( event.GetEventObject() == m_SensorProps1 ) {
    char* id = StrOp.dup( m_SensorID1->GetValue().mb_str(wxConvUTF8) );
    TraceOp.trc( "blockdlg", TRCLEVEL_INFO, __LINE__, 9999, "OnSensorProps id=%s", id );
    if( !StrOp.equals( "-", id ) )
      fbProps(id);
    StrOp.free( id );
  }
  else if( event.GetEventObject() == m_SensorProps2 ) {
    char* id = StrOp.dup( m_SensorID2->GetValue().mb_str(wxConvUTF8) );
    if( !StrOp.equals( "-", id ) )
      fbProps(id);
    StrOp.free( id );
  }
  else if( event.GetEventObject() == m_SensorProps3 ) {
    char* id = StrOp.dup( m_SensorID3->GetValue().mb_str(wxConvUTF8) );
    if( !StrOp.equals( "-", id ) )
      fbProps(id);
    StrOp.free( id );
  }
  else if( event.GetEventObject() == m_SensorProps4 ) {
    char* id = StrOp.dup( m_SensorID4->GetValue().mb_str(wxConvUTF8) );
    if( !StrOp.equals( "-", id ) )
      fbProps(id);
    StrOp.free( id );
  }
  else if( event.GetEventObject() == m_SensorProps5 ) {
    char* id = StrOp.dup( m_SensorID5->GetValue().mb_str(wxConvUTF8) );
    if( !StrOp.equals( "-", id ) )
      fbProps(id);
    StrOp.free( id );
  }
}


/*!
 * wxEVT_COMMAND_LISTBOX_SELECTED event handler for ID_LISTBOX_BLOCK_TURNOUTS
 */

void BlockDialog::OnListboxBlockTurnoutsSelected( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_LISTBOX_SELECTED event handler for ID_LISTBOX_BLOCK_TURNOUTS in BlockDialog.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_COMMAND_LISTBOX_SELECTED event handler for ID_LISTBOX_BLOCK_TURNOUTS in BlockDialog.
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_BLOCK_ADD_TURNOUT
 */

void BlockDialog::OnButtonBlockAddTurnoutClick( wxCommandEvent& event )
{
  m_TurnoutList->Append( m_ChoiceTurnouts->GetStringSelection() );
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_BLOCK_REMOVE_TURNOUT
 */

void BlockDialog::OnButtonBlockRemoveTurnoutClick( wxCommandEvent& event )
{
  int i = m_TurnoutList->GetSelection();
  if( i != wxNOT_FOUND )
    m_TurnoutList->Delete(i);
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_BK_SIGNAL
 */

void BlockDialog::OnButtonBkSignalClick( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_BK_SIGNAL in BlockDialog.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_BK_SIGNAL in BlockDialog.
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_BK_WSIGNAL
 */

void BlockDialog::OnButtonBkWsignalClick( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_BK_WSIGNAL in BlockDialog.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_BK_WSIGNAL in BlockDialog.
}


/*!
 * wxEVT_COMMAND_CHECKBOX_CLICKED event handler for wxID_BLOCK_WAIT
 */

void BlockDialog::OnBlockWaitClick( wxCommandEvent& event )
{
  int wait = 0;
  if( !m_WaitFlag->GetValue() )
    wait = 3;
  else {
    if( StrOp.equals( wBlock.wait_random, wBlock.getwaitmode( m_Props ) ) )
      wait = 0;
    else if( StrOp.equals( wBlock.wait_fixed, wBlock.getwaitmode( m_Props ) ) )
      wait = 1;
    else if( StrOp.equals( wBlock.wait_loc, wBlock.getwaitmode( m_Props ) ) )
      wait = 2;
  }
  m_Wait->SetSelection( wait );
}


/*!
 * wxEVT_COMMAND_RADIOBOX_SELECTED event handler for ID_RADIOBOX_BK_WAIT
 */

void BlockDialog::OnRadioboxBkWaitSelected( wxCommandEvent& event )
{
  m_WaitFlag->SetValue(m_Wait->GetSelection() != 3);
}

