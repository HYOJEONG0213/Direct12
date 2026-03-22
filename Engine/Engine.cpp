#include "pch.h"
#include "Engine.h"

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
	_cb->Init(sizeof(Transform), 256);
	_tableDescHeap->Init(512);
	_depthStencilBuffer->Init(_window);

	_input->Init(info.hwnd);
	_timer->Init();

	ResizeWindow(info.width, info.height);
}

void Engine::Render()
{
	RenderBegin();

	// 그려줄(렌더링할) 내용

	RenderEnd();
}

void Engine::ShowFps()
{
	uint32 fps = _timer->GetFps();

	WCHAR text[100] = L"";
	::wsprintf(text, L"FPS : %d", fps);

	::SetWindowText(_window.hwnd, text);
}

// 커멘더큐에 요청사항 넣기
void Engine::RenderBegin() { _cmdQueue->RenderBegin(&_viewport, &_scissorRect); }

// 커멘더큐에 요청사항 다 넣었음을 알린뒤 실행시키기
void Engine::RenderEnd() { _cmdQueue->RenderEnd(); }

void Engine::Update()
{
	_input->Update();
	_timer->Update();

	ShowFps();
}

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
