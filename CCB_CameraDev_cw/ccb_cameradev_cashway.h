#pragma once

#include <afx.h>

#define CCB_CAMERADEV_API  __declspec(dllexport) 
#define STDCALL _stdcall

extern "C" CCB_CAMERADEV_API char *GetCameraCode();

extern "C" CCB_CAMERADEV_API int OpenCamera();

extern "C" CCB_CAMERADEV_API int GetCameraRatio(int *w, int *h);

extern "C" CCB_CAMERADEV_API int GetCameraFrame(BYTE* pBuf_Color,BYTE* pBuf_BW);

extern "C" CCB_CAMERADEV_API void CameraClose();

extern "C" CCB_CAMERADEV_API void ExitCamera(); //�˳�Ӧ���ǵ��ã�����Ӧ�ö���

extern "C" CCB_CAMERADEV_API void GetCameraResolutions(char *eyed,CString &strRes); //ȡ��ָ����ɫ����ķֱ��ʵĽ����,char *eyes������� ֵ"black","color",CString strRes IN OUT

extern "C" CCB_CAMERADEV_API void SetCameraResoltuions(char *eyed,int iWidth,int iHeight,CString fmt);//����ָ����ɫ������ķֱ��ʼ����ݸ�ʽ��char *eyed,������� ȡֵ"black","color",�������������

extern "C" CCB_CAMERADEV_API int GetCameraRatio_1(char *eyed,int *w, int *h);  //ȡ��ָ��������ķֱ��ʡ� char *eyes���������ֵ"black","color", int *w,int *h�����������

extern "C" CCB_CAMERADEV_API void store_res(int w,int h); //�����޸ĵķֱ���

extern "C" CCB_CAMERADEV_API int GetRandom(char* command, int clen,char **rdata, int *rlen);

extern "C" /*CCB_CAMERADEV_API*/ int __stdcall GetMessageEx(char* command, int clen,char **rdata, int *rlen);

extern "C" CCB_CAMERADEV_API int WriteKey(char* command, int clen,char **rdata, int *rlen);

extern "C" CCB_CAMERADEV_API int InternalAuthenticate(char* command, int clen,char **rdata, int *rlen);

extern "C" CCB_CAMERADEV_API int ExternalAuthenticate(char* command, int clen,char **rdata, int *rlen);

extern "C" CCB_CAMERADEV_API int ImageSignature(char* command, int clen,char **rdata, int *rlen);

extern "C" CCB_CAMERADEV_API int ImageEncrypt(char* command, int clen,char **rdata, int *rlen);