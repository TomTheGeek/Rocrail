/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2012 Rob Versluis, Rocrail.net

 Without an official permission commercial use is not permitted.
 Forking this project is not permitted.

 This program is free software; you can redistribute it and/or
 as published by the Free Software Foundation; either version 2
 modify it under the terms of the GNU General Public License
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "bidibidentdlg.h"

#include <wx/clipbrd.h>


#include "rocrail/wrapper/public/Program.h"

BidibIdentDlg::BidibIdentDlg( wxWindow* parent ):BidibIdentDlgGen( parent )
{

}

BidibIdentDlg::BidibIdentDlg( wxWindow* parent, iONode node ):BidibIdentDlgGen( parent )
{
  this->node = node;
  initLabels();
  initValues();
}


void BidibIdentDlg::onCancel( wxCommandEvent& event ) {
  EndModal( 0 );
}


void BidibIdentDlg::onOK( wxCommandEvent& event ) {
  wxClipboard* cb = new wxClipboard();
  if( cb->Open() ) {
    cb->SetData( new wxTextDataObject(m_UID->GetValue()) );
    cb->Close();
  }
  delete cb;

  EndModal( wxID_OK );
}


void BidibIdentDlg::event(iONode node) {
  this->node = node;
  initValues();
}


void BidibIdentDlg::initLabels() {

}


void BidibIdentDlg::initValues() {
  m_Path->SetValue( wxString( wProgram.getfilename(node), wxConvUTF8) );
  m_UID->SetValue( wxString::Format(_T("%d"), wProgram.getmodid(node) ) );
  m_Vendor->SetValue( wxString::Format(_T("%d"), wProgram.getmanu(node) ) );
  m_Class->SetValue( wxString::Format(_T("0x%02X"), wProgram.getprod(node) ) );
}
