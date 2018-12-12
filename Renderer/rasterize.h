#ifndef _RASTERIZE_
#define _RASTERIZE_

#include <vector>
#include "utility3d.h"

namespace RASTERIZE
{
	using namespace UTILITY3D;
	using namespace UTILITY2D;
	using namespace MATHLIB3D;
	using namespace BITMAP_UTILITY;

	using std::vector;

	enum class DRAWING_MODE : int { BLINN_PHONG, LINE_FRAME };

	/*struct DrawTriangle
	{
		DrawTriangle() noexcept = default;
		DrawTriangle(const DrawTriangle & rhs) noexcept = default;
		DrawTriangle(DrawTriangle && rhs) noexcept;
		DrawTriangle & operator= (const DrawTriangle & rhs) noexcept = default;
		DrawTriangle & operator= (DrawTriangle && rhs) noexcept;
		~DrawTriangle() noexcept = default;

		DrawTriangle
		(
			ScreenData * _screen_data,
			const vector<Vertex> * _viewport_vertex,
			DRAWING_MODE _drawing_mode,
			const vector<Vector2D<float>> * _tex_vertex,
			const vector<Texture> * _texture,
			const vector<Vector3D<float>> * _camera_normal
		) noexcept;

		void operator() (const TriangleIndex & t) const noexcept;

		ScreenData * screen_data;
		const vector<Vertex> * viewport_vertex;
		DRAWING_MODE drawing_mode;
		const vector<Vector2D<float>> * tex_vertex;
		const vector<Texture> * texture;
		const vector<Vector3D<float>> * camera_normal;
	};*/

	void draw_background(Bitmap<BGRA> & bitmap_bits, const Bitmap<BGRA> & bg_image) noexcept;
	void init_z_depth_buf(Bitmap<float> & z_depth_buf, float init_depth) noexcept;
	//Bitmap<BGRA> ssaa_filter(const Bitmap<BGRA> & background_buffer, int32_t ssaa_factor) noexcept;
	BGRA blinn_phong_lighting
	(
		const Vector3D<float> & N,
		const Vector3D<float> & L,
		const Vector3D<float> & H,
		const Vector4D<float> & ambient_color,
		const Vector4D<float> & diffuse_color,
		const Vector4D<float> & specular_color,
		float Ns,
		int illum
	) noexcept;
	void draw_line_bresenham(const Vertex & start, const Vertex & end, ScreenData & screen_data) noexcept;
	void draw_triangle_line_frame(const Vertex & v0, const Vertex & v1, const Vertex & v2, ScreenData & screen_data) noexcept;
	template<typename T>
	T edge_function(const Vector2D<T> & v0, const Vector2D<T> & v1, const Vector2D<T> & v2) noexcept
	{
		return (v2.x - v0.x) * (v1.y - v0.y) - (v2.y - v0.y) * (v1.x - v0.x);
	}

	BGRA texture_map_pixel(const Vector2D<float> & tv, const Bitmap<BGRA> & map) noexcept;
	BGRA anisotropic_filtering(const Bitmap<BGRA> & tex_bits, const Vector2D<float> & tv) noexcept;



	struct RenderJob
	{
		int y;
		int x_start, x_end;
		const TriangleIndex * t;
	};

	void assign_job
	(
		vector<vector<RenderJob>> & queue,
		const TriangleIndex & t,
		const vector<Vertex> & viewport_vertex
	) noexcept;

	void line_blinn_phong
	(
		const RenderJob & rj,
		const vector<Vertex> & viewport_vertex,
		const vector<Vector2D<float>> & tex_vertex,
		const vector<Vector3D<float>> & camera_normal,
		const vector<Texture> & texture,
		ScreenData & screen_data
	) noexcept;

	












	/*void draw_triangle_blinn_phong
	(
		const Vertex & v0,
		const Vertex & v1,
		const Vertex & v2,
		const Vector2D<float> & tv0,
		const Vector2D<float> & tv1,
		const Vector2D<float> & tv2,
		const Vector3D<float> & n0,
		const Vector3D<float> & n1,
		const Vector3D<float> & n2,
		const Texture & texture,
		ScreenData & screen_data
	) noexcept;*/



};

#endif