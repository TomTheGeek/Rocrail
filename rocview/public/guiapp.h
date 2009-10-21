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
#ifndef __ROCRAIL_rocgui_GUIAPP_H
#define __ROCRAIL_rocgui_GUIAPP_H


#include "rocs/public/rocs.h"
#include "rocs/public/cmdln.h"
#include "rocs/public/doc.h"
#include "rocs/public/node.h"
#include "rocs/public/list.h"
#include "rocs/public/trace.h"
#include "rocs/public/res.h"
#include "rocs/public/wrpinf.h"

#include "rocrail/public/rcon.h"

#include "rocview/public/guiframe.h"
#include "rocview/public/planpanel.h"

enum {
  PT_EVENT = 20001,
};

class RocGui : public wxApp {
public:
  virtual bool OnInit();
  virtual int OnExit();
  int getItemSize();
  void sendToRocrail( iONode cmd, bool close=false );
  bool sendToRocrail( char* szCmd, bool wait4rr=false, bool close=false );
  RocGuiFrame* getFrame() { return m_Frame; }
  void setHost( const char* hostname, int port) { m_Host = hostname; m_Port = port; }
  void setModel( iONode node );
  iONode getModel();
  bool isModelSet();
  iONode getIni() { return m_Ini; }
  bool hasUndoItems() { return ListOp.size(m_UndoItems) > 0; }
  iOList getUndoItems() { return m_UndoItems; }
  void pushUndoItem( iONode item );
  iONode popUndoItem();
  bool isOffline() { return m_bOffline; }
  bool isStayOffline() { return m_bStayOffline; }
  void setStayOffline( bool stay ) { m_bStayOffline = stay; }
  wxString getMsg( const char* key );
  wxString getTip( const char* key );
  wxString getMenu( const char* key );
  iONode getWrpInf( const char* wrpname );
  void Callback( obj me, iONode node );
  void cleanupOldModel();
  void setLocalModelModified(bool modified=true){m_LocalModelModified=modified;}
  bool isLocalModelModified(){return m_LocalModelModified;}
  int getTabAlign();
  int getSvn() { return m_Svn; }
  int getTabSel() { return m_TabSel; }
  bool isModView() { return m_bModView; }
  void setModView(bool modview) { m_bForceTabView ?  m_bModView = false:m_bModView = modview; }
  bool isInit() { return m_bInit; }
  bool isRestrictedEdit();
  bool isForceTabView() { return m_bForceTabView; }

  iORCon        m_RCon;
  const char*   m_Host;
  int           m_Port;
  int           m_Svn;

private:
  void saveSizePos();
  void readArgs();
  wxString      m_LocalPlan;

  const char*   m_IniFileName;
  iONode        m_Ini;
  RocGuiFrame*  m_Frame;
  iOCmdLn       m_CmdLn;
  iOTrace       m_Trace;
  iONode        m_Model;
  iOList        m_UndoItems;
  iONode        m_OldModel;
  int           m_TabSel;
  bool          m_bOnExit;
  bool          m_bOffline;
  bool          m_bStayOffline;
  bool          m_LocalModelModified;
  bool          m_bModView;
  bool          m_bForceTabView;
  iORes         m_Res;
  iOWrpInf      m_WrpInf;
  bool          m_bInit;
};
DECLARE_APP(RocGui)





#endif
