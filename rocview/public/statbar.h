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
#ifndef __ROCRAIL_rocgui_STATBAR_H
#define __ROCRAIL_rocgui_STATBAR_H

/*!
 * Includes
 */

#include "wx/statusbr.h"
#include "wx/statbmp.h"

class StatusBar : public wxStatusBar
{
public:
    StatusBar(wxWindow *parent);
    virtual ~StatusBar();

    void OnSize(wxSizeEvent& event);
    void Update(int flags);

private:

    wxStaticBitmap *m_statbmpP;
    wxStaticBitmap *m_statbmpT;
    wxStaticBitmap *m_statbmpS;
    wxStaticBitmap *m_statbmpA;

    DECLARE_EVENT_TABLE()
};

enum {
  status_info = 0,
  status_rcon,
  status_digint,
  status_load,
  status_controller,
  status_max
};

#define BITMAP_SIZE_X 16
#define BITMAP_SIZE_Y 16

#endif
