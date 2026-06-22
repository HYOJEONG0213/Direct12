#pragma once
#include "BaseCollider.h"

class PlaneCollider : public BaseCollider
{
public:
	PlaneCollider();
	virtual ~PlaneCollider() = default;

	virtual void FinalUpdate() override;
	virtual bool Intersects(Vec4 rayOrigin, Vec4 rayDir, OUT float &distance) override;

	bool IntersectsSphere(Vec3 sphereCenter, float sphereRadius, OUT Vec3 &normal, OUT float &penetration) const;

	void SetNormal(Vec3 n)
	{
		_normal = n;
		_normal.Normalize();
	}
	void SetRadius(float r) { _radius = r; }

	Vec3  GetNormal() const { return _normal; }
	Vec3  GetWorldPos() const { return _worldPos; }
	float GetRadius() const { return _radius; }

private:
	Vec3  _normal = Vec3(0.f, 1.f, 0.f);
	Vec3  _worldPos = {};
	float _radius = 0.f;
};
