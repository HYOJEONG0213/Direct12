#pragma once
enum class PLANET_TYPE
{
	MERCURY,
	MARS,
	VENUS,
	EARTH,
	NEPTUNE,
	URANUS,
	SATURN,
	JUPITER,
	END
};

struct PlanetData
{
	PLANET_TYPE	 type;
	PLANET_TYPE	 nextType;
	float		 scale;
	float		 mass;
	int			 score;
	std::wstring material;
};