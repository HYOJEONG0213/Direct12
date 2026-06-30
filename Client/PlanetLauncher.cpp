#include "pch.h"
#include "PlanetLauncher.h"
#include "Input.h"
#include "Timer.h"
#include "SceneManager.h"
#include "PlanetFactory.h"
#include "Engine.h"
#include "Scene.h"
#include "GameObject.h"
#include "Resources.h"
#include "Material.h"
#include "Shader.h"
#include "Texture.h"
#include "MeshRenderer.h"
#include "Rigidbody.h"
#include "Transform.h"

void PlanetLauncher::Update()
{
	_cooldown -= DELTA_TIME;

	if (_current == nullptr) SpawnCurrent(); // 발사대에 공 채워넣기

	// 마우스 X좌표 = 착지 지점
	POINT mouse = INPUT->GetMousePos();
	Vec4  rayOrigin, rayDir;
	GET_SINGLE(SceneManager)->GetPickRay(mouse.x, mouse.y, rayOrigin, rayDir);

	float distToPlane = (_spawnZ - rayOrigin.z) / rayDir.z;
	float worldX = rayOrigin.x + rayDir.x * distToPlane; // 가야하는 z축 양만큼 x축 이동

	/* 디버깅용, 마우스 위치에 수직으로 행성 떨어트리기!
	auto planet = PlanetFactory::CreatePlanet(_nextType, Vec3(worldX, _spawnY, _spawnZ));
	GET_SINGLE(SceneManager)->GetActiveScene()->AddGameObject(planet);

	_nextType = static_cast<PLANET_TYPE>(rand() % (static_cast<int>(PLANET_TYPE::VENUS) + 1));

	UpdatePreview();*/

	if (INPUT->GetButtonDown(KEY_TYPE::LBUTTON) && _cooldown <= 0.f)
	{
		_cooldown = 0.5f;
		Throw(worldX);
	}
}

Vec3 PlanetLauncher::ComputeVelocity(float targetX) const
{
	Vec3 launchPos = Vec3(0.f, _launchY, _launchZ);

	// X, Z 방향 속도 계산 (등속 직선 운동)
	float vx = (targetX - launchPos.x) / _t;
	float vz = (_spawnZ - launchPos.z) / _t;
	float vy = _launchPower;

	return Vec3(vx, vy, vz);
}

void PlanetLauncher::SpawnCurrent()
{
	Vec3 launchPos = Vec3(0.f, _launchY, _launchZ);

	_current = PlanetFactory::CreatePlanet(_currentType, launchPos);
	_current->GetRigidbody()->SetUseGravity(false); // 발사 전까지 제자리 고정

	GET_SINGLE(SceneManager)->GetActiveScene()->AddGameObject(_current);
}

void PlanetLauncher::Throw(float targetX)
{
	Vec3 velocity = ComputeVelocity(targetX);

	_current->GetRigidbody()->SetUseGravity(true);
	_current->GetRigidbody()->SetVelocity(velocity);
	_current = nullptr;

	_currentType = _nextType;
	_nextType = static_cast<PLANET_TYPE>(rand() % (static_cast<int>(PLANET_TYPE::VENUS) + 1));
	UpdatePreview();
}

void PlanetLauncher::UpdatePreview()
{
	if (_preview == nullptr) return;

	PlanetData			&data = g_PlanetTable[static_cast<int>(_nextType)];
	shared_ptr<Material> material = GET_SINGLE(Resources)->Get<Material>(data.material);
	if (material != nullptr)
	{
		shared_ptr<Material> uiMat = make_shared<Material>();
		uiMat->SetShader(GET_SINGLE(Resources)->Get<Shader>(L"Texture"));
		uiMat->SetTexture(0, material->GetTexture(0));
		_preview->GetMeshRenderer()->SetMaterial(uiMat);
	}
}
