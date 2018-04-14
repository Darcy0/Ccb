#pragma once

#include <afx.h>

#define CCB_CAMERADEV_API  __declspec(dllexport) 
#define STDCALL _stdcall

extern "C" CCB_CAMERADEV_API char *GetCameraCode();

extern "C" CCB_CAMERADEV_API int OpenCamera();

extern "C" CCB_CAMERADEV_API int GetCameraRatio(int *w, int *h);

extern "C" CCB_CAMERADEV_API int GetCameraFrame(BYTE* pBuf_Color,BYTE* pBuf_BW);

extern "C" CCB_CAMERADEV_API void CameraClose();

extern "C" CCB_CAMERADEV_API void ExitCamera(); //退出应用是调用，析构应用对象

extern "C" CCB_CAMERADEV_API void GetCameraResolutions(char *eyed,CString &strRes); //取得指定颜色相机的分辨率的结果集,char *eyes输入参数 值"black","color",CString strRes IN OUT

extern "C" CCB_CAMERADEV_API void SetCameraResoltuions(char *eyed,int iWidth,int iHeight,CString fmt);//设置指定颜色摄像机的分辨率及数据格式。char *eyed,输入参数 取值"black","color",其余是输入参数

extern "C" CCB_CAMERADEV_API int GetCameraRatio_1(char *eyed,int *w, int *h);  //取得指定摄像机的分辨率。 char *eyes输入参数，值"black","color", int *w,int *h输入参数宽，高

extern "C" CCB_CAMERADEV_API void store_res(int w,int h); //保存修改的分辨率

extern "C" CCB_CAMERADEV_API int GetRandom(char* command, int clen,char **rdata, int *rlen);

extern "C" /*CCB_CAMERADEV_API*/ int __stdcall GetMessageEx(char* command, int clen,char **rdata, int *rlen);

extern "C" CCB_CAMERADEV_API int WriteKey(char* command, int clen,char **rdata, int *rlen);

extern "C" CCB_CAMERADEV_API int InternalAuthenticate(char* command, int clen,char **rdata, int *rlen);

extern "C" CCB_CAMERADEV_API int ExternalAuthenticate(char* command, int clen,char **rdata, int *rlen);

extern "C" CCB_CAMERADEV_API int ImageSignature(char* command, int clen,char **rdata, int *rlen);

extern "C" CCB_CAMERADEV_API int ImageEncrypt(char* command, int clen,char **rdata, int *rlen);