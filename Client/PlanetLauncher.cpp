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
#include "MeshRenderer.h"

void PlanetLauncher::Update()
{
	_cooldown -= DELTA_TIME;

	if (INPUT->GetButtonDown(KEY_TYPE::LBUTTON) && _cooldown <= 0.f)
	{
		_cooldown = 0.5f;

		// 임시!! 마우스 X좌표에 행성 생성
		// TODO: 포물선 운동 구현 필요!!
		POINT mouse = INPUT->GetMousePos();
		Vec4  rayOrigin, rayDir;
		GET_SINGLE(SceneManager)->GetPickRay(mouse.x, mouse.y, rayOrigin, rayDir);

		float distToPlane = (_spawnZ - rayOrigin.z) / rayDir.z;
		float worldX = rayOrigin.x + rayDir.x * distToPlane; // 가야하는 z축 양만큼 x축 이동

		auto planet = PlanetFactory::CreatePlanet(_nextType, Vec3(worldX, _spawnY, _spawnZ));
		GET_SINGLE(SceneManager)->GetActiveScene()->AddGameObject(planet);

		_nextType = static_cast<PLANET_TYPE>(rand() % (static_cast<int>(PLANET_TYPE::VENUS) + 1));

		UpdatePreview();
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
