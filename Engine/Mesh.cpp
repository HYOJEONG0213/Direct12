#include "pch.h"
#include "Mesh.h"
#include "Engine.h"
#include "Material.h"
#include "InstancingBuffer.h"
#include "FBXLoader.h"
#include "StructuredBuffer.h"
#include <fstream>

Mesh::Mesh() : Object(OBJECT_TYPE::MESH) {}

Mesh::~Mesh() {}

void Mesh::Load(const wstring &path)
{
	ifstream ifs(path, ios::binary);
	assert(ifs.is_open());

	// Vertex Buffer
	ifs.read(reinterpret_cast<char *>(&_vertexCount), sizeof(_vertexCount));
	vector<Vertex> vertices(_vertexCount);
	ifs.read(reinterpret_cast<char *>(vertices.data()), _vertexCount * sizeof(Vertex));
	CreateVertexBuffer(vertices);

	// Index Buffers
	uint32 indexCount;
	ifs.read(reinterpret_cast<char *>(&indexCount), sizeof(indexCount));
	for (uint32 i = 0; i < indexCount; i++)
	{
		uint32 count;
		ifs.read(reinterpret_cast<char *>(&count), sizeof(count));
		vector<uint32> indices(count);
		ifs.read(reinterpret_cast<char *>(indices.data()), count * sizeof(uint32));
		CreateIndexBuffer(indices);
	}

	// Bones
	uint32 boneCount;
	ifs.read(reinterpret_cast<char *>(&boneCount), sizeof(boneCount));
	_bones.resize(boneCount);
	for (uint32 i = 0; i < boneCount; i++)
	{
		BoneInfo &info = _bones[i];
		string	  name;
		size_t	  nameSize;
		ifs.read(reinterpret_cast<char *>(&nameSize), sizeof(nameSize));
		name.resize(nameSize);
		ifs.read(&name[0], nameSize);
		info.boneName = s2ws(name);
		ifs.read(reinterpret_cast<char *>(&info.parentIdx), sizeof(info.parentIdx));
		ifs.read(reinterpret_cast<char *>(&info.matOffset), sizeof(info.matOffset));
	}

	// Animation Clips
	uint32 animCount;
	ifs.read(reinterpret_cast<char *>(&animCount), sizeof(animCount));
	_animClips.resize(animCount);
	for (uint32 i = 0; i < animCount; i++)
	{
		AnimClipInfo &info = _animClips[i];
		string		  name;
		size_t		  nameSize;
		ifs.read(reinterpret_cast<char *>(&nameSize), sizeof(nameSize));
		name.resize(nameSize);
		ifs.read(&name[0], nameSize);
		info.animName = s2ws(name);
		ifs.read(reinterpret_cast<char *>(&info.frameCount), sizeof(info.frameCount));
		ifs.read(reinterpret_cast<char *>(&info.duration), sizeof(info.duration));

		info.keyFrames.resize(boneCount);
		for (uint32 b = 0; b < boneCount; b++)
		{
			uint32 keyframeCount;
			ifs.read(reinterpret_cast<char *>(&keyframeCount), sizeof(keyframeCount));
			info.keyFrames[b].resize(keyframeCount);
			ifs.read(reinterpret_cast<char *>(info.keyFrames[b].data()), keyframeCount * sizeof(KeyFrameInfo));
		}
	}

	// SkinData
	if (IsAnimMesh())
	{
		// BoneOffet 행렬
		const int32	   boneCount = static_cast<int32>(_bones.size());
		vector<Matrix> offsetVec(boneCount);
		for (size_t b = 0; b < boneCount; b++) offsetVec[b] = _bones[b].matOffset;

		// OffsetMatrix StructuredBuffer 세팅
		_offsetBuffer = make_shared<StructuredBuffer>();
		_offsetBuffer->Init(sizeof(Matrix), static_cast<uint32>(offsetVec.size()), offsetVec.data());

		const int32 animCount = static_cast<int32>(_animClips.size());
		for (int32 i = 0; i < animCount; i++)
		{
			AnimClipInfo &animClip = _animClips[i];

			// 애니메이션 프레임 정보
			vector<AnimFrameParams> frameParams;
			frameParams.resize(_bones.size() * animClip.frameCount);

			for (int32 b = 0; b < boneCount; b++)
			{
				const int32 keyFrameCount = static_cast<int32>(animClip.keyFrames[b].size());
				for (int32 f = 0; f < keyFrameCount; f++)
				{
					int32 idx = static_cast<int32>(boneCount * f + b);

					frameParams[idx] = AnimFrameParams{Vec4(animClip.keyFrames[b][f].scale),
													   animClip.keyFrames[b][f].rotation, // Quaternion
													   Vec4(animClip.keyFrames[b][f].translate)};
				}
			}

			// StructuredBuffer 세팅
			_frameBuffer.push_back(make_shared<StructuredBuffer>());
			_frameBuffer.back()->Init(sizeof(AnimFrameParams), static_cast<uint32>(frameParams.size()),
									  frameParams.data());
		}
	}
}

void Mesh::Save(const wstring &path)
{
	ofstream ofs(path, ios::binary);
	assert(ofs.is_open());

	// Vertex Buffer
	ofs.write(reinterpret_cast<char *>(&_vertexCount), sizeof(_vertexCount));
	ofs.write(reinterpret_cast<const char *>(_vertices.data()), _vertexCount * sizeof(Vertex));

	// Index Buffers
	uint32 indexCount = static_cast<uint32>(_indices.size());
	ofs.write(reinterpret_cast<char *>(&indexCount), sizeof(indexCount));
	for (uint32 i = 0; i < indexCount; i++)
	{
		uint32 count = static_cast<uint32>(_indices[i].size());
		ofs.write(reinterpret_cast<char *>(&count), sizeof(count));
		ofs.write(reinterpret_cast<const char *>(_indices[i].data()), count * sizeof(uint32));
	}

	// Bones
	uint32 boneCount = static_cast<uint32>(_bones.size());
	ofs.write(reinterpret_cast<char *>(&boneCount), sizeof(boneCount));
	for (uint32 i = 0; i < boneCount; i++)
	{
		BoneInfo &info = _bones[i];
		string	  name = ws2s(info.boneName);
		size_t	  nameSize = name.size();
		ofs.write(reinterpret_cast<char *>(&nameSize), sizeof(nameSize));
		ofs.write(name.c_str(), nameSize);
		ofs.write(reinterpret_cast<char *>(&info.parentIdx), sizeof(info.parentIdx));
		ofs.write(reinterpret_cast<char *>(&info.matOffset), sizeof(info.matOffset));
	}

	// Animation Clips
	uint32 animCount = static_cast<uint32>(_animClips.size());
	ofs.write(reinterpret_cast<char *>(&animCount), sizeof(animCount));
	for (uint32 i = 0; i < animCount; i++)
	{
		AnimClipInfo &info = _animClips[i];
		string		  name = ws2s(info.animName);
		size_t		  nameSize = name.size();
		ofs.write(reinterpret_cast<char *>(&nameSize), sizeof(nameSize));
		ofs.write(name.c_str(), nameSize);
		ofs.write(reinterpret_cast<char *>(&info.frameCount), sizeof(info.frameCount));
		ofs.write(reinterpret_cast<char *>(&info.duration), sizeof(info.duration));

		for (uint32 b = 0; b < boneCount; b++)
		{
			uint32 keyframeCount = static_cast<uint32>(info.keyFrames[b].size());
			ofs.write(reinterpret_cast<char *>(&keyframeCount), sizeof(keyframeCount));
			ofs.write(reinterpret_cast<const char *>(info.keyFrames[b].data()), keyframeCount * sizeof(KeyFrameInfo));
		}
	}
}

void Mesh::Create(const vector<Vertex> &vertexBuffer, const vector<uint32> &indexbuffer)
{
	CreateVertexBuffer(vertexBuffer);
	CreateIndexBuffer(indexbuffer);
}

void Mesh::Render(uint32 instanceCount, uint32 idx)
{
	GRAPHICS_CMD_LIST->IASetVertexBuffers(0, 1, &_vertexBufferView); // Slot: (0~15)
	GRAPHICS_CMD_LIST->IASetIndexBuffer(&_vecIndexInfo[idx].bufferView);

	GEngine->GetGraphicsDescHeap()->CommitTable();

	GRAPHICS_CMD_LIST->DrawIndexedInstanced(_vecIndexInfo[idx].count, instanceCount, 0, 0, 0);
}

void Mesh::Render(shared_ptr<InstancingBuffer> &buffer, uint32 idx)
{
	D3D12_VERTEX_BUFFER_VIEW bufferViews[] = {_vertexBufferView, buffer->GetBufferView()};
	GRAPHICS_CMD_LIST->IASetVertexBuffers(0, 2, bufferViews);
	GRAPHICS_CMD_LIST->IASetIndexBuffer(&_vecIndexInfo[idx].bufferView);

	GEngine->GetGraphicsDescHeap()->CommitTable();

	GRAPHICS_CMD_LIST->DrawIndexedInstanced(_vecIndexInfo[idx].count, buffer->GetCount(), 0, 0, 0);
}

shared_ptr<Mesh> Mesh::CreateFromFBX(const FbxMeshInfo *meshInfo, FBXLoader &loader)
{
	shared_ptr<Mesh> mesh = make_shared<Mesh>();
	mesh->SetName(meshInfo->name);
	mesh->CreateVertexBuffer(meshInfo->vertices);

	for (const vector<uint32> &buffer : meshInfo->indices)
	{
		if (buffer.empty())
		{
			// FBX 파일이 이상하다. IndexBuffer가 없으면 에러 나니까 임시 처리
			vector<uint32> defaultBuffer{0};
			mesh->CreateIndexBuffer(defaultBuffer);
		}
		else { mesh->CreateIndexBuffer(buffer); }
	}

	if (meshInfo->hasAnimation) mesh->CreateBonesAndAnimations(loader);

	return mesh;
}

// 벡터에 vertex 받기 (위치, 컬러 정보)
void Mesh::CreateVertexBuffer(const vector<Vertex> &buffer)
{
	_vertices = buffer;
	_vertexCount = static_cast<uint32>(buffer.size());
	uint32 bufferSize = _vertexCount * sizeof(Vertex);

	// 정점을 GPU 메모리에 복사하기 위해 해당 공간 할당받음
	// 어떤 타입? 얼마나 공간 필요해?
	D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC	  desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

	// vertexBuffer 생성 (GPU 메모리 공간에 할당되어있음)
	DEVICE->CreateCommittedResource(&heapProperty, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ,
									nullptr, IID_PPV_ARGS(&_vertexBuffer));

	// vertexDataBuffer를 통해 vertexBuffer(실제 GPU 공간)에
	// 데이터 복사할 수 있게 연결
	// Copy the triangle data to the vertex buffer.
	void		 *vertexDataBuffer = nullptr;
	CD3DX12_RANGE readRange(0, 0); // We do not intend to read from this resource on the CPU.

	// 공간 매핑 (뚜껑 열고, 닫고)
	_vertexBuffer->Map(0, &readRange, &vertexDataBuffer);
	// 실제 GPU 공간에 데이터 복사
	::memcpy(vertexDataBuffer, &buffer[0], bufferSize);
	_vertexBuffer->Unmap(0, nullptr);

	// Initialize the vertex buffer view.
	// 각각의 뷰 정보
	_vertexBufferView.BufferLocation = _vertexBuffer->GetGPUVirtualAddress();
	_vertexBufferView.StrideInBytes = sizeof(Vertex); // 정점 1개 크기
	_vertexBufferView.SizeInBytes = bufferSize;		  // 버퍼의 크기
}

void Mesh::CreateIndexBuffer(const vector<uint32> &buffer)
{
	_indices.push_back(buffer);
	uint32 indexCount = static_cast<uint32>(buffer.size());
	uint32 bufferSize = indexCount * sizeof(uint32);

	D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC	  desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

	ComPtr<ID3D12Resource> indexBuffer;
	DEVICE->CreateCommittedResource(&heapProperty, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ,
									nullptr, IID_PPV_ARGS(&indexBuffer));

	void		 *indexDataBuffer = nullptr;
	CD3DX12_RANGE readRange(0, 0);
	indexBuffer->Map(0, &readRange, &indexDataBuffer);
	::memcpy(indexDataBuffer, &buffer[0], bufferSize);
	indexBuffer->Unmap(0, nullptr);

	D3D12_INDEX_BUFFER_VIEW indexBufferView;
	indexBufferView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
	indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	indexBufferView.SizeInBytes = bufferSize;

	IndexBufferInfo info = {indexBuffer, indexBufferView, DXGI_FORMAT_R32_UINT, indexCount};

	_vecIndexInfo.push_back(info);
}

void Mesh::CreateBonesAndAnimations(class FBXLoader &loader)
{
#pragma region AnimClip
	uint32								 frameCount = 0;
	vector<shared_ptr<FbxAnimClipInfo>> &animClips = loader.GetAnimClip();
	for (shared_ptr<FbxAnimClipInfo> &ac : animClips)
	{
		AnimClipInfo info = {};

		info.animName = ac->name;
		info.duration = ac->endTime.GetSecondDouble() - ac->startTime.GetSecondDouble();

		int32 startFrame = static_cast<int32>(ac->startTime.GetFrameCount(ac->mode));
		int32 endFrame = static_cast<int32>(ac->endTime.GetFrameCount(ac->mode));
		info.frameCount = endFrame - startFrame;

		info.keyFrames.resize(ac->keyFrames.size());

		const int32 boneCount = static_cast<int32>(ac->keyFrames.size());
		for (int32 b = 0; b < boneCount; b++)
		{
			auto &vec = ac->keyFrames[b];

			const int32 size = static_cast<int32>(vec.size());
			frameCount = max(frameCount, static_cast<uint32>(size));
			info.keyFrames[b].resize(size);

			for (int32 f = 0; f < size; f++)
			{
				FbxKeyFrameInfo &kf = vec[f];
				// FBX에서 파싱한 정보들로 채워준다
				KeyFrameInfo &kfInfo = info.keyFrames[b][f];
				kfInfo.time = kf.time;
				kfInfo.frame = static_cast<int32>(size);
				kfInfo.scale.x = static_cast<float>(kf.matTransform.GetS().mData[0]);
				kfInfo.scale.y = static_cast<float>(kf.matTransform.GetS().mData[1]);
				kfInfo.scale.z = static_cast<float>(kf.matTransform.GetS().mData[2]);
				kfInfo.rotation.x = static_cast<float>(kf.matTransform.GetQ().mData[0]);
				kfInfo.rotation.y = static_cast<float>(kf.matTransform.GetQ().mData[1]);
				kfInfo.rotation.z = static_cast<float>(kf.matTransform.GetQ().mData[2]);
				kfInfo.rotation.w = static_cast<float>(kf.matTransform.GetQ().mData[3]);
				kfInfo.translate.x = static_cast<float>(kf.matTransform.GetT().mData[0]);
				kfInfo.translate.y = static_cast<float>(kf.matTransform.GetT().mData[1]);
				kfInfo.translate.z = static_cast<float>(kf.matTransform.GetT().mData[2]);
			}
		}

		_animClips.push_back(info);
	}
#pragma endregion

#pragma region Bones
	vector<shared_ptr<FbxBoneInfo>> &bones = loader.GetBones();
	for (shared_ptr<FbxBoneInfo> &bone : bones)
	{
		BoneInfo boneInfo = {};
		boneInfo.parentIdx = bone->parentIndex;
		boneInfo.matOffset = GetMatrix(bone->matOffset);
		boneInfo.boneName = bone->boneName;
		_bones.push_back(boneInfo);
	}
#pragma endregion

#pragma region SkinData
	if (IsAnimMesh()) // 애니메이션이 있는 메쉬인 경우
	{
		// BoneOffet 행렬
		const int32	   boneCount = static_cast<int32>(_bones.size());
		vector<Matrix> offsetVec(boneCount);
		for (size_t b = 0; b < boneCount; b++) offsetVec[b] = _bones[b].matOffset;

		// OffsetMatrix StructuredBuffer 세팅
		_offsetBuffer = make_shared<StructuredBuffer>();
		_offsetBuffer->Init(sizeof(Matrix), static_cast<uint32>(offsetVec.size()), offsetVec.data());

		const int32 animCount = static_cast<int32>(_animClips.size());
		for (int32 i = 0; i < animCount; i++)
		{
			AnimClipInfo &animClip = _animClips[i];

			// 애니메이션 프레임 정보
			vector<AnimFrameParams> frameParams;
			frameParams.resize(_bones.size() * animClip.frameCount);

			for (int32 b = 0; b < boneCount; b++)
			{
				const int32 keyFrameCount = static_cast<int32>(animClip.keyFrames[b].size());
				for (int32 f = 0; f < keyFrameCount; f++)
				{
					int32 idx = static_cast<int32>(boneCount * f + b);

					frameParams[idx] = AnimFrameParams{Vec4(animClip.keyFrames[b][f].scale),
													   animClip.keyFrames[b][f].rotation, // Quaternion
													   Vec4(animClip.keyFrames[b][f].translate)};
				}
			}

			// StructuredBuffer 세팅
			_frameBuffer.push_back(make_shared<StructuredBuffer>());
			_frameBuffer.back()->Init(sizeof(AnimFrameParams), static_cast<uint32>(frameParams.size()),
									  frameParams.data());
		}
	}
#pragma endregion
}

Matrix Mesh::GetMatrix(FbxAMatrix &matrix)
{
	Matrix mat;

	for (int32 y = 0; y < 4; ++y)
		for (int32 x = 0; x < 4; ++x) mat.m[y][x] = static_cast<float>(matrix.Get(y, x));

	return mat;
}