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
#ifndef _PLANPANELDLG_H_
#define _PLANPANELDLG_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "planpaneldlg.cpp"
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
#define ID_DIALOG_PLANPANEL_PROPS 10145
#define ID_STATICTEXT_PP_TITLE 10000
#define ID_TEXTCTRL_PP_TITLE 10001
#define ID_STATICTEXT_PP_ZLEVEL 10002
#define ID_TEXTCTRL_PP_ZLEVEL 10003
#define SYMBOL_PLANPANELPROPS_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_PLANPANELPROPS_TITLE _("PlanPanel Properties")
#define SYMBOL_PLANPANELPROPS_IDNAME ID_DIALOG_PLANPANEL_PROPS
#define SYMBOL_PLANPANELPROPS_SIZE wxDefaultSize
#define SYMBOL_PLANPANELPROPS_POSITION wxDefaultPosition
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
 * PlanPanelProps class declaration
 */

class PlanPanelProps: public wxDialog
{
    DECLARE_DYNAMIC_CLASS( PlanPanelProps )
    DECLARE_EVENT_TABLE()

  void initLabels();
  void initValues();
  void evaluate();

public:
    /// Constructors
    PlanPanelProps( wxWindow* parent, iONode p_Props );
    PlanPanelProps( );
    PlanPanelProps( wxWindow* parent, wxWindowID id = SYMBOL_PLANPANELPROPS_IDNAME, const wxString& caption = SYMBOL_PLANPANELPROPS_TITLE, const wxPoint& pos = SYMBOL_PLANPANELPROPS_POSITION, const wxSize& size = SYMBOL_PLANPANELPROPS_SIZE, long style = SYMBOL_PLANPANELPROPS_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_PLANPANELPROPS_IDNAME, const wxString& caption = SYMBOL_PLANPANELPROPS_TITLE, const wxPoint& pos = SYMBOL_PLANPANELPROPS_POSITION, const wxSize& size = SYMBOL_PLANPANELPROPS_SIZE, long style = SYMBOL_PLANPANELPROPS_STYLE );

    /// Creates the controls and sizers
    void CreateControls();

////@begin PlanPanelProps event handler declarations

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
    void OnOkClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
    void OnCancelClick( wxCommandEvent& event );

////@end PlanPanelProps event handler declarations

////@begin PlanPanelProps member function declarations

    iONode GetProps() const { return m_Props ; }
    void SetProps(iONode value) { m_Props = value ; }

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end PlanPanelProps member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin PlanPanelProps member variables
    wxStaticText* m_LabelTitle;
    wxTextCtrl* m_Title;
    wxStaticText* m_LabelZLevel;
    wxTextCtrl* m_ZLevel;
    wxButton* m_OK;
    wxButton* m_Cancel;
    iONode m_Props;
////@end PlanPanelProps member variables
};

#endif
    // _PLANPANELDLG_H_
