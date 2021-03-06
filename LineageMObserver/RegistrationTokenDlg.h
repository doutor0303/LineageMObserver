#pragma once


// RegistrationTokenDlg 대화 상자

class RegistrationTokenDlg : public CDialogEx
{
	DECLARE_DYNAMIC(RegistrationTokenDlg)

public:
	RegistrationTokenDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~RegistrationTokenDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_TOKEN };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	void setToken(CString strToken);
	CString getToken();

	CString strToken;
	afx_msg void OnBnClickedOk();

	CString m_strToken;
	virtual BOOL OnInitDialog();
};
