#pragma once

enum class COMPONENT_TYPE : uint8
{
	TRANSFORM,
	MESH_RENDERER,
	// 기타 컴포넌트들
	MONO_BEHAVIOUR, // 꼭 마지막에 위치!
	END,
};

enum
{
	FIXED_COMPONENT_COUNT = static_cast<uint8>(COMPONENT_TYPE::END) - 1 // MONO_BEHAVIOUR 는 미포함
};

class GameObject;
class Transform;

class Component
{
public:
	Component(COMPONENT_TYPE type); // base 클래스로 사용 예정
	virtual ~Component();			// 이므로 여기엔 virtual

public:
	virtual void Awake() {}
	virtual void Start() {}
	virtual void Update() {}
	virtual void LateUpdate() {}

public:
	COMPONENT_TYPE GetType() { return _type; }
	bool		   IsValid() { return _gameObject.expired() == false; }

	shared_ptr<GameObject> GetGameObject();
	shared_ptr<Transform>  GetTransform();

private:
	friend class GameObject;
	void SetGameObject(shared_ptr<GameObject> gameObject) { _gameObject = gameObject; }

protected:
	COMPONENT_TYPE		 _type;
	weak_ptr<GameObject> _gameObject; // shared_ptr 양쪽에서 하면 순환구조 만들어짐;;
};
