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
#ifndef __ROCRAIL_rocgui_PLANPANEL_H
#define __ROCRAIL_rocgui_PLANPANEL_H

#include "rocs/public/thread.h"
#include "rocs/public/node.h"
#include "rocs/public/map.h"
#include "rocview/public/basepanel.h"

class PlanPanel : public BasePanel
{
public:
  PlanPanel(wxWindow *parent, int itemsize, double scale, double bktext, int z, iONode zlevel=NULL, bool showBorder=true );
  void OnPaint(wxPaintEvent& event);
  void init(bool modview=false);
  void clean();
  char* itemKey( iONode node, char* key, char* prevkey );
  char* itemKey( const char* nodename, const char* id, char* key );
  void addItems( iONode node );
  void addItemCmd(wxCommandEvent& event);
  void updateItemCmd(wxCommandEvent& event);
  void update4Route(wxCommandEvent& event);
  void updateTTItemCmd(wxCommandEvent& event);
  void addItem(iONode child, bool add2list=true);
  void addItem(const char* nodename, const char* id, wxWindow* item);
  void addMultipleItem(wxCommandEvent& event);
  void addTrackStraight(wxCommandEvent& event);
  void addTrackCurve(wxCommandEvent& event);
  void addTrackDir(wxCommandEvent& event);
  void addTrackDirAll(wxCommandEvent& event);
  void addTrackBuffer(wxCommandEvent& event);
  void addTrackConnector(wxCommandEvent& event);
  void addSwitchLeft(wxCommandEvent& event);
  void addSwitchRight(wxCommandEvent& event);
  void addSwitchCrossing(wxCommandEvent& event);
  void addSwitchDCrossing(wxCommandEvent& event);
  void addSwitchThreeway(wxCommandEvent& event);
  void addSwitchDecoupler(wxCommandEvent& event);
  void addSwitchAccessory(wxCommandEvent& event);
  void addSignal(wxCommandEvent& event);
  void addOutput(wxCommandEvent& event);
  void addBlock(wxCommandEvent& event);
  void addStage(wxCommandEvent& event);
  void addFBack(wxCommandEvent& event);
  void addRoute(wxCommandEvent& event);
  void addTT(wxCommandEvent& event);
  void addSelTab(wxCommandEvent& event);
  void addText(wxCommandEvent& event);

  void addItemAttr( iONode node );
  void modelEvent( iONode node );
  void reScale( double scale );
  void setShow(bool show);
  void OnSize(wxSizeEvent& event);
  void OnShow(wxCommandEvent& event);
  void OnPopup(wxMouseEvent& event);
  void OnLeftUp(wxMouseEvent& event);
  void OnLeftDown(wxMouseEvent& event);
  void OnMotion(wxMouseEvent& event);
  void OnRotate(wxCommandEvent& event);
  void removeItemFromList( iONode item );
  void OnPanelProps(wxCommandEvent& event);
  void OnModProps(wxCommandEvent& event);
  BasePanel* updateZLevel(iONode zlevel);
  void OnAddPanel(wxCommandEvent& event);
  void OnSelect(wxCommandEvent& event);
  void processSelection(iONode sel);
  void moveSelection(iONode sel);
  void copySelection(iONode sel);
  void deleteSelection(iONode sel);
  void OnRemovePanel(wxCommandEvent& event);
  const char* getZLevelTitle();
  bool isBlockOccupied( const char* id );
  bool isBlockReserved( const char* id );
  void blockEvent( const char* id );
  int getZ() { return m_Z; }
  const char* getZID();
  iONode getZLevel() { return m_zLevel; }
  bool isModView(){ return m_bModView;}
  bool SetBackgroundColor(int red, int green, int blue, bool savecolor);
  void SetScBackgroundColor(int red, int green, int blue, bool showSc);
  void refresh(bool eraseBackground = true);
  void setPosition();
  bool isRouteLocked(const char* id);
  void putChild(void* item);
  iONode m_zLevel;
  int m_Z;
  const char* m_Ori;

private:
  void ChangeItemKey( const char* key, const char* prev_key);
  iOMap m_LockedRoutes;
  int m_dragX;
  int m_dragY;
  bool m_hasMouse;
  bool m_isDragged;
  wxStaticText* m_ModViewLabel;
  bool show;
  void Zchanged();
  iONode addItemInList( iONode item );
  iOThread  m_InitThread;
	wxWindow* m_Parent;
  wxHashTable* m_ChildTable;
  double m_Scale;
  double m_Bktext;
  int m_ItemSize;
  int m_mouseX;
  int m_mouseY;
  int m_X;
  int m_Y;
  bool m_Initialized;
  bool m_MultiAdd;
  bool m_ProcessingSelect;
  bool m_bModView;
  iONode m_lastAddedItem;
  int m_Red;
  int m_Green;
  int m_Blue;
  int m_ScRed;
  int m_ScGreen;
  int m_ScBlue;
  bool m_ShowSc;

  DECLARE_EVENT_TABLE()
};

enum {
  ADDITEM_EVENT = 201,
  UPDATEITEM_EVENT,
  UPDATE4ROUTE_EVENT,
  UPDATETT_EVENT,
  ADDMULTIPLEITEM_EVENT,
  SHOW_EVENT,
  ME_AddTrackStraight,
  ME_AddTrackCurve,
  ME_AddTrackDir,
  ME_AddTrackDirAll,
  ME_AddTrackBuffer,
  ME_AddTrackConnector,
  ME_AddSwitchLeft,
  ME_AddSwitchRight,
  ME_AddSwitchCrossing,
  ME_AddSwitchDCrossing,
  ME_AddSwitchThreeway,
  ME_AddSwitchDecoupler,
  ME_AddSwitchAccessory,
  ME_AddSignal,
  ME_AddOutput,
  ME_AddBlock,
  ME_AddStage,
  ME_AddFBack,
  ME_AddRoute,
  ME_AddTT,
  ME_AddSelTab,
  ME_AddText,
  ME_PlanProps,
  ME_ModProps,
  ME_AddPlan,
  ME_PanelSelect,
  ME_RemovePlan,
  ME_AddRoadStraight,
  ME_AddRoadCurve,
  ME_AddRoadDir,
  ME_AddRoadLeft,
  ME_AddRoadRight,
  ME_AddRoadCrossing,
  ME_AddRoadBlock,
  ME_AddRoadFBack,
  ME_ModuleRotate,
  ME_ModuleNorth,
  ME_ModuleEast,
  ME_ModuleSouth,
  ME_ModuleWest,
};


#endif
