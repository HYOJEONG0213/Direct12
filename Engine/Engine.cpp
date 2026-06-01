#include "pch.h"
#include "Engine.h"
#include "Material.h"
#include "Transform.h"
#include "Input.h"
#include "Timer.h"
#include "SceneManager.h"
#include "Light.h"
#include "Resources.h"
#include "InstancingManager.h"

void Engine::Init(const WindowInfo &info)
{
	_window = info;

	// 그려질 화면 크기 설정
	_viewport = {0, 0, static_cast<FLOAT>(info.width), static_cast<FLOAT>(info.height), 0.0f, 1.0f};
	_scissorRect = CD3DX12_RECT(0, 0, info.width, info.height);

	_device->Init();
	_graphicsCmdQueue->Init(_device->GetDevice(), _swapChain);
	_computeCmdQueue->Init(_device->GetDevice());
	_swapChain->Init(info, _device->GetDevice(), _device->GetDXGI(), _graphicsCmdQueue->GetGraphicsCmdQueue());
	_rootSignature->Init();
	_graphicsDescHeap->Init(256);
	_computeDescHeap->Init();

	CreateConstantBuffer(CBV_REGISTER::b0, sizeof(LightParams), 1);		  // b0: 라이트 저장 (전역으로 사용)
	CreateConstantBuffer(CBV_REGISTER::b1, sizeof(TransformParams), 256); // b1: 트랜스폼 저장
	CreateConstantBuffer(CBV_REGISTER::b2, sizeof(MaterialParams), 256);  // Material 파람 저장

	CreateRenderTargetGroups();

	ResizeWindow(info.width, info.height);

	GET_SINGLE(Input)->Init(info.hwnd);
	GET_SINGLE(Timer)->Init();
	GET_SINGLE(Resources)->Init();
}

void Engine::ShowFps()
{
	uint32 fps = GET_SINGLE(Timer)->GetFps();

	WCHAR text[100] = L"";
	::wsprintf(text, L"FPS : %d", fps);

	::SetWindowText(_window.hwnd, text);
}

void Engine::CreateConstantBuffer(CBV_REGISTER reg, uint32 bufferSize, uint32 count)
{
	// 배열의 인덱스 = 레지스터 번호 맞추기
	uint8 typeInt = static_cast<uint8>(reg);
	assert(_constantBuffers.size() == typeInt);

	// 2차원 벡터마냥 만들어주고, 버퍼 Init() 해주고 push_back
	shared_ptr<ConstantBuffer> buffer = make_shared<ConstantBuffer>();
	buffer->Init(reg, bufferSize, count);
	_constantBuffers.push_back(buffer);
}

void Engine::Update()
{
	GET_SINGLE(Input)->Update();
	GET_SINGLE(Timer)->Update();
	GET_SINGLE(SceneManager)->Update();
	GET_SINGLE(InstancingManager)->ClearBuffer();

	Render();

	ShowFps();
}

void Engine::Render()
{
	RenderBegin();

	// 그려줄(렌더링할) 내용
	GET_SINGLE(SceneManager)->Render();

	RenderEnd();
}

// 커멘더큐에 요청사항 넣기
void Engine::RenderBegin() { _graphicsCmdQueue->RenderBegin(); }

// 커멘더큐에 요청사항 다 넣었음을 알린뒤 실행시키기
void Engine::RenderEnd() { _graphicsCmdQueue->RenderEnd(); }

// 윈도우 크기 변경
void Engine::ResizeWindow(int32 width, int32 height)
{
	_window.width = width;
	_window.height = height;

	RECT rect = {0, 0, _window.width, _window.height};
	//:: 글로벌 네임스페이스에서 찾아주겠다
	::AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);
	//(100,100) 위치에 핸들러 윈도우된거 띄워줘라.
	::SetWindowPos(_window.hwnd, 0, 100, 100, width, height, 0);
}

void Engine::CreateRenderTargetGroups()
{
	// DepthStencil
	shared_ptr<Texture> dsTexture =
		GET_SINGLE(Resources)->CreateTexture(L"DepthStencil", DXGI_FORMAT_D32_FLOAT, _window.width, _window.height,
											 CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
											 D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

	// SwapChain Group
	{
		vector<RenderTarget> rtVec(SWAP_CHAIN_BUFFER_COUNT);

		// 전면 버퍼, 후면 버퍼 꺼내기
		for (uint32 i = 0; i < SWAP_CHAIN_BUFFER_COUNT; ++i)
		{
			wstring name = L"SwapChainTarget_" + std::to_wstring(i);

			ComPtr<ID3D12Resource> resource;
			_swapChain->GetSwapChain()->GetBuffer(i, IID_PPV_ARGS(&resource));
			// 이미 존재하는 리소스를 통해 텍스처 제작
			rtVec[i].target = GET_SINGLE(Resources)->CreateTextureFromResource(name, resource);
		}

		_rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)] = make_shared<RenderTargetGroup>();
		_rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)]->Create(
			RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN, rtVec, dsTexture);
	}

	// Shadow Group
	{
		vector<RenderTarget> rtVec(RENDER_TARGET_SHADOW_GROUP_MEMBER_COUNT);

		rtVec[0].target = GET_SINGLE(Resources)->CreateTexture(
			L"ShadowTarget", DXGI_FORMAT_R32_FLOAT, 4096, 4096, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

		shared_ptr<Texture> shadowDepthTexture = GET_SINGLE(Resources)->CreateTexture(
			L"ShadowDepthStencil", DXGI_FORMAT_D32_FLOAT, 4096, 4096, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

		_rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::SHADOW)] = make_shared<RenderTargetGroup>();
		_rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::SHADOW)]->Create(RENDER_TARGET_GROUP_TYPE::SHADOW, rtVec,
																				shadowDepthTexture);
	}

	// Deferred Group
	{
		vector<RenderTarget> rtVec(RENDER_TARGET_G_BUFFER_GROUP_MEMBER_COUNT);

		rtVec[0].target =
			GET_SINGLE(Resources)->CreateTexture(L"PositionTarget", DXGI_FORMAT_R32G32B32A32_FLOAT, _window.width,
												 _window.height, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
												 D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

		rtVec[1].target =
			GET_SINGLE(Resources)->CreateTexture(L"NormalTarget", DXGI_FORMAT_R32G32B32A32_FLOAT, _window.width,
												 _window.height, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
												 D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

		rtVec[2].target =
			GET_SINGLE(Resources)->CreateTexture(L"DiffuseTarget", DXGI_FORMAT_R8G8B8A8_UNORM, _window.width,
												 _window.height, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
												 D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

		_rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::G_BUFFER)] = make_shared<RenderTargetGroup>();
		_rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::G_BUFFER)]->Create(RENDER_TARGET_GROUP_TYPE::G_BUFFER,
																				  rtVec, dsTexture);
	}

	// Lighting Group
	{
		vector<RenderTarget> rtVec(RENDER_TARGET_LIGHTING_GROUP_MEMBER_COUNT);

		rtVec[0].target =
			GET_SINGLE(Resources)->CreateTexture(L"DiffuseLightTarget", DXGI_FORMAT_R8G8B8A8_UNORM, _window.width,
												 _window.height, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
												 D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

		rtVec[1].target =
			GET_SINGLE(Resources)->CreateTexture(L"SpecularLightTarget", DXGI_FORMAT_R8G8B8A8_UNORM, _window.width,
												 _window.height, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
												 D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

		_rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::LIGHTING)] = make_shared<RenderTargetGroup>();
		_rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::LIGHTING)]->Create(RENDER_TARGET_GROUP_TYPE::LIGHTING,
																				  rtVec, dsTexture);
	}
}