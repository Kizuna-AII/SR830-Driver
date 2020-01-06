#pragma once
#include "ni4882.h"

/*****************************************************************************
 *                       SR830-Driver Declaration
 * This Header contains initial function, exit function, simple I/O function
 * as well as error processing function. You should read the comment first and 
 * be very careful to use these functions or design your own function because 
 * SR830 is very precise and delicate.
 *****************************************************************************/

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

void GpibError(int Device, const char *msg);

/*****************************************************************************
 * InitSR830 - Initialization, Done only once at the beginning of your application.
 * int   PrimaryAddress        Primary address of the device
 * int   SecondaryAddress      Secondary address of the device
 * int   BoardIndex            Interface Index (GPIB0=0,GPIB1=1,etc.)
   ***************************************************************************** */
int InitSR830(int PrimaryAddress, int SecondaryAddress, int BoardIndex);

/*****************************************************************************
 * ExitSR830 - Uninitialization, Done only once at the end of your application.
 * int Device - Target device handle
 *****************************************************************************/
void ExitSR830(int Device);

/*****************************************************************************
 * SendMsg - Send the identification query command
 * int Device - Target device handle
 * const char * command - Target query command string
 ******************************************************************************/
void SendMsg(int Device, const char * command);

/*****************************************************************************
 * ReadMsg - Read the device output
 * int Device - Target device handle
 * char * res - Output result string buffer
 ******************************************************************************/
void ReadMsg(int Device, char * res);

/*****************************************************************************
 * GetTimeConstant - Get TIME CONSTANT by querying and reading data
 * int Device - Target device handle
 * float return_value - Output float result of TIME CONSTANT
 ******************************************************************************/
float GetTimeConstant(int Device);

