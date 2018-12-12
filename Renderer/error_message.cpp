#include "error_message.h"	
#include "string_utility.h"

using namespace ERROR_MESSAGE;
using namespace STRING_UTILITY;
using std::move;
using std::swap;

SystemErrorMessageObject::SystemErrorMessageObject() noexcept
{
	TCHAR * message_buffer;
	FormatMessage
	(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
		(LPTSTR)&message_buffer,
		0,
		NULL
	);
	message_string = to_tstring(message_buffer);
	LocalFree(message_buffer);
}

SystemErrorMessageObject::SystemErrorMessageObject(SystemErrorMessageObject && rhs) noexcept
	:
	message_string(move(rhs.message_string))
{
}

SystemErrorMessageObject & SystemErrorMessageObject::operator=(SystemErrorMessageObject && rhs) noexcept
{
	if (&rhs != this)
	{
		swap(message_string, rhs.message_string);
	}
	return *this;
}

