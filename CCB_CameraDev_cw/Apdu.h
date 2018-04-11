#if !defined(AFX_H__7AE1B5DC_759D_4CA7_8D27_439532ABFAB3__INCLUDED_)
#define AFX_H__7AE1B5DC_759D_4CA7_8D27_439532ABFAB3__INCLUDED_

#include "string.h"
#include "mytype.h"
#include "MiddleWareDataStruct.h"

#pragma once

typedef void* HDRIVER;

typedef T_U16 APDU_RT;

/*
*  digest����
*/
typedef enum _DIGEST_TYPE{
	DIGEST_SHA1_TYPE = 0, 
	DIGEST_SHA256_TYPE = 1, 
	DIGEST_SM3_TYPE = 2,
	DIGEST_END,
}DIGEST_TYPE;

/*
*  RSA��Կλ��������RSA��Կʱʹ��
*/
typedef enum _RSA_BITS{
	RSA_1024_BIT    =0,	
	RSA_2048_BIT    =1,	
}RSA_BITS;

/*
*  PIN����
*/
typedef enum _PIN_TYPE{
	USER_PIN  = 0,
	SO_PIN    = 1,
}PIN_TYPE;

/*
*  ��������
*/
typedef enum _ENCRYPT_TYPE{
	ET_ENCRYPT      = 0,
	ET_DECRYPT      = 1,
}ENCRYPT_TYPE;


typedef enum _ALGO_TYPE{
	ALGO_TYPE_DES			=0,
	ALGO_TYPE_3DES			=1,
	ALGO_TYPE_SSF33		    =3,
	ALGO_TYPE_SSF44		    =4,
	ALGO_TYPE_SCB2          =5,
	ALGO_TYPE_NULL          =6, //KID������Ч��ʱ��ʹ��
}ALGO_TYPE;


typedef enum _FILE_CLASS{
	FILE_CLASS_MF					= 0,
	FILE_CLASS_DF					= 1,
    FILE_CLASS_EF_COMMON			= 2,	
	FILE_CLASS_EF_PRIVATEKEY		= 3,
	FILE_CLASS_EF_FIXED_LEN_REC		= 4,
	FILE_CLASS_EF_NO_FIXED_LEN_REC	= 5,
	FILE_CLASS_EF_CYC_REC			= 6,
	FILE_CLASS_KEY					= 7,
	FILE_CLASS_EF_PRIVATEKEY_SIGN	= 8,
	FILE_CLASS_EF_PUBLICKEY_SIGN	= 9,
	FILE_CLASS_EF_PRIVATEKEY_EXCH	= 10,
	FILE_CLASS_EF_PUBLICKEY_EXCH	= 11,
}FILE_CLASS;

/*
typedef struct _FILE_ATTR 
{
	FILE_CLASS		enumFileType;			// �ļ�����
	T_U32	ulFileSize;				// �ļ���С
	T_U8	bCreateOrReadRight;		// ����/��Ȩ��
	T_U8	bDeleteOrWriteRight;	// ɾ��/дȨ��
	T_U8	bShortID;				// ���ļ���ʶ
	T_U8	bSecurityAttr;			// ��ȫ����
	T_U8	bInitState;				// �ļ��ĳ�ʼ״̬		//���ֶ�ASP�ĵ���û�ж���
	T_U8	bUsePurview;									//���ֶ�ASP�ĵ���û�ж���
	T_U8	bRFU[32];		//�����֣�ʹ��ʱ��TLV��ʽ��ʾ
}FILE_ATTR,*PFILE_ATTR;
*/


/*
*  �����ļ���Flag�ṹ
*/
typedef struct _CREATE_FILE_FLAGS {
	T_U8      bInitState;       //����EF��ʱ��ʹ�ã���FF�����ļ����ݳ�ʼ��Ϊ��00��; "00"��ʾ������ʼ��
	T_U8      bShortFileID;     //���ļ����ƣ�Ĭ��ȡ��00��
	T_U8      bSafeAttribute;   //��ȫ����
	T_U8      bUsePurwrite;      //����DF��ʱ��ʹ�ã�Ϊ˽Կ���޸�Ȩ��
	T_U8      bUsePurview;      //����DF��ʱ��ʹ�ã�Ϊ˽Կ��ʹ��Ȩ��
	T_U8	  bRecordCount;		//����ѭ����¼��������¼�ļ��ļ�¼����
}CREATE_FILE_FLAGS,*PCREATE_FILE_FLAGS;



#define NULL					0
#define MAX_PIN_LENGTH		128
#define MIN_PIN_LENGTH		0
#define COMMAND_MAX_LEN         1024
#define REPLY_MAX_LEN           1024
#define PARA_INVALID_HTOKEN		((void*)-1)
#define ECC_MAX_DATA_BLOCK		128

#define TRANSFER_BUFFER_LEN 224   
#define RANDOM_BLOCK_LEN 	0x08
#define DIGEST_BLOCK_LEN 	0x40


//RET APDU
//�˸��ֽڣ���Ҫ�޸�
const T_U16 RT_FAILED 					= 0x0000;	//ʧ��			   
const T_U16 RT_OK	        				= 0x9000;   //�ɹ�
const T_U16 RT_NOT_SUPORT	        		= 0x000D;   //���ܲ�֧��
const T_U16 RT_INVALID_PARAMETER   		= 0x000E;   //��Ч����   
const T_U16 RT_UNKNOW_ERROR        		= 0x000F;   //δ֪���� 
const T_U16 RT_BUFFER_SMALL        		= 0x000C;   //�ռ䲻��         
const T_U16 RT_RANGE_OVERFLOW      		= 0x000B;    //ƫ�Ƴ�����Χ    
const T_U16 RT_OFFSET_RANGE_OVERFLOW       = 0x6B00;   //ƫ�Ƴ�����Χ     
const T_U16 RT_PRIVATE_KEY_SIZE_ERR        = 0x0007;    //˽Կ�ļ����ȴ���



T_S32 EnumDevices(DEVICE *pDevice);
T_S32 EnumAllDevices(DEVICE *pDevice);
HDRIVER OpenDevice(T_S8 * pszDeviceStr);
void CloseDevice(HDRIVER hDriver);
T_S32 SendAPDU(IN HDRIVER hDriver,IN T_S8* szCommand, OUT T_S8* szReply, OUT T_U16* pulSW);



//���ú���
//ѡ��Ӧ���ļ�

APDU_RT APDU_ReloadPIN(IN HDRIVER hToken,IN PIN_TYPE ptPINType,IN T_U8 *pbReloadPIN,
					IN T_S32 ulReloadPINLen,IN T_U8 *pbNewPIN,IN T_S8 ulNewPINLen);


APDU_RT APDU_VerifyPIN(IN HDRIVER hToken,IN PIN_TYPE ptPINType,IN T_U8* pbPIN,IN T_U32 ulPinLen);
APDU_RT APDU_VerifyPIN_SM4(IN HDRIVER hToken,IN PIN_TYPE ptPINType,IN T_U8* pbPIN,IN T_U32 ulPinLen);


APDU_RT APDU_ChangePIN(IN HDRIVER hToken,IN PIN_TYPE ptPINType,IN T_U8 *pbOldPIN,
					IN T_U32 ulOldPinlen,IN T_U8 *pbNewPIN,IN T_U32 ulNewPinLen);

APDU_RT APDU_ChangePIN_SM4(IN HDRIVER hToken,IN PIN_TYPE ptPINType,IN T_U8 *pbOldPIN,
					   IN T_U32 ulOldPinlen,IN T_U8 *pbNewPIN,IN T_U32 ulNewPinLen);


APDU_RT APDU_UnblockPIN(IN HDRIVER hToken,IN T_U8 *pbSOPIN,IN T_U32 ulSOPinlen,
					IN T_U8 *pbUserPIN,IN T_U32 ulUserPINLen);


APDU_RT APDU_RemandEncyptPIN(IN HDRIVER hToken,IN OUT T_S8* pPin);
APDU_RT APDU_RemandEncyptPIN_SM4(IN HDRIVER hToken,IN OUT T_S8* pPin);


APDU_RT APDU_CreateFile(IN HDRIVER hToken,IN FILE_CLASS fcFileType,IN T_U32 ulFileID,
					IN T_U32 ulFileSize,IN T_U8 bCreateOrReadOrSignRight,IN T_U8 bDeleteOrWriteOrAddRight,
					IN CREATE_FILE_FLAGS cffFlags, IN T_U8 *pbFileName,IN T_U32 ulFileNamelen);


APDU_RT APDU_DeleteFile(IN HDRIVER hToken,IN T_U32 ulFileID);


APDU_RT APDU_DeleteFileEx(IN HDRIVER hToken,IN FILE_CLASS fcFileClass,IN T_U32 ulFileID);


APDU_RT APDU_EraseDF(IN HDRIVER hToken);


APDU_RT APDU_ReadBinary(IN HDRIVER hToken,IN T_U32 ulOffset,IN T_U32 ulSize,
						   OUT T_U8 *pbOutData,IN OUT T_U32* pulOutDataLen);


APDU_RT APDU_ReadBinaryEx(IN HDRIVER hToken,IN T_U32 ulFileID,IN T_U32 ulOffset,IN T_U32 ulSize,
						   OUT T_U8 *pbOutData,IN OUT T_U32* pulOutDataLen);



APDU_RT APDU_WriteBinary(IN HDRIVER hToken,IN T_U32 ulOffset,IN T_U32 ulSize,
						IN T_U8 *pbData,IN OUT T_U32 *pulLen);



APDU_RT APDU_WriteBinaryEx(IN HDRIVER hToken,IN T_U32 ulFileID,IN T_U32 ulOffset,
						IN T_U32 ulSize,IN T_U8 *pbData,IN OUT T_U32 *pulLen);



APDU_RT APDU_ReadRecord(IN HDRIVER hToken,IN T_U8 bRecordID,IN T_U8 bCtrlFlags,
					IN T_U32 ulSize,OUT T_U8 *pbOutData,IN OUT T_U32* pulOutDataLen);


APDU_RT APDU_UpdateRecord(IN HDRIVER hToken,IN T_U8 bRecordID,IN T_U8 bCtrlFlags,
						IN T_U32 ulSize,OUT T_U8 *pbData,IN T_U32* pulDataLen);


APDU_RT APDU_AppendRecord(IN HDRIVER hToken,IN T_U8 bCtrlFlags,IN T_U32 ulSize,
						OUT T_U8 *pbData,IN OUT T_U32* pulDataLen);


APDU_RT APDU_SelectFile(IN HDRIVER hToken,IN T_U32 ulFileID);


APDU_RT APDU_SelectApp(IN HDRIVER hToken,IN T_U32 ulFileID,OUT T_U8 *pbReply,IN OUT T_U32 *pulReplyLen);


APDU_RT APDU_GetFileInfo(IN HDRIVER hToken,IN FILE_CLASS fcFileClass,OUT PFILE_ATTR fiFileInfo);



APDU_RT APDU_SetDeviceInfo(IN HDRIVER hToken,IN DEVICE_INFO diDevInfo);


APDU_RT APDU_GetDeviceInfo(IN HDRIVER hToken,OUT DEVICE_INFO *pdiDevInfo);


APDU_RT APDU_SM2Gensm2KeyPair(IN HDRIVER hToken,
							  IN T_U32 ulPrvKeyID, IN T_U32 ulPubKeyID);

APDU_RT APDU_SM2PriKeyOperate(IN HDRIVER hToken,IN T_U32 ulFileID,IN T_U8 *pbInData,
							  IN T_U32 ulInDataLen,OUT T_U8 *pbOutData,IN OUT T_U32 *pulOutDataLen);

APDU_RT APDU_SM2PubKeyEncrypt(IN HDRIVER hToken,IN T_U32 ulFileID,IN T_U8 *pbInData,
							  IN T_U32 ulInDataLen,OUT T_U8 *pbOutData,IN OUT T_U32 *pulOutDataLen)	;

APDU_RT APDU_SM2PubKeyVerify(IN HDRIVER hToken,IN T_U32 ulFileID,IN T_U8 *pbInSignData,IN T_U8 *pbInSM3HashData,
							  IN T_U32 ulInDataLen);

APDU_RT APDU_RSAGenRsaKeyPair(IN HDRIVER hToken,IN RSA_BITS rbBits,
						IN T_U32 ulPrvKeyID, IN T_U32 ulPubKeyID);

APDU_RT APDU_RSAPriKeyOperate(IN HDRIVER hToken,IN T_U32 ulFileID,IN T_U8 *pbInData,
						IN T_U32 ulInDataLen,OUT T_U8 *pbOutData,IN OUT T_U32 *pulOutDataLen);


APDU_RT APDU_RSAPubKeyOperate(IN HDRIVER hToken,IN T_U32 ulFileID,IN T_U8 *pbInData,
						IN T_U32 ulInDataLen,OUT T_U8 *pbOutData,IN OUT T_U32 *pulOutDataLen);


APDU_RT APDU_GetResponse(IN HDRIVER hToken,IN T_U32 ulSize,OUT T_U8 *pbOutData,
						IN OUT T_U32 *pulOutDataLen);


APDU_RT APDU_SymmetricKeyIDOperate(IN HDRIVER hToken,IN ENCRYPT_TYPE etType,IN T_U32 ulKeyID,
								IN T_U8 *pbInData,IN T_U32 ulInDataLen,OUT T_U8 *pbOutData,
								IN OUT T_U32 *pulOutDataLen);


APDU_RT APDU_WriteKey(IN HDRIVER hToken, IN T_U32 ulKeyFlags,T_BOOL endFlag, 
								IN T_U32 ulKeyID, IN T_U8* pbKey, IN T_U32 ulKeyLen);


APDU_RT APDU_GenRandomNumber(IN HDRIVER hToken,IN T_U32 ulSize,OUT T_U8 *pbOutData,IN OUT T_U32 *pulOutDataLen);


APDU_RT APDU_ExternalAuthentication(IN HDRIVER hToken,IN T_U32 ulKeyID,IN T_U8* pbEAKey,IN T_U32 ulEAKeyLen);


APDU_RT APDU_InternalAuthentication(IN HDRIVER hToken,IN T_U32 ulKeyID,IN T_U8* pbIAData,
								IN T_U32 ulIADataLen,OUT T_U8 *pbOutData,IN OUT T_U32 *pulOutDataLen);


APDU_RT APDU_VerifyTransportKey(IN HDRIVER hToken,IN T_U8* pbKey,IN T_U32 ulKeyLen);

//���ô�����Կ2
APDU_RT APDU_SetupTransportKey(IN HDRIVER hToken,IN T_U8* pbKey,IN T_U32 ulKeyLen);

APDU_RT APDU_GenDigest(IN HDRIVER hToken,IN DIGEST_TYPE eDigType,
							IN T_U8 *pbInData, IN T_U32 ulInDataLen);

//������ʼ��
APDU_RT APDU_FinishInit(IN HDRIVER hToken);
//��ȡ�û�����ʣ�ೢ�Դ���
APDU_RT APDU_GetLeftUserPinTryCount(IN HDRIVER hToken);
//reset smartcard
APDU_RT APDU_ResetSmartCard(IN HDRIVER hToken, OUT T_U8 *pbOutData, IN OUT T_U32 *pulOutDataLen);

//�޸�����ǩ��˽Կ��������
APDU_RT APDU_ChangePrivateKeyState(IN HDRIVER hToken,IN T_U32 ulFileID);

#if 1//def PRODUCTION_TEST
//ɾ��MF
APDU_RT APDU_DeleteMF(IN HDRIVER hToken);

//��ȡ����Ȩ��
APDU_RT APDU_GetSuperRight(IN HDRIVER hToken, IN T_U8* pbSuperPIN, IN T_U32 ulSuperPINLen);

//�ָ����׿�
APDU_RT APDU_RestoreWhiteCard(IN HDRIVER hToken);
#endif

HANDLE APDU_BeginTransaction(IN HDRIVER hToken,IN ULONG ulWaitTime = 40000);
APDU_RT EndTransaction(IN HANDLE hMutex);

#endif

//#endif // !defined(AFX_H__7AE1B5DC_759D_4CA7_8D27_439532ABFAB3__INCLUDED_)
