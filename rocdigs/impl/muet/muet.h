/*
[00:000] - Open COM1 port (D:\Programme\Railroad & Co.70\RailTCS32.exe)
Kommentar by rpa - System wurde gestartet mit zwei nachfolgenden Loks
Kommentar by rpa - Lok mit Adresse 21 auf Block 4 mit Adresse 60 Port 4 und Zugerkennung Adresse 61 Port 4
Kommentar by rpa - Lok mit Adresse 22 auf Block 13 mit Adresse 66 Port 5 und Zugerkennung Adresse 67 Port 5


[00:307] - Written data
113 000 126                                                       q.~

[03:219] - Read data
126 000                                                           ~.

[05:224] - Written data
255 128 113 001 240 113 002 021 113 002 020                       ˇÄq.q..q..

[05:231] - Read data
128 021 064 128 060 008 128 061                                   Ä.@Ä<.Ä=

[05:232] - Written data
113 002 022                                                       q..

[05:232] - Read data
091 128 066 016 128 067 076                                       [ÄB.ÄCL

[05:233] - Written data
060                                                               <

[05:234] - Read data
060 008                                                           <.

[05:234] - Written data
068                                                               D

[05:235] - Read data
068 022 128 020 000 128 022 000                                   D.Ä..Ä..

[05:235] - Written data
062                                                               >

[05:236] - Read data
062 021                                                           >.

[05:236] - Written data
113 002 026 113 002 024                                           q..q..

[05:237] - Read data
128 026 000                                                       Ä..

[05:238] - Written data
113 002 023 113 002 025 113 002 029                               q..q..q..

[05:239] - Read data
128 023 000 128 024 000 128 025 000                               Ä..Ä..Ä..

[05:240] - Written data
066                                                               B

[05:240] - Read data
066 016                                                           B.

[05:241] - Written data
113 002 060 189 128                                               q.<ΩÄ

[05:242] - Read data
128 029 064 128 060 008                                           Ä.@Ä<.

[05:242] - Written data
195 128 113 002 066                                               √Äq.B

[05:243] - Read data
128 061 128 128 067 128                                           Ä=ÄÄCÄ

[05:244] - Written data
113 002 061 113 002 067                                           q.=q.C

[05:245] - Read data
128 061 128 128 066 016 128 067 204 128 067 204 128 061 219 128   Ä=ÄÄB.ÄCÃÄCÃÄ=€Ä
067 012                                                           C.

[05:251] - Written data
068                                                               D

[05:251] - Read data
068 022                                                           D.

[05:253] - Written data
072                                                               H

[05:253] - Read data
072 000 128 061 027                                               H.Ä=.

[05:254] - Written data
062                                                               >

[05:254] - Read data
062 021                                                           >.

[05:255] - Written data
113 002 072                                                       q.H

[05:256] - Read data
128 072 000                                                       ÄH.

[05:281] - Written data
149 064 148 000 150 000 154 000 152 000 151 000 153 000 157 064   ï@î.ñ.ö.ò.ó.ô.ù@
127                                                               

[05:297] - Read data
127 128                                                           Ä




 ************************************************************
 * Kommentar by rpa
 * Lok mit Adresse 21 von Block 4 mit Adresse 60 Port 4 und Zugerkennung Adresse 61 Port 4 entfernt.
[34:243] - Read data
127 128 128 061 051

[34:258] - Written data
062

[34:262] - Read data
062 021 128 060 000

[36:218] - Written data
127


 ************************************************************
 * Kommentar by rpa
 * Lok mit Adresse 21 auf Block 4 mit Adresse 60 Port 4 und Zugerkennung Adresse 61 Port 4 gestellt
[52:249] - Read data
127 128 128 060 008 128 061 091

[53:885] - Written data
062

[53:889] - Read data
062 021

[54:329] - Written data
127


 ************************************************************
 * Kommentar by rpa
 * Lok mit Adresse 22 von Block 13 mit Adresse 66 Port 5 und Zugerkennung Adresse 67 Port 5 entfernt
[01:24:455] - Read data
127 128 128 067 036

[01:26:003] - Written data
068

[01:26:007] - Read data
068 022 128 066 000

[01:26:451] - Written data
127


 ************************************************************
 * Kommentar by rpa
 * Lok mit Adresse 22 auf Block 13 mit Adresse 66 Port 5 und Zugerkennung Adresse 67 Port 5 gestellt
[01:34:530] - Read data
127 128 128 066 016

[01:36:470] - Written data
127

[01:36:473] - Read data
127 128 128 067 076

[01:37:019] - Written data
068

[01:37:023] - Read data
068 022

[01:38:527] - Written data
127


*/
#ifndef MUET_H_
#define MUET_H_

#define SX_GET_BUS 126
#define SX_SET_BUS0 240
#define SX_SET_BUS1 241
#define SX_SET_BUS2 242
#define SX_SET_BUS3 243
#define SX_SET_BUS4 244
#define SX_SET_BUS5 245
#define SX_SET_BUS6 246
#define SX_SET_BUS7 247
#define SX_SET_BUS8 248
#define SX_SET_BUS9 249

#define CS_SET_STATUS 255
#define CS_GET_STATUS 127
#define CS_ON 128
#define CS_OFF 0

#define BIT_MOD 112
#define BIT_LOW 0
#define BIT_HIGH 8
#define BIT_INV 16

#define MONITORING 113
#define MONITORING_OFF 0
#define MONITORING_ON 1
#define MONITORING_ADD 2
#define MONITORING_DEL 3

#endif
