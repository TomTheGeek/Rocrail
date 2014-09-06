/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2014 Rob Versluis, Rocrail.net

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

#include "variabledlg.h"

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/defs.h"
#endif

#include "rocview/public/guiapp.h"
#include "rocrail/wrapper/public/ModelCmd.h"
#include "rocrail/wrapper/public/Plan.h"
#include "rocrail/wrapper/public/Item.h"
#include "rocrail/wrapper/public/Variable.h"
#include "rocrail/wrapper/public/VariableList.h"


VariableDlg::VariableDlg( wxWindow* parent ):VariableDlgGen( parent )
{
  m_Props = NULL;
  initLabels();
  initIndex();
}

void VariableDlg::initLabels() {
  SetTitle(wxGetApp().getMsg( "variable" ));
  m_VarBook->SetPageText( 0, wxGetApp().getMsg( "index" ) );
  m_VarBook->SetPageText( 1, wxGetApp().getMsg( "general" ) );

  m_VarList->InsertColumn(0, wxGetApp().getMsg( "id" ), wxLIST_FORMAT_LEFT );
  m_VarList->InsertColumn(1, wxGetApp().getMsg( "type" ), wxLIST_FORMAT_LEFT );
  m_VarList->InsertColumn(2, wxGetApp().getMsg( "text" ), wxLIST_FORMAT_LEFT );
  m_VarList->InsertColumn(3, wxGetApp().getMsg( "value" ), wxLIST_FORMAT_LEFT );

  m_labID->SetLabel( wxGetApp().getMsg( "id" ) );
  m_Type->SetLabel( wxGetApp().getMsg( "type" ) );
  m_Type->SetString( 0, wxGetApp().getMsg( "text" ) );
  m_Type->SetString( 1, wxGetApp().getMsg( "number" ) );
  m_Type->SetString( 2, wxGetApp().getMsg( "timer" ) );
  m_Type->SetString( 3, wxGetApp().getMsg( "random" ) );
  m_labMin->SetLabel( wxGetApp().getMsg( "min" ) );
  m_labMax->SetLabel( wxGetApp().getMsg( "max" ) );
  m_labText->SetLabel( wxGetApp().getMsg( "text" ) );
  m_labValue->SetLabel( wxGetApp().getMsg( "value" ) );
}

void VariableDlg::onListSelected( wxListEvent& event ) {
  int index = event.GetIndex();
  m_Props = (iONode)m_VarList->GetItemData(index);
  if( m_Props != NULL )
    initValues();
  else
    TraceOp.trc( "vardlg", TRCLEVEL_INFO, __LINE__, 9999, "no selection..." );
}


static int __sortID(obj* _a, obj* _b)
{
    iONode a = (iONode)*_a;
    iONode b = (iONode)*_b;
    const char* idA = wItem.getid( a );
    const char* idB = wItem.getid( b );
    return strcmp( idA, idB );
}

void VariableDlg::initIndex() {
  SetTitle(wxGetApp().getMsg( "variable" ));

  m_VarList->DeleteAllItems();

  iONode model = wxGetApp().getModel();
  if( model != NULL ) {
    iONode varlist = wPlan.getvrlist( model );
    if( varlist != NULL ) {
      iOList list = ListOp.inst();
      int cnt = NodeOp.getChildCnt( varlist );
      for( int i = 0; i < cnt; i++ ) {
        iONode var = NodeOp.getChild( varlist, i );
        const char* id = wVariable.getid( var );
        if( id != NULL ) {
          ListOp.add(list, (obj)var);
        }
      }

      ListOp.sort(list, &__sortID);

      cnt = ListOp.size( list );
      for( int i = 0; i < cnt; i++ ) {
        iONode var = (iONode)ListOp.get( list, i );
        m_VarList->InsertItem( i, wxString( wVariable.getid( var ), wxConvUTF8) );
        m_VarList->SetItem( i, 1, wxString::Format(wxT("%d"), wVariable.gettype( var )) );
        m_VarList->SetItem( i, 2, wxString( wVariable.gettext( var ), wxConvUTF8) );
        m_VarList->SetItem( i, 3, wxString::Format(wxT("%d"), wVariable.getvalue( var )) );
        m_VarList->SetItemPtrData(i, (wxUIntPtr)var);
      }

      // resize
      for( int n = 0; n < 4; n++ ) {
        m_VarList->SetColumnWidth(n, wxLIST_AUTOSIZE_USEHEADER);
        int autoheadersize = m_VarList->GetColumnWidth(n);
        m_VarList->SetColumnWidth(n, wxLIST_AUTOSIZE);
        int autosize = m_VarList->GetColumnWidth(n);
        if(autoheadersize > autosize )
          m_VarList->SetColumnWidth(n, wxLIST_AUTOSIZE_USEHEADER);
        else if( autosize > 120 )
          m_VarList->SetColumnWidth(n, autoheadersize > 120 ? autoheadersize:120);
      }

      /* clean up the temp. list */
      ListOp.base.del(list);

      if( m_Props != NULL ) {
        char* title = StrOp.fmt( "%s %s", (const char*)wxGetApp().getMsg("variable").mb_str(wxConvUTF8), wVariable.getid( m_Props ) );
        SetTitle( wxString(title,wxConvUTF8) );
        StrOp.free( title );

        setSelection(wVariable.getid( m_Props ));

      }
      else if(m_VarList->GetItemCount() > 0 ) {
        m_VarList->SetItemState(0, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
        m_Props = (iONode)m_VarList->GetItemData(0);

      }

    }
  }

}


void VariableDlg::setSelection(const char* ID) {
  int size = m_VarList->GetItemCount();
  for( int index = 0; index < size; index++ ) {
    iONode node = (iONode)m_VarList->GetItemData(index);
    if( StrOp.equals( ID, wVariable.getid(node) ) ) {
      m_VarList->SetItemState(index, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
      break;
    }
  }

}


bool VariableDlg::evaluate() {
  if( m_Props == NULL )
    return false;

  TraceOp.trc( "vardlg", TRCLEVEL_INFO, __LINE__, 9999, "Evaluate %s", wVariable.getid( m_Props ) );

  if( m_ID->GetValue().Len() == 0 ) {
    wxMessageDialog( this, wxGetApp().getMsg("invalidid"), _T("Rocrail"), wxOK | wxICON_ERROR ).ShowModal();
    m_ID->SetValue( wxString(wVariable.getid( m_Props ),wxConvUTF8) );
    return false;
  }
  // evaluate General
  wVariable.setid( m_Props, m_ID->GetValue().mb_str(wxConvUTF8) );
  wVariable.settype( m_Props, m_Type->GetSelection() );
  wVariable.setmin( m_Props, m_MinValue->GetValue() );
  wVariable.setmax( m_Props, m_MaxValue->GetValue() );
  wVariable.settext( m_Props, m_Text->GetValue().mb_str(wxConvUTF8) );
  wVariable.setvalue( m_Props, m_Value->GetValue() );
  return true;
}


int VariableDlg::findID( const char* ID ) {
  int size = m_VarList->GetItemCount();
  for( int index = 0; index < size; index++ ) {
    iONode node = (iONode)m_VarList->GetItemData(index);
    if( StrOp.equals( ID, wVariable.getid(node) ) ) {
      return index;
    }
  }
  return wxNOT_FOUND;
}


void VariableDlg::initValues() {
  if( m_Props == NULL ) {
    TraceOp.trc( "vardlg", TRCLEVEL_DEBUG, __LINE__, 9999, "no var selected" );
    return;
  }
  char* title = StrOp.fmt( "%s %s", (const char*)wxGetApp().getMsg("variable").mb_str(wxConvUTF8), wVariable.getid( m_Props ) );
  SetTitle( wxString(title,wxConvUTF8) );
  StrOp.free( title );

  TraceOp.trc( "vardlg", TRCLEVEL_INFO, __LINE__, 9999, "initValues for car [%s]", wVariable.getid( m_Props ) );
  m_ID->SetValue( wxString(wVariable.getid( m_Props ),wxConvUTF8) );
  m_Type->SetSelection(wVariable.gettype( m_Props ));
  m_MinValue->SetValue( wVariable.getmin( m_Props ) );
  m_MaxValue->SetValue( wVariable.getmax( m_Props ) );
  m_Text->SetValue( wxString(wVariable.gettext( m_Props ),wxConvUTF8) );
  m_Value->SetValue( wVariable.getvalue( m_Props ) );
}


void VariableDlg::onNew( wxCommandEvent& event )
{
  int i = findID("NEW");
  if( i == wxNOT_FOUND ) {
    iONode model = wxGetApp().getModel();
    if( model != NULL ) {
      iONode varlist = wPlan.getvrlist( model );
      if( varlist == NULL ) {
        varlist = NodeOp.inst( wVariableList.name(), model, ELEMENT_NODE );
        NodeOp.addChild( model, varlist );
      }
      if( varlist != NULL ) {
        iONode var = NodeOp.inst( wVariable.name(), varlist, ELEMENT_NODE );
        NodeOp.addChild( varlist, var );
        wVariable.setid( var, "NEW" );
        m_Props = var;
        initValues();
        initIndex();
        setSelection(wVariable.getid( var ));
      }
    }
  }
}


void VariableDlg::onDelete( wxCommandEvent& event )
{
  if( m_Props == NULL )
    return;

  int action = wxMessageDialog( this, wxGetApp().getMsg("removewarning"), _T("Rocrail"), wxYES_NO | wxICON_EXCLAMATION ).ShowModal();
  if( action == wxID_NO )
    return;

  wxGetApp().pushUndoItem( (iONode)NodeOp.base.clone( m_Props ) );
  /* Notify RocRail. */
  iONode cmd = NodeOp.inst( wModelCmd.name(), NULL, ELEMENT_NODE );
  wModelCmd.setcmd( cmd, wModelCmd.remove );
  NodeOp.addChild( cmd, (iONode)m_Props->base.clone( m_Props ) );
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);

  iONode model = wxGetApp().getModel();
  if( model != NULL ) {
    iONode varlist = wPlan.getvrlist( model );
    if( varlist != NULL ) {
      NodeOp.removeChild( varlist, m_Props );
      m_Props = NULL;
    }
  }

  initIndex();
}


void VariableDlg::onApply( wxCommandEvent& event )
{
  if( m_Props == NULL )
    return;

  if( !evaluate() )
    return;

  if( !wxGetApp().isStayOffline() ) {
    /* Notify RocRail. */
    iONode cmd = NodeOp.inst( wModelCmd.name(), NULL, ELEMENT_NODE );
    wModelCmd.setcmd( cmd, wModelCmd.modify );
    NodeOp.addChild( cmd, (iONode)m_Props->base.clone( m_Props ) );
    wxGetApp().sendToRocrail( cmd );
    cmd->base.del(cmd);
  }
  else {
    wxGetApp().setLocalModelModified(true);
  }
  initIndex();
}


void VariableDlg::onCancel( wxCommandEvent& event )
{
  EndModal( 0 );
}


void VariableDlg::onClose( wxCloseEvent& event ) {
  EndModal( 0 );
}


void VariableDlg::onHelp( wxCommandEvent& event )
{
  wxGetApp().openLink( "variable" );
}


void VariableDlg::onOK( wxCommandEvent& event )
{
  onApply(event);
  EndModal( wxID_OK );
}
