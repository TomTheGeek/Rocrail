/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2011 - Rob Versluis <r.j.versluis@rocrail.net>

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

#include "bidibdlg.h"
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/defs.h"
#endif


#include "rocview/public/guiapp.h"
#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/BiDiB.h"
#include "rocrail/wrapper/public/BiDiBnode.h"

BidibDlg::BidibDlg( wxWindow* parent, iONode props )
  :bidibdlggen( parent )
{
  m_Props = props;
  initLabels();
  initValues();

  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);
  GetSizer()->Layout();

}


void BidibDlg::initLabels() {
  m_labIID->SetLabel( wxGetApp().getMsg( "iid" ) );
  m_labDevice->SetLabel( wxGetApp().getMsg( "port" ) );
}


void BidibDlg::initValues() {
  if( m_Props == NULL )
    return;

  m_IID->SetValue( wxString( wDigInt.getiid( m_Props ), wxConvUTF8 ) );

  iONode bidib = wDigInt.getbidib(m_Props);
  if( bidib == NULL ) {
    bidib = NodeOp.inst( wBiDiB.name(), m_Props, ELEMENT_NODE );
    NodeOp.addChild( m_Props, bidib );
  }

  m_Device->SetValue( wxString( wDigInt.getdevice( m_Props ), wxConvUTF8 ) );
}


void BidibDlg::evaluate() {
  if( m_Props == NULL )
    return;

  wDigInt.setiid( m_Props, m_IID->GetValue().mb_str(wxConvUTF8) );

  iONode bidib = wDigInt.getbidib(m_Props);
  wDigInt.setdevice( m_Props, m_Device->GetValue().mb_str(wxConvUTF8) );
}


void BidibDlg::OnAddNode( wxCommandEvent& event ) {
}


void BidibDlg::OnModifyNode( wxCommandEvent& event ) {
}


void BidibDlg::OnDeleteNode( wxCommandEvent& event ) {
}


void BidibDlg::OnCancel( wxCommandEvent& event ) {
  EndModal( wxID_CANCEL );
}


void BidibDlg::OnOK( wxCommandEvent& event ) {
  evaluate();
  EndModal( wxID_OK );
}
