#include "post_process.h"
#include "mathlib3d.h"
#include "cuda_post_process.h"

using namespace POST_PROCESS;
using namespace MATHLIB3D;
using namespace CUDA_POST_PROCESS;

//Bitmap<BGRA> POST_PROCESS::gaussian_blur(const Bitmap<BGRA> & buffer) noexcept
//{
//	Bitmap<BGRA> blurred_buffer(buffer.width, buffer.height);
//
//	Vector2D<int32_t> diff[] = 
//	{
//		{ -1, -1 },{  0, -1 },{ +1, -1 },
//		{ -1,  0 },{  0,  0 },{ +1,  0 },
//		{ -1, +1 },{  0, +1 },{ +1, +1 }
//	};
//
//	Vector2D<int32_t> p;
//	for (p.y = 0; p.y < buffer.height; ++p.y)
//	{
//		for (p.x = 0; p.x < buffer.width; ++p.x)
//		{
//			Vector4D<float> new_color{ 0.0f, 0.0f, 0.0f, 0.0f };
//			for (auto d : diff)
//			{
//				auto new_p = p + d;
//				new_p.x = new_p.x < 0 ? (-new_p.x - 1) % buffer.width : new_p.x;
//				//new_p.x = new_p.x >= buffer.width ? buffer.width - (new_p.x - (buffer.width - 1)) : new_p.x;
//				new_p.x = new_p.x >= buffer.width ? buffer.width - ((new_p.x % buffer.width) + 1) : new_p.x;
//				new_p.y = new_p.y < 0 ? (-new_p.y - 1) % buffer.height : new_p.y;
//				//new_p.y = new_p.y >= buffer.height ? buffer.height - (new_p.y - (buffer.height - 1)) : new_p.y;
//				new_p.y = new_p.y >= buffer.height ? buffer.height - ((new_p.y % buffer.height) + 1) : new_p.y;
//
//				auto weight = !d.x && !d.y ? 0.2f : 0.1f;
//				new_color += type_cast<float>(buffer[new_p.y][new_p.x].v4d) * weight;
//			}
//			blurred_buffer[p.y][p.x] = BGRA{ static_cast<uint8_t>(new_color.x), static_cast<uint8_t>(new_color.y), static_cast<uint8_t>(new_color.z), static_cast<uint8_t>(new_color.w) };
//		}
//	}
//
//	return blurred_buffer;
//}





Bitmap<BGRA> POST_PROCESS::gaussian_blur(const Bitmap<BGRA> & buffer) noexcept
{
	Bitmap<BGRA> blurred_buffer(buffer.width, buffer.height);

	float filter[] =
	{
		0.01f,0.01f,0.01f,0.01f,0.01f,0.01f,0.01f,0.01f,0.01f,0.01f,
		0.01f,0.01f,0.01f,0.01f,0.01f,0.01f,0.01f,0.01f,0.01f,0.01f,
		0.01f,0.01f,0.01f,0.01f,0.01f,0.01f,0.01f,0.01f,0.01f,0.01f,
		0.01f,0.01f,0.01f,0.01f,0.01f,0.01f,0.01f,0.01f,0.01f,0.01f,
		0.01f,0.01f,0.01f,0.01f,0.01f,0.01f,0.01f,0.01f,0.01f,0.01f,
		0.01f,0.01f,0.01f,0.01f,0.01f,0.01f,0.01f,0.01f,0.01f,0.01f,
		0.01f,0.01f,0.01f,0.01f,0.01f,0.01f,0.01f,0.01f,0.01f,0.01f,
		0.01f,0.01f,0.01f,0.01f,0.01f,0.01f,0.01f,0.01f,0.01f,0.01f,
		0.01f,0.01f,0.01f,0.01f,0.01f,0.01f,0.01f,0.01f,0.01f,0.01f,
		0.01f,0.01f,0.01f,0.01f,0.01f,0.01f,0.01f,0.01f,0.01f,0.01f
	};

	your_gaussian_blur(&buffer[0][0], &blurred_buffer[0][0], buffer.height, buffer.width, filter, 10);

	return blurred_buffer;
}









//Bitmap<BGRA> POST_PROCESS::grey_scale(const Bitmap<BGRA> & buffer) noexcept
//{
//	Bitmap<BGRA> grey_scale_buffer(buffer.width, buffer.height);
//
//	Vector2D<int32_t> p;
//	for (p.y = 0; p.y < buffer.height; ++p.y)
//	{
//		for (p.x = 0; p.x < buffer.width; ++p.x)
//		{
//			Vector4D<float> weight{ 0.114f, 0.587f, 0.299f, 0.0f };
//			auto grey = static_cast<uint8_t>(dot(type_cast<float>(buffer[p.y][p.x].v4d), weight));
//			grey_scale_buffer[p.y][p.x] = BGRA{ grey , grey ,grey, 0xff };
//		}
//	}
//
//	return grey_scale_buffer;
//}


Bitmap<BGRA> POST_PROCESS::grey_scale(const Bitmap<BGRA> & buffer) noexcept
{
	auto grey_scale_buffer = Bitmap<BGRA>{ buffer.width, buffer.height };
	your_bgra_to_greyscale(&buffer[0][0], &grey_scale_buffer[0][0], buffer.height, buffer.width);
	return grey_scale_buffer;
}











Bitmap<BGRA> POST_PROCESS::ssaa_filter(const Bitmap<BGRA> & background_buffer, int32_t ssaa_factor) noexcept
{
	Bitmap<BGRA> real_image(background_buffer.width / ssaa_factor, background_buffer.height / ssaa_factor);
	your_average_pooling(&background_buffer[0][0], &real_image[0][0], real_image.height, real_image.width, ssaa_factor);
	return real_image;
}



















void POST_PROCESS::bresenham_line_to(float x1, float y1, float x2, float y2, int32_t pen_width, Bitmap<BGRA> & pen_buffer) noexcept
{
	auto current_x = static_cast<int>(x1);
	auto current_y = static_cast<int>(y1);
	auto end_x = static_cast<int>(x2);
	auto end_y = static_cast<int>(y2);

	auto origin_dx = abs(end_x - current_x);
	auto origin_dy = abs(end_y - current_y);

	if (origin_dy > origin_dx)
	{
		swap(current_x, current_y);
		swap(end_x, end_y);
	}
	auto & real_x = origin_dy > origin_dx ? current_y : current_x;
	auto & real_y = origin_dy > origin_dx ? current_x : current_y;

	if (current_x > end_x)
	{
		swap(current_x, end_x);
		swap(current_y, end_y);
	}
	auto dx = end_x - current_x;
	auto dy = end_y - current_y;


	if (current_y == end_y)
	{
		while (current_x <= end_x)
		{
			auto width = pen_buffer.width;
			auto height = pen_buffer.height;
			for (auto j = 0; j < pen_width; ++j)
			{
				for (auto i = 0; i < pen_width; ++i)
				{
					if (is_within_boundary(real_x + i, 0, width) && is_within_boundary(real_y + j, 0, height))
					{
						pen_buffer[real_y + j][real_x + i] = BGRA(0x0, 0x0, 0xff, 0xff);
					}
				}
			}
			++current_x;
		}
	}
	else if (current_y < end_y)
	{
		auto p = 2 * dy - dx;
		auto p_increment1 = 2 * dy;
		auto p_increment2 = 2 * (dy - dx);

		while (current_x <= end_x)
		{
			auto width = pen_buffer.width;
			auto height = pen_buffer.height;
			for (auto j = 0; j < pen_width; ++j)
			{
				for (auto i = 0; i < pen_width; ++i)
				{
					if (is_within_boundary(real_x + i, 0, width) && is_within_boundary(real_y + j, 0, height))
					{
						pen_buffer[real_y + j][real_x + i] = BGRA(0x0, 0x0, 0xff, 0xff);
					}
				}
			}
			++current_x;
			if (p < 0)
			{
				p += p_increment1;
			}
			else
			{
				++current_y;
				p += p_increment2;
			}
		}
	}
	else
	{
		auto p = 2 * dy - dx;
		auto p_increment1 = 2 * dy;
		auto p_increment2 = 2 * (dy + dx);

		while (current_x <= end_x)
		{
			auto width = pen_buffer.width;
			auto height = pen_buffer.height;
			for (auto j = 0; j < pen_width; ++j)
			{
				for (auto i = 0; i < pen_width; ++i)
				{
					if (is_within_boundary(real_x + i, 0, width) && is_within_boundary(real_y + j, 0, height))
					{
						pen_buffer[real_y + j][real_x + i] = BGRA(0x0, 0x0, 0xff, 0xff);
					}
				}
			}
			++current_x;
			if (p >= 0)
			{
				p += p_increment1;
			}
			else
			{
				--current_y;
				p += p_increment2;
			}
		}
	}
}


