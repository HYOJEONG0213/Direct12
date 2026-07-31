#pragma once

class GameObject;

// 각 게임오브젝트 시점에 맞춰 호출하기
class Scene
{
public:
	void Awake();
	void Start();
	void Update();
	void LateUpdate();
	void PhysicsUpdate();
	void FinalUpdate();

	shared_ptr<class Camera> GetMainCamera();

	void Render();

	void ClearRTV();

	void RenderShadow();
	void RenderDeferred();
	void RenderLights();
	void RenderFinal();

	void RenderForward();

private:
	void PushLightData();

public:
	void AddGameObject(shared_ptr<GameObject> gameObject);
	void RemoveGameObject(shared_ptr<GameObject> gameObject);
	void RemoveGameObjectDeferred(shared_ptr<GameObject> gameObject);

	const vector<shared_ptr<GameObject>> &GetGameObjects() { return _gameObjects; }

private:
	void FlushDeferredObjects();
	void FlushDeferredRemovals();

private:
	vector<shared_ptr<GameObject>>	 _gameObjects;
	vector<shared_ptr<GameObject>>	 _deferredObjects;
	vector<shared_ptr<GameObject>>	 _deferredRemovals;
	bool							 _isUpdating = false;
	vector<shared_ptr<class Camera>> _cameras;
	vector<shared_ptr<class Light>>	 _lights;
};
