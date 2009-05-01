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
/** ------------------------------------------------------------
  * Module: rocgui/symbols
  * Object: renderer
  * ------------------------------------------------------------
  */

#include "renderer.h"
#include "sym.h"

#include "rocgui/public/guiapp.h"

#include "rocrail/wrapper/public/Item.h"
#include "rocrail/wrapper/public/Block.h"
#include "rocrail/wrapper/public/Switch.h"
#include "rocrail/wrapper/public/Signal.h"
#include "rocrail/wrapper/public/Output.h"
#include "rocrail/wrapper/public/Track.h"
#include "rocrail/wrapper/public/Turntable.h"
#include "rocrail/wrapper/public/TTTrack.h"
#include "rocrail/wrapper/public/Feedback.h"
#include "rocrail/wrapper/public/Text.h"
#include "rocrail/wrapper/public/SelTab.h"
#include "rocrail/wrapper/public/SelTabPos.h"
#include "rocrail/wrapper/public/DataReq.h"

#include "rocs/public/system.h"

#include "rocgui/wrapper/public/Gui.h"
#include "rocgui/wrapper/public/PlanPanel.h"

#include "staticsymbols.h"


static double PI25DT = 3.141592653589793238462643;


SymbolRenderer::SymbolRenderer( iONode props, wxWindow* parent, iOMap symmap ) {
  m_Props = props;
  m_Parent = parent;
  m_SymMap = symmap;
  m_bRotateable = true;
  m_bShowID = false;
  m_iOccupied = 0;
  m_Bitmap = NULL;
  m_Scale = 1.0;
  initSym();
  m_Label = StrOp.dup("...");
}


/**
 * determine symbol type and subtype
 */
void SymbolRenderer::initSym() {
  m_SvgSym1  = NULL;
  m_SvgSym2  = NULL;
  m_SvgSym3  = NULL;
  m_SvgSym4  = NULL;
  m_SvgSym5  = NULL;
  m_SvgSym6  = NULL;
  m_SvgSym7  = NULL;
  m_SvgSym8  = NULL;
  m_SvgSym9  = NULL;
  m_SvgSym10 = NULL;
  m_SvgSym11 = NULL;
  m_SvgSym12 = NULL;

  const char* nodeName = NodeOp.getName( m_Props );

  // TRACKS
  if( StrOp.equals( wTrack.name(), nodeName ) ) {
    m_iSymType = symtype::i_track;
    if( StrOp.equals( wTrack.curve, wTrack.gettype( m_Props ) ) ) {
      m_iSymSubType = tracktype::i_curve;
      if( m_SymMap != NULL ) {
        if( wItem.isroad( m_Props ) ) {
          m_SvgSym1 = (svgSymbol*)MapOp.get( m_SymMap, tracktype::road_curve );
          m_SvgSym2 = (svgSymbol*)MapOp.get( m_SymMap, tracktype::road_curve_occ );
        }
        else {
          m_SvgSym1 = (svgSymbol*)MapOp.get( m_SymMap, tracktype::curve );
          m_SvgSym2 = (svgSymbol*)MapOp.get( m_SymMap, tracktype::curve_occ );
          m_SvgSym3 = (svgSymbol*)MapOp.get( m_SymMap, tracktype::curve_route );
        }
      }
    }
    else if( StrOp.equals( wTrack.buffer, wTrack.gettype( m_Props ) ) ) {
      m_iSymSubType = tracktype::i_buffer;
      if( m_SymMap != NULL ) {
        m_SvgSym1 = (svgSymbol*)MapOp.get( m_SymMap, tracktype::buffer );
        m_SvgSym2 = (svgSymbol*)MapOp.get( m_SymMap, tracktype::buffer_occ );
        m_SvgSym3 = (svgSymbol*)MapOp.get( m_SymMap, tracktype::buffer_route );
      }
    }
    else if( StrOp.equals( wTrack.connector, wTrack.gettype( m_Props ) ) ) {
      m_iSymSubType = tracktype::i_connector;
      if( m_SymMap != NULL ) {
        m_SvgSym1 = (svgSymbol*)MapOp.get( m_SymMap, tracktype::connector );
        m_SvgSym2 = (svgSymbol*)MapOp.get( m_SymMap, tracktype::connector_occ );
        m_SvgSym3 = (svgSymbol*)MapOp.get( m_SymMap, tracktype::connector_route );
      }
    }
    else if( StrOp.equals( wTrack.dir, wTrack.gettype( m_Props ) ) ) {
      m_iSymSubType = tracktype::i_dir;
      if( m_SymMap != NULL ) {
        if( wItem.isroad( m_Props ) ) {
          m_SvgSym1 = (svgSymbol*)MapOp.get( m_SymMap, tracktype::road_dir );
          m_SvgSym2 = (svgSymbol*)MapOp.get( m_SymMap, tracktype::road_dir_occ );
        }
        else {
          m_SvgSym1 = (svgSymbol*)MapOp.get( m_SymMap, tracktype::dir );
          m_SvgSym2 = (svgSymbol*)MapOp.get( m_SymMap, tracktype::dir_occ );
          m_SvgSym3 = (svgSymbol*)MapOp.get( m_SymMap, tracktype::dir_route );
        }
      }
    }
    else if( StrOp.equals( wTrack.dirall, wTrack.gettype( m_Props ) ) ) {
      m_iSymSubType = tracktype::i_dirall;
      if( m_SymMap != NULL ) {
        m_SvgSym1 = (svgSymbol*)MapOp.get( m_SymMap, tracktype::dirall );
        m_SvgSym2 = (svgSymbol*)MapOp.get( m_SymMap, tracktype::dirall_occ );
        m_SvgSym3 = (svgSymbol*)MapOp.get( m_SymMap, tracktype::dirall_route );
      }
    }
    else {
      m_iSymSubType = tracktype::i_straight;
      if( m_SymMap != NULL ) {
        if( wItem.isroad( m_Props ) ) {
          m_SvgSym1 = (svgSymbol*)MapOp.get( m_SymMap, tracktype::road_straight );
          m_SvgSym2 = (svgSymbol*)MapOp.get( m_SymMap, tracktype::road_straight_occ );
        }
        else {
          m_SvgSym1 = (svgSymbol*)MapOp.get( m_SymMap, tracktype::straight );
          m_SvgSym2 = (svgSymbol*)MapOp.get( m_SymMap, tracktype::straight_occ );
          m_SvgSym3 = (svgSymbol*)MapOp.get( m_SymMap, tracktype::straight_route );
        }
      }
    }
  }

  // SWITCHES
  else if( StrOp.equals( wSwitch.name(), nodeName ) ) {
    Boolean raster = StrOp.equals( wSwitch.getswtype( m_Props ), wSwitch.swtype_raster );
    m_iSymType = symtype::i_switch;

    if( wSwitch.isvirtual( m_Props ) ) {
      m_iSymSubType = switchtype::i_turnoutleft;
      if( m_SymMap != NULL ) {
        m_SvgSym1 = (svgSymbol*)MapOp.get( m_SymMap, switchtype::vturnout );
        m_SvgSym2 = (svgSymbol*)MapOp.get( m_SymMap, switchtype::vturnout_t );
        m_SvgSym3 = (svgSymbol*)MapOp.get( m_SymMap, switchtype::vturnout_t );
      }
    }
    else if( StrOp.equals( wSwitch.crossing, wSwitch.gettype( m_Props ) ) ) {
      m_iSymSubType = switchtype::i_crossing;
      if( m_SymMap != NULL ) {
        if( wItem.isroad( m_Props ) ) {
          m_SvgSym1 = (svgSymbol*)MapOp.get( m_SymMap, switchtype::road_crossing );
          m_SvgSym2 = (svgSymbol*)MapOp.get( m_SymMap, switchtype::road_crossing_t );
          m_SvgSym3 = (svgSymbol*)MapOp.get( m_SymMap, switchtype::road_crossing_t );
        }
        else {
          m_SvgSym1 = (svgSymbol*)MapOp.get( m_SymMap, wSwitch.isdir(m_Props) ? switchtype::crossingleft : switchtype::crossingright );
          m_SvgSym2 = (svgSymbol*)MapOp.get( m_SymMap, wSwitch.isdir(m_Props) ? switchtype::crossingleft_t : switchtype::crossingright_t );
          m_iSymSubType = wSwitch.isdir(m_Props) ? switchtype::i_crossingleft : switchtype::i_crossingright;
        }
      }
    }
    else if( StrOp.equals( wSwitch.ccrossing, wSwitch.gettype( m_Props ) ) ) {
      m_iSymSubType = switchtype::i_ccrossing;
      if( m_SymMap != NULL ) {
        if( wItem.isroad( m_Props ) ) {
          m_SvgSym1 = (svgSymbol*)MapOp.get( m_SymMap, switchtype::road_ccrossing );
        }
        else {
          m_SvgSym1 = (svgSymbol*)MapOp.get( m_SymMap, switchtype::ccrossing );
          m_SvgSym2 = (svgSymbol*)MapOp.get( m_SymMap, switchtype::ccrossing_occ );
        }
      }
    }
    else if( StrOp.equals( wSwitch.dcrossing, wSwitch.gettype( m_Props ) ) ) {
      m_iSymSubType = wSwitch.isdir(m_Props) ? switchtype::i_dcrossingleft:switchtype::i_dcrossingright;
      if( m_SymMap != NULL ) {
        if( raster ) {
          m_SvgSym1 = (svgSymbol*)MapOp.get( m_SymMap, wSwitch.isdir(m_Props) ? switchtype::dcrossingleft_r:switchtype::dcrossingright_r );
          m_SvgSym2 = (svgSymbol*)MapOp.get( m_SymMap, wSwitch.isdir(m_Props) ? switchtype::dcrossingleft_r_t:switchtype::dcrossingright_r_t );
          m_SvgSym3 = (svgSymbol*)MapOp.get( m_SymMap, wSwitch.isdir(m_Props) ? switchtype::dcrossingleft_r_tl:switchtype::dcrossingright_r_tl );
          m_SvgSym4 = (svgSymbol*)MapOp.get( m_SymMap, wSwitch.isdir(m_Props) ? switchtype::dcrossingleft_r_tr:switchtype::dcrossingright_r_tr );
        }
        else {
          m_SvgSym1 = (svgSymbol*)MapOp.get( m_SymMap, wSwitch.isdir(m_Props) ? switchtype::dcrossingleft:switchtype::dcrossingright );
          m_SvgSym2 = (svgSymbol*)MapOp.get( m_SymMap, wSwitch.isdir(m_Props) ? switchtype::dcrossingleft_t:switchtype::dcrossingright_t );
          m_SvgSym3 = (svgSymbol*)MapOp.get( m_SymMap, wSwitch.isdir(m_Props) ? switchtype::dcrossingleft_tl:switchtype::dcrossingright_tl );
          m_SvgSym4 = (svgSymbol*)MapOp.get( m_SymMap, wSwitch.isdir(m_Props) ? switchtype::dcrossingleft_tr:switchtype::dcrossingright_tr );
        }
      }
    }
    else if( StrOp.equals( wSwitch.decoupler, wSwitch.gettype( m_Props ) ) ) {
      m_iSymSubType = switchtype::i_decoupler;
      if( m_SymMap != NULL ) {
        m_SvgSym1 = (svgSymbol*)MapOp.get( m_SymMap, switchtype::decoupler );
        m_SvgSym2 = (svgSymbol*)MapOp.get( m_SymMap, switchtype::decoupler_occ );
        m_SvgSym3 = (svgSymbol*)MapOp.get( m_SymMap, switchtype::decoupler_on );
        m_SvgSym4 = (svgSymbol*)MapOp.get( m_SymMap, switchtype::decoupler_on_occ );
      }
    }
    else if( StrOp.equals( wSwitch.threeway, wSwitch.gettype( m_Props ) ) ) {
      m_iSymSubType = switchtype::i_threeway;
      if( m_SymMap != NULL ) {
        m_SvgSym1 = (svgSymbol*)MapOp.get( m_SymMap, switchtype::threeway );
        m_SvgSym2 = (svgSymbol*)MapOp.get( m_SymMap, switchtype::threeway_tl );
        m_SvgSym3 = (svgSymbol*)MapOp.get( m_SymMap, switchtype::threeway_tr );
      }
    }
    else if( StrOp.equals( wSwitch.accessory, wSwitch.gettype( m_Props ) ) ) {
      m_iSymSubType = switchtype::i_accessory;
      if( m_SymMap != NULL ) {
        char key[256];
        StrOp.fmtb( key, switchtype::accessory_on, wSwitch.getaccnr( m_Props ) );
        m_SvgSym1 = (svgSymbol*)MapOp.get( m_SymMap, key );
        StrOp.fmtb( key, switchtype::accessory_off, wSwitch.getaccnr( m_Props ) );
        m_SvgSym2 = (svgSymbol*)MapOp.get( m_SymMap, key );
        StrOp.fmtb( key, switchtype::accessory_on_occ, wSwitch.getaccnr( m_Props ) );
        m_SvgSym3 = (svgSymbol*)MapOp.get( m_SymMap, key );
        StrOp.fmtb( key, switchtype::accessory_off_occ, wSwitch.getaccnr( m_Props ) );
        m_SvgSym4 = (svgSymbol*)MapOp.get( m_SymMap, key );
      }
    }
    else if( StrOp.equals( wSwitch.left, wSwitch.gettype( m_Props ) ) ) {
      // process raster flag
      m_iSymSubType = switchtype::i_turnoutleft;
      if( m_SymMap != NULL ) {
        if( raster ) {
          m_SvgSym1 = (svgSymbol*)MapOp.get( m_SymMap, switchtype::turnoutleft_r );
          m_SvgSym2 = (svgSymbol*)MapOp.get( m_SymMap, switchtype::turnoutleft_r_t );
          m_SvgSym3 = (svgSymbol*)MapOp.get( m_SymMap, switchtype::turnoutleft_r_t );
        }
        else {
          if( wItem.isroad( m_Props ) ) {
            m_SvgSym1 = (svgSymbol*)MapOp.get( m_SymMap, switchtype::road_turnoutleft );
            m_SvgSym2 = (svgSymbol*)MapOp.get( m_SymMap, switchtype::road_turnoutleft_t );
            m_SvgSym3 = (svgSymbol*)MapOp.get( m_SymMap, switchtype::road_turnoutleft_t );
          }
          else {
            m_SvgSym1 = (svgSymbol*)MapOp.get( m_SymMap, switchtype::turnoutleft );
            m_SvgSym2 = (svgSymbol*)MapOp.get( m_SymMap, switchtype::turnoutleft_t );
            m_SvgSym3 = (svgSymbol*)MapOp.get( m_SymMap, switchtype::turnoutleft_t );
          }
        }
      }
    }
    else {
      // process raster flag
      m_iSymSubType = switchtype::i_turnoutright;
      if( m_SymMap != NULL ) {
        if( raster ) {
          m_SvgSym1 = (svgSymbol*)MapOp.get( m_SymMap, switchtype::turnoutright_r );
          m_SvgSym2 = (svgSymbol*)MapOp.get( m_SymMap, switchtype::turnoutright_r_t );
          m_SvgSym3 = (svgSymbol*)MapOp.get( m_SymMap, switchtype::turnoutright_r_t );
        }
        else {
          if( wItem.isroad( m_Props ) ) {
            m_SvgSym1 = (svgSymbol*)MapOp.get( m_SymMap, switchtype::road_turnoutright );
            m_SvgSym2 = (svgSymbol*)MapOp.get( m_SymMap, switchtype::road_turnoutright_t );
            m_SvgSym3 = (svgSymbol*)MapOp.get( m_SymMap, switchtype::road_turnoutright_t );
          }
          else {
            m_SvgSym1 = (svgSymbol*)MapOp.get( m_SymMap, switchtype::turnoutright );
            m_SvgSym2 = (svgSymbol*)MapOp.get( m_SymMap, switchtype::turnoutright_t );
            m_SvgSym3 = (svgSymbol*)MapOp.get( m_SymMap, switchtype::turnoutright_t );
          }
        }
      }
    }
  }

  // SIGNALS
  else if( StrOp.equals( wSignal.name(), nodeName ) ) {
    int aspects = wSignal.getaspects( m_Props );
    Boolean dwarf = wSignal.isdwarf( m_Props );
    m_iSymType = symtype::i_signal;
    if( StrOp.equals( wSignal.semaphore, wSignal.gettype( m_Props ) ) ) {
      if( StrOp.equals( wSignal.main, wSignal.getsignal( m_Props ) ) ) {
        m_iSymSubType = signaltype::i_semaphoremain;
        if( m_SymMap != NULL ) {
          if( aspects == 2 ) {
            m_SvgSym1 = (svgSymbol*)MapOp.get( m_SymMap, signaltype::semaphoremain_2_r );
            m_SvgSym2 = (svgSymbol*)MapOp.get( m_SymMap, signaltype::semaphoremain_2_g );
            m_SvgSym4 = (svgSymbol*)MapOp.get( m_SymMap, signaltype::semaphoremain_2_r_occ );
            m_SvgSym5 = (svgSymbol*)MapOp.get( m_SymMap, signaltype::semaphoremain_2_g_occ );
            m_SvgSym9  = (svgSymbol*)MapOp.get( m_SymMap, signaltype::semaphoremain_2_r_route );
            m_SvgSym11 = (svgSymbol*)MapOp.get( m_SymMap, signaltype::semaphoremain_2_g_route );
          }
          else {
            m_SvgSym1 = (svgSymbol*)MapOp.get( m_SymMap, signaltype::semaphoremain_r );
            m_SvgSym2 = (svgSymbol*)MapOp.get( m_SymMap, signaltype::semaphoremain_y );
            m_SvgSym3 = (svgSymbol*)MapOp.get( m_SymMap, signaltype::semaphoremain_g );
            m_SvgSym4 = (svgSymbol*)MapOp.get( m_SymMap, signaltype::semaphoremain_r_occ );
            m_SvgSym5 = (svgSymbol*)MapOp.get( m_SymMap, signaltype::semaphoremain_y_occ );
            m_SvgSym6 = (svgSymbol*)MapOp.get( m_SymMap, signaltype::semaphoremain_g_occ );
            m_SvgSym9  = (svgSymbol*)MapOp.get( m_SymMap, signaltype::semaphoremain_r_route );
            m_SvgSym10 = (svgSymbol*)MapOp.get( m_SymMap, signaltype::semaphoremain_y_route );
            m_SvgSym11 = (svgSymbol*)MapOp.get( m_SymMap, signaltype::semaphoremain_g_route );
          }
        }
      }
      else if( StrOp.equals( wSignal.distant, wSignal.getsignal( m_Props ) ) ) {
        m_iSymSubType = signaltype::i_semaphoredistant;
        if( m_SymMap != NULL ) {
          if( aspects == 2 ) {
            m_SvgSym1 = (svgSymbol*)MapOp.get( m_SymMap, signaltype::semaphoredistant_2_r );
            m_SvgSym2 = (svgSymbol*)MapOp.get( m_SymMap, signaltype::semaphoredistant_2_g );
            m_SvgSym4 = (svgSymbol*)MapOp.get( m_SymMap, signaltype::semaphoredistant_2_r_occ );
            m_SvgSym5 = (svgSymbol*)MapOp.get( m_SymMap, signaltype::semaphoredistant_2_g_occ );
            m_SvgSym9  = (svgSymbol*)MapOp.get( m_SymMap, signaltype::semaphoredistant_2_r_route );
            m_SvgSym11 = (svgSymbol*)MapOp.get( m_SymMap, signaltype::semaphoredistant_2_g_route );
          }
          else {
            m_SvgSym1 = (svgSymbol*)MapOp.get( m_SymMap, signaltype::semaphoredistant_r );
            m_SvgSym2 = (svgSymbol*)MapOp.get( m_SymMap, signaltype::semaphoredistant_y );
            m_SvgSym3 = (svgSymbol*)MapOp.get( m_SymMap, signaltype::semaphoredistant_g );
            m_SvgSym4 = (svgSymbol*)MapOp.get( m_SymMap, signaltype::semaphoredistant_r_occ );
            m_SvgSym5 = (svgSymbol*)MapOp.get( m_SymMap, signaltype::semaphoredistant_y_occ );
            m_SvgSym6 = (svgSymbol*)MapOp.get( m_SymMap, signaltype::semaphoredistant_g_occ );
            m_SvgSym9  = (svgSymbol*)MapOp.get( m_SymMap, signaltype::semaphoredistant_r_route );
            m_SvgSym10 = (svgSymbol*)MapOp.get( m_SymMap, signaltype::semaphoredistant_y_route );
            m_SvgSym11 = (svgSymbol*)MapOp.get( m_SymMap, signaltype::semaphoredistant_g_route );
          }
        }
      }
      else if( StrOp.equals( wSignal.shunting, wSignal.getsignal( m_Props ) ) ) {
        m_iSymSubType = signaltype::i_semaphoreshunting;
        if( m_SymMap != NULL ) {
          m_SvgSym1 = (svgSymbol*)MapOp.get( m_SymMap, signaltype::semaphoreshunting_2_r );
          m_SvgSym2 = (svgSymbol*)MapOp.get( m_SymMap, signaltype::semaphoreshunting_2_w );
          m_SvgSym4 = (svgSymbol*)MapOp.get( m_SymMap, signaltype::semaphoreshunting_2_r_occ );
          m_SvgSym5 = (svgSymbol*)MapOp.get( m_SymMap, signaltype::semaphoreshunting_2_w_occ );
          m_SvgSym9  = (svgSymbol*)MapOp.get( m_SymMap, signaltype::semaphoreshunting_2_r_route );
          m_SvgSym12 = (svgSymbol*)MapOp.get( m_SymMap, signaltype::semaphoreshunting_2_w_route );
        }
      }
    }
    else if( StrOp.equals( wSignal.light, wSignal.gettype( m_Props ) ) ) {
      if( StrOp.equals( wSignal.main, wSignal.getsignal( m_Props ) ) ) {
        m_iSymSubType = signaltype::i_signalmain;
        if( m_SymMap != NULL ) {
          if( aspects == 2 ) {
            m_SvgSym1 = (svgSymbol*)MapOp.get( m_SymMap, signaltype::signalmain_2_r );
            m_SvgSym2 = (svgSymbol*)MapOp.get( m_SymMap, signaltype::signalmain_2_g );
            m_SvgSym4 = (svgSymbol*)MapOp.get( m_SymMap, signaltype::signalmain_2_r_occ );
            m_SvgSym5 = (svgSymbol*)MapOp.get( m_SymMap, signaltype::signalmain_2_g_occ );
            m_SvgSym9  = (svgSymbol*)MapOp.get( m_SymMap, signaltype::signalmain_2_r_route );
            m_SvgSym11 = (svgSymbol*)MapOp.get( m_SymMap, signaltype::signalmain_2_g_route );
          }
          else {
            m_SvgSym1 = (svgSymbol*)MapOp.get( m_SymMap, dwarf?signaltype::signalmain_dwarf_r:signaltype::signalmain_r );
            m_SvgSym2 = (svgSymbol*)MapOp.get( m_SymMap, dwarf?signaltype::signalmain_dwarf_y:signaltype::signalmain_y );
            m_SvgSym3 = (svgSymbol*)MapOp.get( m_SymMap, dwarf?signaltype::signalmain_dwarf_g:signaltype::signalmain_g );
            m_SvgSym7 = (svgSymbol*)MapOp.get( m_SymMap, dwarf?signaltype::signalmain_dwarf_w:signaltype::signalmain_w );
            m_SvgSym4 = (svgSymbol*)MapOp.get( m_SymMap, dwarf?signaltype::signalmain_dwarf_r_occ:signaltype::signalmain_r_occ );
            m_SvgSym5 = (svgSymbol*)MapOp.get( m_SymMap, dwarf?signaltype::signalmain_dwarf_y_occ:signaltype::signalmain_y_occ );
            m_SvgSym6 = (svgSymbol*)MapOp.get( m_SymMap, dwarf?signaltype::signalmain_dwarf_g_occ:signaltype::signalmain_g_occ );
            m_SvgSym8 = (svgSymbol*)MapOp.get( m_SymMap, dwarf?signaltype::signalmain_dwarf_w_occ:signaltype::signalmain_w_occ );
            m_SvgSym9  = (svgSymbol*)MapOp.get( m_SymMap, dwarf?signaltype::signalmain_dwarf_r_occ:signaltype::signalmain_r_route );
            m_SvgSym10 = (svgSymbol*)MapOp.get( m_SymMap, dwarf?signaltype::signalmain_dwarf_y_occ:signaltype::signalmain_y_route );
            m_SvgSym11 = (svgSymbol*)MapOp.get( m_SymMap, dwarf?signaltype::signalmain_dwarf_g_occ:signaltype::signalmain_g_route );
            m_SvgSym12 = (svgSymbol*)MapOp.get( m_SymMap, dwarf?signaltype::signalmain_dwarf_w_occ:signaltype::signalmain_w_route );
          }
        }
      }
      else if( StrOp.equals( wSignal.distant, wSignal.getsignal( m_Props ) ) ) {
        m_iSymSubType = signaltype::i_signaldistant;
        if( m_SymMap != NULL ) {
          if( aspects == 2 ) {
            m_SvgSym1 = (svgSymbol*)MapOp.get( m_SymMap, signaltype::signaldistant_2_r );
            m_SvgSym2 = (svgSymbol*)MapOp.get( m_SymMap, signaltype::signaldistant_2_g );
            m_SvgSym4 = (svgSymbol*)MapOp.get( m_SymMap, signaltype::signaldistant_2_r_occ );
            m_SvgSym5 = (svgSymbol*)MapOp.get( m_SymMap, signaltype::signaldistant_2_g_occ );
            m_SvgSym9  = (svgSymbol*)MapOp.get( m_SymMap, signaltype::signaldistant_2_r_route );
            m_SvgSym11 = (svgSymbol*)MapOp.get( m_SymMap, signaltype::signaldistant_2_g_route );
          }
          else {
            m_SvgSym1 = (svgSymbol*)MapOp.get( m_SymMap, signaltype::signaldistant_r );
            m_SvgSym2 = (svgSymbol*)MapOp.get( m_SymMap, signaltype::signaldistant_y );
            m_SvgSym3 = (svgSymbol*)MapOp.get( m_SymMap, signaltype::signaldistant_g );
            m_SvgSym7 = (svgSymbol*)MapOp.get( m_SymMap, signaltype::signaldistant_w );
            m_SvgSym4 = (svgSymbol*)MapOp.get( m_SymMap, signaltype::signaldistant_r_occ );
            m_SvgSym5 = (svgSymbol*)MapOp.get( m_SymMap, signaltype::signaldistant_y_occ );
            m_SvgSym6 = (svgSymbol*)MapOp.get( m_SymMap, signaltype::signaldistant_g_occ );
            m_SvgSym8 = (svgSymbol*)MapOp.get( m_SymMap, signaltype::signaldistant_w_occ );
            m_SvgSym9  = (svgSymbol*)MapOp.get( m_SymMap, signaltype::signaldistant_r_route );
            m_SvgSym10 = (svgSymbol*)MapOp.get( m_SymMap, signaltype::signaldistant_y_route );
            m_SvgSym11 = (svgSymbol*)MapOp.get( m_SymMap, signaltype::signaldistant_g_route );
            m_SvgSym12 = (svgSymbol*)MapOp.get( m_SymMap, signaltype::signaldistant_w_route );
          }
        }
      }
      else if( StrOp.equals( wSignal.shunting, wSignal.getsignal( m_Props ) ) ) {
        m_iSymSubType = signaltype::i_signalshunting;
        if( m_SymMap != NULL ) {
          m_SvgSym1 = (svgSymbol*)MapOp.get( m_SymMap, signaltype::signalshunting_2_r );
          m_SvgSym2 = (svgSymbol*)MapOp.get( m_SymMap, signaltype::signalshunting_2_w );
          m_SvgSym4 = (svgSymbol*)MapOp.get( m_SymMap, signaltype::signalshunting_2_r_occ );
          m_SvgSym5 = (svgSymbol*)MapOp.get( m_SymMap, signaltype::signalshunting_2_w_occ );
          m_SvgSym9  = (svgSymbol*)MapOp.get( m_SymMap, signaltype::signalshunting_2_r_route );
          m_SvgSym12 = (svgSymbol*)MapOp.get( m_SymMap, signaltype::signalshunting_2_w_route );
        }
      }
    }
  }
  else if( StrOp.equals( wOutput.name(), nodeName ) ) {
    m_iSymType = symtype::i_output;
    m_iSymSubType = outputtype::i_button;
    if( m_SymMap != NULL ) {
      char key[256];
      StrOp.fmtb( key, outputtype::button_off, wOutput.getsvgtype( m_Props ) );
      m_SvgSym1 = (svgSymbol*)MapOp.get( m_SymMap, key );
      StrOp.fmtb( key, outputtype::button_on, wOutput.getsvgtype( m_Props ) );
      m_SvgSym2 = (svgSymbol*)MapOp.get( m_SymMap, key );
      StrOp.fmtb( key, outputtype::button_active, wOutput.getsvgtype( m_Props ) );
      m_SvgSym3 = (svgSymbol*)MapOp.get( m_SymMap, key );
    }
  }
  else if( StrOp.equals( wBlock.name(), nodeName ) ) {
    m_iSymType = symtype::i_block;
    m_iSymSubType = blocktype::i_block;
    if( m_SymMap != NULL ) {
      if( wItem.isroad( m_Props ) ) {
        m_SvgSym1 = (svgSymbol*)MapOp.get( m_SymMap, blocktype::road_block );
        m_SvgSym2 = (svgSymbol*)MapOp.get( m_SymMap, blocktype::road_block_occ );
        m_SvgSym3 = (svgSymbol*)MapOp.get( m_SymMap, blocktype::road_block_res );
        m_SvgSym4 = (svgSymbol*)MapOp.get( m_SymMap, blocktype::road_block_ent );
      }
      else {
        m_SvgSym1 = (svgSymbol*)MapOp.get( m_SymMap, blocktype::block );
        m_SvgSym2 = (svgSymbol*)MapOp.get( m_SymMap, blocktype::block_occ );
        m_SvgSym3 = (svgSymbol*)MapOp.get( m_SymMap, blocktype::block_res );
        m_SvgSym4 = (svgSymbol*)MapOp.get( m_SymMap, blocktype::block_ent );
        m_SvgSym5 = (svgSymbol*)MapOp.get( m_SymMap, blocktype::block_closed );
        m_SvgSym6 = (svgSymbol*)MapOp.get( m_SymMap, blocktype::block_ghost );
      }
    }
  }
  else if( StrOp.equals( wSelTab.name(), nodeName ) ) {
    m_iSymType = symtype::i_selecttable;
    m_iSymSubType = seltabtype::i_seltab;
    if( m_SymMap != NULL ) {
      m_SvgSym1 = (svgSymbol*)MapOp.get( m_SymMap, seltabtype::seltab );
      m_SvgSym2 = (svgSymbol*)MapOp.get( m_SymMap, seltabtype::seltab_occ );
      m_SvgSym3 = (svgSymbol*)MapOp.get( m_SymMap, seltabtype::seltab_res );
    }
  }
  else if( StrOp.equals( wText.name(), nodeName ) ) {
    m_iSymType = symtype::i_text;
    m_iSymSubType = texttype::i_text;
  }
  else if( StrOp.equals( wFeedback.name(), nodeName ) ) {
    m_iSymType = symtype::i_feedback;
    m_iSymSubType = feedbacktype::i_sensor;
    if( m_SymMap != NULL ) {
      if( wItem.isroad( m_Props ) ) {
        m_SvgSym1 = (svgSymbol*)MapOp.get( m_SymMap, feedbacktype::road_sensor_off );
        m_SvgSym2 = (svgSymbol*)MapOp.get( m_SymMap, feedbacktype::road_sensor_on );
        m_SvgSym3 = (svgSymbol*)MapOp.get( m_SymMap, feedbacktype::road_sensor_off_occ );
        m_SvgSym4 = (svgSymbol*)MapOp.get( m_SymMap, feedbacktype::road_sensor_on_occ );
      }
      else {
        if( wFeedback.iscurve(m_Props) ) {
          m_SvgSym1 = (svgSymbol*)MapOp.get( m_SymMap, feedbacktype::curve_sensor_off );
          m_SvgSym2 = (svgSymbol*)MapOp.get( m_SymMap, feedbacktype::curve_sensor_on );
          m_SvgSym3 = (svgSymbol*)MapOp.get( m_SymMap, feedbacktype::curve_sensor_off_occ );
          m_SvgSym4 = (svgSymbol*)MapOp.get( m_SymMap, feedbacktype::curve_sensor_on_occ );
          m_SvgSym5 = (svgSymbol*)MapOp.get( m_SymMap, feedbacktype::curve_sensor_off_route );
          m_SvgSym6 = (svgSymbol*)MapOp.get( m_SymMap, feedbacktype::curve_sensor_on_route );
        }
        else {
          m_SvgSym1 = (svgSymbol*)MapOp.get( m_SymMap, feedbacktype::sensor_off );
          m_SvgSym2 = (svgSymbol*)MapOp.get( m_SymMap, feedbacktype::sensor_on );
          m_SvgSym3 = (svgSymbol*)MapOp.get( m_SymMap, feedbacktype::sensor_off_occ );
          m_SvgSym4 = (svgSymbol*)MapOp.get( m_SymMap, feedbacktype::sensor_on_occ );
          m_SvgSym5 = (svgSymbol*)MapOp.get( m_SymMap, feedbacktype::sensor_off_route );
          m_SvgSym6 = (svgSymbol*)MapOp.get( m_SymMap, feedbacktype::sensor_on_route );
        }
      }
    }
  }
  else if( StrOp.equals( wTurntable.name(), nodeName ) ) {
    m_iSymType = symtype::i_turntable;
    m_iSymSubType = 0;
  }
  else if( StrOp.equals( wSelTab.name(), nodeName ) ) {
    m_iSymType = symtype::i_selecttable;
    m_iSymSubType = 0;
  }

}



void SymbolRenderer::setLabel( const char* label, int occupied ) {
  StrOp.free(m_Label);
  m_Label = StrOp.dup(label);
  m_iOccupied = occupied;
  m_bLabelChanged = true;
}


/**
 * prepare symbol size
 */
void SymbolRenderer::sizeToScale( double symsize, double scale, double bktext, int* cx, int* cy ) {
  *cx = 1;
  *cy = 1;
  const char* ori = wItem.getori(m_Props);
  m_fText = bktext;
  m_Scale = scale;

  if( StrOp.equals( wBlock.name(), NodeOp.getName( m_Props ) ) ) {

    if( StrOp.equals( ori, wItem.north ) || StrOp.equals( ori, wItem.south ) ) {
      // vertical
      *cx = 1;
      *cy = 4;
    }
    else { // horizontal
      *cx = 4;
      *cy = 1;
    }
    /*
      wxFont* font = new wxFont( m_BlockLabel->GetFont() );
      font->SetPointSize( (int)(font->GetPointSize() * scale * bktext ) );
    }
    */
  }
  else if( StrOp.equals( wSelTab.name(), NodeOp.getName( m_Props ) ) ) {

    if( StrOp.equals( ori, wItem.north ) || StrOp.equals( ori, wItem.south ) ) {
      // vertical
      *cx = 1;
      *cy = wSelTab.getnrtracks(m_Props);
    }
    else { // horizontal
      *cx = wSelTab.getnrtracks(m_Props);
      *cy = 1;
    }
    /*
      wxFont* font = new wxFont( m_BlockLabel->GetFont() );
      font->SetPointSize( (int)(font->GetPointSize() * scale * bktext ) );
    }
    */
  }
  else if( StrOp.equals( wSelTab.name(), NodeOp.getName( m_Props ) ) ) {
    *cx = 4;
    *cy = 1;
  }
  else if( StrOp.equals( wTurntable.name(), NodeOp.getName( m_Props ) ) ) {
    *cx = 5;
    *cy = 5;
  }
  else if( StrOp.equals( wSwitch.name(), NodeOp.getName( m_Props ) ) &&
           StrOp.equals( wSwitch.dcrossing, wSwitch.gettype( m_Props ) ) ) {
    *cx = 2;
    *cy = 1;
  }
  else if( StrOp.equals( wText.name(), NodeOp.getName( m_Props ) ) ) {
    if( StrOp.equals( ori, wItem.north ) || StrOp.equals( ori, wItem.south ) ) {
      // vertical
      *cx = wText.getcy( m_Props );
      *cy = wText.getcx( m_Props );
    }
    else { // horizontal
    *cx = wText.getcx( m_Props );
    *cy = wText.getcy( m_Props );
    }
    setLabel( wText.gettext( m_Props ), false );
  }

  // use svg size
  if( m_SvgSym1 != NULL ) {
    if( StrOp.equals( ori, wItem.north ) || StrOp.equals( ori, wItem.south ) ) {
      *cy = m_SvgSym1->width  / 32;
      *cx = m_SvgSym1->height / 32;
    }
    else {
      *cx = m_SvgSym1->width  / 32;
      *cy = m_SvgSym1->height / 32;
    }
  }

  m_cx = *cx;
  m_cy = *cy;
}


wxPen* SymbolRenderer::getPen( const char* stroke ) {
  if( stroke == NULL )
    return new wxPen(_T("black"));
  else
    return new wxPen(wxString(stroke,wxConvUTF8), 1, wxSOLID);
}

wxBrush* SymbolRenderer::getBrush( const char* fill, wxPaintDC& dc ) {
  if( fill == NULL || StrOp.equalsi( "none", fill ) )
    return new wxBrush(dc.GetBrush());
  else
    return new wxBrush(wxString(fill,wxConvUTF8), wxSOLID);
}

void SymbolRenderer::drawSvgSym( wxPaintDC& dc, svgSymbol* svgsym, const char* ori ) {
  const wxBrush& b = dc.GetBrush();

  int xOffset = 0;
  int yOffset = 0;

  if( StrOp.equals( wItem.north, ori ) && m_cy > 1) {
    yOffset = 32 * (m_cy-1);
  }
  else if( StrOp.equals( wItem.east, ori ) && m_cx > 1) {
    xOffset = 32 * (m_cx-1);
  }
  else if( StrOp.equals( wItem.east, ori ) && m_cy > 1) {
    yOffset = 32 * (m_cy-1);
  }
  else if( StrOp.equals( wItem.south, ori ) && m_cx > 1) {
    xOffset = 32 * (m_cx-1);
  }


  int cnt = ListOp.size( svgsym->polyList );
  for( int i = 0; i < cnt; i++ ) {
    svgPoly* svgpoly = (svgPoly*)ListOp.get(svgsym->polyList, i);
    wxPen* pen = getPen(svgpoly->stroke);
    pen->SetWidth(1);
    dc.SetPen(*pen);
    wxBrush* brush = getBrush(svgpoly->fill, dc );
    dc.SetBrush( *brush );
    if( svgpoly->arc ) {
      wxPoint* points = rotateShape( svgpoly->poly, svgpoly->cnt, ori );
      dc.DrawArc( points[0].x, points[0].y, points[1].x, points[1].y, points[2].x, points[2].y );
    }
    else
      dc.DrawPolygon( svgpoly->cnt, rotateShape( svgpoly->poly, svgpoly->cnt, ori ), xOffset, yOffset );
    delete pen;
    delete brush;
  }

  if( svgsym->circleList != NULL ) {
    cnt = ListOp.size( svgsym->circleList );
    for( int i = 0; i < cnt; i++ ) {
      svgCircle* svgcircle = (svgCircle*)ListOp.get(svgsym->circleList, i);
      wxPen* pen = getPen(svgcircle->stroke);
      pen->SetWidth(1);
      dc.SetPen(*pen);
      wxBrush* brush = getBrush(svgcircle->fill, dc );
      dc.SetBrush( *brush );
      wxPoint point = wxPoint(svgcircle->cx, svgcircle->cy);
      wxPoint* points = rotateShape( &point, 1, ori );
      dc.DrawCircle( points[0].x, points[0].y, svgcircle->r );
      delete pen;
      delete brush;
    }
  }

  dc.SetBrush( b );
}


/**
 * Track object
 */
void SymbolRenderer::drawTrack( wxPaintDC& dc, bool fill, bool occupied, bool actroute, const char* ori ) {
  const wxBrush& b = dc.GetBrush();

  // SVG Symbol:
  if( occupied && m_SvgSym2!=NULL ) {
    drawSvgSym(dc, m_SvgSym2, ori);
    dc.SetBrush( b );
    return;
  }
  else if( actroute && m_SvgSym3!=NULL ) {
    drawSvgSym(dc, m_SvgSym3, ori);
    dc.SetBrush( b );
    return;
  }
  else if( actroute && m_SvgSym2!=NULL ) {
    drawSvgSym(dc, m_SvgSym2, ori);
    dc.SetBrush( b );
    return;
  }
  else if( m_SvgSym1!=NULL ) {
    drawSvgSym(dc, m_SvgSym1, ori);
    dc.SetBrush( b );
    return;
  }

  // Static Symbol:
  switch( m_iSymSubType ) {
    case tracktype::i_curve:
      if( fill )
        dc.SetBrush( *wxLIGHT_GREY_BRUSH );
      dc.DrawPolygon( curve_size, rotateShape( tk_curveL, curve_size, ori ) );
      break;

    case tracktype::i_buffer:
      if( fill )
        dc.SetBrush( *wxLIGHT_GREY_BRUSH );
      dc.DrawPolygon( buffer_size, rotateShape( tk_buffer, buffer_size, ori ) );
      break;

    case tracktype::i_connector:
      if( fill )
        dc.SetBrush( *wxLIGHT_GREY_BRUSH );
      dc.DrawPolygon( connector_size, rotateShape( tk_connector, connector_size, ori ) );
      break;

    case tracktype::i_dir:
      dc.DrawPolygon( track_size, rotateShape( tk_straight, track_size, ori ) );
      dc.SetBrush( *wxLIGHT_GREY_BRUSH );
      dc.DrawPolygon( 4, rotateShape( tk_dir_1, 4, ori ) );
      break;

    case tracktype::i_dirall:
      dc.DrawPolygon( track_size, rotateShape( tk_straight, track_size, ori ) );
      dc.SetBrush( *wxLIGHT_GREY_BRUSH );
      dc.DrawPolygon( 4, rotateShape( tk_dir_1, 4, ori ) );
      dc.DrawPolygon( 4, rotateShape( tk_dir_2, 4, ori ) );
      break;

    case tracktype::i_straight:
      if( occupied ) {
        dc.DrawPolygon( track_size, rotateShape( tk_straight, track_size, ori ) );
        dc.SetBrush( *wxBLACK_BRUSH );
        dc.DrawPolygon( track_fill_size, rotateShape( tk_straight_fill, track_fill_size, ori ) );
      }
      else if( fill ) {
        dc.SetBrush( *wxLIGHT_GREY_BRUSH );
        dc.DrawPolygon( track_size, rotateShape( tk_straight, track_size, ori ) );
      }
      else {
        dc.DrawPolygon( track_size, rotateShape( tk_straight, track_size, ori ) );
      }
      break;
  }

  dc.SetBrush( b );
}


void SymbolRenderer::drawCCrossing( wxPaintDC& dc, bool fill, bool occupied, const char* ori ) {
  const char* state = wSwitch.getstate( m_Props );
  Boolean hasUnit = wSwitch.getaddr1( m_Props ) > 0 ? True:False;

  if( wSwitch.getaddr1( m_Props ) > 0 || wSwitch.getport1( m_Props ) > 0 )
    hasUnit = True;

  // SVG Symbol:
  if( m_SvgSym1 != NULL && m_SvgSym2 != NULL ) {
    drawSvgSym(dc, occupied? m_SvgSym2:m_SvgSym1, ori);
  }
  else if( m_SvgSym1 != NULL ) {
    drawSvgSym(dc, m_SvgSym1, ori);
  }
}


/**
 * Crossing Switch object
 */
void SymbolRenderer::drawCrossing( wxPaintDC& dc, bool fill, bool occupied, const char* ori ) {
  const char* state = wSwitch.getstate( m_Props );
  Boolean hasUnit = wSwitch.getaddr1( m_Props ) > 0 ? True:False;

  if( wSwitch.getaddr1( m_Props ) > 0 || wSwitch.getport1( m_Props ) > 0 )
    hasUnit = True;


  // SVG Symbol:
  if( !hasUnit && m_SvgSym1!=NULL ) {
    drawSvgSym(dc, m_SvgSym1, ori);
  }
  else if( hasUnit && m_SvgSym2!=NULL && StrOp.equals( state, wSwitch.turnout ) ) {
    drawSvgSym(dc, m_SvgSym2, ori);
  }
  else if( hasUnit && m_SvgSym1!=NULL ) {
    drawSvgSym(dc, m_SvgSym1, ori);
  }
  else {


    const char* ori2 = ori;
    const char* ori3 = ori;
    if( StrOp.equals( ori, wItem.north ) ) {
      ori2 = wItem.east;
      ori3 = wItem.south;
    }
    else if( StrOp.equals( ori, wItem.south ) ) {
      ori2 = wItem.west;
      ori3 = wItem.north;
    }
    else if( StrOp.equals( ori, wItem.east ) ) {
      ori2 = wItem.south;
      ori3 = wItem.west;
    }
    else if( StrOp.equals( ori, wItem.west ) ) {
      ori2 = wItem.north;
      ori3 = wItem.east;
    }

    dc.DrawPolygon( track_size, rotateShape( tk_straight, track_size, ori ) );
    dc.DrawPolygon( track_size, rotateShape( tk_straight, track_size, ori2 ) );
    if( hasUnit ) {
      dc.DrawPolygon( curve_size, rotateShape( tk_curveR, curve_size, ori ) );
      dc.DrawPolygon( curve_size, rotateShape( tk_curveR, curve_size, ori3 ) );
    }

    if( hasUnit ) {
      if( StrOp.equals( state, wSwitch.straight ) ) {
        const wxBrush& b = dc.GetBrush();
        dc.SetBrush( *wxBLACK_BRUSH );
        dc.DrawPolygon( track_size, rotateShape( tk_straight, track_size, ori ) );
        dc.DrawPolygon( track_size, rotateShape( tk_straight, track_size, ori2 ) );
        dc.SetBrush( b );
      }
      else if( StrOp.equals( state, wSwitch.turnout ) ) {
        const wxBrush& b = dc.GetBrush();
        dc.SetBrush( *wxBLACK_BRUSH );
        dc.DrawPolygon( curve_size, rotateShape( tk_curveR, curve_size, ori ) );
        dc.DrawPolygon( curve_size, rotateShape( tk_curveR, curve_size, ori3 ) );
        dc.SetBrush( b );
      }
    }
  }


  if( m_bShowID ) {
    wxFont* font = new wxFont( dc.GetFont() );
    font->SetPointSize( 7 );
    dc.SetFont(*font);

    if( m_iSymSubType == switchtype::i_crossingright ) {
      if( StrOp.equals( ori, wItem.south ) )
        dc.DrawRotatedText( wxString(wItem.getid(m_Props),wxConvUTF8), 0, 63, 90.0 );
      else if( StrOp.equals( ori, wItem.north ) )
        dc.DrawRotatedText( wxString(wItem.getid(m_Props),wxConvUTF8), 0, 63, 90.0 );
      else if( StrOp.equals( ori, wItem.east ) )
        dc.DrawRotatedText( wxString(wItem.getid(m_Props),wxConvUTF8), 0, 0, 0.0 );
      else
        dc.DrawRotatedText( wxString(wItem.getid(m_Props),wxConvUTF8), 0, 0, 0.0 );
    }
    else {
      if( StrOp.equals( ori, wItem.south ) )
        dc.DrawRotatedText( wxString(wItem.getid(m_Props),wxConvUTF8), 20, 63, 90.0 );
      else if( StrOp.equals( ori, wItem.north ) )
        dc.DrawRotatedText( wxString(wItem.getid(m_Props),wxConvUTF8), 20, 63, 90.0 );
      else if( StrOp.equals( ori, wItem.east ) )
        dc.DrawRotatedText( wxString(wItem.getid(m_Props),wxConvUTF8), 0, 21, 0.0 );
      else
        dc.DrawRotatedText( wxString(wItem.getid(m_Props),wxConvUTF8), 0, 21, 0.0 );
    }
    delete font;
  }

}


/**
 * DoubleCrossing Switch object
 */
void SymbolRenderer::drawDCrossing( wxPaintDC& dc, bool fill, bool occupied, const char* ori ) {
  const char* state = wSwitch.getstate( m_Props );
  Boolean has2Units = ( wSwitch.getaddr2( m_Props ) > 0 || wSwitch.getport2( m_Props ) > 0 )  ? True:False;

  // SVG Symbol:
  if( has2Units ) {
    if( m_SvgSym3!=NULL && StrOp.equals( state, wSwitch.left ) ) {
      drawSvgSym(dc, m_SvgSym3, ori);
    }
    else if( m_SvgSym4!=NULL && StrOp.equals( state, wSwitch.right ) ) {
      drawSvgSym(dc, m_SvgSym4, ori);
    }
    else if( m_SvgSym2!=NULL && StrOp.equals( state, wSwitch.turnout ) ) {
      drawSvgSym(dc, m_SvgSym2, ori);
    }
    else if( m_SvgSym1!=NULL ) {
      drawSvgSym(dc, m_SvgSym1, ori);
    }
  }

  else if( m_SvgSym2!=NULL && StrOp.equals( state, wSwitch.turnout ) ) {
    drawSvgSym(dc, m_SvgSym2, ori);
  }

  else if( m_SvgSym1!=NULL ) {
    drawSvgSym(dc, m_SvgSym1, ori);
  }

  else {
    m_bRotateable = false;
    ori = wItem.west; // not rotateable.
    const char* ori2 = ori;
    const char* ori3 = ori;
    if( StrOp.equals( ori, wItem.north ) ) {
      ori2 = wItem.east;
      ori3 = wItem.south;
    }
    else if( StrOp.equals( ori, wItem.south ) ) {
      ori2 = wItem.west;
      ori3 = wItem.north;
    }
    else if( StrOp.equals( ori, wItem.east ) ) {
      ori2 = wItem.south;
      ori3 = wItem.west;
    }
    else if( StrOp.equals( ori, wItem.west ) ) {
      ori2 = wItem.north;
      ori3 = wItem.east;
    }

    dc.DrawPolygon( track_size, rotateShape( tk_straight, track_size, ori ) );
    dc.DrawPolygon( track_size, rotateShape( tk_straight, track_size, ori ), 32 );

    if( wSwitch.isdir( m_Props ) ) {
      dc.DrawPolygon( curve_size, rotateShape( tk_curveR, curve_size, ori2 ) );
      dc.DrawPolygon( curve_size, rotateShape( tk_curveL, curve_size, ori ), 32 );
    }
    else {
      dc.DrawPolygon( curve_size, rotateShape( tk_curveL, curve_size, ori2 ) );
      dc.DrawPolygon( curve_size, rotateShape( tk_curveR, curve_size, ori3 ), 32 );
    }

    if( has2Units ) {
      if( StrOp.equals( state, wSwitch.straight ) ) {
        const wxBrush& b = dc.GetBrush();
        dc.SetBrush( *wxBLACK_BRUSH );
        dc.DrawPolygon( track_size, rotateShape( tk_straight, track_size, ori ) );
        dc.DrawPolygon( track_size, rotateShape( tk_straight, track_size, ori ), 32 );
        dc.SetBrush( b );
      }
      else if( StrOp.equals( state, wSwitch.turnout ) ) {
        const wxBrush& b = dc.GetBrush();
        dc.SetBrush( *wxBLACK_BRUSH );
        if( wSwitch.isdir( m_Props ) ) {
          dc.DrawPolygon( curve_size, rotateShape( tk_curveR, curve_size, ori2 ) );
          dc.DrawPolygon( curve_size, rotateShape( tk_curveL, curve_size, ori ), 32 );
        }
        else {
          dc.DrawPolygon( curve_size, rotateShape( tk_curveL, curve_size, ori2 ) );
          dc.DrawPolygon( curve_size, rotateShape( tk_curveR, curve_size, ori3 ), 32 );
        }
        dc.SetBrush( b );
      }
      else if( StrOp.equals( state, wSwitch.left ) ) {
        const wxBrush& b = dc.GetBrush();
        dc.SetBrush( *wxBLACK_BRUSH );
        dc.DrawPolygon( track_size, rotateShape( tk_straight, track_size, ori ) );
        if( wSwitch.isdir( m_Props ) ) {
          dc.DrawPolygon( curve_size, rotateShape( tk_curveL, curve_size, ori ), 32 );
        }
        else {
          dc.DrawPolygon( curve_size, rotateShape( tk_curveR, curve_size, ori3 ), 32 );
        }
        dc.SetBrush( b );
      }
      else if( StrOp.equals( state, wSwitch.right ) ) {
        const wxBrush& b = dc.GetBrush();
        dc.SetBrush( *wxBLACK_BRUSH );
        dc.DrawPolygon( track_size, rotateShape( tk_straight, track_size, ori ), 32 );
        if( wSwitch.isdir( m_Props ) ) {
          dc.DrawPolygon( curve_size, rotateShape( tk_curveR, curve_size, ori2 ) );
        }
        else {
          dc.DrawPolygon( curve_size, rotateShape( tk_curveL, curve_size, ori2 ) );
        }
        dc.SetBrush( b );
      }
    }
    else {
      if( StrOp.equals( state, wSwitch.straight ) ) {
        const wxBrush& b = dc.GetBrush();
        dc.SetBrush( *wxBLACK_BRUSH );
        dc.DrawPolygon( track_size, rotateShape( tk_straight, track_size, ori ) );
        dc.DrawPolygon( track_size, rotateShape( tk_straight, track_size, ori ), 32 );
        if( wSwitch.isdir( m_Props ) ) {
          dc.DrawPolygon( curve_size, rotateShape( tk_curveR, curve_size, ori2 ) );
          dc.DrawPolygon( curve_size, rotateShape( tk_curveL, curve_size, ori ), 32 );
        }
        else {
          dc.DrawPolygon( curve_size, rotateShape( tk_curveL, curve_size, ori2 ) );
          dc.DrawPolygon( curve_size, rotateShape( tk_curveR, curve_size, ori3 ), 32 );
        }
        dc.SetBrush( b );
      }
      else if( StrOp.equals( state, wSwitch.turnout ) ) {
        const wxBrush& b = dc.GetBrush();
        dc.SetBrush( *wxBLACK_BRUSH );
        if( wSwitch.isdir( m_Props ) ) {
          dc.DrawPolygon( crossingcurve_size, rotateShape( tk_crossingcurve, crossingcurve_size, ori ) );
          dc.DrawPolygon( crossingcurve_size, rotateShape( tk_crossingcurve, crossingcurve_size, ori3 ), 32 );
        }
        else {
          dc.DrawPolygon( crossingcurve_size, rotateShape( tk_crossingcurveL, crossingcurve_size, ori ) );
          dc.DrawPolygon( crossingcurve_size, rotateShape( tk_crossingcurveL, crossingcurve_size, ori3 ), 32 );
        }
        dc.SetBrush( b );
      }
    }

  }

  if( m_bShowID ) {
    wxFont* font = new wxFont( dc.GetFont() );
    font->SetPointSize( 7 );
    dc.SetFont(*font);

    if( m_iSymSubType == switchtype::i_dcrossingright ) {
      if( StrOp.equals( ori, wItem.south ) )
        dc.DrawRotatedText( wxString(wItem.getid(m_Props),wxConvUTF8), 32, 1, 270.0 );
      else if( StrOp.equals( ori, wItem.north ) )
        dc.DrawRotatedText( wxString(wItem.getid(m_Props),wxConvUTF8), 1, 63, 90.0 );
      else if( StrOp.equals( ori, wItem.east ) )
        dc.DrawRotatedText( wxString(wItem.getid(m_Props),wxConvUTF8), 0, 1, 0.0 );
      else
        dc.DrawRotatedText( wxString(wItem.getid(m_Props),wxConvUTF8), 0, 1, 0.0 );
    }
    else {
      if( StrOp.equals( ori, wItem.south ) )
        dc.DrawRotatedText( wxString(wItem.getid(m_Props),wxConvUTF8), 32, 32, 270.0 );
      else if( StrOp.equals( ori, wItem.north ) )
        dc.DrawRotatedText( wxString(wItem.getid(m_Props),wxConvUTF8), 1, 32, 90.0 );
      else if( StrOp.equals( ori, wItem.east ) )
        dc.DrawRotatedText( wxString(wItem.getid(m_Props),wxConvUTF8), 0, 20, 0.0 );
      else
        dc.DrawRotatedText( wxString(wItem.getid(m_Props),wxConvUTF8), 0, 20, 0.0 );
    }
    delete font;
  }


}


/**
 * Threeway Switch object
 */
void SymbolRenderer::drawThreeway( wxPaintDC& dc, bool fill, bool occupied, const char* ori ) {
  const char* state = wSwitch.getstate( m_Props );

  // SVG Symbol:
  if( m_SvgSym2!=NULL && StrOp.equals( state, wSwitch.left ) ) {
    drawSvgSym(dc, m_SvgSym2, ori);
  }
  else if( m_SvgSym3!=NULL && StrOp.equals( state, wSwitch.right ) ) {
    drawSvgSym(dc, m_SvgSym3, ori);
  }
  else if( m_SvgSym1!=NULL ) {
    drawSvgSym(dc, m_SvgSym1, ori);
  }
  else {
    const char* ori3w = ori;
    if( StrOp.equals( ori, wItem.north ) )
      ori3w = wItem.south;
    else if( StrOp.equals( ori, wItem.south ) )
      ori3w = wItem.north;
    else if( StrOp.equals( ori, wItem.east ) )
      ori3w = wItem.west;
    else if( StrOp.equals( ori, wItem.west ) )
      ori3w = wItem.east;

    if( StrOp.equals( state, wSwitch.left ) ) {
      dc.DrawPolygon( track_size, rotateShape( tk_straight, track_size, ori ) );
      dc.DrawPolygon( curve_size, rotateShape( tk_curveL, curve_size, ori3w ) );
      const wxBrush& b = dc.GetBrush();
      dc.SetBrush( *wxBLACK_BRUSH );
      dc.DrawPolygon( curve_size, rotateShape( tk_curveR, curve_size, ori ) );
      dc.SetBrush( b );
    }
    else if( StrOp.equals( state, wSwitch.right ) ) {
      dc.DrawPolygon( track_size, rotateShape( tk_straight, track_size, ori ) );
      dc.DrawPolygon( curve_size, rotateShape( tk_curveR, curve_size, ori ) );
      const wxBrush& b = dc.GetBrush();
      dc.SetBrush( *wxBLACK_BRUSH );
      dc.DrawPolygon( curve_size, rotateShape( tk_curveL, curve_size, ori3w ) );
      dc.SetBrush( b );
    }
    else {
      dc.DrawPolygon( curve_size, rotateShape( tk_curveL, curve_size, ori3w ) );
      dc.DrawPolygon( curve_size, rotateShape( tk_curveR, curve_size, ori ) );
      const wxBrush& b = dc.GetBrush();
      dc.SetBrush( *wxBLACK_BRUSH );
      dc.DrawPolygon( track_size, rotateShape( tk_straight, track_size, ori ) );
      dc.SetBrush( b );
    }
  }

  if( m_bShowID ) {
    wxFont* font = new wxFont( dc.GetFont() );
    font->SetPointSize( 7 );
    dc.SetFont(*font);

    wxSize wxfontsize = dc.GetTextExtent( wxString(wItem.getid(m_Props),wxConvUTF8) );

    if( StrOp.equals( ori, wItem.south ) )
      dc.DrawRotatedText( wxString(wItem.getid(m_Props),wxConvUTF8), 1, wxfontsize.GetWidth(), 90.0 );
    else if( StrOp.equals( ori, wItem.north ) )
      dc.DrawRotatedText( wxString(wItem.getid(m_Props),wxConvUTF8), 1, 31, 90.0 );
    else if( StrOp.equals( ori, wItem.east ) )
      dc.DrawRotatedText( wxString(wItem.getid(m_Props),wxConvUTF8), 32 - wxfontsize.GetWidth(), 1, 0.0 );
    else
      dc.DrawRotatedText( wxString(wItem.getid(m_Props),wxConvUTF8), 0, 1, 0.0 );

    delete font;
  }



}


/**
 * Accessory Switch object
 */
void SymbolRenderer::drawAccessory( wxPaintDC& dc, bool fill, bool occupied, const char* ori ) {
  const char* state = wSwitch.getstate( m_Props );

  // SVG Symbol:
  if( m_SvgSym2!=NULL && StrOp.equals( state, wSwitch.turnout ) ) {
    if( occupied && m_SvgSym4!=NULL)
      drawSvgSym(dc, m_SvgSym4, ori);
    else
      drawSvgSym(dc, m_SvgSym2, ori);
  }
  else if( m_SvgSym1!=NULL ) {
    if( occupied && m_SvgSym3!=NULL)
      drawSvgSym(dc, m_SvgSym3, ori);
    else
      drawSvgSym(dc, m_SvgSym1, ori);
  }
}


/**
 * Turnout Switch object
 */
void SymbolRenderer::drawTurnout( wxPaintDC& dc, bool fill, bool occupied, const char* ori ) {
  const char* state = wSwitch.getstate( m_Props );
  Boolean raster = StrOp.equals( wSwitch.getswtype( m_Props ), wSwitch.swtype_raster );

  // SVG Symbol:
  if( m_SvgSym2!=NULL && StrOp.equals( state, wSwitch.turnout ) ) {
    drawSvgSym(dc, m_SvgSym2, ori);
  }
  else if( m_SvgSym1!=NULL ) {
    drawSvgSym(dc, m_SvgSym1, ori);
  }
  else {
  if( m_iSymSubType == switchtype::i_turnoutleft ) {
    if( StrOp.equals( state, wSwitch.turnout ) ) {
      dc.DrawPolygon( track_size, rotateShape( tk_straight, track_size, ori ) );
      const wxBrush& b = dc.GetBrush();
      dc.SetBrush( *wxBLACK_BRUSH );
      dc.DrawPolygon( curve_size, rotateShape( raster?tk_curveSWR:tk_curveR, curve_size, ori ) );
      dc.SetBrush( b );
    }
    else {
      dc.DrawPolygon( curve_size, rotateShape( raster?tk_curveSWR:tk_curveR, curve_size, ori ) );
      const wxBrush& b = dc.GetBrush();
      dc.SetBrush( *wxBLACK_BRUSH );
      dc.DrawPolygon( track_size, rotateShape( tk_straight, track_size, ori ) );
      dc.SetBrush( b );
    }
  }
  else {
    if( StrOp.equals( state, wSwitch.turnout ) ) {
      dc.DrawPolygon( track_size, rotateShape( tk_straight, track_size, ori ) );
      const wxBrush& b = dc.GetBrush();
      dc.SetBrush( *wxBLACK_BRUSH );
      dc.DrawPolygon( curve_size, rotateShape( raster?tk_curveSWL:tk_curveL, curve_size, ori ) );
      dc.SetBrush( b );
    }
    else {
      dc.DrawPolygon( curve_size, rotateShape( raster?tk_curveSWL:tk_curveL, curve_size, ori ) );
      const wxBrush& b = dc.GetBrush();
      dc.SetBrush( *wxBLACK_BRUSH );
      dc.DrawPolygon( track_size, rotateShape( tk_straight, track_size, ori ) );
      dc.SetBrush( b );
    }
  }
  }

  if( m_bShowID ) {
    wxFont* font = new wxFont( dc.GetFont() );
    font->SetPointSize( 7 );
    dc.SetFont(*font);

    if( m_iSymSubType == switchtype::i_turnoutleft ) {
      if( StrOp.equals( ori, wItem.south ) )
        dc.DrawRotatedText( wxString(wItem.getid(m_Props),wxConvUTF8), 32, 1, 270.0 );
      else if( StrOp.equals( ori, wItem.north ) )
        dc.DrawRotatedText( wxString(wItem.getid(m_Props),wxConvUTF8), 1, 32, 90.0 );
      else if( StrOp.equals( ori, wItem.east ) )
        dc.DrawRotatedText( wxString(wItem.getid(m_Props),wxConvUTF8), 0, 20, 0.0 );
      else
        dc.DrawRotatedText( wxString(wItem.getid(m_Props),wxConvUTF8), 0, 1, 0.0 );
    }
    else {
      if( StrOp.equals( ori, wItem.south ) )
        dc.DrawRotatedText( wxString(wItem.getid(m_Props),wxConvUTF8), 32, 1, 270.0 );
      else if( StrOp.equals( ori, wItem.north ) )
        dc.DrawRotatedText( wxString(wItem.getid(m_Props),wxConvUTF8), 1, 32, 90.0 );
      else if( StrOp.equals( ori, wItem.east ) )
        dc.DrawRotatedText( wxString(wItem.getid(m_Props),wxConvUTF8), 0, 20, 0.0 );
      else
        dc.DrawRotatedText( wxString(wItem.getid(m_Props),wxConvUTF8), 0, 1, 0.0 );
    }
    delete font;
  }

}

/**
 * Switch object
 */
void SymbolRenderer::drawSwitch( wxPaintDC& dc, bool fill, bool occupied, const char* ori ) {
  const char* state = wSwitch.getstate( m_Props );

  TraceOp.trc( "render", TRCLEVEL_DEBUG, __LINE__, 9999, "Switch %s state=%s", wSwitch.getid( m_Props ), state );

  switch( m_iSymSubType ) {
    case switchtype::i_decoupler:
      // SVG Symbol:
      if( m_SvgSym3!=NULL && StrOp.equals( state, wSwitch.straight ) ) {
        if( occupied && m_SvgSym4 != NULL )
          drawSvgSym(dc, m_SvgSym4, ori);
        else
          drawSvgSym(dc, m_SvgSym3, ori);
        return;
      }
      else if( m_SvgSym1!=NULL ) {
        if( occupied && m_SvgSym2 != NULL )
          drawSvgSym(dc, m_SvgSym2, ori);
        else
          drawSvgSym(dc, m_SvgSym1, ori);
        return;
      }
      dc.DrawPolygon( decoupler2_size, rotateShape( decoupler2, decoupler2_size, ori ) );
      dc.DrawPolygon( decoupler1_size, rotateShape( decoupler1, decoupler1_size, ori ) );
//      const wxBrush& b = dc.GetBrush();
//      if( StrOp.equals( state, wSwitch.turnout ) )
//        dc.SetBrush( *wxBLACK_BRUSH );
//      dc.DrawPolygon( decoupler2_size, rotateShape( decoupler2, decoupler2_size, ori ) );
//      dc.SetBrush( b );
      break;

    case switchtype::i_ccrossing:
      drawCCrossing( dc, fill, occupied, ori );
      break;

    case switchtype::i_crossing:
    case switchtype::i_crossingleft:
    case switchtype::i_crossingright:
      drawCrossing( dc, fill, occupied, ori );
      break;

    case switchtype::i_dcrossingleft:
    case switchtype::i_dcrossingright:
      drawDCrossing( dc, fill, occupied, ori );
      break;

    case switchtype::i_threeway:
      drawThreeway( dc, fill, occupied, ori );
      break;

    case switchtype::i_accessory:
      drawAccessory( dc, fill, occupied, ori );
      break;

    case switchtype::i_turnoutleft:
    case switchtype::i_turnoutright:
      drawTurnout( dc, fill, occupied, ori );
      break;

  }
}


/**
 * Signal object
 */
void SymbolRenderer::drawSignal( wxPaintDC& dc, bool fill, bool occupied, bool actroute, const char* ori ) {
  const char* state = wSignal.getstate( m_Props );
  int aspects = wSignal.getaspects( m_Props );
  TraceOp.trc( "render", TRCLEVEL_INFO, __LINE__, 9999, "setting signal %s to %s", wSignal.getid( m_Props ), state );

  // SVG Symbol:
  if( m_SvgSym2!=NULL && StrOp.equals( state, wSignal.yellow ) ) {
    if( actroute && m_SvgSym10 != NULL)
      drawSvgSym(dc, m_SvgSym10, ori);
    else if( occupied && m_SvgSym5 != NULL)
      drawSvgSym(dc, m_SvgSym5, ori);
    else
      drawSvgSym(dc, m_SvgSym2, ori);
  }
  else if( m_SvgSym3!=NULL && StrOp.equals( state, wSignal.green ) ) {
    if( actroute && m_SvgSym11 != NULL)
      drawSvgSym(dc, m_SvgSym11, ori);
    else if( occupied && m_SvgSym6 != NULL)
      drawSvgSym(dc, m_SvgSym6, ori);
    else
      drawSvgSym(dc, m_SvgSym3, ori);
  }
  else if( m_SvgSym7!=NULL && StrOp.equals( state, wSignal.white ) ) {
    if( actroute && m_SvgSym12 != NULL)
      drawSvgSym(dc, m_SvgSym12, ori);
    else if( occupied && m_SvgSym8 != NULL)
      drawSvgSym(dc, m_SvgSym8, ori);
    else
      drawSvgSym(dc, m_SvgSym7, ori);
  }
  else if( m_SvgSym2!=NULL && ( StrOp.equals( state, wSignal.green ) || StrOp.equals( state, wSignal.white ) ) && aspects == 2 ) {
    /* default to yellow aspect */
    if( actroute && m_SvgSym11 != NULL)
      drawSvgSym(dc, m_SvgSym11, ori);
    else if( occupied && m_SvgSym5 != NULL)
      drawSvgSym(dc, m_SvgSym5, ori);
    else
      drawSvgSym(dc, m_SvgSym2, ori);
  }
  else if( m_SvgSym1!=NULL ) {
    if( actroute && m_SvgSym9 != NULL)
      drawSvgSym(dc, m_SvgSym9, ori);
    else if( occupied && m_SvgSym4 != NULL)
      drawSvgSym(dc, m_SvgSym4, ori);
    else
      drawSvgSym(dc, m_SvgSym1, ori);
  }
  else {


  if( occupied ) {
    const wxBrush& b = dc.GetBrush();
    dc.DrawPolygon( track_size, rotateShape( tk_straight, track_size, ori ) );
    dc.SetBrush( *wxBLACK_BRUSH );
    dc.DrawPolygon( track_fill_size, rotateShape( tk_straight_fill, track_fill_size, ori ) );
    dc.SetBrush( b );
  }
  else if( fill ) {
    const wxBrush& b = dc.GetBrush();
    dc.SetBrush( *wxLIGHT_GREY_BRUSH );
    dc.DrawPolygon( track_size, rotateShape( tk_straight, track_size, ori ) );
    dc.SetBrush( b );
  }
  else {
    dc.DrawPolygon( track_size, rotateShape( tk_straight, track_size, ori ) );
  }

  dc.DrawPolygon( 4, rotateShape( sg2, 4, ori ) );
  if( StrOp.equals( state, wSignal.red ) ) {
    const wxBrush& b = dc.GetBrush();
    dc.SetBrush( *wxRED_BRUSH );
    dc.DrawPolygon( 8, rotateShape( sg1, 8, ori ) );
    dc.SetBrush( b );
  }
  else if( StrOp.equals( state, wSignal.yellow ) ) {
    const wxBrush& b = dc.GetBrush();
    wxBrush* yellow = new wxBrush( _T("yellow"), wxSOLID );

    dc.SetBrush( *yellow );
    dc.DrawPolygon( 8, rotateShape( sg1, 8, ori ) );
    dc.SetBrush( b );
    delete yellow;
  }
  else if( StrOp.equals( state, wSignal.green ) ) {
    const wxBrush& b = dc.GetBrush();
    dc.SetBrush( *wxGREEN_BRUSH );
    dc.DrawPolygon( 8, rotateShape( sg1, 8, ori ) );
    dc.SetBrush( b );
  }
  else if( StrOp.equals( state, wSignal.white ) ) {
    const wxBrush& b = dc.GetBrush();
    dc.SetBrush( *wxWHITE_BRUSH );
    dc.DrawPolygon( 8, rotateShape( sg1, 8, ori ) );
    dc.SetBrush( b );
  }
  else {
    const wxBrush& b = dc.GetBrush();
    dc.SetBrush( *wxRED_BRUSH );
    dc.DrawPolygon( 8, rotateShape( sg1, 8, ori ) );
    dc.SetBrush( b );
  }
  }

  if( m_bShowID ) {
    wxFont* font = new wxFont( dc.GetFont() );
    font->SetPointSize( 7 );
    dc.SetFont(*font);

    if( StrOp.equals( ori, wItem.north ) )
      dc.DrawRotatedText( wxString(wItem.getid(m_Props),wxConvUTF8), 32, 1, 270.0 );
    else if( StrOp.equals( ori, wItem.south ) )
      dc.DrawRotatedText( wxString(wItem.getid(m_Props),wxConvUTF8), 1, 32, 90.0 );
    else if( StrOp.equals( ori, wItem.east ) )
      dc.DrawRotatedText( wxString(wItem.getid(m_Props),wxConvUTF8), 0, 1, 0.0 );
    else
      dc.DrawRotatedText( wxString(wItem.getid(m_Props),wxConvUTF8), 0, 20, 0.0 );

    delete font;
  }
}


/**
 * Output object
 */
void SymbolRenderer::drawOutput( wxPaintDC& dc, bool fill, bool occupied, const char* ori ) {
  const char* state = wOutput.getstate( m_Props );
  TraceOp.trc( "render", TRCLEVEL_INFO, __LINE__, 9999, "setting output %s to %s", wSignal.getid( m_Props ), state );

  // SVG Symbol:
  if( m_SvgSym3!=NULL && StrOp.equals( state, wOutput.active ) ) {
    drawSvgSym(dc, m_SvgSym3, ori);
    return;
  }
  else if( m_SvgSym2!=NULL && StrOp.equals( state, wOutput.on ) ) {
    drawSvgSym(dc, m_SvgSym2, ori);
    return;
  }
  else if( m_SvgSym1!=NULL ) {
    drawSvgSym(dc, m_SvgSym1, ori);
    return;
  }

  dc.DrawCircle( 16, 16, 12 );

  if( StrOp.equals( state, wOutput.on ) ) {
    const wxBrush& b = dc.GetBrush();
    dc.SetBrush( *wxRED_BRUSH );
    dc.DrawCircle( 16, 16, 10 );
    dc.SetBrush( b );
  }
  else {
    const wxBrush& b = dc.GetBrush();
    dc.SetBrush( *wxGREEN_BRUSH );
    dc.DrawCircle( 16, 16, 10 );
    dc.SetBrush( b );
  }
}


/**
 * Block object
 */
void SymbolRenderer::drawBlock( wxPaintDC& dc, bool fill, bool occupied, const char* ori ) {
  m_bRotateable = true;

  if( StrOp.equals( ori, wItem.east ) )
    ori = wItem.west;

  // SVG Symbol:
  if( m_SvgSym1!=NULL && m_iOccupied == 0 ||
      m_SvgSym1!=NULL && m_SvgSym5==NULL && m_iOccupied == 4 ||
      m_SvgSym1!=NULL && m_SvgSym6==NULL && m_iOccupied == 5  )
  {
    drawSvgSym(dc, m_SvgSym1, ori);
  }
  else if( m_SvgSym2!=NULL && m_iOccupied == 1 ) {
    drawSvgSym(dc, m_SvgSym2, ori);
  }
  else if( m_SvgSym3!=NULL && m_iOccupied == 2 || m_SvgSym3!=NULL && m_SvgSym4==NULL && m_iOccupied == 3 ) {
    /* reserved state */
    drawSvgSym(dc, m_SvgSym3, ori);
  }
  else if( m_SvgSym4!=NULL && m_iOccupied == 3 ) {
    /* reserved state */
    drawSvgSym(dc, m_SvgSym4, ori);
  }
  else if( m_SvgSym5!=NULL && m_iOccupied == 4 ) {
    /* reserved state */
    drawSvgSym(dc, m_SvgSym5, ori);
  }
  else if( m_SvgSym6!=NULL && m_iOccupied == 5 ) {
    /* reserved state */
    drawSvgSym(dc, m_SvgSym6, ori);
  }
  else {
    dc.DrawPolygon( 4, rotateShape( bk, 4, ori ) );
  }
  // TODO: Blocktext scaling!!!
#ifdef __WIN32__ // no scaling is done when exchanging the font in wx 2.6.3
  //wxFont* font = new wxFont( dc.GetFont() );
  //font->SetPointSize( (int)(font->GetPointSize() * m_fText ) );
  //dc.SetFont(*font);
#else
  wxFont* font = new wxFont( dc.GetFont() );
  font->SetPointSize( (int)(font->GetPointSize() * m_fText ) );
  dc.SetFont(*font);
#endif

  if( StrOp.len(m_Label) > 0 ) {
    wxColour tfc = dc.GetTextForeground();

    int red = 0;
    int green = 0;
    int blue = 0;


    iONode planpanelIni = wGui.getplanpanel(wxGetApp().getIni());
    if( planpanelIni != NULL ) {
      red = wPlanPanel.getbktext_red(planpanelIni);
      green = wPlanPanel.getbktext_green(planpanelIni);
      blue = wPlanPanel.getbktext_blue(planpanelIni);
    }

    dc.SetTextForeground(wxColour(red,green,blue));

    if( StrOp.equals( ori, wItem.south ) )
      dc.DrawRotatedText( wxString(m_Label,wxConvUTF8), 32-5, 3, 270.0 );
    else if( StrOp.equals( ori, wItem.north ) )
      dc.DrawRotatedText( wxString(m_Label,wxConvUTF8), 7, (32 * 4)-3, 90.0 );
    else
      dc.DrawRotatedText( wxString(m_Label,wxConvUTF8), 3, 5, 0.0 );

    // restore previous color
    dc.SetTextForeground(tfc);
  }

#ifdef __WIN32__ // no scaling is done when exchanging the font in wx 2.6.3
#else
  delete font;
#endif
}


/**
 * SelTab object
 */
void SymbolRenderer::drawSelTab( wxPaintDC& dc, bool fill, bool occupied, const char* ori ) {
  m_bRotateable = true;
  int nrtracks = wSelTab.getnrtracks(m_Props);

  const wxBrush& b = dc.GetBrush();
  if( m_iOccupied == 1 ) {
    dc.SetBrush( wxColour(255,200,200) );
  }
  else if( m_iOccupied == 2 ) {
    dc.SetBrush( wxColour(255,255,200) );
  }

  dc.SetPen( wxPen( wxColour(0,0,0), 2));

  wxPoint seltab[4];
  seltab[0].x = 1;
  seltab[0].y = 3;
  seltab[1].x = (32 * nrtracks) - 1;
  seltab[1].y = 3;
  seltab[2].x = (32 * nrtracks) - 1;
  seltab[2].y = 28;
  seltab[3].x = 1;
  seltab[3].y = 28;
  //dc.DrawPolygon( 4, rotateShape( seltab, 4, ori ) );
  if( StrOp.equals( wItem.west, ori ) || StrOp.equals( wItem.east, ori ) )
    dc.DrawRoundedRectangle( 1, 3, (32 * nrtracks) - 1, 28, 10 );
  else
    dc.DrawRoundedRectangle( 3, 1, 28, (32 * nrtracks) - 1, 10 );

  dc.SetBrush( b );

  // TODO: Blocktext scaling!!!
#ifdef __WIN32__ // no scaling is done when exchanging the font in wx 2.6.3
  //wxFont* font = new wxFont( dc.GetFont() );
  //font->SetPointSize( (int)(font->GetPointSize() * m_fText ) );
  //dc.SetFont(*font);
#else
  wxFont* font = new wxFont( dc.GetFont() );
  font->SetPointSize( (int)(font->GetPointSize() * m_fText ) );
  dc.SetFont(*font);
#endif

  if( StrOp.equals( ori, wItem.south ) )
    dc.DrawRotatedText( wxString(m_Label,wxConvUTF8), 32-5, 3, 270.0 );
  else if( StrOp.equals( ori, wItem.north ) )
    dc.DrawRotatedText( wxString(m_Label,wxConvUTF8), 5, (32 * nrtracks)-3, 90.0 );
  else
    dc.DrawRotatedText( wxString(m_Label,wxConvUTF8), 5, 5, 0.0 );

#ifdef __WIN32__ // no scaling is done when exchanging the font in wx 2.6.3
#else
  delete font;
#endif
}


/**
 * Text object
 */
void SymbolRenderer::drawText( wxPaintDC& dc, bool fill, bool occupied, const char* ori ) {
  m_bRotateable = true;

  if( StrOp.endsWithi(m_Label, ".png") ) {
    if((m_bLabelChanged || !StrOp.equals( ori, m_Ori) )&& m_Bitmap != NULL ) {
      m_bLabelChanged = false;
      delete m_Bitmap;
      m_Bitmap = NULL;
    }
    if( m_Bitmap == NULL ) {
      const char* imagepath = wGui.getimagepath(wxGetApp().getIni());
      static char pixpath[256];
      StrOp.fmtb( pixpath, "%s%c%s", imagepath, SystemOp.getFileSeparator(), FileOp.ripPath( m_Label ) );

      if( FileOp.exist(pixpath)) {
        TraceOp.trc( "renderer", TRCLEVEL_INFO, __LINE__, 9999, "picture [%s]", pixpath );
        m_Bitmap = new wxBitmap(wxString(pixpath,wxConvUTF8), wxBITMAP_TYPE_PNG);

        if( StrOp.equals( ori, wItem.north ) || StrOp.equals( ori, wItem.south ) || StrOp.equals( ori, wItem.east ) ) {
          TraceOp.trc( "renderer", TRCLEVEL_INFO, __LINE__, 9999, "rotate [%s]", pixpath );
          wxImage img = m_Bitmap->ConvertToImage();
          delete m_Bitmap;
          img = img.Rotate90( StrOp.equals( ori, wItem.north ) ? true:false );
          if(StrOp.equals( ori, wItem.east ))
            img = img.Rotate90( StrOp.equals( ori, wItem.north ) ? true:false );
          m_Bitmap = new wxBitmap(img);
        }

      }
      else {
        TraceOp.trc( "locdialog", TRCLEVEL_WARNING, __LINE__, 9999, "picture [%s] not found", pixpath );
        // request the image from server:
        iONode node = NodeOp.inst( wDataReq.name(), NULL, ELEMENT_NODE );
        wDataReq.setcmd( node, wDataReq.get );
        wDataReq.settype( node, wDataReq.image );
        wDataReq.setfilename( node, FileOp.ripPath( m_Label ) );
        wxGetApp().sendToRocrail( node );
      }
    }
    m_Ori = ori;

    if( m_Bitmap != NULL ) {
      dc.DrawBitmap(*m_Bitmap, 0, 0, true);
      return;
    }
  }

  int pointsize = wText.getpointsize(m_Props);

#ifdef __WIN32__ // no scaling is done when exchanging the font in wx 2.6.3
  wxFont* font = new wxFont( dc.GetFont() );
  if( pointsize > 0 ) {
    font->SetPointSize( pointsize );

    if( wText.isbold(m_Props))
      font->SetWeight(wxFONTWEIGHT_BOLD);

    if( wText.isitalic(m_Props))
      font->SetStyle(wxFONTSTYLE_ITALIC);

    font->SetUnderlined( wText.isunderlined(m_Props) ? true:false);

    dc.SetFont(*font);
  }
#else
  wxFont* font = new wxFont( dc.GetFont() );
  if( pointsize > 0 )
    font->SetPointSize( pointsize );
  else
    font->SetPointSize( (int)(font->GetPointSize() * m_fText ) );

  if( wText.isbold(m_Props))
    font->SetWeight(wxFONTWEIGHT_BOLD);
  if( wText.isitalic(m_Props))
    font->SetStyle(wxFONTSTYLE_ITALIC);
  font->SetUnderlined( wText.isunderlined(m_Props) ? true:false);

  dc.SetFont(*font);
#endif


  wxColour color( wText.getred(m_Props), wText.getgreen(m_Props), wText.getblue(m_Props) );

  dc.SetTextForeground(color);

  if( !wText.istransparent(m_Props) && wText.getbackred(m_Props) != -1 && wText.getbackgreen(m_Props) != -1 && wText.getbackblue(m_Props) != -1 ){
    wxColour color( wText.getbackred(m_Props), wText.getbackgreen(m_Props), wText.getbackblue(m_Props) );
    dc.SetTextBackground(color);
    dc.SetBackgroundMode(wxSOLID);
  }

  if( StrOp.equals( ori, wItem.south ) )
    dc.DrawRotatedText( wxString(m_Label,wxConvUTF8), 32-5, 3, 270.0 );
  else if( StrOp.equals( ori, wItem.north ) )
    dc.DrawRotatedText( wxString(m_Label,wxConvUTF8), 5, (32 * wText.getcx( m_Props ))-3, 90.0 );
  else
    dc.DrawRotatedText( wxString(m_Label,wxConvUTF8), 3, 5, 0.0 );

  delete font;
}


/**
 *
 */
void SymbolRenderer::drawSensor( wxPaintDC& dc, bool fill, bool occupied, bool actroute, const char* ori ) {

  // SVG Symbol:
  if( m_SvgSym2!=NULL && wFeedback.isstate( m_Props ) ) {
    if(occupied && m_SvgSym4!= NULL)
      drawSvgSym(dc, m_SvgSym4, ori);
    else if(actroute && m_SvgSym6!= NULL)
      drawSvgSym(dc, m_SvgSym6, ori);
    else if(actroute && m_SvgSym4!= NULL)
      drawSvgSym(dc, m_SvgSym4, ori);
    else
      drawSvgSym(dc, m_SvgSym2, ori);
  }
  else if( m_SvgSym1!=NULL ) {
    if(occupied && m_SvgSym3!= NULL)
      drawSvgSym(dc, m_SvgSym3, ori);
    else if(actroute && m_SvgSym5!= NULL)
      drawSvgSym(dc, m_SvgSym5, ori);
    else if(actroute && m_SvgSym3!= NULL)
      drawSvgSym(dc, m_SvgSym3, ori);
    else
      drawSvgSym(dc, m_SvgSym1, ori);
  }
  else {
    dc.DrawPolygon( 16, rotateShape( fb1, 16, ori )  );
    // Show state.
    const wxBrush& b = dc.GetBrush();
    dc.SetBrush( wFeedback.isstate( m_Props )?*wxRED_BRUSH:*wxGREEN_BRUSH );
    dc.DrawPolygon( 4, rotateShape( fb2, 4, ori ) );
    dc.SetBrush( b );
  }

  if( m_bShowID ) {
    wxFont* font = new wxFont( dc.GetFont() );
    font->SetPointSize( 7 );
    dc.SetFont(*font);

    if( StrOp.equals( ori, wItem.south ) )
      dc.DrawRotatedText( wxString(wItem.getid(m_Props),wxConvUTF8), 32, 1, 270.0 );
    else if( StrOp.equals( ori, wItem.north ) )
      dc.DrawRotatedText( wxString(wItem.getid(m_Props),wxConvUTF8), 1, 32, 90.0 );
    else
      dc.DrawRotatedText( wxString(wItem.getid(m_Props),wxConvUTF8), 0, 1, 0.0 );

    delete font;
  }

}


/**
 * Turntable object
 */
void SymbolRenderer::drawTurntable( wxPaintDC& dc, bool fill, bool occupied, double* bridgepos, const char* ori ) {

  TraceOp.trc( "render", TRCLEVEL_INFO, __LINE__, 9999, "turntable with bridge pos=%f", *bridgepos );

  wxPen* pen = (wxPen*)wxLIGHT_GREY_PEN;
  pen->SetStyle(wxSHORT_DASH);
  dc.SetPen( *pen );

  dc.DrawCircle( 79, 79, 79 );
  pen->SetStyle(wxSOLID);

  iONode track = wTurntable.gettrack( m_Props );
  while( track != NULL ) {
    double degr = 7.5 * wTTTrack.getnr( track );
    double a = (degr*2*PI25DT)/360;
    double xa = cos(a) * 79.0;
    double ya = sin(a) * 79.0;
    int x = 79 + (int)xa;
    int y = 79 - (int)ya;

    if( wTTTrack.isstate( track ) || wTurntable.getbridgepos(m_Props) == wTTTrack.getnr(track) ) {
      pen = (wxPen*)wxBLACK_PEN;
      pen->SetWidth(5);
      dc.SetPen(*pen);
      *bridgepos = degr;
    }
    else {
      pen = (wxPen*)wxGREY_PEN;
      pen->SetWidth(5);
      dc.SetPen(*pen);
    }

    dc.DrawLine( 79, 79, x, y );

    track = wTurntable.nexttrack( m_Props, track );
  }

  pen = (wxPen*)wxBLACK_PEN;
  pen->SetWidth(1);
  dc.SetPen(*pen);

  dc.DrawCircle( 79, 79, 36 );
  dc.DrawCircle( 79, 79, 32 );
  dc.DrawPolygon( 5, rotateBridge( *bridgepos ) );

  const wxBrush& b = dc.GetBrush();
  Boolean sensor1 = wTurntable.isstate1( m_Props );
  Boolean sensor2 = wTurntable.isstate2( m_Props );

  wxBrush* yellow = NULL;

  if( sensor1 && sensor2 )
    dc.SetBrush( *wxRED_BRUSH );
  else if( sensor1 || sensor2 ) {
    yellow = new wxBrush( _T("yellow"), wxSOLID );
    dc.SetBrush( *yellow );
  }
  else
    dc.SetBrush( *wxGREEN_BRUSH );

  dc.DrawPolygon( 5, rotateBridgeSensors( *bridgepos ) );
  dc.SetBrush( b );
  if( yellow != NULL )
    delete yellow;

}


/**
 * Draw dispatcher
 */
void SymbolRenderer::drawShape( wxPaintDC& dc, bool fill, bool occupied, bool actroute, double* bridgepos, bool showID ) {
  m_bShowID = showID;
  const char* nodeName = NodeOp.getName( m_Props );
  const char* ori      = wItem.getori( m_Props );

  if( ori == NULL || StrOp.len( ori ) == 0 )
    ori = wItem.west;

  switch( m_iSymType ) {
    case symtype::i_track:
      drawTrack( dc, fill, occupied, actroute, ori );
      break;
    case symtype::i_switch:
      drawSwitch( dc, fill, occupied, ori );
      break;
    case symtype::i_signal:
      drawSignal( dc, fill, occupied, actroute, ori );
      break;
    case symtype::i_output:
      drawOutput( dc, fill, occupied, ori );
      break;
    case symtype::i_block:
      drawBlock( dc, fill, occupied, ori );
      break;
    case symtype::i_text:
      drawText( dc, fill, occupied, ori );
      break;
    case symtype::i_feedback:
      drawSensor( dc, fill, occupied, actroute, ori );
      break;
    case symtype::i_turntable:
      drawTurntable( dc, fill, occupied, bridgepos, ori );
      break;
    case symtype::i_selecttable:
      drawSelTab( dc, fill, occupied, ori );
      break;
  }
}


/**
 * Rotate shape according its orientation
 */
wxPoint* SymbolRenderer::rotateShape( wxPoint* poly, int cnt, const char* oriStr ) {
  static wxPoint p[64];
  double __cos[3] = {0,-1,0};
  double __sin[3] = {1,0,-1};
  enum { north=0, east, south };
  int ori = east;

  if( StrOp.equals( wItem.north, oriStr ) ) {
    ori = north;
  }
  else if( StrOp.equals( wItem.east, oriStr ) ) {
    ori = east;
  }
  else if( StrOp.equals( wItem.south, oriStr ) ) {
    ori = south;
  }
  else {
    // Default ori = west
    return poly;
  }

  for( int i = 0; i < cnt; i++ ) {
    double x1 = 0;
    double y1 = 0;
    double x = poly[i].x;
    double y = poly[i].y;
    x = x - 15.5;
    y = 15.5 - y;
    x1 = (x * __cos[ori]) - (y * __sin[ori]);
    y1 = (x * __sin[ori]) + (y * __cos[ori]);
    p[i].x = (int)(x1 + 15.5);
    p[i].y = (int)(15.5 - y1);
  }

  return p;
}


wxPoint* SymbolRenderer::rotateBridge( double ori ) {
  TraceOp.trc( "render", TRCLEVEL_INFO, __LINE__, 9999, "rotate bridge pos=%f", ori );
  static wxPoint p[5];
  double bp[4] = { 10.0, 170.0, 190.0, 350.0 };

  for( int i = 0; i < 4; i++ ) {
    double angle = ori+bp[i];
    if( angle > 360.0 )
      angle = angle -360.0;
    double a = (angle*2*PI25DT)/360;
    double xa = cos(a) * 32.0;
    double ya = sin(a) * 32.0;
    p[i].x = 79 + (int)xa;
    p[i].y = 79 - (int)ya;
    if( i == 0 ) {
      // end point to close the polygon
      p[4].x = p[i].x;
      p[4].y = p[i].y;
    }
  }
  return p;
}


wxPoint* SymbolRenderer::rotateBridgeSensors( double ori ) {
  TraceOp.trc( "render", TRCLEVEL_INFO, __LINE__, 9999, "rotate bridge pos=%f", ori );
  static wxPoint p[5];
  double bp[4] = { 10.0, 170.0, 190.0, 350.0 };

  for( int i = 0; i < 4; i++ ) {
    double angle = ori+bp[i];
    if( angle > 360.0 )
      angle = angle -360.0;
    double a = (angle*2*PI25DT)/360;
    double xa = cos(a) * 20.0;
    double ya = sin(a) * 20.0;
    p[i].x = 79 + (int)xa;
    p[i].y = 79 - (int)ya;
    if( i == 0 ) {
      // end point to close the polygon
      p[4].x = p[i].x;
      p[4].y = p[i].y;
    }
  }
  return p;
}


