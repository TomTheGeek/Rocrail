/*
 Copyright (C) 2002-2012 Rob Versluis, Rocrail.net

 Without an official permission commercial use is not permitted.
 Forking this project is not permitted.
 */
#include "basenotebook.h"

  basenotebook::basenotebook(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
  :wxNotebook(parent, id, pos, size, wxGetApp().getTabAlign(), name) {
  }

