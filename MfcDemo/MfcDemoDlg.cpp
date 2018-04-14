
// MfcDemoDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MfcDemo.h"
#include "MfcDemoDlg.h"
#include "afxdialogex.h"
#include "SAStatusLog.h"
#include ".\\Eloam\\include\\el.h"

#pragma  comment(lib,".\\Eloam\\lib\\el.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// hex to asc: 0x22 -> "22"
int Hex2Asc(char *Dest,char *Src,int SrcLen)
{
	int i;
	for ( i = 0; i < SrcLen; i ++ )
	{
		sprintf(Dest + i * 2,"%02X",(unsigned char)Src[i]);
	}
	Dest[i * 2] = 0;
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// asc to hex: "22" -> 0x22
int Asc2Hex(char *Dest,char *Src,int SrcLen)
{
	int i;
	for ( i = 0; i < SrcLen / 2; i ++ )
	{
		sscanf(Src + i * 2,"%02X",(unsigned char *)&Dest[i]);
	}
	return TRUE;
}
// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMfcDemoDlg �Ի���

CMfcDemoDlg::CMfcDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMfcDemoDlg::IDD, pParent)
{
	m_dllHandle=NULL;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_dllHandle=LoadLibrary(_T("CCB_CameraDev_CW.dll"));
	if (!m_dllHandle)
	{
		MessageBox(_T("��ʾ"),_T("��̬��CCB_CameraDev_CW.dll����ʧ��!"));
	}
}

void CMfcDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_commandText);
	DDX_Control(pDX, IDC_EDIT2, m_clenText);
}

BEGIN_MESSAGE_MAP(CMfcDemoDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(GetRandom, &CMfcDemoDlg::OnBnClickedGetrandom)
	ON_BN_CLICKED(GetMessageEx, &CMfcDemoDlg::OnBnClickedGetmessageex)
	ON_BN_CLICKED(WriteKey, &CMfcDemoDlg::OnBnClickedWritekey)
	ON_BN_CLICKED(InternalAuthenticate, &CMfcDemoDlg::OnBnClickedInternalauthenticate)
	ON_BN_CLICKED(ExternalAuthenticate, &CMfcDemoDlg::OnBnClickedExternalauthenticate)
	ON_BN_CLICKED(ImageSignature, &CMfcDemoDlg::OnBnClickedImagesignature)
	ON_BN_CLICKED(ImageEncrypt, &CMfcDemoDlg::OnBnClickedImageencrypt)
	ON_BN_CLICKED(GetCameraCode, &CMfcDemoDlg::OnBnClickedGetcameracode)
	ON_BN_CLICKED(OpenCamera, &CMfcDemoDlg::OnBnClickedOpencamera)
	ON_BN_CLICKED(GetCameraRatio, &CMfcDemoDlg::OnBnClickedGetcameraratio)
	ON_BN_CLICKED(GetCameraFrame, &CMfcDemoDlg::OnBnClickedGetcameraframe)
	ON_BN_CLICKED(CameraClose, &CMfcDemoDlg::OnBnClickedCameraclose)
END_MESSAGE_MAP()


// CMfcDemoDlg ��Ϣ�������

BOOL CMfcDemoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CMfcDemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CMfcDemoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CMfcDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMfcDemoDlg::OnBnClickedGetrandom()
{
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("Start..."));
	if (NULL==m_dllHandle)
	{
		LogOut.StatusOut(Info,_T("(%s) %s %s\r\n"),__FUNCTION__,_T("CCB_CameraDev_cw.dll��̬�����ʧ��"),_T("End."));
		return;
	}
	pFunGetRandom fun=(pFunGetRandom)GetProcAddress(m_dllHandle,_T("GetRandom"));
	if (NULL==fun)
	{
		LogOut.StatusOut(Info,_T("(%s) ����%s����ʧ�� %s\r\n"),__FUNCTION__,_T("GetRandom"),_T("End."));
		return;
	}	
	CString cmdText,clenText;
	m_commandText.GetWindowTextA(cmdText);
	m_clenText.GetWindowTextA(clenText);
	char *cCmdText=cmdText.GetBuffer(0);
	int clen=atoi(clenText);
	
	char *cmdBuf=new char[strlen(cCmdText)/2];
	memset(cmdBuf,0,strlen(cCmdText)/2);
	Asc2Hex(cmdBuf,cCmdText,strlen(cCmdText));
	
	char rdata[16];
	char *pRdata=rdata;
	int rlen=0;
	int iRet=fun(cmdBuf,clen,&pRdata,&rlen);
	
	char rdataText[100]={0};
	Hex2Asc(rdataText,rdata,rlen);	

	char buf[100]={0};
	sprintf(buf,"����:command:%s,clen:%d,���:rdata:%s,rlen:%d,����ֵ:0x%X",cmdText,clen,rdataText,rlen,iRet);
	
	LogOut.StatusOut(Info,_T("(%s)  ����:command:%s,clen:%d,���:rdata:%s,rlen:%d,����ֵ:0x%X\r\n"),
		__FUNCTION__,cmdText,clen,rdataText,rlen,iRet);
	MessageBox(buf,_T("��ʾ"));

	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("End."));
}

void CMfcDemoDlg::OnBnClickedGetmessageex()
{

	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("Start..."));

	pFunGetMessageEx pFun=(pFunGetMessageEx)getFunctionPointer(_T("GetMessageEx"));
	if (NULL==pFun)
	{
		LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("End."));
		return;
	}

	char *cmdBuf=NULL;
	int clen=0;
	if (0!=getInputParam(&cmdBuf,&clen))
	{//�в���
		/*LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("End."));
		return;*/
	}

	char rdata[256]={0};
	char *pRdata=rdata;
	int rlen=0;
	int iRet=pFun(cmdBuf,clen,&pRdata,&rlen);
	//free(cmdBuf);//�ͷ�cmdBufΪʲô���clenҲ�ͷ��ˡ�

	char buf[100];
	sprintf(buf,"���rdata:%s, rlen:%d,����ֵ:0x%X",rdata,rlen,iRet);

	//LogOut.StatusOut(Info,_T("(%s)  ���rdata:%s, rlen:%d,����ֵ:0x%X\r\n"),__FUNCTION__,rdata,rlen,iRet);
	MessageBox(buf,_T("��ʾ"));	

	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("End."));


}

void CMfcDemoDlg::OnBnClickedWritekey()
{
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("Start..."));
	if (NULL==m_dllHandle)
	{
		LogOut.StatusOut(Info,_T("(%s) %s %s\r\n"),__FUNCTION__,_T("CCB_CameraDev_cw.dll��̬�����ʧ��"),_T("End."));
		return;
	}
	pFunWriteKey fun=(pFunWriteKey)GetProcAddress(m_dllHandle,_T("WriteKey"));
	if (NULL==fun)
	{
		LogOut.StatusOut(Info,_T("(%s) ����%s����ʧ�� %s\r\n"),__FUNCTION__,_T("WriteKey"),_T("End."));
		return;
	}
	CString cmdText,clenText;
	m_commandText.GetWindowTextA(cmdText);
	m_clenText.GetWindowTextA(clenText);
	char *cCmdText=cmdText.GetBuffer(0);
	int clen=atoi(clenText);

	char *cmdBuf=new char[strlen(cCmdText)/2];
	memset(cmdBuf,0,strlen(cCmdText)/2);
	Asc2Hex(cmdBuf,cCmdText,strlen(cCmdText));

	char rdata[256];
	char *pRdata=rdata;
	int rlen=0;
	int iRet=fun(cmdBuf,clen,&pRdata,&rlen);

	char rdataText[100]={0};
	Hex2Asc(rdataText,rdata,rlen);	

	char buf[100]={0};
	sprintf(buf,"����:command:%s,clen:%d,���:rdata:%s,rlen:%d,����ֵ:0x%X",cmdText,clen,rdataText,rlen,iRet);

	LogOut.StatusOut(Info,_T("(%s)  ����:command:%s,clen:%d,���:rdata:%s,rlen:%d,����ֵ:0x%X\r\n"),
		__FUNCTION__,cmdText,clen,rdataText,rlen,iRet);
	MessageBox(buf,_T("��ʾ"));
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("End."));
}

void CMfcDemoDlg::OnBnClickedInternalauthenticate()
{
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("Start..."));
	if (NULL==m_dllHandle)
	{
		LogOut.StatusOut(Info,_T("(%s) %s %s\r\n"),__FUNCTION__,_T("CCB_CameraDev_cw.dll��̬�����ʧ��"),_T("End."));
		return;
	}
	pFunInternalAuthenticate fun=(pFunInternalAuthenticate)GetProcAddress(m_dllHandle,_T("InternalAuthenticate"));
	if (NULL==fun)
	{
		LogOut.StatusOut(Info,_T("(%s) ����%s����ʧ�� %s\r\n"),__FUNCTION__,_T("InternalAuthenticate"),_T("End."));
		return;
	}
	CString cmdText,clenText;
	m_commandText.GetWindowTextA(cmdText);
	m_clenText.GetWindowTextA(clenText);
	char *cCmdText=cmdText.GetBuffer(0);
	int clen=atoi(clenText);

	char *cmdBuf=new char[strlen(cCmdText)/2];
	memset(cmdBuf,0,strlen(cCmdText)/2);
	Asc2Hex(cmdBuf,cCmdText,strlen(cCmdText));

	char rdata[256];
	char *pRdata=rdata;
	int rlen=0;
	int iRet=fun(cmdBuf,clen,&pRdata,&rlen);

	char rdataText[100]={0};
	Hex2Asc(rdataText,rdata,rlen);	

	char buf[100]={0};
	sprintf(buf,"����:command:%s,clen:%d,���:rdata:%s,rlen:%d,����ֵ:0x%X",cmdText,clen,rdataText,rlen,iRet);

	LogOut.StatusOut(Info,_T("(%s)  ����:command:%s,clen:%d,���:rdata:%s,rlen:%d,����ֵ:0x%X\r\n"),
		__FUNCTION__,cmdText,clen,rdataText,rlen,iRet);
	MessageBox(buf,_T("��ʾ"));
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("End."));
}

void CMfcDemoDlg::OnBnClickedExternalauthenticate()
{
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("Start..."));
	if (NULL==m_dllHandle)
	{
		LogOut.StatusOut(Info,_T("(%s) %s %s\r\n"),__FUNCTION__,_T("CCB_CameraDev_cw.dll��̬�����ʧ��"),_T("End."));
		return;
	}
	pFunExternalAuthenticate fun=(pFunExternalAuthenticate)GetProcAddress(m_dllHandle,_T("ExternalAuthenticate"));
	if (NULL==fun)
	{
		LogOut.StatusOut(Info,_T("(%s) ����%s����ʧ�� %s\r\n"),__FUNCTION__,_T("ExternalAuthenticate"),_T("End."));
		return;
	}

	pFunGetRandom funGetRandom=(pFunGetRandom)GetProcAddress(m_dllHandle,_T("GetRandom"));
	if (NULL==funGetRandom)
	{
		LogOut.StatusOut(Info,_T("(%s) ����%s����ʧ�� %s\r\n"),__FUNCTION__,_T("GetRandom"),_T("End."));
		return;
	}	
	CString cmdText,clenText;
	m_commandText.GetWindowTextA(cmdText);
	m_clenText.GetWindowTextA(clenText);
	char *cCmdText=cmdText.GetBuffer(0);
	int clen=atoi(clenText);

	
	char *cmdBuf=new char[strlen(cCmdText)/2];
	memset(cmdBuf,0,strlen(cCmdText)/2);
	Asc2Hex(cmdBuf,cCmdText,strlen(cCmdText));

	char rdata[256];
	char *pRdata=rdata;
	int rlen=0;
	int iRet=fun(cmdBuf,clen,&pRdata,&rlen);

	char rdataText[100]={0};
	Hex2Asc(rdataText,rdata,rlen);	

	char buf[100]={0};
	sprintf(buf,"����:command:%s,clen:%d,���:rdata:%s,rlen:%d,����ֵ:0x%X",cmdText,clen,rdataText,rlen,iRet);

	LogOut.StatusOut(Info,_T("(%s)  ����:command:%s,clen:%d,���:rdata:%s,rlen:%d,����ֵ:0x%X\r\n"),
		__FUNCTION__,cmdText,clen,rdataText,rlen,iRet);
	MessageBox(buf,_T("��ʾ"));
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("End."));
}

void CMfcDemoDlg::OnBnClickedImagesignature()
{
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("Start..."));
	if (NULL==m_dllHandle)
	{
		LogOut.StatusOut(Info,_T("(%s) %s %s\r\n"),__FUNCTION__,_T("CCB_CameraDev_cw.dll��̬�����ʧ��"),_T("End."));
		return;
	}
	pFunImageSignature fun=(pFunImageSignature)GetProcAddress(m_dllHandle,_T("ImageSignature"));
	if (NULL==fun)
	{
		LogOut.StatusOut(Info,_T("(%s) ����%s����ʧ�� %s\r\n"),__FUNCTION__,_T("ImageSignature"),_T("End."));
		return;
	}
	CString cmdText,clenText;
	m_commandText.GetWindowTextA(cmdText);
	m_clenText.GetWindowTextA(clenText);
	char *cCmdText=cmdText.GetBuffer(0);
	int clen=atoi(clenText);

	char *cmdBuf=new char[strlen(cCmdText)/2];
	memset(cmdBuf,0,strlen(cCmdText)/2);
	Asc2Hex(cmdBuf,cCmdText,strlen(cCmdText));

	char rdata[256];
	char *pRdata=rdata;
	int rlen=0;
	int iRet=fun(cmdBuf,clen,&pRdata,&rlen);

	char rdataText[100]={0};
	Hex2Asc(rdataText,rdata,rlen);	

	char buf[100]={0};
	sprintf(buf,"����:command:%s,clen:%d,���:rdata:%s,rlen:%d,����ֵ:0x%X",cmdText,clen,rdataText,rlen,iRet);

	LogOut.StatusOut(Info,_T("(%s)  ����:command:%s,clen:%d,���:rdata:%s,rlen:%d,����ֵ:0x%X\r\n"),
		__FUNCTION__,cmdText,clen,rdataText,rlen,iRet);
	MessageBox(buf,_T("��ʾ"));
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("End."));
}

void CMfcDemoDlg::OnBnClickedImageencrypt()
{
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("Start..."));
	if (NULL==m_dllHandle)
	{
		LogOut.StatusOut(Info,_T("(%s) %s %s\r\n"),__FUNCTION__,_T("CCB_CameraDev_cw.dll��̬�����ʧ��"),_T("End."));
		return;
	}
	pFunImageEncrypt fun=(pFunImageEncrypt)GetProcAddress(m_dllHandle,_T("ImageEncrypt"));
	if (NULL==fun)
	{
		LogOut.StatusOut(Info,_T("(%s) ����%s����ʧ�� %s\r\n"),__FUNCTION__,_T("ImageEncrypt"),_T("End."));
		return;
	}
	CString cmdText,clenText;
	m_commandText.GetWindowTextA(cmdText);
	m_clenText.GetWindowTextA(clenText);
	char *cCmdText=cmdText.GetBuffer(0);
	int clen=atoi(clenText);

	char *cmdBuf=new char[strlen(cCmdText)/2];
	memset(cmdBuf,0,strlen(cCmdText)/2);
	Asc2Hex(cmdBuf,cCmdText,strlen(cCmdText));

	char rdata[256];
	char *pRdata=rdata;
	int rlen=0;
	int iRet=fun(cmdBuf,clen,&pRdata,&rlen);

	char rdataText[100]={0};
	Hex2Asc(rdataText,rdata,rlen);	

	char buf[100]={0};
	sprintf(buf,"����:command:%s,clen:%d,���:rdata:%s,rlen:%d,����ֵ:0x%X",cmdText,clen,rdataText,rlen,iRet);

	LogOut.StatusOut(Info,_T("(%s)  ����:command:%s,clen:%d,���:rdata:%s,rlen:%d,����ֵ:0x%X\r\n"),
		__FUNCTION__,cmdText,clen,rdataText,rlen,iRet);
	MessageBox(buf,_T("��ʾ"));
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("End."));
}

void CMfcDemoDlg::OnBnClickedGetcameracode()
{
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("Start..."));
	pFunGetCameraCode pFun= (pFunGetCameraCode)getFunctionPointer(_T("GetCameraCode"));
	if (NULL!=pFun)
	{
		char* sRet=pFun();
		
		char text[100]={0};
		sprintf(text,"CameraCode:%s",sRet);
		MessageBox(text,_T("��ʾ"));	
		
		LogOut.StatusOut(Info,_T("(%s) ���������:%s\r\n"),__FUNCTION__,sRet);
	}
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("End."));
}

void CMfcDemoDlg::OnBnClickedOpencamera()
{
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("Start..."));
	pFunOpenCamera pFun=(pFunOpenCamera)getFunctionPointer(_T("OpenCamera"));
	if (NULL!=pFun)
	{
		int iRet=pFun();
		if (iRet>=0)
		{
			MessageBox(_T("����ͷ�򿪳ɹ�!"),_T("��ʾ"));	
		}
		else
		{
			MessageBox(_T("����ͷ��ʧ��!"),_T("��ʾ"));	
		}
		LogOut.StatusOut(Info,_T("(%s) ����ֵ:%d\r\n"),__FUNCTION__,iRet);
	}
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("End."));
}

void CMfcDemoDlg::OnBnClickedGetcameraratio()
{
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("Start..."));
	pFunGetCameraRatio pFun=(pFunGetCameraRatio)getFunctionPointer(_T("GetCameraRatio"));
	if (NULL!=pFun)
	{
		int w=0,h=0;
		int iRet=pFun(&w,&h);

		char text[100];
		sprintf(text,"w:%d,h:%d",w,h);
		MessageBox(text,_T("��ʾ"));	
		
		LogOut.StatusOut(Info,_T("(%s) ���:width:%d,height:%d,����ֵ:%d\r\n"),__FUNCTION__,w,h,iRet);
	}
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("End."));
}

void CMfcDemoDlg::OnBnClickedGetcameraframe()
{
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("Start..."));
	
	pFunGetCameraRatio getCameraRatio=(pFunGetCameraRatio)getFunctionPointer(_T("GetCameraRatio"));
	pFunGetCameraFrame getCameraFrame=(pFunGetCameraFrame)getFunctionPointer(_T("GetCameraFrame"));	
	if (NULL==getCameraRatio||NULL==getCameraFrame)
	{
		LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("End."));
		return;
	}

	int width,height;//��ȡͼ��ߴ�
	getCameraRatio(&width,&height);
	LogOut.StatusOut(Info,_T("(%s) width:%d height:%d\r\n"),__FUNCTION__,width,height);
			
	BYTE* pBuf_Color =  (BYTE*)malloc(width*height*3*sizeof(BYTE));
	BYTE* pBuf_BW =(BYTE*)malloc(width*height*3*sizeof(BYTE));
	memset(pBuf_Color,0,width*height*3);
	memset(pBuf_BW,0,width*height*3);
	
	if (0 != getCameraFrame(pBuf_Color,pBuf_BW))
	{
		MessageBox(_T("ͼ��֡��ȡʧ��"),_T("��ʾ"));
		LogOut.StatusOut(Info,_T("(%s) %s %s\r\n"),__FUNCTION__,_T("ͼ��֡��ȡʧ��"),_T("End."));
		return;
	}

	ELsize size;
	size.width = width;
	size.height = height;
	if(pBuf_Color[0] != 0)
	{
		ELimage image1 = elCreateImage(&size,3,pBuf_Color,width*3,EL_CREATE_TOP_LEFT);
		if (NULL==image1)
		{
			LogOut.StatusOut(Info,_T("(%s) %s\r\n"),__FUNCTION__,_T("����ͼ��1ʧ��"));
		}
		else
		{
			LogOut.StatusOut(Info,_T("(%s) %s\r\n"),__FUNCTION__,_T("����ͼ��1�ɹ�"));
		}
		elSaveImage(image1,"1.jpg");
		elReleaseImage(image1);	
	}
	if(pBuf_BW[0] !=0)
	{
		ELimage image2 = elCreateImage(&size,3,pBuf_BW,width*3,EL_CREATE_TOP_LEFT);
		if (NULL==image2)
		{
			LogOut.StatusOut(Info,_T("(%s) %s\r\n"),__FUNCTION__,_T("����ͼ��2ʧ��"));
		}
		else
		{
			LogOut.StatusOut(Info,_T("(%s) %s\r\n"),__FUNCTION__,_T("����ͼ��2�ɹ�"));
		}
		elSaveImage(image2,"2.jpg");
		elReleaseImage(image2);
	}
	free(pBuf_Color);
	free(pBuf_BW);
	pBuf_Color = NULL;
	pBuf_BW = NULL;
	MessageBox(_T("GetCameraFrame��ִ��"),_T("��ʾ"));
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("End."));
}

void CMfcDemoDlg::OnBnClickedCameraclose()
{
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("Start..."));
	pFunCameraClose pFun=(pFunCameraClose)getFunctionPointer(_T("CameraClose"));
	if (NULL!=pFun)
	{
		pFun();
		MessageBox(_T("CameraClose��ִ��"),_T("��ʾ"));	
	}	
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("End."));
}

void * CMfcDemoDlg::getFunctionPointer( const char* funName )
{
	LogOut.StatusOut(Info,_T("(%s) %s\r\n"),__FUNCTION__,_T("Start..."));
	if (NULL==funName)
	{
		LogOut.StatusOut(Error,_T("(%s) %s %s\r\n"),__FUNCTION__,_T("���뺯����Ϊ��."),_T("End."));
		return NULL;
	}
	if (NULL==m_dllHandle)
	{
		LogOut.StatusOut(Error,_T("(%s) %s %s\r\n"),__FUNCTION__,_T("CCB_CameraDev_CW.dll��̬�����ʧ��"),_T("End."));
		return NULL;
	}
	void* pFun=(void *)GetProcAddress(m_dllHandle,funName);
	if (NULL==pFun)
	{//����ָ���ȡʧ��
		LogOut.StatusOut(Error,_T("(%s) ����%s����ʧ��\r\n"),__FUNCTION__,funName);
	}
	else
	{//����ָ���ȡ�ɹ�
		LogOut.StatusOut(Info,_T("(%s) ����%s���سɹ�\r\n"),__FUNCTION__,funName);
	}
	LogOut.StatusOut(Info,_T("(%s) %s\r\n"),__FUNCTION__,_T("End."));
	return pFun;
}

int CMfcDemoDlg::getInputParam( char **cmdBuf,int* len )
{
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("Start..."));
	CString cmdText,clenText;
	
	m_commandText.GetWindowTextA(cmdText);
	m_clenText.GetWindowTextA(clenText);
	if (cmdText.IsEmpty()||clenText.IsEmpty())
	{
		LogOut.StatusOut(Error,_T("(%s) %s %s\r\n"),__FUNCTION__,_T("û���������"),_T("End."));
		return -1;
	}

	char *cCmdText=cmdText.GetBuffer(0);
	*cmdBuf=(char *)malloc(strlen(cCmdText)/2);

	memset(*cmdBuf,0,strlen(cCmdText)/2);
	Asc2Hex(*cmdBuf,cCmdText,strlen(cCmdText));

	*len=atoi(clenText);

	LogOut.StatusOut(Info,_T("(%s) command:%s,clen:%d\r\n"),__FUNCTION__,cCmdText,*len);
	LogOut.StatusOut(Info,_T("(%s) %s\r\n"),__FUNCTION__,_T("End."));
	return 0;
}