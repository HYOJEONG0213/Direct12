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
	_planes.clear();
	_spheres.clear();

	for (const auto &object : scene->GetGameObjects())
	{
		if (object->IsStatic()) // 정적 PlaneCollider 수집
		{
			if (auto plane = dynamic_pointer_cast<PlaneCollider>(object->GetCollider())) _planes.push_back(plane);
		}
		else // 동적 오브젝트 수집(구)
		{
			RegisterDynamic(object);
		}
	}
}

void PhysicsManager::Update(class Scene *scene)
{
	// 구-평면 충돌
	for (auto &sphere : _spheres)
	{
		Vec3 pos = sphere.object->GetTransform()->GetLocalPosition();
		Vec3 vel = sphere.rigid->GetVelocity();

		for (const auto &plane : _planes)
		{
			Vec3  n;
			float penetration;
			if (!plane->IntersectsSphere(pos, sphere.radius, n, penetration)) continue;

			// 위치 보정
			pos += n * penetration;

			// 속도 반전
			float vDotN = vel.Dot(n);
			if (vDotN < 0.f) vel -= n * (1.05f * vDotN);

			// 마찰력 적용
			Vec3 velTangent = vel - n * vel.Dot(n); // 수평 성분만
			vel -= velTangent * 0.1f;
		}

		sphere.object->GetTransform()->SetLocalPosition(pos);
		sphere.rigid->SetVelocity(vel);
	}

	// 구-구 충돌
	int count = (int)_spheres.size();
	for (int i = 0; i < count; i++)
	{
		for (int j = i + 1; j < count; j++)
		{
			SphereData &a = _spheres[i];
			SphereData &b = _spheres[j];

			Vec3 posA = a.object->GetTransform()->GetLocalPosition();
			Vec3 posB = b.object->GetTransform()->GetLocalPosition();
			Vec3 velA = a.rigid->GetVelocity();
			Vec3 velB = b.rigid->GetVelocity();

			Vec3  diff = posA - posB;
			float dist = diff.Length();
			float minDist = a.radius + b.radius;

			if (dist >= minDist) continue; // 충돌 발생x

			// 위치 보정(겹친만큼 밀어내기)
			Vec3  n = diff / dist; // n : posB -> posA 방향 단위벡터
			float penetration = minDist - dist;
			float totalMass = a.mass + b.mass;
			posA += n * (penetration * b.mass / totalMass);
			posB -= n * (penetration * a.mass / totalMass);

			// 탄성 충돌
			float vRelN = (velA - velB).Dot(n);
			if (vRelN >= 0.f) continue;

			float impulse = -(1.f + COR) * vRelN / (1.f / a.mass + 1.f / b.mass);
			velA += n * (impulse / a.mass);
			velB -= n * (impulse / b.mass);

			// 공끼리 마찰력 적용
			Vec3 vRel = velA - velB;
			Vec3 vRelTangent = vRel - n * vRel.Dot(n); // 접선 성분만
			velA -= vRelTangent * 0.03f;
			velB += vRelTangent * 0.03f;

			a.object->GetTransform()->SetLocalPosition(posA);
			b.object->GetTransform()->SetLocalPosition(posB);
			a.rigid->SetVelocity(velA);
			b.rigid->SetVelocity(velB);

			// 합치기 판정
			a.object->TriggerCollision(b.object);
		}
	}
}

void PhysicsManager::UnregisterDynamic(shared_ptr<GameObject> object)
{
	for (int i = 0; i < _spheres.size(); i++)
	{
		if (_spheres[i].object == object)
		{
			_spheres.erase(_spheres.begin() + i);
			return;
		}
	}
}

void PhysicsManager::RegisterDynamic(shared_ptr<GameObject> object)
{
	auto rigid = object->GetRigidbody();
	auto collider = dynamic_pointer_cast<SphereCollider>(object->GetCollider());
	if (!rigid || !collider) return;

	Vec3  scale = object->GetTransform()->GetLocalScale();
	float radius = collider->GetRadius() * max(max(scale.x, scale.y), scale.z);
	_spheres.push_back({object, rigid, collider, radius, rigid->GetMass()});
}
