#pragma once
class TelegramBot
{
public:
	TelegramBot();
	TelegramBot(CString userID, CString token);
	~TelegramBot();
	int sendMessage(CString msg);
	static void _AsciiToUTF8(CString parm_ascii_string, CString &parm_utf8_string);
	bool sendPhoto(CString filename);

	CString strUser;
	CString strToken;
	CString strPrefix;
	void setPrefixString(CString prefix);

	void setToken(CString token);
	void setUserID(CString userid);
};

