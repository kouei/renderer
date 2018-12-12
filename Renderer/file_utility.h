#ifndef _FILE_UTILITY_
#define _FILE_UTILITY_

#include <cstdio>
#include <string>

namespace FILE_UTILITY
{
	using std::string;

	enum FILE_MODE : int { READ, WRITE };

	struct FileObject
	{
		FileObject() noexcept;
		FileObject(const FileObject & rhs) = delete;
		FileObject(FileObject && rhs) noexcept;
		FileObject & operator= (const FileObject & rhs) = delete;
		FileObject & operator= (FileObject && rhs) noexcept;
		~FileObject() noexcept;

		FileObject(const string & file_path, FILE_MODE mode) noexcept;

		operator FILE *() noexcept;
		operator const FILE *() const noexcept;
		FILE & operator *() noexcept;
		const FILE & operator *() const noexcept;
		FILE * operator ->() noexcept;
		const FILE * operator ->() const noexcept;

		FILE * file;
	};

}

#endif