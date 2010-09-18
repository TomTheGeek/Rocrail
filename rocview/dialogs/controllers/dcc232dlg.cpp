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

#include "dcc232dlg.h"
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/defs.h"
#endif

#include "rocview/public/guiapp.h"
#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/DCC232.h"

DCC232Dlg::DCC232Dlg( wxWindow* parent, iONode props )
  :dcc232gen( parent )
{
  m_Props = props;
  initLabels();
  initValues();
}


void DCC232Dlg::initLabels() {
  m_labIID->SetLabel( wxGetApp().getMsg( "iid" ) );
  m_PortBox->GetStaticBox()->SetLabel( wxGetApp().getMsg( "port" ) );
  m_labDevice->SetLabel( wxGetApp().getMsg( "port" ) );
  m_labOffset->SetLabel( wxGetApp().getMsg( "portbase" ) );

  m_SlotBox->GetStaticBox()->SetLabel( wxGetApp().getMsg( "refresh" ) );
  m_AutoPurge->SetLabel( wxGetApp().getMsg( "purgeslots" ) );
  m_labPurgeTime->SetLabel( wxGetApp().getMsg( "purgetime" ) );

  m_ShortCutBox->GetStaticBox()->SetLabel( wxGetApp().getMsg( "shortcutchecking" ) );
  m_ShortcutDetection->SetLabel( wxGetApp().getMsg( "activate" ) );
  m_labShortcutDelay->SetLabel( wxGetApp().getMsg( "delay" ) );
  m_labShortcutDelayMs->SetLabel( wxGetApp().getMsg( "ms" ) );
}


void DCC232Dlg::initValues() {
  if( m_Props == NULL )
    return;

  m_IID->SetValue( wxString( wDigInt.getiid( m_Props ), wxConvUTF8 ) );

  iONode dcc232 = wDigInt.getdcc232(m_Props);
  if( dcc232 == NULL ) {
    dcc232 = NodeOp.inst( wDCC232.name(), m_Props, ELEMENT_NODE );
    NodeOp.addChild( m_Props, dcc232 );
  }

  m_Device->SetValue( wxString( wDCC232.getport( dcc232 ), wxConvUTF8 ) );
  m_Offset->SetValue( wxString( wDCC232.getportbase( dcc232 ), wxConvUTF8 ) );
  m_AutoPurge->SetValue( wDCC232.ispurge( dcc232 ) ? true:false );
  m_PurgeTime->SetValue( wDCC232.getpurgetime( dcc232 ) );
  m_ShortcutDetection->SetValue( wDCC232.isshortcut( dcc232 ) ? true:false );
  m_ShortcutDelay->SetValue( wDCC232.getshortcutdelay( dcc232 ) );
}


void DCC232Dlg::evaluate() {
  if( m_Props == NULL )
    return;

  wDigInt.setiid( m_Props, m_IID->GetValue().mb_str(wxConvUTF8) );

  iONode dcc232 = wDigInt.getdcc232(m_Props);
  wDCC232.setport( dcc232, m_Device->GetValue().mb_str(wxConvUTF8) );
  wDCC232.setportbase( dcc232, m_Offset->GetValue().mb_str(wxConvUTF8) );

  wDCC232.setpurge( dcc232, m_AutoPurge->IsChecked() ? True:False );
  wDCC232.setpurgetime( dcc232, m_PurgeTime->GetValue() );
  wDCC232.setshortcut( dcc232, m_ShortcutDetection->IsChecked() ? True:False );
  wDCC232.setshortcutdelay( dcc232, m_ShortcutDelay->GetValue() );
}


void DCC232Dlg::onOK( wxCommandEvent& event ) {
  evaluate();
  EndModal( wxID_OK );
}


void DCC232Dlg::onCancel( wxCommandEvent& event ) {
  EndModal( wxID_CANCEL );
}




