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
#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "undodlg.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

////@begin includes
////@end includes

#include "undodlg.h"

////@begin XPM images
////@end XPM images

#include "rocview/public/guiapp.h"
#include "rocrail/wrapper/public/Item.h"
#include "rocrail/wrapper/public/ModelCmd.h"

/*!
 * UndoDlg type definition
 */

IMPLEMENT_DYNAMIC_CLASS( UndoDlg, wxDialog )

/*!
 * UndoDlg event table definition
 */

BEGIN_EVENT_TABLE( UndoDlg, wxDialog )

////@begin UndoDlg event table entries
    EVT_LISTBOX( ID_LISTBOX_UNDO_LIST, UndoDlg::OnListboxUndoListSelected )

    EVT_BUTTON( ID_BUTTON_UNDO_CANCEL, UndoDlg::OnButtonUndoCancelClick )

    EVT_BUTTON( ID_BUTTON_UNDO_DELETE, UndoDlg::OnButtonUndoDeleteClick )

    EVT_BUTTON( ID_BUTTON_UNDO_UNDO, UndoDlg::OnButtonUndoUndoClick )

////@end UndoDlg event table entries

END_EVENT_TABLE()

/*!
 * UndoDlg constructors
 */

UndoDlg::UndoDlg()
{
    Init();
}

UndoDlg::UndoDlg( wxWindow* parent, iOList undoList )
{
  Init();
  Create(parent, -1, wxGetApp().getMsg("undo"));
  
  if( undoList != NULL ) {
    m_List->Clear();
    for( int i = 0; i < ListOp.size( undoList ); i++ ) {
      iONode item = (iONode)ListOp.get(undoList, i);
      
      char* info = StrOp.fmt( "%s [%s]", NodeOp.getName(item), wItem.getid( item ) );
      m_List->Append( wxString(info,wxConvUTF8), item );

    }
  }
  m_Delete->Enable(false);
  m_Undo->Enable(false);

  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);
  GetSizer()->Layout();
}

/*!
 * UndoDlg creator
 */

bool UndoDlg::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin UndoDlg creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end UndoDlg creation
    return true;
}

/*!
 * UndoDlg destructor
 */

UndoDlg::~UndoDlg()
{
////@begin UndoDlg destruction
////@end UndoDlg destruction
}

/*!
 * Member initialisation 
 */

void UndoDlg::Init()
{
////@begin UndoDlg member initialisation
    m_List = NULL;
    m_Cancel = NULL;
    m_Delete = NULL;
    m_Undo = NULL;
////@end UndoDlg member initialisation
}

/*!
 * Control creation for UndoDlg
 */

void UndoDlg::CreateControls()
{    
////@begin UndoDlg content construction
    UndoDlg* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    wxArrayString m_ListStrings;
    m_List = new wxListBox( itemDialog1, ID_LISTBOX_UNDO_LIST, wxDefaultPosition, wxDefaultSize, m_ListStrings, wxLB_SINGLE );
    itemBoxSizer2->Add(m_List, 1, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer4, 0, wxGROW|wxALL, 5);

    m_Cancel = new wxButton( itemDialog1, ID_BUTTON_UNDO_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer4->Add(m_Cancel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Delete = new wxButton( itemDialog1, ID_BUTTON_UNDO_DELETE, _("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer4->Add(m_Delete, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Undo = new wxButton( itemDialog1, ID_BUTTON_UNDO_UNDO, _("Undo"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Undo->SetDefault();
    itemBoxSizer4->Add(m_Undo, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

////@end UndoDlg content construction
}

/*!
 * Should we show tooltips?
 */

bool UndoDlg::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap UndoDlg::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin UndoDlg bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end UndoDlg bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon UndoDlg::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin UndoDlg icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end UndoDlg icon retrieval
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_UNDO_UNDO
 */

void UndoDlg::OnButtonUndoUndoClick( wxCommandEvent& event )
{
  int sel = m_List->GetSelection();
  if( sel != wxNOT_FOUND ) {
    iOList unduList = wxGetApp().getUndoItems();
    iONode item = (iONode)m_List->GetClientData(sel);
    m_List->Delete(sel);
    for( int i = 0; i < ListOp.size( unduList ); i++ ) {
      if( (obj)item == ListOp.get(unduList,i) ) {
        ListOp.remove(unduList,i);
        wxGetApp().getFrame()->GetToolBar()->EnableTool(ME_Undo, ListOp.size( unduList ) > 0 );
        break;
      }
    }
    
    // check if it is already a model command
    if( StrOp.equals( wModelCmd.name(), NodeOp.getName(item) ) ) {
      // TODO: make constants for the select attributes
      if( StrOp.equals( "move", wItem.getid(item) ) ) {
        // "pre_move_x"
        // loop all child to restore original position
        int cnt = NodeOp.getChildCnt(item);
        for( int i = 0; i < cnt; i++ ) {
          iONode child = NodeOp.getChild(item, i);
          wItem.setx( child, NodeOp.getInt( child, "pre_move_x", 0 ) );
          wItem.sety( child, NodeOp.getInt( child, "pre_move_y", 0 ) );
          wItem.setz( child, NodeOp.getInt( child, "pre_move_z", 0 ) );
        }
        wxGetApp().sendToRocrail( item );
        NodeOp.base.del(item);
      }
      else if( StrOp.equals( "delete", wItem.getid(item) ) ) {
        wModelCmd.setcmd( item, wModelCmd.add );
        wxGetApp().sendToRocrail( item );
        NodeOp.base.del(item);
      }
      else if( StrOp.equals( "movemod", wItem.getid(item) ) ) {
        int cnt = NodeOp.getChildCnt(item);
        for( int i = 0; i < cnt; i++ ) {
          iONode child = NodeOp.getChild(item, i);
          wItem.setx( child, NodeOp.getInt( child, "pre_move_x", 0 ) );
          wItem.sety( child, NodeOp.getInt( child, "pre_move_y", 0 ) );
        }
        wxGetApp().sendToRocrail( item );
        NodeOp.base.del(item);
      }
    
    }
    else {
      /* Notify RocRail. */
      iONode cmd = NodeOp.inst( wModelCmd.name(), NULL, ELEMENT_NODE );
      wModelCmd.setcmd( cmd, wModelCmd.add );
      NodeOp.addChild( cmd, item );
      wxGetApp().sendToRocrail( cmd );
      cmd->base.del(cmd);
    }
    
    EndModal( wxID_OK );
  }
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_UNDO_DELETE
 */

void UndoDlg::OnButtonUndoDeleteClick( wxCommandEvent& event )
{
  int sel = m_List->GetSelection();
  if( sel != wxNOT_FOUND ) {
    iOList unduList = wxGetApp().getUndoItems();
    iONode item = (iONode)m_List->GetClientData(sel);
    m_List->Delete(sel);
    for( int i = 0; i < ListOp.size( unduList ); i++ ) {
      if( (obj)item == ListOp.get(unduList,i) ) {
        ListOp.remove(unduList,i);
        wxGetApp().getFrame()->GetToolBar()->EnableTool(ME_Undo, ListOp.size( unduList ) > 0 );
        break;
      }
    }
    NodeOp.base.del(item);
    EndModal( wxID_OK );
  }
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_UNDO_CANCEL
 */

void UndoDlg::OnButtonUndoCancelClick( wxCommandEvent& event )
{
  EndModal( 0 );
}

/*!
 * wxEVT_COMMAND_LISTBOX_SELECTED event handler for ID_LISTBOX_UNDO_LIST
 */

void UndoDlg::OnListboxUndoListSelected( wxCommandEvent& event )
{
  int sel = m_List->GetSelection();
  if( sel == wxNOT_FOUND ) {
    m_Delete->Enable(false);
    m_Undo->Enable(false);
    return;
  }
  m_Delete->Enable(true);
  m_Undo->Enable(true);
}

