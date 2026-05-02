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
	void FinalUpdate();

	void Render();

private:
	void PushLightData();

public:
	void AddGameObject(shared_ptr<GameObject> gameObject);
	void RemoveGameObject(shared_ptr<GameObject> gameObject);

	const vector<shared_ptr<GameObject>> &GetGameObjects() { return _gameObjects; }

private:
	vector<shared_ptr<GameObject>> _gameObjects;
};
