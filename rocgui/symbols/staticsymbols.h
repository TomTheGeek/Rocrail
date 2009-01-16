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
#ifndef STATICSYMBOLS_H_
#define STATICSYMBOLS_H_


  // Track: straight
  static wxPoint tk_straight[] = {
    wxPoint( 0,15),wxPoint( 2,13),wxPoint(29,13),wxPoint( 31,15),wxPoint( 31,16),
    wxPoint(29,18),wxPoint( 2,18),wxPoint( 0,16)
  };
  const int track_size = 8;
  static wxPoint tk_straight_fill[] = {
    wxPoint( 9,13),wxPoint(23,13),wxPoint(23,18),wxPoint( 9,18)
  };
  const int track_fill_size = 4;

  // Track: buffer
  static wxPoint tk_buffer[] = {
    wxPoint( 0,15),wxPoint( 2,13),wxPoint(15,13),wxPoint( 15,8),wxPoint( 20,8),
    wxPoint(20,23),wxPoint(15,23),wxPoint(15,18),wxPoint( 2,18),wxPoint( 0,16)
  };
  const int buffer_size = 10;

  // Track: connector
  static wxPoint tk_connector[] = {
    wxPoint( 0,15),wxPoint( 2,13),wxPoint(11,13),wxPoint( 11,10),wxPoint( 13,8),
    wxPoint( 22,8),wxPoint( 24,10),wxPoint( 24,21),wxPoint( 22,23),
    wxPoint(13,23),wxPoint(11,21),wxPoint(11,18),wxPoint( 2,18),wxPoint( 0,16)
  };
  const int connector_size = 14;

  // direcrion
  static wxPoint tk_dir_1[] = {
    wxPoint( 6,15),wxPoint( 13,8),wxPoint(13,23),wxPoint( 6,16)
  };
  static wxPoint tk_dir_2[] = {
    wxPoint( 26,15),wxPoint( 19,8),wxPoint(19,23),wxPoint( 26,16)
  };

  // Track: curve
  static wxPoint tk_curveL[] = {
      wxPoint( 0,16),wxPoint( 0,19),wxPoint(12,31),
      wxPoint(15,31),wxPoint(15,28),wxPoint( 3,16)
  };
  static wxPoint tk_curveSWL[] = {
      wxPoint( 16,16),wxPoint( 16,19),wxPoint(28,31),
      wxPoint(31,31),wxPoint(31,28),wxPoint( 19,16)
  };
  static wxPoint tk_curveR[] = {
      wxPoint(28,16),wxPoint(31,16),wxPoint(31,19),
      wxPoint(19,31),wxPoint(16,31),wxPoint(16,28)
  };
  static wxPoint tk_curveSWR[] = {
      wxPoint(12,16),wxPoint(15,16),wxPoint(15,19),
      wxPoint(3,31),wxPoint(0,31),wxPoint(0,28)
  };
  const int curve_size = 6;

  // crossing curve
  static wxPoint tk_crossingcurveL[] = {
      wxPoint(19,31),wxPoint(16,31),wxPoint(16,27),
      wxPoint(61,13),wxPoint(63,15),wxPoint(63,16),wxPoint(61,18)
  };
  static wxPoint tk_crossingcurve[] = {
      wxPoint( 0,15),wxPoint( 2,13),wxPoint(48,28),
      wxPoint(48,31),wxPoint(45,31),wxPoint( 2,18),wxPoint( 0,16)
  };
  const int crossingcurve_size = 7;


  // FeedBack
  static wxPoint fb1[] = {
    wxPoint( 0,15),wxPoint( 2,13),wxPoint( 6,13),wxPoint( 6, 8),wxPoint(25, 8),
    wxPoint(25,13),wxPoint(29,13),wxPoint(31,15),wxPoint(31,16),wxPoint(29,18),wxPoint(25,18),
    wxPoint(25,23),wxPoint( 6,23),wxPoint( 6,18),wxPoint( 2,18),wxPoint(0,16)
  };
  static wxPoint fb2[] = {
    wxPoint( 9,11),wxPoint(22,11),wxPoint(22,20),wxPoint( 9,20)
  };

  // Decoupler
  static wxPoint decoupler1[] = {
    wxPoint( 0,15),wxPoint( 2,13),wxPoint(29,13),wxPoint( 31,15),wxPoint( 31,16),
    wxPoint(29,18),wxPoint( 2,18),wxPoint( 0,16)
  };
  const int decoupler1_size = 8;
  static wxPoint decoupler2[] = {
    wxPoint( 8,11),wxPoint(23,11),wxPoint(23,20),wxPoint( 8,20)
  };
  const int decoupler2_size = 4;

  // Block
  static wxPoint bk[] = {
    wxPoint( 0, 3),wxPoint(126, 3),wxPoint(126,28),wxPoint( 0,28)
  };

  /* Signal */
  static wxPoint sg1[] = {
    wxPoint( 4,3 ),wxPoint( 6,1),wxPoint(18,1),wxPoint(20,3),
    wxPoint(20,8),wxPoint(18,10),wxPoint( 6,10),wxPoint( 4,8)
  };
  static wxPoint sg2[] = {
    wxPoint(20,4),wxPoint(30,4),
    wxPoint(30,7),wxPoint(20,7)
  };


#endif /*STATICSYMBOLS_H_*/
