/*
 Rocrail - Model Railroad Software

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
#include <wx/dnd.h>

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

#include "rocview/dialogs/locdialog.h"
#include "rocview/dialogs/cardlg.h"
#include "rocview/dialogs/waybilldlg.h"
#include "rocview/dialogs/operatordlg.h"
#include "rocview/dialogs/locseldlg.h"
#include "rocview/dialogs/loccontroldlg.h"
#include "rocview/dialogs/switchdialog.h"
#include "rocview/dialogs/signaldialog.h"
#include "rocview/dialogs/scheduledialog.h"
#include "rocview/dialogs/locationsdlg.h"
#include "rocview/dialogs/outputdialog.h"
#include "rocview/dialogs/feedbackdialog.h"
#include "rocview/dialogs/routedialog.h"
#include "rocview/dialogs/blockdialog.h"
#include "rocview/dialogs/blockgroupingdlg.h"
#include "rocview/dialogs/planpaneldlg.h"
#include "rocview/dialogs/connectiondialog.h"
#include "rocview/dialogs/rocrailinidialog.h"
#include "rocview/dialogs/rocguiinidlg.h"
#include "rocview/dialogs/infodialog.h"
#include "rocview/dialogs/undodlg.h"
#include "rocview/dialogs/turntabledialog.h"
#include "rocview/dialogs/softwareupdates.h"
#include "rocview/dialogs/mvtrackdlg.h"
#include "rocview/dialogs/timedactions.h"
#include "rocview/dialogs/powermandlg.h"
#include "rocview/dialogs/powerctrldlg.h"
#include "rocview/dialogs/donkey.h"
#include "rocview/dialogs/toursdlg.h"
#include "rocview/dialogs/actionsctrldlg.h"
#include "rocview/dialogs/gotodlg.h"

#include "rocview/dialogs/decoders/locoio.h"
#include "rocview/dialogs/decoders/mgvdlg.h"
#include "rocview/dialogs/decoders/mgv141dlg.h"
#include "rocview/dialogs/decoders/dtopswdlg.h"
#include "rocview/dialogs/decoders/uhl633x0dlg.h"
#include "rocview/dialogs/decoders/uhl68610dlg.h"

#include "rocview/public/guiapp.h"
#include "rocview/public/swdlg.h"
#include "rocview/public/routedlg.h"
#include "rocview/public/item.h"
#include "rocview/public/jssupport.h"

#include "rocview/wrapper/public/Gui.h"
#include "rocview/wrapper/public/RRCon.h"
#include "rocview/wrapper/public/ModPanel.h"
#include "rocview/wrapper/public/PlanPanel.h"
#include "rocview/wrapper/public/SplitPanel.h"
#include "rocview/wrapper/public/Window.h"
#include "rocview/wrapper/public/Cmdline.h"
#include "rocview/wrapper/public/Theme.h"
#include "rocview/wrapper/public/ThemePanel.h"
#include "rocview/wrapper/public/ThemeBlock.h"
#include "rocview/wrapper/public/LcCtrl.h"
#include "rocview/wrapper/public/Shortcut.h"
#include "rocview/wrapper/public/WorkSpaces.h"
#include "rocview/wrapper/public/WorkSpace.h"

#include "rocrail/wrapper/public/Item.h"
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
#include "rocrail/wrapper/public/Tour.h"
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
#include "rocrail/wrapper/public/Booster.h"
#include "rocrail/wrapper/public/BoosterList.h"
#include "rocrail/wrapper/public/PwrEvent.h"
#include "rocrail/wrapper/public/SystemActions.h"
#include "rocrail/wrapper/public/Stage.h"
#include "rocrail/wrapper/public/StageSection.h"

#include "rocview/symbols/svg.h"
#include "rocview/res/icons.hpp"
// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------
// the application icon

#include "rocview/xpm/rocrail-32.xpm"
#include "rocview/xpm/nopict.xpm"

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
    EVT_MENU( ME_PowerEvent, RocGuiFrame::OnPowerEvent)

    EVT_MENU( wxID_EXIT         , RocGuiFrame::OnQuit)
    EVT_MENU( ME_Analyze        , RocGuiFrame::OnAnalyze)
    EVT_MENU( ME_AnalyzeClean   , RocGuiFrame::OnAnalyze)
    EVT_MENU( ME_Save           , RocGuiFrame::OnSave)
    EVT_MENU( ME_SaveAs         , RocGuiFrame::OnSaveAs)
    EVT_MENU( ME_Open           , RocGuiFrame::OnOpen)
    EVT_MENU( ME_OpenWorkspace  , RocGuiFrame::OnOpenWorkspace)
    EVT_MENU( ME_OpenWorkspace+1, RocGuiFrame::OnOpenWorkspace)
    EVT_MENU( ME_OpenWorkspace+2, RocGuiFrame::OnOpenWorkspace)
    EVT_MENU( ME_OpenWorkspace+3, RocGuiFrame::OnOpenWorkspace)
    EVT_MENU( ME_OpenWorkspace+4, RocGuiFrame::OnOpenWorkspace)
    EVT_MENU( ME_OpenWorkspace+5, RocGuiFrame::OnOpenWorkspace)
    EVT_MENU( ME_OpenWorkspace+6, RocGuiFrame::OnOpenWorkspace)
    EVT_MENU( ME_OpenWorkspace+7, RocGuiFrame::OnOpenWorkspace)
    EVT_MENU( ME_OpenWorkspace+8, RocGuiFrame::OnOpenWorkspace)
    EVT_MENU( ME_OpenWorkspace+9, RocGuiFrame::OnOpenWorkspace)
    EVT_MENU( ME_New            , RocGuiFrame::OnNew)
    EVT_MENU( ME_Upload         , RocGuiFrame::OnUpload)
    EVT_MENU( ME_GoOffline      , RocGuiFrame::OnGoOffline)
    EVT_MENU( wxID_ABOUT        , RocGuiFrame::OnAbout)
    EVT_MENU( ME_Update         , RocGuiFrame::OnUpdate)
    EVT_MENU( wxID_HELP         , RocGuiFrame::OnHelp)
    EVT_MENU( ME_RUG            , RocGuiFrame::OnRUG)
    EVT_MENU( ME_GCA            , RocGuiFrame::OnGCA)
    EVT_MENU( ME_DonKey         , RocGuiFrame::OnDonKey)
    EVT_MENU( ME_DonKeyInfo     , RocGuiFrame::OnDonKeyInfo)
    EVT_MENU( ME_Translations   , RocGuiFrame::OnTranslations)
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
    EVT_MENU( wxID_PREFERENCES  , RocGuiFrame::OnRocguiIni)
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
    EVT_MENU( ME_QuerySensors   , RocGuiFrame::OnQuerySensors)
    EVT_MENU( ME_TxShortIDs     , RocGuiFrame::OnTxShortIDs)
    EVT_MENU( ME_EditMode       , RocGuiFrame::OnEditMode)
    EVT_MENU( ME_EditModPlan    , RocGuiFrame::OnEditModPlan)
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
    EVT_MENU( ME_EditTours      , RocGuiFrame::OnEditTours)
    EVT_MENU( ME_EditLocations  , RocGuiFrame::OnEditLocations)
    EVT_MENU( ME_EditMVTrack    , RocGuiFrame::OnEditMVTrack)
    EVT_MENU( ME_EditTimedActions, RocGuiFrame::OnEditTimedActions)
    EVT_MENU( ME_EditBoosters   , RocGuiFrame::OnEditBoosters)
    EVT_MENU( ME_CtrlBoosters   , RocGuiFrame::OnCtrlBoosters)
    EVT_MENU( ME_EditActions    , RocGuiFrame::OnEditActions)
    EVT_MENU( ME_PanelProps     , RocGuiFrame::OnPanelProps)
    EVT_MENU( ME_AddPanel       , RocGuiFrame::OnAddPanel)
    EVT_MENU( ME_Undo           , RocGuiFrame::OnUndo)
    EVT_MENU( ME_OpenDecoder    , RocGuiFrame::OnOpenDecoder)
    EVT_MENU( ME_CBusNode       , RocGuiFrame::OnCBusNode)
    EVT_MENU( ME_Zoom25         , RocGuiFrame::OnZoom25)
    EVT_MENU( ME_Zoom50         , RocGuiFrame::OnZoom50)
    EVT_MENU( ME_Zoom75         , RocGuiFrame::OnZoom75)
    EVT_MENU( ME_Zoom100        , RocGuiFrame::OnZoom100)
    EVT_MENU( ME_LocoBook       , RocGuiFrame::OnLocoBook)
    EVT_MENU( ME_PlanBook       , RocGuiFrame::OnPlanBook)
    EVT_MENU( ME_TraceWindow    , RocGuiFrame::OnTraceWindow)
    EVT_MENU( ME_LocoSortByAddr , RocGuiFrame::OnLocoSortByAddr)
    EVT_MENU( ME_LocoViewAll    , RocGuiFrame::OnLocoViewAll)
    EVT_MENU( ME_LocoViewSteam  , RocGuiFrame::OnLocoViewSteam)
    EVT_MENU( ME_LocoViewDiesel , RocGuiFrame::OnLocoViewDiesel)
    EVT_MENU( ME_LocoViewElectric, RocGuiFrame::OnLocoViewElectric)
    EVT_MENU( ME_LocoViewCommuter, RocGuiFrame::OnLocoViewCommuter)
    EVT_MENU( ME_LocoViewSpecial, RocGuiFrame::OnLocoViewSpecial)
    EVT_MENU( ME_ShowID         , RocGuiFrame::OnShowID)
    EVT_MENU( ME_FullScreen     , RocGuiFrame::OnFullScreen)
    EVT_MENU( ME_Raster         , RocGuiFrame::OnRaster)
    EVT_MENU( ME_Tooltip        , RocGuiFrame::OnTooltip)
    EVT_MENU( ME_BackColor      , RocGuiFrame::OnBackColor)
    EVT_MENU( ME_UHL_63350      , RocGuiFrame::OnUhl63350)
    EVT_MENU( ME_UHL_68610      , RocGuiFrame::OnUhl68610)
    EVT_MENU( ME_LOCOIO         , RocGuiFrame::OnLocoIO)
    EVT_MENU( ME_MGV            , RocGuiFrame::OnMGV)
    EVT_MENU( ME_MGV141         , RocGuiFrame::OnMGV141)
    EVT_MENU( ME_DTOpSw         , RocGuiFrame::OnDTOpSw)

    EVT_MENU( ME_LangEnglish    , RocGuiFrame::OnLangEnglish)
    EVT_MENU( ME_LangEnglishAU  , RocGuiFrame::OnLangEnglishAU)
    EVT_MENU( ME_LangEnglishGB  , RocGuiFrame::OnLangEnglishGB)
    EVT_MENU( ME_LangEnglishCA  , RocGuiFrame::OnLangEnglishCA)
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
    EVT_MENU( ME_LangBulgarian  , RocGuiFrame::OnLangBulgarian)
    EVT_MENU( ME_LangCatalan    , RocGuiFrame::OnLangCatalan)
    EVT_MENU( ME_LangGreek      , RocGuiFrame::OnLangGreek)
    EVT_MENU( ME_LangRussian    , RocGuiFrame::OnLangRussian)
    EVT_MENU( ME_LangRomanian    , RocGuiFrame::OnLangRomanian)
    EVT_MENU( ME_LangPortuguese  , RocGuiFrame::OnLangPortuguese)
    EVT_MENU( ME_LangPortugueseBR, RocGuiFrame::OnLangPortugueseBR)
    EVT_MENU( ME_LangHungarian   , RocGuiFrame::OnLangHungarian)
    EVT_MENU( ME_LangPolish      , RocGuiFrame::OnLangPolish)
    EVT_MENU( ME_LangNorwegian   , RocGuiFrame::OnLangNorwegian)
    EVT_MENU( ME_LangChinese     , RocGuiFrame::OnLangChinese)
    EVT_MENU( ME_LangChineseCH   , RocGuiFrame::OnLangChineseCH)
    EVT_MENU( ME_LangIndonesian  , RocGuiFrame::OnLangIndonesian)
    EVT_MENU( ME_LangJapanese    , RocGuiFrame::OnLangJapanese)
    EVT_MENU( ME_LangKorean      , RocGuiFrame::OnLangKorean)
    EVT_GRID_CELL_LEFT_CLICK( RocGuiFrame::OnCellLeftClick )
    EVT_GRID_CELL_LEFT_DCLICK( RocGuiFrame::OnCellLeftDClick )
    EVT_GRID_CELL_RIGHT_CLICK( RocGuiFrame::OnCellRightClick )
    EVT_GRID_SELECT_CELL( RocGuiFrame::OnSelectCell )

    EVT_MENU     (ME_GridLocGo      , RocGuiFrame::OnLocGo  )
    EVT_MENU     (ME_GridLocStop    , RocGuiFrame::OnLocStop)
    EVT_MENU     (ME_GridLocReset   , RocGuiFrame::OnLocReset)
    EVT_MENU     (ME_GridLocResetAll, RocGuiFrame::OnLocResetAll)
    EVT_MENU     (ME_GridLocSwap    , RocGuiFrame::OnLocSwap)
    EVT_MENU     (ME_GridLocSwapBlockSide, RocGuiFrame::OnLocSwapBlockSide)
    EVT_MENU     (ME_GridLocDispatch, RocGuiFrame::OnLocDispatch)
    EVT_MENU     (ME_GridLocProps   , RocGuiFrame::OnLocProps)
    EVT_MENU     (ME_GridLocGoTo    , RocGuiFrame::OnLocGoTo)
    EVT_MENU     (ME_GridLocSchedule, RocGuiFrame::OnLocSchedule)
    EVT_MENU     (ME_GridLocTour    , RocGuiFrame::OnLocTour)
    EVT_MENU     (ME_GridLocShortID , RocGuiFrame::OnLocShortID)
    EVT_MENU     (ME_GridLocActivate, RocGuiFrame::OnLocActivate)
    EVT_MENU     (ME_GridLocDeActivate, RocGuiFrame::OnLocDeActivate)

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
    EVT_MENU( ME_SetStatusText  , RocGuiFrame::OnSetStatusText)

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


bool RocGuiFrame::isInStage( const char* locid, const char* blockid ) {
  iONode model = wxGetApp().getModel();
  iONode sblist = wPlan.getsblist( model );
  if( sblist != NULL ) {
    int cnt = NodeOp.getChildCnt( sblist );
    if( blockid != NULL ) {
      for( int i = 0; i < cnt; i++ ) {
        iONode sb = NodeOp.getChild( sblist, i );
        const char* id = wStage.getid( sb );

        if( id != NULL && StrOp.equals( blockid, id ) ) {
          iONode section = wStage.getsection( sb );
          while( section != NULL ) {
            if( StrOp.equals( locid, wStageSection.getlcid(section) ) ) {
              return true;
            }
            section = wStage.nextsection( sb, section );
          }
        }
      }
    }
  }
  return false;
}


iONode RocGuiFrame::findBlock4Loc( const char* locid, const char* blockid ) {
  iONode model = wxGetApp().getModel();
  iONode bklist = wPlan.getbklist( model );
  if( bklist != NULL ) {
    int cnt = NodeOp.getChildCnt( bklist );
    if( blockid != NULL ) {
      for( int i = 0; i < cnt; i++ ) {
        iONode bk = NodeOp.getChild( bklist, i );
        const char* id = wBlock.getid( bk );

        if( id != NULL && StrOp.equals( blockid, id ) ) {
          return bk;
        }
      }
    }
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
    m_PlanNotebook->AddPage(m_ModPanel, wxString(wPlan.getmodtitle(wxGetApp().getModel()),wxConvUTF8), true);
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

  p->SetBackgroundColor((byte)wPlanPanel.getred( ini), (byte)wPlanPanel.getgreen( ini), (byte)wPlanPanel.getblue( ini), true);
  p->SetScBackgroundColor((byte)wPlanPanel.getscred( ini), (byte)wPlanPanel.getscgreen( ini), (byte)wPlanPanel.getscblue( ini), wPlanPanel.isshowsc( ini)?true:false);
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
  TraceOp.trc( "plan", TRCLEVEL_INFO, __LINE__, 9999, "scroll cx=%d cy=%d", wPlanPanel.getcx(ini), wPlanPanel.getcy(ini) );
  p->SetScrollbars( (int)(itemsize*m_Scale), (int)(itemsize*m_Scale), wPlanPanel.getcx(ini), wPlanPanel.getcy(ini) );
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


void RocGuiFrame::putChild( void* item, int z  ) {
  if( m_ModPanel != NULL ) {
    // Not supported;
  }
  else {
    int pages = m_PlanNotebook->GetPageCount();
    for( int i = 0; i < pages; i++ ) {
      PlanPanel* p = (PlanPanel*)m_PlanNotebook->GetPage(i);
      if( z == p->getZ() ) {
        p->putChild(item);
        break;
      }
    }
  }
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


void RocGuiFrame::CleanNotebook() {

  wxCursor cursor = wxCursor(wxCURSOR_WAIT);
  SetCursor(cursor);

  int pages = m_PlanNotebook->GetPageCount();
  for( int i = 0; i < pages; i++ ) {
    BasePanel* p = (BasePanel*)m_PlanNotebook->GetPage(i);
    p->clean();
  }

  m_PlanNotebook->DeleteAllPages();
  if( m_ActiveLocs->GetNumberRows() > 0 )
    m_ActiveLocs->DeleteRows( 0, m_ActiveLocs->GetNumberRows() );

  wxGetApp().cleanupOldModel();
  m_bInitialized = false;

  cursor = wxCursor(wxCURSOR_ARROW);

}


void RocGuiFrame::OnInitNotebook( wxCommandEvent& event ) {
  CleanNotebook();
  wxCursor cursor = wxCursor(wxCURSOR_WAIT);

  if( m_WorkSpace != NULL ) {
    wWorkSpace.settitle(m_WorkSpace, wPlan.gettitle(wxGetApp().getModel()));
  }

  m_StatusBar->Health( wPlan.ishealthy(wxGetApp().getModel()) );

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

  // the plan flags if it is a modular layout:
  wxGetApp().setModView(wPlan.ismodplan(wxGetApp().getModel()) ? true:false);

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


static int locAddrComparator(obj* o1, obj* o2) {
  if( *o1 == NULL || *o2 == NULL )
    return 0;
  if( wLoc.getaddr( (iONode)*o1) == wLoc.getaddr( (iONode)*o2 ) )
    return 0;
  if( wLoc.getaddr( (iONode)*o1)  > wLoc.getaddr( (iONode)*o2 ) )
    return 1;
  return -1;
}


void RocGuiFrame::modifyLoc( iONode props, bool deep ) {
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
    TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "%s loc [%s]", deep?"merge":"update", wLoc.getid(loc) );
    int cnt = NodeOp.getAttrCnt( props );
    for( i = 0; i < cnt; i++ ) {
      iOAttr attr = NodeOp.getAttr( props, i );
      const char* name  = AttrOp.getName( attr );
      const char* value = AttrOp.getVal( attr );
      if( !deep && ( StrOp.equals("id", name) || StrOp.equals("addr", name) ) ) {
        // skip
        TraceOp.trc( "frame", TRCLEVEL_DEBUG, __LINE__, 9999, "skip attribute [%s] for loc [%s]", name, wLoc.getid(loc) );
      }
      else {
        NodeOp.setStr( loc, name, value );
      }
    }

    /* Leave the childs if no new are coming */
    if( deep && NodeOp.getChildCnt( props ) > 0 ) {
      cnt = NodeOp.getChildCnt( loc );
      TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "remove %d child nodes from loco[%s]", cnt, wLoc.getid(loc) );
      for( i = 0; i < cnt; i++ ) {
        iONode child = NodeOp.getChild( loc, 0 );
        TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "remove child %d %s 0x%08X", i, NodeOp.getName(child), child );
        NodeOp.removeChild( loc, child );
        NodeOp.base.del(child);
      }
      cnt = NodeOp.getChildCnt( props );
      TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "add %d child nodes to loco[%s]", cnt, wLoc.getid(loc) );
      for( i = 0; i < cnt; i++ ) {
        iONode child = (iONode)NodeOp.base.clone(NodeOp.getChild( props, i ));
        TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "add child %d %s 0x%08X", i, NodeOp.getName(child), child );
        NodeOp.addChild( loc, child );
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
  m_ActiveLocs->ClearGrid();

  if( m_ActiveLocs->GetNumberRows() > 0 )
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
          if( m_LocoCategory == LOCO_VIEW_STEAM && StrOp.equals(wLoc.engine_steam, wLoc.getengine(lc)) &&
              !wLoc.iscommuter(lc) && !StrOp.equals(wLoc.cargo_cleaning, wLoc.getcargo(lc)) )
            ListOp.add( list, (obj)lc );
          else if( m_LocoCategory == LOCO_VIEW_DIESEL && StrOp.equals(wLoc.engine_diesel, wLoc.getengine(lc)) &&
              !wLoc.iscommuter(lc) && !StrOp.equals(wLoc.cargo_cleaning, wLoc.getcargo(lc)) )
            ListOp.add( list, (obj)lc );
          else if( m_LocoCategory == LOCO_VIEW_ELECTRIC && StrOp.equals(wLoc.engine_electric, wLoc.getengine(lc)) &&
              !wLoc.iscommuter(lc) && !StrOp.equals(wLoc.cargo_cleaning, wLoc.getcargo(lc)) )
            ListOp.add( list, (obj)lc );
          else if( m_LocoCategory == LOCO_VIEW_COMMUTER && wLoc.iscommuter(lc) && !StrOp.equals(wLoc.cargo_cleaning, wLoc.getcargo(lc)) )
            ListOp.add( list, (obj)lc );
          else if( m_LocoCategory == LOCO_VIEW_SPECIAL && !wLoc.iscommuter(lc) && StrOp.equals(wLoc.cargo_cleaning, wLoc.getcargo(lc)) )
            ListOp.add( list, (obj)lc );
          else if( m_LocoCategory == LOCO_VIEW_ALL )
            ListOp.add( list, (obj)lc );
          else
            TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "loc [%s] fits not in category %d", wLoc.getid(lc), m_LocoCategory );
        }
        else {
          hiddenlocos = true;
          TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "loc [%s] is not shown", wLoc.getid(lc) );
        }
      }
      // Sort the list:
      ListOp.sort( list, m_LocoSortByAddress ? locAddrComparator:locComparator );

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
        iONode bk = findBlock4Loc(id, NULL);
        if( bk != NULL ) {
          m_ActiveLocs->SetCellValue(m_ActiveLocs->GetNumberRows()-1, LOC_COL_BLOCK,
              (wLoc.isblockenterside(lc)?_T(""):_T("-")) + wxString(wBlock.getid( bk ),wxConvUTF8) );
        }
        else if( wLoc.getblockid(lc) != NULL && StrOp.len(wLoc.getblockid(lc)) > 0 ) {
          // Could be a staging block.
          if( isInStage(id, wLoc.getblockid(lc)) ) {
            m_ActiveLocs->SetCellValue(m_ActiveLocs->GetNumberRows()-1, LOC_COL_BLOCK,
                (wLoc.isblockenterside(lc)?_T(""):_T("-")) + wxString(wLoc.getblockid(lc),wxConvUTF8) );
          }
        }
        m_ActiveLocs->SetReadOnly( m_ActiveLocs->GetNumberRows()-1, LOC_COL_BLOCK, true );
        m_ActiveLocs->SetCellAlignment( m_ActiveLocs->GetNumberRows()-1, LOC_COL_BLOCK, wxALIGN_LEFT, wxALIGN_CENTRE );

        Boolean dir = wLoc.isdir(lc);
        Boolean placing = wLoc.isplacing(lc);
        val = StrOp.fmt( "%s%s%d%s", dir?"":"<", placing?"":"-", wLoc.getV(lc)==-1?0:wLoc.getV(lc), dir?">":"" );
        //val = StrOp.fmt( "%c%d", wLoc.isplacing(lc)?' ':'-', wLoc.getV( lc )==-1?0:wLoc.getV( lc ) );
        m_ActiveLocs->SetCellValue(m_ActiveLocs->GetNumberRows()-1, LOC_COL_V, wxString(val,wxConvUTF8) );
        m_ActiveLocs->SetReadOnly( m_ActiveLocs->GetNumberRows()-1, LOC_COL_V, true );
        m_ActiveLocs->SetCellAlignment( m_ActiveLocs->GetNumberRows()-1, LOC_COL_V, wxALIGN_RIGHT, wxALIGN_CENTRE );

        m_ActiveLocs->SetCellBackgroundColour( m_ActiveLocs->GetNumberRows()-1, LOC_COL_V,
            wLoc.isplacing(lc)?
                m_ActiveLocs->GetCellBackgroundColour(m_ActiveLocs->GetNumberRows()-1, LOC_COL_BLOCK):
                  wxColour(240,200,200));

        if( wLoc.getmint( lc ) > 0 ) {
          if(wLoc.getruntime( lc ) - wLoc.getmtime( lc ) > wLoc.getmint( lc ) * 3600 ) {
            m_ActiveLocs->SetCellBackgroundColour( i, LOC_COL_ID, wxColour(255,200,200));
          }
          else {
            m_ActiveLocs->SetCellBackgroundColour( i, LOC_COL_ID,
                m_ActiveLocs->GetCellBackgroundColour(i, LOC_COL_ID));
          }
        }

        int jsdev = m_JsSupport!= NULL ? m_JsSupport->getDev4ID(wLoc.getid( lc )) : -1;
        if( jsdev != -1 )
          m_ActiveLocs->SetCellValue( i, LOC_COL_MODE, _T("ctrl") + wxString::Format(_T("%d"), jsdev) );
        else if( StrOp.len(wLoc.getthrottleid( lc )) > 0 )
          m_ActiveLocs->SetCellValue( i, LOC_COL_MODE, wxString(wLoc.getthrottleid( lc ), wxConvUTF8) );
        else if(!wLoc.isactive(lc))
          m_ActiveLocs->SetCellValue( i, LOC_COL_MODE, _T("hold") );
        else
          m_ActiveLocs->SetCellValue( m_ActiveLocs->GetNumberRows()-1, LOC_COL_MODE, wxString(wLoc.isresumeauto( lc ) ? "*":"",wxConvUTF8) + wxString(wLoc.getmode( lc ),wxConvUTF8) );

        m_ActiveLocs->SetReadOnly( m_ActiveLocs->GetNumberRows()-1, LOC_COL_MODE, true );
        m_ActiveLocs->SetCellBackgroundColour( m_ActiveLocs->GetNumberRows()-1, LOC_COL_MODE,
            wLoc.isactive(lc)?
                m_ActiveLocs->GetCellBackgroundColour(m_ActiveLocs->GetNumberRows()-1, LOC_COL_BLOCK):wxColour(240,200,200));



        m_ActiveLocs->SetCellAlignment( m_ActiveLocs->GetNumberRows()-1, LOC_COL_MODE, wxALIGN_LEFT, wxALIGN_CENTRE );

        m_ActiveLocs->SetCellAlignment( m_ActiveLocs->GetNumberRows()-1, LOC_COL_DESTBLOCK, wxALIGN_LEFT, wxALIGN_CENTRE );
      }
      ListOp.base.del( list );
    }
    m_ActiveLocs->SetCellBackgroundColour( hiddenlocos ? wxColour(255,255,200):wxColour(255,255,255));

  }

  m_ActiveLocs->Show( true );
  if( firstset && firstid!=NULL && selectedLoc != NULL ) {
    m_LocID = StrOp.dup(firstid);
    m_LC->setLocProps(selectedLoc);
    m_CV->setLocProps(selectedLoc);


    wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, UPDATE_LOC_IMAGE_EVENT );
    event.SetClientData( NodeOp.base.clone( selectedLoc ) );
    wxPostEvent( this, event );
  }

  m_ActiveLocs->AutoSizeColumns(false);
  /*
  m_ActiveLocs->FitInside();
  m_ActiveLocs->UpdateDimensions();
  m_ActiveLocsPanel->GetSizer()->Layout();
  */
  m_ActiveLocs->SelectRow(m_iLcRowSelection);

  if( wGui.isrestorelcctrl(m_Ini) ) {
    initLocCtrlDialogs();
  }
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

void RocGuiFrame::OnPowerEvent( wxCommandEvent& event ) {
  iONode node = (iONode)event.GetClientData();

  iONode model = wxGetApp().getModel();
  if( model != NULL ) {
    iONode boosterlist = wPlan.getboosterlist( model );
    if( boosterlist != NULL ) {
      iONode booster = wBoosterList.getbooster( boosterlist );
      while( booster != NULL ) {
        if( StrOp.equals( wPwrEvent.getid(node), wBooster.getid(booster) ) ) {
          wBooster.setpower( booster, wPwrEvent.ispower(node) );
          wBooster.setshortcut( booster, wPwrEvent.isshortcut(node) );
          break;
        }
        booster = wBoosterList.nextbooster( boosterlist, booster );
      };
    }
  }


  if( m_PowerCtrl != NULL ) {
    m_PowerCtrl->powerEvent(node);
  }
}


void RocGuiFrame::CVevent( wxCommandEvent& event ) {
  // Get copied node:
  iONode node = (iONode)event.GetClientData();
  if( wProgram.islncv(node) ) {
    if( m_LocoIO != NULL )
      m_LocoIO->event( node );
    else if( m_MGV141 != NULL )
      m_MGV141->event( node );
    else if( m_DTOpSw != NULL )
      m_DTOpSw->event( node );
    else if( m_Uhl68610 != NULL )
      m_Uhl68610->event( node );
    else if( m_Uhl633x0 != NULL )
      m_Uhl633x0->event( node );
    else
      m_LNCV->event( node );
  }
  else if( wProgram.getlntype(node) == wProgram.lntype_cbus ) {
    if( m_CBusNodeDlg != NULL )
      m_CBusNodeDlg->event( node );
    else if(wProgram.getcmd(node) == wProgram.nnreq ) {
      /* instantiate the dialog */
      m_CBusNodeDlg = new CBusNodeDlg(this, node);
      if( wxID_OK == m_CBusNodeDlg->ShowModal() ) {
        /* Notify RocRail. */
      }
      m_CBusNodeDlg->Destroy();
      m_CBusNodeDlg = NULL;
    }
  }
  else if( wProgram.getlntype(node) == wProgram.lntype_cs ) {
    if( m_RocrailIniDlg != NULL )
      m_RocrailIniDlg->event( node );
  }
  else {
    m_CV->event( node );
  }
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
    if( !m_LC->updateLoc( node ) ) {
      // Update loco not selected in the loco panel.
      iONode loco = findLoc(wFunCmd.getid(node));
      if( loco != NULL )
        m_LC->setFX(loco, node);
    }
  }
  else if( StrOp.equals( wLoc.name(), NodeOp.getName( node ) ) )
  {
    TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "Loc event: [%s] block=[%s] destblock=[%s] throttleID=%s",
        wLoc.getid( node ), (wLoc.getblockid( node ) != NULL ? wLoc.getblockid( node ):"-"),
        (wLoc.getdestblockid( node ) != NULL ? wLoc.getdestblockid( node ):"-") , wLoc.getthrottleid(node) );

    for( int i = 0; i < ListOp.size(m_LocCtrlList); i++ ) {
      LocControlDialog* dlg = (LocControlDialog*)ListOp.get(m_LocCtrlList, i);
      dlg->modelEvent(node);
    }

    modifyLoc( node, StrOp.equals( wModelCmd.modify, wLoc.getcmd(node) ) ? true:false );

    m_LC->updateLoc( node );

    for( int i = 0; i < m_ActiveLocs->GetNumberRows(); i++ ) {
      char* locid = StrOp.dup( m_ActiveLocs->GetCellValue( i, 0 ).mb_str(wxConvUTF8) );
      if( StrOp.equals( wLoc.getid( node ), locid ) ) {
        Boolean dir = wLoc.isdir(node);
        Boolean placing = wLoc.isplacing(node);
        char* val = StrOp.fmt( "%s%s%d%s", dir?"":"<", placing?"":"-", wLoc.getV( node )==-1?0:wLoc.getV( node ), dir?">":"" );
        m_ActiveLocs->SetCellValue( i, LOC_COL_V, wxString(val,wxConvUTF8) );
        StrOp.free( val );
        m_ActiveLocs->SetCellBackgroundColour( i, LOC_COL_V,
            wLoc.isplacing(node)?m_ActiveLocs->GetCellBackgroundColour(i, LOC_COL_BLOCK):wxColour(240,200,200));

        if( wLoc.getmint( node ) > 0 ) {
          if(wLoc.getruntime( node ) - wLoc.getmtime( node ) > wLoc.getmint( node ) * 3600 ) {
            m_ActiveLocs->SetCellBackgroundColour( i, LOC_COL_ID, wxColour(255,200,200));
          }
          else {
            m_ActiveLocs->SetCellBackgroundColour( i, LOC_COL_ID,
                m_ActiveLocs->GetCellBackgroundColour(i, LOC_COL_ID));
          }
        }

        val = StrOp.fmt( "%d", wLoc.getaddr( node ) );
        m_ActiveLocs->SetCellValue( i, LOC_COL_ADDR, wxString(val,wxConvUTF8) );
        StrOp.free( val );


        int jsdev = m_JsSupport!= NULL ? m_JsSupport->getDev4ID(locid) : -1;

        TraceOp.trc( "frame", TRCLEVEL_DEBUG, __LINE__, 9999, "jsdev=%d throttleid.len=%d", jsdev, StrOp.len(wLoc.getthrottleid( node )));


        if( jsdev != -1 )
          m_ActiveLocs->SetCellValue( i, LOC_COL_MODE, _T("ctrl") + wxString::Format(_T("%d"), jsdev) );
        else if( StrOp.len(wLoc.getthrottleid( node )) > 0 )
          m_ActiveLocs->SetCellValue( i, LOC_COL_MODE, wxString(wLoc.getthrottleid( node ), wxConvUTF8) );
        else if(!wLoc.isactive(node))
          m_ActiveLocs->SetCellValue( i, LOC_COL_MODE, _T("hold") );
        else if( wLoc.ismanual(node))
          m_ActiveLocs->SetCellValue( i, LOC_COL_MODE, _T(">") + wxString(wLoc.getmode( node ),wxConvUTF8) );
        else
          m_ActiveLocs->SetCellValue( i, LOC_COL_MODE, wxString(wLoc.isresumeauto( node ) ? "*":"",wxConvUTF8) + wxString(wLoc.getmode( node ),wxConvUTF8) );

        m_ActiveLocs->SetCellBackgroundColour( i, LOC_COL_MODE,
            wLoc.isactive(node)?m_ActiveLocs->GetCellBackgroundColour(i, LOC_COL_BLOCK):wxColour(240,200,200));

        if( wLoc.getblockid( node ) != NULL ) {
          m_ActiveLocs->SetCellValue( i, LOC_COL_BLOCK, (wLoc.isblockenterside(node)?_T(""):_T("-")) + wxString(wLoc.getblockid( node ),wxConvUTF8) );

          iONode block = findBlock4Loc(wLoc.getid( node ), wLoc.getblockid( node ));
          if(block != NULL ) {
            wBlock.setlocid(block, wLoc.getid( node ) );
            wBlock.setupdateenterside(block, True);
            if( m_ModPanel != NULL) {
              m_ModPanel->modelEvent( block );
            } else {
              int pagecnt = getNotebook()->GetPageCount();
              for( int i = 0; i < pagecnt; i++ ) {
                PlanPanel* p = (PlanPanel*)wxGetApp().getFrame()->getNotebook()->GetPage(i);
                p->modelEvent( block );
              }
            }
          }
          else {
            //m_ActiveLocs->SetCellValue( i, LOC_COL_BLOCK, wxT("") );
          }

        }
        else {
          m_ActiveLocs->SetCellValue( i, LOC_COL_BLOCK, wxT("") );
        }

        if( wLoc.getdestblockid( node ) != NULL ) {
          iONode block = wxGetApp().getFrame()->findBlock4Loc(wLoc.getid( node ), wLoc.getdestblockid( node ));
          if(block != NULL ) {
            wBlock.setlocid(block, wLoc.getid( node ) );
            wBlock.setupdateenterside(block, True);
            if( m_ModPanel != NULL) {
              m_ModPanel->modelEvent( block );
            }
            else {
              int pagecnt = getNotebook()->GetPageCount();
              for( int i = 0; i < pagecnt; i++ ) {
                PlanPanel* p = (PlanPanel*)wxGetApp().getFrame()->getNotebook()->GetPage(i);
                p->modelEvent( block );
              }
            }
          }

        }

        if( wLoc.getdestblockid( node ) != NULL ) {
          if( wLoc.getscidx(node) == -1 )
          m_ActiveLocs->SetCellValue( i, LOC_COL_DESTBLOCK, wxString(wLoc.getdestblockid( node ),wxConvUTF8) );
          else {
            val = StrOp.fmt( "[%s:%d] %s", wLoc.getscheduleid(node), wLoc.getscidx( node ), wLoc.getdestblockid( node ) );
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
    const char* id = wBlock.getid( node );
    const char* locid = wBlock.getlocid( node );
    iONode block = NULL;

    if( id != NULL ) {
      if( locid != NULL ) {
        TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "Block event %s, locid=%s",
                     NodeOp.getStr(node, "id", "?"), wBlock.getlocid( node ) );

        block = wxGetApp().getFrame()->findBlock4Loc(wBlock.getlocid( node ), wBlock.getid( node ));
      }

      if(block != NULL ) {
        if( m_ModPanel != NULL) {
          m_ModPanel->modelEvent( block );
        }
        else {
          int pagecnt = getNotebook()->GetPageCount();
          for( int i = 0; i < pagecnt; i++ ) {
            PlanPanel* p = (PlanPanel*)wxGetApp().getFrame()->getNotebook()->GetPage(i);
            p->modelEvent( block );
          }
        }
      }
    }


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
  m_ActiveLocs->AutoSizeColumns(false);
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
RocGuiFrame::RocGuiFrame(const wxString& title, const wxPoint& pos, const wxSize& size, iONode ini, const char* theme, const char* sp, const char* tp)
       : wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
  m_Ini                = ini;
  m_RocrailIni         = NULL;
  m_WarningPanel       = NULL;
  m_MonitorPanel       = NULL;
  m_bEditMode          = false;
  m_bEditModPlan       = false;
  m_bServerConsoleMode = false;
  m_LocID              = NULL;
  m_iLcRowSelection    = 0;
  m_SymbolMap          = NULL;
  m_LocalPlan          = _T("");
  m_LocoIO             = NULL;
  m_CBusNodeDlg        = NULL;
  m_MGV141             = NULL;
  m_DTOpSw             = NULL;
  m_RocrailIniDlg      = NULL;
  m_Uhl68610           = NULL;
  m_Uhl633x0           = NULL;
  m_ModPanel           = NULL;
  m_LocCtrlList        = ListOp.inst();
  m_LocDlgMap          = MapOp.inst();
  m_bAutoMode          = false;
  m_ThemePath          = theme;
  m_ServerPath         = sp;
  m_ThemesPath         = tp;
  m_bInitialized       = false;
  m_PowerCtrl          = NULL;
  m_bActiveWorkspace   = false;
  m_bCheckedDonKey     = false;
  m_WorkSpace          = NULL;
  m_LocoCategory       = LOCO_VIEW_ALL;
  m_LocoSortByAddress  = wGui.islocosortbyaddress(m_Ini)?true:false;
  m_FakeLeftClick      = false;
  m_LC                 = NULL;
  m_CV                 = NULL;
  m_LNCV               = NULL; 
}

void RocGuiFrame::initFrame() {
  TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "m_ThemePath=0x%08X", m_ThemePath );
  TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "m_ServerPath=0x%08X", m_ServerPath );
  TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "init Frame..." );
  SetIcon(wxIcon(rocrail_32_xpm));

  // define accelerator keys for some frequently used functions
  wxAcceleratorEntry acc_entries[46];
  acc_entries[0].Set(wxACCEL_CTRL, (int) 'Z', ME_Undo);
  acc_entries[1].Set(wxACCEL_CTRL, (int) 'N', ME_New);
  acc_entries[2].Set(wxACCEL_CTRL, (int) 'S', ME_Save);
  acc_entries[3].Set(wxACCEL_CTRL, (int) 'O', ME_Open);
  acc_entries[4].Set(wxACCEL_CTRL, (int) 'P', ME_Go);
  acc_entries[5].Set(wxACCEL_CTRL, (int) 'M', ME_MIC);
  acc_entries[6].Set(wxACCEL_CTRL, (int) 'L', ME_LcDlg);
  acc_entries[7].Set(wxACCEL_ALT , (int) 'P', ME_LocoViewSpecial);
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

  acc_entries[37].Set(wxACCEL_CTRL, (int) 'W', ME_OpenWorkspace);
  acc_entries[38].Set(wxACCEL_CTRL, (int) 'D', ME_ShutdownRocRail);

  acc_entries[39].Set(wxACCEL_ALT, (int) 'A', ME_LocoViewAll);
  acc_entries[40].Set(wxACCEL_ALT, (int) 'S', ME_LocoViewSteam);
  acc_entries[41].Set(wxACCEL_ALT, (int) 'I', ME_LocoViewDiesel);
  acc_entries[42].Set(wxACCEL_ALT, (int) 'E', ME_LocoViewElectric);
  acc_entries[43].Set(wxACCEL_ALT, (int) 'C', ME_LocoViewCommuter);

  wxAcceleratorTable m_accel(44, acc_entries);
  this->SetAcceleratorTable(m_accel);
//DA


  // create a menu bar
  wxMenu *menuFile = new wxMenu();
  wxMenuItem *connect_menuFile = new wxMenuItem(menuFile, ME_Connect, wxGetApp().getMenu("connect"), wxGetApp().getTip("connect") );
  connect_menuFile->SetBitmap(*_img_connect);
  menuFile->Append(connect_menuFile);
  menuFile->Append(ME_GoOffline, wxGetApp().getMenu("gooffline"), wxGetApp().getTip("gooffline") );

  menuFile->Append(wxID_PREFERENCES, wxGetApp().getMenu("rocguiini"), wxGetApp().getTip("rocguiini") );
  menuFile->Append(ME_RocrailIni, wxGetApp().getMenu("rocrailini"), wxGetApp().getTip("rocrailini") );
  menuFile->AppendSeparator();
  menuFile->Append(ME_PlanTitle, wxGetApp().getMenu("plantitle"), wxGetApp().getTip("plantitle") );
  menuFile->AppendSeparator();

  wxMenuItem *new_menuFile = new wxMenuItem(menuFile, ME_New, wxGetApp().getMenu("new"), wxGetApp().getTip("new") );
  new_menuFile->SetBitmap(*_img_new);
  menuFile->Append(new_menuFile);

  wxMenuItem *open_menuFile = new wxMenuItem(menuFile, ME_Open, wxGetApp().getMenu("open"), wxGetApp().getTip("open") );
  open_menuFile->SetBitmap(*_img_open);
  menuFile->Append(open_menuFile);

  wxMenuItem *openworkspace_menuFile = new wxMenuItem(menuFile, ME_OpenWorkspace, wxGetApp().getMenu("openworkspace"), wxGetApp().getTip("openworkspace") );
  openworkspace_menuFile->SetBitmap(*_img_system);
  menuFile->Append(openworkspace_menuFile);

  iONode workspaces = wGui.getworkspaces(m_Ini);
  if( workspaces != NULL ) {
    int i = 1;
    wxMenu* menuWorkSpaces = new wxMenu();
    iONode ws = wWorkSpaces.getworkspace(workspaces);
    while(ws != NULL) {
      menuWorkSpaces->Append( ME_OpenWorkspace+i, wxString(wWorkSpace.gettitle(ws),wxConvUTF8) + _T(" - ") + wxString(wWorkSpace.getpath(ws),wxConvUTF8) );
      i++;
      ws = wWorkSpaces.nextworkspace(workspaces, ws);
    }
    menuFile->Append( ME_RecentWorkspaces, wxGetApp().getMenu("recentworkspaces"), menuWorkSpaces );

  }

  wxMenuItem *save_menuFile = new wxMenuItem(menuFile, ME_Save, wxGetApp().getMenu("save"), wxGetApp().getTip("save") );
  save_menuFile->SetBitmap(*_img_save);
  menuFile->Append(save_menuFile);

  menuFile->Append(ME_SaveAs, wxGetApp().getMenu("saveas"), wxGetApp().getTip("saveas") );

  menuFile->AppendSeparator();
  wxMenu *menuAnalyze = new wxMenu();
  menuAnalyze->Append(ME_Analyze, wxGetApp().getMenu("analyze"), wxGetApp().getTip("analyze") );
  menuAnalyze->Append(ME_AnalyzeClean, wxGetApp().getMenu("clearall") + _T(" ") +  wxGetApp().getMenu("analyze"), wxGetApp().getTip("analyze") );
  menuFile->Append( -1, wxGetApp().getMenu("analyze"), menuAnalyze );
  menuFile->AppendSeparator();

  wxMenuItem *upload_menuFile = new wxMenuItem(menuFile, ME_Upload, wxGetApp().getMenu("upload"), wxGetApp().getTip("upload") );
  upload_menuFile->SetBitmap(*_img_upload);
  menuFile->Append(upload_menuFile);

  menuFile->AppendSeparator();

  wxMenuItem *shutdown_menuFile = new wxMenuItem(menuFile, ME_ShutdownRocRail, wxGetApp().getMenu("shutdown"), wxGetApp().getTip("shutdown") );
  shutdown_menuFile->SetBitmap(*_img_server);
  menuFile->Append(shutdown_menuFile);
  //menuFile->Append(ME_ShutdownRocRail, wxGetApp().getMenu("shutdown"), wxGetApp().getTip("shutdown") );

#ifndef __APPLE__

  menuFile->AppendSeparator();

  wxMenuItem *exit_menuFile = new wxMenuItem(menuFile, wxID_EXIT, wxGetApp().getMenu("exit"), wxGetApp().getTip("exit") );
  exit_menuFile->SetBitmap(*_img_exit);
  menuFile->Append(exit_menuFile);
#endif

  wxMenu *menuEdit = new wxMenu();
  wxMenuItem *undo_menuEdit = new wxMenuItem(menuEdit, ME_Undo, wxGetApp().getMenu("undo"), wxGetApp().getTip("undo") );
  undo_menuEdit->SetBitmap(*_img_undo);
  menuEdit->Append(undo_menuEdit);

  wxMenu *menuPanel = new wxMenu();
  menuPanel->AppendCheckItem(ME_CtrlMode, wxGetApp().getMenu("ctrlmode"), wxGetApp().getTip("ctrlmode") );
  menuPanel->AppendCheckItem(ME_EditMode, wxGetApp().getMenu("editmode"), wxGetApp().getTip("editmode") );
  menuPanel->AppendCheckItem(ME_EditModPlan, wxGetApp().getMenu("editmodplan"), wxGetApp().getTip("editmodplan") );
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
  menuTables->Append(ME_EditTours, wxGetApp().getMenu("tourtable"), wxGetApp().getTip("tourtable") );
  menuTables->Append(ME_EditLocations, wxGetApp().getMenu("locationtable"), wxGetApp().getTip("locationtable") );
  menuTables->Append(ME_EditBlockGroups, wxGetApp().getMenu("blockgrouptable"), wxGetApp().getTip("blockgrouptable") );
  menuTables->Append(ME_EditTurnouts, wxGetApp().getMenu("turnouttable"), wxGetApp().getTip("turnouttable") );
  menuTables->Append(ME_EditSignals, wxGetApp().getMenu("signaltable"), wxGetApp().getTip("signaltable") );
  menuTables->Append(ME_EditOutputs, wxGetApp().getMenu("outputtable"), wxGetApp().getTip("outputtable") );
  menuTables->Append(ME_EditSensors, wxGetApp().getMenu("sensortable"), wxGetApp().getTip("sensortable") );
  menuTables->Append(ME_EditMVTrack, wxGetApp().getMenu("mvtrack"), wxGetApp().getTip("mvtrack") );
  menuTables->Append(ME_EditTimedActions, wxGetApp().getMenu("actiontable"), wxGetApp().getTip("actiontable") );
  menuTables->Append(ME_EditBoosters, wxGetApp().getMenu("boostertable"), wxGetApp().getTip("boostertable") );
  menuTables->Append(ME_EditActions, wxGetApp().getMenu("systemactions"), wxGetApp().getTip("systemactions") );

  // Turntable dialog has no index tab... disable
  //menuTables->Append(ME_EditTurntables, wxGetApp().getMenu("turntabletable"), wxGetApp().getTip("turntabletable") );

  wxMenu *menuControl = new wxMenu();
  menuControl->AppendCheckItem(ME_Go, wxGetApp().getMenu("poweron"), wxGetApp().getTip("poweron") );
  menuControl->Append(ME_EmergencyBreak, wxGetApp().getMenu("break"), wxGetApp().getTip("break") );
  menuControl->Append(ME_CtrlBoosters, wxGetApp().getMenu("powerctrl"), wxGetApp().getTip("powerctrl") );

  menuControl->AppendSeparator();
  wxMenu *menuSoD = new wxMenu();
  menuSoD->Append(ME_InitField, wxGetApp().getMenu("initfield"), wxGetApp().getTip("initfield") );
  menuSoD->Append(ME_QuerySensors, wxGetApp().getMenu("querysensors"), wxGetApp().getTip("querysensors") );
  menuControl->Append( -1, wxGetApp().getMenu("startofday"), menuSoD );

  menuControl->AppendSeparator();
  menuControl->Append(ME_TxShortIDs, wxGetApp().getMenu("txshortids"), wxGetApp().getTip("txshortids") );
  menuControl->AppendSeparator();

  wxMenuItem *opctrl_menuControl = new wxMenuItem(menuControl, ME_OperatorDlg, wxGetApp().getMenu("operator"), wxGetApp().getTip("operator") );
  opctrl_menuControl->SetBitmap(*_img_operator);
  menuControl->Append(opctrl_menuControl);

  menuControl->AppendSeparator();

  wxMenuItem *mic_menuControl = new wxMenuItem(menuControl, ME_MIC, wxGetApp().getMenu("mic"), wxGetApp().getTip("mic") );
  mic_menuControl->SetBitmap(*_img_mic);
  menuControl->Append(mic_menuControl);

  wxMenuItem *locctrl_menuControl = new wxMenuItem(menuControl, ME_LcDlg, wxGetApp().getMenu("locctrl"), wxGetApp().getTip("locctrl") );
  locctrl_menuControl->SetBitmap(*_img_locctrl);
  menuControl->Append(locctrl_menuControl);

  wxMenuItem *swctrl_menuControl = new wxMenuItem(menuControl, ME_SwDlg, wxGetApp().getMenu("swctrl"), wxGetApp().getTip("swctrl") );
  swctrl_menuControl->SetBitmap(*_img_swctrl);
  menuControl->Append(swctrl_menuControl);

  wxMenuItem *stctrl_menuControl = new wxMenuItem(menuControl, ME_RouteDlg, wxGetApp().getMenu("stctrl"), wxGetApp().getTip("stctrl") );
  stctrl_menuControl->SetBitmap(*_img_routes);
  menuControl->Append(stctrl_menuControl);

  wxMenu *menuAuto = new wxMenu();
  menuAuto->AppendCheckItem(ME_AutoMode, wxGetApp().getMenu("automode"), wxGetApp().getTip("automode") );
  menuAuto->AppendSeparator();
  menuAuto->Append(ME_AutoGo  , wxGetApp().getMenu("startall"), wxGetApp().getTip("startall") );
  menuAuto->Append(ME_AutoResume, wxGetApp().getMenu("resumeall"), wxGetApp().getTip("resumeall") );

  wxMenuItem *stop_menuAuto = new wxMenuItem(menuAuto, ME_AutoStop, wxGetApp().getMenu("stopall"), wxGetApp().getTip("stopall") );
  stop_menuAuto->SetBitmap(*_img_stopall);
  menuAuto->Append(stop_menuAuto);

  menuAuto->AppendSeparator();
  menuAuto->Append(ME_AutoSoftReset, wxGetApp().getMenu("softresetall"), wxGetApp().getTip("softresetall") );
  menuAuto->Append(ME_AutoReset, wxGetApp().getMenu("resetall"), wxGetApp().getTip("resetall") );

  wxMenu *menuLang = new wxMenu();
  menuLang->AppendCheckItem( ME_LangBosnian  , wxGetApp().getMenu("lang_bs"), wxGetApp().getMsg("changelang") );
  menuLang->AppendCheckItem( ME_LangBulgarian, wxGetApp().getMenu("lang_bg"), wxGetApp().getMsg("changelang") );
  menuLang->AppendCheckItem( ME_LangCatalan  , wxGetApp().getMenu("lang_ca"), wxGetApp().getMsg("changelang") );
  menuLang->AppendCheckItem( ME_LangChinese  , wxGetApp().getMenu("lang_zh_TW"), wxGetApp().getMsg("changelang") );
  menuLang->AppendCheckItem( ME_LangChineseCH, wxGetApp().getMenu("lang_zh_CH"), wxGetApp().getMsg("changelang") );
  menuLang->AppendCheckItem( ME_LangCzech    , wxGetApp().getMenu("lang_cs"), wxGetApp().getMsg("changelang") );
  menuLang->AppendCheckItem( ME_LangDanish   , wxGetApp().getMenu("lang_da"), wxGetApp().getMsg("changelang") );
  menuLang->AppendCheckItem( ME_LangDutch    , wxGetApp().getMenu("lang_nl"), wxGetApp().getMsg("changelang") );
  menuLang->AppendCheckItem( ME_LangEnglish  , wxGetApp().getMenu("lang_en"), wxGetApp().getMsg("changelang") );
  menuLang->AppendCheckItem( ME_LangEnglishAU, wxGetApp().getMenu("lang_en_AU"), wxGetApp().getMsg("changelang") );
  menuLang->AppendCheckItem( ME_LangEnglishCA, wxGetApp().getMenu("lang_en_CA"), wxGetApp().getMsg("changelang") );
  menuLang->AppendCheckItem( ME_LangEnglishGB, wxGetApp().getMenu("lang_en_GB"), wxGetApp().getMsg("changelang") );
  menuLang->AppendCheckItem( ME_LangFrench   , wxGetApp().getMenu("lang_fr"), wxGetApp().getMsg("changelang") );
  menuLang->AppendCheckItem( ME_LangGerman   , wxGetApp().getMenu("lang_de"), wxGetApp().getMsg("changelang") );
  menuLang->AppendCheckItem( ME_LangGreek    , wxGetApp().getMenu("lang_el"), wxGetApp().getMsg("changelang") );
  menuLang->AppendCheckItem( ME_LangHungarian, wxGetApp().getMenu("lang_hu"), wxGetApp().getMsg("changelang") );
  menuLang->AppendCheckItem( ME_LangIndonesian, wxGetApp().getMenu("lang_id"), wxGetApp().getMsg("changelang") );
  menuLang->AppendCheckItem( ME_LangItalien  , wxGetApp().getMenu("lang_it"), wxGetApp().getMsg("changelang") );
  menuLang->AppendCheckItem( ME_LangJapanese , wxGetApp().getMenu("lang_ja"), wxGetApp().getMsg("changelang") );
  menuLang->AppendCheckItem( ME_LangKorean   , wxGetApp().getMenu("lang_ko"), wxGetApp().getMsg("changelang") );
  menuLang->AppendCheckItem( ME_LangNorwegian, wxGetApp().getMenu("lang_nb"), wxGetApp().getMsg("changelang") );
  menuLang->AppendCheckItem( ME_LangPolish   , wxGetApp().getMenu("lang_pl"), wxGetApp().getMsg("changelang") );
  menuLang->AppendCheckItem( ME_LangPortuguese, wxGetApp().getMenu("lang_pt"), wxGetApp().getMsg("changelang") );
  menuLang->AppendCheckItem( ME_LangPortugueseBR, wxGetApp().getMenu("lang_pt_BR"), wxGetApp().getMsg("changelang") );
  menuLang->AppendCheckItem( ME_LangRomanian , wxGetApp().getMenu("lang_ro"), wxGetApp().getMsg("changelang") );
  menuLang->AppendCheckItem( ME_LangRussian  , wxGetApp().getMenu("lang_ru"), wxGetApp().getMsg("changelang") );
  menuLang->AppendCheckItem( ME_LangSpanish  , wxGetApp().getMenu("lang_es"), wxGetApp().getMsg("changelang") );
  menuLang->AppendCheckItem( ME_LangSwedisch , wxGetApp().getMenu("lang_sv"), wxGetApp().getMsg("changelang") );
  menuLang->AppendCheckItem( ME_LangTurkish  , wxGetApp().getMenu("lang_tr"), wxGetApp().getMsg("changelang") );


  wxMenu *menuZoom = new wxMenu();
  menuZoom->AppendCheckItem( ME_Zoom25, _T("25%") );
  menuZoom->AppendCheckItem( ME_Zoom50, _T("50%") );
  menuZoom->AppendCheckItem( ME_Zoom75, _T("75%") );
  menuZoom->AppendCheckItem( ME_Zoom100, _T("100%") );

  wxMenu *menuView = new wxMenu();

  wxMenuItem *clear_menuView = new wxMenuItem(menuView, ME_ClearMsg, wxGetApp().getMenu("clearmsg"), wxGetApp().getTip("clearmsg") );
  clear_menuView->SetBitmap(*_img_clear);
  menuView->Append(clear_menuView);

  menuView->AppendSeparator();
  menuView->Append( -1, wxGetApp().getMenu("language"), menuLang );
  menuView->AppendSeparator();
  menuView->Append( -1, wxGetApp().getMenu("zoom"), menuZoom );
  menuView->AppendSeparator();

  menuView->AppendCheckItem( ME_LocoBook, wxGetApp().getMenu("locobook"), wxGetApp().getTip("locobook") );
  menuView->AppendCheckItem( ME_PlanBook, wxGetApp().getMenu("panel"), wxGetApp().getTip("panel") );
  menuView->AppendCheckItem( ME_TraceWindow, wxGetApp().getMenu("trace"), wxGetApp().getTip("trace") );
  menuView->AppendCheckItem( ME_LocoSortByAddr, wxGetApp().getMenu("locosortbyaddr"), wxGetApp().getTip("locosortbyaddr") );
  wxMenu *menuLocoView = new wxMenu();
  menuLocoView->AppendCheckItem( ME_LocoViewAll, wxGetApp().getMenu("locoviewall"), wxGetApp().getTip("locoviewall") );
  menuLocoView->AppendCheckItem( ME_LocoViewSteam, wxGetApp().getMenu("locoviewsteam"), wxGetApp().getTip("locoviewsteam") );
  menuLocoView->AppendCheckItem( ME_LocoViewDiesel, wxGetApp().getMenu("locoviewdiesel"), wxGetApp().getTip("locoviewdiesel") );
  menuLocoView->AppendCheckItem( ME_LocoViewElectric, wxGetApp().getMenu("locoviewelectric"), wxGetApp().getTip("locoviewelectric") );
  menuLocoView->AppendCheckItem( ME_LocoViewCommuter, wxGetApp().getMenu("locoviewcommuter"), wxGetApp().getTip("locoviewcommuter") );
  menuLocoView->AppendCheckItem( ME_LocoViewSpecial, wxGetApp().getMenu("locoviewspecial"), wxGetApp().getTip("locoviewspecial") );
  menuView->Append( -1, wxGetApp().getMenu("locofilter"), menuLocoView );

  menuView->AppendSeparator();
  menuView->AppendCheckItem( ME_ShowID, wxGetApp().getMenu("showid"), wxGetApp().getTip("showid") );
  menuView->AppendCheckItem( ME_Raster, wxGetApp().getMenu("raster"), wxGetApp().getTip("raster") );
  menuView->AppendCheckItem( ME_Tooltip, wxGetApp().getMenu("tooltip"), wxGetApp().getTip("tooltip") );
  menuView->AppendCheckItem( ME_FullScreen, wxGetApp().getMenu("fullscreen"), wxGetApp().getTip("fullscreen") );
  menuView->Append( ME_BackColor, wxGetApp().getMenu("panelcolor"), wxGetApp().getTip("panelcolor") );

  // Programming
  wxMenu *menuProgramming = new wxMenu();
  wxMenu *menuPTLN = new wxMenu();
  menuPTLN->Append( ME_LOCOIO, wxString(_T("LocoIO")) + wxString(_T("...")), _T("LocoIO") );
  menuPTLN->Append( ME_DTOpSw, wxString(_T("Digitrax")) + wxString(_T("...")), _T("Digitrax") );
  wxMenu *menuUhlenbrock = new wxMenu();
  //menuUhlenbrock->Append( ME_UHL_63350, _T("Uhlenbrock 63340/63350") + wxString(_T("...")), _T("Uhlenbrock 63340/63350") );
  menuUhlenbrock->Append( ME_UHL_68610, _T("Uhlenbrock 68610") + wxString(_T("...")), _T("Uhlenbrock 68610") );
  menuPTLN->Append( -1, wxString(_T("Uhlenbrock")), menuUhlenbrock );

  menuProgramming->Append( -1, _T("LocoNet"), menuPTLN );

  wxMenu *menuMGV = new wxMenu();
  menuMGV->Append( ME_LOCOIO, wxString(_T("GCA50")) + wxString(_T("...")), _T("GCA50") );
  menuMGV->Append( ME_MGV, _T("GCA136") + wxString(_T("...")), _T("GCA136") );
  menuMGV->Append( ME_MGV141, _T("GCA141") + wxString(_T("...")), _T("GCA141") );
  menuProgramming->Append( -1, _T("GCA"), menuMGV );

  //wxMenu *menuPTDCC = new wxMenu();
  //menuPTDCC->Append( ME_OpenDecoder, wxGetApp().getMenu("opendecoder"), wxGetApp().getTip("opendecoder") );
  //menuProgramming->Append( -1, _T("NMRA DCC"), menuPTDCC );

  //wxMenu *menuCBus = new wxMenu();
  //menuCBus->Append( ME_CBusNode, _T("CBUS Node"), _T("CBUS Node") );
  //menuProgramming->Append( -1, _T("CBUS"), menuCBus );
  menuProgramming->Append( ME_CBusNode, _T("CBUS Node"), _T("CBUS Node") );

  // the "About" item should be in the help menu
  wxMenu *menuHelp = new wxMenu();
  wxMenuItem *help_menuHelp = new wxMenuItem(menuHelp, wxID_HELP, wxGetApp().getMenu("documentation"), wxGetApp().getTip("documentation") );
  help_menuHelp->SetBitmap(*_img_manual);
  menuHelp->Append(help_menuHelp);

  wxMenuItem *rug_menuHelp = new wxMenuItem(menuHelp, ME_RUG, wxGetApp().getMenu("usergroup"), wxGetApp().getTip("usergroup") );
  rug_menuHelp->SetBitmap(*_img_rug);
  menuHelp->Append(rug_menuHelp);

  wxMenuItem *translations_menuHelp = new wxMenuItem(menuHelp, ME_Translations, wxGetApp().getMenu("translations"), wxGetApp().getTip("translations") );
  translations_menuHelp->SetBitmap(*_img_lp);
  menuHelp->Append(translations_menuHelp);

  menuHelp->AppendSeparator();
  wxMenuItem *gca_menuHelp = new wxMenuItem(menuHelp,
      ME_GCA, wxT("Giling Computer Applications..."), wxT("LocoNet - CBus - Interfaces") );
  menuHelp->Append(gca_menuHelp);
  menuHelp->AppendSeparator();


  wxMenuItem *bug_menuHelp = new wxMenuItem(menuHelp, ME_Bug, wxGetApp().getMenu("bug"), wxGetApp().getTip("bug") );
  bug_menuHelp->SetBitmap(*_img_bug);
  menuHelp->Append(bug_menuHelp);

  wxMenuItem *feature_menuHelp = new wxMenuItem(menuHelp, ME_Feature, wxGetApp().getMenu("feature"), wxGetApp().getTip("feature") );
  feature_menuHelp->SetBitmap(*_img_lp);
  menuHelp->Append(feature_menuHelp);

  menuHelp->AppendSeparator();
  wxMenuItem *update_menuHelp = new wxMenuItem(menuHelp, ME_Update, wxGetApp().getMenu("softwareupdates"), wxGetApp().getTip("softwareupdates") );
  update_menuHelp->SetBitmap(*_img_updates);
  menuHelp->Append(update_menuHelp);

  menuHelp->AppendSeparator();
  wxMenuItem *donkey_menuHelp = new wxMenuItem(menuHelp, ME_DonKeyInfo, wxGetApp().getMenu("donkey"), wxGetApp().getTip("donkey") );
  menuHelp->Append(donkey_menuHelp);

#ifndef __APPLE__
  menuHelp->AppendSeparator();
#endif

  wxMenuItem *info_menuHelp = new wxMenuItem(menuHelp, wxID_ABOUT, wxGetApp().getMenu("about"), wxGetApp().getTip("about") );
  info_menuHelp->SetBitmap(*_img_info);
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
  SetStatusText( _T("0mA"), status_load );


  // Create tool bar
  m_ToolBar = CreateToolBar();
  m_ToolBar->SetToolBitmapSize(wxSize(24, 24));


  m_ToolBar->AddTool(ME_Connect, wxGetApp().getMsg("connect"), *_img_connect, wxGetApp().getTip("connect") );
  //m_ToolBar->AddTool(ME_Upload, wxGetApp().getMsg("upload"), *_img_upload, wxGetApp().getTip("upload") );
  m_ToolBar->AddTool(ME_OpenWorkspace, wxGetApp().getMsg("openworkspace"), *_img_system, wxGetApp().getTip("openworkspace") );

#ifndef __APPLE__
  m_ToolBar->AddSeparator();
#endif

  m_ToolBar->AddTool(ME_New, wxGetApp().getMsg("new"), *_img_new, wxGetApp().getTip("new") );
  m_ToolBar->AddTool(ME_Open, wxGetApp().getMsg("open"), *_img_open, wxGetApp().getTip("open") );
  m_ToolBar->AddTool(ME_Save, wxGetApp().getMsg("save"), *_img_save, wxGetApp().getTip("save") );
  m_ToolBar->AddTool(ME_Undo, wxGetApp().getMsg("undo"), *_img_undo, wxGetApp().getTip("undo") );
  m_ToolBar->EnableTool(ME_Undo, false );

#ifndef __APPLE__
  m_ToolBar->AddSeparator();
#endif

  m_ToolBar->AddCheckTool(ME_Go, wxGetApp().getMenu("poweron"), *_img_poweron,
                        wxNullBitmap, wxGetApp().getTip("poweron") );
  m_ToolBar->AddCheckTool(ME_AutoMode, wxGetApp().getMenu("automode"), *_img_automode,
                        wxNullBitmap, wxGetApp().getTip("automode") );
  m_ToolBar->AddTool(ME_AutoStop, wxGetApp().getMsg("stopall"), *_img_stopall, wxGetApp().getTip("stopall") );

#ifndef __APPLE__
  m_ToolBar->AddSeparator();
#endif

  m_ToolBar->AddTool(ME_OperatorDlg, wxGetApp().getMsg("operator"), *_img_operator, wxGetApp().getTip("operator") );
  m_ToolBar->AddTool(ME_MIC, wxGetApp().getMsg("mic"), *_img_mic, wxGetApp().getTip("mic") );
  m_ToolBar->AddTool(ME_LcDlg, wxGetApp().getMsg("locctrl"), *_img_locctrl, wxGetApp().getTip("locctrl") );
  m_ToolBar->AddTool(ME_SwDlg, wxGetApp().getMsg("swctrl"), *_img_swctrl, wxGetApp().getTip("swctrl") );
  m_ToolBar->AddTool(ME_RouteDlg, wxGetApp().getMsg("stctrl"), *_img_routes, wxGetApp().getTip("stctrl") );

#ifndef __APPLE__
  m_ToolBar->AddSeparator();
#endif

  m_ScaleComboBox = new wxComboBox(m_ToolBar, ID_SCALE_COMBO, wxEmptyString, wxDefaultPosition, wxSize(80,-1), 0, NULL, wxTE_PROCESS_ENTER );
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


#ifndef __APPLE__
  m_ToolBar->AddSeparator();
#endif

  m_ToolBar->AddTool(ME_Update, wxGetApp().getMsg("softwareupdates"), *_img_updates, wxGetApp().getTip("softwareupdates") );
  m_ToolBar->AddTool(wxID_HELP, wxGetApp().getMsg("documentation"), *_img_manual, wxGetApp().getTip("documentation") );

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
    m_SymbolMap = svg->readSvgSymbols( m_ThemePath, NULL, m_ThemesPath );
  else
    m_SymbolMap = svg->readSvgSymbols( wPlanPanel.getsvgpath( wGui.getplanpanel(m_Ini) ), NULL, m_ThemesPath );

  m_SymbolMap = svg->readSvgSymbols( wPlanPanel.getsvgpath2( wGui.getplanpanel(m_Ini) ), m_SymbolMap, m_ThemesPath );
  m_SymbolMap = svg->readSvgSymbols( wPlanPanel.getsvgpath3( wGui.getplanpanel(m_Ini) ), m_SymbolMap, m_ThemesPath );
  m_SymbolMap = svg->readSvgSymbols( wPlanPanel.getsvgpath4( wGui.getplanpanel(m_Ini) ), m_SymbolMap, m_ThemesPath );
  m_SymbolMap = svg->readSvgSymbols( wPlanPanel.getsvgpath5( wGui.getplanpanel(m_Ini) ), m_SymbolMap, m_ThemesPath );
  delete svg;

  // check for theme properties:
  char* propPath = NULL;

  if( FileOp.isAbsolute( wPlanPanel.getsvgpath( wGui.getplanpanel(m_Ini) ) ) )
    propPath = StrOp.fmt( "%s%cproperties.xml", wPlanPanel.getsvgpath( wGui.getplanpanel(m_Ini) ), SystemOp.getFileSeparator() );
  else
    propPath = StrOp.fmt( "%s%c%s%cproperties.xml", m_ThemesPath, SystemOp.getFileSeparator(), wPlanPanel.getsvgpath( wGui.getplanpanel(m_Ini) ), SystemOp.getFileSeparator() );

  iOFile f = FileOp.inst( propPath, OPEN_READONLY );
  if( f != NULL && FileOp.size(f) > 0 ) {
    char* propsXml = (char*)allocMem( FileOp.size( f ) + 1 );
    FileOp.read( f, propsXml, FileOp.size( f ) );
    FileOp.close( f );
    iODoc doc = DocOp.parse( propsXml );
    if( doc != NULL ) {
      iONode themeProps = DocOp.getRootNode( doc );
      iONode themePanelProps = wTheme.getpanel(themeProps);
      iONode themeBlockProps = wTheme.getthemeblock(themeProps);
      iONode themeScProps    = wTheme.getshortcut(themeProps);
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
      if( themeScProps != NULL ) {
        wPlanPanel.setscred  ( wGui.getplanpanel(m_Ini), wShortcut.getred  (themeScProps) );
        wPlanPanel.setscgreen( wGui.getplanpanel(m_Ini), wShortcut.getgreen(themeScProps) );
        wPlanPanel.setscblue ( wGui.getplanpanel(m_Ini), wShortcut.getblue (themeScProps) );
        wPlanPanel.setshowsc ( wGui.getplanpanel(m_Ini), wShortcut.isshow  (themeScProps) );
      }
    }

  }
  StrOp.free(propPath);

  create();

  m_JsSupport = NULL;
  if( wGui.isjssupport(m_Ini) )
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
  m_bShowID = (wPlanPanel.isshowid( wGui.getplanpanel( m_Ini ) ) ? true:false);
  m_bRaster = (wPlanPanel.israster( wGui.getplanpanel( m_Ini ) ) ? true:false);
  m_bTooltip = (wPlanPanel.istooltip( wGui.getplanpanel( m_Ini ) ) ? true:false);
  m_bLocoBook = (wPlanPanel.islocobook( wGui.getplanpanel( m_Ini ) ) ? true:false);
  m_bPlanBook = (wPlanPanel.isplanbook( wGui.getplanpanel( m_Ini ) ) ? true:false);
  m_bTraceWindow = (wPlanPanel.istracewindow( wGui.getplanpanel( m_Ini ) ) ? true:false);

  if( !m_bLocoBook && !m_bPlanBook ) {
    m_bLocoBook = true;
    m_bPlanBook = true;
  }

  if( m_Scale < 0.1 )
    m_Scale = 0.5;

  m_StatNotebook = new wxNotebook( m_PlanSplitter, -1, wxDefaultPosition, wxDefaultSize, wxNB_TOP );

  m_ActiveLocsPanel = new wxPanel( m_StatNotebook, -1, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
  wxBoxSizer* activeLocsSizer = new wxBoxSizer(wxVERTICAL);
  m_ActiveLocsPanel->SetSizer(activeLocsSizer);

  wxBitmap m_LocImageBitmap(wxNullBitmap);
  m_LocImage = new BitmapButton( m_ActiveLocsPanel, -1, m_LocImageBitmap, wxDefaultPosition, wxSize(250,88), wxBU_AUTODRAW|wxBU_EXACTFIT );
  activeLocsSizer->Add(m_LocImage, 0, wxGROW|wxALL|wxADJUST_MINSIZE, 2);

  m_ActiveLocs = new wxGrid( m_ActiveLocsPanel, -1, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
  m_ActiveLocs->SetRowLabelSize(0);
  m_ActiveLocs->CreateGrid(1, 6, wxGrid::wxGridSelectRows);
  //wxFont* font = new wxFont( m_ActiveLocs->GetDefaultCellFont() );
  //font->SetPointSize( (int)(font->GetPointSize() - 1 ) );
  m_ActiveLocs->SetSelectionMode(wxGrid::wxGridSelectRows);
  //m_ActiveLocs->SetDefaultCellFont( *font );
  //m_ActiveLocs->SetDefaultRowSize(20);
  //m_ActiveLocs->SetColLabelSize(m_ActiveLocs->GetDefaultRowSize());
  m_ActiveLocs->SetColLabelValue(LOC_COL_ID, wxGetApp().getMsg("id") );
  m_ActiveLocs->SetColLabelValue(LOC_COL_ADDR, _("#__") );
  m_ActiveLocs->SetColLabelValue(LOC_COL_BLOCK, wxGetApp().getMsg("block") );
  m_ActiveLocs->SetColLabelValue(LOC_COL_V, _("V___") );
  m_ActiveLocs->SetColLabelValue(LOC_COL_MODE, wxGetApp().getMsg("mode") );
  m_ActiveLocs->SetColLabelValue(LOC_COL_DESTBLOCK, wxGetApp().getMsg("destination") );
  m_ActiveLocs->AutoSizeColumns();
  m_ActiveLocs->AutoSizeRows();

  activeLocsSizer->Add(m_ActiveLocs, 1, wxGROW|wxALL|wxADJUST_MINSIZE, 2);

  TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "Creating LocPanel..." );
  m_LCPanel = new wxPanel( m_ActiveLocsPanel, -1, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
  m_LC = NULL;
  m_LC = new LC( m_LCPanel );
  activeLocsSizer->Add(m_LCPanel, 0, wxGROW|wxALL|wxADJUST_MINSIZE, 2);

  m_LocImage->setLC(m_LC);


  m_StatNotebook->AddPage(m_ActiveLocsPanel, wxGetApp().getMsg("activelocs") );


  TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "Creating PT Panel..." );
  m_CVPanel = new wxScrolledWindow( m_StatNotebook, -1, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER||wxHSCROLL|wxVSCROLL );
  m_CVPanel->SetScrollbars(1, 1, 0, 0);
  m_CV = NULL;
  m_CV = new CV( m_CVPanel, wGui.getcvconf( m_Ini ), this );

  m_StatNotebook->AddPage(m_CVPanel, wxGetApp().getMsg("programming") );


  TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "Creating LNCV Panel..." );
  m_LNCVPanel = new wxScrolledWindow( m_StatNotebook, -1, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER||wxHSCROLL|wxVSCROLL );
  m_LNCVPanel->SetScrollbars(1, 1, 0, 0);
  m_LNCV = NULL;
  m_LNCV = new LNCV( m_LNCVPanel, this );
  m_LNCVPanel->Show(wGui.islncvtab(m_Ini));

  if( wGui.islncvtab(m_Ini) ) {
    m_StatNotebook->AddPage(m_LNCVPanel, wxGetApp().getMsg("lncvprogramming") );
  }

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

  m_ActiveLocsPanel->GetSizer()->Layout();
  m_StatNotebook->Fit();
  //GetSizer()->Fit(this);
  //GetSizer()->SetSizeHints(this);


  if( !m_bLocoBook ) {
    m_StatNotebook->Show(m_bLocoBook);
    m_PlanSplitter->SetSashPosition( 1 );
    m_PlanSplitter->Unsplit( m_StatNotebook );
  }
  if( !m_bPlanBook ) {
    m_PlanNotebook->Show(m_bPlanBook);
    m_PlanSplitter->SetSashPosition( 1 );
    m_PlanSplitter->Unsplit( m_PlanNotebook );
  }
  if( !m_bTraceWindow ) {
    m_Splitter->Unsplit( m_TraceSplitter );
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
  long      maxlen = 4 * 1024;
  int          len = StrOp.len(text);

  TraceOp.trc( "frame", TRCLEVEL_DEBUG, __LINE__, 9999, "Got an info message: %s", text );

  if( m_bTraceWindow && m_WarningPanel != NULL ) {
    long i = m_WarningPanel->GetLastPosition();
    if( i > maxlen )
      m_WarningPanel->Remove(0, len+1);
    if( level == TRCLEVEL_EXCEPTION ) {
      m_WarningPanel->SetDefaultStyle(wxTextAttr(*wxRED));
      m_WarningPanel->AppendText( wxString(text,wxConvUTF8) );
      m_WarningPanel->AppendText( _T("\n") );
      m_WarningPanel->ScrollLines(1);
    }
    else if( level == TRCLEVEL_WARNING ) {
      m_WarningPanel->SetDefaultStyle(wxTextAttr(*wxBLACK));
      m_WarningPanel->AppendText( wxString(text,wxConvUTF8) );
      m_WarningPanel->AppendText( _T("\n") );
      m_WarningPanel->ScrollLines(1);
    }
    else if( level == TRCLEVEL_CALC ) {
      m_WarningPanel->SetDefaultStyle(wxTextAttr(*wxBLUE));
      m_WarningPanel->AppendText( wxString(text,wxConvUTF8) );
      m_WarningPanel->AppendText( _T("\n") );
      m_WarningPanel->ScrollLines(1);
    }
    else if( level == TRCLEVEL_MONITOR && m_MonitorPanel != NULL) {
      long i = m_MonitorPanel->GetLastPosition();
      if( i > maxlen )
        m_MonitorPanel->Remove(0, len+1);
      m_MonitorPanel->SetDefaultStyle(wxTextAttr(*wxBLACK));
      m_MonitorPanel->AppendText( wxString(text,wxConvUTF8) );
      m_MonitorPanel->AppendText( _T("\n") );
      m_MonitorPanel->ScrollLines(1);
    }
    else {
      m_WarningPanel->SetDefaultStyle(wxTextAttr(*wxBLUE));
      m_WarningPanel->AppendText( wxString(text,wxConvUTF8) );
      m_WarningPanel->AppendText( _T("\n") );
      m_MonitorPanel->ScrollLines(1);
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
  ShutdownRocRail();
  wxGetApp().OnExit();
  Close(TRUE);
}

bool RocGuiFrame::ShutdownRocRail() {
  int action = wxMessageDialog( this, wxGetApp().getMsg("shutdownwarning"), _T("Rocrail"), wxYES_NO | wxICON_EXCLAMATION ).ShowModal();
  if( action == wxID_NO ) {
    return false;
  }
  iONode cmd = NodeOp.inst( wSysCmd.name(), NULL, ELEMENT_NODE );
  wSysCmd.setcmd( cmd, wSysCmd.shutdown );
  wxGetApp().sendToRocrail( cmd, true );
  cmd->base.del(cmd);
  m_bActiveWorkspace = false;
  return true;
}

void RocGuiFrame::OnConnect( wxCommandEvent& event ) {
  iONode ini = wxGetApp().getIni();
  ConnectionDialog* dlg = new ConnectionDialog( this, ini );

  if( wxID_OK == dlg->ShowModal() ) {
    CleanNotebook();
    wxGetApp().setModel(NULL);
    wxGetApp().setStayOffline(false);
    m_ControlCode = StrOp.dup( dlg->getControlCode().mb_str(wxConvUTF8) );
    Connect(StrOp.dup( dlg->getHostname().mb_str(wxConvUTF8) ), dlg->getPort());
	}
  dlg->Destroy();
}


void RocGuiFrame::Connect( const char* host, int port, bool wait4rr ) {
  wxGetApp().setHost( host, port );

  wxGetApp().setStayOffline( false );

  if( wxGetApp().sendToRocrail( (char*)NULL, wait4rr, false ) ) {

    iONode cmd = NodeOp.inst( wModelCmd.name(), NULL, ELEMENT_NODE );
    wModelCmd.setcmd( cmd, wModelCmd.themes );
    wxGetApp().sendToRocrail( cmd );
    cmd->base.del( cmd );

    // Initial connection.
    cmd = NodeOp.inst( wModelCmd.name(), NULL, ELEMENT_NODE );
    wModelCmd.setcmd( cmd, wModelCmd.plan );
    wModelCmd.setcontrolcode( cmd, m_ControlCode );
    TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999,"control code %s", m_ControlCode );
    wModelCmd.setdisablemonitor(cmd, wGui.ismonitoring(wxGetApp().getIni()) ? False:True);
    TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "monitoring is %s", wModelCmd.isdisablemonitor(cmd)?"off":"on" );
    wxGetApp().sendToRocrail( cmd );
    cmd->base.del( cmd );

    this->setOnline( true );
  }
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

  // TODO: why this? wxGetApp().setStayOffline( true );
  wxGetApp().setLocalModelModified(false);
}

void RocGuiFrame::OnAnalyze( wxCommandEvent& event ) {
  if( !wxGetApp().isOffline() ) {
    iONode cmd = NodeOp.inst( wSysCmd.name(), NULL, ELEMENT_NODE );
    wSysCmd.setcmd( cmd, wSysCmd.analyze );
    if( event.GetId() == ME_AnalyzeClean ) {
      wSysCmd.setval( cmd, 1 );
    }
    wxGetApp().sendToRocrail( cmd );
    cmd->base.del(cmd);
  }
}


void RocGuiFrame::OnGoOffline( wxCommandEvent& event ) {
  if( !wxGetApp().isOffline() ) {
    wxGetApp().disConnect();
    setOnline(false);
    iONode plan = NodeOp.inst( wPlan.name(), NULL, ELEMENT_NODE);
    wxGetApp().Callback( (obj)&wxGetApp(), plan );
    NodeOp.base.del(plan);
  }
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
  TraceOp.trc( "cv", TRCLEVEL_INFO, __LINE__, 9999, "openpath=%s", l_openpath );
  wxString ms_FileExt = wxGetApp().getMsg("planfiles");
  wxFileDialog* fdlg = new wxFileDialog(this, wxGetApp().getMenu("saveplanfileas"), wxString(l_openpath,wxConvUTF8), m_LocalPlan, ms_FileExt, wxFD_SAVE);
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
      //wPlan.settitle(node, FileOp.ripPath(m_LocalPlan.mb_str(wxConvUTF8)));
      wxGetApp().setStayOffline( true );
      wxGetApp().Callback( (obj)&wxGetApp(), node );
      SetStatusText( plan, status_rcon );

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
  TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "openpath=%s", l_openpath );
  wxFileDialog* fdlg = new wxFileDialog(this, wxGetApp().getMenu("openplanfile"), wxString(l_openpath,wxConvUTF8) , _T(""), ms_FileExt, wxFD_OPEN);
  if( fdlg->ShowModal() == wxID_OK ) {
    setLocalPlan( fdlg->GetPath() );
    wGui.setopenpath( wxGetApp().getIni(), fdlg->GetPath().mb_str(wxConvUTF8) );
    // strip filename:
    wGui.setopenpath( wxGetApp().getIni(), FileOp.getPath(wGui.getopenpath( wxGetApp().getIni() ) ) );
  }
  fdlg->Destroy();
}

void RocGuiFrame::OnSetStatusText( wxCommandEvent& event ) {
  SetStatusText(event.GetString(), event.GetExtraLong());
}


void RocGuiFrame::OnOpenWorkspace( wxCommandEvent& event ) {
  if( !wxGetApp().isOffline() ) {
    return;
  }

  int idx = event.GetId() - ME_OpenWorkspace;
  char* workspace = NULL;

  if( idx == 0 ) {
    if( wGui.isstartdefaultworkspace(wxGetApp().getIni()) && StrOp.len( wGui.getdefaultworkspace(wxGetApp().getIni()) ) > 0 ) {
      workspace = StrOp.dup( wGui.getdefaultworkspace(wxGetApp().getIni()) );
    }
    else if( event.GetExtraLong() != 4711 ) {
      wxDirDialog* dlg = new wxDirDialog( this );
      if( StrOp.len( wGui.getdefaultworkspace(wxGetApp().getIni()) ) > 0 )
        dlg->SetPath(wxString(wGui.getdefaultworkspace(wxGetApp().getIni()),wxConvUTF8));
      if( dlg->ShowModal() == wxID_OK ) {
        workspace = StrOp.dup((const char*)dlg->GetPath().mb_str(wxConvUTF8));
      }
    }
  }
  else {
    // get the workspace path from ini
    int i = 1;
    iONode workspaces = wGui.getworkspaces(m_Ini);
    if( workspaces != NULL ) {
      iONode ws = wWorkSpaces.getworkspace(workspaces);
      while( ws != NULL ) {
        if( i == idx ) {
          workspace = StrOp.dup(wWorkSpace.getpath(ws));
          break;
        }
        i++;
        ws = wWorkSpaces.nextworkspace(workspaces, ws);
      };
    }
  }

  if( workspace != NULL ) {
    // Open Workspace
    if(m_ServerPath == NULL)
      m_ServerPath = FileOp.pwd();

    char* rr = StrOp.fmt( "%s%crocrail%s", m_ServerPath, SystemOp.getFileSeparator(), SystemOp.getPrgExt() );
    if( FileOp.exist( rr ) ) {
#if defined _WIN32
      char* rrcall = StrOp.fmt( "\"\" \"%s%crocrail%s\" -l \"%s\" -w \"%s\"", m_ServerPath, SystemOp.getFileSeparator(), SystemOp.getPrgExt(), m_ServerPath, workspace );
#else
      char* rrcall = StrOp.fmt( "%s%crocrail%s -l \"%s\" -w \"%s\"", m_ServerPath, SystemOp.getFileSeparator(), SystemOp.getPrgExt(), m_ServerPath, workspace );
#endif
      TraceOp.trc( "frame", TRCLEVEL_WARNING, __LINE__, 9999, "open workspace=\"%s\"", rrcall );

      CleanNotebook();
      wxGetApp().setModel(NULL);


      SystemOp.system( rrcall, True, True );
      StrOp.free(rrcall);
      m_bActiveWorkspace = true;
      m_WorkSpace = NULL;
      Connect( "localhost", 8051, true); // TODO: add const to the wrapper.xml for the defaults.

      iONode workspaces = wGui.getworkspaces(m_Ini);
      if( workspaces == NULL ) {
        workspaces = NodeOp.inst( wWorkSpaces.name(), m_Ini, ELEMENT_NODE );
        NodeOp.addChild( m_Ini, workspaces );
      }

      m_WorkSpace = wWorkSpaces.getworkspace(workspaces);
      Boolean hasWS = False;
      while( m_WorkSpace != NULL && !hasWS) {
        hasWS = StrOp.equals(wWorkSpace.getpath(m_WorkSpace), workspace);
        if( hasWS )
          break;
        m_WorkSpace = wWorkSpaces.nextworkspace(workspaces, m_WorkSpace);
      };

      if( !hasWS ) {
        // save the most recent 8 workspaces
        iOList list = ListOp.inst();
        int cnt = 0;
        iONode ws = wWorkSpaces.getworkspace(workspaces);
        while( ws != NULL && cnt < 8 ) {
          ListOp.add( list, (obj)ws);
          cnt++;
          ws = wWorkSpaces.nextworkspace(workspaces, ws);
        };

        // remove all from parent
        ws = wWorkSpaces.getworkspace(workspaces);
        while( ws != NULL ) {
          NodeOp.removeChild( workspaces, ws );
          ws = wWorkSpaces.getworkspace(workspaces);
        };

        // Add workspace to the ini
        m_WorkSpace = NodeOp.inst(wWorkSpace.name(), workspaces, ELEMENT_NODE);
        wWorkSpace.setpath(m_WorkSpace, workspace);
        NodeOp.addChild(workspaces,m_WorkSpace);
        for( int i = 0; i < ListOp.size(list); i++ ) {
          NodeOp.addChild(workspaces,(iONode)ListOp.get(list, i));
        }
        ListOp.base.del(list);
      }

    }
    else {
      wxMessageDialog( this,
          wxGetApp().getMsg("rocrailservernotfound") + _T(" ") +
          wxString(rr,wxConvUTF8),
          _T("Rocrail"), wxOK | wxICON_EXCLAMATION ).ShowModal();
    }

    StrOp.free(workspace);
    StrOp.free(rr);
  }
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
  wxFileDialog* fdlg = new wxFileDialog(this, wxGetApp().getMenu("openplanfile"), _T("."), _T(""), ms_FileExt, wxFD_OPEN);
  if( fdlg->ShowModal() == wxID_OK ) {
    // Read and send to Rocrail:
    if( fdlg->GetPath().Len() > 0 ) {
      char* fname = StrOp.dup( fdlg->GetPath().mb_str(wxConvUTF8) );
      iOFile f = FileOp.inst( fname, OPEN_READONLY );
      if( f != NULL ) {
        char* buffer = (char*)allocMem( FileOp.size( f ) +1 );
        FileOp.read( f, buffer, FileOp.size( f ) );
        FileOp.base.del( f );
        // save file name in the name attribute:
        iODoc plan = DocOp.parse( buffer );
        if( plan != NULL ) {
          iONode root = DocOp.getRootNode( plan );
          if( root != NULL ) {
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
            DocOp.base.del( plan );
            TraceOp.trc( "frame", TRCLEVEL_WARNING, __LINE__, 9999, "Plan [%s] is not well formed.", FileOp.ripPath(fname) );
          }
        }
        else {
          TraceOp.trc( "frame", TRCLEVEL_WARNING, __LINE__, 9999, "Plan [%s] does not exist.", FileOp.ripPath(fname) );
        }
        freeMem( buffer );
      }
      else {
        TraceOp.trc( "frame", TRCLEVEL_WARNING, __LINE__, 9999, "Plan [%s] not valid.", FileOp.ripPath(fname) );
      }
      StrOp.free(fname);
    }
  }
  fdlg->Destroy();
}

void RocGuiFrame::OnGo( wxCommandEvent& event ) {
  TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "GO" );
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
    wSysCmd.setcmd( cmd, wSysCmd.ebreak );
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

void RocGuiFrame::OnQuerySensors( wxCommandEvent& event ) {
  iONode cmd = NodeOp.inst( wSysCmd.name(), NULL, ELEMENT_NODE );
  wSysCmd.setcmd( cmd, wSysCmd.sod );
  wSysCmd.setinformall( cmd, True );
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

void RocGuiFrame::OnEditModPlan( wxCommandEvent& event ) {
  wxMenuItem* mi_editmodplan = menuBar->FindItem(ME_EditModPlan);
  if( mi_editmodplan == NULL )
    return;

  m_bEditModPlan = mi_editmodplan->IsChecked();
  TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "EditModPlan is %s", m_bEditModPlan?"true":"false" );
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
  int zoom = atoi(event.GetString().mb_str(wxConvUTF8));
  TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "Combobox zoom=%d", zoom );
  if( zoom < 0 || zoom > 200 ) {
    ((wxComboBox*)event.GetEventObject())->SetValue(_T(""));
  }
}

void RocGuiFrame::OnScaleCombo(wxCommandEvent& event)
{
  TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999,
      "Combobox string '%s' selected", (const char*)m_ScaleComboBox->GetValue().c_str() );
  int zoom = 100;

  if( m_ScaleComboBox->GetValue().IsNumber() ) {
    zoom = atoi(m_ScaleComboBox->GetValue().mb_str(wxConvUTF8));
    if( zoom < 0 || zoom > 200 ) {
      m_ScaleComboBox->SetValue(_T(""));
    }
    else {
      Zoom(zoom);
    }
  }
  else {
    m_ScaleComboBox->SetValue(_T(""));
  }

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


void RocGuiFrame::OnLocoViewAll( wxCommandEvent& event ) {
  m_LocoCategory = LOCO_VIEW_ALL;
  event.SetClientData(NULL);
  InitActiveLocs(event);
}

void RocGuiFrame::OnLocoViewSteam( wxCommandEvent& event ) {
  m_LocoCategory = LOCO_VIEW_STEAM;
  event.SetClientData(NULL);
  InitActiveLocs(event);
}

void RocGuiFrame::OnLocoViewDiesel( wxCommandEvent& event ) {
  m_LocoCategory = LOCO_VIEW_DIESEL;
  event.SetClientData(NULL);
  InitActiveLocs(event);
}

void RocGuiFrame::OnLocoViewElectric( wxCommandEvent& event ) {
  m_LocoCategory = LOCO_VIEW_ELECTRIC;
  event.SetClientData(NULL);
  InitActiveLocs(event);
}

void RocGuiFrame::OnLocoViewCommuter( wxCommandEvent& event ) {
  m_LocoCategory = LOCO_VIEW_COMMUTER;
  event.SetClientData(NULL);
  InitActiveLocs(event);
}

void RocGuiFrame::OnLocoViewSpecial( wxCommandEvent& event ) {
  m_LocoCategory = LOCO_VIEW_SPECIAL;
  event.SetClientData(NULL);
  InitActiveLocs(event);
}

void RocGuiFrame::OnTraceWindow( wxCommandEvent& event ) {
  wxMenuItem* mi = menuBar->FindItem(ME_TraceWindow);
  m_bTraceWindow = mi->IsChecked();
  wPlanPanel.settracewindow( wGui.getplanpanel( m_Ini ), m_bTraceWindow ? True:False);
  if(m_bTraceWindow) {
    int pos = wSplitPanel.getmain( wGui.getsplitpanel( m_Ini) );
    m_Splitter->SplitHorizontally( m_PlanSplitter, m_TraceSplitter, pos );
  }
  else {
    m_Splitter->Unsplit( m_TraceSplitter );
  }
}

void RocGuiFrame::OnLocoBook( wxCommandEvent& event ) {
  wxMenuItem* mi = menuBar->FindItem(ME_LocoBook);
  m_bLocoBook = mi->IsChecked();
  if( !m_bLocoBook && !m_bPlanBook ) {
    m_bLocoBook = true;
    return;
  }
  if(m_bLocoBook) {
    int pos = wSplitPanel.getplan( wGui.getsplitpanel( m_Ini) );
    m_PlanSplitter->SplitVertically( m_StatNotebook, m_PlanNotebook, pos );
  }
  else {
    m_PlanSplitter->Unsplit( m_StatNotebook );
  }
}

void RocGuiFrame::OnPlanBook( wxCommandEvent& event ) {
  wxMenuItem* mi = menuBar->FindItem(ME_PlanBook);
  m_bPlanBook = mi->IsChecked();
  if( !m_bLocoBook && !m_bPlanBook ) {
    m_bPlanBook = true;
    return;
  }
  if(m_bPlanBook) {
    int pos = wSplitPanel.getplan( wGui.getsplitpanel( m_Ini) );
    m_PlanSplitter->SplitVertically( m_StatNotebook, m_PlanNotebook, pos );
  }
  else {
    m_PlanSplitter->Unsplit( m_PlanNotebook );
  }
}

void RocGuiFrame::OnLocoSortByAddr( wxCommandEvent& event ) {
  wxMenuItem* mi = menuBar->FindItem(ME_LocoSortByAddr);
  m_LocoSortByAddress = mi->IsChecked();
  wGui.setlocosortbyaddress(m_Ini, m_LocoSortByAddress?True:False );
  event.SetClientData(NULL);
  InitActiveLocs(event);
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
      p->SetBackgroundColor(color.Red(),color.Green(),color.Blue(), true);
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

void RocGuiFrame::OnTooltip( wxCommandEvent& event ) {
  wxMenuItem* mi_tooltip = menuBar->FindItem(ME_Tooltip);
  m_bTooltip = mi_tooltip->IsChecked();

  int pages = m_PlanNotebook->GetPageCount();
  for( int i = 0; i < pages; i++ ) {
    BasePanel* p = (BasePanel*)m_PlanNotebook->GetPage(i);
    p->showTooltip( m_bTooltip );
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
  dlg->Show(TRUE);

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

void RocGuiFrame::OnEditTours( wxCommandEvent& event ) {
  ToursDlg*  dlg = new ToursDlg(this, (iONode)NULL, true );
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

void RocGuiFrame::OnEditBoosters( wxCommandEvent& event ) {
  PowerManDlg*  dlg = new PowerManDlg(this );
  if( wxID_OK == dlg->ShowModal() ) {
    /* Notify RocRail. */
  }
  dlg->Destroy();
}

void RocGuiFrame::OnCtrlBoosters( wxCommandEvent& event ) {
  m_PowerCtrl = new PowerCtrlDlg(this );
  if( wxID_OK == m_PowerCtrl->ShowModal() ) {
    /* Notify RocRail. */
  }
  m_PowerCtrl->Destroy();
  m_PowerCtrl = NULL;
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


void RocGuiFrame::OnEditActions( wxCommandEvent& event ) {
  if( wxGetApp().getModel() == NULL )
    return;
  iONode system = wPlan.getsystem(wxGetApp().getModel());
  if( system == NULL ) {
    system = NodeOp.inst(wSystemActions.name(), wxGetApp().getModel(), ELEMENT_NODE );
    NodeOp.addChild(wxGetApp().getModel(), system);
  }
  ActionsCtrlDlg*  dlg = new ActionsCtrlDlg(this, system );

  if( wxID_OK == dlg->ShowModal() ) {
    // TODO: inform
  }

  dlg->Destroy();
}


void RocGuiFrame::OnUhl63350( wxCommandEvent& event ) {
  m_Uhl633x0 = new Uhl633x0Dlg(this);
  if( wxID_OK == m_Uhl633x0->ShowModal() ) {
    /* Notify RocRail. */
  }
  m_Uhl633x0->Destroy();
  m_Uhl633x0 = NULL;
}

void RocGuiFrame::OnUhl68610( wxCommandEvent& event ) {
  m_Uhl68610 = new Uhl68610Dlg(this);
  if( wxID_OK == m_Uhl68610->ShowModal() ) {
    /* Notify RocRail. */
  }
  m_Uhl68610->Destroy();
  m_Uhl68610 = NULL;
}

void RocGuiFrame::OnMGV( wxCommandEvent& event ) {
  m_MGV = new MGVDlg(this);
  if( wxID_OK == m_MGV->ShowModal() ) {
    /* Notify RocRail. */
  }
  m_MGV->Destroy();
  m_MGV = NULL;
}

void RocGuiFrame::OnMGV141( wxCommandEvent& event ) {
  m_MGV141 = new MGV141Dlg(this);
  if( wxID_OK == m_MGV141->ShowModal() ) {
    /* Notify RocRail. */
  }
  m_MGV141->Destroy();
  m_MGV141 = NULL;
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

void RocGuiFrame::OnCBusNode( wxCommandEvent& event ) {
  m_CBusNodeDlg = new CBusNodeDlg(this);
  m_CBusNodeDlg->ShowModal();
  m_CBusNodeDlg->Destroy();
  m_CBusNodeDlg = NULL;
}

void RocGuiFrame::setOnline( bool online ) {
  GetToolBar()->EnableTool(ME_New, !online);
  GetToolBar()->EnableTool(ME_Open, !online);
  GetToolBar()->EnableTool(ME_Upload, online);
  GetToolBar()->EnableTool(ME_Connect, (!online && !m_bActiveWorkspace) );
  GetToolBar()->EnableTool(ME_OpenWorkspace, (!m_bActiveWorkspace && !online) );
}


void RocGuiFrame::OnDonKey( wxCommandEvent& event ) {
  long expdays = 0;
  if( m_SymbolMap == NULL || MapOp.size(m_SymbolMap) == 0 ) {
    wxMessageDialog( this, wxGetApp().getMsg("setupsvgtheme"), _T("Rocrail"), wxOK | wxICON_EXCLAMATION ).ShowModal();
  }
  if( SystemOp.isExpired(SystemOp.decode(StrOp.strToByte(wxGetApp().m_donkey), StrOp.len(wxGetApp().m_donkey)/2, wxGetApp().m_doneml), NULL, &expdays) ) {
    DonKey* dlg = new DonKey(this, m_Ini );
    if( wxID_OK == dlg->ShowModal() ) {
    }
    dlg->Destroy();
  }
  else if( expdays < 8 && expdays >= 0 ) {
    DonKey* dlg = new DonKey(this, m_Ini );
    if( wxID_OK == dlg->ShowModal() ) {
    }
    dlg->Destroy();
  }
}

void RocGuiFrame::OnDonKeyInfo( wxCommandEvent& event ) {
  DonKey* dlg = new DonKey(this, m_Ini );
  if( wxID_OK == dlg->ShowModal() ) {
  }
  dlg->Destroy();
}

void RocGuiFrame::OnMenu( wxMenuEvent& event ) {
  bool l_bOffline = wxGetApp().isOffline();
  Boolean l_Expired = SystemOp.isExpired(SystemOp.decode(StrOp.strToByte(wxGetApp().m_donkey), StrOp.len(wxGetApp().m_donkey)/2, wxGetApp().m_doneml), NULL, NULL);

  wxMenuItem* mi = menuBar->FindItem(ME_New);
  if( mi != NULL ) mi->Enable( l_bOffline );


  GetToolBar()->EnableTool(ME_Open, l_bOffline);

  mi = menuBar->FindItem(ME_GoOffline);
  if( mi != NULL ) mi->Enable( !l_bOffline );

  mi = menuBar->FindItem(ME_Open);
  if( mi != NULL ) mi->Enable( l_bOffline );
  //mi = menuBar->FindItem(ME_SaveAs);
    //if( mi != NULL ) mi->Enable( l_bOffline );
  mi = menuBar->FindItem(ME_Upload);
  if( mi != NULL ) mi->Enable( !l_bOffline );
  mi = menuBar->FindItem(ME_Analyze);
  if( mi != NULL ) mi->Enable( !l_bOffline && !m_bAutoMode );
  mi = menuBar->FindItem(ME_ShutdownRocRail);
  if( mi != NULL ) mi->Enable( (!l_bOffline && !wxGetApp().isConsoleMode()) );
  mi = menuBar->FindItem(ME_Quit);
  if( mi != NULL ) mi->Enable( (!m_bActiveWorkspace) );
  mi = menuBar->FindItem(ME_OpenWorkspace);
  if( mi != NULL ) mi->Enable( (!m_bActiveWorkspace) && l_bOffline );
  mi = menuBar->FindItem(ME_RecentWorkspaces);
  if( mi != NULL ) mi->Enable( (!m_bActiveWorkspace) && l_bOffline );

  mi = menuBar->FindItem(ME_Go);
  if( mi != NULL ) mi->Enable( !l_bOffline );
  mi = menuBar->FindItem(ME_Stop);
  if( mi != NULL ) mi->Enable( !l_bOffline );
  mi = menuBar->FindItem(ME_EmergencyBreak);
  if( mi != NULL ) mi->Enable( !l_bOffline );
  mi = menuBar->FindItem(ME_InitField);
  if( mi != NULL ) mi->Enable( !l_bOffline && !m_bAutoMode );
  mi = menuBar->FindItem(ME_QuerySensors);
  if( mi != NULL ) mi->Enable( !l_bOffline && !m_bAutoMode );

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
  mi = menuBar->FindItem(ME_EditTours);
  if( mi != NULL ) mi->Enable( !m_bAutoMode || !wxGetApp().isRestrictedEdit() );
  mi = menuBar->FindItem(ME_EditLocations);
  if( mi != NULL ) mi->Enable( !m_bAutoMode || !wxGetApp().isRestrictedEdit() );
  mi = menuBar->FindItem(ME_EditTurntables);
  if( mi != NULL ) mi->Enable( !m_bAutoMode || !wxGetApp().isRestrictedEdit() );

  if( wxGetApp().isModView() ) {
    mi = menuBar->FindItem(ME_EditMode);
    if( mi != NULL ) mi->Enable( false );
    mi = menuBar->FindItem(ME_EditModPlan);
    if( mi != NULL ) mi->Enable( true );
    mi = menuBar->FindItem(ME_CtrlMode);
    if( mi != NULL ) mi->Enable( false );
    mi = menuBar->FindItem(ME_AddPanel);
    if( mi != NULL ) mi->Enable( false );
    mi = menuBar->FindItem(ME_PanelProps);
    if( mi != NULL ) mi->Enable( false );
  }
  else {
    mi = menuBar->FindItem(ME_EditMode);
    if( mi != NULL ) mi->Enable( !m_bAutoMode );
    mi = menuBar->FindItem(ME_EditModPlan);
    if( mi != NULL ) mi->Enable( false );
    mi = menuBar->FindItem(ME_CtrlMode);
    if( mi != NULL ) mi->Enable( !m_bAutoMode );
    mi = menuBar->FindItem(ME_AddPanel);
    if( mi != NULL ) mi->Enable( !m_bAutoMode );
    mi = menuBar->FindItem(ME_PanelProps);
    if( mi != NULL ) mi->Enable( !m_bAutoMode );
  }

  mi = menuBar->FindItem(ME_LocoViewAll);
  if( mi != NULL ) mi->Check( m_LocoCategory == LOCO_VIEW_ALL );
  mi = menuBar->FindItem(ME_LocoViewSteam);
  if( mi != NULL ) mi->Check( m_LocoCategory == LOCO_VIEW_STEAM );
  mi = menuBar->FindItem(ME_LocoViewDiesel);
  if( mi != NULL ) mi->Check( m_LocoCategory == LOCO_VIEW_DIESEL );
  mi = menuBar->FindItem(ME_LocoViewElectric);
  if( mi != NULL ) mi->Check( m_LocoCategory == LOCO_VIEW_ELECTRIC );
  mi = menuBar->FindItem(ME_LocoViewCommuter);
  if( mi != NULL ) mi->Check( m_LocoCategory == LOCO_VIEW_COMMUTER );
  mi = menuBar->FindItem(ME_LocoViewSpecial);
  if( mi != NULL ) mi->Check( m_LocoCategory == LOCO_VIEW_SPECIAL );

  mi = menuBar->FindItem(ME_LangEnglish);
  if( mi != NULL )
    mi->Check( StrOp.equals( wGui.lang_english, wGui.getlang( wxGetApp().getIni() ) ) );
  mi = menuBar->FindItem(ME_LangEnglishAU);
  if( mi != NULL )
    mi->Check( StrOp.equals( wGui.lang_englishAU, wGui.getlang( wxGetApp().getIni() ) ) );
  mi = menuBar->FindItem(ME_LangEnglishCA);
  if( mi != NULL )
    mi->Check( StrOp.equals( wGui.lang_englishCA, wGui.getlang( wxGetApp().getIni() ) ) );
  mi = menuBar->FindItem(ME_LangEnglishCA);
  if( mi != NULL )
    mi->Check( StrOp.equals( wGui.lang_englishGB, wGui.getlang( wxGetApp().getIni() ) ) );
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
  mi = menuBar->FindItem(ME_LangBosnian);
  if( mi != NULL )
    mi->Check( StrOp.equals( wGui.lang_bulgarian, wGui.getlang( wxGetApp().getIni() ) ) );
  mi = menuBar->FindItem(ME_LangBulgarian);
  if( mi != NULL )
    mi->Check( StrOp.equals( wGui.lang_catalan, wGui.getlang( wxGetApp().getIni() ) ) );
  mi = menuBar->FindItem(ME_LangCatalan);
  if( mi != NULL )
    mi->Check( StrOp.equals( wGui.lang_catalan, wGui.getlang( wxGetApp().getIni() ) ) );
  mi = menuBar->FindItem(ME_LangGreek);
  if( mi != NULL )
    mi->Check( StrOp.equals( wGui.lang_greek, wGui.getlang( wxGetApp().getIni() ) ) );
  mi = menuBar->FindItem(ME_LangRussian);
  if( mi != NULL )
    mi->Check( StrOp.equals( wGui.lang_russian, wGui.getlang( wxGetApp().getIni() ) ) );
  mi = menuBar->FindItem(ME_LangRomanian);
  if( mi != NULL )
    mi->Check( StrOp.equals( wGui.lang_romanian, wGui.getlang( wxGetApp().getIni() ) ) );
  mi = menuBar->FindItem(ME_LangPortuguese);
  if( mi != NULL )
    mi->Check( StrOp.equals( wGui.lang_portuguese, wGui.getlang( wxGetApp().getIni() ) ) );
  mi = menuBar->FindItem(ME_LangHungarian);
  if( mi != NULL )
    mi->Check( StrOp.equals( wGui.lang_hungarian, wGui.getlang( wxGetApp().getIni() ) ) );
  mi = menuBar->FindItem(ME_LangPolish);
  if( mi != NULL )
    mi->Check( StrOp.equals( wGui.lang_polish, wGui.getlang( wxGetApp().getIni() ) ) );
  mi = menuBar->FindItem(ME_LangNorwegian);
  if( mi != NULL )
    mi->Check( StrOp.equals( wGui.lang_norwegian, wGui.getlang( wxGetApp().getIni() ) ) );
  mi = menuBar->FindItem(ME_LangChinese);
  if( mi != NULL )
    mi->Check( StrOp.equals( wGui.lang_chinese, wGui.getlang( wxGetApp().getIni() ) ) );
  mi = menuBar->FindItem(ME_LangIndonesian);
  if( mi != NULL )
    mi->Check( StrOp.equals( wGui.lang_indonesian, wGui.getlang( wxGetApp().getIni() ) ) );
  mi = menuBar->FindItem(ME_LangJapanese);
  if( mi != NULL )
    mi->Check( StrOp.equals( wGui.lang_japanese, wGui.getlang( wxGetApp().getIni() ) ) );
  mi = menuBar->FindItem(ME_LangChineseCH);
  if( mi != NULL )
    mi->Check( StrOp.equals( wGui.lang_chinesesimplified, wGui.getlang( wxGetApp().getIni() ) ) );
  mi = menuBar->FindItem(ME_LangKorean);
  if( mi != NULL )
    mi->Check( StrOp.equals( wGui.lang_korean, wGui.getlang( wxGetApp().getIni() ) ) );
  mi = menuBar->FindItem(ME_LangPortugueseBR);
  if( mi != NULL )
    mi->Check( StrOp.equals( wGui.lang_portuguesebr, wGui.getlang( wxGetApp().getIni() ) ) );

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
    mi->Enable( l_bOffline && !m_bActiveWorkspace);

  GetToolBar()->EnableTool(ME_Connect, (l_bOffline && !m_bActiveWorkspace) );
  GetToolBar()->EnableTool(ME_OpenWorkspace, (!m_bActiveWorkspace && l_bOffline) );

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
  wxMenuItem* mi_planbook  = menuBar->FindItem(ME_PlanBook);
  mi_planbook->Check( m_bPlanBook );
  wxMenuItem* mi_tracewindow  = menuBar->FindItem(ME_TraceWindow);
  mi_tracewindow->Check( m_bTraceWindow );
  wxMenuItem* mi_showid  = menuBar->FindItem(ME_ShowID);
  mi_showid->Check( m_bShowID );
  wxMenuItem* mi_raster  = menuBar->FindItem(ME_Raster);
  mi_raster->Check( m_bRaster );
  wxMenuItem* mi_tooltip  = menuBar->FindItem(ME_Tooltip);
  mi_tooltip->Check( m_bTooltip );

}

void RocGuiFrame::OnAutoMode( wxCommandEvent& event ) {
  wxMenuItem* mi_automode = menuBar->FindItem(ME_AutoMode);
  if( mi_automode == NULL )
    return;

  //OnDonKey(event);

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
  TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "shutting down..." );
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

  if( m_bActiveWorkspace ) {
    if( !ShutdownRocRail() )
      return;
  }

  MapOp.clear(m_LocDlgMap);
  while( ListOp.size(m_LocCtrlList) > 0 ) {
    LocControlDialog* dlg = (LocControlDialog*)ListOp.get(m_LocCtrlList, 0);
    TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "inform loc control dialog to save position..." );
    dlg->Close();
  }

  /*
  wxMenuItem* mi = menuBar->FindItem(ME_LocoBook);
  m_bLocoBook = mi->IsChecked();
  if(!m_bLocoBook) {
    int pos = wSplitPanel.getplan( wGui.getsplitpanel( m_Ini) );
    m_PlanSplitter->SplitVertically( m_StatNotebook, m_PlanNotebook, pos );
    m_bLocoBook = true;
  }
  */

  m_LC->stopTimer();
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

void RocGuiFrame::OnGCA(wxCommandEvent& WXUNUSED(event)) {
  wxLaunchDefaultBrowser(wxT("http://www.phgiling.net"), wxBROWSER_NEW_WINDOW );
}

void RocGuiFrame::OnTranslations(wxCommandEvent& WXUNUSED(event)) {
  wxLaunchDefaultBrowser(wxGetApp().getMsg("translationslink"), wxBROWSER_NEW_WINDOW );
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
  //dlg->ShowModal();
  dlg->Show(TRUE);
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
  SetStatusText( wxString::Format( _T("%d"), wState.getload( node )) + _T("mA"), status_load );

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
      m_FakeLeftClick = true;
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

    // Dispatching Throttle in the mode column if dispatchmode is on
    if( event.GetCol() == LOC_COL_MODE && wGui.isdispatchmode( m_Ini )) {
      OnLocDispatch( event);
    }
    // TODO: Block if the event is not initiated from the mouse.
    //else if( event.GetCol() == LOC_COL_ID && event.GetEventType() == wxEVT_LEFT_DOWN  ) {
    // 10217 seems to be the mouse event
    else if( !m_FakeLeftClick && event.GetCol() == LOC_COL_ID && lc != NULL ) {
      TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "D&D eventtype [%d]", event.GetEventType() );

      wxString my_text = _T("moveto:")+wxString(wLoc.getid(lc),wxConvUTF8)+_T("::");
      TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "D&D text [%s]", (const char*)my_text.mb_str(wxConvUTF8) );
      wxTextDataObject my_data(my_text);
      wxDropSource dragSource( this );
      dragSource.SetData( my_data );
      wxDragResult result = dragSource.DoDragDrop(wxDrag_CopyOnly);
    }

    m_LC->setLocProps( lc );
    m_CV->setLocProps( lc );

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

  m_FakeLeftClick = false;

  event.Skip(true);
}

BitmapButton::BitmapButton(wxWindow* parent, wxWindowID id, const wxBitmap& bitmap, const wxPoint& pos, const wxSize& size, long style, const wxValidator& validator, const wxString& name) :
  wxBitmapButton(parent, id, bitmap, pos, size, style, validator, name)
{
  m_Parent = parent;
  this->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( BitmapButton::OnLeftDown ), NULL, this );
  this->Connect( wxEVT_LEFT_UP, wxMouseEventHandler( BitmapButton::OnLeftUp ), NULL, this );
}


void BitmapButton::OnLeftUp(wxMouseEvent& event) {
  if( m_LC != NULL && m_LC->getLocProps() != NULL ) {
    iONode lc = m_LC->getLocProps();
    LocDialog*  dlg = new LocDialog(this, lc );
    if( wxID_OK == dlg->ShowModal() ) {
      /* Notify Notebook. */
      wxCommandEvent evt( wxEVT_COMMAND_MENU_SELECTED, ME_INITACTIVELOCS );
      wxPostEvent( m_Parent, evt );
    }
    dlg->Destroy();
  }
}
void BitmapButton::OnLeftDown(wxMouseEvent& event) {
  if( m_LC != NULL && m_LC->getLocProps() != NULL ) {
    wxTextDataObject my_data(_T("moveto:") + wxString(wLoc.getid( m_LC->getLocProps() ),wxConvUTF8)+_T("::"));
    wxDropSource dragSource( this );
    dragSource.SetData( my_data );
    wxDragResult result = dragSource.DoDragDrop(wxDrag_CopyOnly);
  }
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
    m_FakeLeftClick = true;
    OnCellLeftClick(event);
    wxString str = m_ActiveLocs->GetCellValue( event.GetRow(), 0 );

    iONode lc = m_LC->getLocProps();
    Boolean active = wLoc.isactive(lc);

    TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "OnCellRightClick %s", m_LocID );
    wxMenu menu( str );
    menu.Append( ME_GridLocGo, wxGetApp().getMenu("start"), wxGetApp().getTip("start"));
    wxMenuItem *mi = menu.FindItem( ME_GridLocGo );
    mi->Enable( isAutoMode() );
    menu.Append( ME_GridLocStop, wxGetApp().getMenu("stop"), wxGetApp().getTip("stop"));
    menu.AppendSeparator();
    menu.Append( ME_GridLocReset, wxGetApp().getMenu("softresetall"), wxGetApp().getTip("softresetall") );
    menu.Append( ME_GridLocResetAll, wxGetApp().getMenu("resetall"), wxGetApp().getTip("resetall") );
    menu.Append( ME_GridLocSwap, wxGetApp().getMenu("swapplacing"), wxGetApp().getTip("swapplacing") );
    menu.Append( ME_GridLocSwapBlockSide, wxGetApp().getMenu("swapblockenterside"), wxGetApp().getTip("swapblockenterside") );
    menu.AppendSeparator();
    menu.Append( ME_GridLocDispatch, wxGetApp().getMenu("dispatch"), wxGetApp().getTip("dispatch") );
    menu.AppendSeparator();
    mi = menu.Append( ME_GridLocProps, wxGetApp().getMsg("properties") );
    mi->Enable( !isAutoMode() || !wxGetApp().isRestrictedEdit() );

    menu.Append( ME_GridLocGoTo, wxGetApp().getMenu("gotoblock"), wxGetApp().getTip("gotoblock") );
    menu.Append( ME_GridLocSchedule, wxGetApp().getMenu("selectschedule"), wxGetApp().getTip("selectschedule") );
    menu.Append( ME_GridLocTour, wxGetApp().getMenu("selecttour"), wxGetApp().getTip("selecttour") );
    //menu.Append( ME_GridLocShortID, wxGetApp().getMenu("setshortid"), wxGetApp().getTip("selectschedule") );
    menu.AppendSeparator();
    if( !active )
      mi = menu.Append( ME_GridLocActivate, wxGetApp().getMsg("activate") );
    else
      mi = menu.Append( ME_GridLocDeActivate, wxGetApp().getMsg("deactivate") );
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
    m_FakeLeftClick = true;
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

void RocGuiFrame::OnLocActivate(wxCommandEvent& event) {
  /* Inform RocRail... */
  iONode cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
  wLoc.setid( cmd, m_LocID );
  wLoc.setcmd( cmd, wLoc.activate );
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);
}

void RocGuiFrame::OnLocDeActivate(wxCommandEvent& event) {
  /* Inform RocRail... */
  iONode cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
  wLoc.setid( cmd, m_LocID );
  wLoc.setcmd( cmd, wLoc.deactivate );
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
  wLoc.setcmd( cmd, wLoc.softreset );
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);
}

void RocGuiFrame::OnLocResetAll(wxCommandEvent& event) {
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

void RocGuiFrame::OnLocSwapBlockSide(wxCommandEvent& event) {


  TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "event type = %d, %d", event.GetEventType(), event.GetId() );

  int i = m_iLcRowSelection;

  m_ActiveLocs->SetCellValue( i, LOC_COL_MODE, _T("") );
  m_ActiveLocs->SetCellValue( i, LOC_COL_BLOCK, _T("") );
  m_ActiveLocs->SetCellValue( i, LOC_COL_DESTBLOCK, _T("") );


  /* Inform RocRail... */
  iONode cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
  wLoc.setid( cmd, m_LocID );
  wLoc.setcmd( cmd, wLoc.blockside );
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
  GotoDlg* gotoDlg = new GotoDlg( this );
  if( wxID_OK == gotoDlg->ShowModal() ) {
    iONode selection = gotoDlg->getSelected();
    if( selection != NULL ) {
      /* Inform RocRail... */
      iONode lc = findLoc(m_LocID);
      iONode cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
      wLoc.setid( cmd, wLoc.getid( lc ) );
      wLoc.setcmd( cmd, wLoc.gotoblock );
      wLoc.setblockid( cmd, wItem.getid(selection) );
      wxGetApp().sendToRocrail( cmd );
      cmd->base.del(cmd);
    }
  }
  gotoDlg->Destroy();
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

void RocGuiFrame::OnLocTour(wxCommandEvent& event) {
  iONode lc = findLoc(m_LocID);
  const char* blockid = NULL;
  if( lc != NULL )
    blockid = wLoc.getblockid(lc);
  ToursDlg* dlg = new ToursDlg( this, (iONode)NULL, false, blockid );
  if( wxID_OK == dlg->ShowModal() ) {
    iONode sel = dlg->getProperties();
    if( sel != NULL ) {
      const char* id = wTour.getid( sel );

      if( id != NULL ) {
        /* Inform RocRail... */
        iONode cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
        wLoc.setid( cmd, wLoc.getid( lc ) );
        wLoc.setcmd( cmd, wLoc.usetour );
        wLoc.settourid( cmd, id );
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
void RocGuiFrame::OnLangEnglishAU(wxCommandEvent& event) {
  wGui.setlang( wxGetApp().getIni(), wGui.lang_englishAU );
  wxMessageDialog( this, wxGetApp().getMsg("change_language_msg"), _T("Rocrail"), wxOK | wxICON_INFORMATION ).ShowModal();
}
void RocGuiFrame::OnLangEnglishCA(wxCommandEvent& event) {
  wGui.setlang( wxGetApp().getIni(), wGui.lang_englishCA );
  wxMessageDialog( this, wxGetApp().getMsg("change_language_msg"), _T("Rocrail"), wxOK | wxICON_INFORMATION ).ShowModal();
}
void RocGuiFrame::OnLangEnglishGB(wxCommandEvent& event) {
  wGui.setlang( wxGetApp().getIni(), wGui.lang_englishGB );
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

void RocGuiFrame::OnLangBulgarian(wxCommandEvent& event) {
  wGui.setlang( wxGetApp().getIni(), wGui.lang_bulgarian );
  wxMessageDialog( this, wxGetApp().getMsg("change_language_msg"), _T("Rocrail"), wxOK | wxICON_INFORMATION ).ShowModal();
}

void RocGuiFrame::OnLangCatalan(wxCommandEvent& event) {
  wGui.setlang( wxGetApp().getIni(), wGui.lang_catalan );
  wxMessageDialog( this, wxGetApp().getMsg("change_language_msg"), _T("Rocrail"), wxOK | wxICON_INFORMATION ).ShowModal();
}

void RocGuiFrame::OnLangGreek(wxCommandEvent& event) {
  wGui.setlang( wxGetApp().getIni(), wGui.lang_greek );
  wxMessageDialog( this, wxGetApp().getMsg("change_language_msg"), _T("Rocrail"), wxOK | wxICON_INFORMATION ).ShowModal();
}

void RocGuiFrame::OnLangRussian(wxCommandEvent& event) {
  wGui.setlang( wxGetApp().getIni(), wGui.lang_russian );
  wxMessageDialog( this, wxGetApp().getMsg("change_language_msg"), _T("Rocrail"), wxOK | wxICON_INFORMATION ).ShowModal();
}

void RocGuiFrame::OnLangRomanian(wxCommandEvent& event) {
  wGui.setlang( wxGetApp().getIni(), wGui.lang_romanian );
  wxMessageDialog( this, wxGetApp().getMsg("change_language_msg"), _T("Rocrail"), wxOK | wxICON_INFORMATION ).ShowModal();
}

void RocGuiFrame::OnLangPortuguese(wxCommandEvent& event) {
  wGui.setlang( wxGetApp().getIni(), wGui.lang_portuguese );
  wxMessageDialog( this, wxGetApp().getMsg("change_language_msg"), _T("Rocrail"), wxOK | wxICON_INFORMATION ).ShowModal();
}

void RocGuiFrame::OnLangPortugueseBR(wxCommandEvent& event) {
  wGui.setlang( wxGetApp().getIni(), wGui.lang_portuguesebr );
  wxMessageDialog( this, wxGetApp().getMsg("change_language_msg"), _T("Rocrail"), wxOK | wxICON_INFORMATION ).ShowModal();
}

void RocGuiFrame::OnLangHungarian(wxCommandEvent& event) {
  wGui.setlang( wxGetApp().getIni(), wGui.lang_hungarian );
  wxMessageDialog( this, wxGetApp().getMsg("change_language_msg"), _T("Rocrail"), wxOK | wxICON_INFORMATION ).ShowModal();
}

void RocGuiFrame::OnLangPolish(wxCommandEvent& event) {
  wGui.setlang( wxGetApp().getIni(), wGui.lang_polish );
  wxMessageDialog( this, wxGetApp().getMsg("change_language_msg"), _T("Rocrail"), wxOK | wxICON_INFORMATION ).ShowModal();
}

void RocGuiFrame::OnLangNorwegian(wxCommandEvent& event) {
  wGui.setlang( wxGetApp().getIni(), wGui.lang_norwegian );
  wxMessageDialog( this, wxGetApp().getMsg("change_language_msg"), _T("Rocrail"), wxOK | wxICON_INFORMATION ).ShowModal();
}

void RocGuiFrame::OnLangChinese(wxCommandEvent& event) {
  wGui.setlang( wxGetApp().getIni(), wGui.lang_chinese );
  wxMessageDialog( this, wxGetApp().getMsg("change_language_msg"), _T("Rocrail"), wxOK | wxICON_INFORMATION ).ShowModal();
}

void RocGuiFrame::OnLangChineseCH(wxCommandEvent& event) {
  wGui.setlang( wxGetApp().getIni(), wGui.lang_chinesesimplified );
  wxMessageDialog( this, wxGetApp().getMsg("change_language_msg"), _T("Rocrail"), wxOK | wxICON_INFORMATION ).ShowModal();
}

void RocGuiFrame::OnLangIndonesian(wxCommandEvent& event) {
  wGui.setlang( wxGetApp().getIni(), wGui.lang_indonesian );
  wxMessageDialog( this, wxGetApp().getMsg("change_language_msg"), _T("Rocrail"), wxOK | wxICON_INFORMATION ).ShowModal();
}

void RocGuiFrame::OnLangJapanese(wxCommandEvent& event) {
  wGui.setlang( wxGetApp().getIni(), wGui.lang_japanese );
  wxMessageDialog( this, wxGetApp().getMsg("change_language_msg"), _T("Rocrail"), wxOK | wxICON_INFORMATION ).ShowModal();
}

void RocGuiFrame::OnLangKorean(wxCommandEvent& event) {
  wGui.setlang( wxGetApp().getIni(), wGui.lang_korean );
  wxMessageDialog( this, wxGetApp().getMsg("change_language_msg"), _T("Rocrail"), wxOK | wxICON_INFORMATION ).ShowModal();
}

void RocGuiFrame::OnRocrailIni(wxCommandEvent& event) {
  // Get the copied node:
  m_RocrailIni = (iONode)event.GetClientData();
  if( m_RocrailIni != NULL && StrOp.equals( wRocRail.name(), NodeOp.getName( m_RocrailIni ) ) ) {
    // RocrailPropertiesDialog:
    m_RocrailIniDlg = new RocrailIniDialog( this, m_RocrailIni );
    if( wxID_OK == m_RocrailIniDlg->ShowModal() ) {
    }
    m_RocrailIniDlg->Destroy();
    m_RocrailIniDlg = NULL;
  }
  else {
    wxGetApp().m_InitialRocrailIni = false;
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


