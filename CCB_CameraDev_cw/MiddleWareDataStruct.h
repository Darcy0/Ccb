#if !defined(MIDDLEWAREDATASTRUCT_H)
#define MIDDLEWAREDATASTRUCT_H

#include "mytype.h"

#pragma once

/*
*  DEVICE INFO
*/
#define APDU_MAX_PATH 256

#if 0
typedef struct _DEVICE_INFO {
	T_U16               ulVenderID;					//Tag:0x01
	T_U16               ulProductID;				//Tag:0x02
	T_S8				sbSerialNumber[APDU_MAX_PATH];	//Tag:0x03  LV 结构
	T_U32				ulProduceDate;				//Tag:0x04
	T_S8				sbReserved[APDU_MAX_PATH];		//Tag:0x05  LV 结构
	T_U8				bAlg;				        //算法类型
}DEVICE_INFO,*PDEVICE_INFO;
#else

#define TRANSACTION_SIGN			0x00000001
#define EXTERNAL_HASH_SIGN			0x00000002
#define FORCED_CIPHER_PIN			0x00000004
#define XML_TEMPLATE				0x00000008
#define RSA2048_SIGN				0x00010000
#define SM2_SIGN					0x00020000
#define BLUETOOTH_CHANNEL			0x01000000

typedef struct _DEVICE_INFO {
	T_U16               ulVenderID;					//Tag:0x01
	T_U16               ulProductID;				//Tag:0x01
	T_U32				cfgWord;					//Tag:0x02
	T_U32				ulProduceDate;				//Tag:0x03
	T_U16				timeOut;					//Tag:0x04
	T_U8				sn[APDU_MAX_PATH];			//Tag:0x05  LV 结构	序列号
	T_U8				preFix[APDU_MAX_PATH];		//Tag:0x06  LV 结构	蓝牙名称前缀
	T_U8				displayContext[APDU_MAX_PATH];		//Tag:0x07  LV 结构	显示文字
}DEVICE_INFO,*PDEVICE_INFO;
#endif

/*
*  文件结构
*/
typedef struct _FILE_ATTR 
{
	T_S32      wTotalSpareSize;	     //Key总的空间
	T_S32      wFileSpareSize;	    //当前文件的总空间
	T_S32      wFileRemainSpareSize; //文件可用空间
	T_U8      bFileType;            //文件类型
	T_U8      bCreateOrReadRight;   //创建或者读权限
	T_U8      bDeleteOrWriteRight;  //删除或者写权限
	T_U8      bAppOrSafe;           //应用和安全
}FILE_ATTR,*PFILE_ATTR;

/*
*  设备类型定义
*/
#define DEVICE_MAX_PATH 256

//卡状态
typedef enum _DEVICE_CARD_TYPE{
	DEVICE_UNCONNECTED_TYPE = 1, 
	DEVICE_CONNECTED_TYPE = 2, 
	DEVICE_END,
}DEVICE_CARD_TYPE;

typedef struct _DEVICE {
	T_S8				szDeviceName[DEVICE_MAX_PATH];
	T_S8				szDeviceAddress[DEVICE_MAX_PATH];	//使用此设备地址可以打开设备
	DEVICE_CARD_TYPE				bDeviceType;
	T_U32      ulRevered1;
	T_U32      ulRevered2;
}DEVICE;

typedef		struct Bot_Reader
{
	char	szReaderName[260];
	char	szDevpath[260];
	HANDLE  hToken;
}BotReader, *PBotReader;

#endif
