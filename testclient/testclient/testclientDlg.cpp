
// testclientDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "testclient.h"
#include "testclientDlg.h"
#include "afxdialogex.h"

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


// CtestclientDlg 对话框



CtestclientDlg::CtestclientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CtestclientDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_GameProcess = NULL;
}

void CtestclientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CtestclientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CtestclientDlg::OnBnClickedButton1)
	ON_WM_DESTROY()
	ON_WM_CLOSE()
END_MESSAGE_MAP()

// CtestclientDlg 消息处理程序

BOOL CtestclientDlg::OnInitDialog()
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

	//网络初始化
	this->m_udp.InitialUDP();
	//模拟登陆
	this->m_udp.SendToUserLoginInfo("hjs","123456");

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CtestclientDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CtestclientDlg::OnPaint()
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
HCURSOR CtestclientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CtestclientDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	//启动参数
	CHAR szRunParamete[16] = {0};
	//加载的dll路径
	CHAR szDllPath[MAX_PATH] = {0};
	//本地游戏路径
	CHAR szLocalGamePath[MAX_PATH] = {0};
	BOOL bIsSuccess = FALSE;
	STARTUPINFO sInfo;
	PROCESS_INFORMATION pInfo;
	DWORD dwServiceIp = 0;
	ZeroMemory(&sInfo,sizeof(sInfo));
	ZeroMemory(&pInfo,sizeof(pInfo));

	sInfo.cb=sizeof(sInfo);

	//获取注入的dll路径
	strcpy_s(szDllPath,MAX_PATH,"E:\\flym\\war3\\testclient\\Debug\\HookNetwork.dll");
	//获取游戏路径
	strcpy_s(szLocalGamePath,MAX_PATH,"F:\\魔兽3-冰封王座\\war3.exe");

	strcpy_s(szRunParamete,16,"tango -window");

	//创建游戏进程，并处于挂起状态
	if(CreateProcess(szLocalGamePath,szRunParamete,NULL,NULL,FALSE,CREATE_SUSPENDED,NULL,NULL,&sInfo,&pInfo))
	{
		//远程线程注入模块
		if(RemoteThreadInjectModu(szDllPath,pInfo.hProcess))
		{
			TRACE("Initialize Game Success!\r\n");
			bIsSuccess = TRUE;
			//保存进程对象句柄
			m_GameProcess = pInfo.hProcess;
			//恢复游戏主线程运行
			ResumeThread(pInfo.hThread);
			//关闭主线程的线程句柄
			CloseHandle(pInfo.hThread);
		}
		else
		{
			TerminateProcess(pInfo.hProcess,0);
			CloseHandle(pInfo.hThread);
			CloseHandle(pInfo.hProcess);
		}
	}
	else
	{
		TRACE("Create Game Process Failed!\r\n");
	}
}

BOOL CtestclientDlg::RemoteThreadInjectModu(TCHAR * szInjectDllPath,HANDLE hProcess)
{
	ASSERT(hProcess != INVALID_HANDLE_VALUE);
	LPVOID		ParameterAddress = NULL;
	DWORD		dwWirite = 0;
	LPVOID		lpLibraryAddress = NULL;
	HANDLE		hRemote = INVALID_HANDLE_VALUE;
	BOOL		bIsSuccess = FALSE;

	//加上结构化异常处理，回收资源方便
	__try
	{
		//申请虚拟内存
		ParameterAddress = VirtualAllocEx(hProcess,NULL,MAX_PATH+1,MEM_COMMIT,PAGE_READWRITE);
		if (!ParameterAddress)
		{
			__leave;
		}
		
		//写入注入的dll路径
		if(!WriteProcessMemory(hProcess,ParameterAddress,szInjectDllPath,MAX_PATH+1,&dwWirite))
		{
			__leave;
		}
		
		//查找函数地址
		lpLibraryAddress = GetProcAddress(GetModuleHandle("Kernel32.dll"),"LoadLibraryA");
		if (!lpLibraryAddress)
		{
			__leave;
		}
		
		//创建远程线程注入
		hRemote = CreateRemoteThread(hProcess,NULL,0,(LPTHREAD_START_ROUTINE)lpLibraryAddress,ParameterAddress,0,NULL);
		if(!hRemote)
		{
			__leave;
		}
		//等待远程线程返回
		WaitForSingleObject(hRemote,INFINITE);
		bIsSuccess = TRUE;
	}
	__finally
	{
		//关闭远程线程句柄
		if(hRemote)
		{
			CloseHandle(hRemote);	
		}
		//释放内存
		VirtualFreeEx(hProcess,ParameterAddress,0,MEM_RELEASE);
	}
	return bIsSuccess;
}

void CtestclientDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	this->m_udp.SendToUserQuitInfo();
	Sleep(100);
	this->m_udp.UnInitial();
}

void CtestclientDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialogEx::OnClose();
}
