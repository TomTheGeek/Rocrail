/*
 Copyright (C) 2002-2014 Rob Versluis, Rocrail.net

 

 */
#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "modulepropsdlg.h"
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

#include "modulepropsdlg.h"

#include "rocs/public/node.h"

#include "rocrail/wrapper/public/SysCmd.h"
#include "rocrail/wrapper/public/Item.h"
#include "rocrail/wrapper/public/ModPlan.h"
#include "rocrail/wrapper/public/Module.h"
#include "rocrail/wrapper/public/ModuleConnection.h"


#include "rocview/public/guiapp.h"

////@begin XPM images
////@end XPM images


/*!
 * ModulePropsDlg type definition
 */

IMPLEMENT_DYNAMIC_CLASS( ModulePropsDlg, wxDialog )


/*!
 * ModulePropsDlg event table definition
 */

BEGIN_EVENT_TABLE( ModulePropsDlg, wxDialog )

////@begin ModulePropsDlg event table entries
    EVT_BUTTON( wxID_OK, ModulePropsDlg::OnOkClick )
    EVT_BUTTON( wxID_CANCEL, ModulePropsDlg::OnCancelClick )
    EVT_BUTTON( wxID_HELP, ModulePropsDlg::OnHelpClick )
////@end ModulePropsDlg event table entries

END_EVENT_TABLE()


/*!
 * ModulePropsDlg constructors
 */

ModulePropsDlg::ModulePropsDlg()
{
    Init();
}

ModulePropsDlg::ModulePropsDlg( wxWindow* parent, iONode p_Props )
{
  m_Props = p_Props;
  m_TabAlign = wxGetApp().getTabAlign();
  Init();
  Create(parent, -1, wxGetApp().getMsg("module") );

  if( m_Props != NULL ) {
    initLabels();
    initValues();
  }
  
  m_GeneralPanel->GetSizer()->Layout();
  m_ConnectionPanel->GetSizer()->Layout();
  m_NoteBook->Fit();
  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);
}


void ModulePropsDlg::initLabels() {
  m_NoteBook->SetPageText( 0, wxGetApp().getMsg( "general" ) );
  m_NoteBook->SetPageText( 1, wxGetApp().getMsg( "connections" ) );

  // General
  m_labTitle->SetLabel( wxGetApp().getMsg( "title" ) );
  m_labID->SetLabel( wxGetApp().getMsg( "id" ) );
  m_labModNr->SetLabel( wxGetApp().getMsg( "number" ) );
  m_labFile->SetLabel( wxGetApp().getMsg( "file" ) );
  m_Rotation->SetLabel(wxGetApp().getMsg( "rotation" ) );
  m_Rotation->SetString( 0, wxGetApp().getMsg( "north" ) );
  m_Rotation->SetString( 1, wxGetApp().getMsg( "east" ) );
  m_Rotation->SetString( 2, wxGetApp().getMsg( "south" ) );
  m_Rotation->SetString( 3, wxGetApp().getMsg( "west" ) );
  
  // Connections
  m_NorthBox->SetLabel( wxGetApp().getMsg( "north" ) );
  m_labNorthModID->SetLabel( wxGetApp().getMsg( "id" ) );
  
  m_EastBox->SetLabel( wxGetApp().getMsg( "east" ) );
  m_labEastModID->SetLabel( wxGetApp().getMsg( "id" ) );
  
  m_SouthBox->SetLabel( wxGetApp().getMsg( "south" ) );
  m_labSouthModID->SetLabel( wxGetApp().getMsg( "id" ) );
  
  m_WestBox->SetLabel( wxGetApp().getMsg( "west" ) );
  m_labWestModID->SetLabel( wxGetApp().getMsg( "id" ) );
  
  m_NorthModID->Append( wxString( "-",wxConvUTF8 ) );
  m_EastModID->Append( wxString( "-",wxConvUTF8 ) );
  m_SouthModID->Append( wxString( "-",wxConvUTF8 ) );
  m_WestModID->Append( wxString( "-",wxConvUTF8 ) );
  
  iOList l_ModList = wxGetApp().getFrame()->getModPanel()->getModList();
  if( l_ModList != NULL ) {
    BasePanel* modpanel = (BasePanel*)ListOp.first(l_ModList);
    while( modpanel != NULL ) {
      const char* id = modpanel->getZID();
      if( !StrOp.equals( id, wModule.getid(m_Props) ) ) {
        m_NorthModID->Append( wxString( id,wxConvUTF8 ) );
        m_EastModID->Append( wxString( id,wxConvUTF8 ) );
        m_SouthModID->Append( wxString( id,wxConvUTF8 ) );
        m_WestModID->Append( wxString( id,wxConvUTF8 ) );
      }
      modpanel = (BasePanel*)ListOp.next(l_ModList);
    }
  }
  m_NorthModID->SetSelection(0);
  m_EastModID->SetSelection(0);
  m_SouthModID->SetSelection(0);
  m_WestModID->SetSelection(0);

  
  // Std buttons
  m_OK->SetLabel( wxGetApp().getMsg( "ok" ) );
  m_Cancel->SetLabel( wxGetApp().getMsg( "cancel" ) );
  
}

void ModulePropsDlg::initValues() {
  SetTitle( wxString( wModule.gettitle( m_Props ),wxConvUTF8 ) +
            _T(" / ") + wxString( wModule.getid( m_Props ),wxConvUTF8 ) );
  
  
  // General
  m_Title->SetValue( wxString( wModule.gettitle( m_Props ),wxConvUTF8 ) );
  m_ModuleID->SetValue( wxString( wModule.getid( m_Props ),wxConvUTF8 ) );
  m_ModNr->SetValue( wModule.getnr( m_Props ) );
  
  // Save the current ID:
  wItem.setprev_id( m_Props, wModule.getid( m_Props ) );
  
  m_File->SetValue( wxString( wModule.getfilename( m_Props ),wxConvUTF8 ) );
  m_X->SetValue(wModule.getx( m_Props ));
  m_Y->SetValue(wModule.gety( m_Props ));
  
  switch( wModule.getrotation( m_Props ) ) {
    case 0:
      m_Rotation->SetSelection(0);
      break;
    case 90:
      m_Rotation->SetSelection(1);
      break;
    case 180:
      m_Rotation->SetSelection(2);
      break;
    case 270:
      m_Rotation->SetSelection(3);
      break;
  }
  
  // Connections
  iONode conn = wModule.getconnection(m_Props);
  while( conn != NULL ) {
    if( StrOp.equals( wModuleConnection.point_n, wModuleConnection.getside(conn) ) ) {
      m_NorthModID->SetStringSelection(wxString( wModuleConnection.getmodid(conn),wxConvUTF8 ));
    }
    else if( StrOp.equals( wModuleConnection.point_e, wModuleConnection.getside(conn) ) ) {
      m_EastModID->SetStringSelection(wxString( wModuleConnection.getmodid(conn),wxConvUTF8 ));
    }
    else if( StrOp.equals( wModuleConnection.point_s, wModuleConnection.getside(conn) ) ) {
      m_SouthModID->SetStringSelection(wxString( wModuleConnection.getmodid(conn),wxConvUTF8 ));
    }
    else if( StrOp.equals( wModuleConnection.point_w, wModuleConnection.getside(conn) ) ) {
      m_WestModID->SetStringSelection(wxString( wModuleConnection.getmodid(conn),wxConvUTF8 ));
    }
    conn = wModule.nextconnection(m_Props, conn );
  };
}


void ModulePropsDlg::evaluate() {
  if( m_Props == NULL )
    return;
  // General
  wModule.settitle( m_Props, m_Title->GetValue().mb_str(wxConvUTF8) );
  wModule.setid( m_Props, m_ModuleID->GetValue().mb_str(wxConvUTF8) );
  wModule.setnr( m_Props, m_ModNr->GetValue() );
  wModule.setfilename( m_Props, m_File->GetValue().mb_str(wxConvUTF8) );
  
  wModule.setx( m_Props, m_X->GetValue() );
  wModule.sety( m_Props, m_Y->GetValue() );
  
  switch( m_Rotation->GetSelection() ) {
    case 0:
      wModule.setrotation( m_Props, 0 );
      break;
    case 1:
      wModule.setrotation( m_Props, 90 );
      break;
    case 2:
      wModule.setrotation( m_Props, 180 );
      break;
    case 3:
      wModule.setrotation( m_Props, 270 );
      break;
  }
  
  
  // Connections
  if( m_NorthModID->GetStringSelection() == _T("-") )
    deleteConnection(wModuleConnection.point_n);
  else
    addConnection(wModuleConnection.point_n, m_NorthModID->GetStringSelection().mb_str(wxConvUTF8) );
    
  if( m_EastModID->GetStringSelection() == _T("-") )
    deleteConnection(wModuleConnection.point_e);
  else
    addConnection(wModuleConnection.point_e, m_EastModID->GetStringSelection().mb_str(wxConvUTF8) );
    
  if( m_SouthModID->GetStringSelection() == _T("-") )
    deleteConnection(wModuleConnection.point_s);
  else
    addConnection(wModuleConnection.point_s, m_SouthModID->GetStringSelection().mb_str(wxConvUTF8) );
    
  if( m_WestModID->GetStringSelection() == _T("-") )
    deleteConnection(wModuleConnection.point_w);
  else
    addConnection(wModuleConnection.point_w, m_WestModID->GetStringSelection().mb_str(wxConvUTF8) );
    
}

void ModulePropsDlg::deleteConnection(const char* side) {
  iONode conn = wModule.getconnection(m_Props);
  while( conn != NULL ) {
    if( StrOp.equals( wModuleConnection.getside(conn), side ) ) {
      NodeOp.removeChild( m_Props, conn );
      break;
    }
    conn = wModule.nextconnection(m_Props, conn );
  };
  
}

void ModulePropsDlg::addConnection(const char* side, const char* id) {
  iONode conn = wModule.getconnection(m_Props);
  while( conn != NULL ) {
    if( StrOp.equals( wModuleConnection.getside(conn), side ) ) {
      wModuleConnection.setmodid(conn, id );
      return;
    }
    conn = wModule.nextconnection(m_Props, conn );
  };
  conn = NodeOp.inst( wModuleConnection.name(), m_Props, ELEMENT_NODE );
  NodeOp.addChild( m_Props, conn );
  wModuleConnection.setside(conn, side );
  wModuleConnection.setmodid(conn, id );
}

/*!
 * ModulePropsDlg creator
 */

bool ModulePropsDlg::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin ModulePropsDlg creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end ModulePropsDlg creation
    return true;
}


/*!
 * ModulePropsDlg destructor
 */

ModulePropsDlg::~ModulePropsDlg()
{
////@begin ModulePropsDlg destruction
////@end ModulePropsDlg destruction
}


/*!
 * Member initialisation
 */

void ModulePropsDlg::Init()
{
////@begin ModulePropsDlg member initialisation
    m_NoteBook = NULL;
    m_GeneralPanel = NULL;
    m_labTitle = NULL;
    m_Title = NULL;
    m_labID = NULL;
    m_ModuleID = NULL;
    m_labModNr = NULL;
    m_ModNr = NULL;
    m_labFile = NULL;
    m_File = NULL;
    m_labX = NULL;
    m_X = NULL;
    m_labY = NULL;
    m_Y = NULL;
    m_Rotation = NULL;
    m_ConnectionPanel = NULL;
    m_NorthBox = NULL;
    m_labNorthModID = NULL;
    m_NorthModID = NULL;
    m_EastBox = NULL;
    m_labEastModID = NULL;
    m_EastModID = NULL;
    m_SouthBox = NULL;
    m_labSouthModID = NULL;
    m_SouthModID = NULL;
    m_WestBox = NULL;
    m_labWestModID = NULL;
    m_WestModID = NULL;
    m_OK = NULL;
    m_Cancel = NULL;
////@end ModulePropsDlg member initialisation
}


/*!
 * Control creation for ModulePropsDlg
 */

void ModulePropsDlg::CreateControls()
{    
////@begin ModulePropsDlg content construction
    ModulePropsDlg* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    m_NoteBook = new wxNotebook( itemDialog1, ID_NOTEBOOK_MODPROPS, wxDefaultPosition, wxDefaultSize, wxBK_DEFAULT|m_TabAlign );

    m_GeneralPanel = new wxPanel( m_NoteBook, ID_PANEL_MODPROPS_GEN, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxVERTICAL);
    m_GeneralPanel->SetSizer(itemBoxSizer5);

    wxFlexGridSizer* itemFlexGridSizer6 = new wxFlexGridSizer(0, 2, 0, 0);
    itemBoxSizer5->Add(itemFlexGridSizer6, 0, wxALIGN_LEFT|wxALL, 5);
    m_labTitle = new wxStaticText( m_GeneralPanel, wxID_ANY, _("Title"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(m_labTitle, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Title = new wxTextCtrl( m_GeneralPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(160, -1), 0 );
    itemFlexGridSizer6->Add(m_Title, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labID = new wxStaticText( m_GeneralPanel, wxID_ANY, _("Module ID"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(m_labID, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_ModuleID = new wxTextCtrl( m_GeneralPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(100, -1), 0 );
    itemFlexGridSizer6->Add(m_ModuleID, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labModNr = new wxStaticText( m_GeneralPanel, wxID_ANY, _("Module number"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(m_labModNr, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_ModNr = new wxSpinCtrl( m_GeneralPanel, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 65535, 0 );
    itemFlexGridSizer6->Add(m_ModNr, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labFile = new wxStaticText( m_GeneralPanel, wxID_ANY, _("File"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(m_labFile, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_File = new wxTextCtrl( m_GeneralPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(160, -1), 0 );
    itemFlexGridSizer6->Add(m_File, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer15 = new wxFlexGridSizer(0, 4, 0, 0);
    itemBoxSizer5->Add(itemFlexGridSizer15, 0, wxALIGN_LEFT|wxALL, 5);
    m_labX = new wxStaticText( m_GeneralPanel, wxID_ANY, _("X"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer15->Add(m_labX, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_X = new wxSpinCtrl( m_GeneralPanel, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize(70, -1), wxSP_ARROW_KEYS, 0, 100, 0 );
    itemFlexGridSizer15->Add(m_X, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labY = new wxStaticText( m_GeneralPanel, wxID_ANY, _("Y"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer15->Add(m_labY, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Y = new wxSpinCtrl( m_GeneralPanel, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize(70, -1), wxSP_ARROW_KEYS, 0, 100, 0 );
    itemFlexGridSizer15->Add(m_Y, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer20 = new wxFlexGridSizer(0, 2, 0, 0);
    itemBoxSizer5->Add(itemFlexGridSizer20, 0, wxGROW|wxALL, 5);
    wxArrayString m_RotationStrings;
    m_RotationStrings.Add(_("&North"));
    m_RotationStrings.Add(_("&East"));
    m_RotationStrings.Add(_("&South"));
    m_RotationStrings.Add(_("&West"));
    m_Rotation = new wxRadioBox( m_GeneralPanel, wxID_ANY, _("Rotation"), wxDefaultPosition, wxDefaultSize, m_RotationStrings, 1, wxRA_SPECIFY_ROWS );
    m_Rotation->SetSelection(0);
    itemFlexGridSizer20->Add(m_Rotation, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_NoteBook->AddPage(m_GeneralPanel, _("General"));

    m_ConnectionPanel = new wxPanel( m_NoteBook, ID_PANEL_MODPROPS_CONN, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer23 = new wxBoxSizer(wxVERTICAL);
    m_ConnectionPanel->SetSizer(itemBoxSizer23);

    m_NorthBox = new wxStaticBox(m_ConnectionPanel, wxID_ANY, _("North"));
    wxStaticBoxSizer* itemStaticBoxSizer24 = new wxStaticBoxSizer(m_NorthBox, wxVERTICAL);
    itemBoxSizer23->Add(itemStaticBoxSizer24, 0, wxGROW|wxALL, 5);
    wxFlexGridSizer* itemFlexGridSizer25 = new wxFlexGridSizer(0, 2, 0, 0);
    itemStaticBoxSizer24->Add(itemFlexGridSizer25, 0, wxGROW|wxALL, 5);
    m_labNorthModID = new wxStaticText( m_ConnectionPanel, wxID_ANY, _("Module ID"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer25->Add(m_labNorthModID, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    wxArrayString m_NorthModIDStrings;
    m_NorthModID = new wxChoice( m_ConnectionPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_NorthModIDStrings, 0 );
    itemFlexGridSizer25->Add(m_NorthModID, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    itemFlexGridSizer25->AddGrowableCol(1);

    m_EastBox = new wxStaticBox(m_ConnectionPanel, wxID_ANY, _("East"));
    wxStaticBoxSizer* itemStaticBoxSizer28 = new wxStaticBoxSizer(m_EastBox, wxVERTICAL);
    itemBoxSizer23->Add(itemStaticBoxSizer28, 0, wxGROW|wxALL, 5);
    wxFlexGridSizer* itemFlexGridSizer29 = new wxFlexGridSizer(0, 2, 0, 0);
    itemStaticBoxSizer28->Add(itemFlexGridSizer29, 0, wxGROW|wxALL, 5);
    m_labEastModID = new wxStaticText( m_ConnectionPanel, wxID_ANY, _("Module ID"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer29->Add(m_labEastModID, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    wxArrayString m_EastModIDStrings;
    m_EastModID = new wxChoice( m_ConnectionPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_EastModIDStrings, 0 );
    itemFlexGridSizer29->Add(m_EastModID, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    itemFlexGridSizer29->AddGrowableCol(1);

    m_SouthBox = new wxStaticBox(m_ConnectionPanel, wxID_ANY, _("South"));
    wxStaticBoxSizer* itemStaticBoxSizer32 = new wxStaticBoxSizer(m_SouthBox, wxVERTICAL);
    itemBoxSizer23->Add(itemStaticBoxSizer32, 0, wxGROW|wxALL, 5);
    wxFlexGridSizer* itemFlexGridSizer33 = new wxFlexGridSizer(0, 2, 0, 0);
    itemStaticBoxSizer32->Add(itemFlexGridSizer33, 0, wxGROW|wxALL, 5);
    m_labSouthModID = new wxStaticText( m_ConnectionPanel, wxID_ANY, _("Module ID"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer33->Add(m_labSouthModID, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    wxArrayString m_SouthModIDStrings;
    m_SouthModID = new wxChoice( m_ConnectionPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_SouthModIDStrings, 0 );
    itemFlexGridSizer33->Add(m_SouthModID, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    itemFlexGridSizer33->AddGrowableCol(1);

    m_WestBox = new wxStaticBox(m_ConnectionPanel, wxID_ANY, _("West"));
    wxStaticBoxSizer* itemStaticBoxSizer36 = new wxStaticBoxSizer(m_WestBox, wxVERTICAL);
    itemBoxSizer23->Add(itemStaticBoxSizer36, 0, wxGROW|wxALL, 5);
    wxFlexGridSizer* itemFlexGridSizer37 = new wxFlexGridSizer(0, 2, 0, 0);
    itemStaticBoxSizer36->Add(itemFlexGridSizer37, 0, wxGROW|wxALL, 5);
    m_labWestModID = new wxStaticText( m_ConnectionPanel, wxID_ANY, _("Module ID"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer37->Add(m_labWestModID, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    wxArrayString m_WestModIDStrings;
    m_WestModID = new wxChoice( m_ConnectionPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_WestModIDStrings, 0 );
    itemFlexGridSizer37->Add(m_WestModID, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    itemFlexGridSizer37->AddGrowableCol(1);

    m_NoteBook->AddPage(m_ConnectionPanel, _("Connections"));

    itemBoxSizer2->Add(m_NoteBook, 0, wxGROW|wxALL, 5);

    wxStdDialogButtonSizer* itemStdDialogButtonSizer40 = new wxStdDialogButtonSizer;

    itemBoxSizer2->Add(itemStdDialogButtonSizer40, 0, wxGROW|wxALL, 5);
    m_OK = new wxButton( itemDialog1, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer40->AddButton(m_OK);

    m_Cancel = new wxButton( itemDialog1, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer40->AddButton(m_Cancel);

    wxButton* itemButton43 = new wxButton( itemDialog1, wxID_HELP, _("&Help"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer40->AddButton(itemButton43);

    itemStdDialogButtonSizer40->Realize();

////@end ModulePropsDlg content construction
}


/*!
 * Should we show tooltips?
 */

bool ModulePropsDlg::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap ModulePropsDlg::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin ModulePropsDlg bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end ModulePropsDlg bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon ModulePropsDlg::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin ModulePropsDlg icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end ModulePropsDlg icon retrieval
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
 */

void ModulePropsDlg::OnOkClick( wxCommandEvent& event )
{
  if( m_Props != NULL ) {
    evaluate();
    /* Notify RocRail. */
    iONode cmd = NodeOp.inst( wSysCmd.name(), NULL, ELEMENT_NODE );
    wSysCmd.setcmd( cmd, wSysCmd.setmodule );
    wSysCmd.setid( cmd, wModule.getid(m_Props) );
    NodeOp.addChild( cmd, (iONode)NodeOp.base.clone( m_Props ) );
    wxGetApp().sendToRocrail( cmd );
    cmd->base.del(cmd);
  
    EndModal( wxID_OK );
  }
  else {
    EndModal( 0 );
  }
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
 */

void ModulePropsDlg::OnCancelClick( wxCommandEvent& event )
{
  EndModal( 0 );
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_HELP
 */

void ModulePropsDlg::OnHelpClick( wxCommandEvent& event )
{
  wxGetApp().openLink( "modular-setup" );
}

