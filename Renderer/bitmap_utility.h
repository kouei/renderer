// Copyright 2016 Kouei

#ifndef _BITMAP_UTILITY_
#define _BITMAP_UTILITY_

#include <cstdint>
#include <utility>
#include "my_assert.h"

namespace BITMAP_UTILITY
{
	using std::nothrow;
	using std::swap;
	using std::move;

	template<typename T>
	struct Bitmap
	{
		Bitmap(int32_t _width, int32_t _height) noexcept
		{
			my_assert(_width > 0 && _height > 0);
			bytes = new (nothrow) T[static_cast<uint64_t>(_width) * static_cast<uint64_t>(_height)];
			my_assert(bytes);
			size = _width * _height;
			width = _width;
			height = _height;
		}
		Bitmap() noexcept
			: bytes(nullptr), size(0), width(0), height(0)
		{}
		Bitmap(const Bitmap & rhs) = delete;
		Bitmap(Bitmap && rhs) noexcept
			:bytes(move(rhs.bytes)), size(move(rhs.size)), width(move(rhs.width)), height(move(rhs.height))
		{
			rhs.bytes = nullptr;
			rhs.size = 0;
			rhs.width = 0;
			rhs.height = 0;
		}
		Bitmap & operator= (const Bitmap & rhs) = delete;
		Bitmap & operator= (Bitmap && rhs) noexcept
		{
			if (&rhs != this)
			{
				swap(bytes, rhs.bytes);
				swap(size, rhs.size);
				swap(width, rhs.width);
				swap(height, rhs.height);
			}

			return *this;
		}
		~Bitmap()
		{
			delete[] bytes;
		}

		void resize(int32_t _width, int32_t _height) noexcept
		{
			if (_width * _height > size)
			{
				*this = Bitmap(_width, _height);
			}
			else
			{
				width = _width;
				height = _height;
			}
		}

		T * operator[](int32_t i) noexcept { return &bytes[i * width]; }
		const T * operator[](int32_t i) const noexcept { return &bytes[i * width]; }

		T * bytes;
		int32_t size;
		int32_t width;
		int32_t height;
	};

	template<typename T>
	Bitmap<T> clone(const Bitmap<T> & origin) noexcept
	{
		Bitmap<T> result(origin.width, origin.height);
		memcpy(&result[0][0], &origin[0][0], result.size * sizeof(result[0][0]));
		return result;
	}
};    // namespace BITMAP_UTILITY

#endif    // _BITMAP_UTILITY_
