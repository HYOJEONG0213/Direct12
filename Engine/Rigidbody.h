#pragma once
#include "Component.h"

class Rigidbody : public Component
{
public:
	Rigidbody();
	virtual ~Rigidbody() = default;

	virtual void Update() override;

	Vec3 GetVelocity() const { return _velocity; }
	void SetVelocity(const Vec3 &v) { _velocity = v; }
	void AddVelocity(const Vec3 &dv) { _velocity += dv; }

	float GetMass() const { return _mass; }
	void  SetMass(float m) { _mass = m; }

	bool GetUseGravity() const { return _useGravity; }
	void SetUseGravity(bool b) { _useGravity = b; }

	float GetRadius() const { return _radius; }
	void  SetRadius(float r) { _radius = r; }

private:
	Vec3  _velocity = {};
	float _mass = 1.f;
	bool  _useGravity = true;
	float _radius = 1.f;

	static constexpr float GRAVITY = 30.f;
};
