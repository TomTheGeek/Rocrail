/*
 Rocrail - Model Railroad Software

 Copyright (C) Rob Versluis <r.j.versluis@rocrail.net>

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
#ifndef MODPANEL_H_
#define MODPANEL_H_


#include "rocgui/public/basepanel.h"
#include "rocgui/dialogs/modplandlg.h"

#include "rocs/public/list.h"


class ModPanel : public BasePanel
{
  double m_Scale;
  int    m_ItemSize;
  iOList m_ModList;
  int m_mouseX;
  int m_mouseY;
  int m_X;
  int m_Y;
  wxWindow* m_Parent;
  ModPlanDlg* m_ModPlanDlg;

public:
  ModPanel(wxWindow *parent, int itemsize, double scale );
  void OnPaint(wxPaintEvent& event);
  void addModule(BasePanel* modpanel);
  void removeModule(iONode zlevel);
  void OnPopup(wxMouseEvent& event);
  void OnAddModule(wxCommandEvent& event);
  void OnProperties(wxCommandEvent& event);

  void modelEvent( iONode node );
  void reScale( double scale );
  void removeItemFromList( iONode item );
  void init(bool modview=false);
  void clean();
  const char* getZLevelTitle();
  int getZ();
  const char* getZID(){ return NULL; }
  iONode getZLevel();
  void refresh(bool eraseBackground = true);
  bool SetBackgroundColor(int red, int green, int blue);
  void OnPanelProps(wxCommandEvent& event);
  void OnBackColor(wxCommandEvent& event);
  void OnModuleProps(wxCommandEvent& event);
  bool hasZLevel( iONode zlevel );
  BasePanel* updateZLevel(iONode zlevel);
  iOList getModList() { return m_ModList; }

private:
  DECLARE_EVENT_TABLE()
};

enum {
  ME_AddModule = 801,
  ME_ModPlanProperties,
  ME_ModuleProperties,
  ME_ModPlanBackColor,
};


#endif /*MODPANEL_H_*/
