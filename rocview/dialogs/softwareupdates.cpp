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
#pragma implementation "softwareupdates.h"
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

#include "softwareupdates.h"

////@begin XPM images
////@end XPM images

#include "rocview/public/guiapp.h"
#include "rocview/wrapper/public/Gui.h"
#include "rocview/wrapper/public/Release.h"
#include "rocview/wrapper/public/Patch.h"

#include "rocs/public/socket.h"
#include "rocs/public/trace.h"
#include "rocs/public/thread.h"
#include "rocs/public/doc.h"
#include "rocs/public/system.h"

/*!
 * Softwareupdates type definition
 */

IMPLEMENT_DYNAMIC_CLASS( Softwareupdates, wxDialog )


/*!
 * Softwareupdates event table definition
 */

BEGIN_EVENT_TABLE( Softwareupdates, wxDialog )

////@begin Softwareupdates event table entries
    EVT_LISTBOX( wxID_ANY, Softwareupdates::OnAnySelected )
    EVT_CHECKLISTBOX( wxID_ANY, Softwareupdates::OnAnyToggled )

    EVT_BUTTON( wxID_OK, Softwareupdates::OnOkClick )

    EVT_BUTTON( wxID_CANCEL, Softwareupdates::OnCancelClick )

////@end Softwareupdates event table entries
    EVT_TIMER (ME_UpdateTimer, Softwareupdates::OnTimer)

END_EVENT_TABLE()


/*!
 * Softwareupdates constructors
 */

Softwareupdates::Softwareupdates()
{
    Init();
}

static void _updateLoader( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  Softwareupdates* o = (Softwareupdates*)ThreadOp.getParm( th );
  bool OK = true;
  
  if( o->m_ReleaseNode != NULL ) {
    iONode patch = wRelease.getpatch(o->m_ReleaseNode);
    while( patch!=NULL && OK ) {
      if(wPatch.isdownload(patch)) {
        TraceOp.trc( "updates", TRCLEVEL_INFO, __LINE__, 9999, "downloading [%s]...", wPatch.getfile(patch) );

        iOSocket sh = SocketOp.inst( wGui.getupdatesserver(wxGetApp().getIni()), 80, False, False, False );
        if( SocketOp.connect( sh ) ) {
          TraceOp.trc( "updates", TRCLEVEL_INFO, __LINE__, 9999, "Connected to rocrail.net" );

          char* releasename = StrOp.dup(wGui.releasename);
          StrOp.replaceAll( releasename, ' ', '\0');
          char* httpget = StrOp.fmt("GET /releases/%s/%s.%s.%s.%d HTTP/1.1\nHost: www.rocrail.net\n\n", 
              StrOp.strlwr( releasename ), wPatch.getfile(patch),
              wPatch.getos(patch), wRelease.getversion(o->m_ReleaseNode), wPatch.getnr(patch));
          TraceOp.trc( "updates", TRCLEVEL_INFO, __LINE__, 9999, "%s", httpget );
          SocketOp.write( sh, httpget, StrOp.len(httpget) );
          StrOp.free(releasename);
          StrOp.free(httpget);
          
          TraceOp.trc( "updates", TRCLEVEL_INFO, __LINE__, 9999, "Read response..." );
          char str[1024] = {'\0'};
          SocketOp.setRcvTimeout( sh, 1000 );
          /* Read first HTTP header line: */
          OK = false;
          SocketOp.readln( sh, str );
          TraceOp.trc( "updates", TRCLEVEL_INFO, __LINE__, 9999, str );
          if( StrOp.find( str, "200 OK" ) ) {
            OK = true;
            TraceOp.trc( "updates", TRCLEVEL_INFO, __LINE__, 9999, "OK" );
          }

          
          /* Reading rest of HTTP header: */

          int contlen = 0;
          while( SocketOp.readln( sh, str ) && !SocketOp.isBroken( sh ) ) {
            if( str[0] == '\r' || str[0] == '\n' ) {
              break;
            }
            if( StrOp.find( str, "Content-Length:" ) ) {
              char* p = StrOp.find( str, ": " ) + 2;
              contlen = atoi( p );
              TraceOp.trc( "updates", TRCLEVEL_INFO, __LINE__, 9999, "contlen = %d", contlen );
            }

            TraceOp.trc( "updates", TRCLEVEL_INFO, __LINE__, 9999, str );
          };
          
          if( OK && contlen > 0 ) {
            TraceOp.trc( "updates", TRCLEVEL_INFO, __LINE__, 9999, "downloading [%s]", wPatch.getfile(patch) );
            char* release = (char*)allocMem(contlen+1);
            SocketOp.read( sh, release, contlen );

            char* path = StrOp.fmt("%s%c%s.%s.%s.%d", 
                wGui.getupdatespath(wxGetApp().getIni()),
                SystemOp.getFileSeparator(),
                wPatch.getfile(patch), wPatch.getos(patch),
                wRelease.getversion(o->m_ReleaseNode), wPatch.getnr(patch));
            
            iOFile file = FileOp.inst(path, OPEN_WRITE );
            if(file != NULL) {
              FileOp.write( file, release, contlen);
              FileOp.base.del(file);
            }
            StrOp.free(path);
            freeMem(release);
          }

          
          
          SocketOp.disConnect(sh);
        }
        else {
          TraceOp.trc( "updates", TRCLEVEL_WARNING, __LINE__, 9999, "could not connected to %s",
              wGui.getupdatesserver(wxGetApp().getIni()) );
        }
        SocketOp.base.del(sh);
        
      }
      patch = wRelease.nextpatch(o->m_ReleaseNode, patch);
    }
  }
  
  
  o->setReady(OK);
}


static iONode __evaluate( const char* release, bool* newPatches ) {
  TraceOp.trc( "updates", TRCLEVEL_INFO, __LINE__, 9999, "parsing release info:\n%s", release );
  iODoc doc = DocOp.parse( release );
  iONode releaseNode = NULL;
  *newPatches = false;
  if( doc != NULL ) {
    releaseNode = DocOp.getRootNode(doc);
    if( releaseNode != NULL ) {
      TraceOp.trc( "updates", TRCLEVEL_INFO, __LINE__, 9999, 
          "release info %ld parsed and has %d entries", 
          wRelease.gettimestamp(releaseNode), NodeOp.getChildCnt(releaseNode) );
      if( wRelease.gettimestamp(releaseNode) > wGui.getupdatestimestamp(wxGetApp().getIni()) ) {
        *newPatches = true;
      }
    }
    else {
      TraceOp.trc( "updates", TRCLEVEL_WARNING, __LINE__, 9999, "empty release info?" );
    }
    DocOp.base.del(doc);
  }
  else {
    TraceOp.trc( "updates", TRCLEVEL_WARNING, __LINE__, 9999, "could not parse release info" );
  }
  return releaseNode;
}


void updateReaderThread( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  Softwareupdates* o = (Softwareupdates*)ThreadOp.getParm( th );
  bool newPatches = false;
  
  if( o == NULL ) {
    ThreadOp.sleep(5000);
  }
  
  iOSocket sh = SocketOp.inst( wGui.getupdatesserver(wxGetApp().getIni()), 80, False, False, False );
  if( SocketOp.connect( sh ) ) {
    if( o != NULL )
      o->setConnected(true);
    TraceOp.trc( "updates", TRCLEVEL_INFO, __LINE__, 9999, "Connected to rocrail.net" );
    
    char* releasename = StrOp.dup(wGui.releasename);
    StrOp.replaceAll( releasename, ' ', '\0');
    char* httpget = StrOp.fmt("GET /releases/%s.xml HTTP/1.1\nHost: www.rocrail.net\n\n", 
        StrOp.strlwr( releasename ) );
    TraceOp.trc( "updates", TRCLEVEL_INFO, __LINE__, 9999, "%s", httpget );
    SocketOp.write( sh, httpget, StrOp.len(httpget) );
    StrOp.free(releasename);
    StrOp.free(httpget);
    
    TraceOp.trc( "updates", TRCLEVEL_INFO, __LINE__, 9999, "Read response..." );
    char str[1024] = {'\0'};
    SocketOp.setRcvTimeout( sh, 1000 );
    /* Read first HTTP header line: */
    SocketOp.readln( sh, str );
    TraceOp.trc( "updates", TRCLEVEL_INFO, __LINE__, 9999, str );
    /* Reading rest of HTTP header: */

    int contlen = 0;
    while( SocketOp.readln( sh, str ) && !SocketOp.isBroken( sh ) ) {
      if( str[0] == '\r' || str[0] == '\n' ) {
        break;
      }
      if( StrOp.find( str, "Content-Length:" ) ) {
        char* p = StrOp.find( str, ": " ) + 2;
        contlen = atoi( p );
        TraceOp.trc( "updates", TRCLEVEL_INFO, __LINE__, 9999, "contlen = %d", contlen );
      }
      TraceOp.trc( "updates", TRCLEVEL_INFO, __LINE__, 9999, str );
    };
    if( contlen > 0 ) {
      char* release = (char*)allocMem(contlen+1);
      SocketOp.read( sh, release, contlen );
      TraceOp.trc( "updates", TRCLEVEL_INFO, __LINE__, 9999, release );
      iONode releasenode = __evaluate( release, &newPatches );
      if( o != NULL )
        o->m_ReleaseNode = releasenode;
    }

    SocketOp.disConnect(sh);
  }
  else {
    if( o != NULL )
      o->setConnected(false);
    TraceOp.trc( "updates", TRCLEVEL_WARNING, __LINE__, 9999, "could not connected to %s",
        wGui.getupdatesserver(wxGetApp().getIni()) );
    
  }
  if( newPatches ) {
    // inform toolbar
    TraceOp.trc( "updates", TRCLEVEL_INFO, __LINE__, 9999, "unviewed patches..." );
    
    wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, NEWUPDATES_EVENT );
    wxPostEvent( wxGetApp().getFrame(), event );
  }
  else {
    TraceOp.trc( "updates", TRCLEVEL_INFO, __LINE__, 9999, "no unviewed patches" );
  }
  
  if( o != NULL )
    o->setReady(true);
  
  SocketOp.base.del(sh);
}


Softwareupdates::Softwareupdates( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    Create(parent, -1, wxGetApp().getMsg("softwareupdates") );
    initLabels();

    m_Timer = new wxTimer( this, ME_UpdateTimer );
    m_Progress = NULL;
    m_TimerCount = 0;
    m_Ready = false;
    m_Connected = false;
    m_Release = NULL;
    m_ReleaseNode = NULL;

    checkForNew();
    
    bool rc = m_Timer->Start( 100, true );
    
    m_Progress = new wxProgressDialog(wxGetApp().getMsg( "softwareupdates" ), wxGetApp().getMsg( "searchingupdates" ), 
        2, NULL, wxPD_CAN_ABORT | wxPD_AUTO_HIDE | wxPD_APP_MODAL );
    m_Progress->ShowModal();

    initValues();
}

void Softwareupdates::initLabels() {
  m_labUpdates->SetLabel( wxGetApp().getMsg( "availableupdates" ) );
  m_labInfo->SetLabel( wxGetApp().getMsg( "updateinfo" ) );
}


void Softwareupdates::checkForNew() {
  iOThread updateReader = ThreadOp.inst( "update", updateReaderThread, this );
  ThreadOp.start( updateReader );
}


void Softwareupdates::initValues() {
  if( m_ReleaseNode != NULL && m_Updates != NULL ) {
    wGui.setupdatestimestamp(wxGetApp().getIni(), wRelease.gettimestamp(m_ReleaseNode) );
    m_Updates->Clear();
    
    wxString label = wxGetApp().getMsg( "availableupdates" );
    wxString version( wRelease.getversion(m_ReleaseNode), wxConvUTF8);
    wxString alias(wRelease.getalias(m_ReleaseNode), wxConvUTF8);
    
    m_labUpdates->SetLabel( label + _T(": ") + version + _T(" \'") + alias + _T(" \'") );
    
    iONode patch = wRelease.getpatch(m_ReleaseNode);
    while( patch!=NULL ) {
      char patchStr[256] = {'\0'};
      StrOp.fmtb( patchStr, "%s %s.%d %s", 
          wPatch.getfile(patch), wRelease.getversion(m_ReleaseNode), 
          wPatch.getnr(patch), wPatch.getos(patch) );
      m_Updates->Append( wxString( patchStr,wxConvUTF8) );
      patch = wRelease.nextpatch(m_ReleaseNode, patch);
    }
  }
  else if( !m_Connected ) {
    m_Info->SetValue( wxGetApp().getMsg( "unabletoconnect" ) + 
        wxString( wGui.getupdatesserver(wxGetApp().getIni()),wxConvUTF8) );
  }
}


void Softwareupdates::setReady(bool ok) {
  TraceOp.trc( "updates", TRCLEVEL_INFO, __LINE__, 9999, "Ready: [%s]", ok?"Successful":"Error" );
  m_DownloadOK = ok;
  m_Ready = true;
}


void Softwareupdates::setConnected(bool ok) {
  TraceOp.trc( "updates", TRCLEVEL_INFO, __LINE__, 9999, "Connected: [%s]", ok?"Successful":"Error" );
  m_Connected = ok;
}


void Softwareupdates::OnTimer(wxTimerEvent& event) {
  if( m_Ready ) {
    if( m_Progress != NULL ) {
      m_Progress->EndModal(0);
      m_Progress->Destroy();
    }
  }
  else {
    if( m_Progress != NULL && !m_Progress->Pulse() ) {
      m_Progress->EndModal(0);
      m_Progress->Destroy();
      m_Timer->Stop();
    }
    else {
      bool rc = m_Timer->Start( 100, true );
    }
  }
}


/*!
 * Softwareupdates creator
 */

bool Softwareupdates::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin Softwareupdates creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end Softwareupdates creation
    return true;
}


/*!
 * Softwareupdates destructor
 */

Softwareupdates::~Softwareupdates()
{
////@begin Softwareupdates destruction
////@end Softwareupdates destruction
}


/*!
 * Member initialisation
 */

void Softwareupdates::Init()
{
////@begin Softwareupdates member initialisation
    m_labUpdates = NULL;
    m_Updates = NULL;
    m_labInfo = NULL;
    m_Info = NULL;
    m_OK = NULL;
    m_Cancel = NULL;
////@end Softwareupdates member initialisation
}


/*!
 * Control creation for Softwareupdates
 */

void Softwareupdates::CreateControls()
{    
////@begin Softwareupdates content construction
    Softwareupdates* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    m_labUpdates = new wxStaticText( itemDialog1, wxID_ANY, _("Available updates"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer2->Add(m_labUpdates, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP, 5);

    wxArrayString m_UpdatesStrings;
    m_Updates = new wxCheckListBox( itemDialog1, wxID_ANY, wxDefaultPosition, wxSize(300, 100), m_UpdatesStrings, wxLB_SINGLE|wxLB_ALWAYS_SB );
    itemBoxSizer2->Add(m_Updates, 1, wxGROW|wxALL, 5);

    m_labInfo = new wxStaticText( itemDialog1, wxID_ANY, _("Update info"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer2->Add(m_labInfo, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP, 5);

    m_Info = new wxTextCtrl( itemDialog1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY );
    m_Info->Enable(false);
    itemBoxSizer2->Add(m_Info, 0, wxGROW|wxALL, 5);

    wxStdDialogButtonSizer* itemStdDialogButtonSizer7 = new wxStdDialogButtonSizer;

    itemBoxSizer2->Add(itemStdDialogButtonSizer7, 0, wxALIGN_RIGHT|wxALL, 5);
    m_OK = new wxButton( itemDialog1, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    m_OK->Enable(false);
    itemStdDialogButtonSizer7->AddButton(m_OK);

    m_Cancel = new wxButton( itemDialog1, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer7->AddButton(m_Cancel);

    itemStdDialogButtonSizer7->Realize();

////@end Softwareupdates content construction
}


/*!
 * Should we show tooltips?
 */

bool Softwareupdates::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap Softwareupdates::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin Softwareupdates bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end Softwareupdates bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon Softwareupdates::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin Softwareupdates icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end Softwareupdates icon retrieval
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
 */

void Softwareupdates::OnOkClick( wxCommandEvent& event )
{
  const char* path = wGui.getupdatespath(wxGetApp().getIni());
  if( !FileOp.exist(path) )
    FileOp.mkdir(path);
    
  /* download */
  m_Ready = false;
  iOThread updateLoader = ThreadOp.inst( "update", _updateLoader, this );
  ThreadOp.start( updateLoader );
  
  bool rc = m_Timer->Start( 100, true );
  
  m_Progress = new wxProgressDialog(wxGetApp().getMsg( "softwareupdates" ), wxGetApp().getMsg( "downloadingupdates" ), 
      2, NULL, wxPD_CAN_ABORT | wxPD_AUTO_HIDE | wxPD_APP_MODAL );
  m_Progress->ShowModal();

  if(m_Ready && m_DownloadOK ) {
    wxMessageDialog( this, wxGetApp().getMsg("successfullydownloaded"), 
        wxGetApp().getMsg("softwareupdates"), wxOK | wxICON_INFORMATION ).ShowModal();
  }
  else {
    wxMessageDialog( this, wxGetApp().getMsg("errorondownloading"), 
        wxGetApp().getMsg("softwareupdates"), wxOK | wxICON_ERROR ).ShowModal();
  }
  
  NodeOp.base.del( m_ReleaseNode );
  EndModal(wxID_OK);
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
 */

void Softwareupdates::OnCancelClick( wxCommandEvent& event )
{
  NodeOp.base.del( m_ReleaseNode );
  EndModal(0);
}


/*!
 * wxEVT_COMMAND_LISTBOX_SELECTED event handler for wxID_ANY
 */

void Softwareupdates::OnAnySelected( wxCommandEvent& event )
{
  int selection = m_Updates->GetSelection();
  if( selection != wxNOT_FOUND && m_ReleaseNode != NULL ) {
    iONode patch = NodeOp.getChild(m_ReleaseNode, selection); 
    if( patch!=NULL) {
      m_Info->SetValue( wxString( wPatch.getremark(patch),wxConvUTF8) );
    }
    else {
      
    }
  }
}


/*!
 * wxEVT_COMMAND_CHECKLISTBOX_TOGGLED event handler for wxID_ANY
 */

void Softwareupdates::OnAnyToggled( wxCommandEvent& event )
{
  m_OK->Enable(false);
  if( m_ReleaseNode == NULL )
    return;
  
  int count = m_Updates->GetCount();
  for( int i = 0; i < count; i++ ) {
    iONode patch = NodeOp.getChild(m_ReleaseNode, i); 
    if( patch != NULL ) {
      wPatch.setdownload(patch, m_Updates->IsChecked(i)?True:False);
    }
    if( m_Updates->IsChecked(i) ) {
      m_OK->Enable(true);
    }
  }
}

