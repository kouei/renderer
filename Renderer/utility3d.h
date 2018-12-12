#ifndef _UTILITY_3D_
#define _UTILITY_3D_

#include "utility2d.h"

namespace UTILITY3D
{
	using namespace MATHLIB3D;
	using namespace UTILITY2D;
	using namespace BITMAP_UTILITY;

	using std::string;

	struct Vertex
	{
		Vector3D<float> pos;
		Vector3D<float> to_light_source;
		Vector3D<float> halfway;
	};

	struct TextureInfo
	{
		TextureInfo
		(
			int _id, const string & _name,
			const Vector3D<float> & _Ka, const Vector3D<float> & _Kd, const Vector3D<float> & _Ks,
			float _d, float _Ns, int _illum,
			const string & _map_Ka, const string & _map_Kd, const string & _map_Ks
		) noexcept;
		TextureInfo() noexcept;
		TextureInfo(const TextureInfo & rhs) = default;
		TextureInfo(TextureInfo && rhs) noexcept = default;
		TextureInfo & operator= (const TextureInfo & rhs) = default;
		TextureInfo & operator= (TextureInfo && rhs) noexcept = default;
		~TextureInfo() noexcept = default;

		int id;
		string name;
		Vector3D<float> Ka, Kd, Ks;
		float d;
		float Ns;
		int illum;
		string map_Ka, map_Kd, map_Ks;
	};
	struct TextureBits
	{
		TextureBits() noexcept = default;
		TextureBits(const TextureBits & rhs) = delete;
		TextureBits(TextureBits && rhs) noexcept = default;
		TextureBits & operator=(const TextureBits & rhs) = delete;
		TextureBits & operator=(TextureBits && rhs) noexcept = default;
		~TextureBits() noexcept = default;

		Bitmap<BGRA> map_Ka_bits, map_Kd_bits, map_Ks_bits;
	};

	struct Texture
	{
		Texture() noexcept = default;
		Texture(const Texture & rhs) = delete;
		Texture(Texture && rhs) noexcept = default;
		Texture & operator=(const Texture & rhs) = delete;
		Texture & operator=(Texture && rhs) noexcept = default;
		~Texture() noexcept = default;

		TextureInfo texture_info;
		TextureBits texture_bits;
	};

	struct TriangleIndex
	{
		int vertex_index[3];
		int texture_index[3];
		int normal_index[3];
		int texture_id;
	};



	struct Frustum
	{
		float near_plane;
		float far_plane;
		float left_plane;
		float right_plane;
		float bottom_plane;
		float top_plane;
	};
};

#endif