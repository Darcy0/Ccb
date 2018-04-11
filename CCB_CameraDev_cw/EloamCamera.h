#include<vector>
#include<map>
#include<time.h>
#include "SAStatusLog.h"
#include ".\\Eloam\\include\\el.h"

#pragma  comment(lib,".\\Eloam\\lib\\el.lib")

using namespace std;

class EloamCamera{
public:
	EloamCamera();
	~EloamCamera();
	static void  devCallback(int type, int idx, int dbt, void *param);
	static void  viewCallback1(ELdevice dev, ELimage image, int frame, void *param);
	static void   viewCallback2(ELdevice dev, ELimage image, int frame, void *param);
	char* _stdcall GetCameraCode();
	int _stdcall OpenCamera();
	int _stdcall GetCameraRatio(int* w,int* h);
	int _stdcall GetCameraFrame(BYTE* pBuf_Color,BYTE* pBuf_BW);
	void _stdcall CameraClose();
private:
	vector<ELdevice> m_hDevList;
	BYTE* pBuf_Color;
	BYTE* pBuf_BW;
	ELimage m_hImage1;
	ELimage m_hImage2;
	int devSub1;
	int devSub2;
	CRITICAL_SECTION cs;
	BOOL  m_hScan1;
	BOOL  m_hScan2;
	static BOOL m_hFirstOpen;
	HANDLE m_EventTask1;
	HANDLE m_EventTask2;
};