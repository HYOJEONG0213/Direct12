#pragma once
#include "PlanetData.h"
class GameObject;

extern PlanetData g_PlanetTable[];

class PlanetFactory
{
public:
	static shared_ptr<GameObject> CreatePlanet(PLANET_TYPE type, Vec3 position);
};
