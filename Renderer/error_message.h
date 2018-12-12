#ifndef _ERROR_MESSAGE_
#define _ERROR_MESSAGE_

#include <windows.h>
#include <string>
#include <cstdio>

namespace ERROR_MESSAGE
{
	using std::basic_string;

	struct SystemErrorMessageObject
	{
		SystemErrorMessageObject() noexcept;
		SystemErrorMessageObject(const SystemErrorMessageObject & rhs) = delete;
		SystemErrorMessageObject(SystemErrorMessageObject && rhs) noexcept;
		SystemErrorMessageObject & operator=(const SystemErrorMessageObject & rhs) = delete;
		SystemErrorMessageObject & operator=(SystemErrorMessageObject && rhs) noexcept;
		~SystemErrorMessageObject() noexcept = default;

		operator basic_string<TCHAR>() const noexcept 
		{
			return message_string;
		}

		basic_string<TCHAR> message_string;
	};
};


#ifdef UNICODE
static auto ftprintf = fwprintf;
#else
static auto ftprintf = fprintf;
#endif

static FILE * error_message_fp;

#define fatal_error(error_message) \
( \
	static_cast<void> \
	( \
		error_message_fp = fopen("fatal-error-info.txt", "w"), \
		ftprintf(error_message_fp, TEXT("Fatal Error!\n")), \
		ftprintf(error_message_fp, TEXT("Error: ")), ftprintf(error_message_fp, TEXT("%s\n"), (error_message)), \
		ftprintf(error_message_fp, TEXT("File: ")), ftprintf(error_message_fp, TEXT("%s\n"), TEXT(__FILE__)), \
		ftprintf(error_message_fp, TEXT("Line: ")), ftprintf(error_message_fp, TEXT("%d\n"), __LINE__), \
		fclose(error_message_fp), \
		system("notepad fatal-error-info.txt"), \
		exit(-1), \
		0 \
	) \
)




#endif