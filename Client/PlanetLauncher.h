#pragma once
#include "MonoBehaviour.h"
#include "PlanetData.h"

class PlanetLauncher : public MonoBehaviour
{
public:
	PlanetLauncher() = default;
	virtual ~PlanetLauncher() = default;

	virtual void Update() override;

	void		SetNextType(PLANET_TYPE type) { _nextType = type; }
	PLANET_TYPE GetNextType() const { return _nextType; }

private:
	PLANET_TYPE _nextType = PLANET_TYPE::MERCURY;
	float		_cooldown = 0.f;

	float _spawnY = 100.f;
	float _spawnZ = 200.f;
};
