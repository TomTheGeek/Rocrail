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

#include "ucons88dlg.h"
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/defs.h"
#endif


#include "rocview/public/guiapp.h"

#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/uConS88.h"
//#include "rocrail/wrapper/public/uConS88node.h"

#include "rocutils/public/vendors.h"

#include "rocs/public/strtok.h"

uConS88Dlg::uConS88Dlg( wxWindow* parent, iONode props, const char* devices )
:
ucons88dlggen( parent ) {
  m_Props = props;
  m_Devices = devices;
  __initVendors();
  initLabels();
  initValues();

//  m_General->GetSizer()->Layout();
//  m_Options->GetSizer()->Layout();

//  m_General->Fit();
//  m_Options->Fit();

  GetSizer()->Layout();
  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);
}

void uConS88Dlg::initLabels() {
  // General
  m_labIID->SetLabel( wxGetApp().getMsg( "iid" ) );

  m_DeviceBox->GetStaticBox()->SetLabel( wxGetApp().getMsg( "device" ) );
  m_labHost->SetLabel( wxGetApp().getMsg( "host" ) );
  m_labPort->SetLabel( wxGetApp().getMsg( "port" ) );

  m_S88BusBox->GetStaticBox()->SetLabel( _T("S88 ") + wxGetApp().getMsg( "bus" ) );

  char* val = StrOp.fmt( wxGetApp().getMsg( "buslength" ), 1 );
  m_labBus1->SetLabel( val );
  StrOp.free( val );
  val = StrOp.fmt( wxGetApp().getMsg( "buslength" ), 2 );
  m_labBus2->SetLabel( val );
  StrOp.free( val );
  val = StrOp.fmt( wxGetApp().getMsg( "buslength" ), 3 );
  m_labBus3->SetLabel( val );
  StrOp.free( val );

  // Buttons
  m_StdButtonOK->SetLabel( wxGetApp().getMsg( "ok" ) );
  m_StdButtonCancel->SetLabel( wxGetApp().getMsg( "cancel" ) );

}

void uConS88Dlg::initValues() {
  if( m_Props == NULL )
    return;

  iONode ucons88ini = wDigInt.getucons88(m_Props);
  if( ucons88ini == NULL ) {
    ucons88ini = NodeOp.inst( wuConS88.name(), m_Props, ELEMENT_NODE );
    NodeOp.addChild(m_Props, ucons88ini );
  }

  // General
  m_IID->SetValue( wxString( wDigInt.getiid( m_Props ), wxConvUTF8 ) );

  if ( wuConS88.getport( m_Props ) > 0 ) {
    m_Host->SetValue( wxString( wuConS88.gethost( m_Props ), wxConvUTF8 ) );
    m_Port->SetValue( wuConS88.getport( m_Props ) );
    m_B1->SetValue( wuConS88.getfbB1( m_Props ) );
    m_B2->SetValue( wuConS88.getfbB2( m_Props ) );
    m_B3->SetValue( wuConS88.getfbB2( m_Props ) );
  } else {
    m_Host->SetValue( wxString( wuConS88.gethost( ucons88ini ), wxConvUTF8 ) );
    m_Port->SetValue( wuConS88.getport( ucons88ini ) );
    m_B1->SetValue( wuConS88.getfbB1( ucons88ini ) );
    m_B2->SetValue( wuConS88.getfbB2( ucons88ini ) );
    m_B3->SetValue( wuConS88.getfbB2( ucons88ini ) );
  }
  
}

void uConS88Dlg::evaluate() {
  if( m_Props == NULL )
    return;

  // General
  wDigInt.setiid( m_Props, m_IID->GetValue().mb_str(wxConvUTF8) );
  wDigInt.sethost( m_Props, m_Host->GetValue().mb_str(wxConvUTF8) );
  wDigInt.setport( m_Props, m_Port->GetValue() );

  wuConS88.setfbB1( m_Props, m_B1->GetValue() );
  wuConS88.setfbB2( m_Props, m_B2->GetValue() );
  wuConS88.setfbB3( m_Props, m_B3->GetValue() );

}

void uConS88Dlg::OnCancel( wxCommandEvent& event )
{
  EndModal( wxID_CANCEL );
}

void uConS88Dlg::OnOK( wxCommandEvent& event )
{
  evaluate();
  EndModal( wxID_OK );
}

void uConS88Dlg::OnHelp( wxCommandEvent& event )
{
  wxGetApp().openLink( "ucons88" );
}
