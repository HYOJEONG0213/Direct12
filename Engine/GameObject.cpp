#include "pch.h"
#include "GameObject.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "MonoBehaviour.h"

GameObject::GameObject() {}

GameObject::~GameObject() {}

void GameObject::Init() { AddComponent(make_shared<Transform>()); }

void GameObject::Awake()
{
	for (shared_ptr<Component> &component : _components)
	{
		if (component) component->Awake();
	}

	for (shared_ptr<MonoBehaviour> &script : _scripts) { script->Awake(); }
}

void GameObject::Start()
{
	for (shared_ptr<Component> &component : _components)
	{
		if (component) component->Start();
	}

	for (shared_ptr<MonoBehaviour> &script : _scripts) { script->Start(); }
}

void GameObject::Update()
{
	for (shared_ptr<Component> &component : _components)
	{
		if (component) component->Update();
	}

	for (shared_ptr<MonoBehaviour> &script : _scripts) { script->Update(); }
}

void GameObject::LateUpdate()
{
	for (shared_ptr<Component> &component : _components)
	{
		if (component) component->LateUpdate();
	}

	for (shared_ptr<MonoBehaviour> &script : _scripts) { script->LateUpdate(); }
}

void GameObject::FinalUpdate()
{
	for (shared_ptr<Component> &component : _components)
	{
		if (component) component->FinalUpdate();
	}
}

shared_ptr<Transform> GameObject::GetTransform()
{
	uint8 index = static_cast<uint8>(COMPONENT_TYPE::TRANSFORM);
	return static_pointer_cast<Transform>(_components[index]);
}

void GameObject::AddComponent(shared_ptr<Component> component)
{
	// shared_from_this() : 자기 자신에 대한 포인터 만듦
	component->SetGameObject(shared_from_this()); // 너희 부모는 나다!

	uint8 index = static_cast<uint8>(component->GetType());
	// 모노비헤이비어가 아니라면~
	if (index < FIXED_COMPONENT_COUNT) { _components[index] = component; }
	else { _scripts.push_back(dynamic_pointer_cast<MonoBehaviour>(component)); }
}