#include "pch.h"
#include "BasketCollider.h"
#include "Scene.h"
#include "GameObject.h"
#include "Transform.h"
#include "CylinderCollider.h"
#include "PlaneCollider.h"

static const float BASKET_WALL_R = 24.f;
static const float BASKET_HALF_H = 11.5f;
static const float BASKET_BOTTOM_OFS = -11.f;

void BasketCollider::Init(shared_ptr<Scene> scene, Vec3 basketPos)
{
	float bottomY = basketPos.y + BASKET_BOTTOM_OFS;
	float cylinderCenY = bottomY + BASKET_HALF_H;

	// 옆면
	shared_ptr<GameObject> wallObj = make_shared<GameObject>();
	wallObj->AddComponent(make_shared<Transform>());
	wallObj->GetTransform()->SetLocalPosition(Vec3(basketPos.x, cylinderCenY, basketPos.z));
	wallObj->SetStatic(true);
	shared_ptr<CylinderCollider> cylCol = make_shared<CylinderCollider>();
	cylCol->SetRadius(BASKET_WALL_R);
	cylCol->SetHalfHeight(BASKET_HALF_H);
	wallObj->AddComponent(cylCol);
	scene->AddGameObject(wallObj);

	// 바닥
	shared_ptr<GameObject> floorObj = make_shared<GameObject>();
	floorObj->AddComponent(make_shared<Transform>());
	floorObj->GetTransform()->SetLocalPosition(Vec3(basketPos.x, bottomY, basketPos.z));
	floorObj->SetStatic(true);
	shared_ptr<PlaneCollider> planeCol = make_shared<PlaneCollider>();
	planeCol->SetNormal(Vec3(0.f, 1.f, 0.f));
	floorObj->AddComponent(planeCol);
	scene->AddGameObject(floorObj);
}
