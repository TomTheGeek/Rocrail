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
// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------
#ifdef __GNUG__
    #pragma implementation "rocgui.cpp"
    #pragma interface "rocgui.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include <wx/colordlg.h>


#include "rocs/public/node.h"

#include "rocgui/public/modpanel.h"
#include "rocgui/public/guiapp.h"
#include "rocgui/dialogs/modplandlg.h"

#include "rocrail/wrapper/public/ZLevel.h"
#include "rocrail/wrapper/public/ModelCmd.h"
#include "rocrail/wrapper/public/Module.h"
#include "rocrail/wrapper/public/ModPlan.h"
#include "rocrail/wrapper/public/SysCmd.h"

#include "rocgui/wrapper/public/Gui.h"
#include "rocgui/wrapper/public/ModPanel.h"



BEGIN_EVENT_TABLE(ModPanel, wxScrolledWindow)
  EVT_RIGHT_UP(ModPanel::OnPopup)
  EVT_MENU( ME_AddModule , ModPanel::OnAddModule )
  EVT_MENU( ME_ModPlanProperties, ModPanel::OnPanelProps )
  EVT_MENU( ME_ModuleProperties , ModPanel::OnModuleProps )
  EVT_MENU( ME_ModPlanBackColor , ModPanel::OnBackColor)
  //EVT_PAINT(ModPanel::OnPaint)
END_EVENT_TABLE()

ModPanel::ModPanel(wxWindow *parent, int itemsize, double scale )
             :BasePanel( parent, wxSUNKEN_BORDER | wxWANTS_CHARS )
{
  m_Parent    = parent;
  m_Scale     = scale;
  m_ItemSize  = itemsize;
  m_ModList   = ListOp.inst();

  SetBackgroundColour( *wxWHITE );

  // TODO: resize virtual size after every module add
  SetVirtualSize( (int)(m_ItemSize*m_Scale*128), (int)(m_ItemSize*m_Scale*96) );
  SetScrollRate( (int)(m_ItemSize*m_Scale), (int)(m_ItemSize*m_Scale) );
}


void ModPanel::OnPopup(wxMouseEvent& event) {
  m_mouseX = event.GetX();
  m_mouseY = event.GetY();
  m_X = (int)(m_mouseX / (m_ItemSize*m_Scale));
  m_Y = (int)(m_mouseY / (m_ItemSize*m_Scale));

  wxMenu menu( wxGetApp().getMenu("modpanel") );
  menu.Append( ME_AddModule, wxGetApp().getMenu("addmodule") );
  menu.Append( ME_ModPlanProperties, wxGetApp().getMenu("properties") );
  menu.AppendSeparator();
  menu.Append( ME_ModPlanBackColor, wxGetApp().getMenu("background") + _T(" ") +wxGetApp().getMenu("color") );
  PopupMenu(&menu, event.GetX(), event.GetY() );
}


void ModPanel::OnBackColor( wxCommandEvent& event ) {
  wxColourDialog* dlg = new wxColourDialog(this);
  if( wxID_OK == dlg->ShowModal() ) {
    wxColour &color = dlg->GetColourData().GetColour();
    iONode ini = wGui.getmodpanel( wxGetApp().getIni() );

    wModPanel.setred  ( ini, (int)color.Red() );
    wModPanel.setgreen( ini, (int)color.Green() );
    wModPanel.setblue ( ini, (int)color.Blue() );

    SetBackgroundColour(color);
  }
  dlg->Destroy();
}

void ModPanel::OnProperties(wxCommandEvent& event) {
}


void ModPanel::OnAddModule(wxCommandEvent& event) {
  wxString ms_FileExt = wxGetApp().getMsg("planfiles");
  const char* l_openpath = wGui.getopenpath( wxGetApp().getIni() );
  wxFileDialog* fdlg = new wxFileDialog(m_Parent, wxGetApp().getMenu("openplanfile"), wxString(l_openpath,wxConvUTF8) , _T(""), ms_FileExt, wxOPEN);
  if( fdlg->ShowModal() == wxID_OK ) {
    wGui.setopenpath( wxGetApp().getIni(), fdlg->GetPath().mb_str(wxConvUTF8) );

    /* Notify RocRail. */
    TraceOp.trc( "modpanel", TRCLEVEL_INFO, __LINE__, 9999,
        "Add module [%s]", wGui.getopenpath( wxGetApp().getIni() ) );

    iONode cmd = NodeOp.inst( wModelCmd.name(), NULL, ELEMENT_NODE );
    wModelCmd.setcmd( cmd, wModelCmd.addmodule );
    iONode module = NodeOp.inst( wModule.name(), cmd, ELEMENT_NODE );
    wModule.setfilename( module, wGui.getopenpath( wxGetApp().getIni() ) );
    wModule.setx( module, m_X );
    wModule.sety( module, m_Y );


    wxTextEntryDialog* dlg = new wxTextEntryDialog(m_Parent, wxGetApp().getMenu("enterid") );

    if( wxID_OK == dlg->ShowModal() ) {
      wModule.setid( module, dlg->GetValue().mb_str(wxConvUTF8) );
    }
    dlg->Destroy();

    NodeOp.addChild( cmd, module );
    wxGetApp().sendToRocrail( cmd );
    cmd->base.del(cmd);

    // strip filename:
    wGui.setopenpath( wxGetApp().getIni(), FileOp.getPath(wGui.getopenpath( wxGetApp().getIni() ) ) );
  }
  fdlg->Destroy();
}


void ModPanel::addModule(BasePanel* modpanel) {
  ListOp.add( m_ModList, (obj)modpanel );
}



void ModPanel::removeModule(iONode zlevel) {
  int level = wZLevel.getz( zlevel );
  BasePanel* module = (BasePanel*)ListOp.first(m_ModList);
  while( module != NULL ) {
    if( level == module->getZ() ) {
      module->clean();
      this->RemoveChild(module);
      ListOp.removeObj( m_ModList, (obj)module );
      module->Destroy();
      break;
    }
    module = (BasePanel*)ListOp.next( m_ModList );
  };
}



void ModPanel::modelEvent( iONode node ) {
  BasePanel* p = (BasePanel*)ListOp.first(m_ModList);
  while( p != NULL ) {
    p->modelEvent(node);
    p = (BasePanel*)ListOp.next(m_ModList);
  }
}


void ModPanel::reScale( double scale ) {
  int itemsize = wxGetApp().getItemSize();
  m_Scale     = scale;
  m_ItemSize  = itemsize;

  SetVirtualSize( (int)(m_ItemSize*m_Scale*128), (int)(m_ItemSize*m_Scale*96) );
  SetScrollRate( (int)(m_ItemSize*m_Scale), (int)(m_ItemSize*m_Scale) );

  BasePanel* p = (BasePanel*)ListOp.first(m_ModList);
  while( p != NULL ) {
    p->reScale(scale);

    int xpos = (int)(itemsize * m_Scale * wZLevel.getmodviewx(p->getZLevel()));
    int ypos = (int)(itemsize * m_Scale * wZLevel.getmodviewy(p->getZLevel()));

    p->SetPosition( wxPoint( xpos, ypos) );


    p = (BasePanel*)ListOp.next(m_ModList);
  }
  Refresh();
}


void ModPanel::removeItemFromList( iONode item ) {

}


void ModPanel::init(bool modview) {

}


void ModPanel::clean() {

}


const char* ModPanel::getZLevelTitle() {
  return "Module Overview";
}


int ModPanel::getZ() {
  return 0;
}


iONode ModPanel::getZLevel() {
  return NULL;
}


void ModPanel::refresh(bool eraseBackground) {
  Refresh(eraseBackground);
}


bool ModPanel::SetBackgroundColor(int red, int green, int blue) {
  BasePanel* p = (BasePanel*)ListOp.first(m_ModList);
  while( p != NULL ) {
    p->SetBackgroundColor(red, green, blue);
    p = (BasePanel*)ListOp.next(m_ModList);
  }
  /*
  wxColor color((byte)red, (byte)green, (byte)blue);
  return SetBackgroundColour(color);
  */
  return true;
}

void ModPanel::OnPanelProps(wxCommandEvent& event) {
  // Get the copied node:
  iONode ini = (iONode)event.GetClientData();
  if( ini != NULL && StrOp.equals( wModPlan.name(), NodeOp.getName( ini ) ) ) {
    // PropertiesDialog:
    m_ModPlanDlg = new ModPlanDlg( this, ini );
    if( wxID_OK == m_ModPlanDlg->ShowModal() ) {
    }
    m_ModPlanDlg->Destroy();
    m_ModPlanDlg = NULL;
  }
  else {
    iONode cmd = NodeOp.inst( wSysCmd.name(), NULL, ELEMENT_NODE );
    wSysCmd.setcmd( cmd, wSysCmd.getmodplan );
    wxGetApp().sendToRocrail( cmd, false );
    cmd->base.del(cmd);
  }
}


void ModPanel::OnModuleProps(wxCommandEvent& event) {
  // Get the copied node:
  iONode ini = (iONode)event.GetClientData();
  if( ini != NULL && StrOp.equals( wModule.name(), NodeOp.getName( ini ) ) ) {
    // TODO: find module with the given ID and dispatch the event
    BasePanel* p = (BasePanel*)ListOp.first(m_ModList);
    while( p != NULL ) {
      if( StrOp.equals( p->getZID(), wModule.getid(ini) ) ) {
        p->OnPanelProps(event);
      }
      p = (BasePanel*)ListOp.next(m_ModList);
    }
  }
}


bool ModPanel::hasZLevel(iONode zlevel) {
  BasePanel* p = (BasePanel*)ListOp.first(m_ModList);
  while( p != NULL ) {
    if( p->getZ() == wZLevel.getz(zlevel) )
      return true;
    p = (BasePanel*)ListOp.next(m_ModList);
  }
  return false;
}

BasePanel* ModPanel::updateZLevel(iONode zlevel) {
  BasePanel* p = (BasePanel*)ListOp.first(m_ModList);
  while( p != NULL ) {
    if( p->getZ() == wZLevel.getz(zlevel) ) {
      p->updateZLevel(zlevel);
      return p;
    }
    p = (BasePanel*)ListOp.next(m_ModList);
  }
  return NULL;
}


void ModPanel::OnPaint(wxPaintEvent& event)
{
  ClearBackground();
}


