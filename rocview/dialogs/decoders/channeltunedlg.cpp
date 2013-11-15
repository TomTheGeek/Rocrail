/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2013 Rob Versluis, Rocrail.net

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

#include "channeltunedlg.h"

ChannelTuneDlg::ChannelTuneDlg( wxWindow* parent, DecoderBase* decoderbase, int channel, int type, int offpos, int onpos, bool servo ):ChannelTuneDlgGen( parent )
{
  m_DecoderBase = decoderbase;
  m_Channel = channel;
  m_Type = type;
  m_RangePreset->SetSelection( servo?0:1 );
  wxCommandEvent cmd;
  onPreset(cmd);
  m_OffPos->SetValue(offpos);
  m_OnPos->SetValue(onpos);
  SetTitle(wxString::Format(wxT("Channel %d fine tuning"), m_Channel));
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
}

void ChannelTuneDlg::onOffPosRelease( wxScrollEvent& event )
{
  m_DecoderBase->setPortValue(m_Channel, m_OffPos->GetValue(), 0);
}

void ChannelTuneDlg::onOnPos( wxScrollEvent& event )
{
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

