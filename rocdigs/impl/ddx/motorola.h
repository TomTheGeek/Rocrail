/* +----------------------------------------------------------------------+ */
/* | DDL - Digital Direct for Linux                                       | */
/* +----------------------------------------------------------------------+ */
/* | Copyright (c) 1999 - 2003 Vogt IT                                    | */
/* +----------------------------------------------------------------------+ */
/* | This source file is subject of the GNU general public license 2,     | */
/* | that is bundled with this package in the file COPYING, and is        | */
/* | available at through the world-wide-web at                           | */
/* | http://www.gnu.org/licenses/gpl.txt                                  | */
/* | If you did not receive a copy of the PHP license and are unable to   | */
/* | obtain it through the world-wide-web, please send a note to          | */
/* | gpl-license@vogt-it.com so we can mail you a copy immediately.       | */
/* +----------------------------------------------------------------------+ */
/* | Authors:   Torsten Vogt vogt@vogt-it.com                             | */
/* |                                                                      | */
/* +----------------------------------------------------------------------+ */

/***************************************************************/
/* erddcd - Electric Railroad Direct Digital Command Daemon    */
/*    generates without any other hardware digital commands    */
/*    to control electric model railroads                      */
/*                                                             */
/* file: maerklin.h                                            */
/* job : exports the functions from maerklin.c                 */
/*                                                             */
/* Torsten Vogt, january 1999                                  */
/*                                                             */
/* last changes: Torsten Vogt, march 2000                      */
/*               Torsten Vogt, january 2001                    */
/*                                                             */ 
/***************************************************************/

#ifndef __MOTOROLA_H__
#define __MOTOROLA_H__ 

#define LO (char)63
#define HI (char)0

#define LO_38K (char)55
#define HI_38K (char)4
#define OP_38K (char)52
#define LO_115K (char)254
#define HI_115K (char)0

/* signal generating functions for maerklin */
int comp_maerklin_1(int address, int direction, int speed, int func);
int comp_maerklin_2(int address, int direction, int speed, int func,
                    int f1, int f2, int f3, int f4);
int comp_maerklin_3(int address, int direction, int speed, int func,
                    int f1, int f2, int f3, int f4);
int comp_maerklin_4(int address, int direction, int speed, int func,
                    int f1, int f2, int f3, int f4);
int comp_maerklin_5(int address, int direction, int speed, int func,
                    int f1, int f2, int f3, int f4);
int comp_maerklin_ms(int address, int port, int gate, int action);
int comp_maerklin_mf(int address, 
                     int f1, int f2, int f3, int f4);

#endif
