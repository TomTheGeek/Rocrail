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
#ifndef _UNDODLG_H_
#define _UNDODLG_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "undodlg.h"
#endif

/*!
 * Includes
 */

////@begin includes
////@end includes
#include "rocs/public/node.h"
#include "rocs/public/list.h"

/*!
 * Forward declarations
 */

////@begin forward declarations
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_DIALOG_UNDO 10237
#define ID_LISTBOX_UNDO_LIST 10238
#define ID_BUTTON_UNDO_CANCEL 10239
#define ID_BUTTON_UNDO_DELETE 10240
#define ID_BUTTON_UNDO_UNDO 10241
#define SYMBOL_UNDODLG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_UNDODLG_TITLE _("Undo")
#define SYMBOL_UNDODLG_IDNAME ID_DIALOG_UNDO
#define SYMBOL_UNDODLG_SIZE wxDefaultSize
#define SYMBOL_UNDODLG_POSITION wxDefaultPosition
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

/*!
 * UndoDlg class declaration
 */

class UndoDlg: public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( UndoDlg )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    UndoDlg();
    UndoDlg( wxWindow* parent, iOList undoList );
    
    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_UNDODLG_IDNAME, const wxString& caption = SYMBOL_UNDODLG_TITLE, const wxPoint& pos = SYMBOL_UNDODLG_POSITION, const wxSize& size = SYMBOL_UNDODLG_SIZE, long style = SYMBOL_UNDODLG_STYLE );

    /// Destructor
    ~UndoDlg();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

////@begin UndoDlg event handler declarations

    /// wxEVT_COMMAND_LISTBOX_SELECTED event handler for ID_LISTBOX_UNDO_LIST
    void OnListboxUndoListSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_UNDO_CANCEL
    void OnButtonUndoCancelClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_UNDO_DELETE
    void OnButtonUndoDeleteClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_UNDO_UNDO
    void OnButtonUndoUndoClick( wxCommandEvent& event );

////@end UndoDlg event handler declarations

////@begin UndoDlg member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end UndoDlg member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin UndoDlg member variables
    wxListBox* m_List;
    wxButton* m_Cancel;
    wxButton* m_Delete;
    wxButton* m_Undo;
////@end UndoDlg member variables
};

#endif
    // _UNDODLG_H_
