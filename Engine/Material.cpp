#include "pch.h"
#include "Material.h"
#include "Engine.h"
#include "Resources.h"
#include "Shader.h"
#include "Texture.h"
#include <fstream>

Material::Material() : Object(OBJECT_TYPE::MATERIAL) {}

Material::~Material() {}

void Material::Load(const wstring &path)
{
	ifstream ifs(path, ios::binary);
	assert(ifs.is_open());

	// Shader
	string shaderName;
	{
		size_t size;
		ifs.read(reinterpret_cast<char *>(&size), sizeof(size));
		shaderName.resize(size);
		ifs.read(&shaderName[0], size);
	}
	_shader = GET_SINGLE(Resources)->Get<Shader>(s2ws(shaderName));

	// Params
	ifs.read(reinterpret_cast<char *>(&_params), sizeof(_params));

	// Textures
	for (int32 i = 0; i < MATERIAL_ARG_COUNT; i++)
	{
		string texName;
		size_t nameSize;
		ifs.read(reinterpret_cast<char *>(&nameSize), sizeof(nameSize));
		if (nameSize > 0)
		{
			texName.resize(nameSize);
			ifs.read(&texName[0], nameSize);

			string texPath;
			size_t pathSize;
			ifs.read(reinterpret_cast<char *>(&pathSize), sizeof(pathSize));
			texPath.resize(pathSize);
			ifs.read(&texPath[0], pathSize);

			_textures[i] = GET_SINGLE(Resources)->Get<Texture>(s2ws(texName));
			if (_textures[i] == nullptr)
			{
				_textures[i] = GET_SINGLE(Resources)->Load<Texture>(s2ws(texName), s2ws(texPath));
			}
		}
	}
}

void Material::Save(const wstring &path)
{
	ofstream ofs(path, ios::binary);
	assert(ofs.is_open());

	// Shader
	string shaderName = ws2s(_shader->GetName());
	size_t size = shaderName.size();
	ofs.write(reinterpret_cast<char *>(&size), sizeof(size));
	ofs.write(shaderName.c_str(), size);

	// Params
	ofs.write(reinterpret_cast<char *>(&_params), sizeof(_params));

	// Textures
	for (int32 i = 0; i < MATERIAL_ARG_COUNT; i++)
	{
		string texName;
		string texPath;
		if (_textures[i])
		{
			texName = ws2s(_textures[i]->GetName());
			texPath = ws2s(_textures[i]->GetPath());
		}

		size_t nameSize = texName.size();
		ofs.write(reinterpret_cast<char *>(&nameSize), sizeof(nameSize));
		if (nameSize > 0)
		{
			ofs.write(texName.c_str(), nameSize);

			size_t pathSize = texPath.size();
			ofs.write(reinterpret_cast<char *>(&pathSize), sizeof(pathSize));
			ofs.write(texPath.c_str(), pathSize);
		}
	}
}

void Material::PushGraphicsData()
{
	// CBV 업로드
	CONST_BUFFER(CONSTANT_BUFFER_TYPE::MATERIAL)->PushGraphicsData(&_params, sizeof(_params));

	// SRV 업로드
	for (size_t i = 0; i < _textures.size(); i++)
	{
		if (_textures[i] == nullptr) continue;

		SRV_REGISTER reg = SRV_REGISTER(static_cast<int8>(SRV_REGISTER::t0) + i);
		GEngine->GetGraphicsDescHeap()->SetSRV(_textures[i]->GetSRVHandle(), reg);
	}

	// 파이프라인 세팅
	_shader->Update();
}

void Material::PushComputeData()
{
	// CBV 업로드
	CONST_BUFFER(CONSTANT_BUFFER_TYPE::MATERIAL)->PushComputeData(&_params, sizeof(_params));

	// SRV 업로드
	for (size_t i = 0; i < _textures.size(); i++)
	{
		if (_textures[i] == nullptr) continue;

		SRV_REGISTER reg = SRV_REGISTER(static_cast<int8>(SRV_REGISTER::t0) + i);
		GEngine->GetComputeDescHeap()->SetSRV(_textures[i]->GetSRVHandle(), reg);
	}

	// 파이프라인 세팅
	_shader->Update();
}

void Material::Dispatch(uint32 x, uint32 y, uint32 z)
{
	// CBV + SRV + SetPipelineState
	PushComputeData();

	// SetDescriptorHeaps + SetComputeRootDescriptorTable
	GEngine->GetComputeDescHeap()->CommitTable();

	COMPUTE_CMD_LIST->Dispatch(x, y, z);

	GEngine->GetComputeCmdQueue()->FlushComputeCommandQueue();
}

shared_ptr<Material> Material::Clone()
{
	shared_ptr<Material> material = make_shared<Material>();

	material->SetShader(_shader);
	material->_params = _params;
	material->_textures = _textures;

	return material;
}