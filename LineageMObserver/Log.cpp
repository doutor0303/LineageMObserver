// Log.cpp: 구현 파일
//

#include "stdafx.h"
#include "LineageMObserver.h"
#include "Log.h"
#include "afxdialogex.h"


// Log 대화 상자

IMPLEMENT_DYNAMIC(Log, CDialogEx)

Log::Log(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TAB_LOG, pParent)
{

}

Log::~Log()
{
}

void Log::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(Log, CDialogEx)
END_MESSAGE_MAP()


// Log 메시지 처리기
