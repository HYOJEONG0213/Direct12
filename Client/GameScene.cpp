#include "pch.h"
#include "GameScene.h"

#include "MeshRenderer.h"
#include "Engine.h"
#include "Scene.h"
#include "SceneManager.h"
#include "Resources.h"
#include "Material.h"
#include "GameObject.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "Transform.h"
#include "Camera.h"
#include "Light.h"
#include "SphereCollider.h"
#include "BasketCollider.h"
#include "TableCollider.h"
#include "MeshData.h"
#include "TestCameraScript.h"
#include "ParticleSystem.h"

#include "PlanetFactory.h"
#include "PlanetLauncher.h"

shared_ptr<Scene> LoadGameScene()
{
#pragma region LayerMask
	GET_SINGLE(SceneManager)->SetLayerName(0, L"Default");
	GET_SINGLE(SceneManager)->SetLayerName(1, L"UI");
#pragma endregion

#pragma region ComputeShader
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"ComputeShader");

		// UAV 용 Texture 생성
		shared_ptr<Texture> texture = GET_SINGLE(Resources)->CreateTexture(
			L"UAVTexture", DXGI_FORMAT_R8G8B8A8_UNORM, 1024, 1024, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

		shared_ptr<Material> material = GET_SINGLE(Resources)->Get<Material>(L"ComputeShader");
		material->SetShader(shader);
		material->SetInt(0, 1);
		GEngine->GetComputeDescHeap()->SetUAV(texture->GetUAVHandle(), UAV_REGISTER::u0);

		// 쓰레드 그룹 (1 * 1024 * 1)
		material->Dispatch(1, 1024, 1);
	}
#pragma endregion

	shared_ptr<Scene> scene = make_shared<Scene>();

#pragma region Camera
	shared_ptr<GameObject> camera = make_shared<GameObject>();
	camera->SetName(L"Main_Camera");
	camera->AddComponent(make_shared<Transform>());
	camera->AddComponent(make_shared<Camera>()); // Near:1, Far:1000, Fov:45도
	camera->GetCamera()->SetFar(10000.f);
	camera->AddComponent(make_shared<TestCameraScript>());
	camera->AddComponent(make_shared<PlanetLauncher>());

	camera->GetTransform()->SetLocalPosition(Vec3(0.f, 80.f, 30.f));
	camera->GetTransform()->SetLocalRotation(Vec3(0.4f, 0.f, 0.f));

	uint8 layerIndex = GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI");
	camera->GetCamera()->SetCullingMaskLayerOnOff(layerIndex, true); // UI는 안 찍음

	scene->AddGameObject(camera);

#pragma endregion

#pragma region UI_Camera
	{
		shared_ptr<GameObject> camera = make_shared<GameObject>();
		camera->SetName(L"Orthographic_Camera");
		camera->AddComponent(make_shared<Transform>());
		camera->AddComponent(make_shared<Camera>()); // Near=1, Far=1000, 800*600
		camera->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 0.f));
		camera->GetCamera()->SetProjectionType(PROJECTION_TYPE::ORTHOGRAPHIC);
		uint8 layerIndex = GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI");
		camera->GetCamera()->SetCullingMaskAll();						  // 다 끄고
		camera->GetCamera()->SetCullingMaskLayerOnOff(layerIndex, false); // UI만 찍음
		scene->AddGameObject(camera);
	}
#pragma endregion

#pragma region SkyBox
	{
		shared_ptr<GameObject> skybox = make_shared<GameObject>();
		skybox->AddComponent(make_shared<Transform>());
		skybox->SetCheckFrustum(false); // 무조건 렌더링
		auto			 meshRenderer = make_shared<MeshRenderer>();
		shared_ptr<Mesh> sphereMesh = GET_SINGLE(Resources)->LoadSphereMesh();
		meshRenderer->SetMesh(sphereMesh);

		shared_ptr<Shader>	skyboxShader = GET_SINGLE(Resources)->Get<Shader>(L"Skybox");
		shared_ptr<Texture> skyboxTexture =
			GET_SINGLE(Resources)->Load<Texture>(L"Sky01", L"..\\Resources\\Texture\\Sky01.jpg");
		shared_ptr<Material> skyboxMaterial = make_shared<Material>();
		skyboxMaterial->SetShader(skyboxShader);
		skyboxMaterial->SetTexture(0, skyboxTexture);
		meshRenderer->SetMaterial(skyboxMaterial);

		skybox->AddComponent(meshRenderer);
		scene->AddGameObject(skybox);
	}
#pragma endregion

	// #pragma region Terrain
	//	{
	//		shared_ptr<GameObject> obj = make_shared<GameObject>();
	//		obj->AddComponent(make_shared<Transform>());
	//		obj->AddComponent(make_shared<Terrain>());
	//		obj->AddComponent(make_shared<MeshRenderer>());
	//
	//		obj->GetTransform()->SetLocalScale(Vec3(50.f, 250.f, 50.f));
	//		obj->GetTransform()->SetLocalPosition(Vec3(-100.f, -200.f, 300.f));
	//		obj->SetStatic(true);
	//		obj->GetTerrain()->Init(64, 64);
	//		obj->SetCheckFrustum(false);
	//
	//		scene->AddGameObject(obj);
	//	}
	// #pragma endregion

#pragma region UI_Test
	for (int32 i = 0; i < 6; i++)
	{
		shared_ptr<GameObject> obj = make_shared<GameObject>();
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI")); // UI
		obj->AddComponent(make_shared<Transform>());
		obj->GetTransform()->SetLocalScale(Vec3(100.f, 100.f, 100.f));
		obj->GetTransform()->SetLocalPosition(Vec3(-350.f + (i * 120), 250.f, 500.f));
		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Texture");

			shared_ptr<Texture> texture;
			if (i < 3)
				texture = GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::G_BUFFER)->GetRTTexture(i);
			else if (i < 5)
				texture = GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::LIGHTING)->GetRTTexture(i - 3);
			else
				texture = GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SHADOW)->GetRTTexture(0);

			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			meshRenderer->SetMaterial(material);
		}
		obj->AddComponent(meshRenderer);
		scene->AddGameObject(obj);
	}
#pragma endregion

#pragma region Directional Light
	{
		shared_ptr<GameObject> light = make_shared<GameObject>();
		light->AddComponent(make_shared<Transform>());
		light->GetTransform()->SetLocalPosition(Vec3(0, 1000, 500));
		light->AddComponent(make_shared<Light>());
		light->GetLight()->SetLightDirection(Vec3(0, -1, 1.f));
		light->GetLight()->SetLightType(LIGHT_TYPE::DIRECTIONAL_LIGHT);
		light->GetLight()->SetDiffuse(Vec3(1.f, 1.f, 1.f));
		light->GetLight()->SetAmbient(Vec3(0.1f, 0.1f, 0.1f));
		light->GetLight()->SetSpecular(Vec3(0.1f, 0.1f, 0.1f));

		scene->AddGameObject(light);
	}
#pragma endregion

#pragma region PlanetFBX
	{
		shared_ptr<MeshData> meshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\PlanetsFBX.fbx");
	}
#pragma endregion
#pragma region PlateFBX
	{
		shared_ptr<MeshData> meshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Plate.fbx");

		vector<shared_ptr<GameObject>> gameObjects = meshData->Instantiate();
		shared_ptr<Material>		   plateMaterial = GET_SINGLE(Resources)->Get<Material>(L"Plate");

		Vec3 platePos = Vec3(0.f, -45.f, 200.f);

		for (auto &gameObject : gameObjects)
		{
			gameObject->SetName(L"Plate");
			gameObject->SetCheckFrustum(false);
			gameObject->GetTransform()->SetLocalPosition(platePos);
			gameObject->GetTransform()->SetLocalScale(Vec3(10.f, 10.f, 10.f));
			gameObject->GetMeshRenderer()->SetMaterial(plateMaterial->Clone());
			scene->AddGameObject(gameObject);
		}

		TableCollider::Init(scene, platePos, 44.f);
	}
#pragma endregion
#pragma region TableFBX
	{
		shared_ptr<MeshData> meshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Table.fbx");

		vector<shared_ptr<GameObject>> gameObjects = meshData->Instantiate();
		shared_ptr<Material>		   tableMaterial = GET_SINGLE(Resources)->Get<Material>(L"Table");

		Vec3 tablePos = Vec3(0.f, -55.f, 200.f);

		for (auto &gameObject : gameObjects)
		{
			gameObject->SetName(L"Table");
			gameObject->SetCheckFrustum(false);
			gameObject->GetTransform()->SetLocalPosition(tablePos);
			gameObject->GetTransform()->SetLocalScale(Vec3(250.f, 250.f, 250.f));
			gameObject->GetMeshRenderer()->SetMaterial(tableMaterial->Clone());
			scene->AddGameObject(gameObject);
		}

		TableCollider::Init(scene, tablePos + Vec3(0.f, 10.f, 0.f), 97.f);
	}
#pragma endregion

	shared_ptr<GameObject> jupiter = PlanetFactory::CreatePlanet(PLANET_TYPE::JUPITER, Vec3(100.f, 300.f, 200.f));
	scene->AddGameObject(jupiter);

	return scene;
}