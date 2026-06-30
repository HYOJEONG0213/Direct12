#include "pch.h"
#include "PlanetMerger.h"
#include "PlanetData.h"
#include "PlanetFactory.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Transform.h"
#include "GameObject.h"
#include "UIManager.h"

void PlanetMerger::OnCollision(shared_ptr<GameObject> other)
{
	if (_merged) return;

	PlanetData &data = g_PlanetTable[static_cast<int>(_type)];
	if (other->GetName() != data.material) return; // 다른 타입시 ㅈㅈ
	if (data.nextType == PLANET_TYPE::END) return; // TODO: 목성끼리 충돌시 게임 종료

	_merged = true;

	Vec3 pos = GetGameObject()->GetTransform()->GetLocalPosition();
	Vec3 otherPos = other->GetTransform()->GetLocalPosition();
	Vec3 midpoint = (pos + otherPos) * 0.5f;

	auto scene = GET_SINGLE(SceneManager)->GetActiveScene();
	scene->RemoveGameObjectDeferred(GetGameObject());
	scene->RemoveGameObjectDeferred(other);

	GET_SINGLE(UIManager)->AddScore(data.score);

	auto newPlanet = PlanetFactory::CreatePlanet(data.nextType, midpoint);
	scene->AddGameObject(newPlanet);
}
