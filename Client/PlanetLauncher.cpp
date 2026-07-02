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
	float worldX = rayOrigin.x + rayDir.x * distToPlane; // 마우스 X를 착지 평면상의 월드 X로 변환

	UpdateGuideLine(worldX);

	if (INPUT->GetButtonDown(KEY_TYPE::LBUTTON) && _cooldown <= 0.f)
	{
		_cooldown = 0.5f;
		Throw(worldX);
	}
}

float PlanetLauncher::FallTime() const
{
	// s = 1/2 * g * t^2  =>  t = sqrt(2s/g)
	float gravity = Rigidbody::GetGravity();
	return sqrtf(2.f * (_launchY - _spawnY) / gravity);
}

Vec3 PlanetLauncher::ComputeVelocity(float targetX) const
{
	Vec3 launchPos = Vec3(0.f, _launchY, _launchZ);

	// X, Z 방향 속도 계산 (등속 직선 운동)
	float t = FallTime();
	float vx = (targetX - launchPos.x) / t * _powerScale;
	float vz = (_spawnZ - launchPos.z) / t * _powerScale;
	float vy = 0.f;

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

void PlanetLauncher::UpdateGuideLine(float targetX)
{
	if (_current == nullptr) return;

	Vec3  launchPos = _current->GetTransform()->GetLocalPosition();
	Vec3  velocity = ComputeVelocity(targetX);
	float gravity = Rigidbody::GetGravity();

	// 처음만 생성!
	if (_guideDots.empty())
	{
		// 인스턴싱 방법 이용해 궤적 UI 생성
		shared_ptr<Mesh>	 sharedMesh = GET_SINGLE(Resources)->LoadSphereMesh();
		shared_ptr<Material> sharedMaterial = GET_SINGLE(Resources)->Get<Material>(L"GuideDot");

		for (int32 i = 0; i < GUIDE_DOT_COUNT; i++)
		{
			shared_ptr<GameObject> dot = make_shared<GameObject>();
			dot->AddComponent(make_shared<Transform>());
			dot->SetCheckFrustum(false);

			shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
			meshRenderer->SetMesh(sharedMesh);
			meshRenderer->SetMaterial(sharedMaterial);

			dot->AddComponent(meshRenderer);
			GET_SINGLE(SceneManager)->GetActiveScene()->AddGameObject(dot);
			_guideDots.push_back(dot);
		}
	}

	// x = x0 + vx*t, y = y0 + vy*t - 0.5*g*t^2
	float fallTime = FallTime();
	for (int32 i = 0; i < GUIDE_DOT_COUNT; i++)
	{
		float t = fallTime * (float)i / (float)(GUIDE_DOT_COUNT - 1);

		Vec3 pos;
		pos.x = launchPos.x + velocity.x * t;
		pos.y = launchPos.y + velocity.y * t - 0.5f * gravity * t * t;
		pos.z = launchPos.z + velocity.z * t;

		_guideDots[i]->GetTransform()->SetLocalPosition(pos);
		_guideDots[i]->GetTransform()->SetLocalScale(Vec3(1.2f, 1.2f, 1.2f));
	}
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
