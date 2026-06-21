#include "pch.h"
#include "Rigidbody.h"
#include "Transform.h"
#include "GameObject.h"
#include "Timer.h"

Rigidbody::Rigidbody() : Component(COMPONENT_TYPE::RIGIDBODY) {}

void Rigidbody::Update()
{
	float dt = DELTA_TIME;

	if (_useGravity)
		_velocity.y -= GRAVITY * dt;

	Vec3 pos = GetTransform()->GetLocalPosition();
	pos += _velocity * dt;
	GetTransform()->SetLocalPosition(pos);
}
