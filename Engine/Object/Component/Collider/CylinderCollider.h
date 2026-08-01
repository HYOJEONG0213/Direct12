#pragma once
#include "BaseCollider.h"

class CylinderCollider : public BaseCollider
{
public:
	CylinderCollider();
	virtual ~CylinderCollider() = default;

	virtual void FinalUpdate() override;
	virtual bool Intersects(Vec4 rayOrigin, Vec4 rayDir, OUT float &distance) override;

	bool IntersectsSphere(Vec3 sphereCenter, float sphereRadius, OUT Vec3 &normal, OUT float &penetration) const;

	void SetRadius(float r) { _radius = r; }
	void SetHalfHeight(float h) { _halfHeight = h; }

	float GetRadius() const { return _radius; }
	float GetHalfHeight() const { return _halfHeight; }
	Vec3  GetCenter() const { return _worldCenter; }

private:
	float _radius = 1.f;
	float _halfHeight = 1.f;

	Vec3 _worldCenter = {};
};
