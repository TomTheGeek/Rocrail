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

#include "rocgui/public/guiapp.h"
#include "rocgui/public/base.h"
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

#include "rocgui/wrapper/public/Gui.h"

#include "rocgui/xpm/rocrail.xpm"
#include "rocgui/xpm/rocrail-forwards.xpm"
#include "rocgui/xpm/rocrail-reverse.xpm"

#include "rocgui/xpm/nopict.xpm"


////@begin XPM images
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

    EVT_SLIDER( ID_SLIDER_LOCCTRL_SPEED, LocControlDialog::OnSliderLocctrlSpeedUpdated )
    EVT_COMMAND_SCROLL( ID_SLIDER_LOCCTRL_SPEED, LocControlDialog::OnSliderLocctrlSpeedScroll )

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

  initLabels();

  m_iSpeed = 0;
  m_SpeedCtrl->SetRange( 0, 100 );
  m_SpeedCtrl->SetValue(0);
  initLocMap(locid);

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

  m_bFn = false;
  m_bF1 = false;
  m_bF2 = false;
  m_bF3 = false;
  m_bF4 = false;
  m_bF5 = false;
  m_bF6 = false;
  m_bF7 = false;
  m_bF8 = false;
  m_bF9 = false;
  m_bF10 = false;
  m_bF11 = false;
  m_bF12 = false;
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
  m_Dir->SetBitmapLabel( wxBitmap(rocrail_xpm) );
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

      /* update further functions */
      wLoc.setfx(lc,
          (wFunCmd.isf1 (evt)?0x0001:0x00) |
          (wFunCmd.isf2 (evt)?0x0002:0x00) |
          (wFunCmd.isf3 (evt)?0x0004:0x00) |
          (wFunCmd.isf4 (evt)?0x0008:0x00) |
          (wFunCmd.isf5 (evt)?0x0010:0x00) |
          (wFunCmd.isf6 (evt)?0x0020:0x00) |
          (wFunCmd.isf7 (evt)?0x0040:0x00) |
          (wFunCmd.isf8 (evt)?0x0080:0x00) |
          (wFunCmd.isf9 (evt)?0x0100:0x00) |
          (wFunCmd.isf10(evt)?0x0200:0x00) |
          (wFunCmd.isf11(evt)?0x0400:0x00) |
          (wFunCmd.isf12(evt)?0x0800:0x00)
          );
      initFx(lc);
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

  initFx(lc);

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
  if( lc != NULL ) {
    int fx = wLoc.getfx(lc);
    m_bF1 = setButtonColor( m_F1, (fx & 0x01)?false:true );
    m_bF2 = setButtonColor( m_F2, (fx & 0x02)?false:true );
    m_bF3 = setButtonColor( m_F3, (fx & 0x04)?false:true );
    m_bF4 = setButtonColor( m_F4, (fx & 0x08)?false:true );
    m_bF5 = setButtonColor( m_F5, (fx & 0x10)?false:true );
    m_bF6 = setButtonColor( m_F6, (fx & 0x20)?false:true );
    m_bF7 = setButtonColor( m_F7, (fx & 0x40)?false:true );
    m_bF8 = setButtonColor( m_F8, (fx & 0x80)?false:true );
    m_bF9  = setButtonColor( m_F9 , (fx & 0x0100)?false:true );
    m_bF10 = setButtonColor( m_F10, (fx & 0x0200)?false:true );
    m_bF11 = setButtonColor( m_F11, (fx & 0x0400)?false:true );
    m_bF12 = setButtonColor( m_F12, (fx & 0x0800)?false:true );
  }

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

    m_Image = new wxBitmapButton( itemDialog1, ID_BITMAPBUTTON_LOCCTRL_IMAGE, wxNullBitmap, wxDefaultPosition, wxSize(240, 88), wxBU_AUTODRAW|wxBU_LEFT );
    itemBoxSizer2->Add(m_Image, 0, wxGROW|wxALL, 2);

    wxFlexGridSizer* itemFlexGridSizer4 = new wxFlexGridSizer(2, 2, 0, 0);
    itemFlexGridSizer4->AddGrowableCol(1);
    itemBoxSizer2->Add(itemFlexGridSizer4, 0, wxGROW|wxALL, 0);

    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxVERTICAL);
    itemFlexGridSizer4->Add(itemBoxSizer5, 0, wxGROW|wxGROW|wxADJUST_MINSIZE, 0);

    m_Speed = new wxTextCtrl( itemDialog1, ID_TEXTCTRL_LOCCTRL_SPEED, _("0"), wxDefaultPosition, wxSize(-1, 40), wxTE_READONLY|wxTE_CENTRE );
    itemBoxSizer5->Add(m_Speed, 0, wxGROW|wxALL, 2);

    m_SpeedCtrl = new wxSlider( itemDialog1, ID_SLIDER_LOCCTRL_SPEED, 0, 0, 100, wxDefaultPosition, wxSize(-1, 100), wxSL_VERTICAL|wxSL_INVERSE );
    itemBoxSizer5->Add(m_SpeedCtrl, 1, wxALIGN_CENTER_HORIZONTAL|wxALL|wxADJUST_MINSIZE, 2);

    wxBoxSizer* itemBoxSizer8 = new wxBoxSizer(wxVERTICAL);
    itemFlexGridSizer4->Add(itemBoxSizer8, 0, wxALIGN_CENTER_HORIZONTAL|wxGROW, 0);

    wxFlexGridSizer* itemFlexGridSizer9 = new wxFlexGridSizer(2, 3, 0, 0);
    itemBoxSizer8->Add(itemFlexGridSizer9, 0, wxGROW, 5);

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

    wxFlexGridSizer* itemFlexGridSizer22 = new wxFlexGridSizer(2, 2, 0, 0);
    itemFlexGridSizer22->AddGrowableCol(1);
    itemBoxSizer8->Add(itemFlexGridSizer22, 0, wxGROW, 5);

    m_Fn = new wxButton( itemDialog1, ID_BUTTON_LOCCTRL_FN, _("Fn"), wxDefaultPosition, wxSize(50, -1), 0 );
    itemFlexGridSizer22->Add(m_Fn, 0, wxALIGN_LEFT|wxALIGN_TOP|wxBOTTOM, 2);

    m_Dir = new wxBitmapButton( itemDialog1, ID_BITMAPBUTTON_LOCCTRL_DIR, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
    itemFlexGridSizer22->Add(m_Dir, 1, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxBOTTOM, 2);

    wxArrayString m_LcListStrings;
    m_LcList = new wxComboBox( itemDialog1, ID_COMBOBOX_LOCCTRL_LOC, wxEmptyString, wxDefaultPosition, wxDefaultSize, m_LcListStrings, wxCB_READONLY|wxCB_SORT );
    itemBoxSizer2->Add(m_LcList, 0, wxGROW|wxALL, 2);

    wxBoxSizer* itemBoxSizer26 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer26, 0, wxGROW|wxALL|wxADJUST_MINSIZE, 0);

    m_Cancel = new wxButton( itemDialog1, ID_BUTTON_LOCCTRL_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Cancel->SetDefault();
    itemBoxSizer26->Add(m_Cancel, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 2);

    m_Stop = new wxButton( itemDialog1, ID_BUTTON_LOCCTRL_STOP, _("Stop"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer26->Add(m_Stop, 1, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 2);

    wxBoxSizer* itemBoxSizer29 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer29, 0, wxGROW|wxALL, 0);

    m_Break = new wxButton( itemDialog1, ID_BUTTON_LOCCTRL_BREAK, _("BREAK"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer29->Add(m_Break, 1, wxGROW|wxALL|wxADJUST_MINSIZE, 2);

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
 * wxEVT_COMMAND_SLIDER_UPDATED event handler for ID_SLIDER_LOCCTRL_SPEED
 */

void LocControlDialog::OnSliderLocctrlSpeedUpdated( wxCommandEvent& event )
{
  int val = m_SpeedCtrl->GetValue();
  m_iSpeed = val;
  speedCmd( event.GetEventType() != wxEVT_SCROLL_THUMBTRACK );
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
  wxString id = m_LcList->GetStringSelection();

  iONode lc = (iONode)MapOp.get( m_lcMap, id.mb_str(wxConvUTF8) );
  if(lc != NULL) {
    int fx = 0;
    fx |= m_bF1?0x01:0x00;
    fx |= m_bF2?0x02:0x00;
    fx |= m_bF3?0x04:0x00;
    fx |= m_bF4?0x08:0x00;
    fx |= m_bF5?0x10:0x00;
    fx |= m_bF6?0x20:0x00;
    fx |= m_bF7?0x40:0x00;
    fx |= m_bF8?0x80:0x00;
    fx |= m_bF9 ?0x0100:0x00;
    fx |= m_bF10?0x0200:0x00;
    fx |= m_bF11?0x0400:0x00;
    fx |= m_bF12?0x0800:0x00;
    wLoc.setfx(lc, fx );
  }

  iONode cmd = NodeOp.inst( wFunCmd.name(), NULL, ELEMENT_NODE );
  wFunCmd.setgroup ( cmd, group+1 );
  wFunCmd.setid( cmd, id.mb_str(wxConvUTF8) );
  wFunCmd.setf0( cmd, m_bFn?True:False );
  wFunCmd.setf1( cmd, m_bF1?True:False );
  wFunCmd.setf2( cmd, m_bF2?True:False );
  wFunCmd.setf3( cmd, m_bF3?True:False );
  wFunCmd.setf4( cmd, m_bF4?True:False );
  wFunCmd.setf5( cmd, m_bF5?True:False );
  wFunCmd.setf6( cmd, m_bF6?True:False );
  wFunCmd.setf7( cmd, m_bF7?True:False );
  wFunCmd.setf8( cmd, m_bF8?True:False );
  wFunCmd.setf9( cmd, m_bF9?True:False );
  wFunCmd.setf10( cmd, m_bF10?True:False );
  wFunCmd.setf11( cmd, m_bF11?True:False );
  wFunCmd.setf12( cmd, m_bF12?True:False );
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_LOCCTRL_F1
 */

void LocControlDialog::OnButtonLocctrlF1Click( wxCommandEvent& event )
{
  wxString id = m_LcList->GetStringSelection();

  m_bF1 = setButtonColor( m_F1, m_bF1 );
  funCmd(0);
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_LOCCTRL_F2
 */

void LocControlDialog::OnButtonLocctrlF2Click( wxCommandEvent& event )
{
  wxString id = m_LcList->GetStringSelection();

  m_bF2 = setButtonColor( m_F2, m_bF2 );
  funCmd(0);
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_LOCCTRL_F3
 */

void LocControlDialog::OnButtonLocctrlF3Click( wxCommandEvent& event )
{
  wxString id = m_LcList->GetStringSelection();

  m_bF3 = setButtonColor( m_F3, m_bF3 );
  funCmd(0);
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_LOCCTRL
 */

void LocControlDialog::OnButtonLocctrlF4Click( wxCommandEvent& event )
{
  wxString id = m_LcList->GetStringSelection();

  m_bF4 = setButtonColor( m_F4, m_bF4 );
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

  ListOp.removeObj( m_DlgList, (obj)this );
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
 * wxEVT_SCROLL_* event handler for ID_SLIDER_LOCCTRL_SPEED
 */

void LocControlDialog::OnSliderLocctrlSpeedScroll( wxScrollEvent& event )
{
  int val = m_SpeedCtrl->GetValue();
  if( val == m_iSpeed )
    return;
  m_iSpeed = val;
  //m_Dir->Enable( m_iSpeed==0?true:false);

  speedCmd(true);
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_LOCCTRL_F5
 */

void LocControlDialog::OnButtonLocctrlF5Click( wxCommandEvent& event )
{
  wxString id = m_LcList->GetStringSelection();

  m_bF5 = setButtonColor( m_F5, m_bF5 );
  funCmd(1);
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_LOCCTRL_F6
 */

void LocControlDialog::OnButtonLocctrlF6Click( wxCommandEvent& event )
{
  wxString id = m_LcList->GetStringSelection();

  m_bF6 = setButtonColor( m_F6, m_bF6 );
  funCmd(1);
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_LCCTRL_F7
 */

void LocControlDialog::OnButtonLcctrlF7Click( wxCommandEvent& event )
{
  wxString id = m_LcList->GetStringSelection();

  m_bF7 = setButtonColor( m_F7, m_bF7 );
  funCmd(1);
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_LCCTRL_F8
 */

void LocControlDialog::OnButtonLcctrlF8Click( wxCommandEvent& event )
{
  wxString id = m_LcList->GetStringSelection();

  m_bF8 = setButtonColor( m_F8, m_bF8 );
  funCmd(1);
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_LCCTRL_9
 */

void LocControlDialog::OnButtonLcctrlF12Click( wxCommandEvent& event )
{
  wxString id = m_LcList->GetStringSelection();

  m_bF12 = setButtonColor( m_F12, m_bF12 );
  funCmd(2);
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_LCCTRL_9
 */

void LocControlDialog::OnButtonLcctrl9Click( wxCommandEvent& event )
{
  wxString id = m_LcList->GetStringSelection();

  m_bF9 = setButtonColor( m_F9, m_bF9 );
  funCmd(2);
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_LCCTRL_F10
 */

void LocControlDialog::OnButtonLcctrlF10Click( wxCommandEvent& event )
{
  wxString id = m_LcList->GetStringSelection();

  m_bF10 = setButtonColor( m_F10, m_bF10 );
  funCmd(2);
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON1
 */

void LocControlDialog::OnButtonLcctrlF11Click( wxCommandEvent& event )
{
  wxString id = m_LcList->GetStringSelection();

  m_bF11 = setButtonColor( m_F11, m_bF11 );
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
      m_bF1 = setButtonColor( m_F1, m_bF1 );
      funCmd(0);
      break;
    case WXK_F2:
      m_bF2 = setButtonColor( m_F2, m_bF2 );
      funCmd(0);
      break;
    case WXK_F3:
      m_bF3 = setButtonColor( m_F3, m_bF3 );
      funCmd(0);
      break;
    case WXK_F4:
      m_bF4 = setButtonColor( m_F4, m_bF4 );
      funCmd(0);
      break;
    case WXK_F5:
      m_bF5 = setButtonColor( m_F5, m_bF5 );
      funCmd(1);
      break;
    case WXK_F6:
      m_bF6 = setButtonColor( m_F6, m_bF6 );
      funCmd(1);
      break;
    case WXK_F7:
      m_bF7 = setButtonColor( m_F7, m_bF7 );
      funCmd(1);
      break;
    case WXK_F8:
      m_bF8 = setButtonColor( m_F8, m_bF8 );
      funCmd(1);
      break;
    case WXK_F9:
      m_bF9 = setButtonColor( m_F9, m_bF9 );
      funCmd(2);
      break;
    case WXK_F10:
      m_bF10 = setButtonColor( m_F10, m_bF10 );
      funCmd(2);
      break;
    case WXK_F11:
      m_bF11 = setButtonColor( m_F11, m_bF11 );
      funCmd(2);
      break;
    case WXK_F12:
      m_bF12 = setButtonColor( m_F12, m_bF12 );
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

