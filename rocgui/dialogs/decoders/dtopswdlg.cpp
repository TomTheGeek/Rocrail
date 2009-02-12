/*
 Rocrail - Model Railroad Software

 Copyright (C) Rob Versluis <r.j.versluis@rocrail.net>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 3
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
#include "dtopswdlg.h"

#include "rocdigs/impl/loconet/dtopsw.h"


DTOpSwDlg::DTOpSwDlg( wxWindow* parent )
  :dtopswdlg( parent )
{
  initLabels();
  initValues();
}


void DTOpSwDlg::initLabels() {
  int i = 0;
  while( DT_BoardTypes[i] != NULL ) {
    m_BoardType->Append( wxString(DT_BoardTypes[i],wxConvUTF8) );
    i++;
  }
}


void DTOpSwDlg::initValues() {

}


void DTOpSwDlg::onBoardType( wxCommandEvent& event ) {
  int selectedBoard = m_BoardType->GetSelection();
  m_OpSwList->Clear();

  int i = 0;
  while( DT_OpSw[selectedBoard][i] != NULL ) {
    m_OpSwList->Append( wxString::Format(_T("%02d"), DT_idxOpSw[selectedBoard][i]) + _T(" ") + wxString(DT_OpSw[selectedBoard][i],wxConvUTF8) );
    i++;
  }
}


void DTOpSwDlg::onReadAll( wxCommandEvent& event ) {

}


void DTOpSwDlg::onWriteAll( wxCommandEvent& event ) {

}

void DTOpSwDlg::onOK( wxCommandEvent& event ){
  EndModal( wxID_OK );
}

