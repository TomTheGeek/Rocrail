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

#ifndef __channeltunedlg__
#define __channeltunedlg__

/**
@file
Subclass of ChannelTuneDlgGen, which is generated by wxFormBuilder.
*/

#include "channeltunegen.h"
#include "decoderbase.h"

//// end generated include

/** Implementing ChannelTuneDlgGen */
class ChannelTuneDlg : public ChannelTuneDlgGen
{
  DecoderBase* m_DecoderBase;
  int m_Channel;
  int m_Type;
  void initLabels();

	protected:
		// Handlers for ChannelTuneDlgGen events.
		void onMaxRange( wxSpinEvent& event );
		void onMinRange( wxSpinEvent& event );
		void onPreset( wxCommandEvent& event );
		void onOffPos( wxScrollEvent& event );
		void onOffPosRelease( wxScrollEvent& event );
		void onOnPos( wxScrollEvent& event );
		void onOnPosRelease( wxScrollEvent& event );
		void onOK( wxCommandEvent& event );
	public:
		/** Constructor */
		ChannelTuneDlg( wxWindow* parent, DecoderBase* decoderbase, int channel, int type, int offpos, int onpos, bool servo );
	//// end generated class members
		int getOffPos();
    int getOnPos();
	
};

#endif // __channeltunedlg__