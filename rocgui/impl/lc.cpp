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
#include "rocgui/public/guiapp.h"
#include "rocgui/public/base.h"

#include "rocgui/public/lc.h"

#include "rocgui/wrapper/public/Gui.h"

#include "rocrail/wrapper/public/Program.h"
#include "rocrail/wrapper/public/FunCmd.h"
#include "rocrail/wrapper/public/FunDef.h"
#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/RocRail.h"
#include "rocrail/wrapper/public/DigInt.h"

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
}

void LC::SyncClock( iONode node ) {
  m_Clock->SyncClock( node );
}


iONode LC::getLocProps() {
  return m_LocProps;
}


void LC::setLocProps( iONode props ) {
  m_LocProps = props;
  TraceOp.trc( "lc", TRCLEVEL_DEBUG, __LINE__, 9999, "setLocProps 0x%08X", props );

  m_iFnGroup = 0;

  m_F1->SetToolTip( wxGetApp().getMsg( "unused" ) );
  m_F2->SetToolTip( wxGetApp().getMsg( "unused" ) );
  m_F3->SetToolTip( wxGetApp().getMsg( "unused" ) );
  m_F4->SetToolTip( wxGetApp().getMsg( "unused" ) );

  setFLabels();

  if( m_LocProps != NULL ) {

    m_bFn = wLoc.isfn( m_LocProps )?true:false;
    setButtonColor( m_F0, !m_bFn );

    m_Vslider->SetRange( 0, wLoc.getV_max(m_LocProps) );
    m_Vslider->SetValue( wLoc.getV(m_LocProps) );
    m_bDir = wLoc.isdir(m_LocProps)?true:false;
    m_Dir->SetLabel( m_bDir?_T(">>"):_T("<<") );
    m_Dir->SetToolTip( m_bDir?wxGetApp().getMsg( "forwards" ):wxGetApp().getMsg( "reverse" ) );

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

  iONode cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
  wLoc.setid( cmd, wLoc.getid( m_LocProps ) );
  wLoc.setV( cmd, m_iSpeed );
  wLoc.setfn( cmd, m_bFn?True:False );
  wLoc.setdir( cmd, m_bDir?True:False );
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);
}


void LC::funCmd()
{
  if( m_LocProps == NULL )
    return;

  wLoc.setfx(m_LocProps,
      (m_bFx[ 0]?0x0001:0x00) | (m_bFx[ 1]?0x0002:0x00) | (m_bFx[ 2]?0x0004:0x00) | (m_bFx[ 3]?0x0008:0x00) |
      (m_bFx[ 4]?0x0010:0x00) | (m_bFx[ 5]?0x0020:0x00) | (m_bFx[ 6]?0x0040:0x00) | (m_bFx[ 7]?0x0080:0x00) |
      (m_bFx[ 8]?0x0100:0x00) | (m_bFx[ 9]?0x0200:0x00) | (m_bFx[10]?0x0400:0x00) | (m_bFx[11]?0x0800:0x00)
      );

  iONode cmd = NodeOp.inst( wFunCmd.name(), NULL, ELEMENT_NODE );
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
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);
}


bool LC::setButtonColor( wxButton* button, bool state ) {
  if( state ) {
    button->SetBackgroundColour( Base::getGreen() );
    return false;
  }
  else {
    button->SetBackgroundColour( Base::getRed() );
    return true;
  }
}

void LC::updateLoc( iONode node ) {
  if( m_LocProps != NULL && node != NULL ) {
    if( StrOp.equals( wLoc.getid( m_LocProps ), wLoc.getid( node ) ) ) {
      TraceOp.trc( "lc", TRCLEVEL_INFO, __LINE__, 9999, "updating %s", wLoc.getid( node ) );

      if( StrOp.equals( wFunCmd.name(), NodeOp.getName( node ) ) ) {
        TraceOp.trc( "lc", TRCLEVEL_INFO, __LINE__, 9999, "function update %s", wLoc.getid( node ) );

        wLoc.setfx(m_LocProps,
            (wFunCmd.isf1 (node)?0x0001:0x00) |
            (wFunCmd.isf2 (node)?0x0002:0x00) |
            (wFunCmd.isf3 (node)?0x0004:0x00) |
            (wFunCmd.isf4 (node)?0x0008:0x00) |
            (wFunCmd.isf5 (node)?0x0010:0x00) |
            (wFunCmd.isf6 (node)?0x0020:0x00) |
            (wFunCmd.isf7 (node)?0x0040:0x00) |
            (wFunCmd.isf8 (node)?0x0080:0x00) |
            (wFunCmd.isf9 (node)?0x0100:0x00) |
            (wFunCmd.isf10(node)?0x0200:0x00) |
            (wFunCmd.isf11(node)?0x0400:0x00) |
            (wFunCmd.isf12(node)?0x0800:0x00)
            );

        setFLabels();
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
        m_Dir->SetToolTip( m_bDir?wxGetApp().getMsg( "forwards" ):wxGetApp().getMsg( "reverse" ) );

        m_bFn = wLoc.isfn( node )?true:false;
        wLoc.setfn( m_LocProps, m_bFn?True:False );
        setButtonColor( m_F0, !m_bFn );
      }
    }
  }
}


void LC::OnSlider(wxScrollEvent& event)
{
  if ( event.GetEventObject() == m_Vslider ) {
    m_iSpeed = m_Vslider->GetValue();
    speedCmd( event.GetEventType() != wxEVT_SCROLL_THUMBTRACK );
  }
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

  if( m_LocProps != NULL ) {
    int fx = wLoc.getfx(m_LocProps);
    fx = fx >> (m_iFnGroup * 4 );
    m_bFx[0+m_iFnGroup * 4] = setButtonColor( m_F1, (fx & 0x01)?false:true );
    m_bFx[1+m_iFnGroup * 4] = setButtonColor( m_F2, (fx & 0x02)?false:true );
    m_bFx[2+m_iFnGroup * 4] = setButtonColor( m_F3, (fx & 0x04)?false:true );
    m_bFx[3+m_iFnGroup * 4] = setButtonColor( m_F4, (fx & 0x08)?false:true );

    iONode fundef = wLoc.getfundef( m_LocProps );
    while( fundef != NULL ) {
      wxString fntxt = wxString(wFunDef.gettext( fundef ),wxConvUTF8);
      if( wFunDef.getfn( fundef ) == 1 + (m_iFnGroup * 4 )) {
        m_F1->SetToolTip( fntxt );
      }
      else if( wFunDef.getfn( fundef ) == 2 + (m_iFnGroup * 4 ) ) {
        m_F2->SetToolTip( fntxt );
      }
      else if( wFunDef.getfn( fundef ) == 3 + (m_iFnGroup * 4 ) ) {
        m_F3->SetToolTip( fntxt );
      }
      else if( wFunDef.getfn( fundef ) == 4 + (m_iFnGroup * 4 ) ) {
        m_F4->SetToolTip( fntxt );
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
    m_Vslider->SetValue( m_iSpeed );
    speedCmd(true);
  }
  else if ( event.GetEventObject() == m_FG ) {
    m_iFnGroup++;
    if( m_iFnGroup > 2 )
      m_iFnGroup = 0;
    setFLabels();
  }
  else if ( event.GetEventObject() == m_F0 ) {
    m_bFn = setButtonColor( m_F0, m_bFn );
    speedCmd(true);
    funCmd();
  }
  else if ( event.GetEventObject() == m_F1 ) {
    m_bFx[0+m_iFnGroup*4] = setButtonColor( m_F1, m_bFx[0+m_iFnGroup*4] );
    funCmd();
  }
  else if ( event.GetEventObject() == m_F2 ) {
    m_bFx[1+m_iFnGroup*4] = setButtonColor( m_F2, m_bFx[1+m_iFnGroup*4] );
    funCmd();
  }
  else if ( event.GetEventObject() == m_F3 ) {
    m_bFx[2+m_iFnGroup*4] = setButtonColor( m_F3, m_bFx[2+m_iFnGroup*4] );
    funCmd();
  }
  else if ( event.GetEventObject() == m_F4 ) {
    m_bFx[3+m_iFnGroup*4] = setButtonColor( m_F4, m_bFx[3+m_iFnGroup*4] );
    funCmd();
  }
  else if ( event.GetEventObject() == m_Dir ) {
    m_bDir = ! m_bDir;
    m_Dir->SetLabel( m_bDir?_T(">>"):_T("<<") );
    m_Dir->SetToolTip( m_bDir?wxGetApp().getMsg( "forwards" ):wxGetApp().getMsg( "reverse" ) );
    speedCmd(true);
  }
  else if ( event.GetId() == ME_F1 ) {
    m_iFnGroup = 0;
    setFLabels();
    m_bFx[0+m_iFnGroup*4] = setButtonColor( m_F1, m_bFx[0+m_iFnGroup*4] );
    funCmd();
  }
  else if ( event.GetId() == ME_F2 ) {
    m_iFnGroup = 0;
    setFLabels();
    m_bFx[1+m_iFnGroup*4] = setButtonColor( m_F2, m_bFx[1+m_iFnGroup*4] );
    funCmd();
  }
  else if ( event.GetId() == ME_F3 ) {
    m_iFnGroup = 0;
    setFLabels();
    m_bFx[2+m_iFnGroup*4] = setButtonColor( m_F3, m_bFx[2+m_iFnGroup*4] );
    funCmd();
  }
  else if ( event.GetId() == ME_F4 ) {
    m_iFnGroup = 0;
    setFLabels();
    m_bFx[3+m_iFnGroup*4] = setButtonColor( m_F4, m_bFx[3+m_iFnGroup*4] );
    funCmd();
  }
  else if ( event.GetId() == ME_F5 ) {
    m_iFnGroup = 1;
    setFLabels();
    m_bFx[0+m_iFnGroup*4] = setButtonColor( m_F1, m_bFx[0+m_iFnGroup*4] );
    funCmd();
  }
  else if ( event.GetId() == ME_F6 ) {
    m_iFnGroup = 1;
    setFLabels();
    m_bFx[1+m_iFnGroup*4] = setButtonColor( m_F2, m_bFx[1+m_iFnGroup*4] );
    funCmd();
  }
  else if ( event.GetId() == ME_F7 ) {
    m_iFnGroup = 1;
    setFLabels();
    m_bFx[2+m_iFnGroup*4] = setButtonColor( m_F3, m_bFx[2+m_iFnGroup*4] );
    funCmd();
  }
  else if ( event.GetId() == ME_F8 ) {
    m_iFnGroup = 1;
    setFLabels();
    m_bFx[3+m_iFnGroup*4] = setButtonColor( m_F4, m_bFx[3+m_iFnGroup*4] );
    funCmd();
  }
  else if ( event.GetId() == ME_F9 ) {
    m_iFnGroup = 2;
    setFLabels();
    m_bFx[0+m_iFnGroup*4] = setButtonColor( m_F1, m_bFx[0+m_iFnGroup*4] );
    funCmd();
  }
  else if ( event.GetId() == ME_F10 ) {
    m_iFnGroup = 2;
    setFLabels();
    m_bFx[1+m_iFnGroup*4] = setButtonColor( m_F2, m_bFx[1+m_iFnGroup*4] );
    funCmd();
  }
  else if ( event.GetId() == ME_F11 ) {
    m_iFnGroup = 2;
    setFLabels();
    m_bFx[2+m_iFnGroup*4] = setButtonColor( m_F3, m_bFx[2+m_iFnGroup*4] );
    funCmd();
  }
  else if ( event.GetId() == ME_F12 ) {
    m_iFnGroup = 2;
    setFLabels();
    m_bFx[3+m_iFnGroup*4] = setButtonColor( m_F4, m_bFx[3+m_iFnGroup*4] );
    funCmd();
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
    m_bFx[0] = false;
    m_bFx[1] = false;
    m_bFx[2] = false;
    m_bFx[3] = false;
    m_bFx[4] = false;
    m_bFx[5] = false;
    m_bFx[6] = false;
    m_bFx[7] = false;
    m_bFx[8] = false;
    m_bFx[9] = false;
    m_bFx[10] = false;
    m_bFx[11] = false;
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
  m_Stop->SetLabel( wxGetApp().getMsg( "stop" ) );
  m_F0->SetToolTip( wxGetApp().getMsg( "lights" ) );
  m_Dir->SetToolTip( wxGetApp().getMsg( "forwards" ) );
  m_Stop->SetToolTip( wxGetApp().getTip( "stop" ) );
  m_V->SetToolTip( wxGetApp().getMsg( "speed" ) );
  m_Vslider->SetToolTip( wxGetApp().getMsg( "speedcontroller" ) );

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
  m_MainSizer->Add(m_ButtonSizer, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

  wxBoxSizer* l_sizer0 = new wxBoxSizer(wxHORIZONTAL);
  m_ButtonSizer->Add(l_sizer0, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, 0);

  m_FG = new wxButton( m_Parent, -1, _("FG"), wxDefaultPosition, wxSize(40, -1), wxBU_EXACTFIT );
  l_sizer0->Add(m_FG, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 2);

  m_V = new wxTextCtrl( m_Parent, -1, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_READONLY|wxTE_CENTRE );
  l_sizer0->Add(m_V, 0, wxGROW|wxALL, 2);

  m_Button1Sizer = new wxBoxSizer(wxHORIZONTAL);
  m_ButtonSizer->Add(m_Button1Sizer, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 0);

  m_F1 = new wxButton( m_Parent, -1, _("F1"), wxDefaultPosition, wxSize(40, -1), wxBU_EXACTFIT );
  m_Button1Sizer->Add(m_F1, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

  m_F2 = new wxButton( m_Parent, -1, _("F2"), wxDefaultPosition, wxSize(40, -1), wxBU_EXACTFIT );
  m_Button1Sizer->Add(m_F2, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

  m_F0 = new wxButton( m_Parent, -1, _("F0"), wxDefaultPosition, wxSize(40, -1), wxBU_EXACTFIT );
  m_Button1Sizer->Add(m_F0, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

  m_Button2Sizer = new wxBoxSizer(wxHORIZONTAL);
  m_ButtonSizer->Add(m_Button2Sizer, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 0);

  m_F3 = new wxButton( m_Parent, -1, _("F3"), wxDefaultPosition, wxSize(40, -1), wxBU_EXACTFIT );
  m_Button2Sizer->Add(m_F3, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

  m_F4 = new wxButton( m_Parent, -1, _("F4"), wxDefaultPosition, wxSize(40, -1), wxBU_EXACTFIT );
  m_Button2Sizer->Add(m_F4, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

  m_Dir = new wxButton( m_Parent, -1, _(">>"), wxDefaultPosition, wxSize(40, -1), wxBU_EXACTFIT );
  m_Button2Sizer->Add(m_Dir, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);

  m_SliderSizer = new wxBoxSizer(wxHORIZONTAL);
  m_MainSizer->Add(m_SliderSizer, 0, wxGROW|wxALL, 0);

  m_Vslider = new wxSlider( m_Parent, -1, 0, 0, 100, wxDefaultPosition, wxDefaultSize, wxSL_VERTICAL|wxSL_INVERSE );
  m_SliderSizer->Add(m_Vslider, 1, wxGROW|wxALL, 2);

  m_Stop = new wxButton( m_Parent, -1, _("Stop"), wxDefaultPosition, wxSize(40, -1), 0 );
  m_SliderSizer->Add(m_Stop, 0, wxGROW|wxALL, 2);

  const char* clocktype = wGui.getclocktype( wxGetApp().getIni() );
  int type = 0;
  if( StrOp.equals( wGui.clock_ampm, clocktype ) ) type = 1;
  else if( StrOp.equals( wGui.clock_24h, clocktype ) ) type = 2;
  m_Clock = new Clock(m_Parent, -1, 0, 0, 2, 1, type);
  m_SliderSizer->Add(m_Clock, 0, wxGROW|wxALL, 2);

}

