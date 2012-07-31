/*
 Copyright (C) 2002-2012 Rob Versluis, Rocrail.net

 Without an official permission commercial use is not permitted.
 Forking this project is not permitted.
 */
#include "wx/wx.h"


#include "basedlg.h"

#include "rocrail/wrapper/public/Plan.h"
#include "rocrail/wrapper/public/Item.h"
#include "rocview/wrapper/public/Gui.h"

#include "rocview/public/guiapp.h"

#include "rocs/public/node.h"
#include "rocs/public/file.h"
#include "rocs/public/system.h"


void BaseDialog::sortOnColumn( int col ) {
  TraceOp.trc( "basedlg", TRCLEVEL_INFO, __LINE__, 9999,"sort on column %d", col);
  if( col != -1 ) {
    m_SortCol = col;
    fillIndex(m_Items);
    if( m_SelectedID != NULL )
      setIDSelection(m_SelectedID);
  }
}


void BaseDialog::initList( wxListCtrl* list, wxWindow* parent, bool showPos ) {
  m_ItemList = list;
  m_Parent = parent;
  m_SortCol = 0;
  m_ShowPos = showPos;
  m_SelectedID = NULL;
  list->InsertColumn(0, wxGetApp().getMsg( "id" ), wxLIST_FORMAT_LEFT );
  list->InsertColumn(1, wxGetApp().getMsg( "IID" ), wxLIST_FORMAT_LEFT );
  list->InsertColumn(2, wxGetApp().getMsg( "address" ), wxLIST_FORMAT_RIGHT );
  list->InsertColumn(3, wxGetApp().getMsg( "description" ), wxLIST_FORMAT_LEFT );
  list->InsertColumn(4, wxGetApp().getMsg( "show" ), wxLIST_FORMAT_LEFT );
  if( m_ShowPos ) {
    list->InsertColumn(5, wxGetApp().getMsg( "position" ), wxLIST_FORMAT_LEFT );
    list->InsertColumn(6, wxGetApp().getMsg( "orientation" ), wxLIST_FORMAT_LEFT );
  }
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
static int __sortIID(obj* _a, obj* _b)
{
    iONode a = (iONode)*_a;
    iONode b = (iONode)*_b;
    const char* idA = wItem.getiid( a );
    const char* idB = wItem.getiid( b );
    return strcmp( idA, idB );
}
static int __sortAddr(obj* _a, obj* _b)
{
    iONode a = (iONode)*_a;
    iONode b = (iONode)*_b;
    int idA = wItem.getaddr( a );
    int idB = wItem.getaddr( b );
    if( idA == idB )
      return 0;
    if( idA > idB )
      return 1;
    if( idA < idB )
      return -1;
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
      return 1;
    if( xA < xB )
      return -1;
    if( yA > yB )
      return 1;
    if( yA < yB )
      return -1;
    if( zA > zB )
      return 1;
    if( zA < zB )
      return -1;
}
static int __sortDesc(obj* _a, obj* _b)
{
    iONode a = (iONode)*_a;
    iONode b = (iONode)*_b;
    const char* idA = wItem.getdesc( a );
    const char* idB = wItem.getdesc( b );
    return strcmp( idA, idB );
}
static int __sortOri(obj* _a, obj* _b)
{
    iONode a = (iONode)*_a;
    iONode b = (iONode)*_b;
    const char* idA = (wItem.getori(a)!=NULL?wItem.getori(a):wItem.west);
    const char* idB = (wItem.getori(b)!=NULL?wItem.getori(b):wItem.west);
    return strcmp( idA, idB );
}
static int __sortShow(obj* _a, obj* _b)
{
    iONode a = (iONode)*_a;
    iONode b = (iONode)*_b;
    const char* idA = wItem.isshow( a )?"true":"false";
    const char* idB = wItem.isshow( b )?"true":"false";
    return strcmp( idA, idB );
}


void BaseDialog::fillIndex( iONode Items) {
  m_Items = Items;
  m_ItemList->DeleteAllItems();
  iOList sortlist = ListOp.inst();
  int size = NodeOp.getChildCnt(Items);
  for( int index = 0; index < size; index++ ) {
    iONode Item = NodeOp.getChild( Items, index );
    if( wItem.getid(Item) != NULL )
      ListOp.add(sortlist, (obj)Item);
  }

  ListOp.sort(sortlist, &__sortID);

  if( m_SortCol == 1 )
    ListOp.sort(sortlist, &__sortIID);
  else if( m_SortCol == 2 )
    ListOp.sort(sortlist, &__sortAddr);
  else if( m_SortCol == 3 )
    ListOp.sort(sortlist, &__sortDesc);
  else if( m_SortCol == 4 )
    ListOp.sort(sortlist, &__sortShow);
  else if( m_SortCol == 5 )
    ListOp.sort(sortlist, &__sortPos);
  else if( m_SortCol == 6 )
    ListOp.sort(sortlist, &__sortOri);

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
  m_ItemList->InsertItem( index, wxString::Format(_T("%s"), wItem.getid(Item)));
  m_ItemList->SetItem( index, 1, wxString::Format(_T("%s"), wItem.getiid(Item)));
  m_ItemList->SetItem( index, 2, wxString::Format(_T("%d"), wItem.getaddr(Item)));
  m_ItemList->SetItem( index, 3, wxString::Format(_T("%s"), wItem.getdesc(Item)));
  m_ItemList->SetItem( index, 4, wxString::Format(_T("%s"), wItem.isshow(Item)?"true":"false"));
  if( m_ShowPos ) {
    m_ItemList->SetItem( index, 5, wxString::Format(_T("%d, %d, %d"), wItem.getx(Item), wItem.gety(Item), wItem.getz(Item)) );
    m_ItemList->SetItem( index, 6, wxString::Format(_T("%s"), wItem.getori(Item)!=NULL?wItem.getori(Item):wItem.west));
    m_ItemList->SetColumnWidth(5, wxLIST_AUTOSIZE_USEHEADER);
    m_ItemList->SetColumnWidth(6, wxLIST_AUTOSIZE_USEHEADER);
  }
  m_ItemList->SetItemPtrData(index, (wxUIntPtr)Item);
  m_ItemList->SetColumnWidth(0, wxLIST_AUTOSIZE);
  m_ItemList->SetColumnWidth(1, wxLIST_AUTOSIZE_USEHEADER);
  m_ItemList->SetColumnWidth(2, wxLIST_AUTOSIZE_USEHEADER);
  m_ItemList->SetColumnWidth(3, wxLIST_AUTOSIZE);
  m_ItemList->SetColumnWidth(4, wxLIST_AUTOSIZE_USEHEADER);
  if( m_ItemList->GetColumnWidth(3) < 40 )
    m_ItemList->SetColumnWidth(3, wxLIST_AUTOSIZE_USEHEADER);
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
