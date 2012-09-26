/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2012 Rob Versluis, Rocrail.net

 Without an official permission commercial use is not permitted.
 Forking this project is not permitted.

 This program is free software; you can redistribute it and/or
 as published by the Free Software Foundation; either version 2
 modify it under the terms of the GNU General Public License
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
#ifndef __ROCRAIL_rocgui_GUIFRAME_H
#define __ROCRAIL_rocgui_GUIFRAME_H


#include "rocs/public/rocs.h"
#include "rocs/public/cmdln.h"
#include "rocs/public/doc.h"
#include "rocs/public/node.h"
#include "rocs/public/trace.h"
#include "rocs/public/res.h"
#include "rocs/public/wrpinf.h"
#include "rocs/public/map.h"
#include "rocs/public/mutex.h"

#include "rocrail/public/rcon.h"

#include "rocview/public/basepanel.h"
#include "rocview/public/planpanel.h"
#include "rocview/public/modpanel.h"
#include "rocview/public/cv.h"
#include "rocview/public/lncv.h"
#include "rocview/public/lc.h"
#include "rocview/public/jssupport.h"
#include "rocview/public/statbar.h"

#include "rocview/dialogs/decoders/locoio.h"
#include "rocview/dialogs/decoders/cbusnodedlg.h"
#include "rocview/dialogs/decoders/bidibidentdlg.h"
#include "rocview/dialogs/decoders/mgvdlg.h"
#include "rocview/dialogs/decoders/mgv141dlg.h"
#include "rocview/dialogs/decoders/dtopswdlg.h"
#include "rocview/dialogs/rocrailinidialog.h"
#include "rocview/dialogs/decoders/uhl68610dlg.h"
#include "rocview/dialogs/decoders/uhl633x0dlg.h"
#include "rocview/dialogs/powerctrldlg.h"

#include <wx/splitter.h>
#include <wx/textctrl.h>
#include <wx/grid.h>
#include <wx/notebook.h>

class BitmapButton : public wxBitmapButton
{
public:
  BitmapButton(wxWindow* parent, wxWindowID id, const wxBitmap& bitmap, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
      long style = wxBU_AUTODRAW, const wxValidator& validator = wxDefaultValidator, const wxString& name = _T("button"));
  void OnLeftDown(wxMouseEvent& event);
  void OnLeftUp(wxMouseEvent& event);
  void setLC(LC* lc) {m_LC = lc;};
private:
  LC*  m_LC;
  wxWindow* m_Parent;

};

class RocGuiFrame : public wxFrame
{
public:
  iONode m_Ini;
  const char* m_ControlCode;

  RocGuiFrame(const wxString& title, const wxPoint& pos, const wxSize& size, iONode ini, const char* theme, const char* sp, const char* tp);
  void create();

  void setOnline( bool online );
  void SyncClock( iONode node );

  void OnOpenWorkspace(wxCommandEvent& event);
  void OnSetStatusText(wxCommandEvent& event);
  void OnQuit(wxCommandEvent& event);
  void OnSize(wxSizeEvent& event);
  void OnMenu(wxMenuEvent& event);
  void OnMouse(wxMouseEvent& event);
  void OnClose(wxCloseEvent& event);
  void OnSave(wxCommandEvent& event);
  void OnGoOffline(wxCommandEvent& event);
  void OnAnalyze(wxCommandEvent& event);
  void OnSaveAs(wxCommandEvent& event);
  void OnOpen(wxCommandEvent& event);
  void OnNew(wxCommandEvent& event);
  void OnUpload(wxCommandEvent& event);
  void OnAbout(wxCommandEvent& event);
  void OnUpdate(wxCommandEvent& event);
  void OnHelp(wxCommandEvent& event);
  void OnRUG(wxCommandEvent& event);
  void OnGCA(wxCommandEvent& event);
  void OnTranslations(wxCommandEvent& event);
  void OnBug(wxCommandEvent& event);
  void OnFeature(wxCommandEvent& event);
  void OnService(wxCommandEvent& event);
  void OnLcDlg(wxCommandEvent& event);
  void OnOperatorDlg(wxCommandEvent& event);
  void OnMIC(wxCommandEvent& event);
  void OnSwDlg(wxCommandEvent& event);
  void OnRouteDlg(wxCommandEvent& event);
  void OnConnect(wxCommandEvent& event);
  void OnPlanTitle(wxCommandEvent& event);
  void OnGo(wxCommandEvent& event);
  void OnStop(wxCommandEvent& event);
  void OnEmergencyBreak(wxCommandEvent& event);
  void OnAutoMode(wxCommandEvent& event);
  void OnAutoGo(wxCommandEvent& event);
  void OnAutoResume(wxCommandEvent& event);
  void OnAutoStop(wxCommandEvent& event);
  void OnAutoReset(wxCommandEvent& event);
  void OnAutoSoftReset(wxCommandEvent& event);
  void OnClearMsg(wxCommandEvent& event);
  void showLogo( bool show );
  void OnLogo( wxCommandEvent& event );
  void OnAddException( wxCommandEvent& event );
  void OnLocEvent( wxCommandEvent& event );
  void OnAutoEvent( wxCommandEvent& event );
  void OnStateEvent( wxCommandEvent& event );
  void OnSystemEvent( wxCommandEvent& event );
  void OnZLevelEventAdd( wxCommandEvent& event );
  void OnZLevelEventRemove( wxCommandEvent& event );
  void setPlanTitle( const char* title );
  void OnShutdownRocRail(wxCommandEvent& event);
  bool ShutdownRocRail();
  void setInfoText( const char* text );
  void setDigintText( const char* text );
  void OnInitField(wxCommandEvent& event);
  void OnQuerySensors(wxCommandEvent& event);
  void OnEditMode(wxCommandEvent& event);
  void OnEditModPlan(wxCommandEvent& event);
  void OnCtrlMode(wxCommandEvent& event);
  void OnNewUpdates(wxCommandEvent& event);

  void OnEditLocs(wxCommandEvent& event);
  void OnEditCars(wxCommandEvent& event);
  void OnEditOperators(wxCommandEvent& event);
  void OnEditWaybills(wxCommandEvent& event);
  void OnEditTurnouts(wxCommandEvent& event);
  void OnEditSensors(wxCommandEvent& event);
  void OnEditRoutes(wxCommandEvent& event);
  void OnEditBlocks(wxCommandEvent& event);
  void OnEditBlockGroups(wxCommandEvent& event);
  void OnEditSignals(wxCommandEvent& event);
  void OnEditOutputs(wxCommandEvent& event);
  void OnEditSchedules(wxCommandEvent& event);
  void OnEditTours(wxCommandEvent& event);
  void OnEditLocations(wxCommandEvent& event);
  void OnEditTurntables(wxCommandEvent& event);
  void OnEditMVTrack(wxCommandEvent& event);
  void OnEditTimedActions(wxCommandEvent& event);
  void OnEditBoosters(wxCommandEvent& event);
  void OnEditActions(wxCommandEvent& event);

  void OnCtrlBoosters(wxCommandEvent& event);
  void OnDonKey(wxCommandEvent& event);
  void OnDonKeyInfo(wxCommandEvent& event);

  void setLocalPlan( wxString plan );
  void CleanNotebook();
  void OnInitNotebook(wxCommandEvent& event);
  void OnPageChange(wxNotebookEvent& event);
  BasePanel* InitNotebookPage( iONode zlevel );
  BasePanel* initModPanel( iONode zlevel );
  void RemoveNotebookPage( iONode zlevel );
  iONode findLoc(const char* locid);
  iONode findCar(const char* carid);
  iONode findWaybill(const char* carid);
  iONode findBlock4Loc(const char* locid, const char* blockid);
  bool isInStage(const char* locid, const char* blockid);
  iONode getRocrailIni(){return m_RocrailIni;}
  void setRocrailIni(iONode ini){m_RocrailIni = ini;}
  void OnCellLeftDClick( wxGridEvent& event );
  void OnCellLeftClick( wxGridEvent& event );
  void OnCellRightClick( wxGridEvent& event );
  void OnSelectCell( wxGridEvent& event );
  void OnLocGo(wxCommandEvent& event);
  void OnLocStop(wxCommandEvent& event);
  void OnLocReset(wxCommandEvent& event);
  void OnLocResetAll(wxCommandEvent& event);
  void OnLocSwap(wxCommandEvent& event);
  void OnLocSwapBlockSide(wxCommandEvent& event);
  void OnLocDispatch(wxCommandEvent& event);
  void OnLocProps(wxCommandEvent& event);
  void OnLocGoTo(wxCommandEvent& event);
  void OnLocSchedule(wxCommandEvent& event);
  void OnLocTour(wxCommandEvent& event);
  void OnLocShortID(wxCommandEvent& event);
  void OnTxShortIDs(wxCommandEvent& event);
  void OnLocActivate(wxCommandEvent& event);
  void OnLocDeActivate(wxCommandEvent& event);

  void OnPanelProps(wxCommandEvent& event);
  void OnAddPanel(wxCommandEvent& event);
  void OnUndo(wxCommandEvent& event);

  void OnLangEnglish(wxCommandEvent& event);
  void OnLangGerman(wxCommandEvent& event);
  void OnLangDutch(wxCommandEvent& event);
  void OnLangSwedisch(wxCommandEvent& event);
  void OnLangFrench(wxCommandEvent& event);
  void OnLangTurkish(wxCommandEvent& event);
  void OnLangSpanish(wxCommandEvent& event);
  void OnLangItalien(wxCommandEvent& event);
  void OnLangDanish(wxCommandEvent& event);
  void OnLangCzech(wxCommandEvent& event);
  void OnLangBosnian(wxCommandEvent& event);
  void OnLangBulgarian(wxCommandEvent& event);
  void OnLangCatalan(wxCommandEvent& event);
  void OnLangGreek(wxCommandEvent& event);
  void OnLangRussian(wxCommandEvent& event);
  void OnLangPortuguese(wxCommandEvent& event);
  void OnLangPortugueseBR(wxCommandEvent& event);
  void OnLangHungarian(wxCommandEvent& event);
  void OnLangPolish(wxCommandEvent& event);
  void OnLangNorwegian(wxCommandEvent& event);
  void OnLangChinese(wxCommandEvent& event);
  void OnLangChineseCH(wxCommandEvent& event);
  void OnLangEnglishAU(wxCommandEvent& event);
  void OnLangEnglishGB(wxCommandEvent& event);
  void OnLangEnglishCA(wxCommandEvent& event);
  void OnLangIndonesian(wxCommandEvent& event);
  void OnLangJapanese(wxCommandEvent& event);
  void OnLangRomanian(wxCommandEvent& event);
  void OnLangKorean(wxCommandEvent& event);

  void OnRocrailIni(wxCommandEvent& event);
  void OnRocguiIni(wxCommandEvent& event);

  void OnPowerEvent(wxCommandEvent& event);

  void Zoom(int zoom );
  void OnZoom25(wxCommandEvent& event);
  void OnZoom50(wxCommandEvent& event);
  void OnZoom75(wxCommandEvent& event);
  void OnZoom100(wxCommandEvent& event);

  void OnLocoBook(wxCommandEvent& event);
  void OnPlanBook(wxCommandEvent& event);
  void OnTraceWindow(wxCommandEvent& event);
  void OnLocoSortByAddr(wxCommandEvent& event);
  void OnLocoViewAll(wxCommandEvent& event);
  void OnLocoViewSteam(wxCommandEvent& event);
  void OnLocoViewDiesel(wxCommandEvent& event);
  void OnLocoViewElectric(wxCommandEvent& event);
  void OnLocoViewCommuter(wxCommandEvent& event);
  void OnLocoViewSpecial(wxCommandEvent& event);
  void OnShowID(wxCommandEvent& event);
  void OnRaster(wxCommandEvent& event);
  void OnTooltip(wxCommandEvent& event);
  void OnBackColor(wxCommandEvent& event);
  void OnFullScreen(wxCommandEvent& event);

  void OnMGV(wxCommandEvent& event);
  void OnMGV141(wxCommandEvent& event);
  void OnUhl63350(wxCommandEvent& event);
  void OnUhl68610(wxCommandEvent& event);
  void OnLocoIO(wxCommandEvent& event);
  void OnOpenDecoder(wxCommandEvent& event);
  void OnDTOpSw(wxCommandEvent& event);
  void OnCBusNode(wxCommandEvent& event);

  void OnButton(wxCommandEvent& event);
  void OnSlider(wxScrollEvent& event);

  void UpdateActiveLocs(wxCommandEvent& event);
  void CVevent(wxCommandEvent& event);
  void UpdateLocImage(wxCommandEvent& event);

  bool isEditMode() { return m_bEditMode; }
  void setEditMode( bool editmode ) { m_bEditMode = editmode; }
  bool isEditModPlan() { return m_bEditModPlan; }
  void setEditModPlan( bool editmodplan ) { m_bEditModPlan = editmodplan; }
  bool isAutoMode() { return m_bAutoMode; }
  bool isLocoBook() { return m_bLocoBook; }
  bool isPlanBook() { return m_bPlanBook; }
  bool isTraceWindow() { return m_bTraceWindow; }
  bool isShowID() { return m_bShowID; }
  bool isRaster() { return m_bRaster; }
  bool isTooltip() { return m_bTooltip; }
  bool isInitialized() { return m_bInitialized; }
  void setLocID( const char* locid );
  double getScale() { return m_Scale; }
  double getBktext() { return m_Bktext; }
  iOMap getSymMap() { return m_SymbolMap; }
  void OnTimer(wxTimerEvent& event);
  void OnScaleCombo(wxCommandEvent& event);
  void OnScaleComboCheck(wxCommandEvent& event);
  void resetActiveWorkspace() { m_bActiveWorkspace = false; }

  wxNotebook* getNotebook() { return m_PlanNotebook; }
  BasePanel* getPlanPanel() { return m_PlanPanel; }
  ModPanel* getModPanel() { return m_ModPanel; }
  wxGrid* getActiveLocsGrid() { return m_ActiveLocs; }
  void setPlanPanel(BasePanel* p) { m_PlanPanel = p; }
  wxTextCtrl* getExceptionPanel() { return m_WarningPanel; }
  wxTextCtrl* getWarningPanel() { return m_WarningPanel; }
  wxTextCtrl* getMonitorPanel() { return m_MonitorPanel; }
  wxSplitterWindow* getSplitter() { return m_Splitter; }
  wxSplitterWindow* getPlanSplitter() { return m_PlanSplitter; }
  wxSplitterWindow* getTraceSplitter() { return m_TraceSplitter; }
  iOMap getLocDlgMap() { return m_LocDlgMap; }

  void InitActiveLocs(wxCommandEvent& event);
  void initLocCtrlDialogs();
  void setOffline( bool p_bOffline );
  void initFrame();
  void Connect( const char* host, int port, bool wait4rr=false );
  void putChild( void* item, int z  );


private:
  void Save();
  void initJS();
  void modifyLoc( iONode props, bool deep );

  wxToolBar *       m_ToolBar;
  BitmapButton*     m_LocImage;
  wxNotebook*       m_StatNotebook;
  wxNotebook*       m_PlanNotebook;
  wxPanel*          m_ActiveLocsPanel;
  wxPanel*          m_LCPanel;
  LC*               m_LC;
  wxScrolledWindow* m_CVPanel;
  CV*               m_CV;
  wxScrolledWindow* m_LNCVPanel;
  LNCV*             m_LNCV;
  wxSplitterWindow* m_Splitter;
  wxSplitterWindow* m_PlanSplitter;
  wxSplitterWindow* m_TraceSplitter;
  //wxTextCtrl*       m_ExceptionPanel;
  wxTextCtrl*       m_WarningPanel;
  wxTextCtrl*       m_MonitorPanel;
  BasePanel*        m_PlanPanel;
  ModPanel*         m_ModPanel;
  wxGrid*           m_ActiveLocs;
  wxStaticBitmap*   wxLogo;
  wxString          m_LocalPlan;
  StatusBar *       m_StatusBar;
  wxComboBox*       m_ScaleComboBox;

  LocoIO* m_LocoIO;
  CBusNodeDlg* m_CBusNodeDlg;
  BidibIdentDlg* m_BidibIdentDlg;
  DTOpSwDlg* m_DTOpSw;
  RocrailIniDialog* m_RocrailIniDlg;
  Uhl68610Dlg* m_Uhl68610;
  Uhl633x0Dlg* m_Uhl633x0;
  MGVDlg* m_MGV;
  MGV141Dlg* m_MGV141;
  PowerCtrlDlg* m_PowerCtrl;

  iOMutex m_muxInitActiveLocs;

  const char* m_ThemePath;
  const char* m_ServerPath;
  const char* m_ThemesPath;

  bool  m_bLocoBook;
  bool  m_bPlanBook;
  bool  m_bTraceWindow;
  bool  m_LocoSortByAddress;
  int   m_LocoCategory; // 0=all 1=steam 2=diesel 3=electric 4=commuter 5=special
  bool  m_bShowID;
  bool  m_bRaster;
  bool  m_bTooltip;
  bool  m_bServerConsoleMode;
  bool  m_bInitialized;
  bool  m_bActiveWorkspace;

  bool  showlogo;
  bool  m_bEditMode;
  bool  m_bEditModPlan;
  bool  m_bAutoMode;
  wxMenuBar *menuBar;
  char* m_LocID;
  int m_iLcRowSelection;
  double m_Scale;
  double m_Bktext;
  iOMap m_SymbolMap;
  iOList m_LocCtrlList;
  iOMap m_LocDlgMap;
  JsSupport* m_JsSupport;
  bool m_bCheckedDonKey;
  iONode m_WorkSpace;
  iONode m_RocrailIni;
  bool m_FakeLeftClick;
  // any class wishing to process wxWindows events must use this macro
  DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items
    ME_Quit = 10000,
    ME_Analyze,
    ME_AnalyzeClean,
    ME_Save,
    ME_SaveAs,
    ME_Upload,
    ME_Open,
    ME_New,
    ME_About,
    ME_Update,
    ME_Help,
    ME_LcDlg,
    ME_OperatorDlg,
    ME_MIC,
    ME_SwDlg,
    ME_RouteDlg,
    ME_Connect,
    ME_Go,
    ME_Stop,
    ME_EmergencyBreak,
    ME_AutoMode,
    ME_AutoGo,
    ME_AutoResume,
    ME_AutoStop,
    ME_AutoReset,
    ME_AutoSoftReset,
    ME_ClearMsg,
    SHOWLOGO_EVENT,
    ME_ShutdownRocRail,
    ME_InitField,
    ME_QuerySensors,
    ME_TxShortIDs,
    ADDEXCEPTION_EVENT,
    LOC_EVENT,
    AUTO_EVENT,
    SYSTEM_EVENT,
    STATE_EVENT,
    ZLEVEL_EVENT_ADD,
    ZLEVEL_EVENT_REMOVE,
    ME_EditMode,
    ME_EditModPlan,
    ME_CtrlMode,
    ME_EditLocs,
    ME_EditCars,
    ME_EditOperators,
    ME_EditWaybills,
    ME_EditBlocks,
    ME_EditBlockGroups,
    ME_EditRoutes,
    ME_EditTurnouts,
    ME_EditSensors,
    ME_EditSignals,
    ME_EditOutputs,
    ME_EditSchedules,
    ME_EditTours,
    ME_EditLocations,
    ME_EditTurntables,
    ME_EditMVTrack,
    ME_EditTimedActions,
    INIT_NOTEBOOK,
    ME_GridLocGo,
    ME_GridLocStop,
    ME_GridLocReset,
    ME_GridLocResetAll,
    ME_GridLocSwap,
    ME_GridLocSwapBlockSide,
    ME_GridLocDispatch,
    ME_GridLocProps,
    ME_GridLocGoTo,
    ME_GridLocSchedule,
    ME_GridLocTour,
    ME_GridLocShortID,
    ME_GridLocActivate,
    ME_GridLocDeActivate,
    UPDATE_ACTIVELOCS_EVENT,
    UPDATE_LOC_IMAGE_EVENT,
    CV_EVENT,
    ME_PanelProps,
    ME_AddPanel,
    ME_Undo,
    ME_PlanTitle,
    ME_LangEnglish,
    ME_LangEnglishAU,
    ME_LangEnglishGB,
    ME_LangEnglishCA,
    ME_LangGerman,
    ME_LangDutch,
    ME_LangSwedisch,
    ME_LangFrench,
    ME_LangTurkish,
    ME_LangSpanish,
    ME_LangItalien,
    ME_LangDanish,
    ME_LangBosnian,
    ME_LangCzech,
    ME_LangBulgarian,
    ME_LangCatalan,
    ME_LangGreek,
    ME_LangRussian,
    ME_LangPortuguese,
    ME_LangHungarian,
    ME_LangPolish,
    ME_LangNorwegian,
    ME_LangChinese,
    ME_LangIndonesian,
    ME_LangJapanese,
    ME_LangRomanian,
    ME_LangKorean,
    ME_LangChineseCH,
    ME_LangPortugueseBR,
    ME_RocrailIni,
    ME_PowerEvent,
    ME_RocguiIni,
    ME_Zoom25,
    ME_Zoom50,
    ME_Zoom75,
    ME_Zoom100,
    ME_LocoBook,
    ME_PlanBook,
    ME_TraceWindow,
    ME_LocoViewAll,
    ME_LocoViewSteam,
    ME_LocoViewDiesel,
    ME_LocoViewElectric,
    ME_LocoViewCommuter,
    ME_LocoViewSpecial,
    ME_LocoSortByAddr,
    ME_ShowID,
    ME_FullScreen,
    ME_Raster,
    ME_Tooltip,
    NEWUPDATES_EVENT,
    ME_RUG,
    ME_GCA,
    ME_Translations,
    ME_Bug,
    ME_Feature,
    ME_Survey,
    ME_BackColor,
    ME_UHL_63350,
    ME_UHL_68610,
    ME_LOCOIO,
    ME_MGV,
    ME_MGV141,
    ME_OpenDecoder,
    ME_CBusNode,
    ME_DTOpSw,
    ME_Uhlenbrock,
    ME_EditBoosters,
    ME_CtrlBoosters,
    ME_EditActions,
    ME_DonKey,
    ME_DonKeyInfo,
    ME_F1,
    ME_F2,
    ME_F3,
    ME_F4,
    ME_F5,
    ME_F6,
    ME_F7,
    ME_F8,
    ME_F9,
    ME_F10,
    ME_F11,
    ME_F12,
    ME_F13,
    ME_F14,
    ME_F15,
    ME_F16,
    ME_F17,
    ME_F18,
    ME_F19,
    ME_F20,
    ME_F21,
    ME_F22,
    ME_F23,
    ME_F24,
    ME_F25,
    ME_F26,
    ME_F27,
    ME_F28,
    ID_SCALE_COMBO,
    ME_INITACTIVELOCS,
    ME_SetStatusText,
    ME_GoOffline,
    ME_OpenWorkspace,
    ME_RecentWorkspaces = ME_OpenWorkspace + 10,
};

enum {
  LOC_COL_ID = 0,
  LOC_COL_ADDR = 1,
  LOC_COL_BLOCK = 2,
  LOC_COL_V = 3,
  LOC_COL_MODE = 4,
  LOC_COL_DESTBLOCK = 5,

};

#define LOCO_VIEW_ALL 0
#define LOCO_VIEW_STEAM 1
#define LOCO_VIEW_DIESEL 2
#define LOCO_VIEW_ELECTRIC 3
#define LOCO_VIEW_COMMUTER 4
#define LOCO_VIEW_SPECIAL 5


#endif
