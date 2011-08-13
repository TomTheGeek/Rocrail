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
#ifndef _BLOCKGROUPINGDLG_H_
#define _BLOCKGROUPINGDLG_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "blockgroupingdlg.h"
#endif

/*!
 * Includes
 */

////@begin includes
#include "wx/notebook.h"
#include "wx/spinctrl.h"
////@end includes

#include "rocs/public/node.h"

/*!
 * Forward declarations
 */

////@begin forward declarations
class wxNotebook;
class wxSpinCtrl;
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_BLOCKGROUPING_DLG 10211
#define ID_NOTEBOOK_LINK 10012
#define ID_PANEL_LINK_INDEX 10005
#define ID_LISTBOX_LINK 10006
#define ID_BUTTON_LINK_NEW 10007
#define ID_BUTTON_LINK_DELETE 10008
#define ID_BUTTON_LINK_ACTIVATE 10233
#define ID_BUTTON_LINK_DEACTIVATE 10234
#define ID_PANEL_LINK_GENERAL 10009
#define wxID_STATIC_LINK_ID 10227
#define ID_TEXTCTRL_LINK_ID 10212
#define wxID_STATIC_LINK_DESC 10229
#define ID_TEXTCTRL_LINK_DESC 10228
#define ID_PANEL_LINK_PROPS 10230
#define wxID_STATIC_LINK_MAIN 10000
#define ID_COMBOBOX_LINK_MAIN 10001
#define wxID_STATIC_LINK_LINKED 10002
#define ID_LISTBOX_LINK_LIST 10003
#define ID_COMBOBOX_LINK_ADD_BLOCK 10004
#define ID_BUTTON_LINK_ADD 10010
#define ID_BUTTON_LINK_REMOVE 10011
#define ID_CHECKBOX 10013
#define ID_PANEL_LINK_CONDS 10230
#define ID_LINK_COND_LIST 10394
#define ID_BUTTON_LINK_ADD_COND 11110
#define ID_BUTTON_LINK_MOD_COND 11111
#define ID_BUTTON_LINK_DEL_COND 11112
#define SYMBOL_BLOCKGROUPINGDIALOG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_BLOCKGROUPINGDIALOG_TITLE _("Block Grouping")
#define SYMBOL_BLOCKGROUPINGDIALOG_IDNAME ID_BLOCKGROUPING_DLG
#define SYMBOL_BLOCKGROUPINGDIALOG_SIZE wxDefaultSize
#define SYMBOL_BLOCKGROUPINGDIALOG_POSITION wxDefaultPosition
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

/*!
 * BlockGroupingDialog class declaration
 */

class BlockGroupingDialog: public wxDialog
{
    DECLARE_DYNAMIC_CLASS( BlockGroupingDialog )
    DECLARE_EVENT_TABLE()

  void initLabels();
  void initIndex();
  void initValues();
  bool evaluate();
  int m_TabAlign;

public:
    /// Constructors
    BlockGroupingDialog();
    BlockGroupingDialog( wxWindow* parent, iONode props, bool save );
    BlockGroupingDialog( wxWindow* parent, wxWindowID id = SYMBOL_BLOCKGROUPINGDIALOG_IDNAME, const wxString& caption = SYMBOL_BLOCKGROUPINGDIALOG_TITLE, const wxPoint& pos = SYMBOL_BLOCKGROUPINGDIALOG_POSITION, const wxSize& size = SYMBOL_BLOCKGROUPINGDIALOG_SIZE, long style = SYMBOL_BLOCKGROUPINGDIALOG_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_BLOCKGROUPINGDIALOG_IDNAME, const wxString& caption = SYMBOL_BLOCKGROUPINGDIALOG_TITLE, const wxPoint& pos = SYMBOL_BLOCKGROUPINGDIALOG_POSITION, const wxSize& size = SYMBOL_BLOCKGROUPINGDIALOG_SIZE, long style = SYMBOL_BLOCKGROUPINGDIALOG_STYLE );

    /// Creates the controls and sizers
    void CreateControls();
    iONode getProperties(){ return m_Props;}
    void OnSelectPage( wxCommandEvent& event );

////@begin BlockGroupingDialog event handler declarations

    /// wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED event handler for ID_NOTEBOOK_LINK
    void OnNotebookLinkPageChanged( wxNotebookEvent& event );

    /// wxEVT_COMMAND_LISTBOX_SELECTED event handler for ID_LISTBOX_LINK
    void OnListboxLinkSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_LINK_NEW
    void OnButtonLinkNewClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_LINK_DELETE
    void OnButtonLinkDeleteClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_LINK_ACTIVATE
    void OnButtonLinkActivateClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_LINK_DEACTIVATE
    void OnButtonLinkDeactivateClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_COMBOBOX_SELECTED event handler for ID_COMBOBOX_LINK_MAIN
    void OnComboboxLinkMainSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_LISTBOX_SELECTED event handler for ID_LISTBOX_LINK_LIST
    void OnListboxLinkListSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_LINK_ADD
    void OnButtonLinkAddClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_LINK_REMOVE
    void OnButtonLinkRemoveClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CHECKBOX
    void OnCheckboxClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_LISTBOX_SELECTED event handler for ID_LINK_COND_LIST
    void OnLinkCondListSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_LINK_ADD_COND
    void OnButtonLinkAddCondClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_LINK_MOD_COND
    void OnButtonLinkModCondClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_LINK_DEL_COND
    void OnButtonLinkDelCondClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
    void OnCancelClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
    void OnOkClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_APPLY
    void OnApplyClick( wxCommandEvent& event );

////@end BlockGroupingDialog event handler declarations

////@begin BlockGroupingDialog member function declarations

    iONode GetProps() const { return m_Props ; }
    void SetProps(iONode value) { m_Props = value ; }

    bool GetBSave() const { return m_bSave ; }
    void SetBSave(bool value) { m_bSave = value ; }

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end BlockGroupingDialog member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin BlockGroupingDialog member variables
    wxNotebook* m_Notebook;
    wxPanel* m_IndexPanel;
    wxListBox* m_List;
    wxButton* m_New;
    wxButton* m_Delete;
    wxButton* m_Activate;
    wxButton* m_DeActivate;
    wxPanel* m_GeneralPanel;
    wxStaticText* m_LabelID;
    wxTextCtrl* m_ID;
    wxStaticText* m_LabelDesc;
    wxTextCtrl* m_Desc;
    wxPanel* m_PropertiesPanel;
    wxStaticText* m_LabelMainBlock;
    wxComboBox* m_MainBlock;
    wxStaticText* m_LabelLinked;
    wxListBox* m_BlockList;
    wxComboBox* m_AddBlockList;
    wxButton* m_Add;
    wxButton* m_Remove;
    wxCheckBox* m_Critical;
    wxCheckBox* m_AllowFollowUp;
    wxSpinCtrl* m_MaxFollowUp;
    wxPanel* m_ConditionsPanel;
    wxListBox* m_CondList;
    wxStaticText* m_labCondFrom;
    wxComboBox* m_CondFrom;
    wxStaticText* m_labCondFree;
    wxTextCtrl* m_CondFree;
    wxButton* m_CondAdd;
    wxButton* m_CondModify;
    wxButton* m_CondDelete;
    wxButton* m_Cancel;
    wxButton* m_OK;
    wxButton* m_Apply;
    iONode m_Props;
    bool m_bSave;
////@end BlockGroupingDialog member variables
};

#endif
    // _BLOCKGROUPINGDLG_H_
