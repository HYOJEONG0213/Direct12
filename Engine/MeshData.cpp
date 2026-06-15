#include "pch.h"
#include "MeshData.h"
#include "FBXLoader.h"
#include "Mesh.h"
#include "Material.h"
#include "Resources.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "Animator.h"
#include <fstream>

MeshData::MeshData() : Object(OBJECT_TYPE::MESH_DATA) {}

MeshData::~MeshData() {}

shared_ptr<MeshData> MeshData::LoadFromFBX(const wstring &path)
{
	FBXLoader loader;
	loader.LoadFbx(path);

	shared_ptr<MeshData> meshData = make_shared<MeshData>();

	for (int32 i = 0; i < loader.GetMeshCount(); i++)
	{
		shared_ptr<Mesh> mesh = Mesh::CreateFromFBX(&loader.GetMesh(i), loader);

		GET_SINGLE(Resources)->Add<Mesh>(mesh->GetName(), mesh);

		// Material 찾아서 연동
		vector<shared_ptr<Material>> materials;
		for (size_t j = 0; j < loader.GetMesh(i).materials.size(); j++)
		{
			shared_ptr<Material> material = GET_SINGLE(Resources)->Get<Material>(loader.GetMesh(i).materials[j].name);
			materials.push_back(material);
		}

		MeshRenderInfo info = {};
		info.mesh = mesh;
		info.materials = materials;
		meshData->_meshRenders.push_back(info);
	}

	return meshData;
}

void MeshData::Load(const wstring &path)
{
	ifstream ifs(path, ios::binary);
	assert(ifs.is_open());

	uint32 count;
	ifs.read(reinterpret_cast<char *>(&count), sizeof(count));

	for (uint32 i = 0; i < count; i++)
	{
		MeshRenderInfo info = {};

		// Mesh
		string meshName;
		size_t meshSize;
		ifs.read(reinterpret_cast<char *>(&meshSize), sizeof(meshSize));
		meshName.resize(meshSize);
		ifs.read(&meshName[0], meshSize);

		wstring meshPath = L"..\\Resources\\Mesh\\" + s2ws(meshName) + L".mesh";
		info.mesh = GET_SINGLE(Resources)->Load<Mesh>(s2ws(meshName), meshPath);

		// Materials
		uint32 materialCount;
		ifs.read(reinterpret_cast<char *>(&materialCount), sizeof(materialCount));
		for (uint32 j = 0; j < materialCount; j++)
		{
			string materialName;
			size_t materialSize;
			ifs.read(reinterpret_cast<char *>(&materialSize), sizeof(materialSize));
			materialName.resize(materialSize);
			ifs.read(&materialName[0], materialSize);

			wstring				 materialPath = L"..\\Resources\\Material\\" + s2ws(materialName) + L".mat";
			shared_ptr<Material> material = GET_SINGLE(Resources)->Load<Material>(s2ws(materialName), materialPath);
			info.materials.push_back(material);
		}

		_meshRenders.push_back(info);
	}
}

void MeshData::Save(const wstring &path)
{
	// 디렉토리 생성
	fs::path parentPath = fs::path(path).parent_path();
	fs::create_directories(L"..\\Resources\\Mesh");
	fs::create_directories(L"..\\Resources\\Material");

	ofstream ofs(path, ios::binary);
	assert(ofs.is_open());

	uint32 count = static_cast<uint32>(_meshRenders.size());
	ofs.write(reinterpret_cast<char *>(&count), sizeof(count));

	for (uint32 i = 0; i < count; i++)
	{
		MeshRenderInfo &info = _meshRenders[i];

		// Mesh
		string meshName = ws2s(info.mesh->GetName());
		size_t meshSize = meshName.size();
		ofs.write(reinterpret_cast<char *>(&meshSize), sizeof(meshSize));
		ofs.write(meshName.c_str(), meshSize);

		// Mesh 별도 저장
		wstring meshPath = L"..\\Resources\\Mesh\\" + info.mesh->GetName() + L".mesh";
		info.mesh->Save(meshPath);

		// Materials
		uint32 materialCount = static_cast<uint32>(info.materials.size());
		ofs.write(reinterpret_cast<char *>(&materialCount), sizeof(materialCount));
		for (uint32 j = 0; j < materialCount; j++)
		{
			string materialName = ws2s(info.materials[j]->GetName());
			size_t materialSize = materialName.size();
			ofs.write(reinterpret_cast<char *>(&materialSize), sizeof(materialSize));
			ofs.write(materialName.c_str(), materialSize);

			// Material 별도 저장
			wstring materialPath = L"..\\Resources\\Material\\" + info.materials[j]->GetName() + L".mat";
			info.materials[j]->Save(materialPath);
		}
	}
}

vector<shared_ptr<GameObject>> MeshData::Instantiate()
{
	vector<shared_ptr<GameObject>> v;

	for (MeshRenderInfo &info : _meshRenders)
	{
		shared_ptr<GameObject> gameObject = make_shared<GameObject>();
		gameObject->AddComponent(make_shared<Transform>());
		gameObject->AddComponent(make_shared<MeshRenderer>());
		gameObject->GetMeshRenderer()->SetMesh(info.mesh);

		for (uint32 i = 0; i < info.materials.size(); i++)
			gameObject->GetMeshRenderer()->SetMaterial(info.materials[i], i);

		if (info.mesh->IsAnimMesh())
		{
			shared_ptr<Animator> animator = make_shared<Animator>();
			gameObject->AddComponent(animator);
			animator->SetBones(info.mesh->GetBones());
			animator->SetAnimClip(info.mesh->GetAnimClip());
		}

		v.push_back(gameObject);
	}

	return v;
}
