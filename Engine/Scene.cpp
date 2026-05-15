#include "pch.h"
#include "Scene.h"
#include "GameObject.h"
#include "Camera.h"
#include "Light.h"
#include "Engine.h"
#include "ConstantBuffer.h"

void Scene::Awake()
{
	for (const shared_ptr<GameObject> &gameObject : _gameObjects) { gameObject->Awake(); }
}

void Scene::Start()
{
	for (const shared_ptr<GameObject> &gameObject : _gameObjects) { gameObject->Start(); }
}

void Scene::Update()
{
	for (const shared_ptr<GameObject> &gameObject : _gameObjects) { gameObject->Update(); }
}

void Scene::LateUpdate()
{
	for (const shared_ptr<GameObject> &gameObject : _gameObjects) { gameObject->LateUpdate(); }
}

void Scene::FinalUpdate()
{
	for (const shared_ptr<GameObject> &gameObject : _gameObjects) { gameObject->FinalUpdate(); }
}

void Scene::Render()
{
	PushLightData();

	// SwapChain Group 초기화
	int8 backIndex = GEngine->GetSwapChain()->GetBackBufferIndex();
	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)->ClearRenderTargetView(backIndex);

	// Deferred Group 초기화
	GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::G_BUFFER)->ClearRenderTargetView();

	for (auto &gameObject : _gameObjects)
	{
		if (gameObject->GetCamera() == nullptr) continue;

		// Deffered 쉐이더와 Forward 쉐이더는 렌더링 방식이 다르기 때문에 카메라가 정렬을 해주는게 좋음
		gameObject->GetCamera()->SortGameObject();

		// Deferred OMSet
		GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::G_BUFFER)->OMSetRenderTargets();
		gameObject->GetCamera()->Render_Deferred();

		// Light OMSet

		// Swapchain OMSet
		GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)->OMSetRenderTargets(1, backIndex);
		gameObject->GetCamera()->Render_Forward();
	}
}

// 한 프레임에 한번씩만 해준다
void Scene::PushLightData()
{
	LightParams lightParams = {};

	for (auto &gameObject : _gameObjects)
	{
		if (gameObject->GetLight() == nullptr) continue;

		const LightInfo &lightInfo = gameObject->GetLight()->GetLightInfo();

		lightParams.lights[lightParams.lightCount] = lightInfo;
		lightParams.lightCount++;
	}

	CONST_BUFFER(CONSTANT_BUFFER_TYPE::GLOBAL)->SetGlobalData(&lightParams, sizeof(lightParams));
}

void Scene::AddGameObject(shared_ptr<GameObject> gameObject) { _gameObjects.push_back(gameObject); }

void Scene::RemoveGameObject(shared_ptr<GameObject> gameObject)
{
	auto findIt = std::find(_gameObjects.begin(), _gameObjects.end(), gameObject);
	if (findIt != _gameObjects.end()) { _gameObjects.erase(findIt); }
}