#include "stdafx.h"
#include "Utils.h"


Utils::Utils()
{
}


Utils::~Utils()
{
}


bool Utils::executeShell(const wchar_t *file, const wchar_t *parm)
{
	// TODO: ���⿡ ���� �ڵ� �߰�.
	SHELLEXECUTEINFO execinfo;
	DWORD ExitCode;

	// ������ ���� ����ü ��Ʈ
	ZeroMemory(&execinfo, sizeof(execinfo));
	execinfo.cbSize = sizeof(execinfo);
	execinfo.lpVerb = _T("open");
	execinfo.lpFile = file;
	execinfo.lpParameters = parm;
	execinfo.fMask = SEE_MASK_FLAG_NO_UI | SEE_MASK_NOCLOSEPROCESS;
	execinfo.nShow = SW_HIDE;

	if (ShellExecuteEx(&execinfo) == TRUE) {
		do {
			GetExitCodeProcess(execinfo.hProcess, &ExitCode);
			Sleep(100);
		} while (ExitCode == STILL_ACTIVE);

		return true;
	}
	return false;

	return false;
}
