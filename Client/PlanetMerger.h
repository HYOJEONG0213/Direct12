#pragma once
#include "MonoBehaviour.h"
#include "PlanetData.h"

class PlanetMerger : public MonoBehaviour
{
public:
	PlanetMerger(PLANET_TYPE type) : _type(type) {}
	virtual ~PlanetMerger() {}

	virtual void OnCollision(shared_ptr<GameObject> other) override;

private:
	PLANET_TYPE _type;
	bool		_merged = false;
};
