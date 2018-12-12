#ifndef _STRING_UTILITY_
#define _STRING_UTILITY_

#include <string>
#include <sstream>
#include "error_message.h"

namespace STRING_UTILITY
{
	using namespace ERROR_MESSAGE;

	using std::string;
	using std::wstring;
	using std::wstringstream;
	using std::basic_string;

	template<typename T, typename... Types>
	wstring to_wstring(const T & t, const Types &... args) noexcept
	{
		wstringstream bss;
		wstring result;

		bss << t;
		bss >> result;

		return result + to_wstring(args...);
	}
	template<typename... Types>
	wstring to_wstring(const wstring & wstr, const Types &... args) noexcept
	{
		return wstr + to_wstring(args...);
	}
	template<typename... Types>
	wstring to_wstring(const string & str, const Types &... args) noexcept
	{
		auto required_size = MultiByteToWideChar(CP_ACP, 0, str.c_str(), static_cast<int>(str.size()), nullptr, 0);
		wstring result(required_size, 0);
		if (!MultiByteToWideChar(CP_ACP, 0, str.c_str(), static_cast<int>(str.size()), &result[0], static_cast<int>(result.size())))
		{
			auto error_message = SystemErrorMessageObject().message_string;
			MessageBox(NULL, error_message.c_str(), TEXT("to_wstring: MultiByteToWideChar Error"), MB_OK);
			exit(-1);
		}
		return result + to_wstring(args...);
	}
	template<typename... Types>
	wstring to_wstring(const char * str, const Types &... args) noexcept
	{
		return to_wstring(string(str), args...);
	}
	template<typename... Types>
	wstring to_wstring(char * str, const Types &... args) noexcept
	{
		return to_wstring(const_cast<const char *>(str), args...);
	}
	template<typename... Types>
	wstring to_wstring(const wchar_t * wstr, const Types &... args) noexcept
	{
		return to_wstring(wstring(wstr), args...);
	}
	template<typename... Types>
	wstring to_wstring(wchar_t * wstr, const Types &... args) noexcept
	{
		return to_wstring(const_cast<const wchar_t *>(wstr), args...);
	}
	wstring to_wstring() noexcept;


	template<typename T, typename... Types>
	string to_string(const T & t, const Types &... args) noexcept
	{
		stringstream bss;
		string result;

		bss << t;
		bss >> result;

		return result + to_string(args...);
	}
	template<typename... Types>
	string to_string(const string & str, const Types &... args) noexcept
	{
		return str + to_string(args...);
	}
	template<typename... Types>
	string to_string(const wstring & wstr, const Types &... args) noexcept
	{
		auto required_size = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), static_cast<int>(wstr.size()), nullptr, 0, NULL, NULL);
		string result(required_size, 0);
		if (!WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), static_cast<int>(wstr.size()), &result[0], static_cast<int>(result.size()), NULL, NULL))
		{
			auto error_message = SystemErrorMessageObject().message_string;
			MessageBox(NULL, error_message.c_str(), TEXT("to_string: WideCharToMultiByte Error"), MB_OK);
			exit(-1);
		}
		return result + to_string(args...);
	}
	template<typename... Types>
	string to_string(const char * str, const Types &... args) noexcept
	{
		return to_string(string(str), args...);
	}
	template<typename... Types>
	string to_string(char * str, const Types &... args) noexcept
	{
		return to_string(const_cast<const char *>(str), args...);
	}
	template<typename... Types>
	string to_string(const wchar_t * wstr, const Types &... args) noexcept
	{
		return to_string(wstring(wstr), args...);
	}
	template<typename... Types>
	string to_string(wchar_t * wstr, const Types &... args) noexcept
	{
		return to_string(const_cast<const wchar_t *>(wstr), args...);
	}
	string to_string() noexcept;

	template<typename T, typename... Types>
	basic_string<TCHAR> to_tstring(const T & t, const Types &... args) noexcept
	{
#ifdef UNICODE
		return to_wstring(t, args...);
#else
		return to_string(t, args...);
#endif
	}

	bool is_white_space(int c) noexcept;
	string remove_extra_white_space(const string & str) noexcept;
	string get_directory(const string & file_path) noexcept;
	string change_space_to_underline(const string & str) noexcept;
};

#endif