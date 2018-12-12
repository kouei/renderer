#include"utility2d.h"
using namespace UTILITY2D;

BGRA::BGRA(uint8_t _b, uint8_t _g, uint8_t _r, uint8_t _a) noexcept
	:b(_b), g(_g), r(_r), a(_a)
{}
BGRA::BGRA(uint32_t _bgra) noexcept
	: bgra(_bgra)
{}

void ScreenData::resize(int32_t width, int32_t height) noexcept
{
	background_buffer.resize(width, height);
	z_depth_buffer.resize(width, height);
}

BGRA UTILITY2D::blend_color(BGRA color_a, BGRA color_b) noexcept
{
	auto opacity = (static_cast<float>(color_a.a) / static_cast<float>(0xff));
	auto new_r = static_cast<uint8_t>(opacity * color_a.r + (1.0f - opacity) * color_b.r);
	auto new_g = static_cast<uint8_t>(opacity * color_a.g + (1.0f - opacity) * color_b.g);
	auto new_b = static_cast<uint8_t>(opacity * color_a.b + (1.0f - opacity) * color_b.b);
	return BGRA{ new_b, new_g, new_r, 0x0 };
}


BGR::BGR(uint8_t _b, uint8_t _g, uint8_t _r) noexcept
	:b(_b), g(_g), r(_r)
{}