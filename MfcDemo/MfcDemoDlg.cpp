
// MfcDemoDlg.cpp : 实现文件
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

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CMfcDemoDlg 对话框

CMfcDemoDlg::CMfcDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMfcDemoDlg::IDD, pParent)
{
	m_dllHandle=NULL;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_dllHandle=LoadLibrary(_T("CCB_CameraDev_cw.dll"));
	if (!m_dllHandle)
	{
		MessageBox(_T("提示"),_T("动态库CCB_CameraDev_cw.dll加载失败!"));
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


// CMfcDemoDlg 消息处理程序

BOOL CMfcDemoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMfcDemoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMfcDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMfcDemoDlg::OnBnClickedGetrandom()
{
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("Start..."));
	if (NULL==m_dllHandle)
	{
		LogOut.StatusOut(Info,_T("(%s) %s %s\r\n"),__FUNCTION__,_T("CCB_CameraDev_cw.dll动态库加载失败"),_T("End."));
		return;
	}
	pFunGetRandom fun=(pFunGetRandom)GetProcAddress(m_dllHandle,_T("GetRandom"));
	if (NULL==fun)
	{
		LogOut.StatusOut(Info,_T("(%s) 函数%s加载失败 %s\r\n"),__FUNCTION__,_T("GetRandom"),_T("End."));
		return;
	}
	CString cmdText,clenText;
	m_commandText.GetWindowTextA(cmdText);
	m_clenText.GetWindowTextA(clenText);
	char rdata[100]={0};
	int rlen=0;
	int iRet=fun(cmdText.GetBuffer(0),atoi(clenText),rdata,&rlen);
	LogOut.StatusOut(Info,_T("(%s)  输入:command:%s,clen:%d,输出:rdata:%s,rlen:%d,返回值:%d\r\n"),
		__FUNCTION__,cmdText,atoi(clenText),rdata,rlen,iRet);
	MessageBox(_T("GetRandom已执行,执行结果已写入文件!"),_T("提示"));
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("End."));
}

void CMfcDemoDlg::OnBnClickedGetmessageex()
{
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("Start..."));
	if (NULL==m_dllHandle)
	{
		LogOut.StatusOut(Info,_T("(%s) %s %s\r\n"),__FUNCTION__,_T("CCB_CameraDev_cw.dll动态库加载失败"),_T("End."));
		return;
	}
	pFunGetMessageEx fun=(pFunGetMessageEx)GetProcAddress(m_dllHandle,_T("GetMessageEx"));
	if (NULL==fun)
	{
		LogOut.StatusOut(Info,_T("(%s) 函数%s加载失败 %s\r\n"),__FUNCTION__,_T("GetMessageEx"),_T("End."));
		return;
	}
	CString cmdText,clenText;
	m_commandText.GetWindowTextA(cmdText);
	m_clenText.GetWindowTextA(clenText);
	char rdata[100]={0};
	int rlen=0;
	int iRet=fun(cmdText.GetBuffer(0),atoi(clenText),rdata,&rlen);
	LogOut.StatusOut(Info,_T("(%s)  输入:command:%s,clen:%d,输出:rdata:%s,rlen:%d,返回值:%d\r\n"),
		__FUNCTION__,cmdText,atoi(clenText),rdata,rlen,iRet);
	MessageBox(_T("GetMessageEx已执行,执行结果已写入文件!"),_T("提示"));	
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("End."));
}

void CMfcDemoDlg::OnBnClickedWritekey()
{
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("Start..."));
	if (NULL==m_dllHandle)
	{
		LogOut.StatusOut(Info,_T("(%s) %s %s\r\n"),__FUNCTION__,_T("CCB_CameraDev_cw.dll动态库加载失败"),_T("End."));
		return;
	}
	pFunWriteKey fun=(pFunWriteKey)GetProcAddress(m_dllHandle,_T("WriteKey"));
	if (NULL==fun)
	{
		LogOut.StatusOut(Info,_T("(%s) 函数%s加载失败 %s\r\n"),__FUNCTION__,_T("WriteKey"),_T("End."));
		return;
	}
	CString cmdText,clenText;
	m_commandText.GetWindowTextA(cmdText);
	m_clenText.GetWindowTextA(clenText);
	char rdata[100]={0};
	int rlen=0;
	int iRet=fun(cmdText.GetBuffer(0),atoi(clenText),rdata,&rlen);
	LogOut.StatusOut(Info,_T("(%s)  输入:command:%s,clen:%d,输出:rdata:%s,rlen:%d,返回值:%d\r\n"),
		__FUNCTION__,cmdText,atoi(clenText),rdata,rlen,iRet);
	MessageBox(_T("WriteKey已执行,执行结果已写入文件!"),_T("提示"));	
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("End."));
}

void CMfcDemoDlg::OnBnClickedInternalauthenticate()
{
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("Start..."));
	if (NULL==m_dllHandle)
	{
		LogOut.StatusOut(Info,_T("(%s) %s %s\r\n"),__FUNCTION__,_T("CCB_CameraDev_cw.dll动态库加载失败"),_T("End."));
		return;
	}
	pFunInternalAuthenticate fun=(pFunInternalAuthenticate)GetProcAddress(m_dllHandle,_T("InternalAuthenticate"));
	if (NULL==fun)
	{
		LogOut.StatusOut(Info,_T("(%s) 函数%s加载失败 %s\r\n"),__FUNCTION__,_T("InternalAuthenticate"),_T("End."));
		return;
	}
	CString cmdText,clenText;
	m_commandText.GetWindowTextA(cmdText);
	m_clenText.GetWindowTextA(clenText);
	char rdata[100]={0};
	int rlen=0;
	int iRet=fun(cmdText.GetBuffer(0),atoi(clenText),rdata,&rlen);
	LogOut.StatusOut(Info,_T("(%s)  输入:command:%s,clen:%d,输出:rdata:%s,rlen:%d,返回值:%d\r\n"),
		__FUNCTION__,cmdText,atoi(clenText),rdata,rlen,iRet);
	MessageBox(_T("InternalAuthenticate已执行,执行结果已写入文件!"),_T("提示"));	
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("End."));
}

void CMfcDemoDlg::OnBnClickedExternalauthenticate()
{
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("Start..."));
	if (NULL==m_dllHandle)
	{
		LogOut.StatusOut(Info,_T("(%s) %s %s\r\n"),__FUNCTION__,_T("CCB_CameraDev_cw.dll动态库加载失败"),_T("End."));
		return;
	}
	pFunExternalAuthenticate fun=(pFunExternalAuthenticate)GetProcAddress(m_dllHandle,_T("ExternalAuthenticate"));
	if (NULL==fun)
	{
		LogOut.StatusOut(Info,_T("(%s) 函数%s加载失败 %s\r\n"),__FUNCTION__,_T("ExternalAuthenticate"),_T("End."));
		return;
	}
	CString cmdText,clenText;
	m_commandText.GetWindowTextA(cmdText);
	m_clenText.GetWindowTextA(clenText);
	char rdata[100]={0};
	int rlen=0;
	int iRet=fun(cmdText.GetBuffer(0),atoi(clenText),rdata,&rlen);
	LogOut.StatusOut(Info,_T("(%s)  输入:command:%s,clen:%d,输出:rdata:%s,rlen:%d,返回值:%d\r\n"),
		__FUNCTION__,cmdText,atoi(clenText),rdata,rlen,iRet);
	MessageBox(_T("ExternalAuthenticate已执行,执行结果已写入文件!"),_T("提示"));	
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("End."));
}

void CMfcDemoDlg::OnBnClickedImagesignature()
{
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("Start..."));
	if (NULL==m_dllHandle)
	{
		LogOut.StatusOut(Info,_T("(%s) %s %s\r\n"),__FUNCTION__,_T("CCB_CameraDev_cw.dll动态库加载失败"),_T("End."));
		return;
	}
	pFunImageSignature fun=(pFunImageSignature)GetProcAddress(m_dllHandle,_T("ImageSignature"));
	if (NULL==fun)
	{
		LogOut.StatusOut(Info,_T("(%s) 函数%s加载失败 %s\r\n"),__FUNCTION__,_T("ImageSignature"),_T("End."));
		return;
	}
	CString cmdText,clenText;
	m_commandText.GetWindowTextA(cmdText);
	m_clenText.GetWindowTextA(clenText);
	char rdata[100]={0};
	int rlen=0;
	int iRet=fun(cmdText.GetBuffer(0),atoi(clenText),rdata,&rlen);
	LogOut.StatusOut(Info,_T("(%s)  输入:command:%s,clen:%d,输出:rdata:%s,rlen:%d,返回值:%d\r\n"),
		__FUNCTION__,cmdText,atoi(clenText),rdata,rlen,iRet);
	MessageBox(_T("ImageSignature已执行,执行结果已写入文件!"),_T("提示"));	
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("End."));
}

void CMfcDemoDlg::OnBnClickedImageencrypt()
{
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("Start..."));
	if (NULL==m_dllHandle)
	{
		LogOut.StatusOut(Info,_T("(%s) %s %s\r\n"),__FUNCTION__,_T("CCB_CameraDev_cw.dll动态库加载失败"),_T("End."));
		return;
	}
	pFunImageEncrypt fun=(pFunImageEncrypt)GetProcAddress(m_dllHandle,_T("ImageEncrypt"));
	if (NULL==fun)
	{
		LogOut.StatusOut(Info,_T("(%s) 函数%s加载失败 %s\r\n"),__FUNCTION__,_T("ImageEncrypt"),_T("End."));
		return;
	}
	CString cmdText,clenText;
	m_commandText.GetWindowTextA(cmdText);
	m_clenText.GetWindowTextA(clenText);
	char rdata[100]={0};
	int rlen=0;
	int iRet=fun(cmdText.GetBuffer(0),atoi(clenText),rdata,&rlen);
	LogOut.StatusOut(Info,_T("(%s)  输入:command:%s,clen:%d,输出:rdata:%s,rlen:%d,返回值:%d\r\n"),
		__FUNCTION__,cmdText,atoi(clenText),rdata,rlen,iRet);
	MessageBox(_T("ImageEncrypt已执行,执行结果已写入文件!"),_T("提示"));		
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("End."));
}

void CMfcDemoDlg::OnBnClickedGetcameracode()
{
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("Start..."));
	if (NULL==m_dllHandle)
	{
		LogOut.StatusOut(Info,_T("(%s) %s %s\r\n"),__FUNCTION__,_T("CCB_CameraDev_cw.dll动态库加载失败"),_T("End."));
		return;
	}
	pFunGetCameraCode fun=(pFunGetCameraCode)GetProcAddress(m_dllHandle,_T("GetCameraCode"));
	if (NULL==fun)
	{
		LogOut.StatusOut(Info,_T("(%s) 函数%s加载失败 %s\r\n"),__FUNCTION__,_T("GetCameraCode"),_T("End."));
		return;
	}
	char* sRet=fun();
	char text[100]={0};
	sprintf(text,"CameraCode:%s",sRet);
	MessageBox(text,_T("提示"));	
	LogOut.StatusOut(Info,_T("(%s) 摄像机编码:%s\r\n"),__FUNCTION__,sRet);
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("End."));
}

void CMfcDemoDlg::OnBnClickedOpencamera()
{
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("Start..."));
	if (NULL==m_dllHandle)
	{
		LogOut.StatusOut(Info,_T("(%s) %s %s\r\n"),__FUNCTION__,_T("CCB_CameraDev_cw.dll动态库加载失败"),_T("End."));
		return;
	}
	pFunOpenCamera fun=(pFunOpenCamera)GetProcAddress(m_dllHandle,_T("OpenCamera"));
	if (NULL==fun)
	{
		LogOut.StatusOut(Info,_T("(%s) 函数%s加载失败 %s\r\n"),__FUNCTION__,_T("GetRandom"),_T("End."));
		return;
	}
	int iRet=fun();
	if (iRet>=0)
	{
		MessageBox(_T("摄像头打开成功!"),_T("提示"));	
	}
	else
	{
		MessageBox(_T("摄像头打开失败!"),_T("提示"));	
	}
	LogOut.StatusOut(Info,_T("(%s) 返回值:%d\r\n"),__FUNCTION__,iRet);
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("End."));
}

void CMfcDemoDlg::OnBnClickedGetcameraratio()
{
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("Start..."));
	if (NULL==m_dllHandle)
	{
		LogOut.StatusOut(Info,_T("(%s) %s %s\r\n"),__FUNCTION__,_T("CCB_CameraDev_cw.dll动态库加载失败"),_T("End."));
		return;
	}
	pFunGetCameraRatio fun=(pFunGetCameraRatio)GetProcAddress(m_dllHandle,_T("GetCameraRatio"));
	if (NULL==fun)
	{
		LogOut.StatusOut(Info,_T("(%s) 函数%s加载失败 %s\r\n"),__FUNCTION__,_T("GetRandom"),_T("End."));
		return;
	}
	int w=0,h=0;
	int iRet=fun(&w,&h);
	char text[100];
	sprintf(text,"w:%d,h:%d",w,h);
	MessageBox(text,_T("提示"));	
	LogOut.StatusOut(Info,_T("(%s) 输出:rdata:%d,rlen:%d,返回值:%d\r\n"),__FUNCTION__,w,h,iRet);
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("End."));
}

void CMfcDemoDlg::OnBnClickedGetcameraframe()
{
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("Start..."));
	if (NULL==m_dllHandle)
	{
		LogOut.StatusOut(Info,_T("(%s) %s %s\r\n"),__FUNCTION__,_T("CCB_CameraDev_cw.dll动态库加载失败"),_T("End."));
		return;
	}
	pFunGetCameraFrame getCameraFrame=(pFunGetCameraFrame)GetProcAddress(m_dllHandle,_T("GetCameraFrame"));
	pFunGetCameraRatio getCameraRatio=(pFunGetCameraRatio)GetProcAddress(m_dllHandle,_T("GetCameraRatio"));
	if (NULL==getCameraFrame || NULL==getCameraRatio)
	{
		LogOut.StatusOut(Info,_T("(%s) 函数%s或%s加载失败 %s\r\n"),__FUNCTION__,_T("GetCameraFrame"),_T("GetCameraRatio"),_T("End."));
		return;
	}
	int width,height;
	getCameraRatio(&width,&height);
	LogOut.StatusOut(Info,_T("(%s) width:%d height:%d\r\n"),__FUNCTION__,width,height);
	ELsize size;
	size.width = width;
	size.height = height;
	BYTE* pBuf_Color =  (BYTE*)malloc(width*height*3*sizeof(BYTE));
	BYTE* pBuf_BW =(BYTE*)malloc(width*height*3*sizeof(BYTE));
	memset(pBuf_Color,0,width*height*3);
	memset(pBuf_BW,0,width*height*3);
	if (0 != getCameraFrame(pBuf_Color,pBuf_BW))
	{
		MessageBox(_T("图像帧获取失败"),_T("提示"));
		LogOut.StatusOut(Info,_T("(%s) %s %s\r\n"),__FUNCTION__,_T("图像帧获取失败"),_T("End."));
		return;
	}
	if(pBuf_Color[0] != 0)
	{
		ELimage image1 = elCreateImage(&size,3,pBuf_Color,width*3,EL_CREATE_TOP_LEFT);
		if (NULL==image1)
		{
			LogOut.StatusOut(Info,_T("(%s) %s\r\n"),__FUNCTION__,_T("创建图像1失败"));
		}
		else
		{
			LogOut.StatusOut(Info,_T("(%s) %s\r\n"),__FUNCTION__,_T("创建图像1成功"));
		}
		elSaveImage(image1,"1.jpg");
		elReleaseImage(image1);	
	}
	if(pBuf_BW[0] !=0)
	{

		ELimage image2 = elCreateImage(&size,3,pBuf_BW,width*3,EL_CREATE_TOP_LEFT);
		if (NULL==image2)
		{
			LogOut.StatusOut(Info,_T("(%s) %s\r\n"),__FUNCTION__,_T("创建图像2失败"));
		}
		else
		{
			LogOut.StatusOut(Info,_T("(%s) %s\r\n"),__FUNCTION__,_T("创建图像2成功"));
		}
		elSaveImage(image2,"2.jpg");
		elReleaseImage(image2);
	}
	free(pBuf_Color);
	free(pBuf_BW);
	pBuf_Color = NULL;
	pBuf_BW = NULL;
	MessageBox(_T("GetCameraFrame已执行"),_T("提示"));
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("End."));
}

void CMfcDemoDlg::OnBnClickedCameraclose()
{
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("Start..."));
	if (NULL==m_dllHandle)
	{
		LogOut.StatusOut(Info,_T("(%s) %s %s\r\n"),__FUNCTION__,_T("CCB_CameraDev_cw.dll动态库加载失败"),_T("End."));
		return;
	}
	pFunCameraClose fun=(pFunCameraClose)GetProcAddress(m_dllHandle,_T("CameraClose"));
	if (NULL==fun)
	{
		LogOut.StatusOut(Info,_T("(%s) 函数%s加载失败 %s\r\n"),__FUNCTION__,_T("GetRandom"),_T("End."));
		return;
	}
	fun();
	MessageBox(_T("CameraClose已执行"),_T("提示"));	
	LogOut.StatusOut(Info,_T("(%s)  %s\r\n"),__FUNCTION__,_T("End."));
}