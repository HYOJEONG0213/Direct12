#include "pch.h"
#include "TestCameraScript.h"
#include "Transform.h"
#include "Camera.h"
#include "GameObject.h"
#include "Input.h"
#include "Timer.h"
#include "SceneManager.h"

TestCameraScript::TestCameraScript() {}

TestCameraScript::~TestCameraScript() {}

void TestCameraScript::Start()
{
	Vec3 pos = GetTransform()->GetLocalPosition();

	float offsetX = pos.x - _orbitPivot.x;
	float offsetZ = pos.z - _orbitPivot.z;

	// 접시-카메라 거리 = 궤도 반지름 길이
	_orbitRadius = sqrtf(offsetX * offsetX + offsetZ * offsetZ);

	// offset : 접시->카메라, forward : 카메라->접시
	// forward = (sin세타, 0, cos세타), camPos = pivot - forward*radius
	// forward = -(offset)/radius 이므로 아크탄젠트 적용!
	_orbitAngle = atan2f(-offsetX, -offsetZ);
}

void TestCameraScript::LateUpdate()
{
	Vec3 pos = GetTransform()->GetLocalPosition();

	if (INPUT->GetButton(KEY_TYPE::W)) pos += GetTransform()->GetLook() * _speed * DELTA_TIME;

	if (INPUT->GetButton(KEY_TYPE::S)) pos -= GetTransform()->GetLook() * _speed * DELTA_TIME;

	if (INPUT->GetButton(KEY_TYPE::A)) pos -= GetTransform()->GetRight() * _speed * DELTA_TIME;

	if (INPUT->GetButton(KEY_TYPE::D)) pos += GetTransform()->GetRight() * _speed * DELTA_TIME;

	// 카메라 방향 회전
	if (INPUT->GetButton(KEY_TYPE::Q))
	{
		Vec3 rotation = GetTransform()->GetLocalRotation();
		rotation.x += DELTA_TIME * 0.5f;
		GetTransform()->SetLocalRotation(rotation);
	}

	if (INPUT->GetButton(KEY_TYPE::E))
	{
		Vec3 rotation = GetTransform()->GetLocalRotation();
		rotation.x -= DELTA_TIME * 0.5f;
		GetTransform()->SetLocalRotation(rotation);
	}

	// 접시 중심 회전
	bool orbiting = false;
	if (INPUT->GetButton(KEY_TYPE::Z))
	{
		_orbitAngle += DELTA_TIME * _orbitSpeed;
		orbiting = true;
	}

	if (INPUT->GetButton(KEY_TYPE::C))
	{
		_orbitAngle -= DELTA_TIME * _orbitSpeed;
		orbiting = true;
	}

	if (orbiting)
	{
		// 바뀐 각도만큼 pos 재계산 후 이동 및 회전 적용
		Vec3 forward = Vec3(sinf(_orbitAngle), 0.f, cosf(_orbitAngle));

		pos.x = _orbitPivot.x - forward.x * _orbitRadius;
		pos.z = _orbitPivot.z - forward.z * _orbitRadius;

		Vec3 rotation = GetTransform()->GetLocalRotation();
		rotation.y = _orbitAngle;
		GetTransform()->SetLocalRotation(rotation);
	}

	if (INPUT->GetButtonDown(KEY_TYPE::RBUTTON))
	{
		const POINT &pos = INPUT->GetMousePos();
		GET_SINGLE(SceneManager)->Pick(pos.x, pos.y);
	}

	GetTransform()->SetLocalPosition(pos);
}