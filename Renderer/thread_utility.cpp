#include "thread_utility.h"

using namespace THREAD_UTILITY;

AtomicFlagObject::AtomicFlagObject() noexcept
	:flag{ ATOMIC_FLAG_INIT }
{
}
AtomicFlagObject::~AtomicFlagObject() noexcept
{
	flag.clear();
}

AtomicFlagObject::operator atomic_flag & () noexcept { return flag; }
AtomicFlagObject::operator const atomic_flag & () const noexcept { return flag; }


AtomicFlagGuard::AtomicFlagGuard(atomic_flag & _flag) noexcept
	:flag(_flag)
{
	while (flag.test_and_set()) {}
}
AtomicFlagGuard::~AtomicFlagGuard() noexcept { flag.clear(); }
