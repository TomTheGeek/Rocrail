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
#ifndef __ROCRAIL_rocgui_SWITCHDLG_H
#define __ROCRAIL_rocgui_SWITCHDLG_H


// A custom modal dialog
#include <wx/spinctrl.h>

class SwCtrlDlg : public wxDialog
{
public:
  SwCtrlDlg(wxWindow *parent);

  void OnButton(wxCommandEvent& event);
  void OnSpinButton(wxSpinEvent& event);
  void OnClose(wxCloseEvent& event);

private:
  void setTooltips(void);
  int m_Bus;
  int m_Unit;

  wxButton*   m_Pin1Red;
  wxButton*   m_Pin1Green;
  wxButton*   m_Pin2Red;
  wxButton*   m_Pin2Green;
  wxButton*   m_Pin3Red;
  wxButton*   m_Pin3Green;
  wxButton*   m_Pin4Red;
  wxButton*   m_Pin4Green;

  wxStaticText* m_labIID;
  wxTextCtrl* m_IID;

  wxStaticText* m_labBus;
  wxSpinCtrl* m_BusSpin;

  wxSpinCtrl* m_UnitSpin;
  wxButton*   m_Quit;

  DECLARE_EVENT_TABLE()
};

#endif
