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
#pragma implementation "mvtrackdlg.h"
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

#include "mvtrackdlg.h"

////@begin XPM images
////@end XPM images

#include "rocgui/public/guiapp.h"
#include "rocrail/wrapper/public/MVTrack.h"
#include "rocrail/wrapper/public/Plan.h"
#include "rocrail/wrapper/public/Feedback.h"

#include "rocs/public/list.h"

/*!
 * MVTrackDlg type definition
 */

IMPLEMENT_DYNAMIC_CLASS( MVTrackDlg, wxDialog )


/*!
 * MVTrackDlg event table definition
 */

BEGIN_EVENT_TABLE( MVTrackDlg, wxDialog )

////@begin MVTrackDlg event table entries
    EVT_BUTTON( wxID_OK, MVTrackDlg::OnOkClick )

    EVT_BUTTON( wxID_CANCEL, MVTrackDlg::OnCancelClick )

////@end MVTrackDlg event table entries

END_EVENT_TABLE()


/*!
 * MVTrackDlg constructors
 */

MVTrackDlg::MVTrackDlg()
{
    Init();
}

MVTrackDlg::MVTrackDlg( wxWindow* parent, iONode p_Props )
{
    Init();
    Create(parent, -1, wxGetApp().getMsg("mvtrack"));
    m_Props = p_Props;
    initLabels();
    initSensorCombos();
    initValues();
	
	GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);
    GetSizer()->Layout();

}


/*!
 * MVTrackDlg creator
 */

bool MVTrackDlg::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin MVTrackDlg creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end MVTrackDlg creation
    return true;
}


void MVTrackDlg::initLabels() {
  m_labGauge->SetLabel( wxGetApp().getMsg( "scale" ) );
  m_labDistance->SetLabel( wxGetApp().getMsg( "distance" ) );
  m_labSensor1->SetLabel( wxGetApp().getMsg( "sensor" ) + _T(" 1") );
  m_labSensor2->SetLabel( wxGetApp().getMsg( "sensor" ) + _T(" 2") );
  m_MPH->SetLabel( wxGetApp().getMsg( "convert2mph" ) );
	
  m_OK->SetLabel( wxGetApp().getMsg( "ok" ) );
  m_Cancel->SetLabel( wxGetApp().getMsg( "cancel" ) );
}

/* comparator for sorting by id: */
static int __sortStr(obj* _a, obj* _b)
{
    const char* a = (const char*)*_a;
    const char* b = (const char*)*_b;
    return strcmp( a, b );
}

void MVTrackDlg::initSensorCombos() {
  m_Sensor1->Clear();
  m_Sensor2->Clear();
  m_Sensor1->Append( wxString("-",wxConvUTF8) );
  m_Sensor2->Append( wxString("-",wxConvUTF8) );
    
  iONode model = wxGetApp().getModel();
  iOList list = ListOp.inst();
  if( model != NULL ) {
    iONode fblist = wPlan.getfblist( model );
    if( fblist != NULL ) {
      int cnt = NodeOp.getChildCnt( fblist );
      for( int i = 0; i < cnt; i++ ) {
        iONode fb = NodeOp.getChild( fblist, i );
        const char* id = wFeedback.getid( fb );
        if( id != NULL ) {
          ListOp.add(list, (obj)id);
        }
      }
      ListOp.sort(list, &__sortStr);
      cnt = ListOp.size( list );
      for( int i = 0; i < cnt; i++ ) {
        const char* id = (const char*)ListOp.get( list, i );       
        m_Sensor1->Append( wxString(id,wxConvUTF8) );
        m_Sensor2->Append( wxString(id,wxConvUTF8) );
      }
    }
  }
  /* clean up the temp. list */
  ListOp.base.del(list);

  m_Sensor1->SetStringSelection( wxString("-",wxConvUTF8) );
  m_Sensor2->SetStringSelection( wxString("-",wxConvUTF8) );
}

void MVTrackDlg::initValues() {
  if( m_Props == NULL )
    return;
  m_Gauge->SetValue( wMVTrack.getscale( m_Props) );
  wxString val; val.Printf( _T("%d"), wMVTrack.getdistance( m_Props ) );
  m_Distance->SetValue( val );
  m_MPH->SetValue( wMVTrack.ismph( m_Props )?true:false );
  m_Sensor1->SetStringSelection( wxString(wMVTrack.gets1( m_Props ),wxConvUTF8) );
  m_Sensor2->SetStringSelection( wxString(wMVTrack.gets2( m_Props ),wxConvUTF8) );
}

void MVTrackDlg::evaluate() {
  if( m_Props == NULL )
    return;
  wMVTrack.setscale( m_Props, m_Gauge->GetValue() );
  wMVTrack.setdistance( m_Props, atoi( m_Distance->GetValue().mb_str(wxConvUTF8) ) );
  wMVTrack.setmph( m_Props, m_MPH->IsChecked() ? True:False ); 

  if( StrOp.equals( "-", m_Sensor1->GetStringSelection().mb_str(wxConvUTF8) ) )
    wMVTrack.sets1( m_Props, "" );
  else
    wMVTrack.sets1( m_Props, m_Sensor1->GetStringSelection().mb_str(wxConvUTF8) );

  if( StrOp.equals( "-", m_Sensor2->GetStringSelection().mb_str(wxConvUTF8) ) )
    wMVTrack.sets2( m_Props, "" );
  else
    wMVTrack.sets2( m_Props, m_Sensor2->GetStringSelection().mb_str(wxConvUTF8) );

}



/*!
 * MVTrackDlg destructor
 */

MVTrackDlg::~MVTrackDlg()
{
////@begin MVTrackDlg destruction
////@end MVTrackDlg destruction
}


/*!
 * Member initialisation
 */

void MVTrackDlg::Init()
{
////@begin MVTrackDlg member initialisation
    m_labGauge = NULL;
    m_labGaugePrefix = NULL;
    m_Gauge = NULL;
    m_labDistance = NULL;
    m_Distance = NULL;
    m_labMM = NULL;
    m_MPH = NULL;
    m_labSensor1 = NULL;
    m_Sensor1 = NULL;
    m_labSensor2 = NULL;
    m_Sensor2 = NULL;
    m_OK = NULL;
    m_Cancel = NULL;
////@end MVTrackDlg member initialisation
}


/*!
 * Control creation for MVTrackDlg
 */

void MVTrackDlg::CreateControls()
{    
////@begin MVTrackDlg content construction
    MVTrackDlg* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    wxFlexGridSizer* itemFlexGridSizer3 = new wxFlexGridSizer(2, 3, 0, 0);
    itemBoxSizer2->Add(itemFlexGridSizer3, 0, wxGROW|wxALL, 5);

    m_labGauge = new wxStaticText( itemDialog1, wxID_ANY, _("Gauge"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer3->Add(m_labGauge, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labGaugePrefix = new wxStaticText( itemDialog1, wxID_ANY, _("1:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer3->Add(m_labGaugePrefix, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    m_Gauge = new wxSpinCtrl( itemDialog1, wxID_ANY, _T("87"), wxDefaultPosition, wxSize(70, -1), wxSP_ARROW_KEYS, 1, 300, 87 );
    itemFlexGridSizer3->Add(m_Gauge, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxRIGHT|wxTOP|wxBOTTOM, 5);

    wxFlexGridSizer* itemFlexGridSizer7 = new wxFlexGridSizer(2, 3, 0, 0);
    itemBoxSizer2->Add(itemFlexGridSizer7, 0, wxGROW|wxALL, 5);

    m_labDistance = new wxStaticText( itemDialog1, wxID_ANY, _("Distance"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer7->Add(m_labDistance, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Distance = new wxTextCtrl( itemDialog1, wxID_ANY, _("0"), wxDefaultPosition, wxSize(70, -1), wxTE_CENTRE );
    itemFlexGridSizer7->Add(m_Distance, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    m_labMM = new wxStaticText( itemDialog1, wxID_ANY, _("mm"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer7->Add(m_labMM, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxRIGHT|wxTOP|wxBOTTOM, 5);

    m_MPH = new wxCheckBox( itemDialog1, wxID_ANY, _("Convert to MPH"), wxDefaultPosition, wxDefaultSize, 0 );
    m_MPH->SetValue(false);
    itemBoxSizer2->Add(m_MPH, 0, wxALIGN_LEFT|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer12 = new wxFlexGridSizer(2, 2, 0, 0);
    itemBoxSizer2->Add(itemFlexGridSizer12, 0, wxGROW|wxALL, 5);

    m_labSensor1 = new wxStaticText( itemDialog1, wxID_ANY, _("Sensor 1"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer12->Add(m_labSensor1, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxArrayString m_Sensor1Strings;
    m_Sensor1 = new wxComboBox( itemDialog1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(130, -1), m_Sensor1Strings, wxCB_DROPDOWN );
    itemFlexGridSizer12->Add(m_Sensor1, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labSensor2 = new wxStaticText( itemDialog1, wxID_ANY, _("Sensor 2"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer12->Add(m_labSensor2, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxArrayString m_Sensor2Strings;
    m_Sensor2 = new wxComboBox( itemDialog1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(130, -1), m_Sensor2Strings, wxCB_DROPDOWN );
    itemFlexGridSizer12->Add(m_Sensor2, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStdDialogButtonSizer* itemStdDialogButtonSizer17 = new wxStdDialogButtonSizer;

    itemBoxSizer2->Add(itemStdDialogButtonSizer17, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
    m_OK = new wxButton( itemDialog1, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer17->AddButton(m_OK);

    m_Cancel = new wxButton( itemDialog1, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer17->AddButton(m_Cancel);

    itemStdDialogButtonSizer17->Realize();

////@end MVTrackDlg content construction
}


/*!
 * Should we show tooltips?
 */

bool MVTrackDlg::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap MVTrackDlg::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin MVTrackDlg bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end MVTrackDlg bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon MVTrackDlg::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin MVTrackDlg icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end MVTrackDlg icon retrieval
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
 */

void MVTrackDlg::OnCancelClick( wxCommandEvent& event )
{
  EndModal( 0 );
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
 */

void MVTrackDlg::OnOkClick( wxCommandEvent& event )
{
  evaluate();
  EndModal( wxID_OK );
}

