#pragma once


// Capture 대화 상자

class Capture : public CDialogEx
{
	DECLARE_DYNAMIC(Capture)

public:
	Capture(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~Capture();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_CAPTURE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
};
