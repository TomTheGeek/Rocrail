#ifndef BASEPANEL_H_
#define BASEPANEL_H_


#include "rocs/public/node.h"
#include "wx/colour.h"


class BasePanel : public wxScrolledWindow
{
public:
  BasePanel(wxWindow* parent, long style = wxHSCROLL | wxVSCROLL);

  virtual void modelEvent( iONode node ) = 0;
  virtual void reScale( double scale ) = 0;
  virtual void removeItemFromList( iONode item ) = 0;
  virtual void init(bool modview=false) = 0;
  virtual void clean() = 0;
  virtual const char* getZLevelTitle() = 0;
  virtual int getZ() = 0;
  virtual const char* getZID() = 0;
  virtual iONode getZLevel() = 0;
  virtual void refresh(bool eraseBackground = true) = 0;
  virtual bool SetBackgroundColor(int red, int green, int blue, bool savecolor) = 0;
  virtual void OnPanelProps(wxCommandEvent& event) = 0;
  virtual BasePanel* updateZLevel(iONode zlevel) = 0;

};

#endif /*BASEPANEL_H_*/
