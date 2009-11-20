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
#pragma implementation "seltabtrackdlg.h"
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

#include "seltabtrackdlg.h"

#include "rocs/public/node.h"
#include "rocrail/wrapper/public/SelTabPos.h"
#include "rocrail/wrapper/public/Block.h"
#include "rocrail/wrapper/public/Feedback.h"
#include "rocrail/wrapper/public/Plan.h"
#include "rocview/public/guiapp.h"

////@begin XPM images
////@end XPM images


/*!
 * SelTabTrackDialog type definition
 */

IMPLEMENT_DYNAMIC_CLASS( SelTabTrackDialog, wxDialog )


/*!
 * SelTabTrackDialog event table definition
 */

BEGIN_EVENT_TABLE( SelTabTrackDialog, wxDialog )

////@begin SelTabTrackDialog event table entries
    EVT_BUTTON( wxID_OK, SelTabTrackDialog::OnOkClick )

    EVT_BUTTON( wxID_CANCEL, SelTabTrackDialog::OnCancelClick )

////@end SelTabTrackDialog event table entries

END_EVENT_TABLE()


/*!
 * SelTabTrackDialog constructors
 */

SelTabTrackDialog::SelTabTrackDialog()
{
    Init();
}

SelTabTrackDialog::SelTabTrackDialog( wxWindow* parent, iONode p_Props )
{
    Init();
    Create(parent, -1, wxGetApp().getMsg("seltabpos") );
    m_Props = p_Props;
    initLabels();
    initValues();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);
}


/* comparator for sorting by id: */
static int __sortStr(obj* _a, obj* _b)
{
    const char* a = (const char*)*_a;
    const char* b = (const char*)*_b;
    return strcmp( a, b );
}

void SelTabTrackDialog::initLabels() {
  m_LabelTrackNr->SetLabel( wxGetApp().getMsg( "tracknr" ) );
  m_LabelDesc->SetLabel( wxGetApp().getMsg( "description" ) );
  m_labBlockID->SetLabel( wxGetApp().getMsg( "blockid" ) );

  // Buttons
  m_OK->SetLabel( wxGetApp().getMsg( "ok" ) );
  m_Cancel->SetLabel( wxGetApp().getMsg( "cancel" ) );
  
  m_BlockID->Append( _T("<undef>"), (void*)NULL );
  
  iONode model = wxGetApp().getModel();
  iOList list = ListOp.inst();

  if( model != NULL ) {
    iONode bklist = wPlan.getbklist( model );
    if( bklist != NULL ) {
      int cnt = NodeOp.getChildCnt( bklist );
      for( int i = 0; i < cnt; i++ ) {
        iONode bk = NodeOp.getChild( bklist, i );
        const char* id = wBlock.getid( bk );
        if( id != NULL ) {
          ListOp.add(list, (obj)id);    
        }
      }
      ListOp.sort(list, &__sortStr);
      cnt = ListOp.size( list );
      for( int i = 0; i < cnt; i++ ) {
        const char* id = (const char*)ListOp.get( list, i );       
        m_BlockID->Append( wxString(id,wxConvUTF8) );
      }
    }
  }
  /* clean up the temp. list */
  ListOp.base.del(list);
}


void SelTabTrackDialog::initValues() {
  if( m_Props == NULL )
    return;
  // General
  m_TrackNr->SetValue( wSelTabPos.getnr( m_Props ) );
  m_Desc->SetValue( wxString( wSelTabPos.getdesc( m_Props ),wxConvUTF8) );

  iONode model = wxGetApp().getModel();

  if( model != NULL ) {
    m_Sen1->Append( _T("") );
    m_Sen2->Append( _T("") );
    iONode fblist = wPlan.getfblist( model );
    if( fblist != NULL ) {
      int cnt = NodeOp.getChildCnt( fblist );
      for( int i = 0; i < cnt; i++ ) {
        iONode fb = NodeOp.getChild( fblist, i );
        const char* id = wFeedback.getid( fb );
        m_Sen1->Append( wxString(id,wxConvUTF8) );
        m_Sen2->Append( wxString(id,wxConvUTF8) );
      }
      m_Sen1->SetStringSelection( wxString(wSelTabPos.gets1id(m_Props),wxConvUTF8) );
      m_Sen2->SetStringSelection( wxString(wSelTabPos.gets2id(m_Props),wxConvUTF8) );

    }
  }

  m_BlockID->SetStringSelection( wxString(wSelTabPos.getbkid(m_Props),wxConvUTF8) );
}

void SelTabTrackDialog::evaluate() {
  if( m_Props == NULL )
    return;
  // General
  wSelTabPos.setnr( m_Props, m_TrackNr->GetValue() );
  wSelTabPos.setdesc( m_Props, m_Desc->GetValue().mb_str(wxConvUTF8) );
  wSelTabPos.sets1id( m_Props, m_Sen1->GetStringSelection().mb_str(wxConvUTF8) );
  wSelTabPos.sets2id( m_Props, m_Sen2->GetStringSelection().mb_str(wxConvUTF8) );
  wSelTabPos.setbkid( m_Props, m_BlockID->GetStringSelection().mb_str(wxConvUTF8) );
}

/*!
 * SelTabTrackDialog creator
 */

bool SelTabTrackDialog::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin SelTabTrackDialog creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end SelTabTrackDialog creation
    return true;
}


/*!
 * SelTabTrackDialog destructor
 */

SelTabTrackDialog::~SelTabTrackDialog()
{
////@begin SelTabTrackDialog destruction
////@end SelTabTrackDialog destruction
}


/*!
 * Member initialisation
 */

void SelTabTrackDialog::Init()
{
////@begin SelTabTrackDialog member initialisation
    m_LabelTrackNr = NULL;
    m_TrackNr = NULL;
    m_LabelDesc = NULL;
    m_Desc = NULL;
    m_labBlockID = NULL;
    m_BlockID = NULL;
    m_labSen1 = NULL;
    m_Sen1 = NULL;
    m_labSen2 = NULL;
    m_Sen2 = NULL;
    m_OK = NULL;
    m_Cancel = NULL;
////@end SelTabTrackDialog member initialisation
}


/*!
 * Control creation for SelTabTrackDialog
 */

void SelTabTrackDialog::CreateControls()
{    
////@begin SelTabTrackDialog content construction
    SelTabTrackDialog* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    wxFlexGridSizer* itemFlexGridSizer3 = new wxFlexGridSizer(2, 2, 0, 0);
    itemFlexGridSizer3->AddGrowableCol(1);
    itemBoxSizer2->Add(itemFlexGridSizer3, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    m_LabelTrackNr = new wxStaticText( itemDialog1, wxID_ANY, _("Track Nr."), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer3->Add(m_LabelTrackNr, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_TrackNr = new wxSpinCtrl( itemDialog1, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(70, -1), wxSP_ARROW_KEYS, 0, 47, 0 );
    itemFlexGridSizer3->Add(m_TrackNr, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_LabelDesc = new wxStaticText( itemDialog1, wxID_ANY, _("Description"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer3->Add(m_LabelDesc, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Desc = new wxTextCtrl( itemDialog1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(200, -1), 0 );
    itemFlexGridSizer3->Add(m_Desc, 1, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labBlockID = new wxStaticText( itemDialog1, wxID_ANY, _("BlockID"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer3->Add(m_labBlockID, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxArrayString m_BlockIDStrings;
    m_BlockID = new wxComboBox( itemDialog1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, m_BlockIDStrings, wxCB_DROPDOWN );
    itemFlexGridSizer3->Add(m_BlockID, 1, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labSen1 = new wxStaticText( itemDialog1, wxID_ANY, _("Sensor 1"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer3->Add(m_labSen1, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxArrayString m_Sen1Strings;
    m_Sen1 = new wxComboBox( itemDialog1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, m_Sen1Strings, wxCB_DROPDOWN );
    itemFlexGridSizer3->Add(m_Sen1, 1, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labSen2 = new wxStaticText( itemDialog1, wxID_ANY, _("Sensor 2"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer3->Add(m_labSen2, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxArrayString m_Sen2Strings;
    m_Sen2 = new wxComboBox( itemDialog1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, m_Sen2Strings, wxCB_DROPDOWN );
    itemFlexGridSizer3->Add(m_Sen2, 1, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStdDialogButtonSizer* itemStdDialogButtonSizer14 = new wxStdDialogButtonSizer;

    itemBoxSizer2->Add(itemStdDialogButtonSizer14, 0, wxALIGN_RIGHT|wxALL, 5);
    m_OK = new wxButton( itemDialog1, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    m_OK->SetDefault();
    itemStdDialogButtonSizer14->AddButton(m_OK);

    m_Cancel = new wxButton( itemDialog1, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer14->AddButton(m_Cancel);

    itemStdDialogButtonSizer14->Realize();

////@end SelTabTrackDialog content construction
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
 */

void SelTabTrackDialog::OnOkClick( wxCommandEvent& event )
{
  evaluate();
  EndModal( wxID_OK );
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
 */

void SelTabTrackDialog::OnCancelClick( wxCommandEvent& event )
{
  EndModal( 0 );
}


/*!
 * Should we show tooltips?
 */

bool SelTabTrackDialog::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap SelTabTrackDialog::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin SelTabTrackDialog bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end SelTabTrackDialog bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon SelTabTrackDialog::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin SelTabTrackDialog icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end SelTabTrackDialog icon retrieval
}
