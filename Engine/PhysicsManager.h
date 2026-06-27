#pragma once
class PlaneCollider;
class SphereCollider;
class Rigidbody;
class GameObject;

class PhysicsManager
{
	DECLARE_SINGLE(PhysicsManager);

public:
	void Init(class Scene *scene);
	void Update(class Scene *scene);
	void RegisterDynamic(shared_ptr<GameObject> object);

private:
	struct SphereData
	{
		shared_ptr<GameObject>	   object;
		shared_ptr<Rigidbody>	   rigid;
		shared_ptr<SphereCollider> collider;
		float					   radius;
		float					   mass;
	};

	vector<shared_ptr<PlaneCollider>> _planes;
	vector<SphereData>				  _spheres;

	static constexpr float COR = 0.1f; // 반발계수
};
