#ifndef _MESSAGE_HANDLER_
#define _MESSAGE_HANDLER_

#include"rasterize.h"

namespace MESSAGE_HANDLER
{
	using std::string;
	using std::vector;
	using namespace MATHLIB3D;
	using namespace UTILITY3D;
	using namespace RASTERIZE;

	void read_mesh_file
	(
		const string & mesh_file,
		vector<Vertex> & object_vertex,
		vector<TriangleIndex> & triangle,
		vector<Vector2D<float>> & tex_vertex,
		vector<Texture> & texture,
		Frustum & frustum,
		Vector3D<float> & light_source,
		Vector3D<float> & camera_pos,
		Vector3D<float> & camera_transform,
		float & scale,
		vector<Vector3D<float>> & normal
	) noexcept;
	

	/*void save_mesh_file
	(
		vector<Vertex> & object_vertex,
		vector<TriangleIndex> & triangle,
		vector<Vector2D<float>> & tex_vertex,
		vector<Texture> & texture,
		Frustum & frustum,
		Vector3D<float> & light_source,
		Vector3D<float> & camera_pos,
		Vector3D<float> & camera_transform,
		float & scale,
		const vector<Vector3D<float>> & world_normal
	) noexcept;*/

	void print_info(HDC hdc, float fps, int32_t ssaa_factor, bool is_ssaa) noexcept;

	Bitmap<BGRA> load_bitmap(const string & path) noexcept;
	void save_bitmap(const string & path, Bitmap<BGRA> & bitmap) noexcept;

	void load_texture(const string & mesh_file, vector<Texture> & texture) noexcept;

	void wm_size_handler
	(
		ScreenData & screen_data,
		Bitmap<BGRA> & pen_buffer,
		Frustum & frustum,
		LPARAM lParam,
		int32_t ssaa_factor,
		bool is_ssaa
	) noexcept;

	/*void render
	(
		int start,
		int end,
		const vector<TriangleIndex> & triangle,
		const DrawTriangle & draw_triangle_functor
	) noexcept;*/

	float get_fps(float time_delta) noexcept;

	void wm_paint_handler
	(
		HWND hwnd,
		vector<Vertex> & camera_vertex,
		Vector3D<float> & camera_transform,
		vector<Vertex> & world_vertex,
		vector<TriangleIndex> & triangle,
		Vector3D<float> & rotate_axis,
		float rotate_degree,
		Vector3D<float> & camera_pos,
		Vector3D<float> & light_source,
		Frustum & frustum,
		vector<Vertex> & homogeneous_clipping_vertex,
		float scale,
		vector<Vertex> viewport_vertex,
		ScreenData & screen_data,
		DRAWING_MODE drawing_mode,
		const vector<Vector2D<float>> & tex_vertex,
		const vector<Texture> & texture,
		const vector<Vector3D<float>> & world_normal,
		vector<Vector3D<float>> & camera_normal,
		const Bitmap<BGRA> & bg_image
	) noexcept;

	void wm_keydown_handler
	(
		WPARAM wParam,
		Vector3D<float> & light_source,
		Vector3D<float> & rotate_axis,
		float & rotate_degree,
		DRAWING_MODE & drawing_mode,
		HWND hwnd,
		LPARAM lParam,
		vector<Vertex> & world_vertex,
		vector<TriangleIndex> & triangle,
		vector<Vector2D<float>> & tex_vertex,
		vector<Texture> & texture,
		Frustum & frustum,
		Vector3D<float> & camera_pos,
		Vector3D<float> & camera_transform,
		float & scale,
		ScreenData & screen_data,
		int32_t & ssaa_factor,
		bool & is_ssaa,
		bool is_pen,
		const vector<Vector3D<float>> & world_normal,
		int32_t & pen_width
	) noexcept;

	void wm_lbuttondown_handler
	(
		float & mouse_down_x,
		float & mouse_down_y,
		float & last_mouse_x,
		float & last_mouse_y,
		bool & mouse_leftkey_down,
		LPARAM lParam,
		HWND hwnd
	) noexcept;

	void wm_keyup_handler
	(
		vector<Vertex> & world_vertex,
		Vector3D<float> & rotate_axis,
		float & rotate_degree,
		vector<Vector3D<float>> & world_normal
	) noexcept;

	void wm_lbuttonup_handler
	(
		vector<Vertex> & world_vertex,
		Vector3D<float> & rotate_axis,
		float & rotate_degree,
		bool & mouse_leftkey_down,
		vector<Vector3D<float>> & world_normal
	) noexcept;

	void wm_mousemove_handler
	(
		bool mouse_leftkey_down,
		Vector3D<float> & rotate_axis,
		float & rotate_degree,
		HWND hwnd,
		LPARAM lParam,
		float mouse_down_x,
		float mouse_down_y,
		float & last_mouse_x,
		float & last_mouse_y,
		bool is_pen,
		Bitmap<BGRA> & pen_buffer,
		int32_t pen_width
	) noexcept;

	void wm_mousewheel_handler
	(
		Vector3D<float> & camera_transform,
		Frustum & frustum,
		WPARAM wParam,
		HWND hwnd
	) noexcept;

	void wm_command_handler
	(
		HWND hwnd,
		HCURSOR pen_cursor,
		HCURSOR default_cursor,
		WPARAM wParam,
		vector<Vertex> & object_vertex,
		vector<TriangleIndex> & triangle,
		vector<Vector2D<float>> & tex_vertex,
		vector<Texture> & texture,
		Frustum & frustum,
		float & scale,
		vector<Vertex> & world_vertex,
		Vector3D<float> & camera_pos,
		Vector3D<float> & camera_transform,
		Vector3D<float> & light_source,
		vector<Vector3D<float>> & normal,
		vector<Vector3D<float>> & world_normal,
		bool & is_gaussian_blur,
		bool & is_grey_scale,
		bool & is_pen,
		Bitmap<BGRA> & bg_image,
		Bitmap<BGRA> & rendered_buffer,
		Bitmap<BGRA> & pen_buffer
	) noexcept;
};

#endif