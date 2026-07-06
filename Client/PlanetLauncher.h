#pragma once
#include "MonoBehaviour.h"
#include "PlanetData.h"

class PlanetLauncher : public MonoBehaviour
{
public:
	PlanetLauncher() = default;
	virtual ~PlanetLauncher() = default;

	virtual void LateUpdate() override;

	void		SetNextType(PLANET_TYPE type) { _nextType = type; }
	PLANET_TYPE GetNextType() const { return _nextType; }
	Vec3		GetLaunchPos();

	void SetPreviewObject(shared_ptr<class GameObject> obj) { _preview = obj; }
	void SetCameraScript(shared_ptr<class TestCameraScript> script) { _cameraScript = script; }

private:
	void  UpdatePreview();
	void  SpawnCurrent(const Vec3 &launchPos);
	void  Throw(const Vec3 &launchPos, const Vec3 &targetPos);
	Vec3  ComputeVelocity(const Vec3 &launchPos, const Vec3 &targetPos) const;
	void  UpdateGuideLine(const Vec3 &launchPos, const Vec3 &targetPos);
	float FallTime(float launchY) const;

	static constexpr int				 GUIDE_DOT_COUNT = 200;
	vector<shared_ptr<class GameObject>> _guideDots;

	float _cooldown = 0.f;

	float _spawnY = -30.f; // 착지 높이

	float _launchY = 5.f;  // 발사대 높이
	float _launchZ = 40.f; // 발사대 깊이

	float _powerScale = 0.7f; // 거리조절용 상수

	PLANET_TYPE					 _currentType = PLANET_TYPE::MERCURY;
	PLANET_TYPE					 _nextType = PLANET_TYPE::MERCURY;
	shared_ptr<class GameObject> _current;
	shared_ptr<class GameObject> _preview;

	shared_ptr<class TestCameraScript> _cameraScript;
};
