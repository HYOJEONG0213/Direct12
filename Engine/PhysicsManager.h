#pragma once
class PlaneCollider;

class PhysicsManager
{
	DECLARE_SINGLE(PhysicsManager);

public:
	void Init(class Scene *scene);
	void Update(class Scene *scene);

private:
	vector<shared_ptr<PlaneCollider>> planes;
};
