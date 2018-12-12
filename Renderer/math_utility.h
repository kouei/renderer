#ifndef _MATH_UTILITY_
#define _MATH_UTILITY_

#include <type_traits>

namespace MATH_UTILITY
{
	using std::true_type;
	using std::is_integral;

	const auto PI = 3.14159;

	// Check point whether value is within boundary [lower, upper)
	template<typename T>
	bool is_within_boundary(T value, T lower, T upper) noexcept
	{
		return lower <= value && value < upper;
	}

	template<typename T>
	T truncated_value(T value, T lower, T upper) noexcept
	{
		return value < lower ? lower : (value > upper ? upper : value);
	}

	template<typename T>
	T greatest_common_divisor_impl(T a, T b, true_type) noexcept
	{
		return b == 0 ? a : greatest_common_divisor_impl(b, a % b, is_integral<T>());
	}

	template<typename T>
	T greatest_common_divisor(T a, T b) noexcept
	{
		return greatest_common_divisor_impl(a, b, is_integral<T>());
	}

	template<typename T>
	T defree_to_radian(T degree) noexcept
	{
		return degree / static_cast<T>(180.0) * static_cast<T>(PI);
	}
};

#endif