#include "pch.h"
#include "BasketCollider.h"
#include "Scene.h"
#include "GameObject.h"
#include "Transform.h"
#include "CylinderCollider.h"
#include "PlaneCollider.h"
#include "MeshRenderer.h"
#include "Resources.h"

static const float BASKET_WALL_R = 60.f;
static const float BASKET_HALF_H = 28.75f;
static const float BASKET_BOTTOM_OFS = -27.5f;

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

#ifdef _DEBUG
	shared_ptr<Material> debugMat = GET_SINGLE(Resources)->Get<Material>(L"ColliderDebug");

	// 옆면
	{
		shared_ptr<GameObject> visObj = make_shared<GameObject>();
		visObj->AddComponent(make_shared<Transform>());
		visObj->GetTransform()->SetLocalPosition(Vec3(basketPos.x, cylinderCenY, basketPos.z));
		visObj->GetTransform()->SetLocalScale(
			Vec3(cylCol->GetRadius(), cylCol->GetHalfHeight() * 2.f, cylCol->GetRadius()));
		visObj->SetCheckFrustum(false);
		shared_ptr<MeshRenderer> mr = make_shared<MeshRenderer>();
		mr->SetMesh(GET_SINGLE(Resources)->LoadCylinderMesh());
		mr->SetMaterial(debugMat->Clone());
		visObj->AddComponent(mr);
		scene->AddGameObject(visObj);
	}

	// 바닥
	{
		shared_ptr<GameObject> visObj = make_shared<GameObject>();
		visObj->AddComponent(make_shared<Transform>());
		visObj->GetTransform()->SetLocalPosition(floorObj->GetTransform()->GetLocalPosition());
		visObj->GetTransform()->SetLocalScale(Vec3(cylCol->GetRadius(), 1.f, cylCol->GetRadius()));
		visObj->SetCheckFrustum(false);
		shared_ptr<MeshRenderer> mr = make_shared<MeshRenderer>();
		mr->SetMesh(GET_SINGLE(Resources)->LoadDiscMesh());
		mr->SetMaterial(debugMat->Clone());
		visObj->AddComponent(mr);
		scene->AddGameObject(visObj);
	}
#endif
}
