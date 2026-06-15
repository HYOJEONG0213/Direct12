#include "pch.h"
#include "Animator.h"
#include "Timer.h"
#include "Resources.h"
#include "Material.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include "StructuredBuffer.h"
#include <fstream>

Animator::Animator() : Component(COMPONENT_TYPE::ANIMATOR)
{
	_computeMaterial = GET_SINGLE(Resources)->Get<Material>(L"ComputeAnimation");
	_boneFinalMatrix = make_shared<StructuredBuffer>();
}

Animator::~Animator() {}

void Animator::Load(const wstring &path)
{
	ifstream ifs(path, ios::binary);
	assert(ifs.is_open());

	// Bones
	uint32 boneCount;
	ifs.read(reinterpret_cast<char *>(&boneCount), sizeof(boneCount));
	vector<BoneInfo> *bones = new vector<BoneInfo>(boneCount);
	for (uint32 i = 0; i < boneCount; i++)
	{
		BoneInfo &info = (*bones)[i];
		string	  name;
		size_t	  nameSize;
		ifs.read(reinterpret_cast<char *>(&nameSize), sizeof(nameSize));
		name.resize(nameSize);
		ifs.read(&name[0], nameSize);
		info.boneName = s2ws(name);
		ifs.read(reinterpret_cast<char *>(&info.parentIdx), sizeof(info.parentIdx));
		ifs.read(reinterpret_cast<char *>(&info.matOffset), sizeof(info.matOffset));
	}
	_bones = bones;

	// Animation Clips
	uint32 animCount;
	ifs.read(reinterpret_cast<char *>(&animCount), sizeof(animCount));
	vector<AnimClipInfo> *animClips = new vector<AnimClipInfo>(animCount);
	for (uint32 i = 0; i < animCount; i++)
	{
		AnimClipInfo &info = (*animClips)[i];
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
	_animClips = animClips;
}

void Animator::Save(const wstring &path)
{
	ofstream ofs(path, ios::binary);
	assert(ofs.is_open());

	// Bones
	uint32 boneCount = static_cast<uint32>(_bones->size());
	ofs.write(reinterpret_cast<char *>(&boneCount), sizeof(boneCount));
	for (uint32 i = 0; i < boneCount; i++)
	{
		const BoneInfo &info = (*_bones)[i];
		string			name = ws2s(info.boneName);
		size_t			nameSize = name.size();
		ofs.write(reinterpret_cast<char *>(&nameSize), sizeof(nameSize));
		ofs.write(name.c_str(), nameSize);
		ofs.write(reinterpret_cast<const char *>(&info.parentIdx), sizeof(info.parentIdx));
		ofs.write(reinterpret_cast<const char *>(&info.matOffset), sizeof(info.matOffset));
	}

	// Animation Clips
	uint32 animCount = static_cast<uint32>(_animClips->size());
	ofs.write(reinterpret_cast<char *>(&animCount), sizeof(animCount));
	for (uint32 i = 0; i < animCount; i++)
	{
		const AnimClipInfo &info = (*_animClips)[i];
		string				name = ws2s(info.animName);
		size_t				nameSize = name.size();
		ofs.write(reinterpret_cast<char *>(&nameSize), sizeof(nameSize));
		ofs.write(name.c_str(), nameSize);
		ofs.write(reinterpret_cast<const char *>(&info.frameCount), sizeof(info.frameCount));
		ofs.write(reinterpret_cast<const char *>(&info.duration), sizeof(info.duration));

		for (uint32 b = 0; b < boneCount; b++)
		{
			uint32 keyframeCount = static_cast<uint32>(info.keyFrames[b].size());
			ofs.write(reinterpret_cast<char *>(&keyframeCount), sizeof(keyframeCount));
			ofs.write(reinterpret_cast<const char *>(info.keyFrames[b].data()), keyframeCount * sizeof(KeyFrameInfo));
		}
	}
}

void Animator::FinalUpdate()
{
	// 클립 시간에 따라 프레임 계산
	_updateTime += DELTA_TIME;

	const AnimClipInfo &animClip = _animClips->at(_clipIndex);
	if (_updateTime >= animClip.duration) _updateTime = 0.f;

	const int32 ratio = static_cast<int32>(animClip.frameCount / animClip.duration);
	_frame = static_cast<int32>(_updateTime * ratio);
	_frame = min(_frame, animClip.frameCount - 1);
	_nextFrame = min(_frame + 1, animClip.frameCount - 1);
	_frameRatio = static_cast<float>(_frame - _frame);
}

void Animator::SetAnimClip(const vector<AnimClipInfo> *animClips) { _animClips = animClips; }

void Animator::PushData()
{
	uint32 boneCount = static_cast<uint32>(_bones->size());
	if (_boneFinalMatrix->GetElementCount() < boneCount) _boneFinalMatrix->Init(sizeof(Matrix), boneCount);

	// Compute Shader
	shared_ptr<Mesh> mesh = GetGameObject()->GetMeshRenderer()->GetMesh();
	mesh->GetBoneFrameDataBuffer(_clipIndex)->PushComputeSRVData(SRV_REGISTER::t8);
	mesh->GetBoneOffsetBuffer()->PushComputeSRVData(SRV_REGISTER::t9);

	_boneFinalMatrix->PushComputeUAVData(UAV_REGISTER::u0);

	_computeMaterial->SetInt(0, boneCount);
	_computeMaterial->SetInt(1, _frame);
	_computeMaterial->SetInt(2, _nextFrame);
	_computeMaterial->SetFloat(0, _frameRatio);

	uint32 groupCount = (boneCount / 256) + 1;
	_computeMaterial->Dispatch(groupCount, 1, 1);

	// Graphics Shader
	_boneFinalMatrix->PushGraphicsData(SRV_REGISTER::t7);
}

void Animator::Play(uint32 idx)
{
	assert(idx < _animClips->size());
	_clipIndex = idx;
	_updateTime = 0.f;
}