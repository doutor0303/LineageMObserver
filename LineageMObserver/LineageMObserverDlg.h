
// LineageMObserverDlg.h: 헤더 파일
//

#pragma once

#include "Capture.h"
#include "Log.h"
#include "TelegramBot.h"
#include "Utils.h"
#include "ColorListBox.h"

struct _IconInfo {
	unsigned int w;
	unsigned int h;
	unsigned int x;
	unsigned int y;
	unsigned int threshold;
	unsigned int type;
	unsigned int lut;
	bool isSave;
	bool isLutEnable;

	CString name;
	Mat src;
};

enum {
	TYPE_ZONE = 0,
	TYPE_HP = 1,
	TYPE_PK = 2,
	TYPE_HP_BAR = 3,
	TYPE_FULL = 4,
	TYPE_KILL = 5,
	TYPE_MAX = 6
};

// CLineageMObserverDlg 대화 상자
class CLineageMObserverDlg : public CDialogEx
{
// 생성입니다.
public:
	CLineageMObserverDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

	Capture m_Capture;
	Log m_Log;
	CWnd* m_pWndShow;

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LINEAGEMOBSERVER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	CWnd* m_hMainWnd;
	CWnd* m_hRenderWnd;

public:
	afx_msg void OnBnClickedBtnFindWindow();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedBtnTest();
	afx_msg void OnBnClickedCheckPk();
	afx_msg void OnBnClickedCheckHp30();
	afx_msg void OnBnClickedCheckHp();
	afx_msg void OnBnClickedBtnRegToken();
	afx_msg void OnBnClickedBtnObserve();
	afx_msg void OnCbnSelchangeComboTeletime();
	afx_msg void OnEnChangeEditLocation();
	
	int TestOpenCV();
	int doWarf();
	int getCharacterStatus(int r, int g, int b);

	bool doCapture(int type);
	bool processImageComp(int type);
	bool isSamePrevResult(int curType, int curAcc);
	void loadIconInfo();
	void GetBitmapFromWindow(/*_IconInfo* iconInfo*/);
	void printLog(int type, int cur, const int THRESHOLD);
	void reloadMap();
	void loadOpenCVResource();
	void loadTelegramTokenFromFile();
	void setToken(CString token);

	CString typetoString(int type);

	bool m_bNoramlZone;
	bool m_bCaptureReady;

	int m_prevType;
	int m_prevAcc;
	int m_ReasonType;
	int m_LastZoneTypeIdx;
	int m_TelegramTime;

	CEdit m_EditTime;
	CEdit m_EditStatus;
	CEdit mEditTeleTime;
	CButton m_CheckHp30Under;
	CButton m_CheckPlayerKill;
	CButton m_CheckHpPotion;
	CColorListBox m_ListLog;
	CMap<int, int, bool, bool> m_MapType;
	CString m_LatestPkFileName;
	CString m_strTelegramToken;
	CString m_EditTimeValue;
	CComboBox m_Combo_Pk;
	CComboBox m_Combo_TeleTime;
	
	// Custom Class
	TelegramBot teleBot;
	Utils utils;

	_IconInfo m_IconInfo[TYPE_MAX];

	MatND getReferenceHist(int type);

private:
	static UINT CaptureThread(LPVOID _method);
	static UINT PkMonitorThread(LPVOID _method);

	CWinThread *pCaptureThread;
	CWinThread *pPkMonitorThread;

	int _getZoneType();
	CString _getZoneString(int type);
public:
	_IconInfo getIconInfo(int type);
};
