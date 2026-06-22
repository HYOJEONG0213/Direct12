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
			if (vDotN < 0.f) vel -= n * (1.3f * vDotN);
		}

		object->GetTransform()->SetLocalPosition(pos);
		rigid->SetVelocity(vel);
	}
}
