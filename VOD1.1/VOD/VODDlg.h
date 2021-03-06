
// VODDlg.h: 头文件
//

#pragma once
#include <unordered_map>
#include <map>

// CVODDlg 对话框
class CVODDlg : public CDialogEx
{
	// 构造
public:
	CVODDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_VOD_DIALOG };
#endif

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
	int FrameCount();
	CEdit m_FileName;
	CEdit m_DisplayStep;

	CString m_VodFile;
	CString FileName;
	CString m_ImageFile;

	CStdioFile m_HashFile;
	CStdioFile m_TxtFile;
	CProgressCtrl m_Process;
	CEdit m_ImageTest;

	static UINT ThreadFun_7(LPVOID lpParam);
	static UINT ThreadFun_5(LPVOID lpParam);
	static UINT ThreadFun_read(LPVOID lpParam);
	//static CVODDlg *p;
	static std::unordered_map<std::string, std::vector<std::string>> insert_map(CString filename);
	static std::map<std::string, std::string> _insert_map(CString filename);

	int hamdist(std::string c, std::string d);
	afx_msg void OnClose();
	afx_msg void OnBnClickedOk();
//	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
};
