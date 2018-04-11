#ifndef DSPPROVIDER_H
#define DSPPROVIDER_H

#include "MiddleWareDataStruct.h"
#include "WinSCard.h"
typedef void* DTOKEN; 
void SetDeviceType();
int DSP_EnumDevices(DEVICE *pDevice);
LONG GetStatusChange(LPSCARD_READERSTATE rgReaderStates,int Count);
DTOKEN DSP_OpenDevice(char * pszDeviceStr);
void DSP_CloseDevice(DTOKEN hToken);
int DSP_XSendAPDU(DTOKEN hToken,char *szCommand, char *szReply,  int *pulSW);

#endif