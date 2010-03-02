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
#ifndef _SCHEDULEDIALOG_H_
#define _SCHEDULEDIALOG_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "scheduledialog.h"
#endif

/*!
 * Includes
 */

////@begin includes
#include "wx/notebook.h"
#include "wx/spinctrl.h"
#include "wx/statline.h"
#include "wx/grid.h"
////@end includes

#include "basedlg.h"

#include "rocs/public/node.h"
#include "rocs/public/list.h"

/*!
 * Forward declarations
 */

////@begin forward declarations
class wxNotebook;
class wxSpinCtrl;
class wxGrid;
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_DIALOG_SCHEDULE 10290
#define ID_NOTEBOOK_SCHEDULE 10291
#define ID_DIALOG_SCHEDULE_INDEX 10292
#define ID_LISTBOX_SCHEDULE_LIST 10294
#define ID_CHECKBOX_SCHEDULES_SHOW_ALL 10070
#define ID_BUTTON_SCHEDULE_NEW 10295
#define ID_BUTTON_SCHEDULE_DELETE 10296
#define ID_BUTTON_SC_DOC 10353
#define ID_PANEL_SCHEDULE_DESTINATIONS 10293
#define wxID_STATIC_SCHEDULE_ID 10308
#define ID_TEXTCTRL_SCHEDULE_ID 10307
#define ID_SCHEDULE_FROMHOUR 10379
#define ID_SC_TIMEPROCESSING 10204
#define ID_GRID_SCHEDULE 10000
#define ID_STATICTEXT_SCHEDULE_FROM 10001
#define ID_STATICTEXT_SCHEDULE_TO 10002
#define ID_COMBOBOX_SCHEDULE_FROM_LOCATION 10005
#define ID_COMBOBOX_SCHEDULE_TO_LOCATION 10006
#define wxID_BUTTON_SCHEDULE_ADD_LOCATION 10243
#define wxID_BUTTON_SCHEDULE_ADD_BLOCK 10274
#define wxID_BUTTON_SCHEDULE_REMOVE_DESTINATION 10297
#define wxID_BUTTON_SCHEDULE_MODIFY_DESTINATION 10330
#define wxID_BUTTON_SCHEDULE_ENTRY_ACTIONS 10343
#define ID_PANEL_SCHEDULES_ACTIONS 10079
#define ID_SCHEDULE_ACTIONS 10209
#define SYMBOL_SCHEDULEDIALOG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_SCHEDULEDIALOG_TITLE _("Schedule")
#define SYMBOL_SCHEDULEDIALOG_IDNAME ID_DIALOG_SCHEDULE
#define SYMBOL_SCHEDULEDIALOG_SIZE wxDefaultSize
#define SYMBOL_SCHEDULEDIALOG_POSITION wxDefaultPosition
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

/*!
 * ScheduleDialog class declaration
 */

class ScheduleDialog: public wxDialog, public BaseDialog
{
    DECLARE_DYNAMIC_CLASS( ScheduleDialog )
    DECLARE_EVENT_TABLE()

  void initLabels();
  void initIndex();
  void initSchedule();
  void initEntry( int row );
  bool evaluate();
  int m_TabAlign;
  void initBlockCombo();
  void initLocationCombo();
  void initOutputList();
  void initLocIDList();
  void initActions();



public:
    /// Constructors
    ScheduleDialog( );
    ~ScheduleDialog( );
    ScheduleDialog( wxWindow* parent, iONode p_Props, bool save=true, const char* startblock=NULL );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_SCHEDULEDIALOG_IDNAME, const wxString& caption = SYMBOL_SCHEDULEDIALOG_TITLE, const wxPoint& pos = SYMBOL_SCHEDULEDIALOG_POSITION, const wxSize& size = SYMBOL_SCHEDULEDIALOG_SIZE, long style = SYMBOL_SCHEDULEDIALOG_STYLE );

    /// Creates the controls and sizers
    void CreateControls();

    iONode getProperties(){ return m_Props;}

////@begin ScheduleDialog event handler declarations

    /// wxEVT_COMMAND_LISTBOX_SELECTED event handler for ID_LISTBOX_SCHEDULE_LIST
    void OnListboxScheduleListSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CHECKBOX_SCHEDULES_SHOW_ALL
    void OnCheckboxSchedulesShowAllClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_SCHEDULE_NEW
    void OnButtonScheduleNewClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_SCHEDULE_DELETE
    void OnButtonScheduleDeleteClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_SC_DOC
    void OnButtonScDocClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_TEXT_UPDATED event handler for ID_TEXTCTRL_SCHEDULE_ID
    void OnTextctrlScheduleIdUpdated( wxCommandEvent& event );

    /// wxEVT_COMMAND_SPINCTRL_UPDATED event handler for ID_SCHEDULE_FROMHOUR
    void OnScheduleFromhourUpdated( wxSpinEvent& event );

    /// wxEVT_COMMAND_RADIOBOX_SELECTED event handler for ID_SC_TIMEPROCESSING
    void OnScTimeprocessingSelected( wxCommandEvent& event );

    /// wxEVT_GRID_CELL_LEFT_CLICK event handler for ID_GRID_SCHEDULE
    void OnCellLeftClick( wxGridEvent& event );

    /// wxEVT_GRID_CELL_RIGHT_CLICK event handler for ID_GRID_SCHEDULE
    void OnCellRightClick( wxGridEvent& event );

    /// wxEVT_GRID_LABEL_LEFT_CLICK event handler for ID_GRID_SCHEDULE
    void OnLabelLeftClick( wxGridEvent& event );

    /// wxEVT_GRID_LABEL_RIGHT_CLICK event handler for ID_GRID_SCHEDULE
    void OnLabelRightClick( wxGridEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_BUTTON_SCHEDULE_ADD_LOCATION
    void OnAddLocationClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_BUTTON_SCHEDULE_ADD_BLOCK
    void OnAddBlockClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_BUTTON_SCHEDULE_REMOVE_DESTINATION
    void OnRemoveDestinationClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_BUTTON_SCHEDULE_MODIFY_DESTINATION
    void OnModifyDestinationClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_BUTTON_SCHEDULE_ENTRY_ACTIONS
    void OnButtonScheduleEntryActionsClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_SCHEDULE_ACTIONS
    void OnScheduleActionsClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
    void OnOkClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
    void OnCancelClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_APPLY
    void OnApplyClick( wxCommandEvent& event );

////@end ScheduleDialog event handler declarations

////@begin ScheduleDialog member function declarations

    iONode GetProps() const { return m_Props ; }
    void SetProps(iONode value) { m_Props = value ; }

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end ScheduleDialog member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin ScheduleDialog member variables
    wxNotebook* m_NoteBook;
    wxPanel* m_Index;
    wxListBox* m_List;
    wxCheckBox* m_ShowAll;
    wxStaticText* m_labStartBlock;
    wxTextCtrl* m_StartBlockID;
    wxButton* m_New;
    wxButton* m_Delete;
    wxButton* m_Doc;
    wxPanel* m_Destinations;
    wxStaticText* m_LabelID;
    wxTextCtrl* m_ID;
    wxStaticText* m_labTimeFrame;
    wxSpinCtrl* m_TimeFrame;
    wxStaticText* m_labFromHour;
    wxSpinCtrl* m_FromHour;
    wxStaticText* m_labToHour;
    wxSpinCtrl* m_ToHour;
    wxStaticText* m_labCycle;
    wxSpinCtrl* m_Cycle;
    wxRadioBox* m_TimeProcessing;
    wxGrid* m_Entries;
    wxStaticText* m_LabelLocation;
    wxStaticText* m_LabelBlock;
    wxComboBox* m_Location;
    wxComboBox* m_Block;
    wxButton* m_AddLocation;
    wxButton* m_AddBlock;
    wxButton* m_RemoveDestination;
    wxButton* m_ModifyDestination;
    wxButton* m_EntryActions;
    wxStaticText* m_labDeparture;
    wxStaticText* m_labHour;
    wxStaticText* m_labMinute;
    wxSpinCtrl* m_Hour;
    wxSpinCtrl* m_Minute;
    wxStaticBox* m_EntryDetails;
    wxCheckBox* m_EntrySwap;
    wxPanel* m_ScheduleActions;
    wxStaticBox* m_ScheduleBox;
    wxComboBox* m_ScheduleAction;
    wxButton* m_Actions;
    wxButton* m_OK;
    wxButton* m_Cancel;
    wxButton* m_Apply;
    iONode m_Props;
////@end ScheduleDialog member variables
    iOList m_EntryPropsList;
    int m_SelectedRow;
    bool m_bSave;
    const char* m_StartBlock;
};

#endif
    // _SCHEDULEDIALOG_H_
