/*
 Copyright (C) 2002-2012 Rob Versluis, Rocrail.net

 Without an official permission commercial use is not permitted.
 Forking this project is not permitted.
 */

#ifndef VENDORS_H_
#define VENDORS_H_
static const char* m_Vendor[256];
static void __initVendors() {
  int i = 0;
  for( i = 0; i < 256; i++ )
    m_Vendor[i] = "Unknown vendor ID";

  m_Vendor[  1]  = "CML Electronics Limited";
  m_Vendor[  2]  = "Train Technology";
  m_Vendor[ 11] = "NCE Corporation";
  m_Vendor[ 12] = "Wangrow";
  m_Vendor[ 13] = "Public Domain & Do-It-Yourself Decoders";
  m_Vendor[ 14] = "PSI - Dynatrol";
  m_Vendor[ 15] = "Ramfixx Technologies (Wangrow)";
  m_Vendor[ 17] = "Advanced IC Engineering, Inc.";
  m_Vendor[ 18] = "JMRI";
  m_Vendor[ 19] = "AMW";
  m_Vendor[ 20] = "T4T - Technology for Trains GmbH";
  m_Vendor[ 21] = "Kreischer Datentechnik";
  m_Vendor[ 22] = "KAM Industries";
  m_Vendor[ 23] = "S Helper Service";
  m_Vendor[ 24] = "MoBaTron.de";
  m_Vendor[ 25] = "Team Digital, LLC";
  m_Vendor[ 26] = "MBTronik - PiN GITmBH";
  m_Vendor[ 27] = "MTH Electric Trains, Inc.";
  m_Vendor[ 28] = "Heljan A/S";
  m_Vendor[ 29] = "Mistral Train Models";
  m_Vendor[ 30] = "Digsight";
  m_Vendor[ 31] = "Brelec";
  m_Vendor[ 32] = "Regal Way Co. Ltd";
  m_Vendor[ 34] = "Aristo-Craft";
  m_Vendor[ 35] = "Elektronik & Modell Produktion";
  m_Vendor[ 36] = "DCCConcepts";
  m_Vendor[ 37] = "NAC Services, Inc.";
  m_Vendor[ 38] = "Broadway Limited Imports, LLC";
  m_Vendor[ 39] = "Educational Computer, Inc (DCCdevices.com)";
  m_Vendor[ 40] = "KATO Precision Models";
  m_Vendor[ 41] = "Passmann Modellbahnzubehoer";
  m_Vendor[ 42] = "Digirails";
  m_Vendor[ 43] = "Ngineering";
  m_Vendor[ 44] = "SPROG-DCC";
  m_Vendor[ 45] = "ANE Model Co., LTD.";
  m_Vendor[ 62] = "Tams Elektronik GmbH";
  m_Vendor[ 66] = "Railnet Solutions, LLC";
  m_Vendor[ 68] = "MAWE Elektronik";
  m_Vendor[ 71] = "New York Byano Limited";
  m_Vendor[ 73] = "The Electric Railroad Company";
  m_Vendor[ 85] = "Uhlenbrock Elektronik GmbH";
  m_Vendor[ 87] = "RR-CirKits";
  m_Vendor[ 95] = "Sanda Kan Industrial (1981) Ltd.";
  m_Vendor[ 97] = "Doehler & Haas";
  m_Vendor[ 99] = "Lenz Elektronik GmbH";
  m_Vendor[101] = "Bachmann Trains";
  m_Vendor[103] = "Nagasue System Design Office";
  m_Vendor[105] = "Computer Dialysis France";
  m_Vendor[109] = "Viessmann Modellspielwaren GmbH";
  m_Vendor[111] = "Haber & Koenig Electronics GmbH";
  m_Vendor[113] = "QS Industries115: Dietz Modellbahntechnik";
  m_Vendor[117] = "cT Elektronik";
  m_Vendor[119] = "W. S. Ataras Engineering";
  m_Vendor[123] = "Massoth Elektronik, GmbH";
  m_Vendor[125] = "ProfiLok Modellbahntechnik GmbH";
  m_Vendor[127] = "Atlas Model Railroad Co., Inc.";
  m_Vendor[129] = "Digitrax";
  m_Vendor[131] = "Trix Modelleisenbahn";
  m_Vendor[132] = "ZTC Controls Ltd.";
  m_Vendor[133] = "Intelligent Command Control";
  m_Vendor[135] = "CVP Products";
  m_Vendor[139] = "RealRail Effects";
  m_Vendor[141] = "Throttle-Up (Soundtraxx)";
  m_Vendor[143] = "Model Rectifier Corp.";
  m_Vendor[145] = "Zimo Elektronik";
  m_Vendor[147] = "Umelec Ing. Buero";
  m_Vendor[149] = "Rock Junction Controls";
  m_Vendor[151] = "Electronic Solutions Ulm GmbH & Co KG";
  m_Vendor[153] = "Train Control Systems";
  m_Vendor[155] = "Gebr. Fleischmann GmbH & Co.";
  m_Vendor[157] = "Kuehn Ing.";
  m_Vendor[159] = "LGB (Ernst Paul Lehmann Patentwerk)";
  m_Vendor[161] = "Modelleisenbahn GmbH (formerly Roco)";
  m_Vendor[163] = "WP Railshops";
  m_Vendor[165] = "Model Electronic Railway Group";
  m_Vendor[170] = "AuroTrains";
  m_Vendor[173] = "Arnold - Rivarossi";
  m_Vendor[186] = "BRAWA Modellspielwaren GmbH & Co.";
  m_Vendor[204] = "Con-Com GmBH";
  m_Vendor[238] = "NMRA reserved";
}
#endif /* VENDORS_H_ */
