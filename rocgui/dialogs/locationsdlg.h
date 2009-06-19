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
#ifndef _LOCATIONSDLG_H_
#define _LOCATIONSDLG_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "locationsdlg.h"
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
#define ID_DIALOG_LOCATIONS 10299
#define wxID_STATIC_LOCATIONS_LOCATIONS 10305
#define ID_LISTBOX_LOCATIONS_LOCATIONS 10300
#define wxID_STATIC_LOCATIONS_NAME 10306
#define ID_TEXTCTRL_LOCATION_NAME 10301
#define wxID_STATIC_LOCATIONS_BLOCKS 10309
#define ID_LISTBOX_LOCATIONS_BLOCKS 10302
#define ID__LOCATIONS_BLOCK_ADD 10278
#define ID__LOCATIONS_BLOCK_DELETE 10279
#define ID_BUTTON_LOCATIONS_NEW 10303
#define ID_BUTTON_LOCATIONS_MODIFY 10310
#define ID_BUTTON_LOCATIONS_DELETE 10304
#define SYMBOL_LOCATIONSDIALOG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_LOCATIONSDIALOG_TITLE _("Locations")
#define SYMBOL_LOCATIONSDIALOG_IDNAME ID_DIALOG_LOCATIONS
#define SYMBOL_LOCATIONSDIALOG_SIZE wxDefaultSize
#define SYMBOL_LOCATIONSDIALOG_POSITION wxDefaultPosition
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

/*!
 * LocationsDialog class declaration
 */

class LocationsDialog: public wxDialog
{
    DECLARE_DYNAMIC_CLASS( LocationsDialog )
    DECLARE_EVENT_TABLE()
  void initLabels();
  void initIndex();
  void initValues();
  bool evaluate();
  int m_TabAlign;

public:
    /// Constructors
    LocationsDialog( );
    LocationsDialog( wxWindow* parent, wxWindowID id = SYMBOL_LOCATIONSDIALOG_IDNAME, const wxString& caption = SYMBOL_LOCATIONSDIALOG_TITLE, const wxPoint& pos = SYMBOL_LOCATIONSDIALOG_POSITION, const wxSize& size = SYMBOL_LOCATIONSDIALOG_SIZE, long style = SYMBOL_LOCATIONSDIALOG_STYLE );
    LocationsDialog( wxWindow* parent, iONode p_Props );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_LOCATIONSDIALOG_IDNAME, const wxString& caption = SYMBOL_LOCATIONSDIALOG_TITLE, const wxPoint& pos = SYMBOL_LOCATIONSDIALOG_POSITION, const wxSize& size = SYMBOL_LOCATIONSDIALOG_SIZE, long style = SYMBOL_LOCATIONSDIALOG_STYLE );

    /// Creates the controls and sizers
    void CreateControls();

////@begin LocationsDialog event handler declarations

    /// wxEVT_COMMAND_LISTBOX_SELECTED event handler for ID_LISTBOX_LOCATIONS_LOCATIONS
    void OnListboxLocationsLocationsSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_LISTBOX_SELECTED event handler for ID_LISTBOX_LOCATIONS_BLOCKS
    void OnListboxLocationsBlocksSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID__LOCATIONS_BLOCK_ADD
    void OnLocationsBlockAddClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID__LOCATIONS_BLOCK_DELETE
    void OnLocationsBlockDeleteClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_LOCATIONS_NEW
    void OnButtonLocationsNewClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_LOCATIONS_MODIFY
    void OnButtonLocationsModifyClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_LOCATIONS_DELETE
    void OnButtonLocationsDeleteClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
    void OnOkClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
    void OnCancelClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_APPLY
    void OnApplyClick( wxCommandEvent& event );

////@end LocationsDialog event handler declarations

////@begin LocationsDialog member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end LocationsDialog member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin LocationsDialog member variables
    wxStaticText* m_LabLocations;
    wxListBox* m_LocationList;
    wxStaticText* m_LabName;
    wxTextCtrl* m_Name;
    wxStaticText* m_LabBlocks;
    wxListBox* m_BlockList;
    wxComboBox* m_BlockCombo;
    wxButton* m_AddBlock;
    wxButton* m_DeleteBlock;
    wxButton* m_New;
    wxButton* m_Modify;
    wxButton* m_Delete;
    wxButton* m_OK;
    wxButton* m_Cancel;
    wxButton* m_Apply;
////@end LocationsDialog member variables
    iONode m_Props;
};

#endif
    // _LOCATIONSDLG_H_
