/*
 Rocrail - Model Railroad Control System

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

/** ------------------------------------------------------------
  * Module:
  * Object:
  * ------------------------------------------------------------
  * $Source: /cvsroot/rojav/rocgui/dialogs/rocgui-dialogs.pjd,v $
  * $Author: robvrs $
  * $Date: 2006/02/22 14:10:57 $
  * $Revision: 1.63 $
  * $Name:  $
  */
#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "seltabdlg.h"
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

#include "seltabdlg.h"
#include "seltabtrackdlg.h"

#include "rocs/public/node.h"

#include "rocview/public/guiapp.h"

#include "rocrail/wrapper/public/Item.h"
#include "rocrail/wrapper/public/Feedback.h"
#include "rocrail/wrapper/public/Plan.h"
#include "rocrail/wrapper/public/SelTab.h"
#include "rocrail/wrapper/public/SelTabPos.h"
#include "rocrail/wrapper/public/ModelCmd.h"
#include "rocrail/wrapper/public/FeedbackEvent.h"
#include "rocrail/wrapper/public/Route.h"

////@begin XPM images
////@end XPM images


/*!
 * SelTabDialog type definition
 */

IMPLEMENT_DYNAMIC_CLASS( SelTabDialog, wxDialog )


/*!
 * SelTabDialog event table definition
 */

BEGIN_EVENT_TABLE( SelTabDialog, wxDialog )

////@begin SelTabDialog event table entries
    EVT_CHECKBOX( ID_FY_SHAREDFB, SelTabDialog::OnFySharedfbClick )

    EVT_COMBOBOX( ID_SELTAB_TYPE, SelTabDialog::OnSeltabTypeSelected )

    EVT_GRID_CELL_LEFT_CLICK( SelTabDialog::OnCellLeftClick )
    EVT_GRID_CELL_RIGHT_CLICK( SelTabDialog::OnCellRightClick )

    EVT_BUTTON( ID_BUTTON_SELTAB_ADDTRACK, SelTabDialog::OnButtonSeltabAddtrackClick )

    EVT_BUTTON( ID_BUTTON_SELTAB_DELTRACK, SelTabDialog::OnButtonSeltabDeltrackClick )

    EVT_BUTTON( ID_BUTTON_SELTAB_MODIFYTRACK, SelTabDialog::OnButtonSeltabModifytrackClick )

    EVT_LISTBOX( ID_LISTBOX_SELTAB_ROUTES, SelTabDialog::OnListboxSeltabRoutesSelected )

    EVT_COMBOBOX( wxID_SELTAB_ROUTESENSOR_1, SelTabDialog::OnBlockSensor )

    EVT_COMBOBOX( wxID_SELTAB_ROUTEEVENT_1, SelTabDialog::OnBlockSensor )

    EVT_COMBOBOX( wxID_SELTAB_ROUTESENSOR_2, SelTabDialog::OnBlockSensor )

    EVT_COMBOBOX( wxID_SELTAB_ROUTEEVENT_2, SelTabDialog::OnBlockSensor )

    EVT_COMBOBOX( wxID_SELTAB_ROUTESENSOR_3, SelTabDialog::OnBlockSensor )

    EVT_COMBOBOX( wxID_SELTAB_ROUTEEVENT_3, SelTabDialog::OnBlockSensor )

    EVT_COMBOBOX( wxID_SELTAB_ROUTESENSOR_4, SelTabDialog::OnBlockSensor )

    EVT_COMBOBOX( wxID_SELTAB_ROUTEEVENT_4, SelTabDialog::OnBlockSensor )

    EVT_BUTTON( wxID_OK, SelTabDialog::OnOkClick )

    EVT_BUTTON( wxID_CANCEL, SelTabDialog::OnCancelClick )

////@end SelTabDialog event table entries

END_EVENT_TABLE()


/*!
 * SelTabDialog constructors
 */

SelTabDialog::SelTabDialog()
{
    Init();
}

SelTabDialog::SelTabDialog( wxWindow* parent, iONode p_Props )
{
  m_Props = p_Props;
  m_TabAlign = wxGetApp().getTabAlign();
  Init();
  Create(parent, -1, wxGetApp().getMsg("seltab") );

  wxFont* font = new wxFont( m_TracksGrid->GetDefaultCellFont() );
  font->SetPointSize( (int)(font->GetPointSize() - 1 ) );
  m_TracksGrid->SetDefaultCellFont( *font );
  m_TracksGrid->SetColLabelValue(0, wxGetApp().getMsg("tracknr") );
  m_TracksGrid->SetColLabelValue(1, wxGetApp().getMsg("blockid") );
  m_TracksGrid->SetColLabelValue(2, wxGetApp().getMsg("description") );
  m_TracksGrid->AutoSizeColumns();
  m_TracksGrid->AutoSizeRows();

  initLabels();
  if( m_Props != NULL ) {
    initValues();
  }

  m_GeneralPanel->GetSizer()->Layout();
  m_LocationPanel->GetSizer()->Layout();
  m_Interface->GetSizer()->Layout();
  m_TracksPanel->GetSizer()->Layout();
  m_Notebook->Fit();
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

void SelTabDialog::initLabels() {
  m_Notebook->SetPageText( 0, wxGetApp().getMsg( "general" ) );
  m_Notebook->SetPageText( 1, wxGetApp().getMsg( "position" ) );
  m_Notebook->SetPageText( 2, wxGetApp().getMsg( "interface" ) );
  m_Notebook->SetPageText( 3, wxGetApp().getMsg( "tracks" ) );
  m_Notebook->SetPageText( 4, wxGetApp().getMsg( "routes" ) );

  // General
  m_LabelID->SetLabel( wxGetApp().getMsg( "id" ) );
  m_LabelDesc->SetLabel( wxGetApp().getMsg( "description" ) );
  m_labNrTracks->SetLabel( wxGetApp().getMsg( "tracks" ) );
  m_SharedFB->SetLabel( wxGetApp().getMsg( "sharedfb" ) );
  m_Manager->SetLabel( wxGetApp().getMsg( "managetrackblocks" ) );
  m_labMinOcc->SetLabel( wxGetApp().getMsg( "minocc" ) );
  m_labMoveDelay->SetLabel( wxGetApp().getMsg( "delay" ) );
  m_labOffPos->SetLabel( wxGetApp().getMsg( "offpos" ) );

  // Location
  m_LabelX->SetLabel( wxGetApp().getMsg( "x" ) );
  m_LabelY->SetLabel( wxGetApp().getMsg( "y" ) );
  m_LabelZ->SetLabel( wxGetApp().getMsg( "z" ) );
  m_ori->SetLabel( wxGetApp().getMsg( "orientation" ) );
  m_ori->SetString( 0, wxGetApp().getMsg( "north" ) );
  m_ori->SetString( 1, wxGetApp().getMsg( "east" ) );
  m_ori->SetString( 2, wxGetApp().getMsg( "south" ) );
  m_ori->SetString( 3, wxGetApp().getMsg( "west" ) );

  // Interface
  m_Labeliid->SetLabel( wxGetApp().getMsg( "iid" ) );
  m_LabelAddress0->SetLabel( wxGetApp().getMsg( "address" ) + _T(" 0") );
  m_LabelAddress1->SetLabel( wxGetApp().getMsg( "address" ) + _T(" 1") );
  m_LabelAddress2->SetLabel( wxGetApp().getMsg( "address" ) + _T(" 2") );
  m_LabelAddress3->SetLabel( wxGetApp().getMsg( "address" ) + _T(" 3") );
  m_LabelAddress4->SetLabel( wxGetApp().getMsg( "address" ) );
  m_LabelAddress5->SetLabel( wxGetApp().getMsg( "address" ) + _T(" 4") );
  m_LabelAddress6->SetLabel( wxGetApp().getMsg( "address" ) + _T(" 5") );
  m_LabelAddress7->SetLabel( wxGetApp().getMsg( "address" ) + _T(" 6") );
  m_labPort0->SetLabel( wxGetApp().getMsg( "port" ) );
  m_labPort1->SetLabel( wxGetApp().getMsg( "port" ) );
  m_labPort2->SetLabel( wxGetApp().getMsg( "port" ) );
  m_labPort3->SetLabel( wxGetApp().getMsg( "port" ) );
  m_labPort4->SetLabel( wxGetApp().getMsg( "port" ) );
  m_labPort5->SetLabel( wxGetApp().getMsg( "port" ) );
  m_labPort6->SetLabel( wxGetApp().getMsg( "port" ) );
  m_labPort7->SetLabel( wxGetApp().getMsg( "port" ) );
  m_LabelType->SetLabel( wxGetApp().getMsg( "type" ) );
  m_Type->Append( wxString(wSelTab.prot_DEF,wxConvUTF8 ) );
  m_Type->Append( wxString(wSelTab.prot_N,wxConvUTF8 ) );
  m_Type->Append( wxString(wSelTab.prot_M,wxConvUTF8 ) );
  m_Type->Append( wxString(wSelTab.prot_MP,wxConvUTF8 ) );
  m_Invert->SetLabel( wxGetApp().getMsg( "invert" ) );
  m_InvertNew->SetLabel( wxGetApp().getMsg( "invert" ) );
  m_SingleGate->SetLabel( wxGetApp().getMsg( "singlegate" ) );

  // Buttons
  m_DelTrack->SetLabel( wxGetApp().getMsg( "delete" ) );
  m_ModifyTrack->SetLabel( wxGetApp().getMsg( "edit" ) );
  m_AddTrack->SetLabel( wxGetApp().getMsg( "add" ) );

  m_OK->SetLabel( wxGetApp().getMsg( "ok" ) );
  m_Cancel->SetLabel( wxGetApp().getMsg( "cancel" ) );


  m_PosSen->Clear();
  m_PosSen->Append( wxString("-",wxConvUTF8) );
  m_b0Sen->Append( wxString("-",wxConvUTF8) );
  m_b1Sen->Append( wxString("-",wxConvUTF8) );
  m_b2Sen->Append( wxString("-",wxConvUTF8) );
  m_b3Sen->Append( wxString("-",wxConvUTF8) );
  m_b4Sen->Append( wxString("-",wxConvUTF8) );
  m_b5Sen->Append( wxString("-",wxConvUTF8) );
  m_b6Sen->Append( wxString("-",wxConvUTF8) );

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
        m_PosSen->Append( wxString(id,wxConvUTF8) );
        m_b0Sen->Append( wxString(id,wxConvUTF8) );
        m_b1Sen->Append( wxString(id,wxConvUTF8) );
        m_b2Sen->Append( wxString(id,wxConvUTF8) );
        m_b3Sen->Append( wxString(id,wxConvUTF8) );
        m_b4Sen->Append( wxString(id,wxConvUTF8) );
        m_b5Sen->Append( wxString(id,wxConvUTF8) );
        m_b6Sen->Append( wxString(id,wxConvUTF8) );
      }
    }
  }
  /* clean up the temp. list */
  ListOp.base.del(list);

  // Route Sensors
  m_LabelSensorIDs->SetLabel( wxGetApp().getMsg( "id" ) );
  m_LabelSensorActions->SetLabel( wxGetApp().getMsg( "event" ) );
  initSensorCombos();

}


void SelTabDialog::initValues() {
  // General
  m_ID->SetValue( wxString( wSelTab.getid( m_Props ),wxConvUTF8 ) );
  m_Desc->SetValue( wxString(wSelTab.getdesc( m_Props ),wxConvUTF8 ) );
  m_NrTracks->SetValue(wSelTab.getnrtracks( m_Props ));
  m_SharedFB->SetValue(wSelTab.issharedfb( m_Props ));
  m_Manager->SetValue(wSelTab.ismanager( m_Props ));
  m_MinOcc->SetValue(wSelTab.getminocc( m_Props ));
  m_MoveDelay->SetValue(wSelTab.getmovedelay( m_Props ));
  m_FiFo->SetValue(wSelTab.isfifo( m_Props ));
  m_OffPos->SetValue(wSelTab.getoffpos( m_Props ));

  m_RoutesPanel->Enable( m_SharedFB->IsChecked() );

  // Location
  char* str = StrOp.fmt( "%d", wSelTab.getx( m_Props ) );
  m_x->SetValue( wxString(str,wxConvUTF8 ) ); StrOp.free( str );
  str = StrOp.fmt( "%d", wSelTab.gety( m_Props ) );
  m_y->SetValue( wxString(str,wxConvUTF8 ) ); StrOp.free( str );
  str = StrOp.fmt( "%d", wSelTab.getz( m_Props ) );
  m_z->SetValue( wxString(str,wxConvUTF8 ) ); StrOp.free( str );
  if( StrOp.equals( wItem.north, wItem.getori( m_Props ) ) )
    m_ori->SetSelection( 0 );
  else if( StrOp.equals( wItem.east, wItem.getori( m_Props ) ) )
    m_ori->SetSelection( 1 );
  else if( StrOp.equals( wItem.south, wItem.getori( m_Props ) ) )
    m_ori->SetSelection( 2 );
  else
    m_ori->SetSelection( 3 );

  // Interface
  m_IID->SetValue( wxString(wSelTab.getiid( m_Props ),wxConvUTF8 ) );

  str = StrOp.fmt( "%d", wSelTab.getaddr0( m_Props ) );
  m_Address0->SetValue( wxString(str,wxConvUTF8 ) ); StrOp.free( str );
  str = StrOp.fmt( "%d", wSelTab.getaddr1( m_Props ) );
  m_Address1->SetValue( wxString(str,wxConvUTF8 ) ); StrOp.free( str );
  str = StrOp.fmt( "%d", wSelTab.getaddr2( m_Props ) );
  m_Address2->SetValue( wxString(str,wxConvUTF8 ) ); StrOp.free( str );
  str = StrOp.fmt( "%d", wSelTab.getaddr3( m_Props ) );
  m_Address3->SetValue( wxString(str,wxConvUTF8 ) ); StrOp.free( str );
  str = StrOp.fmt( "%d", wSelTab.getaddr4( m_Props ) );
  m_Address4->SetValue( wxString(str,wxConvUTF8 ) ); StrOp.free( str );
  str = StrOp.fmt( "%d", wSelTab.getaddr5( m_Props ) );
  m_Address5->SetValue( wxString(str,wxConvUTF8 ) ); StrOp.free( str );
  str = StrOp.fmt( "%d", wSelTab.getaddr6( m_Props ) );
  m_Address6->SetValue( wxString(str,wxConvUTF8 ) ); StrOp.free( str );
  str = StrOp.fmt( "%d", wSelTab.getaddr7( m_Props ) );
  m_Address7->SetValue( wxString(str,wxConvUTF8 ) ); StrOp.free( str );

  str = StrOp.fmt( "%d", wSelTab.getport0( m_Props ) );
  m_Port0->SetValue( wxString(str,wxConvUTF8 ) ); StrOp.free( str );
  str = StrOp.fmt( "%d", wSelTab.getport1( m_Props ) );
  m_Port1->SetValue( wxString(str,wxConvUTF8 ) ); StrOp.free( str );
  str = StrOp.fmt( "%d", wSelTab.getport2( m_Props ) );
  m_Port2->SetValue( wxString(str,wxConvUTF8 ) ); StrOp.free( str );
  str = StrOp.fmt( "%d", wSelTab.getport3( m_Props ) );
  m_Port3->SetValue( wxString(str,wxConvUTF8 ) ); StrOp.free( str );
  str = StrOp.fmt( "%d", wSelTab.getport4( m_Props ) );
  m_Port4->SetValue( wxString(str,wxConvUTF8 ) ); StrOp.free( str );
  str = StrOp.fmt( "%d", wSelTab.getport5( m_Props ) );
  m_Port5->SetValue( wxString(str,wxConvUTF8 ) ); StrOp.free( str );
  str = StrOp.fmt( "%d", wSelTab.getport6( m_Props ) );
  m_Port6->SetValue( wxString(str,wxConvUTF8 ) ); StrOp.free( str );
  str = StrOp.fmt( "%d", wSelTab.getport7( m_Props ) );
  m_Port7->SetValue( wxString(str,wxConvUTF8 ) ); StrOp.free( str );

  m_Invert->SetValue( wSelTab.isinv(m_Props)?true:false );
  m_InvertNew->SetValue( wSelTab.isinvnew(m_Props)?true:false );
  m_SingleGate->SetValue( wSelTab.issinglegate(m_Props)?true:false );

  m_Type->SetStringSelection( wxString(wSelTab.getprot( m_Props ),wxConvUTF8 ) );

  m_PosSen->SetSelection(0);
  if( wSelTab.getpsen( m_Props ) != NULL && StrOp.len( wSelTab.getpsen( m_Props ) ) > 0 )
    m_PosSen->SetStringSelection( wxString(wSelTab.getpsen(m_Props),wxConvUTF8) );
  m_b0Sen->SetSelection(0);
  if( wSelTab.getb0sen( m_Props ) != NULL && StrOp.len( wSelTab.getb0sen( m_Props ) ) > 0 )
    m_b0Sen->SetStringSelection( wxString(wSelTab.getb0sen(m_Props),wxConvUTF8) );
  m_b1Sen->SetSelection(0);
  if( wSelTab.getb1sen( m_Props ) != NULL && StrOp.len( wSelTab.getb1sen( m_Props ) ) > 0 )
    m_b1Sen->SetStringSelection( wxString(wSelTab.getb1sen(m_Props),wxConvUTF8) );
  m_b2Sen->SetSelection(0);
  if( wSelTab.getb2sen( m_Props ) != NULL && StrOp.len( wSelTab.getb2sen( m_Props ) ) > 0 )
    m_b2Sen->SetStringSelection( wxString(wSelTab.getb2sen(m_Props),wxConvUTF8) );
  m_b3Sen->SetSelection(0);
  if( wSelTab.getb3sen( m_Props ) != NULL && StrOp.len( wSelTab.getb3sen( m_Props ) ) > 0 )
    m_b3Sen->SetStringSelection( wxString(wSelTab.getb3sen(m_Props),wxConvUTF8) );
  m_b4Sen->SetSelection(0);
  if( wSelTab.getb4sen( m_Props ) != NULL && StrOp.len( wSelTab.getb4sen( m_Props ) ) > 0 )
    m_b4Sen->SetStringSelection( wxString(wSelTab.getb4sen(m_Props),wxConvUTF8) );
  m_b5Sen->SetSelection(0);
  if( wSelTab.getb5sen( m_Props ) != NULL && StrOp.len( wSelTab.getb5sen( m_Props ) ) > 0 )
    m_b5Sen->SetStringSelection( wxString(wSelTab.getb5sen(m_Props),wxConvUTF8) );
  m_b6Sen->SetSelection(0);
  if( wSelTab.getb6sen( m_Props ) != NULL && StrOp.len( wSelTab.getb6sen( m_Props ) ) > 0 )
    m_b6Sen->SetStringSelection( wxString(wSelTab.getb6sen(m_Props),wxConvUTF8) );

  // Tracks
  m_TracksGrid->DeleteRows(0,m_TracksGrid->GetNumberRows());
  iONode track = wSelTab.getseltabpos( m_Props );
  while( track != NULL ) {
    m_TracksGrid->AppendRows();
    char* nr = StrOp.fmt( "%d", wSelTabPos.getnr( track ) );
    m_TracksGrid->SetCellValue(m_TracksGrid->GetNumberRows()-1, 0, wxString(nr,wxConvUTF8 ) );
    StrOp.free( nr );
    m_TracksGrid->SetCellValue(m_TracksGrid->GetNumberRows()-1, 1, wxString(wSelTabPos.getbkid( track ),wxConvUTF8 ) );
    m_TracksGrid->SetCellValue(m_TracksGrid->GetNumberRows()-1, 2, wxString(wSelTabPos.getdesc( track ),wxConvUTF8 ) );
    m_TracksGrid->SetReadOnly( m_TracksGrid->GetNumberRows()-1, 0, true );
    m_TracksGrid->SetReadOnly( m_TracksGrid->GetNumberRows()-1, 1, true );
    m_TracksGrid->SetReadOnly( m_TracksGrid->GetNumberRows()-1, 2, true );
    track = wSelTab.nextseltabpos( m_Props, track );
  };

  m_DelTrack->Enable( false );
  m_ModifyTrack->Enable( false );

  m_TracksGrid->AutoSize();
  m_TracksGrid->FitInside();
  m_TracksGrid->UpdateDimensions();
  m_TracksPanel->GetSizer()->Layout();

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
        if( bkb != NULL && StrOp.equals( bkb, wSelTab.getid( m_Props ) ) ||
            bka != NULL && StrOp.equals( bka, wSelTab.getid( m_Props ) ) && !wRoute.isdir(st) ) {
          char* str = StrOp.fmt( "%s = from \"%s\" to \"%s\"", id, bka, bkb );
          m_Routes->Append( wxString(str,wxConvUTF8), st );
          StrOp.free( str );
        }
      }
    }
  }

}


bool SelTabDialog::evaluate() {
  if( m_Props == NULL )
    return false;

  if( m_ID->GetValue().Len() == 0 ) {
    wxMessageDialog( this, wxGetApp().getMsg("invalidid"), _T("Rocrail"), wxOK | wxICON_ERROR ).ShowModal();
    m_ID->SetValue( wxString(wSelTab.getid( m_Props ),wxConvUTF8) );
    return false;
  }

  // General
  wItem.setprev_id( m_Props, wItem.getid(m_Props) );
  wSelTab.setid( m_Props, m_ID->GetValue().mb_str(wxConvUTF8) );
  wSelTab.setdesc( m_Props, m_Desc->GetValue().mb_str(wxConvUTF8) );
  wSelTab.setnrtracks( m_Props, m_NrTracks->GetValue() );
  wSelTab.setsharedfb(m_Props, m_SharedFB->IsChecked()?True:False);
  wSelTab.setmanager(m_Props, m_Manager->IsChecked()?True:False);
  wSelTab.setminocc( m_Props, m_MinOcc->GetValue() );
  wSelTab.setmovedelay( m_Props, m_MoveDelay->GetValue() );
  wSelTab.setfifo(m_Props, m_FiFo->IsChecked()?True:False);
  wSelTab.setoffpos( m_Props, m_OffPos->GetValue() );

  // Location
  wSelTab.setx( m_Props, atoi( m_x->GetValue().mb_str(wxConvUTF8) ) );
  wSelTab.sety( m_Props, atoi( m_y->GetValue().mb_str(wxConvUTF8) ) );
  wSelTab.setz( m_Props, atoi( m_z->GetValue().mb_str(wxConvUTF8) ) );
  int ori = m_ori->GetSelection();
  if( ori == 0 )
    wItem.setori( m_Props, wItem.north );
  else if( ori == 1 )
    wItem.setori( m_Props, wItem.east );
  else if( ori == 2 )
    wItem.setori( m_Props, wItem.south );
  else if( ori == 3 )
    wItem.setori( m_Props, wItem.west );

  // Interface
  wSelTab.setiid( m_Props, m_IID->GetValue().mb_str(wxConvUTF8) );
  wSelTab.setaddr0( m_Props, atoi( m_Address0->GetValue().mb_str(wxConvUTF8) ) );
  wSelTab.setaddr1( m_Props, atoi( m_Address1->GetValue().mb_str(wxConvUTF8) ) );
  wSelTab.setaddr2( m_Props, atoi( m_Address2->GetValue().mb_str(wxConvUTF8) ) );
  wSelTab.setaddr3( m_Props, atoi( m_Address3->GetValue().mb_str(wxConvUTF8) ) );
  wSelTab.setaddr4( m_Props, atoi( m_Address4->GetValue().mb_str(wxConvUTF8) ) );
  wSelTab.setaddr5( m_Props, atoi( m_Address5->GetValue().mb_str(wxConvUTF8) ) );
  wSelTab.setaddr6( m_Props, atoi( m_Address6->GetValue().mb_str(wxConvUTF8) ) );
  wSelTab.setaddr7( m_Props, atoi( m_Address7->GetValue().mb_str(wxConvUTF8) ) );

  wSelTab.setport0( m_Props, atoi( m_Port0->GetValue().mb_str(wxConvUTF8) ) );
  wSelTab.setport1( m_Props, atoi( m_Port1->GetValue().mb_str(wxConvUTF8) ) );
  wSelTab.setport2( m_Props, atoi( m_Port2->GetValue().mb_str(wxConvUTF8) ) );
  wSelTab.setport3( m_Props, atoi( m_Port3->GetValue().mb_str(wxConvUTF8) ) );
  wSelTab.setport4( m_Props, atoi( m_Port4->GetValue().mb_str(wxConvUTF8) ) );
  wSelTab.setport5( m_Props, atoi( m_Port5->GetValue().mb_str(wxConvUTF8) ) );
  wSelTab.setport6( m_Props, atoi( m_Port6->GetValue().mb_str(wxConvUTF8) ) );
  wSelTab.setport7( m_Props, atoi( m_Port7->GetValue().mb_str(wxConvUTF8) ) );

  wSelTab.setinv(m_Props, m_Invert->IsChecked()?True:False);
  wSelTab.setinvnew(m_Props, m_InvertNew->IsChecked()?True:False);
  wSelTab.setsinglegate(m_Props, m_SingleGate->IsChecked()?True:False);

  wSelTab.setprot( m_Props, m_Type->GetStringSelection().mb_str(wxConvUTF8) );

  if( StrOp.equals( "-", m_PosSen->GetStringSelection().mb_str(wxConvUTF8) ) )
    wSelTab.setpsen( m_Props, "" );
  else
    wSelTab.setpsen( m_Props, m_PosSen->GetStringSelection().mb_str(wxConvUTF8) );

  if( StrOp.equals( "-", m_b0Sen->GetStringSelection().mb_str(wxConvUTF8) ) )
    wSelTab.setb0sen( m_Props, "" );
  else
    wSelTab.setb0sen( m_Props, m_b0Sen->GetStringSelection().mb_str(wxConvUTF8) );

  if( StrOp.equals( "-", m_b1Sen->GetStringSelection().mb_str(wxConvUTF8) ) )
    wSelTab.setb1sen( m_Props, "" );
  else
    wSelTab.setb1sen( m_Props, m_b1Sen->GetStringSelection().mb_str(wxConvUTF8) );

  if( StrOp.equals( "-", m_b2Sen->GetStringSelection().mb_str(wxConvUTF8) ) )
    wSelTab.setb2sen( m_Props, "" );
  else
    wSelTab.setb2sen( m_Props, m_b2Sen->GetStringSelection().mb_str(wxConvUTF8) );

  if( StrOp.equals( "-", m_b3Sen->GetStringSelection().mb_str(wxConvUTF8) ) )
    wSelTab.setb3sen( m_Props, "" );
  else
    wSelTab.setb3sen( m_Props, m_b3Sen->GetStringSelection().mb_str(wxConvUTF8) );

  if( StrOp.equals( "-", m_b4Sen->GetStringSelection().mb_str(wxConvUTF8) ) )
    wSelTab.setb4sen( m_Props, "" );
  else
    wSelTab.setb4sen( m_Props, m_b4Sen->GetStringSelection().mb_str(wxConvUTF8) );

  if( StrOp.equals( "-", m_b5Sen->GetStringSelection().mb_str(wxConvUTF8) ) )
    wSelTab.setb5sen( m_Props, "" );
  else
    wSelTab.setb5sen( m_Props, m_b5Sen->GetStringSelection().mb_str(wxConvUTF8) );

  if( StrOp.equals( "-", m_b6Sen->GetStringSelection().mb_str(wxConvUTF8) ) )
    wSelTab.setb6sen( m_Props, "" );
  else
    wSelTab.setb6sen( m_Props, m_b6Sen->GetStringSelection().mb_str(wxConvUTF8) );

  return true;
}

void SelTabDialog::initSensorCombos() {
  wxComboBox* ids[4] = {m_SensorID1,m_SensorID2,m_SensorID3,m_SensorID4};
  wxComboBox* acts[4] = {m_SensorAction1,m_SensorAction2,m_SensorAction3,m_SensorAction4};

  for( int i = 0; i < 4; i++ ) {
    ids[i]->Clear();
    acts[i]->Clear();
    ids[i]->Append( wxString("-",wxConvUTF8) );
    acts[i]->Append( wxString("-",wxConvUTF8) );

    acts[i]->Append( wxString(wFeedbackEvent.enter_event,wxConvUTF8) );
    acts[i]->Append( wxString(wFeedbackEvent.enter2route_event,wxConvUTF8) );
    acts[i]->Append( wxString(wFeedbackEvent.enter2in_event,wxConvUTF8) );
    acts[i]->Append( wxString(wFeedbackEvent.in_event,wxConvUTF8) );
    acts[i]->Append( wxString(wFeedbackEvent.exit_event,wxConvUTF8) );
    acts[i]->Append( wxString(wFeedbackEvent.pre2in_event,wxConvUTF8) );
    acts[i]->Append( wxString(wFeedbackEvent.occupied_event,wxConvUTF8) );
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
      }
    }
  }
  /* clean up the temp. list */
  ListOp.base.del(list);
}

void SelTabDialog::initSensors() {
  TraceOp.trc( "seltabdlg", TRCLEVEL_INFO, __LINE__, 9999, "initSensors" );

  wxComboBox* ids[4] = {m_SensorID1,m_SensorID2,m_SensorID3,m_SensorID4};
  wxComboBox* acts[4] = {m_SensorAction1,m_SensorAction2,m_SensorAction3,m_SensorAction4};

  for( int i = 0; i < 4; i++ ) {
    ids[i]->SetStringSelection( wxString("-",wxConvUTF8) );
    acts[i]->SetStringSelection( wxString("-",wxConvUTF8) );
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

  TraceOp.trc( "seltabdlg", TRCLEVEL_INFO, __LINE__, 9999, "route [%s]", routeID );

  if( st == NULL ) {
    /* if available use selection from above , otherwise default is "all" */
    if( routeID != NULL ) {
      m_FromBlockID = routeID;
    }
    else {
      m_FromBlockID = wFeedbackEvent.from_all;
    }
  }
  else if( StrOp.equals( wSelTab.getid( m_Props ), wRoute.getbkb( st ) ) ) {
    m_FromBlockID = wRoute.getbka( st );
  }
  else if( !wRoute.isdir(st) ) {
    m_FromBlockID = wRoute.getbkb( st );
  }
  else {
    m_LabelSensorsFromBlock->SetLabel( _T("") );
    return;
  }

  TraceOp.trc( "seltabdlg", TRCLEVEL_INFO, __LINE__, 9999, "FromBlockID [%s]", m_FromBlockID );



  char* s = StrOp.fmt( wxGetApp().getMsg("sensorfromblock").mb_str(wxConvUTF8), m_FromBlockID );
  m_LabelSensorsFromBlock->SetLabel( wxString( s ,wxConvUTF8) );
  StrOp.free(s);

  iONode fb = wSelTab.getfbevent( m_Props );
  idx = 0;
  while( fb != NULL && idx < 5 ) {
    TraceOp.trc( "blockdlg", TRCLEVEL_INFO, __LINE__, 9999, "fbIndex %d", idx );
    if( StrOp.equals( m_FromBlockID, wFeedbackEvent.getfrom( fb ) ) ) {
      ids[idx]->SetStringSelection( wxString(wFeedbackEvent.getid( fb ),wxConvUTF8) );
      acts[idx]->SetStringSelection( wxString(wFeedbackEvent.getaction( fb ),wxConvUTF8) );

      m_fbEvents[idx] = fb;
      idx++;
    }
    fb = wSelTab.nextfbevent( m_Props, fb );
  };

}


void SelTabDialog::evaluateSensors() {
  TraceOp.trc( "blockdlg", TRCLEVEL_INFO, __LINE__, 9999, "evaluateSensors" );
  if( m_Props == NULL || m_Routes->GetSelection()== wxNOT_FOUND )
    return;

  wxComboBox* ids[4] = {m_SensorID1,m_SensorID2,m_SensorID3,m_SensorID4};
  wxComboBox* acts[4] = {m_SensorAction1,m_SensorAction2,m_SensorAction3,m_SensorAction4};

  for( int i = 0; i < 4; i++ ) {
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
    }

  }
}


/*!
 * SenTabDialog creator
 */

bool SelTabDialog::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin SelTabDialog creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end SelTabDialog creation
    return true;
}


/*!
 * SelTabDialog destructor
 */

SelTabDialog::~SelTabDialog()
{
////@begin SelTabDialog destruction
////@end SelTabDialog destruction
}


/*!
 * Member initialisation
 */

void SelTabDialog::Init()
{
////@begin SelTabDialog member initialisation
    m_Notebook = NULL;
    m_GeneralPanel = NULL;
    m_LabelID = NULL;
    m_ID = NULL;
    m_LabelDesc = NULL;
    m_Desc = NULL;
    m_labNrTracks = NULL;
    m_NrTracks = NULL;
    m_labMinOcc = NULL;
    m_MinOcc = NULL;
    m_labMoveDelay = NULL;
    m_MoveDelay = NULL;
    m_labOffPos = NULL;
    m_OffPos = NULL;
    m_SharedFB = NULL;
    m_Manager = NULL;
    m_FiFo = NULL;
    m_LocationPanel = NULL;
    m_LabelX = NULL;
    m_x = NULL;
    m_LabelY = NULL;
    m_y = NULL;
    m_LabelZ = NULL;
    m_z = NULL;
    m_ori = NULL;
    m_Interface = NULL;
    m_Labeliid = NULL;
    m_IID = NULL;
    m_LabelType = NULL;
    m_Type = NULL;
    m_LabelAddress0 = NULL;
    m_Address0 = NULL;
    m_labPort0 = NULL;
    m_Port0 = NULL;
    m_LabelAddress1 = NULL;
    m_Address1 = NULL;
    m_labPort1 = NULL;
    m_Port1 = NULL;
    m_LabelAddress2 = NULL;
    m_Address2 = NULL;
    m_labPort2 = NULL;
    m_Port2 = NULL;
    m_LabelAddress3 = NULL;
    m_Address3 = NULL;
    m_labPort3 = NULL;
    m_Port3 = NULL;
    m_LabelAddress5 = NULL;
    m_Address5 = NULL;
    m_labPort5 = NULL;
    m_Port5 = NULL;
    m_LabelAddress6 = NULL;
    m_Address6 = NULL;
    m_labPort6 = NULL;
    m_Port6 = NULL;
    m_LabelAddress7 = NULL;
    m_Address7 = NULL;
    m_labPort7 = NULL;
    m_Port7 = NULL;
    m_Invert = NULL;
    m_SingleGate = NULL;
    m_labPosSen = NULL;
    m_PosSen = NULL;
    m_labb0Sen = NULL;
    m_b0Sen = NULL;
    m_labb1Sen = NULL;
    m_b1Sen = NULL;
    m_labb2Sen = NULL;
    m_b2Sen = NULL;
    m_labb3Sen = NULL;
    m_b3Sen = NULL;
    m_labb4Sen = NULL;
    m_b4Sen = NULL;
    m_labb5Sen = NULL;
    m_b5Sen = NULL;
    m_labb6Sen = NULL;
    m_b6Sen = NULL;
    m_NewPosBox = NULL;
    m_LabelAddress4 = NULL;
    m_Address4 = NULL;
    m_labPort4 = NULL;
    m_Port4 = NULL;
    m_InvertNew = NULL;
    m_TracksPanel = NULL;
    m_TracksGrid = NULL;
    m_AddTrack = NULL;
    m_DelTrack = NULL;
    m_ModifyTrack = NULL;
    m_RoutesPanel = NULL;
    m_Routes = NULL;
    m_LabelSensorsFromBlock = NULL;
    m_LabelSensorIDs = NULL;
    m_LabelSensorActions = NULL;
    m_SensorID1 = NULL;
    m_SensorAction1 = NULL;
    m_SensorID2 = NULL;
    m_SensorAction2 = NULL;
    m_SensorID3 = NULL;
    m_SensorAction3 = NULL;
    m_SensorID4 = NULL;
    m_SensorAction4 = NULL;
    m_OK = NULL;
    m_Cancel = NULL;
////@end SelTabDialog member initialisation
}


/*!
 * Control creation for SenTabDialog
 */

void SelTabDialog::CreateControls()
{
////@begin SelTabDialog content construction
    SelTabDialog* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    m_Notebook = new wxNotebook( itemDialog1, ID_NOTEBOOK_TURNTABLE, wxDefaultPosition, wxDefaultSize, m_TabAlign );

    m_GeneralPanel = new wxPanel( m_Notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
    m_GeneralPanel->SetSizer(itemBoxSizer5);

    wxBoxSizer* itemBoxSizer6 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer5->Add(itemBoxSizer6, 1, wxALIGN_TOP|wxALL, 5);
    wxFlexGridSizer* itemFlexGridSizer7 = new wxFlexGridSizer(0, 2, 0, 0);
    itemFlexGridSizer7->AddGrowableCol(1);
    itemBoxSizer6->Add(itemFlexGridSizer7, 0, wxGROW|wxALL, 5);
    m_LabelID = new wxStaticText( m_GeneralPanel, wxID_ANY, _("id"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer7->Add(m_LabelID, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_ID = new wxTextCtrl( m_GeneralPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer7->Add(m_ID, 1, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_LabelDesc = new wxStaticText( m_GeneralPanel, wxID_ANY, _("Desc."), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer7->Add(m_LabelDesc, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Desc = new wxTextCtrl( m_GeneralPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer7->Add(m_Desc, 0, wxALIGN_LEFT|wxGROW|wxALL, 5);

    m_labNrTracks = new wxStaticText( m_GeneralPanel, wxID_ANY, _("Tracks"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer7->Add(m_labNrTracks, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_NrTracks = new wxSpinCtrl( m_GeneralPanel, wxID_ANY, _T("8"), wxDefaultPosition, wxSize(70, -1), wxSP_ARROW_KEYS, 2, 128, 8 );
    itemFlexGridSizer7->Add(m_NrTracks, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labMinOcc = new wxStaticText( m_GeneralPanel, wxID_ANY, _("MinOcc"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer7->Add(m_labMinOcc, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_MinOcc = new wxSpinCtrl( m_GeneralPanel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(70, -1), wxSP_ARROW_KEYS, 0, 128, 0 );
    itemFlexGridSizer7->Add(m_MinOcc, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labMoveDelay = new wxStaticText( m_GeneralPanel, wxID_ANY, _("Move delay"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer7->Add(m_labMoveDelay, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_MoveDelay = new wxSpinCtrl( m_GeneralPanel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(70, -1), wxSP_ARROW_KEYS, 0, 5, 0 );
    itemFlexGridSizer7->Add(m_MoveDelay, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labOffPos = new wxStaticText( m_GeneralPanel, wxID_ANY, _("Power Off pos."), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer7->Add(m_labOffPos, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_OffPos = new wxSpinCtrl( m_GeneralPanel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(70, -1), wxSP_ARROW_KEYS, 0, 16, 0 );
    itemFlexGridSizer7->Add(m_OffPos, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticLine* itemStaticLine20 = new wxStaticLine( m_GeneralPanel, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL );
    itemBoxSizer5->Add(itemStaticLine20, 0, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer21 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer5->Add(itemBoxSizer21, 0, wxALIGN_TOP|wxALL, 5);
    m_SharedFB = new wxCheckBox( m_GeneralPanel, ID_FY_SHAREDFB, _("Shared sensors"), wxDefaultPosition, wxDefaultSize, 0 );
    m_SharedFB->SetValue(false);
    itemBoxSizer21->Add(m_SharedFB, 0, wxALIGN_LEFT|wxALL, 5);

    m_Manager = new wxCheckBox( m_GeneralPanel, wxID_ANY, _("Manage Track Blocks"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Manager->SetValue(false);
    itemBoxSizer21->Add(m_Manager, 0, wxALIGN_LEFT|wxALL, 5);

    m_FiFo = new wxCheckBox( m_GeneralPanel, wxID_ANY, _("FiFo"), wxDefaultPosition, wxDefaultSize, 0 );
    m_FiFo->SetValue(false);
    itemBoxSizer21->Add(m_FiFo, 0, wxALIGN_LEFT|wxALL, 5);

    m_Notebook->AddPage(m_GeneralPanel, _("General"));

    m_LocationPanel = new wxPanel( m_Notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer26 = new wxBoxSizer(wxHORIZONTAL);
    m_LocationPanel->SetSizer(itemBoxSizer26);

    wxFlexGridSizer* itemFlexGridSizer27 = new wxFlexGridSizer(0, 2, 0, 0);
    itemBoxSizer26->Add(itemFlexGridSizer27, 0, wxALIGN_TOP|wxALL, 5);
    m_LabelX = new wxStaticText( m_LocationPanel, wxID_ANY, _("x"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer27->Add(m_LabelX, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_x = new wxTextCtrl( m_LocationPanel, wxID_ANY, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer27->Add(m_x, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_TOP|wxALL, 5);

    m_LabelY = new wxStaticText( m_LocationPanel, wxID_ANY, _("y"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer27->Add(m_LabelY, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_y = new wxTextCtrl( m_LocationPanel, wxID_ANY, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer27->Add(m_y, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_LabelZ = new wxStaticText( m_LocationPanel, wxID_ANY, _("z"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer27->Add(m_LabelZ, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_z = new wxTextCtrl( m_LocationPanel, wxID_ANY, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer27->Add(m_z, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxArrayString m_oriStrings;
    m_oriStrings.Add(_("&north"));
    m_oriStrings.Add(_("&east"));
    m_oriStrings.Add(_("&south"));
    m_oriStrings.Add(_("&west"));
    m_ori = new wxRadioBox( m_LocationPanel, ID_RADIOBOX_FY_ORI, _("Orientation"), wxDefaultPosition, wxDefaultSize, m_oriStrings, 1, wxRA_SPECIFY_COLS );
    m_ori->SetSelection(0);
    itemBoxSizer26->Add(m_ori, 0, wxALIGN_TOP|wxALL, 5);

    m_Notebook->AddPage(m_LocationPanel, _("Location"));

    m_Interface = new wxPanel( m_Notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer36 = new wxBoxSizer(wxHORIZONTAL);
    m_Interface->SetSizer(itemBoxSizer36);

    wxBoxSizer* itemBoxSizer37 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer36->Add(itemBoxSizer37, 0, wxALIGN_TOP|wxALL, 5);
    wxFlexGridSizer* itemFlexGridSizer38 = new wxFlexGridSizer(0, 2, 0, 0);
    itemFlexGridSizer38->AddGrowableCol(1);
    itemBoxSizer37->Add(itemFlexGridSizer38, 0, wxGROW|wxALL, 5);
    m_Labeliid = new wxStaticText( m_Interface, wxID_ANY, _("iid"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer38->Add(m_Labeliid, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_IID = new wxTextCtrl( m_Interface, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer38->Add(m_IID, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer41 = new wxFlexGridSizer(0, 2, 0, 0);
    itemBoxSizer37->Add(itemFlexGridSizer41, 0, wxGROW|wxALL, 5);
    m_LabelType = new wxStaticText( m_Interface, wxID_ANY, _("type"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer41->Add(m_LabelType, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxArrayString m_TypeStrings;
    m_Type = new wxComboBox( m_Interface, ID_SELTAB_TYPE, wxEmptyString, wxDefaultPosition, wxSize(100, -1), m_TypeStrings, wxCB_READONLY );
    itemFlexGridSizer41->Add(m_Type, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer44 = new wxFlexGridSizer(0, 4, 0, 0);
    itemBoxSizer37->Add(itemFlexGridSizer44, 0, wxGROW|wxALL, 5);
    m_LabelAddress0 = new wxStaticText( m_Interface, wxID_ANY, _("address 0"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer44->Add(m_LabelAddress0, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_Address0 = new wxTextCtrl( m_Interface, wxID_ANY, _("0"), wxDefaultPosition, wxSize(60, -1), wxTE_CENTRE );
    itemFlexGridSizer44->Add(m_Address0, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_labPort0 = new wxStaticText( m_Interface, wxID_ANY, _("port"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer44->Add(m_labPort0, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_Port0 = new wxTextCtrl( m_Interface, wxID_ANY, _("0"), wxDefaultPosition, wxSize(60, -1), wxTE_CENTRE );
    itemFlexGridSizer44->Add(m_Port0, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_LabelAddress1 = new wxStaticText( m_Interface, wxID_ANY, _("address 1"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer44->Add(m_LabelAddress1, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_Address1 = new wxTextCtrl( m_Interface, wxID_ANY, _("0"), wxDefaultPosition, wxSize(60, -1), wxTE_CENTRE );
    itemFlexGridSizer44->Add(m_Address1, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_labPort1 = new wxStaticText( m_Interface, wxID_ANY, _("port"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer44->Add(m_labPort1, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_Port1 = new wxTextCtrl( m_Interface, wxID_ANY, _("0"), wxDefaultPosition, wxSize(60, -1), wxTE_CENTRE );
    itemFlexGridSizer44->Add(m_Port1, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_LabelAddress2 = new wxStaticText( m_Interface, wxID_ANY, _("address 2"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer44->Add(m_LabelAddress2, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_Address2 = new wxTextCtrl( m_Interface, wxID_ANY, _("0"), wxDefaultPosition, wxSize(60, -1), wxTE_CENTRE );
    itemFlexGridSizer44->Add(m_Address2, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_labPort2 = new wxStaticText( m_Interface, wxID_ANY, _("port"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer44->Add(m_labPort2, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_Port2 = new wxTextCtrl( m_Interface, wxID_ANY, _("0"), wxDefaultPosition, wxSize(60, -1), wxTE_CENTRE );
    itemFlexGridSizer44->Add(m_Port2, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_LabelAddress3 = new wxStaticText( m_Interface, wxID_ANY, _("address 3"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer44->Add(m_LabelAddress3, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_Address3 = new wxTextCtrl( m_Interface, wxID_ANY, _("0"), wxDefaultPosition, wxSize(60, -1), wxTE_CENTRE );
    itemFlexGridSizer44->Add(m_Address3, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_labPort3 = new wxStaticText( m_Interface, wxID_ANY, _("port"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer44->Add(m_labPort3, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_Port3 = new wxTextCtrl( m_Interface, wxID_ANY, _("0"), wxDefaultPosition, wxSize(60, -1), wxTE_CENTRE );
    itemFlexGridSizer44->Add(m_Port3, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_LabelAddress5 = new wxStaticText( m_Interface, wxID_ANY, _("address 4"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer44->Add(m_LabelAddress5, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_Address5 = new wxTextCtrl( m_Interface, wxID_ANY, _("0"), wxDefaultPosition, wxSize(60, -1), wxTE_CENTRE );
    itemFlexGridSizer44->Add(m_Address5, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_labPort5 = new wxStaticText( m_Interface, wxID_ANY, _("port"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer44->Add(m_labPort5, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_Port5 = new wxTextCtrl( m_Interface, wxID_ANY, _("0"), wxDefaultPosition, wxSize(60, -1), wxTE_CENTRE );
    itemFlexGridSizer44->Add(m_Port5, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_LabelAddress6 = new wxStaticText( m_Interface, wxID_ANY, _("address 5"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer44->Add(m_LabelAddress6, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_Address6 = new wxTextCtrl( m_Interface, wxID_ANY, _("0"), wxDefaultPosition, wxSize(60, -1), wxTE_CENTRE );
    itemFlexGridSizer44->Add(m_Address6, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_labPort6 = new wxStaticText( m_Interface, wxID_STATIC, _("port"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer44->Add(m_labPort6, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_Port6 = new wxTextCtrl( m_Interface, wxID_ANY, _("0"), wxDefaultPosition, wxSize(60, -1), wxTE_CENTRE );
    itemFlexGridSizer44->Add(m_Port6, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_LabelAddress7 = new wxStaticText( m_Interface, wxID_ANY, _("address 6"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer44->Add(m_LabelAddress7, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_Address7 = new wxTextCtrl( m_Interface, wxID_ANY, _("0"), wxDefaultPosition, wxSize(60, -1), wxTE_CENTRE );
    itemFlexGridSizer44->Add(m_Address7, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_labPort7 = new wxStaticText( m_Interface, wxID_ANY, _("port"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer44->Add(m_labPort7, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_Port7 = new wxTextCtrl( m_Interface, wxID_ANY, _("0"), wxDefaultPosition, wxSize(60, -1), wxTE_CENTRE );
    itemFlexGridSizer44->Add(m_Port7, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    wxFlexGridSizer* itemFlexGridSizer73 = new wxFlexGridSizer(0, 2, 0, 0);
    itemBoxSizer37->Add(itemFlexGridSizer73, 0, wxALIGN_LEFT, 5);
    m_Invert = new wxCheckBox( m_Interface, wxID_ANY, _("Invert commands "), wxDefaultPosition, wxDefaultSize, 0 );
    m_Invert->SetValue(false);
    itemFlexGridSizer73->Add(m_Invert, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_SingleGate = new wxCheckBox( m_Interface, wxID_ANY, _("Single gate"), wxDefaultPosition, wxDefaultSize, 0 );
    m_SingleGate->SetValue(false);
    itemFlexGridSizer73->Add(m_SingleGate, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticLine* itemStaticLine76 = new wxStaticLine( m_Interface, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL );
    itemBoxSizer36->Add(itemStaticLine76, 0, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer77 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer36->Add(itemBoxSizer77, 0, wxALIGN_TOP|wxALL, 5);
    wxFlexGridSizer* itemFlexGridSizer78 = new wxFlexGridSizer(0, 2, 0, 0);
    itemFlexGridSizer78->AddGrowableCol(1);
    itemBoxSizer77->Add(itemFlexGridSizer78, 0, wxGROW|wxALL, 5);
    m_labPosSen = new wxStaticText( m_Interface, wxID_ANY, _("Position Sensor"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer78->Add(m_labPosSen, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5);

    wxArrayString m_PosSenStrings;
    m_PosSen = new wxComboBox( m_Interface, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, m_PosSenStrings, wxCB_READONLY );
    itemFlexGridSizer78->Add(m_PosSen, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5);

    m_labb0Sen = new wxStaticText( m_Interface, wxID_ANY, _("b0 Sensor"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer78->Add(m_labb0Sen, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5);

    wxArrayString m_b0SenStrings;
    m_b0Sen = new wxComboBox( m_Interface, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, m_b0SenStrings, wxCB_READONLY );
    itemFlexGridSizer78->Add(m_b0Sen, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5);

    m_labb1Sen = new wxStaticText( m_Interface, wxID_ANY, _("b1 Sensor"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer78->Add(m_labb1Sen, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    wxArrayString m_b1SenStrings;
    m_b1Sen = new wxComboBox( m_Interface, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, m_b1SenStrings, wxCB_READONLY );
    itemFlexGridSizer78->Add(m_b1Sen, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_labb2Sen = new wxStaticText( m_Interface, wxID_ANY, _("b2 Sensor"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer78->Add(m_labb2Sen, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    wxArrayString m_b2SenStrings;
    m_b2Sen = new wxComboBox( m_Interface, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, m_b2SenStrings, wxCB_READONLY );
    itemFlexGridSizer78->Add(m_b2Sen, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_labb3Sen = new wxStaticText( m_Interface, wxID_ANY, _("b3 Sensor"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer78->Add(m_labb3Sen, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    wxArrayString m_b3SenStrings;
    m_b3Sen = new wxComboBox( m_Interface, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, m_b3SenStrings, wxCB_READONLY );
    itemFlexGridSizer78->Add(m_b3Sen, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_labb4Sen = new wxStaticText( m_Interface, wxID_ANY, _("b4 Sensor"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer78->Add(m_labb4Sen, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    wxArrayString m_b4SenStrings;
    m_b4Sen = new wxComboBox( m_Interface, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, m_b4SenStrings, wxCB_READONLY );
    itemFlexGridSizer78->Add(m_b4Sen, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_labb5Sen = new wxStaticText( m_Interface, wxID_ANY, _("b5 Sensor"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer78->Add(m_labb5Sen, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    wxArrayString m_b5SenStrings;
    m_b5Sen = new wxComboBox( m_Interface, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, m_b5SenStrings, wxCB_READONLY );
    itemFlexGridSizer78->Add(m_b5Sen, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_labb6Sen = new wxStaticText( m_Interface, wxID_ANY, _("b6 Sensor"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer78->Add(m_labb6Sen, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    wxArrayString m_b6SenStrings;
    m_b6Sen = new wxComboBox( m_Interface, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, m_b6SenStrings, wxCB_READONLY );
    itemFlexGridSizer78->Add(m_b6Sen, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_NewPosBox = new wxStaticBox(m_Interface, wxID_ANY, _("New position flag"));
    wxStaticBoxSizer* itemStaticBoxSizer95 = new wxStaticBoxSizer(m_NewPosBox, wxVERTICAL);
    itemBoxSizer77->Add(itemStaticBoxSizer95, 0, wxGROW|wxALL, 5);
    wxFlexGridSizer* itemFlexGridSizer96 = new wxFlexGridSizer(0, 4, 0, 0);
    itemStaticBoxSizer95->Add(itemFlexGridSizer96, 0, wxGROW|wxALL, 5);
    m_LabelAddress4 = new wxStaticText( m_Interface, wxID_ANY, _("address"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer96->Add(m_LabelAddress4, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_Address4 = new wxTextCtrl( m_Interface, wxID_ANY, _("0"), wxDefaultPosition, wxSize(60, -1), wxTE_CENTRE );
    itemFlexGridSizer96->Add(m_Address4, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_labPort4 = new wxStaticText( m_Interface, wxID_ANY, _("port"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer96->Add(m_labPort4, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Port4 = new wxTextCtrl( m_Interface, wxID_ANY, _("0"), wxDefaultPosition, wxSize(60, -1), wxTE_CENTRE );
    itemFlexGridSizer96->Add(m_Port4, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    wxBoxSizer* itemBoxSizer101 = new wxBoxSizer(wxVERTICAL);
    itemStaticBoxSizer95->Add(itemBoxSizer101, 0, wxGROW|wxALL, 5);
    m_InvertNew = new wxCheckBox( m_Interface, wxID_ANY, _("invert command"), wxDefaultPosition, wxDefaultSize, 0 );
    m_InvertNew->SetValue(false);
    itemBoxSizer101->Add(m_InvertNew, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT, 5);

    m_Notebook->AddPage(m_Interface, _("Interface"));

    m_TracksPanel = new wxPanel( m_Notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer104 = new wxBoxSizer(wxVERTICAL);
    m_TracksPanel->SetSizer(itemBoxSizer104);

    m_TracksGrid = new wxGrid( m_TracksPanel, ID_SELTAB_GRID, wxDefaultPosition, wxSize(220, 200), wxSUNKEN_BORDER|wxHSCROLL|wxVSCROLL|wxALWAYS_SHOW_SB );
    m_TracksGrid->SetDefaultColSize(50);
    m_TracksGrid->SetDefaultRowSize(20);
    m_TracksGrid->SetColLabelSize(20);
    m_TracksGrid->SetRowLabelSize(0);
    m_TracksGrid->CreateGrid(1, 3, wxGrid::wxGridSelectRows);
    itemBoxSizer104->Add(m_TracksGrid, 1, wxGROW|wxALL, 2);

    wxBoxSizer* itemBoxSizer106 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer104->Add(itemBoxSizer106, 0, wxGROW|wxALL, 5);
    m_AddTrack = new wxButton( m_TracksPanel, ID_BUTTON_SELTAB_ADDTRACK, _("Add"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer106->Add(m_AddTrack, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_DelTrack = new wxButton( m_TracksPanel, ID_BUTTON_SELTAB_DELTRACK, _("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer106->Add(m_DelTrack, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_ModifyTrack = new wxButton( m_TracksPanel, ID_BUTTON_SELTAB_MODIFYTRACK, _("Modify"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer106->Add(m_ModifyTrack, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Notebook->AddPage(m_TracksPanel, _("Tracks"));

    m_RoutesPanel = new wxPanel( m_Notebook, ID_PANEL_SELTAB_ROUTES, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer111 = new wxBoxSizer(wxVERTICAL);
    m_RoutesPanel->SetSizer(itemBoxSizer111);

    wxArrayString m_RoutesStrings;
    m_Routes = new wxListBox( m_RoutesPanel, ID_LISTBOX_SELTAB_ROUTES, wxDefaultPosition, wxDefaultSize, m_RoutesStrings, wxLB_SINGLE|wxLB_ALWAYS_SB );
    itemBoxSizer111->Add(m_Routes, 1, wxGROW|wxALL, 5);

    m_LabelSensorsFromBlock = new wxStaticText( m_RoutesPanel, wxID_ANY, _("..."), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer111->Add(m_LabelSensorsFromBlock, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP, 5);

    wxFlexGridSizer* itemFlexGridSizer114 = new wxFlexGridSizer(0, 2, 0, 0);
    itemBoxSizer111->Add(itemFlexGridSizer114, 0, wxGROW|wxALL, 5);
    m_LabelSensorIDs = new wxStaticText( m_RoutesPanel, wxID_ANY, _("Sensor ID:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer114->Add(m_LabelSensorIDs, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxTOP, 2);

    m_LabelSensorActions = new wxStaticText( m_RoutesPanel, wxID_ANY, _("Event:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer114->Add(m_LabelSensorActions, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxTOP, 2);

    wxArrayString m_SensorID1Strings;
    m_SensorID1 = new wxComboBox( m_RoutesPanel, wxID_SELTAB_ROUTESENSOR_1, wxEmptyString, wxDefaultPosition, wxSize(120, -1), m_SensorID1Strings, wxCB_READONLY );
    itemFlexGridSizer114->Add(m_SensorID1, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 2);

    wxArrayString m_SensorAction1Strings;
    m_SensorAction1 = new wxComboBox( m_RoutesPanel, wxID_SELTAB_ROUTEEVENT_1, wxEmptyString, wxDefaultPosition, wxSize(120, -1), m_SensorAction1Strings, wxCB_READONLY );
    itemFlexGridSizer114->Add(m_SensorAction1, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 2);

    wxArrayString m_SensorID2Strings;
    m_SensorID2 = new wxComboBox( m_RoutesPanel, wxID_SELTAB_ROUTESENSOR_2, wxEmptyString, wxDefaultPosition, wxSize(120, -1), m_SensorID2Strings, wxCB_READONLY );
    itemFlexGridSizer114->Add(m_SensorID2, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 2);

    wxArrayString m_SensorAction2Strings;
    m_SensorAction2 = new wxComboBox( m_RoutesPanel, wxID_SELTAB_ROUTEEVENT_2, wxEmptyString, wxDefaultPosition, wxSize(120, -1), m_SensorAction2Strings, wxCB_READONLY );
    itemFlexGridSizer114->Add(m_SensorAction2, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 2);

    wxArrayString m_SensorID3Strings;
    m_SensorID3 = new wxComboBox( m_RoutesPanel, wxID_SELTAB_ROUTESENSOR_3, wxEmptyString, wxDefaultPosition, wxSize(120, -1), m_SensorID3Strings, wxCB_READONLY );
    itemFlexGridSizer114->Add(m_SensorID3, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 2);

    wxArrayString m_SensorAction3Strings;
    m_SensorAction3 = new wxComboBox( m_RoutesPanel, wxID_SELTAB_ROUTEEVENT_3, wxEmptyString, wxDefaultPosition, wxSize(120, -1), m_SensorAction3Strings, wxCB_READONLY );
    itemFlexGridSizer114->Add(m_SensorAction3, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 2);

    wxArrayString m_SensorID4Strings;
    m_SensorID4 = new wxComboBox( m_RoutesPanel, wxID_SELTAB_ROUTESENSOR_4, wxEmptyString, wxDefaultPosition, wxSize(120, -1), m_SensorID4Strings, wxCB_READONLY );
    itemFlexGridSizer114->Add(m_SensorID4, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 2);

    wxArrayString m_SensorAction4Strings;
    m_SensorAction4 = new wxComboBox( m_RoutesPanel, wxID_SELTAB_ROUTEEVENT_4, wxEmptyString, wxDefaultPosition, wxSize(120, -1), m_SensorAction4Strings, wxCB_READONLY );
    itemFlexGridSizer114->Add(m_SensorAction4, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_Notebook->AddPage(m_RoutesPanel, _("Routes"));

    itemBoxSizer2->Add(m_Notebook, 1, wxGROW|wxALL, 5);

    wxStdDialogButtonSizer* itemStdDialogButtonSizer125 = new wxStdDialogButtonSizer;

    itemBoxSizer2->Add(itemStdDialogButtonSizer125, 0, wxGROW|wxALL, 5);
    m_OK = new wxButton( itemDialog1, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    m_OK->SetDefault();
    itemStdDialogButtonSizer125->AddButton(m_OK);

    m_Cancel = new wxButton( itemDialog1, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer125->AddButton(m_Cancel);

    itemStdDialogButtonSizer125->Realize();

////@end SelTabDialog content construction
}


/*!
 * wxEVT_GRID_CELL_LEFT_CLICK event handler for ID_SELTAB_GRID
 */

void SelTabDialog::OnCellLeftClick( wxGridEvent& event )
{
  m_TracksGrid->SelectRow(event.GetRow());
  m_DelTrack->Enable( true );
  m_ModifyTrack->Enable( true );

  wxString str = m_TracksGrid->GetCellValue( event.GetRow(), 0 );
  m_TrackNr = event.GetRow();
}


/*!
 * wxEVT_GRID_CELL_RIGHT_CLICK event handler for ID_SELTAB_GRID
 */

void SelTabDialog::OnCellRightClick( wxGridEvent& event )
{
  OnCellLeftClick( event );
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_SELTAB_ADDTRACK
 */

void SelTabDialog::OnButtonSeltabAddtrackClick( wxCommandEvent& event )
{
  TraceOp.trc( "seltab", TRCLEVEL_INFO, __LINE__, 9999, "New for track" );

  iONode track = NodeOp.inst( wSelTabPos.name(), NULL, ELEMENT_NODE );

  SelTabTrackDialog* dlg = new SelTabTrackDialog( this, track );
  if( wxID_OK == dlg->ShowModal() ) {
    NodeOp.addChild( m_Props, track );
    initValues();
  }
  else
    NodeOp.base.del( track );
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_SELTAB_DELTRACK
 */

void SelTabDialog::OnButtonSeltabDeltrackClick( wxCommandEvent& event )
{
  int idx = 0;
  iONode track = wSelTab.getseltabpos( m_Props );
  while( track != NULL ) {
    if( m_TrackNr == idx )
      break;
    idx++;
    track = wSelTab.nextseltabpos( m_Props, track );
  }

  if( track != NULL ) {
    NodeOp.removeChild( m_Props, track );
    NodeOp.base.del( track );
    initValues();
  }
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_SELTAB_MODIFYTRACK
 */

void SelTabDialog::OnButtonSeltabModifytrackClick( wxCommandEvent& event )
{
  if( m_Props == NULL )
    return;

  TraceOp.trc( "seltab", TRCLEVEL_INFO, __LINE__, 9999, "Props for track[%d]", m_TrackNr );

  int idx = 0;
  iONode track = wSelTab.getseltabpos( m_Props );
  while( track != NULL ) {
    if( m_TrackNr == idx )
      break;
    idx++;
    track = wSelTab.nextseltabpos( m_Props, track );
  }

  if( track != NULL ) {
    SelTabTrackDialog* dlg = new SelTabTrackDialog( this, track );
    if( wxID_OK == dlg->ShowModal() ) {
      initValues();
    }
  }

}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
 */

void SelTabDialog::OnOkClick( wxCommandEvent& event )
{
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
  EndModal( wxID_OK );
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
 */

void SelTabDialog::OnCancelClick( wxCommandEvent& event )
{
  EndModal( 0 );
}


/*!
 * Should we show tooltips?
 */

bool SelTabDialog::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap SelTabDialog::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin SelTabDialog bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end SelTabDialog bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon SelTabDialog::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin SelTabDialog icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end SelTabDialog icon retrieval
}


/*!
 * wxEVT_COMMAND_COMBOBOX_SELECTED event handler for ID_SELTAB_TYPE
 */

void SelTabDialog::OnSeltabTypeSelected( wxCommandEvent& event )
{
  bool locoio = m_Type->GetSelection() == 3 ? true:false;

  m_Address2->Enable(!locoio);
  m_Address3->Enable(!locoio);
  m_Address4->Enable(!locoio);
  m_Port0->Enable(!locoio);
  m_Port1->Enable(!locoio);
  m_Port2->Enable(!locoio);
  m_Port3->Enable(!locoio);
  m_Port4->Enable(!locoio);
  m_b0Sen->Enable(!locoio);
  m_b1Sen->Enable(!locoio);
  m_b2Sen->Enable(!locoio);
  m_b3Sen->Enable(!locoio);
}




/*!
 * wxEVT_COMMAND_LISTBOX_SELECTED event handler for ID_LISTBOX_SELTAB_ROUTES
 */

void SelTabDialog::OnListboxSeltabRoutesSelected( wxCommandEvent& event )
{
  initSensors();
}


/*!
 * wxEVT_COMMAND_COMBOBOX_SELECTED event handler for wxID_ANY
 */

void SelTabDialog::OnBlockSensor( wxCommandEvent& event )
{
  evaluateSensors();
}


/*!
 * wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_FY_SHAREDFB
 */

void SelTabDialog::OnFySharedfbClick( wxCommandEvent& event )
{
  m_RoutesPanel->Enable( m_SharedFB->IsChecked() );
}

