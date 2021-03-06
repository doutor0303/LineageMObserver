
// LineageMObserverDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "LineageMObserver.h"
#include "LineageMObserverDlg.h"
#include "RegistrationTokenDlg.h"
#include "afxdialogex.h"

#include <opencv2\core\core.hpp>
#include <opencv2\opencv.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>

#include <map>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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

#define ZONE_ICON_SIZE_X 733 // 695
#define ZONE_ICON_SIZE_Y 210
#define ZONE_ICON_SIZE_W 25
#define ZONE_ICON_SIZE_H 9
const int ZONE_THRESHOLD = 85;

#define POISON_R 15
#define POISON_G 70
#define POISON_B 15

#define PK_MAX_LEVEL	8
#define MAX_ZONE_LIST 3
#define MAX_TELE_TIME_LIST 7

#define NORMAL_ZONE		0
#define COMBAT_ZONE		1
#define SAFETY_ZONE		2

#define KILL_POS_X		735
#define KILL_POS_Y		236
#define KILL_POS_W		22
#define KILL_POS_H		13
const int KILL_THRESHOLD = 90;

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.
#define RENDER_WINDOW_NAME "RenderWindow"
static bool m_bFindWindowBtn = false;

#define FILENAME_HP			".\\hp.bmp"
#define FILENAME_HP_BAR		".\\hp_bar.bmp"
#define FILENAME_PK			".\\pk.bmp"
#define FILENAME_KILL		 ".\\kill.bmp"
#define FILENAME_ZONE		 ".\\zone.bmp"

#define FILENAME_REF_HP0		".\\reference\\ref_hp0.bmp"
#define FILENAME_REF_KILL		".\\reference\\ref_kill.bmp"
#define FILENAME_REF_PK_COUNTER	".\\reference\\ref_pk_counter.bmp"
#define FILENAME_REF_PK_SMALL	".\\reference\\ref_pk_small.bmp"

#define FILENAME_ZONE_NORMAL	".\\reference\\NormalZone.bmp"
#define FILENAME_ZONE_COMBAT	".\\reference\\CombatZone.bmp"
#define FILENAME_ZONE_SAFETY	".\\reference\\SafetyZone.bmp"

/* OpenCV Variables */
int hbins = 30, sbins = 32;
int histSize[] = { hbins, sbins };
int channels[] = { 0, 1 };

float hranges[] = { 0, 180};
float sranges[] = { 0, 256 };
const float* ranges[] = { hranges, sranges };

using namespace cv;

Mat pk_hsv[PK_MAX_LEVEL];

Mat RefHpHsv, RefPkHsv, RefKillHsv;
MatND tmpHist, RefHpHist, RefHpBarHist, RefPkHist, RefKillHist;

const char *kZoneList[] = { FILENAME_ZONE_NORMAL, FILENAME_ZONE_COMBAT, FILENAME_ZONE_SAFETY };
const char *kTeleTimeList[] = { "2s", "5s", "10s", "30s", "60s", "5m", "10m"};

Mat zoneImg[3];
MatND zoneHist[3];

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
	DDX_Control(pDX, IDC_COMBO_TELETIME, m_Combo_TeleTime);
}

BEGIN_MESSAGE_MAP(CLineageMObserverDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_FIND_WINDOW, &CLineageMObserverDlg::OnBnClickedBtnFindWindow)
	ON_WM_LBUTTONUP()
	ON_BN_CLICKED(IDC_BTN_TEST, &CLineageMObserverDlg::OnBnClickedBtnTest)
	ON_BN_CLICKED(IDC_CHECK_PK, &CLineageMObserverDlg::OnBnClickedCheckPk)
	ON_BN_CLICKED(IDC_CHECK_HP30, &CLineageMObserverDlg::OnBnClickedCheckHp30)
	ON_BN_CLICKED(IDC_CHECK_HP, &CLineageMObserverDlg::OnBnClickedCheckHp)
	ON_BN_CLICKED(IDC_BTN_REG_TOKEN, &CLineageMObserverDlg::OnBnClickedBtnRegToken)
	ON_BN_CLICKED(IDC_BTN_OBSERVE, &CLineageMObserverDlg::OnBnClickedBtnObserve)
	ON_EN_CHANGE(IDC_EDIT_LOCATION, &CLineageMObserverDlg::OnEnChangeEditLocation)
	ON_CBN_SELCHANGE(IDC_COMBO_TELETIME, &CLineageMObserverDlg::OnCbnSelchangeComboTeletime)
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
	m_Combo_TeleTime.SetCurSel(1);

	loadIconInfo();

	m_CheckHp30Under.SetCheck(BST_CHECKED);
	m_CheckPlayerKill.SetCheck(BST_CHECKED);
	m_CheckHpPotion.SetCheck(BST_CHECKED);

	m_Combo_Pk.AddString(_T("비웃고 귀환"));
	m_Combo_Pk.AddString(_T("마을로 귀환"));
	m_Combo_Pk.AddString(_T("순간이동"));
	m_Combo_Pk.SetCurSel(1); 

	m_Combo_TeleTime.AddString(_T("2s"));
	m_Combo_TeleTime.AddString(_T("5s"));
	m_Combo_TeleTime.AddString(_T("10s"));
	m_Combo_TeleTime.AddString(_T("30s"));
	m_Combo_TeleTime.AddString(_T("60s"));
	m_Combo_TeleTime.AddString(_T("5m"));
	m_Combo_TeleTime.AddString(_T("10m"));

	m_Combo_TeleTime.SetCurSel(3); // 30s
	m_TelegramTime = atoi(kTeleTimeList[m_Combo_TeleTime.GetCurSel()]);

	loadOpenCVResource();
	loadTelegramTokenFromFile();

//	teleBot = TelegramBot(_T("544284998"), m_strTelegramToken);

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
				m_bCaptureReady = true;
				pCaptureThread = AfxBeginThread(CaptureThread, this);
				reloadMap();
				teleBot.setPrefixString(windowName);
				GetDlgItem(IDC_BTN_OBSERVE)->SetWindowText(_T("감지 중"));
				m_ListLog.ResetContent();
			}
			else {
				AfxMessageBox(_T("화면을 찾을 수 없습니다."));
				m_hRenderWnd = NULL;
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

	src = imread(FILENAME_PK, 1);

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

	/*	Telegram Test */
/*	CString tmp;
	m_LatestPkFileName = _T(".\\pklist\\pk.bmp");
	tmp.Format(_T(" -k -F chat_id=544284998 -F photo=@" + m_LatestPkFileName + " https://api.telegram.org/bot641687142:AAHh_PiL27onti0AnocDzdjIzf3oBEU9Ofc/sendPhoto"));
	Utils::executeShell(_T(".\\bin\\curl.exe"), tmp);
	teleBot.sendMessage(_T("A"));*/

	//TestOpenCV();
	//processImageComp(TYPE_PK);
//	HBITMAP hBitmap = (HBITMAP)LoadImage(NULL, _T(".\\pk.bmp"), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE);
//	imshow("name", img);
}

static int count=0;

void CLineageMObserverDlg::GetBitmapFromWindow()
{
	CRect rct;
	_IconInfo* iconInfo;
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

	/*	for (int idx = 0; idx < TYPE_MAX; idx++) {
			hBitmap = ::CreateCompatibleBitmap(hDC, m_IconInfo[idx].w, m_IconInfo[idx].h);

			if (!hBitmap)
				return;

			hOldBitmap = (HBITMAP)::SelectObject(hMemDC, hBitmap);

			if (!::PrintWindow(hTargetWnd, hMemDC, 0x01)) {
				bSuccess = FALSE;
				AfxMessageBox(_T("실패"));
			}
			else
				bSuccess = TRUE;

			BitBlt(hMemDC, 0, 0, m_IconInfo[idx].w, m_IconInfo[idx].h, hDC, m_IconInfo[idx].x, m_IconInfo[idx].y, SRCCOPY);
			m_IconInfo[idx].src = Utils::hBitmapToMat(hBitmap);
		}*/

	for (int idx = 0; idx < TYPE_MAX; idx++)
	{
		iconInfo = &m_IconInfo[idx];

		hBitmap = ::CreateCompatibleBitmap(hDC, iconInfo->w, iconInfo->h);

		if (!hBitmap)
			return;

		hOldBitmap = (HBITMAP)::SelectObject(hMemDC, hBitmap);

		if (!::PrintWindow(hTargetWnd, hMemDC, 0x01)) {
			bSuccess = FALSE;
			AfxMessageBox(_T("실패"));
		}
		else
			bSuccess = TRUE;

		BitBlt(hMemDC, 0, 0, iconInfo->w, iconInfo->h, hDC, iconInfo->x, iconInfo->y, SRCCOPY);
		iconInfo->src = Utils::hBitmapToMat(hBitmap);

		if (iconInfo->isSave) {
			if (bSuccess && !utils.SaveFromHBitmapToJpg(hBitmap, iconInfo->name))
				bSuccess = FALSE;
			else
				bSuccess = TRUE;
		}
	}

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

static int sTeleSendingCnt = 0;

UINT CLineageMObserverDlg::CaptureThread(LPVOID _method) {
	int noContinousKillCount = 0;
	CTime cTime;
	CString tmp;
	int start = 0;
	int end = 0;
	int time_data=0;

	CLineageMObserverDlg *pDlg = (CLineageMObserverDlg*)AfxGetApp()->m_pMainWnd;

	while(true) {
/*		pDlg->doCapture(TYPE_ZONE);
		pDlg->doCapture(TYPE_HP);
		pDlg->doCapture(TYPE_HP_BAR);
		pDlg->doCapture(TYPE_KILL);
		pDlg->doCapture(TYPE_PK);*/

		pDlg->doCapture(TYPE_FULL);

		if (pDlg->_getZoneType() != SAFETY_ZONE) { // NORMAL ZONE
			if (pDlg->processImageComp(TYPE_PK) || pDlg->processImageComp(TYPE_HP) || pDlg->processImageComp(TYPE_HP_BAR)) {
				cTime = CTime::GetCurrentTime();
				tmp.Format(_T("[%02d:%02d:%02d] 마을로 워프!!! 이동사유: %s"), cTime.GetHour(), cTime.GetMinute(), cTime.GetSecond(), pDlg->typetoString(pDlg->m_ReasonType));
				pDlg->m_ListLog.SetCurSel(pDlg->m_ListLog.InsertString(-1, tmp, RGB(255,0,0)));
				pDlg->doWarf();

				pDlg->teleBot.sendMessage(_T("마을로 이동... 이동사유 : ") + pDlg->typetoString(pDlg->m_ReasonType));

				if (pDlg->m_ReasonType == TYPE_PK) {
					pDlg->teleBot.sendPhoto(pDlg->m_IconInfo[TYPE_FULL].name);
				}
			}

			end = GetTickCount();
			time_data = end - start;

			if (!pDlg->processImageComp(TYPE_KILL)) {
				if (30 < noContinousKillCount++ && time_data > pDlg->m_TelegramTime*1000) { // 60s
					pDlg->teleBot.sendMessage(_T("사냥을 못하고 있는 듯"));
					start = GetTickCount();
					sTeleSendingCnt = 0;
				}
			}
			else {
				noContinousKillCount = 0; // 사냥 중
				sTeleSendingCnt = 0;
			}
		}
		else {
			noContinousKillCount = 0; // Safety Zone, rset KillCount 0
			sTeleSendingCnt = 0;
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

//	_IconInfo iconInfo = getIconInfo(type);
	GetBitmapFromWindow();

	return false;
}

bool CLineageMObserverDlg::processImageComp(int type)
{	// TODO: 여기에 구현 코드 추가.
	bool nValue;

	m_MapType.Lookup(type, nValue);
	if (!nValue) {
		return false;
	}

	m_ReasonType = type;

	Mat src, arti;
	MatND hist1, hist2;
	double res = 0;
	int lut_th = 0;

	(type == TYPE_PK) ? lut_th = 200 : lut_th = 150;

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

	src = m_IconInfo[type].src;

	switch (type) {
		case TYPE_HP:
			tmpHist = RefHpHist;
			break;
		case TYPE_HP_BAR:
			tmpHist = RefHpBarHist;
			break;
		case TYPE_PK:
			break;
		case TYPE_KILL:
			tmpHist = RefKillHist;
			break;

		default:
			break;
	}

	// 5픽셀 : 상태 체크

	if (m_IconInfo[type].isLutEnable) {
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
			LUT(src, lut, src);

			int count = 0;
			int height = src.rows;
			int width = src.cols;

			uchar *data_input = src.data;

			if (data_input != NULL) {
				if (src.channels() == 3) {
					for (int y = 0; y < height; y++) {
						for (int x = 0; x < width; x++) {

							uchar b = data_input[y*width * 3 + x * 3 + 0]; // 124 / 30
							uchar g = data_input[y*width * 3 + x * 3 + 1];
							uchar r = data_input[y*width * 3 + x * 3 + 2];

							if (r > 200) {
								count++;
							}
						}
					}
					if (count > 45) {
						teleBot.sendMessage(TEXT("PK..."));
						
						CTime cTime = CTime::GetCurrentTime();
						m_IconInfo[TYPE_FULL].name.Format(_T(".\\pklist\\%04d%02d%02d_%02d%02d%02d_PK.jpg"), cTime.GetYear(), cTime.GetMonth(), cTime.GetDay(), cTime.GetHour(), cTime.GetMinute(), cTime.GetSecond());

						Utils::SaveFromHBitmapToJpg(Utils::Mat2HBITMAP(m_IconInfo[TYPE_FULL].src), m_IconInfo[TYPE_FULL].name);

						printLog(type, 0, 0);
						return true;
					}
				}
			}
		}
		return false;
	}

	printLog(type, (int)(100 - (res * 100)), m_IconInfo[type].threshold);

	if (100 - (res * 100) > m_IconInfo[type].threshold) {
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
	COLORREF rgb = RGB(255, 0, 0);
	CTime cTime;
	CString tmp;
	cTime = CTime::GetCurrentTime();

	if(type == TYPE_ZONE /*type == TYPE_HP || type == TYPE_ZONE || type || TYPE_KILL*/) {
		tmp.Format(_T("[%02d:%02d:%02d] 타입:%s, 유사도:%3d%%, 기준:%3d%%"), cTime.GetHour(), cTime.GetMinute(), cTime.GetSecond(), typetoString(type), cur, THRESHOLD);
		rgb = RGB(0, 0, 0);
	}
	else if(type == TYPE_HP_BAR) {
		tmp.Format(_T("[%02d:%02d:%02d] 타입:%s, HP가 30%% 미만..."), cTime.GetHour(), cTime.GetMinute(), cTime.GetSecond(), typetoString(type));
	}
	else if (type == TYPE_PK) {
		tmp.Format(_T("[%02d:%02d:%02d] 타입:%s, PK 당하는 중인 듯?"), cTime.GetHour(), cTime.GetMinute(), cTime.GetSecond(), typetoString(type));
	}
	else {
		return;
	}


	if (!isSamePrevResult(type, cur)) {
		idx = m_ListLog.InsertString(-1, tmp, rgb);
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
			return _T("HP포션   ");
		case TYPE_HP_BAR:
			return _T("HP_BAR");
		case TYPE_PK:
			return _T("PK       ");
		case TYPE_KILL:
			return _T("KILL     ");
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
	m_MapType.SetAt(TYPE_KILL, true);

	// TODO: 여기에 구현 코드 추가.
}

void CLineageMObserverDlg::OnBnClickedCheckPk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	reloadMap();
}


void CLineageMObserverDlg::OnBnClickedCheckHp30()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	reloadMap();
}


void CLineageMObserverDlg::OnBnClickedCheckHp()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	reloadMap();
}


void CLineageMObserverDlg::OnBnClickedBtnRegToken()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	
	RegistrationTokenDlg modal;
	modal.setToken(m_strTelegramToken);
	if (IDOK == modal.DoModal()) {
		m_strTelegramToken = modal.getToken();
		AfxMessageBox(m_strTelegramToken);
	}
}

void CLineageMObserverDlg::OnBnClickedBtnObserve()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_bCaptureReady) {
		if (pCaptureThread != NULL) {
			pCaptureThread->SuspendThread();
			pCaptureThread = NULL;

			GetDlgItem(IDC_BTN_OBSERVE)->SetWindowText(_T("감지 시작"));
		}
		else {
			pCaptureThread = AfxBeginThread(CaptureThread, this);
			pCaptureThread = NULL;
			GetDlgItem(IDC_BTN_OBSERVE)->SetWindowText(_T("감지 중"));
		}
	}
	else {
		AfxMessageBox(_T("화면 찾기부터 먼저 하세요."));
	}



}


void CLineageMObserverDlg::OnEnChangeEditLocation()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialogEx::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CLineageMObserverDlg::loadOpenCVResource()
{
	// TODO: 여기에 구현 코드 추가.
	// LUT
	int dim(256); // 영상 역전 테이블 생성
	cv::Mat lut(1, &dim, CV_8U);
	// 1은 1차원, &dim은 256 항목, CV_8U는 uchar
	for (int i = 0; i < 256; i++) {
		if (i < 150) {
			lut.at<uchar>(i) = 0;
		}
		else {
			lut.at<uchar>(i) = i;
		}
	}

	RefHpHsv = imread(FILENAME_REF_HP0);
	RefKillHsv = imread(FILENAME_REF_KILL);

	LUT(RefHpHsv, lut, RefHpHsv);
	calcHist(&RefHpHsv, 1, channels, Mat(), /* do not use mask*/   RefHpHist, 2, histSize, ranges);

	LUT(RefKillHsv, lut, RefKillHsv);
	calcHist(&RefKillHsv, 1, channels, Mat(), /* do not use mask*/   RefKillHist, 2, histSize, ranges);

	// ZONE LIST 0:NORMAL, 1:SAFETY, 2:COMBAT
	for (int idx = 0; idx < MAX_ZONE_LIST; idx++) {
		zoneImg[idx] = imread(kZoneList[idx]);
		calcHist(&zoneImg[idx], 1, channels, Mat(), /* do not use mask*/ zoneHist[idx], 2, histSize, ranges);
	}
}


int CLineageMObserverDlg::_getZoneType()
{
	Mat z;
	MatND zHist;
	double res=0.0;
	int bestAcc=0, bestAccIdx=0;

	z = m_IconInfo[TYPE_ZONE].src;

	calcHist(&z, 1, channels, Mat(), /* do not use mask*/ zHist, 2, histSize, ranges);
	
	for (int idx = 0; idx < MAX_ZONE_LIST; idx++) {
		res = compareHist(zHist, zoneHist[idx], CV_COMP_BHATTACHARYYA); // 차이점 0부터

		int tmpAcc = (int)(100 - (res * 100));

		if (bestAcc < tmpAcc) {
			bestAcc = tmpAcc;
			bestAccIdx = idx;
		}
	}

	if (bestAcc < 20) {
		bestAccIdx = m_LastZoneTypeIdx;
	}

	GetDlgItem(IDC_EDIT_ZONE)->SetWindowTextW(_getZoneString(bestAccIdx));

	// TODO: 여기에 구현 코드 추가.
	m_LastZoneTypeIdx = bestAccIdx;

	return bestAccIdx;
}


CString CLineageMObserverDlg::_getZoneString(int type)
{
	// TODO: 여기에 구현 코드 추가.
	switch (type) {
		case NORMAL_ZONE:
			return _T("NORMAL");
		case COMBAT_ZONE:
			return _T("COMBAT");
		case SAFETY_ZONE:
			return _T("SAFETY");

		default:
			break;
	}

	return _T("UNKNOWN");
}


void CLineageMObserverDlg::loadTelegramTokenFromFile()
{
	// TODO: 여기에 구현 코드 추가.
	CString strToken;
	CStdioFile file;
	bool bSuccess = file.Open(_T(".\\token.txt"), CFile::modeRead | CFile::typeUnicode);

	if (bSuccess) {
		if (file != NULL) {
			file.ReadString(strToken);
			file.Close();

			m_strTelegramToken = strToken;
		}
	}
}


void CLineageMObserverDlg::setToken(CString token)
{
	// TODO: 여기에 구현 코드 추가.
}


void CLineageMObserverDlg::OnCbnSelchangeComboTeletime()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int comboCnt = m_Combo_TeleTime.GetCurSel();
	int value = atoi(kTeleTimeList[comboCnt]);
	

	switch (comboCnt) {
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
			m_TelegramTime = value;
			break;
		case 5:
		case 6:
			m_TelegramTime = value * 60;
			break;

		default:
			break;
	}

	CString tmp;
	tmp.Format(_T("%d"), m_TelegramTime);
	AfxMessageBox(tmp);
}


MatND CLineageMObserverDlg::getReferenceHist(int type)
{
	// TODO: 여기에 구현 코드 추가.
	return MatND();
}


_IconInfo CLineageMObserverDlg::getIconInfo(int type)
{
	// TODO: 여기에 구현 코드 추가.
	return m_IconInfo[type];
}


void CLineageMObserverDlg::loadIconInfo() {

	m_IconInfo[TYPE_ZONE].w = ZONE_ICON_SIZE_W;
	m_IconInfo[TYPE_ZONE].h = ZONE_ICON_SIZE_H;
	m_IconInfo[TYPE_ZONE].x = ZONE_ICON_SIZE_X;
	m_IconInfo[TYPE_ZONE].y = ZONE_ICON_SIZE_Y;
	m_IconInfo[TYPE_ZONE].isSave = false;
	m_IconInfo[TYPE_ZONE].isLutEnable = true;
	m_IconInfo[TYPE_ZONE].threshold = ZONE_THRESHOLD;
	m_IconInfo[TYPE_ZONE].name.Format(_T(FILENAME_ZONE));
			
	m_IconInfo[TYPE_HP].w = HP_ICON_SIZE_W;
	m_IconInfo[TYPE_HP].h = HP_ICON_SIZE_H;
	m_IconInfo[TYPE_HP].x = HP_ICON_SIZE_X;
	m_IconInfo[TYPE_HP].y = HP_ICON_SIZE_Y;
	m_IconInfo[TYPE_HP].isSave = true;
	m_IconInfo[TYPE_HP].isLutEnable = true;
	m_IconInfo[TYPE_HP].threshold = HP_ICON_THRESHOLD;
	m_IconInfo[TYPE_HP].name.Format(_T(FILENAME_HP));

	m_IconInfo[TYPE_PK].w = PK_ICON_SIZE_W;
	m_IconInfo[TYPE_PK].h = PK_ICON_SIZE_H;
	m_IconInfo[TYPE_PK].x = PK_ICON_SIZE_X;
	m_IconInfo[TYPE_PK].y = PK_ICON_SIZE_Y;
	m_IconInfo[TYPE_PK].isSave = false;
	m_IconInfo[TYPE_PK].isLutEnable = false;
	m_IconInfo[TYPE_PK].threshold = PK_ICON_THRESHOLD;
	m_IconInfo[TYPE_PK].name.Format(_T(FILENAME_PK));

	m_IconInfo[TYPE_HP_BAR].w = HP_BAR_SIZE_W;
	m_IconInfo[TYPE_HP_BAR].h = HP_BAR_SIZE_H;
	m_IconInfo[TYPE_HP_BAR].x = HP_BAR_SIZE_X;
	m_IconInfo[TYPE_HP_BAR].y = HP_BAR_SIZE_Y;
	m_IconInfo[TYPE_HP_BAR].isSave = false;
	m_IconInfo[TYPE_HP_BAR].isLutEnable = false;
	m_IconInfo[TYPE_HP_BAR].threshold = HP_BAR_THRESHOLD;
	m_IconInfo[TYPE_HP_BAR].name.Format(_T(FILENAME_HP_BAR));

	m_IconInfo[TYPE_FULL].w = 800;
	m_IconInfo[TYPE_FULL].h = 480;
	m_IconInfo[TYPE_FULL].x = 0;
	m_IconInfo[TYPE_FULL].y = 0;
	m_IconInfo[TYPE_FULL].isSave = true;
	m_IconInfo[TYPE_FULL].isLutEnable = true;
	m_IconInfo[TYPE_FULL].name.Format(_T("default.bmp"));

	m_IconInfo[TYPE_KILL].w = KILL_POS_W;
	m_IconInfo[TYPE_KILL].h = KILL_POS_H;
	m_IconInfo[TYPE_KILL].x = KILL_POS_X;
	m_IconInfo[TYPE_KILL].y = KILL_POS_Y;
	m_IconInfo[TYPE_KILL].isSave = false;
	m_IconInfo[TYPE_KILL].isLutEnable = true;
	m_IconInfo[TYPE_KILL].threshold = KILL_THRESHOLD;
	m_IconInfo[TYPE_KILL].name.Format(_T(FILENAME_KILL));
}