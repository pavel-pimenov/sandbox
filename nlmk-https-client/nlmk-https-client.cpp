#pragma hdrstop
#pragma argsused

#ifdef _WIN32
#include <tchar.h>
#else
  typedef char _TCHAR;
  #define _tmain main
#endif

#include <stdio.h>

//======================================================================
#include <windows.h>
#include <wininet.h>
#include <cassert>
#include <stdint.h>

#include <string>
#include <vector>

using std::string;
using std::vector;
using std::wstring;

#pragma comment (lib, "Wininet.lib")

//#define dcassert(exp) assert(exp)
//======================================================================
static string toString(int val)
{
	char buf[32];
	_snprintf(buf, sizeof(buf), "%d", val);
	return buf;
}

//======================================================================
DWORD g_last_error_code;
//======================================================================
string translateError(DWORD aError)
{
   return string("Error code = ") + toString(aError);
/*	switch (aError)
	{
			DWORD l_formatMessageFlag =
			    FORMAT_MESSAGE_ALLOCATE_BUFFER |
			    FORMAT_MESSAGE_FROM_SYSTEM |
			    FORMAT_MESSAGE_IGNORE_INSERTS;

			LPCVOID lpSource = NULL;
			// Обработаем расширенные ошибки по инету
			// http://stackoverflow.com/questions/20435591/internetgetlastresponseinfo-returns-strange-characters-instead-of-error-message
			{
				wstring l_error;
				DWORD dwLen = 0;
				DWORD dwErr = aError;
				if (dwErr == ERROR_INTERNET_EXTENDED_ERROR)
				{
					InternetGetLastResponseInfo(&dwErr, NULL, &dwLen); //
					if (::GetLastError() == ERROR_INSUFFICIENT_BUFFER && dwLen)
					{
						dwLen++;
						assert(dwLen);
						if (dwLen)
						{
							l_error.resize(dwLen);
							InternetGetLastResponseInfoW(&dwErr, &l_error[0], &dwLen);
						}
					}
					if (dwLen)
					{
						return "Internet Error = " + Text::fromT(l_error) + " [Code = " + Util::toString(dwErr) + "]";
					}
				}
			}
			// http://stackoverflow.com/questions/2159458/why-is-formatmessage-failing-to-find-a-message-for-wininet-errors/2159488#2159488
			if (aError >= INTERNET_ERROR_BASE && aError < INTERNET_ERROR_LAST)
			{
				l_formatMessageFlag |= FORMAT_MESSAGE_FROM_HMODULE;
				lpSource = GetModuleHandle(_T("wininet.dll"));
			}

			LPTSTR lpMsgBuf = 0;
			DWORD chars = FormatMessage(
							  l_formatMessageFlag,
							  lpSource,
			                  aError,
#if defined (_CONSOLE) || defined (_DEBUG)
			                  MAKELANGID(LANG_NEUTRAL, SUBLANG_ENGLISH_US), // US
#else
			                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
#endif
							  (LPTSTR) &lpMsgBuf,
			                  0,
			                  NULL
			              );
			string tmp;
			if (chars != 0)
			{
				tmp = Text::fromT(lpMsgBuf);
				// Free the buffer.
				LocalFree(lpMsgBuf);
				string::size_type i = 0;

				while ((i = tmp.find_first_of("\r\n", i)) != string::npos)
				{
					tmp.erase(i, 1);
				}
			}
			tmp += "[error: " + toString(aError) + "]";
#if 0 // TODO
			if (aError >= WSAEADDRNOTAVAIL && aError <= WSAEHOSTDOWN)
			{
				tmp += "\r\n\t" + STRING(SOCKET_ERROR_NOTE) + " " + Util::getWikiLink() + "socketerror#error_" + toString(aError);  // as  LANG:socketerror#error_10060
			}
#endif
			return tmp;
#else // _WIN32
	return Text::toUtf8(strerror(aError));
#endif // _WIN32
	}
*/
}
//======================================================================
class CInternetHandle
{
	public:
		explicit CInternetHandle(HINTERNET p_hInternet): m_hInternet(p_hInternet)
		{
		}
		~CInternetHandle()
		{
			if (m_hInternet)
			{
				BOOL l_res = ::InternetCloseHandle(m_hInternet);
				assert(l_res);
			}
		}
		operator const HINTERNET() const
		{
			return m_hInternet;
		}
	protected:
		const HINTERNET m_hInternet;
};
//======================================================================
class CFlyHTTPDownloader
{
		DWORD m_inet_flag;
		DWORD m_last_error_code;
		string m_error_message;
		bool switchMirrorURL(string& p_url, int p_mirror);
	public:
		static int g_last_stable_mirror;
		static void nextMirror();
		bool m_is_add_url;
		bool m_is_use_cache;
		string m_user_agent;
		CFlyHTTPDownloader() : m_inet_flag(0), m_last_error_code(0), m_is_add_url(true), m_is_use_cache(false)
		{
		}
		std::vector<string> m_get_http_header_item;
		uint64_t getBinaryDataFromInetSafe(const string& p_url, std::vector<unsigned char>& p_dataOut, LONG p_time_out = 0);
		uint64_t getBinaryDataFromInet(const string& p_url, std::vector<unsigned char>& p_dataOut, LONG p_time_out = 0);
		struct CFlyUrlItem
		{
			string m_url;
			std::vector<unsigned char> p_body;
		};
		typedef std::vector<CFlyUrlItem> CFlyUrlItemArray;

		uint64_t getBinaryDataFromInetArray(CFlyUrlItemArray& p_url_array, LONG p_time_out = 0);
		void clear()
		{
			m_get_http_header_item.clear();
		}
		void setInetFlag(DWORD p_inet_flag)
		{
			m_inet_flag = p_inet_flag;
		}
		const string& getErroMessage() const
		{
			return m_error_message;
		}
		DWORD getLastErrorCode() const
		{
			return m_last_error_code;
		}
		void create_error_message(const char* p_type, const string& p_url);
};
//======================================================================
string postQuery(const char* p_url,
				 unsigned short p_port,
				 const char* p_type,
				 const char* p_query,
				 const string& p_body,
				 bool& p_is_send,
				 bool& p_is_error,
				 DWORD p_time_out = 0)
{
	p_is_send = false;
	p_is_error = false;
	assert(!p_body.empty());
	const string l_log_marker;
	string l_reason;
	string l_result_query;
	std::vector<uint8_t> l_post_compress_query;
	string l_log_string;
// Передача
	CInternetHandle hSession(InternetOpen("nlmk-https-client", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0));
	DWORD l_timeOut = 0;
	if (l_timeOut < 500)
		l_timeOut = 1000;
	if (!InternetSetOption(hSession, INTERNET_OPTION_CONNECT_TIMEOUT, &l_timeOut, sizeof(l_timeOut)))
	{
		g_last_error_code = GetLastError();
		printf("%s","Error InternetSetOption INTERNET_OPTION_CONNECT_TIMEOUT: " + translateError(g_last_error_code));
		p_is_error = true;
	}
	//InternetSetOption(hSession, INTERNET_OPTION_RECEIVE_TIMEOUT, &CFlyServerConfig::g_winet_receive_timeout, sizeof(CFlyServerConfig::g_winet_receive_timeout));
	//InternetSetOption(hSession, INTERNET_OPTION_SEND_TIMEOUT, &CFlyServerConfig::g_winet_send_timeout, sizeof(CFlyServerConfig::g_winet_send_timeout));
	if (hSession)
	{
		DWORD dwFlags = INTERNET_FLAG_SECURE; // INTERNET_FLAG_SECURE | INTERNET_FLAG_IGNORE_CERT_CN_INVALID | INTERNET_FLAG_IGNORE_CERT_DATE_INVALID
		CInternetHandle hConnect(InternetConnectA(hSession, p_url, p_port, NULL, NULL, INTERNET_SERVICE_HTTP, dwFlags, NULL));
		if (hConnect)
		{
			CInternetHandle hRequest(HttpOpenRequestA(hConnect, p_type, p_query , NULL, NULL, NULL /*g_accept*/, dwFlags, NULL));
			if (hRequest)
			{
				string l_fly_header;
				if (HttpSendRequestA(hRequest,
									 l_fly_header.length() ? l_fly_header.c_str() : NULL,
				                     l_fly_header.length(),
									 LPVOID(p_body.data()),
									 p_body.size()))
				{
					DWORD l_dwBytesAvailable = 0;
					std::vector<char> l_zlib_blob;
					std::vector<unsigned char> l_MessageBody;
					while (InternetQueryDataAvailable(hRequest, &l_dwBytesAvailable, 0, 0))
					{
						if (l_dwBytesAvailable == 0)
							break;
#ifdef _DEBUG
						printf("InternetQueryDataAvailable dwBytesAvailable = %d\n",l_dwBytesAvailable);
#endif
						l_MessageBody.resize(l_dwBytesAvailable + 1);
						DWORD dwBytesRead = 0;
						const BOOL bResult = InternetReadFile(hRequest, &l_MessageBody[0], l_dwBytesAvailable, &dwBytesRead);
						if (!bResult)
						{
							g_last_error_code = GetLastError();
							printf("InternetReadFile error %s\n", translateError(g_last_error_code).c_str());
							break;
						}
						if (dwBytesRead == 0)
							break;
						l_MessageBody[dwBytesRead] = 0;
						const auto l_cur_size = l_zlib_blob.size();
						l_zlib_blob.resize(l_cur_size + dwBytesRead);
						memcpy(&l_zlib_blob[0] + l_cur_size, &l_MessageBody[0], dwBytesRead);
					}
					// TODO обобщить в утилитную функцию и подменить в области DHT с другим стартовым коэф-центом. для уменьшения кол-ва релокаций
					// TODO коэф. можно высчитывать динамические и не делалать 10-кой
					if (l_zlib_blob.size())
					{
							const auto l_cur_size = l_zlib_blob.size();
							l_zlib_blob.resize(l_cur_size + 1);
							l_zlib_blob[l_cur_size] = 0;
							l_result_query = (const char*) &l_zlib_blob[0];
							//l_decompress.resize(l_zlib_blob.size());
							//memcpy(l_decompress.data(), l_zlib_blob.data(), l_decompress.size());
					}
					p_is_send = true;
#ifdef _DEBUG
					printf("InternetReadFile Ok! size = %d\n", l_result_query.size());
#endif
				}
				else
				{
					g_last_error_code = GetLastError();
					printf("HttpSendRequest error %s\n", translateError(g_last_error_code).c_str());
					p_is_error = true;
				}
			}
			else
			{
				g_last_error_code = GetLastError();
				printf("HttpOpenRequest error %s\n", translateError(g_last_error_code).c_str());
				p_is_error = true;
			}
		}
		else
		{
			g_last_error_code = GetLastError();
			printf("InternetConnect error %s\n", + translateError(g_last_error_code).c_str());
			p_is_error = true;
		}
	}
	else
	{
		g_last_error_code = GetLastError();
		printf("InternetOpen error %s\n", translateError(g_last_error_code).c_str());
		p_is_error = true;
	}
	return l_result_query;
}
//======================================================================


 int _tmain(int argc, _TCHAR* argv[]) 
{
  bool l_is_send;
  bool l_is_error;
  string l_result = postQuery("yandex.ru",
				 443,
				 "POST",
				 "test-query",
				 "test-body",
				 l_is_send,
				 l_is_error);
	printf("l_result = %s",l_result.c_str());
	return 0;
}
