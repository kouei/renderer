#include <algorithm>
#include "transform.h"

using namespace TRANSFORM;
using std::min;


CalculateLightingRelatedVector::CalculateLightingRelatedVector(const Vector3D<float> & _camera_pos, const Vector3D<float> & _light_source) noexcept
	: camera_pos(_camera_pos), light_source(_light_source)
{}

void CalculateLightingRelatedVector::operator() (Vertex & v) const noexcept
{
	v.to_light_source = light_source - v.pos;
	auto to_camera = camera_pos - v.pos;
	v.halfway = to_camera / len(to_camera) + v.to_light_source / len(v.to_light_source);
}



CameraToHomogeneousClipping::CameraToHomogeneousClipping(const Frustum & frustum) noexcept
	:
matrix_to_homogeneous_clipping
{
	2.0f * frustum.near_plane / (frustum.right_plane - frustum.left_plane), 0.0f, -(frustum.right_plane + frustum.left_plane) / (frustum.right_plane - frustum.left_plane), 0.0f,
	0.0f, 2.0f * frustum.near_plane / (frustum.top_plane - frustum.bottom_plane), -(frustum.top_plane + frustum.bottom_plane) / (frustum.top_plane - frustum.bottom_plane), 0.0f,
	0.0f, 0.0f, -(frustum.near_plane + frustum.far_plane) / (frustum.near_plane - frustum.far_plane), 2.0f * frustum.far_plane * frustum.near_plane / (frustum.near_plane - frustum.far_plane),
	0.0f, 0.0f, 1.0f, 0.0f
}
{}

Vertex CameraToHomogeneousClipping::operator() (const Vertex & v) const noexcept
{
	Vector3D<float> homogeneous_clipping_pos;
	homogeneous_clipping_pos.x = dot(v.pos, matrix_to_homogeneous_clipping[0].v3d) /*+ 0.0f*/;
	homogeneous_clipping_pos.y = dot(v.pos, matrix_to_homogeneous_clipping[1].v3d) /*+ 0.0f*/;
	homogeneous_clipping_pos.z = dot(v.pos, matrix_to_homogeneous_clipping[2].v3d) + matrix_to_homogeneous_clipping[2].w;
	homogeneous_clipping_pos /= v.pos.z;
	return Vertex{ homogeneous_clipping_pos, v.to_light_source, v.halfway };
}


HomogeneousClippingCulling::HomogeneousClippingCulling(HomogeneousClippingCulling && rhs) noexcept
	:homogeneous_clipping_vertex(move(rhs.homogeneous_clipping_vertex))
{
	rhs.homogeneous_clipping_vertex = nullptr;
}
HomogeneousClippingCulling & HomogeneousClippingCulling::operator= (HomogeneousClippingCulling && rhs) noexcept
{
	if (this != &rhs)
	{
		swap(homogeneous_clipping_vertex, rhs.homogeneous_clipping_vertex);
	}
	return *this;
}
HomogeneousClippingCulling::HomogeneousClippingCulling(const vector<Vertex> * _homogeneous_clipping_vertex_) noexcept
	: homogeneous_clipping_vertex(_homogeneous_clipping_vertex_)
{}

bool HomogeneousClippingCulling::operator() (const TriangleIndex & t) const noexcept
{
	for (auto current_index : t.vertex_index)
	{
		for (auto current_coordinate : (*homogeneous_clipping_vertex)[current_index].pos.a)
		{
			if (abs(current_coordinate) > 1.0f) return false;
		}
	}
	return true;
}



HomogeneousClippingToViewport::HomogeneousClippingToViewport(int _cxClient, int _cyClient, const Frustum & _frustum, float _scale) noexcept
	: cxClient(_cxClient), cyClient(_cyClient), frustum(_frustum), scale(_scale)
{
	matrix_to_homogeneous_clipping = Matrix4D<float>
	{
		2.0f * frustum.near_plane / (frustum.right_plane - frustum.left_plane), 0.0f, -(frustum.right_plane + frustum.left_plane) / (frustum.right_plane - frustum.left_plane), 0.0f,
		0.0f, 2.0f * frustum.near_plane / (frustum.top_plane - frustum.bottom_plane), -(frustum.top_plane + frustum.bottom_plane) / (frustum.top_plane - frustum.bottom_plane), 0.0f,
		0.0f, 0.0f, -(frustum.near_plane + frustum.far_plane) / (frustum.near_plane - frustum.far_plane), 2.0f * frustum.far_plane * frustum.near_plane / (frustum.near_plane - frustum.far_plane),
		0.0f, 0.0f, 1.0f, 0.0f
	};
}

Vertex HomogeneousClippingToViewport::operator() (const Vertex & v) const noexcept
{
	float x_len = abs(frustum.right_plane - frustum.left_plane);
	float y_len = abs(frustum.top_plane - frustum.bottom_plane);
	float x_scale = static_cast<float>(cxClient) / x_len;
	float y_scale = static_cast<float>(cyClient) / y_len;
	float client_scale = min<float>(x_scale, y_scale);

	Vertex result;
	result.pos = Vector3D<float>
		(
			scale * client_scale * ((v.pos.x + 1.0f) * (frustum.right_plane - frustum.left_plane) * 0.5f + frustum.left_plane) + static_cast<float>(cxClient) / 2.0f,
			scale * client_scale * ((v.pos.y + 1.0f) * (frustum.top_plane - frustum.bottom_plane) * 0.5f + frustum.bottom_plane) + static_cast<float>(cyClient) / 2.0f,
			1.0f / ((v.pos.z - matrix_to_homogeneous_clipping[2][2]) / matrix_to_homogeneous_clipping[2][3]) //recover the z coordinate in camera space
			);
	result.to_light_source = v.to_light_source;
	result.halfway = v.halfway;
	return result;
}
