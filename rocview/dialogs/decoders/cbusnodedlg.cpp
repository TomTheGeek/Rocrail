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
#include "cbusnodedlg.h"

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/defs.h"
#endif

#include "rocview/public/guiapp.h"

#include "rocrail/wrapper/public/Program.h"
#include "rocview/wrapper/public/Gui.h"

#include "rocs/public/trace.h"

CBusNodeDlg::CBusNodeDlg( wxWindow* parent )
:
cbusnodedlggen( parent )
{

}

void CBusNodeDlg::onOK( wxCommandEvent& event )
{
	// TODO: Implement onOK
}

void CBusNodeDlg::event( iONode event ) {
}
