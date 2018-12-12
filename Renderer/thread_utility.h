#ifndef _THREAD_UTILITY_
#define _THREAD_UTILITY_

#include <atomic>

namespace THREAD_UTILITY
{
	using std::atomic_flag;

	struct AtomicFlagObject
	{
		AtomicFlagObject() noexcept;
		AtomicFlagObject(const AtomicFlagObject & rhs) = delete;
		AtomicFlagObject(AtomicFlagObject && rhs) = delete;
		AtomicFlagObject & operator= (const AtomicFlagObject & rhs) = delete;
		AtomicFlagObject & operator= (AtomicFlagObject && rhs) = delete;
		~AtomicFlagObject() noexcept;

		operator atomic_flag & () noexcept;
		operator const atomic_flag & () const noexcept;

		atomic_flag flag;
	};

	struct AtomicFlagGuard
	{
		AtomicFlagGuard(atomic_flag & _flag) noexcept;
		AtomicFlagGuard(const AtomicFlagGuard & rhs) = delete;
		AtomicFlagGuard(AtomicFlagGuard && rhs) = delete;
		AtomicFlagGuard & operator= (const AtomicFlagGuard & rhs) = delete;
		AtomicFlagGuard & operator= (AtomicFlagGuard && rhs) = delete;
		~AtomicFlagGuard() noexcept;

		atomic_flag & flag;
	};
};

#endif