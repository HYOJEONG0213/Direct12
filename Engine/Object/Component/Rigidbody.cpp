#include "pch.h"
#include "Rigidbody.h"
#include "Transform.h"
#include "GameObject.h"
#include "Timer.h"

Rigidbody::Rigidbody() : Component(COMPONENT_TYPE::RIGIDBODY) {}

void Rigidbody::Update()
{
	float dt = DELTA_TIME;

	// 중력 적용
	if (_useGravity) _velocity.y -= GRAVITY * dt;
	Vec3 pos = GetTransform()->GetLocalPosition();
	pos += _velocity * dt;
	GetTransform()->SetLocalPosition(pos);

	// 구의 회전
	float vx = _velocity.x;
	float vz = _velocity.z;
	float speed = sqrtf(vx * vx + vz * vz);
	if (speed > 0.01f && _radius > 0.f)
	{
		float anglePerSec = speed / _radius;
		float angleDeg = anglePerSec * dt * (180.f / 3.14159265f); // rad/s

		Vec3 rotation = GetTransform()->GetLocalRotation();
		rotation.x += (vz / speed) * angleDeg * 0.03f; // Z방향 이동 = X축 회전
		rotation.z -= (vx / speed) * angleDeg * 0.03f; // X방향 이동 = Z축 회전
		GetTransform()->SetLocalRotation(rotation);
	}
}
