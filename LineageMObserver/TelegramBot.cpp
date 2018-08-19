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
	// TODO: 여기에 구현 코드 추가.
	CString strSMS = msg;
	CString strURL;
	CString strSMS_UTF8;
	strSMS.Format(_T("[%s] %s"), strPrefix, strSMS);

	_AsciiToUTF8(strSMS, strSMS_UTF8);
	strURL.Format(TEXT("https://api.telegram.org/bot%s/sendMessage?chat_id=%s&text=%s"), strToken, strUser, strSMS_UTF8);

	HINTERNET  hInet = InternetOpen(TEXT("HTTP-POST"), INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
	if (hInet == NULL)
	{
		AfxMessageBox(TEXT("InternetOpen() 호출 실패!!!"));
		return -1;
	}

	HINTERNET hurl = InternetOpenUrl(hInet, strURL, NULL, 0, 0, 0);

	if (hurl == NULL)
		AfxMessageBox(TEXT("텔레그램 봇 전송 실패!!!"));
	InternetCloseHandle(hInet);

	return 0;
}


void TelegramBot::_AsciiToUTF8(CString parm_ascii_string, CString &parm_utf8_string)
{
	// TODO: 여기에 구현 코드 추가.
	parm_utf8_string.Empty();

	int temp_length;
	BSTR unicode_str;

#ifdef _UNICODE
	unicode_str = parm_ascii_string.AllocSysString();
	temp_length = WideCharToMultiByte(CP_UTF8, 0, unicode_str, -1, NULL, 0, NULL, NULL);
#else

	// 아스키 코드를 UTF8형식의 코드로 변환해야 한다. 아스키 코드를 UTF8 코드로 변환할때는 
	// 아스키 코드를 유니코드로 먼저 변환하고 변환된 유니코드를 UTF8 코드로 변환해야 한다.

	// 아스키 코드로된 문자열을 유니코드화 시켰을 때의 길이를 구한다.
	temp_length = MultiByteToWideChar(CP_ACP, 0, (char *)(const char *)parm_ascii_string, -1, NULL, 0);


	// 변환된 유니코드를 저장할 공간을 할당한다.
	unicode_str = SysAllocStringLen(NULL, temp_length + 1);

	// 아스키 코드로된 문자열을 유니 코드 형식의 문자열로 변경한다.
	MultiByteToWideChar(CP_ACP, 0, (char *)(const char *)parm_ascii_string, -1, unicode_str, temp_length);

	// 유니코드 형식의 문자열을 UTF8 형식으로 변경했을때 필요한 메모리 공간의 크기를 얻는다.
	temp_length = WideCharToMultiByte(CP_UTF8, 0, unicode_str, -1, NULL, 0, NULL, NULL);

#endif // _UNICODE


	if (temp_length > 0) {
		CString str;


		// UTF8 코드를 저장할 메모리 공간을 할당한다.
		char *p_utf8_string = new char[temp_length];
		memset(p_utf8_string, 0, temp_length);
		// 유니코드를 UTF8코드로 변환한다.
		WideCharToMultiByte(CP_UTF8, 0, unicode_str, -1, p_utf8_string, temp_length, NULL, NULL);

		// UTF8 형식으로 변경된 문자열을 각 문자의 코드값별로 웹 URL에 사용되는 형식으로 변환한다.
		for (int i = 0; i < temp_length - 1; i++) {
			if (p_utf8_string[i] & 0x80) {
				// 현재 코드가 한글인 경우..
				// UTF8 코드로 표현된 한글은 3바이트로 표시된다. "한글"  ->  %ED%95%9C%EA%B8%80
				for (int sub_i = 0; sub_i < 3; sub_i++) {
					str.Format(TEXT("%%%X"), p_utf8_string[i] & 0x00FF);
					parm_utf8_string += str;
					i++;
				}

				i--;
			}
			else {
				// 현재 코드가 영문인 경우, 변경없이 그대로 사용한다.
				parm_utf8_string += p_utf8_string[i];
			}
		}

		delete[] p_utf8_string;
	}

	// 유니코드 형식의 문자열을 저장하기 위해 생성했던 메모리를 삭제한다.
	SysFreeString(unicode_str);
}


bool TelegramBot::sendPhoto(CString filename)
{
	// TODO: 여기에 구현 코드 추가.
	CString strArg;

	//tmp.Format(_T(" -k -F chat_id=544284998 -F photo=@" + m_LatestPkFileName + " https://api.telegram.org/bot641687142:AAHh_PiL27onti0AnocDzdjIzf3oBEU9Ofc/sendPhoto"));

	strArg.Format(TEXT(" -k -F chat_id=%s -F photo=@\"%s\" https://api.telegram.org/bot%s/sendPhoto"), strUser, filename, strToken);
	Utils utils;
	utils.executeShell(_T(CURL_PATH), strArg);

	return true;
}


void TelegramBot::setPrefixString(CString prefix)
{
	// TODO: 여기에 구현 코드 추가.
	strPrefix = prefix;
}
