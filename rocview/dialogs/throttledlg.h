/* Rocrail - Model Railroad Software Copyright (C) 2002-2013 Rob Versluis, Rocrail.net Without an official permission commercial use is not permitted. Forking this project is not permitted. This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version. This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.*/#ifndef __throttledlg__#define __throttledlg__/**@fileSubclass of ThrottleDlgGen, which is generated by wxFormBuilder.*/#include "throttledlggen.h"//// end generated include#include "rocs/public/node.h"#include "rocs/public/list.h"#include "rocs/public/map.h"/** Implementing ThrottleDlgGen */class ThrottleDlg : public ThrottleDlgGen{  iONode m_Props;  int m_iFnGroup;  bool m_bDir;  bool m_bFn;  int m_iSpeed;  bool m_bFx[32];  iOList m_DlgList;  iOMap m_DlgMap;  iONode getConsistLoco();	public:  /** Constructor */  ThrottleDlg( wxWindow* parent, iOList list, iOMap map, const char* locid=NULL );	//// end generated class members  ~ThrottleDlg();  void onButton(wxCommandEvent& event);  void onSlider(wxScrollEvent& event);  void onClose( wxCloseEvent& event );  void onDestroy( wxWindowDestroyEvent& event );  void modelEvent( iONode evt );  void updateImage();  void setFLabels();  void setFLabels(iONode p_Props, bool merge, int fmap=0xFF);  wxBitmap* getIcon(const char* icon);  void funCmd(int fidx, bool on);  void speedCmd(bool sendCmd);  bool setFX( iONode node );};#endif // __throttledlg__