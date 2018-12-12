#include <windows.h>
#include <chrono>
#include "win32utility.h"
#include "mathlib3d.h"
#include "utility3d.h"
#include "message_handler.h"
#include "post_process.h"
#include "resource.h"

using std::string;
using std::vector;
using namespace std::chrono;
using namespace MATHLIB3D;
using namespace RASTERIZE;
using namespace UTILITY2D;
using namespace UTILITY3D;
using namespace MESSAGE_HANDLER;
using namespace WIN32_UTILITY;
using namespace POST_PROCESS;




LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) noexcept
{
	static Vector3D<float> camera_transform;
	static auto drawing_mode = DRAWING_MODE::BLINN_PHONG;
	static Vector3D<float> camera_pos;
	static Vector3D<float> light_source;

	static ScreenData screen_data;
	static Bitmap<BGRA> bg_image;
	static Bitmap<BGRA> rendered_buffer;
	static Bitmap<BGRA> pen_buffer;
	static float mouse_down_x = 0.0f, mouse_down_y = 0.0f;
	static float last_mouse_x = 0.0f, last_mouse_y = 0.0f;
	static Vector3D<float> rotate_axis = { 0.0f, 0.0f, 1.0f };
	static float rotate_degree = 0.0f;
	static bool mouse_leftkey_down = false;
	
	static int32_t ssaa_factor = 2;
	static bool is_ssaa = false;
	static bool is_gaussian_blur = false;
	static bool is_grey_scale = false;
	static bool is_pen = false;
	static HCURSOR pen_cursor = (HCURSOR)LoadImage(NULL, TEXT("./resource/aero_pen_xl.cur"), IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
	static HCURSOR default_cursor = (HCURSOR)LoadImage(NULL, TEXT("./resource/Rewrite.ani"), IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
	static int32_t pen_width = 1;

	static Frustum frustum;

	static vector<Vertex> object_vertex;
	static vector<Vertex> world_vertex;
	static vector<Vertex> camera_vertex;
	static vector<Vertex> homogeneous_clipping_vertex;
	static vector<Vertex> viewport_vertex;
	static vector<Vector3D<float>> normal;
	static vector<Vector3D<float>> world_normal;
	static vector<Vector3D<float>> camera_normal;
	static vector<TriangleIndex> triangle;
	static vector<Vector2D<float>> tex_vertex;
	static vector<Texture> texture;

	static float scale = 1.0f;

	if (message == WM_SIZE)
	{
		wm_size_handler
			(
				screen_data,
				pen_buffer,
				frustum,
				lParam,
				ssaa_factor,
				is_ssaa
				);
		return 0;
	}
	else if (message == WM_PAINT)
	{
		if (!screen_data.background_buffer.width || !screen_data.background_buffer.height) return 0;

		auto StartTime = high_resolution_clock::now();

		wm_paint_handler
		(
			hwnd,
			camera_vertex,
			camera_transform,
			world_vertex,
			triangle,
			rotate_axis,
			rotate_degree,
			camera_pos,
			light_source,
			frustum,
			homogeneous_clipping_vertex,
			scale,
			viewport_vertex,
			screen_data,
			drawing_mode,
			tex_vertex,
			texture,
			world_normal,
			camera_normal,
			bg_image
		);

		rendered_buffer = clone(screen_data.background_buffer);

		if (is_ssaa) rendered_buffer = ssaa_filter(rendered_buffer, ssaa_factor);

		for (auto j = 0; j < rendered_buffer.height; ++j)
		{
			for (auto i = 0; i < rendered_buffer.width; ++i)
			{
				if (!pen_buffer[j][i].a) continue;

				rendered_buffer[j][i] = pen_buffer[j][i];
				rendered_buffer[j][i].a = 0xff;
			}
		}

		if (is_gaussian_blur)
		{
			rendered_buffer = gaussian_blur(rendered_buffer);
		}
		if (is_grey_scale) rendered_buffer = grey_scale(rendered_buffer);

		

		auto width = rendered_buffer.width;
		auto height = rendered_buffer.height;
		auto bytes = rendered_buffer.bytes;

		HDCObject hdco(hwnd);
		HMemDCObject hdcMemo(hdco.hdc);
		HBITMAPObject hbitmapo(hdco.hdc, width, height);
		my_assert(SelectObject(hdcMemo.hdcMem, hbitmapo.hbitmap));
		if (width > 0 && height > 0)
		{
			my_assert(SetBitmapBits(hbitmapo.hbitmap, sizeof(*bytes) * width * height, bytes));
		}

		auto EndTime = high_resolution_clock::now();

		auto time_delta = (duration_cast<duration<float>>(EndTime - StartTime)).count();
		auto fps = get_fps(time_delta);
		//print_info(hdcMemo.hdcMem, fps, ssaa_factor, is_ssaa);

		my_assert(BitBlt(hdco.hdc, 0, 0, width, height, hdcMemo.hdcMem, 0, 0, SRCCOPY));

		return 0;
	}
	else if (message == WM_KEYDOWN)
	{
		wm_keydown_handler
			(
				wParam,
				light_source,
				rotate_axis,
				rotate_degree,
				drawing_mode,
				hwnd,
				lParam,
				world_vertex,
				triangle,
				tex_vertex,
				texture,
				frustum,
				camera_pos,
				camera_transform,
				scale,
				screen_data,
				ssaa_factor,
				is_ssaa,
				is_pen,
				world_normal,
				pen_width
				);
		return 0;
	}
	else if (message == WM_LBUTTONDOWN)
	{
		wm_lbuttondown_handler
			(
				mouse_down_x,
				mouse_down_y,
				last_mouse_x,
				last_mouse_y,
				mouse_leftkey_down,
				lParam,
				hwnd
				);
		return 0;
	}
	else if (message == WM_KEYUP)
	{
		wm_keyup_handler
			(
				world_vertex,
				rotate_axis,
				rotate_degree,
				world_normal
				);
		return 0;
	}
	else if (message == WM_LBUTTONUP)
	{
		wm_lbuttonup_handler
			(
				world_vertex,
				rotate_axis,
				rotate_degree,
				mouse_leftkey_down,
				world_normal
				);
		return 0;
	}
	else if (message == WM_MOUSEMOVE)
	{
		wm_mousemove_handler
			(
				mouse_leftkey_down,
				rotate_axis,
				rotate_degree,
				hwnd,
				lParam,
				mouse_down_x,
				mouse_down_y,
				last_mouse_x,
				last_mouse_y,
				is_pen,
				pen_buffer,
				pen_width
				);
		return 0;
	}
	else if (message == WM_MOUSEWHEEL)
	{
		wm_mousewheel_handler
			(
				camera_transform,
				frustum,
				wParam,
				hwnd
				);
		return 0;
	}
	else if (message == WM_COMMAND)
	{
		wm_command_handler
		(
			hwnd,
			pen_cursor,
			default_cursor,
			wParam,
			object_vertex,
			triangle,
			tex_vertex,
			texture,
			frustum,
			scale,
			world_vertex,
			camera_pos,
			camera_transform,
			light_source,
			normal,
			world_normal,
			is_gaussian_blur,
			is_grey_scale,
			is_pen,
			bg_image,
			rendered_buffer,
			pen_buffer
		);
		return 0;
	}
	else if (message == WM_DESTROY)
	{
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}




int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR szCmdLine, _In_ int iCmdShow)
{
	static const TCHAR szAppName[] = TEXT("Renderer");
	WndClassObject wco(hInstance, szAppName, WndProc);


	HMENU hMenu = CreateMenu(); // Main Menu
	
	/************* File Menu *************/
	HMENU hMenuPop = CreateMenu();
	AppendMenu(hMenuPop, MF_STRING, IDM_FILE_OPEN, TEXT("Open"));
	AppendMenu(hMenuPop, MF_STRING, IDM_FILE_SAVE, TEXT("Save"));
	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hMenuPop, TEXT("File"));
	
	/************* Edit Menu *************/
	hMenuPop = CreateMenu();
	AppendMenu(hMenuPop, MF_STRING, IDM_EDIT_GAUSSIAN_BLUR, TEXT("Gaussian Blur"));
	AppendMenu(hMenuPop, MF_STRING, IDM_EDIT_GREY_SCALE, TEXT("Grey Scale"));
	AppendMenu(hMenuPop, MF_STRING, IDM_EDIT_CLEAR, TEXT("Clear"));
	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hMenuPop, TEXT("Edit"));

	/************* Draw Menu *************/
	hMenuPop = CreateMenu();
	AppendMenu(hMenuPop, MF_STRING, IDM_DRAW_PEN, TEXT("Pen"));
	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hMenuPop, TEXT("Draw"));

	HwndObject ho(hInstance, szAppName, hMenu, iCmdShow);


	MSG msg;
	for (;;)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		InvalidateRect(ho, nullptr, false);
		UpdateWindow(ho);
	}

	return 0;
}