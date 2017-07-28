// testclientDlg.h : 头文件
//
#pragma once

#include "UDPNet.h"
#include "GlobalInformation.h"

// CtestclientDlg 对话框
class CtestclientDlg : public CDialogEx
{
// 构造
public:
	CtestclientDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_TESTCLIENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnDestroy();
//远程注入
public:
	BOOL RemoteThreadInjectModu(TCHAR * szInjectDllPath,HANDLE hProcess);
	HANDLE m_GameProcess;
//网络
public:
	CUDPNet m_udp;
	
	afx_msg void OnClose();
};
