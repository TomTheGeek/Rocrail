/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2007 - Rob Versluis <r.j.versluis@rocrail.net>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "loccontroldlg.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

////@begin includes
////@end includes

#include "loccontroldlg.h"

#include "rocview/public/guiapp.h"
#include "rocview/public/base.h"
#include "rocs/public/str.h"
#include "rocs/public/map.h"
#include "rocs/public/mem.h"
#include "rocs/public/list.h"
#include "rocs/public/system.h"

#include "rocrail/wrapper/public/Plan.h"
#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/SysCmd.h"
#include "rocrail/wrapper/public/FunCmd.h"
#include "rocrail/wrapper/public/FunDef.h"

#include "rocview/wrapper/public/Gui.h"

#include "rocview/xpm/rocrail-forwards.xpm"
#include "rocview/xpm/rocrail-reverse.xpm"

#include "rocview/xpm/nopict.xpm"


////@begin XPM images
#include "../xpm/dir.xpm"
////@end XPM images

/*!
 * LocControlDialog type definition
 */

IMPLEMENT_DYNAMIC_CLASS( LocControlDialog, wxDialog )

/*!
 * LocControlDialog event table definition
 */

BEGIN_EVENT_TABLE( LocControlDialog, wxDialog )

////@begin LocControlDialog event table entries
    EVT_CLOSE( LocControlDialog::OnCloseWindow )
    EVT_WINDOW_DESTROY( LocControlDialog::OnDestroy )
    EVT_KEY_DOWN( LocControlDialog::OnKeyDown )
    EVT_KEY_UP( LocControlDialog::OnKeyUp )

    EVT_BUTTON( ID_BITMAPBUTTON_LOCCTRL_IMAGE, LocControlDialog::OnBitmapbuttonLocctrlImageClick )

    EVT_COMMAND_SCROLL( ID_SLIDER_LOCCTRL_SPEED, LocControlDialog::OnSliderLocctrlSpeedScroll )
    EVT_COMMAND_SCROLL_THUMBTRACK( ID_SLIDER_LOCCTRL_SPEED, LocControlDialog::OnSliderLocctrlSpeedScroll )

    EVT_BUTTON( ID_BUTTON_LOCCTRL_F1, LocControlDialog::OnButtonLocctrlF1Click )

    EVT_BUTTON( ID_BUTTON_LOCCTRL_F2, LocControlDialog::OnButtonLocctrlF2Click )

    EVT_BUTTON( ID_BUTTON_LOCCTRL_F3, LocControlDialog::OnButtonLocctrlF3Click )

    EVT_BUTTON( ID_BUTTON_LOCCTRL_F4, LocControlDialog::OnButtonLocctrlF4Click )

    EVT_BUTTON( ID_BUTTON_LOCCTRL_F5, LocControlDialog::OnButtonLocctrlF5Click )

    EVT_BUTTON( ID_BUTTON_LOCCTRL_F6, LocControlDialog::OnButtonLocctrlF6Click )

    EVT_BUTTON( ID_BUTTON_LCCTRL_F7, LocControlDialog::OnButtonLcctrlF7Click )

    EVT_BUTTON( ID_BUTTON_LCCTRL_F8, LocControlDialog::OnButtonLcctrlF8Click )

    EVT_BUTTON( ID_BUTTON_LCCTRL_9, LocControlDialog::OnButtonLcctrl9Click )

    EVT_BUTTON( ID_BUTTON_LCCTRL_F10, LocControlDialog::OnButtonLcctrlF10Click )

    EVT_BUTTON( ID_BUTTON_LCCTRL_F11, LocControlDialog::OnButtonLcctrlF11Click )

    EVT_BUTTON( ID_BUTTON_LCCTRL_F12, LocControlDialog::OnButtonLcctrlF12Click )

    EVT_BUTTON( ID_BUTTON_LOCCTRL_FN, LocControlDialog::OnButtonLocctrlFnClick )

    EVT_BUTTON( ID_FG, LocControlDialog::OnFgClick )

    EVT_BUTTON( ID_BITMAPBUTTON_LOCCTRL_DIR, LocControlDialog::OnBitmapbuttonLocctrlDirClick )

    EVT_COMBOBOX( ID_COMBOBOX_LOCCTRL_LOC, LocControlDialog::OnComboboxLocctrlLocSelected )

    EVT_BUTTON( ID_BUTTON_LOCCTRL_CANCEL, LocControlDialog::OnButtonLocctrlCancelClick )

    EVT_BUTTON( ID_BUTTON_LOCCTRL_STOP, LocControlDialog::OnButtonLocctrlStopClick )

    EVT_BUTTON( ID_BUTTON_LOCCTRL_BREAK, LocControlDialog::OnButtonLocctrlBreakClick )

////@end LocControlDialog event table entries

END_EVENT_TABLE()

/*!
 * LocControlDialog constructors
 */

LocControlDialog::LocControlDialog( )
{
}

LocControlDialog::LocControlDialog( wxWindow* parent, iOList list, iOMap map, const char* locid )
{
    Create(parent, -1, wxGetApp().getMsg( "locctrl" ));

  m_iSpeed = 0;
  m_iFnGroup = 0;
  m_SpeedCtrl->SetRange( 0, 100 );
  m_SpeedCtrl->SetValue(0);
  initLocMap(locid);

  initLabels();
  setFLabels();

  m_DlgList = list;
  m_DlgMap  = map;
  ListOp.add( m_DlgList, (obj)this );

  GetSizer()->Layout();
  GetSizer()->Fit(this);
  SetFocus();
}


void LocControlDialog::initLabels() {
  m_Cancel->SetLabel( wxGetApp().getMsg( "cancel" ) );
  m_Stop->SetLabel( wxGetApp().getMsg( "stop" ) );
  m_Break->SetLabel( wxGetApp().getMsg( "break" ) );

  wxFont font = m_Speed->GetFont();
  wxFont font2( font.GetPointSize() * 2, font.GetFamily(), font.GetStyle(), wxBOLD );
  m_Speed->SetFont( font2 );
  m_Speed->SetBackgroundColour( Base::getBlue() );

  for( int i = 0; i < 36; i++ ) {
    m_bFx[i] = false;;
  }

  m_bFn = false;
  m_Fn->SetBackgroundColour( Base::getGreen() );
  m_F1->SetBackgroundColour( Base::getGreen() );
  m_F2->SetBackgroundColour( Base::getGreen() );
  m_F3->SetBackgroundColour( Base::getGreen() );
  m_F4->SetBackgroundColour( Base::getGreen() );
  m_F5->SetBackgroundColour( Base::getGreen() );
  m_F6->SetBackgroundColour( Base::getGreen() );
  m_F7->SetBackgroundColour( Base::getGreen() );
  m_F8->SetBackgroundColour( Base::getGreen() );
  m_F9->SetBackgroundColour( Base::getGreen() );
  m_F10->SetBackgroundColour( Base::getGreen() );
  m_F11->SetBackgroundColour( Base::getGreen() );
  m_F12->SetBackgroundColour( Base::getGreen() );

  m_bDir = true;
  m_Dir->SetBitmapLabel( wxBitmap(rocrail_forwards_xpm) );
}


void LocControlDialog::setFLabels() {
  if( m_iFnGroup == 0 ) {
    m_F1->SetLabel( _T("F1") );
    m_F2->SetLabel( _T("F2") );
    m_F3->SetLabel( _T("F3") );
    m_F4->SetLabel( _T("F4") );
    m_F5->SetLabel( _T("F5") );
    m_F6->SetLabel( _T("F6") );
    m_F7->SetLabel( _T("F7") );
    m_F8->SetLabel( _T("F8") );
    m_F9->SetLabel( _T("F9") );
    m_F10->SetLabel( _T("F10") );
    m_F11->SetLabel( _T("F11") );
    m_F12->SetLabel( _T("F12") );
  }
  else if( m_iFnGroup == 1 ) {
    m_F1->SetLabel( _T("F13") );
    m_F2->SetLabel( _T("F14") );
    m_F3->SetLabel( _T("F15") );
    m_F4->SetLabel( _T("F16") );
    m_F5->SetLabel( _T("F17") );
    m_F6->SetLabel( _T("F18") );
    m_F7->SetLabel( _T("F19") );
    m_F8->SetLabel( _T("F20") );
    m_F9->SetLabel( _T("F21") );
    m_F10->SetLabel( _T("F22") );
    m_F11->SetLabel( _T("F23") );
    m_F12->SetLabel( _T("F24") );
  }

  if( m_LcList->GetSelection() == wxNOT_FOUND )
    return;
  wxString id = m_LcList->GetStringSelection();

  iONode lc = (iONode)MapOp.get( m_lcMap, id.mb_str(wxConvUTF8) );
  if(lc != NULL) {
    int fx = wLoc.getfx(lc);
    if( m_iFnGroup > 1 )
      m_iFnGroup = 0;
    fx = fx >> (m_iFnGroup * 12 );
    m_bFx[0 +m_iFnGroup * 12] = setButtonColor( m_F1 , (fx & 0x001)?false:true );
    m_bFx[1 +m_iFnGroup * 12] = setButtonColor( m_F2 , (fx & 0x002)?false:true );
    m_bFx[2 +m_iFnGroup * 12] = setButtonColor( m_F3 , (fx & 0x004)?false:true );
    m_bFx[3 +m_iFnGroup * 12] = setButtonColor( m_F4 , (fx & 0x008)?false:true );
    m_bFx[4 +m_iFnGroup * 12] = setButtonColor( m_F5 , (fx & 0x010)?false:true );
    m_bFx[5 +m_iFnGroup * 12] = setButtonColor( m_F6 , (fx & 0x020)?false:true );
    m_bFx[6 +m_iFnGroup * 12] = setButtonColor( m_F7 , (fx & 0x040)?false:true );
    m_bFx[7 +m_iFnGroup * 12] = setButtonColor( m_F8 , (fx & 0x080)?false:true );
    m_bFx[8 +m_iFnGroup * 12] = setButtonColor( m_F9 , (fx & 0x100)?false:true );
    m_bFx[9 +m_iFnGroup * 12] = setButtonColor( m_F10, (fx & 0x200)?false:true );
    m_bFx[10+m_iFnGroup * 12] = setButtonColor( m_F11, (fx & 0x400)?false:true );
    m_bFx[11+m_iFnGroup * 12] = setButtonColor( m_F12, (fx & 0x800)?false:true );

    m_F1 ->SetToolTip( wxString::Format(_T("F%d"), 1  + (m_iFnGroup * 12 ) ));
    m_F2 ->SetToolTip( wxString::Format(_T("F%d"), 2  + (m_iFnGroup * 12 ) ));
    m_F3 ->SetToolTip( wxString::Format(_T("F%d"), 3  + (m_iFnGroup * 12 ) ));
    m_F4 ->SetToolTip( wxString::Format(_T("F%d"), 4  + (m_iFnGroup * 12 ) ));
    m_F5 ->SetToolTip( wxString::Format(_T("F%d"), 5  + (m_iFnGroup * 12 ) ));
    m_F6 ->SetToolTip( wxString::Format(_T("F%d"), 6  + (m_iFnGroup * 12 ) ));
    m_F7 ->SetToolTip( wxString::Format(_T("F%d"), 7  + (m_iFnGroup * 12 ) ));
    m_F8 ->SetToolTip( wxString::Format(_T("F%d"), 8  + (m_iFnGroup * 12 ) ));
    m_F9 ->SetToolTip( wxString::Format(_T("F%d"), 9  + (m_iFnGroup * 12 ) ));
    m_F10->SetToolTip( wxString::Format(_T("F%d"), 10 + (m_iFnGroup * 12 ) ));
    m_F11->SetToolTip( wxString::Format(_T("F%d"), 11 + (m_iFnGroup * 12 ) ));
    m_F12->SetToolTip( wxString::Format(_T("F%d"), 12 + (m_iFnGroup * 12 ) ));

    iONode fundef = wLoc.getfundef( lc );
    while( fundef != NULL ) {
      wxString fntxt = wxString(wFunDef.gettext( fundef ),wxConvUTF8);
      if( wFunDef.getfn( fundef ) == 1 + (m_iFnGroup * 12 )) {
        m_F1->SetToolTip( fntxt );
      }
      else if( wFunDef.getfn( fundef ) == 2 + (m_iFnGroup * 12 ) ) {
        m_F2->SetToolTip( fntxt );
      }
      else if( wFunDef.getfn( fundef ) == 3 + (m_iFnGroup * 12 ) ) {
        m_F3->SetToolTip( fntxt );
      }
      else if( wFunDef.getfn( fundef ) == 4 + (m_iFnGroup * 12 ) ) {
        m_F4->SetToolTip( fntxt );
      }
      else if( wFunDef.getfn( fundef ) == 5 + (m_iFnGroup * 12 ) ) {
        m_F5->SetToolTip( fntxt );
      }
      else if( wFunDef.getfn( fundef ) == 6 + (m_iFnGroup * 12 ) ) {
        m_F6->SetToolTip( fntxt );
      }
      else if( wFunDef.getfn( fundef ) == 7 + (m_iFnGroup * 12 ) ) {
        m_F7->SetToolTip( fntxt );
      }
      else if( wFunDef.getfn( fundef ) == 8 + (m_iFnGroup * 12 ) ) {
        m_F8->SetToolTip( fntxt );
      }
      else if( wFunDef.getfn( fundef ) == 9 + (m_iFnGroup * 12 ) ) {
        m_F9->SetToolTip( fntxt );
      }
      else if( wFunDef.getfn( fundef ) == 10 + (m_iFnGroup * 12 ) ) {
        m_F10->SetToolTip( fntxt );
      }
      else if( wFunDef.getfn( fundef ) == 11 + (m_iFnGroup * 12 ) ) {
        m_F11->SetToolTip( fntxt );
      }
      else if( wFunDef.getfn( fundef ) == 12 + (m_iFnGroup * 12 ) ) {
        m_F12->SetToolTip( fntxt );
      }
      fundef = wLoc.nextfundef( lc, fundef );
    }
  }

}


void LocControlDialog::modelEvent( iONode evt ) {
  const char* et = NodeOp.getName( evt );
  const char* id = wLoc.getid( evt );
  wxString listid = m_LcList->GetStringSelection();
  if( StrOp.equals( wLoc.name(), et ) && StrOp.equals( id, listid.mb_str(wxConvUTF8) ) ) {
    /* update speed */
    wxString value;
    value.Printf( _T("%d"), wLoc.getV( evt ) );
    m_Speed->SetValue( value );
    m_SpeedCtrl->SetValue( wLoc.getV( evt ) );

    /* update direction */
    m_bDir = wLoc.isdir( evt)?true:false;
    m_Dir->SetBitmapLabel( m_bDir?wxBitmap(rocrail_forwards_xpm):wxBitmap(rocrail_reverse_xpm) );
    m_Dir->SetToolTip( m_bDir? wxGetApp().getMsg( "forwards" ):wxGetApp().getMsg( "reverse" ) );
    m_Dir->Refresh();

    /* update function "n" */
    m_bFn = wLoc.isfn( evt)?true:false;
    setButtonColor( m_Fn, !m_bFn);
  }
  else if( StrOp.equals( wFunCmd.name(), et ) && StrOp.equals( id, listid.mb_str(wxConvUTF8) ) ) {
    iONode lc = (iONode)MapOp.get( m_lcMap, listid.mb_str(wxConvUTF8) );

    if(lc != NULL) {
      /* update function "n" */
      m_bFn = wFunCmd.isf0( evt)?true:false;
      setButtonColor( m_Fn, !m_bFn);
      setFLabels();
      /* update further functions */
      int fx = wLoc.getfx(evt);
      TraceOp.trc( "lcdlg", TRCLEVEL_INFO, __LINE__, 9999, "function update %s", wLoc.getid( evt ) );
      iONode node = evt;
      wLoc.setfx(lc,
          (NodeOp.getBool(node, "f1",  fx&0x0001?True:False)?0x0001:0x00) |
          (NodeOp.getBool(node, "f2",  fx&0x0002?True:False)?0x0002:0x00) |
          (NodeOp.getBool(node, "f3",  fx&0x0004?True:False)?0x0004:0x00) |
          (NodeOp.getBool(node, "f4",  fx&0x0008?True:False)?0x0008:0x00) |
          (NodeOp.getBool(node, "f5",  fx&0x0010?True:False)?0x0010:0x00) |
          (NodeOp.getBool(node, "f6",  fx&0x0020?True:False)?0x0020:0x00) |
          (NodeOp.getBool(node, "f7",  fx&0x0040?True:False)?0x0040:0x00) |
          (NodeOp.getBool(node, "f8",  fx&0x0080?True:False)?0x0080:0x00) |

          (NodeOp.getBool(node, "f9" ,  fx&0x0100?True:False)?0x0100:0x00) |
          (NodeOp.getBool(node, "f10",  fx&0x0200?True:False)?0x0200:0x00) |
          (NodeOp.getBool(node, "f11",  fx&0x0400?True:False)?0x0400:0x00) |
          (NodeOp.getBool(node, "f12",  fx&0x0800?True:False)?0x0800:0x00) |
          (NodeOp.getBool(node, "f13",  fx&0x1000?True:False)?0x1000:0x00) |
          (NodeOp.getBool(node, "f14",  fx&0x2000?True:False)?0x2000:0x00) |
          (NodeOp.getBool(node, "f15",  fx&0x4000?True:False)?0x4000:0x00) |
          (NodeOp.getBool(node, "f16",  fx&0x8000?True:False)?0x8000:0x00) |

          (NodeOp.getBool(node, "f17",  fx&0x010000?True:False)?0x010000:0x00) |
          (NodeOp.getBool(node, "f18",  fx&0x020000?True:False)?0x020000:0x00) |
          (NodeOp.getBool(node, "f19",  fx&0x040000?True:False)?0x040000:0x00) |
          (NodeOp.getBool(node, "f20",  fx&0x080000?True:False)?0x080000:0x00) |
          (NodeOp.getBool(node, "f21",  fx&0x100000?True:False)?0x100000:0x00) |
          (NodeOp.getBool(node, "f22",  fx&0x200000?True:False)?0x200000:0x00) |
          (NodeOp.getBool(node, "f23",  fx&0x400000?True:False)?0x400000:0x00) |
          (NodeOp.getBool(node, "f24",  fx&0x800000?True:False)?0x800000:0x00) |

          (NodeOp.getBool(node, "f25",  fx&0x01000000?True:False)?0x01000000:0x00) |
          (NodeOp.getBool(node, "f26",  fx&0x02000000?True:False)?0x02000000:0x00) |
          (NodeOp.getBool(node, "f27",  fx&0x04000000?True:False)?0x04000000:0x00) |
          (NodeOp.getBool(node, "f28",  fx&0x08000000?True:False)?0x08000000:0x00)
          );
      setFLabels();
    }
  }
}


void LocControlDialog::initValues() {
  if( m_LcList->GetSelection() == wxNOT_FOUND )
    return;

  wxString selVal = m_LcList->GetStringSelection();
  updateFnText( selVal );

  iONode lc = (iONode)MapOp.get( m_lcMap, selVal.mb_str(wxConvUTF8) );

  if( lc != NULL && wLoc.getimage( lc ) != NULL && StrOp.len(wLoc.getimage( lc )) > 0 ) {
    wxBitmapType bmptype = wxBITMAP_TYPE_XPM;
    if( StrOp.endsWithi( wLoc.getimage( lc ), ".gif" ) )
      bmptype = wxBITMAP_TYPE_GIF;
    else if( StrOp.endsWithi( wLoc.getimage( lc ), ".png" ) )
      bmptype = wxBITMAP_TYPE_PNG;

    const char* imagepath = wGui.getimagepath(wxGetApp().getIni());
    static char pixpath[256];
    StrOp.fmtb( pixpath, "%s%c%s", imagepath, SystemOp.getFileSeparator(), FileOp.ripPath( wLoc.getimage( lc ) ) );

    if( FileOp.exist(pixpath)) {
      TraceOp.trc( "locdialog", TRCLEVEL_INFO, __LINE__, 9999, "picture [%s]", pixpath );
      m_Image->SetBitmapLabel( wxBitmap(wxString(pixpath,wxConvUTF8), bmptype) );
    }
    else {
      TraceOp.trc( "locdialog", TRCLEVEL_WARNING, __LINE__, 9999, "picture [%s] not found", pixpath );
      m_Image->SetBitmapLabel( wxBitmap(nopict_xpm) );
    }
    m_Image->SetToolTip(wxString(wLoc.getdesc( lc ),wxConvUTF8));
  }
  else {
    m_Image->SetBitmapLabel( wxBitmap(nopict_xpm) );
  }
  m_Image->Refresh();

  m_SpeedCtrl->SetRange( 0, wLoc.getV_max(lc) );
  m_SpeedCtrl->SetValue( wLoc.getV(lc) );
  m_iSpeed = wLoc.getV(lc) != -1 ? wLoc.getV(lc) : 0;
  m_bDir = wLoc.isdir(lc)?true:false;
  m_Dir->SetBitmapLabel( m_bDir?wxBitmap(rocrail_forwards_xpm):wxBitmap(rocrail_reverse_xpm) );
  m_Dir->SetToolTip( m_bDir? wxGetApp().getMsg( "forwards" ):wxGetApp().getMsg( "reverse" ) );
  m_Dir->Refresh();

  setFLabels();

  m_bFn = wLoc.isfn( lc )?true:false;
  setButtonColor( m_Fn, !m_bFn );

  wxString value;
  value.Printf( _T("%d"), m_iSpeed );
  m_Speed->SetValue( value );
  //m_Dir->Enable( m_iSpeed==0?true:false);
  GetSizer()->Fit(this);
  GetSizer()->Layout();
}


void LocControlDialog::initFx(iONode lc) {
}


void LocControlDialog::updateFnText( wxString& selVal ) {
  iONode lc = (iONode)MapOp.get( m_lcMap, selVal.mb_str(wxConvUTF8) );
  if( lc != NULL ) {
    iONode fundef = wLoc.getfundef( lc );
    while( fundef != NULL ) {
      wxString fntxt = wxString(wFunDef.gettext( fundef ),wxConvUTF8);
      switch( wFunDef.getfn( fundef ) ) {
        case 1: m_F1->SetToolTip( fntxt ); break;
        case 2: m_F2->SetToolTip( fntxt ); break;
        case 3: m_F3->SetToolTip( fntxt ); break;
        case 4: m_F4->SetToolTip( fntxt ); break;
        case 5: m_F5->SetToolTip( fntxt ); break;
        case 6: m_F6->SetToolTip( fntxt ); break;
        case 7: m_F7->SetToolTip( fntxt ); break;
        case 8: m_F8->SetToolTip( fntxt ); break;
        case 9 : m_F9->SetToolTip( fntxt ); break;
        case 10: m_F10->SetToolTip( fntxt ); break;
        case 11: m_F11->SetToolTip( fntxt ); break;
        case 12: m_F12->SetToolTip( fntxt ); break;
      }
      fundef = wLoc.nextfundef( lc, fundef );
    }
    //GetSizer()->Fit(this);
    //GetSizer()->Layout();
  }
}


static int locComparator(obj* o1, obj* o2) {
  if( *o1 == NULL || *o2 == NULL )
    return 0;
  return strcmp( wLoc.getid( (iONode)*o1 ), wLoc.getid( (iONode)*o2 ) );
}

void LocControlDialog::initLocMap(const char* locid) {
  m_lcMap     = MapOp.inst();

  iONode model = wxGetApp().getModel();
  if( model != NULL ) {
    iONode lclist = wPlan.getlclist( model );
    if( lclist != NULL ) {
      int i;
      int cnt = NodeOp.getChildCnt( lclist );
      iOList list = ListOp.inst();

      for( i = 0; i < cnt; i++ ) {
        iONode lc = NodeOp.getChild( lclist, i );
        ListOp.add( list, (obj)lc );
      }
      // Sort the list:
      ListOp.sort( list, locComparator );

      for( i = 0; i < ListOp.size( list ); i++ ) {
        iONode lc = (iONode)ListOp.get( list, i );
        if( lc == NULL )
          continue;
        const char* id = wLoc.getid( lc );
      }

      for( i = 0; i < cnt; i++ ) {
        iONode lc = (iONode)ListOp.get( list, i );
        if( lc == NULL )
          continue;
        const char* id = wLoc.getid( lc );
        if( id != NULL && wLoc.getaddr(lc) > 0 ) {
          m_LcList->Append( wxString(id,wxConvUTF8) );
          MapOp.put( m_lcMap, id, (obj)lc );
        }
      }
      if( cnt > 0 ) {
        if( locid == NULL )
          m_LcList->SetSelection(0);
        else
          m_LcList->SetStringSelection(wxString(locid,wxConvUTF8));
        initValues();
      }

      ListOp.base.del( list );
    }
  }
}



/*!
 * LocControlDialog creator
 */

bool LocControlDialog::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin LocControlDialog member initialisation
    m_Image = NULL;
    m_Speed = NULL;
    m_SpeedCtrl = NULL;
    m_F1 = NULL;
    m_F2 = NULL;
    m_F3 = NULL;
    m_F4 = NULL;
    m_F5 = NULL;
    m_F6 = NULL;
    m_F7 = NULL;
    m_F8 = NULL;
    m_F9 = NULL;
    m_F10 = NULL;
    m_F11 = NULL;
    m_F12 = NULL;
    m_Fn = NULL;
    m_FG = NULL;
    m_Dir = NULL;
    m_LcList = NULL;
    m_Cancel = NULL;
    m_Stop = NULL;
    m_Break = NULL;
////@end LocControlDialog member initialisation

////@begin LocControlDialog creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end LocControlDialog creation
    return true;
}

/*!
 * Control creation for LocControlDialog
 */

void LocControlDialog::CreateControls()
{
////@begin LocControlDialog content construction
    LocControlDialog* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    m_Image = new wxBitmapButton( itemDialog1, ID_BITMAPBUTTON_LOCCTRL_IMAGE, wxNullBitmap, wxDefaultPosition, wxSize(240, 88), wxBU_AUTODRAW );
    itemBoxSizer2->Add(m_Image, 0, wxGROW|wxALL, 2);

    wxFlexGridSizer* itemFlexGridSizer4 = new wxFlexGridSizer(0, 2, 0, 0);
    itemFlexGridSizer4->AddGrowableCol(1);
    itemBoxSizer2->Add(itemFlexGridSizer4, 0, wxGROW|wxALL, 0);

    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxVERTICAL);
    itemFlexGridSizer4->Add(itemBoxSizer5, 0, wxGROW|wxGROW, 0);

    m_Speed = new wxTextCtrl( itemDialog1, ID_TEXTCTRL_LOCCTRL_SPEED, _("0"), wxDefaultPosition, wxSize(-1, 40), wxTE_READONLY|wxTE_CENTRE );
    itemBoxSizer5->Add(m_Speed, 0, wxGROW|wxALL, 2);

    m_SpeedCtrl = new wxSlider( itemDialog1, ID_SLIDER_LOCCTRL_SPEED, 0, 0, 100, wxDefaultPosition, wxSize(-1, 100), wxSL_VERTICAL|wxSL_INVERSE );
    itemBoxSizer5->Add(m_SpeedCtrl, 1, wxALIGN_CENTER_HORIZONTAL|wxALL, 2);

    wxBoxSizer* itemBoxSizer8 = new wxBoxSizer(wxVERTICAL);
    itemFlexGridSizer4->Add(itemBoxSizer8, 0, wxALIGN_CENTER_HORIZONTAL|wxGROW, 0);

    wxFlexGridSizer* itemFlexGridSizer9 = new wxFlexGridSizer(0, 3, 0, 0);
    itemBoxSizer8->Add(itemFlexGridSizer9, 0, wxGROW|wxRIGHT, 5);

    m_F1 = new wxButton( itemDialog1, ID_BUTTON_LOCCTRL_F1, _("F1"), wxDefaultPosition, wxSize(50, -1), 0 );
    itemFlexGridSizer9->Add(m_F1, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxBOTTOM, 2);

    m_F2 = new wxButton( itemDialog1, ID_BUTTON_LOCCTRL_F2, _("F2"), wxDefaultPosition, wxSize(50, -1), 0 );
    itemFlexGridSizer9->Add(m_F2, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxBOTTOM, 2);

    m_F3 = new wxButton( itemDialog1, ID_BUTTON_LOCCTRL_F3, _("F3"), wxDefaultPosition, wxSize(50, -1), 0 );
    itemFlexGridSizer9->Add(m_F3, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxBOTTOM, 2);

    m_F4 = new wxButton( itemDialog1, ID_BUTTON_LOCCTRL_F4, _("F4"), wxDefaultPosition, wxSize(50, -1), 0 );
    itemFlexGridSizer9->Add(m_F4, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxBOTTOM, 2);

    m_F5 = new wxButton( itemDialog1, ID_BUTTON_LOCCTRL_F5, _("F5"), wxDefaultPosition, wxSize(50, -1), 0 );
    itemFlexGridSizer9->Add(m_F5, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxBOTTOM, 2);

    m_F6 = new wxButton( itemDialog1, ID_BUTTON_LOCCTRL_F6, _("F6"), wxDefaultPosition, wxSize(50, -1), 0 );
    itemFlexGridSizer9->Add(m_F6, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxBOTTOM, 2);

    m_F7 = new wxButton( itemDialog1, ID_BUTTON_LCCTRL_F7, _("F7"), wxDefaultPosition, wxSize(50, -1), 0 );
    itemFlexGridSizer9->Add(m_F7, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxBOTTOM, 2);

    m_F8 = new wxButton( itemDialog1, ID_BUTTON_LCCTRL_F8, _("F8"), wxDefaultPosition, wxSize(50, -1), 0 );
    itemFlexGridSizer9->Add(m_F8, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxBOTTOM, 2);

    m_F9 = new wxButton( itemDialog1, ID_BUTTON_LCCTRL_9, _("F9"), wxDefaultPosition, wxSize(50, -1), 0 );
    itemFlexGridSizer9->Add(m_F9, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxBOTTOM, 2);

    m_F10 = new wxButton( itemDialog1, ID_BUTTON_LCCTRL_F10, _("F10"), wxDefaultPosition, wxSize(50, -1), 0 );
    itemFlexGridSizer9->Add(m_F10, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxBOTTOM, 2);

    m_F11 = new wxButton( itemDialog1, ID_BUTTON_LCCTRL_F11, _("F11"), wxDefaultPosition, wxSize(50, -1), 0 );
    itemFlexGridSizer9->Add(m_F11, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxBOTTOM, 2);

    m_F12 = new wxButton( itemDialog1, ID_BUTTON_LCCTRL_F12, _("F12"), wxDefaultPosition, wxSize(50, -1), 0 );
    itemFlexGridSizer9->Add(m_F12, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxBOTTOM, 2);

    wxFlexGridSizer* itemFlexGridSizer22 = new wxFlexGridSizer(0, 2, 0, 0);
    itemFlexGridSizer22->AddGrowableCol(1);
    itemBoxSizer8->Add(itemFlexGridSizer22, 0, wxGROW|wxRIGHT, 5);

    m_Fn = new wxButton( itemDialog1, ID_BUTTON_LOCCTRL_FN, _("Fn"), wxDefaultPosition, wxSize(50, -1), 0 );
    itemFlexGridSizer22->Add(m_Fn, 0, wxALIGN_LEFT|wxALIGN_TOP|wxBOTTOM, 2);

    m_FG = new wxButton( itemDialog1, ID_FG, _("FG"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer22->Add(m_FG, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxBOTTOM, 5);

    m_Dir = new wxBitmapButton( itemDialog1, ID_BITMAPBUTTON_LOCCTRL_DIR, itemDialog1->GetBitmapResource(wxT("../xpm/dir.xpm")), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
    itemBoxSizer8->Add(m_Dir, 1, wxGROW|wxLEFT|wxRIGHT|wxBOTTOM, 2);

    wxArrayString m_LcListStrings;
    m_LcList = new wxComboBox( itemDialog1, ID_COMBOBOX_LOCCTRL_LOC, _T(""), wxDefaultPosition, wxDefaultSize, m_LcListStrings, wxCB_READONLY );
    itemBoxSizer2->Add(m_LcList, 0, wxGROW|wxALL, 2);

    wxBoxSizer* itemBoxSizer27 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer27, 0, wxGROW|wxALL, 0);

    m_Cancel = new wxButton( itemDialog1, ID_BUTTON_LOCCTRL_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Cancel->SetDefault();
    itemBoxSizer27->Add(m_Cancel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    m_Stop = new wxButton( itemDialog1, ID_BUTTON_LOCCTRL_STOP, _("Stop"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer27->Add(m_Stop, 1, wxALIGN_CENTER_VERTICAL|wxALL, 2);

    wxBoxSizer* itemBoxSizer30 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer30, 0, wxGROW|wxALL, 0);

    m_Break = new wxButton( itemDialog1, ID_BUTTON_LOCCTRL_BREAK, _("BREAK"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer30->Add(m_Break, 1, wxGROW|wxALL, 2);

    // Connect events and objects
    itemDialog1->Connect(ID_LOCCONTROL, wxEVT_DESTROY, wxWindowDestroyEventHandler(LocControlDialog::OnDestroy), NULL, this);
////@end LocControlDialog content construction
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BITMAPBUTTON_LOCCTRL_IMAGE
 */

void LocControlDialog::OnBitmapbuttonLocctrlImageClick( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BITMAPBUTTON_LOCCTRL_IMAGE in LocControlDialog.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BITMAPBUTTON_LOCCTRL_IMAGE in LocControlDialog.
}

/*!
 * wxEVT_SCROLL_* event handler for ID_SLIDER_LOCCTRL_SPEED
 */

void LocControlDialog::OnSliderLocctrlSpeedScroll( wxScrollEvent& event )
{
  if ( event.GetEventObject() == m_SpeedCtrl ) {
    m_iSpeed = m_SpeedCtrl->GetValue();
    speedCmd( event.GetEventType() != wxEVT_SCROLL_THUMBTRACK );
  }
}



/*!
 * wxEVT_SCROLL_ENDSCROLL event handler for ID_SLIDER_LOCCTRL_SPEED

void LocControlDialog::OnSliderLocctrlSpeedEndScroll( wxScrollEvent& event )
{
  int val = m_SpeedCtrl->GetValue();
  m_iSpeed = val;
  speedCmd(true);
}
*/

bool LocControlDialog::setButtonColor( wxButton* button, bool state ) {
  if( state ) {
    button->SetBackgroundColour( Base::getGreen() );
    wxFont f = button->GetFont();
    f.SetUnderlined(false);
    button->SetFont(f);
    return false;
  }
  else {
    button->SetBackgroundColour( Base::getRed() );
    wxFont f = button->GetFont();
    f.SetUnderlined(true);
    button->SetFont(f);
    return true;
  }
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_LOCCTRL_FN
 */

void LocControlDialog::OnButtonLocctrlFnClick( wxCommandEvent& event )
{
  m_bFn = setButtonColor( m_Fn, m_bFn );
  speedCmd(true);
  funCmd(0);
}


void LocControlDialog::speedCmd(bool sendCmd)
{
  if( m_LcList->GetSelection() == wxNOT_FOUND )
    return;

  wxString value;
  value.Printf( _T("%d"), m_iSpeed );
  m_Speed->SetValue( value );

  wxString id = m_LcList->GetStringSelection();

  if( sendCmd ) {
    iONode cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
    wLoc.setid( cmd, id.mb_str(wxConvUTF8) );
    wLoc.setV( cmd, m_iSpeed );
    wLoc.setfn( cmd, m_bFn?True:False );
    wLoc.setdir( cmd, m_bDir?True:False );
    wxGetApp().sendToRocrail( cmd );
    cmd->base.del(cmd);
  }
}


void LocControlDialog::funCmd(int group)
{
  if( m_LcList->GetSelection() == wxNOT_FOUND )
    return;
  wxString id = m_LcList->GetStringSelection();

  iONode lc = (iONode)MapOp.get( m_lcMap, id.mb_str(wxConvUTF8) );
  if(lc != NULL) {
    wLoc.setfx(lc,
        (m_bFx[ 0]?0x0001:0x00) | (m_bFx[ 1]?0x0002:0x00) | (m_bFx[ 2]?0x0004:0x00) | (m_bFx[ 3]?0x0008:0x00) |
        (m_bFx[ 4]?0x0010:0x00) | (m_bFx[ 5]?0x0020:0x00) | (m_bFx[ 6]?0x0040:0x00) | (m_bFx[ 7]?0x0080:0x00) |
        (m_bFx[ 8]?0x0100:0x00) | (m_bFx[ 9]?0x0200:0x00) | (m_bFx[10]?0x0400:0x00) | (m_bFx[11]?0x0800:0x00) |

        (m_bFx[12]?0x00001000:0x00) | (m_bFx[13]?0x00002000:0x00) | (m_bFx[14]?0x00004000:0x00) | (m_bFx[15]?0x00008000:0x00) |
        (m_bFx[16]?0x00010000:0x00) | (m_bFx[17]?0x00020000:0x00) | (m_bFx[18]?0x00040000:0x00) | (m_bFx[19]?0x00080000:0x00) |
        (m_bFx[20]?0x00100000:0x00) | (m_bFx[21]?0x00200000:0x00) | (m_bFx[22]?0x00400000:0x00) | (m_bFx[23]?0x00800000:0x00) |
        (m_bFx[24]?0x01000000:0x00) | (m_bFx[25]?0x02000000:0x00) | (m_bFx[26]?0x04000000:0x00) | (m_bFx[27]?0x08000000:0x00)
        );

    iONode cmd = NodeOp.inst( wFunCmd.name(), NULL, ELEMENT_NODE );
    wFunCmd.setgroup ( cmd, m_iFnGroup * 3 + 1 + group );
    wFunCmd.setfncnt ( cmd, wLoc.getfncnt( lc ) );
    wFunCmd.setid ( cmd, wLoc.getid( lc ) );
    wFunCmd.setf0 ( cmd, m_bFn?True:False );
    wFunCmd.setf1 ( cmd, m_bFx[ 0]?True:False );
    wFunCmd.setf2 ( cmd, m_bFx[ 1]?True:False );
    wFunCmd.setf3 ( cmd, m_bFx[ 2]?True:False );
    wFunCmd.setf4 ( cmd, m_bFx[ 3]?True:False );
    wFunCmd.setf5 ( cmd, m_bFx[ 4]?True:False );
    wFunCmd.setf6 ( cmd, m_bFx[ 5]?True:False );
    wFunCmd.setf7 ( cmd, m_bFx[ 6]?True:False );
    wFunCmd.setf8 ( cmd, m_bFx[ 7]?True:False );
    wFunCmd.setf9 ( cmd, m_bFx[ 8]?True:False );
    wFunCmd.setf10( cmd, m_bFx[ 9]?True:False );
    wFunCmd.setf11( cmd, m_bFx[10]?True:False );
    wFunCmd.setf12( cmd, m_bFx[11]?True:False );
    wFunCmd.setf13( cmd, m_bFx[12]?True:False );
    wFunCmd.setf14( cmd, m_bFx[13]?True:False );
    wFunCmd.setf15( cmd, m_bFx[14]?True:False );
    wFunCmd.setf16( cmd, m_bFx[15]?True:False );
    wFunCmd.setf17( cmd, m_bFx[16]?True:False );
    wFunCmd.setf18( cmd, m_bFx[17]?True:False );
    wFunCmd.setf19( cmd, m_bFx[18]?True:False );
    wFunCmd.setf20( cmd, m_bFx[19]?True:False );
    wFunCmd.setf21( cmd, m_bFx[20]?True:False );
    wFunCmd.setf22( cmd, m_bFx[21]?True:False );
    wFunCmd.setf23( cmd, m_bFx[22]?True:False );
    wFunCmd.setf24( cmd, m_bFx[23]?True:False );
    wFunCmd.setf25( cmd, m_bFx[24]?True:False );
    wFunCmd.setf26( cmd, m_bFx[25]?True:False );
    wFunCmd.setf27( cmd, m_bFx[26]?True:False );
    wFunCmd.setf28( cmd, m_bFx[27]?True:False );
    wxGetApp().sendToRocrail( cmd );
    cmd->base.del(cmd);
  }
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_LOCCTRL_F1
 */

void LocControlDialog::OnButtonLocctrlF1Click( wxCommandEvent& event )
{
  wxString id = m_LcList->GetStringSelection();

  m_bFx[0+m_iFnGroup*12] = setButtonColor( m_F1, m_bFx[0+m_iFnGroup*12] );
  funCmd(0);
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_LOCCTRL_F2
 */

void LocControlDialog::OnButtonLocctrlF2Click( wxCommandEvent& event )
{
  wxString id = m_LcList->GetStringSelection();

  m_bFx[1+m_iFnGroup*12] = setButtonColor( m_F2, m_bFx[1+m_iFnGroup*12] );
  funCmd(0);
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_LOCCTRL_F3
 */

void LocControlDialog::OnButtonLocctrlF3Click( wxCommandEvent& event )
{
  wxString id = m_LcList->GetStringSelection();

  m_bFx[2+m_iFnGroup*12] = setButtonColor( m_F3, m_bFx[2+m_iFnGroup*12] );
  funCmd(0);
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_LOCCTRL
 */

void LocControlDialog::OnButtonLocctrlF4Click( wxCommandEvent& event )
{
  wxString id = m_LcList->GetStringSelection();

  m_bFx[3+m_iFnGroup*12] = setButtonColor( m_F4, m_bFx[3+m_iFnGroup*12] );
  funCmd(0);
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BITMAPBUTTON_LOCCTRL_DIR
 */

void LocControlDialog::OnBitmapbuttonLocctrlDirClick( wxCommandEvent& event )
{
  m_bDir = !m_bDir;
  m_Dir->SetBitmapLabel( m_bDir?wxBitmap(rocrail_forwards_xpm):wxBitmap(rocrail_reverse_xpm) );
  m_Dir->SetToolTip( m_bDir? wxGetApp().getMsg( "forwards" ):wxGetApp().getMsg( "reverse" ) );
  m_Dir->Refresh();
  speedCmd(true);
}

/*!
 * wxEVT_COMMAND_COMBOBOX_SELECTED event handler for ID_COMBOBOX_LOCCTRL_LOC
 */

void LocControlDialog::OnComboboxLocctrlLocSelected( wxCommandEvent& event )
{
  initValues();
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_LOCCTRL_CANCEL
 */

void LocControlDialog::OnButtonLocctrlCancelClick( wxCommandEvent& event )
{
  ListOp.removeObj( m_DlgList, (obj)this );

  if( m_LcList->GetSelection() == wxNOT_FOUND ) {
    Destroy();
    return;
  }

  int x,y;
  GetPosition(&x,&y);

  wxString id = m_LcList->GetStringSelection();
  char* LocID = StrOp.dup((const char*)id.mb_str(wxConvUTF8));

  TraceOp.trc( "locdialog", TRCLEVEL_INFO, __LINE__, 9999, "position [%d,%d] for [%s]", x, y, LocID );
  if( StrOp.len(LocID) > 0 ) {
    char* pos = (char*)MapOp.get(m_DlgMap, LocID);
    if( pos != NULL ) {
      TraceOp.trc( "locdialog", TRCLEVEL_INFO, __LINE__, 9999, "remove previous position [%s] for [%s]", pos, LocID );
      StrOp.free(pos);
      MapOp.remove(m_DlgMap, LocID);
    }
    pos = StrOp.fmt("%d,%d", x, y);
    MapOp.put( m_DlgMap, LocID, (obj)pos );
  }

  StrOp.free(LocID);

  Destroy();
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_LOCCTRL_STOP
 */

void LocControlDialog::OnButtonLocctrlStopClick( wxCommandEvent& event )
{
  m_iSpeed = 0;
  m_SpeedCtrl->SetValue( m_iSpeed );
  speedCmd(true);
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_LOCCTRL_BREAK
 */

void LocControlDialog::OnButtonLocctrlBreakClick( wxCommandEvent& event )
{
  iONode cmd = NodeOp.inst( wSysCmd.name(), NULL, ELEMENT_NODE );
  wLoc.setcmd( cmd, wSysCmd.stop );
  wSysCmd.setinformall(cmd, True);
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);
}

/*!
 * Should we show tooltips?
 */

bool LocControlDialog::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap LocControlDialog::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin LocControlDialog bitmap retrieval
    wxUnusedVar(name);
    if (name == _T("../xpm/dir.xpm"))
    {
        wxBitmap bitmap( rocrail_direction_xpm);
        return bitmap;
    }
    return wxNullBitmap;
////@end LocControlDialog bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon LocControlDialog::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin LocControlDialog icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end LocControlDialog icon retrieval
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_LOCCTRL_F5
 */

void LocControlDialog::OnButtonLocctrlF5Click( wxCommandEvent& event )
{
  wxString id = m_LcList->GetStringSelection();

  m_bFx[4+m_iFnGroup*12] = setButtonColor( m_F5, m_bFx[4+m_iFnGroup*12] );
  funCmd(1);
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_LOCCTRL_F6
 */

void LocControlDialog::OnButtonLocctrlF6Click( wxCommandEvent& event )
{
  wxString id = m_LcList->GetStringSelection();

  m_bFx[5+m_iFnGroup*12] = setButtonColor( m_F6, m_bFx[5+m_iFnGroup*12] );
  funCmd(1);
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_LCCTRL_F7
 */

void LocControlDialog::OnButtonLcctrlF7Click( wxCommandEvent& event )
{
  wxString id = m_LcList->GetStringSelection();

  m_bFx[6+m_iFnGroup*12] = setButtonColor( m_F7, m_bFx[6+m_iFnGroup*12] );
  funCmd(1);
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_LCCTRL_F8
 */

void LocControlDialog::OnButtonLcctrlF8Click( wxCommandEvent& event )
{
  wxString id = m_LcList->GetStringSelection();

  m_bFx[7+m_iFnGroup*12] = setButtonColor( m_F8, m_bFx[7+m_iFnGroup*12] );
  funCmd(1);
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_LCCTRL_9
 */

void LocControlDialog::OnButtonLcctrlF12Click( wxCommandEvent& event )
{
  wxString id = m_LcList->GetStringSelection();

  m_bFx[11+m_iFnGroup*12] = setButtonColor( m_F12, m_bFx[11+m_iFnGroup*12] );
  funCmd(2);
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_LCCTRL_9
 */

void LocControlDialog::OnButtonLcctrl9Click( wxCommandEvent& event )
{
  wxString id = m_LcList->GetStringSelection();

  m_bFx[8+m_iFnGroup*12] = setButtonColor( m_F9, m_bFx[8+m_iFnGroup*12] );
  funCmd(2);
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_LCCTRL_F10
 */

void LocControlDialog::OnButtonLcctrlF10Click( wxCommandEvent& event )
{
  wxString id = m_LcList->GetStringSelection();

  m_bFx[9+m_iFnGroup*12] = setButtonColor( m_F10, m_bFx[9+m_iFnGroup*12] );
  funCmd(2);
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON1
 */

void LocControlDialog::OnButtonLcctrlF11Click( wxCommandEvent& event )
{
  wxString id = m_LcList->GetStringSelection();

  m_bFx[10+m_iFnGroup*12] = setButtonColor( m_F11, m_bFx[10+m_iFnGroup*12] );
  funCmd(2);
}


/*!
 * wxEVT_CLOSE_WINDOW event handler for ID_LOCCONTROL
 */

void LocControlDialog::OnCloseWindow( wxCloseEvent& event )
{
  OnButtonLocctrlCancelClick((wxCommandEvent&)event);
}


/*!
 * wxEVT_DESTROY event handler for ID_LOCCONTROL
 */

void LocControlDialog::OnDestroy( wxWindowDestroyEvent& event )
{
  OnButtonLocctrlCancelClick((wxCommandEvent&)event);
}


/*!
 * wxEVT_KEY_DOWN event handler for ID_LOCCONTROL
 */

void LocControlDialog::OnKeyDown( wxKeyEvent& event )
{
  if( event.AltDown() || event.ControlDown() ) {
    event.Skip();
    return;
  }

  switch( event.GetKeyCode()) {
    case WXK_F1:
      m_bFx[0+m_iFnGroup*12] = setButtonColor( m_F1, m_bFx[0+m_iFnGroup*12] );
      funCmd(0);
      break;
    case WXK_F2:
      m_bFx[1+m_iFnGroup*12] = setButtonColor( m_F2, m_bFx[1+m_iFnGroup*12] );
      funCmd(0);
      break;
    case WXK_F3:
      m_bFx[2+m_iFnGroup*12] = setButtonColor( m_F3, m_bFx[2+m_iFnGroup*12] );
      funCmd(0);
      break;
    case WXK_F4:
      m_bFx[3+m_iFnGroup*12] = setButtonColor( m_F4, m_bFx[3+m_iFnGroup*12] );
      funCmd(0);
      break;
    case WXK_F5:
      m_bFx[4+m_iFnGroup*12] = setButtonColor( m_F5, m_bFx[4+m_iFnGroup*12] );
      funCmd(1);
      break;
    case WXK_F6:
      m_bFx[5+m_iFnGroup*12] = setButtonColor( m_F6, m_bFx[5+m_iFnGroup*12] );
      funCmd(1);
      break;
    case WXK_F7:
      m_bFx[6+m_iFnGroup*12] = setButtonColor( m_F7, m_bFx[6+m_iFnGroup*12] );
      funCmd(1);
      break;
    case WXK_F8:
      m_bFx[7+m_iFnGroup*12] = setButtonColor( m_F8, m_bFx[7+m_iFnGroup*12] );
      funCmd(1);
      break;
    case WXK_F9:
      m_bFx[8+m_iFnGroup*12] = setButtonColor( m_F9, m_bFx[8+m_iFnGroup*12] );
      funCmd(2);
      break;
    case WXK_F10:
      m_bFx[9+m_iFnGroup*12] = setButtonColor( m_F10, m_bFx[9+m_iFnGroup*12] );
      funCmd(2);
      break;
    case WXK_F11:
      m_bFx[10+m_iFnGroup*12] = setButtonColor( m_F11, m_bFx[10+m_iFnGroup*12] );
      funCmd(2);
      break;
    case WXK_F12:
      m_bFx[11+m_iFnGroup*12] = setButtonColor( m_F12, m_bFx[11+m_iFnGroup*12] );
      funCmd(2);
      break;
  }
}


/*!
 * wxEVT_KEY_UP event handler for ID_LOCCONTROL
 */

void LocControlDialog::OnKeyUp( wxKeyEvent& event )
{
////@begin wxEVT_KEY_UP event handler for ID_LOCCONTROL in LocControlDialog.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_KEY_UP event handler for ID_LOCCONTROL in LocControlDialog.
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_FG
 */

void LocControlDialog::OnFgClick( wxCommandEvent& event )
{
  m_iFnGroup++;
  if( m_iFnGroup > 1 )
    m_iFnGroup = 0;

  setFLabels();
}

