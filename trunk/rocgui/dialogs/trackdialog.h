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
#ifndef _TRACKDIALOG_H_
#define _TRACKDIALOG_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "trackdialog.cpp"
#endif

/*!
 * Includes
 */

////@begin includes
#include "wx/notebook.h"
////@end includes

#include "rocs/public/node.h"

/*!
 * Forward declarations
 */

////@begin forward declarations
class wxNotebook;
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_DIALOG 10124
#define ID_NOTEBOOK_TRACK 10125
#define ID_PANEL_TK_GENERAL 10000
#define wxID_STATIC_TK_ID 10001
#define ID_TEXTCTRL_TK_ID 10002
#define wxID_STATIC_TRACK_BLOCKID 10168
#define ID_COMBOBOX_TRACK_BLOCKID 10169
#define wxID_STATIC_TK_TYPE 10003
#define ID_COMBOBOX_TK_TYPE 10004
#define ID_PANEL_TK_LOCATION 10009
#define wxID_STATIC_TK_X 10010
#define ID_TEXTCTRL_TK_X 10011
#define wxID_STATIC_TK_Y 10012
#define ID_TEXTCTRL_TK_Y 10013
#define wxID_STATIC_TK_Z 10014
#define ID_TEXTCTRL_TK_Z 10015
#define ID_RADIOBOX_TK_ORI 10016
#define SYMBOL_TRACKDIALOG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_TRACKDIALOG_TITLE _("Dialog")
#define SYMBOL_TRACKDIALOG_IDNAME ID_DIALOG
#define SYMBOL_TRACKDIALOG_SIZE wxDefaultSize
#define SYMBOL_TRACKDIALOG_POSITION wxDefaultPosition
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif
#ifndef wxFIXED_MINSIZE
#define wxFIXED_MINSIZE 0
#endif

/*!
 * TrackDialog class declaration
 */

class TrackDialog: public wxDialog
{
    DECLARE_DYNAMIC_CLASS( TrackDialog )
    DECLARE_EVENT_TABLE()

  void initLabels();
  void initValues();
  void evaluate();
  int m_TabAlign;

public:
    /// Constructors
    TrackDialog( );
    TrackDialog( wxWindow* parent, iONode p_Props );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_TRACKDIALOG_IDNAME, const wxString& caption = SYMBOL_TRACKDIALOG_TITLE, const wxPoint& pos = SYMBOL_TRACKDIALOG_POSITION, const wxSize& size = SYMBOL_TRACKDIALOG_SIZE, long style = SYMBOL_TRACKDIALOG_STYLE );

    /// Creates the controls and sizers
    void CreateControls();

////@begin TrackDialog event handler declarations

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
    void OnOkClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
    void OnCancelClick( wxCommandEvent& event );

////@end TrackDialog event handler declarations

////@begin TrackDialog member function declarations

    iONode GetProps() const { return m_Props ; }
    void SetProps(iONode value) { m_Props = value ; }

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end TrackDialog member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin TrackDialog member variables
    wxNotebook* m_Notebook;
    wxPanel* m_GeneralPanel;
    wxStaticText* m_LabelID;
    wxTextCtrl* m_ID;
    wxStaticText* m_LabelBlockID;
    wxComboBox* m_BlockID;
    wxStaticText* m_labRouteIDs;
    wxTextCtrl* m_RouteIDs;
    wxStaticText* m_LabelType;
    wxComboBox* m_Type;
    wxCheckBox* m_Road;
    wxPanel* m_LocationPanel;
    wxStaticText* m_LabelX;
    wxTextCtrl* m_x;
    wxStaticText* m_LabelY;
    wxTextCtrl* m_y;
    wxStaticText* m_LabelZ;
    wxTextCtrl* m_z;
    wxRadioBox* m_ori;
    wxButton* m_OK;
    wxButton* m_Cancel;
    iONode m_Props;
////@end TrackDialog member variables
};

#endif
    // _TRACKDIALOG_H_
