#pragma once


// Log 대화 상자

class Log : public CDialogEx
{
	DECLARE_DYNAMIC(Log)

public:
	Log(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~Log();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TAB_LOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
};
