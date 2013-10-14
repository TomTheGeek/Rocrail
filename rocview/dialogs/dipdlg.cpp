/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2013 Rob Versluis, Rocrail.net

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

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "rocrail/wrapper/public/DIP.h"
#include "rocrail/wrapper/public/DIPGroup.h"
#include "rocrail/wrapper/public/DIPValue.h"

#include "dipdlg.h"


DIPDlg::DIPDlg( wxWindow* parent, iONode dip )
: wxDialog( parent, wxID_ANY, wxT("DIP"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE )
{
  m_DIP = dip;

  this->SetSizeHints( wxDefaultSize, wxDefaultSize );

  wxBoxSizer* bSizer;
  bSizer = new wxBoxSizer( wxVERTICAL );

  m_Sizer =  new wxBoxSizer( wxHORIZONTAL );
  bSizer->Add( m_Sizer, 0, wxEXPAND|wxALL, 0 );

  m_sdButtons = new wxStdDialogButtonSizer();
  m_sdButtonsOK = new wxButton( this, wxID_OK );
  m_sdButtons->AddButton( m_sdButtonsOK );
  m_sdButtonsCancel = new wxButton( this, wxID_CANCEL );
  m_sdButtons->AddButton( m_sdButtonsCancel );
  m_sdButtons->Realize();

  bSizer->Add( m_sdButtons, 0, wxEXPAND|wxALL, 5 );

  if( m_DIP != NULL ) {
    initDIP();
  }

  this->SetSizer( bSizer );
  this->Layout();
  bSizer->Fit( this );

  this->Centre( wxBOTH );

  // Connect Events
  m_sdButtonsCancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIPDlg::onCancel ), NULL, this );
  m_sdButtonsOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIPDlg::onOK ), NULL, this );
  this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( DIPDlg::onClose ) );
}

DIPDlg::~DIPDlg()
{
  m_sdButtonsCancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIPDlg::onCancel ), NULL, this );
  m_sdButtonsOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIPDlg::onOK ), NULL, this );
  this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( DIPDlg::onClose ) );
}



void DIPDlg::addDIPGroup(iONode group, int idx) {
  TraceOp.trc( "dip", TRCLEVEL_INFO, __LINE__, 9999, "add group [%s]", wDIPGroup.getcaption(group) );
  if( wDIPGroup.gettype(group) == wDIPGroup.grouptype_radiobox ) {
    int n = NodeOp.getChildCnt(group);
    wxString choices[32];
    for( int i = 0; i < n && i < 32; i++ ) {
      iONode value = NodeOp.getChild(group, i);
      choices[i] = wxString(wDIPValue.getname(value), wxConvUTF8);
      m_Group[idx][i] = value;
    }
    m_RadioBox[idx] = new wxRadioBox(this, wxID_ANY,
        wxString( wDIPGroup.getcaption(group), wxConvUTF8), wxDefaultPosition, wxDefaultSize, n, choices);
    m_Sizer->Add( m_RadioBox[idx], 0, wxEXPAND|wxALL, 5 );

  }
}

void DIPDlg::initDIP() {
  TraceOp.trc( "dip", TRCLEVEL_INFO, __LINE__, 9999, "init [%s]", wDIP.gettitle(m_DIP) );
  SetTitle(wxString( wDIP.gettitle(m_DIP), wxConvUTF8));

  MemOp.set(m_RadioBox, 0, sizeof(m_RadioBox) );

  int idx = 0;
  iONode group = wDIP.getdipgroup(m_DIP);
  while( group != NULL ) {
    addDIPGroup(group, idx);
    group = wDIP.nextdipgroup(m_DIP, group);
  }

}


void DIPDlg::onCancel( wxCommandEvent& event ) {
  EndModal(0);
}

void DIPDlg::onOK( wxCommandEvent& event ) {
  EndModal( wxID_OK );
}

void DIPDlg::onClose( wxCloseEvent& event ) {
  EndModal(0);
}

int DIPDlg::getValue() {
  int val = 0;
  for( int i = 0; i < 32; i++ ) {
    if( m_RadioBox[i] != NULL ) {
      int sel = m_RadioBox[i]->GetSelection();
      val += wDIPValue.getvalue(m_Group[i][sel]);
      TraceOp.trc( "dip", TRCLEVEL_INFO, __LINE__, 9999,
          "group=%d selection=%d name=%s value=%d", i, sel, wDIPValue.getname(m_Group[i][sel]), val );
    }
    else {
      break;
    }
  }
  return val;
}

