#include "pch.h"
#include "SwapChain.h"

void SwapChain::Init(const WindowInfo &info, ComPtr<ID3D12Device> device, ComPtr<IDXGIFactory> dxgi,
					 ComPtr<ID3D12CommandQueue> cmdQueue)
{
	CreateSwapChain(info, dxgi, cmdQueue);
	CreateRTV(device);
}

// 현재 화면에 어떤게 표시되어야하는지, 표시하기
void SwapChain::Present() { _swapChain->Present(0, 0); }

void SwapChain::SwapIndex() { _backBufferIndex = (_backBufferIndex + 1) % SWAP_CHAIN_BUFFER_COUNT; }

void SwapChain::CreateSwapChain(const WindowInfo &info, ComPtr<IDXGIFactory> dxgi, ComPtr<ID3D12CommandQueue> cmdQueue)
{
	// 이전에 만든 정보 날린다
	_swapChain.Reset();

	// 버퍼 어떻게 만들지 묘사하기
	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Width = static_cast<uint32>(info.width);	 // 버퍼의 해상도 너비
	sd.BufferDesc.Height = static_cast<uint32>(info.height); // 버퍼의 해상도 높이
	sd.BufferDesc.RefreshRate.Numerator = 60;				 // 화면 갱신 비율
	sd.BufferDesc.RefreshRate.Denominator = 1;				 // 화면 갱신 비율
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;		 // 버퍼의 디스플레이 형식
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.SampleDesc.Count = 1; // 멀티 샘플링 OFF
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // 후면 버퍼에 렌더링할 것
	// 버퍼카운터
	sd.BufferCount = SWAP_CHAIN_BUFFER_COUNT; // 전면+후면 버퍼
	sd.OutputWindow = info.hwnd;
	sd.Windowed = info.windowed;
	// 전면 후면 버퍼 교체 시 이전 프레임 정보 버림
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	// dxgi: device 출력과 관련된 부분들, &_swapChin : 결과물을 여기에 알려줘
	dxgi->CreateSwapChain(cmdQueue.Get(), &sd, &_swapChain);

	// 버퍼를 _renderTargets 에 넣어준다..
	for (int32 i = 0; i < SWAP_CHAIN_BUFFER_COUNT; i++) _swapChain->GetBuffer(i, IID_PPV_ARGS(&_rtvBuffer[i]));
}

void SwapChain::CreateRTV(ComPtr<ID3D12Device> device)
{
	// Descriptor (DX12) = View (~DX11)
	// [서술자 힙]으로 RTV 생성 (원래는 각 리소스마다 뷰가 있었으나, 다렉12에서 모든얘들 관리함)
	// DX11의 RTV(RenderTargetView), DSV(DepthStencilView),
	// CBV(ConstantBufferView), SRV(ShaderResourceView), UAV(UnorderedAccessView)를 전부!

	// 렌더타켓뷰 사이즈 생성
	int32 _rtvHeapSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	// rtv를 만들거다! 이때 SWAP_CHAIN_BUFFER_COUNT(2)만큼 만들거다!
	D3D12_DESCRIPTOR_HEAP_DESC rtvDesc;
	rtvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvDesc.NumDescriptors = SWAP_CHAIN_BUFFER_COUNT;
	rtvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvDesc.NodeMask = 0;

	// 같은 종류의 데이터끼리 배열로 관리
	// RTV 목록 : [ ] [ ] : 2개짜리 관리 가능
	device->CreateDescriptorHeap(&rtvDesc, IID_PPV_ARGS(&_rtvHeap));

	// rtvHeap의 첫주소 꺼낸후
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHeapBegin = _rtvHeap->GetCPUDescriptorHandleForHeapStart();

	for (int i = 0; i < SWAP_CHAIN_BUFFER_COUNT; i++)
	{
		// (첫주소 + i* 사이즈) 만큼해서 다음 주소로 이동하겠다!
		// Handle : 포인터처럼 원격으로 접근해 사용
		_rtvHandle[i] = CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvHeapBegin, i * _rtvHeapSize);
		// 0번과 1번 각각 꺼내서 렌더타켓뷰 생성
		device->CreateRenderTargetView(_rtvBuffer[i].Get(), nullptr, _rtvHandle[i]);
	}
}
