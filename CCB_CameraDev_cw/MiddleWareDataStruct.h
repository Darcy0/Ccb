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
	T_S8				sbSerialNumber[APDU_MAX_PATH];	//Tag:0x03  LV �ṹ
	T_U32				ulProduceDate;				//Tag:0x04
	T_S8				sbReserved[APDU_MAX_PATH];		//Tag:0x05  LV �ṹ
	T_U8				bAlg;				        //�㷨����
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
	T_U8				sn[APDU_MAX_PATH];			//Tag:0x05  LV �ṹ	���к�
	T_U8				preFix[APDU_MAX_PATH];		//Tag:0x06  LV �ṹ	��������ǰ׺
	T_U8				displayContext[APDU_MAX_PATH];		//Tag:0x07  LV �ṹ	��ʾ����
}DEVICE_INFO,*PDEVICE_INFO;
#endif

/*
*  �ļ��ṹ
*/
typedef struct _FILE_ATTR 
{
	T_S32      wTotalSpareSize;	     //Key�ܵĿռ�
	T_S32      wFileSpareSize;	    //��ǰ�ļ����ܿռ�
	T_S32      wFileRemainSpareSize; //�ļ����ÿռ�
	T_U8      bFileType;            //�ļ�����
	T_U8      bCreateOrReadRight;   //�������߶�Ȩ��
	T_U8      bDeleteOrWriteRight;  //ɾ������дȨ��
	T_U8      bAppOrSafe;           //Ӧ�úͰ�ȫ
}FILE_ATTR,*PFILE_ATTR;

/*
*  �豸���Ͷ���
*/
#define DEVICE_MAX_PATH 256

//��״̬
typedef enum _DEVICE_CARD_TYPE{
	DEVICE_UNCONNECTED_TYPE = 1, 
	DEVICE_CONNECTED_TYPE = 2, 
	DEVICE_END,
}DEVICE_CARD_TYPE;

typedef struct _DEVICE {
	T_S8				szDeviceName[DEVICE_MAX_PATH];
	T_S8				szDeviceAddress[DEVICE_MAX_PATH];	//ʹ�ô��豸��ַ���Դ��豸
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
