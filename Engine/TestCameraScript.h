#pragma once
#include "MonoBehaviour.h"

class TestCameraScript : public MonoBehaviour
{
public:
	TestCameraScript();
	virtual ~TestCameraScript();

	virtual void Start() override;
	virtual void LateUpdate() override;

	void SetMovementLocked(bool locked) { _movementLocked = locked; }

private:
	float _speed = 100.f;

	Vec3  _orbitPivot = Vec3(0.f, 0.f, 200.f);
	float _orbitRadius = 0.f;
	float _orbitAngle = 0.f;
	float _orbitSpeed = 1.0f;

	bool _movementLocked = false;
};
