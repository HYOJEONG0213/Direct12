#pragma once
#include "Component.h"

class MonoBehaviour : public Component
{
public:
	MonoBehaviour();
	virtual ~MonoBehaviour();

	virtual void OnCollision(shared_ptr<class GameObject> other) {}

private:
	virtual void FinalUpdate() sealed {}
};
