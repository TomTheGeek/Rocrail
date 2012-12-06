/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2012 Rob Versluis, Rocrail.net

 Without an official permission commercial use is not permitted.
 Forking this project is not permitted.

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
#pragma implementation "lc.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "rocs/public/trace.h"
#include "rocs/public/system.h"
#include "rocview/public/guiapp.h"
#include "rocview/public/base.h"


#define USENEWLOOK

#include "rocview/public/lc.h"
#include "rocview/public/ledbutton.h"

#include "rocview/wrapper/public/Gui.h"

#include "rocrail/wrapper/public/Program.h"
#include "rocrail/wrapper/public/FunCmd.h"
#include "rocrail/wrapper/public/FunDef.h"
#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/RocRail.h"
#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/DataReq.h"

/*!
 * Programming type definition
 */


/*!
 * Programming constructors
 */

LC::LC( wxPanel* parent ) {
  m_Parent = parent;
  m_LocProps = NULL;
  Create();
  init();
}

void LC::init() {
  m_iFnGroup = 0;
}

void LC::SyncClock( iONode node ) {
  m_Clock->SyncClock( node );
}


void LC::stopTimer() {
  m_Clock->stopTimer();
}


iONode LC::getLocProps() {
  return m_LocProps;
}


void LC::setLocProps( iONode props ) {
  m_LocProps = props;
  TraceOp.trc( "lc", TRCLEVEL_DEBUG, __LINE__, 9999, "setLocProps 0x%08X", props );

  m_iFnGroup = 0;

  if( wxGetApp().getFrame()->isTooltip()) {
    m_F1->SetToolTip( wxGetApp().getMsg( "unused" ) );
    m_F2->SetToolTip( wxGetApp().getMsg( "unused" ) );
    m_F3->SetToolTip( wxGetApp().getMsg( "unused" ) );
    m_F4->SetToolTip( wxGetApp().getMsg( "unused" ) );
  }
  else {
    m_F1->SetToolTip( wxString("",wxConvUTF8) );
    m_F2->SetToolTip( wxString("",wxConvUTF8) );
    m_F3->SetToolTip( wxString("",wxConvUTF8) );
    m_F4->SetToolTip( wxString("",wxConvUTF8) );
  }

  setFLabels();

  if( m_LocProps != NULL ) {

    m_bFn = wLoc.isfn( m_LocProps )?true:false;
    setButtonColor( m_F0, !m_bFn );

    m_Vslider->SetRange( 0, wLoc.getV_max(m_LocProps) );
    m_Vslider->SetValue( wLoc.getV(m_LocProps), true );
    m_iSpeed = wLoc.getV(m_LocProps);
    wxString value;
    value.Printf( _T("%d"), m_iSpeed );
    m_V->SetValue( value );
    m_bDir = wLoc.isdir(m_LocProps)?true:false;
    m_Dir->SetLabel( m_bDir?_T(">>"):_T("<<") );

    if( wxGetApp().getFrame()->isTooltip()) {
      m_Dir->SetToolTip( m_bDir?wxGetApp().getMsg( "forwards" ):wxGetApp().getMsg( "reverse" ) );
    }

  }
}

void LC::speedCmd(bool sendCmd)
{
  wxString value;
  value.Printf( _T("%d"), m_iSpeed );
  m_V->SetValue( value );

  if( !sendCmd || m_LocProps == NULL ) {
    return;
  }

  TraceOp.trc( "lc", TRCLEVEL_INFO, __LINE__, 9999, "speedCmd" );

  iONode cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
  wLoc.setid( cmd, wLoc.getid( m_LocProps ) );
  wLoc.setV( cmd, m_iSpeed );
  wLoc.setfn( cmd, m_bFn?True:False );
  wLoc.setdir( cmd, m_bDir?True:False );
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);
}


void LC::funCmd(int fidx)
{
  if( m_LocProps == NULL )
    return;

  TraceOp.trc( "lc", TRCLEVEL_INFO, __LINE__, 9999, "funCmd" );

  wLoc.setfx(m_LocProps,
      (m_bFx[ 0]?0x0001:0x00) | (m_bFx[ 1]?0x0002:0x00) | (m_bFx[ 2]?0x0004:0x00) | (m_bFx[ 3]?0x0008:0x00) |
      (m_bFx[ 4]?0x0010:0x00) | (m_bFx[ 5]?0x0020:0x00) | (m_bFx[ 6]?0x0040:0x00) | (m_bFx[ 7]?0x0080:0x00) |
      (m_bFx[ 8]?0x0100:0x00) | (m_bFx[ 9]?0x0200:0x00) | (m_bFx[10]?0x0400:0x00) | (m_bFx[11]?0x0800:0x00) |

      (m_bFx[12]?0x00001000:0x00) | (m_bFx[13]?0x00002000:0x00) | (m_bFx[14]?0x00004000:0x00) | (m_bFx[15]?0x00008000:0x00) |
      (m_bFx[16]?0x00010000:0x00) | (m_bFx[17]?0x00020000:0x00) | (m_bFx[18]?0x00040000:0x00) | (m_bFx[19]?0x00080000:0x00) |
      (m_bFx[20]?0x00100000:0x00) | (m_bFx[21]?0x00200000:0x00) | (m_bFx[22]?0x00400000:0x00) | (m_bFx[23]?0x00800000:0x00) |
      (m_bFx[24]?0x01000000:0x00) | (m_bFx[25]?0x02000000:0x00) | (m_bFx[26]?0x04000000:0x00) | (m_bFx[27]?0x08000000:0x00)
      );

  iONode cmd = NodeOp.inst( wFunCmd.name(), NULL, ELEMENT_NODE );
  wFunCmd.setgroup ( cmd, m_iFnGroup + 1 );
  wFunCmd.setfnchanged ( cmd, fidx==-1?0:fidx + 1 );
  wFunCmd.setfncnt ( cmd, wLoc.getfncnt( m_LocProps ) );
  wFunCmd.setid ( cmd, wLoc.getid( m_LocProps ) );
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


#ifdef USENEWLOOK
bool LC::setButtonColor( LEDButton* button, bool state ) {
#else
  bool LC::setButtonColor( wxButton* button, bool state ) {
#endif
#ifdef USENEWLOOK
  button->setLED(!state);
#endif
  if( state ) {
    button->SetBackgroundColour( Base::getGreen() );
    //button->SetForegroundColour( Base::getGreen() );
    wxFont f = button->GetFont();
    f.SetUnderlined(false);
    f.SetWeight(wxFONTWEIGHT_NORMAL);
    button->SetFont(f);
    button->Refresh();
    return false;
  }
  else {
    button->SetBackgroundColour( Base::getRed() );
    //button->SetForegroundColour( Base::getRed() );
    wxFont f = button->GetFont();
    f.SetUnderlined(true);
    f.SetWeight(wxFONTWEIGHT_BOLD);
    button->SetFont(f);
    button->Refresh();
    return true;
  }
}

bool LC::setFX( iONode l_LocProps, iONode node ) {
  int fx = wLoc.getfx(l_LocProps);
  wLoc.setfx(l_LocProps,
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

  return true;
}


bool LC::updateLoc( iONode node ) {
  if( m_LocProps != NULL && node != NULL ) {
    if( StrOp.equals( wLoc.getid( m_LocProps ), wLoc.getid( node ) ) ) {
      TraceOp.trc( "lc", TRCLEVEL_INFO, __LINE__, 9999, "updating %s", wLoc.getid( node ) );

      if( StrOp.equals( wFunCmd.name(), NodeOp.getName( node ) ) ) {
        TraceOp.trc( "lc", TRCLEVEL_INFO, __LINE__, 9999, "function update %s", wLoc.getid( node ) );

        setFX(m_LocProps, node);

        setFLabels();

        if( NodeOp.findAttr(node, "f0") ) {
          m_bFn = wFunCmd.isf0( node )?true:false;
          wLoc.setfn( m_LocProps, m_bFn?True:False );
          setButtonColor( m_F0, !m_bFn );
          TraceOp.trc( "lc", TRCLEVEL_INFO, __LINE__, 9999, "function lights=%d", m_bFn );
        }

      }
      else {
        m_iSpeed = wLoc.getV( node );
        wLoc.setV( m_LocProps, m_iSpeed );
        m_Vslider->SetValue( m_iSpeed );
        wxString value;
        value.Printf( _T("%d"), m_iSpeed );
        m_V->SetValue( value );

        TraceOp.trc( "lc", TRCLEVEL_INFO, __LINE__, 9999, "velocity update %d", m_iSpeed );

        m_bDir = wLoc.isdir( node )?true:false;
        wLoc.setdir( m_LocProps, m_bDir?True:False );
        m_Dir->SetLabel( m_bDir?_T(">>"):_T("<<") );
        if( wxGetApp().getFrame()->isTooltip()) {
          m_Dir->SetToolTip( m_bDir?wxGetApp().getMsg( "forwards" ):wxGetApp().getMsg( "reverse" ) );
        }

        if( NodeOp.findAttr(node, "fn") ) {
          m_bFn = wLoc.isfn( node )?true:false;
          wLoc.setfn( m_LocProps, m_bFn?True:False );
          setButtonColor( m_F0, !m_bFn );
          TraceOp.trc( "lc", TRCLEVEL_INFO, __LINE__, 9999, "velocity lights=%d", m_bFn );
        }
      }
      return true;
    }
  }
  return false;
}


void LC::OnSlider(wxScrollEvent& event)
{
  if ( event.GetEventObject() == m_Vslider ) {
    m_iSpeed = m_Vslider->GetValue();
    speedCmd( event.GetEventType() != wxEVT_SCROLL_THUMBTRACK );
  }
}

wxBitmap* LC::getIcon(const char* icon) {
  wxBitmap* bitmap = NULL;

  wxBitmapType bmptype = wxBITMAP_TYPE_XPM;
  if( StrOp.endsWithi( icon, ".gif" ) )
    bmptype = wxBITMAP_TYPE_GIF;
  else if( StrOp.endsWithi( icon, ".png" ) )
    bmptype = wxBITMAP_TYPE_PNG;

  TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "get icon %s", icon );

  const char* imagepath = wGui.getimagepath(wxGetApp().m_Ini);
  static char pixpath[256];
  StrOp.fmtb( pixpath, "%s%c%s", imagepath, SystemOp.getFileSeparator(), FileOp.ripPath( icon ) );

  if( FileOp.exist(pixpath))
    bitmap = new wxBitmap(wxString(pixpath,wxConvUTF8), bmptype);
  else {
    // request the image from server:
    iONode node = NodeOp.inst( wDataReq.name(), NULL, ELEMENT_NODE );
    wDataReq.setid( node, wLoc.getid(this->m_LocProps) );
    wDataReq.setcmd( node, wDataReq.get );
    wDataReq.settype( node, wDataReq.image );
    wDataReq.setfilename( node, icon );
    wxGetApp().sendToRocrail( node );
  }
  return bitmap;
}

void LC::setFLabels() {
  if( m_iFnGroup == 0 ) {
    m_F1->SetLabel( _T("F1") );
    m_F2->SetLabel( _T("F2") );
    m_F3->SetLabel( _T("F3") );
    m_F4->SetLabel( _T("F4") );
  }
  else if( m_iFnGroup == 1 ) {
    m_F1->SetLabel( _T("F5") );
    m_F2->SetLabel( _T("F6") );
    m_F3->SetLabel( _T("F7") );
    m_F4->SetLabel( _T("F8") );
  }
  else if( m_iFnGroup == 2 ) {
    m_F1->SetLabel( _T("F9") );
    m_F2->SetLabel( _T("F10") );
    m_F3->SetLabel( _T("F11") );
    m_F4->SetLabel( _T("F12") );
  }
  else if( m_iFnGroup == 3 ) {
    m_F1->SetLabel( _T("F13") );
    m_F2->SetLabel( _T("F14") );
    m_F3->SetLabel( _T("F15") );
    m_F4->SetLabel( _T("F16") );
  }
  else if( m_iFnGroup == 4 ) {
    m_F1->SetLabel( _T("F17") );
    m_F2->SetLabel( _T("F18") );
    m_F3->SetLabel( _T("F19") );
    m_F4->SetLabel( _T("F20") );
  }
  else if( m_iFnGroup == 5 ) {
    m_F1->SetLabel( _T("F21") );
    m_F2->SetLabel( _T("F22") );
    m_F3->SetLabel( _T("F23") );
    m_F4->SetLabel( _T("F24") );
  }
  else if( m_iFnGroup == 6 ) {
    m_F1->SetLabel( _T("F25") );
    m_F2->SetLabel( _T("F26") );
    m_F3->SetLabel( _T("F27") );
    m_F4->SetLabel( _T("F28") );
  }

  if( m_LocProps != NULL ) {
    int fx = wLoc.getfx(m_LocProps);
    fx = fx >> (m_iFnGroup * 4 );
    m_bFx[0+m_iFnGroup * 4] = setButtonColor( m_F1, (fx & 0x01)?false:true );
    m_bFx[1+m_iFnGroup * 4] = setButtonColor( m_F2, (fx & 0x02)?false:true );
    m_bFx[2+m_iFnGroup * 4] = setButtonColor( m_F3, (fx & 0x04)?false:true );
    m_bFx[3+m_iFnGroup * 4] = setButtonColor( m_F4, (fx & 0x08)?false:true );

    if( wxGetApp().getFrame()->isTooltip()) {
      m_F0->SetToolTip( wxString::Format(_T("F%d"), 0 ));
      m_F1->SetToolTip( wxString::Format(_T("F%d"), 1 + (m_iFnGroup * 4 ) ));
      m_F2->SetToolTip( wxString::Format(_T("F%d"), 2 + (m_iFnGroup * 4 ) ));
      m_F3->SetToolTip( wxString::Format(_T("F%d"), 3 + (m_iFnGroup * 4 ) ));
      m_F4->SetToolTip( wxString::Format(_T("F%d"), 4 + (m_iFnGroup * 4 ) ));
    }
    else {
      m_F0->SetToolTip( wxString("",wxConvUTF8) );
      m_F1->SetToolTip( wxString("",wxConvUTF8) );
      m_F2->SetToolTip( wxString("",wxConvUTF8) );
      m_F3->SetToolTip( wxString("",wxConvUTF8) );
      m_F4->SetToolTip( wxString("",wxConvUTF8) );
    }

    m_F0->SetIcon(NULL);
    m_F1->SetIcon(NULL);
    m_F2->SetIcon(NULL);
    m_F3->SetIcon(NULL);
    m_F4->SetIcon(NULL);

    iONode fundef = wLoc.getfundef( m_LocProps );
    while( fundef != NULL ) {
      wxString fntxt = wxString(wFunDef.gettext( fundef ),wxConvUTF8);
      if( wFunDef.getfn( fundef ) == 0 ) {
        if( wxGetApp().getFrame()->isTooltip())
          m_F0->SetToolTip( fntxt );
        if( wFunDef.geticon(fundef) != NULL && StrOp.len( wFunDef.geticon(fundef) ) > 0 ) {
          m_F0->SetIcon(getIcon(wFunDef.geticon(fundef)));
        }
        else
          m_F0->SetIcon(NULL);
      }
      else if( wFunDef.getfn( fundef ) == 1 + (m_iFnGroup * 4 )) {
        if( wxGetApp().getFrame()->isTooltip())
          m_F1->SetToolTip( fntxt );
        if( wFunDef.geticon(fundef) != NULL && StrOp.len( wFunDef.geticon(fundef) ) > 0 ) {
          m_F1->SetIcon(getIcon(wFunDef.geticon(fundef)));
        }
        else
          m_F1->SetIcon(NULL);
      }
      else if( wFunDef.getfn( fundef ) == 2 + (m_iFnGroup * 4 ) ) {
        if( wxGetApp().getFrame()->isTooltip())
          m_F2->SetToolTip( fntxt );
        if( wFunDef.geticon(fundef) != NULL && StrOp.len( wFunDef.geticon(fundef) ) > 0 ) {
          m_F2->SetIcon(getIcon(wFunDef.geticon(fundef)));
        }
        else
          m_F2->SetIcon(NULL);
      }
      else if( wFunDef.getfn( fundef ) == 3 + (m_iFnGroup * 4 ) ) {
        if( wxGetApp().getFrame()->isTooltip())
          m_F3->SetToolTip( fntxt );
        if( wFunDef.geticon(fundef) != NULL && StrOp.len( wFunDef.geticon(fundef) ) > 0 ) {
          m_F3->SetIcon(getIcon(wFunDef.geticon(fundef)));
        }
        else
          m_F3->SetIcon(NULL);
      }
      else if( wFunDef.getfn( fundef ) == 4 + (m_iFnGroup * 4 ) ) {
        if( wxGetApp().getFrame()->isTooltip())
          m_F4->SetToolTip( fntxt );
        if( wFunDef.geticon(fundef) != NULL && StrOp.len( wFunDef.geticon(fundef) ) > 0 ) {
          m_F4->SetIcon(getIcon(wFunDef.geticon(fundef)));
        }
        else
          m_F4->SetIcon(NULL);
      }
      fundef = wLoc.nextfundef( m_LocProps, fundef );
    }
  }
}

void LC::OnButton(wxCommandEvent& event)
{
  TraceOp.trc( "lc", TRCLEVEL_DEBUG, __LINE__, 9999, "event for LC [%d]", event.GetId() );

  if ( event.GetEventObject() == m_Stop ) {
    m_iSpeed = 0;
    m_Vslider->SetValue( m_iSpeed, true );
    speedCmd(true);
  }
  else if ( event.GetEventObject() == m_FG ) {
    int maxgroups = 6;
    if( m_LocProps != NULL ) {
      maxgroups = wLoc.getfncnt(m_LocProps);
      maxgroups = maxgroups / 4 + ((maxgroups % 4) > 0 ? 1:0 );
      TraceOp.trc( "lc", TRCLEVEL_INFO, __LINE__, 9999, "max function goups is %d", maxgroups );
      // zero based index for this dialog
      maxgroups--;
    }
    m_iFnGroup++;
    if( m_iFnGroup > maxgroups )
      m_iFnGroup = 0;
    setFLabels();
  }
  else if ( event.GetEventObject() == m_F0 ) {
    m_bFn = setButtonColor( m_F0, m_bFn );
    speedCmd(true);
    funCmd(-1);
  }
  else if ( event.GetEventObject() == m_F1 ) {
    m_bFx[0+m_iFnGroup*4] = setButtonColor( m_F1, m_bFx[0+m_iFnGroup*4] );
    funCmd(0+m_iFnGroup*4);
  }
  else if ( event.GetEventObject() == m_F2 ) {
    m_bFx[1+m_iFnGroup*4] = setButtonColor( m_F2, m_bFx[1+m_iFnGroup*4] );
    funCmd(1+m_iFnGroup*4);
  }
  else if ( event.GetEventObject() == m_F3 ) {
    m_bFx[2+m_iFnGroup*4] = setButtonColor( m_F3, m_bFx[2+m_iFnGroup*4] );
    funCmd(2+m_iFnGroup*4);
  }
  else if ( event.GetEventObject() == m_F4 ) {
    m_bFx[3+m_iFnGroup*4] = setButtonColor( m_F4, m_bFx[3+m_iFnGroup*4] );
    funCmd(3+m_iFnGroup*4);
  }
  else if ( event.GetEventObject() == m_Dir ) {
    m_bDir = ! m_bDir;
    m_Dir->SetLabel( m_bDir?_T(">>"):_T("<<") );
    if( wxGetApp().getFrame()->isTooltip())
      m_Dir->SetToolTip( m_bDir?wxGetApp().getMsg( "forwards" ):wxGetApp().getMsg( "reverse" ) );
    speedCmd(true);
  }
  else if ( event.GetId() == ME_F1 ) {
    m_iFnGroup = 0;
    setFLabels();
    m_bFx[0+m_iFnGroup*4] = setButtonColor( m_F1, m_bFx[0+m_iFnGroup*4] );
    funCmd(0+m_iFnGroup*4);
  }
  else if ( event.GetId() == ME_F2 ) {
    m_iFnGroup = 0;
    setFLabels();
    m_bFx[1+m_iFnGroup*4] = setButtonColor( m_F2, m_bFx[1+m_iFnGroup*4] );
    funCmd(1+m_iFnGroup*4);
  }
  else if ( event.GetId() == ME_F3 ) {
    m_iFnGroup = 0;
    setFLabels();
    m_bFx[2+m_iFnGroup*4] = setButtonColor( m_F3, m_bFx[2+m_iFnGroup*4] );
    funCmd(2+m_iFnGroup*4);
  }
  else if ( event.GetId() == ME_F4 ) {
    m_iFnGroup = 0;
    setFLabels();
    m_bFx[3+m_iFnGroup*4] = setButtonColor( m_F4, m_bFx[3+m_iFnGroup*4] );
    funCmd(3+m_iFnGroup*4);
  }
  else if ( event.GetId() == ME_F5 ) {
    m_iFnGroup = 1;
    setFLabels();
    m_bFx[0+m_iFnGroup*4] = setButtonColor( m_F1, m_bFx[0+m_iFnGroup*4] );
    funCmd(0+m_iFnGroup*4);
  }
  else if ( event.GetId() == ME_F6 ) {
    m_iFnGroup = 1;
    setFLabels();
    m_bFx[1+m_iFnGroup*4] = setButtonColor( m_F2, m_bFx[1+m_iFnGroup*4] );
    funCmd(1+m_iFnGroup*4);
  }
  else if ( event.GetId() == ME_F7 ) {
    m_iFnGroup = 1;
    setFLabels();
    m_bFx[2+m_iFnGroup*4] = setButtonColor( m_F3, m_bFx[2+m_iFnGroup*4] );
    funCmd(2+m_iFnGroup*4);
  }
  else if ( event.GetId() == ME_F8 ) {
    m_iFnGroup = 1;
    setFLabels();
    m_bFx[3+m_iFnGroup*4] = setButtonColor( m_F4, m_bFx[3+m_iFnGroup*4] );
    funCmd(3+m_iFnGroup*4);
  }
  else if ( event.GetId() == ME_F9 ) {
    m_iFnGroup = 2;
    setFLabels();
    m_bFx[0+m_iFnGroup*4] = setButtonColor( m_F1, m_bFx[0+m_iFnGroup*4] );
    funCmd(0+m_iFnGroup*4);
  }
  else if ( event.GetId() == ME_F10 ) {
    m_iFnGroup = 2;
    setFLabels();
    m_bFx[1+m_iFnGroup*4] = setButtonColor( m_F2, m_bFx[1+m_iFnGroup*4] );
    funCmd(1+m_iFnGroup*4);
  }
  else if ( event.GetId() == ME_F11 ) {
    m_iFnGroup = 2;
    setFLabels();
    m_bFx[2+m_iFnGroup*4] = setButtonColor( m_F3, m_bFx[2+m_iFnGroup*4] );
    funCmd(2+m_iFnGroup*4);
  }
  else if ( event.GetId() == ME_F12 ) {
    m_iFnGroup = 2;
    setFLabels();
    m_bFx[3+m_iFnGroup*4] = setButtonColor( m_F4, m_bFx[3+m_iFnGroup*4] );
    funCmd(3+m_iFnGroup*4);
  }

  else if ( event.GetId() == ME_F13 ) {
    m_iFnGroup = 3;
    setFLabels();
    m_bFx[0+m_iFnGroup*4] = setButtonColor( m_F1, m_bFx[0+m_iFnGroup*4] );
    funCmd(0+m_iFnGroup*4);
  }
  else if ( event.GetId() == ME_F14 ) {
    m_iFnGroup = 3;
    setFLabels();
    m_bFx[1+m_iFnGroup*4] = setButtonColor( m_F2, m_bFx[1+m_iFnGroup*4] );
    funCmd(1+m_iFnGroup*4);
  }
  else if ( event.GetId() == ME_F15 ) {
    m_iFnGroup = 3;
    setFLabels();
    m_bFx[2+m_iFnGroup*4] = setButtonColor( m_F3, m_bFx[2+m_iFnGroup*4] );
    funCmd(2+m_iFnGroup*4);
  }
  else if ( event.GetId() == ME_F16 ) {
    m_iFnGroup = 3;
    setFLabels();
    m_bFx[3+m_iFnGroup*4] = setButtonColor( m_F4, m_bFx[3+m_iFnGroup*4] );
    funCmd(3+m_iFnGroup*4);
  }

  else if ( event.GetId() == ME_F17 ) {
    m_iFnGroup = 4;
    setFLabels();
    m_bFx[0+m_iFnGroup*4] = setButtonColor( m_F1, m_bFx[0+m_iFnGroup*4] );
    funCmd(0+m_iFnGroup*4);
  }
  else if ( event.GetId() == ME_F18 ) {
    m_iFnGroup = 4;
    setFLabels();
    m_bFx[1+m_iFnGroup*4] = setButtonColor( m_F2, m_bFx[1+m_iFnGroup*4] );
    funCmd(1+m_iFnGroup*4);
  }
  else if ( event.GetId() == ME_F19 ) {
    m_iFnGroup = 4;
    setFLabels();
    m_bFx[2+m_iFnGroup*4] = setButtonColor( m_F3, m_bFx[2+m_iFnGroup*4] );
    funCmd(2+m_iFnGroup*4);
  }
  else if ( event.GetId() == ME_F20 ) {
    m_iFnGroup = 4;
    setFLabels();
    m_bFx[3+m_iFnGroup*4] = setButtonColor( m_F4, m_bFx[3+m_iFnGroup*4] );
    funCmd(3+m_iFnGroup*4);
  }

  else if ( event.GetId() == ME_F21 ) {
    m_iFnGroup = 5;
    setFLabels();
    m_bFx[0+m_iFnGroup*4] = setButtonColor( m_F1, m_bFx[0+m_iFnGroup*4] );
    funCmd(0+m_iFnGroup*4);
  }
  else if ( event.GetId() == ME_F22 ) {
    m_iFnGroup = 5;
    setFLabels();
    m_bFx[1+m_iFnGroup*4] = setButtonColor( m_F2, m_bFx[1+m_iFnGroup*4] );
    funCmd(1+m_iFnGroup*4);
  }
  else if ( event.GetId() == ME_F23 ) {
    m_iFnGroup = 5;
    setFLabels();
    m_bFx[2+m_iFnGroup*4] = setButtonColor( m_F3, m_bFx[2+m_iFnGroup*4] );
    funCmd(2+m_iFnGroup*4);
  }
  else if ( event.GetId() == ME_F24 ) {
    m_iFnGroup = 5;
    setFLabels();
    m_bFx[3+m_iFnGroup*4] = setButtonColor( m_F4, m_bFx[3+m_iFnGroup*4] );
    funCmd(3+m_iFnGroup*4);
  }

  else if ( event.GetId() == ME_F25 ) {
    m_iFnGroup = 6;
    setFLabels();
    m_bFx[0+m_iFnGroup*4] = setButtonColor( m_F1, m_bFx[0+m_iFnGroup*4] );
    funCmd(0+m_iFnGroup*4);
  }
  else if ( event.GetId() == ME_F26 ) {
    m_iFnGroup = 6;
    setFLabels();
    m_bFx[1+m_iFnGroup*4] = setButtonColor( m_F2, m_bFx[1+m_iFnGroup*4] );
    funCmd(1+m_iFnGroup*4);
  }
  else if ( event.GetId() == ME_F27 ) {
    m_iFnGroup = 6;
    setFLabels();
    m_bFx[2+m_iFnGroup*4] = setButtonColor( m_F3, m_bFx[2+m_iFnGroup*4] );
    funCmd(2+m_iFnGroup*4);
  }
  else if ( event.GetId() == ME_F28 ) {
    m_iFnGroup = 6;
    setFLabels();
    m_bFx[3+m_iFnGroup*4] = setButtonColor( m_F4, m_bFx[3+m_iFnGroup*4] );
    funCmd(3+m_iFnGroup*4);
  }
}


/*!
 * Programming creator
 */

bool LC::Create()
{
    m_MainSizer = NULL;
    m_ButtonSizer = NULL;
    m_V = NULL;
    m_Button1Sizer = NULL;
    m_FG = NULL;
    m_F0 = NULL;
    m_F1 = NULL;
    m_F2 = NULL;
    m_Button2Sizer = NULL;
    m_F3 = NULL;
    m_F4 = NULL;
    m_Dir = NULL;
    m_SliderSizer = NULL;
    m_Vslider = NULL;
    m_Stop = NULL;

    m_bFn = false;
    m_bFx[ 0] = false;
    m_bFx[ 1] = false;
    m_bFx[ 2] = false;
    m_bFx[ 3] = false;
    m_bFx[ 4] = false;
    m_bFx[ 5] = false;
    m_bFx[ 6] = false;
    m_bFx[ 7] = false;
    m_bFx[ 8] = false;
    m_bFx[ 9] = false;
    m_bFx[10] = false;
    m_bFx[11] = false;
    m_bFx[12] = false;
    m_bFx[13] = false;
    m_bFx[14] = false;
    m_bFx[15] = false;
    m_bFx[16] = false;
    m_bFx[17] = false;
    m_bFx[18] = false;
    m_bFx[19] = false;
    m_bFx[20] = false;
    m_bFx[21] = false;
    m_bFx[22] = false;
    m_bFx[23] = false;
    m_bFx[24] = false;
    m_bFx[25] = false;
    m_bFx[26] = false;
    m_bFx[27] = false;
    m_bDir = true;
    m_iSpeed = 0;
    m_iFnGroup = 0;
    CreateControls();

  //m_V->SetBackgroundColour( Base::getBlue() );
  m_F0->SetBackgroundColour( Base::getGreen() );
  m_F1->SetBackgroundColour( Base::getGreen() );
  m_F2->SetBackgroundColour( Base::getGreen() );
  m_F3->SetBackgroundColour( Base::getGreen() );
  m_F4->SetBackgroundColour( Base::getGreen() );

  //m_Stop->SetBackgroundColour( Base::getRed() );
  if( wxGetApp().getFrame()->isTooltip())
    m_Stop->SetLabel( wxGetApp().getMsg( "stop" ) );
  if( wxGetApp().getFrame()->isTooltip())
    m_F0->SetToolTip( wxGetApp().getMsg( "lights" ) );
  if( wxGetApp().getFrame()->isTooltip())
    m_Dir->SetToolTip( wxGetApp().getMsg( "forwards" ) );
  if( wxGetApp().getFrame()->isTooltip())
    m_Stop->SetToolTip( wxGetApp().getTip( "stop" ) );
  if( wxGetApp().getFrame()->isTooltip())
    m_V->SetToolTip( wxGetApp().getMsg( "speed" ) );
  if( wxGetApp().getFrame()->isTooltip())
    m_Vslider->SetToolTip( wxGetApp().getMsg( "speedcontroller" ) );

  if( wxGetApp().getFrame()->isTooltip())
    m_FG->SetToolTip( wxGetApp().getMsg( "functiongroup" ) );

  return true;
}

/*!
 * Control creation for Programming
 */

void LC::CreateControls() {

  m_MainSizer = new wxBoxSizer(wxHORIZONTAL);
  m_Parent->SetSizer(m_MainSizer);


  m_ButtonSizer = new wxBoxSizer(wxVERTICAL);
  m_MainSizer->Add(m_ButtonSizer, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

  wxBoxSizer* l_sizer0 = new wxBoxSizer(wxHORIZONTAL);
  m_ButtonSizer->Add(l_sizer0, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, 0);

#ifdef USENEWLOOK
  m_FG = new LEDButton( m_Parent, _("FG"), 50, 25, false );
#else
  m_FG = new wxButton( m_Parent, -1, _("FG"), wxDefaultPosition, wxSize(50, -1), wxBU_EXACTFIT );
#endif
  l_sizer0->Add(m_FG, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 1);

#ifdef USENEWLOOK
  m_V = new LEDButton( m_Parent, _("0"), 102, 25, false, true );
#else
  m_V = new wxTextCtrl( m_Parent, -1, _("0"), wxDefaultPosition, wxSize(103, 25), wxTE_READONLY|wxTE_CENTRE );
  wxFont f = m_V->GetFont();
  f.SetWeight(wxFONTWEIGHT_BOLD);
  f.SetPointSize(f.GetPointSize()+2);
  m_V->SetFont(f);

#endif
  l_sizer0->Add(m_V, 0, wxALIGN_CENTER_VERTICAL|wxGROW|wxALL, 1);

  m_Button1Sizer = new wxBoxSizer(wxHORIZONTAL);
  m_ButtonSizer->Add(m_Button1Sizer, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, 0);

#ifdef USENEWLOOK
  m_F1 = new LEDButton( m_Parent, _("F1"), 50, 25 );
#else
  m_F1 = new wxButton( m_Parent, -1, _("F1"), wxDefaultPosition, wxSize(50, -1), wxBU_EXACTFIT );
#endif
  m_Button1Sizer->Add(m_F1, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

#ifdef USENEWLOOK
  m_F2 = new LEDButton( m_Parent, _("F2"), 50, 25 );
#else
  m_F2 = new wxButton( m_Parent, -1, _("F2"), wxDefaultPosition, wxSize(50, -1), wxBU_EXACTFIT );
#endif
  m_Button1Sizer->Add(m_F2, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

#ifdef USENEWLOOK
  m_F0 = new LEDButton( m_Parent, _("F0"), 50, 25 );
#else
  m_F0 = new wxButton( m_Parent, -1, _("F0"), wxDefaultPosition, wxSize(50, -1), wxBU_EXACTFIT );
#endif
  m_Button1Sizer->Add(m_F0, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

  m_Button2Sizer = new wxBoxSizer(wxHORIZONTAL);
  m_ButtonSizer->Add(m_Button2Sizer, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 0);

#ifdef USENEWLOOK
  m_F3 = new LEDButton( m_Parent, _("F3"), 50, 25 );
#else
  m_F3 = new wxButton( m_Parent, -1, _("F3"), wxDefaultPosition, wxSize(50, -1), wxBU_EXACTFIT );
#endif
  m_Button2Sizer->Add(m_F3, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

#ifdef USENEWLOOK
  m_F4 = new LEDButton( m_Parent, _("F4"), 50, 25 );
#else
  m_F4 = new wxButton( m_Parent, -1, _("F4"), wxDefaultPosition, wxSize(50, -1), wxBU_EXACTFIT );
#endif
  m_Button2Sizer->Add(m_F4, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

#ifdef USENEWLOOK
  m_Dir = new LEDButton( m_Parent, _(">>"), 50, 25, false );
#else
  m_Dir = new wxButton( m_Parent, -1, _(">>"), wxDefaultPosition, wxSize(50, -1), wxBU_EXACTFIT );
#endif
  m_Button2Sizer->Add(m_Dir, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

  m_SliderSizer = new wxBoxSizer(wxHORIZONTAL);
  m_MainSizer->Add(m_SliderSizer, 0, wxGROW|wxALL, 5);

#ifdef USENEWLOOK
  m_Vslider = new Slider( m_Parent, 35, 106 );
#else
  m_Vslider = new wxSlider( m_Parent, -1, 0, 0, 100, wxDefaultPosition, wxSize(-1, -1), wxSL_VERTICAL|wxSL_INVERSE );
#endif
  m_SliderSizer->Add(m_Vslider, 1, wxGROW|wxALL, 1);

#ifdef USENEWLOOK
  m_Stop = new LEDButton( m_Parent, _(">>"), 154, 25, false );
#else
  m_Stop = new wxButton( m_Parent, -1, _("Stop"), wxDefaultPosition, wxSize(130, -1), 0 );
#endif
  m_ButtonSizer->Add(m_Stop, 0, wxGROW|wxALL, 1);

  const char* clocktype = wGui.getclocktype( wxGetApp().getIni() );
  int type = 0;
  if( StrOp.equals( wGui.clock_ampm, clocktype ) ) type = 1;
  else if( StrOp.equals( wGui.clock_24h, clocktype ) ) type = 2;

  TraceOp.trc( "lc", TRCLEVEL_INFO, __LINE__, 9999, "creating clock...");
  m_Clock = new Clock(m_Parent, -1, 0, 0, 2, 1, type, wGui.isshowsecondhand( wxGetApp().getIni() ));
  m_SliderSizer->Add(m_Clock, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL|wxALL, 2);

}

void LC::showTooltip(bool p_bTooltip) {
  if( !p_bTooltip ) {
    m_F0->SetToolTip( wxString("",wxConvUTF8) );
    m_F1->SetToolTip( wxString("",wxConvUTF8) );
    m_F2->SetToolTip( wxString("",wxConvUTF8) );
    m_F3->SetToolTip( wxString("",wxConvUTF8) );
    m_F4->SetToolTip( wxString("",wxConvUTF8) );
    m_Dir->SetToolTip( wxString("",wxConvUTF8) );
    m_FG->SetToolTip( wxString("",wxConvUTF8) );
    m_Stop->SetToolTip( wxString("",wxConvUTF8) );
    m_Vslider->SetToolTip( wxString("",wxConvUTF8) );
    m_V->SetToolTip( wxString("",wxConvUTF8) );
  }
  else {
    setFLabels();
    m_Stop->SetLabel( wxGetApp().getMsg( "stop" ) );
    m_Dir->SetToolTip( wxGetApp().getMsg( "forwards" ) );
    m_Stop->SetToolTip( wxGetApp().getTip( "stop" ) );
    m_V->SetToolTip( wxGetApp().getMsg( "speed" ) );
    m_Vslider->SetToolTip( wxGetApp().getMsg( "speedcontroller" ) );
    m_FG->SetToolTip( wxGetApp().getMsg( "functiongroup" ) );
  }
}
