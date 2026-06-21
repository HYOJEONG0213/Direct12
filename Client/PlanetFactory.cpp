#include "pch.h"
#include "PlanetFactory.h"
#include "PlanetData.h"
#include "GameObject.h"
#include "Transform.h"
#include "SphereCollider.h"
#include "MeshRenderer.h"
#include "Resources.h"

PlanetData g_PlanetTable[] = {
	//			현재 타입, 다음 진화 타입,	 스케일, 질량, 점수, 매터리얼
	{PLANET_TYPE::MERCURY, PLANET_TYPE::MARS, 5.0f, 1.0f, 2, L"Mercury"},
	{PLANET_TYPE::MARS, PLANET_TYPE::VENUS, 7.5f, 1.5f, 4, L"Mars"},
	{PLANET_TYPE::VENUS, PLANET_TYPE::EARTH, 10.0f, 2.0f, 8, L"Venus"},
	{PLANET_TYPE::EARTH, PLANET_TYPE::NEPTUNE, 15.0f, 3.0f, 16, L"Earth"},
	{PLANET_TYPE::NEPTUNE, PLANET_TYPE::URANUS, 22.5f, 4.5f, 32, L"Neptune"},
	{PLANET_TYPE::URANUS, PLANET_TYPE::SATURN, 32.5f, 6.5f, 64, L"Uranus"},
	{PLANET_TYPE::SATURN, PLANET_TYPE::JUPITER, 47.5f, 9.5f, 128, L"Saturn"},
	{PLANET_TYPE::JUPITER, PLANET_TYPE::END, 81.0f, 16.0f, 256, L"Jupiter"}};

shared_ptr<GameObject> PlanetFactory::CreatePlanet(PLANET_TYPE type, Vec3 spawnPos)
{
	PlanetData &data = g_PlanetTable[static_cast<int>(type)];

	shared_ptr<GameObject> obj = make_shared<GameObject>();
	obj->SetName(data.material);

	// Transform
	obj->AddComponent(make_shared<Transform>());
	obj->GetTransform()->SetLocalScale(Vec3(data.scale, data.scale, data.scale));
	obj->GetTransform()->SetLocalPosition(spawnPos);

	// Collider
	shared_ptr<SphereCollider> collider = make_shared<SphereCollider>();
	collider->SetRadius(0.8f);
	obj->AddComponent(collider);

	// 물리 최적화
	obj->SetCheckFrustum(false);
	obj->SetStatic(false);

	// MeshRenderer
	shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
	shared_ptr<Mesh>		 sphereMesh = GET_SINGLE(Resources)->LoadSphereMesh();
	meshRenderer->SetMesh(sphereMesh);

	shared_ptr<Material> material = GET_SINGLE(Resources)->Get<Material>(data.material);
	if (material != nullptr) { meshRenderer->SetMaterial(material->Clone()); }

	obj->AddComponent(meshRenderer);

	return obj;
}