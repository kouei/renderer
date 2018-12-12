#ifndef _UTILITY_2D_
#define _UTILITY_2D_

#include "bitmap_utility.h"
#include "mathlib3d.h"
#include "thread_utility.h"

namespace UTILITY2D
{
	using namespace MATHLIB3D;
	using namespace UTILITY2D;
	using namespace THREAD_UTILITY;
	using namespace BITMAP_UTILITY;

	struct BGRA
	{
		BGRA(uint8_t _b, uint8_t _g, uint8_t _r, uint8_t _a) noexcept;
		explicit BGRA(uint32_t _bgra) noexcept;
		BGRA() noexcept = default;
		BGRA(const BGRA & rhs) noexcept = default;
		BGRA(BGRA && rhs) noexcept = default;
		BGRA & operator= (const BGRA & rhs) noexcept = default;
		BGRA & operator= (BGRA && rhs) noexcept = default;

		union
		{

			struct
			{
				uint8_t b, g, r, a;
			};
			Vector4D<uint8_t> v4d;
			uint32_t bgra;
		};
	};
	
	struct BGR
	{
		BGR(uint8_t _b, uint8_t _g, uint8_t _r) noexcept;
		explicit BGR(uint32_t _bgra) noexcept;
		BGR() noexcept = default;
		BGR(const BGR & rhs) noexcept = default;
		BGR(BGR && rhs) noexcept = default;
		BGR & operator= (const BGR & rhs) noexcept = default;
		BGR & operator= (BGR && rhs) noexcept = default;

		union
		{

			struct
			{
				uint8_t b, g, r;
			};
			Vector3D<uint8_t> v3d;
		};
	};

	struct ScreenData
	{
		ScreenData() noexcept = default;
		ScreenData(const ScreenData & rhs) = delete;
		ScreenData(ScreenData && rhs) noexcept = default;
		ScreenData & operator= (const ScreenData & rhs) = delete;
		ScreenData & operator= (ScreenData && rhs) noexcept = default;

		void resize(int32_t width, int32_t height) noexcept;

		Bitmap<BGRA> background_buffer;
		Bitmap<float> z_depth_buffer;
	};


	//draw color a on color b, using alpha blend
	BGRA blend_color(BGRA color_a, BGRA color_b) noexcept;
};

#endif