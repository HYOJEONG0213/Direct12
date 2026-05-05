#include "pch.h"
#include "Camera.h"
#include "Transform.h"
#include "Scene.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Engine.h"

Matrix Camera::S_MatView;
Matrix Camera::S_MatProjection;

Camera::Camera() : Component(COMPONENT_TYPE::CAMERA) {}

Camera::~Camera() {}

void Camera::FinalUpdate()
{
	// 뷰행렬 = 카메라의 월드 행렬의 역행렬
	_matView = GetTransform()->GetLocalToWorldMatrix().Invert();

	float width = static_cast<float>(GEngine->GetWindow().width);
	float height = static_cast<float>(GEngine->GetWindow().height);

	// 투영 행렬: ratio(화면의 비율)을 알아야했다..
	if (_type == PROJECTION_TYPE::PERSPECTIVE) // 원근 투영
		_matProjection = ::XMMatrixPerspectiveFovLH(_fov, width / height, _near, _far);
	else // 직교 투영
		_matProjection = ::XMMatrixOrthographicLH(width * _scale, height * _scale, _near, _far);

	// static으로 잠시만 셋팅! (Transform에서 사용할 예정)
	S_MatView = _matView;
	S_MatProjection = _matProjection;

	_frustum.FinalUpdate();
}

void Camera::Render()
{
	shared_ptr<Scene> scene = GET_SINGLE(SceneManager)->GetActiveScene();

	// TODO : Layer 구분
	const vector<shared_ptr<GameObject>> &gameObjects = scene->GetGameObjects();

	for (auto &gameObject : gameObjects)
	{
		if (gameObject->GetMeshRenderer() == nullptr) continue;

		if (gameObject->GetCheckFrustum())
		{
			// 가상의 구를 만들어 체크하기
			if (_frustum.ContainsSphere(gameObject->GetTransform()->GetWorldPosition(),
										gameObject->GetTransform()->GetBoundingSphereRadius()) == false)
			{
				continue;
			}
		}

		gameObject->GetMeshRenderer()->Render();
	}
}