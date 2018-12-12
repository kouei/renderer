#include<Windowsx.h>
#include<thread>
#include<iterator>
#include<algorithm>
#include"message_handler.h"
#include"file_utility.h"
#include"string_utility.h"
#include"wic_utility.h"
#include"transform.h"
#include"win32utility.h"
#include"rasterize.h"
#include "resource.h"
#include "post_process.h"

using namespace FILE_UTILITY;
using namespace STRING_UTILITY;
using namespace BITMAP_UTILITY;
using namespace UTILITY2D;
using namespace WIC_UTILITY;
using namespace TRANSFORM;
using namespace WIN32_UTILITY;
using namespace RASTERIZE;
using namespace MESSAGE_HANDLER;
using namespace POST_PROCESS;
using std::max;
using std::min;
using std::move;
using std::swap;
using std::basic_string;
using std::thread;
using std::numeric_limits;
using std::advance;


void MESSAGE_HANDLER::read_mesh_file
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
) noexcept
{
	FileObject mesh(mesh_file, FILE_MODE::READ);

	int vertex_count;
	my_assert(fscanf(mesh, "%d", &vertex_count) == 1);
	object_vertex.clear();
	for (int i = 0; i < vertex_count; ++i)
	{
		Vertex v;
		my_assert(fscanf(mesh, "%f%f%f", &v.pos.x, &v.pos.y, &v.pos.z) == 3);
		object_vertex.push_back(v);
	}

	int tex_vertex_count;
	my_assert(fscanf(mesh, "%d", &tex_vertex_count) == 1);
	tex_vertex.clear();

	for (int i = 0; i < tex_vertex_count; ++i)
	{
		Vector2D<float> tv;
		my_assert(fscanf(mesh, "%f%f", &tv.x, &tv.y) == 2);
		tex_vertex.push_back(tv);
	}

	int normal_count;
	my_assert(fscanf(mesh, "%d", &normal_count) == 1);
	normal.clear();

	for (int i = 0; i < normal_count; ++i)
	{
		Vector3D<float> n;
		my_assert(fscanf(mesh, "%f%f%f", &n.x, &n.y, &n.z) == 3);
		normal.push_back(n);
	}

	int triangle_count;
	my_assert(fscanf(mesh, "%d", &triangle_count) == 1);
	triangle.clear();

	for (int i = 0; i < triangle_count; ++i)
	{
		TriangleIndex t;
		my_assert(fscanf(mesh, "%d%d%d%d%d%d%d%d%d%d", &t.vertex_index[0], &t.vertex_index[1], &t.vertex_index[2], &t.texture_index[0], &t.texture_index[1], &t.texture_index[2], &t.normal_index[0], &t.normal_index[1], &t.normal_index[2], &t.texture_id) == 10);
		triangle.push_back(t);
	}
	int tex_file_count;
	my_assert(fscanf(mesh, "%d", &tex_file_count) == 1);
	fgetc(mesh);
	texture.resize(tex_file_count);
	for (int i = 0; i < tex_file_count; ++i)
	{
		int c;
		string current_line;
		Texture current_texture;
		for (;;)
		{
			c = fgetc(mesh.file);
			current_line += c == EOF ? '\n' : static_cast<char>(c);

			if (c == EOF || c == '\n')
			{
				current_line = remove_extra_white_space(current_line);
				int pos = 0;
				my_assert(sscanf(&current_line[pos], "%d", &current_texture.texture_info.id) == 1);
				while (pos < current_line.size() && current_line[pos] != ' ' && current_line[pos] != '\0') ++pos;
				++pos;
				while (pos < current_line.size() && current_line[pos] != ' ' && current_line[pos] != '\0')
				{
					current_texture.texture_info.name += current_line[pos];
					++pos;
				}
				++pos;
				my_assert(sscanf(&current_line[pos], "%f", &current_texture.texture_info.Ka[0]) == 1);
				while (pos < current_line.size() && current_line[pos] != ' ' && current_line[pos] != '\0') ++pos;
				++pos;
				my_assert(sscanf(&current_line[pos], "%f", &current_texture.texture_info.Ka[1]) == 1);
				while (pos < current_line.size() && current_line[pos] != ' ' && current_line[pos] != '\0') ++pos;
				++pos;
				my_assert(sscanf(&current_line[pos], "%f", &current_texture.texture_info.Ka[2]) == 1);
				while (pos < current_line.size() && current_line[pos] != ' ' && current_line[pos] != '\0') ++pos;
				++pos;
				my_assert(sscanf(&current_line[pos], "%f", &current_texture.texture_info.Kd[0]) == 1);
				while (pos < current_line.size() && current_line[pos] != ' ' && current_line[pos] != '\0') ++pos;
				++pos;
				my_assert(sscanf(&current_line[pos], "%f", &current_texture.texture_info.Kd[1]) == 1);
				while (pos < current_line.size() && current_line[pos] != ' ' && current_line[pos] != '\0') ++pos;
				++pos;
				my_assert(sscanf(&current_line[pos], "%f", &current_texture.texture_info.Kd[2]) == 1);
				while (pos < current_line.size() && current_line[pos] != ' ' && current_line[pos] != '\0') ++pos;
				++pos;
				my_assert(sscanf(&current_line[pos], "%f", &current_texture.texture_info.Ks[0]) == 1);
				while (pos < current_line.size() && current_line[pos] != ' ' && current_line[pos] != '\0') ++pos;
				++pos;
				my_assert(sscanf(&current_line[pos], "%f", &current_texture.texture_info.Ks[1]) == 1);
				while (pos < current_line.size() && current_line[pos] != ' ' && current_line[pos] != '\0') ++pos;
				++pos;
				my_assert(sscanf(&current_line[pos], "%f", &current_texture.texture_info.Ks[2]) == 1);
				while (pos < current_line.size() && current_line[pos] != ' ' && current_line[pos] != '\0') ++pos;
				++pos;
				my_assert(sscanf(&current_line[pos], "%f", &current_texture.texture_info.d) == 1);
				while (pos < current_line.size() && current_line[pos] != ' ' && current_line[pos] != '\0') ++pos;
				++pos;
				my_assert(sscanf(&current_line[pos], "%f", &current_texture.texture_info.Ns) == 1);
				while (pos < current_line.size() && current_line[pos] != ' ' && current_line[pos] != '\0') ++pos;
				++pos;
				my_assert(sscanf(&current_line[pos], "%d", &current_texture.texture_info.illum) == 1);
				while (pos < current_line.size() && current_line[pos] != ' ' && current_line[pos] != '\0') ++pos;
				++pos;
				while (pos < current_line.size() && current_line[pos] != ' ' && current_line[pos] != '\0')
				{
					current_texture.texture_info.map_Ka += current_line[pos];
					++pos;
				}
				++pos;
				while (pos < current_line.size() && current_line[pos] != ' ' && current_line[pos] != '\0')
				{
					current_texture.texture_info.map_Kd += current_line[pos];
					++pos;
				}
				++pos;
				while (pos < current_line.size() && current_line[pos] != ' ' && current_line[pos] != '\0')
				{
					current_texture.texture_info.map_Ks += current_line[pos];
					++pos;
				}

				break;
			}
		}
		texture[current_texture.texture_info.id] = move(current_texture);
	}

	//making near plane and far plane very far can make the fov very small,
	//and thus eliminate the distortion of the object which is away from the center of the frustum
	my_assert(fscanf(mesh, "%f%f%f%f%f%f", &frustum.near_plane, &frustum.far_plane, &frustum.left_plane, &frustum.right_plane, &frustum.top_plane, &frustum.bottom_plane) == 6);
	my_assert(fscanf(mesh, "%f%f%f", &light_source.x, &light_source.y, &light_source.z) == 3);
	my_assert(fscanf(mesh, "%f%f%f", &camera_pos.x, &camera_pos.y, &camera_pos.z) == 3);
	my_assert(fscanf(mesh, "%f%f%f", &camera_transform.x, &camera_transform.y, &camera_transform.z) == 3);
	my_assert(fscanf(mesh, "%f", &scale) == 1);
	//frustum.n = 1000.0f;
	//frustum.f = 1012.0f;
}

//void MESSAGE_HANDLER::save_mesh_file
//(
//	vector<Vertex> & object_vertex,
//	vector<TriangleIndex> & triangle,
//	vector<Vector2D<float>> & tex_vertex,
//	vector<Texture> & texture,
//	Frustum & frustum,
//	Vector3D<float> & light_source,
//	Vector3D<float> & camera_pos,
//	Vector3D<float> & camera_transform,
//	float & scale,
//	const vector<Vector3D<float>> & world_normal
//) noexcept
//{
//	FileObject mesh(mesh_file, FILE_MODE::WRITE);
//	fprintf(mesh, "%llu\n", object_vertex.size());
//	for (auto & v : object_vertex) fprintf(mesh, "%f %f %f\n", v.pos.x, v.pos.y, v.pos.z);
//
//	fprintf(mesh, "%llu\n", tex_vertex.size());
//	for (auto & tv : tex_vertex) fprintf(mesh, "%f %f\n", tv.x, tv.y);
//
//	fprintf(mesh, "%llu\n", world_normal.size());
//	for (auto & n : world_normal) fprintf(mesh, "%f %f %f\n", n.x, n.y, n.z);
//
//	fprintf(mesh, "%llu\n", triangle.size());
//	for (auto & t : triangle) fprintf(mesh, "%d %d %d %d %d %d %d %d %d %d\n", t.vertex_index[0], t.vertex_index[1], t.vertex_index[2], t.texture_index[0], t.texture_index[1], t.texture_index[2], t.normal_index[0], t.normal_index[1], t.normal_index[2], t.texture_id);
//
//	fprintf(mesh, "%llu\n", texture.size());
//	for (auto & tex : texture)
//	{
//		auto & texture_info = tex.texture_info;
//		fprintf
//		(
//			mesh,
//			"%d %s %f %f %f %f %f %f %f %f %f %f %f %d %s %s %s\n",
//			texture_info.id,
//			texture_info.name.empty() ? "<empty>" : texture_info.name.c_str(),
//			texture_info.Ka[0], texture_info.Ka[1], texture_info.Ka[2],
//			texture_info.Kd[0], texture_info.Kd[1], texture_info.Kd[2],
//			texture_info.Ks[0], texture_info.Ks[1], texture_info.Ks[2],
//			texture_info.d,
//			texture_info.Ns,
//			texture_info.illum,
//			texture_info.map_Ka.empty() ? "<empty>" : texture_info.map_Ka.c_str(),
//			texture_info.map_Kd.empty() ? "<empty>" : texture_info.map_Kd.c_str(),
//			texture_info.map_Ks.empty() ? "<empty>" : texture_info.map_Ks.c_str()
//		);
//	}
//
//	fprintf(mesh, "%f %f %f %f %f %f\n", frustum.near_plane, frustum.far_plane, frustum.left_plane, frustum.right_plane, frustum.top_plane, frustum.bottom_plane);
//	fprintf(mesh, "%f %f %f\n", light_source.x, light_source.y, light_source.z);
//	fprintf(mesh, "%f %f %f\n", camera_pos.x, camera_pos.y, camera_pos.z);
//	fprintf(mesh, "%f %f %f\n", camera_transform.x, camera_transform.y, camera_transform.z);
//	fprintf(mesh, "%f\n", scale);
//
//}

void MESSAGE_HANDLER::print_info(HDC hdc, float fps, int32_t ssaa_factor, bool is_ssaa) noexcept
{
	basic_string<TCHAR> to_print;
	TEXTMETRIC tm;

	SetBkMode(hdc, TRANSPARENT);
	GetTextMetrics(hdc, &tm);
	int current_y = 0;

	to_print = to_tstring(TEXT("FPS:"), fps);
	TextOut(hdc, 0, current_y, to_print.c_str(), static_cast<int>(to_print.size()));
	to_print = TEXT("Move Object: WASD↑↓");
	TextOut(hdc, 0, current_y += tm.tmHeight, to_print.c_str(), static_cast<int>(to_print.size()));
	to_print = TEXT("Move Light Source: IKJLYH");
	TextOut(hdc, 0, current_y += tm.tmHeight, to_print.c_str(), static_cast<int>(to_print.size()));
	to_print = TEXT("Scale Image: +-");
	TextOut(hdc, 0, current_y += tm.tmHeight, to_print.c_str(), static_cast<int>(to_print.size()));
	to_print = TEXT("Change Drawing Model: Tab");
	TextOut(hdc, 0, current_y += tm.tmHeight, to_print.c_str(), static_cast<int>(to_print.size()));
	to_print = TEXT("Turn ON/OFF SSAA: X");
	TextOut(hdc, 0, current_y += tm.tmHeight, to_print.c_str(), static_cast<int>(to_print.size()));
	to_print = to_tstring("SSAA Factor: ", ssaa_factor);
	TextOut(hdc, 0, current_y += tm.tmHeight, to_print.c_str(), static_cast<int>(to_print.size()));
	to_print = is_ssaa ? TEXT("SSAA: ON") : TEXT("SSAA: OFF");
	TextOut(hdc, 0, current_y += tm.tmHeight, to_print.c_str(), static_cast<int>(to_print.size()));
}

Bitmap<BGRA> MESSAGE_HANDLER::load_bitmap(const string & path) noexcept
{
	static WicImagingFactoryObject wic_image_factory;

	auto tpath = to_tstring(path);
	auto wic_bitmap_decoder = WicBitmapDecoderObject(tpath.c_str(), wic_image_factory);
	auto wic_bitmap_frame_decode = WicBitmapFrameDecodeObject(wic_bitmap_decoder);
	WICPixelFormatGUID wicpf_guid;
	wic_bitmap_frame_decode->GetPixelFormat(&wicpf_guid);
	
	UINT width, height;
	wic_bitmap_frame_decode->GetSize(&width, &height);
	
	if (wicpf_guid == GUID_WICPixelFormat32bppBGRA)
	{
		auto bitmap = Bitmap<BGRA>(width, height);
		wic_bitmap_frame_decode->CopyPixels
		(
			nullptr,
			width * sizeof(bitmap[0][0]),
			static_cast<UINT>(width * height * sizeof(bitmap[0][0])),
			reinterpret_cast<BYTE *>(&bitmap[0][0])
		);

		return bitmap;
	}
	else if (wicpf_guid == GUID_WICPixelFormat24bppBGR)
	{
		auto bitmap = Bitmap<BGR>(width, height);
		wic_bitmap_frame_decode->CopyPixels
		(
			nullptr,
			width * sizeof(bitmap[0][0]),
			static_cast<UINT>(width * height * sizeof(bitmap[0][0])),
			reinterpret_cast<BYTE *>(&bitmap[0][0])
		);

		auto bitmap_32bit = Bitmap<BGRA>(width, height);
		for (auto j = 0; j < bitmap_32bit.height; ++j)
		{
			for (auto i = 0; i < bitmap_32bit.width; ++i)
			{
				bitmap_32bit[j][i].b = bitmap[j][i].b;
				bitmap_32bit[j][i].g = bitmap[j][i].g;
				bitmap_32bit[j][i].r = bitmap[j][i].r;
				bitmap_32bit[j][i].a = 0xff;
			}
		}

		return bitmap_32bit;
	}
	else
	{
		my_assert("Invalid Color Type" && false);
		return Bitmap<BGRA>();
	}
}

void MESSAGE_HANDLER::save_bitmap(const string & path, Bitmap<BGRA> & bitmap) noexcept
{
	IWICImagingFactory *piFactory = NULL;
	IWICBitmapEncoder *piEncoder = NULL;
	IWICBitmapFrameEncode *piBitmapFrame = NULL;
	IPropertyBag2 *pPropertybag = NULL;

	IWICStream *piStream = NULL;

	HRESULT hr = CoCreateInstance(
		CLSID_WICImagingFactory,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_IWICImagingFactory,
		(LPVOID*)&piFactory);

	my_assert(SUCCEEDED(hr));
	hr = piFactory->CreateStream(&piStream);

	my_assert(SUCCEEDED(hr));
	hr = piStream->InitializeFromFilename(L"output.png", GENERIC_WRITE);

	my_assert(SUCCEEDED(hr));
	hr = piFactory->CreateEncoder(GUID_ContainerFormatPng, NULL, &piEncoder);

	my_assert(SUCCEEDED(hr));
	hr = piEncoder->Initialize(piStream, WICBitmapEncoderNoCache);

	my_assert(SUCCEEDED(hr));
	hr = piEncoder->CreateNewFrame(&piBitmapFrame, &pPropertybag);

	my_assert(SUCCEEDED(hr));
	hr = piBitmapFrame->Initialize(pPropertybag);

	my_assert(SUCCEEDED(hr));
	hr = piBitmapFrame->SetSize(bitmap.width, bitmap.height);

	WICPixelFormatGUID formatGUID = GUID_WICPixelFormat32bppBGRA;
	my_assert(SUCCEEDED(hr));
	hr = piBitmapFrame->SetPixelFormat(&formatGUID);

	my_assert(SUCCEEDED(hr));
	// We're expecting to write out 24bppRGB. Fail if the encoder cannot do it.
	hr = IsEqualGUID(formatGUID, GUID_WICPixelFormat32bppBGRA) ? S_OK : E_FAIL;

	my_assert(SUCCEEDED(hr));
	//UINT cbStride = uiWidth * 4/***WICGetStride***/;
	//UINT cbBufferSize = uiHeight * cbStride;

	//BYTE *pbBuffer = new BYTE[cbBufferSize];

	//if (pbBuffer != NULL)
	//{
	//	for (UINT i = 0; i < cbBufferSize; i++)
	//	{
	//		pbBuffer[i] = static_cast<BYTE>(rand());
	//	}

	//	hr = piBitmapFrame->WritePixels(uiHeight, cbStride, cbBufferSize, pbBuffer);

	//	delete[] pbBuffer;
	//}
	//else
	//{
	//	hr = E_OUTOFMEMORY;
	//}

	hr = piBitmapFrame->WritePixels(bitmap.height, bitmap.width * sizeof(BGRA), bitmap.height * bitmap.width * sizeof(BGRA), reinterpret_cast<BYTE *>(&bitmap[0][0]));

	my_assert(SUCCEEDED(hr));
	hr = piBitmapFrame->Commit();

	my_assert(SUCCEEDED(hr));
	hr = piEncoder->Commit();

	if (piFactory)
		piFactory->Release();

	if (piBitmapFrame)
		piBitmapFrame->Release();

	if (piEncoder)
		piEncoder->Release();

	if (piStream)
		piStream->Release();
}


void MESSAGE_HANDLER::load_texture(const string & mesh_file, vector<Texture> & texture) noexcept
{

	auto current_directory = get_directory(mesh_file);

	for (auto & tex : texture)
	{
		auto & map_Ka = tex.texture_info.map_Ka;
		auto & map_Kd = tex.texture_info.map_Kd;
		auto & map_Ks = tex.texture_info.map_Ks;
		auto & map_Ka_bits = tex.texture_bits.map_Ka_bits;
		auto & map_Kd_bits = tex.texture_bits.map_Kd_bits;
		auto & map_Ks_bits = tex.texture_bits.map_Ks_bits;
		if (map_Ka != "<empty>")
		{
			map_Ka_bits = load_bitmap(current_directory + map_Ka);
		}
		if (map_Kd != "<empty>")
		{
			map_Kd_bits = load_bitmap(current_directory + map_Kd);
		}
		if (map_Ks != "<empty>")
		{
			map_Ks_bits = load_bitmap(current_directory + map_Ks);
		}
	}

}

void MESSAGE_HANDLER::wm_size_handler
(
	ScreenData & screen_data,
	Bitmap<BGRA> & pen_buffer,
	Frustum & frustum,
	LPARAM lParam,
	int32_t ssaa_factor,
	bool is_ssaa
) noexcept
{
	auto cxClient = LOWORD(lParam);
	auto cyClient = HIWORD(lParam);

	if (is_ssaa) screen_data.resize(cxClient * ssaa_factor, cyClient * ssaa_factor);
	else screen_data.resize(cxClient, cyClient);
	pen_buffer.resize(cxClient, cyClient);
	for (auto j = 0; j < pen_buffer.height; ++j)
	{
		for (auto i = 0; i < pen_buffer.width; ++i)
		{
			pen_buffer[j][i] = BGRA{ 0x0, 0x0, 0x0, 0x0 };
		}
	}
}

float MESSAGE_HANDLER::get_fps(float time_delta) noexcept
{
	static auto frame_count = 0;
	static auto time_count = 0.0f;
	++frame_count;
	time_count += time_delta;
	float fps = frame_count / time_count;
	if (time_count >= 1.0f)
	{
		time_count = 0.0f;
		frame_count = 0;
	}
	return fps;
}

void MESSAGE_HANDLER::wm_paint_handler
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
) noexcept
{
	auto width = screen_data.background_buffer.width;
	auto height = screen_data.background_buffer.height;

	/******************** Clear BackGround ********************/
	thread back_ground_clear_thread
	(
		[&screen_data, width, height, &bg_image]
	()
	{
		draw_background(screen_data.background_buffer, bg_image);
		init_z_depth_buf(screen_data.z_depth_buffer, (numeric_limits<float>::max)());
	}
	);

	/******************** World to Camera ********************/
	camera_vertex.clear();
	for (const auto & wv : world_vertex)
	{
		auto v = wv;
		v.pos = rotate(v.pos, rotate_axis, rotate_degree) + camera_transform;
		camera_vertex.push_back(v);
	}
	camera_normal.clear();
	for (const auto & n : world_normal)
	{
		camera_normal.push_back(rotate(n, rotate_axis, rotate_degree));
	}


	/******************** Calculate Lighting Related Vector ********************/
	thread calculate_lighting_related_vector_thread 
	(
		[&camera_vertex, &camera_pos, &light_source]() 
		{
			for (auto & v : camera_vertex) 
			{
				v.to_light_source = light_source - v.pos;
				auto to_camera = camera_pos - v.pos;
				v.halfway = normalization(to_camera) + normalization(v.to_light_source);
			}
		}
	);
	calculate_lighting_related_vector_thread.join();


	
	/******************** Camera to Homogeneous Clipping ********************/
	Matrix4D<float> matrix_to_homogeneous_clipping
	{
		2.0f * frustum.near_plane / (frustum.right_plane - frustum.left_plane), 0.0f, -(frustum.right_plane + frustum.left_plane) / (frustum.right_plane - frustum.left_plane), 0.0f,
		0.0f, 2.0f * frustum.near_plane / (frustum.top_plane - frustum.bottom_plane), -(frustum.top_plane + frustum.bottom_plane) / (frustum.top_plane - frustum.bottom_plane), 0.0f,
		0.0f, 0.0f, -(frustum.near_plane + frustum.far_plane) / (frustum.near_plane - frustum.far_plane), 2.0f * frustum.far_plane * frustum.near_plane / (frustum.near_plane - frustum.far_plane),
		0.0f, 0.0f, 1.0f, 0.0f
	};

	homogeneous_clipping_vertex.clear();
	for (const auto & v : camera_vertex)
	{
		Vector3D<float> homogeneous_clipping_pos;
		homogeneous_clipping_pos.x = dot(v.pos, matrix_to_homogeneous_clipping[0].v3d) /*+ 0.0f*/;
		homogeneous_clipping_pos.y = dot(v.pos, matrix_to_homogeneous_clipping[1].v3d) /*+ 0.0f*/;
		homogeneous_clipping_pos.z = dot(v.pos, matrix_to_homogeneous_clipping[2].v3d) + matrix_to_homogeneous_clipping[2].w;
		homogeneous_clipping_pos /= v.pos.z;
		homogeneous_clipping_vertex.push_back(Vertex{ homogeneous_clipping_pos, v.to_light_source, v.halfway });
	}


	/******************** Homogeneous Clipping to Viewport ********************/
	thread homogeneous_clipping_to_viewport_thread
	(
			[
				&viewport_vertex,
				&homogeneous_clipping_vertex,
				&matrix_to_homogeneous_clipping,
				&frustum,
				width,
				height,
				scale
			]()
		{
			viewport_vertex.clear();
			for (const auto & v : homogeneous_clipping_vertex)
			{
				auto x_scale = width / abs(frustum.right_plane - frustum.left_plane);
				auto y_scale = height / abs(frustum.top_plane - frustum.bottom_plane);
				auto client_scale = min<float>(x_scale, y_scale);

				Vertex result;
				result.pos = Vector3D<float>
					(
						scale * client_scale * ((v.pos.x + 1.0f) * (frustum.right_plane - frustum.left_plane) * 0.5f + frustum.left_plane) + width / 2.0f,
						scale * client_scale * ((v.pos.y + 1.0f) * (frustum.top_plane - frustum.bottom_plane) * 0.5f + frustum.bottom_plane) + height / 2.0f,
						1.0f / ((v.pos.z - matrix_to_homogeneous_clipping[2][2]) / matrix_to_homogeneous_clipping[2][3]) //recover the z coordinate in camera space
						);
				result.to_light_source = v.to_light_source;
				result.halfway = v.halfway;
				viewport_vertex.push_back(result);
			}
		}
	);


	/******************** Homogeneous Clipping Culling ********************/
	int keep_triangle_count = 0;
	for(size_t i = 0; i < triangle.size(); ++i)
	{
		bool to_keep = true;
		for (auto vi : triangle[i].vertex_index)
		{
			if (!to_keep) break;
			for (auto c : homogeneous_clipping_vertex[vi].pos.a)
			{
				if (abs(c) > 1.0f)
				{
					to_keep = false;
					break;
				}
			}
		}

		if (to_keep) swap(triangle[keep_triangle_count++], triangle[i]);
	}

	//auto HomogeneousClippingCullingEnd = partition(triangle.begin(), triangle.end(), HomogeneousClippingCulling(&homogeneous_clipping_vertex));

	homogeneous_clipping_to_viewport_thread.join();
	back_ground_clear_thread.join();






	/******************** New Rendering Method ********************/

	auto threads_count = max<int>(thread::hardware_concurrency(), 2);
	vector<vector<RenderJob>> queue(threads_count);
	for (int i = 0; i < keep_triangle_count; ++i)
	{
		assign_job(queue, triangle[i], viewport_vertex);
	}

	vector<thread> threads(threads_count);
	for (int i = 0; i < threads_count; ++i)
	{
		threads[i] = thread
		(
				[&queue, &viewport_vertex, &tex_vertex, &camera_normal, &texture, &screen_data, i]()
			{
				for (const auto & rj : queue[i])
				{
					line_blinn_phong(rj, viewport_vertex, tex_vertex, camera_normal, texture, screen_data);
				}
			}
		);
	}
	for (auto & t : threads) t.join();

}

void MESSAGE_HANDLER::wm_keydown_handler
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
) noexcept
{
	auto step = 5.0f;
	if (wParam == 'I')
	{
		light_source.y -= step;
	}
	else if (wParam == 'K')
	{
		light_source.y += step;
	}
	else if (wParam == 'J')
	{
		light_source.x -= step;
	}
	else if (wParam == 'L')
	{
		light_source.x += step;
	}
	else if (wParam == 'Y')
	{
		light_source.z += step;
	}
	else if (wParam == 'H')
	{
		light_source.z -= step;
	}
	else if (wParam == 'W')
	{
		camera_transform.y -= step;
	}
	else if (wParam == 'S')
	{
		camera_transform.y += step;
	}
	else if (wParam == 'A')
	{
		camera_transform.x -= step;
	}
	else if (wParam == 'D')
	{
		camera_transform.x += step;
	}
	else if (wParam == 'X')
	{
		is_ssaa = !is_ssaa;
		RECT rc;
		GetClientRect(hwnd, &rc);
		auto lParam = (rc.bottom << 16) | rc.right;
		SendMessage(hwnd, WM_SIZE, NULL, lParam);
	}
	else if ('1' <= wParam && wParam <= '8')
	{
		ssaa_factor = static_cast<int32_t>(wParam) - '0';
		RECT rc;
		GetClientRect(hwnd, &rc);
		auto lParam = (rc.bottom << 16) | rc.right;
		SendMessage(hwnd, WM_SIZE, NULL, lParam);
	}
	else if (wParam == VK_ADD)
	{
		if (is_pen) ++pen_width;
		else scale += 0.1f;
	}
	else if (wParam == VK_SUBTRACT)
	{
		if (is_pen) pen_width = max<int32_t>(1, pen_width - 1);
		else scale -= 0.1f;
	}
	else if (wParam == VK_LEFT)
	{
		rotate_axis = { 0.0f, 0.0f, 1.0f };
		rotate_degree -= 1.0f;
		if (rotate_degree < -360.0f) rotate_degree += 360.0f;
	}
	else if (wParam == VK_RIGHT)
	{
		rotate_axis = { 0.0f, 0.0f, 1.0f };
		rotate_degree += 1.0f;
		if (rotate_degree > 360.0f) rotate_degree -= 360.0f;
	}
	else if (wParam == VK_UP)
	{
		camera_transform.z -= step;
	}
	else if (wParam == VK_DOWN)
	{
		camera_transform.z += step;
	}
	else if (wParam == VK_TAB)
	{
		if (drawing_mode == DRAWING_MODE::BLINN_PHONG) drawing_mode = DRAWING_MODE::LINE_FRAME;
		else if (drawing_mode == DRAWING_MODE::LINE_FRAME) drawing_mode = DRAWING_MODE::BLINN_PHONG;
		else my_assert("Unknown Drawing Mode" && false);
	}
	else if (wParam == VK_ESCAPE)
	{
		SendMessage(hwnd, WM_DESTROY, wParam, lParam);
	}
	else if (wParam == VK_CONTROL)
	{
		/*save_mesh_file
		(
			world_vertex,
			triangle,
			tex_vertex,
			texture,
			frustum,
			light_source,
			camera_pos,
			camera_transform,
			scale,
			world_normal
		);*/
	}


	my_assert(InvalidateRect(hwnd, nullptr, false));
	my_assert(UpdateWindow(hwnd));
}

void MESSAGE_HANDLER::wm_lbuttondown_handler
(
	float & mouse_down_x,
	float & mouse_down_y,
	float & last_mouse_x,
	float & last_mouse_y,
	bool & mouse_leftkey_down,
	LPARAM lParam,
	HWND hwnd
) noexcept
{
	mouse_down_x = last_mouse_x = static_cast<float>(GET_X_LPARAM(lParam));
	mouse_down_y = last_mouse_y = static_cast<float>(GET_Y_LPARAM(lParam));
	mouse_leftkey_down = true;
	SetCapture(hwnd);
}

void MESSAGE_HANDLER::wm_keyup_handler
(
	vector<Vertex> & world_vertex,
	Vector3D<float> & rotate_axis,
	float & rotate_degree,
	vector<Vector3D<float>> & world_normal
) noexcept
{
	for (auto & v : world_vertex)
	{
		v.pos = rotate(v.pos, rotate_axis, rotate_degree);
	}
	for (auto & n : world_normal)
	{
		n = rotate(n, rotate_axis, rotate_degree);
	}
	rotate_axis = Vector3D<float>{ 0.0f, 0.0f, 1.0f, };
	rotate_degree = 0.0f;
}

void MESSAGE_HANDLER::wm_lbuttonup_handler
(
	vector<Vertex> & world_vertex,
	Vector3D<float> & rotate_axis,
	float & rotate_degree,
	bool & mouse_leftkey_down,
	vector<Vector3D<float>> & world_normal
) noexcept
{
	mouse_leftkey_down = false;
	for (auto & v : world_vertex)
	{
		v.pos = rotate(v.pos, rotate_axis, rotate_degree);
	}
	for (auto & n : world_normal)
	{
		n = rotate(n, rotate_axis, rotate_degree);
	}
	rotate_axis = Vector3D<float>{ 0.0f, 0.0f, 1.0f };
	rotate_degree = 0.0f;
	my_assert(ReleaseCapture());
}

void MESSAGE_HANDLER::wm_mousemove_handler
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
) noexcept
{
	if (mouse_leftkey_down)
	{
		auto current_mouse_x = static_cast<float>(GET_X_LPARAM(lParam));
		auto current_mouse_y = static_cast<float>(GET_Y_LPARAM(lParam));
		if (is_pen)
		{
			bresenham_line_to(last_mouse_x, last_mouse_y, current_mouse_x, current_mouse_y, pen_width, pen_buffer);
			last_mouse_x = current_mouse_x;
			last_mouse_y = current_mouse_y;
		}
		else
		{
			Vector3D<float> v = { current_mouse_x - mouse_down_x, current_mouse_y - mouse_down_y, 0.0f };
			Vector3D<float> z = { 0.0f, 0.0f, 1.0f };
			auto v_dot_v = dot(v, v);
			if (v_dot_v > 0.0f)
			{
				rotate_axis = normalization(cross(v, z));
				rotate_degree = sqrt(v_dot_v) / 5.0f;
			}
		}
	}
}

void MESSAGE_HANDLER::wm_mousewheel_handler
(
	Vector3D<float> & camera_transform,
	Frustum & frustum,
	WPARAM wParam,
	HWND hwnd
) noexcept
{
	camera_transform.z -= 0.2f * GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
}

void MESSAGE_HANDLER::wm_command_handler
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
) noexcept
{
	if (LOWORD(wParam) == IDM_FILE_OPEN)
	{
		OPENFILENAME ofn = {};
		TCHAR strFilename[MAX_PATH] = {};
		ofn.lStructSize = sizeof(ofn);//结构体大小
		ofn.hwndOwner = NULL;//拥有着窗口句柄，为NULL表示对话框是非模态的，实际应用中一般都要有这个句柄
		ofn.lpstrFilter = TEXT("所有文件\0*.*\0PNG\0*.png\0Model\0*.txt\0\0");//设置过滤
		ofn.nFilterIndex = 1;//过滤器索引
		ofn.lpstrFile = strFilename;//接收返回的文件名，注意第一个字符需要为NULL
		ofn.nMaxFile = sizeof(strFilename);//缓冲区长度
		ofn.lpstrInitialDir = nullptr;//初始目录为默认
		ofn.lpstrTitle = nullptr;// TEXT("请选择一个文件");//使用系统默认标题留空即可
		ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;//文件、目录必须存在，隐藏只读选项

		if (!GetOpenFileName(&ofn)) return;
		
		auto filename = STRING_UTILITY::to_string(strFilename);
		if (filename.size() >= 3 && filename.substr(filename.size() - 3) == "txt")
		{
			read_mesh_file(filename, object_vertex, triangle, tex_vertex, texture, frustum, light_source, camera_pos, camera_transform, scale, normal);
			load_texture(filename, texture);
			world_vertex = object_vertex;
			world_normal = normal;
			bg_image = Bitmap<BGRA>();
		}
		else if (
			filename.size() >= 3 && 
			(
				filename.substr(filename.size() - 3) == "png" ||
				filename.substr(filename.size() - 3) == "jpg" ||
				filename.substr(filename.size() - 3) == "bmp"
				)
			)
		{
			object_vertex.clear();
			world_vertex.clear();
			triangle.clear();
			tex_vertex.clear();
			normal.clear();
			bg_image = load_bitmap(filename);
		}
	}
	else if (LOWORD(wParam) == IDM_FILE_SAVE)
	{
		OPENFILENAME ofn = {};
		TCHAR strFilename[MAX_PATH] = {};
		ofn.lStructSize = sizeof(ofn);//结构体大小
		ofn.hwndOwner = NULL;//拥有着窗口句柄，为NULL表示对话框是非模态的，实际应用中一般都要有这个句柄
		ofn.lpstrFilter = TEXT("所有文件\0*.*\0PNG\0*.png\0\0");//设置过滤
		ofn.nFilterIndex = 1;//过滤器索引
		ofn.lpstrFile = strFilename;//接收返回的文件名，注意第一个字符需要为NULL
		ofn.nMaxFile = sizeof(strFilename);//缓冲区长度
		ofn.lpstrInitialDir = nullptr;//初始目录为默认
		ofn.lpstrTitle = nullptr;// TEXT("请选择一个文件");//使用系统默认标题留空即可
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;//目录必须存在，覆盖文件前发出警告  
		ofn.lpstrDefExt = TEXT("png");

		if (!GetSaveFileName(&ofn)) return;

		auto filename = STRING_UTILITY::to_string(strFilename);

		auto save_rendered_buffer = clone(rendered_buffer);
		save_bitmap(filename, save_rendered_buffer);
	}
	else if (LOWORD(wParam) == IDM_EDIT_GAUSSIAN_BLUR)
	{
		is_gaussian_blur = !is_gaussian_blur;
		CheckMenuItem(GetMenu(hwnd), IDM_EDIT_GAUSSIAN_BLUR, is_gaussian_blur ? MF_CHECKED : MF_UNCHECKED);
	}
	else if (LOWORD(wParam) == IDM_EDIT_GREY_SCALE)
	{
		is_grey_scale = !is_grey_scale;
		CheckMenuItem(GetMenu(hwnd), IDM_EDIT_GREY_SCALE, is_grey_scale ? MF_CHECKED : MF_UNCHECKED);
	}
	else if (LOWORD(wParam) == IDM_EDIT_CLEAR)
	{
		for (auto j = 0; j < pen_buffer.height; ++j)
		{
			for (auto i = 0; i < pen_buffer.width; ++i)
			{
				pen_buffer[j][i].a = 0x0;
			}
		}
	}
	else if (LOWORD(wParam) == IDM_DRAW_PEN)
	{
		is_pen = !is_pen;
		CheckMenuItem(GetMenu(hwnd), IDM_DRAW_PEN, is_pen ? MF_CHECKED : MF_UNCHECKED);
		auto cursor = is_pen ? pen_cursor : default_cursor;
		SetClassLongPtr(hwnd, GCLP_HCURSOR, reinterpret_cast<LONG_PTR>(cursor));
	}
}
