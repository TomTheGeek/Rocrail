/*
 * z21.h
 *
 *  Created on: Mar 9, 2013
 *      Author: rob
 */

#ifndef Z21_H_
#define Z21_H_


#define bcAll             0x00000001
#define bcRBus            0x00000002
#define bcRailcom         0x00000004
#define bcSystemInfo      0x00000100

#define bcXPressNetAll    0x00010000 // Alles, auch alle Loks ohne vorher die Lokadresse abonnieren zu müssen (für PC Steuerung)

#define bcLocoNet         0x01000000 // LocoNet Meldungen an LAN Client weiterleiten (ohne Loks und Weichen)
#define bcLocoNetLocos    0x02000000 // Lok-spezifische LocoNet Meldungen an LAN Client weiterleiten
#define bcLocoNetSwitches 0x04000000 // Weichen-spezifische LocoNet Meldungen an LAN Client weiterleiten
/*
 * 3 separate Flags damit da nichts unnötig doppelt herumgeschickt wird (XPressNet und LocoNet)
 */

/*
 * bcLocoNetLocos = zusätzlich LN Meldungen OPC_LOCO_SPD, OPC_LOCO_DIRF, OPC_LOCO_SND, OPC_LOCO_F912, OPC_LOCO_SP_FN
 * bcLocoNetSwitches =  zusätzlich LN Meldungen OPC_SW_REQ, OPC_SW_REP, OPC_SW_ACK, OPC_SW_STATE
 */



/* LocoNet-Meldungen werden dann im LAN getunnelt (ähnlich den XBUS Meldungen) mit Header */
#define LAN_LOCONET_Z21_RX    0xA0   // Meldungen welche die Z21 vom LocoNet empfangen hat; von Z21 an LAN-client
#define LAN_LOCONET_Z21_TX    0xA1   // Meldungen welche die Z21 über LocoNet gesendet hat; von Z21 an LAN-client
#define LAN_LOCONET_FROM_LAN  0xA2   // Meldungen eines LAN-Client an den "LocoNet" Bus; beide Richtungen

/* Mit 0xA2 kannst Du also Meldungen in den LocoNet-Bus schicken */

#define LAN_LOCONET_DISPATCH_ADDR 0xA3 // Lokadresse dispatchen (Parameter: Lokadresse, little endian)



#endif /* Z21_H_ */
