#include "pch.h"
#include "Camera.h"
#include "Transform.h"
#include "Scene.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Engine.h"
#include "Material.h"
#include "Shader.h"
#include "ParticleSystem.h"

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

	_frustum.FinalUpdate();
}

void Camera::SortGameObject()
{
	shared_ptr<Scene>					  scene = GET_SINGLE(SceneManager)->GetActiveScene();
	const vector<shared_ptr<GameObject>> &gameObjects = scene->GetGameObjects();

	_vecForward.clear();
	_vecDeferred.clear();
	_vecParticle.clear();

	for (auto &gameObject : gameObjects)
	{
		if (gameObject->GetMeshRenderer() == nullptr && gameObject->GetParticleSystem() == nullptr) continue;

		if (IsCulled(gameObject->GetLayerIndex())) continue;

		if (gameObject->GetCheckFrustum())
		{
			if (_frustum.ContainsSphere(gameObject->GetTransform()->GetWorldPosition(),
										gameObject->GetTransform()->GetBoundingSphereRadius()) == false)
			{
				continue;
			}
		}

		if (gameObject->GetMeshRenderer())
		{
			SHADER_TYPE shaderType = gameObject->GetMeshRenderer()->GetMaterial()->GetShader()->GetShaderType();
			switch (shaderType)
			{
			case SHADER_TYPE::DEFERRED:
				_vecDeferred.push_back(gameObject);
				break;
			case SHADER_TYPE::FORWARD:
				_vecForward.push_back(gameObject);
				break;
			}
		}
		else
		{
			// 일반적인 파티클
			_vecParticle.push_back(gameObject);
		}
	}
}

void Camera::Render_Deferred()
{
	S_MatView = _matView;
	S_MatProjection = _matProjection;

	for (auto &gameObject : _vecDeferred) { gameObject->GetMeshRenderer()->Render(); }
}

void Camera::Render_Forward()
{
	S_MatView = _matView;
	S_MatProjection = _matProjection;

	for (auto &gameObject : _vecForward) { gameObject->GetMeshRenderer()->Render(); }
	for (auto &gameObject : _vecParticle) { gameObject->GetParticleSystem()->Render(); }
}