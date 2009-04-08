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
// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------
// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
    #include "wx/event.h"
#endif

#ifdef __linux__
#include <sys/resource.h>
#endif

#include <wx/image.h>
#include <wx/filedlg.h>
#include <wx/splash.h>
#include <wx/html/helpctrl.h>
#include <wx/help.h>
#include <wx/cshelp.h>
#include <wx/colordlg.h>

#include "rocs/public/str.h"
#include "rocs/public/file.h"
#include "rocs/public/doc.h"
#include "rocs/public/node.h"
#include "rocs/public/list.h"
#include "rocs/public/mem.h"
#include "rocs/public/res.h"
#include "rocs/public/wrpinf.h"
#include "rocs/public/system.h"
#include "rocs/public/strtok.h"

#include "rocgui/dialogs/locdialog.h"
#include "rocgui/dialogs/cardlg.h"
#include "rocgui/dialogs/waybilldlg.h"
#include "rocgui/dialogs/operatordlg.h"
#include "rocgui/dialogs/locseldlg.h"
#include "rocgui/dialogs/loccontroldlg.h"
#include "rocgui/dialogs/switchdialog.h"
#include "rocgui/dialogs/signaldialog.h"
#include "rocgui/dialogs/scheduledialog.h"
#include "rocgui/dialogs/locationsdlg.h"
#include "rocgui/dialogs/outputdialog.h"
#include "rocgui/dialogs/feedbackdialog.h"
#include "rocgui/dialogs/routedialog.h"
#include "rocgui/dialogs/blockdialog.h"
#include "rocgui/dialogs/blockgroupingdlg.h"
#include "rocgui/dialogs/planpaneldlg.h"
#include "rocgui/dialogs/connectiondialog.h"
#include "rocgui/dialogs/rocrailinidialog.h"
#include "rocgui/dialogs/rocguiinidlg.h"
#include "rocgui/dialogs/infodialog.h"
#include "rocgui/dialogs/undodlg.h"
#include "rocgui/dialogs/turntabledialog.h"
#include "rocgui/dialogs/softwareupdates.h"
#include "rocgui/dialogs/mvtrackdlg.h"
#include "rocgui/dialogs/timedactions.h"

#include "rocgui/dialogs/decoders/locoio.h"
#include "rocgui/dialogs/decoders/dtopswdlg.h"
#include "rocgui/dialogs/decoders/uhl633x0dlg.h"

#include "rocgui/public/guiapp.h"
#include "rocgui/public/swdlg.h"
#include "rocgui/public/routedlg.h"
#include "rocgui/public/item.h"
#include "rocgui/public/jssupport.h"

#include "rocgui/wrapper/public/Gui.h"
#include "rocgui/wrapper/public/RRCon.h"
#include "rocgui/wrapper/public/ModPanel.h"
#include "rocgui/wrapper/public/PlanPanel.h"
#include "rocgui/wrapper/public/SplitPanel.h"
#include "rocgui/wrapper/public/Window.h"
#include "rocgui/wrapper/public/Cmdline.h"
#include "rocgui/wrapper/public/Theme.h"
#include "rocgui/wrapper/public/ThemePanel.h"
#include "rocgui/wrapper/public/ThemeBlock.h"
#include "rocgui/wrapper/public/LcCtrl.h"

#include "rocrail/wrapper/public/JsEvent.h"
#include "rocrail/wrapper/public/JsMap.h"
#include "rocrail/wrapper/public/Plan.h"
#include "rocrail/wrapper/public/TrackList.h"
#include "rocrail/wrapper/public/RouteList.h"
#include "rocrail/wrapper/public/BlockList.h"
#include "rocrail/wrapper/public/FeedbackList.h"
#include "rocrail/wrapper/public/SwitchList.h"
#include "rocrail/wrapper/public/TurntableList.h"
#include "rocrail/wrapper/public/LocList.h"
#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/Exception.h"
#include "rocrail/wrapper/public/Response.h"
#include "rocrail/wrapper/public/Block.h"
#include "rocrail/wrapper/public/Schedule.h"
#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/Car.h"
#include "rocrail/wrapper/public/Waybill.h"
#include "rocrail/wrapper/public/FunCmd.h"
#include "rocrail/wrapper/public/SysCmd.h"
#include "rocrail/wrapper/public/ModelCmd.h"
#include "rocrail/wrapper/public/AutoCmd.h"
#include "rocrail/wrapper/public/ZLevel.h"
#include "rocrail/wrapper/public/RocRail.h"
#include "rocrail/wrapper/public/State.h"
#include "rocrail/wrapper/public/Program.h"
#include "rocrail/wrapper/public/DataReq.h"
#include "rocrail/wrapper/public/MVTrack.h"

#include "rocgui/symbols/svg.h"
// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------
// the application icon

#include "rocgui/xpm/rocrail.xpm"
#include "rocgui/xpm/nopict.xpm"

static void myjsListener( int device, int type, int number, int value, unsigned long msec );

enum {
  CONNECT_EVENT = 201,
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWindows
// ----------------------------------------------------------------------------

// the event tables connect the wxWindows events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(RocGuiFrame, wxFrame)
    //EVT_NOTEBOOK_PAGE_CHANGING(-1, RocGuiFrame::OnPageChange)
    EVT_COMBOBOX(ID_SCALE_COMBO, RocGuiFrame::OnScaleCombo)
    EVT_TEXT_ENTER(ID_SCALE_COMBO, RocGuiFrame::OnScaleCombo)
    EVT_TEXT(ID_SCALE_COMBO, RocGuiFrame::OnScaleComboCheck)

    EVT_NOTEBOOK_PAGE_CHANGED(-1, RocGuiFrame::OnPageChange)
    EVT_BUTTON(-1, RocGuiFrame::OnButton)
    EVT_CHECKBOX( -1, RocGuiFrame::OnButton )
    EVT_TEXT( VAL_GENERIC, RocGuiFrame::OnButton )
    EVT_COMBOBOX( ID_COMBOBOX_LOCLIST, RocGuiFrame::OnButton )
    EVT_TIMER (ME_CVTimer, RocGuiFrame::OnTimer)

    EVT_TOGGLEBUTTON(-1, RocGuiFrame::OnButton)
    //EVT_SLIDER( -1, RocGuiFrame::OnSliderCmd )
    EVT_COMMAND_SCROLL( -1, RocGuiFrame::OnSlider )
    //EVT_COMMAND_SCROLL_ENDSCROLL( -1, RocGuiFrame::OnSlider )
    EVT_COMMAND_SCROLL_THUMBRELEASE( -1, RocGuiFrame::OnSlider )

    EVT_CLOSE(RocGuiFrame::OnClose)
    EVT_SIZE(RocGuiFrame::OnSize)
    EVT_MOUSE_EVENTS(RocGuiFrame::OnMouse)

    EVT_MENU_OPEN(RocGuiFrame::OnMenu)

    EVT_MENU( SHOWLOGO_EVENT, RocGuiFrame::OnLogo)
    EVT_MENU( NEWUPDATES_EVENT, RocGuiFrame::OnNewUpdates)

    EVT_MENU( ADDEXCEPTION_EVENT, RocGuiFrame::OnAddException)
    EVT_MENU( LOC_EVENT, RocGuiFrame::OnLocEvent)
    EVT_MENU( AUTO_EVENT, RocGuiFrame::OnAutoEvent)
    EVT_MENU( SYSTEM_EVENT, RocGuiFrame::OnSystemEvent)
    EVT_MENU( STATE_EVENT, RocGuiFrame::OnStateEvent)
    EVT_MENU( ZLEVEL_EVENT_ADD, RocGuiFrame::OnZLevelEventAdd)
    EVT_MENU( ZLEVEL_EVENT_REMOVE, RocGuiFrame::OnZLevelEventRemove)
    EVT_MENU( INIT_NOTEBOOK, RocGuiFrame::OnInitNotebook)
    EVT_MENU( UPDATE_ACTIVELOCS_EVENT, RocGuiFrame::UpdateActiveLocs)
    EVT_MENU( UPDATE_LOC_IMAGE_EVENT, RocGuiFrame::UpdateLocImage)
    EVT_MENU( CV_EVENT, RocGuiFrame::CVevent)

    EVT_MENU( ME_Quit           , RocGuiFrame::OnQuit)
    EVT_MENU( ME_Save           , RocGuiFrame::OnSave)
    EVT_MENU( ME_SaveAs         , RocGuiFrame::OnSaveAs)
    EVT_MENU( ME_Open           , RocGuiFrame::OnOpen)
    EVT_MENU( ME_New            , RocGuiFrame::OnNew)
    EVT_MENU( ME_Upload         , RocGuiFrame::OnUpload)
    EVT_MENU( ME_About          , RocGuiFrame::OnAbout)
    EVT_MENU( ME_Update         , RocGuiFrame::OnUpdate)
    EVT_MENU( ME_Help           , RocGuiFrame::OnHelp)
    EVT_MENU( ME_RUG            , RocGuiFrame::OnRUG)
    EVT_MENU( ME_Bug            , RocGuiFrame::OnBug)
    EVT_MENU( ME_Feature        , RocGuiFrame::OnFeature)
    EVT_MENU( ME_Survey         , RocGuiFrame::OnService)
    EVT_MENU( ME_MIC            , RocGuiFrame::OnMIC)
    EVT_MENU( ME_LcDlg          , RocGuiFrame::OnLcDlg)
    EVT_MENU( ME_OperatorDlg    , RocGuiFrame::OnOperatorDlg)
    EVT_MENU( ME_SwDlg          , RocGuiFrame::OnSwDlg)
    EVT_MENU( ME_RouteDlg       , RocGuiFrame::OnRouteDlg)
    EVT_MENU( ME_Connect        , RocGuiFrame::OnConnect)
    EVT_MENU( ME_RocrailIni     , RocGuiFrame::OnRocrailIni)
    EVT_MENU( ME_RocguiIni      , RocGuiFrame::OnRocguiIni)
    EVT_MENU( ME_PlanTitle      , RocGuiFrame::OnPlanTitle)
    EVT_MENU( ME_Go             , RocGuiFrame::OnGo)
    EVT_MENU( ME_Stop           , RocGuiFrame::OnStop)
    EVT_MENU( ME_EmergencyBreak , RocGuiFrame::OnEmergencyBreak)
    EVT_MENU( ME_AutoMode       , RocGuiFrame::OnAutoMode)
    EVT_MENU( ME_AutoGo         , RocGuiFrame::OnAutoGo)
    EVT_MENU( ME_AutoResume     , RocGuiFrame::OnAutoResume)
    EVT_MENU( ME_AutoStop       , RocGuiFrame::OnAutoStop)
    EVT_MENU( ME_AutoReset      , RocGuiFrame::OnAutoReset)
    EVT_MENU( ME_AutoSoftReset  , RocGuiFrame::OnAutoSoftReset)
    EVT_MENU( ME_ClearMsg       , RocGuiFrame::OnClearMsg)
    EVT_MENU( ME_ShutdownRocRail, RocGuiFrame::OnShutdownRocRail)
    EVT_MENU( ME_InitField      , RocGuiFrame::OnInitField)
    EVT_MENU( ME_TxShortIDs     , RocGuiFrame::OnTxShortIDs)
    EVT_MENU( ME_EditMode       , RocGuiFrame::OnEditMode)
    EVT_MENU( ME_CtrlMode       , RocGuiFrame::OnCtrlMode)
    EVT_MENU( ME_EditLocs       , RocGuiFrame::OnEditLocs)
    EVT_MENU( ME_EditCars       , RocGuiFrame::OnEditCars)
    EVT_MENU( ME_EditOperators  , RocGuiFrame::OnEditOperators)
    EVT_MENU( ME_EditWaybills   , RocGuiFrame::OnEditWaybills)
    EVT_MENU( ME_EditTurnouts   , RocGuiFrame::OnEditTurnouts)
    EVT_MENU( ME_EditTurntables , RocGuiFrame::OnEditTurntables)
    EVT_MENU( ME_EditSensors    , RocGuiFrame::OnEditSensors)
    EVT_MENU( ME_EditRoutes     , RocGuiFrame::OnEditRoutes)
    EVT_MENU( ME_EditBlocks     , RocGuiFrame::OnEditBlocks)
    EVT_MENU( ME_EditBlockGroups, RocGuiFrame::OnEditBlockGroups)
    EVT_MENU( ME_EditSignals    , RocGuiFrame::OnEditSignals)
    EVT_MENU( ME_EditOutputs    , RocGuiFrame::OnEditOutputs)
    EVT_MENU( ME_EditSchedules  , RocGuiFrame::OnEditSchedules)
    EVT_MENU( ME_EditLocations  , RocGuiFrame::OnEditLocations)
    EVT_MENU( ME_EditMVTrack    , RocGuiFrame::OnEditMVTrack)
    EVT_MENU( ME_EditTimedActions, RocGuiFrame::OnEditTimedActions)
    EVT_MENU( ME_PanelProps     , RocGuiFrame::OnPanelProps)
    EVT_MENU( ME_AddPanel       , RocGuiFrame::OnAddPanel)
    EVT_MENU( ME_Undo           , RocGuiFrame::OnUndo)
    EVT_MENU( ME_OpenDecoder    , RocGuiFrame::OnOpenDecoder)
    EVT_MENU( ME_Zoom25         , RocGuiFrame::OnZoom25)
    EVT_MENU( ME_Zoom50         , RocGuiFrame::OnZoom50)
    EVT_MENU( ME_Zoom75         , RocGuiFrame::OnZoom75)
    EVT_MENU( ME_Zoom100        , RocGuiFrame::OnZoom100)
    EVT_MENU( ME_LocoBook       , RocGuiFrame::OnLocoBook)
    EVT_MENU( ME_Fill           , RocGuiFrame::OnFill)
    EVT_MENU( ME_ShowID         , RocGuiFrame::OnShowID)
    EVT_MENU( ME_FullScreen     , RocGuiFrame::OnFullScreen)
    EVT_MENU( ME_Raster         , RocGuiFrame::OnRaster)
    EVT_MENU( ME_BackColor      , RocGuiFrame::OnBackColor)
    EVT_MENU( ME_UHL_63350      , RocGuiFrame::OnUhl63350)
    EVT_MENU( ME_LOCOIO         , RocGuiFrame::OnLocoIO)
    EVT_MENU( ME_DTOpSw         , RocGuiFrame::OnDTOpSw)

    EVT_MENU( ME_LangEnglish    , RocGuiFrame::OnLangEnglish)
    EVT_MENU( ME_LangGerman     , RocGuiFrame::OnLangGerman)
    EVT_MENU( ME_LangDutch      , RocGuiFrame::OnLangDutch)
    EVT_MENU( ME_LangSwedisch   , RocGuiFrame::OnLangSwedisch)
    EVT_MENU( ME_LangFrench     , RocGuiFrame::OnLangFrench)
    EVT_MENU( ME_LangTurkish    , RocGuiFrame::OnLangTurkish)
    EVT_MENU( ME_LangSpanish    , RocGuiFrame::OnLangSpanish)
    EVT_MENU( ME_LangItalien    , RocGuiFrame::OnLangItalien)
    EVT_MENU( ME_LangDanish     , RocGuiFrame::OnLangDanish)
    EVT_MENU( ME_LangCzech      , RocGuiFrame::OnLangCzech)
    EVT_MENU( ME_LangBosnian    , RocGuiFrame::OnLangBosnian)

    EVT_GRID_CELL_LEFT_CLICK( RocGuiFrame::OnCellLeftClick )
    EVT_GRID_CELL_LEFT_DCLICK( RocGuiFrame::OnCellLeftDClick )
    EVT_GRID_CELL_RIGHT_CLICK( RocGuiFrame::OnCellRightClick )
    EVT_GRID_SELECT_CELL( RocGuiFrame::OnSelectCell )

    EVT_MENU     (ME_GridLocGo      , RocGuiFrame::OnLocGo  )
    EVT_MENU     (ME_GridLocStop    , RocGuiFrame::OnLocStop)
    EVT_MENU     (ME_GridLocReset   , RocGuiFrame::OnLocReset)
    EVT_MENU     (ME_GridLocSwap    , RocGuiFrame::OnLocSwap)
    EVT_MENU     (ME_GridLocDispatch, RocGuiFrame::OnLocDispatch)
    EVT_MENU     (ME_GridLocProps   , RocGuiFrame::OnLocProps)
    EVT_MENU     (ME_GridLocGoTo    , RocGuiFrame::OnLocGoTo)
    EVT_MENU     (ME_GridLocSchedule, RocGuiFrame::OnLocSchedule)
    EVT_MENU     (ME_GridLocShortID , RocGuiFrame::OnLocShortID)

    EVT_MENU( ME_F1 , RocGuiFrame::OnButton)
    EVT_MENU( ME_F2 , RocGuiFrame::OnButton)
    EVT_MENU( ME_F3 , RocGuiFrame::OnButton)
    EVT_MENU( ME_F4 , RocGuiFrame::OnButton)
    EVT_MENU( ME_F5 , RocGuiFrame::OnButton)
    EVT_MENU( ME_F6 , RocGuiFrame::OnButton)
    EVT_MENU( ME_F7 , RocGuiFrame::OnButton)
    EVT_MENU( ME_F8 , RocGuiFrame::OnButton)
    EVT_MENU( ME_F9 , RocGuiFrame::OnButton)
    EVT_MENU( ME_F10, RocGuiFrame::OnButton)
    EVT_MENU( ME_F11, RocGuiFrame::OnButton)
    EVT_MENU( ME_F12, RocGuiFrame::OnButton)
    EVT_MENU( ME_F13, RocGuiFrame::OnButton)
    EVT_MENU( ME_F14, RocGuiFrame::OnButton)
    EVT_MENU( ME_F15, RocGuiFrame::OnButton)
    EVT_MENU( ME_F16, RocGuiFrame::OnButton)
    EVT_MENU( ME_F17, RocGuiFrame::OnButton)
    EVT_MENU( ME_F18, RocGuiFrame::OnButton)
    EVT_MENU( ME_F19, RocGuiFrame::OnButton)
    EVT_MENU( ME_F20, RocGuiFrame::OnButton)
    EVT_MENU( ME_F21, RocGuiFrame::OnButton)
    EVT_MENU( ME_F22, RocGuiFrame::OnButton)
    EVT_MENU( ME_F23, RocGuiFrame::OnButton)
    EVT_MENU( ME_F24, RocGuiFrame::OnButton)
    EVT_MENU( ME_F25, RocGuiFrame::OnButton)
    EVT_MENU( ME_F26, RocGuiFrame::OnButton)
    EVT_MENU( ME_F27, RocGuiFrame::OnButton)
    EVT_MENU( ME_F28, RocGuiFrame::OnButton)

    EVT_MENU( ME_INITACTIVELOCS, RocGuiFrame::InitActiveLocs)

END_EVENT_TABLE()



iONode RocGuiFrame::findLoc( const char* locid ) {
  iONode model = wxGetApp().getModel();
  iONode lclist = wPlan.getlclist( model );
  if( lclist != NULL ) {
    int cnt = NodeOp.getChildCnt( lclist );
    for( int i = 0; i < cnt; i++ ) {
      iONode lc = NodeOp.getChild( lclist, i );
      const char* id = wLoc.getid( lc );

      if( id != NULL && StrOp.equals( locid, id ) ) {
        return lc;
      }
    }
  }
  return NULL;
}


iONode RocGuiFrame::findCar( const char* carid ) {
  iONode model = wxGetApp().getModel();
  iONode carlist = wPlan.getcarlist( model );
  if( carlist != NULL ) {
    int cnt = NodeOp.getChildCnt( carlist );
    for( int i = 0; i < cnt; i++ ) {
      iONode car = NodeOp.getChild( carlist, i );
      const char* id = wCar.getid( car );

      if( id != NULL && StrOp.equals( carid, id ) ) {
        return car;
      }
    }
  }
  return NULL;
}


iONode RocGuiFrame::findWaybill( const char* billid ) {
  iONode model = wxGetApp().getModel();
  iONode waybilllist = wPlan.getwaybilllist( model );
  if( waybilllist != NULL ) {
    int cnt = NodeOp.getChildCnt( waybilllist );
    for( int i = 0; i < cnt; i++ ) {
      iONode waybill = NodeOp.getChild( waybilllist, i );
      const char* id = wWaybill.getid( waybill );

      if( id != NULL && StrOp.equals( billid, id ) ) {
        return waybill;
      }
    }
  }
  return NULL;
}


iONode RocGuiFrame::findBlock4Loc( const char* locid ) {
  iONode model = wxGetApp().getModel();
  iONode bklist = wPlan.getbklist( model );
  if( bklist != NULL ) {
    int cnt = NodeOp.getChildCnt( bklist );
    for( int i = 0; i < cnt; i++ ) {
      iONode bk = NodeOp.getChild( bklist, i );
      const char* id = wBlock.getlocid( bk );

      if( id != NULL && StrOp.equals( locid, id ) ) {
        return bk;
      }
    }
  }
  return NULL;
}


BasePanel* RocGuiFrame::initModPanel( iONode zlevel  ) {
  int itemsize = wxGetApp().getItemSize();
  iONode ini = wGui.getplanpanel(m_Ini);

  if( m_ModPanel == NULL ) {
    iONode ini = wGui.getmodpanel(m_Ini);
    if( ini == NULL ) {
      ini = NodeOp.inst( wModPanel.name(), m_Ini, ELEMENT_NODE );
      NodeOp.addChild( m_Ini, ini );
    }
    m_ModPanel = new ModPanel( m_PlanNotebook, itemsize, m_Scale );
    m_PlanNotebook->AddPage(m_ModPanel, _T("Module Overview"), true);
    wxColor color((byte)wModPanel.getred( ini), (byte)wModPanel.getgreen( ini), (byte)wModPanel.getblue( ini));
    m_ModPanel->SetBackgroundColour(color);
  }

  if(zlevel == NULL)
    return NULL;

  if( m_ModPanel->hasZLevel(zlevel) ) {
    m_ModPanel->updateZLevel(zlevel);
    return NULL;
  }

  PlanPanel* p = new PlanPanel( m_ModPanel, itemsize, m_Scale, m_Bktext, wZLevel.getz( zlevel ), zlevel, wPlanPanel.isshowborder( ini) );
  p->EnableScrolling( false, false );
  m_ModPanel->addModule(p);

  int xpos = (int)(itemsize * m_Scale * wZLevel.getmodviewx(zlevel));
  int ypos = (int)(itemsize * m_Scale * wZLevel.getmodviewy(zlevel));

  p->SetPosition( wxPoint( xpos, ypos) );

  p->SetBackgroundColor((byte)wPlanPanel.getred( ini), (byte)wPlanPanel.getgreen( ini), (byte)wPlanPanel.getblue( ini));
  p->clean();
  p->init(true);
  return p;
}


BasePanel* RocGuiFrame::InitNotebookPage( iONode zlevel  ) {

  if( m_Scale < 0.1 )
    m_Scale = 0.5;

  m_ScaleComboBox->SetValue( wxString::Format( _T("%d"), (int)(m_Scale*100)) );

  if( wxGetApp().isModView() && !wxGetApp().isOffline() ) {
    return initModPanel( zlevel );
  }

  int pages = m_PlanNotebook->GetPageCount();
  for( int i = 0; i < pages; i++ ) {
    BasePanel* p = (BasePanel*)m_PlanNotebook->GetPage(i);
    if( StrOp.equals( p->getZLevelTitle(), wZLevel.gettitle( zlevel ) ) ||
        p->getZ() == wZLevel.getz( zlevel ) ) {
      p->updateZLevel(zlevel);
      return p;
    }
  }


  iONode ini = wGui.getplanpanel(m_Ini);
  PlanPanel* p = new PlanPanel( m_PlanNotebook, wxGetApp().getItemSize(), m_Scale, m_Bktext, wZLevel.getz( zlevel ), zlevel, wPlanPanel.isshowborder( ini) );
  int itemsize = wxGetApp().getItemSize();
  p->SetScrollbars( (int)(itemsize*m_Scale), (int)(itemsize*m_Scale), 64, 48 );
  wxColor color((byte)wPlanPanel.getred( ini), (byte)wPlanPanel.getgreen( ini), (byte)wPlanPanel.getblue( ini));
  p->SetBackgroundColour(color);
  TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "Adding level %s (%d,%d)",
      wZLevel.gettitle( zlevel ), wZLevel.getmodviewcx( zlevel ), wZLevel.getmodviewcy( zlevel ) );
  m_PlanNotebook->AddPage(p, wxString(wZLevel.gettitle( zlevel ),wxConvUTF8), wZLevel.isactive( zlevel )?true:false);
  p->clean();
  p->init();
  return p;
}



void RocGuiFrame::RemoveNotebookPage( iONode zlevel  ) {
  if( m_ModPanel != NULL ) {
    m_ModPanel->removeModule(zlevel);
  }
  else {
    int pages = m_PlanNotebook->GetPageCount();
    for( int i = 0; i < pages; i++ ) {
      BasePanel* p = (BasePanel*)m_PlanNotebook->GetPage(i);
      if( wZLevel.getz( zlevel ) == p->getZ() ) {
        p->clean();
        m_PlanNotebook->DeletePage( i );
        break;
      }
    }
  }
  NodeOp.base.del( zlevel );
}


void RocGuiFrame::OnPageChange(wxNotebookEvent& event) {
  if( event.GetEventObject() == m_PlanNotebook ) {
    TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "select changed from %d to %d",
        event.GetOldSelection(), event.GetSelection() );
    int pages = m_PlanNotebook->GetPageCount();
    for( int i = 0; i < pages; i++ ) {
      BasePanel* p = (BasePanel*)m_PlanNotebook->GetPage(i);
      if( i == event.GetSelection() ) {
        TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "selecting %d", i );
        wZLevel.setactive( p->getZLevel(), True );
        p->refresh();
      }
      else {
        wZLevel.setactive( p->getZLevel(), False );
      }
    }
  }
}

void RocGuiFrame::OnInitNotebook( wxCommandEvent& event ) {

  wxCursor cursor = wxCursor(wxCURSOR_WAIT);
  SetCursor(cursor);

  int pages = m_PlanNotebook->GetPageCount();
  for( int i = 0; i < pages; i++ ) {
    BasePanel* p = (BasePanel*)m_PlanNotebook->GetPage(i);
    p->clean();
  }

  m_PlanNotebook->DeleteAllPages();
  iONode zlevel = wPlan.getzlevel( wxGetApp().getModel() );

  int idx = 0;
  iONode firstlevel = zlevel;
  iONode activelevel = NULL;

  while( zlevel != NULL ) {
    if( wZLevel.isactive( zlevel ) )
      activelevel = zlevel;
    zlevel = wPlan.nextzlevel( wxGetApp().getModel(), zlevel );
  };

  if( firstlevel != NULL && activelevel == NULL ) {
    wZLevel.setactive( firstlevel, True );
  }

  if( wxGetApp().isModView() && !wxGetApp().isOffline() ) {
    initModPanel( NULL );
  }


  zlevel = wPlan.getzlevel( wxGetApp().getModel() );
  while( zlevel != NULL ) {
    BasePanel* p = InitNotebookPage( zlevel );
    if( idx == 0 )
      setPlanPanel(p);
    zlevel = wPlan.nextzlevel( wxGetApp().getModel(), zlevel );
    idx++;
  };

  // Set selection:
  if( wxGetApp().getTabSel() != -1 ) {
    int tabsel = wxGetApp().getTabSel();
    TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "Set tab selection to [%d]", tabsel );
    m_PlanNotebook->ChangeSelection(tabsel);

    int pages = m_PlanNotebook->GetPageCount();
    for( int i = 0; i < pages; i++ ) {
      BasePanel* p = (BasePanel*)m_PlanNotebook->GetPage(i);
      wZLevel.setactive( p->getZLevel(), False );
    }

    BasePanel* p = (BasePanel*)m_PlanNotebook->GetPage(tabsel);
    TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "selecting %d", tabsel );
    wZLevel.setactive( p->getZLevel(), True );
    p->refresh();
  }


  iONode model = wxGetApp().getModel();
  setPlanTitle( wPlan.gettitle(model) );

  wxCommandEvent evt( wxEVT_COMMAND_MENU_SELECTED, ME_INITACTIVELOCS );
  wxPostEvent( this, evt );

   wxGetApp().cleanupOldModel();

  cursor = wxCursor(wxCURSOR_ARROW);
  SetCursor(cursor);

  m_bInitialized = true;

}

static int locComparator(obj* o1, obj* o2) {
  if( *o1 == NULL || *o2 == NULL )
    return 0;
  return strcmp( wLoc.getid( (iONode)*o1 ), wLoc.getid( (iONode)*o2 ) );
}


void RocGuiFrame::modifyLoc( iONode props ) {
  iONode model = wxGetApp().getModel();
  iONode loc = NULL;
  int i = 0;


  if( model != NULL ) {
    iONode lclist = wPlan.getlclist( model );
    if( lclist != NULL ) {
      int cnt = NodeOp.getChildCnt( lclist );
      for( i = 0; i < cnt; i++ ) {
        iONode lc = NodeOp.getChild( lclist, i );
        if( StrOp.equals( wLoc.getid(props), wLoc.getid(lc))) {
          // merge
          loc = lc;
          break;
        }
      }
    }
  }

  if( loc != NULL ) {
    TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "merge loc [%s]", wLoc.getid(loc) );
    int cnt = NodeOp.getAttrCnt( props );
    for( i = 0; i < cnt; i++ ) {
      iOAttr attr = NodeOp.getAttr( props, i );
      const char* name  = AttrOp.getName( attr );
      const char* value = AttrOp.getVal( attr );
      NodeOp.setStr( loc, name, value );
    }

    /* Leave the childs if no new are comming */
    if( NodeOp.getChildCnt( props ) > 0 ) {
      cnt = NodeOp.getChildCnt( loc );
      while( cnt > 0 ) {
        iONode child = NodeOp.getChild( loc, 0 );
        NodeOp.removeChild( loc, child );
        child->base.del(child);
        cnt = NodeOp.getChildCnt( loc );
      }
      cnt = NodeOp.getChildCnt( props );
      for( i = 0; i < cnt; i++ ) {
        iONode child = NodeOp.getChild( props, i );
        NodeOp.addChild( loc, (iONode)NodeOp.base.clone(child) );
      }
    }
  }
}

void RocGuiFrame::InitActiveLocs(wxCommandEvent& event) {
  iONode node = (iONode)event.GetClientData();
  if( node != NULL ) {
    iONode lc   = findLoc(wLoc.getid(node));

    if( lc != NULL ) {
     /* merge all attribute */
      NodeOp.mergeNode( lc, node, True, True, True );
    }
    NodeOp.base.del(node);
  }

  iONode model = wxGetApp().getModel();
  bool hiddenlocos = false;
  bool firstset = false;
  const char* firstid = NULL;
  iONode selectedLoc = NULL;

  m_LocImage->SetBitmapLabel( wxBitmap(nopict_xpm) );
  m_LocImage->Refresh();

  m_ActiveLocs->EnableEditing(false);
  m_ActiveLocs->EnableDragGridSize(false);

  m_ActiveLocs->DeleteRows( 0, m_ActiveLocs->GetNumberRows() );
  m_ActiveLocs->Show( false );
  if( model != NULL ) {
    iONode lclist = wPlan.getlclist( model );
    if( lclist != NULL ) {
      int i;
      iOList list = ListOp.inst();
      int cnt = NodeOp.getChildCnt( lclist );
      for( i = 0; i < cnt; i++ ) {
        iONode lc = NodeOp.getChild( lclist, i );

        if( wLoc.isshow(lc) ) {
          // list only loco's with the show flag set.
          ListOp.add( list, (obj)lc );
        }
        else {
          hiddenlocos = true;
          TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "loc [%s] is not shown", wLoc.getid(lc) );
        }
      }
      // Sort the list:
      ListOp.sort( list, locComparator );

      for( i = 0; i < ListOp.size( list ); i++ ) {
        iONode lc = (iONode)ListOp.get( list, i );
        if( lc == NULL )
          continue;
        const char* id = wLoc.getid( lc );

        if( !firstset && m_LC != NULL && id != NULL ) {
          selectedLoc = lc;
          firstset = true;
          firstid = id;
          TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "Set first active loc to %s", id );
        }

        TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "Adding active loc %s", id );
        m_ActiveLocs->AppendRows();
        m_ActiveLocs->SetCellValue(m_ActiveLocs->GetNumberRows()-1, LOC_COL_ID, wxString(id,wxConvUTF8) );
        m_ActiveLocs->SetReadOnly( m_ActiveLocs->GetNumberRows()-1, LOC_COL_ID, true );
        m_ActiveLocs->SetCellAlignment( m_ActiveLocs->GetNumberRows()-1, LOC_COL_ID, wxALIGN_LEFT, wxALIGN_CENTRE );

        char* val = StrOp.fmt( "%d", wLoc.getaddr( lc ) );
        m_ActiveLocs->SetCellValue(m_ActiveLocs->GetNumberRows()-1, LOC_COL_ADDR, wxString(val,wxConvUTF8) );
        m_ActiveLocs->SetReadOnly( m_ActiveLocs->GetNumberRows()-1, LOC_COL_ADDR, true );
        m_ActiveLocs->SetCellAlignment( m_ActiveLocs->GetNumberRows()-1, LOC_COL_ADDR, wxALIGN_RIGHT, wxALIGN_CENTRE );
        StrOp.free( val );
        iONode bk = findBlock4Loc(id);
        if( bk != NULL ) {
          m_ActiveLocs->SetCellValue(m_ActiveLocs->GetNumberRows()-1, LOC_COL_BLOCK, wxString(wBlock.getid( bk ),wxConvUTF8) );
        }
        m_ActiveLocs->SetReadOnly( m_ActiveLocs->GetNumberRows()-1, LOC_COL_BLOCK, true );
        m_ActiveLocs->SetCellAlignment( m_ActiveLocs->GetNumberRows()-1, LOC_COL_BLOCK, wxALIGN_LEFT, wxALIGN_CENTRE );

        val = StrOp.fmt( "%c%d", wLoc.isplacing(lc)?' ':'-', wLoc.getV( lc )==-1?0:wLoc.getV( lc ) );
        m_ActiveLocs->SetCellValue(m_ActiveLocs->GetNumberRows()-1, LOC_COL_V, wxString(val,wxConvUTF8) );
        m_ActiveLocs->SetReadOnly( m_ActiveLocs->GetNumberRows()-1, LOC_COL_V, true );
        m_ActiveLocs->SetCellAlignment( m_ActiveLocs->GetNumberRows()-1, LOC_COL_V, wxALIGN_RIGHT, wxALIGN_CENTRE );

        m_ActiveLocs->SetCellBackgroundColour( m_ActiveLocs->GetNumberRows()-1, LOC_COL_V,
            wLoc.isplacing(lc)?
                m_ActiveLocs->GetCellBackgroundColour(m_ActiveLocs->GetNumberRows()-1, LOC_COL_BLOCK):
                  wxColour(240,200,200));

        int jsdev = m_JsSupport->getDev4ID(wLoc.getid( lc ));
        if( jsdev != -1 )
          m_ActiveLocs->SetCellValue( m_ActiveLocs->GetNumberRows()-1, LOC_COL_MODE, _T("ctrl") + wxString::Format(_T("%d"), jsdev) );
        else
          m_ActiveLocs->SetCellValue( m_ActiveLocs->GetNumberRows()-1, LOC_COL_MODE, wxString(wLoc.isresumeauto( lc ) ? "*":"",wxConvUTF8) + wxString(wLoc.getmode( lc ),wxConvUTF8) );

        m_ActiveLocs->SetReadOnly( m_ActiveLocs->GetNumberRows()-1, LOC_COL_MODE, true );
        m_ActiveLocs->SetCellAlignment( m_ActiveLocs->GetNumberRows()-1, LOC_COL_MODE, wxALIGN_LEFT, wxALIGN_CENTRE );

      }
      ListOp.base.del( list );
    }
    m_ActiveLocs->AutoSize();
    m_ActiveLocs->SetCellBackgroundColour( hiddenlocos ? wxColour(255,255,200):wxColour(255,255,255));

  }

  m_ActiveLocs->Show( true );
  if( firstset && firstid!=NULL && selectedLoc != NULL ) {
    m_LocID = StrOp.dup(firstid);
    m_LC->setLocProps(selectedLoc);


    wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, UPDATE_LOC_IMAGE_EVENT );
    event.SetClientData( NodeOp.base.clone( selectedLoc ) );
    wxPostEvent( this, event );
  }

  m_ActiveLocs->FitInside();
  m_ActiveLocsPanel->GetSizer()->Layout();
  m_ActiveLocs->SelectRow(m_iLcRowSelection);

  initLocCtrlDialogs();
  m_CV->init();

}



void RocGuiFrame::initLocCtrlDialogs() {
  iONode lcctrl = wGui.getlcctrl(m_Ini);
  while( lcctrl != NULL ) {
    const char* id = wLcCtrl.getid(lcctrl);
    int x = wLcCtrl.getx(lcctrl);
    int y = wLcCtrl.gety(lcctrl);
    TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "previous position [%d,%d] for [%s]", x, y, id );

    LocControlDialog* dlg = new LocControlDialog(this, m_LocCtrlList, m_LocDlgMap, id);
    dlg->Show(TRUE);
    dlg->Move( x, y );

    lcctrl = wGui.nextlcctrl(m_Ini, lcctrl);
  };

}


void RocGuiFrame::CVevent( wxCommandEvent& event ) {
  // Get copied node:
  iONode node = (iONode)event.GetClientData();
  if( wProgram.islncv(node) ) {
    if( m_LocoIO != NULL )
      m_LocoIO->event( node );
    else if( m_DTOpSw != NULL )
      m_DTOpSw->event( node );
    else
      m_LNCV->event( node );
  }
  else if( wProgram.getlntype(node) == wProgram.lntype_cs ) {
    if( m_RocrailIniDlg != NULL )
      m_RocrailIniDlg->event( node );
  }
  else {
    m_CV->event( node );
  }
}


wxString RocGuiFrame::getIconPath(const char* iconfile) {
  if( wxGetApp().getIni() != NULL ) {
    const char* iconpath = wGui.geticonpath(wxGetApp().getIni());
    char* path = NULL;
    TraceOp.trc( "frame", TRCLEVEL_DEBUG, __LINE__, 9999, "IconPath=0x%08X", m_IconPath );
    if( m_IconPath != NULL ) {
      path = StrOp.fmt( "%s%c%s.png", m_IconPath, SystemOp.getFileSeparator(), iconfile );
    }
    else
      path = StrOp.fmt( "%s%c%s.png", iconpath, SystemOp.getFileSeparator(), iconfile );
    if( !FileOp.exist(path) ) {
      char* str = StrOp.fmt( wxGetApp().getMsg("iconnotfound").mb_str(wxConvUTF8), path );
      int action = wxMessageDialog( this, wxString(str,wxConvUTF8), _T("Rocrail"), wxCANCEL | wxICON_ERROR ).ShowModal();
      StrOp.free(str);
      //exit(-1);
    }
    return wxString(path,wxConvUTF8);
    StrOp.free(path);
  }
  else
    return wxString("",wxConvUTF8);
}


void RocGuiFrame::UpdateActiveLocs( wxCommandEvent& event ) {
  // Get copied node:
  iONode node = (iONode)event.GetClientData();

  if( StrOp.equals( wFunCmd.name(), NodeOp.getName( node ) ) )
  {
    TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "FunCmd event: [%s] destblock=[%s]",
        wLoc.getid( node ), (wLoc.getdestblockid( node ) != NULL ? wLoc.getdestblockid( node ):"-") );
    for( int i = 0; i < ListOp.size(m_LocCtrlList); i++ ) {
      LocControlDialog* dlg = (LocControlDialog*)ListOp.get(m_LocCtrlList, i);
      dlg->modelEvent(node);
    }
    m_LC->updateLoc( node );
  }
  else if( StrOp.equals( wLoc.name(), NodeOp.getName( node ) ) )
  {
    TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "Loc event: [%s] destblock=[%s]",
        wLoc.getid( node ), (wLoc.getdestblockid( node ) != NULL ? wLoc.getdestblockid( node ):"-") );

    for( int i = 0; i < ListOp.size(m_LocCtrlList); i++ ) {
      LocControlDialog* dlg = (LocControlDialog*)ListOp.get(m_LocCtrlList, i);
      dlg->modelEvent(node);
    }

    modifyLoc( node );

    m_LC->updateLoc( node );

    for( int i = 0; i < m_ActiveLocs->GetNumberRows(); i++ ) {
      char* locid = StrOp.dup( m_ActiveLocs->GetCellValue( i, 0 ).mb_str(wxConvUTF8) );
      if( StrOp.equals( wLoc.getid( node ), locid ) ) {
        char* val = StrOp.fmt( "%c%d", wLoc.isplacing( node )?' ':'-', wLoc.getV( node )==-1?0:wLoc.getV( node ) );
        m_ActiveLocs->SetCellValue( i, LOC_COL_V, wxString(val,wxConvUTF8) );
        StrOp.free( val );
        m_ActiveLocs->SetCellBackgroundColour( i, LOC_COL_V,
            wLoc.isplacing(node)?m_ActiveLocs->GetCellBackgroundColour(i, LOC_COL_BLOCK):wxColour(240,200,200));

        int jsdev = m_JsSupport->getDev4ID(locid);
        if( jsdev != -1 )
          m_ActiveLocs->SetCellValue( i, LOC_COL_MODE, _T("ctrl") + wxString::Format(_T("%d"), jsdev) );
        else
          m_ActiveLocs->SetCellValue( i, LOC_COL_MODE, wxString(wLoc.isresumeauto( node ) ? "*":"",wxConvUTF8) + wxString(wLoc.getmode( node ),wxConvUTF8) );

        if( wLoc.getblockid( node ) != NULL ) {
          m_ActiveLocs->SetCellValue( i, LOC_COL_BLOCK, wxString(wLoc.getblockid( node ),wxConvUTF8) );
        }

        if( wLoc.getdestblockid( node ) != NULL ) {
          if( wLoc.getscidx(node) == -1 )
          m_ActiveLocs->SetCellValue( i, LOC_COL_DESTBLOCK, wxString(wLoc.getdestblockid( node ),wxConvUTF8) );
          else {
            val = StrOp.fmt( "[%d] %s", wLoc.getscidx( node ), wLoc.getdestblockid( node ) );
            m_ActiveLocs->SetCellValue( i, LOC_COL_DESTBLOCK, wxString(val,wxConvUTF8) );
            StrOp.free( val );
          }
        }
        StrOp.free( locid );
        break;
      }
      else {
        StrOp.free( locid );
      }
    }
  }
  else if( StrOp.equals( wBlock.name(), NodeOp.getName( node ) ) ) {
    /*
    for( int i = 0; i < m_ActiveLocs->GetNumberRows(); i++ ) {
      const char* blockid = "";
      char* locid = StrOp.dup( m_ActiveLocs->GetCellValue( i, 0 ).mb_str(wxConvUTF8) );
      iONode bk = findBlock4Loc(locid);
      if( bk != NULL )
        blockid = wBlock.getid(bk);
      m_ActiveLocs->SetCellValue( i, LOC_COL_BLOCK, wxString(blockid,wxConvUTF8) );
      StrOp.free( locid );
    }
    */
    TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "Block event %s, locid=%s",
                 NodeOp.getStr(node, "id", "?"), wBlock.getlocid( node ) );
  }
  else if( StrOp.equals( wDataReq.name(), NodeOp.getName( node ) ) ) {
    const char* byteStr = wDataReq.getdata(node);
    if( byteStr != NULL ) {
      // convert string to byte array
      byte* image = StrOp.strToByte( byteStr );
      int len = StrOp.len(byteStr)/2;

      // save image
      const char* imagepath = wGui.getimagepath(m_Ini);
      static char pixpath[256];
      StrOp.fmtb( pixpath, "%s%c%s", imagepath, SystemOp.getFileSeparator(), FileOp.ripPath( wDataReq.getfilename(node) ) );
      if( !FileOp.exist(imagepath) ) {
        FileOp.mkdir(imagepath);
      }
      iOFile f = FileOp.inst( pixpath, OPEN_WRITE );
      if( f != NULL ) {
        FileOp.write( f, (const char*)image, len);
        FileOp.base.del(f);
      }
      freeMem(image);

      // get the active loco
      iONode lc = m_LC->getLocProps();

      if( lc != NULL ) {
        // update loco image
        wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, UPDATE_LOC_IMAGE_EVENT );
        event.SetClientData( NodeOp.base.clone( lc ) );
        wxPostEvent( this, event );
      }
    }
  }
  //m_ActiveLocs->AutoSize();
  //m_ActiveLocs->FitInside();
  //m_ActiveLocsPanel->GetSizer()->Layout();

  // Cleanup node:
  node->base.del( node );
}



void RocGuiFrame::setInfoText( const char* text ) {
  SetStatusText( wxString(text,wxConvUTF8), status_info );
}

void RocGuiFrame::setDigintText( const char* text ) {
  SetStatusText( wxString(text,wxConvUTF8), status_digint );
}

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

// frame constructor
RocGuiFrame::RocGuiFrame(const wxString& title, const wxPoint& pos, const wxSize& size, iONode ini, const char* icons, const char* theme)
       : wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
  m_Ini                = ini;
  m_WarningPanel       = NULL;
  m_MonitorPanel       = NULL;
  m_bEditMode          = false;
  m_bServerConsoleMode = false;
  m_LocID              = NULL;
  m_iLcRowSelection    = 0;
  m_SymbolMap          = NULL;
  m_LocalPlan          = _T("");
  m_LocoIO             = NULL;
  m_DTOpSw             = NULL;
  m_RocrailIniDlg      = NULL;
  m_ModPanel           = NULL;
  m_LocCtrlList        = ListOp.inst();
  m_LocDlgMap          = MapOp.inst();
  m_bAutoMode          = false;
  m_IconPath           = icons;
  m_ThemePath          = theme;
  m_bInitialized       = false;


}

void RocGuiFrame::initFrame() {
  TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "IconPath=0x%08X", m_IconPath );
  TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "m_ThemePath=0x%08X", m_ThemePath );
  TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "init Frame..." );
  SetIcon(wxIcon(rocrail_xpm));

  // define accelerator keys for some frequently used functions
  wxAcceleratorEntry acc_entries[64];
  acc_entries[0].Set(wxACCEL_CTRL, (int) 'Z', ME_Undo);
  acc_entries[1].Set(wxACCEL_CTRL, (int) 'N', ME_New);
  acc_entries[2].Set(wxACCEL_CTRL, (int) 'S', ME_Save);
  acc_entries[3].Set(wxACCEL_CTRL, (int) 'O', ME_Open);
  acc_entries[4].Set(wxACCEL_CTRL, (int) 'P', ME_Go);
  acc_entries[5].Set(wxACCEL_CTRL, (int) 'M', ME_MIC);
  acc_entries[6].Set(wxACCEL_CTRL, (int) 'L', ME_LcDlg);
  acc_entries[8].Set(wxACCEL_CTRL, (int) 'T', ME_SwDlg);

  acc_entries[9].Set(wxACCEL_CTRL, (int) '1', ME_EditLocs);
  acc_entries[10].Set(wxACCEL_CTRL, (int) '2', ME_EditRoutes);
  acc_entries[11].Set(wxACCEL_CTRL, (int) '3', ME_EditBlocks);
  acc_entries[12].Set(wxACCEL_CTRL, (int) '4', ME_EditSchedules);
  acc_entries[13].Set(wxACCEL_CTRL, (int) '5', ME_EditLocations);
  acc_entries[14].Set(wxACCEL_CTRL, (int) '6', ME_EditBlockGroups);
  acc_entries[15].Set(wxACCEL_CTRL, (int) '7', ME_EditTurnouts);
  acc_entries[16].Set(wxACCEL_CTRL, (int) '8', ME_EditSignals);
  acc_entries[17].Set(wxACCEL_CTRL, (int) '9', ME_EditOutputs);
  acc_entries[18].Set(wxACCEL_ALT, (int) '1', ME_EditSensors);
  //acc_entries[19].Set(wxACCEL_ALT, (int) '2', ME_EditTurntables);
  acc_entries[19].Set(wxACCEL_ALT, (int) '2', ME_EditMVTrack);

  acc_entries[20].Set(wxACCEL_NORMAL, WXK_F1 , ME_F1 );
  acc_entries[21].Set(wxACCEL_NORMAL, WXK_F2 , ME_F2 );
  acc_entries[22].Set(wxACCEL_NORMAL, WXK_F3 , ME_F3 );
  acc_entries[23].Set(wxACCEL_NORMAL, WXK_F4 , ME_F4 );
  acc_entries[24].Set(wxACCEL_NORMAL, WXK_F5 , ME_F5 );
  acc_entries[25].Set(wxACCEL_NORMAL, WXK_F6 , ME_F6 );
  acc_entries[26].Set(wxACCEL_NORMAL, WXK_F7 , ME_F7 );
  acc_entries[27].Set(wxACCEL_NORMAL, WXK_F8 , ME_F8 );
  acc_entries[28].Set(wxACCEL_NORMAL, WXK_F9 , ME_F9 );
  acc_entries[29].Set(wxACCEL_NORMAL, WXK_F10, ME_F10);
  acc_entries[30].Set(wxACCEL_NORMAL, WXK_F11, ME_F11);
  acc_entries[31].Set(wxACCEL_NORMAL, WXK_F12, ME_F12);
  acc_entries[32].Set(wxACCEL_NORMAL, WXK_PAUSE, ME_EmergencyBreak);

  acc_entries[33].Set(wxACCEL_ALT, (int) '3', ME_EditTimedActions);
  acc_entries[34].Set(wxACCEL_ALT, (int) '4', ME_EditCars);
  acc_entries[35].Set(wxACCEL_ALT, (int) '5', ME_EditWaybills);
  acc_entries[36].Set(wxACCEL_ALT, (int) '6', ME_EditOperators);

  wxAcceleratorTable m_accel(37, acc_entries);
  this->SetAcceleratorTable(m_accel);
//DA


  // create a menu bar
  wxMenu *menuFile = new wxMenu();
  wxMenuItem *connect_menuFile = new wxMenuItem(menuFile, ME_Connect, wxGetApp().getMenu("connect"), wxGetApp().getTip("connect") );
  connect_menuFile->SetBitmap(wxBitmap(getIconPath("connect"), wxBITMAP_TYPE_PNG));
  menuFile->Append(connect_menuFile);

  menuFile->Append(ME_RocguiIni, wxGetApp().getMenu("rocguiini"), wxGetApp().getTip("rocguiini") );
  menuFile->Append(ME_RocrailIni, wxGetApp().getMenu("rocrailini"), wxGetApp().getTip("rocrailini") );
  menuFile->AppendSeparator();
  menuFile->Append(ME_PlanTitle, wxGetApp().getMenu("plantitle"), wxGetApp().getTip("plantitle") );
  menuFile->AppendSeparator();

  wxMenuItem *new_menuFile = new wxMenuItem(menuFile, ME_New, wxGetApp().getMenu("new"), wxGetApp().getTip("new") );
  //new_menuFile->SetBitmap(wxBitmap(new16_xpm));
  new_menuFile->SetBitmap(wxBitmap(getIconPath("new"), wxBITMAP_TYPE_PNG));
  menuFile->Append(new_menuFile);

  wxMenuItem *open_menuFile = new wxMenuItem(menuFile, ME_Open, wxGetApp().getMenu("open"), wxGetApp().getTip("open") );
  open_menuFile->SetBitmap(wxBitmap(getIconPath("open"), wxBITMAP_TYPE_PNG));
  menuFile->Append(open_menuFile);

  wxMenuItem *save_menuFile = new wxMenuItem(menuFile, ME_Save, wxGetApp().getMenu("save"), wxGetApp().getTip("save") );
  save_menuFile->SetBitmap(wxBitmap(getIconPath("save"), wxBITMAP_TYPE_PNG));
  menuFile->Append(save_menuFile);

  menuFile->Append(ME_SaveAs, wxGetApp().getMenu("saveas"), wxGetApp().getTip("saveas") );

  menuFile->AppendSeparator();

  wxMenuItem *upload_menuFile = new wxMenuItem(menuFile, ME_Upload, wxGetApp().getMenu("upload"), wxGetApp().getTip("upload") );
  upload_menuFile->SetBitmap(wxBitmap(getIconPath("upload"), wxBITMAP_TYPE_PNG));
  menuFile->Append(upload_menuFile);

  menuFile->AppendSeparator();

  wxMenuItem *shutdown_menuFile = new wxMenuItem(menuFile, ME_ShutdownRocRail, wxGetApp().getMenu("shutdown"), wxGetApp().getTip("shutdown") );
  shutdown_menuFile->SetBitmap(wxBitmap(getIconPath("server"), wxBITMAP_TYPE_PNG));
  menuFile->Append(shutdown_menuFile);
  //menuFile->Append(ME_ShutdownRocRail, wxGetApp().getMenu("shutdown"), wxGetApp().getTip("shutdown") );
  menuFile->AppendSeparator();

  wxMenuItem *exit_menuFile = new wxMenuItem(menuFile, ME_Quit, wxGetApp().getMenu("exit"), wxGetApp().getTip("exit") );
  exit_menuFile->SetBitmap(wxBitmap(getIconPath("exit"), wxBITMAP_TYPE_PNG));
  menuFile->Append(exit_menuFile);


  wxMenu *menuEdit = new wxMenu();
  wxMenuItem *undo_menuEdit = new wxMenuItem(menuEdit, ME_Undo, wxGetApp().getMenu("undo"), wxGetApp().getTip("undo") );
  undo_menuEdit->SetBitmap(wxBitmap(getIconPath("undo"), wxBITMAP_TYPE_PNG));
  menuEdit->Append(undo_menuEdit);

  wxMenu *menuPanel = new wxMenu();
  menuPanel->AppendCheckItem(ME_CtrlMode, wxGetApp().getMenu("ctrlmode"), wxGetApp().getTip("ctrlmode") );
  menuPanel->AppendCheckItem(ME_EditMode, wxGetApp().getMenu("editmode"), wxGetApp().getTip("editmode") );
  menuPanel->AppendSeparator();
  menuPanel->Append(ME_AddPanel, wxGetApp().getMenu("addpanel"), wxGetApp().getTip("addpanel") );
  menuPanel->Append(ME_PanelProps, wxGetApp().getMenu("panelprops"), wxGetApp().getTip("panelprops") );

  wxMenu *menuTables = new wxMenu();
  menuTables->Append(ME_EditLocs, wxGetApp().getMenu("loctable"), wxGetApp().getTip("loctable") );
  menuTables->Append(ME_EditCars, wxGetApp().getMenu("cartable"), wxGetApp().getTip("cartable") );
  menuTables->Append(ME_EditWaybills, wxGetApp().getMenu("waybilltable"), wxGetApp().getTip("waybilltable") );
  menuTables->Append(ME_EditOperators, wxGetApp().getMenu("operatortable"), wxGetApp().getTip("operatortable") );
  menuTables->Append(ME_EditRoutes, wxGetApp().getMenu("routetable"), wxGetApp().getTip("routetable") );
  menuTables->Append(ME_EditBlocks, wxGetApp().getMenu("blocktable"), wxGetApp().getTip("blocktable") );
  menuTables->Append(ME_EditSchedules, wxGetApp().getMenu("scheduletable"), wxGetApp().getTip("scheduletable") );
  menuTables->Append(ME_EditLocations, wxGetApp().getMenu("locationtable"), wxGetApp().getTip("locationtable") );
  menuTables->Append(ME_EditBlockGroups, wxGetApp().getMenu("blockgrouptable"), wxGetApp().getTip("blockgrouptable") );
  menuTables->Append(ME_EditTurnouts, wxGetApp().getMenu("turnouttable"), wxGetApp().getTip("turnouttable") );
  menuTables->Append(ME_EditSignals, wxGetApp().getMenu("signaltable"), wxGetApp().getTip("signaltable") );
  menuTables->Append(ME_EditOutputs, wxGetApp().getMenu("outputtable"), wxGetApp().getTip("outputtable") );
  menuTables->Append(ME_EditSensors, wxGetApp().getMenu("sensortable"), wxGetApp().getTip("sensortable") );
  menuTables->Append(ME_EditMVTrack, wxGetApp().getMenu("mvtrack"), wxGetApp().getTip("mvtrack") );
  menuTables->Append(ME_EditTimedActions, wxGetApp().getMenu("actiontable"), wxGetApp().getTip("actiontable") );

  // Turntable dialog has no index tab... disable
  //menuTables->Append(ME_EditTurntables, wxGetApp().getMenu("turntabletable"), wxGetApp().getTip("turntabletable") );

  wxMenu *menuControl = new wxMenu();
  menuControl->AppendCheckItem(ME_Go, wxGetApp().getMenu("poweron"), wxGetApp().getTip("poweron") );

  menuControl->AppendSeparator();
  menuControl->Append(ME_InitField, wxGetApp().getMenu("initfield"), wxGetApp().getTip("initfield") );
  menuControl->AppendSeparator();
  menuControl->Append(ME_TxShortIDs, wxGetApp().getMenu("txshortids"), wxGetApp().getTip("txshortids") );
  menuControl->AppendSeparator();

  wxMenuItem *opctrl_menuControl = new wxMenuItem(menuControl, ME_OperatorDlg, wxGetApp().getMenu("operator"), wxGetApp().getTip("operator") );
  opctrl_menuControl->SetBitmap(wxBitmap(getIconPath("operator"), wxBITMAP_TYPE_PNG));
  menuControl->Append(opctrl_menuControl);

  menuControl->AppendSeparator();

  wxMenuItem *mic_menuControl = new wxMenuItem(menuControl, ME_MIC, wxGetApp().getMenu("mic"), wxGetApp().getTip("mic") );
  mic_menuControl->SetBitmap(wxBitmap(getIconPath("mic"), wxBITMAP_TYPE_PNG));
  menuControl->Append(mic_menuControl);

  wxMenuItem *locctrl_menuControl = new wxMenuItem(menuControl, ME_LcDlg, wxGetApp().getMenu("locctrl"), wxGetApp().getTip("locctrl") );
  locctrl_menuControl->SetBitmap(wxBitmap(getIconPath("locctrl"), wxBITMAP_TYPE_PNG));
  menuControl->Append(locctrl_menuControl);

  wxMenuItem *swctrl_menuControl = new wxMenuItem(menuControl, ME_SwDlg, wxGetApp().getMenu("swctrl"), wxGetApp().getTip("swctrl") );
  swctrl_menuControl->SetBitmap(wxBitmap(getIconPath("swctrl"), wxBITMAP_TYPE_PNG));
  menuControl->Append(swctrl_menuControl);

  wxMenuItem *stctrl_menuControl = new wxMenuItem(menuControl, ME_RouteDlg, wxGetApp().getMenu("stctrl"), wxGetApp().getTip("stctrl") );
  stctrl_menuControl->SetBitmap(wxBitmap(getIconPath("routes"), wxBITMAP_TYPE_PNG));
  menuControl->Append(stctrl_menuControl);

  wxMenu *menuAuto = new wxMenu();
/*
 * seems that check items with bitmaps does not function...
  wxMenuItem *auto_menuAuto = new wxMenuItem(menuAuto, ME_AutoMode, wxGetApp().getMenu("automode"), wxGetApp().getTip("automode"), wxITEM_CHECK );
  auto_menuAuto->SetBitmap(wxBitmap(getIconPath("automode"), wxBITMAP_TYPE_PNG));
  menuAuto->Append(auto_menuAuto);
*/
  menuAuto->AppendCheckItem(ME_AutoMode, wxGetApp().getMenu("automode"), wxGetApp().getTip("automode") );
  menuAuto->AppendSeparator();
  menuAuto->Append(ME_AutoGo  , wxGetApp().getMenu("startall"), wxGetApp().getTip("startall") );
  menuAuto->Append(ME_AutoResume, wxGetApp().getMenu("resumeall"), wxGetApp().getTip("resumeall") );

  wxMenuItem *stop_menuAuto = new wxMenuItem(menuAuto, ME_AutoStop, wxGetApp().getMenu("stopall"), wxGetApp().getTip("stopall") );
  stop_menuAuto->SetBitmap(wxBitmap(getIconPath("stopall"), wxBITMAP_TYPE_PNG));
  menuAuto->Append(stop_menuAuto);

  menuAuto->AppendSeparator();
  menuAuto->Append(ME_AutoSoftReset, wxGetApp().getMenu("softresetall"), wxGetApp().getTip("softresetall") );
  menuAuto->Append(ME_AutoReset, wxGetApp().getMenu("resetall"), wxGetApp().getTip("resetall") );

  wxMenu *menuLang = new wxMenu();
  menuLang->AppendCheckItem( ME_LangBosnian , wxGetApp().getMenu("lang_bs"), wxGetApp().getTip("lang_bs") );
  menuLang->AppendCheckItem( ME_LangCzech   , wxGetApp().getMenu("lang_cs"), wxGetApp().getTip("lang_cs") );
  menuLang->AppendCheckItem( ME_LangDanish  , wxGetApp().getMenu("lang_da"), wxGetApp().getTip("lang_da") );
  menuLang->AppendCheckItem( ME_LangGerman  , wxGetApp().getMenu("lang_de"), wxGetApp().getTip("lang_de") );
  menuLang->AppendCheckItem( ME_LangEnglish , wxGetApp().getMenu("lang_en"), wxGetApp().getTip("lang_en") );
  menuLang->AppendCheckItem( ME_LangSpanish , wxGetApp().getMenu("lang_es"), wxGetApp().getTip("lang_es") );
  menuLang->AppendCheckItem( ME_LangFrench  , wxGetApp().getMenu("lang_fr"), wxGetApp().getTip("lang_fr") );
  menuLang->AppendCheckItem( ME_LangItalien , wxGetApp().getMenu("lang_it"), wxGetApp().getTip("lang_it") );
  menuLang->AppendCheckItem( ME_LangDutch   , wxGetApp().getMenu("lang_nl"), wxGetApp().getTip("lang_nl") );
  menuLang->AppendCheckItem( ME_LangSwedisch, wxGetApp().getMenu("lang_sv"), wxGetApp().getTip("lang_sv") );
  menuLang->AppendCheckItem( ME_LangTurkish , wxGetApp().getMenu("lang_tr"), wxGetApp().getTip("lang_tr") );

  wxMenu *menuZoom = new wxMenu();
  menuZoom->AppendCheckItem( ME_Zoom25, _T("25%") );
  menuZoom->AppendCheckItem( ME_Zoom50, _T("50%") );
  menuZoom->AppendCheckItem( ME_Zoom75, _T("75%") );
  menuZoom->AppendCheckItem( ME_Zoom100, _T("100%") );

  wxMenu *menuView = new wxMenu();

  wxMenuItem *clear_menuView = new wxMenuItem(menuView, ME_ClearMsg, wxGetApp().getMenu("clearmsg"), wxGetApp().getTip("clearmsg") );
  clear_menuView->SetBitmap(wxBitmap(getIconPath("clear"), wxBITMAP_TYPE_PNG));
  menuView->Append(clear_menuView);

  menuView->AppendSeparator();
  menuView->Append( -1, wxGetApp().getMenu("language"), menuLang );
  menuView->AppendSeparator();
  menuView->Append( -1, wxGetApp().getMenu("zoom"), menuZoom );
  menuView->AppendSeparator();
  menuView->AppendCheckItem( ME_LocoBook, wxGetApp().getMenu("locobook"), wxGetApp().getTip("locobook") );
  menuView->AppendSeparator();
  menuView->AppendCheckItem( ME_Fill, wxGetApp().getMenu("fill"), wxGetApp().getTip("fill") );
  menuView->AppendCheckItem( ME_ShowID, wxGetApp().getMenu("showid"), wxGetApp().getTip("showid") );
  menuView->AppendCheckItem( ME_Raster, wxGetApp().getMenu("raster"), wxGetApp().getTip("raster") );
  menuView->AppendCheckItem( ME_FullScreen, wxGetApp().getMenu("fullscreen"), wxGetApp().getTip("fullscreen") );
  menuView->Append( ME_BackColor, wxGetApp().getMenu("panelcolor"), wxGetApp().getTip("panelcolor") );

  // Programming
  wxMenu *menuProgramming = new wxMenu();
  wxMenu *menuPTLN = new wxMenu();
  menuPTLN->Append( ME_LOCOIO, wxString(_T("LocoIO")) + wxString(_T("...")), _T("LocoIO") );
  menuPTLN->Append( ME_DTOpSw, wxString(_T("Digitrax")) + wxString(_T("...")), _T("Digitrax") );
  wxMenu *menuUhlenbrock = new wxMenu();
  menuUhlenbrock->Append( ME_UHL_63350, _T("Uhlenbrock 63340/63350") + wxString(_T("...")), _T("Uhlenbrock 63340/63350") );
  menuUhlenbrock->Append( ME_UHL_68610, _T("Uhlenbrock 68610") + wxString(_T("...")), _T("Uhlenbrock 68610") );
  menuPTLN->Append( -1, wxString(_T("Uhlenbrock")), menuUhlenbrock );

  menuProgramming->Append( -1, _T("LocoNet"), menuPTLN );

  wxMenu *menuPTDCC = new wxMenu();
  menuPTDCC->Append( ME_OpenDecoder, wxGetApp().getMenu("opendecoder"), wxGetApp().getTip("opendecoder") );
  menuProgramming->Append( -1, _T("NMRA DCC"), menuPTDCC );

  // the "About" item should be in the help menu
  wxMenu *menuHelp = new wxMenu();
  wxMenuItem *help_menuHelp = new wxMenuItem(menuHelp, ME_Help, wxGetApp().getMenu("documentation"), wxGetApp().getTip("documentation") );
  help_menuHelp->SetBitmap(wxBitmap(getIconPath("manual"), wxBITMAP_TYPE_PNG));
  menuHelp->Append(help_menuHelp);

  wxMenuItem *rug_menuHelp = new wxMenuItem(menuHelp, ME_RUG, wxGetApp().getMenu("usergroup"), wxGetApp().getTip("usergroup") );
  rug_menuHelp->SetBitmap(wxBitmap(getIconPath("rug"), wxBITMAP_TYPE_PNG));
  menuHelp->Append(rug_menuHelp);

  wxMenuItem *bug_menuHelp = new wxMenuItem(menuHelp, ME_Bug, wxGetApp().getMenu("bug"), wxGetApp().getTip("bug") );
  bug_menuHelp->SetBitmap(wxBitmap(getIconPath("bug"), wxBITMAP_TYPE_PNG));
  menuHelp->Append(bug_menuHelp);

  wxMenuItem *feature_menuHelp = new wxMenuItem(menuHelp, ME_Feature, wxGetApp().getMenu("feature"), wxGetApp().getTip("feature") );
  //bug_menuHelp->SetBitmap(wxBitmap(getIconPath("feature"), wxBITMAP_TYPE_PNG));
  menuHelp->Append(feature_menuHelp);

  menuHelp->AppendSeparator();
  /*
  wxMenuItem *survey_menuHelp = new wxMenuItem(menuHelp, ME_Survey, wxGetApp().getMenu("consrv"), wxGetApp().getTip("consrv") );
  survey_menuHelp->SetBitmap(wxBitmap(getIconPath("consrv"), wxBITMAP_TYPE_PNG));
  menuHelp->Append(survey_menuHelp);

  menuHelp->AppendSeparator();
  */
  wxMenuItem *update_menuHelp = new wxMenuItem(menuHelp, ME_Update, wxGetApp().getMenu("softwareupdates"), wxGetApp().getTip("softwareupdates") );
  update_menuHelp->SetBitmap(wxBitmap(getIconPath("updates"), wxBITMAP_TYPE_PNG));
  menuHelp->Append(update_menuHelp);

  menuHelp->AppendSeparator();

  wxMenuItem *info_menuHelp = new wxMenuItem(menuHelp, ME_About, wxGetApp().getMenu("about"), wxGetApp().getTip("about") );
  info_menuHelp->SetBitmap(wxBitmap(getIconPath("info"), wxBITMAP_TYPE_PNG));
  menuHelp->Append(info_menuHelp);


  // now append the freshly created menu to the menu bar...
  menuBar = new wxMenuBar();
  menuBar->Append(menuFile,    wxGetApp().getMenu("file"));
  menuBar->Append(menuEdit,    wxGetApp().getMenu("edit"));
  menuBar->Append(menuAuto,    wxGetApp().getMenu("auto"));
  menuBar->Append(menuPanel,   wxGetApp().getMenu("panel"));
  menuBar->Append(menuTables,  wxGetApp().getMenu("tables"));
  menuBar->Append(menuControl, wxGetApp().getMenu("control"));
  menuBar->Append(menuProgramming, wxGetApp().getMenu("programming"));
  menuBar->Append(menuView,    wxGetApp().getMenu("view"));
  menuBar->Append(menuHelp,    wxGetApp().getMenu("help"));

  SetMenuBar(menuBar);

  m_StatusBar = new StatusBar(this);
  SetStatusBar(m_StatusBar);


  //CreateStatusBar(4);
  SetStatusText( _T("Welcome to Rocrail!"), status_info );
  SetStatusText( _T(""), status_digint );


  // Create tool bar
  m_ToolBar = CreateToolBar();
  m_ToolBar->SetToolBitmapSize(wxSize(16, 16));

  m_ToolBar->AddTool(ME_Connect, wxBitmap(getIconPath("connect"), wxBITMAP_TYPE_PNG), wxGetApp().getTip("connect") );
  m_ToolBar->AddTool(ME_Upload, wxBitmap(getIconPath("upload"), wxBITMAP_TYPE_PNG), wxGetApp().getTip("upload") );

  m_ToolBar->AddSeparator();

  m_ToolBar->AddTool(ME_New, wxBitmap(getIconPath("new"), wxBITMAP_TYPE_PNG), wxGetApp().getTip("new") );
  m_ToolBar->AddTool(ME_Open, wxBitmap(getIconPath("open"), wxBITMAP_TYPE_PNG), wxGetApp().getTip("open") );
  m_ToolBar->AddTool(ME_Save, wxBitmap(getIconPath("save"), wxBITMAP_TYPE_PNG), wxGetApp().getTip("save") );
// Dirk 18.4.2007 added Undo to toolbar
  m_ToolBar->AddTool(ME_Undo, wxBitmap(getIconPath("undo"), wxBITMAP_TYPE_PNG), wxGetApp().getTip("undo") );
  m_ToolBar->EnableTool(ME_Undo, false );

  m_ToolBar->AddSeparator();

  m_ToolBar->AddCheckTool(ME_Go, wxGetApp().getMenu("poweron"), wxBitmap(getIconPath("poweron"), wxBITMAP_TYPE_PNG),
                        wxNullBitmap, wxGetApp().getTip("poweron") );
  m_ToolBar->AddCheckTool(ME_AutoMode, wxGetApp().getMenu("automode"), wxBitmap(getIconPath("automode"), wxBITMAP_TYPE_PNG),
                        wxNullBitmap, wxGetApp().getTip("automode") );
  m_ToolBar->AddTool(ME_AutoStop, wxBitmap(getIconPath("stopall"), wxBITMAP_TYPE_PNG), wxGetApp().getTip("stopall") );

  m_ToolBar->AddSeparator();

  m_ToolBar->AddTool(ME_OperatorDlg, wxBitmap(getIconPath("operator"), wxBITMAP_TYPE_PNG), wxGetApp().getTip("operator") );
  m_ToolBar->AddTool(ME_MIC, wxBitmap(getIconPath("mic"), wxBITMAP_TYPE_PNG), wxGetApp().getTip("mic") );
  m_ToolBar->AddTool(ME_LcDlg, wxBitmap(getIconPath("locctrl"), wxBITMAP_TYPE_PNG), wxGetApp().getTip("locctrl") );
  m_ToolBar->AddTool(ME_SwDlg, wxBitmap(getIconPath("swctrl"), wxBITMAP_TYPE_PNG), wxGetApp().getTip("swctrl") );
  m_ToolBar->AddTool(ME_RouteDlg, wxBitmap(getIconPath("routes"), wxBITMAP_TYPE_PNG), wxGetApp().getTip("stctrl") );

  m_ToolBar->AddSeparator();

  m_ScaleComboBox = new wxComboBox(m_ToolBar, ID_SCALE_COMBO, wxEmptyString, wxDefaultPosition, wxSize(80,-1) );
  m_ScaleComboBox->Append(_T("10"));
  m_ScaleComboBox->Append(_T("20"));
  m_ScaleComboBox->Append(_T("30"));
  m_ScaleComboBox->Append(_T("40"));
  m_ScaleComboBox->Append(_T("50"));
  m_ScaleComboBox->Append(_T("60"));
  m_ScaleComboBox->Append(_T("70"));
  m_ScaleComboBox->Append(_T("80"));
  m_ScaleComboBox->Append(_T("90"));
  m_ScaleComboBox->Append(_T("100"));
  m_ScaleComboBox->Append(_T("110"));
  m_ScaleComboBox->Append(_T("120"));
  m_ScaleComboBox->Append(_T("130"));
  m_ScaleComboBox->Append(_T("140"));
  m_ScaleComboBox->Append(_T("150"));
  m_ScaleComboBox->Append(_T("160"));
  m_ScaleComboBox->Append(_T("170"));
  m_ScaleComboBox->Append(_T("180"));
  m_ScaleComboBox->Append(_T("190"));
  m_ScaleComboBox->Append(_T("200"));
  m_ToolBar->AddControl(m_ScaleComboBox);


  m_ToolBar->AddSeparator();

  m_ToolBar->AddTool(ME_Update, wxBitmap(getIconPath("updates"), wxBITMAP_TYPE_PNG), wxGetApp().getTip("softwareupdates") );
  m_ToolBar->AddTool(ME_Help, wxBitmap(getIconPath("manual"), wxBITMAP_TYPE_PNG), wxGetApp().getTip("documentation") );

  m_ToolBar->Realize();

  m_ToolBar->EnableTool(ME_Update, false);

  // checking for new updates
  if(wGui.ischeckupdates(m_Ini)) {
    iOThread updateReader = ThreadOp.inst( "update", updateReaderThread, NULL );
    ThreadOp.start( updateReader );
  }

  wxLogo = NULL;

  // read the svg symbols:
  svgReader* svg = new svgReader();
  if( m_ThemePath != NULL )
    m_SymbolMap = svg->readSvgSymbols( m_ThemePath, NULL );
  else
    m_SymbolMap = svg->readSvgSymbols( wPlanPanel.getsvgpath( wGui.getplanpanel(m_Ini) ), NULL );

  m_SymbolMap = svg->readSvgSymbols( wPlanPanel.getsvgpath2( wGui.getplanpanel(m_Ini) ), m_SymbolMap );
  m_SymbolMap = svg->readSvgSymbols( wPlanPanel.getsvgpath3( wGui.getplanpanel(m_Ini) ), m_SymbolMap );
  m_SymbolMap = svg->readSvgSymbols( wPlanPanel.getsvgpath4( wGui.getplanpanel(m_Ini) ), m_SymbolMap );
  m_SymbolMap = svg->readSvgSymbols( wPlanPanel.getsvgpath5( wGui.getplanpanel(m_Ini) ), m_SymbolMap );
  delete svg;

  // check for theme properties:
  char* propPath = StrOp.fmt( "%s%cproperties.xml", wPlanPanel.getsvgpath( wGui.getplanpanel(m_Ini) ), SystemOp.getFileSeparator() );
  iOFile f = FileOp.inst( propPath, OPEN_READONLY );
  if( f != NULL && FileOp.size(f) > 0 ) {
    char* propsXml = (char*)allocMem( FileOp.size( f ) + 1 );
    FileOp.read( f, propsXml, FileOp.size( f ) );
    FileOp.close( f );
    iODoc doc = DocOp.parse( propsXml );
    if( doc != NULL ) {
      iONode themeProps = DocOp.getRootNode( doc );
      iONode themePanelProps = wTheme.getpanel(themeProps);
      iONode themeBlockProps = wTheme.getblock(themeProps);
      if( themePanelProps != NULL ) {
        wPlanPanel.setshowborder( wGui.getplanpanel(m_Ini), wThemePanel.isborder(themePanelProps) );
        wPlanPanel.setred  ( wGui.getplanpanel(m_Ini), wThemePanel.getred  (themePanelProps) );
        wPlanPanel.setgreen( wGui.getplanpanel(m_Ini), wThemePanel.getgreen(themePanelProps) );
        wPlanPanel.setblue ( wGui.getplanpanel(m_Ini), wThemePanel.getblue (themePanelProps) );
        if( wGui.getmodpanel(m_Ini) != NULL ) {
          wModPanel.setred  ( wGui.getmodpanel(m_Ini), wThemePanel.getred  (themePanelProps) );
          wModPanel.setgreen( wGui.getmodpanel(m_Ini), wThemePanel.getgreen(themePanelProps) );
          wModPanel.setblue ( wGui.getmodpanel(m_Ini), wThemePanel.getblue (themePanelProps) );
        }
      }
      if( themeBlockProps != NULL ) {
        wPlanPanel.setbktextid    ( wGui.getplanpanel(m_Ini), wThemeBlock.isshowid(themeBlockProps) );
        wPlanPanel.setbktext_red  ( wGui.getplanpanel(m_Ini), wThemeBlock.getred  (themeBlockProps) );
        wPlanPanel.setbktext_green( wGui.getplanpanel(m_Ini), wThemeBlock.getgreen(themeBlockProps) );
        wPlanPanel.setbktext_blue ( wGui.getplanpanel(m_Ini), wThemeBlock.getblue (themeBlockProps) );
      }
    }

  }
  StrOp.free(propPath);

  create();
  initJS();
}


void RocGuiFrame::setOffline( bool p_bOffline ) {
  m_bEditMode = p_bOffline;
  m_ToolBar->EnableTool(ME_Upload, !p_bOffline );
}


void RocGuiFrame::initJS() {
  m_JsSupport = JsSupport::inst( wGui.getjsmap(m_Ini) );
}


void RocGuiFrame::SyncClock( iONode node ) {
  m_LC->SyncClock(node);
}


void RocGuiFrame::create() {
  int iX = wWindow.getx( wGui.getwindow( m_Ini ) );
  int iY = wWindow.gety( wGui.getwindow( m_Ini ) );

  int iWidth  = wWindow.getcx( wGui.getwindow( m_Ini ) );
  int iHeight = wWindow.getcy( wGui.getwindow( m_Ini ) );

  m_Splitter = new wxSplitterWindow( this, 1,wxDefaultPosition, wxSize(iWidth, iHeight), wxSP_3DBORDER|wxSP_3DSASH|wxNO_BORDER );
  m_Splitter->SetMinimumPaneSize(50);

  m_PlanSplitter = new wxSplitterWindow( m_Splitter, 10, wxDefaultPosition, wxSize(iWidth, iHeight), wxSP_3DBORDER|wxSP_3DSASH|wxNO_BORDER );
  m_PlanSplitter->SetMinimumPaneSize(1);

  TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "Creating PlanPanel..." );
  m_Scale = wPlanPanel.getscale( wGui.getplanpanel( m_Ini ) );
  m_Bktext = wPlanPanel.getbktext( wGui.getplanpanel( m_Ini ) );
  m_bFill = (wPlanPanel.isfill( wGui.getplanpanel( m_Ini ) ) ? true:false);
  m_bShowID = (wPlanPanel.isshowid( wGui.getplanpanel( m_Ini ) ) ? true:false);
  m_bRaster = (wPlanPanel.israster( wGui.getplanpanel( m_Ini ) ) ? true:false);
  m_bLocoBook = (wPlanPanel.islocobook( wGui.getplanpanel( m_Ini ) ) ? true:false);

  if( m_Scale < 0.1 )
    m_Scale = 0.5;

  m_StatNotebook = new wxNotebook( m_PlanSplitter, -1, wxDefaultPosition, wxDefaultSize, wxNB_TOP );

  m_ActiveLocsPanel = new wxPanel( m_StatNotebook, -1, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
  wxBoxSizer* activeLocsSizer = new wxBoxSizer(wxVERTICAL);
  m_ActiveLocsPanel->SetSizer(activeLocsSizer);

  wxBitmap m_LocImageBitmap(wxNullBitmap);
  m_LocImage = new wxBitmapButton( m_ActiveLocsPanel, -1, m_LocImageBitmap, wxDefaultPosition, wxSize(250,88), wxBU_AUTODRAW|wxBU_EXACTFIT );
  activeLocsSizer->Add(m_LocImage, 0, wxGROW|wxALL|wxADJUST_MINSIZE, 2);

  m_ActiveLocs = new wxGrid( m_ActiveLocsPanel, -1, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
  m_ActiveLocs->SetRowLabelSize(0);
  m_ActiveLocs->CreateGrid(0, 6, wxGrid::wxGridSelectRows);
  //wxFont* font = new wxFont( m_ActiveLocs->GetDefaultCellFont() );
  //font->SetPointSize( (int)(font->GetPointSize() - 1 ) );
  m_ActiveLocs->SetSelectionMode(wxGrid::wxGridSelectRows);
  //m_ActiveLocs->SetDefaultCellFont( *font );
  //m_ActiveLocs->SetDefaultRowSize(20);
  //m_ActiveLocs->SetColLabelSize(m_ActiveLocs->GetDefaultRowSize());
  m_ActiveLocs->SetColLabelValue(LOC_COL_ID, wxGetApp().getMsg("id") );
  m_ActiveLocs->SetColLabelValue(LOC_COL_ADDR, _("#__") );
  m_ActiveLocs->SetColLabelValue(LOC_COL_BLOCK, wxGetApp().getMsg("block") );
  m_ActiveLocs->SetColLabelValue(LOC_COL_V, _("V__") );
  m_ActiveLocs->SetColLabelValue(LOC_COL_MODE, wxGetApp().getMsg("mode") );
  m_ActiveLocs->SetColLabelValue(LOC_COL_DESTBLOCK, wxGetApp().getMsg("destination") );
  m_ActiveLocs->AutoSizeColumns();
  m_ActiveLocs->AutoSizeRows();

  activeLocsSizer->Add(m_ActiveLocs, 1, wxGROW|wxALL|wxADJUST_MINSIZE, 2);

  TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "Creating LocPanel..." );
  m_LCPanel = new wxPanel( m_ActiveLocsPanel, -1, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
  m_LC = NULL;
  m_LC = new LC( m_LCPanel );
  activeLocsSizer->Add(m_LCPanel, 0, wxADJUST_MINSIZE, 2);


  m_StatNotebook->AddPage(m_ActiveLocsPanel, wxGetApp().getMsg("activelocs") );


  TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "Creating PT Panel..." );
  m_CVPanel = new wxScrolledWindow( m_StatNotebook, -1, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER||wxHSCROLL|wxVSCROLL );
  m_CVPanel->SetScrollbars(1, 1, 0, 0);
  m_CV = NULL;
  m_CV = new CV( m_CVPanel, wGui.getcvconf( wxGetApp().getIni() ), this );

  m_StatNotebook->AddPage(m_CVPanel, wxGetApp().getMsg("programming") );


  TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "Creating LNCV Panel..." );
  m_LNCVPanel = new wxScrolledWindow( m_StatNotebook, -1, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER||wxHSCROLL|wxVSCROLL );
  m_LNCVPanel->SetScrollbars(1, 1, 0, 0);
  m_LNCV = NULL;
  m_LNCV = new LNCV( m_LNCVPanel, this );
  m_LNCVPanel->Show(wGui.islncvtab(m_Ini));

  m_StatNotebook->AddPage(m_LNCVPanel, wxGetApp().getMsg("lncvprogramming") );


  TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "Creating PlanNotebook..." );
  m_PlanNotebook = new wxNotebook( m_PlanSplitter, -1, wxDefaultPosition, wxDefaultSize, wxNB_TOP );

  m_TraceSplitter = new wxSplitterWindow( m_Splitter, 2 );
  m_TraceSplitter->SetMinimumPaneSize(20);

  TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "Creating Serverpanel..." );
  wxBoxSizer* warningSizer = new wxBoxSizer(wxVERTICAL);
  wxPanel* warningPanel = new wxPanel(m_TraceSplitter);
  warningPanel->SetSizer(warningSizer);
  wxStaticText* warningTitle = new wxStaticText( warningPanel, -1, wxGetApp().getMsg("server") );
  m_WarningPanel = new wxTextCtrl( warningPanel, 4, _T(""),
                         wxDefaultPosition, wxDefaultSize,
                         wxTE_MULTILINE | wxTE_READONLY );
  warningSizer->Add(warningTitle, 0, wxGROW|wxALL|wxADJUST_MINSIZE, 0);
  warningSizer->Add(m_WarningPanel, 1, wxGROW|wxALL|wxADJUST_MINSIZE, 0);


  TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "Creating Controllerpanel..." );
  wxBoxSizer* monitorSizer = new wxBoxSizer(wxVERTICAL);
  wxPanel* monitorPanel = new wxPanel(m_TraceSplitter);
  monitorPanel->SetSizer(monitorSizer);
  wxStaticText* monitorTitle = new wxStaticText( monitorPanel, -1, wxGetApp().getMsg("controller") );
  m_MonitorPanel = new wxTextCtrl( monitorPanel, 4, _T(""),
                         wxDefaultPosition, wxDefaultSize,
                         wxTE_MULTILINE | wxTE_READONLY );
  monitorSizer->Add(monitorTitle, 0, wxGROW|wxALL|wxADJUST_MINSIZE, 0);
  monitorSizer->Add(m_MonitorPanel, 1, wxGROW|wxALL|wxADJUST_MINSIZE, 0);


  int pos = wSplitPanel.getmain( wGui.getsplitpanel( m_Ini) );

  m_Splitter->SplitHorizontally( m_PlanSplitter, m_TraceSplitter, pos );

  pos = wSplitPanel.getmsg( wGui.getsplitpanel( m_Ini) );

  m_TraceSplitter->SplitVertically( warningPanel, monitorPanel, pos );

  pos = wSplitPanel.getplan( wGui.getsplitpanel( m_Ini) );
  m_PlanSplitter->SplitVertically( m_StatNotebook, m_PlanNotebook, pos );

  if( !m_bLocoBook ) {
    m_StatNotebook->Show(m_bLocoBook);
    m_PlanSplitter->SetSashPosition( 1 );
  }

  TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "Show..." );

  this->Show( true );

}


void RocGuiFrame::setPlanTitle( const char* title ) {
  char* l_title = StrOp.fmt( "Rocrail: %s", title );
  SetTitle( wxString(l_title,wxConvUTF8) );
  StrOp.free( l_title );
}


void RocGuiFrame::OnNewUpdates( wxCommandEvent& event ) {
  TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "OnNewUpdates..." );
  GetToolBar()->EnableTool(ME_Update, true);
}


void RocGuiFrame::showLogo( bool show ) {
  showlogo = show;
  wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, SHOWLOGO_EVENT );
  wxPostEvent( this, event );
}

void RocGuiFrame::OnLogo( wxCommandEvent& event ) {
  if( !showlogo && wxLogo != NULL ) {
    delete wxLogo;
    wxLogo = NULL;
  }
  else {
    int cx, cy;
    GetSize(&cx, &cy);
    wxImage::AddHandler( new wxGIFHandler );
    wxLogo = new wxStaticBitmap( this, -1,
          wxBitmap( _T("wxlogo.gif"), wxBITMAP_TYPE_GIF ),
          wxPoint( cx/4, cy/4 ), wxDefaultSize, wxSUNKEN_BORDER
          );
    wxLogo->Refresh();
  }
}


void RocGuiFrame::OnAddException( wxCommandEvent& event ) {
  // Get the copied node:
  iONode node = (iONode)event.GetClientData();
  const char* text = wException.gettext( node );
  int        level = wException.getlevel( node );
  long      maxlen = 16 * 1024;
  int          len = StrOp.len(text);

  if( m_WarningPanel != NULL ) {
    long i = m_WarningPanel->GetLastPosition();
    if( i > maxlen )
      m_WarningPanel->Remove(0, len+1);
    if( level == TRCLEVEL_EXCEPTION ) {
      m_WarningPanel->SetDefaultStyle(wxTextAttr(*wxRED));
      m_WarningPanel->AppendText( wxString(text,wxConvUTF8) );
      m_WarningPanel->AppendText( _T("\n") );
    }
    else if( level == TRCLEVEL_WARNING ) {
      m_WarningPanel->SetDefaultStyle(wxTextAttr(*wxBLACK));
      m_WarningPanel->AppendText( wxString(text,wxConvUTF8) );
      m_WarningPanel->AppendText( _T("\n") );
    }
    else if( level == TRCLEVEL_CALC ) {
      m_WarningPanel->SetDefaultStyle(wxTextAttr(*wxBLUE));
      m_WarningPanel->AppendText( wxString(text,wxConvUTF8) );
      m_WarningPanel->AppendText( _T("\n") );
    }
    else if( level == TRCLEVEL_MONITOR && m_MonitorPanel != NULL) {
      long i = m_MonitorPanel->GetLastPosition();
      if( i > maxlen )
        m_MonitorPanel->Remove(0, len+1);
      m_MonitorPanel->SetDefaultStyle(wxTextAttr(*wxBLACK));
      m_MonitorPanel->AppendText( wxString(text,wxConvUTF8) );
      m_MonitorPanel->AppendText( _T("\n") );
    }
    else {
      m_WarningPanel->SetDefaultStyle(wxTextAttr(*wxBLUE));
      m_WarningPanel->AppendText( wxString(text,wxConvUTF8) );
      m_WarningPanel->AppendText( _T("\n") );
    }
  }

  // Cleanup node:
  node->base.del( node );
}

// event handlers

void RocGuiFrame::OnClearMsg( wxCommandEvent& event ) {
  m_MonitorPanel->Clear();
  m_WarningPanel->Clear();
}

void RocGuiFrame::OnShutdownRocRail( wxCommandEvent& event ) {
  int action = wxMessageDialog( this, wxGetApp().getMsg("shutdownwarning"), _T("Rocrail"), wxYES_NO | wxICON_EXCLAMATION ).ShowModal();
  if( action == wxID_NO ) {
    return;
  }
  iONode cmd = NodeOp.inst( wSysCmd.name(), NULL, ELEMENT_NODE );
  wSysCmd.setcmd( cmd, wSysCmd.shutdown );
  wxGetApp().sendToRocrail( cmd, true );
  cmd->base.del(cmd);
  wxGetApp().OnExit();
  Close(TRUE);
}

void RocGuiFrame::OnConnect( wxCommandEvent& event ) {
  iONode ini = wxGetApp().getIni();
  ConnectionDialog* dlg = new ConnectionDialog( this, ini );

  if( wxID_OK == dlg->ShowModal() ) {
    //wRRCon.sethost( wGui.getrrcon( ini ), dlg->GetValue().mb_str(wxConvUTF8) );
    wxGetApp().setHost( StrOp.dup( dlg->getHostname().mb_str(wxConvUTF8) ), dlg->getPort() );

    wxGetApp().setStayOffline( false );

    wxGetApp().sendToRocrail( (char*)NULL );

    // Initial connection.
    iONode cmd = NodeOp.inst( wModelCmd.name(), NULL, ELEMENT_NODE );
    wModelCmd.setcmd( cmd, wModelCmd.plan );
    wxGetApp().sendToRocrail( cmd );
    cmd->base.del( cmd );

	// error handling missing
	this->setOnline( true );

	}
  dlg->Destroy();
}


void RocGuiFrame::OnPlanTitle( wxCommandEvent& event ) {
  wxTextEntryDialog* dlg = new wxTextEntryDialog(this, wxGetApp().getMsg("plantitle"), _T("Rocrail"),
    wxString(wPlan.gettitle( wxGetApp().getModel() ),wxConvUTF8) );
  if( wxID_OK == dlg->ShowModal() ) {
    wPlan.settitle( wxGetApp().getModel(), dlg->GetValue().mb_str(wxConvUTF8) );
    setPlanTitle(wPlan.gettitle( wxGetApp().getModel() ));
    // Inform Rocrail.
    iONode cmd = NodeOp.inst( wModelCmd.name(), NULL, ELEMENT_NODE );
    wModelCmd.setcmd( cmd, wModelCmd.plantitle );
    wModelCmd.setval( cmd, wPlan.gettitle( wxGetApp().getModel() ) );
    wxGetApp().sendToRocrail( cmd );
    cmd->base.del( cmd );
  }
  dlg->Destroy();
}


void RocGuiFrame::Save() {
  iONode model = wxGetApp().getModel();
  char* localplan = StrOp.dup(m_LocalPlan.mb_str(wxConvUTF8));

  if( FileOp.exist( localplan ) ) {
    char* backupfile;

    backupfile = StrOp.fmt( "%s.bak",localplan );
    TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "renaming %s to %s",
        localplan, backupfile );

    /* Make Backup copy. */
    if( FileOp.exist(backupfile) )
      FileOp.remove(backupfile);

    FileOp.rename(localplan, backupfile);
    StrOp.free(backupfile);

  }

  iOFile f = FileOp.inst( localplan, OPEN_WRITE );
  if( f != NULL ) {
    long size = 0;
    char* buffer = NULL;
    char* version = StrOp.fmt( "%d.%d.%d revision %d", wGui.vmajor, wGui.vminor, wGui.patch,  wxGetApp().getSvn() );
    wPlan.setrocguiversion( model, version );
    buffer = (char*)NodeOp.base.toString( model );
    size = StrOp.len( buffer );
    TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "Saving (%ld bytes) %s...",
                 size, localplan );
    FileOp.write( f, buffer, size );
    FileOp.base.del( f );
    StrOp.free( buffer );
  }

  StrOp.free( localplan );

  wxGetApp().setStayOffline( true );
  wxGetApp().setLocalModelModified(false);
}

void RocGuiFrame::OnSave( wxCommandEvent& event ) {
  if( wxGetApp().isOffline() ) {
    if( m_LocalPlan.Len() == 0 ) {
      m_LocalPlan = wxString(wPlan.gettitle( wxGetApp().getModel() ),wxConvUTF8);
      OnSaveAs( event );
      return;
    }
    Save();
  }
  else {
    iONode cmd = NodeOp.inst( wModelCmd.name(), NULL, ELEMENT_NODE );
    wModelCmd.setcmd( cmd, wModelCmd.save );
    wxGetApp().sendToRocrail( cmd );
    cmd->base.del(cmd);
    wxGetApp().setLocalModelModified(false);
  }
}

void RocGuiFrame::OnSaveAs( wxCommandEvent& event ) {
  const char* l_openpath = wGui.getopenpath( wxGetApp().getIni() );
  wxString ms_FileExt = wxGetApp().getMsg("planfiles");
  wxFileDialog* fdlg = new wxFileDialog(this, wxGetApp().getMenu("saveplanfileas"), wxString(l_openpath,wxConvUTF8), m_LocalPlan, ms_FileExt, wxSAVE);
  if( fdlg->ShowModal() == wxID_OK ) {
    iONode model = wxGetApp().getModel();
    // Check for existence.
    wxString path = fdlg->GetPath();
    if( FileOp.exist( path.mb_str(wxConvUTF8) ) ) {
      int action = wxMessageDialog( this, wxGetApp().getMsg("fileexistwarning"), _T("Rocrail"), wxYES_NO | wxICON_EXCLAMATION ).ShowModal();
      if( action == wxID_NO ) {
        fdlg->Destroy();
        return;
      }
    }
    if( !path.Contains( _T(".xml") ) )
      path.Append( _T(".xml") );
    char* title = StrOp.dup( path.mb_str(wxConvUTF8) );
    if( wPlan.gettitle( model ) == NULL ||
        StrOp.len( wPlan.gettitle( model ) ) == 0 ||
        StrOp.equals( wPlan.gettitle( model ), "New Plan" )
        ) {
      TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "Setting PlanTitle to %s", FileOp.ripPath( title ) );
      wPlan.settitle( model, FileOp.ripPath( title ) );
    }
    StrOp.free( title );
    m_LocalPlan = path;
    Save();
  }
  fdlg->Destroy();
}



void RocGuiFrame::setLocalPlan( wxString plan ) {
  m_LocalPlan = plan;
  TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "LocalPlan [%s]", (const char*)m_LocalPlan.mb_str(wxConvUTF8) );

  if( m_LocalPlan.Len() > 0 && FileOp.exist(m_LocalPlan.mb_str(wxConvUTF8)) ) {
    iOFile f = FileOp.inst( m_LocalPlan.mb_str(wxConvUTF8), OPEN_READONLY );
    char* buffer = (char*)allocMem( FileOp.size( f ) +1 );
    FileOp.read( f, buffer, FileOp.size( f ) );
    FileOp.base.del( f );
    iODoc doc = DocOp.parse( buffer );
    if( doc != NULL ) {
      iONode node = DocOp.getRootNode( doc );
      DocOp.base.del( doc );
      wxGetApp().Callback( (obj)&wxGetApp(), node );
      wxGetApp().setStayOffline( true );
    }
    else {
      TraceOp.trc( "frame", TRCLEVEL_EXCEPTION, __LINE__, 9999, "LocalPlan [%s] is not parseable!", (const char*)m_LocalPlan.mb_str(wxConvUTF8) );
    }
  }
  else if( m_LocalPlan.Len() == 0 ) {
    iONode node = NodeOp.inst( wPlan.name(), NULL, ELEMENT_NODE );
    TraceOp.trc( "app", TRCLEVEL_WARNING, __LINE__, 9999, "created an empty model..." );
    wxGetApp().Callback( (obj)&wxGetApp(), node );
    wxGetApp().setStayOffline( true );
  }
}

void RocGuiFrame::OnOpen( wxCommandEvent& event ) {
  wxString ms_FileExt = wxGetApp().getMsg("planfiles");
  const char* l_openpath = wGui.getopenpath( wxGetApp().getIni() );
  wxFileDialog* fdlg = new wxFileDialog(this, wxGetApp().getMenu("openplanfile"), wxString(l_openpath,wxConvUTF8) , _T(""), ms_FileExt, wxOPEN);
  if( fdlg->ShowModal() == wxID_OK ) {
    setLocalPlan( fdlg->GetPath() );
    wGui.setopenpath( wxGetApp().getIni(), fdlg->GetPath().mb_str(wxConvUTF8) );
    // strip filename:
    wGui.setopenpath( wxGetApp().getIni(), FileOp.getPath(wGui.getopenpath( wxGetApp().getIni() ) ) );
  }
  fdlg->Destroy();
}

void RocGuiFrame::OnNew( wxCommandEvent& event ) {
  if( wxGetApp().isLocalModelModified() ) {
    int action = wxMessageDialog( this, wxGetApp().getMsg("notsaved"), _T("Rocrail"), wxOK | wxCANCEL | wxICON_QUESTION ).ShowModal();
    if( action == wxID_OK ) {
      Save();
    }
  }
  setLocalPlan( _T("") );

}

void RocGuiFrame::OnUpload( wxCommandEvent& event ) {
  wxString ms_FileExt = wxGetApp().getMsg("planfiles");
  wxFileDialog* fdlg = new wxFileDialog(this, wxGetApp().getMenu("openplanfile"), _T("."), _T(""), ms_FileExt, wxOPEN);
  if( fdlg->ShowModal() == wxID_OK ) {
    // Read and send to Rocrail:
    if( fdlg->GetPath().Len() > 0 ) {
      char* fname = StrOp.dup( fdlg->GetPath().mb_str(wxConvUTF8) );
      iOFile f = FileOp.inst( fname, OPEN_READONLY );
      char* buffer = (char*)allocMem( FileOp.size( f ) +1 );
      FileOp.read( f, buffer, FileOp.size( f ) );
      FileOp.base.del( f );
      // save file name in the name attribute:
      iODoc plan = DocOp.parse( buffer );
      if( plan != NULL ) {
        iONode root = DocOp.getRootNode( plan );
        wPlan.setname( root, FileOp.ripPath(fname) );

        wxGetApp().sendToRocrail( root );
        char* msg = StrOp.fmt("Plan [%s] uploaded as [%s].", wPlan.gettitle(root),  FileOp.ripPath(fname));
        TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, msg );
        DocOp.base.del( plan );
        NodeOp.base.del( root );
        wxMessageDialog( this, wxString(msg,wxConvUTF8), _T("Rocrail"), wxOK ).ShowModal();
        StrOp.free( msg );
      }
      else {
        TraceOp.trc( "frame", TRCLEVEL_WARNING, __LINE__, 9999, "Plan [%s] not valid.", FileOp.ripPath(fname) );
      }
      freeMem( buffer );
      StrOp.free(fname);
    }
  }
  fdlg->Destroy();
}

void RocGuiFrame::OnGo( wxCommandEvent& event ) {
  wxMenuItem* mi_go = menuBar->FindItem(ME_Go);

  bool power = event.IsChecked();
  mi_go->Check(power);

  iONode cmd = NodeOp.inst( wSysCmd.name(), NULL, ELEMENT_NODE );
  if( power ) {
    wSysCmd.setcmd( cmd, wSysCmd.go );
  }
  else {
    wSysCmd.setcmd( cmd, wSysCmd.stop );
  }
  wSysCmd.setinformall( cmd, True );
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);
}

void RocGuiFrame::OnStop( wxCommandEvent& event ) {
  TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "STOP" );
  iONode cmd = NodeOp.inst( wSysCmd.name(), NULL, ELEMENT_NODE );
  wSysCmd.setcmd( cmd, wSysCmd.stop );
  wSysCmd.setinformall( cmd, True );
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);
}

void RocGuiFrame::OnEmergencyBreak( wxCommandEvent& event ) {
  if( m_bAutoMode ) {
    TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "EmergencyBreak" );
    iONode cmd = NodeOp.inst( wSysCmd.name(), NULL, ELEMENT_NODE );
    wSysCmd.setcmd( cmd, wSysCmd.stop );
    wSysCmd.setinformall( cmd, True );
    wxGetApp().sendToRocrail( cmd );
    cmd->base.del(cmd);
  }
  else {
    TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "EmergencyBreak is for auto mode only." );
  }
}

void RocGuiFrame::OnInitField( wxCommandEvent& event ) {
  iONode cmd = NodeOp.inst( wModelCmd.name(), NULL, ELEMENT_NODE );
  wModelCmd.setcmd( cmd, wModelCmd.initfield );
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);
}

void RocGuiFrame::OnTxShortIDs( wxCommandEvent& event ) {
  iONode cmd = NodeOp.inst( wSysCmd.name(), NULL, ELEMENT_NODE );
  wSysCmd.setcmd( cmd, wSysCmd.txshortids );
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);
}

void RocGuiFrame::OnEditMode( wxCommandEvent& event ) {
  wxMenuItem* mi_editmode = menuBar->FindItem(ME_EditMode);
  if( mi_editmode == NULL )
    return;

  m_bEditMode = mi_editmode->IsChecked();
  TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "EditMode is %s", m_bEditMode?"true":"false" );
}

void RocGuiFrame::OnCtrlMode( wxCommandEvent& event ) {
  wxMenuItem* mi_ctrlmode = menuBar->FindItem(ME_CtrlMode);
  if( mi_ctrlmode == NULL )
    return;

  m_bEditMode = !mi_ctrlmode->IsChecked();
  TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "EditMode is %s", m_bEditMode?"true":"false" );
}


void RocGuiFrame::OnZoom25( wxCommandEvent& event ) {
  Zoom(25);
}
void RocGuiFrame::OnZoom50( wxCommandEvent& event ) {
  Zoom(50);
}
void RocGuiFrame::OnZoom75( wxCommandEvent& event ) {
  Zoom(75);
}
void RocGuiFrame::OnZoom100( wxCommandEvent& event ) {
  Zoom(100);
}

void RocGuiFrame::OnScaleComboCheck(wxCommandEvent& event)
{
  if( !event.GetString().IsNumber() ) {
    ((wxComboBox*)event.GetEventObject())->SetValue(_T(""));
  }
  else {
    int zoom = atoi(event.GetString().mb_str(wxConvUTF8));
    if( zoom < 0 || zoom > 200 )
      ((wxComboBox*)event.GetEventObject())->SetValue(_T(""));
  }
}

void RocGuiFrame::OnScaleCombo(wxCommandEvent& event)
{
  TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "Combobox string '%s' selected", event.GetString().c_str() );
  int zoom = 100;

  if( !event.GetString().IsNumber() ) {
    ((wxComboBox*)event.GetEventObject())->SetValue(_T(""));
    return;
  }
  else {
    zoom = atoi(event.GetString().mb_str(wxConvUTF8));
    if( zoom < 0 || zoom > 200 ) {
      ((wxComboBox*)event.GetEventObject())->SetValue(_T(""));
      return;
    }
  }

  if( zoom > 0 && zoom <= 200 )
    Zoom(zoom);
}

void RocGuiFrame::Zoom( int zoom ) {
  wxMenuItem* mi_zoom25  = menuBar->FindItem(ME_Zoom25);
  wxMenuItem* mi_zoom50  = menuBar->FindItem(ME_Zoom50);
  wxMenuItem* mi_zoom75  = menuBar->FindItem(ME_Zoom75);
  wxMenuItem* mi_zoom100 = menuBar->FindItem(ME_Zoom100);

  if( mi_zoom25 == NULL || mi_zoom50 == NULL || mi_zoom75 == NULL || mi_zoom100 == NULL )
    return;

  mi_zoom25->Check( zoom == 25 );
  mi_zoom50->Check( zoom == 50 );
  mi_zoom75->Check( zoom == 75 );
  mi_zoom100->Check( zoom == 100 );


  double scale = zoom;
  scale = scale / 100.0;
  m_Scale = scale;

  int pages = m_PlanNotebook->GetPageCount();
  for( int i = 0; i < pages; i++ ) {
    BasePanel* p = (BasePanel*)m_PlanNotebook->GetPage(i);
    p->reScale( scale );
  }

  TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "scale is %lf", scale );

  m_ScaleComboBox->SetValue( wxString::Format( _T("%d"), (int)(m_Scale*100)) );

}


void RocGuiFrame::OnFill( wxCommandEvent& event ) {
  wxMenuItem* mi_fill = menuBar->FindItem(ME_Fill);
  m_bFill = mi_fill->IsChecked();

  int pages = m_PlanNotebook->GetPageCount();
  for( int i = 0; i < pages; i++ ) {
    BasePanel* p = (BasePanel*)m_PlanNotebook->GetPage(i);
    p->reScale( m_Scale );
  }
}


void RocGuiFrame::OnShowID( wxCommandEvent& event ) {
  wxMenuItem* mi_showid = menuBar->FindItem(ME_ShowID);
  m_bShowID = mi_showid->IsChecked();

  int pages = m_PlanNotebook->GetPageCount();
  for( int i = 0; i < pages; i++ ) {
    BasePanel* p = (BasePanel*)m_PlanNotebook->GetPage(i);
    p->reScale( m_Scale );
  }
}


void RocGuiFrame::OnFullScreen( wxCommandEvent& event ) {
  wxMenuItem* mi_fullscreen = menuBar->FindItem(ME_FullScreen);

  if( mi_fullscreen->IsChecked() ) {
    TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "full screen on" );
    ShowFullScreen(true, wxFULLSCREEN_NOTOOLBAR | wxFULLSCREEN_NOSTATUSBAR | wxFULLSCREEN_NOBORDER | wxFULLSCREEN_NOCAPTION );
  }
  else {
    TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "full screen off" );
    ShowFullScreen(false);
  }
}


void RocGuiFrame::OnLocoBook( wxCommandEvent& event ) {
  wxMenuItem* mi = menuBar->FindItem(ME_LocoBook);
  m_bLocoBook = mi->IsChecked();
  if(m_bLocoBook) {
    int pos = wSplitPanel.getplan( wGui.getsplitpanel( m_Ini) );
    m_PlanSplitter->SplitVertically( m_StatNotebook, m_PlanNotebook, pos );

  }
  else {
    m_PlanSplitter->Unsplit( m_StatNotebook );
  }
}

void RocGuiFrame::OnBackColor( wxCommandEvent& event ) {
  wxColourDialog* dlg = new wxColourDialog(this);
  if( wxID_OK == dlg->ShowModal() ) {
    wxColour &color = dlg->GetColourData().GetColour();

    wPlanPanel.setred( wGui.getplanpanel(m_Ini), (int)color.Red() );
    wPlanPanel.setgreen( wGui.getplanpanel(m_Ini), (int)color.Green() );
    wPlanPanel.setblue( wGui.getplanpanel(m_Ini), (int)color.Blue() );

    /* Notify Panels. */
    int pages = m_PlanNotebook->GetPageCount();
    for( int i = 0; i < pages; i++ ) {
      BasePanel* p = (BasePanel*)m_PlanNotebook->GetPage(i);
      p->SetBackgroundColor(color.Red(),color.Green(),color.Blue());
      p->reScale( m_Scale );
    }
  }
  dlg->Destroy();
}

void RocGuiFrame::OnRaster( wxCommandEvent& event ) {
  wxMenuItem* mi_raster = menuBar->FindItem(ME_Raster);
  m_bRaster = mi_raster->IsChecked();

  int pages = m_PlanNotebook->GetPageCount();
  for( int i = 0; i < pages; i++ ) {
    BasePanel* p = (BasePanel*)m_PlanNotebook->GetPage(i);
    p->reScale( m_Scale );
  }
}

void RocGuiFrame::OnEditLocs( wxCommandEvent& event ) {
  LocDialog* locdialog = new LocDialog(this, (iONode)NULL );
  if( wxID_OK == locdialog->ShowModal() ) {
    /* Notify Notebook. */
    wxCommandEvent evt( wxEVT_COMMAND_MENU_SELECTED, ME_INITACTIVELOCS );
    wxPostEvent( this, evt );
  }
  locdialog->Destroy();
}

void RocGuiFrame::OnEditCars( wxCommandEvent& event ) {
  CarDlg* dlg = new CarDlg(this, NULL );
  if( wxID_OK == dlg->ShowModal() ) {
    /* Notify Notebook. */
  }
  dlg->Destroy();
}

void RocGuiFrame::OnEditOperators( wxCommandEvent& event ) {
  OperatorDlg* dlg = new OperatorDlg(this, NULL);
  if( wxID_OK == dlg->ShowModal() ) {
    /* Notify Notebook. */
  }
  dlg->Destroy();
}

void RocGuiFrame::OnEditWaybills( wxCommandEvent& event ) {
  WaybillDlg* dlg = new WaybillDlg(this, NULL );
  if( wxID_OK == dlg->ShowModal() ) {
    /* Notify Notebook. */
  }
  dlg->Destroy();
}

void RocGuiFrame::OnEditTurnouts( wxCommandEvent& event ) {
  SwitchDialog*  dlg = new SwitchDialog(this, (iONode)NULL );
  if( wxID_OK == dlg->ShowModal() ) {
    /* Notify RocRail. */
  }
  dlg->Destroy();
}

void RocGuiFrame::OnEditTurntables( wxCommandEvent& event ) {
  TurntableDialog*  dlg = new TurntableDialog(this, (iONode)NULL );
  if( wxID_OK == dlg->ShowModal() ) {
    /* Notify RocRail. */
  }
  dlg->Destroy();
}

void RocGuiFrame::OnEditSignals( wxCommandEvent& event ) {
  SignalDialog*  dlg = new SignalDialog(this, (iONode)NULL );
  if( wxID_OK == dlg->ShowModal() ) {
    /* Notify RocRail. */
  }
  dlg->Destroy();
}

void RocGuiFrame::OnEditOutputs( wxCommandEvent& event ) {
  OutputDialog*  dlg = new OutputDialog(this, (iONode)NULL );
  if( wxID_OK == dlg->ShowModal() ) {
    /* Notify RocRail. */
    int pages = m_PlanNotebook->GetPageCount();
    for( int i = 0; i < pages; i++ ) {
      BasePanel* p = (BasePanel*)m_PlanNotebook->GetPage(i);
      p->reScale( m_Scale );
    }
  }
  dlg->Destroy();
}

void RocGuiFrame::OnEditSchedules( wxCommandEvent& event ) {
  ScheduleDialog*  dlg = new ScheduleDialog(this, (iONode)NULL );
  if( wxID_OK == dlg->ShowModal() ) {
    /* Notify RocRail. */
  }
  dlg->Destroy();
}

void RocGuiFrame::OnEditLocations( wxCommandEvent& event ) {
  LocationsDialog*  dlg = new LocationsDialog(this, (iONode)NULL );
  if( wxID_OK == dlg->ShowModal() ) {
    /* Notify RocRail. */
  }
  dlg->Destroy();
}

void RocGuiFrame::OnEditMVTrack( wxCommandEvent& event ) {
  iONode model = wxGetApp().getModel();
  iONode mv = wPlan.getmv( model );
  if( mv == NULL ) {
    mv = NodeOp.inst( wMVTrack.name(), model, ELEMENT_NODE );
    NodeOp.addChild( model, mv );
  }
  MVTrackDlg*  dlg = new MVTrackDlg(this, mv );
  if( wxID_OK == dlg->ShowModal() ) {
    /* Notify RocRail. */
    iONode cmd = NodeOp.inst( wModelCmd.name(), NULL, ELEMENT_NODE );
    wModelCmd.setcmd( cmd, wModelCmd.modify );
    NodeOp.addChild( cmd, (iONode)mv->base.clone( mv ) );
    wxGetApp().sendToRocrail( cmd );
    cmd->base.del(cmd);
  }
  dlg->Destroy();
}

void RocGuiFrame::OnEditTimedActions( wxCommandEvent& event ) {
  TimedActions*  dlg = new TimedActions(this );
  if( wxID_OK == dlg->ShowModal() ) {
    /* Notify RocRail. */
  }
  dlg->Destroy();
}

void RocGuiFrame::OnEditSensors( wxCommandEvent& event ) {
  FeedbackDialog*  dlg = new FeedbackDialog(this, (iONode)NULL );
  if( wxID_OK == dlg->ShowModal() ) {
    /* Notify RocRail. */
    int pages = m_PlanNotebook->GetPageCount();
    for( int i = 0; i < pages; i++ ) {
       BasePanel* p = (BasePanel*)m_PlanNotebook->GetPage(i);
      p->reScale( m_Scale );
    }
  }
  dlg->Destroy();
}

void RocGuiFrame::OnEditRoutes( wxCommandEvent& event ) {
  RouteDialog*  dlg = new RouteDialog(this, (iONode)NULL );
  if( wxID_OK == dlg->ShowModal() ) {
    /* Notify RocRail. */
  }
  dlg->Destroy();
}

void RocGuiFrame::OnEditBlocks( wxCommandEvent& event ) {
  BlockDialog*  dlg = new BlockDialog(this, (iONode)NULL );
  if( wxID_OK == dlg->ShowModal() ) {
    /* Notify RocRail. */
  }
  dlg->Destroy();
}

void RocGuiFrame::OnEditBlockGroups( wxCommandEvent& event ) {
  BlockGroupingDialog*  dlg = new BlockGroupingDialog(this, (iONode)NULL, true );
  if( wxID_OK == dlg->ShowModal() ) {
    /* Notify RocRail. */
  }
  dlg->Destroy();
}

void RocGuiFrame::OnUhl63350( wxCommandEvent& event ) {
  Uhl633x0Dlg* dlg = new Uhl633x0Dlg(this);
  if( wxID_OK == dlg->ShowModal() ) {
    /* Notify RocRail. */
  }
  dlg->Destroy();
}

void RocGuiFrame::OnLocoIO( wxCommandEvent& event ) {
  m_LocoIO = new LocoIO(this);
  if( wxID_OK == m_LocoIO->ShowModal() ) {
    /* Notify RocRail. */
  }
  m_LocoIO->Destroy();
  m_LocoIO = NULL;
}

void RocGuiFrame::OnDTOpSw( wxCommandEvent& event ) {
  m_DTOpSw = new DTOpSwDlg(this);
  if( wxID_OK == m_DTOpSw->ShowModal() ) {
    /* Notify RocRail. */
  }
  m_DTOpSw->Destroy();
  m_DTOpSw = NULL;
}

void RocGuiFrame::OnOpenDecoder( wxCommandEvent& event ) {
  /*
  m_OpenDecoder = new OpenDecoderDlg(this);
  m_OpenDecoder->ShowModal();
  m_OpenDecoder->Destroy();
  m_OpenDecoder = NULL;
  */
}

void RocGuiFrame::setOnline( bool online ) {
  GetToolBar()->EnableTool(ME_New, !online);
  GetToolBar()->EnableTool(ME_Open, !online);
  GetToolBar()->EnableTool(ME_Upload, online);
}


void RocGuiFrame::OnMenu( wxMenuEvent& event ) {
  bool l_bOffline = wxGetApp().isOffline();

  wxMenuItem* mi = menuBar->FindItem(ME_New);
  if( mi != NULL ) mi->Enable( l_bOffline );


  GetToolBar()->EnableTool(ME_Open, l_bOffline);

  mi = menuBar->FindItem(ME_Open);
  if( mi != NULL ) mi->Enable( l_bOffline );
  //mi = menuBar->FindItem(ME_SaveAs);
  //if( mi != NULL ) mi->Enable( l_bOffline );
  mi = menuBar->FindItem(ME_Upload);
  if( mi != NULL ) mi->Enable( !l_bOffline );
  mi = menuBar->FindItem(ME_ShutdownRocRail);
  if( mi != NULL ) mi->Enable( (!l_bOffline && !m_bServerConsoleMode) );

  mi = menuBar->FindItem(ME_Go);
  if( mi != NULL ) mi->Enable( !l_bOffline );
  mi = menuBar->FindItem(ME_Stop);
  if( mi != NULL ) mi->Enable( !l_bOffline );
  mi = menuBar->FindItem(ME_InitField);
  if( mi != NULL ) mi->Enable( !l_bOffline );

  mi = menuBar->FindItem(ME_AutoMode);
  if( mi != NULL ) mi->Enable( !l_bOffline );
  mi = menuBar->FindItem(ME_AutoGo);
  if( mi != NULL ) mi->Enable( !l_bOffline && m_bAutoMode );
  mi = menuBar->FindItem(ME_AutoResume);
  if( mi != NULL ) mi->Enable( !l_bOffline && m_bAutoMode );
  mi = menuBar->FindItem(ME_AutoStop);
  if( mi != NULL ) mi->Enable( !l_bOffline && m_bAutoMode );
  mi = menuBar->FindItem(ME_AutoReset);
  if( mi != NULL ) mi->Enable( !l_bOffline && m_bAutoMode );
  mi = menuBar->FindItem(ME_AutoSoftReset);
  if( mi != NULL ) mi->Enable( !l_bOffline && m_bAutoMode );


  mi = menuBar->FindItem(ME_Undo);
  if( mi != NULL ) mi->Enable( wxGetApp().hasUndoItems() );

  // Disable table editing in case of automode:
  mi = menuBar->FindItem(ME_EditLocs);
  if( mi != NULL ) mi->Enable( !m_bAutoMode || !wxGetApp().isRestrictedEdit() );
  mi = menuBar->FindItem(ME_EditCars);
  if( mi != NULL ) mi->Enable( !m_bAutoMode || !wxGetApp().isRestrictedEdit() );
  mi = menuBar->FindItem(ME_EditTurnouts);
  if( mi != NULL ) mi->Enable( !m_bAutoMode || !wxGetApp().isRestrictedEdit() );
  mi = menuBar->FindItem(ME_EditSensors);
  if( mi != NULL ) mi->Enable( !m_bAutoMode || !wxGetApp().isRestrictedEdit() );
  mi = menuBar->FindItem(ME_EditRoutes);
  if( mi != NULL ) mi->Enable( !m_bAutoMode || !wxGetApp().isRestrictedEdit() );
  mi = menuBar->FindItem(ME_EditBlocks);
  if( mi != NULL ) mi->Enable( !m_bAutoMode || !wxGetApp().isRestrictedEdit() );
  mi = menuBar->FindItem(ME_EditBlockGroups);
  if( mi != NULL ) mi->Enable( !m_bAutoMode || !wxGetApp().isRestrictedEdit() );
  mi = menuBar->FindItem(ME_EditSignals);
  if( mi != NULL ) mi->Enable( !m_bAutoMode || !wxGetApp().isRestrictedEdit() );
  mi = menuBar->FindItem(ME_EditOutputs);
  if( mi != NULL ) mi->Enable( !m_bAutoMode || !wxGetApp().isRestrictedEdit() );
  mi = menuBar->FindItem(ME_EditSchedules);
  if( mi != NULL ) mi->Enable( !m_bAutoMode || !wxGetApp().isRestrictedEdit() );
  mi = menuBar->FindItem(ME_EditLocations);
  if( mi != NULL ) mi->Enable( !m_bAutoMode || !wxGetApp().isRestrictedEdit() );
  mi = menuBar->FindItem(ME_EditTurntables);
  if( mi != NULL ) mi->Enable( !m_bAutoMode || !wxGetApp().isRestrictedEdit() );

  mi = menuBar->FindItem(ME_InitField);
  if( mi != NULL ) mi->Enable( !m_bAutoMode );

  mi = menuBar->FindItem(ME_EditMode);
  if( mi != NULL ) mi->Enable( !m_bAutoMode );
  mi = menuBar->FindItem(ME_CtrlMode);
  if( mi != NULL ) mi->Enable( !m_bAutoMode );
  mi = menuBar->FindItem(ME_AddPanel);
  if( mi != NULL ) mi->Enable( !m_bAutoMode );
  mi = menuBar->FindItem(ME_PanelProps);
  if( mi != NULL ) mi->Enable( !m_bAutoMode );

  mi = menuBar->FindItem(ME_LangEnglish);
  if( mi != NULL )
    mi->Check( StrOp.equals( wGui.lang_english, wGui.getlang( wxGetApp().getIni() ) ) );
  mi = menuBar->FindItem(ME_LangGerman);
  if( mi != NULL )
    mi->Check( StrOp.equals( wGui.lang_german, wGui.getlang( wxGetApp().getIni() ) ) );
  mi = menuBar->FindItem(ME_LangDutch);
  if( mi != NULL )
    mi->Check( StrOp.equals( wGui.lang_dutch, wGui.getlang( wxGetApp().getIni() ) ) );
  mi = menuBar->FindItem(ME_LangSwedisch);
  if( mi != NULL )
    mi->Check( StrOp.equals( wGui.lang_swedisch, wGui.getlang( wxGetApp().getIni() ) ) );
  mi = menuBar->FindItem(ME_LangFrench);
  if( mi != NULL )
    mi->Check( StrOp.equals( wGui.lang_french, wGui.getlang( wxGetApp().getIni() ) ) );
  mi = menuBar->FindItem(ME_LangTurkish);
  if( mi != NULL )
    mi->Check( StrOp.equals( wGui.lang_turkish, wGui.getlang( wxGetApp().getIni() ) ) );
  mi = menuBar->FindItem(ME_LangSpanish);
  if( mi != NULL )
    mi->Check( StrOp.equals( wGui.lang_spanish, wGui.getlang( wxGetApp().getIni() ) ) );
  mi = menuBar->FindItem(ME_LangItalien);
  if( mi != NULL )
    mi->Check( StrOp.equals( wGui.lang_italien, wGui.getlang( wxGetApp().getIni() ) ) );
  mi = menuBar->FindItem(ME_LangDanish);
  if( mi != NULL )
    mi->Check( StrOp.equals( wGui.lang_danish, wGui.getlang( wxGetApp().getIni() ) ) );
  mi = menuBar->FindItem(ME_LangCzech);
  if( mi != NULL )
    mi->Check( StrOp.equals( wGui.lang_czech, wGui.getlang( wxGetApp().getIni() ) ) );
  mi = menuBar->FindItem(ME_LangBosnian);
  if( mi != NULL )
    mi->Check( StrOp.equals( wGui.lang_bosnian, wGui.getlang( wxGetApp().getIni() ) ) );

  mi = menuBar->FindItem(ME_RocrailIni);
  if( mi != NULL )
    mi->Enable( !l_bOffline && !m_bAutoMode );

  mi = menuBar->FindItem(ME_Upload);
  if( mi != NULL )
    mi->Enable( !l_bOffline && !m_bAutoMode );


  if( !m_bAutoMode ) {
    mi = menuBar->FindItem(ME_EditMode);
    if( mi != NULL ) {
      mi->Check( m_bEditMode );
      mi->Enable( !wxGetApp().isModView() );
    }

    mi = menuBar->FindItem(ME_PanelProps);
    mi->Enable( !wxGetApp().isModView() );

    mi = menuBar->FindItem(ME_AddPanel);
    mi->Enable( !wxGetApp().isModView() );

    mi = menuBar->FindItem(ME_CtrlMode);
    if( mi != NULL )
      mi->Check( !m_bEditMode );
  }

  mi = menuBar->FindItem(ME_Connect);
  if( mi != NULL )
    mi->Enable( !wxGetApp().isStayOffline() );

  wxMenuItem* mi_zoom25  = menuBar->FindItem(ME_Zoom25);
  wxMenuItem* mi_zoom50  = menuBar->FindItem(ME_Zoom50);
  wxMenuItem* mi_zoom75  = menuBar->FindItem(ME_Zoom75);
  wxMenuItem* mi_zoom100 = menuBar->FindItem(ME_Zoom100);
  mi_zoom25->Check( m_Scale == 0.25 );
  mi_zoom50->Check( m_Scale == 0.50 );
  mi_zoom75->Check( m_Scale == 0.75 );
  mi_zoom100->Check( m_Scale == 1.00 );

  wxMenuItem* mi_locobook  = menuBar->FindItem(ME_LocoBook);
  mi_locobook->Check( m_bLocoBook );
  wxMenuItem* mi_fill  = menuBar->FindItem(ME_Fill);
  mi_fill->Check( m_bFill );
  wxMenuItem* mi_showid  = menuBar->FindItem(ME_ShowID);
  mi_showid->Check( m_bShowID );
  wxMenuItem* mi_raster  = menuBar->FindItem(ME_Raster);
  mi_raster->Check( m_bRaster );
}

void RocGuiFrame::OnAutoMode( wxCommandEvent& event ) {
  wxMenuItem* mi_automode = menuBar->FindItem(ME_AutoMode);
  if( mi_automode == NULL )
    return;

  bool autoMode;

  autoMode = event.IsChecked();
  mi_automode->Check(autoMode);


  m_bAutoMode = autoMode?True:False;
  if( m_bEditMode && m_bAutoMode)
    m_bEditMode = False;

  iONode cmd = NodeOp.inst( wAutoCmd.name(), NULL, ELEMENT_NODE );
  if( !m_bAutoMode ) {
    wAutoCmd.setcmd( cmd, wAutoCmd.stop );
    wxGetApp().sendToRocrail( cmd );
  }
  wAutoCmd.setcmd( cmd, m_bAutoMode ? wAutoCmd.on:wAutoCmd.off );
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);
}

void RocGuiFrame::OnAutoGo( wxCommandEvent& event ) {
  int action = wxMessageDialog( this, wxGetApp().getMsg("startallwarning"), _T("Rocrail"), wxYES_NO | wxICON_EXCLAMATION ).ShowModal();
  if( action == wxID_NO ) {
    return;
  }
  iONode cmd = NodeOp.inst( wAutoCmd.name(), NULL, ELEMENT_NODE );
  wAutoCmd.setcmd( cmd, wAutoCmd.start );
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);
}

void RocGuiFrame::OnAutoResume( wxCommandEvent& event ) {
  int action = wxMessageDialog( this, wxGetApp().getMsg("resumeallwarning"), _T("Rocrail"), wxYES_NO | wxICON_EXCLAMATION ).ShowModal();
  if( action == wxID_NO ) {
    return;
  }
  iONode cmd = NodeOp.inst( wAutoCmd.name(), NULL, ELEMENT_NODE );
  wAutoCmd.setcmd( cmd, wAutoCmd.resume );
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);
}

void RocGuiFrame::OnAutoStop( wxCommandEvent& event ) {
  int action = wxMessageDialog( this, wxGetApp().getMsg("stopallwarning"), _T("Rocrail"), wxYES_NO | wxICON_EXCLAMATION ).ShowModal();
  if( action == wxID_NO ) {
    return;
  }
  iONode cmd = NodeOp.inst( wAutoCmd.name(), NULL, ELEMENT_NODE );
  wAutoCmd.setcmd( cmd, wAutoCmd.stop );
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);
}

void RocGuiFrame::OnAutoReset( wxCommandEvent& event ) {
  int action = wxMessageDialog( this, wxGetApp().getMsg("resetallwarning"), _T("Rocrail"), wxYES_NO | wxICON_EXCLAMATION ).ShowModal();
  if( action == wxID_NO ) {
    return;
  }
  iONode cmd = NodeOp.inst( wAutoCmd.name(), NULL, ELEMENT_NODE );
  wAutoCmd.setcmd( cmd, wAutoCmd.reset );
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);
}

void RocGuiFrame::OnAutoSoftReset( wxCommandEvent& event ) {
  iONode cmd = NodeOp.inst( wAutoCmd.name(), NULL, ELEMENT_NODE );
  wAutoCmd.setcmd( cmd, wAutoCmd.softreset );
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);
}

void RocGuiFrame::OnQuit(wxCommandEvent& WXUNUSED(event)) {
  /*wxGetApp().OnExit();*/
  Close(TRUE);
}

void RocGuiFrame::OnClose(wxCloseEvent& event) {
  if( wxGetApp().isLocalModelModified() ) {
    int action = wxMessageDialog( this, wxGetApp().getMsg("notsaved"), _T("Rocrail"), wxYES_NO | wxCANCEL | wxICON_EXCLAMATION ).ShowModal();
    if( action == wxID_CANCEL ) {
      return;
    }

    if( action == wxID_YES ) {
      if( wxGetApp().isOffline() )
        Save();
      else {
        iONode cmd = NodeOp.inst( wModelCmd.name(), NULL, ELEMENT_NODE );
        wModelCmd.setcmd( cmd, wModelCmd.save );
        wxGetApp().sendToRocrail( cmd );
        cmd->base.del(cmd);

      }
    }

  }

  MapOp.clear(m_LocDlgMap);
  while( ListOp.size(m_LocCtrlList) > 0 ) {
    LocControlDialog* dlg = (LocControlDialog*)ListOp.get(m_LocCtrlList, 0);
    TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "inform loc control dialog to save position..." );
    dlg->Close();
  }


  wxGetApp().OnExit();
  event.Skip();
}


bool m_isDragging = false;
void RocGuiFrame::OnMouse(wxMouseEvent& event) {
  m_isDragging = event.Dragging();
  TraceOp.trc( "frame", TRCLEVEL_DEBUG, __LINE__, 9999, "Dragging=%d", m_isDragging );
}

void RocGuiFrame::OnSize(wxSizeEvent& event) {
  TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "wxSizeEvent" );
  // Multiple events are posted when resizing window.
  // Call base.
  if( !m_isDragging )
    wxFrame::OnSize( event );
}


void RocGuiFrame::OnAbout(wxCommandEvent& WXUNUSED(event)) {
  InfoDialog* dlg = new InfoDialog(this);
  dlg->ShowModal();
  dlg->Destroy();
}

void RocGuiFrame::OnUpdate(wxCommandEvent& WXUNUSED(event)) {
  Softwareupdates* dlg = new Softwareupdates(this);
  dlg->ShowModal();
  dlg->Destroy();
}

void RocGuiFrame::OnHelp(wxCommandEvent& WXUNUSED(event)) {
  wxLaunchDefaultBrowser(wxGetApp().getMsg("rocrail_help"), wxBROWSER_NEW_WINDOW );
}

void RocGuiFrame::OnRUG(wxCommandEvent& WXUNUSED(event)) {
  wxLaunchDefaultBrowser(wxGetApp().getMsg("rug"), wxBROWSER_NEW_WINDOW );
}

void RocGuiFrame::OnService(wxCommandEvent& WXUNUSED(event)) {
  wxLaunchDefaultBrowser(wxGetApp().getMsg("rocrail_consrv"), wxBROWSER_NEW_WINDOW );
}

void RocGuiFrame::OnBug(wxCommandEvent& WXUNUSED(event)) {
  wxLaunchDefaultBrowser(wxGetApp().getMsg("rocrail_bug"), wxBROWSER_NEW_WINDOW );
}

void RocGuiFrame::OnFeature(wxCommandEvent& WXUNUSED(event)) {
  wxLaunchDefaultBrowser(wxGetApp().getMsg("rocrail_feature"), wxBROWSER_NEW_WINDOW );
}

void RocGuiFrame::OnOperatorDlg(wxCommandEvent& event){
  OperatorDlg* dlg = new OperatorDlg(this, NULL);
  dlg->ShowModal();
}

void RocGuiFrame::OnLcDlg(wxCommandEvent& event){
  LocControlDialog* dlg = new LocControlDialog(this, m_LocCtrlList, m_LocDlgMap, m_LocID);
  dlg->Show(TRUE);

  if( m_LocID != NULL && StrOp.len(m_LocID) > 0 ) {
    char* pos = (char*)MapOp.get( m_LocDlgMap, m_LocID );
    if( pos != NULL ) {
      TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "previous position [%s] for [%s]", pos, m_LocID );

      iOStrTok tok = StrTokOp.inst( pos, ',' );
      const char* x = StrTokOp.nextToken(tok);
      const char* y = StrTokOp.nextToken(tok);
      StrTokOp.base.del( tok );
      dlg->Move( atoi(x), atoi(y) );

    }
  }
}

void RocGuiFrame::OnMIC(wxCommandEvent& event){
  LocSelDlg* dlg = new LocSelDlg(this, NULL, true );
  dlg->ShowModal();
  dlg->Destroy();
}

void RocGuiFrame::OnLocEvent( wxCommandEvent& event ) {
  UpdateActiveLocs( event );
}

void RocGuiFrame::OnAutoEvent( wxCommandEvent& event ) {
  // Get copied node:
  iONode node = (iONode)event.GetClientData();
  Boolean autoMode = StrOp.equals( wAutoCmd.on, wAutoCmd.getcmd( node ) );
  m_bAutoMode = autoMode;
  wxMenuItem* mi = menuBar->FindItem(ME_AutoMode);
  if( mi != NULL ) mi->Check(autoMode);

  GetToolBar()->ToggleTool(ME_AutoMode, m_bAutoMode);

  // Cleanup node:
  node->base.del( node );
}

void RocGuiFrame::OnStateEvent( wxCommandEvent& event ) {
  // Get copied node:
  iONode node = (iONode)event.GetClientData();

  Boolean power = wState.ispower( node );
  Boolean console = wState.isconsolemode( node );
  m_bServerConsoleMode = console;

  GetToolBar()->ToggleTool(ME_Go, power);

  wxMenuItem* mi = menuBar->FindItem(ME_ShutdownRocRail);
  if( mi != NULL ) mi->Enable(!console);

  mi = menuBar->FindItem(ME_Go);
  if( mi != NULL ) mi->Check(power);

  int state = (wState.ispower( node )?0x01:0x00) |
              (wState.istrackbus( node )?0x02:0x00) |
              (wState.issensorbus( node )?0x04:0x00) |
              (wState.isaccessorybus( node )?0x08:0x00);

  TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "new state 0x%04X %s", state, console?"(consolemode)":"" );
  m_StatusBar->Update( state );

  // Cleanup node:
  node->base.del( node );
}

void RocGuiFrame::OnSystemEvent( wxCommandEvent& event ) {
  // Get copied node:
  iONode node = (iONode)event.GetClientData();
  Boolean stop = StrOp.equals( wSysCmd.stop, wSysCmd.getcmd( node ) );
  Boolean go = StrOp.equals( wSysCmd.go, wSysCmd.getcmd( node ) );

  if( stop ) {
    GetToolBar()->ToggleTool(ME_Go, false);
    wxMenuItem* mi = menuBar->FindItem(ME_Go);
    if( mi != NULL ) mi->Check(false);
  }
  else if( go ) {
    GetToolBar()->ToggleTool(ME_Go, true);
    wxMenuItem* mi = menuBar->FindItem(ME_Go);
    if( mi != NULL ) mi->Check(true);
  }


  // Cleanup node:
  node->base.del( node );
}

void RocGuiFrame::OnZLevelEventAdd( wxCommandEvent& event ) {
  // Get copied node:
  iONode zlevel = (iONode)event.GetClientData();
  InitNotebookPage( zlevel );
}

void RocGuiFrame::OnZLevelEventRemove( wxCommandEvent& event ) {
  // Get copied node:
  iONode zlevel = (iONode)event.GetClientData();
  RemoveNotebookPage( zlevel );
}

static SwCtrlDlg* swDlg = NULL;
void RocGuiFrame::OnSwDlg(wxCommandEvent& event){
  swDlg = new SwCtrlDlg(this);
  //swDlg->ShowModal();
  swDlg->Show(TRUE);
}

static RouteCtrlDlg* routeDlg = NULL;
void RocGuiFrame::OnRouteDlg(wxCommandEvent& event){
  routeDlg = new RouteCtrlDlg(this);
  //routeDlg->ShowModal();
  routeDlg->Show(TRUE);
}

void RocGuiFrame::setLocID( const char* locid ) {
  int rowcnt = m_ActiveLocs->GetNumberRows();
  for( int i = 0; i  < rowcnt; i++ ) {
    wxString str = m_ActiveLocs->GetCellValue( i, 0 );
    if( StrOp.equals( (const char*)str.mb_str(wxConvUTF8), locid ) ) {
      wxGridEvent event(-1,0,NULL,i);
      OnCellLeftClick(event);
      break;
    }
  }

}

void RocGuiFrame::OnCellLeftDClick( wxGridEvent& event ){
  TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "OnCellLeftDClick..." );
  OnCellLeftClick(event);
  OnLcDlg(event);
}

void RocGuiFrame::OnCellLeftClick( wxGridEvent& event ){
  if( event.GetEventObject() == m_ActiveLocs || event.GetEventObject() == NULL ) {
    m_iLcRowSelection = event.GetRow();
    m_ActiveLocs->SelectRow(event.GetRow());
    m_ActiveLocs->MakeCellVisible( event.GetRow(), 0 );
    wxString str = m_ActiveLocs->GetCellValue( event.GetRow(), 0 );
    StrOp.free(m_LocID);
    m_LocID = StrOp.dup((const char*)str.mb_str(wxConvUTF8));
    TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "OnCellLeftClick %s", (const char*)str.mb_str(wxConvUTF8) );
    iONode lc = findLoc(str.mb_str(wxConvUTF8));

    m_LC->setLocProps( lc );

    if( lc != NULL ) {
      wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, UPDATE_LOC_IMAGE_EVENT );
      event.SetClientData( NodeOp.base.clone( lc ) );
      wxPostEvent( this, event );
    }

  }
  else {
    if(m_CV!=NULL)
      m_CV->OnGrid( event );
  }

  event.Skip(true);
}


void RocGuiFrame::UpdateLocImage( wxCommandEvent& event ){
  // Get copied node:
  iONode lc = (iONode)event.GetClientData();

  if( lc != NULL ) {
    if( wLoc.getimage( lc ) != NULL && StrOp.len(wLoc.getimage( lc )) > 0 ) {
      wxBitmapType bmptype = wxBITMAP_TYPE_XPM;
      if( StrOp.endsWithi( wLoc.getimage( lc ), ".gif" ) )
        bmptype = wxBITMAP_TYPE_GIF;
      else if( StrOp.endsWithi( wLoc.getimage( lc ), ".png" ) )
        bmptype = wxBITMAP_TYPE_PNG;

      TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "***** UpdateLocImage %s", wLoc.getimage( lc ) );

      const char* imagepath = wGui.getimagepath(m_Ini);
      static char pixpath[256];
      StrOp.fmtb( pixpath, "%s%c%s", imagepath, SystemOp.getFileSeparator(), FileOp.ripPath( wLoc.getimage( lc ) ) );

      if( FileOp.exist(pixpath))
        m_LocImage->SetBitmapLabel( wxBitmap(wxString(pixpath,wxConvUTF8), bmptype) );
      else {
        m_LocImage->SetBitmapLabel( wxBitmap(nopict_xpm) );
        // request the image from server:
        iONode node = NodeOp.inst( wDataReq.name(), NULL, ELEMENT_NODE );
        wDataReq.setid( node, wLoc.getid(lc) );
        wDataReq.setcmd( node, wDataReq.get );
        wDataReq.settype( node, wDataReq.image );
        wDataReq.setfilename( node, wLoc.getimage(lc) );
        wxGetApp().sendToRocrail( node );
      }
    }
    else
      m_LocImage->SetBitmapLabel( wxBitmap(nopict_xpm) );

    m_LocImage->Refresh();

    m_LocImage->SetToolTip(wxString(wLoc.getid( lc ),wxConvUTF8) + _T(" ") + wxString(wLoc.getdesc( lc ),wxConvUTF8));

    // Cleanup node:
    NodeOp.base.del( lc );
  }
}


void RocGuiFrame::OnCellRightClick( wxGridEvent& event ) {
  if( event.GetEventObject() == m_ActiveLocs ) {
    OnCellLeftClick(event);
    wxString str = m_ActiveLocs->GetCellValue( event.GetRow(), 0 );

    TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "OnCellRightClick %s", m_LocID );
    wxMenu menu( str );
    menu.Append( ME_GridLocGo, wxGetApp().getMenu("start"), wxGetApp().getTip("start"));
    wxMenuItem *mi = menu.FindItem( ME_GridLocGo );
    mi->Enable( isAutoMode() );
    menu.Append( ME_GridLocStop, wxGetApp().getMenu("stop"), wxGetApp().getTip("stop"));
    menu.AppendSeparator();
    menu.Append( ME_GridLocReset, wxGetApp().getMenu("softresetall"), wxGetApp().getTip("softresetall") );
    menu.Append( ME_GridLocSwap, wxGetApp().getMenu("swapplacing"), wxGetApp().getTip("swapplacing") );
    menu.AppendSeparator();
    menu.Append( ME_GridLocDispatch, wxGetApp().getMenu("dispatch"), wxGetApp().getTip("dispatch") );
    menu.AppendSeparator();
    mi = menu.Append( ME_GridLocProps, wxGetApp().getMsg("properties") );
    mi->Enable( !isAutoMode() || !wxGetApp().isRestrictedEdit() );

    menu.Append( ME_GridLocGoTo, wxGetApp().getMenu("gotoblock"), wxGetApp().getTip("gotoblock") );
    menu.Append( ME_GridLocSchedule, wxGetApp().getMenu("selectschedule"), wxGetApp().getTip("selectschedule") );
    menu.Append( ME_GridLocShortID, wxGetApp().getMenu("setshortid"), wxGetApp().getTip("selectschedule") );
    //PopupMenu(&menu, event.GetPosition().x, event.GetPosition().y );
    PopupMenu(&menu );
  }
  else {
    if(m_CV!=NULL)
      m_CV->OnGrid( event );
  }

  event.Skip(true);
}

void RocGuiFrame::OnSelectCell( wxGridEvent& event ){
  if( event.GetEventObject() == m_ActiveLocs ) {
    wxString str = m_ActiveLocs->GetCellValue( event.GetRow(), 0 );
    OnCellLeftClick(event);
    TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "OnSelectCell %s", (const char*)str.mb_str(wxConvUTF8) );
  }
  else {
    if(m_CV!=NULL)
      m_CV->OnGrid( event );
  }

  event.Skip(true);
}

void RocGuiFrame::OnLocGo(wxCommandEvent& event) {
  /* Inform RocRail... */
  iONode cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
  wLoc.setid( cmd, m_LocID );
  wLoc.setcmd( cmd, wLoc.go );
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);
}

void RocGuiFrame::OnLocStop(wxCommandEvent& event) {
  /* Inform RocRail... */
  iONode cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
  wLoc.setid( cmd, m_LocID );
  wLoc.setcmd( cmd, wLoc.stop );
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);
}

void RocGuiFrame::OnLocReset(wxCommandEvent& event) {
  int i = m_iLcRowSelection;

  m_ActiveLocs->SetCellValue( i, LOC_COL_MODE, _T("") );
  m_ActiveLocs->SetCellValue( i, LOC_COL_BLOCK, _T("") );
  m_ActiveLocs->SetCellValue( i, LOC_COL_DESTBLOCK, _T("") );


  /* Inform RocRail... */
  iONode cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
  wLoc.setid( cmd, m_LocID );
  wLoc.setcmd( cmd, wLoc.reset );
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);
}

void RocGuiFrame::OnLocSwap(wxCommandEvent& event) {


  TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "event type = %d, %d", event.GetEventType(), event.GetId() );

  int i = m_iLcRowSelection;

  m_ActiveLocs->SetCellValue( i, LOC_COL_MODE, _T("") );
  m_ActiveLocs->SetCellValue( i, LOC_COL_BLOCK, _T("") );
  m_ActiveLocs->SetCellValue( i, LOC_COL_DESTBLOCK, _T("") );


  /* Inform RocRail... */
  iONode cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
  wLoc.setid( cmd, m_LocID );
  wLoc.setcmd( cmd, wLoc.swap );
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);
}

void RocGuiFrame::OnLocDispatch(wxCommandEvent& event) {
  /* Inform RocRail... */
  iONode cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
  wLoc.setid( cmd, m_LocID );
  wLoc.setcmd( cmd, wLoc.dispatch );
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);
}

void RocGuiFrame::OnTimer(wxTimerEvent& event) {
  TraceOp.trc( "frame", TRCLEVEL_DEBUG, __LINE__, 9999, "OnTimer event..." );
  if( event.GetId() == ME_CVTimer )
    m_CV->OnTimer( event );
}

void RocGuiFrame::OnButton(wxCommandEvent& event)
{
  TraceOp.trc( "frame", TRCLEVEL_DEBUG, __LINE__, 9999, "OnButton event..." );
  if ( event.GetEventObject() == m_LocImage && !m_bAutoMode ) {
    iONode lc = findLoc(m_LocID);
    LocDialog*  dlg = new LocDialog(this, lc );
    TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "LocID=%s...0x%08X", m_LocID==NULL?"":m_LocID, lc );
    if( wxID_OK == dlg->ShowModal() ) {
      /* Notify Notebook. */
      wxCommandEvent evt( wxEVT_COMMAND_MENU_SELECTED, ME_INITACTIVELOCS );
      wxPostEvent( this, evt );
    }
    dlg->Destroy();
  }
  else {
    if(m_LC!=NULL) {
      TraceOp.trc( "frame", TRCLEVEL_DEBUG, __LINE__, 9999, "OnButton event for LC..." );
      m_LC->OnButton( event );
    }
    if(m_CV!=NULL) {
      TraceOp.trc( "frame", TRCLEVEL_DEBUG, __LINE__, 9999, "OnButton event for CV..." );
      m_CV->OnButton( event );
    }
    if(m_LNCV!=NULL) {
      TraceOp.trc( "frame", TRCLEVEL_DEBUG, __LINE__, 9999, "OnButton event for LNCV..." );
      m_LNCV->OnButton( event );
    }
  }
}

void RocGuiFrame::OnSlider(wxScrollEvent& event)
{
  m_LC->OnSlider( event );
}

void RocGuiFrame::OnLocGoTo(wxCommandEvent& event) {
  BlockDialog* blockDlg = new BlockDialog( this, NULL, false );
  if( wxID_OK == blockDlg->ShowModal() ) {
    iONode sel = blockDlg->getProperties();
    if( sel != NULL ) {
      const char* id = wBlock.getid( sel );

      if( id != NULL ) {
        /* Inform RocRail... */
        iONode lc = findLoc(m_LocID);
        iONode cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
        wLoc.setid( cmd, wLoc.getid( lc ) );
        wLoc.setcmd( cmd, wLoc.gotoblock );
        wLoc.setblockid( cmd, id );
        wxGetApp().sendToRocrail( cmd );
        cmd->base.del(cmd);
      }
    }
  }
  blockDlg->Destroy();
}


void RocGuiFrame::OnLocShortID(wxCommandEvent& event) {
  iONode lc = findLoc(m_LocID);
  if( lc != NULL ) {
    /* Inform RocRail... */
    iONode cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
    wLoc.setid( cmd, wLoc.getid( lc ) );
    wLoc.setcmd( cmd, wLoc.shortid );
    wxGetApp().sendToRocrail( cmd );
    cmd->base.del(cmd);
  }
}


void RocGuiFrame::OnLocSchedule(wxCommandEvent& event) {
  iONode lc = findLoc(m_LocID);
  const char* blockid = NULL;
  if( lc != NULL )
    blockid = wLoc.getblockid(lc);
  ScheduleDialog* dlg = new ScheduleDialog( this, (iONode)NULL, false, blockid );
  if( wxID_OK == dlg->ShowModal() ) {
    iONode sel = dlg->getProperties();
    if( sel != NULL ) {
      const char* id = wSchedule.getid( sel );

      if( id != NULL ) {
        /* Inform RocRail... */
        iONode cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
        wLoc.setid( cmd, wLoc.getid( lc ) );
        wLoc.setcmd( cmd, wLoc.useschedule );
        wLoc.setscheduleid( cmd, id );
        wxGetApp().sendToRocrail( cmd );
        cmd->base.del(cmd);
      }
    }
  }
  dlg->Destroy();
}

void RocGuiFrame::OnLocProps(wxCommandEvent& event) {
  TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "OnLocProps %s", m_LocID );
  iONode lc = findLoc(m_LocID);
  LocDialog*  dlg = new LocDialog(this, lc );
  if( wxID_OK == dlg->ShowModal() ) {
    /* Notify Notebook. */
    wxCommandEvent evt( wxEVT_COMMAND_MENU_SELECTED, ME_INITACTIVELOCS );
    wxPostEvent( this, evt );
  }
  dlg->Destroy();
}

void RocGuiFrame::OnPanelProps(wxCommandEvent& event) {
  BasePanel* panel = (BasePanel*)m_PlanNotebook->GetCurrentPage();
  if( panel != NULL ) {
    panel->OnPanelProps( event );
    int page = m_PlanNotebook->GetSelection();
    m_PlanNotebook->SetPageText( page, wxString( panel->getZLevelTitle(),wxConvUTF8 ) );
  }
}


void RocGuiFrame::OnAddPanel(wxCommandEvent& event) {
  PlanPanelProps* dlg = new PlanPanelProps( this, (iONode)NULL );
  if( wxID_OK == dlg->ShowModal() ) {
    iONode zlevel = dlg->GetProps();

    int pages = m_PlanNotebook->GetPageCount();
    for( int i = 0; i < pages; i++ ) {
      BasePanel* p = (BasePanel*)m_PlanNotebook->GetPage(i);
      if( StrOp.equals( p->getZLevelTitle(), wZLevel.gettitle( zlevel ) ) ) {
        dlg->Destroy();
        return;
      }
    }

    if( wxGetApp().isOffline() ) {
      InitNotebookPage( zlevel );
      NodeOp.addChild( wxGetApp().getModel(), zlevel );
      wxGetApp().setLocalModelModified(true);
    }
    else {
      iONode cmd = NodeOp.inst( wModelCmd.name(), NULL, ELEMENT_NODE );
      wModelCmd.setcmd( cmd, wModelCmd.add );
      NodeOp.addChild( cmd, (iONode)zlevel->base.clone( zlevel ) );
      wxGetApp().sendToRocrail( cmd );
      cmd->base.del(cmd);
    }

  }
  dlg->Destroy();
}


void RocGuiFrame::OnUndo(wxCommandEvent& event) {
  UndoDlg*  dlg = new UndoDlg(this, wxGetApp().getUndoItems() );
  if( wxID_OK == dlg->ShowModal() ) {
    // implementation of Undo in in dialog
  }
  dlg->Destroy();
}


void RocGuiFrame::OnLangEnglish(wxCommandEvent& event) {
  wGui.setlang( wxGetApp().getIni(), wGui.lang_english );
  wxMessageDialog( this, wxGetApp().getMsg("change_language_msg"), _T("Rocrail"), wxOK | wxICON_INFORMATION ).ShowModal();
}
void RocGuiFrame::OnLangGerman(wxCommandEvent& event) {
  wGui.setlang( wxGetApp().getIni(), wGui.lang_german );
  wxMessageDialog( this, wxGetApp().getMsg("change_language_msg"), _T("Rocrail"), wxOK | wxICON_INFORMATION ).ShowModal();
}
void RocGuiFrame::OnLangDutch(wxCommandEvent& event) {
  wGui.setlang( wxGetApp().getIni(), wGui.lang_dutch );
  wxMessageDialog( this, wxGetApp().getMsg("change_language_msg"), _T("Rocrail"), wxOK | wxICON_INFORMATION ).ShowModal();
}
void RocGuiFrame::OnLangSwedisch(wxCommandEvent& event) {
  wGui.setlang( wxGetApp().getIni(), wGui.lang_swedisch );
  wxMessageDialog( this, wxGetApp().getMsg("change_language_msg"), _T("Rocrail"), wxOK | wxICON_INFORMATION ).ShowModal();
}
void RocGuiFrame::OnLangFrench(wxCommandEvent& event) {
  wGui.setlang( wxGetApp().getIni(), wGui.lang_french );
  wxMessageDialog( this, wxGetApp().getMsg("change_language_msg"), _T("Rocrail"), wxOK | wxICON_INFORMATION ).ShowModal();
}

void RocGuiFrame::OnLangTurkish(wxCommandEvent& event) {
  wGui.setlang( wxGetApp().getIni(), wGui.lang_turkish );
  wxMessageDialog( this, wxGetApp().getMsg("change_language_msg"), _T("Rocrail"), wxOK | wxICON_INFORMATION ).ShowModal();
}

void RocGuiFrame::OnLangSpanish(wxCommandEvent& event) {
  wGui.setlang( wxGetApp().getIni(), wGui.lang_spanish );
  wxMessageDialog( this, wxGetApp().getMsg("change_language_msg"), _T("Rocrail"), wxOK | wxICON_INFORMATION ).ShowModal();
}

void RocGuiFrame::OnLangItalien(wxCommandEvent& event) {
  wGui.setlang( wxGetApp().getIni(), wGui.lang_italien );
  wxMessageDialog( this, wxGetApp().getMsg("change_language_msg"), _T("Rocrail"), wxOK | wxICON_INFORMATION ).ShowModal();
}
void RocGuiFrame::OnLangDanish(wxCommandEvent& event) {
  wGui.setlang( wxGetApp().getIni(), wGui.lang_danish );
  wxMessageDialog( this, wxGetApp().getMsg("change_language_msg"), _T("Rocrail"), wxOK | wxICON_INFORMATION ).ShowModal();
}
void RocGuiFrame::OnLangCzech(wxCommandEvent& event) {
  wGui.setlang( wxGetApp().getIni(), wGui.lang_czech );
  wxMessageDialog( this, wxGetApp().getMsg("change_language_msg"), _T("Rocrail"), wxOK | wxICON_INFORMATION ).ShowModal();
}
void RocGuiFrame::OnLangBosnian(wxCommandEvent& event) {
  wGui.setlang( wxGetApp().getIni(), wGui.lang_bosnian );
  wxMessageDialog( this, wxGetApp().getMsg("change_language_msg"), _T("Rocrail"), wxOK | wxICON_INFORMATION ).ShowModal();
}

void RocGuiFrame::OnRocrailIni(wxCommandEvent& event) {
  // Get the copied node:
  iONode ini = (iONode)event.GetClientData();
  if( ini != NULL && StrOp.equals( wRocRail.name(), NodeOp.getName( ini ) ) ) {
    // RocrailPropertiesDialog:
    m_RocrailIniDlg = new RocrailIniDialog( this, ini );
    if( wxID_OK == m_RocrailIniDlg->ShowModal() ) {
    }
    m_RocrailIniDlg->Destroy();
    m_RocrailIniDlg = NULL;
  }
  else {
    iONode cmd = NodeOp.inst( wSysCmd.name(), NULL, ELEMENT_NODE );
    wSysCmd.setcmd( cmd, wSysCmd.getini );
    wxGetApp().sendToRocrail( cmd, false );
    cmd->base.del(cmd);
  }
}


void RocGuiFrame::OnRocguiIni(wxCommandEvent& event) {
  // RocguiPropertiesDialog:
  RocguiIniDialog* dlg = new RocguiIniDialog( this, m_Ini );
  if( wxID_OK == dlg->ShowModal() ) {
  }
  dlg->Destroy();
}


