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
#ifndef __ROCRAIL_rocgui_ITEM_H
#define __ROCRAIL_rocgui_ITEM_H

#include "rocview/public/planpanel.h"
#include "rocview/symbols/renderer.h"

#include <wx/dnd.h>

class BlockDrop : public wxTextDropTarget
{
public:
  BlockDrop( iONode props, Symbol* parent ){m_Props = props; m_Parent = parent;};
  virtual bool OnDropText(wxCoord x, wxCoord y, const wxString& data);
private:
  iONode m_Props;
  Symbol* m_Parent;
};


// A custom modal dialog
class Symbol : public wxWindow
{
public:
  Symbol(PlanPanel *parent, iONode props, int itemsize, int z, double scale, double bktext );
  void OnPaint(wxPaintEvent& event);
  void OnLeftUp(wxMouseEvent& event);
  void OnLeftDClick(wxMouseEvent& event);
  void OnLeftDown(wxMouseEvent& event);
  void OnRightDown(wxMouseEvent& event);
  void OnMotion(wxMouseEvent& event);
  void OnMouseEnter(wxMouseEvent& event);
  void OnMouseLeave(wxMouseEvent& event);
  void OnPopup(wxMouseEvent& event);
  void OnProps(wxCommandEvent& event);
  void OnRotate(wxCommandEvent& event);
  void OnSelect(wxCommandEvent& event);
  void OnType(wxCommandEvent& event);
  void OnDelete(wxCommandEvent& event);
  void OnLoc(wxCommandEvent& event);
  void OnUnLoc(wxCommandEvent& event);
  void OnLocGoTo(wxCommandEvent& event);
  void OnLocSchedule(wxCommandEvent& event);
  void OnLocTour(wxCommandEvent& event);
  void OnLocGo(wxCommandEvent& event);
  void OnLocSwap(wxCommandEvent& event);
  void OnLocSwapBlockSide(wxCommandEvent& event);
  void OnScheduleGo(wxCommandEvent& event);
  void OnFYGo(wxCommandEvent& event);
  void OnTTGo(wxCommandEvent& event);
  void OnLocGoManual(wxCommandEvent& event);
  void OnLocStop(wxCommandEvent& event);
  void OnLocReset(wxCommandEvent& event);
  void OnLocResetAll(wxCommandEvent& event);
  void OnCloseBlock(wxCommandEvent& event);
  void OnCloseExitBlock(wxCommandEvent& event);
  void OnOpenBlock(wxCommandEvent& event);
  void OnOpenExitBlock(wxCommandEvent& event);
  void OnAcceptIdent(wxCommandEvent& event);
  void OnInfo(wxCommandEvent& event);
  void OnResetWheelcounter(wxCommandEvent& event);
  void OnResetSensor(wxCommandEvent& event);
  void OnCompress(wxCommandEvent& event);
  void OnLocMIC(wxCommandEvent& event);
  void OnLocActivate(wxCommandEvent& event);
  void OnLocDeActivate(wxCommandEvent& event);

  void OnCmdStraight(wxCommandEvent& event);
  void OnCmdTurnout(wxCommandEvent& event);
  void OnCmdLeft(wxCommandEvent& event);
  void OnCmdRight(wxCommandEvent& event);

  void OnCmdSignalAuto(wxCommandEvent& event);
  void OnCmdSignalManual(wxCommandEvent& event);
  void OnCmdSignalRed(wxCommandEvent& event);
  void OnCmdSignalGreen(wxCommandEvent& event);
  void OnCmdSignalYellow(wxCommandEvent& event);
  void OnCmdSignalWhite(wxCommandEvent& event);
  void OnCmdSignalAspect(wxCommandEvent& event);

  void OnTimer(wxTimerEvent& event);

  void OnTTLight(wxCommandEvent& event);
  void OnTTNext(wxCommandEvent& event);
  void OnTTPrev(wxCommandEvent& event);
  void OnTT180(wxCommandEvent& event);
  void OnTTTrack(wxCommandEvent& event);
  void OnTTCalibrate(wxCommandEvent& event);

  void showTooltip(bool show);
  void modelEvent( iONode node, bool oncreate=false );
  const char* getId();
  iONode getProperties();
  void reScale( double scale );
  void blockEvent( const char* id );
  void routeEvent( const char* id, bool locked );
  void disable();
  void locoDropped() {m_locoIsDropped = true;};
  void setZ(int z){m_Z = z;}
  void setPanel(PlanPanel* panel);
  void setPosition();
  bool isDragged() {return m_isDragged;}

private:
  SymbolRenderer* m_Renderer;
  PlanPanel* m_PlanPanel;
  BlockDrop* m_BlockDrop;
  int m_dragX;
  int m_dragY;
  bool m_hasMouse;
  bool m_isDragged;
  bool m_locoIsDropped;
  char* m_locidStr;
  char* m_Tip;
  //char* m_RouteID;
  void sizeToScale();
  double getSize();
  void updateLabel();
	wxWindow* m_Parent;
  iONode m_Props;
  double bridgepos;
  wxTimer* m_Timer;
  int m_Z;
  int m_ItemSize;
  double m_Scale;
  double m_Bktext;

  iOList m_sclist;
  iOList m_fylist;
  iOList m_ttlist;

  bool m_RotateSym;

  void checkSpeakAction(iONode node);

  DECLARE_EVENT_TABLE()
};

#endif
