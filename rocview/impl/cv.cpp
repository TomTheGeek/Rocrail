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
#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "cv.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/defs.h"
#endif

#include "rocs/public/trace.h"
#include "rocs/public/strtok.h"
#include "rocview/public/guiapp.h"
#include "rocview/dialogs/locdialog.h"
#include "rocview/dialogs/speedcurvedlg.h"
#include "rocview/dialogs/decconfigdlg.h"
#include "rocview/dialogs/fxdlg.h"

#include "rocview/public/cv.h"

#include "rocrail/wrapper/public/Program.h"
#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/RocRail.h"
#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/Plan.h"
#include "rocrail/wrapper/public/CVByte.h"
#include "rocrail/wrapper/public/ModelCmd.h"

#include "rocview/wrapper/public/Gui.h"
#include "rocview/wrapper/public/CVconf.h"

#include "rocutils/public/vendors.h"

/*!
 * Programming type definition
 */


/*!
 * Programming constructors
 */

CV::CV( wxScrolledWindow* parent, iONode cvconf, wxWindow* frame ) {
  m_Parent = parent;
  m_Frame = frame;
  m_LocProps = NULL;
  Create();
  m_CVconf = cvconf;
  m_CVidx = 0;
  m_CVcountAll = 0;
  m_CVoperation = 0;
  m_bCleanUpProgress = false;
  m_TimerMutex = MutexOp.inst( NULL, True );

  for(int i = 0; i < 28; i++ ) {
    m_Curve[i] = 0;
  }
  m_bSpeedCurve  = false;
  m_bConfig      = false;
  m_bFX          = false;
  m_bLongAddress = false;


  CVconf();
  m_Timer = new wxTimer( frame, ME_CVTimer );
  m_Progress = NULL;

  __initVendors();

}


void CV::CVconf() {
  if( m_CVconf == NULL )
    m_CVconf = NodeOp.inst( wCVconf.name(), NULL, ELEMENT_NODE );

  const char* nrs = wCVconf.getnrs( m_CVconf );
  iOStrTok tok = StrTokOp.inst( nrs, ',' );
  int nridx = 0;
  const char* nr = StrTokOp.nextToken(tok);

  while( nr != NULL ) {
    int cvnr = atoi(nr);
    m_CVall[nridx] = cvnr;
    TraceOp.trc( "cv", TRCLEVEL_INFO, __LINE__, 9999, "m_CVall[%d]=%d", nridx, cvnr );
    nridx++;
    nr = StrTokOp.nextToken(tok);
  };
  StrTokOp.base.del( tok );
  m_CVcountAll = nridx;

}


static int locComparator(obj* o1, obj* o2) {
  if( *o1 == NULL || *o2 == NULL )
    return 0;
  return strcmp( wLoc.getid( (iONode)*o1 ), wLoc.getid( (iONode)*o2 ) );
}

void CV::init() {
  iONode model = wxGetApp().getModel();
  m_LcList->Clear();
  m_LcList->Append( _(""), (void*)NULL );
  if( model != NULL ) {
    iONode lclist = wPlan.getlclist( model );
    if( lclist != NULL ) {
      int i;
      int cnt = NodeOp.getChildCnt( lclist );
      iOList list = ListOp.inst();
      TraceOp.trc( "cv", TRCLEVEL_INFO, __LINE__, 9999, "init %d locs...", cnt );

      for( i = 0; i < cnt; i++ ) {
        iONode lc = NodeOp.getChild( lclist, i );
        ListOp.add( list, (obj)lc );
      }
      // Sort the list:
      ListOp.sort( list, locComparator );

      for( i = 0; i < ListOp.size( list ); i++ ) {
        iONode lc = (iONode)ListOp.get( list, i );
        if( lc == NULL )
          continue;
        const char* id = wLoc.getid( lc );
      }

      for( i = 0; i < cnt; i++ ) {
        iONode lc = (iONode)ListOp.get( list, i );
        if( lc == NULL )
          continue;
        const char* id = wLoc.getid( lc );
        if( id != NULL ) {
          m_LcList->Append( wxString(id,wxConvUTF8), lc );
        }
      }
      if( cnt > 0 ) {
        m_LcList->SetSelection(0);
        initValues();
      }

      ListOp.base.del( list );
    }
  }
  m_bPOM = False;
  m_bAcc = False;
  m_bDirect = False;
  m_Acc->Enable(m_POM->IsChecked());
}


void CV::initValues() {
  if( m_LcList->GetSelection() == wxNOT_FOUND )
    return;
  TraceOp.trc( "cv", TRCLEVEL_INFO, __LINE__, 9999, "initValues..." );
  setLocProps((iONode)((wxItemContainer*)m_LcList)->GetClientData(m_LcList->GetSelection()));
}

void CV::initPresets( int nr, const char* val ) {
  if( nr < 9 ) {
    wxTextCtrl* tc = (wxTextCtrl*)wxWindow::FindWindowById( nr + VAL_CV, m_Parent );
    if( tc != NULL ) {
      tc->SetValue( wxString( val,wxConvUTF8) );
      if( nr == 8 )
        tc->SetToolTip(wxString( m_Vendor[atoi(val)&0xFF],wxConvUTF8));
    }
  }
}

void CV::reset() {
  m_CVTable->ClearGrid();
  m_CVTable->EnableEditing(false);

  /* reset */
  m_CVaddress->SetValue( _T("0") );
  m_CVVstart->SetValue( _T("0") );
  m_CVaccel->SetValue( _T("0") );
  m_CVdecel->SetValue( _T("0") );
  m_CVVhigh->SetValue( _T("0") );
  m_CVVmid->SetValue( _T("0") );
  m_CVversion->SetValue( _T("0") );
  m_CVmanufacturedID->SetValue( _T("0") );
  m_CVmanufacturedID->SetToolTip(wxString( m_Vendor[0],wxConvUTF8));

}

void CV::setLocProps( iONode props ) {
  reset();

  if( props != NULL ) {
    m_LocProps = props;
    m_saveAllCVs->Enable(true);
    m_LcList->SetStringSelection(wxString(wLoc.getid(props),wxConvUTF8));

    char* str = StrOp.fmt( "%d", wLoc.getaddr( m_LocProps ) );
    if( wLoc.getaddr( m_LocProps ) < 128 ) {
      m_CVaddress->SetValue( wxString(str,wxConvUTF8) );
      m_CVlongaddress->SetValue( _T("0") );
    }
    else {
      m_CVaddress->SetValue( _T("0") );
      m_CVlongaddress->SetValue( wxString(str,wxConvUTF8) );
    }
    StrOp.free( str );

    iONode cv = wLoc.getcvbyte( m_LocProps );
    while( cv != NULL ) {
      int cvnr = wCVByte.getnr( cv );
      str = StrOp.fmt( "%d", wCVByte.getvalue( cv ) );
      char* rowstr = StrOp.fmt( "%d", cvnr );
      m_CVTable->SetCellValue(cvnr-1, 0, wxString(str,wxConvUTF8) );
      m_CVTable->SetRowLabelValue( cvnr-1, wxString(rowstr,wxConvUTF8) );
      initPresets( cvnr, str );
      StrOp.free( rowstr );
      StrOp.free( str );

      cv = wLoc.nextcvbyte( m_LocProps, cv );
    };
  }
  else {
    m_LocProps = NULL;
    m_saveAllCVs->Enable(false);

  }
}


void CV::event( iONode event ) {
  int cmd = wProgram.getcmd(event);
  int cv  = wProgram.getcv (event);
  int ivalue = wProgram.getvalue(event);
  bool datarsp = false;

  if( !StrOp.equals(NodeOp.getName(event), wProgram.name() ) ) {
    TraceOp.trc( "cv", TRCLEVEL_INFO, __LINE__, 9999,
      "got a none program event...[%s]", NodeOp.getName(event) );
    return;
  }

  if( cmd == wProgram.datarsp || cmd == wProgram.statusrsp )
    datarsp = true;

  if( cv == 0 && datarsp )
    cv = m_CVidx;

  TraceOp.trc( "cv", ivalue != -1 ? TRCLEVEL_INFO:TRCLEVEL_WARNING, __LINE__, 9999,
      "got program event...cmd=%d cv=%d value=%d %s", cmd, cv, ivalue, cmd == wProgram.datarsp ? "datarsp":"statusrsp" );

  if( m_bSpeedCurve && ivalue != -1 && (cmd == wProgram.statusrsp) && m_CVidx >= 67 && m_CVidx <= 94 ) {
    if(m_CVidx < 94 ) {
      m_CVoperation = CVSET;
      m_TimerCount = 0;
      doCV( wProgram.set, m_CVidx + 1, m_Curve[m_CVidx-67] );
    }
    else if(m_CVidx == 94 ) {
      /* post an event to activate the speed curve dialog */
      m_bSpeedCurve = false;
    }
  }
  else if( ivalue != -1 && cmd == wProgram.datarsp ) {
    TraceOp.trc( "cv", TRCLEVEL_INFO, __LINE__, 9999, "event for cv %d...", m_CVidx);
    char* val = StrOp.fmt( "%d", ivalue );

    /*
     * CV18 = addr - 256 * (addr / 256)
     * CV17 = (addr / 256) + 192
     */
    if( m_CVidx == 17 ) {
      m_CV17 = ivalue;
      int laddr = (m_CV17&0x3f) * 256 + m_CV18;
      TraceOp.trc( "cv", TRCLEVEL_INFO, __LINE__, 9999, "part 1 of long address(%d) cv%d=%d",
          laddr, m_CVidx, ivalue );
      char* lval = StrOp.fmt("%d", laddr);
      m_CVlongaddress->SetValue( wxString( lval,wxConvUTF8) );
      StrOp.free(lval);
      doCV( wProgram.get, 18, 0 );
    }
    else if( m_CVidx == 18 ) {
      m_CV18 = ivalue;
      int laddr = (m_CV17&0x3f) * 256 + m_CV18;
      TraceOp.trc( "cv", TRCLEVEL_INFO, __LINE__, 9999, "part 2 of long address(%d) cv%d=%d",
          laddr, m_CVidx, ivalue );
      char* lval = StrOp.fmt("%d", laddr);
      m_CVlongaddress->SetValue( wxString( lval,wxConvUTF8) );
      StrOp.free(lval);
      m_bLongAddress = false;
    }
    else if( m_CVidx >= 67 && m_CVidx <= 94 ) {
      m_Curve[m_CVidx-67] = ivalue;
      if(m_CVidx < 94 && m_bSpeedCurve ) {
        m_CVoperation = CVGET;
        m_TimerCount = 0;
        doCV( wProgram.get, m_CVidx + 1, 0 );
      }
      else if(m_CVidx == 94 && m_bSpeedCurve ) {
        /* post an event to activate the speed curve dialog */
        m_bSpeedCurve = false;
        m_Timer->Stop();
        onSpeedCurve();
      }
    }
    else if( m_CVidx == 29 && m_bConfig ) {
      /* post an event to activate the speed curve dialog */
      m_ConfigVal = ivalue;
      m_Timer->Stop();
      onDecConfig();
    }
    else if( m_bFX ) {
      /* post an event to activate the speed curve dialog */
      m_FxVal = ivalue;
      m_Timer->Stop();
      onDecFX();
    }
    else {
      wxTextCtrl* tc = (wxTextCtrl*)wxWindow::FindWindowById( m_CVidx + VAL_CV, m_Parent );
      if( tc != NULL ) {
        tc->SetValue( wxString( val,wxConvUTF8) );
        if( cv == 8 ) {
          tc->SetToolTip(wxString( m_Vendor[ivalue&0xFF],wxConvUTF8));
          TraceOp.trc( "cv", TRCLEVEL_INFO, __LINE__, 9999, "DCC Manufacturer: %s", m_Vendor[ivalue&0xFF] );
          wLoc.setdectype(m_LocProps, m_Vendor[ivalue&0xFF] );
        }
      }
    }

    if( m_CVnr->GetValue() == m_CVidx ) {
      m_CVvalue->SetValue( wxString( val,wxConvUTF8) );
      updateCVbits();
    }

    char* rowstr = StrOp.fmt( "%d", m_CVidx );
    if( m_CVidx > 0 && m_CVidx < 1024 ) {
      m_CVTable->SetRowLabelValue( m_CVidx-1, wxString(rowstr,wxConvUTF8) );
      m_CVTable->SetCellValue(m_CVidx-1, 0, wxString(val,wxConvUTF8) );
    }
    StrOp.free( rowstr );

    StrOp.free( val );

  }
  else if( cmd == wProgram.statusrsp ) {
    wxTextCtrl* tc = (wxTextCtrl*)wxWindow::FindWindowById( m_CVidx + VAL_CV, m_Parent );
    if( tc != NULL ) {
      char* rowstr = StrOp.fmt( "%d", m_CVidx );
      m_CVTable->SetRowLabelValue( m_CVidx-1, wxString(rowstr,wxConvUTF8) );
      m_CVTable->SetCellValue(m_CVidx-1, 0, tc->GetValue() );
      StrOp.free( rowstr );
    }
  }
  else if( cmd == wProgram.save ) {
  }



  if( datarsp && !m_bSpeedCurve ) {
    if( m_CVoperation == CVGET || m_CVoperation == CVSET ) {
      stopProgress();
    }

    if( m_CVoperation == CVGETALL || m_CVoperation == CVSETALL ) {
      wxCommandEvent event( wxEVT_COMMAND_BUTTON_CLICKED, -1 );
      event.SetEventObject( m_CVoperation == CVGETALL ? m_ReadAll:m_WriteAll );
      wxPostEvent( m_Parent, event );
    }
  }

  NodeOp.base.del( event );
}


void CV::onDecConfig(void) {
  TraceOp.trc( "cv", TRCLEVEL_INFO, __LINE__, 9999, "ConfigVal 1 (%d)", m_ConfigVal );
  DecConfigDlg*  dlg = new DecConfigDlg(m_Frame, m_ConfigVal );
  int rc = dlg->ShowModal();
  if( rc == wxID_OK ) {
    m_ConfigVal = dlg->getConfig();
    TraceOp.trc( "cv", TRCLEVEL_INFO, __LINE__, 9999, "ConfigVal 2 (%d)", m_ConfigVal );
    m_CVoperation = CVSET;
    doCV( wProgram.set, 29, m_ConfigVal );
    if(wCVconf.islissy( m_CVconf )) {
      doCV( wProgram.set, 129, m_ConfigVal & 0x20 ? 32:0 );
    }
  }
  TraceOp.trc( "cv", TRCLEVEL_INFO, __LINE__, 9999, "ConfigVal 3 (%d) rc=%d(%d)", m_ConfigVal, rc, dlg->GetReturnCode() );
  dlg->Destroy();
  m_bConfig = false;
}


void CV::onDecFX(void) {
  TraceOp.trc( "cv", TRCLEVEL_INFO, __LINE__, 9999, "FXVal (%d)", m_FxVal );
  FxDlg*  dlg = new FxDlg(m_Frame, m_FxVal, m_CVnr );
  int rc = dlg->ShowModal();
  if( rc == wxID_OK ) {
    m_FxVal = dlg->getConfig();
    TraceOp.trc( "cv", TRCLEVEL_INFO, __LINE__, 9999, "FxVal (%d)", m_FxVal );
    m_CVoperation = CVSET;
    doCV( wProgram.set, m_CVnr->GetValue(), m_FxVal );
  }
  TraceOp.trc( "cv", TRCLEVEL_INFO, __LINE__, 9999, "FxVal (%d) rc=%d(%d)", m_FxVal, rc, dlg->GetReturnCode() );
  dlg->Destroy();
  m_bFX = false;
}


void CV::update4POM(void) {
  m_bPOM = m_POM->IsChecked()?True:False;
  m_bAcc = m_Acc->IsChecked()?True:False;
  m_Acc->Enable(m_POM->IsChecked());
  /*
  m_getAddress->Enable(!m_bPOM);
  m_getVstart->Enable(!m_bPOM);
  m_getAccel->Enable(!m_bPOM);
  m_getDecel->Enable(!m_bPOM);
  m_getVhigh->Enable(!m_bPOM);
  m_getVmid->Enable(!m_bPOM);
  m_getVersion->Enable(!m_bPOM);
  m_getManu->Enable(!m_bPOM);
  m_Get->Enable(!m_bPOM);
  */
  m_setlongAddress->Enable(!m_bPOM);
  m_setAddress->Enable(!m_bPOM);

  m_ReadAll->Enable(!m_bPOM);
  m_PTonoff->Enable(!m_bPOM);
}


void CV::updateCVval(void) {
  int val = 0;
  val |= m_bit0->IsChecked() ? 0x01:0;
  val |= m_bit1->IsChecked() ? 0x02:0;
  val |= m_bit2->IsChecked() ? 0x04:0;
  val |= m_bit3->IsChecked() ? 0x08:0;
  val |= m_bit4->IsChecked() ? 0x10:0;
  val |= m_bit5->IsChecked() ? 0x20:0;
  val |= m_bit6->IsChecked() ? 0x40:0;
  val |= m_bit7->IsChecked() ? 0x80:0;

  char* str = StrOp.fmt( "%d", val );
  m_CVvalue->SetValue( wxString(str,wxConvUTF8) );
  StrOp.free( str );
}


void CV::updateCVbits(void) {
  int val = atoi( m_CVvalue->GetValue().mb_str(wxConvUTF8) );

  m_bit0->SetValue( val&0x01? true:false );
  m_bit1->SetValue( val&0x02? true:false );
  m_bit2->SetValue( val&0x04? true:false );
  m_bit3->SetValue( val&0x08? true:false );
  m_bit4->SetValue( val&0x10? true:false );
  m_bit5->SetValue( val&0x20? true:false );
  m_bit6->SetValue( val&0x40? true:false );
  m_bit7->SetValue( val&0x80? true:false );
}


void CV::OnGrid(wxGridEvent& event) {
  if ( event.GetEventObject() == m_CVTable ) {
    int cvnr = event.GetRow();
    m_CVTable->SelectRow(cvnr);
    m_CVTable->MakeCellVisible( cvnr, 0 );
    wxString value = m_CVTable->GetCellValue( cvnr, 0 );
    m_CVvalue->SetValue( value );

    char* str = StrOp.fmt( "%d", cvnr+1 );
    m_CVnr->SetValue( wxString(str,wxConvUTF8) );
    StrOp.free( str );

    updateCVbits();
  }
}


void CV::startProgress() {
  if( m_Progress == NULL ) {
    m_Progress = new wxProgressDialog(wxGetApp().getMsg( "cvget" ), wxGetApp().getMsg( "waitforpt" ),
        wCVconf.gettimeout(m_CVconf), NULL, wxPD_CAN_ABORT | wxPD_AUTO_HIDE | wxPD_APP_MODAL );
    TraceOp.trc( "cv", TRCLEVEL_INFO, __LINE__, 9999, "bringing up the progress dialog...0x%08X", m_Progress);
    m_Progress->ShowModal();
  }
}

void CV::stopProgress() {
  if( m_Progress != NULL ) {
    TraceOp.trc( "cv", TRCLEVEL_INFO, __LINE__, 9999, "end progress dialog" );
    m_bCleanUpProgress = true;
    m_bSpeedCurve = false;
    m_Timer->Start( 10, wxTIMER_ONE_SHOT );
  }
}

void CV::loadCVfromFile() {
  const char* l_openpath = wGui.getopenpath( wxGetApp().getIni() );
  TraceOp.trc( "cv", TRCLEVEL_INFO, __LINE__, 9999, "openpath=%s", l_openpath );
  wxString ms_FileExt = _T("LocoIO (*.csv)|*.csv");
  wxFileDialog* fdlg = new wxFileDialog(wxGetApp().getFrame(), wxGetApp().getMenu("opencvfile"),
      wxString(l_openpath,wxConvUTF8) , _T(""), ms_FileExt, wxFD_OPEN);
  if( fdlg->ShowModal() == wxID_OK ) {
    reset();

    //fdlg->GetPath();
    wGui.setopenpath( wxGetApp().getIni(), fdlg->GetPath().mb_str(wxConvUTF8) );
    // strip filename:
    wGui.setopenpath( wxGetApp().getIni(), FileOp.getPath(wGui.getopenpath( wxGetApp().getIni() ) ) );

    TraceOp.trc( "locoio", TRCLEVEL_INFO, __LINE__, 9999, "reading [%s]...", (const char*)fdlg->GetPath().mb_str(wxConvUTF8));
    iOFile f = FileOp.inst( fdlg->GetPath().mb_str(wxConvUTF8), OPEN_READONLY );
    if( f != NULL ) {
      TraceOp.trc( "locoio", TRCLEVEL_INFO, __LINE__, 9999, "file opened...");
      FILE* fs = FileOp.getStream(f);
      char str[32];
      fgets( str, 32, fs );
      int cvIndex = 0;

      // reset array
      MemOp.set( m_CVall, 0, sizeof(m_CVall) );

      /* until end of stream or error: */
      while( !ferror(fs) && !feof(fs) ) {
        TraceOp.trc( "locoio", TRCLEVEL_INFO, __LINE__, 9999, "line=[%s]", str);
        int sv  = 0;
        int val = 0;
        iOStrTok strtok = StrTokOp.inst( str, ',' );
        if( StrTokOp.hasMoreTokens( strtok ) ) {
          const char* ssv  = StrTokOp.nextToken( strtok );
          if( StrTokOp.hasMoreTokens( strtok ) ) {
            const char* sval = StrTokOp.nextToken( strtok );
            sv = atoi( ssv );
            val = atoi( sval );

            m_CVall[cvIndex] = sv;
            cvIndex++;

            char* valstr = StrOp.fmtb( str, "%d", val );
            char* rowstr = StrOp.fmt( "%d", sv );
            m_CVTable->SetCellValue(sv-1, 0, wxString(valstr,wxConvUTF8) );
            m_CVTable->SetRowLabelValue( sv-1, wxString(rowstr,wxConvUTF8) );
            initPresets( sv, valstr );
            StrOp.free( rowstr );
            StrOp.free( valstr );

            // call event method...

            TraceOp.trc( "cv", TRCLEVEL_INFO, __LINE__, 9999, "read cv=%d val=%d", sv, val);
          }
        }
        StrTokOp.base.del( strtok );

        fgets( str, 32, fs );
      };
      m_CVcountAll = cvIndex;

      FileOp.base.del( f );
    }

    m_CVidx = 0;
    m_CVidxAll = 0;
    m_CVoperation = CVSETALL;
    writeAll();


  }
  fdlg->Destroy();
}

void CV::saveCVtoFile() {
  const char* l_openpath = wGui.getopenpath( wxGetApp().getIni() );
  TraceOp.trc( "cv", TRCLEVEL_INFO, __LINE__, 9999, "openpath=%s", l_openpath );
  wxString ms_FileExt = _T("LocoIO (*.csv)|*.csv");
  wxFileDialog* fdlg = new wxFileDialog(wxGetApp().getFrame(), wxGetApp().getMenu("savelocoiofileas"),
      wxString(l_openpath,wxConvUTF8),
                       wxString::Format( _T("decoder.csv")), ms_FileExt, wxFD_SAVE);
  if( fdlg->ShowModal() == wxID_OK ) {
    // Check for existence.
    wxString path = fdlg->GetPath();
    if( FileOp.exist( path.mb_str(wxConvUTF8) ) ) {
      int action = wxMessageDialog( wxGetApp().getFrame(), wxGetApp().getMsg("fileexistwarning"), _T("Rocrail"), wxYES_NO | wxICON_EXCLAMATION ).ShowModal();
      if( action == wxID_NO ) {
        fdlg->Destroy();
        return;
      }
    }
    if( !path.Contains( _T(".csv") ) )
      path.Append( _T(".csv") );

    iOFile f = FileOp.inst( path.mb_str(wxConvUTF8), OPEN_WRITE );
    if( f != NULL ) {
      char s[32];
      for( int i = 0; i < 1024; i++ ) {
        // iterate all cv's
        if( !m_CVTable->GetCellValue( i, 0 ).IsEmpty() ) {
          StrOp.fmtb( s, " %d, %d\r\n", i+1, atoi( m_CVTable->GetCellValue( i, 0 ).mb_str(wxConvUTF8) ) );
          FileOp.writeStr( f, s );
        }
      }

      FileOp.base.del( f );
    }
  }
  fdlg->Destroy();
}



void CV::OnButton(wxCommandEvent& event)
{
  TraceOp.trc( "cv", TRCLEVEL_INFO, __LINE__, 9999, "button event: 0x%08X", event.GetEventObject() );

  if( event.GetEventObject() == m_loadFile ) {
    loadCVfromFile();
  }
  else if( event.GetEventObject() == m_saveFile ) {
    saveCVtoFile();
  }

  else if ( event.GetEventObject() == m_PTonoff ) {
    bool on = m_PTonoff->GetValue();
    m_PTonoff->SetLabel( on ? _T("PT on"):_T("PT off") );
    TraceOp.trc( "cv", TRCLEVEL_INFO, __LINE__, 9999, "PT %s", on?"on":"off" );
    iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
    wProgram.setcmd( cmd, wProgram.ptstat );
    wxGetApp().sendToRocrail( cmd );
    wProgram.setcmd( cmd, on?wProgram.pton:wProgram.ptoff );
    TraceOp.trc( "cv", TRCLEVEL_INFO, __LINE__, 9999, "PT send to Rocrail" );
    wxGetApp().sendToRocrail( cmd );
    TraceOp.trc( "cv", TRCLEVEL_INFO, __LINE__, 9999, "PT sended to Rocrail" );
    cmd->base.del(cmd);
    TraceOp.trc( "cv", TRCLEVEL_INFO, __LINE__, 9999, "PT cleaned up" );
  }
  else if ( event.GetEventObject() == m_saveCVs && m_CVnr->GetValue() > 0 ) {
    m_CVoperation = 0;
    doCV( wProgram.save, m_CVnr->GetValue(), atoi( m_CVvalue->GetValue().mb_str(wxConvUTF8) ) );
    m_CVTable->SetCellValue(m_CVnr->GetValue()-1, 0, m_CVvalue->GetValue() );

  }
  else if ( event.GetEventObject() == m_saveAllCVs && m_LocProps != NULL ) {
    m_CVoperation = 0;
    int i = 0;
    for( int i = 0; i < m_CVcountAll; i++ ) {
      int cv = m_CVall[i];
      if( cv < 1 || cv > 1024 || m_CVTable->GetCellValue( cv-1, 0 ).IsEmpty() )
        continue;
      doCV( wProgram.save, cv, atoi( m_CVTable->GetCellValue( cv-1, 0 ).mb_str(wxConvUTF8) ) );
    }

  }
  else if ( event.GetEventObject() == m_loadCVs ) {
    m_CVoperation = 0;
    doCV( wProgram.load, m_CVnr->GetValue(), atoi( m_CVvalue->GetValue().mb_str(wxConvUTF8) ) );
  }
  else if ( event.GetEventObject() == m_POM || event.GetEventObject() == m_Acc ) {
    update4POM();
  }
  else if ( event.GetEventObject() == m_CVvalue ) {
    updateCVbits();
  }
  else if ( event.GetEventObject() == m_bit0 ) {
    updateCVval();
  }
  else if ( event.GetEventObject() == m_bit1 ) {
    updateCVval();
  }
  else if ( event.GetEventObject() == m_bit2 ) {
    updateCVval();
  }
  else if ( event.GetEventObject() == m_bit3 ) {
    updateCVval();
  }
  else if ( event.GetEventObject() == m_bit4 ) {
    updateCVval();
  }
  else if ( event.GetEventObject() == m_bit5 ) {
    updateCVval();
  }
  else if ( event.GetEventObject() == m_bit6 ) {
    updateCVval();
  }
  else if ( event.GetEventObject() == m_bit7 ) {
    updateCVval();
  }
  // Get CV's:
  else if ( event.GetEventObject() == m_Get ) {
    if( m_CVnr->GetValue() > 0 ) {
      m_CVoperation = CVGET;
      doCV( wProgram.get, m_CVnr->GetValue(), 0 );
    }
  }
  // Set CV's:
  else if ( event.GetEventObject() == m_Set ) {
    if( m_CVnr->GetValue() > 0 ) {
      m_CVoperation = CVSET;
      doCV( wProgram.set, m_CVnr->GetValue(), atoi( m_CVvalue->GetValue().mb_str(wxConvUTF8) ) );
    }
  }
  else if ( event.GetEventObject() == m_LcList ) {
    initValues();
  }
  else if ( event.GetEventObject() == m_WriteAll ) {
    if( m_CVoperation != CVSETALL ) {
      m_CVidx = 0;
      m_CVidxAll = 0;
      m_CVoperation = CVSETALL;
      writeAll();
    }
    else if( m_CVoperation == CVSETALL ) {
      m_CVidxAll = m_CVidxAll+1;
      m_TimerCount = 0;
      if( m_CVidxAll >= m_CVcountAll ) {
        TraceOp.trc( "cv", TRCLEVEL_INFO, __LINE__, 9999, "End of writeall" );
        stopProgress();
        m_CVoperation = 0;
      }
      else {
        TraceOp.trc( "cv", TRCLEVEL_INFO, __LINE__, 9999, "continue write all" );
        writeAll();
      }
    }
  }
  else if ( event.GetEventObject() == m_ReadAll ) {
    if( m_CVoperation != CVGETALL ) {
      m_CVidx = 0;
      m_CVidxAll = 0;
      m_CVoperation = CVGETALL;
      readAll();
    }
    else if( m_CVoperation == CVGETALL ) {
      m_CVidxAll = m_CVidxAll+1;
      m_TimerCount = 0;
      if( m_CVidxAll >= m_CVcountAll ) {
        TraceOp.trc( "cv", TRCLEVEL_INFO, __LINE__, 9999, "End of readall" );
        stopProgress();
        m_CVoperation = 0;
      }
      else {
        TraceOp.trc( "cv", TRCLEVEL_INFO, __LINE__, 9999, "continue read all" );
        readAll();
      }
    }
  }
  else if( event.GetEventObject() == m_CopyFrom ) {
    LocDialog*  dlg = new LocDialog(m_Parent, NULL, false );
    if( wxID_OK == dlg->ShowModal() ) {
      /* Notify RocRail. */
      iONode sel = dlg->getProperties();
      if( sel != NULL ) {
        setLocProps(sel);
      }
    }
  }
  else if( event.GetEventObject() == m_Config ) {
    TraceOp.trc( "cv", TRCLEVEL_INFO, __LINE__, 9999, "Config" );
    m_CVoperation = CVGET;
    m_bConfig = true;
    doCV( wProgram.get, 29, 0 );
  }
  else if( event.GetEventObject() == m_FX ) {
    TraceOp.trc( "cv", TRCLEVEL_INFO, __LINE__, 9999, "FX" );
    m_CVoperation = CVGET;
    m_bFX = true;
    if(m_CVnr->GetValue() < 49 )
      m_CVnr->SetValue(49);
    doCV( wProgram.get, m_CVnr->GetValue(), 0 );
  }
  else if( event.GetEventObject() == m_SpeedCurve ) {
    TraceOp.trc( "cv", TRCLEVEL_INFO, __LINE__, 9999, "Speed Curve" );

    int action = wxMessageDialog( m_Parent, wxGetApp().getMsg("readspeedcurve"), _T("Rocrail"), wxYES_NO ).ShowModal();
    if( action == wxID_NO ) {
      for( int i = 0; i < 28; i++ ) {
        m_Curve[i] = 0;
      }
      onSpeedCurve();
    }
    else {
      m_bSpeedCurve = true;
      m_CVoperation = CVGET;
      doCV( wProgram.get, 67, 0 );
    }
  }
  else {
    TraceOp.trc( "cv", TRCLEVEL_INFO, __LINE__, 9999, "default doCV" );
    doCV( event.GetId() );
  }

}


void CV::onSpeedCurve() {

  /* move to the event handler */

  TraceOp.trc( "cv", TRCLEVEL_INFO, __LINE__, 9999, "Speed Curve Dialog ***start***" );
  SpeedCurveDlg*  dlg = new SpeedCurveDlg(m_Parent, m_Curve );
  if( wxID_OK == dlg->ShowModal() ) {
    int* newCurve = dlg->getCurve();
    TraceOp.trc( "cv", TRCLEVEL_INFO, __LINE__, 9999, "New Speed Curve:" );
    TraceOp.trc( "cv", TRCLEVEL_INFO, __LINE__, 9999, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d ",
        newCurve[0],newCurve[1],newCurve[2],newCurve[3],newCurve[4],newCurve[5],newCurve[6],newCurve[7],
        newCurve[8],newCurve[9],newCurve[10],newCurve[11],newCurve[12],newCurve[13]);
    TraceOp.trc( "cv", TRCLEVEL_INFO, __LINE__, 9999, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d ",
        newCurve[14],newCurve[15],newCurve[16],newCurve[17],newCurve[18],newCurve[19],newCurve[20],newCurve[21],
        newCurve[22],newCurve[23],newCurve[24],newCurve[25],newCurve[26],newCurve[27]);

    for( int i = 0; i < 28; i++ ) {
      m_Curve[i] = newCurve[i];
    }
    m_bSpeedCurve = true;
    m_CVoperation = CVSET;
    doCV( wProgram.set, 67, m_Curve[0] );

  }
  TraceOp.trc( "cv", TRCLEVEL_INFO, __LINE__, 9999, "Speed Curve Dialog ***end***" );
  dlg->Destroy();
}


/*
 * CV18 = addr - 256 * (addr / 256)
 * CV17 = (addr / 256) + 192
 */
void CV::getLongAddress() {
  TraceOp.trc( "cv", TRCLEVEL_INFO, __LINE__, 9999, "get long address..." );
  m_CVoperation = CVGET;
  doCV( wProgram.get, 17, 0 );
  m_bLongAddress = true;
}

void CV::setLongAddress() {
  wxTextCtrl* tc = (wxTextCtrl*)wxWindow::FindWindowById( VAL_LADDRESS, m_Parent );
  if( tc != NULL ) {
    int addr = atoi( tc->GetValue().mb_str(wxConvUTF8) );
    TraceOp.trc( "cv", TRCLEVEL_INFO, __LINE__, 9999, "set long address to %d..." );
    m_CVoperation = CVSET;
    doCV( wProgram.set, 17, (addr / 256) + 192 );
    m_CVoperation = CVSET;
    doCV( wProgram.set, 18, addr - 256 * (addr / 256) );
    if( wCVconf.islissy( m_CVconf ) ) {
      m_CVoperation = CVSET;
      doCV( wProgram.set, 117, (addr / 256) + 192 );
      m_CVoperation = CVSET;
      doCV( wProgram.set, 118, addr - 256 * (addr / 256) );
    }
    if( m_LocProps != NULL ) {
      wLoc.setaddr(m_LocProps, addr);
      if( !wxGetApp().isStayOffline() ) {
        /* Notify RocRail. */
        iONode cmd = NodeOp.inst( wModelCmd.name(), NULL, ELEMENT_NODE );
        wModelCmd.setcmd( cmd, wModelCmd.modify );
        NodeOp.addChild( cmd, (iONode)m_LocProps->base.clone( m_LocProps ) );
        wxGetApp().sendToRocrail( cmd );
        cmd->base.del(cmd);
      }
    }
  }
}

void CV::doCV( int id ) {
  int command = 0;
  int index = 0;
  int val = 0;

  if( id <= 0 )
    return;

  switch(id) {
  case GET_LADDRESS:
    getLongAddress();
    return;
  case SET_LADDRESS:
    setLongAddress();
    return;
  }

  if( id > GET_CV && id < SET_CV ) {
    command = wProgram.get;
    index = id - GET_CV;
    m_CVoperation = CVGET;
  }
  else if( id > SET_CV && id < VAL_CV ) {
    command = wProgram.set;
    index = id - SET_CV;
    m_CVoperation = CVSET;
  }
  else {
    TraceOp.trc( "cv", TRCLEVEL_DEBUG, __LINE__, 9999, "Unknown id=%d", id );
    return;
  }

  wxTextCtrl* tc = (wxTextCtrl*)wxWindow::FindWindowById( index + VAL_CV, m_Parent );
  if( tc != NULL )
    val = atoi( tc->GetValue().mb_str(wxConvUTF8) );
  else if( command == wProgram.set ) {
    TraceOp.trc( "cv", TRCLEVEL_WARNING, __LINE__, 9999, "No VAL_CV found for index=%d", index );
    return;
  }

  TraceOp.trc( "cv", TRCLEVEL_INFO, __LINE__, 9999, "doCV for id=%d", id );
  doCV( command, index, val );

  if( index == 1 && wCVconf.islissy( m_CVconf ) ) {
    TraceOp.trc( "cv", TRCLEVEL_INFO, __LINE__, 9999, "do Lissy CV for address=%d", val );
    doCV( command, 116, val );
  }

}


void CV::doCV( int command, int index, int value ) {
  iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  int addr = atoi( m_CVaddress->GetValue().mb_str(wxConvUTF8) );
  wProgram.setlongaddr( cmd, False );

  if( addr == 0 ) {
    addr = atoi( m_CVlongaddress->GetValue().mb_str(wxConvUTF8) );
    wProgram.setlongaddr( cmd, True );
  }


  update4POM();
  wProgram.setcmd( cmd, command );
  wProgram.setaddr( cmd, addr );
  wProgram.setcv( cmd, index );
  wProgram.setvalue( cmd, value );
  wProgram.setpom( cmd, m_bPOM );
  wProgram.setacc( cmd, m_bAcc );
  wProgram.setdirect( cmd, m_Direct->IsChecked()?True:False );
  if( command == wProgram.save ) {
    if( m_LocProps != NULL ) {
      TraceOp.trc( "cv", TRCLEVEL_INFO, __LINE__, 9999,
          "save CV for %s addr=%d cv=%d value=%d...", wLoc.getid( m_LocProps ), addr, index, value );
      wProgram.setdecaddr( cmd, wLoc.getaddr( m_LocProps ) );
      wProgram.setfilename( cmd, wLoc.getid( m_LocProps ) );

      /* What about long addressing?
      if( index == 1) {
        wLoc.setaddr(m_LocProps, value);
      }
      */

      if( !wxGetApp().isStayOffline() ) {
        /* Notify RocRail. */
        wxGetApp().sendToRocrail( cmd );
        cmd->base.del(cmd);
      }
    }
  }
  else {
    TraceOp.trc( "cv", TRCLEVEL_INFO, __LINE__, 9999,
        "sending program command for addr=%d cmd=%d index=%d value=%d...", addr, command, index, value );
    wxGetApp().sendToRocrail( cmd );
    cmd->base.del(cmd);
    m_CVidx = index;
    if( !m_bPOM && m_CVoperation > 0 ) {
      m_TimerCount = 0;
      bool rc = m_Timer->Start( 1000, wxTIMER_ONE_SHOT );

      TraceOp.trc( "cv", TRCLEVEL_INFO, __LINE__, 9999, "Timeout timer %sstarted %dms...", rc?"":"NOT ", wCVconf.gettimeout(m_CVconf) * 1000 );
      startProgress();

    }
  }
}

void CV::OnTimer(wxTimerEvent& event) {
  if( !MutexOp.trywait( m_TimerMutex, 100 ) ) {
    TraceOp.trc( "cv", TRCLEVEL_DEBUG, __LINE__, 9999, "timeout on timer mutex!" );
    return;
  }

  if( m_bCleanUpProgress ) {
    if( m_Progress != NULL ) {
      wxProgressDialog* dlg = m_Progress;
      m_Progress = NULL;
      dlg->Destroy();
      TraceOp.trc( "cv", TRCLEVEL_INFO, __LINE__, 9999, "cleaned up the progress dialog" );
    }
    m_bCleanUpProgress = false;
    MutexOp.post( m_TimerMutex );
    return;
  }

  m_TimerCount++;
  if( m_TimerCount >= wCVconf.gettimeout(m_CVconf) ) {
    TraceOp.trc( "cv", TRCLEVEL_WARNING, __LINE__, 9999, "timeout on PT acknowledge" );
    stopProgress();
  }
  else if( m_Progress != NULL && !m_bCleanUpProgress ) {
    if( m_Progress->IsShownOnScreen() ) {
      if( !m_Progress->Pulse() ) {
        stopProgress();
        m_TimerCount = wCVconf.gettimeout(m_CVconf);
      }
      else {
        TraceOp.trc( "cv", TRCLEVEL_DEBUG, __LINE__, 9999, "timer for PT acknowledge" );
        bool rc = m_Timer->Start( 1000, wxTIMER_ONE_SHOT );
      }
    }
  }
  MutexOp.post( m_TimerMutex );
}


void CV::readAll() {
  TraceOp.trc( "cv", TRCLEVEL_INFO, __LINE__, 9999, "reading m_CVall[%d]=%d", m_CVidxAll, m_CVall[m_CVidxAll] );
  doCV( wProgram.get, m_CVall[m_CVidxAll], 0 );
}


void CV::writeAll() {
  wxString svalue = m_CVTable->GetCellValue( m_CVall[m_CVidxAll]-1, 0 );
  long val;
  if( svalue.ToLong(&val ) ) {
    TraceOp.trc( "cv", TRCLEVEL_INFO, __LINE__, 9999, "cellval[%d]=%d", m_CVidxAll, (int)val );
    doCV( wProgram.set, m_CVall[m_CVidxAll], (int)val );
  }
}


void CV::initLabels() {
  m_labelCVaddress->SetLabel( _T( "Addr." ) );
  m_labelCVlongaddress->SetLabel( _T( "L.Addr." ) );
  m_labelCVVstart->SetLabel( _T( "Vstart" ) );
  m_labelCVaccel->SetLabel( _T( "Acc.rate" ) );
  m_labelCVdecel->SetLabel( _T( "Dec.rate" ) );
  m_labelCVVhigh->SetLabel( _T( "Vhigh" ) );
  m_labelCVVmid->SetLabel( _T( "Vmid" ) );
  m_labelCVversion->SetLabel( _T( "Version" ) );
  m_labelCVmanufacturedID->SetLabel( _T( "Mfr.ID" ) );
}
/*!
 * Programming creator
 */

bool CV::Create()
{
    m_MainBox = NULL;
    m_ScrollWindow = NULL;
    m_ItemPanel = NULL;
    m_PanelMainBox = NULL;
    m_FlexGrid = NULL;
    m_labelCVaddress = NULL;
    m_CVaddress = NULL;
    m_getAddress = NULL;
    m_setAddress = NULL;
    m_labelCVlongaddress = NULL;
    m_CVlongaddress = NULL;
    m_getlongAddress = NULL;
    m_setlongAddress = NULL;
    m_labelCVVstart = NULL;
    m_CVVstart = NULL;
    m_getVstart = NULL;
    m_setVstart = NULL;
    m_labelCVaccel = NULL;
    m_CVaccel = NULL;
    m_getAccel = NULL;
    m_setAccel = NULL;
    m_labelCVdecel = NULL;
    m_CVdecel = NULL;
    m_getDecel = NULL;
    m_setDecel = NULL;
    m_labelCVVhigh = NULL;
    m_CVVhigh = NULL;
    m_getVhigh = NULL;
    m_setVhigh = NULL;
    m_labelCVVmid = NULL;
    m_CVVmid = NULL;
    m_getVmid = NULL;
    m_setVmid = NULL;
    m_labelCVversion = NULL;
    m_CVversion = NULL;
    m_getVersion = NULL;
    m_setVersion = NULL;
    m_labelCVmanufacturedID = NULL;
    m_CVmanufacturedID = NULL;
    m_getManu = NULL;
    m_setManu = NULL;
    m_CVbox = NULL;
    m_CVnr = NULL;
    m_CVvalue = NULL;
    m_Get = NULL;
    m_Set = NULL;
    m_PTonoff = NULL;

    m_labIID = NULL;
    m_IID = NULL;

    m_saveCVs = NULL;
    m_loadCVs = NULL;
    m_saveAllCVs = NULL;
    m_SpeedCurve = NULL;
    m_loadFile = NULL;
    m_saveFile = NULL;

    m_CVMainBox = NULL;
    m_CVSubBox1 = NULL;
    m_CVSubBox2 = NULL;

    m_bit0 = NULL;
    m_bit1 = NULL;
    m_bit2 = NULL;
    m_bit3 = NULL;
    m_bit4 = NULL;
    m_bit5 = NULL;
    m_bit6 = NULL;
    m_bit7 = NULL;

    TraceOp.trc( "cv", TRCLEVEL_INFO, __LINE__, 9999, "CreateControls..." );
    CreateControls();

    TraceOp.trc( "cv", TRCLEVEL_INFO, __LINE__, 9999, "initLabels..." );
    initLabels();

    //m_Parent->GetSizer()->Fit(m_Parent);
    //m_Parent->GetSizer()->SetSizeHints(m_Parent);
    //m_ItemPanel->Layout();
    return true;
}

/*!
 * Control creation for Programming
 */

void CV::CreateControls() {

  m_MainBox = new wxBoxSizer(wxVERTICAL);
  m_Parent->SetSizer(m_MainBox);

  m_ItemPanel = new wxPanel( m_Parent, -1, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
  m_MainBox->Add(m_ItemPanel, 1, wxALIGN_CENTER_HORIZONTAL|wxALL|wxADJUST_MINSIZE, 2);
  m_PanelMainBox = new wxBoxSizer(wxVERTICAL);
  m_ItemPanel->SetSizer(m_PanelMainBox);


  // LocList
  m_LocBox = new wxBoxSizer(wxHORIZONTAL);
  m_PanelMainBox->Add(m_LocBox, 0, wxGROW|wxALL|wxADJUST_MINSIZE, 2);

  m_labLoc = new wxStaticText( m_ItemPanel, -1, _("Loc"), wxDefaultPosition, wxDefaultSize, 0 );
  m_LocBox->Add(m_labLoc, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 2);
  wxString* m_LcListStrings = NULL;
  m_LcList = new wxComboBox( m_ItemPanel, ID_COMBOBOX_LOCLIST, _T(""), wxDefaultPosition, wxSize(90, 25), 0, m_LcListStrings, wxCB_READONLY );
  m_LocBox->Add(m_LcList, 3, wxGROW|wxALL, 1);

  m_loadFile = new wxButton( m_ItemPanel, -1, _("Import"), wxDefaultPosition, wxSize(60, 26), 0 );
  m_LocBox->Add(m_loadFile, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 2);

  m_saveFile = new wxButton( m_ItemPanel, -1, _("Export"), wxDefaultPosition, wxSize(60, 26), 0 );
  m_LocBox->Add(m_saveFile, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 2);

  m_OptionBox = new wxBoxSizer(wxHORIZONTAL);
  m_PanelMainBox->Add(m_OptionBox, 0, wxGROW|wxALL|wxADJUST_MINSIZE, 2);

  m_POM = new wxCheckBox( m_ItemPanel, -1, _T("PoM"), wxDefaultPosition, wxDefaultSize, 0 );
  m_POM->SetToolTip(_T("Program On the Main") );
  m_OptionBox->Add(m_POM, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 2);

  m_Acc = new wxCheckBox( m_ItemPanel, -1, _T("Acc"), wxDefaultPosition, wxDefaultSize, 0 );
  m_Acc->SetToolTip(_T("Accessory") );
  m_OptionBox->Add(m_Acc, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 2);

  m_Direct = new wxCheckBox( m_ItemPanel, -1, _T("Direct"), wxDefaultPosition, wxDefaultSize, 0 );
  m_Direct->SetToolTip(_T("Direct programming mode") );
  m_OptionBox->Add(m_Direct, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 2);

  m_TableBox = new wxBoxSizer(wxHORIZONTAL);
  m_PanelMainBox->Add(m_TableBox, 0, wxGROW|wxALL|wxADJUST_MINSIZE, 2);

  m_FlexGrid = new wxFlexGridSizer(0, 4, 0, 0);
  m_TableBox->Add(m_FlexGrid, 0, wxGROW|wxALL|wxADJUST_MINSIZE, 2);


  // Address:
  m_labelCVaddress = new wxStaticText( m_ItemPanel, -1, _("address"), wxDefaultPosition, wxDefaultSize, 0 );
  m_FlexGrid->Add(m_labelCVaddress, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 2);

  m_CVaddress = new wxTextCtrl( m_ItemPanel, VAL_ADDRESS, _T("0"), wxDefaultPosition, wxSize(60, -1), wxTE_CENTRE );
  m_FlexGrid->Add(m_CVaddress, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 1);

  m_getAddress = new wxButton( m_ItemPanel, GET_ADDRESS, _("Get"), wxDefaultPosition, wxSize(40, 25), 0 );
  m_FlexGrid->Add(m_getAddress, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 1);

  m_setAddress = new wxButton( m_ItemPanel, SET_ADDRESS, _("Set"), wxDefaultPosition, wxSize(40, 25), 0 );
  m_FlexGrid->Add(m_setAddress, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 1);


  // Long Address:
  m_labelCVlongaddress = new wxStaticText( m_ItemPanel, -1, _("longaddress"), wxDefaultPosition, wxDefaultSize, 0 );
  m_FlexGrid->Add(m_labelCVlongaddress, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 2);

  m_CVlongaddress = new wxTextCtrl( m_ItemPanel, VAL_LADDRESS, _T("0"), wxDefaultPosition, wxSize(60, -1), wxTE_CENTRE );
  m_FlexGrid->Add(m_CVlongaddress, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 1);

  m_getlongAddress = new wxButton( m_ItemPanel, GET_LADDRESS, _("Get"), wxDefaultPosition, wxSize(40, 25), 0 );
  m_FlexGrid->Add(m_getlongAddress, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 1);

  m_setlongAddress = new wxButton( m_ItemPanel, SET_LADDRESS, _("Set"), wxDefaultPosition, wxSize(40, 25), 0 );
  m_FlexGrid->Add(m_setlongAddress, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 1);


  // Vstart:
  m_labelCVVstart = new wxStaticText( m_ItemPanel, -1, _("Vstart"), wxDefaultPosition, wxDefaultSize, 0 );
  m_FlexGrid->Add(m_labelCVVstart, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 2);

  m_CVVstart = new wxTextCtrl( m_ItemPanel, VAL_VSTART, _T("0"), wxDefaultPosition, wxSize(60, -1), wxTE_CENTRE );
  m_FlexGrid->Add(m_CVVstart, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 1);

  m_getVstart = new wxButton( m_ItemPanel, GET_VSTART, _("Get"), wxDefaultPosition, wxSize(40, 25), 0 );
  m_FlexGrid->Add(m_getVstart, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 1);

  m_setVstart = new wxButton( m_ItemPanel, SET_VSTART, _("Set"), wxDefaultPosition, wxSize(40, 25), 0 );
  m_FlexGrid->Add(m_setVstart, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 1);


  // Vaccel:
  m_labelCVaccel = new wxStaticText( m_ItemPanel, -1, _("acceleration rate"), wxDefaultPosition, wxDefaultSize, 0 );
  m_FlexGrid->Add(m_labelCVaccel, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 2);

  m_CVaccel = new wxTextCtrl( m_ItemPanel, VAL_VACCEL, _T("0"), wxDefaultPosition, wxSize(60, -1), wxTE_CENTRE );
  m_FlexGrid->Add(m_CVaccel, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 1);

  m_getAccel = new wxButton( m_ItemPanel, GET_VACCEL, _("Get"), wxDefaultPosition, wxSize(40, 25), 0 );
  m_FlexGrid->Add(m_getAccel, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 1);

  m_setAccel = new wxButton( m_ItemPanel, SET_VACCEL, _("Set"), wxDefaultPosition, wxSize(40, 25), 0 );
  m_FlexGrid->Add(m_setAccel, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 1);


  // Vdecel:
  m_labelCVdecel = new wxStaticText( m_ItemPanel, -1, _("deceleration rate"), wxDefaultPosition, wxDefaultSize, 0 );
  m_FlexGrid->Add(m_labelCVdecel, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 2);

  m_CVdecel = new wxTextCtrl( m_ItemPanel, VAL_VDECEL, _T("0"), wxDefaultPosition, wxSize(60, -1), wxTE_CENTRE );
  m_FlexGrid->Add(m_CVdecel, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 1);

  m_getDecel = new wxButton( m_ItemPanel, GET_VDECEL, _("Get"), wxDefaultPosition, wxSize(40, 25), 0 );
  m_FlexGrid->Add(m_getDecel, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 1);

  m_setDecel = new wxButton( m_ItemPanel, SET_VDECEL, _("Set"), wxDefaultPosition, wxSize(40, 25), 0 );
  m_FlexGrid->Add(m_setDecel, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 1);


  // Vhigh:
  m_labelCVVhigh = new wxStaticText( m_ItemPanel, -1, _("Vhigh"), wxDefaultPosition, wxDefaultSize, 0 );
  m_FlexGrid->Add(m_labelCVVhigh, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 2);

  m_CVVhigh = new wxTextCtrl( m_ItemPanel, VAL_VHIGH, _T("0"), wxDefaultPosition, wxSize(60, -1), wxTE_CENTRE );
  m_FlexGrid->Add(m_CVVhigh, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 1);

  m_getVhigh = new wxButton( m_ItemPanel, GET_VHIGH, _("Get"), wxDefaultPosition, wxSize(40, 25), 0 );
  m_FlexGrid->Add(m_getVhigh, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 1);

  m_setVhigh = new wxButton( m_ItemPanel, SET_VHIGH, _("Set"), wxDefaultPosition, wxSize(40, 25), 0 );
  m_FlexGrid->Add(m_setVhigh, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 1);


  // VMid:
  m_labelCVVmid = new wxStaticText( m_ItemPanel, -1, _("Vmid"), wxDefaultPosition, wxDefaultSize, 0 );
  m_FlexGrid->Add(m_labelCVVmid, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 2);

  m_CVVmid = new wxTextCtrl( m_ItemPanel, VAL_VMID, _T("0"), wxDefaultPosition, wxSize(60, -1), wxTE_CENTRE );
  m_FlexGrid->Add(m_CVVmid, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 1);

  m_getVmid = new wxButton( m_ItemPanel, GET_VMID, _("Get"), wxDefaultPosition, wxSize(40, 25), 0 );
  m_FlexGrid->Add(m_getVmid, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 1);

  m_setVmid = new wxButton( m_ItemPanel, SET_VMID, _("Set"), wxDefaultPosition, wxSize(40, 25), 0 );
  m_FlexGrid->Add(m_setVmid, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 1);


  // version:
  m_labelCVversion = new wxStaticText( m_ItemPanel, -1, _("version"), wxDefaultPosition, wxDefaultSize, 0 );
  m_FlexGrid->Add(m_labelCVversion, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 2);

  m_CVversion = new wxTextCtrl( m_ItemPanel, VAL_VERSION, _T("0"), wxDefaultPosition, wxSize(60, -1), wxTE_READONLY|wxTE_CENTRE );
  m_FlexGrid->Add(m_CVversion, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 1);

  m_getVersion = new wxButton( m_ItemPanel, GET_VERSION, _("Get"), wxDefaultPosition, wxSize(40, 25), 0 );
  m_FlexGrid->Add(m_getVersion, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 1);

  m_setVersion = new wxButton( m_ItemPanel, SET_VERSION, _("Set"), wxDefaultPosition, wxSize(40, 25), 0 );
  m_setVersion->Enable(false);
  m_FlexGrid->Add(m_setVersion, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 1);


  // manufacturedID:
  m_labelCVmanufacturedID = new wxStaticText( m_ItemPanel, -1, _("manufactured ID"), wxDefaultPosition, wxDefaultSize, 0 );
  m_FlexGrid->Add(m_labelCVmanufacturedID, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 2);

  m_CVmanufacturedID = new wxTextCtrl( m_ItemPanel, VAL_MANUFACTUREDID, _T("0"), wxDefaultPosition, wxSize(60, -1), wxTE_READONLY|wxTE_CENTRE );
  m_FlexGrid->Add(m_CVmanufacturedID, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 1);

  m_getManu = new wxButton( m_ItemPanel, GET_MANUFACTUREDID, _("Get"), wxDefaultPosition, wxSize(40, 25), 0 );
  m_FlexGrid->Add(m_getManu, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 1);

  m_setManu = new wxButton( m_ItemPanel, SET_MANUFACTUREDID, _("Set"), wxDefaultPosition, wxSize(40, 25), 0 );
  m_setManu->Enable(false);
  m_FlexGrid->Add(m_setManu, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 1);


  // CVTable
  m_CVTable = new wxGrid( m_ItemPanel, -1, wxDefaultPosition, wxSize(103, 230), wxSUNKEN_BORDER|wxVSCROLL  );
  m_CVTable->SetRowLabelSize(35);
  m_CVTable->CreateGrid(1024, 1, wxGrid::wxGridSelectRows);
  wxFont* font = new wxFont( m_CVTable->GetDefaultCellFont() );
  font->SetPointSize( (int)(font->GetPointSize() - 1 ) );
  m_CVTable->SetSelectionMode(wxGrid::wxGridSelectRows);
  m_CVTable->SetDefaultCellFont( *font );
  m_CVTable->SetDefaultRowSize(20);
  m_CVTable->SetColLabelSize(20);
  //m_CVTable->SetColLabelValue(0, _("CV") );
  m_CVTable->SetColLabelValue(0, _("value") );
  m_CVTable->SetMargins(0,0);
  m_CVTable->AutoSizeColumns();
  m_CVTable->AutoSizeRows();

  m_TableBox->Add(m_CVTable, 0, wxALL|wxADJUST_MINSIZE, 2);


  TraceOp.trc( "cv", TRCLEVEL_DEBUG, __LINE__, 9999, "Create CV Controls..." );

  // CV:
  wxStaticBox* itemStaticBoxSizer20Static = new wxStaticBox(m_ItemPanel, -1, _("CV-Box"));
  m_CVbox = new wxStaticBoxSizer(itemStaticBoxSizer20Static, wxHORIZONTAL);
  m_PanelMainBox->Add(m_CVbox, 0, wxALL, 3);

  m_MainBox = new wxBoxSizer(wxVERTICAL);
  m_CVbox->Add(m_MainBox, 0, wxALIGN_CENTER_VERTICAL|wxALL, 0);
  m_CVSubBox1 = new wxBoxSizer(wxHORIZONTAL);
  m_MainBox->Add(m_CVSubBox1, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 0);
  m_CVSubBox2 = new wxBoxSizer(wxHORIZONTAL);
  m_MainBox->Add(m_CVSubBox2, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 0);
  m_CVSubBox3 = new wxBoxSizer(wxHORIZONTAL);
  m_MainBox->Add(m_CVSubBox3, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 0);

  m_CVnr = new wxSpinCtrl( m_ItemPanel, -1, _T("0"), wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 1024, 0 );
  m_CVSubBox1->Add(m_CVnr, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

  m_CVvalue = new wxTextCtrl( m_ItemPanel, VAL_GENERIC, _T("0"), wxDefaultPosition, wxSize(60, -1), wxTE_CENTRE );
  TraceOp.trc( "cv", TRCLEVEL_DEBUG, __LINE__, 9999, "Create CV Controls 4.1..." );
  m_CVSubBox1->Add(m_CVvalue, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

  m_Get = new wxButton( m_ItemPanel, -1, _("Get"), wxDefaultPosition, wxSize(40, 25), 0 );
  m_CVSubBox1->Add(m_Get, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

  m_Set = new wxButton( m_ItemPanel, -1, _("Set"), wxDefaultPosition, wxSize(40, 25), 0 );
  m_CVSubBox1->Add(m_Set, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);
  m_FX = new wxButton( m_ItemPanel, -1, _("FX"), wxDefaultPosition, wxSize(40, 25), 0 );
  m_CVSubBox1->Add(m_FX, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

  TraceOp.trc( "cv", TRCLEVEL_DEBUG, __LINE__, 9999, "Create CV Bitfield..." );

  m_bit7 = new wxCheckBox( m_ItemPanel, -1, _T("7"), wxDefaultPosition, wxDefaultSize, 0 );
  m_CVSubBox2->Add(m_bit7, 0, wxALIGN_CENTER_VERTICAL, 0);
  m_bit6 = new wxCheckBox( m_ItemPanel, -1, _T("6"), wxDefaultPosition, wxDefaultSize, 0 );
  m_CVSubBox2->Add(m_bit6, 0, wxALIGN_CENTER_VERTICAL, 0);
  m_bit5 = new wxCheckBox( m_ItemPanel, -1, _T("5"), wxDefaultPosition, wxDefaultSize, 0 );
  m_CVSubBox2->Add(m_bit5, 0, wxALIGN_CENTER_VERTICAL, 0);
  m_bit4 = new wxCheckBox( m_ItemPanel, -1, _T("4"), wxDefaultPosition, wxDefaultSize, 0 );
  m_CVSubBox2->Add(m_bit4, 0, wxALIGN_CENTER_VERTICAL, 0);
  m_bit3 = new wxCheckBox( m_ItemPanel, -1, _T("3"), wxDefaultPosition, wxDefaultSize, 0 );
  m_CVSubBox2->Add(m_bit3, 0, wxALIGN_CENTER_VERTICAL, 0);
  m_bit2 = new wxCheckBox( m_ItemPanel, -1, _T("2"), wxDefaultPosition, wxDefaultSize, 0 );
  m_CVSubBox2->Add(m_bit2, 0, wxALIGN_CENTER_VERTICAL, 0);
  m_bit1 = new wxCheckBox( m_ItemPanel, -1, _T("1"), wxDefaultPosition, wxDefaultSize, 0 );
  m_CVSubBox2->Add(m_bit1, 0, wxALIGN_CENTER_VERTICAL, 0);
  m_bit0 = new wxCheckBox( m_ItemPanel, -1, _T("0"), wxDefaultPosition, wxDefaultSize, 0 );
  m_CVSubBox2->Add(m_bit0, 0, wxALIGN_CENTER_VERTICAL, 0);


  wxBoxSizer* buttonSizer1 = new wxBoxSizer(wxHORIZONTAL);
  m_PanelMainBox->Add(buttonSizer1, 0, wxALL|wxADJUST_MINSIZE, 2);

  m_saveCVs = new wxButton( m_ItemPanel, -1, _("Copy"), wxDefaultPosition, wxSize(60, 26), 0 );
  buttonSizer1->Add(m_saveCVs, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);
  m_loadCVs = new wxButton( m_ItemPanel, -1, _("Load"), wxDefaultPosition, wxSize(60, 26), 0 );
  buttonSizer1->Add(m_loadCVs, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);
  m_saveAllCVs = new wxButton( m_ItemPanel, -1, _("SaveAll"), wxDefaultPosition, wxSize(60, 26), 0 );
  buttonSizer1->Add(m_saveAllCVs, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);
  m_SpeedCurve = new wxButton( m_ItemPanel, -1, _("V curve"), wxDefaultPosition, wxSize(60, 26), 0 );
  buttonSizer1->Add(m_SpeedCurve, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);
  m_Config = new wxButton( m_ItemPanel, -1, _("Config"), wxDefaultPosition, wxSize(60, 26), 0 );
  buttonSizer1->Add(m_Config, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

  wxBoxSizer* buttonSizer2 = new wxBoxSizer(wxHORIZONTAL);
  m_PanelMainBox->Add(buttonSizer2, 0, wxALL|wxADJUST_MINSIZE, 2);

  m_PTonoff = new wxToggleButton( m_ItemPanel, -1, _("PT"), wxDefaultPosition, wxSize(75, 26), 0 );
  m_PTonoff->SetValue(false);
  buttonSizer2->Add(m_PTonoff, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

  m_WriteAll = new wxButton( m_ItemPanel, -1, _("Write all"), wxDefaultPosition, wxSize(75, 26), 0 );
  m_ReadAll  = new wxButton( m_ItemPanel, -1, _("Read all"), wxDefaultPosition, wxSize(75, 26), 0 );
  m_CopyFrom  = new wxButton( m_ItemPanel, -1, _("Copy from..."), wxDefaultPosition, wxSize(75, 26), 0 );

  buttonSizer2->Add(m_ReadAll, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);
  buttonSizer2->Add(m_WriteAll, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);
  buttonSizer2->Add(m_CopyFrom, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);


}

