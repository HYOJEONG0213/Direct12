#include "pch.h"
#include "Frustum.h"
#include "Camera.h"

// 프레임마다 한번씩
void Frustum::FinalUpdate()
{
	Matrix matViewInv = Camera::S_MatView.Invert();
	Matrix matProjectionInv = Camera::S_MatProjection.Invert();
	Matrix matInv = matProjectionInv * matViewInv;

	// 투영 좌표계에서의 정점 -> 월드 좌표계로 변환
	// 마지막 w는 1로 고정해서 변환 (동차 클립 좌표계)
	vector<Vec3> worldPos = {::XMVector3TransformCoord(Vec3(-1.f, 1.f, 0.f), matInv),
							 ::XMVector3TransformCoord(Vec3(1.f, 1.f, 0.f), matInv),
							 ::XMVector3TransformCoord(Vec3(1.f, -1.f, 0.f), matInv),
							 ::XMVector3TransformCoord(Vec3(-1.f, -1.f, 0.f), matInv),
							 ::XMVector3TransformCoord(Vec3(-1.f, 1.f, 1.f), matInv),
							 ::XMVector3TransformCoord(Vec3(1.f, 1.f, 1.f), matInv),
							 ::XMVector3TransformCoord(Vec3(1.f, -1.f, 1.f), matInv),
							 ::XMVector3TransformCoord(Vec3(-1.f, -1.f, 1.f), matInv)};

	// 평면 만들 때 점 세개 넣기 (노말벡터가 밖을 향하도록 만들기)
	_planes[PLANE_FRONT] = ::XMPlaneFromPoints(worldPos[0], worldPos[1], worldPos[2]); // CW (시계)
	_planes[PLANE_BACK] = ::XMPlaneFromPoints(worldPos[4], worldPos[7], worldPos[5]);  // CCW (반시계)
	_planes[PLANE_UP] = ::XMPlaneFromPoints(worldPos[4], worldPos[5], worldPos[1]);	   // CW
	_planes[PLANE_DOWN] = ::XMPlaneFromPoints(worldPos[7], worldPos[3], worldPos[6]);  // CCW
	_planes[PLANE_LEFT] = ::XMPlaneFromPoints(worldPos[4], worldPos[0], worldPos[7]);  // CW
	_planes[PLANE_RIGHT] = ::XMPlaneFromPoints(worldPos[5], worldPos[6], worldPos[1]); // CCW
}

bool Frustum::ContainsSphere(const Vec3 &pos, float radius)
{
	for (const Vec4 &plane : _planes)
	{
		// n = (a, b, c)
		Vec3 normal = Vec3(plane.x, plane.y, plane.z);

		// ax + by + cz + d > radius
		if (normal.Dot(pos) + plane.w > radius) return false;
	}

	return true;
}