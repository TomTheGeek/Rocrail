/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2011 - Rob Versluis <r.j.versluis@rocrail.net>

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

#include "rocrail/impl/snmp_impl.h"
#include "rocrail/public/app.h"

#include "rocs/public/mem.h"
#include "rocs/public/trace.h"
#include "rocs/public/str.h"
#include "rocs/public/strtok.h"
#include "rocs/public/system.h"

#include "rocrail/wrapper/public/Global.h"
#include "rocrail/wrapper/public/SnmpService.h"
#include "rocrail/wrapper/public/RocRail.h"

static int instCnt = 0;

/** ----- Declarations ----- */
static int __setSeqInt(byte* b, int val);
static int __setSignedInt(byte* b, int val);
static int __setTimetick(byte* out, int val);
static int __getSignedInt(byte* b, int* offset);
static int __handleGetRequest(iOSNMP snmp, iOSnmpHdr hdr, byte* in, byte* out);




/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    iOSNMPData data = Data(inst);
    /* Cleanup data->xxx members...*/
    
    freeMem( data );
    freeMem( inst );
    instCnt--;
  }
  return;
}

static const char* __name( void ) {
  return name;
}

static unsigned char* __serialize( void* inst, long* size ) {
  return NULL;
}

static void __deserialize( void* inst,unsigned char* bytestream ) {
  return;
}

static char* __toString(void* inst) {
  return NULL;
}

static int __count( void ) {
  return instCnt;
}

static struct OBase* __clone( void* inst ) {
  return NULL;
}

static Boolean __equals( void* inst1, void* inst2 ) {
  return False;
}

static void* __properties( void* inst ) {
  return NULL;
}

static void __modify( void* inst, void* props ) {
  return;
}

static void* __event( void* inst, const void* event ) {
  return NULL;
}

static const char* __id( void* inst ) {
  return NULL;
}



/* --------------------------------------------------------------------------------
 * Convert a value from a byte sequence into an integer.
 */
static int __getInt(byte* in, int* offset) {
  if( in[0] == 2 ) {
    int len = in[1];
    int i = 0;
    int val = 0;
    *offset += len+2;

    for( i = 0; i < len; i++) {
      int tmp = in[2+i];
      val += tmp << (((len-1)-i) * 8);
    }
    TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "%d byte integer with a value of %d", len, val );
    return val;
  }
  else {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "integer type expected: %d", in[0] );
    return 0;
  }
}


/* --------------------------------------------------------------------------------
 * Convert a value from integer into a byte sequence. Max. 32 bit.
 */
static int __setInt(byte* out, int val) {
  int len = 0;

  out[0] = VAR_INT;

  if( val <= 0xFF ) {
    out[1] = 1;
    out[2] = val;
    len = 3;
  }
  else if( val <= 0xFFFF ) {
    out[1] = 2;
    out[2] = (val >> 8) & 0xFF;
    out[3] = val & 0xFF;
    len = 4;
  }
  else if( val <= 0xFFFFFF ) {
    out[1] = 3;
    out[2] = (val >> 16) & 0xFF;
    out[3] = (val >> 8) & 0xFF;
    out[4] = val & 0xFF;
    len = 5;
  }
  else if( val <= 0xFFFFFFFF ) {
    out[1] = 4;
    out[2] = (val >> 24) & 0xFF;
    out[3] = (val >> 16) & 0xFF;
    out[4] = (val >> 8) & 0xFF;
    out[5] = val & 0xFF;
    len = 6;
  }

  return len;
}


/* --------------------------------------------------------------------------------
 * Convert a timetick from integer into a byte sequence.
 */
static int __setTimetick(byte* out, int val) {
  int len = __setInt(out, val );
  out[0] = VAR_TIMETICK;
  return len;
}

/* --------------------------------------------------------------------------------
 * Convert a string from a char array into a byte sequence.
 */
static int __setString(byte* out, const char* val) {
  int len = StrOp.len(val);
  int i = 0;
  out[0] = VAR_STRING;
  out[1] = len;

  for( i = 0; i < len; i++ ) {
    out[2+i] = (byte)val[i];
  }

  return len + 2;
}


/* --------------------------------------------------------------------------------
 * Convert a string from byte sequence into a char array.
 */
static char* __getString(byte* in, int* offset, char* val) {
  if( in[0] == VAR_STRING ) {
    int len = in[1];
    int i = 0;
    *offset += len+2;

    for( i = 0; i < len; i++) {
      val[i] = in[2+i];
      val[i+1] = '\0';
    }
    TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "%d byte string with a value of [%s]", len, val );
    return val;
  }
  else {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "string type expected: %d,%d", in[0], *offset );
    TraceOp.dump( NULL, TRCLEVEL_BYTE, in, 32 );
    return val;
  }
}


/* --------------------------------------------------------------------------------
 * Convert an OID into a byte representation.
 */
static int __setOID(byte* out, const char* oid) {
  int offset = 0;
  int i = 0;

  out[offset] = VAR_OBJECTID; /* object int */
  offset++;
  byte* VarLen = out + offset;
  offset++;

  iOStrTok tok = StrTokOp.inst(oid, '.');
  while( StrTokOp.hasMoreTokens(tok) ) {
    const char* s = StrTokOp.nextToken(tok);
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "OID substr = [%s] i=%d", s, i );
    if( i == 0 ) {
      out[offset] = 0x2b; /* 1.3 TODO: ???? */
      offset++;
      s = StrTokOp.nextToken(tok);
      i++;
    }
    else if( i > 1 ) {
      int val = atoi(s);
      if( val > 127 ) {
        offset += __setSignedInt(out+offset, val);
      }
      else {
        out[offset] = val;
        offset++;
      }
    }
    i++;
  }

  *VarLen = offset - (VarLen - out) - 1;

  return offset;
}


static int __setIP(byte* out, const char* ip) {
  int offset = 0;
  int i = 0;

  out[offset] = VAR_IP; /* object int */
  offset++;
  byte* VarLen = out + offset;
  offset++;

  iOStrTok tok = StrTokOp.inst(ip, '.');
  while( StrTokOp.hasMoreTokens(tok) ) {
    const char* s = StrTokOp.nextToken(tok);
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "IP substr = [%s] i=%d", s, i );
    int val = atoi(s);
    if( val > 127 ) {
      offset += __setSignedInt(out+offset, val);
    }
    else {
      out[offset] = val;
      offset++;
    }
    i++;
  }

  *VarLen = offset - (VarLen - out) - 1;

  return offset;
}


/* --------------------------------------------------------------------------------
 * Convert the Object ID from byte to string representation.
 * 2B 06 01 04 01 81 A2 31 14 03 01 00 05 00
 * 1.3.6.1.4.1.20785.20.3.1.0
 *
 * 81 A2 31 = 20785
 */
static int __getOID(byte* in, char* oid ) {
  int offset = 0;
  int i = 0;

  /* check if the var type matches */
  if( in[0] == VAR_OBJECTID ) {
    int len = in[1];
    int idx = 0;
    for( i = 0; i < len; i++ ) {
      if( i == 0 && in[2+i] == 0x2B ) {
        oid[idx] = '1';
        idx++;
        oid[idx] = '.';
        idx++;
        oid[idx] = '3';
        idx++;
        oid[idx] = '\0';
      }
      else {
        int val = 0;
        int validx = 0;
        int vallen = 0;
        char sval[32];

        val = __getSignedInt(&in[2+i], &vallen);
        i += vallen-1;
        /*
        if( in[2+i] & 0x80 ) {
          val = in[2+i+1] + ((in[2+i] & 0x7F) << 7);
          i++;
        }
        else {
          val = in[2+i];
        }
        */
        StrOp.fmtb(sval, ".%d", val );
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "objectID subval=%s i=%d idx=%d", sval, i, idx );

        /* copy the value in the char array */
        for( validx = 0; validx < StrOp.len(sval); validx++ ) {
          oid[idx] = sval[validx];
          idx++;
          oid[idx] = '\0';
        }

      }
    }
    offset = 2 + len;
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "objectID = %s", oid );
  }
  else {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "wrong type for oid = %d", in[0] );
    TraceOp.dump( NULL, TRCLEVEL_BYTE, in, 32 );
  }

  return offset;
}


/* --------------------------------------------------------------------------------
 * Create a sequence with OID and string variable.
 */
static int __makeStringVariable(byte* out, const char* oid, const char* val) {
  int offset = 0;

  out[offset] = VAR_SEQUENCE;
  offset++;
  byte* VarLen = out + offset;
  offset++;

  offset += __setOID(out+offset, oid);

  offset += __setString( out+offset, val);

  /* calculate the seq len */
  *VarLen = offset - (VarLen - out) - 1;

  return offset;
}


/* --------------------------------------------------------------------------------
 * Create a sequence with OID and string variable.
 */
static int __makeTimetickVariable(byte* out, const char* oid, int val) {
  int offset = 0;

  out[offset] = VAR_SEQUENCE;
  offset++;
  byte* VarLen = out + offset;
  offset++;

  offset += __setOID(out+offset, oid);

  offset += __setTimetick( out+offset, val);

  /* calculate the seq len */
  *VarLen = offset - (VarLen - out) - 1;

  return offset;
}


/* --------------------------------------------------------------------------------
 * Create a sequence with OID and string variable.
 */
static int __makeOIDVariable(byte* out, const char* oid, const char* val) {
  int offset = 0;

  out[offset] = VAR_SEQUENCE;
  offset++;
  byte* VarLen = out + offset;
  offset++;

  offset += __setOID(out+offset, oid);

  offset += __setOID( out+offset, val);

  /* calculate the seq len */
  *VarLen = offset - (VarLen - out) - 1;

  return offset;
}


/* --------------------------------------------------------------------------------
 * Create a sequence with OID and string variable.
 */
static int __makeIntegerVariable(byte* out, const char* oid, int val) {
  int offset = 0;

  out[offset] = VAR_SEQUENCE;
  offset++;
  byte* VarLen = out + offset;
  offset++;

  offset += __setOID(out+offset, oid);

  offset += __setInt( out+offset, val);

  /* calculate the seq len */
  *VarLen = offset - (VarLen - out) - 1;

  return offset;
}


/* --------------------------------------------------------------------------------
 * Read a variable into a SnmpVar structure from a sequence.
 */
static Boolean __readVar(byte* in, iOSnmpVar snmpvar, int* offset) {
  if( in[0] == VAR_SEQUENCE ) {
    int seqlen = in[1];
    *offset += 2;
    int oidlen = __getOID(in+2, snmpvar->oid);
    if( oidlen > 0 ) {
      *offset += oidlen;
      snmpvar->vartype = in[2+oidlen];
      switch(snmpvar->vartype) {
      case VAR_NULL:
        *offset += 2;
        TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "value=NULL" );
        break;
      case VAR_INT:
        snmpvar->vartype = VAR_INT;
        snmpvar->vari = __getInt(in+2+oidlen, offset);
        TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "value=%d", snmpvar->vari );
        break;
      case VAR_STRING:
        snmpvar->vartype = VAR_STRING;
        __getString(in+2+oidlen, offset, snmpvar->vars);
        TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "value=%s", snmpvar->vars );
        break;
      default:
        TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "value type %d is not supported", snmpvar->vartype );
        break;
      }
      return True;
    }
    else {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "no OID in sequence" );
    }
  }
  else {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "not a sequence, or end of container" );
  }
  return False;
}


/* --------------------------------------------------------------------------------
 * Evaluate the SNMP header and OIDs.
 */
static Boolean __evaluateHeader(iOSnmpHdr hdr, byte* in, int* offset) {
  Boolean hasOID = False;
  hdr->oids = 0;

  if( in[0] == VAR_SEQUENCE ) {
    *offset = 1;
    hdr->seqlen = __getSignedInt(in+*offset, offset);
    TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "sequence len=%d", hdr->seqlen );
    hdr->version = __getInt(in+*offset, offset);
    TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "version=%d", hdr->version );
    __getString(in+*offset, offset, hdr->community);
    TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "community=%s", hdr->community );
    hdr->request = in[*offset];
    TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "request=0x%02X", hdr->request );
    *offset += 1;
    int reqlen = __getSignedInt(in+*offset, offset);
    TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "request len=%d", reqlen );

    hdr->reqID = __getInt(in+*offset, offset);
    TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "request ID=%d", hdr->reqID );
    hdr->errstatus = __getInt(in+*offset, offset);
    TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "error status=%d", hdr->errstatus );
    hdr->errindex = __getInt(in+*offset, offset);
    TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "error index=%d", hdr->errindex ); /* expected number of returned OIDs in case of bulk */

    *offset += 1;
    int seqlen = __getSignedInt(in+*offset, offset);
    hasOID = seqlen > 0;

    TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "var container len=%d", seqlen );

    if( seqlen > 0 ) {
      while( __readVar(in+*offset, &hdr->oid[hdr->oids], offset) ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "objectID = %s", hdr->oid[hdr->oids].oid );
        hdr->oids++;
      }
    }
  }
  else {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "not a sequence" );
  }

  return hasOID;
}


static int __setSeqInt(byte* b, int val) {
  if( val > 0xFF ) {
    b[0] = 0x82;
    b[1] = (val >> 8 ) & 0xFF;
    b[2] = val & 0xFF;
    return 3;
  }
  else if( val > 0x7F ) {
    b[0] = 0x81;
    b[1] = val & 0xFF;
    return 2;
  }
  else {
    b[0] = val;
    return 1;
  }
}

static int __setSignedInt(byte* b, int val) {
  if( val > 0x3FFF ) {
    b[0] = 0x80 | ((val >> 14 ) & 0x7F);
    b[1] = 0x80 | ((val >> 7 ) & 0x7F);
    b[2] = val & 0x7F;
    return 3;
  }
  else if( val > 0x7F ) {
    b[0] = 0x80 | ((val >> 7 ) & 0x7F);
    b[1] = val & 0x7F;
    return 2;
  }
  else {
    b[0] = val;
    return 1;
  }
}

/* 81 A2 31 = 20785 */
static int __getSignedInt(byte* b, int* offset) {
  int val = 0;
  int len = 1;
  int i   = 0;
  for(i = 0; i < 4; i++ ) {
    if( b[i] & 0x80 )
      len++;
    else
      break;
  }
  for( i = 0; i < len; i++ ) {
    int tmp = b[i] & 0x7F;
    val += (tmp << ((len-1-i)*7));
  }
  *offset += len;
  return val;
}


/* --------------------------------------------------------------------------------
 * GetRequest handler: Create a sequence for every wanted OID.
 */
static int __handleSetRequest(iOSNMP snmp, iOSnmpHdr hdr, byte* in, byte* out) {
  iOSNMPData data = Data(snmp);

  /* Loop over all requested OID's. */
  int i = 0;
  for( i = 0; i < hdr->oids; i++ ) {
    switch( hdr->oid[i].vartype ) {
      case VAR_STRING: {
        char* val = (char*)MapOp.get( data->mibDB, hdr->oid[i].oid );
        if( val != NULL ) StrOp.free(val);
        MapOp.put( data->mibDB, hdr->oid[i].oid, (obj)StrOp.dup(hdr->oid[i].vars) );
        if( StrOp.equals( wSnmpService.sysLocation, hdr->oid[i].oid ) ) {
          wSnmpService.setlocation(data->ini, hdr->oid[i].vars);
        }
        break;
      }
      case VAR_INT: {
        char* val = (char*)MapOp.get( data->mibDB, hdr->oid[i].oid );
        if( val != NULL ) StrOp.free(val);
        val = StrOp.fmt( "%d", hdr->oid[i].vari);
        MapOp.put( data->mibDB, hdr->oid[i].oid, (obj)val );
        break;
      }
    }
  }

  return __handleGetRequest(snmp, hdr, in, out);
}

/* --------------------------------------------------------------------------------
 * GetRequest handler: Create a sequence for every wanted OID.
 */
static int __handleGetRequest(iOSNMP snmp, iOSnmpHdr hdr, byte* in, byte* out) {
  iOSNMPData data = Data(snmp);
  int outlen = 0;
  int offset = 0;

  /* Start sequence */
  out[offset] = VAR_SEQUENCE;
  offset++;
  byte* TotalMsgLen = out + offset;
  offset++;

  /* Version */
  offset += __setInt( out+offset, hdr->version);

  /* Community */
  offset += __setString( out+offset, hdr->community);

  /* Response */
  out[offset] = PDU_GETRSP; /* GetResponse */
  offset++;
  byte* RspMsgLen = out + offset;
  offset++;

  /* Request ID */
  offset += __setInt( out+offset, hdr->reqID);

  /* Error status */
  offset += __setInt( out+offset, 0);

  /* Error index */
  offset += __setInt( out+offset, 0);

  /* Var container */
  out[offset] = VAR_SEQUENCE; /* int */
  offset++;
  byte* VarContLen = out + offset;
  offset++;

  if( hdr->request == PDU_BULKREQ ) {
    int max = 0;

    int startIdx = 0;
    int n = 0;
    for( n = 0; n < ListOp.size(data->oidList); n++) {
      if( StrOp.equals(hdr->oid[0].oid, (const char*)ListOp.get(data->oidList, n) )) {
        startIdx = n+1;
        break;
      }
    }

    for( n = startIdx; n < ListOp.size(data->oidList) && max < hdr->errindex; n++) {
      const char* oid = (const char*)ListOp.get(data->oidList, n);
      const char* val = (const char*)MapOp.get(data->mibDB, oid);
      if( val != NULL ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "getbulk[%d] oid=%s", max, oid );
        if( StrOp.equals(wSnmpService.sysUpTime, oid ) ) {
          offset += __makeTimetickVariable(out+offset, oid, atoi(val));
          max++;
        }
        else if( StrOp.equals(wSnmpService.sysObjectID, oid ) ) {
          offset += __makeOIDVariable(out+offset, oid, val);
          max++;
        }
        else if( StrOp.equals(wSnmpService.sysServices, oid ) ) {
          offset += __makeIntegerVariable(out+offset, oid, atoi(val));
          max++;
        }
        else {
          offset += __makeStringVariable(out+offset, oid, val);
          max++;
        }
      }
      else {
        TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "oid[%s] not in DB", oid );
      }
    }
  }

  else {
    /* Loop over all requested OID's. */
    int i = 0;
    for( i = 0; i < hdr->oids; i++ ) {
      const char* oid = hdr->oid[i].oid;
      const char* val = (const char*)MapOp.get( data->mibDB, oid );

      if( hdr->request == PDU_GETNEXTREQ ) {
        TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "try to handle next request for %s...", oid );
        int n = 0;
        for( n = 0; n < ListOp.size(data->oidList); n++) {
          if( StrOp.equals(hdr->oid[i].oid, (const char*)ListOp.get(data->oidList, n) )) {
            if( n+1 < ListOp.size(data->oidList) ) {
              oid = (const char*)ListOp.get(data->oidList, n+1);
              val = (const char*)MapOp.get( data->mibDB, oid );
              TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "next request for %s=%s", oid, val );
            }
          }
        }
      }

      if( val != NULL ) {
        if( StrOp.equals(wSnmpService.sysUpTime, oid ) )
          offset += __makeTimetickVariable(out+offset, oid, atoi(val));
        else if( StrOp.equals(wSnmpService.sysObjectID, oid ) )
          offset += __makeOIDVariable(out+offset, oid, val);
        else if( StrOp.equals(wSnmpService.sysServices, oid ) )
          offset += __makeIntegerVariable(out+offset, oid, atoi(val));
        else
          offset += __makeStringVariable(out+offset, oid, val);
      }
    }
  }

  /* Calculate length fields */
  int varContLen  = offset - (VarContLen  - out) - 1;
  if( varContLen > 127 ) {
    byte* m = allocMem(varContLen+1);
    MemOp.copy( m, VarContLen+1, varContLen);
    int seqlen = __setSeqInt(VarContLen, varContLen);
    offset += seqlen - 1;
    MemOp.copy( VarContLen+seqlen, m, varContLen);
    freeMem(m);
  }
  else
    *VarContLen  = varContLen;


  int rspMsgLen   = offset - (RspMsgLen   - out) - 1;
  if( rspMsgLen > 127 ) {
    byte* m = allocMem(rspMsgLen+1);
    MemOp.copy( m, RspMsgLen+1, rspMsgLen);
    int seqlen = __setSeqInt(RspMsgLen, rspMsgLen);
    offset += seqlen - 1;
    MemOp.copy( RspMsgLen+seqlen, m, rspMsgLen);
    freeMem(m);
  }
  else
    *RspMsgLen  = rspMsgLen;

  int totalMsgLen = offset - (TotalMsgLen - out) - 1;
  if( totalMsgLen > 127 ) {
    byte* m = allocMem(totalMsgLen+1);
    MemOp.copy( m, TotalMsgLen+1, totalMsgLen);
    int seqlen = __setSeqInt(TotalMsgLen, totalMsgLen);
    offset += seqlen - 1;
    MemOp.copy( TotalMsgLen+seqlen, m, totalMsgLen);
    freeMem(m);
    return totalMsgLen + seqlen + 1;
  }
  else {
    *TotalMsgLen  = totalMsgLen;
    return totalMsgLen + 2;
  }

}



/*
The SNMP message with PDU type 4 (trap) consists of:

   1. PDU type
   2. Enterprise - The agents OBJECT IDENTIFIER or system objects ID. Falls under a node in the MIB tree.
   3. agent addr - The IP address of the agent.
   4. Trap type - Identifies the type of event being reported.
      Trap Type Name  Description
      0 cold start  Agent is booting
      1 warm start  Agent is rebooting
      2 link down An interface has gone down
      3 link up An interface has come up
      4 authentification failure  An invalid community (password) was received in a message.
      5 egp neighbor loss An EGP peer has gone down.
      6 enterprise specific Look in the enterprise code for information on the trap
   5. Specific code - Must be 0.
   6. Time stamp - The time in 1/100ths of seconds since the agent initialized.
   7. name
   8. Value
   9. Any other names and values

 +--------+---------+--------+----+---------+--------+----------------+
 |version |community|PDU type|OID |Agent IP |trap ID |trap specific ID|..
 ---------+---------+--------+----+---------+--------+----------------+

 +-----------+-----------....----+
 |Time stamp |VarBindList ....   |
 +-----------+-------------------+


 */
static int __makeTrap(iOSNMP snmp, byte* out, int trapid, const char* oid, const char* val) {
  iOSNMPData data = Data(snmp);
  int outlen = 0;
  int offset = 0;

  /* Start sequence */
  out[offset] = VAR_SEQUENCE;
  offset++;
  byte* TotalMsgLen = out + offset;
  offset++;

  /* Version */
  offset += __setInt( out+offset, wSnmpService.getversion(data->ini));

  /* Community */
  offset += __setString( out+offset, wSnmpService.getcommunity(data->ini));

  /* Response */
  out[offset] = PDU_TRAP;
  offset++;
  byte* RspMsgLen = out + offset;
  offset++;

  /* OID */
  offset += __setOID( out+offset, (const char*)MapOp.get( data->mibDB, wSnmpService.sysObjectID) );

  /* VAR_IP */
  offset += __setIP( out+offset, SocketOp.gethostaddr() );

  /* trap ID */
  offset += __setInt( out+offset, trapid );

  /* specific code */
  offset += __setInt( out+offset, 0);

  /* timetick */
  offset += __setTimetick(out+offset, SystemOp.getTick() );


  /* Var container */
  out[offset] = VAR_SEQUENCE; /* int */
  offset++;
  byte* VarContLen = out + offset;
  offset++;

  offset += __makeStringVariable(out+offset, oid, val);

  /* Calculate length fields */
  int varContLen  = offset - (VarContLen  - out) - 1;
  if( varContLen > 127 ) {
    byte* m = allocMem(varContLen+1);
    MemOp.copy( m, VarContLen+1, varContLen);
    offset += __setSignedInt(VarContLen, varContLen) - 1;
    MemOp.copy( VarContLen+2, m, varContLen);
    freeMem(m);
  }
  else
    *VarContLen  = varContLen;


  int rspMsgLen   = offset - (RspMsgLen   - out) - 1;
  if( rspMsgLen > 127 ) {
    byte* m = allocMem(rspMsgLen+1);
    MemOp.copy( m, RspMsgLen+1, rspMsgLen);
    offset += __setSignedInt(RspMsgLen, rspMsgLen) - 1;
    MemOp.copy( RspMsgLen+2, m, rspMsgLen);
    freeMem(m);
  }
  else
    *RspMsgLen  = rspMsgLen;

  int totalMsgLen = offset - (TotalMsgLen - out) - 1;
  if( totalMsgLen > 127 ) {
    byte* m = allocMem(totalMsgLen+1);
    MemOp.copy( m, TotalMsgLen+1, totalMsgLen);
    offset += __setSignedInt(TotalMsgLen, totalMsgLen) - 1;
    MemOp.copy( TotalMsgLen+2, m, totalMsgLen);
    freeMem(m);
    return totalMsgLen + 3;
  }
  else {
    *TotalMsgLen  = totalMsgLen;
    return totalMsgLen + 2;
  }

}

/* --------------------------------------------------------------------------------
 * Check request type and call the fitting handler.
 */
static int __handleRequest(iOSNMP snmp, byte* in, int inlen, byte* out) {
  iOSNMPData data = Data(snmp);

  if( in[0] == 0x30 && (inlen - 2) == in[1] ) {
    struct SnmpHdr hdr;
    int hdroffset = 0;
    Boolean hasVars = __evaluateHeader(&hdr, in, &hdroffset);
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
        "sequence len=%d ver=%d comm=%s req=0x%02X reqid=%d", hdr.seqlen, hdr.version, hdr.community, hdr.request, hdr.reqID  );

    switch( hdr.request ) {
    case PDU_GETREQ:
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "GetRequest" );
      return __handleGetRequest(snmp, &hdr, in, out);
    case PDU_GETNEXTREQ:
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "GetNextRequest (not supported)" );
      return __handleGetRequest(snmp, &hdr, in, out);
    case PDU_GETRSP:
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "GetResponse (not supported; I'm an agent...)" );
      break;
    case PDU_SETREQ:
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "SetRequest" );
      return __handleSetRequest(snmp, &hdr, in, out);
    case PDU_TRAP:
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Trap (not supported; I'm an agent...)" );
      break;
    case PDU_BULKREQ:
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "BulkRequest" );
      return __handleGetRequest(snmp, &hdr, in, out);
    default:
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "unknown request: 0x%02X", hdr.request );
      break;
    }
  }
  else {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "invalid SNMP packet" );
    return 0;
  }

  return 0;
}


/* --------------------------------------------------------------------------------
 * The MIB databank.
 */
static void __initMibDB(iOSNMP snmp) {
  iOSNMPData data = Data(snmp);

  /*
    * 1.3.6.1.2.1.1.1 - sysDescr
    * 1.3.6.1.2.1.1.2 - sysObjectID
    * 1.3.6.1.2.1.1.3 - sysUpTime
    * 1.3.6.1.2.1.1.4 - sysContact
    * 1.3.6.1.2.1.1.5 - sysName
    * 1.3.6.1.2.1.1.6 - sysLocation
    * 1.3.6.1.2.1.1.7 - sysServices
    *
    * Private:
    * 1.3.6.1.4.1.37707.1.1.0 - Build time
    * 1.3.6.1.4.1.37707.1.2.0 - Connection count
   */
  char* sysName = StrOp.fmt("%s %s %d.%d.%d",
      wGlobal.productname, wGlobal.releasename, wGlobal.vmajor,
      wGlobal.vminor, AppOp.getrevno());
  char* sysObjectID = StrOp.fmt("1.3.6.1.4.1.%d.%d.%d",
      wSnmpService.getenterprise(data->ini), wSnmpService.getfamily(data->ini), wSnmpService.getproduct(data->ini));
  char* buildTime = StrOp.fmt("%s %s", wGlobal.buildDate, wGlobal.buildTime);

  /* Map the system values */
  MapOp.put( data->mibDB, wSnmpService.sysDescr   , (obj)wSnmpService.getdescription(data->ini));
  MapOp.put( data->mibDB, wSnmpService.sysObjectID, (obj)sysObjectID);
  MapOp.put( data->mibDB, wSnmpService.sysUpTime  , (obj)StrOp.dup("0"));
  MapOp.put( data->mibDB, wSnmpService.sysContact , (obj)wSnmpService.getcontact(data->ini));
  MapOp.put( data->mibDB, wSnmpService.sysName    , (obj)sysName);
  MapOp.put( data->mibDB, wSnmpService.sysLocation, (obj)wSnmpService.getlocation(data->ini));
  MapOp.put( data->mibDB, wSnmpService.sysServices, (obj)"72");

  /* Map the private values */
  MapOp.put( data->mibDB, wSnmpService.privBuildTime    , (obj)buildTime);
  MapOp.put( data->mibDB, wSnmpService.privThreadCnt    , (obj)StrOp.dup("0"));
  MapOp.put( data->mibDB, wSnmpService.privConnectionCnt, (obj)StrOp.dup("0"));

  /* Map the lists for the bulk request */
  iOList systemList = ListOp.inst();
  ListOp.add( systemList, (obj) wSnmpService.sysDescr    );
  ListOp.add( systemList, (obj) wSnmpService.sysObjectID );
  ListOp.add( systemList, (obj) wSnmpService.sysUpTime   );
  ListOp.add( systemList, (obj) wSnmpService.sysContact  );
  ListOp.add( systemList, (obj) wSnmpService.sysName     );
  ListOp.add( systemList, (obj) wSnmpService.sysLocation );
  ListOp.add( systemList, (obj) wSnmpService.sysServices );

  iOList enterpriseList = ListOp.inst();
  ListOp.add( enterpriseList, (obj) wSnmpService.privBuildTime ); /* build time */
  ListOp.add( enterpriseList, (obj) wSnmpService.privThreadCnt ); /* thread count */
  ListOp.add( enterpriseList, (obj) wSnmpService.privConnectionCnt ); /* thread count */

  MapOp.put( data->mibMap, wSnmpService.systemList , (obj)systemList);
  MapOp.put( data->mibMap, wSnmpService.privateList, (obj)enterpriseList);

  /* flat list for next request*/
  ListOp.add( data->oidList, (obj) wSnmpService.sysDescr    );
  ListOp.add( data->oidList, (obj) wSnmpService.sysObjectID );
  ListOp.add( data->oidList, (obj) wSnmpService.sysUpTime   );
  ListOp.add( data->oidList, (obj) wSnmpService.sysContact  );
  ListOp.add( data->oidList, (obj) wSnmpService.sysName     );
  ListOp.add( data->oidList, (obj) wSnmpService.sysLocation );
  ListOp.add( data->oidList, (obj) wSnmpService.sysServices );

  ListOp.add( data->oidList, (obj) wSnmpService.privBuildTime ); /* build time */
  ListOp.add( data->oidList, (obj) wSnmpService.privThreadCnt ); /* thread count */
  ListOp.add( data->oidList, (obj) wSnmpService.privMemStats  ); /* memory statistics */
  ListOp.add( data->oidList, (obj) wSnmpService.privLastExc   ); /* last logged exception */
}


static void __updateVars(iOSNMP snmp) {
  iOSNMPData data = Data(snmp);

  char* uptime = (char*)MapOp.get( data->mibDB, wSnmpService.sysUpTime );
  if( uptime != NULL ) StrOp.free(uptime);
  uptime = StrOp.fmt("%ld", SystemOp.getTick());
  MapOp.put( data->mibDB, wSnmpService.sysUpTime, (obj)uptime);

  char* cnt = (char*)MapOp.get( data->mibDB, wSnmpService.privThreadCnt );
  if( cnt != NULL ) StrOp.free(cnt);
  cnt = StrOp.fmt("%d", ListOp.size(ThreadOp.getAll()));
  MapOp.put( data->mibDB, wSnmpService.privThreadCnt, (obj)cnt);

  char* memstats = (char*)MapOp.get( data->mibDB, wSnmpService.privMemStats );
  if( memstats != NULL ) StrOp.free(memstats);
  memstats = StrOp.fmt("count=%ld, size=%ld kB", MemOp.getAllocCount(), MemOp.getAllocSize()/1024 );
  MapOp.put( data->mibDB, wSnmpService.privMemStats, (obj)memstats);

  char* lastexc = (char*)MapOp.get( data->mibDB, wSnmpService.privLastExc );
  if( lastexc != NULL ) StrOp.free(lastexc);
  lastexc = StrOp.fmt("%s", data->lastexc==NULL?"none":data->lastexc );
  MapOp.put( data->mibDB, wSnmpService.privLastExc, (obj)lastexc);

  char* conn = (char*)MapOp.get( data->mibDB, wSnmpService.privConnectionCnt );
  if( conn != NULL ) StrOp.free(conn);
  conn = StrOp.fmt("currently=%d, total=%d", data->linkup, data->linkcnt);
  MapOp.put( data->mibDB, wSnmpService.privConnectionCnt, (obj)conn);
}



/* --------------------------------------------------------------------------------
 * SNMP Agent service.
 */
static void __server( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOSNMP snmp = (iOSNMP)ThreadOp.getParm( th );
  iOSNMPData data = Data(snmp);
  byte in[1024];
  byte out[1024];
  char client[256];
  int port = 0;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "SNMP service started on port %d", wSnmpService.getport(data->ini) );

  do {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "SNMP waiting..." );
    MemOp.set(in, 0, 1024);
    int inlen = SocketOp.recvfrom( data->snmpSock, in, 1024, client, &port );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "SNMP received from %s:%d", client, port );

    if( inlen > 0 ) {
      __updateVars(snmp);

      TraceOp.dump( NULL, TRCLEVEL_BYTE, in, inlen );
      int outlen =  __handleRequest(snmp, in, inlen, out);

      if( outlen > 0 ) {
        if( TraceOp.getLevel(NULL) & TRCLEVEL_BYTE ) {
          /* self test of header */
          struct SnmpHdr hdr;
          int hdroffset = 0;
          Boolean hasVars = __evaluateHeader(&hdr, out, &hdroffset);
        }

        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "sending SNMP response of %d bytes...", outlen );
        TraceOp.setDumpsize( NULL, outlen );
        TraceOp.dump( NULL, TRCLEVEL_BYTE, out, outlen );
        if( SocketOp.sendto( data->snmpSock, out, outlen, client, port ) ) {
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "SNMP response is send" );
        }
      }
    }


    ThreadOp.sleep(10);
  } while(data->run);
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "SNMP service ended" );

  SocketOp.base.del(data->snmpSock);
}



/* --------------------------------------------------------------------------------
 * SNMP contructor.
 */
static struct OSNMP* _inst( iONode ini ) {
  iOSNMP RocsOgen_SNMP = allocMem( sizeof( struct OSNMP ) );
  iOSNMPData data = allocMem( sizeof( struct OSNMPData ) );
  MemOp.basecpy( RocsOgen_SNMP, &SNMPOp, 0, sizeof( struct OSNMP ), data );

  /* Initialize data->xxx members... */
  data->ini      = ini;
  data->mibDB    = MapOp.inst();
  data->mibMap   = MapOp.inst();
  data->oidList  = ListOp.inst();

  __initMibDB(RocsOgen_SNMP);


  if( wSnmpService.gettrapport(data->ini) > 0 ) {
    data->snmpTrapSock = SocketOp.inst( wSnmpService.gettraphost(data->ini), wSnmpService.gettrapport(data->ini), False, True, False );
    if( data->snmpTrapSock != NULL ) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "SNMP Trap activated op port %d", wSnmpService.gettrapport(data->ini) );
      data->trap = True;
      byte out[256];
      int outlen = __makeTrap(RocsOgen_SNMP, out, TRAP_COLDSTART, wSnmpService.trapColdStart, "Normal startup." );
      TraceOp.dump( NULL, TRCLEVEL_BYTE, out, outlen );
      if( SocketOp.sendto( data->snmpTrapSock, out, outlen, NULL, 0 ) ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "SNMP trap send" );
      }
    }
  }

  /* activate the ticker */
  SystemOp.inst();

  data->snmpSock = SocketOp.inst( "localhost", wSnmpService.getport(data->ini), False, True, False );
  if( data->snmpSock != NULL && SocketOp.bind(data->snmpSock) ) {
    data->run = True;
    data->server = ThreadOp.inst( "snmpsrvr", &__server, RocsOgen_SNMP );
    ThreadOp.start( data->server );
  }
  else {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Cannot use port %d for the SNMP-Agent. It may be caused by:",  wSnmpService.getport(data->ini) );
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "- Address or port already in use." );
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "- On Unix, only root user can open port between O and 1024." );
  }

  instCnt++;
  return RocsOgen_SNMP;
}


/* --------------------------------------------------------------------------------
 * Shutdown the SNMP service.
 */
static void _shutdown( struct OSNMP* inst ) {
  iOSNMPData data = Data(inst);
  data->run = False;
  byte out[256];
  int outlen = __makeTrap(inst, out, TRAP_USER, wSnmpService.privTrapShutDown, "Shutdown" );
  TraceOp.dump( NULL, TRCLEVEL_BYTE, out, outlen );
  if( SocketOp.sendto( data->snmpTrapSock, out, outlen, NULL, 0 ) ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "SNMP trap send" );
  }
}


static void _link( struct OSNMP* inst, int count, Boolean up ) {
  iOSNMPData data = Data(inst);
  char sCnt[32];
  byte out[256];
  data->linkup += up ?1:-1;
  data->linkcnt += up ?1:0;
  StrOp.fmtb( sCnt, "currently=%d, total=%d", data->linkup, count );
  int outlen = __makeTrap(inst, out, up?TRAP_LINKUP:TRAP_LINKDOWN, up?wSnmpService.trapLinkUp:wSnmpService.trapLinkDown, sCnt );
  TraceOp.dump( NULL, TRCLEVEL_BYTE, out, outlen );
  if( SocketOp.sendto( data->snmpTrapSock, out, outlen, NULL, 0 ) ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "SNMP trap send" );
  }
}

static void _exception( struct OSNMP* inst, const char* msg ) {
  iOSNMPData data = Data(inst);
  byte out[256];
  int outlen = __makeTrap(inst, out, TRAP_USER, wSnmpService.privTrapException, msg );
  TraceOp.dump( NULL, TRCLEVEL_BYTE, out, outlen );
  if( SocketOp.sendto( data->snmpTrapSock, out, outlen, NULL, 0 ) ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "SNMP trap send" );
  }
  if(data->lastexc!= NULL)
    StrOp.free(data->lastexc);
  data->lastexc = StrOp.dup(msg);
}

/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocrail/impl/snmp.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
