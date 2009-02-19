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
#include "operatordlg.h"

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/defs.h"
#endif

#include "rocgui/public/guiapp.h"
#include "rocrail/wrapper/public/ModelCmd.h"
#include "rocrail/wrapper/public/Plan.h"
#include "rocrail/wrapper/public/Item.h"
#include "rocrail/wrapper/public/Car.h"
#include "rocrail/wrapper/public/CarList.h"
#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/LocList.h"
#include "rocrail/wrapper/public/Operator.h"
#include "rocrail/wrapper/public/OperatorList.h"

#include "rocgui/wrapper/public/Gui.h"

#include "rocs/public/trace.h"
#include "rocs/public/strtok.h"
#include "rocs/public/system.h"

#include "rocgui/xpm/nopict.xpm"


OperatorDlg::OperatorDlg( wxWindow* parent, iONode p_Props )
  :operatordlggen( parent )
{

}

void OperatorDlg::initLabels() {

}


void OperatorDlg::evaluate() {

}


void OperatorDlg::initIndex() {

}


void OperatorDlg::initValues() {

}


void OperatorDlg::onNewOperator( wxCommandEvent& event ) {

}


void OperatorDlg::onDelOperator( wxCommandEvent& event ) {

}


void OperatorDlg::onLocoImage( wxCommandEvent& event ) {

}


void OperatorDlg::onLocomotiveCombo( wxCommandEvent& event ) {

}


void OperatorDlg::onReserve( wxCommandEvent& event ) {

}


void OperatorDlg::onRun( wxCommandEvent& event ) {

}


void OperatorDlg::onCarImage( wxCommandEvent& event ) {

}


void OperatorDlg::onAddCar( wxCommandEvent& event ) {

}


void OperatorDlg::onLeaveCar( wxCommandEvent& event ) {

}


void OperatorDlg::onCarCard( wxCommandEvent& event ) {

}


void OperatorDlg::onWayBill( wxCommandEvent& event ) {

}


void OperatorDlg::onApply( wxCommandEvent& event ) {
  if( m_Props == NULL )
    return;

  evaluate();
  if( !wxGetApp().isStayOffline() ) {
    /* Notify RocRail. */
    iONode cmd = NodeOp.inst( wModelCmd.name(), NULL, ELEMENT_NODE );
    wModelCmd.setcmd( cmd, wModelCmd.modify );
    NodeOp.addChild( cmd, (iONode)m_Props->base.clone( m_Props ) );
    wxGetApp().sendToRocrail( cmd );
    cmd->base.del(cmd);
  }
  initIndex();
}


void OperatorDlg::onCancel( wxCommandEvent& event ) {
  EndModal( 0 );
}


void OperatorDlg::onOK( wxCommandEvent& event ) {
  if( m_bSave )
    onApply(event);
  EndModal( wxID_OK );
}

