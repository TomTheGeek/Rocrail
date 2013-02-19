/*
 Copyright (C) 2002-2012 Rob Versluis, Rocrail.net

 Without an official permission commercial use is not permitted.
 Forking this project is not permitted.
 */
#ifndef BASEDLG_CPP_
#define BASEDLG_CPP_

#include <wx/listctrl.h>

#include "rocs/public/node.h"


class BaseDialog
{   
public:
  BaseDialog();
  void doDoc( wxCommandEvent& event, const char* xslName );
  bool existID( wxWindow* dlg, iONode list, iONode props, wxString id );
  void initList(wxListCtrl* list, wxWindow* parent, bool showPos=true, bool showAddr=true, bool showLen=false,
      bool showShow=true, bool showType=false, bool showTime=false);
  void fillIndex( iONode Items, bool sort=false);
  void setIDSelection( const char* ID );
  iONode setSelection( int index );
  iONode getSelection(int index=-1);
  iONode selectNext();
  iONode selectPrev();
  int findID( const char* ID );
  void appendItem( iONode Item);
  void sortOnColumn( int col );

  wxWindow* m_Parent;
  wxListCtrl* m_ItemList;
  iONode m_Items;
  int m_SortCol;
  bool m_ShowPos;
  bool m_ShowAddr;
  bool m_ShowLen;
  bool m_ShowType;
  bool m_ShowShow;
  bool m_ShowTime;
  const char* m_SelectedID;
  int m_colID;
  int m_colIID;
  int m_colAddr;
  int m_colDesc;
  int m_colShow;
  int m_colPos;
  int m_colOri;
  int m_colLen;
  int m_colType;
  int m_colRTime;
  int m_colMTime;
  bool m_sortID;
  bool m_sortIID;
  bool m_sortAddr;
  bool m_sortDesc;
  bool m_sortShow;
  bool m_sortPos;
  bool m_sortOri;
  bool m_sortLen;
  bool m_sortType;
  bool m_sortRTime;
  bool m_sortMTime;
  bool m_longaddr;
};


#endif /*BASEDLG_CPP_*/
