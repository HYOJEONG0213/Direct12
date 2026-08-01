#include "pch.h"
#include "CylinderCollider.h"
#include "GameObject.h"
#include "Transform.h"

CylinderCollider::CylinderCollider() : BaseCollider(ColliderType::Cylinder) {}

void CylinderCollider::FinalUpdate() { _worldCenter = GetGameObject()->GetTransform()->GetWorldPosition(); }

bool CylinderCollider::Intersects(Vec4 rayOrigin, Vec4 rayDir, OUT float &distance)
{
	// 피킹 안하므로 ㅈㅈ
	distance = 0.f;
	return false;
}

bool CylinderCollider::IntersectsSphere(Vec3 sphereCenter, float sphereRadius, OUT Vec3 &normal,
										OUT float &penetration) const
{
	// 원통 Y 범위 체크
	float minY = _worldCenter.y - _halfHeight;
	float maxY = _worldCenter.y + _halfHeight;

	if (sphereCenter.y + sphereRadius < minY) return false;
	if (sphereCenter.y - sphereRadius > maxY) return false;

	// XZ 평면에서 원통 축(중심)까지 거리
	float dx = sphereCenter.x - _worldCenter.x;
	float dz = sphereCenter.z - _worldCenter.z;
	float dist = sqrtf(dx * dx + dz * dz);

	float sumR = _radius + sphereRadius;
	if (dist >= sumR) return false; // 원통 밖

	// 구체가 원통 내부에 있을 때: 벽에 닿았는지 확인
	if (dist < _radius - sphereRadius) return false;

	// 충돌 법선: 원통 중심 → 구체 중심 방향 (XZ)
	if (dist > 0.001f) { normal = Vec3(dx / dist, 0.f, dz / dist); }
	else { normal = Vec3(1.f, 0.f, 0.f); }

	penetration = sumR - dist;
	return true;
}
