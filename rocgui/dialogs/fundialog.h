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
#ifndef _FUNDIALOG_H_
#define _FUNDIALOG_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "fundialog.cpp"
#endif

/*!
 * Includes
 */

 #include "rocs/public/node.h"

////@begin includes
////@end includes

/*!
 * Forward declarations
 */

////@begin forward declarations
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_DIALOG_FUN_EVT 10048
#define wxID_STATIC_FN_DUMMY 10098
#define wxID_STATIC_ONEVENT 10002
#define wxID_STATIC_OFFEVENT 10003
#define wxID_STATIC_FN_BLOCK 10004
#define ID_LISTBOX_BLOCK_ON 10005
#define ID_LISTBOX_BLOCK_OFF 10006
#define wxID_STATIC_EVENT 10007
#define ID_COMBOBOX_BLOCKON_EVENT 10008
#define ID_COMBOBOX_BLOCKOFF_EVENT 10009
#define SYMBOL_FUNCTIONDIALOG_STYLE wxCAPTION|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_FUNCTIONDIALOG_TITLE _("Function Events")
#define SYMBOL_FUNCTIONDIALOG_IDNAME ID_DIALOG_FUN_EVT
#define SYMBOL_FUNCTIONDIALOG_SIZE wxDefaultSize
#define SYMBOL_FUNCTIONDIALOG_POSITION wxDefaultPosition
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
 * FunctionDialog class declaration
 */

class FunctionDialog: public wxDialog
{
    DECLARE_EVENT_TABLE()

  void initLabels();
  void InitValues();
  void Evaluate();
  int m_TabAlign;

public:
    /// Constructors
    FunctionDialog( wxWindow* parent, iONode p_Props, int p_Function );
    FunctionDialog( wxWindow* parent, wxWindowID id = SYMBOL_FUNCTIONDIALOG_IDNAME, const wxString& caption = SYMBOL_FUNCTIONDIALOG_TITLE, const wxPoint& pos = SYMBOL_FUNCTIONDIALOG_POSITION, const wxSize& size = SYMBOL_FUNCTIONDIALOG_SIZE, long style = SYMBOL_FUNCTIONDIALOG_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_FUNCTIONDIALOG_IDNAME, const wxString& caption = SYMBOL_FUNCTIONDIALOG_TITLE, const wxPoint& pos = SYMBOL_FUNCTIONDIALOG_POSITION, const wxSize& size = SYMBOL_FUNCTIONDIALOG_SIZE, long style = SYMBOL_FUNCTIONDIALOG_STYLE );

    /// Creates the controls and sizers
    void CreateControls();

////@begin FunctionDialog event handler declarations

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
    void OnCancelClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
    void OnOkClick( wxCommandEvent& event );

////@end FunctionDialog event handler declarations

////@begin FunctionDialog member function declarations

    iONode GetProps() const { return m_Props ; }
    void SetProps(iONode value) { m_Props = value ; }

    int GetIFunction() const { return m_iFunction ; }
    void SetIFunction(int value) { m_iFunction = value ; }

    iONode GetFunDef() const { return m_FunDef ; }
    void SetFunDef(iONode value) { m_FunDef = value ; }

    bool GetBNewFunEvent() const { return m_bNewFunDef ; }
    void SetBNewFunEvent(bool value) { m_bNewFunDef = value ; }

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end FunctionDialog member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin FunctionDialog member variables
    wxStaticText* m_Label_OnEvent;
    wxStaticText* m_Label_OffEvent;
    wxStaticText* m_Label_Block;
    wxListBox* m_BlockOn;
    wxListBox* m_BlockOff;
    wxStaticText* m_Label_Event;
    wxComboBox* m_BlockOnEvent;
    wxComboBox* m_BlockOffEvent;
    wxButton* m_Cancel;
    wxButton* m_OK;
    iONode m_Props;
    int m_iFunction;
    iONode m_FunDef;
    bool m_bNewFunDef;
////@end FunctionDialog member variables
};

#endif
    // _FUNDIALOG_H_
