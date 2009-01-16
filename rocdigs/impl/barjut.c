/*
 Rocrail - Model Railroad Software

 Copyright (C) 2006-2007 - Ralf Tralow

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

#include "rocdigs/impl/barjut_impl.h"

#include "rocs/public/trace.h"
#include "rocs/public/node.h"
#include "rocs/public/attr.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/system.h"
#include "rocs/public/map.h"


#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/SysCmd.h"
#include "rocrail/wrapper/public/Response.h"
#include "rocrail/wrapper/public/CustomCmd.h"
#include "rocrail/wrapper/public/Block.h"
#include "rocrail/wrapper/public/Feedback.h"


#define STX	0x02
#define DLE 0x10

static void     __BarjutReader(
	void *threadinst );

static int      instCnt = 0;

/* Status */
static int
_state(
	obj inst )
{
	iOBarjutData    data = Data( inst );
	int             state = 0;

	return state;
}

/* VERSION: */
/** vmajor*1000 + vminor*100 + patch */
static int      vmajor = 1;
static int      vminor = 2;
static int      patch = 0;
static int
_version(
	obj inst )
{
	iOBarjutData    data = Data( inst );

	return vmajor * 10000 + vminor * 100 + patch;
}

/** ----- OBarjut ----- */
static const char* __id( void* inst ) {
  return NULL;
}

static void* __event( void* inst, const void* evt ) {
  return NULL;
}

static void
__del(
	void *inst )
{
	if( inst != NULL )
	{
		iOBarjutData    data = Data( inst );

		/*
		   Cleanup data->xxx members...
		 */

		freeMem( data );
		freeMem( inst );
		instCnt--;
	}
	return;
}

static const char *
__name(
	void )
{
	return name;
}

static unsigned char *
__serialize(
	void *inst,
	long *size )
{
	return NULL;
}

static void
__deserialize(
	void *inst,
	unsigned char *bytestream )
{
	return;
}

static char    *
__toString(
	void *inst )
{
	return NULL;
}

static int
__count(
	void )
{
	return instCnt;
}

static struct OBase *
__clone(
	void *inst )
{
	return NULL;
}

static          Boolean
__equals(
	void *inst1,
	void *inst2 )
{
	return False;
}

static void    *
__properties(
	void *inst )
{
	return NULL;
}

/** ----- OBarjut ----- */


/**  */
static          iONode
_cmd(
	obj inst,
	const iONode cmd )
{
	return 0;
}


/**  */
static void
_halt(
	obj inst )
{
	iOBarjutData    data = Data( inst );

	data->run = False;
	while( data->runFinished == False )
	{
		ThreadOp.sleep( 10 );
	}
	SerialOp.close( data->serial );
	TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Shutting down <%s>...",
					 data->iid );
	return;
}


/**  */
static          Boolean
_setListener(
	obj inst,
	obj listenerObj,
	const digint_listener listenerFun )
{
	iOBarjutData    data = Data( inst );

	data->listenerObj = listenerObj;
	data->listenerFun = listenerFun;
	return True;
}


/**  */
static          Boolean
_supportPT(
	obj inst )
{
	iOBarjutData    data = Data( inst );

	return False;
}



/**  */
static struct OBarjut *
_inst(
	const iONode ini,
	const iOTrace trc )
{
	iOBarjut        __Barjut = allocMem( sizeof ( struct OBarjut ) );
	iOBarjutData    data = allocMem( sizeof ( struct OBarjutData ) );

	/*
	   Initialize data->xxx members...
	 */
	iOAttr          attr = NULL;
	const char     *flow = "none";
	const char     *parity = "none";
	int             timeout = 50;

	data->serialOK = False;
	data->initOK = False;

	MemOp.basecpy( __Barjut, &BarjutOp, 0, sizeof ( struct OBarjut ), data );

	TraceOp.set( trc );

	//   data->mux = MutexOp.inst( StrOp.fmt( "serialMux%08X", data ), True );

	//   data->fbstate = allocMem( 31 * 16 * sizeof ( struct FBState ) );

	/*
	   Evaluate attributes.
	 */
	data->device = StrOp.dup( wDigInt.getdevice( ini ) );
	data->iid = StrOp.dup( wDigInt.getiid( ini ) );

	data->bps = 115200;
	//   data->bits = 8;
	//   data->stopBits = 1;
	//   data->timeout = wDigInt.gettimeout( ini );
	//   data->timeout = 50;
	//   data->parity = none;
	//   data->flow = noflow;
	//   data->ctsretry = wDigInt.getctsretry( ini );
	//   data->ctsretry = 0;

	data->timeout = wDigInt.gettimeout( ini );	// f�r die Pollingrate der Module
	if( data->timeout <= 0 )
	{
		data->timeout = 1;
	}


	TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
					 "----------------------------------------" );
	TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "BarJuT %d.%d.%d",
					 vmajor, vminor, patch );
	TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
					 "----------------------------------------" );

	TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
					 "BarJuT[%s]: %s,%d,%s,%d,%d,%s,%d",
					 wDigInt.getiid( ini ) != NULL ? wDigInt.getiid( ini ) : "",
					 data->device, data->bps, parity, 8, 1, flow, timeout );

	data->serial = SerialOp.inst( data->device );
	SerialOp.setFlow( data->serial, -1 );
	SerialOp.setLine( data->serial, data->bps, 8, 1, none );
	SerialOp.setTimeout( data->serial, timeout, timeout );
	data->serialOK = SerialOp.open( data->serial );


	if( data->serialOK )
	{
		data->run = True;
		data->runFinished = False;

		SystemOp.inst(  );
		data->reader = ThreadOp.inst( "Barjutfb", &__BarjutReader, __Barjut );
		ThreadOp.start( data->reader );
	}
	else
		TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999,
						 "Could not init BarJuT port!" );

	instCnt++;
	return __Barjut;
}

iIDigInt
rocGetDigInt(
	const iONode ini,
	const iOTrace trc )
{
	return ( iIDigInt ) _inst( ini, trc );
}


/***********************************/
/****** Start of own routines ******/
/***********************************/

static          Boolean
__sendCommand(
	iOBarjutData o,
	char command,
	unsigned char address,
	char *data,
	unsigned char dataSize )
{
	Boolean         ok = True;
	int             i;
	unsigned char   checksum;
	unsigned char   ch;
	unsigned char   sendData[260];

	// Flush the receive buffer
	i = SerialOp.available( o->serial );
	while( i > 0 )
	{
		if( i < sizeof ( sendData ) )
		{
			SerialOp.read( o->serial, sendData, i );
			break;
		}
		else
		{
			if( !SerialOp.read( o->serial, sendData, sizeof ( sendData ) ) )
				break;
		}
		i -= sizeof ( sendData );
	}


	sendData[0] = STX;
	sendData[1] = command;
	checksum = command;
	sendData[2] = address;
	checksum += address;
	sendData[3] = dataSize;
	checksum += dataSize;
	i = 0;
	for( i = 0; i < dataSize; i++ )
	{
		sendData[4 + i] = data[i];
		checksum += data[i];
	}
	sendData[4 + i] = checksum;


	//  if( CheckCTS( o ) )
	//  {
	// Send start
	ok = SerialOp.write( o->serial, &sendData[0], 1 );
	if( !ok )
		return False;
	// Send command and data with converting with special character
	for( i = 1; i < dataSize + 5; i++ )
	{
		if( ( sendData[i] == STX ) || ( sendData[i] == DLE ) )
		{
			ch = DLE;
			ok = SerialOp.write( o->serial, &ch, 1 );
			if( !ok )
				return False;
		}
		ok = SerialOp.write( o->serial, &sendData[i], 1 );
		if( !ok )
			return False;
	}
	// }

	return True;
}


static          Boolean
__receiveData(
	iOBarjutData o,
	unsigned char *command,
	unsigned char *address,
	unsigned char *data,
	int maxDataSize )
{

	int             avail = 0;
	Boolean         ok = False;
	unsigned char   dle = False;
	int             index = 0;
	unsigned char   checksum = 0;
	int             length = 0;
	int             count = 0;
	unsigned char   c = 0;
	int             i;


	if( SerialOp.available( o->serial ) )
	{
		//TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Available: %i", SerialOp.available( o->serial ));
		i = 260;
		while( i )
		{
			i--;

			if( !SerialOp.available( o->serial ) )
			{
				ThreadOp.sleep( 5 );
				if( !SerialOp.available( o->serial ) )
					break;
			}
			if( !SerialOp.read( o->serial, &c, 1 ) )
				break;

			//TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%x", c);

			if( c == STX )
			{
				if( dle == False )  // Synchronisation
				{
					index = 1;
				}
				else
				{
					dle = False;	  // dies war ein wirkliches STX-Zeichen
				}
			}
			else if( c == DLE )
			{
				if( dle == False )
				{
					dle = True;		  // Flag, um auf n�chstes Sonderzeichen (STX, DLE) zu warten
				}
				else
				{
					dle = False;	  // dies war ein wirkliches DLE-Zeichen
				}
			}

			if( dle == False )
			{
				switch ( index )
				{
					case 1:			  // STX
						//                 TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "STX");
						index++;
						checksum = 0;
						*command = 0;
						length = 0;
						break;

					case 2:			  // Kommando
						//                 TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Command");
						index++;
						if( c & 0x20 )	// war es eine Antwort?
						{
							*command = c;
						}
						else
						{
							*command = 0;
							index = 0;
						}
						checksum += c;
						break;

					case 3:			  // Adresse
						//                 TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Address");
						index++;
						*address = c;
						checksum += c;
						break;

					case 4:			  // L�nge
						//                 TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Length");
						index++;
						length = c;
						count = 0;
						checksum += c;
						break;

					case 5:			  // Daten
						if( count < length )
						{
							//                   TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Data[%i]",count);
							count++;
							if( count < maxDataSize )
							{
								*data++ = c;
							}
							else
							{
								TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__,
												 9999,
												 "Datasize is larger than expected." );
							}
							checksum += c;
						}
						else			  // Checksumme
						{
							//                   TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Checksum");
							if( c == checksum )
							{
								index = 0;
								return True;	// Receive is OK
							}
							else
							{
								index = 0;
								TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__,
												 9999,
												 "Checksum error (a=%i, c=%i, l=%i)",
												 *address, *command, length );
							}
						}
						break;
				}
			}
		}
		//TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Receiving failed" );
	}

	return False;
}


static void
__getVersion(
	iOBarjutData o )
{
	Boolean         ok = False;
	unsigned char   command;
	unsigned char   address;
	unsigned char   data[260];
	int             i;
	char           *version = NULL;
	char           *info = NULL;
	int             waitcounter = 10;


	/*
	   BarJuT-Info
	 */
	if( __sendCommand( o, 'I', 0, 0, 0 ) )
	{
		TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
						 "Version info requested..." );
		waitcounter = 10;
		while( waitcounter )
		{
			waitcounter--;
			if( __receiveData( o, &command, &address, data, sizeof ( data ) ) )
			{
				if( command == 'i' )
				{
					info = StrOp.fmt( "%s", data );
				}
			}
			ThreadOp.sleep( 10 );
		}
	}
	else
	{
		TraceOp.trc( name, TRCLEVEL_ERROR, __LINE__, 9999,
						 "Could not send info-sequence to BarJuT Device. retrying..." );
	}

	/*
	   BarJuT-Version
	 */
	if( __sendCommand( o, 'V', 0, 0, 0 ) )
	{
		waitcounter = 10;
		while( waitcounter )
		{
			waitcounter--;
			if( __receiveData( o, &command, &address, data, sizeof ( data ) ) )
			{
				if( command == 'v' )
				{
					version = StrOp.fmt( "Version: %i.%02i", data[3], data[2] );

					TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "\r\n%s%s",
									 info, version );
				}
			}
			ThreadOp.sleep( 10 );
		}
	}
	else
	{
		TraceOp.trc( name, TRCLEVEL_ERROR, __LINE__, 9999,
						 "Could not send version-sequence to BarJuT Device. retrying..." );
	}

	StrOp.free( version );
	StrOp.free( info );

}

static void
__getBarjutData(
	iOBarjutData o,
	char request
	/*
	   True=send request command, False=only get received data
	 */
	 )
{
	Boolean         ok = False;
	unsigned char   command;
	unsigned char   address;
	unsigned short  barcode;
	unsigned short  speed;
	unsigned short  lastTime;
	unsigned char   data[260];
	int             i;

	/*
	   already sended RIF's
	 */
	iOMap           map = MapOp.inst(  );


	if( request )
	{
		if( !__sendCommand( o, 'B', 255, 0, 0 ) )	// Broadcast-Abfrage
		{
			TraceOp.trc( name, TRCLEVEL_ERROR, __LINE__, 9999,
							 "Could not send data-sequence to BarJuT Device. retrying..." );
		}
	}

	if( __receiveData( o, &command, &address, data, sizeof ( data ) ) )
	{
		if( ( command >= 'a' ) && ( command <= 'z' ) )	// ignore the echo
		{
			switch ( command )
			{
				case 'a':
					{
						char           *str = NULL;
						char           *key = NULL;

						barcode = *( ( unsigned short * ) &data[0] );
						speed = *( ( unsigned short * ) &data[2] );
						lastTime = *( ( unsigned short * ) &data[4] );

						if( address < sizeof ( o->barcodeNumber ) )
						{
							if( o->barcodeNumber[address] != barcode )	// only when changes
							{
								o->barcodeNumber[address] = barcode;	// save changes

								str =
									StrOp.
									fmt
									( "BarJuT: Address: %i, Barcode: %i, Speed: %i, Time: %i, %i",
									  address, barcode, speed, lastTime, o->barcodeNumber[address] );
//								TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, str );
								TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, str );
								StrOp.free( str );


								/*
								   create a key:
								 */
								key = StrOp.fmt( "%d_%d", address, barcode );

								/*
								   check if key is already in map: every read cycle is the occupation send
								 */
								//           if( MapOp.get( map, key ) == NULL )
								{
									iONode          evt =
										NodeOp.inst( wFeedback.name(  ), NULL,
														 ELEMENT_NODE );

									MapOp.put( map, key, ( obj ) "RFI" );

									wFeedback.setstate( evt, True );
									wFeedback.setaddr( evt, address );
									wFeedback.setidentifier( evt, barcode );
									if( o->iid != NULL )
										wFeedback.setiid( evt, o->iid );

//                   iONode          evt =
//                      NodeOp.inst( wBlock.name(  ), NULL, ELEMENT_NODE );
//                   MapOp.put( map, key, ( obj ) "RFI" );
//
//                   wBlock.setaddr( evt, address );
//                   wBlock.setport( evt, address );
//                   wBlock.setid( evt, "Test" );
//                   wBlock.setlocid( evt, "Mein Test" );
//                   wBlock.setfba( evt, "-" );
//                   wBlock.setfbb( evt, "-" );
//                   if( o->iid != NULL )
//                      wBlock.setiid( evt, o->iid );

									o->listenerFun( o->listenerObj, evt,
														 TRCLEVEL_INFO );

								}
								//           else {
								//             /* ignore event; already sended */
								//           }
								StrOp.free( key );
							}

						}
					}
					break;
			}
		}
	}
}


static Boolean
__initBarjut(
	iOBarjutData o )
{
	return True;
};


static void
__BarjutReader(
	void *threadinst )
{
	iOThread        th = ( iOThread ) threadinst;
	iOBarjut        pBarjut = ( iOBarjut ) ThreadOp.getParm( th );
	iOBarjutData    data = Data( pBarjut );
	int             time = 0;

	while( data->run )
	{
		if( !data->initOK )
		{
			__getVersion( data );
			data->initOK = __initBarjut( data );
			if( !data->initOK )
			{
				ThreadOp.sleep( 1000 );
				continue;
			}
		}

		if( time >= ( data->timeout * 1000 ) )
		{
			__getBarjutData( data, True );
			time = 0;
		}
		else
		{
			time += 20;
			ThreadOp.sleep( 20 );
			__getBarjutData( data, False );
		}
	};
	data->runFinished = True;

	TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
					 "BarJuT ended. <%s>", data->iid );
}

/*********************************/
/****** End of own routines ******/
/*********************************/


/*
   ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----
 */
#include "rocdigs/impl/barjut.fm"
/*
   ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----
 */
