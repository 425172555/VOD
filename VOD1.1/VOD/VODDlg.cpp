//
// VOD2Dlg.cpp: 实现文件
//
#include "afxdialogex.h"
#include "stdafx.h"
#include "VOD.h"
#include "VODDlg.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <locale>
#include <map>
#include <unordered_map>
#include <string>


#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "GET_BMP_DATA.h"
#include "GetTestImage.h"
#include "ServerSampleGenerator.h"


using namespace std;
using namespace cv;


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//#define WM_UPDATEDATA WM_USER + 1999
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

CVODDlg* pDlg;//窗口指针
static int g_n = 0;//进度条计数量
std::unordered_map<string, std::vector<string>>g_u;//全局文件读写变量
std::map<string, string>_g_u;//全局文件读写变量

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

														// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CVOD2Dlg 对话框



CVODDlg::CVODDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_VOD_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CVODDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_FileName);
	DDX_Control(pDX, IDC_EDIT2, m_DisplayStep);
	DDX_Control(pDX, IDC_PROGRESS1, m_Process);
	DDX_Control(pDX, IDC_EDIT4, m_ImageTest);
}

BEGIN_MESSAGE_MAP(CVODDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CVODDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CVODDlg::OnBnClickedButton2)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDOK, &CVODDlg::OnBnClickedOk)
	//	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON3, &CVODDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CVODDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CVODDlg::OnBnClickedButton5)
END_MESSAGE_MAP()


// CVOD2Dlg 消息处理程序

BOOL CVODDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, TRUE);		// 设置小图标
	
	// TODO: 在此添加额外的初始化代码
	pDlg = this;					

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CVODDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CVODDlg::OnPaint()
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
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CVODDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

int CVODDlg::FrameCount()
{
	//统计视频的总帧数
	string file_name = CT2A(pDlg->m_VodFile.GetBuffer());
	VideoCapture vc = VideoCapture(file_name);

	int k = vc.get(CAP_PROP_FRAME_COUNT);
	if (k < 1)
		return -1;
	return k;
}



void CVODDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码 
	m_VodFile = _T("");
	m_Process.SetPos(0);
	m_DisplayStep.SetWindowTextW(_T(""));

	m_ImageTest.SetWindowTextW(_T(""));

	CFileDialog    dlgFile(TRUE, NULL, NULL, OFN_HIDEREADONLY, _T("Media Files (*.mp4)|*.mp4|All Files (*.*)|*.*||"), NULL);
	if (dlgFile.DoModal() == IDOK)
	{
		m_VodFile = dlgFile.GetPathName();
		FileName = dlgFile.GetFileName();
		m_FileName.SetWindowTextW(m_VodFile);
		
	}
	
}


void CVODDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	m_Process.SetPos(0);
	int k = FrameCount();
	m_Process.SetRange32(0, k); //设置进度条的范围 

	AfxBeginThread(ThreadFun_7, this);//启动子线程
	
	while (g_n <= k) {
		m_Process.SetPos(g_n); //进度条的位置 
		MSG   msg;
		while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

void CVODDlg::OnBnClickedButton5()
{
	// TODO: 在此添加控件通知处理程序代码
	m_Process.SetPos(0);
	int k = FrameCount();
	m_Process.SetRange32(0, k); //设置进度条的范围 

	AfxBeginThread(ThreadFun_5, this);//启动子线程

	while (g_n <= k) {
		m_Process.SetPos(g_n); //进度条的位置 
		MSG   msg;
		while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}


void CVODDlg::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码
	//初始化g_u
	if (g_u.empty()) {
		CString txt_content, _txt_content;
		txt_content = _T("D:\\VOD\\sum_7.hash");
		_txt_content = _T("D:\\VOD\\sum_5.hash");
		g_u = insert_map(txt_content);
		_g_u = _insert_map(_txt_content);
	}
	CString tmpstr, output = _T("");
	m_ImageFile = _T("");
	CFileDialog    dlgFile(TRUE, NULL, NULL, OFN_HIDEREADONLY, _T("Image Files (*.jpg)|*.jpg|All Files (*.*)|*.*||"), NULL);
	if (dlgFile.DoModal() == IDOK)
	{
		m_ImageFile = dlgFile.GetPathName();
	}

	string image = CT2A(m_ImageFile.GetBuffer());
	GetTestImage Image;
	std::vector<vector<char>>m = Image.get_ImageFingerprint(image, 7);
	std::vector<vector<char>>_m = Image.get_ImageFingerprint(image, 5);

	CString fingerprint = Image.vector_to_CString(m);
	CString _fingerprint = Image.vector_to_CString(_m);

	string n = CT2A(fingerprint.GetBuffer());
	string _n = CT2A(_fingerprint.GetBuffer());
	string _trans, trans;
	vector<string>b;
	int tmp = 0;
	int result = 0;

	if (_g_u.count(_n) > 0) {
		for (std::map<string, string>::iterator iter = _g_u.begin(); iter != _g_u.end(); iter++) {
			if (_n == iter->first) {
				b.push_back(iter->second);
			}
			vector<string>::iterator _b = b.begin();
			if (!b.empty()) {
				result = hamdist(*_b, n);
				if (result >= tmp) {
					tmp = result;
					_trans = *_b;
				}
				b.clear();
			}
			if (g_u.count(_trans) > 0) {
				b = g_u[_trans];
				for (std::vector<string>::iterator it = b.begin(); it != b.end(); ++it) {
					trans = it->data();
					tmpstr = trans.c_str();
					output += tmpstr;
					output += " ";
				}
			}
		}
		m_ImageTest.SetWindowTextW(output);
	}
	else {
		m_ImageTest.SetWindowTextW(_T("unknown!"));
	}
}

void CVODDlg::OnBnClickedButton4()
{
	// TODO: 在此添加控件通知处理程序代码
	//提取文件
	CString txt_content = _T("D:\\VOD\\sum_7.hash");
	CString _txt_content = _T("D:\\VOD\\sum_5.hash");
	g_u = insert_map(txt_content);
	_g_u = _insert_map(_txt_content);

	//合并文件
	int nPos = pDlg->m_VodFile.ReverseFind('.');
	CString tmpstr_txt = pDlg->m_VodFile.Left(nPos) + _T("_7_merge.hash");
	CString _tmpstr_txt = pDlg->m_VodFile.Left(nPos) + _T("_5.hash");

	std::unordered_map<string, std::vector<string>>u = insert_map(tmpstr_txt);
	std::map<string, string>_u = _insert_map(_tmpstr_txt);

	merge(g_u.begin(), g_u.end(), u.begin(), u.end(), inserter(g_u, g_u.end()));
	merge(_g_u.begin(), _g_u.end(), _u.begin(), _u.end(), inserter(_g_u, _g_u.end()));

	//存入文件
	ofstream ous("D:\\VOD\\sum_7.hash");
	ofstream _ous("D:\\VOD\\sum_5.hash");

	for (unordered_map<string, vector<string>>::iterator iter = g_u.begin(); iter != g_u.end(); iter++) {
		ous << iter->first << "    ";
		vector<string>::iterator iter_vector = iter->second.begin();
		for (; iter_vector != iter->second.end(); ++iter_vector) {
			ous << *iter_vector << " ";
		}
		ous << endl;
	}
	for (map<string, string>::iterator _iter = _g_u.begin(); _iter != _g_u.end(); _iter++) {

		_ous << _iter->first << "    ";
		string ccc = _iter->second;
		_ous << _iter->second;
		_ous << endl;
	}
	ous.close();
	_ous.close();
	g_u.clear();
	_g_u.clear();
}

UINT CVODDlg::ThreadFun_7(LPVOID lpParam)
{
	CString outstr, outstr_hash, outstr_txt, tmpstr, tmpstr_hash, tmpstr_txt;
	multimap<std::vector<char>, string>time_stamp;

	//m_VodFile.Open(VodFile, CFile::modeRead | CFile::typeBinary);
	//long  nlen = m_VodFile.GetLength();
	//if (nlen < 10)
	//{
	//	outstr = VodFile + _T(" file error!");
	//	DisplayStep.SetWindowTextW(outstr);
	//	m_VodFile.Close();
	//	return;
	//}
	//m_VodFile.Close();
	bool flag = TRUE;
	string file_name = CT2A(pDlg->m_VodFile.GetBuffer());
	VideoCapture vc = VideoCapture(file_name);
	if (!vc.isOpened())
	{
		outstr = pDlg->m_VodFile + _T(" file error!");
		pDlg->m_DisplayStep.SetWindowTextW(outstr);
		vc.release();
		return -1;
	}
	int k = vc.get(CAP_PROP_FRAME_COUNT);
	if (k < 1)
		return -1;
	int fps = vc.get(CAP_PROP_FPS);
	int imageW = vc.get(CAP_PROP_FRAME_WIDTH);
	int imageH = vc.get(CAP_PROP_FRAME_HEIGHT);

	Mat pFrame, result;
	IplImage* pBinary = NULL;

	tmpstr.Format(_T("%ld"), k);
	outstr = pDlg->m_VodFile + _T(" file frame length is ") + tmpstr;
	pDlg->m_DisplayStep.SetWindowTextW(outstr);

	std::vector< std::vector<char> > v;

	ServerSampleGenerator *s = new ServerSampleGenerator(imageW, imageH);

    int nPos = pDlg->m_VodFile.ReverseFind('.');

	tmpstr_hash = pDlg->m_VodFile.Left(nPos) + _T("_7.hash");
	CFileException mExcept;
	pDlg->m_HashFile.Open(tmpstr_hash, CFile::modeCreate | CFile::typeText | CFile::modeWrite, &mExcept);

	tmpstr_txt = pDlg->m_VodFile.Left(nPos) + _T("_7_merge.hash");
	pDlg->m_TxtFile.Open(tmpstr_txt, CFile::modeCreate | CFile::typeText | CFile::modeWrite, &mExcept);

	tmpstr.Format(_T("%ld"), k);
	outstr = _T(" file frame length is ") + tmpstr;
	outstr += _T("\r\n");
	pDlg->m_DisplayStep.SetWindowTextW(outstr);
	pDlg->m_HashFile.WriteString(outstr);

	for (int i = 0; i < k; ++i)
	{
		CString time;
		time.Format(_T("%f"), (float)(1000 / fps * i));

		CString con = pDlg->FileName + "_" + time;
		string content = CT2A(con.GetBuffer());

		vc.read(pFrame); //抓取一帧   
		cvtColor(pFrame, result, CV_BGR2GRAY);//转灰度图
		pBinary = &IplImage(result);

		//if (ImageBlackDecide(&pBinary) > 0) {
			v = s->GenerateServerSample(pBinary, imageW / 2, imageH / 2, imageW / 2, imageH / 2);
			outstr_hash = _T("");
			vector<vector<char>>::iterator interator = v.begin();
			while (interator != v.end()) {
				vector<char> fingerprint = *interator;
				vector<char>::iterator inner = fingerprint.begin();
				time_stamp.insert(make_pair(fingerprint, content));

				while (inner != fingerprint.end()) {
					tmpstr_hash.Format(_T("%02x"), (unsigned char)*inner);
					outstr_hash += tmpstr_hash;
					inner++;
					g_n = i;//进度条变量
				}
				outstr_hash += _T("   ");
				outstr_hash += pDlg->FileName;
				outstr_hash += _T("_");
				outstr_hash += time;
				interator++;
			}
			outstr_hash += _T("\r\n");
			setlocale(LC_CTYPE, "chs");
			pDlg->m_HashFile.WriteString(outstr_hash);
		}
	outstr_hash = _T("\r\n};\r\n");
	pDlg->m_HashFile.WriteString(outstr_hash);
	pDlg->m_HashFile.Close();
	
	for (multimap<vector<char>, string>::iterator ch = time_stamp.begin(); ch != time_stamp.end();)
	{
		std::pair<std::multimap<vector<char>, string>::iterator, std::multimap<vector<char>, string>::iterator> ret;
		ret = time_stamp.equal_range(ch->first);
		outstr_txt = _T("");

		vector<char>fingerprint = ch->first;
		//	std::string res;
		//	res = CT2A(outstr_txt.GetBuffer());
		vector<char>::iterator inner1 = fingerprint.begin();
		while (inner1 != fingerprint.end()) {
			tmpstr_txt.Format(_T("%02x"), (unsigned char)*inner1);
			outstr_txt += tmpstr_txt;
			inner1++;
		}
		outstr_txt += _T("   ");
		for (std::multimap<vector<char>, string>::iterator it = ret.first; it != ret.second; ++it) {
			tmpstr_txt = it->second.c_str();
			outstr_txt += tmpstr_txt;
			outstr_txt += _T(" ");
		}
		if (flag) {
			outstr_txt += _T("\r\n");
			pDlg->m_TxtFile.WriteString(outstr_txt);
		}
		multimap<vector<char>, string>::iterator ch1 = ch++;
		if (ch != time_stamp.end()) {
			if (ch1->first == ch->first) {
				flag = FALSE;
			}
			else {
				flag = TRUE;
			}
		}
	}
	vc.release();
	pDlg->m_TxtFile.Close();

	outstr = _T("Create hash file done!");
	pDlg->m_DisplayStep.SetWindowTextW(outstr);
	return 0;
}

UINT CVODDlg::ThreadFun_5(LPVOID lpParam)
{
	CString outstr, outstr_hash, outstr_txt, tmpstr, tmpstr_hash, tmpstr_txt;
	multimap<std::vector<char>, std::vector<char>>time_stamp1;

	bool flag = TRUE;
	string file_name = CT2A(pDlg->m_VodFile.GetBuffer());
	VideoCapture vc = VideoCapture(file_name);
	if (!vc.isOpened())
	{
		outstr = pDlg->m_VodFile + _T(" file error!");
		pDlg->m_DisplayStep.SetWindowTextW(outstr);
		vc.release();
		return -1;
	}
	int k = vc.get(CAP_PROP_FRAME_COUNT);
	if (k < 1)
		return -1;
	int fps = vc.get(CAP_PROP_FPS);
	int imageW = vc.get(CAP_PROP_FRAME_WIDTH);
	int imageH = vc.get(CAP_PROP_FRAME_HEIGHT);

	Mat pFrame, result;
	IplImage* pBinary = NULL;

	std::vector< std::vector<char> > v_5;
	std::vector< std::vector<char> > v_7;

	ServerSampleGenerator *s = new ServerSampleGenerator(imageW, imageH);
	CFileException mExcept;

	int nPos = pDlg->m_VodFile.ReverseFind('.');
	tmpstr_hash = pDlg->m_VodFile.Left(nPos) + _T("_5.hash");
	pDlg->m_HashFile.Open(tmpstr_hash, CFile::modeCreate | CFile::typeText | CFile::modeWrite, &mExcept);

	tmpstr.Format(_T("%ld"), k);
	outstr = _T(" file frame length is ") + tmpstr;
	outstr += _T("\r\n");
	pDlg->m_DisplayStep.SetWindowTextW(outstr);

	for (int i = 0; i < k; ++i)
	{
		CString time;
		time.Format(_T("%f"), (float)(1000 / fps * i));
		CString con = pDlg->FileName + "_" + time;
		string content = CT2A(con.GetBuffer());
		vc.read(pFrame); //抓取一帧
		cvtColor(pFrame, result, CV_BGR2GRAY);//转灰度图
		pBinary = &IplImage(result);
		v_7 = s->GenerateServerSample(pBinary, imageW / 2, imageH / 2, imageW / 2, imageH / 2, 7);
		v_5 = s->GenerateServerSample(pBinary, imageW / 2, imageH / 2, imageW / 2, imageH / 2, 5);
		outstr_hash = _T("");

		vector<vector<char>>::iterator interator = v_5.begin();
		vector<vector<char>>::iterator _interator = v_7.begin();
		while (interator != v_5.end()) {
			vector<char> fingerprint = *interator;
			vector<char>::iterator inner = fingerprint.begin();

			while (inner != fingerprint.end()) {
				tmpstr_hash.Format(_T("%02x"), (unsigned char)*inner);
				outstr_hash += tmpstr_hash;
				inner++;
			}
			outstr_hash += _T("   ");
			while (_interator != v_7.end()) {
				vector<char> _fingerprint = *_interator;
				vector<char>::iterator _inner = _fingerprint.begin();
				time_stamp1.insert(make_pair(fingerprint, _fingerprint));
				while (_inner != _fingerprint.end()) {
					tmpstr_hash.Format(_T("%02x"), (unsigned char)*_inner);
					outstr_hash += tmpstr_hash;
					_inner++;
					g_n = i;//进度条变量
				}
				_interator++;
			}

			interator++;
		}
		outstr_hash += _T("\r\n");
		setlocale(LC_CTYPE, "chs");
		pDlg->m_HashFile.WriteString(outstr_hash);
	}
	outstr_hash = _T("\r\n};\r\n");
	pDlg->m_HashFile.WriteString(outstr_hash);
	pDlg->m_HashFile.Close();
	pDlg->m_TxtFile.Close();
	vc.release();

	outstr = _T("Create hash file done!");
	pDlg->m_DisplayStep.SetWindowTextW(outstr);
	return 0;
}



std::unordered_map<std::string, vector<string>> CVODDlg::insert_map(CString filename)
{
	std::string feature;
	std::vector<string> lines;

	std::unordered_map<std::string, std::vector<string>>u;

	ifstream fin(filename, ios::in);
	if (!fin) {
		cerr << "File open error!\n" << endl;
	}
	while (getline(fin, feature)) {
		////删除\r\n回车符，换行符
		//if (!feature.empty() && feature[feature.size() - 1] == '\r')
		//	feature.erase(feature.size() - 1);
		//if (!feature.empty() && feature[feature.size() - 1] == '\n')
		//	feature.erase(feature.size() - 1);
		if (feature.empty()) {
			continue;
		}
		istringstream input(feature);
		// 码纹
		std::string mawen;
		input >> mawen;
		//时间戳
		string temp;
		while (!input.eof()) {
			fin.imbue(locale("", locale::all ^ locale::numeric));
			input >> temp;
			lines.push_back(temp);
		}
		lines.pop_back();
		u.insert(make_pair(mawen, lines));
		lines.clear();
	}
	fin.close();
	return u;
}

std::map<std::string, std::string> CVODDlg::_insert_map(CString filename)
{
	std::string feature;
	std::map<std::string, std::string>u;

	ifstream fin(filename, ios::in);
	if (!fin) {
		cerr << "File open error!\n" << endl;
	}
	while (getline(fin, feature)) {
		////删除\r\n回车符，换行符
		//if (!feature.empty() && feature[feature.size() - 1] == '\r')
		//	feature.erase(feature.size() - 1);
		//if (!feature.empty() && feature[feature.size() - 1] == '\n')
		//	feature.erase(feature.size() - 1);
		if (feature.empty()) {
			continue;
		}
		istringstream input(feature);
		//码纹
		std::string mawen, _mawen;

		input >> mawen >> _mawen;
		u.insert(make_pair(mawen, _mawen));
	}
	fin.close();
	return u;
}

int CVODDlg::hamdist(std::string c, std::string d)
{
	const char *a = c.c_str();
	const char *b = d.c_str();
	int dist = 0;
	while (*a && *b) {
		dist += (*a != *b) ? 1 : 0;
		*a++;
		*b++;
	}
	return dist;
}
void CVODDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	exit(0);
	CDialogEx::OnClose();
}
void CVODDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	exit(0);
	CDialogEx::OnOK();
}
