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

#include "rocview/public/guiapp.h"

#include "rocrail/wrapper/public/Item.h"
#include "rocrail/wrapper/public/Block.h"
#include "rocrail/wrapper/public/Stage.h"
#include "rocrail/wrapper/public/Switch.h"
#include "rocrail/wrapper/public/Signal.h"
#include "rocrail/wrapper/public/Output.h"
#include "rocrail/wrapper/public/Route.h"
#include "rocrail/wrapper/public/Track.h"
#include "rocrail/wrapper/public/Turntable.h"
#include "rocrail/wrapper/public/TTTrack.h"
#include "rocrail/wrapper/public/Feedback.h"
#include "rocrail/wrapper/public/Text.h"
#include "rocrail/wrapper/public/SelTab.h"
#include "rocrail/wrapper/public/SelTabPos.h"
#include "rocrail/wrapper/public/DataReq.h"

#include "rocs/public/system.h"

#include "rocview/wrapper/public/Gui.h"
#include "rocview/wrapper/public/PlanPanel.h"

//#include "staticsymbols.h"


static double PI25DT = 3.141592653589793238462643;


SymbolRenderer::SymbolRenderer( iONode props, wxWindow* parent, iOMap symmap, int itemidps ) {
  m_Props = props;
  m_Parent = parent;
  m_SymMap = symmap;
  m_bRotateable = true;
  m_bShowID = false;
  m_iOccupied = 0;
  m_Bitmap = NULL;
  m_Scale = 1.0;
  TraceOp.trc( "render", TRCLEVEL_INFO, __LINE__, 9999, "symbol map size = %d", symmap == NULL ? 0:MapOp.size(symmap) );
  initSym();
  m_Label = StrOp.dup("...");
  m_iItemIDps = itemidps;
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
  m_SvgSym13 = NULL;
  m_SvgSym14 = NULL;
  m_SvgSym15 = NULL;
  m_SvgSym16 = NULL;

  const char* nodeName = NodeOp.getName( m_Props );

  TraceOp.trc( "render", TRCLEVEL_DEBUG, __LINE__, 9999, "nodename=%s", nodeName );

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
          m_SvgSym4 = (svgSymbol*)MapOp.get( m_SymMap, tracktype::curve_occroute );
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
          m_SvgSym4 = (svgSymbol*)MapOp.get( m_SymMap, tracktype::straight_occroute );
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
          if( wSwitch.getaddr1( m_Props ) == 0 && wSwitch.getport1( m_Props ) == 0 ) {
            m_SvgSym1 = (svgSymbol*)MapOp.get( m_SymMap, switchtype::crossing );
          }
          else {
            m_SvgSym1 = (svgSymbol*)MapOp.get( m_SymMap, wSwitch.isdir(m_Props) ? switchtype::crossingleft : switchtype::crossingright );
            m_SvgSym2 = (svgSymbol*)MapOp.get( m_SymMap, wSwitch.isdir(m_Props) ? switchtype::crossingleft_t : switchtype::crossingright_t );
            m_iSymSubType = wSwitch.isdir(m_Props) ? switchtype::i_crossingleft : switchtype::i_crossingright;
            m_SvgSym4 = (svgSymbol*)MapOp.get( m_SymMap, wSwitch.isdir(m_Props) ? switchtype::crossingleft_occ : switchtype::crossingright_occ );
            m_SvgSym5 = (svgSymbol*)MapOp.get( m_SymMap, wSwitch.isdir(m_Props) ? switchtype::crossingleft_t_occ : switchtype::crossingright_t_occ );
          }
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
          m_SvgSym5 = (svgSymbol*)MapOp.get( m_SymMap, wSwitch.isdir(m_Props) ? switchtype::dcrossingleft_r_occ:switchtype::dcrossingright_r_occ );
          m_SvgSym6 = (svgSymbol*)MapOp.get( m_SymMap, wSwitch.isdir(m_Props) ? switchtype::dcrossingleft_r_t_occ:switchtype::dcrossingright_r_t_occ );
          m_SvgSym7 = (svgSymbol*)MapOp.get( m_SymMap, wSwitch.isdir(m_Props) ? switchtype::dcrossingleft_r_tl_occ:switchtype::dcrossingright_r_tl_occ );
          m_SvgSym8 = (svgSymbol*)MapOp.get( m_SymMap, wSwitch.isdir(m_Props) ? switchtype::dcrossingleft_r_tr_occ:switchtype::dcrossingright_r_tr_occ );
        }
        else {
          m_SvgSym1 = (svgSymbol*)MapOp.get( m_SymMap, wSwitch.isdir(m_Props) ? switchtype::dcrossingleft:switchtype::dcrossingright );
          m_SvgSym2 = (svgSymbol*)MapOp.get( m_SymMap, wSwitch.isdir(m_Props) ? switchtype::dcrossingleft_t:switchtype::dcrossingright_t );
          m_SvgSym3 = (svgSymbol*)MapOp.get( m_SymMap, wSwitch.isdir(m_Props) ? switchtype::dcrossingleft_tl:switchtype::dcrossingright_tl );
          m_SvgSym4 = (svgSymbol*)MapOp.get( m_SymMap, wSwitch.isdir(m_Props) ? switchtype::dcrossingleft_tr:switchtype::dcrossingright_tr );
          m_SvgSym5 = (svgSymbol*)MapOp.get( m_SymMap, wSwitch.isdir(m_Props) ? switchtype::dcrossingleft_occ:switchtype::dcrossingright_occ );
          m_SvgSym6 = (svgSymbol*)MapOp.get( m_SymMap, wSwitch.isdir(m_Props) ? switchtype::dcrossingleft_t_occ:switchtype::dcrossingright_t_occ );
          m_SvgSym7 = (svgSymbol*)MapOp.get( m_SymMap, wSwitch.isdir(m_Props) ? switchtype::dcrossingleft_tl_occ:switchtype::dcrossingright_tl_occ );
          m_SvgSym8 = (svgSymbol*)MapOp.get( m_SymMap, wSwitch.isdir(m_Props) ? switchtype::dcrossingleft_tr_occ:switchtype::dcrossingright_tr_occ );
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
        m_SvgSym5 = (svgSymbol*)MapOp.get( m_SymMap, switchtype::decoupler_route );
        m_SvgSym6 = (svgSymbol*)MapOp.get( m_SymMap, switchtype::decoupler_on_route );
        m_SvgSym7 = (svgSymbol*)MapOp.get( m_SymMap, switchtype::decoupler_on_occ_route );
        m_SvgSym8 = (svgSymbol*)MapOp.get( m_SymMap, switchtype::decoupler_occ_route );
      }
    }
    else if( StrOp.equals( wSwitch.threeway, wSwitch.gettype( m_Props ) ) ) {
      m_iSymSubType = switchtype::i_threeway;
      if( m_SymMap != NULL ) {
        m_SvgSym1 = (svgSymbol*)MapOp.get( m_SymMap, switchtype::threeway );
        m_SvgSym2 = (svgSymbol*)MapOp.get( m_SymMap, switchtype::threeway_tl );
        m_SvgSym3 = (svgSymbol*)MapOp.get( m_SymMap, switchtype::threeway_tr );
        m_SvgSym4 = (svgSymbol*)MapOp.get( m_SymMap, switchtype::threeway_occ );
        m_SvgSym5 = (svgSymbol*)MapOp.get( m_SymMap, switchtype::threeway_tl_occ );
        m_SvgSym6 = (svgSymbol*)MapOp.get( m_SymMap, switchtype::threeway_tr_occ );
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
          m_SvgSym4 = (svgSymbol*)MapOp.get( m_SymMap, switchtype::turnoutleft_r_occ );
          m_SvgSym5 = (svgSymbol*)MapOp.get( m_SymMap, switchtype::turnoutleft_r_t_occ );
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
            m_SvgSym4 = (svgSymbol*)MapOp.get( m_SymMap, switchtype::turnoutleft_occ );
            m_SvgSym5 = (svgSymbol*)MapOp.get( m_SymMap, switchtype::turnoutleft_t_occ );
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
          m_SvgSym4 = (svgSymbol*)MapOp.get( m_SymMap, switchtype::turnoutright_r_occ );
          m_SvgSym5 = (svgSymbol*)MapOp.get( m_SymMap, switchtype::turnoutright_r_t_occ );
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
            m_SvgSym4 = (svgSymbol*)MapOp.get( m_SymMap, switchtype::turnoutright_occ );
            m_SvgSym5 = (svgSymbol*)MapOp.get( m_SymMap, switchtype::turnoutright_t_occ );
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

      StrOp.fmtb( key, outputtype::button_off_occ, wOutput.getsvgtype( m_Props ) );
      m_SvgSym4 = (svgSymbol*)MapOp.get( m_SymMap, key );
      StrOp.fmtb( key, outputtype::button_on_occ, wOutput.getsvgtype( m_Props ) );
      m_SvgSym5 = (svgSymbol*)MapOp.get( m_SymMap, key );
      StrOp.fmtb( key, outputtype::button_active_occ, wOutput.getsvgtype( m_Props ) );
      m_SvgSym6 = (svgSymbol*)MapOp.get( m_SymMap, key );

      StrOp.fmtb( key, outputtype::button_off_route, wOutput.getsvgtype( m_Props ) );
      m_SvgSym7 = (svgSymbol*)MapOp.get( m_SymMap, key );
      StrOp.fmtb( key, outputtype::button_on_route, wOutput.getsvgtype( m_Props ) );
      m_SvgSym8 = (svgSymbol*)MapOp.get( m_SymMap, key );
      StrOp.fmtb( key, outputtype::button_active_route, wOutput.getsvgtype( m_Props ) );
      m_SvgSym9 = (svgSymbol*)MapOp.get( m_SymMap, key );
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
        m_SvgSym13 = (svgSymbol*)MapOp.get( m_SymMap, blocktype::block_sc );
        m_SvgSym15 = (svgSymbol*)MapOp.get( m_SymMap, blocktype::block_aident );

        m_SvgSym7  = (svgSymbol*)MapOp.get( m_SymMap, blocktype::block_s );
        m_SvgSym8  = (svgSymbol*)MapOp.get( m_SymMap, blocktype::block_occ_s );
        m_SvgSym9  = (svgSymbol*)MapOp.get( m_SymMap, blocktype::block_res_s );
        m_SvgSym10 = (svgSymbol*)MapOp.get( m_SymMap, blocktype::block_ent_s );
        m_SvgSym11 = (svgSymbol*)MapOp.get( m_SymMap, blocktype::block_closed_s );
        m_SvgSym12 = (svgSymbol*)MapOp.get( m_SymMap, blocktype::block_ghost_s );
        m_SvgSym14 = (svgSymbol*)MapOp.get( m_SymMap, blocktype::block_sc_s );
        m_SvgSym16 = (svgSymbol*)MapOp.get( m_SymMap, blocktype::block_aident_s );
      }
    }
  }
  else if( StrOp.equals( wStage.name(), nodeName ) ) {
    m_iSymType = symtype::i_stage;
    m_iSymSubType = stagetype::i_stage;
    if( m_SymMap != NULL ) {
      m_SvgSym1 = (svgSymbol*)MapOp.get( m_SymMap, stagetype::stage );
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
          m_SvgSym7 = (svgSymbol*)MapOp.get( m_SymMap, feedbacktype::curve_sensor_cnt );
        }
        else {
          m_SvgSym1 = (svgSymbol*)MapOp.get( m_SymMap, feedbacktype::sensor_off );
          m_SvgSym2 = (svgSymbol*)MapOp.get( m_SymMap, feedbacktype::sensor_on );
          m_SvgSym3 = (svgSymbol*)MapOp.get( m_SymMap, feedbacktype::sensor_off_occ );
          m_SvgSym4 = (svgSymbol*)MapOp.get( m_SymMap, feedbacktype::sensor_on_occ );
          m_SvgSym5 = (svgSymbol*)MapOp.get( m_SymMap, feedbacktype::sensor_off_route );
          m_SvgSym6 = (svgSymbol*)MapOp.get( m_SymMap, feedbacktype::sensor_on_route );
          m_SvgSym7 = (svgSymbol*)MapOp.get( m_SymMap, feedbacktype::sensor_cnt );
        }
      }
    }
  }
  else if( StrOp.equals( wRoute.name(), nodeName ) ) {
    m_iSymType = symtype::i_route;
    m_iSymSubType = routetype::i_route;
    if( m_SymMap != NULL ) {
      m_SvgSym1 = (svgSymbol*)MapOp.get( m_SymMap, routetype::route_free );
      m_SvgSym2 = (svgSymbol*)MapOp.get( m_SymMap, routetype::route_locked );
      m_SvgSym3 = (svgSymbol*)MapOp.get( m_SymMap, routetype::route_selected );
      m_SvgSym4 = (svgSymbol*)MapOp.get( m_SymMap, routetype::route_deselected );
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

void SymbolRenderer::setLabel( const char* label, int occupied, bool rotate ) {
  StrOp.free(m_Label);
  m_Label = StrOp.dup(label);
  m_iOccupied = occupied == -1 ? m_iOccupied:occupied;
  m_bLabelChanged = true;
  m_rotate = rotate;
}


/**
 * prepare symbol size
 */
void SymbolRenderer::sizeToScale( double symsize, double scale, double bktext, int* cx, int* cy, const char* ori ) {
  *cx = 1;
  *cy = 1;
  m_fText = bktext;
  m_Scale = scale;

  if( StrOp.equals( wBlock.name(), NodeOp.getName( m_Props ) ) ) {

    if( StrOp.equals( ori, wItem.north ) || StrOp.equals( ori, wItem.south ) ) {
      // vertical
      *cx = 1;
      *cy = wBlock.issmallsymbol(m_Props) ? 2:4;
    }
    else { // horizontal
      *cx = wBlock.issmallsymbol(m_Props) ? 2:4;
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

  if( StrOp.equals( wBlock.name(), NodeOp.getName( m_Props ) ) && wBlock.issmallsymbol(m_Props) && m_SvgSym7 != NULL ) {
    if( StrOp.equals( ori, wItem.north ) || StrOp.equals( ori, wItem.south ) ) {
      *cy = m_SvgSym7->width  / 32;
      *cx = m_SvgSym7->height / 32;
    }
    else {
      *cx = m_SvgSym7->width  / 32;
      *cy = m_SvgSym7->height / 32;
    }
  }
  else if( m_SvgSym1 != NULL ) {
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
  if( actroute && occupied && m_SvgSym4!=NULL ) {
    drawSvgSym(dc, m_SvgSym4, ori);
    dc.SetBrush( b );
    return;
  }

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
    if( occupied && m_SvgSym4 != NULL )
      drawSvgSym(dc, m_SvgSym4, ori);
    else
      drawSvgSym(dc, m_SvgSym1, ori);
  }
  else if( hasUnit && m_SvgSym2!=NULL && StrOp.equals( state, wSwitch.turnout ) ) {
    if( occupied && m_SvgSym5 != NULL )
      drawSvgSym(dc, m_SvgSym5, ori);
    else
      drawSvgSym(dc, m_SvgSym2, ori);
  }
  else if( hasUnit && m_SvgSym1!=NULL ) {
    if( occupied && m_SvgSym4 != NULL )
      drawSvgSym(dc, m_SvgSym4, ori);
    else
      drawSvgSym(dc, m_SvgSym1, ori);
  }


  if( m_bShowID ) {
    wxFont* font = new wxFont( dc.GetFont() );
    font->SetPointSize( m_iItemIDps );
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
      if( occupied && m_SvgSym7 != NULL )
        drawSvgSym(dc, m_SvgSym7, ori);
      else
        drawSvgSym(dc, m_SvgSym3, ori);
    }
    else if( m_SvgSym4!=NULL && StrOp.equals( state, wSwitch.right ) ) {
      if( occupied && m_SvgSym8 != NULL )
        drawSvgSym(dc, m_SvgSym8, ori);
      else
        drawSvgSym(dc, m_SvgSym4, ori);
    }
    else if( m_SvgSym2!=NULL && StrOp.equals( state, wSwitch.turnout ) ) {
      if( occupied && m_SvgSym6 != NULL )
        drawSvgSym(dc, m_SvgSym6, ori);
      else
        drawSvgSym(dc, m_SvgSym2, ori);
    }
    else if( m_SvgSym1!=NULL ) {
      if( occupied && m_SvgSym5 != NULL )
        drawSvgSym(dc, m_SvgSym5, ori);
      else
        drawSvgSym(dc, m_SvgSym1, ori);
    }
  }

  else if( m_SvgSym2!=NULL && StrOp.equals( state, wSwitch.turnout ) ) {
    if( occupied && m_SvgSym6 != NULL )
      drawSvgSym(dc, m_SvgSym6, ori);
    else
      drawSvgSym(dc, m_SvgSym2, ori);
  }

  else if( m_SvgSym1!=NULL ) {
    if( occupied && m_SvgSym5 != NULL )
      drawSvgSym(dc, m_SvgSym5, ori);
    else
      drawSvgSym(dc, m_SvgSym1, ori);
  }


  if( m_bShowID ) {
    wxFont* font = new wxFont( dc.GetFont() );
    font->SetPointSize( m_iItemIDps );
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
    if( occupied && m_SvgSym5 != NULL )
      drawSvgSym(dc, m_SvgSym5, ori);
    else
      drawSvgSym(dc, m_SvgSym2, ori);
  }
  else if( m_SvgSym3!=NULL && StrOp.equals( state, wSwitch.right ) ) {
    if( occupied && m_SvgSym6 != NULL )
      drawSvgSym(dc, m_SvgSym6, ori);
    else
      drawSvgSym(dc, m_SvgSym3, ori);
  }
  else if( m_SvgSym1!=NULL ) {
    if( occupied && m_SvgSym4 != NULL )
      drawSvgSym(dc, m_SvgSym4, ori);
    else
      drawSvgSym(dc, m_SvgSym1, ori);
  }

  if( m_bShowID ) {
    wxFont* font = new wxFont( dc.GetFont() );
    font->SetPointSize( m_iItemIDps );
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
    if( occupied && m_SvgSym5 != NULL )
      drawSvgSym(dc, m_SvgSym5, ori);
    else
      drawSvgSym(dc, m_SvgSym2, ori);
  }
  else if( m_SvgSym1!=NULL ) {
    if( occupied && m_SvgSym4 != NULL )
      drawSvgSym(dc, m_SvgSym4, ori);
    else
      drawSvgSym(dc, m_SvgSym1, ori);
  }

  if( m_bShowID ) {
    wxFont* font = new wxFont( dc.GetFont() );
    font->SetPointSize( m_iItemIDps );
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
void SymbolRenderer::drawSwitch( wxPaintDC& dc, bool fill, bool occupied, bool actroute, const char* ori ) {
  const char* state = wSwitch.getstate( m_Props );

  TraceOp.trc( "render", TRCLEVEL_DEBUG, __LINE__, 9999, "Switch %s state=%s", wSwitch.getid( m_Props ), state );

  /*
        m_SvgSym1 = (svgSymbol*)MapOp.get( m_SymMap, switchtype::decoupler );
        m_SvgSym2 = (svgSymbol*)MapOp.get( m_SymMap, switchtype::decoupler_occ );
        m_SvgSym3 = (svgSymbol*)MapOp.get( m_SymMap, switchtype::decoupler_on );
        m_SvgSym4 = (svgSymbol*)MapOp.get( m_SymMap, switchtype::decoupler_on_occ );
        m_SvgSym5 = (svgSymbol*)MapOp.get( m_SymMap, switchtype::decoupler_route );
        m_SvgSym6 = (svgSymbol*)MapOp.get( m_SymMap, switchtype::decoupler_on_route );
        m_SvgSym7 = (svgSymbol*)MapOp.get( m_SymMap, switchtype::decoupler_on_occ_route );
        m_SvgSym8 = (svgSymbol*)MapOp.get( m_SymMap, switchtype::decoupler_occ_route );

   */
  switch( m_iSymSubType ) {
    case switchtype::i_decoupler:
      // SVG Symbol:
      if( m_SvgSym3!=NULL && StrOp.equals( state, wSwitch.straight ) ) {
        if( occupied && !actroute && m_SvgSym4 != NULL )
          drawSvgSym(dc, m_SvgSym4, ori);
        else if( occupied && actroute && m_SvgSym7 != NULL )
          drawSvgSym(dc, m_SvgSym7, ori);
        else if( occupied && actroute && m_SvgSym4 != NULL )
          drawSvgSym(dc, m_SvgSym4, ori);
        else if( !occupied && actroute && m_SvgSym6 != NULL )
          drawSvgSym(dc, m_SvgSym6, ori);
        else if( m_SvgSym3 != NULL )
          drawSvgSym(dc, m_SvgSym3, ori);
        return;
      }
      else if( m_SvgSym1!=NULL ) {
        if( occupied && !actroute && m_SvgSym2 != NULL )
          drawSvgSym(dc, m_SvgSym2, ori);
        else if( occupied && actroute && m_SvgSym8 != NULL )
          drawSvgSym(dc, m_SvgSym8, ori);
        else if( occupied && actroute && m_SvgSym2 != NULL )
          drawSvgSym(dc, m_SvgSym2, ori);
        else if( !occupied && actroute && m_SvgSym5 != NULL )
          drawSvgSym(dc, m_SvgSym5, ori);
        else if( m_SvgSym1 != NULL )
          drawSvgSym(dc, m_SvgSym1, ori);
        return;
      }
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
    if( occupied && m_SvgSym5 != NULL)
      drawSvgSym(dc, m_SvgSym5, ori);
    else if( actroute && m_SvgSym10 != NULL)
      drawSvgSym(dc, m_SvgSym10, ori);
    else
      drawSvgSym(dc, m_SvgSym2, ori);
  }
  else if( m_SvgSym3!=NULL && StrOp.equals( state, wSignal.green ) ) {
    if( occupied && m_SvgSym6 != NULL)
      drawSvgSym(dc, m_SvgSym6, ori);
    else if( actroute && m_SvgSym11 != NULL)
      drawSvgSym(dc, m_SvgSym11, ori);
    else
      drawSvgSym(dc, m_SvgSym3, ori);
  }
  else if( m_SvgSym7!=NULL && StrOp.equals( state, wSignal.white ) ) {
    if( occupied && m_SvgSym8 != NULL)
      drawSvgSym(dc, m_SvgSym8, ori);
    else if( actroute && m_SvgSym12 != NULL)
      drawSvgSym(dc, m_SvgSym12, ori);
    else
      drawSvgSym(dc, m_SvgSym7, ori);
  }
  else if( m_SvgSym2!=NULL && ( StrOp.equals( state, wSignal.green ) || StrOp.equals( state, wSignal.white ) ) && aspects == 2 ) {
    /* default to yellow aspect */
    if( occupied && m_SvgSym5 != NULL)
      drawSvgSym(dc, m_SvgSym5, ori);
    else if( actroute && m_SvgSym11 != NULL)
      drawSvgSym(dc, m_SvgSym11, ori);
    else
      drawSvgSym(dc, m_SvgSym2, ori);
  }
  else if( m_SvgSym1!=NULL ) {
    if( occupied && m_SvgSym4 != NULL)
      drawSvgSym(dc, m_SvgSym4, ori);
    else if( actroute && m_SvgSym9 != NULL)
      drawSvgSym(dc, m_SvgSym9, ori);
    else
      drawSvgSym(dc, m_SvgSym1, ori);
  }

  if( m_bShowID ) {
    wxFont* font = new wxFont( dc.GetFont() );
    font->SetPointSize( m_iItemIDps );
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
void SymbolRenderer::drawOutput( wxPaintDC& dc, bool fill, bool occupied, bool actroute, const char* ori ) {
  const char* state = wOutput.getstate( m_Props );
  TraceOp.trc( "render", TRCLEVEL_INFO, __LINE__, 9999, "setting output %s to %s", wSignal.getid( m_Props ), state );

  // SVG Symbol:
  if( m_SvgSym3!=NULL && StrOp.equals( state, wOutput.active ) ) {
    if(occupied && m_SvgSym6!= NULL)
      drawSvgSym(dc, m_SvgSym6, ori);
    else if(actroute && m_SvgSym9!= NULL)
      drawSvgSym(dc, m_SvgSym9, ori);
    else if(actroute && m_SvgSym6!= NULL)
      drawSvgSym(dc, m_SvgSym6, ori);
    else
      drawSvgSym(dc, m_SvgSym3, ori);
    return;
  }
  else if( m_SvgSym2!=NULL && StrOp.equals( state, wOutput.on ) ) {
    if(occupied && m_SvgSym5!= NULL)
      drawSvgSym(dc, m_SvgSym5, ori);
    else if(actroute && m_SvgSym8!= NULL)
      drawSvgSym(dc, m_SvgSym8, ori);
    else if(actroute && m_SvgSym5!= NULL)
      drawSvgSym(dc, m_SvgSym5, ori);
    else
      drawSvgSym(dc, m_SvgSym2, ori);
    return;
  }
  else if( m_SvgSym1!=NULL ) {
    if(occupied && m_SvgSym4!= NULL)
      drawSvgSym(dc, m_SvgSym4, ori);
    else if(actroute && m_SvgSym7!= NULL)
      drawSvgSym(dc, m_SvgSym7, ori);
    else if(actroute && m_SvgSym4!= NULL)
      drawSvgSym(dc, m_SvgSym4, ori);
    else
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
 * Stage object
 */
void SymbolRenderer::drawStage( wxPaintDC& dc, bool fill, bool occupied, const char* ori ) {
  m_bRotateable = true;
  int len = 4;

  if( m_SvgSym1 != NULL )
  {
    drawSvgSym(dc, m_SvgSym1, ori);
  }

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
      dc.DrawRotatedText( wxString(m_Label,wxConvUTF8), 7, (32 * len)-3, 90.0 );
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
 * Block object
 */
void SymbolRenderer::drawBlock( wxPaintDC& dc, bool fill, bool occupied, const char* ori ) {
  m_bRotateable = true;
  Boolean m_bSmall = wBlock.issmallsymbol(m_Props);
  int blocklen = m_bSmall ? 2:4;
  const char* textOri = ori;

  svgSymbol* svgSym[9];

  svgSym[1] = (m_bSmall && m_SvgSym7  != NULL)?m_SvgSym7:m_SvgSym1;
  svgSym[2] = (m_bSmall && m_SvgSym8  != NULL)?m_SvgSym8:m_SvgSym2;
  svgSym[3] = (m_bSmall && m_SvgSym9  != NULL)?m_SvgSym9:m_SvgSym3;
  svgSym[4] = (m_bSmall && m_SvgSym10 != NULL)?m_SvgSym10:m_SvgSym4;
  svgSym[5] = (m_bSmall && m_SvgSym11 != NULL)?m_SvgSym11:m_SvgSym5;
  svgSym[6] = (m_bSmall && m_SvgSym12 != NULL)?m_SvgSym12:m_SvgSym6;
  svgSym[7] = (m_bSmall && m_SvgSym14 != NULL)?m_SvgSym14:m_SvgSym13;
  svgSym[8] = (m_bSmall && m_SvgSym16 != NULL)?m_SvgSym16:m_SvgSym15;

  //if( StrOp.equals( ori, wItem.east ) )
    //ori = wItem.west;

  if( m_rotate) {
    if( StrOp.equals(ori, wItem.west))
        ori = wItem.east;
    else if(StrOp.equals(ori, wItem.east))
        ori = wItem.west;
    else if(StrOp.equals(ori, wItem.north))
        ori = wItem.south;
    else if(StrOp.equals(ori, wItem.south))
        ori = wItem.north;
  }

  // SVG Symbol:
  if( svgSym[1]!=NULL && m_iOccupied == 0 ||
      svgSym[1]!=NULL && svgSym[5]==NULL && m_iOccupied == 4 ||
      svgSym[1]!=NULL && svgSym[6]==NULL && m_iOccupied == 5  )
  {
    drawSvgSym(dc, svgSym[1], ori);
  }
  else if( svgSym[2]!=NULL && m_iOccupied == 1 ) {
    drawSvgSym(dc, svgSym[2], ori);
  }
  else if( svgSym[3]!=NULL && m_iOccupied == 2 || svgSym[3]!=NULL && svgSym[4]==NULL && m_iOccupied == 3 ) {
    /* reserved state */
    drawSvgSym(dc, svgSym[3], ori);
  }
  else if( svgSym[4]!=NULL && m_iOccupied == 3 ) {
    /* reserved state */
    drawSvgSym(dc, svgSym[4], ori);
  }
  else if( svgSym[5]!=NULL && m_iOccupied == 4 ) {
    /* reserved state */
    drawSvgSym(dc, svgSym[5], ori);
  }
  else if( svgSym[6]!=NULL && m_iOccupied == 5 ) {
    /* ghost */
    drawSvgSym(dc, svgSym[6], ori);
  }
  else if( svgSym[7]!=NULL && m_iOccupied == 6 ) {
    /* shortcut */
    drawSvgSym(dc, svgSym[7], ori);
  }
  else if( svgSym[8]!=NULL && m_iOccupied == 7 ) {
     /* aident */
     drawSvgSym(dc, svgSym[8], ori);
   }
  else if( svgSym[1]!=NULL ) {
     drawSvgSym(dc, svgSym[1], ori);
   }
  // TODO: Blocktext scaling!!!
  wxFont* font = new wxFont( dc.GetFont() );
#ifdef __WIN32__ // no scaling is done when exchanging the font in wx 2.6.3
  //font->SetPointSize( (int)(font->GetPointSize() * m_fText ) );
  //dc.SetFont(*font);
#else
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

    /* center the blocktext */
    wxCoord width;
    wxCoord height;
    dc.GetTextExtent(wxString(m_Label,wxConvUTF8).Trim(), &width, &height, 0,0, font);

    if( StrOp.equals( textOri, wItem.south ) )
      dc.DrawRotatedText( wxString(m_Label,wxConvUTF8), 32-5, 3, 270.0 );
    else if( StrOp.equals( textOri, wItem.north ) )
      dc.DrawRotatedText( wxString(m_Label,wxConvUTF8), 7, (32 * blocklen)-3, 90.0 );
    else {
#ifdef __WIN32__
      dc.DrawRotatedText( wxString(m_Label,wxConvUTF8).Trim(), 9, 8, 0.0 );
#else
      dc.DrawRotatedText( wxString(m_Label,wxConvUTF8).Trim(), ((32*blocklen-width)/2), (32-height)/2, 0.0 );
#endif
    }
    // restore previous color
    dc.SetTextForeground(tfc);
  }

  delete font;
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
  bool cnt = false;

  if(wFeedback.getcountedcars( m_Props ) != wFeedback.getcarcount( m_Props )) {
    cnt = true;
    TraceOp.trc( "render", TRCLEVEL_INFO, __LINE__, 9999, "sensor %s is counting cars %d/%d",
        wFeedback.getid( m_Props ), wFeedback.getcountedcars( m_Props ), wFeedback.getcarcount( m_Props ) );
  }

  // SVG Symbol:
  if( m_SvgSym7!=NULL && wFeedback.isstate( m_Props ) && cnt) {
    drawSvgSym(dc, m_SvgSym7, ori);
  }
  else if( m_SvgSym2!=NULL && wFeedback.isstate( m_Props ) ) {
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

  if( m_bShowID ) {
    wxFont* font = new wxFont( dc.GetFont() );
    font->SetPointSize( m_iItemIDps );
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


void SymbolRenderer::drawRoute( wxPaintDC& dc, bool fill, bool occupied, const char* ori, int status ) {
  TraceOp.trc( "render", TRCLEVEL_INFO, __LINE__, 9999, "set route %s to %d", wRoute.getid( m_Props ), status );

  // SVG Symbol:
  if( status == 0 && m_SvgSym1!=NULL ) {
    drawSvgSym(dc, m_SvgSym1, ori);
  }
  else if( status == 1 && m_SvgSym2!=NULL ) {
    drawSvgSym(dc, m_SvgSym2, ori);
  }
  else if( status == 2 && m_SvgSym3!=NULL ) {
    drawSvgSym(dc, m_SvgSym3, ori);
  }
  else if( status == 3 && m_SvgSym4!=NULL ) {
    drawSvgSym(dc, m_SvgSym4, ori);
  }

  if( m_bShowID ) {
    wxFont* font = new wxFont( dc.GetFont() );
    font->SetPointSize( m_iItemIDps );
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
  pen->SetWidth(1);

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
      pen = (wxPen*)wxRED_PEN;
      pen->SetWidth(5);
      dc.SetPen(*pen);
      *bridgepos = degr;
    }
    else {
      pen = (wxPen*)wxGREY_PEN;
      pen->SetWidth(5);
      dc.SetPen(*pen);
    }

    if( wTTTrack.isshow( track ) )
      dc.DrawLine( 79, 79, x, y );

    track = wTurntable.nexttrack( m_Props, track );
  }

  pen = (wxPen*)wxBLACK_PEN;
  pen->SetWidth(2);
  dc.SetPen(*pen);

  dc.DrawCircle( 79, 79, 36 );
  dc.DrawCircle( 79, 79, 32 );
  dc.DrawPolygon( 5, rotateBridge( *bridgepos ) );
  //dc.DrawPolygon( 5, rotateBridgeNose( *bridgepos ) );

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

  dc.DrawRotatedText( wxString(m_Label,wxConvUTF8), 5, 5, 0.0 );

#ifdef __WIN32__ // no scaling is done when exchanging the font in wx 2.6.3
#else
  delete font;
#endif

}


/**
 * Draw dispatcher
 */
void SymbolRenderer::drawShape( wxPaintDC& dc, bool fill, bool occupied, bool actroute, double* bridgepos, bool showID, const char* ori, int status ) {
  m_bShowID = showID;
  const char* nodeName = NodeOp.getName( m_Props );

  if( ori == NULL || StrOp.len( ori ) == 0 )
    ori = wItem.west;

  TraceOp.trc( "render", TRCLEVEL_DEBUG, __LINE__, 9999, "nodename=%s", nodeName );

  switch( m_iSymType ) {
    case symtype::i_track:
      drawTrack( dc, fill, occupied, actroute, ori );
      break;
    case symtype::i_switch:
      drawSwitch( dc, fill, occupied, actroute, ori );
      break;
    case symtype::i_signal:
      drawSignal( dc, fill, occupied, actroute, ori );
      break;
    case symtype::i_output:
      drawOutput( dc, fill, occupied, actroute, ori );
      break;
    case symtype::i_block:
      drawBlock( dc, fill, occupied, ori );
      break;
    case symtype::i_stage:
      drawStage( dc, fill, occupied, ori );
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
    case symtype::i_route:
      drawRoute( dc, fill, occupied, ori, status );
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


wxPoint* SymbolRenderer::rotateBridgeNose( double hoek ) {
  TraceOp.trc( "render", TRCLEVEL_INFO, __LINE__, 9999, "rotate bridge nose ori=%f", hoek );
  static wxPoint p[5];

  double xa;
  double ya;
  double xb;
  double yb;
  double xc;
  double yc;
  double yd;
  double xd;
  double xe;
  double ye;
  double xf;
  double yf;
  double rad;
  double pi= 3.14159265358979;;
  double alfa;
  double sinalfa;
  double cosalfa;

  hoek = 360 - hoek;

  double center = (32 * 5) / 2.0;
  double straal1 = 22 ;  //binnencirkel van vierkantje
  double straal2 = straal1 + 6;  //buitencirkel van vierkantje
  double rib = (straal2 - straal1) / 2.0;

  alfa = hoek * pi / 180.0;
  sinalfa = sin(alfa);
  cosalfa = cos(alfa);

  xa = straal1 * cosalfa;
  ya = straal1 * sinalfa;
  xb = straal2 * cosalfa;
  yb = straal2 * sinalfa;

  xd = xa - rib * sinalfa;
  yd = ya + rib * cosalfa;
  xc = xa + rib * sinalfa;
  yc = ya - rib * cosalfa;

  xe = xb - rib * sinalfa;
  ye = yb + rib * cosalfa;
  xf = xb + rib * sinalfa;
  yf = yb - rib * cosalfa;

  p[0].x = xd + center;
  p[0].y = yd + center;  //rib 1

  p[1].x = xe + center;
  p[1].y = ye + center;  //rib 2

  p[2].x = xf + center;
  p[2].y = yf + center;  //rib 3

  p[3].x = xc + center;
  p[3].y = yc + center;  //rib 4

  // end point to close the polygon
  p[4].x = p[0].x;
  p[4].y = p[0].y;

  return p;
}



