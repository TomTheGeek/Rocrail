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
#ifndef __ROCRAIL_rocgui_STREETDLG_H
#define __ROCRAIL_rocgui_STREETDLG_H


// A custom modal dialog
#include <wx/grid.h>

class RouteCtrlDlg : public wxDialog
{
public:
  RouteCtrlDlg(wxWindow *parent);

  void OnButton(wxCommandEvent& event);
  void OnClose(wxCloseEvent& event);

private:
  void init();
  void fillTable(iONode node);
  wxGrid* m_Grid;

  wxButton*   m_Quit;
  wxButton*   m_Test;
  wxButton*   m_Force;

  DECLARE_EVENT_TABLE()
};

#endif
