#include "my_assert.h"
#include "file_utility.h"
#include "string_utility.h"
#include "error_message.h"

using namespace FILE_UTILITY;
using namespace STRING_UTILITY;
using std::string;
using std::move;
using std::swap;

FileObject::FileObject(const string & file_path, FILE_MODE mode) noexcept
{
	if (mode == FILE_MODE::READ) file = fopen(file_path.c_str(), "r");
	else if (mode == FILE_MODE::WRITE) file = fopen(file_path.c_str(), "w");
	else my_assert("Unknow File Mode!" && false);
	if (!file)
	{
		auto error_message = to_tstring("Can not open file: ", file_path);
		fatal_error(error_message.c_str());
	}
}
FileObject::FileObject() noexcept
	: file(nullptr)
{}

FileObject::FileObject(FileObject && rhs) noexcept
	:file(move(rhs.file))
{
	rhs.file = nullptr;
}
FileObject & FileObject::operator= (FileObject && rhs) noexcept
{
	if (&rhs != this)
	{
		swap(file, rhs.file);
	}
	return *this;
}
FileObject::~FileObject() noexcept
{
	if (file)
	{
		my_assert(fclose(file) != EOF);
	}
}

FileObject::operator FILE *() noexcept { return file; }
FileObject::operator const FILE *() const noexcept { return file; }
FILE & FileObject::operator *() noexcept { return *file; }
const FILE & FileObject::operator *() const noexcept { return *file; }
FILE * FileObject::operator ->() noexcept { return file; }
const FILE * FileObject::operator ->() const noexcept { return file; }
