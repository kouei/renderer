// Copyright 2016 Kouei

#ifndef _MATHLIB3D_
#define _MATHLIB3D_

#include <iostream>
#include <iomanip>
#include "my_assert.h"
#include "math_utility.h"

namespace MATHLIB3D
{
	using namespace MATH_UTILITY;
	using std::is_floating_point;
	using std::is_unsigned;
	using std::fixed;
	using std::cout;

	const double EPSILON = 0.001;

	template<typename T, size_t N>
	struct Vector
	{
		using ValueType = T;

		T & operator[](size_t i) noexcept { return a[i]; }
		const T & operator[](size_t i) const noexcept { return a[i]; }

		T a[N];
	};
	template<typename T>
	struct Vector<T, 2>
	{
		using ValueType = T;

		Vector(T _x, T _y) noexcept
			:x(_x), y(_y)
		{
			// Do Nothing
		}

		Vector() noexcept = default;
		Vector(const Vector & rhs) noexcept = default;
		Vector(Vector && rhs) noexcept = default;
		Vector & operator= (const Vector & rhs) noexcept = default;
		Vector & operator= (Vector && rhs) noexcept = default;
		~Vector() noexcept = default;

		T & operator[](size_t i) noexcept { return a[i]; }
		const T & operator[](size_t i) const noexcept { return a[i]; }

		union
		{
			struct
			{
				T x, y;
			};
			T a[2];
		};
	};
	template<typename T>
	using Vector2D = Vector<T, 2>;

	template<typename T>
	struct Vector<T, 3>
	{
		using ValueType = T;

		Vector(T _x, T _y, T _z) noexcept
			:x(_x), y(_y), z(_z)
		{
			// Do Nothing
		}

		Vector() noexcept = default;
		Vector(const Vector & rhs) noexcept = default;
		Vector(Vector && rhs) noexcept = default;
		Vector & operator= (const Vector & rhs) noexcept = default;
		Vector & operator= (Vector && rhs) noexcept = default;
		~Vector() noexcept = default;

		T & operator[](size_t i) noexcept { return a[i]; }
		const T & operator[](size_t i) const noexcept { return a[i]; }

		union
		{
			struct
			{
				T x, y, z;
			};
			T a[3];
		};
	};
	template<typename T>
	using Vector3D = Vector<T, 3>;

	template<typename T>
	struct Vector<T, 4>
	{
		using ValueType = T;

		Vector(T _x, T _y, T _z, T _w) noexcept
			:x(_x), y(_y), z(_z), w(_w)
		{
			// Do Nothing
		}
		Vector(Vector3D<T> _v3d, T _s) noexcept
			: v3d(_v3d), s(_s)
		{
			// Do Nothing
		}

		Vector() noexcept = default;
		Vector(const Vector & rhs) noexcept = default;
		Vector(Vector && rhs) noexcept = default;
		Vector & operator= (const Vector & rhs) noexcept = default;
		Vector & operator= (Vector && rhs) noexcept = default;
		~Vector() noexcept = default;

		T & operator[](size_t i) noexcept { return a[i]; }
		const T & operator[](size_t i) const noexcept { return a[i]; }

		union
		{
			struct
			{
				T x, y, z, w;
			};
			struct
			{
				Vector3D<T> v3d;
				T s;
			};
			struct
			{
				Vector2D<T> v2d;
				T z, w;
			};
			T a[4];
		};
	};
	template<typename T>
	using Vector4D = Vector<T, 4>;

	template<typename T, size_t N>
	struct Matrix
	{
		using ValueType = T;

		Matrix() noexcept = default;
		Matrix(const Matrix & rhs) noexcept = default;
		Matrix(Matrix && rhs) noexcept = default;
		Matrix & operator= (const Matrix & rhs) noexcept = default;
		Matrix & operator= (Matrix && rhs) noexcept = default;
		~Matrix() noexcept = default;

		Vector<T, N> & operator[] (size_t i) noexcept { return a[i]; }
		const Vector<T, N> & operator[] (size_t i) const noexcept { return a[i]; }
		
		union
		{
			T arr[N][N];
			Vector<T, N> a[N];
		};
	};
	template<typename T>
	using Matrix2D = Matrix<T, 2>;
	template<typename T>
	using Matrix3D = Matrix<T, 3>;
	template<typename T>
	using Matrix4D = Matrix<T, 4>;

	template<typename NEW_TYPE, typename T, size_t N>
	Vector<NEW_TYPE, N> type_cast(Vector<T, N> v) noexcept
	{
		Vector<NEW_TYPE, N> result;
		for (auto i = 0; i < N; ++i) result[i] = static_cast<NEW_TYPE>(v[i]);
		return result;
	}


	template<typename T, size_t N>
	Vector<T, N> & operator+=(Vector<T, N> & v1, Vector<T, N> v2) noexcept
	{
		for (auto i = 0; i < N; ++i) v1[i] += v2[i];
		return v1;
	}
	template<typename T, size_t N>
	Vector<T, N> & operator-=(Vector<T, N> & v1, Vector<T, N> v2) noexcept
	{
		for (auto i = 0; i < N; ++i) v1[i] -= v2[i];
		return v1;
	}
	template<typename T, size_t N>
	Vector<T, N> & operator*=(Vector<T, N> & v, T t) noexcept
	{
		for (auto i = 0; i < N; ++i) v[i] *= t;
		return v;
	}
	template<typename T, size_t N>
	Vector<T, N> & operator/=(Vector<T, N> & v, T t) noexcept
	{
		for (auto i = 0; i < N; ++i)
		{
			my_assert(t != static_cast<T>(0.0));
			v[i] /= t;
		}
		return v;
	}
	template<typename T, size_t N>
	Vector<T, N> operator+(Vector<T, N> v1, Vector<T, N> v2) noexcept
	{
		return v1 += v2;
	}
	template<typename T, size_t N>
	Vector<T, N> operator-(Vector<T, N> v1, Vector<T, N> v2) noexcept
	{
		return v1 -= v2;
	}
	template<typename T, size_t N>
	Vector<T, N> operator*(Vector<T, N> v, T t) noexcept
	{
		return v *= t;
	}
	template<typename T, size_t N>
	Vector<T, N> operator*(T t, Vector<T, N> v) noexcept
	{
		return v *= t;
	}
	template<typename T, size_t N>
	Vector<T, N> dot(Vector<T, N> v, const Matrix<T, N> & m) noexcept
	{
		Vector<T, N> reuslt;
		for (auto i = 0; i < N; ++i)
		{
			reuslt[i] = 0;
			for (auto j = 0; j < N; ++j)
			{
				reuslt[i] += v[j] * m[j][i];
			}
		}
		return reuslt;
	}
	template<typename T, size_t N>
	Vector<T, N> dot(const Matrix<T, N> & m, Vector<T, N> v) noexcept
	{
		Vector<T, N> reuslt;
		for (auto i = 0; i < N; ++i)
		{
			reuslt[i] = 0;
			for (auto j = 0; j < N; ++j)
			{
				reuslt[i] += v[j] * m[i][j];
			}
		}
		return reuslt;
	}
	template<typename T, size_t N>
	Vector<T, N> operator/(Vector<T, N> v, T t) noexcept
	{
		return v /= t;
	}
	template<typename T, size_t N>
	Vector<T, N> operator-(Vector<T, N> v) noexcept
	{
		for (auto i = 0; i < N; ++i)
		{
			v[i] = -v[i];
		}
		return v;
	}
	template<typename T, size_t N>
	T dot(Vector<T, N> v1, Vector<T, N> v2) noexcept
	{
		T result = static_cast<T>(0.0);
		for (auto i = 0; i < N; ++i) result += v1[i] * v2[i];
		return result;
	}
	template<typename T, size_t N>
	Vector<T, N> normalization(Vector<T, N> v) noexcept
	{
		auto len = sqrt(dot(v, v));
		my_assert(len != static_cast<T>(0.0));
		return v /= len;
	}
	template<typename T, size_t N>
	T len(Vector<T, N> v) noexcept
	{
		return sqrt(dot(v, v));
	}
	template<typename T, size_t N>
	Vector<T, N> projection(Vector<T, N> v1, Vector<T, N> v2) noexcept
	{
		auto v2_dot_v2 = dotproduct(v2, v2);
		auto v1_dot_v2 = dotproduct(v1, v2);
		my_assert(v2_dot_v2 != static_cast<T>(0.0));
		return v2 *= v1_dot_v2 / v2_dot_v2;
	}
	template<typename T, size_t N>
	Vector<T, N> perpendicular(Vector<T, N> v1, Vector<T, N> v2) noexcept
	{
		return v1 -= projection(v1, v2);
	}
	template<typename T, size_t N>
	void print(Vector<T, N> v) noexcept
	{

		for (auto i = 0; i < N; ++i)
		{
			cout << static_cast<double>(v[i]) << (i == N - 1 ? '\n' : ' ');
		}
	}

	
	
	template<typename T>
	Vector3D<T> cross(Vector3D<T> v1, Vector3D<T> v2) noexcept
	{
		return Vector3D<T>(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x);
	}
	template<typename T>
	Vector3D<T> calculate_normal(Vector3D<T> v1, Vector3D<T> v2, Vector3D<T> v3) noexcept
	{
		return crossproduct(v2 - v1, v3 - v2);
	}
	template<typename T, typename... Types>
	Vector4D<T> quaternion_product(Vector4D<T> v1, Vector4D<T> v2, Types... args) noexcept
	{
		return quaternion_product(quaternion_product(v1, v2), args...);
	}
	template<typename T>
	Vector4D<T> quaternion_product(Vector4D<T> v1, Vector4D<T> v2) noexcept
	{
		return Vector4D<T>(v1.v3d * v2.s + v2.v3d * v1.s + crossproduct(v1.v3d, v2.v3d), v1.s * v2.s - dotproduct(v1.v3d, v2.v3d));
	}
	template<typename T>
	Vector4D<T> quaternion_inverse(Vector4D<T> v) noexcept
	{
		T factor = dotproduct(v, v);
		return Vector4D<T>(-v.x / factor, -v.y / factor, -v.z / factor, v.w / factor);
	}
	

	template<typename T, size_t N>
	Matrix<T, N> & operator+=(Matrix<T, N> & m1, const Matrix<T, N> & m2) noexcept
	{
		for (auto i = 0; i < N; ++i)
		{
			m1[i] += m2[i];
		}
		return m1;
	}
	template<typename T, size_t N>
	Matrix<T, N> & operator-=(Matrix<T, N> & m1, const Matrix<T, N> & m2) noexcept
	{
		for (auto i = 0; i < N; ++i)
		{
			m1[i] -= m2[i];
		}
		return m1;
	}
	template<typename T, size_t N>
	Matrix<T, N> & operator*=(Matrix<T, N> & m, T t) noexcept
	{
		for (auto i = 0; i < N; ++i)
		{
			m[i] *= t;
		}
		return m;
	}
	template<typename T, size_t N>
	Matrix<T, N> & operator/=(Matrix<T, N> & m, T t) noexcept
	{
		my_assert(t != static_cast<T>(0.0));
		for (auto i = 0; i < N; ++i)
		{
			m[i] /= t;
		}
		return m;
	}
	template<typename T, size_t N>
	Matrix<T, N> operator+(const Matrix<T, N> & m1, const Matrix<T, N> & m2) noexcept
	{
		Matrix<T, N> result;
		for (auto i = 0; i < N; ++i)
		{
			result[i] = m1[i] + m2[i];
		}
		return result;
	}
	template<typename T, size_t N>
	Matrix<T, N> operator-(const Matrix<T, N> & m1, const Matrix<T, N> & m2) noexcept
	{
		Matrix<T, N> result;
		for (auto i = 0; i < N; ++i)
		{
			result[i] = m1[i] - m2[i];
		}
		return result;
	}
	template<typename T, size_t N>
	Matrix<T, N> operator*(const Matrix<T, N> & m, T t) noexcept
	{
		Matrix<T, N> result;
		for (auto i = 0; i < N; ++i)
		{
			result[i] = m * t;
		}
		return result;
	}
	template<typename T, size_t N>
	Matrix<T, N> operator*(T t, const Matrix<T, N> & m) noexcept
	{
		return m * t;
	}
	template<typename T, size_t N>
	Matrix<T, N> operator*(const Matrix<T, N> & m1, const Matrix<T, N> & m2) noexcept
	{
		Matrix<T, N> result;
		for (auto i = 0; i < N; ++i)
		{
			for (auto j = 0; j < N; ++j)
			{
				result[i][j] = static_cast<T>(0.0);
				for (auto k = 0; k < N; ++k)
				{
					result[i][j] += m1[i][k] * m2[k][j];
				}
			}
		}
		return result;
	}
	template<typename T, size_t N>
	Matrix<T, N> operator/(const Matrix<T, N> & m, T t) noexcept
	{
		my_assert(t != static_cast<T>(0.0));
		Matrix<T, N> result;
		for (auto i = 0; i < N; ++i)
		{
			result[i] = m[i] / t;
		}
	}
	template<typename T, size_t N>
	Matrix<T, N> operator-(const Matrix<T, N> & m) noexcept
	{
		Matrix<T, N> result;
		for (auto i = 0; i < N; ++i)
		{
			result[i] = -m[i];
		}
		return result;
	}
	template<typename T, size_t N>
	bool operator==(const Matrix<T, N> & m1, const Matrix<T, N> & m2) noexcept
	{
		for (auto i = 0; i < N; ++i)
		{
			if(m1[i] != m2[i]) return false;
		}
		return true;
	}
	template<typename T, size_t N>
	bool operator!=(const Matrix<T, N> & m1, const Matrix<T, N> & m2) noexcept
	{
		return !(m1 == m2);
	}
	template<typename T, size_t N>
	Matrix<T, N> transpose(const Matrix<T, N> & m) noexcept
	{
		Matrix<T, N> result;
		for (auto i = 0; i < N; ++i)
		{
			for (auto j = 0; j < N; ++j)
			{
				result[i][j] = m[j][i];
			}
		}
		return result;
	}
	template<typename T, size_t N>
	void print(const Matrix<T, N> & m) noexcept
	{
		for (auto i = 0; i < N; ++i)
		{
			print(m[i]);
		}
	}

	template<typename T>
	Vector3D<T> rotate(Vector3D<T> v, Vector3D<T> axis, T degree) noexcept
	{
		auto rad = defree_to_radian(degree);
		auto c = cos(rad);
		auto s = sin(rad);
		auto new_v = v * c + cross(axis, v) * s + axis * dot(axis, v) * (static_cast<T>(1.0) - c);
		return new_v;
	}

};    // namespace MATHLIB3D

#endif    // _MATHLIB3D_H_
