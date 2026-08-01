#include "pch.h"
#include "PlaneCollider.h"
#include "GameObject.h"
#include "Transform.h"

PlaneCollider::PlaneCollider() : BaseCollider(ColliderType::Plane) {}

void PlaneCollider::FinalUpdate() { _worldPos = GetGameObject()->GetTransform()->GetWorldPosition(); }

// 피킹x
bool PlaneCollider::Intersects(Vec4 rayOrigin, Vec4 rayDir, OUT float &distance)
{
	distance = 0.f;
	return false;
}

bool PlaneCollider::IntersectsSphere(Vec3 sphereCenter, float sphereRadius, OUT Vec3 &normal,
									 OUT float &penetration) const
{
	// 반지름 0 = 무한 평면이므로 거리체크 안함
	if (_radius > 0.f)
	{
		float dx = sphereCenter.x - _worldPos.x;
		float dz = sphereCenter.z - _worldPos.z;
		if (sqrtf(dx * dx + dz * dz) > _radius + sphereRadius) return false;
	}

	// 평면 방정식: dot(point - _worldPos, _normal) = 0
	Vec3  diff = sphereCenter - _worldPos;
	float dist = diff.Dot(_normal); // 평면 위쪽이면 양수

	if (dist > sphereRadius) return false; // 충분히 위에 있음

	normal = _normal;
	penetration = sphereRadius - dist;
	return true;
}
