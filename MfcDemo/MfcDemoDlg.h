
// MfcDemoDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"

typedef char*(*pFunGetCameraCode)();  
typedef int(*pFunOpenCamera)();  
typedef int(*pFunGetCameraRatio)(int *w,int *h);  
typedef int(*pFunGetCameraFrame)(BYTE* pBuf_Color,BYTE* pBuf_BW);  
typedef void(*pFunCameraClose)();  
typedef void(*pFunExitCamera)();  
typedef void(*pFunGetCameraResolutions)(char *eyed,CString &strRes);  
typedef void(*pFunSetCameraResoltuions)(char *eyed,int iWidth,int iHeight,CString fmt);  
typedef int(*pFunGetCameraRatio_1)(char *eyed,int *w, int *h);  
typedef void(*pFunStore_res)(int w,int h);  
typedef int(*pFunGetRandom)(char* command, int clen,char **rdata, int *rlen); 
typedef int (*pFunGetMessageEx)(char* command, int clen,char **rdata, int *rlen); 
typedef int(*pFunWriteKey)(char* command, int clen,char **rdata, int *rlen); 
typedef int(*pFunInternalAuthenticate)(char* command, int clen,char **rdata, int *rlen); 
typedef int(*pFunExternalAuthenticate)(char* command, int clen,char **rdata, int *rlen); 
typedef int(*pFunImageSignature)(char* command, int clen,char **rdata, int *rlen); 
typedef int(*pFunImageEncrypt)(char* command, int clen,char **rdata, int *rlen); 

// CMfcDemoDlg �Ի���
class CMfcDemoDlg : public CDialogEx
{
// ����
public:
	CMfcDemoDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_MFCDEMO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedGetrandom();
	afx_msg void OnBnClickedGetmessageex();
	afx_msg void OnBnClickedWritekey();
	afx_msg void OnBnClickedInternalauthenticate();
	afx_msg void OnBnClickedExternalauthenticate();
	afx_msg void OnBnClickedImagesignature();
	afx_msg void OnBnClickedImageencrypt();

private:
	HINSTANCE m_dllHandle;

	void* getFunctionPointer(const char* funName);
	int getInputParam(char **cmdBuf,int* len);
public:
	CEdit m_commandText;
	CEdit m_clenText;
	afx_msg void OnBnClickedGetcameracode();
	afx_msg void OnBnClickedOpencamera();
	afx_msg void OnBnClickedGetcameraratio();
	afx_msg void OnBnClickedGetcameraframe();
	afx_msg void OnBnClickedCameraclose();
};
