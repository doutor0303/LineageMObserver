
// LineageMObserverDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "LineageMObserver.h"
#include "LineageMObserverDlg.h"
#include "afxdialogex.h"

#include <opencv2\core\core.hpp>
#include <opencv2\opencv.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>

#include <map>

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define _PK_ATTACK_COUNTER 0
#define _PK_ATTACK_LARGE 0
#define _PK_ATTACK_SMALL 1

#if _PK_ATTACK_COUNTER
	#define PK_ICON_SIZE_X 658
	#define PK_ICON_SIZE_Y 311
	#define PK_ICON_SIZE_W 54
	#define PK_ICON_SIZE_H 54
#elif _PK_ATTACK_LARGE
	#define PK_ICON_SIZE_X 730
	#define PK_ICON_SIZE_Y 338
	#define PK_ICON_SIZE_W 40
	#define PK_ICON_SIZE_H 40
#elif _PK_ATTACK_SMALL
	#define PK_ICON_SIZE_X 745
	#define PK_ICON_SIZE_Y 353
	#define PK_ICON_SIZE_W 10
	#define PK_ICON_SIZE_H 10	
	const int PK_ICON_THRESHOLD = 90;
#endif

#define HP_ICON_SIZE_X 190
#define HP_ICON_SIZE_Y 28
#define HP_ICON_SIZE_W 34
#define HP_ICON_SIZE_H 34
const int HP_ICON_THRESHOLD = 98;

#define HP_BAR_SIZE_X 48
#define HP_BAR_SIZE_Y 34
#define HP_BAR_SIZE_W 124
#define HP_BAR_SIZE_H 1
const int HP_BAR_THRESHOLD = 98;

#define ZONE_ICON_SIZE_X 695
#define ZONE_ICON_SIZE_Y 210
#define ZONE_ICON_SIZE_W 63
#define ZONE_ICON_SIZE_H 9
const int ZONE_THRESHOLD = 85;

#define POISON_R 15
#define POISON_G 70
#define POISON_B 15

#define PK_MAX_LEVEL	8

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.
#define RENDER_WINDOW_NAME "RenderWindow"
static bool m_bFindWindowBtn = false;

enum {
	TYPE_ZONE = 0,
	TYPE_HP = 1,
	TYPE_PK = 2,
	TYPE_HP_BAR = 3,
	TYPE_FULL = 4,
};

/* OpenCV Variables */
int hbins = 30, sbins = 32;
int histSize[] = { hbins, sbins };
int channels[] = { 0, 1 };

float hranges[] = { 0, 180};
float sranges[] = { 0, 256 };
const float* ranges[] = { hranges, sranges };

using namespace cv;

Mat pk_hsv[PK_MAX_LEVEL];

Mat RefZoneHsv, RefHpHsv, RefHpBarHsv, RefPkHsv;
MatND tmpHist, RefZoneHist, RefHpHist, RefHpBarHist, RefPkHist;

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
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


// CLineageMObserverDlg 대화 상자



CLineageMObserverDlg::CLineageMObserverDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_LINEAGEMOBSERVER_DIALOG, pParent)
	, m_EditTimeValue(_T(""))
	, m_prevType(-1)
	, m_prevAcc(-1)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pWndShow = NULL;
}

void CLineageMObserverDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_LOG, m_ListLog);


	DDX_Control(pDX, IDC_EDIT_TIME, m_EditTime);
	DDV_MinMaxInt(pDX, IDC_EDIT_TIME, 1, 60);
	DDX_Control(pDX, IDC_CHECK_HP30, m_CheckHp30Under);
	DDX_Control(pDX, IDC_CHECK_PK, m_CheckPlayerKill);
	DDX_Control(pDX, IDC_CHECK_HP, m_CheckHpPotion);
	DDX_Control(pDX, IDC_EDIT_STATUS, m_EditStatus);
	DDX_Control(pDX, IDC_COMBO_PK, m_Combo_Pk);
}

BEGIN_MESSAGE_MAP(CLineageMObserverDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_FIND_WINDOW, &CLineageMObserverDlg::OnBnClickedBtnFindWindow)
	ON_WM_LBUTTONUP()
	ON_BN_CLICKED(IDC_BTN_TEST, &CLineageMObserverDlg::OnBnClickedBtnTest)
END_MESSAGE_MAP()


// CLineageMObserverDlg 메시지 처리기

BOOL CLineageMObserverDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	m_ListLog.InitStorage(100000, 10); // 로그 버퍼 최대
	SetDlgItemInt(IDC_EDIT_TIME, 2); // 반복 초기값 2초

	m_CheckHp30Under.SetCheck(BST_CHECKED);
	m_CheckPlayerKill.SetCheck(BST_CHECKED);
	m_CheckHpPotion.SetCheck(BST_CHECKED);

	m_Combo_Pk.AddString(_T("비웃고 귀환"));
	m_Combo_Pk.AddString(_T("마을로 귀환"));
	m_Combo_Pk.AddString(_T("순간이동"));
	m_Combo_Pk.SetCurSel(1);

	{
		int dim(256); // 영상 역전 테이블 생성
		cv::Mat lut(1, &dim, CV_8U);
		// 1은 1차원, &dim은 256 항목, CV_8U는 uchar
		for (int i = 0; i<256; i++) {
			if (i < 150) {
				lut.at<uchar>(i) = 0;
			}
			else {
				lut.at<uchar>(i) = i;
			}
		}

		RefZoneHsv = imread(".\\reference\\ref_normal_zone.bmp");
		RefHpHsv = imread(".\\reference\\ref_hp0.bmp");
		RefHpBarHsv = imread(".\\reference\\ref_hp_bar.bmp");

		LUT(RefZoneHsv, lut, RefZoneHsv);
		calcHist(&RefZoneHsv, 1, channels, Mat(), /* do not use mask*/ RefZoneHist, 2, histSize, ranges);
		LUT(RefHpHsv, lut, RefHpHsv);
		calcHist(&RefHpHsv, 1, channels, Mat(), /* do not use mask*/   RefHpHist, 2, histSize, ranges);
	}

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}
void CLineageMObserverDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CLineageMObserverDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CLineageMObserverDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CLineageMObserverDlg::OnBnClickedBtnFindWindow()
{
	SetCapture();
	m_bFindWindowBtn = true;
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CLineageMObserverDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (m_bFindWindowBtn) {
		CString windowName;
		GetCursorPos(&point);
		m_hMainWnd = WindowFromPoint(point);
		m_hMainWnd->GetWindowTextW(windowName);
		GetDlgItem(IDC_EDIT_WINDOW)->SetWindowTextW(windowName);
		
		if (m_hMainWnd != NULL) {
			m_hRenderWnd = FindWindowEx(m_hMainWnd->m_hWnd, NULL, _T(RENDER_WINDOW_NAME), NULL);

			if (m_hRenderWnd != NULL) {
				pCaptureThread = AfxBeginThread(CaptureThread, this);
//				pPkMonitorThread = AfxBeginThread(PkMonitorThread, this);
				reloadMap();
				teleBot.setPrefixString(windowName);
			}
			else {
				AfxMessageBox(_T("화면을 찾을 수 없습니다."));
			}
		}
		else {
			AfxMessageBox(_T("화면을 찾을 수 없습니다."));
		}
		m_bFindWindowBtn = false;
		ReleaseCapture();
	}

	CDialogEx::OnLButtonUp(nFlags, point);
}


int CLineageMObserverDlg::TestOpenCV()
{
	// TODO: 여기에 구현 코드 추가.

	MatND hist1;
	MatND hist2;
	Mat src, arti_src, dst;
	Mat pk_counter_hsv, arti_ref;

	double res = 0, res1 = 0;

	src = imread(".\\pk.bmp", 1);

	int dim(256); // 영상 역전 테이블 생성
	cv::Mat lut(1, &dim, CV_8U);
	// 1은 1차원, &dim은 256 항목, CV_8U는 uchar
	for (int i = 0; i<256; i++) {
		if (i < 255) {
			lut.at<uchar>(i) = 0;
		}
		else {
			lut.at<uchar>(i) = i;
		}
	}
	
//k_counter_hsv = imread(".\\reference\\ref_pk_small.bmp", 1);
//	LUT(pk_counter_hsv, lut, arti_ref);
//	calcHist(&arti_ref, 1, channels, Mat(), /* do not use mask*/   hist2, 2, histSize, ranges);

	LUT(src, lut, arti_src);
	calcHist(&arti_src, 1, channels, Mat(), /* do not use mask*/   hist1, 2, histSize, ranges);

//	res = compareHist(hist1, hist2, CV_COMP_BHATTACHARYYA); // 차이점 0부터

	imshow("show", arti_src);

	return 0;
}


void CLineageMObserverDlg::OnBnClickedBtnTest()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	/*	Telegram Test
	CString tmp;
	m_LatestPkFileName = _T(".\\pklist\\pk.bmp");
	tmp.Format(_T(" -k -F chat_id=544284998 -F photo=@" + m_LatestPkFileName + " https://api.telegram.org/bot641687142:AAHh_PiL27onti0AnocDzdjIzf3oBEU9Ofc/sendPhoto"));
	utils.executeShell(_T(".\\bin\\curl.exe"), tmp);
	teleBot.sendMessage(_T("A"));*/

	//TestOpenCV();
	processImageComp(TYPE_PK);
}


int CLineageMObserverDlg::SavefileFromBitmap(HBITMAP hBitmap, LPCTSTR lpszFileName)
{
	// TODO: 여기에 구현 코드 추가.
	HDC hDC;
	int iBits;
	WORD wBitCount;
	DWORD dwPaletteSize = 0, dwBmBitsSize = 0, dwDIBSize = 0, dwWritten = 0;
	BITMAP Bitmap;
	BITMAPFILEHEADER bmfHdr;
	BITMAPINFOHEADER bi;
	LPBITMAPINFOHEADER lpbi;
	HANDLE fh, hDib, hPal, hOldPal = NULL;

	hDC = ::CreateDC(_T("DISPLAY"), NULL, NULL, NULL);
	iBits = ::GetDeviceCaps(hDC, BITSPIXEL) * ::GetDeviceCaps(hDC, PLANES);
	::DeleteDC(hDC);
	if (iBits <= 1)
		wBitCount = 1;
	else if (iBits <= 4)
		wBitCount = 4;
	else if (iBits <= 8)
		wBitCount = 8;
	else
		wBitCount = 24;
	GetObject(hBitmap, sizeof(Bitmap), (LPSTR)&Bitmap);
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = Bitmap.bmWidth;
	bi.biHeight = Bitmap.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = wBitCount;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrImportant = 0;
	bi.biClrUsed = 0;
	dwBmBitsSize = ((Bitmap.bmWidth * wBitCount + 31) / 32) * 4 * Bitmap.bmHeight;

	hDib = GlobalAlloc(GHND, dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER));
	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
	*lpbi = bi;

	hPal = GetStockObject(DEFAULT_PALETTE);
	if (hPal)
	{
		hDC = ::GetDC(NULL);
		hOldPal = ::SelectPalette(hDC, (HPALETTE)hPal, FALSE);
		::RealizePalette(hDC);
	}


	GetDIBits(hDC, hBitmap, 0, (UINT)Bitmap.bmHeight, (LPSTR)lpbi + sizeof(BITMAPINFOHEADER)
		+ dwPaletteSize, (BITMAPINFO *)lpbi, DIB_RGB_COLORS);

	if (hOldPal)
	{
		::SelectPalette(hDC, (HPALETTE)hOldPal, TRUE);
		::RealizePalette(hDC);
		::ReleaseDC(NULL, hDC);
	}

	fh = CreateFile(lpszFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);

	if (fh == INVALID_HANDLE_VALUE)
		return FALSE;

	bmfHdr.bfType = 0x4D42; // "BM"
	dwDIBSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwPaletteSize + dwBmBitsSize;
	bmfHdr.bfSize = dwDIBSize;
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + dwPaletteSize;

	WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);

	WriteFile(fh, (LPSTR)lpbi, dwDIBSize, &dwWritten, NULL);
	GlobalUnlock(hDib);
	GlobalFree(hDib);
	CloseHandle(fh);

	return 0;
}

static int count=0;

void CLineageMObserverDlg::GetBitmapFromWindow(_IconInfo iconInfo)
{
	CRect rct;
	HWND hTargetWnd = m_hRenderWnd->m_hWnd;
	if (hTargetWnd)
		::GetWindowRect(hTargetWnd, &rct);
	else
		return;

	HBITMAP hBitmap = NULL;
	HBITMAP hOldBitmap = NULL;
	BOOL bSuccess = FALSE;

	HDC hDC = ::GetDC(hTargetWnd);
	HDC hMemDC = ::CreateCompatibleDC(hDC);
	hBitmap = ::CreateCompatibleBitmap(hDC, iconInfo.w, iconInfo.h);

	if (!hBitmap)
		return;

	hOldBitmap = (HBITMAP)::SelectObject(hMemDC, hBitmap);

	if (!::PrintWindow(hTargetWnd, hMemDC, 0x00)) {
		bSuccess = FALSE;
		AfxMessageBox(_T("실패"));
	}
	else
		bSuccess = TRUE;

	BitBlt(hMemDC, 0, 0, iconInfo.w, iconInfo.h, hDC, iconInfo.x, iconInfo.y, SRCCOPY);

	if (bSuccess && !SavefileFromBitmap(hBitmap, iconInfo.name))
		bSuccess = FALSE;
	else
		bSuccess = TRUE;

	::SelectObject(hMemDC, hOldBitmap);
	DeleteObject(hBitmap);
	::DeleteDC(hMemDC);
	::ReleaseDC(hTargetWnd, hDC);

	return;
}

UINT CLineageMObserverDlg::PkMonitorThread(LPVOID _method) {
	CTime cTime;
	CString tmp;

	CLineageMObserverDlg *pDlg = (CLineageMObserverDlg*)AfxGetApp()->m_pMainWnd;

	while (true) {
		pDlg->doCapture(TYPE_PK);
		if (pDlg->processImageComp(TYPE_PK)) {
			cTime = CTime::GetCurrentTime();
			tmp.Format(_T("[%02d:%02d:%02d] 마을로 워프!!!"), cTime.GetHour(), cTime.GetMinute(), cTime.GetSecond());
			pDlg->m_ListLog.SetCurSel(pDlg->m_ListLog.InsertString(-1, tmp));
			pDlg->doWarf();
		}

		Sleep(500);
	}

	return 0;
}

UINT CLineageMObserverDlg::CaptureThread(LPVOID _method) {
	CTime cTime;
	CString tmp;

	CLineageMObserverDlg *pDlg = (CLineageMObserverDlg*)AfxGetApp()->m_pMainWnd;

	while(true) {
		pDlg->doCapture(TYPE_ZONE);
		pDlg->doCapture(TYPE_HP);
		pDlg->doCapture(TYPE_HP_BAR);
		pDlg->doCapture(TYPE_PK);

		Sleep(123);

		if (pDlg->processImageComp(TYPE_ZONE)) { // NORMAL ZONE
			if (pDlg->processImageComp(TYPE_HP) || pDlg->processImageComp(TYPE_HP_BAR) || pDlg->processImageComp(TYPE_PK)) {
				cTime = CTime::GetCurrentTime();
				tmp.Format(_T("[%02d:%02d:%02d] 마을로 워프!!!"), cTime.GetHour(), cTime.GetMinute(), cTime.GetSecond());
				pDlg->m_ListLog.SetCurSel(pDlg->m_ListLog.InsertString(-1, tmp));
				pDlg->doWarf();

				pDlg->teleBot.sendMessage(_T("마을로 이동... 이동사유 : ") + pDlg->typetoString(pDlg->m_ReasonType));

				if(pDlg->m_ReasonType == TYPE_PK) {
					pDlg->teleBot.sendPhoto(pDlg->m_LatestPkFileName);
				}
			}
		}
		Sleep(pDlg->GetDlgItemInt(IDC_EDIT_TIME)*1000); //
	}

	return 0;
}


bool CLineageMObserverDlg::doCapture(int type)
{
	// TODO: 여기에 구현 코드 추가.
	bool nValue;
	m_MapType.Lookup(type, nValue);
	if (!nValue) {
		return false;
	}

	_IconInfo iconInfo;

	switch (type) {
		case TYPE_ZONE:
			iconInfo.w = ZONE_ICON_SIZE_W;
			iconInfo.h = ZONE_ICON_SIZE_H;
			iconInfo.x = ZONE_ICON_SIZE_X;
			iconInfo.y = ZONE_ICON_SIZE_Y;
			iconInfo.name.Format(_T(".\\zone.bmp"));
			break;
		case TYPE_HP:
			iconInfo.w = HP_ICON_SIZE_W;
			iconInfo.h = HP_ICON_SIZE_H;
			iconInfo.x = HP_ICON_SIZE_X;
			iconInfo.y = HP_ICON_SIZE_Y;
			iconInfo.name.Format(_T(".\\hp.bmp"));
			break;
		case TYPE_HP_BAR:
			iconInfo.w = HP_BAR_SIZE_W;
			iconInfo.h = HP_BAR_SIZE_H;
			iconInfo.x = HP_BAR_SIZE_X;
			iconInfo.y = HP_BAR_SIZE_Y;
			iconInfo.name.Format(_T(".\\hp_bar.bmp"));
			break;
		case TYPE_PK:
			iconInfo.w = PK_ICON_SIZE_W;
			iconInfo.h = PK_ICON_SIZE_H;
			iconInfo.x = PK_ICON_SIZE_X;
			iconInfo.y = PK_ICON_SIZE_Y;
			iconInfo.name.Format(_T(".\\pk.bmp"));
			break;

		case TYPE_FULL: {
			CTime cTime;
			CString tmp;

			iconInfo.w = 800;
			iconInfo.h = 480;
			iconInfo.x = 0;
			iconInfo.y = 0;
			cTime = CTime::GetCurrentTime();
			tmp.Format(_T(".\\pklist\\%04d%02d%02d_%02d%02d%02d_PK.bmp"), cTime.GetYear(), cTime.GetMonth(), cTime.GetDay(), cTime.GetHour(), cTime.GetMinute(), cTime.GetSecond());
			iconInfo.name.Format(tmp);

			m_LatestPkFileName = tmp;
			}
			break;

		default:
			break;
	}

	GetBitmapFromWindow(iconInfo);

	return false;
}

bool CLineageMObserverDlg::processImageComp(int type)
{	// TODO: 여기에 구현 코드 추가.
	bool nValue;
	m_MapType.Lookup(type, nValue);
	if (!nValue) {
		return false;
	}
	int lut_th = 0;

	m_ReasonType = type;

	if (type == TYPE_PK) {
		lut_th = 200;
	}
	else {
		lut_th = 150;
	}

	Mat src, arti;
	MatND hist1, hist2;
	double res = 0;
	bool isLutEnable = true;
	int THRESHOLD = 100;

	int dim(256); // 영상 역전 테이블 생성
	cv::Mat lut(1, &dim, CV_8U);
	// 1은 1차원, &dim은 256 항목, CV_8U는 uchar
	for (int i = 0; i < 256; i++) {
		if (i < lut_th) {
			lut.at<uchar>(i) = 0;
		}
		else {
			lut.at<uchar>(i) = i;
		}
	}

	switch (type) {
		case TYPE_ZONE:
			src = imread(".\\zone.bmp");
			tmpHist = RefZoneHist;
			THRESHOLD = ZONE_THRESHOLD;
			break;
		case TYPE_HP:
			src = imread(".\\hp.bmp");
			tmpHist = RefHpHist;
			THRESHOLD = HP_ICON_THRESHOLD;
			break;
		case TYPE_HP_BAR:
			src = imread(".\\hp_bar.bmp");
			tmpHist = RefHpBarHist;
			isLutEnable = false;
			break;
		case TYPE_PK:
			src = imread(".\\pk.bmp");
			isLutEnable = false;
			break;

		default:
			break;
	}

	// 5픽셀 : 상태 체크

	if (isLutEnable) {
		LUT(src, lut, src);
		calcHist(&src, 1, channels, Mat(), /* do not use mask*/ hist1, 2, histSize, ranges);

		res = compareHist(hist1, tmpHist, CV_COMP_BHATTACHARYYA); // 차이점 0부터
	}
	else {
		if (type == TYPE_HP_BAR) {
			if (src.channels() == 3) {
				int sb = src.data[5 * 3 + 0]; // 돈케어
				int sg = src.data[5 * 3 + 1]; // 70이상
				int sr = src.data[5 * 3 + 2]; // 60미만

				int b = src.data[30 * 3 + 0]; // 124 / 30
				int g = src.data[30 * 3 + 1];
				int r = src.data[30 * 3 + 2];

				int status = getCharacterStatus(sr, sg, sb);

				if (status != 0) { // !독
					if (g > 30 && b > 30) {
						printLog(type, 0, 0);
						return true;
					}
				}
			}
		}
		else if (type == TYPE_PK) {
			LUT(src, lut, arti);

			int count = 0;
			int height = arti.rows;
			int width = arti.cols;

			uchar *data_input = arti.data;

			if (data_input != NULL) {
				if (arti.channels() == 3) {
					for (int y = 0; y < height; y++) {
						for (int x = 0; x < width; x++) {

							uchar b = data_input[y*width * 3 + x * 3 + 0]; // 124 / 30
							uchar g = data_input[y*width * 3 + x * 3 + 1];
							uchar r = data_input[y*width * 3 + x * 3 + 2];

							if (b == 0 && g == 0 && r > 200) {
								count++;
							}
						}
					}
					if (count > 45) {
						teleBot.sendMessage(TEXT("PK..."));
						doCapture(TYPE_FULL);

						printLog(type, 0, 0);
						return true;
					}
				}
			}
		}
		return false;
	}

	printLog(type, (int)(100 - (res * 100)), THRESHOLD);

	if (100 - (res * 100) > THRESHOLD) {
		return true;
	}
				
	return false;
}


int CLineageMObserverDlg::doWarf()
{
	// TODO: 여기에 구현 코드 추가.
	m_hRenderWnd->PostMessageW(WM_LBUTTONUP, MK_LBUTTON, MAKELPARAM(750, 400));
	Sleep(211);
	m_hRenderWnd->PostMessageW(WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(750, 400));
	Sleep(251);
	m_hRenderWnd->PostMessageW(WM_LBUTTONUP, MK_LBUTTON, MAKELPARAM(750, 400));

	return 0;
}

void CLineageMObserverDlg::printLog(int type, int cur, const int THRESHOLD)
{
	// TODO: 여기에 구현 코드 추가.
	int idx;
	CTime cTime;
	CString tmp;
	cTime = CTime::GetCurrentTime();

	if(type == TYPE_HP || type == TYPE_ZONE) {
		tmp.Format(_T("[%02d:%02d:%02d] 타입:%s, 유사도:%3d%%, 기준:%3d%%"), cTime.GetHour(), cTime.GetMinute(), cTime.GetSecond(), typetoString(type), cur, THRESHOLD);
	}
	else if(type == TYPE_HP_BAR) {
		tmp.Format(_T("[%02d:%02d:%02d] 타입:%s, HP가 30%% 미만..."), cTime.GetHour(), cTime.GetMinute(), cTime.GetSecond(), typetoString(type));
	}
	else if (type == TYPE_PK) {
		tmp.Format(_T("[%02d:%02d:%02d] 타입:%s, PK 당하는 중인 듯?"), cTime.GetHour(), cTime.GetMinute(), cTime.GetSecond(), typetoString(type));
	}

	if (!isSamePrevResult(type, cur)) {
		idx = m_ListLog.InsertString(-1, tmp);
		m_ListLog.SetCurSel(idx);

		m_prevAcc = cur;
		m_prevType = type;
	}
}


CString CLineageMObserverDlg::typetoString(int type)
{
	switch (type) {
		case TYPE_ZONE:
			return _T("ZONE   ");
		case TYPE_HP:
			return _T("HP       ");
		case TYPE_HP_BAR:
			return _T("HP_BAR");
		case TYPE_PK:
			return _T("PK       ");
		default:
			break;
	}

	// TODO: 여기에 구현 코드 추가.
	return _T("UNKNOWN");
}


int CLineageMObserverDlg::getCharacterStatus(int r, int g, int b)
{
	// TODO: 여기에 구현 코드 추가.
	if (g > 70 && r < 70) { // 독
		m_EditStatus.SetWindowTextW(_T("독"));
		return 0;
	}
	if (r > 100 && g < 20 && b < 20) { // 빨간 HP
		m_EditStatus.SetWindowTextW(_T("정상"));
		return 1;
	}

	return 0;
}

bool CLineageMObserverDlg::isSamePrevResult(int curType, int curAcc)
{
	// TODO: 여기에 구현 코드 추가.
	if (curAcc == -1) {
		if (curType == m_prevType) {
			return true;
		}
	}
	else {
		if (curType == m_prevType && curAcc == m_prevAcc) {
			return true;
		}
	}

	return false;
}


void CLineageMObserverDlg::reloadMap()
{
	m_MapType.SetAt(TYPE_ZONE, true);
	m_MapType.SetAt(TYPE_HP, m_CheckHpPotion.GetCheck());
	m_MapType.SetAt(TYPE_HP_BAR, m_CheckHp30Under.GetCheck());
	m_MapType.SetAt(TYPE_PK, m_CheckPlayerKill.GetCheck());

	// TODO: 여기에 구현 코드 추가.
}
