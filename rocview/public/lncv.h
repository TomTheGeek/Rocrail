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
#ifndef __ROCRAIL_rocgui_LNCV_H
#define __ROCRAIL_rocgui_LNCV_H


#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "lncv.h"
#endif

#include "rocs/public/node.h"


class LNCV
{    
public:
    LNCV( wxScrolledWindow* parent, wxWindow* frame );
    void OnButton(wxCommandEvent& event);
    void event( iONode event );

private:
    bool Create();
    void CreateControls();
    void init();

    wxScrolledWindow* m_Parent;

    wxBoxSizer* m_MainBox;
    
    wxPanel* m_ItemPanel;
    wxStaticText* m_labModuleKey;
    wxTextCtrl* m_ModuleKey;
    wxButton* m_ModuleDialog;
    wxButton* m_ModuleDetect;
    wxStaticText* m_labAddress;
    wxTextCtrl* m_Address;
    wxCheckBox* m_useLNCV;
    wxTextCtrl* m_CVfrom;
    wxStaticText* m_labValue;
    wxTextCtrl* m_Value;
    wxButton* m_Start;
    wxButton* m_Stop;
    wxButton* m_Get;
    wxButton* m_Set;
};

#endif
