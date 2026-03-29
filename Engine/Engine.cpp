#include "pch.h"
#include "Engine.h"
#include "Material.h"
#include "Transform.h"
#include "Input.h"
#include "Timer.h"
#include "SceneManager.h"

void Engine::Init(const WindowInfo &info)
{
	_window = info;

	// 그려질 화면 크기 설정
	_viewport = {0, 0, static_cast<FLOAT>(info.width), static_cast<FLOAT>(info.height), 0.0f, 1.0f};
	_scissorRect = CD3DX12_RECT(0, 0, info.width, info.height);

	_device->Init();
	_cmdQueue->Init(_device->GetDevice(), _swapChain);
	_swapChain->Init(info, _device->GetDevice(), _device->GetDXGI(), _cmdQueue->GetCmdQueue());
	_rootSignature->Init();
	_tableDescHeap->Init(512);
	_depthStencilBuffer->Init(_window);

	CreateConstantBuffer(CBV_REGISTER::b0, sizeof(TransformMatrix), 256); // b0: 트랜스폼 저장
	CreateConstantBuffer(CBV_REGISTER::b1, sizeof(MaterialParams), 256);  // Material 파람 저장

	ResizeWindow(info.width, info.height);

	GET_SINGLE(Input)->Init(info.hwnd);
	GET_SINGLE(Timer)->Init();
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

	Render();

	ShowFps();
}

void Engine::LateUpdate() {}

void Engine::Render()
{
	RenderBegin();

	// 그려줄(렌더링할) 내용
	GET_SINGLE(SceneManager)->Update();

	RenderEnd();
}

// 커멘더큐에 요청사항 넣기
void Engine::RenderBegin() { _cmdQueue->RenderBegin(&_viewport, &_scissorRect); }

// 커멘더큐에 요청사항 다 넣었음을 알린뒤 실행시키기
void Engine::RenderEnd() { _cmdQueue->RenderEnd(); }

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

	_depthStencilBuffer->Init(_window);
}
