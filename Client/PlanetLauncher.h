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

	void SetPreviewObject(shared_ptr<class GameObject> obj) { _preview = obj; }

private:
	void UpdatePreview();
	void SpawnCurrent();
	void Throw(float targetX);
	Vec3 ComputeVelocity(float targetX) const;

	float _cooldown = 0.f;

	float _spawnY = 100.f; // 착지 높이
	float _spawnZ = 200.f; // 착지 깊이

	float _launchY = 6.f;	// 발사대 높이
	float _launchZ = 120.f; // 발사대 깊이
	float _launchPower = 1.f;
	float _t = 2.5f;

	PLANET_TYPE					 _currentType = PLANET_TYPE::MERCURY;
	PLANET_TYPE					 _nextType = PLANET_TYPE::MERCURY;
	shared_ptr<class GameObject> _current;
	shared_ptr<class GameObject> _preview;
};
