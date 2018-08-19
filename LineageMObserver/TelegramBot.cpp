#include "stdafx.h"
#include "TelegramBot.h"
#include "Utils.h"

#include <Wininet.h>
#pragma comment(lib, "wininet.lib")

#define CURL_PATH ".\\bin\\curl.exe"

TelegramBot::TelegramBot()
{
	strUser = TEXT("544284998");
	strToken = TEXT("641687142:AAHh_PiL27onti0AnocDzdjIzf3oBEU9Ofc");
}

TelegramBot::~TelegramBot()
{
}

int TelegramBot::sendMessage(CString msg)
{
	// TODO: ���⿡ ���� �ڵ� �߰�.
	CString strSMS = msg;
	CString strURL;
	CString strSMS_UTF8;
	strSMS.Format(_T("[%s] %s"), strPrefix, strSMS);

	_AsciiToUTF8(strSMS, strSMS_UTF8);
	strURL.Format(TEXT("https://api.telegram.org/bot%s/sendMessage?chat_id=%s&text=%s"), strToken, strUser, strSMS_UTF8);

	HINTERNET  hInet = InternetOpen(TEXT("HTTP-POST"), INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
	if (hInet == NULL)
	{
		AfxMessageBox(TEXT("InternetOpen() ȣ�� ����!!!"));
		return -1;
	}

	HINTERNET hurl = InternetOpenUrl(hInet, strURL, NULL, 0, 0, 0);

	if (hurl == NULL)
		AfxMessageBox(TEXT("�ڷ��׷� �� ���� ����!!!"));
	InternetCloseHandle(hInet);

	return 0;
}


void TelegramBot::_AsciiToUTF8(CString parm_ascii_string, CString &parm_utf8_string)
{
	// TODO: ���⿡ ���� �ڵ� �߰�.
	parm_utf8_string.Empty();

	int temp_length;
	BSTR unicode_str;

#ifdef _UNICODE
	unicode_str = parm_ascii_string.AllocSysString();
	temp_length = WideCharToMultiByte(CP_UTF8, 0, unicode_str, -1, NULL, 0, NULL, NULL);
#else

	// �ƽ�Ű �ڵ带 UTF8������ �ڵ�� ��ȯ�ؾ� �Ѵ�. �ƽ�Ű �ڵ带 UTF8 �ڵ�� ��ȯ�Ҷ��� 
	// �ƽ�Ű �ڵ带 �����ڵ�� ���� ��ȯ�ϰ� ��ȯ�� �����ڵ带 UTF8 �ڵ�� ��ȯ�ؾ� �Ѵ�.

	// �ƽ�Ű �ڵ�ε� ���ڿ��� �����ڵ�ȭ ������ ���� ���̸� ���Ѵ�.
	temp_length = MultiByteToWideChar(CP_ACP, 0, (char *)(const char *)parm_ascii_string, -1, NULL, 0);


	// ��ȯ�� �����ڵ带 ������ ������ �Ҵ��Ѵ�.
	unicode_str = SysAllocStringLen(NULL, temp_length + 1);

	// �ƽ�Ű �ڵ�ε� ���ڿ��� ���� �ڵ� ������ ���ڿ��� �����Ѵ�.
	MultiByteToWideChar(CP_ACP, 0, (char *)(const char *)parm_ascii_string, -1, unicode_str, temp_length);

	// �����ڵ� ������ ���ڿ��� UTF8 �������� ���������� �ʿ��� �޸� ������ ũ�⸦ ��´�.
	temp_length = WideCharToMultiByte(CP_UTF8, 0, unicode_str, -1, NULL, 0, NULL, NULL);

#endif // _UNICODE


	if (temp_length > 0) {
		CString str;


		// UTF8 �ڵ带 ������ �޸� ������ �Ҵ��Ѵ�.
		char *p_utf8_string = new char[temp_length];
		memset(p_utf8_string, 0, temp_length);
		// �����ڵ带 UTF8�ڵ�� ��ȯ�Ѵ�.
		WideCharToMultiByte(CP_UTF8, 0, unicode_str, -1, p_utf8_string, temp_length, NULL, NULL);

		// UTF8 �������� ����� ���ڿ��� �� ������ �ڵ尪���� �� URL�� ���Ǵ� �������� ��ȯ�Ѵ�.
		for (int i = 0; i < temp_length - 1; i++) {
			if (p_utf8_string[i] & 0x80) {
				// ���� �ڵ尡 �ѱ��� ���..
				// UTF8 �ڵ�� ǥ���� �ѱ��� 3����Ʈ�� ǥ�õȴ�. "�ѱ�"  ->  %ED%95%9C%EA%B8%80
				for (int sub_i = 0; sub_i < 3; sub_i++) {
					str.Format(TEXT("%%%X"), p_utf8_string[i] & 0x00FF);
					parm_utf8_string += str;
					i++;
				}

				i--;
			}
			else {
				// ���� �ڵ尡 ������ ���, ������� �״�� ����Ѵ�.
				parm_utf8_string += p_utf8_string[i];
			}
		}

		delete[] p_utf8_string;
	}

	// �����ڵ� ������ ���ڿ��� �����ϱ� ���� �����ߴ� �޸𸮸� �����Ѵ�.
	SysFreeString(unicode_str);
}


bool TelegramBot::sendPhoto(CString filename)
{
	// TODO: ���⿡ ���� �ڵ� �߰�.
	CString strArg;

	//tmp.Format(_T(" -k -F chat_id=544284998 -F photo=@" + m_LatestPkFileName + " https://api.telegram.org/bot641687142:AAHh_PiL27onti0AnocDzdjIzf3oBEU9Ofc/sendPhoto"));

	strArg.Format(TEXT(" -k -F chat_id=%s -F photo=@\"%s\" https://api.telegram.org/bot%s/sendPhoto"), strUser, filename, strToken);
	Utils utils;
	utils.executeShell(_T(CURL_PATH), strArg);

	return true;
}


void TelegramBot::setPrefixString(CString prefix)
{
	// TODO: ���⿡ ���� �ڵ� �߰�.
	strPrefix = prefix;
}
