/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2014 Rob Versluis, Rocrail.net

 


 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 3
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "accdecdlg.h"

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/defs.h"
#endif

#include "rocs/public/system.h"

#include "rocrail/wrapper/public/Plan.h"
#include "rocrail/wrapper/public/Dec.h"
#include "rocrail/wrapper/public/DecList.h"
#include "rocrail/wrapper/public/Item.h"
#include "rocrail/wrapper/public/ModelCmd.h"
#include "rocrail/wrapper/public/DataReq.h"
#include "rocrail/wrapper/public/Switch.h"
#include "rocrail/wrapper/public/CVByte.h"

#include "rocview/public/guiapp.h"
#include "rocview/wrapper/public/Gui.h"

#include "rocview/xpm/nopict.xpm"

AccDecDlg::AccDecDlg( wxWindow* parent ):AccDecGenDlg( parent )
{
  m_Props = NULL;
  m_Image->SetBitmapLabel( wxBitmap(nopict_xpm) );

  initLabels();
  initCVDesc();

  m_IndexPanel->GetSizer()->Layout();
  m_GeneralPanel->GetSizer()->Layout();

  m_Notebook->Fit();

  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);

  initIndex();
}

void AccDecDlg::onAdd( wxCommandEvent& event )
{
  TraceOp.trc( "accdecdlg", TRCLEVEL_INFO, __LINE__, 9999, "Add" );
  int i = findID("NEW");
  if( i == wxNOT_FOUND ) {
    iONode model = wxGetApp().getModel();
    if( model != NULL ) {
      iONode declist = wPlan.getdeclist( model );
      if( declist == NULL ) {
        declist = NodeOp.inst( wDecList.name(), model, ELEMENT_NODE );
        NodeOp.addChild( model, declist );
      }
      if( declist != NULL ) {
        iONode dec = NodeOp.inst( wDec.name(), declist, ELEMENT_NODE );
        NodeOp.addChild( declist, dec );
        wDec.setid( dec, "NEW" );
        m_Props = dec;
        appendItem(dec);
        setIDSelection(wItem.getid(dec));
        initValues();
      }
    }
  }
}

void AccDecDlg::onDelete( wxCommandEvent& event )
{
  TraceOp.trc( "accdecdlg", TRCLEVEL_INFO, __LINE__, 9999, "Delete" );
  if( m_Props != NULL ) {
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
      iONode declist = wPlan.getdeclist( model );
      if( declist != NULL ) {
        NodeOp.removeChild( declist, m_Props );
        m_Props = selectPrev();
      }
    }

    initIndex();
  }
}

void AccDecDlg::onCancel( wxCommandEvent& event )
{
  EndModal( wxID_CANCEL );
}

void AccDecDlg::onOK( wxCommandEvent& event )
{
  onApply(event);
  EndModal( wxID_OK );
}

void AccDecDlg::initLabels() {
  TraceOp.trc( "accdecdlg", TRCLEVEL_INFO, __LINE__, 9999, "initLabels" );
  SetTitle(wxGetApp().getMsg( "accdectable" ));
  m_Notebook->SetPageText( 0, wxGetApp().getMsg( "index" ) );
  m_Notebook->SetPageText( 1, wxGetApp().getMsg( "general" ) );
  m_Add->SetLabel( wxGetApp().getMsg( "new" ) );
  m_Delete->SetLabel( wxGetApp().getMsg( "delete" ) );
  m_Doc->SetLabel( wxGetApp().getMsg( "doc_report" ) );

  m_labIID->SetLabel( wxGetApp().getMsg( "iid" ) );
  m_labID->SetLabel( wxGetApp().getMsg( "id" ) );
  m_labBus->SetLabel( wxGetApp().getMsg( "bus" ) );
  m_labAddr->SetLabel( wxGetApp().getMsg( "address" ) );
  m_labProt->SetLabel( wxGetApp().getMsg( "protocol" ) );
  m_labVersion->SetLabel( wxGetApp().getMsg( "protocol_version" ) );
  m_labImageFile->SetLabel( wxGetApp().getMsg( "image" ) );
  m_labDesc->SetLabel( wxGetApp().getMsg( "description" ) );

  m_labManu->SetLabel( wxGetApp().getMsg( "manufactured_ID" ) );
  m_labCatNr->SetLabel( wxGetApp().getMsg( "catnr" ) );

  m_StdButtonOK->SetLabel( wxGetApp().getMsg( "ok" ) );
  m_StdButtonCancel->SetLabel( wxGetApp().getMsg( "cancel" ) );
  m_StdButtonApply->SetLabel( wxGetApp().getMsg( "apply" ) );

  // Index
  initList(m_IndexList, this, false, true, false, false);

  m_Protocol->Clear();
  m_Protocol->Append(_T("Default"));
  m_Protocol->Append(_T("Motorola"));
  m_Protocol->Append(_T("NMRA-DCC"));

  //CV
  m_CVList->SetRowLabelSize(0);
  m_CVList->EnableEditing(false);
  m_CVList->EnableDragGridSize(false);
  m_CVList->SetSelectionMode(wxGrid::wxGridSelectRows);
  m_CVList->SetColLabelValue(0, _T("CV"));
  m_CVList->SetColLabelValue(1, wxGetApp().getMsg( "value" ));
  m_CVList->SetColLabelValue(2, wxGetApp().getMsg( "description" ));
  m_CVList->AutoSizeColumns();
  m_CVList->AutoSizeRows();
  m_CVDescModify->SetLabel( wxGetApp().getMsg( "modify" ) );
  m_labCVDesc->SetLabel( wxGetApp().getMsg( "description" ) );
  m_CVDescModify->Enable(false);
  m_CVDescription->Enable(false);


}

bool AccDecDlg::initIndex() {
  TraceOp.trc( "accdecdlg", TRCLEVEL_INFO, __LINE__, 9999, "initIndex" );
  iONode model = wxGetApp().getModel();
  if( model != NULL ) {
    iONode declist = wPlan.getdeclist( model );
    if( declist == NULL ) {
      declist = NodeOp.inst(wDecList.name(), model, ELEMENT_NODE);
      NodeOp.addChild(model, declist);
    }
    if( declist != NULL ) {
      fillIndex(declist);

      if( m_Props != NULL ) {
        setIDSelection(wDec.getid( m_Props ));
        return true;
      }
      else {
        m_Props = setSelection(0);
      }

    }
  }
  return false;
}


static int __sortCV(obj* _a, obj* _b)
{
    iONode a = (iONode)*_a;
    iONode b = (iONode)*_b;
    int nrA = wCVByte.getnr( a );
    int nrB = wCVByte.getnr( b );
    if( nrA == nrB )
      return 0;
    if( nrA > nrB )
      return 1;
    return -1;
}


void AccDecDlg::initValues() {
  TraceOp.trc( "accdecdlg", TRCLEVEL_INFO, __LINE__, 9999, "initValues" );
  if( m_Props == NULL ) {
    TraceOp.trc( "accdecdlg", TRCLEVEL_DEBUG, __LINE__, 9999, "no dec selected" );
    return;
  }

  m_CVDescModify->Enable(false);
  m_CVDescription->Enable(false);

  char* title = StrOp.fmt( "%s %s", (const char*)wxGetApp().getMsg("decoder").mb_str(wxConvUTF8), wDec.getid( m_Props ) );
  SetTitle( wxString(title,wxConvUTF8) );
  StrOp.free( title );

  // Init General
  if( wDec.getimage( m_Props ) != NULL && StrOp.len(wDec.getimage( m_Props )) > 0 ) {
    wxBitmapType bmptype = wxBITMAP_TYPE_XPM;
    if( StrOp.endsWithi( wDec.getimage( m_Props ), ".gif" ) )
      bmptype = wxBITMAP_TYPE_GIF;
    else if( StrOp.endsWithi( wDec.getimage( m_Props ), ".png" ) )
      bmptype = wxBITMAP_TYPE_PNG;

    const char* imagepath = wGui.getimagepath(wxGetApp().getIni());
    static char pixpath[256];
    StrOp.fmtb( pixpath, "%s%c%s", imagepath, SystemOp.getFileSeparator(), FileOp.ripPath( wDec.getimage( m_Props ) ) );

    if( FileOp.exist(pixpath) && StrOp.len(wDec.getimage( m_Props )) > 0 ) {
      TraceOp.trc( "accdecdlg", TRCLEVEL_INFO, __LINE__, 9999, "picture [%s]", pixpath );
      m_Image->SetBitmapLabel( wxBitmap(wxString(pixpath,wxConvUTF8), bmptype) );
    }
    else {
      TraceOp.trc( "accdecdlg", TRCLEVEL_WARNING, __LINE__, 9999, "picture [%s] not found", pixpath );
      m_Image->SetBitmapLabel( wxBitmap(nopict_xpm) );
      // request the image from server:
      iONode node = NodeOp.inst( wDataReq.name(), NULL, ELEMENT_NODE );
      wDataReq.setid( node, wDec.getid(m_Props) );
      wDataReq.setcmd( node, wDataReq.get );
      wDataReq.settype( node, wDataReq.image );
      wDataReq.setfilename( node, wDec.getimage(m_Props) );
      wxGetApp().sendToRocrail( node );
    }
  }
  else {
    m_Image->SetBitmapLabel( wxBitmap(nopict_xpm) );
  }
  m_Image->Refresh();

  // init General
  m_ID->SetValue( wxString(wDec.getid( m_Props ),wxConvUTF8) );
  m_IID->SetValue( wxString(wDec.getiid( m_Props ),wxConvUTF8) );
  char* str = StrOp.fmt( "%d", wDec.getbus( m_Props ) );
  m_Bus->SetValue( wxString(str,wxConvUTF8) ); StrOp.free( str );
  m_Addr->SetValue( wDec.getaddr( m_Props ) );
  m_Desc->SetValue( wxString(wDec.getdesc( m_Props ),wxConvUTF8) );

  if( StrOp.equals( wSwitch.prot_M, wDec.getprot( m_Props ) ) )
    m_Protocol->SetSelection( 1 );
  else if( StrOp.equals( wSwitch.prot_N, wDec.getprot( m_Props ) ) )
    m_Protocol->SetSelection( 2 );
  else
    m_Protocol->SetSelection( 0 );

  m_Version->SetValue( wDec.getprotver( m_Props) );
  m_Manu->SetValue( wxString(wDec.getmanu( m_Props ),wxConvUTF8) );
  m_CatNr->SetValue( wxString(wDec.getcatnr( m_Props ),wxConvUTF8) );
  m_ImageFile->SetValue( wxString(wDec.getimage( m_Props ),wxConvUTF8) );

  // CV's
  TraceOp.trc( "locdlg", TRCLEVEL_INFO, __LINE__, 9999, "CV list...");
  iOList list = ListOp.inst();
  iONode cv = wDec.getcvbyte( m_Props );
  while( cv != NULL ) {
    if( wCVByte.getnr( cv ) > 0 && wCVByte.getnr( cv ) <= 1024 ) {
      ListOp.add(list, (obj)cv);
    }
    cv = wDec.nextcvbyte( m_Props, cv );
  };

  TraceOp.trc( "locdlg", TRCLEVEL_INFO, __LINE__, 9999, "CVs...");
  if( m_CVList->GetNumberRows() > 0 )
    m_CVList->DeleteRows( 0, m_CVList->GetNumberRows() );
  ListOp.sort(list, &__sortCV);
  int cnt = ListOp.size( list );
  for( int i = 0; i < cnt && cnt < 1024; i++ ) {
    iONode cv = (iONode)ListOp.get( list, i );
    char* cvnr = StrOp.fmt( "%d", wCVByte.getnr( cv ) );
    char* cvval = StrOp.fmt( "%d", wCVByte.getvalue( cv ) );
    const char* cvdesc = wCVByte.getdesc(cv);
    m_CVNodes[wCVByte.getnr( cv )] = cv;
    m_CVList->AppendRows();
    int row = m_CVList->GetNumberRows()-1;
    m_CVList->SetCellValue(row, 0, wxString(cvnr,wxConvUTF8) );
    m_CVList->SetCellValue(row, 1, wxString(cvval,wxConvUTF8) );
    if( cvdesc != NULL && StrOp.len(cvdesc) > 0 )
      m_CVList->SetCellValue(row, 2, wxString(cvdesc,wxConvUTF8) );
    else
      m_CVList->SetCellValue(row, 2, wxString(m_CVDesc[wCVByte.getnr( cv )&0xff],wxConvUTF8) );
    m_CVList->SetReadOnly( row, 0, true );
    m_CVList->SetReadOnly( row, 1, true );
    m_CVList->SetReadOnly( row, 2, true );
    StrOp.free( cvnr );
    StrOp.free( cvval );
  }
  /* clean up the temp. list */
  ListOp.base.del(list);
  m_CVList->AutoSizeColumns();

}

void AccDecDlg::initCVDesc() {
  for( int i = 0; i < 256; i++ )
    m_CVDesc[i] = "";

  m_CVDesc[  1]  = "Primary Address";
}


bool AccDecDlg::evaluate() {
  if( m_Props == NULL )
    return false;

  TraceOp.trc( "accdecdlg", TRCLEVEL_INFO, __LINE__, 9999, "evaluate %s", wDec.getid( m_Props ) );

  if( m_ID->GetValue().Len() == 0 ) {
    wxMessageDialog( this, wxGetApp().getMsg("invalidid"), _T("Rocrail"), wxOK | wxICON_ERROR ).ShowModal();
    m_ID->SetValue( wxString(wDec.getid( m_Props ),wxConvUTF8) );
    return false;
  }
  // evaluate General
  wItem.setprev_id( m_Props, wItem.getid(m_Props) );
  wDec.setiid( m_Props, m_IID->GetValue().mb_str(wxConvUTF8) );
  wDec.setid( m_Props, m_ID->GetValue().mb_str(wxConvUTF8) );
  wDec.setbus( m_Props, atoi(m_Bus->GetValue().mb_str(wxConvUTF8)) );
  wDec.setaddr( m_Props, m_Addr->GetValue() );
  wDec.setdesc( m_Props, m_Desc->GetValue().mb_str(wxConvUTF8) );

  if( m_Protocol->GetSelection() == 1 )
    wDec.setprot( m_Props, wSwitch.prot_M );
  else if( m_Protocol->GetSelection() == 2 )
    wDec.setprot( m_Props, wSwitch.prot_N );
  else
    wDec.setprot( m_Props, wSwitch.prot_DEF );

  wDec.setprotver( m_Props, m_Version->GetValue() );
  wDec.setmanu( m_Props, m_Manu->GetValue().mb_str(wxConvUTF8) );
  wDec.setcatnr( m_Props, m_CatNr->GetValue().mb_str(wxConvUTF8) );
  wDec.setimage( m_Props, m_ImageFile->GetValue().mb_str(wxConvUTF8) );

  return true;
}

void AccDecDlg::onApply( wxCommandEvent& event )
{
  TraceOp.trc( "accdecdlg", TRCLEVEL_INFO, __LINE__, 9999, "Apply" );
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


void AccDecDlg::onSelected( wxListEvent& event ) {
  int index = event.GetIndex();
  m_Props = (iONode)m_IndexList->GetItemData(index);
  if( m_Props != NULL )
    initValues();
  else
    TraceOp.trc( "accdecdlg", TRCLEVEL_INFO, __LINE__, 9999, "no selection..." );
}


void AccDecDlg::onImage( wxCommandEvent& event ){
  const char* imagepath = wGui.getimagepath( wxGetApp().getIni() );
  TraceOp.trc( "cardlg", TRCLEVEL_INFO, __LINE__, 9999, "imagepath = [%s]", imagepath );
  wxFileDialog* fdlg = new wxFileDialog(this, _T("Search decoder image"),
      wxString(imagepath,wxConvUTF8), _T(""),
      _T("PNG files (*.png)|*.png|GIF files (*.gif)|*.gif|XPM files (*.xpm)|*.xpm"), wxFD_OPEN);
  if( fdlg->ShowModal() == wxID_OK ) {
    TraceOp.trc( "accdecdlg", TRCLEVEL_INFO, __LINE__, 9999, "Loading %s...", (const char*)fdlg->GetPath().mb_str(wxConvUTF8) );
    wxBitmapType bmptype = wxBITMAP_TYPE_XPM;
    if( StrOp.endsWithi( fdlg->GetPath().mb_str(wxConvUTF8), ".gif" ) )
      bmptype = wxBITMAP_TYPE_GIF;
    else if( StrOp.endsWithi( fdlg->GetPath().mb_str(wxConvUTF8), ".png" ) )
      bmptype = wxBITMAP_TYPE_PNG;
    m_Image->SetBitmapLabel( wxBitmap( fdlg->GetPath(), bmptype ) );
    m_Image->Refresh();
    wDec.setimage( m_Props, FileOp.ripPath(fdlg->GetPath().mb_str(wxConvUTF8)) );
    m_ImageFile->SetValue( wxString(wDec.getimage( m_Props ),wxConvUTF8) );
  }
}


void AccDecDlg::onDoc( wxCommandEvent& event )
{
  doDoc( event, "decoders");
}


void AccDecDlg::onColClick( wxListEvent& event ) {
  sortOnColumn(event.GetColumn());
}

void AccDecDlg::onCVCell( wxGridEvent& event ) {
  m_iSelectedCV = event.GetRow();
  if( m_iSelectedCV == -1 )
    return;

  wxString str = m_CVList->GetCellValue( m_iSelectedCV, 2 );
  m_CVDescription->SetValue(str);
  m_CVDescription->Enable(true);
  m_CVDescModify->Enable(true);
  event.Skip();

}

void AccDecDlg::onCVModify( wxCommandEvent& event ) {
  if( m_iSelectedCV == -1 )
    return;

  wxString str = m_CVList->GetCellValue(m_iSelectedCV, 0 );
  long cvnr = 0;
  str.ToLong(&cvnr);
  if( cvnr < 256 ) {
    iONode cv = m_CVNodes[cvnr];
    wCVByte.setdesc( cv, m_CVDescription->GetValue().mb_str(wxConvUTF8) );
    m_CVList->SetCellValue(m_iSelectedCV, 2, m_CVDescription->GetValue() );
  }

}

void AccDecDlg::onHelp( wxCommandEvent& event ) {
  switch( m_Notebook->GetSelection() ) {
  case 0: wxGetApp().openLink( "accdec" ); break;
  case 1: wxGetApp().openLink( "accdec-gen" ); break;
  case 2: wxGetApp().openLink( "accdec-cv" ); break;
  default: wxGetApp().openLink( "accdec" ); break;
  }
}


