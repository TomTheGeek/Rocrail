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
#ifndef _TTTRACKDIALOG_H_
#define _TTTRACKDIALOG_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "tttrackdialog.cpp"
#endif

/*!
 * Includes
 */

////@begin includes
#include "wx/spinctrl.h"
////@end includes

#include "rocs/public/node.h"

/*!
 * Forward declarations
 */

////@begin forward declarations
class wxSpinCtrl;
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_DIALOG_TTTRACK 10134
#define wxID_STATIC_TTT_NR 10140
#define ID_SPINCTRL 10327
#define wxID_STATIC_TTT_DESC 10141
#define ID_TEXTCTRL_TTT_DESC 10136
#define wxID_STATIC_TTT_POSFB 10142
#define ID_COMBOBOX_TTT_POSFB 10137
#define SYMBOL_TTTRACKDIALOG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_TTTRACKDIALOG_TITLE _("TTTrackDialog")
#define SYMBOL_TTTRACKDIALOG_IDNAME ID_DIALOG_TTTRACK
#define SYMBOL_TTTRACKDIALOG_SIZE wxDefaultSize
#define SYMBOL_TTTRACKDIALOG_POSITION wxDefaultPosition
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
 * TTTrackDialog class declaration
 */

class TTTrackDialog: public wxDialog
{
    DECLARE_DYNAMIC_CLASS( TTTrackDialog )
    DECLARE_EVENT_TABLE()

  void initLabels();
  void initValues();
  void evaluate();

public:
    /// Constructors
    TTTrackDialog( wxWindow* parent, iONode p_Props);
    TTTrackDialog( );
    TTTrackDialog( wxWindow* parent, wxWindowID id = SYMBOL_TTTRACKDIALOG_IDNAME, const wxString& caption = SYMBOL_TTTRACKDIALOG_TITLE, const wxPoint& pos = SYMBOL_TTTRACKDIALOG_POSITION, const wxSize& size = SYMBOL_TTTRACKDIALOG_SIZE, long style = SYMBOL_TTTRACKDIALOG_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_TTTRACKDIALOG_IDNAME, const wxString& caption = SYMBOL_TTTRACKDIALOG_TITLE, const wxPoint& pos = SYMBOL_TTTRACKDIALOG_POSITION, const wxSize& size = SYMBOL_TTTRACKDIALOG_SIZE, long style = SYMBOL_TTTRACKDIALOG_STYLE );

    /// Creates the controls and sizers
    void CreateControls();

////@begin TTTrackDialog event handler declarations

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
    void OnOkClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
    void OnCancelClick( wxCommandEvent& event );

////@end TTTrackDialog event handler declarations

////@begin TTTrackDialog member function declarations

    iONode GetProps() const { return m_Props ; }
    void SetProps(iONode value) { m_Props = value ; }

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end TTTrackDialog member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin TTTrackDialog member variables
    wxStaticText* m_LabelTrackNr;
    wxSpinCtrl* m_TrackNr;
    wxStaticText* m_labDecoder;
    wxSpinCtrl* m_DecTrackNr;
    wxStaticText* m_LabelDesc;
    wxTextCtrl* m_Desc;
    wxStaticText* m_LabelPosFB;
    wxComboBox* m_PosFB;
    wxButton* m_OK;
    wxButton* m_Cancel;
    iONode m_Props;
////@end TTTrackDialog member variables
};

#endif
    // _TTTRACKDIALOG_H_
