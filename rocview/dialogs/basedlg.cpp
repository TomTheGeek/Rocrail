/*
 Copyright (C) 2002-2012 Rob Versluis, Rocrail.net

 Without an official permission commercial use is not permitted.
 Forking this project is not permitted.
 */
#include "wx/wx.h"


#include "basedlg.h"

#include "rocrail/wrapper/public/Plan.h"
#include "rocrail/wrapper/public/Item.h"
#include "rocrail/wrapper/public/Output.h"
#include "rocrail/wrapper/public/Switch.h"
#include "rocrail/wrapper/public/Signal.h"
#include "rocview/wrapper/public/Gui.h"

#include "rocview/public/guiapp.h"

#include "rocs/public/node.h"
#include "rocs/public/file.h"
#include "rocs/public/system.h"


static wxString __getAddrStr(iONode Item) {
  if( StrOp.equals( wOutput.name(), NodeOp.getName(Item) ) )
    return wxString::Format(_T("%d-%d"), wOutput.getaddr(Item), wOutput.getport(Item));
  else if( StrOp.equals( wSwitch.name(), NodeOp.getName(Item) ) )
    return wxString::Format(_T("%d-%d"), wSwitch.getaddr1(Item), wSwitch.getport1(Item));
  else if( StrOp.equals( wSignal.name(), NodeOp.getName(Item) ) )
    return wxString::Format(_T("%d-%d"), wSignal.getaddr(Item), wSignal.getport1(Item));
  else
    return wxString::Format(_T("%d"), wItem.getaddr(Item));
}

void BaseDialog::sortOnColumn( int col ) {
  TraceOp.trc( "basedlg", TRCLEVEL_INFO, __LINE__, 9999,"sort on column %d", col);
  if( col != -1 ) {
    m_SortCol = col;
    fillIndex(m_Items, true);
    if( m_SelectedID != NULL )
      setIDSelection(m_SelectedID);
  }
  if( m_SortCol != m_colID ) m_sortID = true;
  if( m_SortCol != m_colIID ) m_sortIID = true;
  if( m_SortCol != m_colDesc ) m_sortDesc = true;
  if( m_SortCol != m_colPos ) m_sortPos = true;
  if( m_SortCol != m_colShow ) m_sortShow = true;
  if( m_SortCol != m_colAddr ) m_sortAddr = true;
  if( m_SortCol != m_colOri ) m_sortOri = true;

}


void BaseDialog::initList( wxListCtrl* list, wxWindow* parent, bool showPos, bool showAddr ) {
  m_ItemList = list;
  m_Parent = parent;
  m_SortCol = 0;
  m_ShowPos = showPos;
  m_ShowAddr = showAddr;
  m_SelectedID = NULL;
  int col = 0;
  m_colID = col;
  m_sortID = false;
  m_sortIID = true;
  m_sortAddr = true;
  m_sortDesc = true;
  m_sortShow = true;
  m_sortPos = true;
  m_sortOri = true;


  list->InsertColumn(col, wxGetApp().getMsg( "id" ), wxLIST_FORMAT_LEFT );
  col++;
  m_colIID = -1;
  m_colAddr = -1;
  if( m_ShowAddr ) {
    m_colIID = col;
    list->InsertColumn(col, wxGetApp().getMsg( "IID" ), wxLIST_FORMAT_LEFT );
    col++;
    m_colAddr = col;
    list->InsertColumn(col, wxGetApp().getMsg( "address" ), wxLIST_FORMAT_RIGHT );
    col++;
  }
  m_colDesc = col;
  list->InsertColumn(col, wxGetApp().getMsg( "description" ), wxLIST_FORMAT_LEFT );
  col++;
  m_colShow = col;
  list->InsertColumn(col, wxGetApp().getMsg( "show" ), wxLIST_FORMAT_LEFT );
  col++;
  m_colPos = -1;
  m_colOri = -1;
  if( m_ShowPos ) {
    m_colPos = col;
    list->InsertColumn(col, wxGetApp().getMsg( "position" ), wxLIST_FORMAT_LEFT );
    col++;
    m_colOri = col;
    list->InsertColumn(col, wxGetApp().getMsg( "orientation" ), wxLIST_FORMAT_LEFT );
    col++;
  }
}

static bool order = false;
/* comparator for sorting by id: */
static int __sortID(obj* _a, obj* _b)
{
    iONode a = (iONode)*_a;
    iONode b = (iONode)*_b;
    const char* idA = wItem.getid( a );
    const char* idB = wItem.getid( b );
    return order?strcmp( idB, idA ):strcmp( idA, idB );
}
static int __sortIID(obj* _a, obj* _b)
{
    iONode a = (iONode)*_a;
    iONode b = (iONode)*_b;
    const char* idA = wItem.getiid( a );
    const char* idB = wItem.getiid( b );
    return order?strcmp( idB, idA ):strcmp( idA, idB );
}
static int __sortAddr(obj* _a, obj* _b)
{
    iONode a = (iONode)*_a;
    iONode b = (iONode)*_b;
    wxString sA = __getAddrStr(a);
    wxString sB = __getAddrStr(b);
    return order?sB.Cmp(sA):sA.Cmp(sB);
}
static int __sortPos(obj* _a, obj* _b)
{
    iONode a = (iONode)*_a;
    iONode b = (iONode)*_b;
    int xA = wItem.getx( a );
    int xB = wItem.getx( b );
    int yA = wItem.gety( a );
    int yB = wItem.gety( b );
    int zA = wItem.getz( a );
    int zB = wItem.getz( b );
    if( xA == xB && yA == yB && zA == zB )
      return 0;
    if( xA > xB )
      return order?-1:1;
    if( xA < xB )
      return order?1:-1;
    if( yA > yB )
      return order?-1:1;
    if( yA < yB )
      return order?1:-1;
    if( zA > zB )
      return order?-1:1;
    if( zA < zB )
      return order?1:-1;
}
static int __sortDesc(obj* _a, obj* _b)
{
    iONode a = (iONode)*_a;
    iONode b = (iONode)*_b;
    const char* idA = wItem.getdesc( a );
    const char* idB = wItem.getdesc( b );
    return order?strcmp( idB, idA ):strcmp( idA, idB );
}
static int __sortOri(obj* _a, obj* _b)
{
    iONode a = (iONode)*_a;
    iONode b = (iONode)*_b;
    const char* idA = (wItem.getori(a)!=NULL?wItem.getori(a):wItem.west);
    const char* idB = (wItem.getori(b)!=NULL?wItem.getori(b):wItem.west);
    return order?strcmp( idB, idA ):strcmp( idA, idB );
}
static int __sortShow(obj* _a, obj* _b)
{
    iONode a = (iONode)*_a;
    iONode b = (iONode)*_b;
    const char* idA = wItem.isshow( a )?"true":"false";
    const char* idB = wItem.isshow( b )?"true":"false";
    return order?strcmp( idB, idA ):strcmp( idA, idB );
}


void BaseDialog::fillIndex( iONode Items, bool sort) {
  m_Items = Items;
  m_ItemList->DeleteAllItems();
  iOList sortlist = ListOp.inst();
  int size = NodeOp.getChildCnt(Items);
  for( int index = 0; index < size; index++ ) {
    iONode Item = NodeOp.getChild( Items, index );
    if( wItem.getid(Item) != NULL )
      ListOp.add(sortlist, (obj)Item);
  }

  if( m_SortCol == m_colID ) {
    if(sort) m_sortID = !m_sortID;
    order = m_sortID;
    ListOp.sort(sortlist, &__sortID);
  }
  else {
    ListOp.sort(sortlist, &__sortID);
  }

  if( m_SortCol == m_colIID ) {
    if(sort) m_sortIID = !m_sortIID;
    order = m_sortIID;
    ListOp.sort(sortlist, &__sortIID);
  }
  else if( m_SortCol == m_colAddr ) {
    if(sort) m_sortAddr = !m_sortAddr;
    order = m_sortAddr;
    ListOp.sort(sortlist, &__sortAddr);
  }
  else if( m_SortCol == m_colDesc ) {
    if(sort) m_sortDesc = !m_sortDesc;
    order = m_sortDesc;
    ListOp.sort(sortlist, &__sortDesc);
  }
  else if( m_SortCol == m_colShow ) {
    if(sort) m_sortShow = !m_sortShow;
    order = m_sortShow;
    ListOp.sort(sortlist, &__sortShow);
  }
  else if( m_SortCol == m_colPos ) {
    if(sort) m_sortPos = !m_sortPos;
    order = m_sortPos;
    ListOp.sort(sortlist, &__sortPos);
  }
  else if( m_SortCol == m_colOri ) {
    if(sort) m_sortOri = !m_sortOri;
    order = m_sortOri;
    ListOp.sort(sortlist, &__sortOri);
  }

  size = ListOp.size( sortlist );
  for( int i = 0; i < size; i++ ) {
    iONode Item = (iONode)ListOp.get( sortlist, i );
    appendItem(Item);
  }
  /* clean up the temp. list */
  ListOp.base.del(sortlist);
}

void BaseDialog::appendItem( iONode Item) {
  int index = m_ItemList->GetItemCount();
  m_ItemList->InsertItem( index, wxString( wItem.getid(Item), wxConvUTF8));
  if( m_ShowAddr ) {
    m_ItemList->SetItem( index, m_colIID, wxString( wItem.getiid(Item), wxConvUTF8));
    m_ItemList->SetItem( index, m_colAddr, __getAddrStr(Item));
    m_ItemList->SetColumnWidth(m_colIID, wxLIST_AUTOSIZE_USEHEADER);
    m_ItemList->SetColumnWidth(m_colAddr, wxLIST_AUTOSIZE_USEHEADER);
  }
  m_ItemList->SetItem( index, m_colDesc, wxString( wItem.getdesc(Item), wxConvUTF8));
  m_ItemList->SetItem( index, m_colShow, wxString( wItem.isshow(Item)?"true":"false", wxConvUTF8));
  if( m_ShowPos ) {
    m_ItemList->SetItem( index, m_colPos, wxString::Format(_T("%d, %d, %d"), wItem.getx(Item), wItem.gety(Item), wItem.getz(Item)) );
    m_ItemList->SetItem( index, m_colOri, wxString( wItem.getori(Item)!=NULL?wItem.getori(Item):wItem.west, wxConvUTF8));
    m_ItemList->SetColumnWidth(m_colPos, wxLIST_AUTOSIZE_USEHEADER);
    m_ItemList->SetColumnWidth(m_colOri, wxLIST_AUTOSIZE_USEHEADER);
  }
  m_ItemList->SetItemPtrData(index, (wxUIntPtr)Item);
  m_ItemList->SetColumnWidth(m_colID, wxLIST_AUTOSIZE);
  m_ItemList->SetColumnWidth(m_colDesc, wxLIST_AUTOSIZE);
  m_ItemList->SetColumnWidth(m_colShow, wxLIST_AUTOSIZE_USEHEADER);
  if( m_ItemList->GetColumnWidth(m_colDesc) < 40 )
    m_ItemList->SetColumnWidth(m_colDesc, wxLIST_AUTOSIZE_USEHEADER);
}


void BaseDialog::setIDSelection( const char* ID ) {
  int size = m_ItemList->GetItemCount();
  for( int index = 0; index < size; index++ ) {
    iONode node = (iONode)m_ItemList->GetItemData(index);
    if( StrOp.equals( ID, wItem.getid(node) ) ) {
      m_ItemList->SetItemState(index, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
      m_SelectedID = wItem.getid(node);
      break;
    }
  }
}

iONode BaseDialog::setSelection( int index ) {
  int size = m_ItemList->GetItemCount();
  if( index < size ) {
    m_ItemList->SetItemState(index, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
    iONode node = (iONode)m_ItemList->GetItemData(index);
    m_SelectedID = wItem.getid(node);
    return (iONode)m_ItemList->GetItemData(index);
  }
  return NULL;
}

iONode BaseDialog::selectNext() {
  int size = m_ItemList->GetItemCount();
  for( int index = 0; index < size; index++ ) {
    if( wxLIST_STATE_SELECTED == m_ItemList->GetItemState(index, wxLIST_STATE_SELECTED) ) {
      if( index + 1 < size ) {
        m_ItemList->SetItemState(index+1, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
        iONode node = (iONode)m_ItemList->GetItemData(index+1);
        m_SelectedID = wItem.getid(node);
        return node;
      }
      else {
        iONode node = (iONode)m_ItemList->GetItemData(index);
        m_SelectedID = wItem.getid(node);
        return node;
      }
      break;
    }
  }

}

iONode BaseDialog::selectPrev() {
  int size = m_ItemList->GetItemCount();
  for( int index = 0; index < size; index++ ) {
    if( wxLIST_STATE_SELECTED == m_ItemList->GetItemState(index, wxLIST_STATE_SELECTED) ) {
      if( index - 1 >= 0 ) {
        m_ItemList->SetItemState(index-1, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
        iONode node = (iONode)m_ItemList->GetItemData(index-1);
        m_SelectedID = wItem.getid(node);
        return node;
      }
      else {
        iONode node = (iONode)m_ItemList->GetItemData(index);
        m_SelectedID = wItem.getid(node);
        return node;
      }
      break;
    }
  }

}

iONode BaseDialog::getSelection(int index) {
  if( index != -1 ) {
    iONode node = (iONode)m_ItemList->GetItemData(index);
    m_SelectedID = wItem.getid(node);
    return node;
  }

  int size = m_ItemList->GetItemCount();
  for( int index = 0; index < size; index++ ) {
    if( wxLIST_STATE_SELECTED == m_ItemList->GetItemState(index, wxLIST_STATE_SELECTED) ) {
      iONode node = (iONode)m_ItemList->GetItemData(index);
      m_SelectedID = wItem.getid(node);
      return node;
    }
    break;
  }
}

int BaseDialog::findID( const char* ID ) {
  int size = m_ItemList->GetItemCount();
  for( int index = 0; index < size; index++ ) {
    iONode node = (iONode)m_ItemList->GetItemData(index);
    if( StrOp.equals( ID, wItem.getid(node) ) ) {
      return index;
    }
  }
  return wxNOT_FOUND;
}




bool BaseDialog::existID( wxWindow* dlg, iONode list, iONode props, wxString  id ) {
  if( StrOp.equals( wItem.getid(props), id.mb_str(wxConvUTF8) ) ) {
    return false;
  }

  if( id.Len() == 0 ) {
    wxMessageDialog( dlg, wxGetApp().getMsg("invalidid"), _T("Rocrail"), wxOK | wxICON_ERROR ).ShowModal();
    return true;
  }
  if( list != NULL ) {
    int cnt = NodeOp.getChildCnt( list );
    for( int i = 0; i < cnt; i++ ) {
      iONode child = NodeOp.getChild( list, i );
      if( StrOp.equals( wItem.getid(child), id.mb_str(wxConvUTF8) )) {
        wxMessageDialog( dlg,
            wxString::Format(wxGetApp().getMsg("existingid"), wItem.getx(child), wItem.gety(child)) + _T("\n") + wxString(wItem.getdesc(child),wxConvUTF8),
            _T("Rocrail"), wxOK | wxICON_ERROR ).ShowModal();
        return true;
      }
    }
  }
  return false;
}


void BaseDialog::doDoc( wxCommandEvent& event, const char* xslName ) {
  
  // get the model:
  iONode model = wxGetApp().getModel();
  // serialize the model into a string:
  NodeOp.setStr(model, "guiimagepath", wGui.getimagepath(wxGetApp().getIni()));
  char* xml = model->base.toString( model );
  
  // create a path and filename:
  char* fileName = StrOp.fmt( "%s%c%s-doc.xml", FileOp.pwd(), SystemOp.getFileSeparator(), xslName );
  
  // open the file for writing:
  iOFile planFile = FileOp.inst( fileName, False );
  if( planFile != NULL ) {
    char* styleSheet = NULL;
    
    // Check if a XSL exists for the GUI language:
    const char* lang = wGui.getlang( wxGetApp().getIni() );
    
    char* xslPath = NULL;
    if( FileOp.isAbsolute( wGui.getxslpath( wxGetApp().getIni() ) ) ) {
      xslPath = StrOp.dup( wGui.getxslpath( wxGetApp().getIni() ) );
    }
    else {
      xslPath = StrOp.fmt( "%s%c%s",FileOp.pwd(), SystemOp.getFileSeparator(), wGui.getxslpath( wxGetApp().getIni() ));
    }
    
    char* xslFileName = StrOp.fmt( "%s%c%s_%s.xsl", xslPath, SystemOp.getFileSeparator(), xslName, lang );
    if( !FileOp.exist(xslFileName) )
      lang = "en"; // use the default
    StrOp.free(xslFileName);

    // create the stylesheet line:
    styleSheet = StrOp.fmt("<?xml-stylesheet type=\"text/xsl\" href=\"file:///%s%c%s_%s.xsl\"?>\n", 
        xslPath, SystemOp.getFileSeparator(), xslName, lang );
    StrOp.free(xslPath);

    // write the stylsheet line at the beginning of the file:
    FileOp.write( planFile, styleSheet, StrOp.len( styleSheet ) );
    // write the rest:
    FileOp.write( planFile, xml, StrOp.len( xml ) );
    FileOp.close( planFile );
    
    TraceOp.trc( "fbdlg", TRCLEVEL_INFO, __LINE__, 9999, "%ld bytes saved in %s.", 
        FileOp.getWritten( planFile ), fileName );

    /* Cleanup. */
    planFile->base.del(planFile);
    StrOp.free(styleSheet);
  }
  /* Cleanup. */
  StrOp.free(xml);

  // call the browser:
  wxLaunchDefaultBrowser(wxString("file:///",wxConvUTF8 )+wxString(fileName,wxConvUTF8 ), wxBROWSER_NEW_WINDOW );
  
  /* Cleanup. */
  StrOp.free(fileName);
  
}
