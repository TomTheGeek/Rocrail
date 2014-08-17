/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2014 Rob Versluis, Rocrail.net

 


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

#include "channeltunedlg.h"
#include "rocview/public/guiapp.h"

ChannelTuneDlg::ChannelTuneDlg( wxWindow* parent, DecoderBase* decoderbase, int channel, int type, int offpos, int onpos, bool servo ):ChannelTuneDlgGen( parent )
{
  m_DecoderBase = decoderbase;
  m_Channel = channel;
  m_Type = type;

  initLabels();
  // Resize
  GetSizer()->Layout();
  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);


  m_RangePreset->SetSelection( servo?0:1 );
  wxCommandEvent cmd;
  onPreset(cmd);
  m_OffPos->SetValue(offpos);
  m_OnPos->SetValue(onpos);
  m_OffPosValue->SetValue( wxString::Format(wxT("%d"), offpos));
  m_OnPosValue->SetValue( wxString::Format(wxT("%d"), onpos));

  SetTitle( wxGetApp().getMsg( "channel" ) + wxString::Format(wxT(" %d"), m_Channel));
}

void ChannelTuneDlg::initLabels() {
  m_RangeBox->GetStaticBox()->SetLabel(wxGetApp().getMsg( "range" ));;
  m_RangePreset->SetLabel(wxGetApp().getMsg( "preset" ));
  m_RangePreset->SetString(0, wxGetApp().getMsg( "servo" ));
  m_RangePreset->SetString(1, wxGetApp().getMsg( "max" ));
  m_RangePreset->SetString(2, wxGetApp().getMsg( "manually" ));
  m_labOffPos->SetLabel(wxGetApp().getMsg( "left" ));
  m_labOnPos->SetLabel(wxGetApp().getMsg( "right" ));
}

void ChannelTuneDlg::onMaxRange( wxSpinEvent& event )
{
  onMinRange(event);
}

void ChannelTuneDlg::onMinRange( wxSpinEvent& event )
{
  int min = m_MinRange->GetValue();
  int max = m_MaxRange->GetValue();
  m_OffPos->SetRange(min, max);
  m_OnPos->SetRange(min, max);
}

void ChannelTuneDlg::onPreset( wxCommandEvent& event )
{
  int preset = m_RangePreset->GetSelection();
  if( preset == 0 ) {
    // Servo
    m_OffPos->SetRange(150, 600);
    m_OnPos->SetRange(150, 600);
    m_MaxRange->SetValue(600);
    m_MinRange->SetValue(150);
    m_MaxRange->Enable(false);
    m_MinRange->Enable(false);
  }
  else if( preset == 1 ) {
    // Servo
    m_OffPos->SetRange(0, 4095);
    m_OnPos->SetRange(0, 4095);
    m_MaxRange->SetValue(4095);
    m_MinRange->SetValue(0);
    m_MaxRange->Enable(false);
    m_MinRange->Enable(false);
  }
  else {
    m_MaxRange->Enable(true);
    m_MinRange->Enable(true);
  }
}

void ChannelTuneDlg::onOffPos( wxScrollEvent& event )
{
  m_OffPosValue->SetValue( wxString::Format(wxT("%d"), m_OffPos->GetValue()));
}

void ChannelTuneDlg::onOffPosRelease( wxScrollEvent& event )
{
  m_DecoderBase->setPortValue(m_Channel, m_OffPos->GetValue(), 0);
}

void ChannelTuneDlg::onOnPos( wxScrollEvent& event )
{
  m_OnPosValue->SetValue( wxString::Format(wxT("%d"), m_OnPos->GetValue()));
}

void ChannelTuneDlg::onOnPosRelease( wxScrollEvent& event )
{
  m_DecoderBase->setPortValue(m_Channel, m_OnPos->GetValue(), 1);
}

void ChannelTuneDlg::onOK( wxCommandEvent& event )
{
  EndModal( wxID_OK );
}

int ChannelTuneDlg::getOffPos() {
  return m_OffPos->GetValue();
}


int ChannelTuneDlg::getOnPos() {
  return m_OnPos->GetValue();
}


void ChannelTuneDlg::onHelp( wxCommandEvent& event ) {
  wxGetApp().openLink( "rocnet:rocnetnode-setup" );
}

