//
// testserial.c -- test/sample code for MacOS X serial ports
//     this version is keyed to the Keyspan USA-28X
//
// Copyright (c) 2000-2006 Keyspan, a division of InnoSys Inc.
//
// In the public beta, MacOS X serial ports are provided by device 
// specific device drivers publishing the ports through a kernel extension known // as Port Server.  In future versions this mechanism will be
// replaced by IOSerialFamily.
//
// build with:
// cc -o testserial -framework CoreFoundation -framework IOKit testserial.c
//


#include	<stdio.h>
#include	<fcntl.h>
#include	<sys/errno.h>
#include	<sys/termios.h>
#include	<sys/types.h>
#include	<sys/time.h>
#include	<unistd.h>

#include	<sys/filio.h>
#include	<sys/ioctl.h>
#include	<CoreFoundation/CoreFoundation.h>

#include	<IOKit/IOKitLib.h>
#include	<IOKit/serial/IOSerialKeys.h>
#include	<IOKit/IOBSD.h>

#include	<pthread.h>

typedef void *(*__pthreadStart_t)(void *);

pthread_t		theReadThread;
int				readArmed = 0,
                fd = 0;

void
    readThreadCleanup(void *thing),
    startRead(),
    stopRead(),
    readThread(void *thing);


char		line[100];

#define BUFSIZE		4096
u_char		theBuff[BUFSIZE],
            theBuff2[BUFSIZE];
            
int countPorts = 0;
#define	kMaxPorts	10
#define kMaxPortLen	40
char	portList[kMaxPorts][kMaxPortLen];

fd_set		theRd, theWr, theEx;
struct timeval	theTime;

struct termios	theTermios;

//
// several names used for IOSerialFamily ports
//
#define DEVICENAME		"/dev/tty.KeyUSA28Xport1"
#define DEVICENAME2		"/dev/tty.KeyUSA28Xport2"

//
// name of Port #1 on the USA-28X with the PortServer driver in the public beta
//
#define DEVICENAME3		"/dev/ttyd.KeyUSA28Xport"

//
// non-blocked "call out" port for IOSerialFamily
//
#define DEVICENAME4		"/dev/cu.KeyUSA28Xport1"

main(int argn, char **args)
{
    int			i,
                k,
                n,
                done = 0,
                baud,
                ret,
                databits,
                stopbits,
                modem;
    char        parity,
                flowcontrol;
    u_char		*cp;

    listSerialPorts();
    //
    // this command line driven test program allows exercise of various serial port functions by hand
    //
    while (!done)
    {
        printf("-> ");
        gets(line);
        switch(line[0])
        {
            case 'q':					// quit test program
                done = 1;
                break;

            case 'h':					// 'help' -- display a list of commands
                printf("q       quit\n");
                printf("o<n>     open port #n\n");
                printf("c       close\n");
                printf("s       select\n");
                printf("r <n>   read\n");
                printf("w<data> write\n");
                printf("b baud/p/d/s/f  set baud, parity, databits, stopbits, flowcontrol\n");
                printf("g       get all modem bits\n");
                printf("gs x    set modem bits\n");
                printf("l       list serial ports\n");
                printf("m       poll port\n");
                printf("t       toggle read thread\n");
                break;

            case 't':
                if (readArmed)
                {
                    stopRead();
                }
                else
                {
                    startRead();
                }
                break;
                
            case 'm':
                if (fd)
                {
                    int			count;
                    ret = ioctl(fd, FIONREAD, &count);
                    printf("ioctl - FIONREAD returned count %d ret %d errno %d\n", count, ret, errno);
               }
                break;
                
            case 'l':					// list IOSerialFamily (only) serial ports
                listSerialPorts();
                break;

            case 'g':					// get/set modem control bits
                if (line[1] == 's')		// these bits are defined in /sys/ttycom.h
                {
                    sscanf(line+2, "%x", &modem);
                    // set modem bits
                    // the only one defined here is TIOCM_DTR (0x02) for DTR on/off
                    ret = ioctl(fd, TIOCMSET, &modem);
                    if (ret)
                    {
                        printf("ioctl - TIOCMSET returned error %d\n", errno);
                    }
                }
                else
                {
                    // get modem bits
                    // CTS (GPI) is TIOCM_CTS (0x20) and DCD is TIOCM_CD (0x40)
                    ret = ioctl(fd, TIOCMGET, &modem);
                    if (ret)
                    {
                        printf("ioctl - TIOCMGET returned error %d\n", errno);
                    }
                    else
                    {
                        printf("TIOCMGET modem bits %x\n", modem);
                    }
                }
                break;
                
            case 'b':				// configure port
                //
                // get current termios structure (sys/termios.h)
                //
                ret = tcgetattr(fd, &theTermios);
                if (ret)
                {
                    break;
                }
                printf("GET termios: iFlag %x  oFlag %x  cFlag %x  lFlag %x  speed %d\n",
                       theTermios.c_iflag,
                       theTermios.c_oflag,
                       theTermios.c_cflag,
                       theTermios.c_lflag,
                       theTermios.c_ispeed);
                baud = -1;
                parity = 0x00;
                databits = 8;
                stopbits = 1;
                sscanf(line+1, "%d/%c/%d/%d/%c", &baud, &parity, &databits, &stopbits, &flowcontrol);
                if (baud == -1)
                    break;
                    printf("baud %d  parity %c  databits %d  stopbits %d  flow %c\n",
                           baud, parity, databits, stopbits, flowcontrol);
                memset(&theTermios, 0, sizeof(struct termios));
                cfmakeraw(&theTermios);
                cfsetspeed(&theTermios, baud);
                //theTermios.c_cflag = CIGNORE;
                theTermios.c_cflag = CREAD | CLOCAL;		// turn on READ and ignore modem control lines
                switch (databits)
                {
                    case 5:		theTermios.c_cflag |= CS5;		break;
                    case 6:		theTermios.c_cflag |= CS6;		break;
                    case 7:		theTermios.c_cflag |= CS7;		break;
                    case 8:		theTermios.c_cflag |= CS8;		break;
                }
                if (stopbits == 2)
                    theTermios.c_cflag |= CSTOPB;
                switch(parity)
                {
                    case 'e': case 'E':
                        theTermios.c_cflag |= PARENB;
                        break;

                    case 'o': case 'O':
                        theTermios.c_cflag |= PARENB | PARODD;
                        break;
                }
                switch(flowcontrol)
                {
                    case 'c':		theTermios.c_cflag |= CCTS_OFLOW;	break;		// outbound CTS
                    case 'd':		theTermios.c_cflag |= CDTR_IFLOW;	break;		// inbound DTR
                    case 'x':
                        theTermios.c_iflag |= IXON | IXOFF;
                        theTermios.c_cc[VSTART] = 0x11;
                        theTermios.c_cc[VSTOP] = 0x13;
                        break;
                }
                //ret = ioctl(fd, TIOCSETA, &theTermios);
                printf("SET termios: iFlag %x  oFlag %x  cFlag %x  lFlag %x  speed %d\n",
                       theTermios.c_iflag,
                       theTermios.c_oflag,
                       theTermios.c_cflag,
                       theTermios.c_lflag,
                       theTermios.c_ispeed);
                ret = tcsetattr(fd, TCSANOW, &theTermios);
                if (ret)
                    printf("ioctl - TIOCSETA returned %d (errno %d)\n", ret, errno);
                ioctl(fd, TIOCMGET, &modem);
                modem |= TIOCM_DTR;
                ioctl(fd, TIOCMSET, &modem);	
                break;

            case 'o':						// open port, note that O_NONBLOCK is required to keep open() from
                if (fd == 0)				// waiting for CD to go high
                {
                    if (line[1] == 'x')
                    {
                        fd = open(DEVICENAME2, O_RDWR | O_NONBLOCK);
                        printf("open (%s) returned %d\n", DEVICENAME2, fd);
                    }
                    else
                    if (line[1] == 'y')
                    {
                        fd = open(DEVICENAME3, O_RDWR | O_NONBLOCK);
                        printf("open (%s) returned %d\n", DEVICENAME3, fd);
                    }
                    else
                    if (line[1] == 'z')
                    {
                        fd = open(DEVICENAME4, O_RDWR);
                        printf("open (%s) BLOCK returned %d\n", DEVICENAME4, fd);
                    }
                    else
                    if (line[1])
                    {
                        int portNo = 0;
                        sscanf(line+1, "%d", &portNo);
                        if (portNo > countPorts)
                        {
                            printf("%d is out of range (%d)\n", portNo, countPorts);
                        }
                        else
                        {
                            fd = open(portList[portNo], O_RDWR | O_NONBLOCK);
                            printf("open (%s) returned %d\n", portList[portNo], fd);
                        }
                    }
                    else
                    {
                        fd = open(DEVICENAME, O_RDWR | O_NONBLOCK);
                        printf("open (%s) returned %d\n", DEVICENAME, fd);
                    }
                }
                else
                {
                    printf("already open\n");
                }
                break;

            case 'c':					// close port
                if (fd)
                {
                    close(fd);
                    fd = 0;
                }
                break;

            case 'r':					// given that open() was issued with O_NOBLOCK
                if (fd)					// read() here will not block
                {
                    n = 1;
                    sscanf(line+1, "%d", &n);
                    k = read(fd, theBuff, n);
                    if (k==-1)
                    {
                        printf("read returned errno %d\n", errno);
                    }
                    else if (k==0)
                    {
                        printf("read returned 0\n");
                    }
                    else
                    {
                        printf("read %d bytes\n", k);
                        for (i=0; i<k; i++)
                        {
                            printf("%c", theBuff[i]);
                        }
                        printf("\n");
                    }
                }
                break;

            case 'w':						// write data
                if (fd)
                {
                    n = strlen(line+1);		// process "w/ n m" write <n> lines of <m> characters each, beginning with 'A'
                    if (line[1] == '/')
                    {
                        int ii, nn;
                        i = 1;
                        n = 1;
                        sscanf(line+2, "%d %d", &i, &n);
                        cp = theBuff;
                        for (ii=0; ii<i; ii++)
                        {
                            for (nn=0; nn<n; nn++)
                            {
                                *cp++ = 'A'+nn;
                            }
                            *cp++ = '\n';
                            *cp++ = '\r';
                        }
                        *cp = 0;
                        k = write(fd, theBuff, strlen(theBuff));
                        FD_ZERO(&theWr);
                        FD_SET(fd, &theWr);
                        theTime.tv_sec = 5;
                        n = select(fd+1, NULL, &theWr, NULL, &theTime);
                        printf("select (w) returned %d  wr %d\n", n,
                            FD_ISSET(fd, &theWr));
                    }
                    else
                    {
                        k = write(fd, line+1, n);
                    }
                    if (k==-1)
                    {
                        printf("write returned errno %d\n", errno);
                    }
                    else
                    {
                        printf("write returned %d\n", k);
                    }
                }
                break;

            case 's':						// test select
                if (fd)
                {
                    FD_ZERO(&theRd);
                    FD_SET(fd, &theRd);
                    FD_ZERO(&theWr);
                    FD_SET(fd, &theWr);
                    FD_ZERO(&theEx);
                    FD_SET(fd, &theEx);
                    memset(&theTime, 0, sizeof(struct timeval));
                    if (line[1] == 'w')
                    {
                        int 	k = 0;
                        sscanf(line+2, "%d", &k);
                        theTime.tv_sec = k;
                        n = select(fd+1, NULL, &theWr, NULL, &theTime);
                        printf("select (w) returned %d  wr %d\n", n,
                            FD_ISSET(fd, &theWr));
                    }
                    else
                    {
                        n = select(fd+1, &theRd, &theWr, &theEx, &theTime);
                        printf("select returned %d  rd %d  wr %d  ex %d\n", n,
                            FD_ISSET(fd, &theRd), FD_ISSET(fd, &theWr), FD_ISSET(fd, &theEx));
                    }
                }
                break;
        }
    }
}

//
// sample to show reading the MacOS X registry to find IOSerialFamily serial ports
//
kern_return_t
createSerialIterator(io_iterator_t *serialIterator)
{
    kern_return_t	kernResult;
    mach_port_t		masterPort;
    CFMutableDictionaryRef	classesToMatch;
    if ((kernResult=IOMasterPort(NULL, &masterPort)) != KERN_SUCCESS)
    {
        printf("IOMasterPort returned %d\n", kernResult);
        return kernResult;
    }
    if ((classesToMatch = IOServiceMatching(kIOSerialBSDServiceValue)) == NULL)
    {
        printf("IOServiceMatching returned NULL\n");
        return kernResult;
    }
    CFDictionarySetValue(classesToMatch, CFSTR(kIOSerialBSDTypeKey),
                         CFSTR(kIOSerialBSDRS232Type));
    kernResult = IOServiceGetMatchingServices(masterPort, classesToMatch, serialIterator);
    if (kernResult != KERN_SUCCESS)
    {
        printf("IOServiceGetMatchingServices returned %d\n", kernResult);
    }
    return kernResult;
}


char *
getRegistryString(io_object_t sObj, char *propName)
{
    static char resultStr[256];
    CFTypeRef	nameCFstring;
    resultStr[0] = 0;
    nameCFstring = IORegistryEntryCreateCFProperty(sObj,
            CFStringCreateWithCString(kCFAllocatorDefault, propName, kCFStringEncodingASCII),
                                                   kCFAllocatorDefault, 0);
    if (nameCFstring)
    {
        CFStringGetCString(nameCFstring, resultStr, sizeof(resultStr),
                                 kCFStringEncodingASCII);
        CFRelease(nameCFstring);
    }
    return resultStr;
}


listSerialPorts()
{
    io_iterator_t	theSerialIterator;
    io_object_t		theObject;
    char			ttyDevice[256],
        dialInDevice[256],
        callOutDevice[256];
    if (createSerialIterator(&theSerialIterator) != KERN_SUCCESS)
    {
        printf("createSerialIterator failed\n");
        return 0;
    }
    countPorts = 0;
    while (theObject = IOIteratorNext(theSerialIterator))
    {
        strcpy(ttyDevice, getRegistryString(theObject, kIOTTYDeviceKey));
        strcpy(dialInDevice, getRegistryString(theObject, kIODialinDeviceKey));
        if (strlen(dialInDevice) > kMaxPortLen)
        {
            printf("dialInDevice [%s] too long\n", dialInDevice);
            return -1;
        }
        if (countPorts < kMaxPorts)
        {
            strcpy(portList[countPorts++], dialInDevice);
        }
        strcpy(callOutDevice, getRegistryString(theObject, kIOCalloutDeviceKey));
        printf("device [%s]  dialin [%s]  callout [%s]\n", ttyDevice, dialInDevice, callOutDevice);
    }
    return 1;
}


void
readThreadCleanup(void *thing)
{
    //
    // 
}

void
readThread(void *thing)
{
    int		k,
            n;
    pthread_cleanup_push(readThreadCleanup, thing);
    while (readArmed)
    {
        n = 1;
        k = read(fd, theBuff2, n);
        printf("read returned %d data %x\n", k, theBuff2[1]);
    }
    pthread_cleanup_pop(thing);
}


void
startRead()
{
    if (fd == 0 || readArmed)
        return;
    readArmed = 1;
    pthread_create(&theReadThread, NULL, (__pthreadStart_t)readThread, NULL);
}


void
stopRead()
{
    readArmed = 0;
    pthread_cancel(theReadThread);
}
