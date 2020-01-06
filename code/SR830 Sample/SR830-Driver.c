#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SR830-Driver.h"


/*****************************************************************************
 *                      Function GPIBERROR
 * This function will notify you that a NI-488 function failed by
 * printing an error message.  The status variable IBSTA will also be
 * printed in hexadecimal along with the mnemonic meaning of the bit
 * position. The status variable IBERR will be printed in decimal
 * along with the mnemonic meaning of the decimal value.  The status
 * variable IBCNT will be printed in decimal.
 *
 * The NI-488 function IBONL is called to disable the hardware and
 * software.
 *
 * The EXIT function will terminate this program.
 *****************************************************************************/
void GpibError(int Device, const char *msg)
{
	printf("%s\n", msg);

	printf("Ibsta() = 0x%x  <", Ibsta());
	if (Ibsta() & ERR)  printf(" ERR");
	if (Ibsta() & TIMO)  printf(" TIMO");
	if (Ibsta() & END)  printf(" END");
	if (Ibsta() & SRQI)  printf(" SRQI");
	if (Ibsta() & RQS)  printf(" RQS");
	if (Ibsta() & CMPL)  printf(" CMPL");
	if (Ibsta() & LOK)  printf(" LOK");
	if (Ibsta() & REM)  printf(" REM");
	if (Ibsta() & CIC)  printf(" CIC");
	if (Ibsta() & ATN)  printf(" ATN");
	if (Ibsta() & TACS)  printf(" TACS");
	if (Ibsta() & LACS)  printf(" LACS");
	if (Ibsta() & DTAS)  printf(" DTAS");
	if (Ibsta() & DCAS)  printf(" DCAS");
	printf(" >\n");

	printf("Iberr() = %d", Iberr());
	if (Iberr() == EDVR) printf(" EDVR <Driver error>\n");
	if (Iberr() == ECIC) printf(" ECIC <Not Controller-In-Charge>\n");
	if (Iberr() == ENOL) printf(" ENOL <No Listener>\n");
	if (Iberr() == EADR) printf(" EADR <Address error>\n");
	if (Iberr() == EARG) printf(" EARG <Invalid argument>\n");
	if (Iberr() == ESAC) printf(" ESAC <Not System Controller>\n");
	if (Iberr() == EABO) printf(" EABO <Operation aborted>\n");
	if (Iberr() == ENEB) printf(" ENEB <No GPIB board>\n");
	if (Iberr() == EOIP) printf(" EOIP <Async I/O in progress>\n");
	if (Iberr() == ECAP) printf(" ECAP <No capability>\n");
	if (Iberr() == EFSO) printf(" EFSO <File system error>\n");
	if (Iberr() == EBUS) printf(" EBUS <Command error>\n");
	if (Iberr() == ESTB) printf(" ESTB <Status byte lost>\n");
	if (Iberr() == ESRQ) printf(" ESRQ <SRQ stuck on>\n");
	if (Iberr() == ETAB) printf(" ETAB <Table Overflow>\n");
	if (Iberr() == ELCK) printf(" ELCK <Lock error>\n");
	if (Iberr() == EARM) printf(" EARM <Ibnotify rearm error>\n");
	if (Iberr() == EHDL) printf(" EHDL <Invalid Handle>\n");
	if (Iberr() == EWIP) printf(" EWIP <Wait already in progress>\n");
	if (Iberr() == ERST) printf(" ERST <Notification cancelled due to reset>\n");
	if (Iberr() == EPWR) printf(" EPWR <Power error>\n");

	printf("Ibcnt() = %u\n", Ibcnt());
	printf("\n");

	/* Call ibonl to take the device and interface offline */
	ibonl(Device, 0);
	exit(1);
}


/*****************************************************************************
 * InitSR830 - Initialization, Done only once at the beginning of your application.
 * int   PrimaryAddress        Primary address of the device
 * int   SecondaryAddress      Secondary address of the device
 * int   BoardIndex            Interface Index (GPIB0=0,GPIB1=1,etc.)
   ***************************************************************************** */
int InitSR830(int PrimaryAddress, int SecondaryAddress, int BoardIndex)
{
	int Device;
	Device = ibdev(              /* Create a unit descriptor handle         */
		BoardIndex,              /* Board Index (GPIB0 = 0, GPIB1 = 1, ...) */
		PrimaryAddress,          /* Device primary address                  */
		SecondaryAddress,        /* Device secondary address                */
		T10s,                    /* Timeout setting (T10s = 10 seconds)     */
		1,                       /* Assert EOI line at end of write         */
		0);                      /* EOS termination mode                    */
	if (Ibsta() & ERR)           /* Check for GPIB Error                    */
		GpibError(Device, "ibdev Error");

	ibclr(Device);                 /* Clear the device                        */
	if (Ibsta() & ERR) 
		GpibError(Device, "ibclr Error");
	return Device;
}


/*****************************************************************************
 * ExitSR830 - Uninitialization, Done only once at the end of your application.
 * int Device - Target device handle
 *****************************************************************************/
void ExitSR830(int Device)
{
	ibonl(Device, 0);              /* Take the device offline                 */
	if (Ibsta() & ERR) 
		GpibError(Device, "ibonl Error");
}


/*****************************************************************************
 * SendMsg - Send the identification query command
 * int Device - Target device handle
 * const char * command - Target query command string
 ******************************************************************************/
void SendMsg(int Device, const char * command)
{
	ibwrt(Device, command, strlen(command));

	if (Ibsta() & ERR) 
		GpibError(Device, "ibwrt Error");
}


/*****************************************************************************
 * ReadMsg - Read the device output
 * int Device - Target device handle
 * char * res - Output result string buffer
 ******************************************************************************/
void ReadMsg(int Device, char * res)
{
	ibrd(Device, res, 100);     /* Read up to 100 bytes from the device    */
	if (Ibsta() & ERR) 
		GpibError(Device, "ibrd Error");
	res[Ibcnt()] = '\0';        /* Null terminate the ASCII string         */
}


/*****************************************************************************
 * GetTimeConstant - Get TIME CONSTANT by querying and reading data
 * int Device - Target device handle
 * float return_value - Output float result of TIME CONSTANT
 ******************************************************************************/
float GetTimeConstant(int Device)
{
	char tstr[101];
	float TIME_CONSTANT = 0.0;
	SendMsg(Device, "OFLT?"); /* Query TIME_CONSTANT */
	ReadMsg(Device, tstr); /* Read TIME_CONSTANT */

	int temp = atoi(tstr);
	float TCmap[20] = { 0.01f, 0.03f, 0.1f, 0.3f, 1.0f, 3.0f, 10.0f, 30.0f, 100.0f, 300.0f,  /* TC number to real TIME_CONSTANT map */
					1000.0f, 3000.0f, 10000.0f, 30000.0f, 100000.0f, 300000.0f, 1000000.0f, 3000000.0f, 10000000.0f, 30000000.0f };
	TIME_CONSTANT = TCmap[temp]; /* Tranfer TC number to real TIME_CONSTANT */
	printf("Time Constant = %f\n", TIME_CONSTANT); // Output TIME_CONSTANT
	return TIME_CONSTANT;
}