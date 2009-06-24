#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/defs.h"
#endif

#include "rocgui/public/guiapp.h"

#include <wx/notebook.h>


class basenotebook : public wxNotebook
{
public:
  // It is important that the custom class has an identical constructor
  basenotebook(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = wxNotebookNameStr);

};

