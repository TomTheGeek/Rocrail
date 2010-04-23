/** ------------------------------------------------------------
  * Module:
  * Object:
  * ------------------------------------------------------------
  * $Source: /cvsroot/rojav/rocgui/dialogs/rocgui-dialogs.pjd,v $
  * $Author: robvrs $
  * $Date: 2006/02/22 14:10:57 $
  * $Revision: 1.63 $
  * $Name:  $
  */
#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "actionsctrldlg.h"
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
#include "wx/imaglist.h"
////@end includes

#include "actionsctrldlg.h"

////@begin XPM images
////@end XPM images

#include "rocrail/wrapper/public/Plan.h"
#include "rocrail/wrapper/public/Output.h"
#include "rocrail/wrapper/public/Switch.h"
#include "rocrail/wrapper/public/Signal.h"
#include "rocrail/wrapper/public/Feedback.h"
#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/ModelCmd.h"
#include "rocrail/wrapper/public/Action.h"
#include "rocrail/wrapper/public/ActionList.h"
#include "rocrail/wrapper/public/Item.h"
#include "rocrail/wrapper/public/ActionCtrl.h"
#include "rocrail/wrapper/public/ActionCond.h"

#include "rocview/public/guiapp.h"

#include "rocs/public/node.h"
#include "rocs/public/list.h"



/*!
 * ActionsCtrlDlg type definition
 */

IMPLEMENT_DYNAMIC_CLASS( ActionsCtrlDlg, wxDialog )


/*!
 * ActionsCtrlDlg event table definition
 */

BEGIN_EVENT_TABLE( ActionsCtrlDlg, wxDialog )

////@begin ActionsCtrlDlg event table entries
    EVT_LISTBOX( ID_ACTIONCTRL_LIST, ActionsCtrlDlg::OnActionctrlListSelected )

    EVT_BUTTON( ID_ACTIONCTRL_ADD, ActionsCtrlDlg::OnActionctrlAddClick )

    EVT_BUTTON( ID_ACTIONCTRL_DELETE, ActionsCtrlDlg::OnActionctrlDeleteClick )

    EVT_BUTTON( ID_ACTIONCTRL_MODIFY, ActionsCtrlDlg::OnActionctrlModifyClick )

    EVT_LISTBOX( ID_CONDITIONS, ActionsCtrlDlg::OnConditionsSelected )

    EVT_CHOICE( ID_ACTIONCTRL_COND_TYPE, ActionsCtrlDlg::OnActionctrlCondTypeSelected )

    EVT_CHOICE( ID_ACTIOINCTRL_COND_ID, ActionsCtrlDlg::OnActioinctrlCondIdSelected )

    EVT_BUTTON( ID_ACTIONCTRL_COND_ADD, ActionsCtrlDlg::OnActionctrlCondAddClick )

    EVT_BUTTON( ID_ACTIONCTRL_COND_DELETE, ActionsCtrlDlg::OnActionctrlCondDeleteClick )

    EVT_BUTTON( ID_ACTIONCTRL_COND_MODIFY, ActionsCtrlDlg::OnActionctrlCondModifyClick )

    EVT_BUTTON( wxID_OK, ActionsCtrlDlg::OnOkClick )

    EVT_BUTTON( wxID_CANCEL, ActionsCtrlDlg::OnCancelClick )

    EVT_BUTTON( wxID_APPLY, ActionsCtrlDlg::OnApplyClick )

////@end ActionsCtrlDlg event table entries

END_EVENT_TABLE()


/*!
 * ActionsCtrlDlg constructors
 */

ActionsCtrlDlg::ActionsCtrlDlg()
{
    Init();
}

ActionsCtrlDlg::ActionsCtrlDlg( wxWindow* parent, iONode node )
{
  Init();
  Create(parent);

  m_Props = node;

  initLabels();

  m_ConditionsPanel->Enable(false);

  initIndex();
  initValues();

  m_IndexPanel->GetSizer()->Layout();
  m_ConditionsPanel->GetSizer()->Layout();

  m_Notebook->Fit();

  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);
  GetSizer()->Layout();
}

void ActionsCtrlDlg::initLabels() {
  m_Notebook->SetPageText( 0, wxGetApp().getMsg( "index" ) );
  m_Notebook->SetPageText( 1, wxGetApp().getMsg( "conditions" ) );

  // Index
  m_labID->SetLabel( wxGetApp().getMsg( "id" ) );
  m_labState->SetLabel( wxGetApp().getMsg( "state" ) );

  m_Auto->SetLabel( wxGetApp().getMsg( "mode" ) );
  m_Auto->SetString( 0, wxGetApp().getMsg( "auto" ) );
  m_Auto->SetString( 1, wxGetApp().getMsg( "manual" ) );
  m_Auto->SetString( 2, wxGetApp().getMsg( "both" ) );
  m_Auto->SetSelection(2);

  m_Add->SetLabel( wxGetApp().getMsg( "add" ) );
  m_Modify->SetLabel( wxGetApp().getMsg( "modify" ) );
  m_Delete->SetLabel( wxGetApp().getMsg( "delete" ) );

  // Conditions
  m_labCondID->SetLabel( wxGetApp().getMsg( "id" ) );
  m_labCondType->SetLabel( wxGetApp().getMsg( "type" ) );
  m_labCondState->SetLabel( wxGetApp().getMsg( "state" ) );

  m_CondAdd->SetLabel( wxGetApp().getMsg( "add" ) );
  m_CondModify->SetLabel( wxGetApp().getMsg( "modify" ) );
  m_CondDelete->SetLabel( wxGetApp().getMsg( "delete" ) );

  m_CondType->Append(wxGetApp().getMsg( "turnout" ));
  m_CondType->Append(wxGetApp().getMsg( "signal" ));
  m_CondType->Append(wxGetApp().getMsg( "output" ));
  m_CondType->Append(wxGetApp().getMsg( "sensor" ));
  m_CondType->Append(wxGetApp().getMsg( "loc" ));


  // Std buttons
  m_OK->SetLabel( wxGetApp().getMsg( "ok" ) );
  m_Cancel->SetLabel( wxGetApp().getMsg( "cancel" ) );
  m_Apply->SetLabel( wxGetApp().getMsg( "apply" ) );
}


/* comparator for sorting by id: */
static int __sortID(obj* _a, obj* _b)
{
    iONode a = (iONode)*_a;
    iONode b = (iONode)*_b;
    const char* idA = wItem.getid( a );
    const char* idB = wItem.getid( b );
    return strcmp( idA, idB );
}

void ActionsCtrlDlg::initIndex() {
  TraceOp.trc( "scdlg", TRCLEVEL_INFO, __LINE__, 9999, "InitIndex" );
  m_ID->Clear();
  iONode model = wxGetApp().getModel();
  iOList list = ListOp.inst();

  if( model != NULL ) {
    iONode aclist = wPlan.getaclist( model );
    if( aclist != NULL ) {
      int cnt = NodeOp.getChildCnt( aclist );
      TraceOp.trc( "acdlg", TRCLEVEL_INFO, __LINE__, 9999, "%d actions...", cnt );
      for( int i = 0; i < cnt; i++ ) {
        iONode ac = NodeOp.getChild( aclist, i );
        const char* id = wAction.getid( ac );
        if( id != NULL ) {
          ListOp.add(list, (obj)ac);
        }
     }

      ListOp.sort(list, &__sortID);
      cnt = ListOp.size( list );
      for( int i = 0; i < cnt; i++ ) {
        iONode ac = (iONode)ListOp.get( list, i );
        const char* id = wAction.getid(ac);
        TraceOp.trc( "acctrldlg", TRCLEVEL_INFO, __LINE__, 9999, "timed action %s", id );
        m_ID->Append( wxString(id,wxConvUTF8), ac );
      }

    }
  }
  /* clean up the temp. list */
  ListOp.base.del(list);

  m_CtrlList->Clear();
  iONode actionctrl = NodeOp.findNode( m_Props, wActionCtrl.name() );
  while( actionctrl != NULL ) {
    m_CtrlList->Append( wxString(wActionCtrl.getid(actionctrl),wxConvUTF8), NodeOp.base.clone(actionctrl) );
    actionctrl = NodeOp.findNextNode( m_Props, actionctrl );
  };
  if(m_CtrlList->GetCount() > 0)
    m_CtrlList->SetSelection(0);

}


void ActionsCtrlDlg::initValues() {
  if( m_Props == NULL )
    return;

  int cursel = m_CtrlList->GetSelection();


  if( cursel != wxNOT_FOUND ) {
    iONode actionctrl = (iONode)m_CtrlList->GetClientData(cursel);
    m_ID->SetStringSelection( m_CtrlList->GetStringSelection() );

    if(wActionCtrl.isauto(actionctrl) && wActionCtrl.ismanual(actionctrl) )
      m_Auto->SetSelection(2);
    else if(wActionCtrl.isauto(actionctrl) )
      m_Auto->SetSelection(0);
    else if(wActionCtrl.ismanual(actionctrl) )
      m_Auto->SetSelection(1);

    m_State->SetValue( wxString(wActionCtrl.getstate(actionctrl),wxConvUTF8) );


    m_ConditionsPanel->Enable(true);

    m_Conditions->Clear();
    iONode actioncond = wActionCtrl.getactioncond(actionctrl);
    while( actioncond != NULL ) {
      m_Conditions->Append( wxString(wActionCond.getid(actioncond),wxConvUTF8), actioncond );
      actioncond = wActionCtrl.nextactioncond(actionctrl, actioncond);
    };

    if(m_Conditions->GetCount() > 0) {
      m_Conditions->SetSelection(0);
      initCondValues();
    }

  }
  else {
    m_ConditionsPanel->Enable(false);
  }

}


void ActionsCtrlDlg::initCondValues() {
  if( m_Props == NULL )
    return;

  int cursel = m_Conditions->GetSelection();


  if( cursel != wxNOT_FOUND ) {
    iONode actioncond = (iONode)m_Conditions->GetClientData(cursel);
    const char* type = wActionCond.gettype(actioncond);
    if( StrOp.equals( wSwitch.name(), type ) )
      m_CondType->SetSelection(0);
    else if( StrOp.equals( wSignal.name(), type ) )
      m_CondType->SetSelection(1);
    else if( StrOp.equals( wOutput.name(), type ) )
      m_CondType->SetSelection(2);
    else if( StrOp.equals( wFeedback.name(), type ) )
      m_CondType->SetSelection(3);
    else if( StrOp.equals( wLoc.name(), type ) )
      m_CondType->SetSelection(4);
    initCondIDs();
    m_CondID->SetStringSelection( m_Conditions->GetStringSelection() );
    m_CondState->SetValue( wxString(wActionCond.getstate(actioncond),wxConvUTF8) );
  }
}


/* comparator for sorting by id: */
static int __sortStr(obj* _a, obj* _b)
{
    const char* a = (const char*)*_a;
    const char* b = (const char*)*_b;
    return strcmp( a, b );
}

void ActionsCtrlDlg::initCondIDs() {
  iONode model = wxGetApp().getModel();
  iOList list = ListOp.inst();

  m_CondID->Clear();
  m_CondID->Append( _T("") );

  if( model != NULL ) {
    iONode colist = wPlan.getswlist( model );

    int typenr = m_CondType->GetSelection();
    switch( typenr ) {
      case 0: colist = wPlan.getswlist( model ); break;
      case 1: colist = wPlan.getsglist( model ); break;
      case 2: colist = wPlan.getcolist( model ); break;
      case 3: colist = wPlan.getfblist( model ); break;
      case 4: colist = wPlan.getlclist( model ); break;
    }


    if( colist != NULL ) {
      int cnt = NodeOp.getChildCnt( colist );
      for( int i = 0; i < cnt; i++ ) {
        iONode co = NodeOp.getChild( colist, i );
        const char* id = wOutput.getid( co );
        if( id != NULL ) {
          ListOp.add(list, (obj)id);
        }
      }
      ListOp.sort(list, &__sortStr);
      cnt = ListOp.size( list );
      for( int i = 0; i < cnt; i++ ) {
        const char* id = (const char*)ListOp.get( list, i );
          m_CondID->Append( wxString(id,wxConvUTF8) );
      }
    }
  }
  /* clean up the temp. list */
  ListOp.base.del(list);
}


void ActionsCtrlDlg::evaluate() {
  int cursel = m_CtrlList->GetSelection();

  if( cursel != wxNOT_FOUND ) {
    iONode node = (iONode)m_CtrlList->GetClientData(cursel);
    wActionCtrl.setid(node, m_ID->GetStringSelection().mb_str(wxConvUTF8) );

    if( m_Auto->GetSelection() == 2 ) {
      wActionCtrl.setauto(node, True );
      wActionCtrl.setmanual(node, True );
    }
    else if( m_Auto->GetSelection() == 0 ) {
      wActionCtrl.setauto(node, True );
      wActionCtrl.setmanual(node, False );
    }
    else if( m_Auto->GetSelection() == 1 ) {
      wActionCtrl.setauto(node, False );
      wActionCtrl.setmanual(node, True );
    }

    wActionCtrl.setstate(node, m_State->GetValue().mb_str(wxConvUTF8) );

    m_CtrlList->SetString(cursel, m_ID->GetStringSelection() );
  }
}


void ActionsCtrlDlg::evaluateCond() {
  int cursel = m_Conditions->GetSelection();

  if( cursel != wxNOT_FOUND ) {
    iONode node = (iONode)m_Conditions->GetClientData(cursel);
    wActionCond.setid(node, m_CondID->GetStringSelection().mb_str(wxConvUTF8) );
    wActionCond.setstate(node, m_CondState->GetValue().mb_str(wxConvUTF8) );

    int typenr = m_CondType->GetSelection();
    const char* type = wSwitch.name();
    switch( typenr ) {
      case 0: type = wSwitch.name(); break;
      case 1: type = wSignal.name(); break;
      case 2: type = wOutput.name(); break;
      case 3: type = wFeedback.name(); break;
      case 4: type = wLoc.name(); break;
    }
    wActionCond.settype(node, type);

    m_Conditions->SetString(cursel, m_CondID->GetStringSelection() );
  }
}


/**
 * Take over all changes and send to rocrail server.
 */
void ActionsCtrlDlg::apply() {
  // remove all child from original:
  iONode actionctrl = NodeOp.findNode( m_Props, wActionCtrl.name() );
  while( actionctrl != NULL ) {
    NodeOp.removeChild(m_Props, actionctrl);
    NodeOp.base.del(actionctrl);
    actionctrl = NodeOp.findNode( m_Props, wActionCtrl.name() );
  };

  // add all cloned childs to parent:
  int cnt = m_CtrlList->GetCount();
  for( int i = 0; i < cnt; i++ ) {
    iONode node = (iONode)m_CtrlList->GetClientData(i);
    NodeOp.addChild( m_Props, (iONode)NodeOp.base.clone(node) );
  }

  if( !wxGetApp().isStayOffline() ) {
    /* Notify RocRail. */
    iONode cmd = NodeOp.inst( wModelCmd.name(), NULL, ELEMENT_NODE );
    wModelCmd.setcmd( cmd, wModelCmd.modify );
    NodeOp.addChild( cmd, (iONode)m_Props->base.clone( m_Props ) );
    wxGetApp().sendToRocrail( cmd );
    cmd->base.del(cmd);
  }
}


/**
 * Remove selection.
 */
void ActionsCtrlDlg::deleteSelected() {
  int cursel = m_CtrlList->GetSelection();

  if( cursel != wxNOT_FOUND ) {
    iONode node = (iONode)m_CtrlList->GetClientData(cursel);
    NodeOp.base.del(node);
    m_CtrlList->Delete(cursel);
    m_CtrlList->SetSelection(0);
    initValues();
  }
}


void ActionsCtrlDlg::deleteSelectedCond() {
  int cursel = m_CtrlList->GetSelection();

  if( cursel == wxNOT_FOUND )
    return;

  iONode actionctrl = (iONode)m_CtrlList->GetClientData(cursel);

  int condsel = m_Conditions->GetSelection();

  if( condsel != wxNOT_FOUND ) {
    iONode node = (iONode)m_Conditions->GetClientData(condsel);
    NodeOp.removeChild(actionctrl, node);
    NodeOp.base.del(node);
    m_Conditions->Delete(condsel);
    m_Conditions->SetSelection(0);
    initCondValues();
  }
}


/**
 * Add a new entry to the list.
 */
void ActionsCtrlDlg::addActionCtrl() {
  if( m_ID->GetStringSelection().Len() == 0 )
    return;

  iONode node = NodeOp.inst( wActionCtrl.name(), NULL, ELEMENT_NODE);
  wActionCtrl.setid(node, m_ID->GetStringSelection().mb_str(wxConvUTF8) );
  m_CtrlList->Append( wxString( wActionCtrl.getid(node),wxConvUTF8 ), node );
  m_CtrlList->SetStringSelection( m_ID->GetStringSelection() );
  m_CtrlList->SetFirstItem( m_ID->GetStringSelection() );
  evaluate();
}


void ActionsCtrlDlg::addActionCond() {
  if( m_CondID->GetStringSelection().Len() == 0 )
    return;

  int cursel = m_CtrlList->GetSelection();

  if( cursel == wxNOT_FOUND )
    return;


  iONode actionctrl = (iONode)m_CtrlList->GetClientData(cursel);

  iONode node = NodeOp.inst( wActionCond.name(), actionctrl, ELEMENT_NODE);

  wActionCond.setid(node, m_CondID->GetStringSelection().mb_str(wxConvUTF8) );

  m_Conditions->Append( wxString( wActionCond.getid(node),wxConvUTF8 ), node );
  m_Conditions->SetStringSelection( m_CondID->GetStringSelection() );
  m_Conditions->SetFirstItem( m_CondID->GetStringSelection() );

  NodeOp.addChild( actionctrl, node );

  evaluateCond();

}


/**
 * Delete all cloned nodes.
 */
void ActionsCtrlDlg::cleanUp() {
  int cnt = m_CtrlList->GetCount();
  for( int i = 0; i < cnt; i++ ) {
    iONode node = (iONode)m_CtrlList->GetClientData(i);
    NodeOp.base.del(node);
    m_CtrlList->SetClientData(NULL);
  }
}


/*!
 * ActionsCtrlDlg creator
 */

bool ActionsCtrlDlg::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin ActionsCtrlDlg creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end ActionsCtrlDlg creation
    return true;
}


/*!
 * ActionsCtrlDlg destructor
 */

ActionsCtrlDlg::~ActionsCtrlDlg()
{
////@begin ActionsCtrlDlg destruction
////@end ActionsCtrlDlg destruction
}


/*!
 * Member initialisation
 */

void ActionsCtrlDlg::Init()
{
////@begin ActionsCtrlDlg member initialisation
    m_Notebook = NULL;
    m_IndexPanel = NULL;
    m_CtrlList = NULL;
    m_labID = NULL;
    m_ID = NULL;
    m_labState = NULL;
    m_State = NULL;
    m_Auto = NULL;
    m_Add = NULL;
    m_Delete = NULL;
    m_Modify = NULL;
    m_ConditionsPanel = NULL;
    m_Conditions = NULL;
    m_labCondType = NULL;
    m_CondType = NULL;
    m_labCondID = NULL;
    m_CondID = NULL;
    m_labCondState = NULL;
    m_CondState = NULL;
    m_CondAdd = NULL;
    m_CondDelete = NULL;
    m_CondModify = NULL;
    m_OK = NULL;
    m_Cancel = NULL;
    m_Apply = NULL;
////@end ActionsCtrlDlg member initialisation
}


/*!
 * Control creation for ActionsCtrlDlg
 */

void ActionsCtrlDlg::CreateControls()
{
////@begin ActionsCtrlDlg content construction
    ActionsCtrlDlg* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    m_Notebook = new wxNotebook( itemDialog1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBK_DEFAULT );

    m_IndexPanel = new wxPanel( m_Notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxVERTICAL);
    m_IndexPanel->SetSizer(itemBoxSizer5);

    wxArrayString m_CtrlListStrings;
    m_CtrlList = new wxListBox( m_IndexPanel, ID_ACTIONCTRL_LIST, wxDefaultPosition, wxSize(-1, 100), m_CtrlListStrings, wxLB_SINGLE );
    itemBoxSizer5->Add(m_CtrlList, 0, wxGROW|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer7 = new wxFlexGridSizer(2, 2, 0, 0);
    itemFlexGridSizer7->AddGrowableCol(1);
    itemBoxSizer5->Add(itemFlexGridSizer7, 0, wxGROW|wxALL, 5);
    m_labID = new wxStaticText( m_IndexPanel, wxID_ANY, _("Action ID"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer7->Add(m_labID, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxArrayString m_IDStrings;
    m_ID = new wxChoice( m_IndexPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_IDStrings, 0 );
    itemFlexGridSizer7->Add(m_ID, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labState = new wxStaticText( m_IndexPanel, wxID_ANY, _("State"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer7->Add(m_labState, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_State = new wxTextCtrl( m_IndexPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer7->Add(m_State, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer12 = new wxFlexGridSizer(2, 2, 0, 0);
    itemBoxSizer5->Add(itemFlexGridSizer12, 0, wxGROW, 5);
    wxArrayString m_AutoStrings;
    m_AutoStrings.Add(_("&Auto"));
    m_AutoStrings.Add(_("&Manual"));
    m_AutoStrings.Add(_("&Both"));
    m_Auto = new wxRadioBox( m_IndexPanel, ID_RADIOBOX, _("Mode"), wxDefaultPosition, wxDefaultSize, m_AutoStrings, 1, wxRA_SPECIFY_ROWS );
    m_Auto->SetSelection(0);
    itemFlexGridSizer12->Add(m_Auto, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer14 = new wxFlexGridSizer(1, 3, 0, 0);
    itemBoxSizer5->Add(itemFlexGridSizer14, 0, wxGROW|wxALL, 5);
    m_Add = new wxButton( m_IndexPanel, ID_ACTIONCTRL_ADD, _("Add"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer14->Add(m_Add, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Delete = new wxButton( m_IndexPanel, ID_ACTIONCTRL_DELETE, _("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer14->Add(m_Delete, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Modify = new wxButton( m_IndexPanel, ID_ACTIONCTRL_MODIFY, _("Modify"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer14->Add(m_Modify, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Notebook->AddPage(m_IndexPanel, _("Index"));

    m_ConditionsPanel = new wxPanel( m_Notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer19 = new wxBoxSizer(wxVERTICAL);
    m_ConditionsPanel->SetSizer(itemBoxSizer19);

    wxArrayString m_ConditionsStrings;
    m_Conditions = new wxListBox( m_ConditionsPanel, ID_CONDITIONS, wxDefaultPosition, wxSize(-1, 100), m_ConditionsStrings, wxLB_SINGLE );
    itemBoxSizer19->Add(m_Conditions, 0, wxGROW|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer21 = new wxFlexGridSizer(2, 2, 0, 0);
    itemFlexGridSizer21->AddGrowableCol(1);
    itemBoxSizer19->Add(itemFlexGridSizer21, 0, wxGROW|wxALL, 5);
    m_labCondType = new wxStaticText( m_ConditionsPanel, wxID_ANY, _("Type"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer21->Add(m_labCondType, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxArrayString m_CondTypeStrings;
    m_CondType = new wxChoice( m_ConditionsPanel, ID_ACTIONCTRL_COND_TYPE, wxDefaultPosition, wxDefaultSize, m_CondTypeStrings, 0 );
    itemFlexGridSizer21->Add(m_CondType, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labCondID = new wxStaticText( m_ConditionsPanel, wxID_ANY, _("ID"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer21->Add(m_labCondID, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxArrayString m_CondIDStrings;
    m_CondID = new wxChoice( m_ConditionsPanel, ID_ACTIOINCTRL_COND_ID, wxDefaultPosition, wxDefaultSize, m_CondIDStrings, 0 );
    itemFlexGridSizer21->Add(m_CondID, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labCondState = new wxStaticText( m_ConditionsPanel, wxID_ANY, _("State"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer21->Add(m_labCondState, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_CondState = new wxTextCtrl( m_ConditionsPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer21->Add(m_CondState, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer28 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer19->Add(itemBoxSizer28, 0, wxGROW|wxALL, 5);
    m_CondAdd = new wxButton( m_ConditionsPanel, ID_ACTIONCTRL_COND_ADD, _("Add"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer28->Add(m_CondAdd, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_CondDelete = new wxButton( m_ConditionsPanel, ID_ACTIONCTRL_COND_DELETE, _("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer28->Add(m_CondDelete, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_CondModify = new wxButton( m_ConditionsPanel, ID_ACTIONCTRL_COND_MODIFY, _("Modify"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer28->Add(m_CondModify, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Notebook->AddPage(m_ConditionsPanel, _("Conditions"));

    itemBoxSizer2->Add(m_Notebook, 0, wxGROW|wxALL, 5);

    wxStdDialogButtonSizer* itemStdDialogButtonSizer32 = new wxStdDialogButtonSizer;

    itemBoxSizer2->Add(itemStdDialogButtonSizer32, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
    m_OK = new wxButton( itemDialog1, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer32->AddButton(m_OK);

    m_Cancel = new wxButton( itemDialog1, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer32->AddButton(m_Cancel);

    m_Apply = new wxButton( itemDialog1, wxID_APPLY, _("&Apply"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer32->AddButton(m_Apply);

    itemStdDialogButtonSizer32->Realize();

////@end ActionsCtrlDlg content construction
}


/*!
 * Should we show tooltips?
 */

bool ActionsCtrlDlg::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap ActionsCtrlDlg::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin ActionsCtrlDlg bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end ActionsCtrlDlg bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon ActionsCtrlDlg::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin ActionsCtrlDlg icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end ActionsCtrlDlg icon retrieval
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_ACTIONCTRL_ADD
 */

void ActionsCtrlDlg::OnActionctrlAddClick( wxCommandEvent& event )
{
  addActionCtrl();
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_ACTIONCTRL_DELETE
 */

void ActionsCtrlDlg::OnActionctrlDeleteClick( wxCommandEvent& event )
{
  deleteSelected();
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_ACTIONCTRL_MODIFY
 */

void ActionsCtrlDlg::OnActionctrlModifyClick( wxCommandEvent& event )
{
  evaluate();
  //initIndex();
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
 */

void ActionsCtrlDlg::OnCancelClick( wxCommandEvent& event )
{
  cleanUp();
  EndModal( 0 );
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_APPLY
 */

void ActionsCtrlDlg::OnApplyClick( wxCommandEvent& event )
{
  evaluate();
  apply();
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
 */

void ActionsCtrlDlg::OnOkClick( wxCommandEvent& event )
{
  OnApplyClick(event);
  EndModal( wxID_OK );
}


/*!
 * wxEVT_COMMAND_LISTBOX_SELECTED event handler for ID_ACTIONCTRL_LIST
 */

void ActionsCtrlDlg::OnActionctrlListSelected( wxCommandEvent& event )
{
  initValues();
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_ACTIONCTRL_CONDITIONS
 */

void ActionsCtrlDlg::OnConditionsSelected( wxCommandEvent& event )
{
  initCondValues();
}


/*!
 * wxEVT_COMMAND_CHOICE_SELECTED event handler for ID_ACTIONCTRL_COND_TYPE
 */

void ActionsCtrlDlg::OnActionctrlCondTypeSelected( wxCommandEvent& event )
{
  initCondIDs();
}


/*!
 * wxEVT_COMMAND_CHOICE_SELECTED event handler for ID_ACTIOINCTRL_COND_ID
 */

void ActionsCtrlDlg::OnActioinctrlCondIdSelected( wxCommandEvent& event )
{
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_ACTIONCTRL_COND_ADD
 */

void ActionsCtrlDlg::OnActionctrlCondAddClick( wxCommandEvent& event )
{
  addActionCond();
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_ACTIONCTRL_COND_DELETE
 */

void ActionsCtrlDlg::OnActionctrlCondDeleteClick( wxCommandEvent& event )
{
  deleteSelectedCond();
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_ACTIONCTRL_COND_MODIFY
 */

void ActionsCtrlDlg::OnActionctrlCondModifyClick( wxCommandEvent& event )
{
  evaluateCond();
}

