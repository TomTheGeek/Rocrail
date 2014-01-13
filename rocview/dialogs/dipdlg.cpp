/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2014 Rob Versluis, Rocrail.net

 


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
#include "rocrail/wrapper/public/Program.h"

#include "dipdlg.h"
#include "rocprodlg.h"


DIPDlg::DIPDlg( wxWindow* parent, iONode dip, int cvnr, int value, const char* title )
: wxDialog( parent, wxID_ANY, wxT("DIP"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE )
{
  m_Title = title;
  m_DIP = dip;
  m_CVNr = cvnr;
  m_Value = value;
  m_Parent = parent;

  this->SetSizeHints( wxDefaultSize, wxDefaultSize );

  wxBoxSizer* bSizer;
  bSizer = new wxBoxSizer( wxVERTICAL );

  m_Sizer =  new wxBoxSizer( wxHORIZONTAL );
  bSizer->Add( m_Sizer, 0, wxEXPAND|wxALL, 0 );

  m_sdButtons = new wxStdDialogButtonSizer();
  m_sdButtonsOK = new wxButton( this, wxID_OK );
  m_sdButtons->AddButton( m_sdButtonsOK );
  m_sdButtonsApply = new wxButton( this, wxID_APPLY );
  m_sdButtons->AddButton( m_sdButtonsApply );
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
  m_sdButtonsApply->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIPDlg::onApply ), NULL, this );
  this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( DIPDlg::onClose ) );
}

DIPDlg::~DIPDlg()
{
  m_sdButtonsCancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIPDlg::onCancel ), NULL, this );
  m_sdButtonsOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIPDlg::onOK ), NULL, this );
  m_sdButtonsApply->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIPDlg::onApply ), NULL, this );
  this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( DIPDlg::onClose ) );
  for( int i = 0; i < 32; i++ ) {
    if( m_RadioBox[i] != NULL ) {
      delete m_RadioBox[i];
    }
    else {
      break;
    }
  }
}



void DIPDlg::addDIPGroup(iONode group, int idx) {
  TraceOp.trc( "dip", TRCLEVEL_INFO, __LINE__, 9999, "add group [%s]", wDIPGroup.getcaption(group) );
  int n = NodeOp.getChildCnt(group);

  // Radiobuttons
  if( wDIPGroup.gettype(group) == wDIPGroup.grouptype_radiobox ) {
    wxString choices[32];
    wxString infos[32];
    int selection = 0;
    int items = 0;
    for( int i = 0; i < n && i < 32; i++ ) {
      iONode value = NodeOp.getChild(group, i);
      choices[i] = wxString(wDIPValue.getname(value), wxConvUTF8);
      infos[i] = wxString(wDIPValue.getinfo(value), wxConvUTF8);
      m_Group[idx][i] = value;
      if( wDIPValue.gettype(value) == wDIPValue.valuetype_cv &&  wDIPValue.getvalue(value) == m_CVNr )
        selection = i;
      else if( wDIPValue.getvalue(value) == ( m_Value & wDIPGroup.getmask(group)) )
        selection = i;
      items++;
    }
    m_RadioBox[idx] = new wxRadioBox(this, wxID_ANY,
        wxString( wDIPGroup.getcaption(group), wxConvUTF8), wxDefaultPosition, wxDefaultSize, n, choices,
        0, (wDIPGroup.getori(group) == 1) ? wxRA_SPECIFY_ROWS:wxRA_SPECIFY_COLS);
    m_Sizer->Add( m_RadioBox[idx], 0, wxEXPAND|wxALL, 5 );
    m_RadioBox[idx]->SetSelection(selection);
    for( int x = 0; x < items && x < 32; x++ ) {
      m_RadioBox[idx]->SetItemToolTip(x, infos[x]);
    }
  }

  // Checkboxes
  if( wDIPGroup.gettype(group) == wDIPGroup.grouptype_box ) {
    wxStaticBoxSizer* bSizer =  new wxStaticBoxSizer( (wDIPGroup.getori(group) == 1) ? wxVERTICAL:wxHORIZONTAL, this, wxString( wDIPGroup.getcaption(group), wxConvUTF8) );
    m_Sizer->Add( bSizer, 0, wxEXPAND|wxALL, 5 );
    for( int i = 0; i < n && i < 32; i++ ) {
      iONode value = NodeOp.getChild(group, i);
      m_Group[idx][i] = value;
      m_CheckBox[idx][i] = new wxCheckBox(this, wxID_ANY,
          wxString(wDIPValue.getname(value), wxConvUTF8) );
      bSizer->Add( m_CheckBox[idx][i], 0, wxEXPAND|wxBOTTOM|wxRIGHT|wxLEFT|(i==0?wxTOP:0), wDIPGroup.getspacing(group) );

      if( wDIPValue.gettype(value) == wDIPValue.valuetype_cv &&  wDIPValue.getvalue(value) == m_CVNr )
        m_CheckBox[idx][i]->SetValue(true);
      else if( wDIPValue.getvalue(value) & m_Value )
        m_CheckBox[idx][i]->SetValue(true);

      m_CheckBox[idx][i]->SetToolTip(wxString(wDIPValue.getinfo(value), wxConvUTF8));
    }
  }
}

void DIPDlg::initDIP() {
  if( m_Title == NULL )
    m_Title = wDIP.gettitle(m_DIP);

  TraceOp.trc( "dip", TRCLEVEL_INFO, __LINE__, 9999, "init [%s]", m_Title );
  SetTitle(wxString( m_Title, wxConvUTF8));

  MemOp.set(m_RadioBox, 0, sizeof(m_RadioBox) );
  MemOp.set(m_CheckBox, 0, sizeof(m_CheckBox) );

  int idx = 0;
  iONode group = wDIP.getdipgroup(m_DIP);
  while( group != NULL ) {
    addDIPGroup(group, idx);
    idx++;
    group = wDIP.nextdipgroup(m_DIP, group);
  }

}


void DIPDlg::onCancel( wxCommandEvent& event ) {
  EndModal(0);
}

void DIPDlg::onOK( wxCommandEvent& event ) {
  EndModal( wxID_OK );
}

void DIPDlg::onApply( wxCommandEvent& event ) {
  int value = getValue(&m_CVNr);
  ((RocProDlg*)m_Parent)->doCV(wProgram.set, m_CVNr, value);
}

void DIPDlg::onClose( wxCloseEvent& event ) {
  EndModal(0);
}

int DIPDlg::getValue(int* cvnr) {
  int val = 0;
  int cv = 0;
  for( int i = 0; i < 32; i++ ) {
    if( m_RadioBox[i] != NULL ) {
      int sel = m_RadioBox[i]->GetSelection();
      if( wDIPValue.gettype(m_Group[i][sel]) == wDIPValue.valuetype_value )
        val += wDIPValue.getvalue(m_Group[i][sel]);
      else if( wDIPValue.gettype(m_Group[i][sel]) == wDIPValue.valuetype_cv )
        cv = wDIPValue.getvalue(m_Group[i][sel]);
      TraceOp.trc( "dip", TRCLEVEL_INFO, __LINE__, 9999,
          "group=%d selection=%d name=%s value=%d", i, sel, wDIPValue.getname(m_Group[i][sel]), val );
    }
    for( int n = 0; n < 32; n++ ) {
      if( m_CheckBox[i][n] != NULL && m_CheckBox[i][n]->IsChecked() ) {
        if( wDIPValue.gettype(m_Group[i][n]) == wDIPValue.valuetype_value )
          val += wDIPValue.getvalue(m_Group[i][n]);
        else if( wDIPValue.gettype(m_Group[i][n]) == wDIPValue.valuetype_cv )
          cv = wDIPValue.getvalue(m_Group[i][n]);
        TraceOp.trc( "dip", TRCLEVEL_INFO, __LINE__, 9999,
            "group=%d selection=%d name=%s value=%d", i, n, wDIPValue.getname(m_Group[i][n]), val );
      }
    }
  }

  if( cvnr != NULL && cv > 0 )
    *cvnr = cv;

  return val;
}

