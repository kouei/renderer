#include<string>
#include<algorithm>
#include "my_assert.h"
#include"string_utility.h"

using std::string;
using std::wstring;
using std::max;

wstring STRING_UTILITY::to_wstring() noexcept
{
	return wstring();
}

string STRING_UTILITY::to_string() noexcept
{
	return string();
}

bool STRING_UTILITY::is_white_space(int c) noexcept
{
	return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

string STRING_UTILITY::remove_extra_white_space(const string & str) noexcept
{
	string result;

	int start = 0;
	while (start < str.size() && is_white_space(str[start])) ++start;
	int end = static_cast<int>(str.size()) - 1;
	while (end >= 0 && is_white_space(str[end])) --end;

	++end;
	for (int i = start; i < end;)
	{
		if (is_white_space(str[i]))
		{
			result += ' ';
			while (i < end && is_white_space(str[i])) ++i;
		}
		else
		{
			result += str[i];
			++i;
		}
	}
	return result;
}

string STRING_UTILITY::get_directory(const string & file_path) noexcept
{
	auto last_slash_pos = file_path.find_last_of('/');
	auto last_back_slash_pos = file_path.find_last_of('\\');
	my_assert(last_slash_pos != string::npos || last_back_slash_pos != string::npos);
	if (last_slash_pos != string::npos && last_back_slash_pos != string::npos)
	{
		return file_path.substr(0, max<size_t>(last_slash_pos, last_slash_pos) + 1);
	}
	else if (last_slash_pos != string::npos)
	{
		return file_path.substr(0, last_slash_pos + 1);
	}
	else
	{
		return file_path.substr(0, last_back_slash_pos + 1);
	}
}

string STRING_UTILITY::change_space_to_underline(const string & str) noexcept
{
	string result;
	for (auto c : str)
	{
		result += c == ' ' ? '_' : c;
	}
	return result;
}