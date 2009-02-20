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

#include "rocgui/public/basepanel.h"
#include "rocgui/public/planpanel.h"
#include "rocgui/public/modpanel.h"
#include "rocgui/public/cv.h"
#include "rocgui/public/lncv.h"
#include "rocgui/public/lc.h"
#include "rocgui/public/jssupport.h"
#include "rocgui/public/statbar.h"

#include "rocgui/dialogs/decoders/locoio.h"
#include "rocgui/dialogs/decoders/dtopswdlg.h"
#include "rocgui/dialogs/decoders/opendecoder.h"
#include "rocgui/dialogs/rocrailinidialog.h"

#include <wx/splitter.h>
#include <wx/textctrl.h>
#include <wx/grid.h>
#include <wx/notebook.h>


class RocGuiFrame : public wxFrame
{
public:
  iONode m_Ini;
  RocGuiFrame(const wxString& title, const wxPoint& pos, const wxSize& size, iONode ini);
  void create();

  void setOnline( bool online );
  void SyncClock( iONode node );

  void OnQuit(wxCommandEvent& event);
  void OnSize(wxSizeEvent& event);
  void OnMenu(wxMenuEvent& event);
  void OnMouse(wxMouseEvent& event);
  void OnClose(wxCloseEvent& event);
  void OnSave(wxCommandEvent& event);
  void OnSaveAs(wxCommandEvent& event);
  void OnOpen(wxCommandEvent& event);
  void OnNew(wxCommandEvent& event);
  void OnUpload(wxCommandEvent& event);
  void OnAbout(wxCommandEvent& event);
  void OnUpdate(wxCommandEvent& event);
  void OnHelp(wxCommandEvent& event);
  void OnRUG(wxCommandEvent& event);
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
  void setInfoText( const char* text );
  void setDigintText( const char* text );
  void OnInitField(wxCommandEvent& event);
  void OnEditMode(wxCommandEvent& event);
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
  void OnEditLocations(wxCommandEvent& event);
  void OnEditTurntables(wxCommandEvent& event);
  void OnEditMVTrack(wxCommandEvent& event);
  void OnEditTimedActions(wxCommandEvent& event);

  void setLocalPlan( wxString plan );
  void OnInitNotebook(wxCommandEvent& event);
  void OnPageChange(wxNotebookEvent& event);
  BasePanel* InitNotebookPage( iONode zlevel );
  BasePanel* initModPanel( iONode zlevel );
  void RemoveNotebookPage( iONode zlevel );
  iONode findLoc(const char* locid);
  iONode findBlock4Loc(const char* locid);
  void OnCellLeftDClick( wxGridEvent& event );
  void OnCellLeftClick( wxGridEvent& event );
  void OnCellRightClick( wxGridEvent& event );
  void OnSelectCell( wxGridEvent& event );
  void OnLocGo(wxCommandEvent& event);
  void OnLocStop(wxCommandEvent& event);
  void OnLocReset(wxCommandEvent& event);
  void OnLocSwap(wxCommandEvent& event);
  void OnLocDispatch(wxCommandEvent& event);
  void OnLocProps(wxCommandEvent& event);
  void OnLocGoTo(wxCommandEvent& event);
  void OnLocSchedule(wxCommandEvent& event);
  void OnLocShortID(wxCommandEvent& event);
  void OnTxShortIDs(wxCommandEvent& event);

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

  void OnRocrailIni(wxCommandEvent& event);
  void OnRocguiIni(wxCommandEvent& event);

  void Zoom(int zoom );
  void OnZoom25(wxCommandEvent& event);
  void OnZoom50(wxCommandEvent& event);
  void OnZoom75(wxCommandEvent& event);
  void OnZoom100(wxCommandEvent& event);

  void OnLocoBook(wxCommandEvent& event);
  void OnFill(wxCommandEvent& event);
  void OnShowID(wxCommandEvent& event);
  void OnRaster(wxCommandEvent& event);
  void OnBackColor(wxCommandEvent& event);

  void OnUhl63350(wxCommandEvent& event);
  void OnLocoIO(wxCommandEvent& event);
  void OnOpenDecoder(wxCommandEvent& event);
  void OnDTOpSw(wxCommandEvent& event);

  void OnButton(wxCommandEvent& event);
  void OnSlider(wxScrollEvent& event);

  void UpdateActiveLocs(wxCommandEvent& event);
  void CVevent(wxCommandEvent& event);
  void UpdateLocImage(wxCommandEvent& event);

  bool isEditMode() { return m_bEditMode; }
  void setEditMode( bool editmode ) { m_bEditMode = editmode; }
  bool isAutoMode() { return m_bAutoMode; }
  bool isLocoBook() { return m_bLocoBook; }
  bool isFill() { return m_bFill; }
  bool isShowID() { return m_bShowID; }
  bool isRaster() { return m_bRaster; }
  void setLocID( const char* locid );
  double getScale() { return m_Scale; }
  double getBktext() { return m_Bktext; }
  iOMap getSymMap() { return m_SymbolMap; }
  void OnTimer(wxTimerEvent& event);
  void OnScaleCombo(wxCommandEvent& event);
  void OnScaleComboCheck(wxCommandEvent& event);

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
  wxString getIconPath(const char* iconfile);


private:
  void Save();
  void initJS();
  void modifyLoc( iONode props );

  wxBitmapButton*   m_LocImage;
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
  OpenDecoderDlg* m_OpenDecoder;
  DTOpSwDlg* m_DTOpSw;
  RocrailIniDialog* m_RocrailIniDlg;

  iOMutex m_muxInitActiveLocs;

  bool  m_bLocoBook;
  bool  m_bFill;
  bool  m_bShowID;
  bool  m_bRaster;
  bool  m_bServerConsoleMode;

  bool  showlogo;
  bool  m_bEditMode;
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
    ME_Quit = 1,
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
    ME_TxShortIDs,
    ADDEXCEPTION_EVENT,
    LOC_EVENT,
    AUTO_EVENT,
    SYSTEM_EVENT,
    STATE_EVENT,
    ZLEVEL_EVENT_ADD,
    ZLEVEL_EVENT_REMOVE,
    ME_EditMode,
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
    ME_EditLocations,
    ME_EditTurntables,
    ME_EditMVTrack,
    ME_EditTimedActions,
    INIT_NOTEBOOK,
    ME_GridLocGo,
    ME_GridLocStop,
    ME_GridLocReset,
    ME_GridLocSwap,
    ME_GridLocDispatch,
    ME_GridLocProps,
    ME_GridLocGoTo,
    ME_GridLocSchedule,
    ME_GridLocShortID,
    UPDATE_ACTIVELOCS_EVENT,
    UPDATE_LOC_IMAGE_EVENT,
    CV_EVENT,
    ME_PanelProps,
    ME_AddPanel,
    ME_Undo,
    ME_PlanTitle,
    ME_LangEnglish,
    ME_LangGerman,
    ME_LangDutch,
    ME_LangSwedisch,
    ME_LangFrench,
    ME_LangTurkish,
    ME_LangSpanish,
    ME_LangItalien,
    ME_LangDanish,
    ME_RocrailIni,
    ME_RocguiIni,
    ME_Zoom25,
    ME_Zoom50,
    ME_Zoom75,
    ME_Zoom100,
    ME_LocoBook,
    ME_Fill,
    ME_ShowID,
    ME_Raster,
    NEWUPDATES_EVENT,
    ME_RUG,
    ME_Bug,
    ME_Feature,
    ME_Survey,
    ME_BackColor,
    ME_UHL_63350,
    ME_LOCOIO,
    ME_OpenDecoder,
    ME_DTOpSw,
    ME_Uhlenbrock,
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
    ID_SCALE_COMBO,
    ME_INITACTIVELOCS,
};

enum {
  LOC_COL_ID = 0,
  LOC_COL_ADDR = 1,
  LOC_COL_BLOCK = 2,
  LOC_COL_V = 3,
  LOC_COL_MODE = 4,
  LOC_COL_DESTBLOCK = 5,

};
#endif
