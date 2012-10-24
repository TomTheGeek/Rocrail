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
// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------
#ifdef __GNUG__
    #pragma implementation "rocgui.cpp"
    #pragma interface "rocgui.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif


#include "rocs/public/node.h"
#include "rocs/public/str.h"
#include "rocs/public/trace.h"
#include "rocs/public/file.h"

#include "rocview/public/base.h"


wxColor Base::getWhite (){ return wxColor( 255, 255, 255 ); }
wxColor Base::getBlack (){ return wxColor(   0,   0,   0 ); }
wxColor Base::getRed   (){ return wxColor( 255, 200, 200 ); }
wxColor Base::getYellow(){ return wxColor( 255, 255,   0 ); }
wxColor Base::getGreen (){ return wxColor( 200, 255, 200 ); }
wxColor Base::getBlue  (){ return wxColor( 200, 200, 255 ); }
wxColor Base::getNoneActiveColor(){ return wxColor( 240, 200, 200 ); }
wxColor Base::getResColor(){ return wxColor( 255, 255, 200 ); }
wxColor Base::getRes2Color(){ return wxColor( 255, 255, 170 ); }
