#ifndef _POST_PROCESS_
#define _POST_PROCESS_

#include "bitmap_utility.h"
#include "utility2d.h"

namespace POST_PROCESS
{
	using namespace BITMAP_UTILITY;
	using namespace UTILITY2D;

	Bitmap<BGRA> gaussian_blur(const Bitmap<BGRA> & buffer) noexcept;
	Bitmap<BGRA> grey_scale(const Bitmap<BGRA> & buffer) noexcept;
	Bitmap<BGRA> ssaa_filter(const Bitmap<BGRA> & background_buffer, int32_t ssaa_factor) noexcept;
	void bresenham_line_to(float x1, float y1, float x2, float y2, int32_t pen_width, Bitmap<BGRA> & pen_buffer) noexcept;
};

#endif