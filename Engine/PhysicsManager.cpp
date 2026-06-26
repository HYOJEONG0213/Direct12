#include "pch.h"
#include "PhysicsManager.h"
#include "Scene.h"
#include "GameObject.h"
#include "Rigidbody.h"
#include "SphereCollider.h"
#include "PlaneCollider.h"
#include "Transform.h"

void PhysicsManager::Init(Scene *scene)
{
	// 정적 PlaneCollider 수집
	for (const auto &object : scene->GetGameObjects())
	{
		if (!object->IsStatic()) continue;
		if (auto plane = dynamic_pointer_cast<PlaneCollider>(object->GetCollider())) planes.push_back(plane);
	}
}

void PhysicsManager::Update(Scene *scene)
{
	// 동적 오브젝트 물리 업데이트
	for (const auto &object : scene->GetGameObjects())
	{
		if (object->IsStatic()) continue;

		auto rigid = object->GetRigidbody();
		auto collion = dynamic_pointer_cast<SphereCollider>(object->GetCollider());
		if (!rigid || !collion) continue;

		Vec3  pos = object->GetTransform()->GetLocalPosition();
		Vec3  vel = rigid->GetVelocity();
		Vec3  scale = object->GetTransform()->GetLocalScale();
		float radius = collion->GetRadius() * max(max(scale.x, scale.y), scale.z);

		// 공과 평면 충돌시 위치 보정
		for (const auto &plane : planes)
		{
			Vec3  n;
			float penetration;
			if (!plane->IntersectsSphere(pos, radius, n, penetration)) continue;

			// 위치 보정
			pos += n * penetration;

			// 속도 반전
			float vDotN = vel.Dot(n);
			if (vDotN < 0.f) vel -= n * (1.05f * vDotN);

			// 가장자리 근처면 XZ 방향으로 밀어내기
			float planeRadius = plane->GetRadius();
			if (planeRadius > 0.f)
			{
				Vec3  planePos = plane->GetWorldPos();
				float dx = pos.x - planePos.x;
				float dz = pos.z - planePos.z;
				float xzDist = sqrtf(dx * dx + dz * dz);
				float outerBound = planeRadius + radius; // 경계끝
				float edgeStart = planeRadius * 0.999f;	 // 경계 시작

				if (xzDist > edgeStart && xzDist < outerBound)
				{
					float t = (xzDist - edgeStart) / (outerBound - edgeStart); // 치우침정도
					Vec3  outward = Vec3(dx, 0.f, dz);						   // 평면 -> 공 방향 벡터
					outward.Normalize();
					vel += outward * (t * 70.f);
				}
			}
		}

		object->GetTransform()->SetLocalPosition(pos);
		rigid->SetVelocity(vel);
	}
}
