#include "pch.h"
#include "TableCollider.h"
#include "Scene.h"
#include "GameObject.h"
#include "Transform.h"
#include "PlaneCollider.h"
#include "MeshRenderer.h"
#include "Resources.h"

void TableCollider::Init(shared_ptr<Scene> scene, Vec3 tablePos, float radius)
{
	shared_ptr<GameObject> tableFloor = make_shared<GameObject>();
	tableFloor->AddComponent(make_shared<Transform>());
	tableFloor->GetTransform()->SetLocalPosition(tablePos);
	tableFloor->SetStatic(true);
	shared_ptr<PlaneCollider> planeCol = make_shared<PlaneCollider>();
	planeCol->SetNormal(Vec3(0.f, 1.f, 0.f));
	planeCol->SetRadius(radius);
	tableFloor->AddComponent(planeCol);
	scene->AddGameObject(tableFloor);

#ifdef _DEBUG
	shared_ptr<Material>   debugMat = GET_SINGLE(Resources)->Get<Material>(L"ColliderDebug");
	shared_ptr<GameObject> visObj = make_shared<GameObject>();
	visObj->AddComponent(make_shared<Transform>());
	visObj->GetTransform()->SetLocalPosition(tablePos);
	visObj->GetTransform()->SetLocalScale(Vec3(radius, 1.f, radius));
	visObj->SetCheckFrustum(false);
	shared_ptr<MeshRenderer> mr = make_shared<MeshRenderer>();
	mr->SetMesh(GET_SINGLE(Resources)->LoadDiscMesh());
	mr->SetMaterial(debugMat->Clone());
	visObj->AddComponent(mr);
	scene->AddGameObject(visObj);
#endif
}
