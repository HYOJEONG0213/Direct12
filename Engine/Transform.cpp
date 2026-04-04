#include "pch.h"
#include "Transform.h"
#include "Engine.h"
#include "Camera.h"

Transform::Transform() : Component(COMPONENT_TYPE::TRANSFORM) {}

Transform::~Transform() {}

void Transform::FinalUpdate()
{
	// S R T 계산
	Matrix matScale = Matrix::CreateScale(_localScale);
	Matrix matRotation = Matrix::CreateRotationX(_localRotation.x);
	matRotation *= Matrix::CreateRotationY(_localRotation.y);
	matRotation *= Matrix::CreateRotationZ(_localRotation.z);
	Matrix matTranslation = Matrix::CreateTranslation(_localPosition);

	// 부모가 없는 SRT = 월드 행렬이 됨
	_matLocal = matScale * matRotation * matTranslation;
	_matWorld = _matLocal;

	// 부모가 있으면 부모의 월드 행렬에 곱해주면 됨
	shared_ptr<Transform> parent = GetParent().lock();
	if (parent != nullptr) { _matWorld *= parent->GetLocalToWorldMatrix(); }
}

void Transform::PushData()
{
	// view : 카메라가 바라보는 방향을 기준으로 만들어지는 행렬..
	// WVP : World * View * Projection
	Matrix matWVP = _matWorld * Camera::S_MatView * Camera::S_MatProjection;
	// CONST_BUFFER(CONSTANT_BUFFER_TYPE::TRANSFORM)->PushData(&matWVP, sizeof(matWVP));
}