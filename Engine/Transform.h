#pragma once
#include "Component.h"

class Transform : public Component
{
public:
	Transform();
	virtual ~Transform();

	virtual void FinalUpdate() override;
	void		 PushData();

	// 임시!
	float GetBoundingSphereRadius() { return max(max(_localScale.x, _localScale.y), _localScale.z); }

public:
	// Parent 기준
	const Vec3 &GetLocalPosition() { return _localPosition; }
	const Vec3 &GetLocalRotation() { return _localRotation; }
	const Vec3 &GetLocalScale() { return _localScale; }

	const Matrix &GetLocalToWorldMatrix() { return _matWorld; }			 // 찐찐찐 월드 행렬 반환
	Vec3		  GetWorldPosition() { return _matWorld.Translation(); } // 월드에서의 내 위치

	Vec3 GetRight() { return _matWorld.Right(); } // 월드에서의 내 오른쪽 방향
	Vec3 GetUp() { return _matWorld.Up(); }
	Vec3 GetLook() { return _matWorld.Backward(); }

	void SetLocalPosition(const Vec3 &position) { _localPosition = position; }
	void SetLocalRotation(const Vec3 &rotation) { _localRotation = rotation; }
	void SetLocalScale(const Vec3 &scale) { _localScale = scale; }

public:
	void				SetParent(shared_ptr<Transform> parent) { _parent = parent; }
	weak_ptr<Transform> GetParent() { return _parent; }

private:
	// Parent 기준으로 하는 로컬 좌표
	Vec3 _localPosition = {};
	Vec3 _localRotation = {};
	Vec3 _localScale = {1.f, 1.f, 1.f};

	Matrix _matLocal = {}; // 부모를 기준으로 한 로컬 행렬
	Matrix _matWorld = {}; // 월드 행렬

	weak_ptr<Transform> _parent;
};
