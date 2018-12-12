#include <algorithm>
#include "rasterize.h"
#include "math_utility.h"

using namespace RASTERIZE;
using namespace BITMAP_UTILITY;
using namespace UTILITY2D;
using namespace UTILITY3D;
using namespace MATH_UTILITY;
using namespace THREAD_UTILITY;
using std::min;
using std::max;
using std::swap;
using std::move;
using std::vector;

void RASTERIZE::draw_background(Bitmap<BGRA> & bitmap_bits, const Bitmap<BGRA> & bg_image) noexcept
{

	for (int32_t j = 0; j < bitmap_bits.height; ++j)
	{
		for (int32_t i = 0; i < bitmap_bits.width; ++i)
		{
			//bitmap_bits[j][i] = Color(0xff, 0x0, 0xcc, 0xff);
			bitmap_bits[j][i] = i < bg_image.width && j < bg_image.height ? bg_image[j][i] : BGRA{ 0x64, 0x64, 0x64, 0xff };
		}
	}
}

void RASTERIZE::init_z_depth_buf(Bitmap<float> & z_depth_buf, float init_depth) noexcept
{
	for (int32_t j = 0; j < z_depth_buf.height; ++j)
	{
		for (int32_t i = 0; i < z_depth_buf.width; ++i)
		{
			z_depth_buf[j][i] = init_depth;
		}
	}
}

//Bitmap<BGRA> RASTERIZE::ssaa_filter(const Bitmap<BGRA> & background_buffer, int32_t ssaa_factor) noexcept
//{
//	Bitmap<BGRA> real_image(background_buffer.width / ssaa_factor, background_buffer.height / ssaa_factor);
//	for (auto j = 0; j < real_image.height; ++j)
//	{
//		for (auto i = 0; i < real_image.width; ++i)
//		{
//			Vector4D<uint32_t> sum{ 0, 0, 0, 0 };
//			for (auto m = 0; m < ssaa_factor; ++m)
//			{
//				for (auto n = 0; n < ssaa_factor; ++n)
//				{
//					sum += type_cast<uint32_t>(background_buffer[j * ssaa_factor + m][i * ssaa_factor + n].v4d);
//				}
//			}
//			sum /= static_cast<uint32_t>(ssaa_factor * ssaa_factor);
//			real_image[j][i].v4d = type_cast<uint8_t>(sum);
//		}
//	}
//
//	return real_image;
//}

BGRA RASTERIZE::blinn_phong_lighting
(
	const Vector3D<float> & N,
	const Vector3D<float> & L,
	const Vector3D<float> & H,
	const Vector4D<float> & ambient_color,
	const Vector4D<float> & diffuse_color,
	const Vector4D<float> & specular_color,
	float Ns,
	int illum
) noexcept
{
	float diffuse_factor;
	float specular_factor;
	float ambient_factor = 0.05f;

	float N_len_square = dot(N, N);
	float L_len_square = dot(L, L);
	float L_len = sqrt(L_len_square);
	if (N_len_square == 0.0f || L_len_square == 0.0f)
	{
		diffuse_factor = 0.0f;
		specular_factor = 0.0f;
	}
	else
	{
		float N_len = sqrt(N_len_square);

		diffuse_factor = dot(N, L);
		if (diffuse_factor > 0.0f)
		{
			diffuse_factor /= N_len * L_len;
			auto H_len_square = dot(H, H);
			if (H_len_square != 0.0f)
			{
				auto H_len = sqrt(H_len_square);
				specular_factor = pow(dot(N, H) / (N_len * H_len), Ns);
			}
		}
		else
		{
			diffuse_factor = 0.0f;
			specular_factor = 0.0f;
		}
	}
	Vector4D<float> final_color;
	//auto k0 = 0.0f, k1 = 0.0f, k2 = 0.01f;
	//auto intensity = 1.0f / (k0 + k1 * L_len + k2 * L_len_square);
	auto intensity = 1.0f;
	if (illum == 1)
	{
		final_color = ambient_color * ambient_factor + diffuse_color * diffuse_factor * intensity;
	}
	else if (illum == 2)
	{
		final_color = ambient_color * ambient_factor + (diffuse_color * diffuse_factor + specular_color * specular_factor) * intensity;
	}
	else my_assert(false && "Unknown illumination model!");

	auto a = static_cast<uint8_t>(min<uint32_t>(static_cast<uint32_t>(final_color[0]), 0xff));
	auto r = static_cast<uint8_t>(min<uint32_t>(static_cast<uint32_t>(final_color[1]), 0xff));
	auto g = static_cast<uint8_t>(min<uint32_t>(static_cast<uint32_t>(final_color[2]), 0xff));
	auto b = static_cast<uint8_t>(min<uint32_t>(static_cast<uint32_t>(final_color[3]), 0xff));

	return BGRA{ b, g, r, a };
}

void RASTERIZE::draw_line_bresenham(const Vertex & start, const Vertex & end, ScreenData & screen_data) noexcept
{
	auto current_x = static_cast<int>(start.pos.x);
	auto current_y = static_cast<int>(start.pos.y);
	auto end_x = static_cast<int>(end.pos.x);
	auto end_y = static_cast<int>(end.pos.y);

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
			auto width = screen_data.background_buffer.width;
			auto height = screen_data.background_buffer.height;
			if (is_within_boundary(real_x, 0, width) && is_within_boundary(real_y, 0, height))
			{
				screen_data.background_buffer[real_y][real_x] = BGRA(0x0, 0x0, 0x0, 0x0);
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
			auto width = screen_data.background_buffer.width;
			auto height = screen_data.background_buffer.height;
			if (is_within_boundary(real_x, 0, width) && is_within_boundary(real_y, 0, height))
			{
				screen_data.background_buffer[real_y][real_x] = BGRA(0x0, 0x0, 0x0, 0x0);
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
			auto width = screen_data.background_buffer.width;
			auto height = screen_data.background_buffer.height;
			if (is_within_boundary(real_x, 0, width) && is_within_boundary(real_y, 0, height))
			{
				screen_data.background_buffer[real_y][real_x] = BGRA(0x0, 0x0, 0x0, 0x0);
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

void RASTERIZE::draw_triangle_line_frame(const Vertex & v0, const Vertex & v1, const Vertex & v2, ScreenData & screen_data) noexcept
{
	draw_line_bresenham(v0, v1, screen_data);
	draw_line_bresenham(v1, v2, screen_data);
	draw_line_bresenham(v2, v0, screen_data);
}

BGRA RASTERIZE::texture_map_pixel(const Vector2D<float> & tv, const Bitmap<BGRA> & map) noexcept
{
	my_assert(map.width > 0 && map.height > 0);

	Vector2D<int32_t> tv_int
	{
		static_cast<int32_t>(truncated_value(tv.x, 0.0f, 1.0f) * (map.width - 1)),
		static_cast<int32_t>(truncated_value(1.0f - tv.y, 0.0f, 1.0f) * (map.height - 1))
	};
	auto map_pixel = map[tv_int.y][tv_int.x];
	return map_pixel;
}

BGRA RASTERIZE::anisotropic_filtering(const Bitmap<BGRA> & tex_bits, const Vector2D<float> & tv) noexcept
{
	auto right_weight = tv.x - floor(tv.x);
	auto left_weight = ceil(tv.x) - tv.x;
	auto up_weight = ceil(tv.y) - tv.y;
	auto down_weight = tv.y - floor(tv.y);
	auto gcd = greatest_common_divisor(tex_bits.width, tex_bits.height);
	auto horizontal_sample_rate = tex_bits.width / gcd;
	auto vertical_sample_rate = tex_bits.height / gcd;
	horizontal_sample_rate = 2;
	vertical_sample_rate = 2;
	//auto tv_int = to_integral_vector<int32_t>(tv);
	auto tv_int = type_cast<int32_t>(tv);
	tv_int.x = truncated_value(tv_int.x, 0, tex_bits.width - 1);
	tv_int.y = truncated_value(tv_int.y, 0, tex_bits.height - 1);
	auto left = tv_int.x - horizontal_sample_rate / 2;
	auto right = left + horizontal_sample_rate;
	auto up = tv_int.y - vertical_sample_rate / 2;
	auto down = up + vertical_sample_rate;
	Vector4D<float> left_color{ 0.0f, 0.0f, 0.0f,0.0f };
	Vector4D<float> right_color{ 0.0f, 0.0f, 0.0f,0.0f };

	for (auto i = left; i < tv_int.x; ++i)
	{
		auto x = truncated_value(i, 0, tex_bits.width - 1);
		left_color += type_cast<float>(tex_bits.bytes[tv_int.y * tex_bits.width + x].v4d);
	}
	for (auto i = tv_int.x; i < right; ++i)
	{
		auto x = truncated_value(i, 0, tex_bits.width - 1);
		right_color += type_cast<float>(tex_bits.bytes[tv_int.y * tex_bits.width + x].v4d);
	}
	Vector4D<float> horizontal_color = left_color / max<float>(static_cast<float>(tv_int.x - left), 1.0f) * left_weight + right_color / static_cast<float>(right - tv_int.x) * right_weight;
	if (tv_int.x == left) horizontal_color = right_color;

	Vector4D<float> up_color{ 0.0f, 0.0f, 0.0f,0.0f };
	Vector4D<float> down_color{ 0.0f, 0.0f, 0.0f,0.0f };
	for (auto i = up; i < tv_int.y; ++i)
	{
		auto y = truncated_value(i, 0, tex_bits.height - 1);
		up_color += type_cast<float>(tex_bits.bytes[y * tex_bits.width + tv_int.x].v4d);
	}
	for (auto i = tv_int.y; i < down; ++i)
	{
		auto y = truncated_value(i, 0, tex_bits.height - 1);
		down_color += type_cast<float>(tex_bits.bytes[y * tex_bits.width + tv_int.x].v4d);
	}
	Vector4D<float> vertical_color = up_color / max<float>(static_cast<float>(tv_int.y - up), 1.0f) * up_weight + down_color / static_cast<float>(down - tv_int.y) * down_weight;
	if (tv_int.y == up) vertical_color = down_color;

	Vector4D<float> color = Vector4D<float>(horizontal_color + vertical_color) / 2.0f;

	BGRA final_color;
	final_color.v4d = type_cast<uint8_t>(color);
	return final_color;
}







void RASTERIZE::assign_job
(
	vector<vector<RenderJob>> & queue,
	const TriangleIndex & t,
	const vector<Vertex> & viewport_vertex
) noexcept
{
	auto v0_int = Vector2D<int32_t>{ static_cast<int32_t>(viewport_vertex[t.vertex_index[0]].pos.x), static_cast<int32_t>(viewport_vertex[t.vertex_index[0]].pos.y) };
	auto v1_int = Vector2D<int32_t>{ static_cast<int32_t>(viewport_vertex[t.vertex_index[1]].pos.x), static_cast<int32_t>(viewport_vertex[t.vertex_index[1]].pos.y) };
	auto v2_int = Vector2D<int32_t>{ static_cast<int32_t>(viewport_vertex[t.vertex_index[2]].pos.x), static_cast<int32_t>(viewport_vertex[t.vertex_index[2]].pos.y) };

	auto minx = min<int32_t>({ v0_int.x, v1_int.x, v2_int.x });
	auto maxx = max<int32_t>({ v0_int.x, v1_int.x, v2_int.x });
	auto miny = min<int32_t>({ v0_int.y, v1_int.y, v2_int.y });
	auto maxy = max<int32_t>({ v0_int.y, v1_int.y, v2_int.y });

	for (auto y = miny; y < maxy; ++y)
	{
		auto queue_index = y % queue.size();
		RenderJob rj{ y, minx, maxx, &t };
		queue[queue_index].push_back(rj);
	}
}

void RASTERIZE::line_blinn_phong
(
	const RenderJob & rj,
	const vector<Vertex> & viewport_vertex,
	const vector<Vector2D<float>> & tex_vertex,
	const vector<Vector3D<float>> & camera_normal,
	const vector<Texture> & texture,
	ScreenData & screen_data
) noexcept
{
	const auto v0 = viewport_vertex[rj.t->vertex_index[0]];
	const auto v1 = viewport_vertex[rj.t->vertex_index[1]];
	const auto v2 = viewport_vertex[rj.t->vertex_index[2]];
	const auto tv0 = tex_vertex[rj.t->texture_index[0]];
	const auto tv1 = tex_vertex[rj.t->texture_index[1]];
	const auto tv2 = tex_vertex[rj.t->texture_index[2]];
	const auto n0 = camera_normal[rj.t->normal_index[0]];
	const auto n1 = camera_normal[rj.t->normal_index[1]];
	const auto n2 = camera_normal[rj.t->normal_index[2]];

	const auto & current_texture = texture[rj.t->texture_id];

	auto v0_int = Vector2D<int32_t>{ static_cast<int32_t>(v0.pos.x), static_cast<int32_t>(v0.pos.y) };
	auto v1_int = Vector2D<int32_t>{ static_cast<int32_t>(v1.pos.x), static_cast<int32_t>(v1.pos.y) };
	auto v2_int = Vector2D<int32_t>{ static_cast<int32_t>(v2.pos.x), static_cast<int32_t>(v2.pos.y) };

	auto area = static_cast<float>(edge_function(v0_int, v1_int, v2_int));

	Vector2D<int32_t> v;
	v.y = rj.y;
	for (v.x = rj.x_start; v.x < rj.x_end; ++v.x)
	{
		Vector3D<float> w
		{
			static_cast<float>(edge_function(v1_int, v2_int, v)),
			static_cast<float>(edge_function(v2_int, v0_int, v)),
			static_cast<float>(edge_function(v0_int, v1_int, v))
		};
		auto edge0 = v2_int - v1_int;
		auto edge1 = v0_int - v2_int;
		auto edge2 = v1_int - v0_int;
		bool is_covered = true;
		is_covered = is_covered && (w[0] == 0.0f ? ((edge0.y == 0 && edge0.x < 0) || edge0.y > 0) : w[0] > 0.0f);
		is_covered = is_covered && (w[1] == 0.0f ? ((edge1.y == 0 && edge1.x < 0) || edge1.y > 0) : w[1] > 0.0f);
		is_covered = is_covered && (w[2] == 0.0f ? ((edge2.y == 0 && edge2.x < 0) || edge2.y > 0) : w[2] > 0.0f);

		if (!is_covered) continue;

		if (area != 0.0f) w /= area;

		auto width = screen_data.background_buffer.width;
		auto height = screen_data.background_buffer.height;

		if (!is_within_boundary(v.x, 0, width) || !is_within_boundary(v.y, 0, height)) continue;

		auto z = 1.0f / (w[0] / v0.pos.z + w[1] / v1.pos.z + w[2] / v2.pos.z);

		if (z >= screen_data.z_depth_buffer[v.y][v.x]) continue;

		screen_data.z_depth_buffer[v.y][v.x] = z;
		auto halfway = z * (v0.halfway * w[0] / v0.pos.z + v1.halfway * w[1] / v1.pos.z + v2.halfway * w[2] / v2.pos.z);
		auto normal = z * (n0 * w[0] / v0.pos.z + n1 * w[1] / v1.pos.z + n2 * w[2] / v2.pos.z);
		auto to_light_source = z * (v0.to_light_source * w[0] / v0.pos.z + v1.to_light_source * w[1] / v1.pos.z + v2.to_light_source * w[2] / v2.pos.z);


		Vector2D<float> tv{ 0.0f, 0.0f };


		auto Ka = current_texture.texture_info.Ka;
		auto Kd = current_texture.texture_info.Kd;
		auto Ks = current_texture.texture_info.Ks;
		const auto & map_Ka_bits = current_texture.texture_bits.map_Ka_bits;
		const auto & map_Kd_bits = current_texture.texture_bits.map_Kd_bits;
		const auto & map_Ks_bits = current_texture.texture_bits.map_Ks_bits;
		auto Ns = current_texture.texture_info.Ns;
		auto illum = current_texture.texture_info.illum;

		if (
			map_Ka_bits.width > 0 && map_Ka_bits.height > 0 ||
			map_Kd_bits.width > 0 && map_Kd_bits.height > 0 ||
			map_Ks_bits.width > 0 && map_Ks_bits.height > 0
			)
		{
			tv = z * (tv0 * w[0] / v0.pos.z + tv1 * w[1] / v1.pos.z + tv2 * w[2] / v2.pos.z);
		}

		Vector4D<float> ambient_color{ 255.0f, 255.0f * Ka[0], 255.0f * Ka[1], 255.0f * Ka[2] };
		Vector4D<float> diffuse_color{ 255.0f, 255.0f * Kd[0], 255.0f * Kd[1], 255.0f * Kd[2] };
		Vector4D<float> specular_color{ 255.0f, 255.0f * Ks[0], 255.0f * Ks[1], 255.0f * Ks[2] };
		if (map_Ka_bits.width > 0 && map_Ka_bits.height > 0)
		{
			auto ambient_pixel = texture_map_pixel(tv, map_Ka_bits);
			ambient_color = Vector4D<float>{ 255.0f, ambient_pixel.r * Ka[0], ambient_pixel.g * Ka[1], ambient_pixel.b * Ka[2] };
		}
		if (map_Kd_bits.width > 0 && map_Kd_bits.height > 0)
		{
			auto diffuse_pixel = texture_map_pixel(tv, map_Kd_bits);
			diffuse_color = Vector4D<float>{ 255.0f, diffuse_pixel.r * Kd[0], diffuse_pixel.g * Kd[1], diffuse_pixel.b * Kd[2] };
		}
		if (map_Ks_bits.width > 0 && map_Ks_bits.height > 0)
		{
			auto specular_pixel = texture_map_pixel(tv, map_Ks_bits);
			specular_color = Vector4D<float>{ 255.0f, specular_pixel.r * Ks[0], specular_pixel.g * Ks[1], specular_pixel.b * Ks[2] };
		}
		auto color = blinn_phong_lighting(normal, to_light_source, halfway, ambient_color, diffuse_color, specular_color, Ns, illum);


		// draw grey-degree only
		//auto grey_degree = static_cast<uint8_t>(0.299f * color.r + 0.586f * color.g + 0.114f * color.b);
		//color.r = color.g = color.b = grey_degree;


		screen_data.background_buffer[v.y][v.x] = color;
	}
}

//void RASTERIZE::draw_triangle_blinn_phong
//(
//	const Vertex & v0,
//	const Vertex & v1,
//	const Vertex & v2,
//	const Vector2D<float> & tv0,
//	const Vector2D<float> & tv1,
//	const Vector2D<float> & tv2,
//	const Vector3D<float> & n0,
//	const Vector3D<float> & n1,
//	const Vector3D<float> & n2,
//	const Texture & texture,
//	ScreenData & screen_data
//) noexcept
//{
//	auto v0_int = Vector2D<int32_t>{ static_cast<int32_t>(v0.pos.x), static_cast<int32_t>(v0.pos.y) };
//	auto v1_int = Vector2D<int32_t>{ static_cast<int32_t>(v1.pos.x), static_cast<int32_t>(v1.pos.y) };
//	auto v2_int = Vector2D<int32_t>{ static_cast<int32_t>(v2.pos.x), static_cast<int32_t>(v2.pos.y) };
//
//	auto minx = min<int32_t>({ v0_int.x, v1_int.x, v2_int.x });
//	auto maxx = max<int32_t>({ v0_int.x, v1_int.x, v2_int.x });
//	auto miny = min<int32_t>({ v0_int.y, v1_int.y, v2_int.y });
//	auto maxy = max<int32_t>({ v0_int.y, v1_int.y, v2_int.y });
//
//	auto area = static_cast<float>(edge_function(v0_int, v1_int, v2_int));
//
//	Vector2D<int32_t> v;
//	for (v.y = miny; v.y < maxy; ++v.y)
//	{
//		for (v.x = minx; v.x < maxx; ++v.x)
//		{
//			Vector3D<float> w
//			{
//				static_cast<float>(edge_function(v1_int, v2_int, v)),
//				static_cast<float>(edge_function(v2_int, v0_int, v)),
//				static_cast<float>(edge_function(v0_int, v1_int, v))
//			};
//			auto edge0 = v2_int - v1_int;
//			auto edge1 = v0_int - v2_int;
//			auto edge2 = v1_int - v0_int;
//			bool is_covered = true;
//			is_covered = is_covered && (w[0] == 0.0f ? ((edge0.y == 0 && edge0.x < 0) || edge0.y > 0) : w[0] > 0.0f);
//			is_covered = is_covered && (w[1] == 0.0f ? ((edge1.y == 0 && edge1.x < 0) || edge1.y > 0) : w[1] > 0.0f);
//			is_covered = is_covered && (w[2] == 0.0f ? ((edge2.y == 0 && edge2.x < 0) || edge2.y > 0) : w[2] > 0.0f);
//
//			if (!is_covered) continue;
//
//			if (area != 0.0f) w /= area;
//
//			auto width = screen_data.background_buffer.width;
//			auto height = screen_data.background_buffer.height;
//
//			if (!is_within_boundary(v.x, 0, width) || !is_within_boundary(v.y, 0, height)) continue;
//
//			auto z = 1.0f / (w[0] / v0.pos.z + w[1] / v1.pos.z + w[2] / v2.pos.z);
//			AtomicFlagGuard atomic_flag_guard(screen_data.atomic_flag_buffer[v.y][v.x]);
//
//			if (z >= screen_data.z_depth_buffer[v.y][v.x]) continue;
//
//			screen_data.z_depth_buffer[v.y][v.x] = z;
//			auto halfway = z * (v0.halfway * w[0] / v0.pos.z + v1.halfway * w[1] / v1.pos.z + v2.halfway * w[2] / v2.pos.z);
//			auto normal = z * (n0 * w[0] / v0.pos.z + n1 * w[1] / v1.pos.z + n2 * w[2] / v2.pos.z);
//			auto to_light_source = z * (v0.to_light_source * w[0] / v0.pos.z + v1.to_light_source * w[1] / v1.pos.z + v2.to_light_source * w[2] / v2.pos.z);
//
//
//			Vector2D<float> tv{ 0.0f, 0.0f };
//
//
//			auto & Ka = texture.texture_info.Ka;
//			auto & Kd = texture.texture_info.Kd;
//			auto & Ks = texture.texture_info.Ks;
//			auto & map_Ka_bits = texture.texture_bits.map_Ka_bits;
//			auto & map_Kd_bits = texture.texture_bits.map_Kd_bits;
//			auto & map_Ks_bits = texture.texture_bits.map_Ks_bits;
//			auto Ns = texture.texture_info.Ns;
//			auto illum = texture.texture_info.illum;
//
//			if (
//				map_Ka_bits.width > 0 && map_Ka_bits.height > 0 ||
//				map_Kd_bits.width > 0 && map_Kd_bits.height > 0 ||
//				map_Ks_bits.width > 0 && map_Ks_bits.height > 0
//				)
//			{
//				tv = z * (tv0 * w[0] / v0.pos.z + tv1 * w[1] / v1.pos.z + tv2 * w[2] / v2.pos.z);
//			}
//
//			Vector4D<float> ambient_color{ 255.0f, 255.0f * Ka[0], 255.0f * Ka[1], 255.0f * Ka[2] };
//			Vector4D<float> diffuse_color{ 255.0f, 255.0f * Kd[0], 255.0f * Kd[1], 255.0f * Kd[2] };
//			Vector4D<float> specular_color{ 255.0f, 255.0f * Ks[0], 255.0f * Ks[1], 255.0f * Ks[2] };
//			if (map_Ka_bits.width > 0 && map_Ka_bits.height > 0)
//			{
//				auto ambient_pixel = texture_map_pixel(tv, map_Ka_bits);
//				ambient_color = Vector4D<float>{ 255.0f, ambient_pixel.r * Ka[0], ambient_pixel.g * Ka[1], ambient_pixel.b * Ka[2] };
//			}
//			if (map_Kd_bits.width > 0 && map_Kd_bits.height > 0)
//			{
//				auto diffuse_pixel = texture_map_pixel(tv, map_Kd_bits);
//				diffuse_color = Vector4D<float>{ 255.0f, diffuse_pixel.r * Kd[0], diffuse_pixel.g * Kd[1], diffuse_pixel.b * Kd[2] };
//			}
//			if (map_Ks_bits.width > 0 && map_Ks_bits.height > 0)
//			{
//				auto specular_pixel = texture_map_pixel(tv, map_Ks_bits);
//				specular_color = Vector4D<float>{ 255.0f, specular_pixel.r * Ks[0], specular_pixel.g * Ks[1], specular_pixel.b * Ks[2] };
//			}
//			auto color = blinn_phong_lighting(normal, to_light_source, halfway, ambient_color, diffuse_color, specular_color, Ns, illum);
//			
//			
//			// draw grey-degree only
//			//auto grey_degree = static_cast<uint8_t>(0.299f * color.r + 0.586f * color.g + 0.114f * color.b);
//			//color.r = color.g = color.b = grey_degree;
//			
//
//			screen_data.background_buffer[v.y][v.x] = color;
//		}
//	}
//}

//DrawTriangle::DrawTriangle(DrawTriangle && rhs) noexcept
//	:
//	screen_data(move(rhs.screen_data)),
//	viewport_vertex(move(rhs.viewport_vertex)),
//	drawing_mode(move(rhs.drawing_mode)),
//	tex_vertex(move(rhs.tex_vertex)),
//	texture(move(rhs.texture)),
//	camera_normal(move(rhs.camera_normal))
//{
//	rhs.screen_data = nullptr;
//	rhs.viewport_vertex = nullptr;
//	rhs.tex_vertex = nullptr;
//	rhs.texture = nullptr;
//	rhs.camera_normal = nullptr;
//}
//
//DrawTriangle & DrawTriangle::operator= (DrawTriangle && rhs) noexcept
//{
//	if (this != &rhs)
//	{
//		swap(screen_data, rhs.screen_data);
//		swap(viewport_vertex, rhs.viewport_vertex);
//		swap(drawing_mode, rhs.drawing_mode);
//		swap(tex_vertex, rhs.tex_vertex);
//		swap(camera_normal, rhs.camera_normal);
//	}
//	return *this;
//}
//
//DrawTriangle::DrawTriangle
//(
//	ScreenData * _screen_data,
//	const vector<Vertex> * _viewport_vertex,
//	DRAWING_MODE _drawing_mode,
//	const vector<Vector2D<float>> * _tex_vertex,
//	const vector<Texture> * _texture,
//	const vector<Vector3D<float>> * _camera_normal
//) noexcept
//	:
//	screen_data(_screen_data),
//	viewport_vertex(_viewport_vertex),
//	drawing_mode(_drawing_mode),
//	tex_vertex(_tex_vertex),
//	texture(_texture),
//	camera_normal(_camera_normal)
//{}
//
//void DrawTriangle::operator() (const TriangleIndex & t) const noexcept
//{
//	if (drawing_mode == DRAWING_MODE::BLINN_PHONG)
//	{
//		draw_triangle_blinn_phong
//		(
//			(*viewport_vertex)[t.vertex_index[0]],
//			(*viewport_vertex)[t.vertex_index[1]],
//			(*viewport_vertex)[t.vertex_index[2]],
//			(*tex_vertex)[t.texture_index[0]],
//			(*tex_vertex)[t.texture_index[1]],
//			(*tex_vertex)[t.texture_index[2]],
//			(*camera_normal)[t.normal_index[0]],
//			(*camera_normal)[t.normal_index[1]],
//			(*camera_normal)[t.normal_index[2]],
//			(*texture)[t.texture_id],
//			*screen_data
//		);
//	}
//	else
//	{
//		draw_triangle_line_frame
//		(
//			(*viewport_vertex)[t.vertex_index[0]],
//			(*viewport_vertex)[t.vertex_index[1]],
//			(*viewport_vertex)[t.vertex_index[2]],
//			*screen_data
//		);
//	}
//}



