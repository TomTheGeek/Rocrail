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
#include "cardlg.h"

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/defs.h"
#endif

#include "rocview/public/guiapp.h"
#include "rocrail/wrapper/public/ModelCmd.h"
#include "rocrail/wrapper/public/Plan.h"
#include "rocrail/wrapper/public/Car.h"
#include "rocrail/wrapper/public/Item.h"
#include "rocrail/wrapper/public/CarList.h"
#include "rocrail/wrapper/public/Location.h"
#include "rocrail/wrapper/public/LocationList.h"
#include "rocrail/wrapper/public/Block.h"

#include "rocview/wrapper/public/Gui.h"

#include "rocs/public/trace.h"
#include "rocs/public/strtok.h"
#include "rocs/public/system.h"

#include "rocview/xpm/nopict.xpm"

CarDlg::CarDlg( wxWindow* parent, iONode p_Props, bool save )
  :cardlggen( parent )
{
  TraceOp.trc( "cardlg", TRCLEVEL_INFO, __LINE__, 9999, "cardlg" );
  m_TabAlign = wxGetApp().getTabAlign();
  m_Props    = p_Props;
  m_bSave    = save;
  initLabels();
  initIndex();

  m_IndexPanel->GetSizer()->Layout();
  m_GeneralPanel->GetSizer()->Layout();
  m_DetailsPanel->GetSizer()->Layout();

  m_CarBook->Fit();

  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);

  m_CarList->SetFocus();

  m_CarBook->Connect( wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( CarDlg::onSetPage ), NULL, this );
  m_SetPage = 0;

  if( m_Props != NULL ) {
    initValues();
    m_SetPage = 1;
  }
  wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, ID_CARBOOK );
  wxPostEvent( m_CarBook, event );
}

void CarDlg::onSetPage(wxCommandEvent& event) {
  TraceOp.trc( "cardlg", TRCLEVEL_INFO, __LINE__, 9999, "set page to %d", m_SetPage );
  m_CarBook->SetSelection( m_SetPage );
}


static int __sortStr(obj* _a, obj* _b)
{
    const char* a = (const char*)*_a;
    const char* b = (const char*)*_b;
    return strcmp( a, b );
}

void CarDlg::initLocationCombo() {
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
        m_Location->Append( wxString(id,wxConvUTF8) );
      }
    }
  }
  /* clean up the temp. list */
  ListOp.base.del(list);
}

void CarDlg::initLabels() {
  TraceOp.trc( "cardlg", TRCLEVEL_INFO, __LINE__, 9999, "initLabels" );
  SetTitle(wxGetApp().getMsg( "cartable" ));
  m_CarBook->SetPageText( 0, wxGetApp().getMsg( "index" ) );
  m_CarBook->SetPageText( 1, wxGetApp().getMsg( "general" ) );
  m_CarBook->SetPageText( 2, wxGetApp().getMsg( "details" ) );

  // Index
  m_NewCar->SetLabel( wxGetApp().getMsg( "new" ) );
  m_DeleteCar->SetLabel( wxGetApp().getMsg( "delete" ) );
  m_Copy->SetLabel( wxGetApp().getMsg( "copy" ) );

  // General
  m_labID->SetLabel( wxGetApp().getMsg( "id" ) );
  m_labIdent->SetLabel( wxGetApp().getMsg( "identifier" ) );
  m_labNumber->SetLabel( wxGetApp().getMsg( "number" ) );
  m_labRoadname->SetLabel( wxGetApp().getMsg( "roadname" ) );
  m_labColor->SetLabel( wxGetApp().getMsg( "color" ) );
  m_Era->SetLabel( wxGetApp().getMsg( "era" ) );
  m_labLocation->SetLabel( wxGetApp().getMsg( "location" ) );
  m_labImageName->SetLabel( wxGetApp().getMsg( "image" ) );
  m_labOwner->SetLabel( wxGetApp().getMsg( "owner" ) );

  m_Status->SetLabel( wxGetApp().getMsg( "status" ) );
  m_Status->SetString( 0, wxGetApp().getMsg( "empty" ) );
  m_Status->SetString( 1, wxGetApp().getMsg( "loaded" ) );
  m_Status->SetString( 2, wxGetApp().getMsg( "maintenance" ) );

  initLocationCombo();


  // Details
  m_Type->SetLabel( wxGetApp().getMsg( "type" ) );
  m_Type->SetString( 0, wxGetApp().getMsg( "goods" ) );
  m_Type->SetString( 1, wxGetApp().getMsg( "passengers" ) );
  m_labSubtype->SetLabel( wxGetApp().getMsg( "subtype" ) );
  m_labLength->SetLabel( wxGetApp().getMsg( "length" ) );
  m_labRemark->SetLabel( wxGetApp().getMsg( "remark" ) );

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


void CarDlg::initIndex(){
  TraceOp.trc( "cardlg", TRCLEVEL_INFO, __LINE__, 9999, "initIndex" );
  iONode l_Props = m_Props;

  SetTitle(wxGetApp().getMsg( "cartable" ));

  m_CarList->Clear();

  iONode model = wxGetApp().getModel();
  if( model != NULL ) {
    iONode carlist = wPlan.getcarlist( model );
    if( carlist != NULL ) {
      iOList list = ListOp.inst();
      int cnt = NodeOp.getChildCnt( carlist );
      for( int i = 0; i < cnt; i++ ) {
        iONode car = NodeOp.getChild( carlist, i );
        const char* id = wCar.getid( car );
        if( id != NULL ) {
          ListOp.add(list, (obj)car);
        }
      }

      ListOp.sort(list, &__sortID);
      cnt = ListOp.size( list );
      for( int i = 0; i < cnt; i++ ) {
        iONode car = (iONode)ListOp.get( list, i );
        const char* id = wCar.getid( car );
        m_CarList->Append( wxString(id,wxConvUTF8), car );
      }
      /* clean up the temp. list */
      ListOp.base.del(list);

      if( l_Props != NULL ) {
        m_CarList->SetStringSelection( wxString(wCar.getid( l_Props ),wxConvUTF8) );
        m_CarList->SetFirstItem( wxString(wCar.getid( l_Props ),wxConvUTF8) );
        m_Props = l_Props;
        char* title = StrOp.fmt( "%s %s", (const char*)wxGetApp().getMsg("car").mb_str(wxConvUTF8), wCar.getid( m_Props ) );
        SetTitle( wxString(title,wxConvUTF8) );
        StrOp.free( title );
      }
      else
        TraceOp.trc( "cardlg", TRCLEVEL_INFO, __LINE__, 9999, "no selection" );
    }

  }

}


void CarDlg::initValues() {
  if( m_Props == NULL ) {
    TraceOp.trc( "cardlg", TRCLEVEL_DEBUG, __LINE__, 9999, "no car selected" );
    return;
  }

  char* title = StrOp.fmt( "%s %s", (const char*)wxGetApp().getMsg("car").mb_str(wxConvUTF8), wCar.getid( m_Props ) );
  SetTitle( wxString(title,wxConvUTF8) );
  StrOp.free( title );

  TraceOp.trc( "cardlg", TRCLEVEL_INFO, __LINE__, 9999, "initValues for car [%s]", wCar.getid( m_Props ) );
  // Init General
  if( wCar.getimage( m_Props ) != NULL ) {
    wxBitmapType bmptype = wxBITMAP_TYPE_XPM;
    if( StrOp.endsWithi( wCar.getimage( m_Props ), ".gif" ) )
      bmptype = wxBITMAP_TYPE_GIF;
    else if( StrOp.endsWithi( wCar.getimage( m_Props ), ".png" ) )
      bmptype = wxBITMAP_TYPE_PNG;

    const char* imagepath = wGui.getimagepath(wxGetApp().getIni());
    static char pixpath[256];
    StrOp.fmtb( pixpath, "%s%c%s", imagepath, SystemOp.getFileSeparator(), FileOp.ripPath( wCar.getimage( m_Props ) ) );

    if( FileOp.exist(pixpath)) {
      TraceOp.trc( "cardlg", TRCLEVEL_INFO, __LINE__, 9999, "picture [%s]", pixpath );
      m_CarImage->SetBitmapLabel( wxBitmap(wxString(pixpath,wxConvUTF8), bmptype) );
    }
    else {
      TraceOp.trc( "cardlg", TRCLEVEL_WARNING, __LINE__, 9999, "picture [%s] not found", pixpath );
      m_CarImage->SetBitmapLabel( wxBitmap(nopict_xpm) );
    }
    m_CarImage->SetToolTip(wxString(wCar.getnumber( m_Props ),wxConvUTF8));


    //m_CarImage->SetBitmapLabel( wxBitmap(wxString(wLoc.getimage( m_Props ),wxConvUTF8), bmptype) );
    //m_CarImageIndex->SetBitmapLabel( wxBitmap(wxString(wLoc.getimage( m_Props ),wxConvUTF8), bmptype) );
  }
  else {
    m_CarImage->SetBitmapLabel( wxBitmap(nopict_xpm) );
    //m_CarImageIndex->SetBitmapLabel( wxBitmap(nopict_xpm) );
  }
  m_CarImage->Refresh();
  //m_CarImageIndex->Refresh();

  // init General
  m_ID->SetValue( wxString(wCar.getid( m_Props ),wxConvUTF8) );
  m_Ident->SetValue( wxString::Format(_T("%ld"), wCar.getident( m_Props )) );
  m_Number->SetValue( wxString(wCar.getnumber( m_Props ),wxConvUTF8) );
  m_Color->SetValue( wxString(wCar.getcolor( m_Props ),wxConvUTF8) );
  m_Roadname->SetValue( wxString(wCar.getroadname( m_Props ),wxConvUTF8) );
  m_ImageName->SetValue( wxString(wCar.getimage( m_Props ),wxConvUTF8) );
  m_Era->SetSelection( wCar.getera( m_Props ) );
  m_Location->SetStringSelection(wxString(wCar.getlocation(m_Props),wxConvUTF8));
  m_Owner->SetValue( wxString(wCar.getowner( m_Props ),wxConvUTF8) );

  // init Status
  if( StrOp.equals( wCar.status_empty, wCar.getstatus( m_Props) ) )
    m_Status->SetSelection(0);
  else if( StrOp.equals( wCar.status_loaded, wCar.getstatus( m_Props) ) )
    m_Status->SetSelection(1);
  else
    m_Status->SetSelection(2);

  // init Details
  if( StrOp.equals( wCar.cartype_freight, wCar.gettype( m_Props) ) )
    m_Type->SetSelection(0);
  else
    m_Type->SetSelection(1);

  initSubType();

  m_Length->SetValue( wCar.getlen( m_Props ) );
  m_Remark->SetValue( wxString(wCar.getremark( m_Props ),wxConvUTF8) );

}



bool CarDlg::evaluate(){
  if( m_Props == NULL )
    return false;

  TraceOp.trc( "cardlg", TRCLEVEL_INFO, __LINE__, 9999, "Evaluate %s", wCar.getid( m_Props ) );

  if( m_ID->GetValue().Len() == 0 ) {
    wxMessageDialog( this, wxGetApp().getMsg("invalidid"), _T("Rocrail"), wxOK | wxICON_ERROR ).ShowModal();
    m_ID->SetValue( wxString(wCar.getid( m_Props ),wxConvUTF8) );
    return false;
  }
  // evaluate General
  wItem.setprev_id( m_Props, wItem.getid(m_Props) );
  wCar.setid( m_Props, m_ID->GetValue().mb_str(wxConvUTF8) );
  long ident = 0;
  m_Ident->GetValue().ToLong(&ident);
  wCar.setident( m_Props, ident );
  wCar.setnumber( m_Props, m_Number->GetValue().mb_str(wxConvUTF8) );
  wCar.setcolor( m_Props, m_Color->GetValue().mb_str(wxConvUTF8) );
  wCar.setroadname( m_Props, m_Roadname->GetValue().mb_str(wxConvUTF8) );
  wCar.setimage( m_Props, m_ImageName->GetValue().mb_str(wxConvUTF8) );
  wCar.setera( m_Props, m_Era->GetSelection() );
  wCar.setlocation(m_Props, m_Location->GetStringSelection().mb_str(wxConvUTF8));
  wCar.setowner( m_Props, m_Owner->GetValue().mb_str(wxConvUTF8) );


  if( m_Status->GetSelection() == 0 )
    wCar.setstatus( m_Props, wCar.status_empty );
  else if( m_Status->GetSelection() == 1 )
    wCar.setstatus( m_Props, wCar.status_loaded );
  else
    wCar.setstatus( m_Props, wCar.status_maintenance );

  // evaluate Details
  if( m_Type->GetSelection() == 0 )
    wCar.settype( m_Props, wCar.cartype_freight );
  else
    wCar.settype( m_Props, wCar.cartype_passenger );

  wCar.setsubtype( m_Props, (char*)m_SubType->GetClientData( m_SubType->GetSelection()) );
  wCar.setlen( m_Props, m_Length->GetValue() );
  wCar.setremark( m_Props, m_Remark->GetValue().mb_str(wxConvUTF8) );

  return true;
}


void CarDlg::onCarImage( wxCommandEvent& event ){
  const char* imagepath = wGui.getimagepath( wxGetApp().getIni() );
  TraceOp.trc( "cardlg", TRCLEVEL_INFO, __LINE__, 9999, "imagepath = [%s]", imagepath );
  wxFileDialog* fdlg = new wxFileDialog(this, _T("Search car image"),
      wxString(imagepath,wxConvUTF8), _T(""),
      _T("PNG files (*.png)|*.png|GIF files (*.gif)|*.gif|XPM files (*.xpm)|*.xpm"), wxFD_OPEN);
  if( fdlg->ShowModal() == wxID_OK ) {
    TraceOp.trc( "cardlg", TRCLEVEL_INFO, __LINE__, 9999, "Loading %s...", (const char*)fdlg->GetPath().mb_str(wxConvUTF8) );
    wxBitmapType bmptype = wxBITMAP_TYPE_XPM;
    if( StrOp.endsWithi( fdlg->GetPath().mb_str(wxConvUTF8), ".gif" ) )
      bmptype = wxBITMAP_TYPE_GIF;
    else if( StrOp.endsWithi( fdlg->GetPath().mb_str(wxConvUTF8), ".png" ) )
      bmptype = wxBITMAP_TYPE_PNG;
    m_CarImage->SetBitmapLabel( wxBitmap( fdlg->GetPath(), bmptype ) );
    m_CarImage->Refresh();
    wCar.setimage( m_Props, FileOp.ripPath(fdlg->GetPath().mb_str(wxConvUTF8)) );
    m_ImageName->SetValue( wxString(wCar.getimage( m_Props ),wxConvUTF8) );
  }
}


void CarDlg::onCarList( wxCommandEvent& event ){
  if( m_CarList->GetSelection() != wxNOT_FOUND ) {
    m_Props = (iONode)m_CarList->GetClientData(m_CarList->GetSelection());
    if( m_Props != NULL )
      initValues();
    else
      TraceOp.trc( "cardlg", TRCLEVEL_INFO, __LINE__, 9999, "no selection..." );
  }
}

iONode CarDlg::getSelectedCar() {
  return m_Props;
}

void CarDlg::onNewCar( wxCommandEvent& event ){
  int i = m_CarList->FindString( _T("NEW") );
  if( i == wxNOT_FOUND ) {
    m_CarList->Append( _T("NEW") );
    iONode model = wxGetApp().getModel();
    if( model != NULL ) {
      iONode carlist = wPlan.getcarlist( model );
      if( carlist == NULL ) {
        carlist = NodeOp.inst( wCarList.name(), model, ELEMENT_NODE );
        NodeOp.addChild( model, carlist );
      }
      if( carlist != NULL ) {
        iONode car = NodeOp.inst( wCar.name(), carlist, ELEMENT_NODE );
        NodeOp.addChild( carlist, car );
        wCar.setid( car, "NEW" );
        m_Props = car;
        initValues();
      }
    }
  }
  m_CarList->SetStringSelection( _T("NEW") );
  m_CarList->SetFirstItem( _T("NEW") );
}


void CarDlg::onDeleteCar( wxCommandEvent& event ){
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
    iONode carlist = wPlan.getcarlist( model );
    if( carlist != NULL ) {
      NodeOp.removeChild( carlist, m_Props );
      m_Props = NULL;
    }
  }

  initIndex();
}


void CarDlg::OnCopy( wxCommandEvent& event ){
  if( m_Props != NULL ) {
    iONode model = wxGetApp().getModel();
    if( model != NULL ) {
      iONode carlist = wPlan.getcarlist( model );
      if( carlist == NULL ) {
        carlist = NodeOp.inst( wCarList.name(), model, ELEMENT_NODE );
        NodeOp.addChild( model, carlist );
      }

      if( carlist != NULL ) {
        iONode lccopy = (iONode)NodeOp.base.clone( m_Props );
        char* id = StrOp.fmt( "%s (copy)", wCar.getid(lccopy));
        wCar.setid(lccopy, id);
        StrOp.free(id);
        NodeOp.addChild( carlist, lccopy );
        initIndex();
      }

    }
  }
}


void CarDlg::onImport( wxCommandEvent& event ){
  wxString ms_FileExt = wxGetApp().getMsg("planfiles");
  const char* l_openpath = wGui.getopenpath( wxGetApp().getIni() );
  wxFileDialog* fdlg = new wxFileDialog(this, wxGetApp().getMenu("openplanfile"), wxString(l_openpath,wxConvUTF8) , _T(""), ms_FileExt, wxFD_OPEN);
  if( fdlg->ShowModal() == wxID_OK ) {

    wGui.setopenpath( wxGetApp().getIni(), fdlg->GetPath().mb_str(wxConvUTF8) );
    // strip filename:
    wGui.setopenpath( wxGetApp().getIni(), FileOp.getPath(wGui.getopenpath( wxGetApp().getIni() ) ) );

    if( fdlg->GetPath().Len() > 0 && FileOp.exist(fdlg->GetPath().mb_str(wxConvUTF8)) ) {
      iOFile f = FileOp.inst( fdlg->GetPath().mb_str(wxConvUTF8), OPEN_READONLY );
      char* buffer = (char*)allocMem( FileOp.size( f ) +1 );
      FileOp.read( f, buffer, FileOp.size( f ) );
      FileOp.base.del( f );
      iODoc doc = DocOp.parse( buffer );
      if( doc != NULL ) {
        iONode plan = DocOp.getRootNode( doc );
        DocOp.base.del( doc );
        TraceOp.trc( "cardlg", TRCLEVEL_INFO, __LINE__, 9999, "Plan [%s] is successfully parsed!", (const char*)fdlg->GetPath().mb_str(wxConvUTF8) );

        /* TODO: read all loco's and add them to the list */
        iONode list = NodeOp.findNode( plan, wCarList.name() );

        if( list != NULL ) {
          int i = 0;
          int cnt = NodeOp.getChildCnt( list );
          TraceOp.trc( "cardlg", TRCLEVEL_INFO, __LINE__, 9999, "%d cars in list", cnt );
          for( i = 0; i < cnt; i++ ) {
            m_Props = (iONode)NodeOp.base.clone( NodeOp.getChild( list, i ) );

            iONode model = wxGetApp().getModel();
            if( model != NULL ) {
              iONode carlist = wPlan.getcarlist( model );
              if( carlist == NULL ) {
                carlist = NodeOp.inst( wCarList.name(), model, ELEMENT_NODE );
                NodeOp.addChild( model, carlist );
              }
              if( carlist != NULL ) {
                iONode car = NodeOp.inst( wCar.name(), carlist, ELEMENT_NODE );
                NodeOp.addChild( carlist, m_Props );
                initValues();
                onApply(event);
              }
            }
          }
        }
        else {
          TraceOp.trc( "cardlg", TRCLEVEL_INFO, __LINE__, 9999, "No cars found in %s", (const char*)fdlg->GetPath().mb_str(wxConvUTF8) );
        }


        NodeOp.base.del( plan );
      }
      else {
        TraceOp.trc( "cardlg", TRCLEVEL_EXCEPTION, __LINE__, 9999, "Plan [%s] is not parseable!", (const char*)fdlg->GetPath().mb_str(wxConvUTF8) );
      }
    }

  }
  fdlg->Destroy();
}



void CarDlg::initSubType(){
  m_SubType->Clear();

  if( m_Type->GetSelection() == 0 ) {
    m_SubType->Append( wxGetApp().getMsg( wCar.freight_boxcar ), (void*)wCar.freight_boxcar );
    m_SubType->Append( wxGetApp().getMsg( wCar.freight_gondola ), (void*)wCar.freight_gondola );
    m_SubType->Append( wxGetApp().getMsg( wCar.freight_flatcar ), (void*)wCar.freight_flatcar );
    m_SubType->Append( wxGetApp().getMsg( wCar.freight_reefer ), (void*)wCar.freight_reefer );
    m_SubType->Append( wxGetApp().getMsg( wCar.freight_stockcar ), (void*)wCar.freight_stockcar );
    m_SubType->Append( wxGetApp().getMsg( wCar.freight_tankcar ), (void*)wCar.freight_tankcar );
    m_SubType->Append( wxGetApp().getMsg( wCar.freight_wellcar ), (void*)wCar.freight_wellcar );
    m_SubType->Append( wxGetApp().getMsg( wCar.freight_hopper ), (void*)wCar.freight_hopper );
    m_SubType->Append( wxGetApp().getMsg( wCar.freight_caboose ), (void*)wCar.freight_caboose );
    m_SubType->Append( wxGetApp().getMsg( wCar.freight_autorack ), (void*)wCar.freight_autorack );
    m_SubType->Append( wxGetApp().getMsg( wCar.freight_autocarrier ), (void*)wCar.freight_autocarrier );
    m_SubType->Append( wxGetApp().getMsg( wCar.freight_logdumpcar ), (void*)wCar.freight_logdumpcar );
    m_SubType->Append( wxGetApp().getMsg( wCar.freight_coilcar ), (void*)wCar.freight_coilcar );
  }
  else {
    m_SubType->Append( wxGetApp().getMsg( wCar.passenger_coach ), (void*)wCar.passenger_coach );
    m_SubType->Append( wxGetApp().getMsg( wCar.passenger_lounge ), (void*)wCar.passenger_lounge );
    m_SubType->Append( wxGetApp().getMsg( wCar.passenger_dome ), (void*)wCar.passenger_dome );
    m_SubType->Append( wxGetApp().getMsg( wCar.passenger_express ), (void*)wCar.passenger_express );
    m_SubType->Append( wxGetApp().getMsg( wCar.passenger_dinner ), (void*)wCar.passenger_dinner );
    m_SubType->Append( wxGetApp().getMsg( wCar.passenger_sleeper ), (void*)wCar.passenger_sleeper );
    m_SubType->Append( wxGetApp().getMsg( wCar.passenger_baggage ), (void*)wCar.passenger_baggage );
    m_SubType->Append( wxGetApp().getMsg( wCar.passenger_postoffice ), (void*)wCar.passenger_postoffice );
  }
  if( StrOp.len( wCar.getsubtype( m_Props ) ) > 0 )
    m_SubType->SetStringSelection( wxGetApp().getMsg( wCar.getsubtype( m_Props ) ) );
}


void CarDlg::onTypeSelect( wxCommandEvent& event ){
  initSubType();
}


void CarDlg::onApply( wxCommandEvent& event ){
  if( m_Props == NULL || !m_bSave )
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


void CarDlg::onCancel( wxCommandEvent& event ){
  EndModal( 0 );
}


void CarDlg::onOK( wxCommandEvent& event ){
  if( m_bSave )
    onApply(event);
  EndModal( wxID_OK );
}

