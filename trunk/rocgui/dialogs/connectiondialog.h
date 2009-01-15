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
#ifndef _CONNECTIONDIALOG_H_
#define _CONNECTIONDIALOG_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "connectiondialog.cpp"
#endif

/*!
 * Includes
 */

////@begin includes
////@end includes

#include "rocs/public/node.h"

/*!
 * Forward declarations
 */

////@begin forward declarations
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_DIALOG_CONNECTION 10083
#define wxID_STATIC_CON_HOST 10088
#define ID_COMBOBOX_CONN_HOST 10086
#define wxID_STATIC_CONN_PORT 10089
#define ID_TEXTCTRL_CONN_PORT 10087
#define SYMBOL_CONNECTIONDIALOG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_CONNECTIONDIALOG_TITLE _("Rocrail Connection")
#define SYMBOL_CONNECTIONDIALOG_IDNAME ID_DIALOG_CONNECTION
#define SYMBOL_CONNECTIONDIALOG_SIZE wxDefaultSize
#define SYMBOL_CONNECTIONDIALOG_POSITION wxDefaultPosition
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
 * ConnectionDialog class declaration
 */

class ConnectionDialog: public wxDialog
{
    DECLARE_DYNAMIC_CLASS( ConnectionDialog )
    DECLARE_EVENT_TABLE()

  void initLabels();
  void initValues();
  void evaluate();
  int m_TabAlign;

public:
    /// Constructors
    ConnectionDialog( );
    ConnectionDialog( wxWindow* parent, iONode props );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_CONNECTIONDIALOG_IDNAME, const wxString& caption = SYMBOL_CONNECTIONDIALOG_TITLE, const wxPoint& pos = SYMBOL_CONNECTIONDIALOG_POSITION, const wxSize& size = SYMBOL_CONNECTIONDIALOG_SIZE, long style = SYMBOL_CONNECTIONDIALOG_STYLE );

    /// Creates the controls and sizers
    void CreateControls();
    wxString getHostname();
    int getPort();

////@begin ConnectionDialog event handler declarations

    /// wxEVT_COMMAND_COMBOBOX_SELECTED event handler for ID_COMBOBOX_CONN_HOST
    void OnComboboxConnHostSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_TEXT_UPDATED event handler for ID_COMBOBOX_CONN_HOST
    void OnComboboxConnHostUpdated( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
    void OnOkClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
    void OnCancelClick( wxCommandEvent& event );

////@end ConnectionDialog event handler declarations

////@begin ConnectionDialog member function declarations

    iONode GetProps() const { return m_Props ; }
    void SetProps(iONode value) { m_Props = value ; }

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end ConnectionDialog member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin ConnectionDialog member variables
    wxStaticText* m_LabelHost;
    wxComboBox* m_Host;
    wxStaticText* m_LabelPort;
    wxTextCtrl* m_Port;
    wxButton* m_OK;
    wxButton* m_Cancel;
    iONode m_Props;
////@end ConnectionDialog member variables
};

#endif
    // _CONNECTIONDIALOG_H_
