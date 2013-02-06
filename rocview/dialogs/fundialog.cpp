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
#if defined(__GNUG__) && !defined(__APPLE__)
#pragma implementation "fundialog.h"
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

#include "fundialog.h"

#include "rocs/public/strtok.h"
#include "rocs/public/trace.h"
#include "rocview/public/guiapp.h"
#include "rocrail/wrapper/public/Plan.h"
#include "rocrail/wrapper/public/Block.h"
#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/FunDef.h"

////@begin XPM images
////@end XPM images

/*!
 * FunctionDialog type definition
 */

/*!
 * FunctionDialog event table definition
 */

BEGIN_EVENT_TABLE( FunctionDialog, wxDialog )

////@begin FunctionDialog event table entries
    EVT_BUTTON( wxID_CANCEL, FunctionDialog::OnCancelClick )

    EVT_BUTTON( wxID_OK, FunctionDialog::OnOkClick )

////@end FunctionDialog event table entries

END_EVENT_TABLE()

/*!
 * FunctionDialog constructors
 */

FunctionDialog::FunctionDialog( wxWindow* parent, iONode p_Props, int p_Function )
{
  m_TabAlign = wxGetApp().getTabAlign();
  Create(parent, -1, wxGetApp().getMsg( "functions" ) );
  m_Props = p_Props;
  m_iFunction = p_Function;
  initLabels();
  InitValues();

  // After filling the listbox we must trigger this event again.
  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);
  GetSizer()->Layout();
}


void FunctionDialog::initLabels() {
  m_Label_OnEvent->SetLabel( wxGetApp().getMsg( "on_event" ) );
  m_Label_OffEvent->SetLabel( wxGetApp().getMsg( "off_event" ) );
  m_Label_Block->SetLabel( wxGetApp().getMsg( "block" ) );
  m_Label_Event->SetLabel( wxGetApp().getMsg( "event" ) );

  // Buttons
  m_OK->SetLabel( wxGetApp().getMsg( "ok" ) );
  m_Cancel->SetLabel( wxGetApp().getMsg( "cancel" ) );
}


void FunctionDialog::InitValues() {
  m_FunDef = NULL;
  iONode model = wxGetApp().getModel();

  m_FunDef = wLoc.getfundef( m_Props );
  while( m_FunDef != NULL ) {
    if( wFunDef.getfn( m_FunDef ) == m_iFunction )
      break;
    m_FunDef = wLoc.nextfundef( m_Props, m_FunDef );
  };

  if( m_FunDef == NULL ) {
    m_FunDef = NodeOp.inst( wFunDef.name(), m_Props, ELEMENT_NODE );
    wFunDef.setfn( m_FunDef, m_iFunction );
    NodeOp.addChild( m_Props, m_FunDef );
    m_bNewFunDef = true;
    TraceOp.trc( "fundlg", TRCLEVEL_INFO, __LINE__, 9999, "FunDef created for f%d", m_iFunction );
  }
  else
    TraceOp.trc( "fundlg", TRCLEVEL_INFO, __LINE__, 9999, "FunDef found for f%d", m_iFunction );

  //m_BlockOn->Append( "" );
  //m_BlockOff->Append( "" );

  if( model != NULL ) {
    iONode bklist = wPlan.getbklist( model );
    if( bklist != NULL ) {
      int cnt = NodeOp.getChildCnt( bklist );
      for( int i = 0; i < cnt; i++ ) {
        iONode bk = NodeOp.getChild( bklist, i );
        const char* id = wBlock.getid( bk );
        if( id != NULL && StrOp.len( id ) > 0 ) {
          m_BlockOn->Append( wxString(id,wxConvUTF8) );
          m_BlockOff->Append( wxString(id,wxConvUTF8) );
        }
      }
    }
  }

  iOStrTok  onblocks = StrTokOp.inst( wFunDef.getonblockid ( m_FunDef ), ',' );
  while( StrTokOp.hasMoreTokens( onblocks ) ) {
    const char* tok = StrTokOp.nextToken( onblocks );
    m_BlockOn->SetStringSelection( wxString(tok,wxConvUTF8) );
    TraceOp.trc( "fundlg", TRCLEVEL_INFO, __LINE__, 9999, "SetSelection to %s", tok );
  };
  StrTokOp.base.del( onblocks );

  iOStrTok offblocks = StrTokOp.inst( wFunDef.getoffblockid( m_FunDef ), ',' );
  while( StrTokOp.hasMoreTokens( offblocks ) ) {
    const char* tok = StrTokOp.nextToken( offblocks );
    m_BlockOff->SetStringSelection( wxString(tok,wxConvUTF8) );
    TraceOp.trc( "fundlg", TRCLEVEL_INFO, __LINE__, 9999, "SetSelection to %s", tok );
  };
  StrTokOp.base.del( offblocks );

  m_BlockOnEvent->Append( wxString("",wxConvUTF8) );
  m_BlockOnEvent->Append( wxString(wFunDef.enter_block,wxConvUTF8) );
  m_BlockOnEvent->Append( wxString(wFunDef.in_block,wxConvUTF8) );
  m_BlockOnEvent->Append( wxString(wFunDef.exit_block,wxConvUTF8) );
  m_BlockOnEvent->Append( wxString(wFunDef.run,wxConvUTF8) );
  m_BlockOnEvent->Append( wxString(wFunDef.stall,wxConvUTF8) );

  m_BlockOffEvent->Append( wxString("",wxConvUTF8) );
  m_BlockOffEvent->Append( wxString(wFunDef.enter_block,wxConvUTF8) );
  m_BlockOffEvent->Append( wxString(wFunDef.in_block,wxConvUTF8) );
  m_BlockOffEvent->Append( wxString(wFunDef.exit_block,wxConvUTF8) );
  m_BlockOffEvent->Append( wxString(wFunDef.run,wxConvUTF8) );
  m_BlockOffEvent->Append( wxString(wFunDef.stall,wxConvUTF8) );

  m_BlockOnEvent->SetValue( wxString(wFunDef.getonevent(m_FunDef),wxConvUTF8) );
  m_BlockOffEvent->SetValue( wxString(wFunDef.getoffevent(m_FunDef),wxConvUTF8) );
}


void FunctionDialog::Evaluate() {
  if( m_Props == NULL )
    return;

  wxArrayInt ai;
  int cnt = m_BlockOn->GetSelections(ai);
  if( cnt > 0 ) {
    char* onids = StrOp.dup("");
    for( int i = 0; i < cnt; i++ ) {
      int idx = ai.Item(i);
      char* p = onids;
      if( StrOp.len(onids) > 0 )
        onids = StrOp.fmt( "%s,%s", onids, (const char*)m_BlockOn->GetString(idx).mb_str(wxConvUTF8) );
      else
        onids = StrOp.fmt( "%s", (const char*)m_BlockOn->GetString(idx).mb_str(wxConvUTF8) );
      StrOp.free( p );
    }
    wFunDef.setonblockid( m_FunDef, onids );
    StrOp.free( onids );
  }
  else
    wFunDef.setonblockid( m_FunDef, "" );

  cnt = m_BlockOff->GetSelections(ai);
  if( cnt > 0 ) {
    char* offids = StrOp.dup("");
    for( int i = 0; i < cnt; i++ ) {
      int idx = ai.Item(i);
      char* p = offids;
      if( StrOp.len(offids) > 0 )
        offids = StrOp.fmt( "%s,%s", offids, (const char*)m_BlockOff->GetString(idx).mb_str(wxConvUTF8) );
      else
        offids = StrOp.fmt( "%s", (const char*)m_BlockOff->GetString(idx).mb_str(wxConvUTF8) );
      StrOp.free( p );
    }
    wFunDef.setoffblockid( m_FunDef, offids );
    StrOp.free( offids );
  }
  else
    wFunDef.setoffblockid( m_FunDef, "" );

  wFunDef.setonevent(m_FunDef, m_BlockOnEvent->GetValue().mb_str(wxConvUTF8) );
  if( StrOp.len(wFunDef.getonevent(m_FunDef)) == 0 )
    wFunDef.setonblockid( m_FunDef, "" );

  wFunDef.setoffevent(m_FunDef, m_BlockOffEvent->GetValue().mb_str(wxConvUTF8) );
  if( StrOp.len(wFunDef.getoffevent(m_FunDef)) == 0 )
    wFunDef.setoffblockid( m_FunDef, "" );
}



FunctionDialog::FunctionDialog( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Create(parent, id, caption, pos, size, style);
}

/*!
 * FunctionDialog creator
 */

bool FunctionDialog::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin FunctionDialog member initialisation
    m_Label_OnEvent = NULL;
    m_Label_OffEvent = NULL;
    m_Label_Block = NULL;
    m_BlockOn = NULL;
    m_BlockOff = NULL;
    m_Label_Event = NULL;
    m_BlockOnEvent = NULL;
    m_BlockOffEvent = NULL;
    m_Cancel = NULL;
    m_OK = NULL;
////@end FunctionDialog member initialisation

////@begin FunctionDialog creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end FunctionDialog creation
    return true;
}

/*!
 * Control creation for FunctionDialog
 */

void FunctionDialog::CreateControls()
{
////@begin FunctionDialog content construction
    FunctionDialog* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    wxFlexGridSizer* itemFlexGridSizer3 = new wxFlexGridSizer(3, 3, 0, 0);
    itemFlexGridSizer3->AddGrowableCol(1);
    itemFlexGridSizer3->AddGrowableCol(2);
    itemBoxSizer2->Add(itemFlexGridSizer3, 1, wxGROW|wxALL|wxADJUST_MINSIZE, 5);

    wxStaticText* itemStaticText4 = new wxStaticText( itemDialog1, wxID_STATIC_FN_DUMMY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer3->Add(itemStaticText4, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    m_Label_OnEvent = new wxStaticText( itemDialog1, wxID_STATIC_ONEVENT, _("On event"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer3->Add(m_Label_OnEvent, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    m_Label_OffEvent = new wxStaticText( itemDialog1, wxID_STATIC_OFFEVENT, _("Off event"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer3->Add(m_Label_OffEvent, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    m_Label_Block = new wxStaticText( itemDialog1, wxID_STATIC_FN_BLOCK, _("Block:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer3->Add(m_Label_Block, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxArrayString m_BlockOnStrings;
    m_BlockOn = new wxListBox( itemDialog1, ID_LISTBOX_BLOCK_ON, wxDefaultPosition, wxSize(-1, 120), m_BlockOnStrings, wxLB_MULTIPLE|wxLB_ALWAYS_SB|wxSUNKEN_BORDER );
    itemFlexGridSizer3->Add(m_BlockOn, 0, wxALIGN_CENTER_HORIZONTAL|wxGROW|wxALL, 5);

    wxArrayString m_BlockOffStrings;
    m_BlockOff = new wxListBox( itemDialog1, ID_LISTBOX_BLOCK_OFF, wxDefaultPosition, wxSize(-1, 120), m_BlockOffStrings, wxLB_MULTIPLE|wxLB_ALWAYS_SB|wxSUNKEN_BORDER );
    itemFlexGridSizer3->Add(m_BlockOff, 0, wxALIGN_CENTER_HORIZONTAL|wxGROW|wxALL, 5);

    m_Label_Event = new wxStaticText( itemDialog1, wxID_STATIC_EVENT, _("Event:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer3->Add(m_Label_Event, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxArrayString m_BlockOnEventStrings;
    m_BlockOnEvent = new wxComboBox( itemDialog1, ID_COMBOBOX_BLOCKON_EVENT, _T(""), wxDefaultPosition, wxDefaultSize, m_BlockOnEventStrings, wxCB_READONLY );
    itemFlexGridSizer3->Add(m_BlockOnEvent, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxArrayString m_BlockOffEventStrings;
    m_BlockOffEvent = new wxComboBox( itemDialog1, ID_COMBOBOX_BLOCKOFF_EVENT, _T(""), wxDefaultPosition, wxDefaultSize, m_BlockOffEventStrings, wxCB_READONLY );
    itemFlexGridSizer3->Add(m_BlockOffEvent, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStdDialogButtonSizer* itemStdDialogButtonSizer13 = new wxStdDialogButtonSizer;

    itemBoxSizer2->Add(itemStdDialogButtonSizer13, 0, wxALIGN_RIGHT|wxALL, 5);
    m_Cancel = new wxButton( itemDialog1, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer13->AddButton(m_Cancel);

    m_OK = new wxButton( itemDialog1, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    m_OK->SetDefault();
    itemStdDialogButtonSizer13->AddButton(m_OK);

    itemStdDialogButtonSizer13->Realize();

////@end FunctionDialog content construction
}

/*!
 * Should we show tooltips?
 */

bool FunctionDialog::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap FunctionDialog::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin FunctionDialog bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end FunctionDialog bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon FunctionDialog::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin FunctionDialog icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end FunctionDialog icon retrieval
}
/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_OK
 */

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
 */

void FunctionDialog::OnCancelClick( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL in FunctionDialog.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL in FunctionDialog.
  EndModal( 0 );
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
 */

void FunctionDialog::OnOkClick( wxCommandEvent& event )
{
  Evaluate();
  EndModal( wxID_OK );
}


