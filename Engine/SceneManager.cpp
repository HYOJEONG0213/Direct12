#include "pch.h"
#include "SceneManager.h"
#include "Scene.h"
#include "PhysicsManager.h"

#include "Engine.h"
#include "Camera.h"
#include "GameObject.h"
#include "BaseCollider.h"

void SceneManager::Update()
{
	if (_activeScene == nullptr) return;

	_activeScene->Update();
	_activeScene->LateUpdate();
	_activeScene->PhysicsUpdate();
	_activeScene->FinalUpdate();
}

void SceneManager::Render()
{
	if (_activeScene) _activeScene->Render();
}

void SceneManager::LoadScene(shared_ptr<Scene> scene)
{
	_activeScene = scene;

	_activeScene->Awake();
	_activeScene->Start();

	// 정적 콜라이더 수집 한 번만!!
	GET_SINGLE(PhysicsManager)->Init(_activeScene.get());
}

shared_ptr<GameObject> SceneManager::Pick(int32 screenX, int32 screenY)
{
	shared_ptr<Camera> camera = GetActiveScene()->GetMainCamera();

	float width = static_cast<float>(GEngine->GetWindow().width);
	float height = static_cast<float>(GEngine->GetWindow().height);

	Matrix projectionMatrix = camera->GetProjectionMatrix();

	// ViewSpace에서 Picking 진행
	float viewX = (+2.0f * screenX / width - 1.0f) / projectionMatrix(0, 0);
	float viewY = (-2.0f * screenY / height + 1.0f) / projectionMatrix(1, 1);

	Matrix viewMatrix = camera->GetViewMatrix();
	Matrix viewMatrixInv = viewMatrix.Invert();

	auto &gameObjects = GET_SINGLE(SceneManager)->GetActiveScene()->GetGameObjects();

	float				   minDistance = FLT_MAX;
	shared_ptr<GameObject> picked;

	for (auto &gameObject : gameObjects)
	{
		if (gameObject->GetCollider() == nullptr) continue;

		// ViewSpace에서의 Ray 정의
		Vec4 rayOrigin = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
		Vec4 rayDir = Vec4(viewX, viewY, 1.0f, 0.0f);

		// WorldSpace에서의 Ray 정의
		rayOrigin = XMVector3TransformCoord(rayOrigin, viewMatrixInv);
		rayDir = XMVector3TransformNormal(rayDir, viewMatrixInv);
		rayDir.Normalize();

		// WorldSpace에서 연산
		float distance = 0.f;
		if (gameObject->GetCollider()->Intersects(rayOrigin, rayDir, OUT distance) == false) continue;

		if (distance < minDistance)
		{
			minDistance = distance;
			picked = gameObject;
		}
	}

	return picked;
}

void SceneManager::SetLayerName(uint8 index, const wstring &name)
{
	// 기존 데이터 삭제
	const wstring &prevName = _layerNames[index];
	_layerIndex.erase(prevName);

	_layerNames[index] = name;
	_layerIndex[name] = index;
}

uint8 SceneManager::LayerNameToIndex(const wstring &name)
{
	auto findIt = _layerIndex.find(name);
	if (findIt == _layerIndex.end()) return 0;

	return findIt->second;
}
