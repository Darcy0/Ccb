/*
 * Copyright (C) 2014 eloam technology
 * Auther: luoliangyi
 * CreateTime: 2014.8.5
 * Email: luoliangyi@eloam.com
 */

#ifndef __EL_H__
#define __EL_H__

#include <stddef.h>
#ifdef _WIN32
#include <windows.h>
#endif /* _WIN32 */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* export */
#ifdef _WIN32
#define EL_CDECL __cdecl
#define EL_STDCALL __stdcall
#define EL_EXPORT __declspec(dllexport)
#else
#define EL_CDECL
#define EL_STDCALL
#define EL_EXPORT
#endif

#ifndef ELAPI
#define ELAPI(rettype) EL_EXPORT rettype EL_CDECL
#endif

#ifdef __cplusplus
#define EL_DEFAULT(val) = val
#else
#define EL_DEFAULT(val)
#endif


/* custom types */

/* represent image */
typedef void *ELimage;
/* represent device,video or audio */
typedef void *ELdevice;
/* record */
typedef void *ELvideoWriter;
/* represent ftp */
typedef void *ELftp;
/* represent http */
typedef void *ELhttp;
/* image file */
typedef void *ELfile;
/* video cap */
typedef void *ELVideoCap;

typedef void *ELRtempl;

#pragma pack(push, 1)

typedef struct ELsize
{
	int width;
	int height;
}ELsize;

typedef struct ELpoint
{
	int x;
	int y;
}ELpoint;

typedef struct ELrect
{
	int x;
	int y;
	int width;
	int height;
}ELrect;

typedef struct ELregion
{
	ELpoint pt[4];
}ELregion;

#pragma pack(push, 1)

typedef struct ELOcrImage
{
	ELimage img;
	const ELrect *rc;
}ELOcrImage;

#pragma pack(pop)

/* quick ocr */

#define EL_QUICK_OCR_CHS					0
#define EL_QUICK_OCR_CHT					1
#define EL_QUICK_OCR_EN					2

/* ocr */
#define EL_OCR_LANG_ENGLISH				0x00000001
#define EL_OCR_LANG_CHINESE_PRC			0x00000002
#define EL_OCR_LANG_CHINESE_TAIWAN		0x00000004
#define EL_OCR_LANG_THAI				0x00000008
#define EL_OCR_LANG_GERMAN				0x00000010
#define EL_OCR_LANG_RUSSIAN				0x00000020
#define EL_OCR_LANG_GREEK				0x00000040
#define EL_OCR_LANG_KOREAN				0x00000080
#define EL_OCR_LANG_SPANISH				0x00000100
#define EL_OCR_LANG_PORTUGUESESTANDARD	0x00000200
#define EL_OCR_LANG_ITALIAN				0x00000400
#define EL_OCR_LANG_TURKISH				0x00000800
#define EL_OCR_LANG_SWEDISH				0x00001000
#define EL_OCR_LANG_UKRAINIAN			0x00002000   

#define EL_OCR_FLAG_TEXT				1
#define EL_OCR_FLAG_BARCODE				2
#define EL_OCR_FLAG_IMAGE				3
#define EL_OCR_RESULT					1
#define EL_OCR_PROGRESS					2

#define EL_OCR_BARCODE_TYPE_UNKOWN			0x00000000
#define EL_OCR_BARCODE_TYPE_CODE39			0x00000001
#define EL_OCR_BARCODE_TYPE_INTERLEAVED25	0x00000002
#define EL_OCR_BARCODE_TYPE_EAN13			0x00000004
#define EL_OCR_BARCODE_TYPE_CODE128			0x00000008
#define EL_OCR_BARCODE_TYPE_EAN8			0x00000010
#define EL_OCR_BARCODE_TYPE_PDF417			0x00000020
#define EL_OCR_BARCODE_TYPE_CODABAR			0x00000040
#define EL_OCR_BARCODE_TYPE_UPCE			0x00000080
#define EL_OCR_BARCODE_TYPE_INDUSTRIAL125	0x00000100
#define EL_OCR_BARCODE_TYPE_IATA25			0x00000200
#define EL_OCR_BARCODE_TYPE_MATRIX25		0x00000400
#define EL_OCR_BARCODE_TYPE_CODE93			0x00000800
#define EL_OCR_BARCODE_TYPE_POSTNET			0x00001000
#define EL_OCR_BARCODE_TYPE_UCC128			0x00002000
#define EL_OCR_BARCODE_TYPE_PATCH			0x00004000
#define EL_OCR_BARCODE_TYPE_AZTEC			0x00008000
#define EL_OCR_BARCODE_TYPE_DATAMATRIX		0x00010000
#define EL_OCR_BARCODE_TYPE_QRCODE			0x00020000
#define EL_OCR_BARCODE_TYPE_UPCA			0x00040000

//ѹ����ѡ��
enum elCompressMode{
	EL_VIDEO_CAP_COMPRESS_HIGH=0,
	EL_VIDEO_CAP_COMPRESS_HIGHER,
	EL_VIDEO_CAP_COMPRESS_MEDIUM,
	EL_VIDEO_CAP_COMPRESS_LOWER,
	EL_VIDEO_CAP_COMPRESS_LOW
};

//λ��ѡ��
enum elWatermarkPosMode{
	EL_VIDEO_CAP_TOPLEFT = 0,
	EL_VIDEO_CAP_TOPRIGHT,
	EL_VIDEO_CAP_BOTTOMLEFT,
	EL_VIDEO_CAP_BOTTOMRIGHT,
	EL_VIDEO_CAP_CENTER
};

//״̬��
enum elVideoCapState
{
	EL_VIDEO_CAP_STATE_STOP = 0,
	EL_VIDEO_CAP_STATE_READY,
	EL_VIDEO_CAP_STATE_RUN,
	EL_VIDEO_CAP_STATE_PAUSE,
};

//¼���������
typedef struct ELvideoCapConf 
{
	char outputFileName[MAX_PATH];		//¼���ļ�������·���������ļ����ͺ�׺������"D:\weike\videoCap\test.mp4"
	int micIndex;			//��˷��豸����

	int frameRate;						//¼����Ƶ֡��
	elCompressMode compressMode;	//��Ƶѹ����, ѹ����Խ��, �ļ�ԽС����������Խ�0:��; 1:�ϸ�; 2:��; 3:�ϵ�; 4:��
	int videoWidth, videoHeight;		//�����Ƶ��С

	BOOL bCapVideo;
	HWND hWnd;				//��Ϣ���մ��ھ��, ¼�������������ִ��󣬿�����˴��ڷ�����Ϣ, ֪ͨ�ϲ����

	ELvideoCapConf()
	{
		ZeroMemory(outputFileName, sizeof(outputFileName));
		
		micIndex = -1;
		frameRate = 10;
		compressMode = EL_VIDEO_CAP_COMPRESS_MEDIUM;
		videoWidth = 800;
		videoHeight = 600;

		bCapVideo = TRUE;
		hWnd = NULL;
	}
}ELvideoCapConf;

//ˮӡ����
typedef struct ELvideoCapWatermark
{
	BOOL bWatermark;		//�Ƿ����ˮӡ
	BOOL bAddTime;			//�Ƿ�����ʱ��ˮӡ, ����: 2016.05.07 12.35.5.237
	int mode;				//0:����ˮӡ, 1:ͼƬˮӡ
	elWatermarkPosMode pos;	//0:���Ͻ�; 1:���Ͻ�; 2:���½�; 3:���½�; 4:�м�
	BYTE alpha;				//0~255, ͸���ȿ���

	char imgPath[MAX_PATH];	//ˮӡͼƬ·��, ��ʽ֧��: jpg/bmp/png/jpeg/gif
	char *pStrText;			//ˮӡ����
	COLORREF textColor;					//������ɫ
	char faceName[LF_FACESIZE];		//��������
	LONG lHeight;						//�����С
	LONG lWeight;						//��������
	BYTE lItalic;						//�Ƿ���б

	ELvideoCapWatermark()
	{
		bWatermark = FALSE;
		bAddTime = FALSE;
		mode = 0;
		pos = EL_VIDEO_CAP_TOPLEFT;
		alpha = 255;
		pStrText = NULL;
		ZeroMemory(imgPath, sizeof(imgPath));
		textColor = RGB(255,0,0);
		lHeight = 42;
		lWeight = FW_NORMAL;
		lItalic = FALSE;
		(GetUserDefaultLangID()==0x0804) ? strcpy_s(faceName, sizeof(faceName), "����")/*��������*/
			:strcpy_s(faceName, sizeof(faceName), "Courier New");
	}
}ELvideoCapWatermark;


//ƬͷƬβ
typedef struct ELvideoCapFilmTitleTail
{
	char imgPathTitle[MAX_PATH];
	char imgPathTail[MAX_PATH];
	int tailTimeLen, titleTimeLen;
	int tailFeatureType, titleFeatureType;

	ELvideoCapFilmTitleTail()
	{
		ZeroMemory(imgPathTail, sizeof(imgPathTail));
		ZeroMemory(imgPathTitle, sizeof(imgPathTitle));
		tailTimeLen = titleTimeLen = 0;
		tailFeatureType = titleFeatureType = 0;
	}
}ELvideoCapFilmTitleTail;

#pragma pack(pop)

typedef unsigned int ELcolor;

/* info level */
#define EL_INFO_LEVEL_FATAL		0x1
#define EL_INFO_LEVEL_ERROR		0x2
#define EL_INFO_LEVEL_WARNING	0x3
#define EL_INFO_LEVEL_INFO		0x4

/* image format */
#define EL_FORMAT_UNKNOWN		-1
#define EL_FORMAT_BMP			0
#define EL_FORMAT_ICO			1
#define EL_FORMAT_JPEG			2
#define EL_FORMAT_PNG			13
#define EL_FORMAT_TIFF			18
#define EL_FORMAT_GIF			25

/* image load and save flag */
#define EL_JPEG_QUALITYSUPERB	0x80	//! save with superb quality (100:1)
#define EL_JPEG_QUALITYGOOD		0x0100	//! save with good quality (75:1)
#define EL_JPEG_QUALITYNORMAL	0x0200	//! save with normal quality (50:1)
#define EL_JPEG_QUALITYAVERAGE	0x0400	//! save with average quality (25:1)
#define EL_JPEG_QUALITYBAD		0x0800	//! save with bad quality (10:1)
//1~100 decimal integer x, save with x:1 

#define EL_TIFF_DEFAULT			0
#define EL_TIFF_PACKBITS		0x0100  //! save using PACKBITS compression
#define EL_TIFF_DEFLATE			0x0200  //! save using DEFLATE compression (a.k.a. ZLIB compression)
#define EL_TIFF_ADOBE_DEFLATE	0x0400  //! save using ADOBE DEFLATE compression
#define EL_TIFF_NONE			0x0800  //! save without any compression
#define EL_TIFF_CCITTFAX3		0x1000  //! save using CCITT Group 3 fax encoding
#define EL_TIFF_CCITTFAX4		0x2000  //! save using CCITT Group 4 fax encoding
#define EL_TIFF_LZW				0x4000	//! save using LZW compression
#define EL_TIFF_JPEG			0x8000	//! save using JPEG compression
#define EL_TIFF_LOGLUV			0x10000	//! save using LogLuv compression

#define EL_PNG_DEFAULT					0
#define EL_PNG_Z_BEST_SPEED				0x0001	//! save using ZLib level 1 compression flag (default value is 6)
#define EL_PNG_Z_DEFAULT_COMPRESSION	0x0006	//! save using ZLib level 6 compression flag (default recommended value)
#define EL_PNG_Z_BEST_COMPRESSION		0x0009	//! save using ZLib level 9 compression flag (default value is 6)
#define EL_PNG_Z_NO_COMPRESSION			0x0100	//! save without ZLib compression
#define EL_PNG_INTERLACED				0x0200	//! save using Adam7 interlacing (use | to combine with other save flags)

/* image create flag */
#define EL_CREATE_TOP_LEFT		0x1
#define EL_CREATE_BOTTOM_LEFT	0x2

/* image channels */
#define EL_CHANNELS_GRAY		0x1
#define EL_CHANNELS_COLOR		0x3

/* image prop */
#define EL_PROP_WIDTH			0x1
#define EL_PROP_HEIGHT			0x2
#define EL_PROP_CHANNELS		0x3
#define EL_PROP_WIDTHSTEP		0x4
#define EL_PROP_XDPI			0x5
#define EL_PROP_YDPI			0x6

/* image cvt */
#define EL_CVT_COLOR2GRAY		0x6
#define	EL_CVT_GRAY2COLOR		0x8

/* image rotate */
#define EL_ROTATE_KEEP_SIZE		0x0
#define EL_ROTATE_KEEP_INFO		0x1

/* image flip */
#define EL_FLIP_HORIZONTAL		0
#define EL_FLIP_VERTICAL		1
#define EL_FLIP_BOTH			-1

#define EL_DESKEW_CLEAN			0
#define EL_DESKEW_MORE			1

/* image rectify */
#define EL_RECTIFY_NONE			0
#define EL_RECTIFY_IDCARD		1

/* color */
#define EL_RGB(r, g, b) ((ELcolor)(((unsigned char)(r) \
	| ((unsigned short)((unsigned char)(g)) << 8)) \
	| (((unsigned int)(unsigned char)(b)) << 16)))

/* dev type */
#define EL_DEV_TYPE_VIDEO			0x1
#define EL_DEV_TYPE_AUDIO			0x2

#define EL_ELOAM_TYPE_VIDEO1		0x1
#define EL_ELOAM_TYPE_VIDEO2		0x2
#define EL_ELOAM_TYPE_VIDEO3		0x3
#define EL_ELOAM_TYPE_VIDEO4		0x4

#define EL_DEV_DBT_ARRIVAL			0x1
#define EL_DEV_DBT_REMOVE			0x2

#define EL_DEV_SUBTYPE_YUY2			0x1
#define EL_DEV_SUBTYPE_MJPG			0x2
#define EL_DEV_SUBTYPE_UYVY			0x4
#define EL_DEV_SUBTYPE_RGB24        0x8
#define EL_DEV_SUBTYPE_H264        0x10

#define EL_SCAN_SIZE_A1				0x01
#define EL_SCAN_SIZE_A2				0x02
#define EL_SCAN_SIZE_A3				0x04
#define EL_SCAN_SIZE_A4				0x08
#define EL_SCAN_SIZE_A5				0x10
#define EL_SCAN_SIZE_A6				0x20
#define EL_SCAN_SIZE_A7				0x40

/* dev stat */
#define EL_DEV_STAT_READY			0x0
#define EL_DEV_STAT_VIEW_PENDING	0x1
#define EL_DEV_STAT_VIEWING			0x2
#define EL_DEV_STAT_PAUSE			0x3

/* dev video procamp */
#define EL_VP_BRIGHTNESS			0x1 
#define	EL_VP_CONTRAST				0x2  
#define	EL_VP_SATURATION			0x3 
#define	EL_VP_HUE					0x4 
#define	EL_VP_SHARPNESS				0x5 
#define	EL_VP_GAMMA					0x6 
#define	EL_VP_WHITEBALANCE			0x7 
#define	EL_VP_BACKLIGHTCOMPENSATION 0x8 
#define	EL_VP_COLORENABLE			0x9 
#define	EL_VP_GAIN					0xA

/*record video*/
#define EL_RV_BEGIN					0x0
#define EL_RV_END					0x1
#define EL_RV_ERR					0x2

/* dev camera control */
#define EL_CC_EXPOSURE				0x1
#define	EL_CC_PAN					0x2
#define	EL_CC_TILT					0x3
#define	EL_CC_ROLL					0x4
#define	EL_CC_ZOOM					0x5
#define	EL_CC_IRIS					0x6
#define	EL_CC_FOCUS					0x7
#define	EL_CC_LIGHT					0x8

/* dev webcam value */
#define EL_WV_MINIMUM				0x1
#define	EL_WV_MAXIMUM				0x2
#define	EL_WV_STEP					0x3
#define	EL_WV_DEFAULT				0x4
#define	EL_WV_VALUE					0x5
#define	EL_WV_AUTO					0x6

/* dev mode*/
#define EL_DEV_MODE_LOCAL			0x0
#define	EL_DEV_MODE_REMOTE			0x1

/* dev contrl panel */

/*button upload define*/
#define EL_CTRL_ONE_KEY_RECORDING			0x1 //һ��¼��¼��
#define EL_CTRL_SCAN						0x2 //����
#define EL_CTRL_AF_FOCUS					0x3 //AF�Զ��Խ�
#define EL_CTRL_RESOLUTION_SWITCHING		0x4 //�ֱ����л�
#define EL_CTRL_TEXT_OR_IMAGE				0x5 //�ı���ͼ��ѭ���л�
#define EL_CTRL_LEFT						0x6 //����/��ͷ�������΢��
#define EL_CTRL_RIGHT						0x7 //����/��ͷ������Զ΢��
#define EL_CTRL_UP 							0x8 //����/ͼ��Ŵ�
#define EL_CTRL_DOWN						0x9 //����/ͼ���С
#define EL_CTRL_OK		 					0x10 //OK/ȷ��
#define EL_CTRL_DELETE		 				0x11 //ɾ��
#define EL_CTRL_FILE_PREVIEW		 		0x12 //�ļ�Ԥ��
#define EL_CTRL_SCREEN_ROTATE		 		0x13 //������ת
#define EL_CTRL_SCREEN_FREEZES		 		0x14 //���ⶳ�ᡢȫ�����ᡢ���ѭ���л�
#define EL_CTRL_SCREEN_NORMAL		 		0x15 //���񡢺ڰס�����Ƭ������ѭ���л�
#define EL_CTRL_SCREEN_COMPARE		 		0x16 //ͬ���Ա�
#define EL_CTRL_VHU_SWITCHING		 		0x17 //VGA��HDMI/USBѭ���л�
#define EL_CTRL_LANGUAGE_SWITCHING			0x18 //�����л�
#define EL_CTRL_BRIGHTNESS_SUB				0x19 //����-
#define EL_CTRL_BRIGHTNESS_ADD				0x20 //����+
#define EL_CTRL_EXIT						0x21 //���ء��˳�
#define EL_CTRL_MENU						0x22 //�˵�������ѡ��
#define EL_CTRL_MICROPHONE_OPEN_CLOSE		0x23 //��˷翪��
#define EL_CTRL_COLOR_BLACK_SWITCHING		0x24 //��ɫ���ڰ�ѭ���л�
#define EL_CTRL_FILL_LIGHT_SWITCHING		0x25 //����ƿ��أ�0-1����
#define EL_CTRL_FILL_AND_LIGHT_SWITCHING	0x26 //����ƿ��������ȵ��ڣ�0-3����
#define EL_CTRL_COLOR_LIGHT_SWITCHING_FOUR	0x27 //1�׹⡢2��ɫ��3��ɫ��4��ɫ�����ĵ���ѭ���л�
#define EL_CTRL_COLOR_LIGHT_SWITCHING_THREE	0x28 //1��ɫ��2��ɫ��3��ɫ��      ��������ѭ���л�

/*Remote control upload define*/
#define EL_CTRL_ZHANGTAILIANJIE				0x29 //չ̨����
#define EL_CTRL_WENBENTUXIANG				0x30 //�ı�/ͼ��
#define EL_CTRL_BUGUANGDENG					0x31 //�����
#define EL_CTRL_DONGJIE						0x32 //����
#define EL_CTRL_TUPIANLIULAN				0x33 //ͼƬ���
#define EL_CTRL_TUXIANGXIAOGUO				0x34 //ͼ��Ч��
#define EL_CTRL_FANGDA						0x35 //�Ŵ�
#define EL_CTRL_PAIZHAO						0x36 //����
#define EL_CTRL_SUOXIAO						0x37 //��С
#define EL_CTRL_XIANGZUO					0x38 //����
#define EL_CTRL_SHANCHU						0x39 //ɾ��
#define EL_CTRL_XIANGYOU					0x40 //����
#define EL_CTRL_JUJING						0x41 //�۽�
#define EL_CTRL_JUJIAO						0x42 //�۽�
#define EL_CTRL_JUYUAN						0x43 //��Զ
#define EL_CTRL_FENGBIANLV					0x44 //�ֱ���
#define EL_CTRL_XUANZHUAN					0x45 //��ת
#define EL_CTRL_ERFENGPINGXIAN				0x46 //��������
#define EL_CTRL_F1							0x47 //F1
#define EL_CTRL_F2							0x48 //F2
#define EL_CTRL_F3							0x49 //F3



/*Xu Tool*/
#define EL_XUTOOL_NORMAL                    0X0//δ����
#define EL_XUTOOL_PRESS                     0X1//���°���

/* [device callback]
 * type: EL_DEV_TYPE_VIDEO or EL_DEV_TYPE_AUDIO
 * idx: index of device
 * dbt: EL_DEV_DBT_ARRIVAL or EL_DEV_DBT_REMOVE
 * param: user data
 * you can change ui in the function
 */
typedef void (EL_CDECL *devCallback)(
	int type, 
	int idx, 
	int dbt, 
	void *param
	);

/* [start preview callback]
 * dev: device
 * img: image of frame
 * id: id of frame
 * param: user data
 */
typedef void (EL_CDECL *viewCallback)(
	ELdevice dev, 
	ELimage img, 
	int id, 
	void *param
	);

typedef void (EL_CDECL *ImageCallback)(	
	ELimage img,
	int id,
	void *param
	);
/* [touch capture callback]
 * dev: device
 * param: user data
 */
typedef void (EL_CDECL *touchCallback)(
	ELdevice dev,
	void *param
	);

/* [http callback]
 * http: http object
 * type: upload or download
 * flag: progress or result
 * value: value for flag
 * param: user data
 */
typedef void (EL_CDECL *httpCallback)(
	ELhttp http,
	int type,
	int flag,
	int value,
	void *param
	);

/* [ftp callback]
 * ftp: ftp object
 * type: upload or download
 * flag: progress or result
 * value: value for flag
 * param: user data
 */


typedef void (EL_CDECL *ftpCallback)(
	ELftp ftp,
	int type,
	int flag,
	int value,
	void *param
	);

/* [record callback]
 * dev: device
 * type: record state
 * flag: 
 * param: user data
 */
typedef void (EL_CDECL *recordCallback)(
	ELdevice dev,
	int type,
	int flag,
	void *param
	);

/*[control callback]
 *ret:index of control
 *param:user data
*/
typedef void (EL_CDECL *controlCallback)(
	int ret,
	void *param
	);

/* ocr dicern callback */
typedef void (CDECL *ocrCallback)(
	int flag, 
	int ret,
	void *param
	);

/* templ dicern callback */
typedef void (CDECL *templCallback)( 
	int ret,
	void *param
	);

/* invoice dicern callback */
typedef void (CDECL *invoiceCallback)(
	int ret,
	void *param
	);

/* XuTool callback 
*ret:press button or not
*param:user data
*/
typedef void (EL_CDECL *xutoolCallback)(
	int ret,
	void *param
	);

ELAPI(int)
elWriteInfo(
	int level,
	const char *format,
	...
	);

ELAPI(int)
	elSetLogNameFormat(
	const char * prefix,
	int flag EL_DEFAULT(0)
	);

ELAPI(ELimage)
elLoadImage(
	const char *fileName,
	int flag EL_DEFAULT(0)
	);

ELAPI(ELimage)
elCreateImage(
	const ELsize *size,
	int channels EL_DEFAULT(EL_CHANNELS_COLOR),
	void *data EL_DEFAULT(NULL),
	int step EL_DEFAULT(-1),
	int flag EL_DEFAULT(0)
	);

#ifdef _WIN32
ELAPI(ELimage)
elCreateImageFromDDB(
	HBITMAP bmp,
	HDC hdc EL_DEFAULT(NULL)
	);

ELAPI(ELimage)
elCreateImageFromDIB(
	HBITMAP bmp
	);
#endif

ELAPI(ELimage)
elCloneImage(
	ELimage image
	);

ELAPI(int)
elCopyImage(
	ELimage dest,
	ELimage src
	);

ELAPI(int)
elSaveImage(
	ELimage image,
	const char *fileName,
	int flag EL_DEFAULT(0)
	);

ELAPI(int)
	elSaveList(
	const char *fileName,
	ELimage *list,
	int len,
	int flag
	);

//ELAPI(int)
//	elSaveListEx(
//	const char *fileName,
//	ELimage *list,
//	int len,
//	int flag
//	);

ELAPI(int)
	elSaveList(
	const char *fileName,
	ELimage *list,
	int len,
	int flag EL_DEFAULT(0)
	);

ELAPI(int)
elReleaseImage(
	ELimage image
	);

ELAPI(int)
elGetImageProperty(
	ELimage image,
	int flag
	);

ELAPI(int)
elSetImageProperty(
	ELimage image,
	int flag,
	int value
	);

ELAPI(void *)
elGetImageData(
	ELimage image
	);

#ifdef _WIN32
ELAPI(HBITMAP)
elCreateDDB(
	ELimage image,
	HDC hdc EL_DEFAULT(NULL)
	);

ELAPI(HBITMAP)
elCreateDIB(
	ELimage image
	);

ELAPI(int)
elDrawText(
	ELimage image,
	HFONT font,
	const ELpoint *pt,
	const char *text,
	ELcolor clr EL_DEFAULT(EL_RGB(0, 0, 0)),
	int weight EL_DEFAULT(0),
	ELimage *dest EL_DEFAULT(NULL)
	);

ELAPI(int)
elDrawTextOnHBITMAP(
	HBITMAP hBmp,
	HFONT font,
	const ELpoint *pt,
	const char *text,
	ELcolor clr EL_DEFAULT(EL_RGB(0, 0, 0)),
	int weight EL_DEFAULT(0)
	);
#endif

ELAPI(int)
elRotateImage(
	ELimage image,
	float angle,
	int flag EL_DEFAULT(EL_ROTATE_KEEP_INFO),
	ELcolor clr EL_DEFAULT(EL_RGB(0, 0, 0)),
	ELimage *dest EL_DEFAULT(NULL)
	);

ELAPI(int)
elCropImage(
	ELimage image,
	const ELrect *rect,
	ELimage *dest EL_DEFAULT(NULL)
	);

ELAPI(int)
elResizeImage(
	ELimage image,
	const ELsize *size,
	ELimage *dest EL_DEFAULT(NULL)
	);

ELAPI(int)
elBlendImage(
	ELimage image1,
	const ELrect *rect1,
	ELimage image2,
	const ELrect *rect2,
	int weight EL_DEFAULT(0),
	ELimage *dest EL_DEFAULT(NULL)
	);

ELAPI(int)
elCvtImageColor(
	ELimage image,
	int flag,
	ELimage *dest EL_DEFAULT(NULL)
	);

ELAPI(int)
elThresholdImage(
	ELimage image,
	int threshold,
	ELimage *dest EL_DEFAULT(NULL)
	);

ELAPI(int)
elAdaptiveThresholdImage(
	ELimage image,
	int flag EL_DEFAULT(0),
	ELimage *dest EL_DEFAULT(NULL)
	);

ELAPI(int)
elDeskewImage(
	ELimage image,
	int flag EL_DEFAULT(EL_DESKEW_CLEAN),
	ELregion *region EL_DEFAULT(NULL),
	ELimage *dest EL_DEFAULT(NULL)
	);

ELAPI(int)
elDelImageBkColor(
	ELimage image,
	ELimage *dest EL_DEFAULT(NULL)
	);

ELAPI(int)
elDelImageBackgrand(
	ELimage image,
	ELimage *dest EL_DEFAULT(NULL)
	);

ELAPI(int)
elImageDenoising(
	ELimage image,
	ELimage *dest EL_DEFAULT(NULL)
	);

ELAPI(int)
elImageGetBlue(
	ELimage image,
	ELimage *dest EL_DEFAULT(NULL)
	);

ELAPI(int)
	elHighLightRremove(
	ELimage image,
	ELimage *dest EL_DEFAULT(NULL)
	);

ELAPI(int)
elFlipImage(
	ELimage image,
	int flag,
	ELimage *dest EL_DEFAULT(NULL)
	);

ELAPI(int)
elSmoothImage(
	ELimage image,
	int flag EL_DEFAULT(0),
	ELimage *dest EL_DEFAULT(NULL)
	);

ELAPI(int)
elCompareImage(
	ELimage image,
	ELimage image2
	);

ELAPI(int)
elReverseImage(
	ELimage image,
	ELimage *dest EL_DEFAULT(NULL)
	);

ELAPI(int)
elRectifyImage(
	ELimage image,
	int flag EL_DEFAULT(0),
	ELimage *dest EL_DEFAULT(NULL)
	);

ELAPI(int)
elMultiDeskewImage(
	ELimage image,
	int flag,
	ELregion *regionList EL_DEFAULT(NULL),
	ELimage *imagelist EL_DEFAULT(NULL)
	);

ELAPI(int)
elEmbossImage(
	ELimage image,
	ELimage *dest EL_DEFAULT(NULL)
	);

ELAPI(int)
elSharpen(
	ELimage image,
	ELimage *dest EL_DEFAULT(NULL)
	);

ELAPI(int)
	elBalanceImage(
	ELimage image,
	ELimage *dest EL_DEFAULT(NULL)
	);

ELAPI(int)
	elGetImagemean (
	ELimage image,
	int *mean,
	int *dev
	);

ELAPI(int)
elFind_rect(
ELimage image,
ELrect *rect);

ELAPI(int)
	elIdCardProcess(
	ELimage image);

ELAPI(int)
	elFormProcess(
	ELimage image);


ELAPI(int)
	elMorphologyImage(
	ELimage image, 
	LONG x, 
	LONG y, 
	LONG w, 
	LONG h);

ELAPI(void *)
elCreateMemory(
	int fmt,
	ELimage image,
	int *len,
	int flag EL_DEFAULT(0)
	);

ELAPI(ELimage)
elCreateImageFromMemory(
	const void *mem,
	int len,
	int flag EL_DEFAULT(0)
	);

ELAPI(int)
elReleaseMemory(
	void *mem
	);

ELAPI(char *)
elCreateBase64(
	void *mem,
	int len
	);

ELAPI(void *)
elCreateMemoryFromBase64(
	const char *data,
	int *len
	);

ELAPI(int)
elReleaseBase64(
	char *data
	);

ELAPI(ELfile)
elOpenFile(
	const char *fileName,
	int flag EL_DEFAULT(0)
	);

ELAPI(int)
elGetImageCount(
	ELfile file
	);

ELAPI(ELimage)
elGetImage(
	ELfile file,
	int idx
	);

ELAPI(ELfile)
elCreateFile(
	const char *fileName,
	int flag EL_DEFAULT(0)
	);

ELAPI(int)
elAppendImage(
	ELfile file,
	ELimage image
	);

ELAPI(int)
elReleaseFile(
	ELfile file
	);

ELAPI(int)
elLoadList(
	const char *fileName,
	ELimage *list,
	int *len,
	int flag EL_DEFAULT(0)
	);


ELAPI(int)
elSetRemoteParameter(
	int flag,
	const char * ip,
	int port
	);

ELAPI(int)
elInitDevs(
	devCallback devFun EL_DEFAULT(NULL),
	void *devParam EL_DEFAULT(NULL)
	);

ELAPI(int)
elDeinitDevs(
	void
	);

ELAPI(int)
elInitCtrlPanel(
	controlCallback contrlfun EL_DEFAULT(NULL),
	void *devParam EL_DEFAULT(NULL)
	);

ELAPI(int)
elDenitCtrlPanel(
	void
	);

ELAPI(int)
elSoftDog(
	bool open
	);

ELAPI(int)
elSoftDogGetKey(
	char *key,
	int len
	);
//pdf
ELAPI(int)
elAddFile(
	const char* fileName
	);

ELAPI(int)
elGetFileCount(
	int* count
	);

//ELAPI(int)
//elGetFilePath(
//	char *fileName,
//	int idx
//	);

ELAPI(int)
elCreatePDF(
	const char *fileName
	);

ELAPI(int)
elClearFile(
	void
	);
//
ELAPI(int)
elGetDevCount(
	int type
	);

ELAPI(int)
elGetDisplayName(
	int type,
	int idx,
	char *name
	);

ELAPI(int)
elGetFriendlyName(
	int type,
	int idx,
	char *name
	);

ELAPI(int)
elGetEloamType(
	int type,
	int idx
	);

ELAPI(ELdevice)
elCreateDevice(
	int type,
	int idx EL_DEFAULT(0)
	);

ELAPI(int)
elReleaseDevice(
	ELdevice dev
	);

ELAPI(int)
elGetDevType(
	ELdevice dev
	);

ELAPI(int)
elGetDevIndex(
	ELdevice dev
	);

ELAPI(int)
elGetDevState(
	ELdevice dev
	);

ELAPI(int)
elGetDevFriendlyName(
	ELdevice dev,
	char *name
	);

ELAPI(int)
elGetDevDisplayName(
	ELdevice dev,
	char *name
	);

ELAPI(int)
	elHasResizeResolution(
	ELdevice dev,
	int *w, 
	int *h
	);

ELAPI(int)
elGetDevEloamType(
	ELdevice dev
	);

ELAPI(int)
elGetDevScanSize(
	ELdevice dev
	);

ELAPI(int)
	elGetSDKtype(
	ELdevice dev
	);

ELAPI(int)
elGetSubtype(
	ELdevice dev
	);

ELAPI(int)
	elGetFrameRate(
	ELdevice dev
	);

ELAPI(int)
elGetResolutionCount(
	ELdevice dev
	);

ELAPI(int)
elGetResolutionWidth(
	ELdevice dev,
	int idx
	);

ELAPI(int)
elGetResolutionHeight(
	ELdevice dev,
	int idx
	);


ELAPI(int)
	elGetResolutionCountEx(
	ELdevice dev,
	int subType
	);

ELAPI(int)
	elGetResolutionWidthEx(
	ELdevice dev,
	int subType,
	int idx
	);

ELAPI(int)
	elGetResolutionHeightEx(
	ELdevice dev,
	int subType,
	int idx);

ELAPI(int)
	elGetCurrentResolutionWidth(
	ELdevice dev
	);

ELAPI(int)
	elGetCurrentResolutionHeight(
	ELdevice dev
	);

ELAPI(int)
elChangeComposeMode(
	ELdevice dev,
	int mode
	);
/*****************************˵��************************************
����������stillcapture ��ȡͼ������ù���
stillImageFun�� ���ûص�����still ͼ������
stillImageParam�� ���ö���
resolutionForStill�� still captureͨ���ķֱ���
subtypeForStill�� still capture ͨ���ĳ�ͼ��ʽ
**********************************************************************/
ELAPI(int)
elStartPreview(
	ELdevice dev,
	viewCallback viewFun,
	void *viewParam,
	touchCallback touchFun,
	void *touchParam,
	int resolution EL_DEFAULT(0),
	int subtype EL_DEFAULT(EL_DEV_SUBTYPE_YUY2),
	ImageCallback stillImageFun = NULL,
	void* stillImageParam = NULL,
	int resolutionForStill EL_DEFAULT(0),
	int subtypeForStill EL_DEFAULT(EL_DEV_SUBTYPE_MJPG)
	);

ELAPI(int)
elStopPreview(
	ELdevice dev
	);

ELAPI(int)
	elImageProcess(
	ELdevice dev,
	char * pData, 
	int width,
	int height
	);

ELAPI(int)
elPausePreview(
	ELdevice dev
	);

ELAPI(int)
elResumePreview(
	ELdevice dev
	);

ELAPI(int)
elGetStillImage(
	ELdevice dev
	);

ELAPI(ELimage)
	elGetImageFromDev(
	ELdevice dev
	);

#ifdef _WIN32
ELAPI(int)
elShowProperty(
	ELdevice dev,
	HWND hOwner
	);
#endif

#ifdef _WIN32
ELAPI(int)
	elShowVideoCapturePin(
	ELdevice dev,
	HWND hOwner
	);
#endif

ELAPI(int)
elGetVideoProcAmp(
	ELdevice dev,
	int prop, 
	int value
	);

ELAPI(int)
elSetVideoProcAmp(
	ELdevice dev,
	int prop, 
	int value, 
	int isAuto
	);

ELAPI(int)
elGetCameraControl(
	ELdevice dev,
	int prop, 
	int value
	);

ELAPI(int)
elSetCameraControl(
	ELdevice dev,
	int prop, 
	int value, 
	int isAuto
	);

ELAPI(int)
elGetSonixSerialNumber(
	ELdevice dev,
	char *number
	);

ELAPI(int) 
	elStartRecord(
	ELdevice dev,
	const char *filePath,
	int flag
	);

ELAPI(int) 
	elStopRecord(
	ELdevice dev
	);

ELAPI(ELvideoWriter)
elCreateVideoWriter(
	const char *fileName,
	double fps, 
	const ELsize *size,
	int flag EL_DEFAULT(0)
	);

ELAPI(int)
elWriteVideoFrame(
	ELvideoWriter writer,
	ELimage image
	);

ELAPI(int)
elReleaseVideoWriter(
	ELvideoWriter writer
	);

ELAPI(ELftp)
elCreateFtp(
	const char *ftpPath
	);

ELAPI(int)
elReleaseFtp(
	ELftp ftp
	);

ELAPI(int)
elUploadFtp(
	ELftp ftp,
	const char *localPath,
	const char *remotePath,
	ftpCallback fun,
	void *param
	);

ELAPI(int)
elStopFtpUpload(
	ELftp ftp
	);

ELAPI(int)
elWaitFtpUpload(
	ELftp ftp
	);

ELAPI(int)
elFtpCreateDir(
	ELftp ftp,
	const char *dir
	);

ELAPI(int)
elFtpRemoveDir(
	ELftp ftp,
	const char *dir
	);

ELAPI(ELhttp)
elCreateHttp(
	const char *httpPath
	);

ELAPI(int)
elReleaseHttp(
	ELhttp http
	);

ELAPI(int)
elUploadFileHttp(
	ELhttp http,
	const char *localPath,
	const char *headers, 
	const char *predata,
	const char *taildata,
	httpCallback fun,
	void *param
	);

ELAPI(int)
elUploadMemoryHttp(
	ELhttp http,
	const void *mem,
	int len,
	const char *headers, 
	const char *predata,
	const char *taildata,
	httpCallback fun,
	void *param
	);

ELAPI(int)
elUploadImageFileHttp(
	ELhttp http,
	const char *fileName, 
	const char *remoteName, 
	httpCallback fun,
	void *param
	);

ELAPI(int)
elUploadImageHttp(
	ELhttp http,
	const void *mem, 
	int len, 
	const char *remoteName, 
	httpCallback fun, 
	void *param
	);

ELAPI(int)
elStopHttpUpload(
	ELhttp http
	);

ELAPI(int)
elWaitHttpUpload(
	ELhttp http
	);

ELAPI(int)
elGetHttpServerInfo(
	ELhttp http,
	char *info
	);

#ifdef _WIN32

ELAPI(int)
	elGetPrinterCount(
	);

ELAPI(int)
	elGetPrinterName(
	int printerId,
	char * printerName
	);

ELAPI(int)
elPrintHBITMAP(
	HBITMAP hBmp,
	float x,
	float y,
	float width,
	float height,
	const char *printer EL_DEFAULT(NULL)
	);

ELAPI(int)
elAdaptivePrintHBITMAP(
	HBITMAP hBmp,
	float width,
	float height,
	const char *printer EL_DEFAULT(NULL)
	);
#endif
ELAPI(void) elRasSetkey();

ELAPI(int)
elRasEncrypt(
	char *result,
	char *data,
	int len,
	const char *keyStr_e,
	const char *keyStr_n
	);

ELAPI(int)
elRasDecrypt(
	char *result,
	char *data,
	int len,
	const char *keyStr_d,
	const char *keyStr_n
	);

ELAPI(int)
elEncrypt(
	char *data,
	int len,
	const char *keyStr
	);

ELAPI(int)
elDecrypt(
	char *data,
	int len,
	const char *keyStr
	);

ELAPI(int)
elGetMD5(
	char *result,
	const char *data,
	int len
	);

ELAPI(ELimage)
elGetRemoteImage(
	void
	);

ELAPI(int)
	elGetDeviceMode(
	void
	);

ELAPI(int)
elInitOcr(
	void
	);

ELAPI(int)
elDeinitOcr(
	void
	);

ELAPI(int)
elSetOcrLanguage(
	int lang
	);

ELAPI(int)
elDiscernOcrList(
	int flag,
	ELOcrImage *list, 
	int count,
	ocrCallback fun,
	void *param
	);

ELAPI(int)
elDiscernOcr(
	int flag,
	ELimage image,
	const ELrect *rc,
	ocrCallback fun,
	void *param
	);

ELAPI(int)
elGetOcrPageCount(
	void
	);

ELAPI(int)
elGetOcrBlockCount(
	int page
	);

ELAPI(int)
elGetOcrBarcodeType(
	int page,
	int block
	);

ELAPI(int)
elGetOcrPlainText(
	int page,
	wchar_t *data
	);

ELAPI(int)
elGetOcrData(
	int page,
	int block, 
	wchar_t *data
	);

ELAPI(int)
elGetOcrLocation(
	int page,
	int block,
	ELrect *rc
	);

ELAPI(int)
elSaveOcr(
	const char *filePath,
	int flag EL_DEFAULT(0)
	);

ELAPI(int)
elStopOcrDiscern(
	void
	);

ELAPI(int)
elWaitOcrDiscern(
	void
	);

ELAPI(ELRtempl)
elCreateTempl(
	const char *templName
	);

ELAPI(ELRtempl)
elLoadTempl(
	const char *fileName,
	int flag EL_DEFAULT(0)
	);

ELAPI(ELRtempl)
elLoadTemplFromXML(
	const wchar_t *data,
	int flag EL_DEFAULT(0)
	);

ELAPI(int)
elReleaseTempl(
	ELRtempl templ
	);

ELAPI(ELRtempl)
elCloneTempl(
	ELRtempl templ
	);

ELAPI(int)
elCopyTempl(
	ELRtempl dest,
	ELRtempl src
	);

ELAPI(int)
elGetTemplName(
	ELRtempl templ,
	char *name
	);

ELAPI(int)
elGetTemplId(
	ELRtempl templ,
	char *id
	);

ELAPI(int)
elTemplAppendField(
	ELRtempl templ,
	const char *fieldName,
	int type,
	float left,
	float top, 
	float right, 
	float bottom
	);

ELAPI(int)
elTemplClearField(
	ELRtempl templ
	);

ELAPI(int)
elTemplGetFieldCount(
	ELRtempl templ
	);

ELAPI(int)
elGetTemplFieldName(
	ELRtempl templ,
	int idx,
	char *name
	);

ELAPI(int)
elGetTemplFieldType(
	ELRtempl templ,
	int idx
	);

ELAPI(int)
elGetTemplFieldRect(
	ELRtempl templ,
	int idx,
	float *left, 
	float *top, 
	float *right, 
	float *bottom
	);

ELAPI(int)
elSetTemplFieldResult(
	ELRtempl templ,
	int idx, 
	const wchar_t *result
	);

ELAPI(int)
elGetTemplFieldResult(
	ELRtempl templ,
	int idx,
	wchar_t *result
	);

ELAPI(int)
elGetTemplXML(
	ELRtempl templ,
	wchar_t *data,
	int flag EL_DEFAULT(0)
	);

ELAPI(int)
elSaveTempl(
	ELRtempl templ,
	const char *fileName,
	int flag EL_DEFAULT(0)
	);


ELAPI(int)
elInitTemplOcr(
	void
	);

ELAPI(int)
elDeinitTemplOcr(
	void
	);

ELAPI(int)
elSetTemplOcrLanguage(
	int lang
	);

ELAPI(int)
elDiscernTempl(
	ELimage image,
	ELRtempl templ,
	templCallback fun,
	void *param
	);

ELAPI(ELRtempl)
elGetTemplResult(
	void
	);

ELAPI(int)
elStopTemplDiscern(
	void
	);

ELAPI(int)
elWaitTemplDiscern(
	void
	);


ELAPI(int)
elInitBarcode(
	void
	);

ELAPI(int)
elDeinitBarcode(
	void
	);

ELAPI(int)
elDiscernBarcode(
	ELimage image,
	const ELrect *rc EL_DEFAULT(NULL)
	);

ELAPI(int)
elGetBarcodeCount(
	void
	);

ELAPI(int)
elGetBarcodeType(
	int idx
	);

ELAPI(int)
elGetBarcodeData(
	int idx,
	wchar_t *data
	);

ELAPI(int)
elGetBarcodeLocation(
	int idx,
	ELpoint *pt
	);


ELAPI(int)
elQuickOcr(
	const char *fileName,
	const char *resultName,
	int flag
	);

/********** �°�¼��¼�� ***********/
/*
* ¼����ʼ��
*/
ELAPI(int) elVideoCapInit(
	/*void*/
	);

/*
* ����¼��ʵ��
*/
ELAPI(ELVideoCap) elCreatVideoCap(
	/*void*/
	);

/*
* ����¼��ʵ��
* hVideoCap: in, ¼��ʵ�����
*/
ELAPI(int) elDestroyVideoCap(
	ELVideoCap hVideoCap
	);

/*
* ¼���������
* hVideoCap: in, ¼��ʵ�����
* pCapConf: in, ��������
*/
ELAPI(int) elVideoCapPreCap(
	ELVideoCap hVideoCap, 
	ELvideoCapConf *pCapConf
	);

/*
* ����¼��
* hVideoCap: in, ¼��ʵ�����
*/
ELAPI(int) elVideoCapStart(
	ELVideoCap hVideoCap
	);

/*
* ֹͣ¼��
* hVideoCap: in, ¼��ʵ�����
*/
ELAPI(int) elVideoCapStop(
	ELVideoCap hVideoCap
	);

/*
* ��ͣ¼��
* hVideoCap: in, ¼��ʵ�����
*/
ELAPI(int) elVideoCapPause(
	ELVideoCap hVideoCap
	);

/*
* ��ȡ��ǰ״̬
* hVideoCap: in, ¼��ʵ�����
*/
ELAPI(int) elVideoCapGetState(
	ELVideoCap hVideoCap
	);

/*
* ¼��ˮӡ����, ����¼��֮ǰ����
* hVideoCap: in, ¼��ʵ�����
* pWatermarkConf: in, ˮӡ����
*/
ELAPI(int) elVideoCapSetWatermark(
	ELVideoCap hVideoCap,
	ELvideoCapWatermark *pWatermarkConf
	);

/*
* ������Ƶ��ע��Ϣ, ��Ϣ���ݽ�д����Ƶ�ļ�, ����¼��֮ǰ����
* hVideoCap: in, ¼��ʵ�����
* pStrComment: in, ��Ϣ����
*/
ELAPI(void) elVideoCapSetComment(
	ELVideoCap hVideoCap, 
	char *pStrComment
	);

/*
* ƬͷƬβ��������, ����¼��֮ǰ����
* hVideoCap: in, ¼��ʵ�����
* pFilmTitleTailConf: in, ����
*/
ELAPI(int) elVideoCapSetFilmTitleTail(
	ELVideoCap hVideoCap,
	ELvideoCapFilmTitleTail *pFilmTitleTailConf
	);

/*
* �Ƴ���ƵԴ
* hVideoCap: in, ¼��ʵ�����
* hCamera: in, ����ͷ�豸���
*/
ELAPI(int) elVideoCapRemoveVideoSrc(
	ELVideoCap hVideoCap,
	ELdevice dev
	);

/*
* �����ƵԴ
* hVideoCap: in, ¼��ʵ�����
* dev: in, ����ͷ�豸���
* dstX/dstY/dstWidth/dstHeight: in, ��ƵԴλ��������Ƶͼ���λ��
* alpha: in, ͸��������(0~255)
*/
ELAPI(int) elVideoCapAddVideoSrc(
	ELVideoCap hVideoCap, 
	ELdevice dev, 
	int dstX, 
	int dstY,
	int dstWidth, 
	int dstHeight,
	int bProportion,
	int alpha=255
	);

/*
* ��ƵԴͼ����ת
* hVideoCap: in, ¼��ʵ�����
* hCamera: in, ����ͷ�豸���
* angle: in, ��ת�Ƕ�, ��Ϊ90��������
*/
ELAPI(void) elVideoCapVideoSrcRotate(
	ELVideoCap hVideoCap, 
	ELdevice dev,
	int angle
	);

/*
* ���������ƵԴ
* hVideoCap: in, ¼��ʵ�����
*/
ELAPI(int) elVideoCapRemoveAllVideoSrc(
	ELVideoCap hVideoCap
	);

/*
* ��ȡϵͳ��Ƶ�豸����
*/
ELAPI(int) elVideoCapGetAudioDevNum();

/*
* ��ȡϵͳ��Ƶ�豸����
* devIndex: in, ��Ƶ�豸����
* pDevName: in, ��Ƶ�豸����
* bufSize: in, pDevName�����С
*/
ELAPI(int) elVideoCapGetAudioDevName(
	int devIndex,
	char *pDevName,
	int bufSize);

/*
* ���WIFI��ƵԴ
* hVideoCap: in, ¼��ʵ�����
* dstX/dstY/dstWidth/dstHeight: in, ��ƵԴλ��������Ƶͼ���λ��
* alpha: in, ͸��������(0~255)
*/
ELAPI(int) elVideoCapAddVideoSrcWIFI(
	ELVideoCap hVideoCap,
	int dstX,
	int dstY,
	int dstWidth,
	int dstHeight,
	int bProportion,
	int alpha=255
	);

/*
* д��һ֡WIFI����ͷͼ������
* hVideoCap: in, ¼��ʵ�����
* pFrameData: in, ͼ�����ݻ���
* frameSize: in, ֡���ݴ�С
* lineBytes: in, ͼ��ÿ���ֽ���
* bytesPerPixel: in, ÿ���ֽ���
*/
ELAPI(int) elVideoCapWriteVideoFrameWIFI(
	ELVideoCap hVideoCap, 
	UINT8 *pFrameData,
	int frameSize,
	int lineBytes,
	int bytesPerPixel
	);

/*
* �������
* hVideoCap: in, ¼��ʵ��
* hCamera: in, ����ͷ���
* lineSize: in, �߿�(����)
* color: in, �ߵ���ɫ
* startPnt: in, ���
*/
ELAPI(void) elVideoCap_AddCurve(
	ELVideoCap hVideoCap,
	ELdevice dev,
	int lineSize,
	COLORREF color,
	POINT startPnt
	);

/*
* ������ߵ�
* hVideoCap: in, ¼��ʵ��
* hCamera: in, ����ͷ���
* pnt: in, �����
*/
ELAPI(void) elVideoCap_AddCurvePnt(
	ELVideoCap hVideoCap, 
	ELdevice dev, 
	POINT pnt
	);

/*
* ��Ӿ���
* hVideoCap: in, ¼��ʵ��
* hCamera: in, ����ͷ���
* lineSize: in, �߿�(����)
* color: in, �ߵ���ɫ
* startPnt: in, ���
*/
ELAPI(void) elVideoCap_AddRect(
	ELVideoCap hVideoCap,
	ELdevice dev,
	int lineSize,
	COLORREF color,
	POINT startPnt
	);

/*
* ���þ��ε�
* hVideoCap: in, ¼��ʵ��
* hCamera: in, ����ͷ���
* pnt: in, ���νǵ�
*/
ELAPI(void) elVideoCap_SetRectPnt(
	ELVideoCap hVideoCap,
	ELdevice dev,
	POINT pnt
	);

/*
* �����Բ
* hVideoCap: in, ¼��ʵ��
* hCamera: in, ����ͷ���
* lineSize: in, �߿�(����)
* color: in, �ߵ���ɫ
* startPnt: in, ���
*/
ELAPI(void) elVideoCap_AddEllipse(
	ELVideoCap hVideoCap,
	ELdevice dev,
	int lineSize,
	COLORREF color,
	POINT startPnt
	);

/*
* ������Բ��
* hVideoCap: in, ¼��ʵ��
* hCamera: in, ����ͷ���
* pnt: in, ��Բ�ǵ�
*/
ELAPI(void) elVideoCap_SetEllipsePnt(
	ELVideoCap hVideoCap,
	ELdevice dev,
	POINT pnt
	);

/*
* ɾ�����һ����ע
* hVideoCap: in, ¼��ʵ��
*/
ELAPI(void) elVideoCap_Annotate_delLastShape(
	ELVideoCap hVideoCap
	);

/*
* ��ע����
* hVideoCap: in, ¼��ʵ��
*/
ELAPI(void) elVideoCap_Annotate_Undo(
	ELVideoCap hVideoCap
	);

/*
* ��ע����
* hVideoCap: in, ¼��ʵ��
*/
ELAPI(void) elVideoCap_Annotate_Redo(
	ELVideoCap hVideoCap
	);

/*
* ������б�ע
* hVideoCap: in, ¼��ʵ��
*/
ELAPI(void) elVideoCap_Annotate_Clear(
	ELVideoCap hVideoCap
	);
/********** �°�¼��¼�� end***********/


/********** ����ʶ��ȶ� end***********/
ELAPI(int)
	elInitFaceDetect(
	void
	);

ELAPI(int)
	elDeinitFaceDetect(
	void
	);

ELAPI(int)
	elGetFaceRect(
	ELimage image,
	ELrect * rect
	);

ELAPI(int)
	elDiscernFaceDetect(
	ELimage image1,
	ELimage image2
	);
/*****
�����ж��Ƿ��ǻ���
����ֵ 1���� 0 ��Ƭ -1ʧ��
���� color_image ������ɫͼƬ
	black_image  ����ڰ�ͼƬ
	nThreshold   �б�����ֵ  �Ƽ�30
	dwReserved  ����
******/
ELAPI(int)
	DetectFaceLive(
	ELimage color_image,
	ELimage black_image,
	int nThreshold,
	DWORD  dwReserved
	);

ELAPI(int)
	elFaceDetectIsVaild(
	void
	);
/********** ����ʶ��ȶ� end***********/

/*
* ͼ���Զ��׵ף�Ҫ�����㱳��Ϊ��ɫ���������������ɫ����
* flag ����
* threshold �ǰ׵���ж���ֵ��С�ڴ�ֵ�ж�Ϊ�ǰ׵㣬��Чֵȡֵ��ΧΪ0~255����-1���㷨�Զ����㣬����ֵ-1
* autoThresholdRatio thresholdΪ-1ʱ���˲�����Ч��Ϊ�Զ����������ֵ���µ�������ȡֵ��ΧΪ0~1������ֵ0.85
* aroundNum �ǰ׵��ж�ʱ�������С������ȡֵ��Χ2~8������ֵ5
* lowestBrightness ͼ��������������ֵ�������ڴ�ֵ���㷨���Զ���������ֵ��ȡֵ��Χ0~255������ֵ180
*/
ELAPI(int)
	elWhitenImage(
	ELimage image,
	int flag,
	int threshold,
	float autoThresholdRatio,
	int aroundNum,
	int lowestBrightness
	);

/*******************��Ʊʶ��ӿ�********************************/
ELAPI(int)
	elInitInvoice();

ELAPI(int)
	elDeinitInvoice();

ELAPI(int)
	elInvoiceGetRect(
	ELimage image, 
	int index, 
	int *x, 
	int *y, 
	int *w, 
	int *h);

ELAPI(int)
	elInvoiceDiscern(
	ELimage img, 
	int index, 
	int x, 
	int y, 
	int w, 
	int h);

ELAPI(int)
	elInvoiceGetData(
	int index,  
	wchar_t *data);

/*���հ�������*/
ELAPI(int)
	elStartXuTool(
	xutoolCallback funCallBack EL_DEFAULT(NULL),
	void *param EL_DEFAULT(NULL)
	);

ELAPI(int)
	elStopXuTool(
	void
	);

#ifdef __cplusplus
}

	
#endif /* __cplusplus */

#endif /* __EL_H__ */