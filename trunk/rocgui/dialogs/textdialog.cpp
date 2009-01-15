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
#pragma implementation "textdialog.h"
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
#include <wx/colordlg.h>

#include "textdialog.h"

#include "rocrail/wrapper/public/Text.h"
#include "rocrail/wrapper/public/Item.h"
#include "rocrail/wrapper/public/Plan.h"
#include "rocrail/wrapper/public/Block.h"
#include "rocgui/wrapper/public/Gui.h"
#include "rocgui/public/guiapp.h"
#include "rocs/public/system.h"

////@begin XPM images
////@end XPM images

/*!
 * TextDialog type definition
 */

IMPLEMENT_DYNAMIC_CLASS( TextDialog, wxDialog )

/*!
 * TextDialog event table definition
 */

BEGIN_EVENT_TABLE( TextDialog, wxDialog )

////@begin TextDialog event table entries
    EVT_BUTTON( ID_BUTTON_TXT_IMAGE, TextDialog::OnButtonTxtImageClick )

    EVT_BUTTON( ID_BUTTON_TEXT_COLOR, TextDialog::OnButtonTextColorClick )

    EVT_BUTTON( ID_BUTTON_TEXT_BACKGROUND, TextDialog::OnButtonTextBackgroundClick )

    EVT_BUTTON( wxID_OK, TextDialog::OnOkClick )

    EVT_BUTTON( wxID_CANCEL, TextDialog::OnCancelClick )

////@end TextDialog event table entries

END_EVENT_TABLE()

/*!
 * TextDialog constructors
 */

TextDialog::TextDialog( )
{
}

TextDialog::TextDialog( wxWindow* parent, iONode p_Props )
{
  m_TabAlign = wxGetApp().getTabAlign();
  Create(parent, -1, wxGetApp().getMsg("text") );
  m_Props = p_Props;
  initLabels();
  initValues();
  
  m_GeneralPanel->GetSizer()->Layout();
  m_LocationPanel->GetSizer()->Layout();
  m_Notebook->Fit();
  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);
}

void TextDialog::initLabels() {
  m_Notebook->SetPageText( 0, wxGetApp().getMsg( "general" ) );
  m_Notebook->SetPageText( 1, wxGetApp().getMsg( "location" ) );

  // General
  m_LabelID->SetLabel( wxGetApp().getMsg( "id" ) );
  m_LabelText->SetLabel( wxGetApp().getMsg( "text" ) );
  m_LabelTip->SetLabel( wxGetApp().getMsg( "tip" ) );
  m_labPointsize->SetLabel( wxGetApp().getMsg( "pointsize" ) );
  m_labColor->SetLabel( wxGetApp().getMsg( "color" ) );
  m_labBackColor->SetLabel( wxGetApp().getMsg( "background" ) );
  m_AttributesBox->SetLabel( wxGetApp().getMsg( "attributes" ) );
  m_labImage->SetLabel( wxGetApp().getMsg( "image" ) );
  m_Bold->SetLabel(wxGetApp().getMsg( "bold" ) );
  m_Italic->SetLabel(wxGetApp().getMsg( "italic" ) );
  m_Underlined->SetLabel(wxGetApp().getMsg( "underlined" ) );
  m_Transparent->SetLabel(wxGetApp().getMsg( "transparent" ) );
  
  m_Ori->SetLabel(wxGetApp().getMsg( "orientation" ) );
  m_Ori->SetString( 0, wxGetApp().getMsg( "default" ) );
  m_Ori->SetString( 1, wxGetApp().getMsg( "up" ) );
  m_Ori->SetString( 2, wxGetApp().getMsg( "down" ) );

  // Location
  m_LabelX->SetLabel( wxGetApp().getMsg( "x" ) );
  m_LabelY->SetLabel( wxGetApp().getMsg( "y" ) );
  m_LabelZ->SetLabel( wxGetApp().getMsg( "z" ) );
  m_LabelCx->SetLabel( wxGetApp().getMsg( "cx" ) );
  m_LabelCy->SetLabel( wxGetApp().getMsg( "cy" ) );

  // Buttons
  m_OK->SetLabel( wxGetApp().getMsg( "ok" ) );
  m_Cancel->SetLabel( wxGetApp().getMsg( "cancel" ) );
}


void TextDialog::initValues() {
  // General
  m_ID->SetValue( wxString(wText.getid( m_Props ),wxConvUTF8) );
  m_Text->SetValue( wxString(wText.gettext( m_Props ),wxConvUTF8) );
  m_Tip->SetValue( wxString(wText.getdesc( m_Props ),wxConvUTF8) );
  char* str = StrOp.fmt( "%d", wText.getpointsize( m_Props ) );
  m_Pointsize->SetValue( wxString(str,wxConvUTF8) ); StrOp.free( str );

  
  m_Bold->SetValue( wText.isbold( m_Props) );
  m_Underlined->SetValue( wText.isunderlined( m_Props) );
  m_Italic->SetValue( wText.isitalic( m_Props ) );
  m_Transparent->SetValue( wText.istransparent( m_Props ) );
  
  wxColour color( wText.getred(m_Props), wText.getgreen(m_Props), wText.getblue(m_Props) );
  m_Color->SetBackgroundColour(color);
  wxColour bcolor( wText.getbackred(m_Props), wText.getbackgreen(m_Props), wText.getbackblue(m_Props) );
  m_Background->SetBackgroundColour(bcolor);
  
  if( StrOp.equals( wText.getori(m_Props), wItem.north ) )
    m_Ori->SetSelection(1);
  else if( StrOp.equals( wText.getori(m_Props), wItem.south ) )
    m_Ori->SetSelection(2);
  else
    m_Ori->SetSelection(0);

  
  // Location
  str = StrOp.fmt( "%d", wText.getx( m_Props ) );
  m_x->SetValue( wxString(str,wxConvUTF8) ); StrOp.free( str );
  str = StrOp.fmt( "%d", wText.gety( m_Props ) );
  m_y->SetValue( wxString(str,wxConvUTF8) ); StrOp.free( str );
  str = StrOp.fmt( "%d", wText.getz( m_Props ) );
  m_z->SetValue( wxString(str,wxConvUTF8) ); StrOp.free( str );
  str = StrOp.fmt( "%d", wText.getcx( m_Props ) );
  m_Cx->SetValue( wxString(str,wxConvUTF8) ); StrOp.free( str );
  str = StrOp.fmt( "%d", wText.getcy( m_Props ) );
  m_Cy->SetValue( wxString(str,wxConvUTF8) ); StrOp.free( str );

}


void TextDialog::evaluate() {
  if( m_Props == NULL )
    return;
  // General
  wItem.setprev_id( m_Props, wItem.getid(m_Props) );
  wText.setid( m_Props, m_ID->GetValue().mb_str(wxConvUTF8) );
  wText.settext( m_Props, m_Text->GetValue().mb_str(wxConvUTF8) );
  wText.setdesc( m_Props, m_Tip->GetValue().mb_str(wxConvUTF8) );
  wText.setpointsize( m_Props, atoi( m_Pointsize->GetValue().mb_str(wxConvUTF8) ) );

  wText.setbold( m_Props, m_Bold->IsChecked()?True:False );
  wText.setunderlined( m_Props, m_Underlined->IsChecked()?True:False );
  wText.setitalic( m_Props, m_Italic->IsChecked()?True:False );
  wText.settransparent( m_Props, m_Transparent->IsChecked()?True:False );

  if( m_Ori->GetSelection() == 0 )
    wText.setori(m_Props, wItem.west );
  else if( m_Ori->GetSelection() == 1 )
    wText.setori(m_Props, wItem.north );
  else if( m_Ori->GetSelection() == 2 )
    wText.setori(m_Props, wItem.south );
  
  // Location
  wText.setx( m_Props, atoi( m_x->GetValue().mb_str(wxConvUTF8) ) );
  wText.sety( m_Props, atoi( m_y->GetValue().mb_str(wxConvUTF8) ) );
  wText.setz( m_Props, atoi( m_z->GetValue().mb_str(wxConvUTF8) ) );
  wText.setcx( m_Props, atoi( m_Cx->GetValue().mb_str(wxConvUTF8) ) );
  wText.setcy( m_Props, atoi( m_Cy->GetValue().mb_str(wxConvUTF8) ) );
}


/*!
 * TextDialog creator
 */

bool TextDialog::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin TextDialog member initialisation
    m_Notebook = NULL;
    m_GeneralPanel = NULL;
    m_LabelID = NULL;
    m_ID = NULL;
    m_LabelText = NULL;
    m_Text = NULL;
    m_labImage = NULL;
    m_ImageButton = NULL;
    m_LabelTip = NULL;
    m_Tip = NULL;
    m_labPointsize = NULL;
    m_Pointsize = NULL;
    m_labColor = NULL;
    m_Color = NULL;
    m_labBackColor = NULL;
    m_Background = NULL;
    m_AttributesBox = NULL;
    m_Bold = NULL;
    m_Underlined = NULL;
    m_Italic = NULL;
    m_Transparent = NULL;
    m_Ori = NULL;
    m_LocationPanel = NULL;
    m_LabelX = NULL;
    m_x = NULL;
    m_LabelY = NULL;
    m_y = NULL;
    m_LabelZ = NULL;
    m_z = NULL;
    m_LabelCx = NULL;
    m_Cx = NULL;
    m_LabelCy = NULL;
    m_Cy = NULL;
    m_OK = NULL;
    m_Cancel = NULL;
////@end TextDialog member initialisation

////@begin TextDialog creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end TextDialog creation
    return true;
}

/*!
 * Control creation for TextDialog
 */

void TextDialog::CreateControls()
{    
////@begin TextDialog content construction
    TextDialog* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    m_Notebook = new wxNotebook( itemDialog1, ID_NOTEBOOK_TEXT, wxDefaultPosition, wxDefaultSize, m_TabAlign );

    m_GeneralPanel = new wxPanel( m_Notebook, ID_PANEL, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxVERTICAL);
    m_GeneralPanel->SetSizer(itemBoxSizer5);

    wxFlexGridSizer* itemFlexGridSizer6 = new wxFlexGridSizer(2, 2, 0, 0);
    itemFlexGridSizer6->AddGrowableCol(1);
    itemBoxSizer5->Add(itemFlexGridSizer6, 0, wxGROW|wxALL, 5);
    m_LabelID = new wxStaticText( m_GeneralPanel, ID_STATICTEXT_TEXT_ID, _("ID"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(m_LabelID, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    m_ID = new wxTextCtrl( m_GeneralPanel, ID_TEXTCTRL_TEXT_ID, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(m_ID, 1, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5);

    m_LabelText = new wxStaticText( m_GeneralPanel, ID_STATICTEXT_TEXT_TEXT, _("Text"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(m_LabelText, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxADJUST_MINSIZE, 5);

    m_Text = new wxTextCtrl( m_GeneralPanel, ID_TEXTCTRL_TEXT_TEXT, _T(""), wxDefaultPosition, wxSize(200, -1), 0 );
    itemFlexGridSizer6->Add(m_Text, 1, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_labImage = new wxStaticText( m_GeneralPanel, wxID_ANY, _("Image"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(m_labImage, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_ImageButton = new wxButton( m_GeneralPanel, ID_BUTTON_TXT_IMAGE, _("..."), wxDefaultPosition, wxSize(40, 25), 0 );
    itemFlexGridSizer6->Add(m_ImageButton, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_LabelTip = new wxStaticText( m_GeneralPanel, ID_STATICTEXT_TEXT_TIP, _("Tip"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(m_LabelTip, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxADJUST_MINSIZE, 5);

    m_Tip = new wxTextCtrl( m_GeneralPanel, ID_TEXTCTRL_TEXT_TIP, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(m_Tip, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_labPointsize = new wxStaticText( m_GeneralPanel, wxID_STATIC_TX_POINT, _("pointsize"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(m_labPointsize, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxADJUST_MINSIZE, 5);

    m_Pointsize = new wxTextCtrl( m_GeneralPanel, ID_TEXTCTRL_TX_POINT, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer6->Add(m_Pointsize, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    wxFlexGridSizer* itemFlexGridSizer17 = new wxFlexGridSizer(2, 4, 0, 0);
    itemBoxSizer5->Add(itemFlexGridSizer17, 0, wxGROW|wxALL, 5);
    m_labColor = new wxStaticText( m_GeneralPanel, wxID_ANY, _("color"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer17->Add(m_labColor, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Color = new wxButton( m_GeneralPanel, ID_BUTTON_TEXT_COLOR, _("..."), wxDefaultPosition, wxSize(40, 25), 0 );
    itemFlexGridSizer17->Add(m_Color, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labBackColor = new wxStaticText( m_GeneralPanel, wxID_ANY, _("background"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer17->Add(m_labBackColor, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Background = new wxButton( m_GeneralPanel, ID_BUTTON_TEXT_BACKGROUND, _("..."), wxDefaultPosition, wxSize(40, 25), 0 );
    itemFlexGridSizer17->Add(m_Background, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_AttributesBox = new wxStaticBox(m_GeneralPanel, wxID_ANY, _("Attributes"));
    wxStaticBoxSizer* itemStaticBoxSizer22 = new wxStaticBoxSizer(m_AttributesBox, wxVERTICAL);
    itemBoxSizer5->Add(itemStaticBoxSizer22, 0, wxGROW|wxALL, 5);
    m_Bold = new wxCheckBox( m_GeneralPanel, wxID_ANY, _("bold"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Bold->SetValue(false);
    itemStaticBoxSizer22->Add(m_Bold, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT, 5);

    m_Underlined = new wxCheckBox( m_GeneralPanel, wxID_ANY, _("underlined"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Underlined->SetValue(false);
    itemStaticBoxSizer22->Add(m_Underlined, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT, 5);

    m_Italic = new wxCheckBox( m_GeneralPanel, wxID_ANY, _("italic"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Italic->SetValue(false);
    itemStaticBoxSizer22->Add(m_Italic, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT, 5);

    m_Transparent = new wxCheckBox( m_GeneralPanel, wxID_ANY, _("transparent"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Transparent->SetValue(false);
    itemStaticBoxSizer22->Add(m_Transparent, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT, 5);

    wxArrayString m_OriStrings;
    m_OriStrings.Add(_("&default"));
    m_OriStrings.Add(_("&up"));
    m_OriStrings.Add(_("&down"));
    m_Ori = new wxRadioBox( m_GeneralPanel, wxID_ANY, _("Orientation"), wxDefaultPosition, wxDefaultSize, m_OriStrings, 1, wxRA_SPECIFY_ROWS );
    m_Ori->SetSelection(0);
    itemBoxSizer5->Add(m_Ori, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP, 5);

    m_Notebook->AddPage(m_GeneralPanel, _("General"));

    m_LocationPanel = new wxPanel( m_Notebook, ID_PANEL1, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxFlexGridSizer* itemFlexGridSizer29 = new wxFlexGridSizer(2, 2, 0, 0);
    m_LocationPanel->SetSizer(itemFlexGridSizer29);

    m_LabelX = new wxStaticText( m_LocationPanel, ID_STATICTEXT_TEXT_X, _("x"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer29->Add(m_LabelX, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    m_x = new wxTextCtrl( m_LocationPanel, ID_TEXTCTRL_TEXT_X, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer29->Add(m_x, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_LabelY = new wxStaticText( m_LocationPanel, ID_STATICTEXT_TEXT_Y, _("y"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer29->Add(m_LabelY, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    m_y = new wxTextCtrl( m_LocationPanel, ID_TEXTCTRL_TEXT_Y, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer29->Add(m_y, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_LabelZ = new wxStaticText( m_LocationPanel, ID_STATICTEXT_TEXT_Z, _("z"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer29->Add(m_LabelZ, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    m_z = new wxTextCtrl( m_LocationPanel, ID_TEXTCTRL_TEXT_Z, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer29->Add(m_z, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_LabelCx = new wxStaticText( m_LocationPanel, wxID_STATICTEXT_TEXT_CX, _("cx"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer29->Add(m_LabelCx, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    m_Cx = new wxTextCtrl( m_LocationPanel, ID_TEXTCTRL_TEXT_CX, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer29->Add(m_Cx, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_LabelCy = new wxStaticText( m_LocationPanel, wxID_STATICTEXT_TEXT_CY, _("cy"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer29->Add(m_LabelCy, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    m_Cy = new wxTextCtrl( m_LocationPanel, ID_TEXTCTRL_TEXT_CY, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer29->Add(m_Cy, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Notebook->AddPage(m_LocationPanel, _("Location"));

    itemBoxSizer2->Add(m_Notebook, 0, wxGROW|wxALL, 5);

    wxStdDialogButtonSizer* itemStdDialogButtonSizer40 = new wxStdDialogButtonSizer;

    itemBoxSizer2->Add(itemStdDialogButtonSizer40, 0, wxGROW|wxALL, 5);
    m_OK = new wxButton( itemDialog1, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    m_OK->SetDefault();
    itemStdDialogButtonSizer40->AddButton(m_OK);

    m_Cancel = new wxButton( itemDialog1, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer40->AddButton(m_Cancel);

    itemStdDialogButtonSizer40->Realize();

////@end TextDialog content construction
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
 */

void TextDialog::OnOkClick( wxCommandEvent& event )
{
  evaluate();
  EndModal( wxID_OK );
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
 */

void TextDialog::OnCancelClick( wxCommandEvent& event )
{
  EndModal( 0 );
}

/*!
 * Should we show tooltips?
 */

bool TextDialog::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap TextDialog::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin TextDialog bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end TextDialog bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon TextDialog::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin TextDialog icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end TextDialog icon retrieval
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_TXT_IMAGE
 */

void TextDialog::OnButtonTxtImageClick( wxCommandEvent& event )
{
  const char* imagepath = wGui.getimagepath( wxGetApp().getIni() );
  TraceOp.trc( "textdialog", TRCLEVEL_INFO, __LINE__, 9999, "imagepath = [%s]", imagepath );
  wxFileDialog* fdlg = new wxFileDialog(this, _T("Search image"), 
      wxString(imagepath,wxConvUTF8), _T(""), 
      _T("PNG files (*.png)|*.png"), wxOPEN);
  if( fdlg->ShowModal() == wxID_OK ) {
    TraceOp.trc( "textdialog", TRCLEVEL_INFO, __LINE__, 9999, "Loading %s...", (const char*)fdlg->GetPath().mb_str(wxConvUTF8) );
    wxBitmapType bmptype = wxBITMAP_TYPE_XPM;
    if( StrOp.endsWithi( fdlg->GetPath().mb_str(wxConvUTF8), ".gif" ) )
      bmptype = wxBITMAP_TYPE_GIF;
    else if( StrOp.endsWithi( fdlg->GetPath().mb_str(wxConvUTF8), ".png" ) )
      bmptype = wxBITMAP_TYPE_PNG;
    wText.settext( m_Props, FileOp.ripPath(fdlg->GetPath().mb_str(wxConvUTF8)) );
    m_Text->SetValue( wxString(wText.gettext( m_Props ),wxConvUTF8) );
  }
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_TEXT_COLOR
 */

void TextDialog::OnButtonTextColorClick( wxCommandEvent& event )
{
  wxColourDialog* dlg = new wxColourDialog(this);
  if( wxID_OK == dlg->ShowModal() ) {
    wxColour &color = dlg->GetColourData().GetColour();
    
    wText.setred( m_Props, (int)color.Red() );
    wText.setgreen( m_Props, (int)color.Green() );
    wText.setblue( m_Props, (int)color.Blue() );
    
    m_Color->SetBackgroundColour( color );
    
  }
  dlg->Destroy();
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_TEXT_BACKGROUND
 */

void TextDialog::OnButtonTextBackgroundClick( wxCommandEvent& event )
{
  wxColourDialog* dlg = new wxColourDialog(this);
  if( wxID_OK == dlg->ShowModal() ) {
    wxColour &color = dlg->GetColourData().GetColour();
    
    wText.setbackred( m_Props, (int)color.Red() );
    wText.setbackgreen( m_Props, (int)color.Green() );
    wText.setbackblue( m_Props, (int)color.Blue() );
    m_Background->SetBackgroundColour( color );
    
  }
  dlg->Destroy();
}

