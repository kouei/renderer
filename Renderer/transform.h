#ifndef _TRANSFORM_
#define _TRANSFORM_

#include <vector>
#include "utility3d.h"


namespace TRANSFORM
{
	using namespace MATHLIB3D;
	using namespace UTILITY3D;
	
	using std::vector;

	struct CalculateLightingRelatedVector
	{
		CalculateLightingRelatedVector() = delete;
		CalculateLightingRelatedVector(const CalculateLightingRelatedVector & rhs) = default;
		CalculateLightingRelatedVector(CalculateLightingRelatedVector && rhs) = default;
		CalculateLightingRelatedVector & operator= (const CalculateLightingRelatedVector & rhs) = default;
		CalculateLightingRelatedVector & operator= (CalculateLightingRelatedVector && rhs) = default;
		~CalculateLightingRelatedVector() = default;

		CalculateLightingRelatedVector(const Vector3D<float> & _camera_pos, const Vector3D<float> & _light_source) noexcept;
		void operator() (Vertex & v) const noexcept;

		const Vector3D<float> camera_pos;
		const Vector3D<float> light_source;
	};

	struct CameraToHomogeneousClipping
	{
		CameraToHomogeneousClipping() = delete;
		CameraToHomogeneousClipping(const CameraToHomogeneousClipping & rhs) = default;
		CameraToHomogeneousClipping(CameraToHomogeneousClipping && rhs) = default;
		CameraToHomogeneousClipping & operator= (const CameraToHomogeneousClipping & rhs) = default;
		CameraToHomogeneousClipping & operator= (CameraToHomogeneousClipping && rhs) = default;
		~CameraToHomogeneousClipping() = default;

		CameraToHomogeneousClipping(const Frustum & frustum) noexcept;
		Vertex operator() (const Vertex & v) const noexcept;

		Matrix4D<float> matrix_to_homogeneous_clipping;
	};

	struct HomogeneousClippingCulling
	{
		HomogeneousClippingCulling() = delete;
		HomogeneousClippingCulling(const HomogeneousClippingCulling & rhs) = delete;
		HomogeneousClippingCulling(HomogeneousClippingCulling && rhs) noexcept;
		HomogeneousClippingCulling & operator= (const HomogeneousClippingCulling & rhs) = delete;
		HomogeneousClippingCulling & operator= (HomogeneousClippingCulling && rhs) noexcept;
		~HomogeneousClippingCulling() = default;

		HomogeneousClippingCulling(const vector<Vertex> * _homogeneous_clipping_vertex_) noexcept;

		bool operator() (const TriangleIndex & t) const noexcept;

		const vector<Vertex> * homogeneous_clipping_vertex;
	};
	struct HomogeneousClippingToViewport
	{
		HomogeneousClippingToViewport() = delete;
		HomogeneousClippingToViewport(const HomogeneousClippingToViewport & rhs) = default;
		HomogeneousClippingToViewport(HomogeneousClippingToViewport && rhs) = default;
		HomogeneousClippingToViewport & operator= (const HomogeneousClippingToViewport & rhs) = default;
		HomogeneousClippingToViewport & operator= (HomogeneousClippingToViewport && rhs) = default;
		~HomogeneousClippingToViewport() = default;

		HomogeneousClippingToViewport(int _cxClient, int _cyClient, const Frustum & _frustum, float _scale) noexcept;
		Vertex operator() (const Vertex & v) const noexcept;

		int cxClient;
		int cyClient;
		const Frustum frustum;
		float scale;
		Matrix4D<float> matrix_to_homogeneous_clipping;
	};
};

#endif