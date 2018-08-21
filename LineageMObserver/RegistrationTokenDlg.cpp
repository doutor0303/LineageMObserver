// RegistrationTokenDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "LineageMObserver.h"
#include "RegistrationTokenDlg.h"
#include "afxdialogex.h"


// RegistrationTokenDlg 대화 상자

IMPLEMENT_DYNAMIC(RegistrationTokenDlg, CDialogEx)

RegistrationTokenDlg::RegistrationTokenDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_TOKEN, pParent)
{

}

RegistrationTokenDlg::~RegistrationTokenDlg()
{
}

void RegistrationTokenDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(RegistrationTokenDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &RegistrationTokenDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// RegistrationTokenDlg 메시지 처리기


void RegistrationTokenDlg::setToken(CString strToken)
{
	// TODO: 여기에 구현 코드 추가.
	m_strToken = strToken;
}


CString RegistrationTokenDlg::getToken()
{
	// TODO: 여기에 구현 코드 추가.
	return m_strToken;
}


void RegistrationTokenDlg::OnBnClickedOk()
{
	CString strToken;
	GetDlgItem(IDC_EDIT_TOKEN)->GetWindowTextW(strToken);
	setToken(strToken);

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnOK();
}
