/*
 Copyright (C) 2002-2012 Rob Versluis, Rocrail.net

 Without an official permission commercial use is not permitted.
 Forking this project is not permitted.
 */
#ifndef BASEDLG_CPP_
#define BASEDLG_CPP_

#include "rocs/public/node.h"


class BaseDialog
{   
public:
  void doDoc( wxCommandEvent& event, const char* xslName );
  bool existID( wxWindow* dlg, iONode list, iONode props, wxString id );
};


#endif /*BASEDLG_CPP_*/
