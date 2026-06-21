#include "pch.h"
#include "TableCollider.h"
#include "Scene.h"
#include "GameObject.h"
#include "Transform.h"
#include "PlaneCollider.h"
#include "MeshRenderer.h"
#include "Resources.h"

static const float TABLE_SURFACE_OFS = 0.f;
static const float TABLE_VIS_RADIUS = 100.f;

void TableCollider::Init(shared_ptr<Scene> scene, Vec3 tablePos)
{
	Vec3 surfacePos = Vec3(tablePos.x, tablePos.y + TABLE_SURFACE_OFS, tablePos.z);

	shared_ptr<GameObject> tableFloor = make_shared<GameObject>();
	tableFloor->AddComponent(make_shared<Transform>());
	tableFloor->GetTransform()->SetLocalPosition(surfacePos);
	tableFloor->SetStatic(true);
	shared_ptr<PlaneCollider> planeCol = make_shared<PlaneCollider>();
	planeCol->SetNormal(Vec3(0.f, 1.f, 0.f));
	tableFloor->AddComponent(planeCol);
	scene->AddGameObject(tableFloor);

#ifdef _DEBUG
	// 디버그에는 동그랗게 보이지만 실제로는 무한한 평면으로 설정
	shared_ptr<Material>   debugMat = GET_SINGLE(Resources)->Get<Material>(L"ColliderDebug");
	shared_ptr<GameObject> visObj = make_shared<GameObject>();
	visObj->AddComponent(make_shared<Transform>());
	visObj->GetTransform()->SetLocalPosition(tableFloor->GetTransform()->GetLocalPosition());
	visObj->GetTransform()->SetLocalScale(Vec3(TABLE_VIS_RADIUS, 1.f, TABLE_VIS_RADIUS));
	visObj->SetCheckFrustum(false);
	shared_ptr<MeshRenderer> mr = make_shared<MeshRenderer>();
	mr->SetMesh(GET_SINGLE(Resources)->LoadDiscMesh());
	mr->SetMaterial(debugMat->Clone());
	visObj->AddComponent(mr);
	scene->AddGameObject(visObj);
#endif
}
