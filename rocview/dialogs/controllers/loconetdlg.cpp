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
#pragma implementation "loconetdlg.h"
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

#include "loconetdlg.h"

////@begin XPM images
////@end XPM images

#include "rocview/public/guiapp.h"

#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/LocoNet.h"
#include "rocrail/wrapper/public/CSOptions.h"
#include "rocrail/wrapper/public/LNSlotServer.h"

#include "rocdigs/impl/loconet/lncmdstn.h"

/*!
 * LocoNetCtrlDlg type definition
 */

IMPLEMENT_DYNAMIC_CLASS( LocoNetCtrlDlg, wxDialog )

/*!
 * LocoNetCtrlDlg event table definition
 */

BEGIN_EVENT_TABLE( LocoNetCtrlDlg, wxDialog )

////@begin LocoNetCtrlDlg event table entries
    EVT_RADIOBOX( wxID_ANY, LocoNetCtrlDlg::OnSublibSelected )

    EVT_COMBOBOX( ID_COMBOBOX_LOCONET_CMDSTN, LocoNetCtrlDlg::OnComboboxLoconetCmdstnSelected )

    EVT_BUTTON( wxID_OK, LocoNetCtrlDlg::OnOkClick )

    EVT_BUTTON( wxID_CANCEL, LocoNetCtrlDlg::OnCancelClick )

////@end LocoNetCtrlDlg event table entries

END_EVENT_TABLE()

/*!
 * LocoNetCtrlDlg constructors
 */

LocoNetCtrlDlg::LocoNetCtrlDlg()
{
    Init();
}

LocoNetCtrlDlg::LocoNetCtrlDlg( wxWindow* parent, iONode props )
{
  m_TabAlign = wxGetApp().getTabAlign();
  Create(parent, -1, _T("LocoNet"));
  m_Props = props;
  initLabels();
  initValues();

  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);

  GetSizer()->Layout();
}

void LocoNetCtrlDlg::initLabels() {
  m_labIID->SetLabel( wxGetApp().getMsg( "iid" ) );
  m_labDevice->SetLabel( wxGetApp().getMsg( "device" ) );
  m_labHost->SetLabel( wxGetApp().getMsg( "host" ) );
  m_labPort->SetLabel( wxGetApp().getMsg( "port" ) );
  m_Baudrate->SetLabel( wxGetApp().getMsg( "bps" ) );
  m_SubLib->SetLabel( wxGetApp().getMsg( "type" ) );
  m_labSWTime->SetLabel( wxGetApp().getMsg( "switchtime" ) );
  m_labPurgeTime->SetLabel( wxGetApp().getMsg( "purgetime" ) );
  m_Flow->SetLabel( wxGetApp().getMsg( "ctsflow" ) );
  m_IgnorePowerCmds->SetLabel( wxGetApp().getMsg( "ignorepowercmds" ) );

  m_UseFC->SetLabel( wxGetApp().getMsg( "enable" ) );
  m_SyncFC->SetLabel( wxGetApp().getMsg( "sync" ) );

  m_SlotServerActive->SetLabel( wxGetApp().getMsg( "enable" ) );
  m_MobileOnly->SetLabel( wxGetApp().getMsg( "onlymobiledec" ) );
  m_PurgeSlots->SetLabel( wxGetApp().getMsg( "purgeslots" ) );
  m_StopAtPurge->SetLabel( wxGetApp().getMsg( "stopatpurge" ) );

  m_CmdStn->Clear();
  m_CmdStn->Append( wxString( wLocoNet.cs_dcs100, wxConvUTF8 ) );
  m_CmdStn->Append( wxString( wLocoNet.cs_dcs50, wxConvUTF8 ) );
  m_CmdStn->Append( wxString( wLocoNet.cs_db150, wxConvUTF8 ) );
  m_CmdStn->Append( wxString( wLocoNet.cs_intellibox, wxConvUTF8 ) );
  m_CmdStn->Append( wxString( wLocoNet.cs_ibcom, wxConvUTF8 ) );
}

void LocoNetCtrlDlg::initValues() {
  m_IID->SetValue( wxString( wDigInt.getiid( m_Props ), wxConvUTF8 ) );
  m_Device->SetValue( wxString( wDigInt.getdevice( m_Props ), wxConvUTF8 ) );
  m_Host->SetValue( wxString( wDigInt.gethost( m_Props ), wxConvUTF8 ) );
  char* val = StrOp.fmt( "%d", wDigInt.getport( m_Props ) );
  m_Port->SetValue( wxString( val, wxConvUTF8 ) ); StrOp.free( val );
  val = StrOp.fmt( "%d", wDigInt.getswtime( m_Props ) );
  m_SWTime->SetValue( wxString( val, wxConvUTF8 ) ); StrOp.free( val );

  if( wDigInt.getbps( m_Props ) == 19200 )
    m_Baudrate->SetSelection(0);
  else if( wDigInt.getbps( m_Props ) == 57600 )
    m_Baudrate->SetSelection(1);
  else if( wDigInt.getbps( m_Props ) == 115200 )
    m_Baudrate->SetSelection(2);
  else
    m_Baudrate->SetSelection(0);

  if( StrOp.equals( wDigInt.sublib_udp, wDigInt.getsublib( m_Props ) ) )
    m_SubLib->SetSelection(3);
  else if( StrOp.equals( wDigInt.sublib_native, wDigInt.getsublib( m_Props ) ) )
    m_SubLib->SetSelection(2);
  else if( StrOp.equals( wDigInt.sublib_socket, wDigInt.getsublib( m_Props ) ) )
    m_SubLib->SetSelection(1);
  else
    m_SubLib->SetSelection(0);

  //m_SubLib->Enable(2, false);

  /* loconet node */
  {
    iONode loconet = wDigInt.getloconet( m_Props );
    iONode options = NULL;
    iONode slotserver = NULL;
    int i = 0;

    if( loconet == NULL ) {
      loconet = NodeOp.inst( wLocoNet.name(), m_Props, ELEMENT_NODE );
      NodeOp.addChild( m_Props, loconet );
    }
    val = StrOp.fmt( "%d", wLocoNet.getpurgetime( loconet ) );
    m_PurgeTime->SetValue( wxString( val, wxConvUTF8 ) ); StrOp.free( val );

    m_Flow->SetValue( StrOp.equals( wDigInt.cts, wDigInt.getflow(m_Props) ) ? true:false );

    m_IgnorePowerCmds->SetValue( wLocoNet.isignorepowercmds(loconet) ? true:false );

    m_UseFC->SetValue( wLocoNet.isusefc(loconet) ? true:false );
    m_SyncFC->SetValue( wLocoNet.issyncfc(loconet) ? true:false );


    // Options
    options = wLocoNet.getoptions( loconet );
    if( options == NULL ) {
      options = NodeOp.inst( wCSOptions.name(), m_Props, ELEMENT_NODE );
      NodeOp.addChild( loconet, options );
    }

    m_CmdStn->SetStringSelection( wxString(wLocoNet.getcmdstn( loconet ),wxConvUTF8) );
    initOptions();

    slotserver = wLocoNet.getslotserver( loconet );
    if( slotserver == NULL ) {
      slotserver = NodeOp.inst( wLNSlotServer.name(), m_Props, ELEMENT_NODE );
      NodeOp.addChild( loconet, slotserver );
    }

    m_SlotServerActive->SetValue( wLNSlotServer.isactive(slotserver) ? true:false );
    m_MobileOnly->SetValue( wLNSlotServer.islconly(slotserver) ? true:false );
    m_PurgeSlots->SetValue( wLNSlotServer.ispurge(slotserver) ? true:false );
    m_StopAtPurge->SetValue( wLNSlotServer.isstopatpurge(slotserver) ? true:false );

    m_DispatchIID->SetValue(wxString( wLNSlotServer.getiid( slotserver ), wxConvUTF8 ));

  }

  initSublib();
}

void LocoNetCtrlDlg::initOptions() {
    iONode loconet = wDigInt.getloconet( m_Props );
    iONode options = wLocoNet.getoptions( loconet );
    const char* csopsw = wCSOptions.getopsw( options );
    int csopswlen = StrOp.len(csopsw);

    int selected = m_CmdStn->GetSelection();
    const int* opsw = NULL;
    if( selected == 0 )
      opsw = opsw_dcs100;
    else if( selected == 1 )
      opsw = opsw_dcs50;
    else if( selected == 2 )
      opsw = opsw_db150;
    else
      opsw = opsw_intellibox;

    m_OptionsList->Clear();
    int idx = 0;
    for( int i = 0; i < MAX_OPTION; i++ ) {
      if( opsw[i] == 0 )
        break;
      m_OptionsList->Append( wxString( cmdstnOPTIONS[opsw[i]], wxConvUTF8 ));
      optidx[idx] = opsw[i];
      if( opsw[i] < csopswlen )
        m_OptionsList->Check(idx, ((csopsw[opsw[i]]=='1')?true:false));
      idx++;
    }

}


void LocoNetCtrlDlg::evaluate() {
  if( m_Props == NULL )
    return;
  wDigInt.setiid( m_Props, m_IID->GetValue().mb_str(wxConvUTF8) );
  wDigInt.setdevice( m_Props, m_Device->GetValue().mb_str(wxConvUTF8) );
  wDigInt.sethost( m_Props, m_Host->GetValue().mb_str(wxConvUTF8) );
  wDigInt.setport( m_Props, atoi( m_Port->GetValue().mb_str(wxConvUTF8) ) );
  wDigInt.setswtime( m_Props, atoi( m_SWTime->GetValue().mb_str(wxConvUTF8) ) );

  if( m_Baudrate->GetSelection() == 0 )
    wDigInt.setbps( m_Props, 19200 );
  else if( m_Baudrate->GetSelection() == 1 )
    wDigInt.setbps( m_Props, 57600 );
  else if( m_Baudrate->GetSelection() == 2 )
    wDigInt.setbps( m_Props, 115200 );

  wDigInt.setflow(m_Props, m_Flow->GetValue() ? wDigInt.cts:wDigInt.no );


  if( m_SubLib->GetSelection() == 0 )
    wDigInt.setsublib( m_Props, wDigInt.sublib_serial );
  else if( m_SubLib->GetSelection() == 1 )
    wDigInt.setsublib( m_Props, wDigInt.sublib_socket );
  else if( m_SubLib->GetSelection() == 2 )
    wDigInt.setsublib( m_Props, wDigInt.sublib_native );
  else if( m_SubLib->GetSelection() == 3 )
    wDigInt.setsublib( m_Props, wDigInt.sublib_udp );

  /* loconet node */
  {
    iONode loconet    = wDigInt.getloconet( m_Props );
    iONode options    = wLocoNet.getoptions( loconet );
    iONode slotserver = wLocoNet.getslotserver( loconet );

    wLocoNet.setpurgetime( loconet, atoi( m_PurgeTime->GetValue().mb_str(wxConvUTF8) ) );
    wDigInt.setfastclock( m_Props, m_UseFC->IsChecked() ? True:False );
    wLocoNet.setusefc(loconet, m_UseFC->IsChecked() ? True:False );
    wLocoNet.setsyncfc(loconet, m_SyncFC->IsChecked() ? True:False );
    wLocoNet.setignorepowercmds(loconet, m_IgnorePowerCmds->IsChecked() ? True:False );

    wLNSlotServer.setactive(slotserver, m_SlotServerActive->IsChecked() ? True:False );
    wLNSlotServer.setlconly(slotserver, m_MobileOnly->IsChecked() ? True:False );
    wLNSlotServer.setpurge(slotserver, m_PurgeSlots->IsChecked() ? True:False );
    wLNSlotServer.setstopatpurge(slotserver, m_StopAtPurge->IsChecked() ? True:False );

    wLNSlotServer.setiid( slotserver, m_DispatchIID->GetValue().mb_str(wxConvUTF8) );


    wCSOptions.setstore( options, m_SetOptions->IsChecked()?True:False );
    wLocoNet.setcmdstn( loconet, m_CmdStn->GetStringSelection().mb_str(wxConvUTF8) );

    char opsw[65] = {'\0'};
    MemOp.set( opsw, '0', 64 );
    opsw[64] = '\0';

    for( int i = 0; i < m_OptionsList->GetCount(); i++ ) {
      bool closed = m_OptionsList->IsChecked(i);
      opsw[optidx[i]] = closed?'1':'0';
    }
    TraceOp.trc( "lndlg", TRCLEVEL_INFO, __LINE__, 9999, "CS options [%s]", opsw);
    wCSOptions.setopsw( options, opsw );
  }

}


/*!
 * Dialog creator
 */

bool LocoNetCtrlDlg::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin LocoNetCtrlDlg creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end LocoNetCtrlDlg creation
    return true;
}

/*!
 * Member initialisation
 */

void LocoNetCtrlDlg::Init()
{
////@begin LocoNetCtrlDlg member initialisation
    m_InterfacePanel = NULL;
    m_labIID = NULL;
    m_IID = NULL;
    m_labDevice = NULL;
    m_Device = NULL;
    m_labHost = NULL;
    m_Host = NULL;
    m_labPort = NULL;
    m_Port = NULL;
    m_Baudrate = NULL;
    m_SubLib = NULL;
    m_Flow = NULL;
    m_IgnorePowerCmds = NULL;
    m_OptionsPanel = NULL;
    m_CmdStn = NULL;
    m_OptionsList = NULL;
    m_SetOptions = NULL;
    m_DetailsPanel = NULL;
    m_labSWTime = NULL;
    m_SWTime = NULL;
    m_labPurgeTime = NULL;
    m_PurgeTime = NULL;
    m_FastClockBox = NULL;
    m_UseFC = NULL;
    m_SyncFC = NULL;
    m_SlotServerBox = NULL;
    m_SlotServerActive = NULL;
    m_MobileOnly = NULL;
    m_PurgeSlots = NULL;
    m_StopAtPurge = NULL;
    m_labDispatchIID = NULL;
    m_DispatchIID = NULL;
    m_OK = NULL;
    m_Cancel = NULL;
////@end LocoNetCtrlDlg member initialisation
}

/*!
 * Control creation for Dialog
 */

void LocoNetCtrlDlg::CreateControls()
{
////@begin LocoNetCtrlDlg content construction
    LocoNetCtrlDlg* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    wxNotebook* itemNotebook3 = new wxNotebook( itemDialog1, ID_NOTEBOOK1, wxDefaultPosition, wxDefaultSize, m_TabAlign );

    m_InterfacePanel = new wxPanel( itemNotebook3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxVERTICAL);
    m_InterfacePanel->SetSizer(itemBoxSizer5);

    wxFlexGridSizer* itemFlexGridSizer6 = new wxFlexGridSizer(2, 2, 0, 0);
    itemFlexGridSizer6->AddGrowableCol(1);
    itemBoxSizer5->Add(itemFlexGridSizer6, 0, wxGROW|wxALL, 5);
    m_labIID = new wxStaticText( m_InterfacePanel, wxID_ANY, _("IID"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(m_labIID, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5);

    m_IID = new wxTextCtrl( m_InterfacePanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(m_IID, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5);

    m_labDevice = new wxStaticText( m_InterfacePanel, wxID_ANY, _("Device"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(m_labDevice, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_Device = new wxTextCtrl( m_InterfacePanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(m_Device, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_labHost = new wxStaticText( m_InterfacePanel, wxID_STATIC, _("Host"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(m_labHost, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_Host = new wxTextCtrl( m_InterfacePanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(m_Host, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_labPort = new wxStaticText( m_InterfacePanel, wxID_ANY, _("Port"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(m_labPort, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_Port = new wxTextCtrl( m_InterfacePanel, wxID_ANY, _("1234"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    m_Port->SetMaxLength(5);
    itemFlexGridSizer6->Add(m_Port, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    wxBoxSizer* itemBoxSizer15 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer5->Add(itemBoxSizer15, 0, wxGROW|wxALL, 5);
    wxArrayString m_BaudrateStrings;
    m_BaudrateStrings.Add(_("&19200"));
    m_BaudrateStrings.Add(_("&57600"));
    m_BaudrateStrings.Add(_("&115200"));
    m_Baudrate = new wxRadioBox( m_InterfacePanel, wxID_ANY, _("Baudrate"), wxDefaultPosition, wxDefaultSize, m_BaudrateStrings, 1, wxRA_SPECIFY_COLS );
    m_Baudrate->SetSelection(0);
    itemBoxSizer15->Add(m_Baudrate, 0, wxALIGN_TOP|wxLEFT|wxRIGHT, 5);

    wxArrayString m_SubLibStrings;
    m_SubLibStrings.Add(_("&LocoBuffer"));
    m_SubLibStrings.Add(_("&LbServer"));
    m_SubLibStrings.Add(_("&MS100"));
    m_SubLibStrings.Add(_("&LNUDP (MGV101)"));
    m_SubLib = new wxRadioBox( m_InterfacePanel, wxID_ANY, _("Type"), wxDefaultPosition, wxDefaultSize, m_SubLibStrings, 1, wxRA_SPECIFY_COLS );
    m_SubLib->SetSelection(0);
    itemBoxSizer15->Add(m_SubLib, 0, wxALIGN_TOP|wxLEFT|wxRIGHT, 5);

    m_Flow = new wxCheckBox( m_InterfacePanel, wxID_ANY, _("CTS Flow"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Flow->SetValue(true);
    itemBoxSizer5->Add(m_Flow, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_IgnorePowerCmds = new wxCheckBox( m_InterfacePanel, wxID_ANY, _("Ignore power commands"), wxDefaultPosition, wxDefaultSize, 0 );
    m_IgnorePowerCmds->SetValue(false);
    itemBoxSizer5->Add(m_IgnorePowerCmds, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    itemNotebook3->AddPage(m_InterfacePanel, _("Interface"));

    m_OptionsPanel = new wxPanel( itemNotebook3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer21 = new wxBoxSizer(wxVERTICAL);
    m_OptionsPanel->SetSizer(itemBoxSizer21);

    wxArrayString m_CmdStnStrings;
    m_CmdStn = new wxComboBox( m_OptionsPanel, ID_COMBOBOX_LOCONET_CMDSTN, _T(""), wxDefaultPosition, wxDefaultSize, m_CmdStnStrings, wxCB_DROPDOWN );
    itemBoxSizer21->Add(m_CmdStn, 0, wxGROW|wxALL, 5);

    wxArrayString m_OptionsListStrings;
    m_OptionsList = new wxCheckListBox( m_OptionsPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_OptionsListStrings, wxLB_SINGLE|wxLB_NEEDED_SB );
    itemBoxSizer21->Add(m_OptionsList, 1, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer24 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer21->Add(itemBoxSizer24, 0, wxGROW|wxALL, 5);
    m_SetOptions = new wxCheckBox( m_OptionsPanel, ID_CHECKBOX1, _("Store options"), wxDefaultPosition, wxDefaultSize, 0 );
    m_SetOptions->SetValue(false);
    itemBoxSizer24->Add(m_SetOptions, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    itemNotebook3->AddPage(m_OptionsPanel, _("Options"));

    m_DetailsPanel = new wxPanel( itemNotebook3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer27 = new wxBoxSizer(wxVERTICAL);
    m_DetailsPanel->SetSizer(itemBoxSizer27);

    wxFlexGridSizer* itemFlexGridSizer28 = new wxFlexGridSizer(2, 2, 0, 0);
    itemBoxSizer27->Add(itemFlexGridSizer28, 0, wxGROW|wxALL, 5);
    m_labSWTime = new wxStaticText( m_DetailsPanel, wxID_ANY, _("Switchtime"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer28->Add(m_labSWTime, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_SWTime = new wxTextCtrl( m_DetailsPanel, wxID_ANY, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    m_SWTime->SetMaxLength(4);
    itemFlexGridSizer28->Add(m_SWTime, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_labPurgeTime = new wxStaticText( m_DetailsPanel, wxID_ANY, _("Purgetime"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer28->Add(m_labPurgeTime, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_PurgeTime = new wxTextCtrl( m_DetailsPanel, wxID_ANY, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    m_PurgeTime->SetMaxLength(3);
    itemFlexGridSizer28->Add(m_PurgeTime, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_FastClockBox = new wxStaticBox(m_DetailsPanel, wxID_ANY, _("Fast Clock"));
    wxStaticBoxSizer* itemStaticBoxSizer33 = new wxStaticBoxSizer(m_FastClockBox, wxVERTICAL);
    itemBoxSizer27->Add(itemStaticBoxSizer33, 0, wxGROW|wxALL, 5);
    m_UseFC = new wxCheckBox( m_DetailsPanel, wxID_ANY, _("Enable"), wxDefaultPosition, wxDefaultSize, 0 );
    m_UseFC->SetValue(false);
    itemStaticBoxSizer33->Add(m_UseFC, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP, 5);

    m_SyncFC = new wxCheckBox( m_DetailsPanel, wxID_ANY, _("Synchronize"), wxDefaultPosition, wxDefaultSize, 0 );
    m_SyncFC->SetValue(false);
    itemStaticBoxSizer33->Add(m_SyncFC, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_SlotServerBox = new wxStaticBox(m_DetailsPanel, wxID_ANY, _("Slot server"));
    wxStaticBoxSizer* itemStaticBoxSizer36 = new wxStaticBoxSizer(m_SlotServerBox, wxVERTICAL);
    itemBoxSizer27->Add(itemStaticBoxSizer36, 0, wxGROW|wxALL, 5);
    m_SlotServerActive = new wxCheckBox( m_DetailsPanel, wxID_ANY, _("Active"), wxDefaultPosition, wxDefaultSize, 0 );
    m_SlotServerActive->SetValue(false);
    itemStaticBoxSizer36->Add(m_SlotServerActive, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP, 5);

    m_MobileOnly = new wxCheckBox( m_DetailsPanel, wxID_ANY, _("Mobile only"), wxDefaultPosition, wxDefaultSize, 0 );
    m_MobileOnly->SetValue(false);
    itemStaticBoxSizer36->Add(m_MobileOnly, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT, 5);

    m_PurgeSlots = new wxCheckBox( m_DetailsPanel, wxID_ANY, _("Purge slots"), wxDefaultPosition, wxDefaultSize, 0 );
    m_PurgeSlots->SetValue(false);
    itemStaticBoxSizer36->Add(m_PurgeSlots, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT, 5);

    m_StopAtPurge = new wxCheckBox( m_DetailsPanel, wxID_ANY, _("Stop at purge"), wxDefaultPosition, wxDefaultSize, 0 );
    m_StopAtPurge->SetValue(false);
    itemStaticBoxSizer36->Add(m_StopAtPurge, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT, 5);

    wxFlexGridSizer* itemFlexGridSizer41 = new wxFlexGridSizer(2, 2, 0, 0);
    itemFlexGridSizer41->AddGrowableCol(1);
    itemStaticBoxSizer36->Add(itemFlexGridSizer41, 0, wxGROW, 5);
    m_labDispatchIID = new wxStaticText( m_DetailsPanel, wxID_ANY, _("IID"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer41->Add(m_labDispatchIID, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_DispatchIID = new wxTextCtrl( m_DetailsPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer41->Add(m_DispatchIID, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    itemNotebook3->AddPage(m_DetailsPanel, _("Details"));

    itemBoxSizer2->Add(itemNotebook3, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxStdDialogButtonSizer* itemStdDialogButtonSizer44 = new wxStdDialogButtonSizer;

    itemBoxSizer2->Add(itemStdDialogButtonSizer44, 0, wxALIGN_RIGHT|wxALL, 5);
    m_OK = new wxButton( itemDialog1, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    m_OK->SetDefault();
    itemStdDialogButtonSizer44->AddButton(m_OK);

    m_Cancel = new wxButton( itemDialog1, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer44->AddButton(m_Cancel);

    itemStdDialogButtonSizer44->Realize();

////@end LocoNetCtrlDlg content construction
}

/*!
 * Should we show tooltips?
 */

bool LocoNetCtrlDlg::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap LocoNetCtrlDlg::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin LocoNetCtrlDlg bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end LocoNetCtrlDlg bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon LocoNetCtrlDlg::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin LocoNetCtrlDlg icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end LocoNetCtrlDlg icon retrieval
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
 */

void LocoNetCtrlDlg::OnOkClick( wxCommandEvent& event )
{
  evaluate();
  EndModal( wxID_OK );
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
 */

void LocoNetCtrlDlg::OnCancelClick( wxCommandEvent& event )
{
  EndModal( 0 );
}

/*!
 * wxEVT_COMMAND_COMBOBOX_SELECTED event handler for ID_COMBOBOX_LOCONET_CMDSTN
 */

void LocoNetCtrlDlg::OnComboboxLoconetCmdstnSelected( wxCommandEvent& event )
{
  initOptions();
}


/*!
 * wxEVT_COMMAND_RADIOBOX_SELECTED event handler for wxID_ANY
 */

void LocoNetCtrlDlg::OnSublibSelected( wxCommandEvent& event )
{
  initSublib();
}

void LocoNetCtrlDlg::initSublib()
{
  if( m_SubLib->GetSelection() == 0 ) {
    // LocoBuffer
    m_Baudrate->Enable(true);
    m_Device->Enable(true);
    m_Host->Enable(false);
    m_Port->Enable(false);
    m_Flow->Enable(true);
  }
  else if( m_SubLib->GetSelection() == 1 ) {
    // LBServer
    m_Baudrate->Enable(false);
    m_Device->Enable(false);
    m_Host->SetValue( wxString( wDigInt.gethost( m_Props ), wxConvUTF8 ) );
    char* val = StrOp.fmt( "%d", wDigInt.getport( m_Props ) );
    m_Port->SetValue( wxString( val, wxConvUTF8 ) ); StrOp.free( val );
    m_Host->Enable(true);
    m_Port->Enable(true);
    m_Flow->Enable(false);
  }
  else if( m_SubLib->GetSelection() == 2 ) {
    // MS100
    m_Baudrate->Enable(false);
    m_Device->Enable(true);
    m_Host->Enable(false);
    m_Port->Enable(false);
    m_Flow->Enable(true);
  }
  else if( m_SubLib->GetSelection() == 3 ) {
    // UDP
    m_Baudrate->Enable(false);
    m_Device->Enable(false);
    m_Host->SetValue( _T("224.0.0.1") );
    m_Port->SetValue( _T("1235") );
    m_Host->Enable(true);
    m_Port->Enable(true);
    m_Flow->Enable(false);
  }
}
