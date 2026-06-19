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
	// 평면 방정식: dot(point - _worldPos, _normal) = 0
	Vec3  diff = sphereCenter - _worldPos;
	float dist = diff.Dot(_normal); // 평면 위쪽이면 양수

	if (dist > sphereRadius) return false; // 충분히 위에 있음

	normal = _normal;
	penetration = sphereRadius - dist;
	return true;
}
