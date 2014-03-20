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
// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------
// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
    #include "wx/event.h"
#endif

#ifdef __linux__
#include <sys/resource.h>
#endif

#include <wx/image.h>
#include <wx/filedlg.h>

#include "rocview/public/guiapp.h"
#include "rocview/wrapper/public/Gui.h"
#include "rocview/wrapper/public/CVconf.h"
#include "rocview/wrapper/public/CVcat.h"
#include "rocrail/wrapper/public/CVByte.h"
#include "rocrail/wrapper/public/Plan.h"
#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/Program.h"
#include "rocrail/wrapper/public/ModelCmd.h"
#include "rocrail/wrapper/public/DataReq.h"
#include "rocrail/wrapper/public/Dec.h"
#include "rocrail/wrapper/public/Car.h"
#include "rocrail/wrapper/public/DIP.h"
#include "rocrail/wrapper/public/DIPGroup.h"
#include "rocrail/wrapper/public/DIPValue.h"

// JMRI
#include "rocrail/wrapper/public/DecoderConfig.h"
#include "rocrail/wrapper/public/Decoder.h"
#include "rocrail/wrapper/public/DecFamily.h"
#include "rocrail/wrapper/public/DecVariables.h"
#include "rocrail/wrapper/public/DecVariable.h"

#include "rocs/public/system.h"
#include "rocs/public/strtok.h"

#include "rocprodlg.h"

#include "rocview/dialogs/speedcurvedlg.h"
#include "rocview/dialogs/dipdlg.h"

#include "rocview/xpm/nopict.xpm"

RocProDlg::RocProDlg( wxWindow* parent )
:
RocProDlgGen( parent )
{
  m_DecFilename = NULL;
  m_LocoProps = NULL;
  m_CVConf = NULL;
  m_bSpeedCurve = false;
  m_CVoperation = 0;
  m_PendingCV = 0;
  m_bLongAddress = false;
  m_CV17 = 0;
  m_CV18 = 0;

  m_CVexpected = 0;
  m_CVidx = 0;
  m_CVidxAll = 0;
  m_CVoperation = 0;
  m_Save = false;
  m_SelectedCV = NULL;

  m_CVconf = wGui.getcvconf( wxGetApp().getIni() );
  if( m_CVconf == NULL ) {
    m_CVconf = NodeOp.inst( wCVconf.name(), wxGetApp().getIni(), ELEMENT_NODE );
    NodeOp.addChild(wxGetApp().getIni(), m_CVconf);
  }

  m_Save = wCVconf.issave(m_CVconf) ? true:false;
  m_UseDecSpec4All = wCVconf.isusedecspec4all(m_CVconf) ? true:false;
  m_SaveCV->SetValue(m_Save);

  const char* nrs = wCVconf.getnrs( m_CVconf );
  iOStrTok tok = StrTokOp.inst( nrs, ',' );
  int nridx = 0;
  const char* nr = StrTokOp.nextToken(tok);

  while( nr != NULL ) {
    if( StrOp.findc(nr, '-')) {
      iOStrTok subtok = StrTokOp.inst( nr, '-' );
      int firstnr = atoi(StrTokOp.nextToken(subtok));
      if( StrTokOp.hasMoreTokens(subtok)) {
        int lastnr = atoi(StrTokOp.nextToken(subtok));
        if( lastnr > firstnr ) {
          int x = 0;
          for( x = 0; x <= lastnr-firstnr; x++) {
            m_CVall[nridx] = x + firstnr;
            TraceOp.trc( "rocpro", TRCLEVEL_INFO, __LINE__, 9999, "m_CVall[%d]=%d", nridx, m_CVall[nridx] );
            nridx++;
          }
        }
      }
      StrTokOp.base.del( subtok );
    }
    else {
      int cvnr = atoi(nr);
      m_CVall[nridx] = cvnr;
      TraceOp.trc( "rocpro", TRCLEVEL_INFO, __LINE__, 9999, "m_CVall[%d]=%d", nridx, cvnr );
      nridx++;
    }
    nr = StrTokOp.nextToken(tok);
  };
  StrTokOp.base.del( tok );
  m_CVcountAll = nridx;


  for(int i = 0; i < 28; i++ ) {
    m_Curve[i] = 0;
  }
  m_CVMap = MapOp.inst();
  m_CVNrMap = MapOp.inst();
  m_CatMap = MapOp.inst();
  m_DIPMap = MapOp.inst();
  m_LocoImage->SetBitmap( wxBitmap(nopict_xpm) );
  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);

  initLocMap();
  m_DIP->Enable(false);
  m_VCurve->Enable(true);
  m_CVURL->Enable(false);

}

RocProDlg::~RocProDlg() {
  MapOp.base.del(m_CVMap);
  MapOp.base.del(m_CVNrMap);
  MapOp.base.del(m_CatMap);
  MapOp.base.del(m_DIPMap);
  if( m_DecFilename != NULL )
    StrOp.free(m_DecFilename);
}


void RocProDlg::onTreeSelChanged( wxTreeEvent& event )
{
  wxString itemText = m_DecTree->GetItemText(event.GetItem());
  char* desc = StrOp.dup(itemText.mb_str(wxConvUTF8));
  TraceOp.trc( "rocpro", TRCLEVEL_INFO, __LINE__, 9999, "tree sel=%s", desc );
  iONode cv = (iONode)MapOp.get( m_CVMap, desc );
  m_SelectedCV = cv;
  if( cv != NULL ) {
    int nr = wCVByte.getnr(cv);
    m_Info->SetValue(wxString( wCVByte.getinfo(cv), wxConvUTF8));
    m_Nr->SetValue( nr );

    iONode lococv = getLocoCV(nr);
    if( lococv != NULL ) {
      setCVVal(wCVByte.getvalue(lococv));
    }
    else
      setCVVal(0);

    m_DIP->Enable( wCVByte.getadip(m_SelectedCV) != NULL );
    m_CVURL->Enable( wCVByte.geturl(m_SelectedCV) != NULL );

    if(wCVByte.getdipid(m_SelectedCV) != NULL && StrOp.len(wCVByte.getdipid(m_SelectedCV)) > 0 ) {
      if( MapOp.haskey(m_DIPMap, wCVByte.getdipid(m_SelectedCV) ) )
        m_DIP->Enable( true );
    }


    m_WriteCV->Enable(wCVByte.isreadonly(cv)?false:true);
    m_ValueSlider->Enable(wCVByte.isword(cv)?false:true);
    m_ValueSlider->SetRange(0, wCVByte.isword(cv)?65535:255);
    m_Bit0->Enable(wCVByte.isword(cv)?false:true);
    m_Bit1->Enable(wCVByte.isword(cv)?false:true);
    m_Bit2->Enable(wCVByte.isword(cv)?false:true);
    m_Bit3->Enable(wCVByte.isword(cv)?false:true);
    m_Bit4->Enable(wCVByte.isword(cv)?false:true);
    m_Bit5->Enable(wCVByte.isword(cv)?false:true);
    m_Bit6->Enable(wCVByte.isword(cv)?false:true);
    m_Bit7->Enable(wCVByte.isword(cv)?false:true);
  }
  else {
    // catagory
    m_Info->SetValue(wxString( NodeOp.getStr(m_DecNode, "info", "-"), wxConvUTF8));
    m_Nr->SetValue( 0 );
    setCVVal(0);
    m_DIP->Enable( false );
    m_CVURL->Enable( false );
  }
  StrOp.free(desc);
}


void RocProDlg::loadDecFile() {
  if( parseDecFile() ) {
    m_DecTree->DeleteAllItems();
    MapOp.clear(m_CVMap);
    MapOp.clear(m_CVNrMap);
    m_SelectedCV = NULL;
    wxTreeItemId root  = m_DecTree->AddRoot(wxString( NodeOp.getStr(m_DecNode, "manu", "?"), wxConvUTF8)+wxT(" ")+wxString( NodeOp.getStr(m_DecNode, "type", "?"), wxConvUTF8));
    iOMap catMap = MapOp.inst();
    int cnt = NodeOp.getChildCnt(m_DecNode);
    int idx = 0;
    for( int i = 0; i < cnt; i++ ) {
      iONode cv = NodeOp.getChild(m_DecNode, i);
      if( StrOp.equals(wCVByte.name(), NodeOp.getName(cv)) ) {
        const char* catName = wCVByte.getcat(cv);
        wxTreeItemId* pcat = (wxTreeItemId*)MapOp.get( catMap, catName );
        wxTreeItemId cat;
        if( pcat == NULL ) {
          cat = m_DecTree->AppendItem( root, wxString( catName, wxConvUTF8));
          pcat = &cat;
          MapOp.put(catMap, catName, (obj)new wxTreeItemId(cat.m_pItem) );
        }
        else {
          cat = *pcat;
        }
        m_DecTree->AppendItem( cat, wxString( wCVByte.getdesc(cv), wxConvUTF8));
        MapOp.put( m_CVMap, wCVByte.getdesc(cv), (obj)cv);
        char key[32];
        StrOp.fmtb(key, "%d", wCVByte.getnr(cv) );
        MapOp.put( m_CVNrMap, key, (obj)cv);

        if( wCVByte.getadip(cv) != NULL ) {
          iONode dip = wCVByte.getadip(cv);
          if( wDIP.getid(dip) != NULL && StrOp.len(wDIP.getid(dip)) > 0 ) {
            MapOp.put(m_DIPMap, wDIP.getid(dip), (obj)dip);
          }
        }

        if( m_UseDecSpec4All ) {
          m_CVall[idx] = wCVByte.getnr(cv);
          m_CVcountAll = idx+1;
        }
        idx++;
      }
    }

    if( NodeOp.getBool(m_DecNode, "collapse", False ) )
      m_DecTree->CollapseAll();
    else
      m_DecTree->ExpandAll();

    m_DecTree->ScrollTo(root);
  }

}

void RocProDlg::onOpen( wxCommandEvent& event )
{
  wxString ms_FileExt = wxGetApp().getMsg("planfiles"); // ToDo: "decfiles"
  const char* l_openpath = wGui.getdecpath( wxGetApp().getIni() );
  TraceOp.trc( "rocpro", TRCLEVEL_INFO, __LINE__, 9999, "openpath=%s", l_openpath );
  wxFileDialog* fdlg = new wxFileDialog(this, wxGetApp().getMenu("opendecfile"), wxString(l_openpath,wxConvUTF8) , _T(""), ms_FileExt, wxFD_OPEN);
  if( fdlg->ShowModal() == wxID_OK ) {
    if( m_DecFilename != NULL )
      StrOp.free(m_DecFilename);
    m_DecFilename = StrOp.dup(fdlg->GetPath().mb_str(wxConvUTF8));
    wGui.setdecpath( wxGetApp().getIni(), m_DecFilename );
    wGui.setdecpath( wxGetApp().getIni(), FileOp.getPath(wGui.getdecpath( wxGetApp().getIni() ) ) );
    m_DecFile->SetValue(wxString(FileOp.ripPath(m_DecFilename),wxConvUTF8));
    if( m_LocoProps != NULL ) {
      wLoc.setdecfile(m_LocoProps, m_DecFile->GetValue().mb_str(wxConvUTF8));

      if( !wxGetApp().isStayOffline() ) {
        /* Notify RocRail. */
        iONode cmd = NodeOp.inst( wModelCmd.name(), NULL, ELEMENT_NODE );
        wModelCmd.setcmd( cmd, wModelCmd.modify );
        NodeOp.addChild( cmd, (iONode)NodeOp.base.clone( m_LocoProps ) );
        wxGetApp().sendToRocrail( cmd );
        NodeOp.base.del(cmd);
      }
      else {
        wxGetApp().setLocalModelModified(true);
      }
    }
    loadDecFile();
  }
  fdlg->Destroy();
}


void RocProDlg::onImgOpen( wxCommandEvent& event ) {
  const char* imagepath = wGui.getimagepath( wxGetApp().getIni() );
  TraceOp.trc( "locdlg", TRCLEVEL_INFO, __LINE__, 9999, "imagepath = [%s]", imagepath );
  wxFileDialog* fdlg = new wxFileDialog(this, _T("Search decoder image"),
      wxString(imagepath,wxConvUTF8), _T(""),
      _T("PNG files (*.png)|*.png|GIF files (*.gif)|*.gif|XPM files (*.xpm)|*.xpm"), wxFD_OPEN);
  if( fdlg->ShowModal() == wxID_OK ) {
    wLoc.setdecimage( m_LocoProps, FileOp.ripPath(fdlg->GetPath().mb_str(wxConvUTF8)) );
    wxBitmapType bmptype = wxBITMAP_TYPE_XPM;
    if( StrOp.endsWithi( fdlg->GetPath().mb_str(wxConvUTF8), ".gif" ) )
      bmptype = wxBITMAP_TYPE_GIF;
    else if( StrOp.endsWithi( fdlg->GetPath().mb_str(wxConvUTF8), ".png" ) )
      bmptype = wxBITMAP_TYPE_PNG;
    m_DecoderImage->SetBitmap( wxBitmap( fdlg->GetPath(), bmptype ) );
    m_DecoderImage->Refresh();
    m_Image->SetValue(wxString(wLoc.getdecimage(m_LocoProps),wxConvUTF8));

    if( !wxGetApp().isStayOffline() ) {
      /* Notify RocRail. */
      iONode cmd = NodeOp.inst( wModelCmd.name(), NULL, ELEMENT_NODE );
      wModelCmd.setcmd( cmd, wModelCmd.modify );
      NodeOp.addChild( cmd, (iONode)NodeOp.base.clone( m_LocoProps ) );
      wxGetApp().sendToRocrail( cmd );
      NodeOp.base.del(cmd);
    }
  }
  fdlg->Destroy();
}


void RocProDlg::importJMRI(iONode decoder) {
  if( m_CVConf == NULL ) {
    char* catfile = StrOp.fmt("%s%c%s", wGui.getdecpath( wxGetApp().getIni() ), SystemOp.getFileSeparator(), "categories.xml" );
    if( FileOp.exist(catfile) ) {
      iOFile f = FileOp.inst( catfile, OPEN_READONLY );
      char* buffer = (char*)allocMem( FileOp.size( f ) +1 );
      FileOp.read( f, buffer, FileOp.size( f ) );
      FileOp.base.del( f );
      iODoc doc = DocOp.parse( buffer );
      if( doc != NULL ) {
        m_CVConf = DocOp.getRootNode( doc );
        DocOp.base.del( doc );
      }
    }

    if( m_CVConf == NULL )
      m_CVConf = wGui.getcvconf( wxGetApp().getIni() );

    m_CatMap = MapOp.inst();
    if( m_CVConf != NULL ) {
      iONode cat = wCVconf.getcvcat(m_CVConf);
      while( cat != NULL ) {
        iOStrTok tok = StrTokOp.inst(wCVcat.getnrs(cat), ',');
        while( StrTokOp.hasMoreTokens(tok) ) {
          const char* nr = StrTokOp.nextToken(tok);
          MapOp.put(m_CatMap, nr, (obj)wCVcat.getname(cat));
        }
        StrTokOp.base.del(tok);
        cat = wCVconf.nextcvcat(m_CVConf, cat);
      }
    }
  }

  iONode l_DecNode = NodeOp.inst("decoder", NULL, ELEMENT_NODE);
  iONode fam = NodeOp.findNode( decoder, wDecFamily.name());
  if( fam != NULL ) {
    NodeOp.setStr( l_DecNode, "type", wDecFamily.getname(fam));
    NodeOp.setStr( l_DecNode, "manu", wDecFamily.getmfg(fam));
  }
  iONode vars = wDecoder.getvariables(decoder);
  if( vars != NULL ) {
    iONode var = wDecVariables.getvariable(vars);
    while( var != NULL ) {
      iONode cv = NodeOp.inst(wCVByte.name(), l_DecNode, ELEMENT_NODE);
      const char* cat = (const char*)MapOp.get( m_CatMap, NodeOp.getStr(var, "CV", "0"));
      if( cat != NULL )
        wCVByte.setcat(cv, cat);
      else
        wCVByte.setcat(cv, "General"); // JMRI does not provide categories.
      wCVByte.setnr(cv, wDecVariable.getCV(var));
      wCVByte.setdesc(cv, wDecVariable.getlabel(var));
      wCVByte.setinfo(cv, wDecVariable.getitem(var));
      if( StrOp.len(wDecVariable.getcomment(var)) > 0 )
        wCVByte.setinfo(cv, wDecVariable.getcomment(var));
      else if( StrOp.len(wDecVariable.gettooltip(var)) > 0 )
        wCVByte.setinfo(cv, wDecVariable.gettooltip(var));
      NodeOp.addChild( l_DecNode, cv );
      var = wDecVariables.nextvariable(vars, var);
    }
  }
  NodeOp.base.del(m_DecNode);
  m_DecNode = l_DecNode;

}


bool RocProDlg::parseDecFile() {
  if( StrOp.len(m_DecFilename) > 0 && FileOp.exist(m_DecFilename) ) {
    iOFile f = FileOp.inst( m_DecFilename, OPEN_READONLY );
    char* buffer = (char*)allocMem( FileOp.size( f ) +1 );
    FileOp.read( f, buffer, FileOp.size( f ) );
    FileOp.base.del( f );
    iODoc doc = DocOp.parse( buffer );
    if( doc != NULL ) {
      m_DecNode = DocOp.getRootNode( doc );
      DocOp.base.del( doc );

      if( m_DecNode != NULL ) {
        iONode decoder = NodeOp.findNode(m_DecNode, wDecoder.name());
        if( decoder != NULL ) {
          // JMRI definition; Translate.
          importJMRI(decoder);
        }

        return true;
      }
      else {
        TraceOp.trc( "frame", TRCLEVEL_EXCEPTION, __LINE__, 9999, "DecFile [%s] is not parseable!", m_DecFilename );
      }
    }
    else {
      TraceOp.trc( "frame", TRCLEVEL_EXCEPTION, __LINE__, 9999, "DecFile [%s] is not parseable!", m_DecFilename );
    }
  }
  return false;
}


void RocProDlg::onOK( wxCommandEvent& event ) {
  wCVconf.setsave(m_CVconf, m_Save?True:False);
  EndModal( wxID_OK );
}

void RocProDlg::onClose( wxCloseEvent& event ) {
  wCVconf.setsave(m_CVconf, m_Save?True:False);
  EndModal(0);
}


void RocProDlg::updateCV(int cv, int value) {

  if( m_bSpeedCurve && m_CVoperation == wProgram.get && cv >= 67 && cv <= 94) {
    m_Curve[m_PendingCV-67] = value;
  }
  else if( cv == 17 ) {
    m_CV17 = value;
    int laddr = (m_CV17&0x3f) * 256 + m_CV18;
    m_ExtAddr->SetValue( laddr );
  }
  else if( cv == 18 ) {
    m_CV18 = value;
    int laddr = (m_CV17&0x3f) * 256 + m_CV18;
    m_ExtAddr->SetValue( laddr );
  }
  else {
    if( cv > 0 )
      m_Nr->SetValue( cv );
    if( cv != -1 )
      setCVVal(value);
  }

}


void RocProDlg::event(iONode node) {
  if( StrOp.equals(NodeOp.getName(node), wProgram.name() ) ) {
    int cmd = wProgram.getcmd(node);
    int cv  = wProgram.getcv (node);
    int value = wProgram.getvalue(node);

    TraceOp.trc( "rocpro", TRCLEVEL_INFO, __LINE__, 9999, "event cmd=%d cv=%d value=%d", cmd, cv, value );

    if( cv == -1 || (cmd == wProgram.datarsp && cv > 0 && cv != m_CVexpected) ) {
      // forced data response
      TraceOp.trc( "rocpro", TRCLEVEL_INFO, __LINE__, 9999, "reject CV%d(%d)=%d ", cv, m_CVexpected, value);
      if( cmd == wProgram.datarsp && cv > 0 ) {
        updateCV(cv, value);
      }
      return;
    }


    if( cmd == wProgram.datarsp || cmd == wProgram.statusrsp ) {

      if( m_bSpeedCurve && m_PendingCV >= 67 && m_PendingCV <= 94) {
        if( m_CVoperation == wProgram.get ) {
          if( cv == 0 || cv ==  m_PendingCV ) {
            m_Curve[m_PendingCV-67] = value;
            if(m_PendingCV == 94) {
              m_bSpeedCurve = false;
              m_VCurve->Enable(true);
              onVCurve();
              return;
            }
          }
          else {
            return;
          }
        }
        m_PendingCV++;

        if(m_PendingCV == 94 && m_CVoperation == wProgram.set) {
          m_bSpeedCurve = false;
          m_VCurve->Enable(true);
        }

        doCV( m_CVoperation?wProgram.set:wProgram.get, m_PendingCV, m_Curve[m_PendingCV-67] );
      }
      else if( m_bLongAddress && m_PendingCV == 17 ) {
        m_CV17 = value;
        int laddr = (m_CV17&0x3f) * 256 + m_CV18;
        TraceOp.trc( "rocpro", TRCLEVEL_INFO, __LINE__, 9999, "part 1 of long address(%d) cv%d=%d",
            laddr, m_PendingCV, value );
        m_ExtAddr->SetValue( laddr );
        m_PendingCV = 18;
        doCV( wProgram.get, 18, 0 );
      }
      else if( m_bLongAddress && m_PendingCV == 18 ) {
        m_CV18 = value;
        int laddr = (m_CV17&0x3f) * 256 + m_CV18;
        TraceOp.trc( "rocpro", TRCLEVEL_INFO, __LINE__, 9999, "part 2 of long address(%d) cv%d=%d",
            laddr, m_PendingCV, value );
        m_ExtAddr->SetValue( laddr );
        m_bLongAddress = false;
      }
      else {
        if( m_CVoperation == CVGETALL ) {
          m_CVidxAll = m_CVidxAll+1;
          if( m_CVidxAll >= m_CVcountAll ) {
            m_CVoperation = 0;
          }
          else {
            doCV( wProgram.get, m_CVall[m_CVidxAll], 0 );
          }
        }
        if( cv > 0 )
          m_Nr->SetValue( cv );
        if( cv != -1 )
          setCVVal(value);
      }
    }

  }
}

static int locComparator(obj* o1, obj* o2) {
  if( *o1 == NULL || *o2 == NULL )
    return 0;
  return strcmp( wLoc.getid( (iONode)*o1 ), wLoc.getid( (iONode)*o2 ) );
}

void RocProDlg::initLocMap(const char* locid) {
  iONode model = wxGetApp().getModel();
  if( model != NULL ) {
    int i = 0;
    iOList list = ListOp.inst();
    iONode lclist  = wPlan.getlclist( model );
    iONode carlist = wPlan.getcarlist( model );
    iONode declist = wPlan.getdeclist( model );

    if( lclist != NULL ) {
      int cnt = NodeOp.getChildCnt( lclist );
      for( i = 0; i < cnt; i++ ) {
        iONode lc = NodeOp.getChild( lclist, i );
        ListOp.add( list, (obj)lc );
      }
    }

    if( carlist != NULL ) {
      int cnt = NodeOp.getChildCnt( carlist );
      for( i = 0; i < cnt; i++ ) {
        iONode car = NodeOp.getChild( carlist, i );
        if( wCar.getaddr(car) > 0 )
          ListOp.add( list, (obj)car );
      }
    }

    if( declist != NULL ) {
      int cnt = NodeOp.getChildCnt( declist );
      for( i = 0; i < cnt; i++ ) {
        iONode dec = NodeOp.getChild( declist, i );
        if( wDec.getaddr(dec) > 0 )
          ListOp.add( list, (obj)dec );
      }
    }

    // Sort the list:
    ListOp.sort( list, locComparator );

    for( i = 0; i < ListOp.size( list ); i++ ) {
      iONode lc = (iONode)ListOp.get( list, i );
      if( lc == NULL )
        continue;
      const char* id = wLoc.getid( lc );
    }

    for( i = 0; i < ListOp.size( list ); i++ ) {
      iONode lc = (iONode)ListOp.get( list, i );
      if( lc == NULL )
        continue;
      const char* id = wLoc.getid( lc );
      if( id != NULL && wLoc.getaddr(lc) > 0 && wLoc.isshow(lc) ) {
        m_LocoList->Append( wxString(id,wxConvUTF8), (void*)lc );
      }
    }

    if( ListOp.size( list ) > 0 ) {
      if( locid == NULL && m_LocoList->GetCount() > 0 )
        m_LocoList->SetSelection(0);
      else
        m_LocoList->SetStringSelection(wxString(locid,wxConvUTF8));
      wxCommandEvent event( 0, -1 );
      onLocoList(event);
    }

    ListOp.base.del( list );

  }
}



void RocProDlg::onLocoList(wxCommandEvent& event) {
  if( m_LocoList->GetSelection() == wxNOT_FOUND )
    return;

  m_LocoProps = (iONode)m_LocoList->GetClientData(m_LocoList->GetSelection());

  if( m_LocoProps != NULL && wLoc.getimage( m_LocoProps ) != NULL && StrOp.len(wLoc.getimage( m_LocoProps )) > 0 ) {
    wxBitmapType bmptype = wxBITMAP_TYPE_XPM;
    if( StrOp.endsWithi( wLoc.getimage( m_LocoProps ), ".gif" ) )
      bmptype = wxBITMAP_TYPE_GIF;
    else if( StrOp.endsWithi( wLoc.getimage( m_LocoProps ), ".png" ) )
      bmptype = wxBITMAP_TYPE_PNG;
    else if( StrOp.endsWithi( wLoc.getimage( m_LocoProps ), ".jpg" ) )
      bmptype = wxBITMAP_TYPE_JPEG;

    const char* imagepath = wGui.getimagepath(wxGetApp().getIni());
    static char pixpath[256];
    StrOp.fmtb( pixpath, "%s%c%s", imagepath, SystemOp.getFileSeparator(), FileOp.ripPath( wLoc.getimage( m_LocoProps ) ) );

    if( FileOp.exist(pixpath)) {
      TraceOp.trc( "rocpro", TRCLEVEL_INFO, __LINE__, 9999, "picture [%s]", pixpath );
      m_LocoImage->SetBitmap( wxBitmap(wxString(pixpath,wxConvUTF8), bmptype) );
    }
    else {
      TraceOp.trc( "rocpro", TRCLEVEL_WARNING, __LINE__, 9999, "picture [%s] not found", pixpath );
      m_LocoImage->SetBitmap( wxBitmap(nopict_xpm) );
    }
    m_LocoImage->SetToolTip(wxString(wLoc.getdesc( m_LocoProps ),wxConvUTF8));
  }
  else {
    m_LocoImage->SetBitmap( wxBitmap(nopict_xpm) );
  }
  m_LocoImage->Refresh();

  if( m_LocoProps != NULL && wLoc.getdecimage( m_LocoProps ) != NULL && StrOp.len(wLoc.getdecimage( m_LocoProps )) > 0 ) {
    wxBitmapType bmptype = wxBITMAP_TYPE_XPM;
    if( StrOp.endsWithi( wLoc.getimage( m_LocoProps ), ".gif" ) )
      bmptype = wxBITMAP_TYPE_GIF;
    else if( StrOp.endsWithi( wLoc.getimage( m_LocoProps ), ".png" ) )
      bmptype = wxBITMAP_TYPE_PNG;
    else if( StrOp.endsWithi( wLoc.getimage( m_LocoProps ), ".jpg" ) )
      bmptype = wxBITMAP_TYPE_JPEG;

    const char* imagepath = wGui.getimagepath(wxGetApp().getIni());
    static char pixpath[256];
    StrOp.fmtb( pixpath, "%s%c%s", imagepath, SystemOp.getFileSeparator(), FileOp.ripPath( wLoc.getdecimage( m_LocoProps ) ) );

    if( FileOp.exist(pixpath)) {
      TraceOp.trc( "rocpro", TRCLEVEL_INFO, __LINE__, 9999, "picture [%s]", pixpath );
      m_DecoderImage->SetBitmap( wxBitmap(wxString(pixpath,wxConvUTF8), bmptype) );
    }
    else {
      TraceOp.trc( "rocpro", TRCLEVEL_WARNING, __LINE__, 9999, "picture [%s] not found", pixpath );
      iONode node = NodeOp.inst( wDataReq.name(), NULL, ELEMENT_NODE );
      wDataReq.setid( node, wLoc.getid(m_LocoProps) );
      wDataReq.setcmd( node, wDataReq.get );
      wDataReq.settype( node, wDataReq.image );
      wDataReq.setfilename( node, wLoc.getdecimage(m_LocoProps) );
      wxGetApp().sendToRocrail( node );
      NodeOp.base.del(node);
    }
    //m_DecoderImage->SetToolTip(wxString(wLoc.getdesc( m_LocoProps ),wxConvUTF8));
  }
  else {
    m_DecoderImage->SetBitmap( wxBitmap(0,0) );
  }
  m_DecoderImage->Refresh();

  if( m_LocoProps != NULL && StrOp.len(wLoc.getdecfile(m_LocoProps)) > 0 ) {
    char* decfile = StrOp.fmt("%s%c%s", wGui.getdecpath( wxGetApp().getIni() ), SystemOp.getFileSeparator(), wLoc.getdecfile(m_LocoProps) );
    if( FileOp.exist(decfile) ) {
      if( m_DecFilename != NULL )
        StrOp.free(m_DecFilename);
      m_DecFilename = StrOp.dup(decfile);
      m_DecFile->SetValue(wxString(FileOp.ripPath(decfile),wxConvUTF8));
      loadDecFile();
    }
    StrOp.free(decfile);
  }

  if( m_LocoProps != NULL && wLoc.getdecimage(m_LocoProps) != NULL && StrOp.len(wLoc.getdecimage(m_LocoProps)) > 0 ) {
    m_Image->SetValue(wxString(wLoc.getdecimage(m_LocoProps),wxConvUTF8));
  }
  else {
    m_Image->SetValue(_T(""));
  }

  m_IID->SetValue( wxT("") );
  if( m_LocoProps != NULL ) {
    m_IID->SetValue(wxString( wLoc.getiid(m_LocoProps), wxConvUTF8));
  }
}

iONode RocProDlg::getLocoCV(int nr) {
  if( m_LocoProps != NULL ) {
    iONode cv = wLoc.getcvbyte(m_LocoProps);
    while( cv != NULL ) {
      if( wCVByte.getnr(cv) == nr )
        return cv;
      cv = wLoc.nextcvbyte(m_LocoProps, cv);
    }
  }
  return NULL;
}

void RocProDlg::setCVVal(int val, bool updateval) {
  if( val >= 0 ) {
    if(updateval) {
      m_Value->SetValue(val);
      char hexval[32];
      StrOp.fmtb( hexval, "%X", val);
      m_HexValue->SetValue(wxString(hexval,wxConvUTF8));
    }
    m_ValueSlider->SetValue(val);
    m_Bit0->SetValue(val&0x01?true:false);
    m_Bit1->SetValue(val&0x02?true:false);
    m_Bit2->SetValue(val&0x04?true:false);
    m_Bit3->SetValue(val&0x08?true:false);
    m_Bit4->SetValue(val&0x10?true:false);
    m_Bit5->SetValue(val&0x20?true:false);
    m_Bit6->SetValue(val&0x40?true:false);
    m_Bit7->SetValue(val&0x80?true:false);

    if( m_Save && m_Nr->GetValue() > 0 ) {
      iONode cv = getLocoCV(m_Nr->GetValue());
      if( cv == NULL || wCVByte.getvalue(cv) != m_Value->GetValue() ) {
        // Change the value in the local copy too.
        wCVByte.setvalue(cv, m_Value->GetValue());
        doCV( wProgram.save, m_Nr->GetValue(), m_Value->GetValue() );
      }
    }

  }
}

void RocProDlg::onVCurve( wxCommandEvent& event ) {
  int action = wxMessageDialog( this, wxGetApp().getMsg("readspeedcurve"), _T("Rocrail"), wxYES_NO ).ShowModal();
  if( action == wxID_NO ) {
    for( int i = 0; i < 28; i++ ) {
      m_Curve[i] = 0;
    }
    onVCurve();
  }
  else {
    m_bSpeedCurve = true;
    m_CVoperation = wProgram.get;
    m_PendingCV = 67;
    m_VCurve->Enable(false);
    doCV( m_CVoperation, m_PendingCV, 0 );
  }
}

void RocProDlg::onVCurve() {
  for( int i = 0; i < 28; i++ ) {
    iONode lococv = getLocoCV(i+67);
    if( lococv != NULL ) {
      m_Curve[i] = wCVByte.getvalue(lococv);
    }
    TraceOp.trc( "rocpro", TRCLEVEL_INFO, __LINE__, 9999, "m_Curve[%d]=%d", i, m_Curve[i] );
  }

  SpeedCurveDlg*  dlg = new SpeedCurveDlg(this, m_Curve );
  if( wxID_OK == dlg->ShowModal() ) {
    int* newCurve = dlg->getCurve();
    for( int i = 0; i < 28; i++ ) {
      m_Curve[i] = newCurve[i];
    }
    m_bSpeedCurve = true;
    m_CVoperation = wProgram.set;
    m_PendingCV = 67;
    m_VCurve->Enable(false);
    TraceOp.trc( "rocpro", TRCLEVEL_INFO, __LINE__, 9999, "m_PendingCV=%d", m_PendingCV );
    doCV( m_CVoperation, m_PendingCV, m_Curve[0] );
  }
  dlg->Destroy();
}

void RocProDlg::onValueSlider( wxScrollEvent& event ) {
  setCVVal(m_ValueSlider->GetValue());
}
void RocProDlg::onValue( wxSpinEvent& event ) {
  setCVVal(m_Value->GetValue());
}
void RocProDlg::onValueText( wxCommandEvent& event ) {
  setCVVal(m_Value->GetValue(), false);
}
void RocProDlg::onBit( wxCommandEvent& event ) {
  int val = m_Bit0->IsChecked()?0x01:0;
  val += m_Bit1->IsChecked()?0x02:0;
  val += m_Bit2->IsChecked()?0x04:0;
  val += m_Bit3->IsChecked()?0x08:0;
  val += m_Bit4->IsChecked()?0x10:0;
  val += m_Bit5->IsChecked()?0x20:0;
  val += m_Bit6->IsChecked()?0x40:0;
  val += m_Bit7->IsChecked()?0x80:0;
  setCVVal(val);
}

void RocProDlg::onNr( wxSpinEvent& event ) {
  int nr = m_Nr->GetValue();
  char key[32];
  StrOp.fmtb( key, "%d", nr );
  iONode cv = (iONode)MapOp.get( m_CVNrMap, key );
  iONode lococv = getLocoCV(nr);
  if( lococv != NULL ) {
    setCVVal(wCVByte.getvalue(lococv));
  }
  else {
    setCVVal(0);
  }

  if( cv != NULL ) {
    m_CVURL->Enable( wCVByte.geturl(cv) != NULL );
    m_DIP->Enable( wCVByte.getadip(cv) != NULL );
    if(wCVByte.getdipid(cv) != NULL && StrOp.len(wCVByte.getdipid(cv)) > 0 ) {
      if( MapOp.haskey(m_DIPMap, wCVByte.getdipid(cv) ) )
        m_DIP->Enable( true );
    }
  }

  if( cv != NULL )
    m_WriteCV->Enable(wCVByte.isreadonly(cv)?false:true);
  else
    m_WriteCV->Enable(true);
}
void RocProDlg::onNrText( wxCommandEvent& event ) {

}



void RocProDlg::onSaveAs( wxCommandEvent& event ) {
  wxString ms_FileExt = wxGetApp().getMsg("planfiles"); // ToDo: "decfiles"
  const char* l_openpath = wGui.getopenpath( wxGetApp().getIni() );
  TraceOp.trc( "rocpro", TRCLEVEL_INFO, __LINE__, 9999, "openpath=%s", l_openpath );
  wxFileDialog* fdlg = new wxFileDialog(this, wxGetApp().getMenu("savedecfile"), wxString(l_openpath,wxConvUTF8) , _T(""), ms_FileExt, wxFD_SAVE);
  if( fdlg->ShowModal() == wxID_OK ) {
    wxString path = fdlg->GetPath();
    if( FileOp.exist( path.mb_str(wxConvUTF8) ) ) {
      int action = wxMessageDialog( wxGetApp().getFrame(), wxGetApp().getMsg("fileexistwarning"), _T("Rocrail"), wxYES_NO | wxICON_EXCLAMATION ).ShowModal();
      if( action == wxID_NO ) {
        fdlg->Destroy();
        return;
      }
    }
    if( !path.Contains( _T(".xml") ) )
      path.Append( _T(".xml") );

    iOFile f = FileOp.inst( path.mb_str(wxConvUTF8), OPEN_WRITE );
    if( f != NULL ) {
      char* dec = NodeOp.base.toString(m_DecNode);
      FileOp.writeStr( f, dec );
      FileOp.base.del( f );
    }
  }
  fdlg->Destroy();
}


void RocProDlg::onRead( wxCommandEvent& event ) {
  doCV( wProgram.get, m_Nr->GetValue(), 0 );
}

void RocProDlg::onWrite( wxCommandEvent& event ) {
  doCV( wProgram.set, m_Nr->GetValue(), m_Value->GetValue() );
}

void RocProDlg::onSaveCV( wxCommandEvent& event ) {
  m_Save = m_SaveCV->GetValue();
}

void RocProDlg::doCV(int command, int nr, int value) {
  m_CVexpected = nr;
  iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  wProgram.setcmd( cmd, command );
  wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8)  );
  TraceOp.trc( "rocpro", TRCLEVEL_INFO, __LINE__, 9999, "doCV cmd=%d cv=%d value=%d", command, nr, value );
  if( m_LocoProps != NULL ) {
    int addr = wLoc.getaddr(m_LocoProps);
    wProgram.setaddr( cmd, addr );
    wProgram.setlongaddr( cmd, (addr > 127) ? True:False );
    wProgram.setdecaddr( cmd, addr );
    wProgram.setfilename( cmd, wLoc.getid( m_LocoProps ) );
    wProgram.setidentifier( cmd, wLoc.getidentifier( m_LocoProps ) );
    wProgram.setstrval1(cmd, wLoc.getprot(m_LocoProps));
  }
  wProgram.setcv( cmd, nr );
  wProgram.setvalue( cmd, value );
  wProgram.setpom( cmd, m_POM->IsChecked()?True:False );
  wProgram.setmode( cmd, m_Mode->GetSelection() );
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);
}


void RocProDlg::onMenu( wxCommandEvent& event ) {
  int nr = event.GetId();
  if( nr == 1001 ) {
    wxLaunchDefaultBrowser(wxString( NodeOp.getStr(m_DecNode, "web", ""), wxConvUTF8), wxBROWSER_NEW_WINDOW );
  }
  else if( nr == 1002 ) {
    wxCommandEvent cmd;
    onDIP(cmd);
  }
  else if( nr == 1003 ) {
    wxCommandEvent cmd;
    onCVURL(cmd);
  }
}

void RocProDlg::onTreeItemPopup( wxTreeEvent& event ) {
  wxString itemText = m_DecTree->GetItemText(event.GetItem());
  const char* desc = itemText.mb_str(wxConvUTF8);
  iONode cv = (iONode)MapOp.get( m_CVMap, desc );
  if( cv != NULL ) {
    wxMenu menu( wxString::Format(_T("cv %d"), wCVByte.getnr(cv)) );
    menu.Append( 1001, wxGetApp().getMenu("info") );
    if( m_DIP->IsEnabled() )
      menu.Append( 1002, wxT("DIP") );
    if( m_CVURL->IsEnabled() )
      menu.Append( 1003, wxT("URL") );
    menu.Connect( wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( RocProDlg::onMenu ), NULL, this );
    PopupMenu(&menu );
  }
}



void RocProDlg::onExtAddrRead( wxCommandEvent& event ) {
  TraceOp.trc( "rocpro", TRCLEVEL_INFO, __LINE__, 9999, "get long address..." );
  m_CVoperation = wProgram.get;
  m_bLongAddress = true;
  m_PendingCV = 17;
  doCV( m_CVoperation, m_PendingCV, 0 );
}


void RocProDlg::onExtAddrWrite( wxCommandEvent& event ) {
  int addr = m_ExtAddr->GetValue();
  TraceOp.trc( "rocpro", TRCLEVEL_INFO, __LINE__, 9999, "set long address to %d...", addr );
  m_CVoperation = wProgram.set;
  doCV( m_CVoperation, 17, (addr / 256) + 192 );
  doCV( m_CVoperation, 18, addr - 256 * (addr / 256) );

  if( m_ExtAddrBidi->IsChecked() ) {
    doCV( m_CVoperation, 117, (addr / 256) + 192 );
    doCV( m_CVoperation, 118, addr - 256 * (addr / 256) );
  }

  if( m_LocoProps != NULL ) {
    wLoc.setaddr(m_LocoProps, addr);
    if( !wxGetApp().isStayOffline() ) {
      /* Notify RocRail. */
      iONode cmd = NodeOp.inst( wModelCmd.name(), NULL, ELEMENT_NODE );
      wModelCmd.setcmd( cmd, wModelCmd.modify );
      NodeOp.addChild( cmd, (iONode)NodeOp.base.clone( m_LocoProps ) );
      wxGetApp().sendToRocrail( cmd );
      NodeOp.base.del(cmd);
    }
  }
}


void RocProDlg::onPT( wxCommandEvent& event ) {
  iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  wProgram.setcmd( cmd, m_PT->GetValue()?wProgram.pton:wProgram.ptoff );
  wxGetApp().sendToRocrail( cmd );
  NodeOp.base.del(cmd);
  m_PT->SetLabel( m_PT->GetValue() ? wxT("PT is on"):wxT("PT") );
}


void RocProDlg::onReadAllCV( wxCommandEvent& event ) {
  m_CVidx = 0;
  m_CVidxAll = 0;
  m_CVoperation = CVGETALL;
  TraceOp.trc( "rocpro", TRCLEVEL_INFO, __LINE__, 9999, "reading m_CVall[%d]=%d", m_CVidxAll, m_CVall[m_CVidxAll] );
  doCV( wProgram.get, m_CVall[m_CVidxAll], 0 );
}

void RocProDlg::onCVInfoEnter( wxCommandEvent& event ) {
  if( m_SelectedCV != NULL ) {
    TraceOp.trc( "rocpro", TRCLEVEL_INFO, __LINE__, 9999, "save changed CV info." );
    wCVByte.setinfo(m_SelectedCV, m_Info->GetValue().mb_str(wxConvUTF8));
    if( StrOp.len(m_DecFilename) > 0 ) {
      iOFile f = FileOp.inst( m_DecFilename, OPEN_WRITE );
      if( f != NULL ) {
        char* dec = NodeOp.base.toString(m_DecNode);
        FileOp.writeStr( f, dec );
        FileOp.base.del( f );
      }
    }
  }
}


void RocProDlg::onHexValue( wxCommandEvent& event ) {
  char* hexval = StrOp.dup((const char*)m_HexValue->GetValue().mb_str(wxConvUTF8));
  long lhexval = strtol(hexval, (char **)NULL, 16);
  setCVVal(lhexval, true);
}


void RocProDlg::onDIP( wxCommandEvent& event ) {
  if( m_SelectedCV == NULL ) {
    return;
  }

  iONode dip = NULL;
  if(wCVByte.getdipid(m_SelectedCV) != NULL && StrOp.len(wCVByte.getdipid(m_SelectedCV)) > 0 ) {
    dip = (iONode)MapOp.get(m_DIPMap, wCVByte.getdipid(m_SelectedCV) );
  }
  if( dip == NULL )
    dip = wCVByte.getadip(m_SelectedCV);

  if( dip == NULL )
    return;

  DIPDlg*  dlg = new DIPDlg(this, dip, m_Nr->GetValue(), m_Value->GetValue(), wCVByte.getdesc(m_SelectedCV) );
  int rc = dlg->ShowModal();
  if( rc == wxID_OK ) {
    int cv = 0;
    int val = dlg->getValue(&cv);
    if( cv > 0 )
      m_Nr->SetValue(cv);
    doCV( wProgram.set, m_Nr->GetValue(), val );
  }
  dlg->Destroy();
}

void RocProDlg::onTreeDIP( wxTreeEvent& event ) {
  if( m_SelectedCV == NULL ) {
    // call default handler
    event.Skip(true);
    return;
  }
  wxCommandEvent cmd;
  onDIP(cmd);
}


void RocProDlg::onCVURL( wxCommandEvent& event ) {
  if( m_SelectedCV != NULL && wCVByte.geturl(m_SelectedCV) != NULL ) {
    wxLaunchDefaultBrowser(wxString( wCVByte.geturl(m_SelectedCV), wxConvUTF8), wxBROWSER_NEW_WINDOW );
  }
}

