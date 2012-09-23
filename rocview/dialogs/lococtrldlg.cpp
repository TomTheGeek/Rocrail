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

#include "lococtrldlg.h"

#include "rocs/public/trace.h"


BEGIN_EVENT_TABLE(LocoCtrlDlg, wxDialog)
  EVT_BUTTON(-1, LocoCtrlDlg::OnButton)
  EVT_COMMAND_SCROLL( -1, LocoCtrlDlg::OnSlider )
  EVT_COMMAND_SCROLL_THUMBRELEASE( -1, LocoCtrlDlg::OnSlider )
END_EVENT_TABLE()


LocoCtrlDlg::LocoCtrlDlg( wxWindow* parent ):LocoCtrlDlgGen( parent )
{

}

LocoCtrlDlg::LocoCtrlDlg( wxWindow* parent, iOList list, iOMap map, const char* locid ):LocoCtrlDlgGen( parent )
{
  m_iSpeed = 0;
  m_iFnGroup = 0;

  //initLabels();
  //initLocMap(locid);
  //setFLabels();

  m_DlgList = list;
  m_DlgMap  = map;
  ListOp.add( m_DlgList, (obj)this );
}


void LocoCtrlDlg::OnButton(wxCommandEvent& event) {
}


void LocoCtrlDlg::OnSlider(wxScrollEvent& event) {
}


void LocoCtrlDlg::onLocoImg( wxCommandEvent& event ) {
}

void LocoCtrlDlg::onDir( wxCommandEvent& event ) {
}

void LocoCtrlDlg::onLocoList( wxCommandEvent& event ) {
}

void LocoCtrlDlg::onCancel( wxCommandEvent& event ) {
  ListOp.removeObj( m_DlgList, (obj)this );

  if( m_LocoList->GetSelection() == wxNOT_FOUND ) {
    Destroy();
    return;
  }

  int x,y;
  GetPosition(&x,&y);

  wxString id = m_LocoList->GetStringSelection();
  char* LocID = StrOp.dup((const char*)id.mb_str(wxConvUTF8));

  TraceOp.trc( "locdialog", TRCLEVEL_INFO, __LINE__, 9999, "position [%d,%d] for [%s]", x, y, LocID );
  if( StrOp.len(LocID) > 0 ) {
    char* pos = (char*)MapOp.get(m_DlgMap, LocID);
    if( pos != NULL ) {
      TraceOp.trc( "locdialog", TRCLEVEL_INFO, __LINE__, 9999, "remove previous position [%s] for [%s]", pos, LocID );
      StrOp.free(pos);
      MapOp.remove(m_DlgMap, LocID);
    }
    pos = StrOp.fmt("%d,%d", x, y);
    MapOp.put( m_DlgMap, LocID, (obj)pos );
  }

  StrOp.free(LocID);

  Destroy();
}

void LocoCtrlDlg::onStop( wxCommandEvent& event ) {
}

void LocoCtrlDlg::onBreak( wxCommandEvent& event ) {
}

void LocoCtrlDlg::onClose( wxCloseEvent& event )
{
  onCancel((wxCommandEvent&)event);
}


