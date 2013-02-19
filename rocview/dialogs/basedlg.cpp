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
#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/Block.h"
#include "rocview/wrapper/public/Gui.h"

#include "rocview/public/guiapp.h"

#include "rocs/public/node.h"
#include "rocs/public/file.h"
#include "rocs/public/system.h"


static wxString __getAddrStr(iONode Item, bool* longaddr) {
  if( StrOp.equals( wOutput.name(), NodeOp.getName(Item) ) )
    return wxString::Format(_T("%d-%d"), wOutput.getaddr(Item), wOutput.getport(Item));
  else if( StrOp.equals( wSwitch.name(), NodeOp.getName(Item) ) ) {
    if( wSwitch.getaddr2(Item) > 0 || wSwitch.getport2(Item) > 0 ) {
      *longaddr = true;
      return wxString::Format(_T("%d-%d/%d-%d"), wSwitch.getaddr1(Item), wSwitch.getport1(Item), wSwitch.getaddr2(Item), wSwitch.getport2(Item));
    }
    else
      return wxString::Format(_T("%d-%d"), wSwitch.getaddr1(Item), wSwitch.getport1(Item));
  }
  else if( StrOp.equals( wSignal.name(), NodeOp.getName(Item) ) )
    return wxString::Format(_T("%d-%d"), wSignal.getaddr(Item), wSignal.getport1(Item));
  else if( StrOp.equals( wBlock.name(), NodeOp.getName(Item) ) )
    return wxString::Format(_T("%d-%d"), wBlock.getaddr(Item), wBlock.getport(Item));
  else
    return wxString::Format(_T("%d"), wItem.getaddr(Item));
}


BaseDialog::BaseDialog() {
  m_ItemList = NULL;
  m_Parent = NULL;
  m_SortCol = 0;
  m_SelectedID = NULL;
}


void BaseDialog::sortOnColumn( int col ) {
  if(m_ItemList == NULL)
    return;

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
  if( m_SortCol != m_colLen ) m_sortLen = true;
  if( m_SortCol != m_colType ) m_sortType = true;
  if( m_SortCol != m_colRTime ) m_sortRTime = true;
  if( m_SortCol != m_colMTime ) m_sortMTime = true;

}


void BaseDialog::initList( wxListCtrl* list, wxWindow* parent, bool showPos, bool showAddr, bool showLen,
    bool showShow, bool showType, bool showTime )
{
  m_ItemList = list;
  m_Parent = parent;
  m_SortCol = 0;
  m_ShowPos = showPos;
  m_ShowAddr = showAddr;
  m_ShowLen = showLen;
  m_ShowType = showType;
  m_ShowShow = showShow;
  m_ShowTime = showTime;
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
  m_sortLen = true;
  m_sortType = true;
  m_sortRTime = true;
  m_sortMTime = true;
  m_longaddr = false;


  list->InsertColumn(col, wxGetApp().getMsg( "id" ), wxLIST_FORMAT_LEFT );
  col++;
  m_colIID  = -1;
  m_colAddr = -1;
  m_colLen  = -1;
  m_colType = -1;
  m_colLen  = -1;
  m_colShow = -1;
  m_colPos  = -1;
  m_colRTime = -1;
  m_colMTime = -1;

  if( m_ShowAddr ) {
    m_colIID = col;
    list->InsertColumn(col, wxGetApp().getMsg( "iid" ), wxLIST_FORMAT_LEFT );
    col++;
    m_colAddr = col;
    list->InsertColumn(col, wxGetApp().getMsg( "address" ), wxLIST_FORMAT_RIGHT );
    col++;
  }
  m_colDesc = col;
  list->InsertColumn(col, wxGetApp().getMsg( "description" ), wxLIST_FORMAT_LEFT );
  col++;
  if( m_ShowLen ) {
    m_colLen = col;
    list->InsertColumn(col, wxGetApp().getMsg( "length" ), wxLIST_FORMAT_RIGHT );
    col++;
  }
  if( m_ShowShow ) {
    m_colShow = col;
    list->InsertColumn(col, wxGetApp().getMsg( "show" ), wxLIST_FORMAT_LEFT );
    col++;
  }
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
  if( m_ShowType ) {
    m_colType = col;
    list->InsertColumn(col, wxGetApp().getMsg( "type" ), wxLIST_FORMAT_LEFT );
    col++;
  }
  if( m_ShowTime ) {
    m_colRTime = col;
    list->InsertColumn(col, wxGetApp().getMsg( "runtime" ), wxLIST_FORMAT_LEFT );
    col++;
    m_colMTime = col;
    list->InsertColumn(col, wxGetApp().getMsg( "mtime" ), wxLIST_FORMAT_LEFT );
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
static int __sortAddr_byAlphabet(obj* _a, obj* _b)
{
  bool longaddr = false;
  iONode a = (iONode)*_a;
  iONode b = (iONode)*_b;
  wxString sA = __getAddrStr(a, &longaddr);
  wxString sB = __getAddrStr(b, &longaddr);
  return order?sB.Cmp(sA):sA.Cmp(sB);
}
static int __sortAddr(obj* _a, obj* _b)
{   /* sort numerical, first by address, then by port (where applicable) */
    iONode a = (iONode)*_a;
    iONode b = (iONode)*_b;
    
    int addrA = 0;
    int portA = 0;
    int addrB = 0;
    int portB = 0;

    /* node a */
    if( StrOp.equals( wOutput.name(), NodeOp.getName(a) ) ) {
      addrA = wOutput.getaddr(a);
      portA = wOutput.getport(a);
    }
    else if( StrOp.equals( wSwitch.name(), NodeOp.getName(a) ) ) {
      addrA = wSwitch.getaddr1(a);
      portA = wSwitch.getport1(a);
    }
    else if(  StrOp.equals( wSignal.name(), NodeOp.getName(a) ) ) {
      addrA = wSignal.getaddr(a);
      portA = wSignal.getport1(a);
    }
    else {
      addrA = wItem.getaddr(a);
    }

    /* node b */
    if( StrOp.equals( wOutput.name(), NodeOp.getName(b) ) ) {
      addrB = wOutput.getaddr(b);
      portB = wOutput.getport(b);
    }
    else if( StrOp.equals( wSwitch.name(), NodeOp.getName(b) ) ) {
      addrB = wSwitch.getaddr1(b);
      portB = wSwitch.getport1(b);
    }
    else if(  StrOp.equals( wSignal.name(), NodeOp.getName(b) ) ) {
      addrB = wSignal.getaddr(b);
      portB = wSignal.getport1(b);
    }
    else {
      addrB = wItem.getaddr(b);
    }

    /* check/compare */
    if( addrA == addrB && portA == portB )
      return 0;
    if( addrA > addrB )
      return order?-1:1;
    if( addrA < addrB )
      return order?1:-1;
    if( portA > portB )
      return order?-1:1;
    if( portA < portB )
      return order?1:-1;

    /* this should never be reached because it is addrA == addrB && portA == portB */
    TraceOp.trc( "__sortAddr", TRCLEVEL_WARNING, __LINE__, 9999, "WARNING unreachable code reached" );
    return 0;
}
static int __sortLen(obj* _a, obj* _b)
{
    iONode a = (iONode)*_a;
    iONode b = (iONode)*_b;
    int lenA = wItem.getlen( a );
    int lenB = wItem.getlen( b );
    if( lenA > lenB )
      return order?-1:1;
    if( lenA < lenB )
      return order?1:-1;
    return 0;
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
    return 0;
}
static int __sortDesc(obj* _a, obj* _b)
{
    iONode a = (iONode)*_a;
    iONode b = (iONode)*_b;
    const char* idA = wItem.getdesc( a );
    const char* idB = wItem.getdesc( b );
    return order?strcmp( idB, idA ):strcmp( idA, idB );
}
static int __sortType(obj* _a, obj* _b)
{
    iONode a = (iONode)*_a;
    iONode b = (iONode)*_b;
    const char* idA = wItem.gettype( a );
    const char* idB = wItem.gettype( b );
    if( idA != NULL && idB != NULL )
      return order?strcmp( idB, idA ):strcmp( idA, idB );
    else
      return 0;
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
static int __sortRTime(obj* _a, obj* _b)
{
    iONode a = (iONode)*_a;
    iONode b = (iONode)*_b;
    long timeA = wLoc.getruntime( a );
    long timeB = wLoc.getruntime( b );
    if( timeA > timeB )
      return order?-1:1;
    if( timeA < timeB )
      return order?1:-1;
    return 0;
}
static int __sortMTime(obj* _a, obj* _b)
{
    iONode a = (iONode)*_a;
    iONode b = (iONode)*_b;
    long timeA = wLoc.getmtime( a );
    long timeB = wLoc.getmtime( b );
    if( timeA > timeB )
      return order?-1:1;
    if( timeA < timeB )
      return order?1:-1;
    return 0;
}


void BaseDialog::fillIndex( iONode Items, bool sort) {
  if(m_ItemList == NULL)
    return;

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
  else if( m_SortCol == m_colLen ) {
    if(sort) m_sortLen = !m_sortLen;
    order = m_sortLen;
    ListOp.sort(sortlist, &__sortLen);
  }
  else if( m_SortCol == m_colType ) {
    if(sort) m_sortType = !m_sortType;
    order = m_sortType;
    ListOp.sort(sortlist, &__sortType);
  }
  else if( m_SortCol == m_colRTime ) {
    if(sort) m_sortRTime = !m_sortRTime;
    order = m_sortRTime;
    ListOp.sort(sortlist, &__sortRTime);
  }
  else if( m_SortCol == m_colMTime ) {
    if(sort) m_sortMTime = !m_sortMTime;
    order = m_sortMTime;
    ListOp.sort(sortlist, &__sortMTime);
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
  if(m_ItemList == NULL)
    return;

  int index = m_ItemList->GetItemCount();
  m_ItemList->InsertItem( index, wxString( wItem.getid(Item), wxConvUTF8));
  if( m_ShowAddr ) {
    m_ItemList->SetItem( index, m_colIID, wxString( wItem.getiid(Item), wxConvUTF8));
    m_ItemList->SetItem( index, m_colAddr, __getAddrStr(Item, &m_longaddr));
    m_ItemList->SetColumnWidth(m_colIID, wxLIST_AUTOSIZE_USEHEADER);
    m_ItemList->SetColumnWidth(m_colAddr, m_longaddr ? wxLIST_AUTOSIZE:wxLIST_AUTOSIZE_USEHEADER);
  }
  m_ItemList->SetItem( index, m_colDesc, wxString( wItem.getdesc(Item), wxConvUTF8));
  if( m_ShowLen ) {
    m_ItemList->SetItem( index, m_colLen, wxString::Format(_T("%d"), wItem.getlen(Item)));
    m_ItemList->SetColumnWidth(m_colLen, wxLIST_AUTOSIZE_USEHEADER);
  }
  if( m_ShowShow ) {
    m_ItemList->SetItem( index, m_colShow, wxString( wItem.isshow(Item)?"true":"false", wxConvUTF8));
  }
  if( m_ShowPos ) {
    m_ItemList->SetItem( index, m_colPos, wxString::Format(_T("%d, %d, %d"), wItem.getx(Item), wItem.gety(Item), wItem.getz(Item)) );
    m_ItemList->SetItem( index, m_colOri, wxString( wItem.getori(Item)!=NULL?wItem.getori(Item):wItem.west, wxConvUTF8));
    m_ItemList->SetColumnWidth(m_colPos, wxLIST_AUTOSIZE_USEHEADER);
    m_ItemList->SetColumnWidth(m_colOri, wxLIST_AUTOSIZE_USEHEADER);
  }
  m_ItemList->SetItemPtrData(index, (wxUIntPtr)Item);
  m_ItemList->SetColumnWidth(m_colID, wxLIST_AUTOSIZE);
  m_ItemList->SetColumnWidth(m_colDesc, wxLIST_AUTOSIZE);
  if( m_ShowShow )
    m_ItemList->SetColumnWidth(m_colShow, wxLIST_AUTOSIZE_USEHEADER);
  if( m_ItemList->GetColumnWidth(m_colDesc) < 60 )
    m_ItemList->SetColumnWidth(m_colDesc, wxLIST_AUTOSIZE_USEHEADER);
  if( m_ShowType ) {
    m_ItemList->SetItem( index, m_colType, wxString( wItem.gettype(Item), wxConvUTF8));
    m_ItemList->SetColumnWidth(m_colType, wxLIST_AUTOSIZE);
  }
  if( m_ShowTime ) {
    long runtime = wLoc.getruntime(Item);
    long mtime   = wLoc.getmtime(Item);
    m_ItemList->SetItem( index, m_colRTime, wxString::Format(_T("%d:%02d.%02d"), (int)(runtime/3600), (int)((runtime%3600)/60), (int)((runtime%3600)%60)));
    m_ItemList->SetColumnWidth(m_colRTime, wxLIST_AUTOSIZE);
    m_ItemList->SetItem( index, m_colMTime, wxString::Format(_T("%d:%02d.%02d"), (int)(mtime/3600), (int)((mtime%3600)/60), (int)((mtime%3600)%60)));
    m_ItemList->SetColumnWidth(m_colMTime, wxLIST_AUTOSIZE);
  }
}


void BaseDialog::setIDSelection( const char* ID ) {
  if(m_ItemList == NULL)
    return;

  int size = m_ItemList->GetItemCount();
  for( int index = 0; index < size; index++ ) {
    iONode node = (iONode)m_ItemList->GetItemData(index);
    if( StrOp.equals( ID, wItem.getid(node) ) ) {
      m_ItemList->SetItemState(index, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
      m_ItemList->EnsureVisible(index);
      m_SelectedID = wItem.getid(node);
      break;
    }
  }
}

iONode BaseDialog::setSelection( int index ) {
  if(m_ItemList == NULL)
    return NULL;

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
  if(m_ItemList == NULL)
    return NULL;

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
  return NULL;
}

iONode BaseDialog::selectPrev() {
  if(m_ItemList == NULL)
    return NULL;

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
  return NULL;
}

iONode BaseDialog::getSelection(int index) {
  if(m_ItemList == NULL)
    return NULL;

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
  return NULL;
}

int BaseDialog::findID( const char* ID ) {
  if(m_ItemList == NULL)
    return wxNOT_FOUND;

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
