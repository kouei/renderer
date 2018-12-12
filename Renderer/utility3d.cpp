#include"utility3d.h"
using namespace UTILITY3D;

TextureInfo::TextureInfo
(
	int _id, const string & _name,
	const Vector3D<float> & _Ka, const Vector3D<float> & _Kd, const Vector3D<float> & _Ks,
	float _d, float _Ns, int _illum,
	const string & _map_Ka, const string & _map_Kd, const string & _map_Ks
) noexcept
	:
id(_id), name(_name),
Ka(_Ka), Kd(_Kd), Ks(_Ks),
d(_d), Ns(_Ns), illum(_illum),
map_Ka(_map_Ka), map_Kd(_map_Kd), map_Ks(_map_Ks)
{}

TextureInfo::TextureInfo() noexcept
	:
id(-1), name(),
Ka(1.0f, 1.0f, 1.0f), Kd(1.0f, 0.0f, 0.0f), Ks(1.0f, 1.0f, 1.0f),
d(1.0f), Ns(100.0f), illum(2),
map_Ka(), map_Kd(), map_Ks()
{}