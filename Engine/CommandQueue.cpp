#include "pch.h"
#include "CommandQueue.h"
#include "SwapChain.h"
#include "Engine.h"

CommandQueue::~CommandQueue()
{
	::CloseHandle(_fenceEvent);
}

void CommandQueue::Init(ComPtr<ID3D12Device> device,shared_ptr<SwapChain> swapChain)
{
	_swapChain = swapChain;

	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	//device를 통해 cmdQueue에 CommandQueue 생성
	device->CreateCommandQueue(&queueDesc,IID_PPV_ARGS(&_cmdQueue));

	//cmdAlloc 으로 메모리 관리 
	// - D3D12_COMMAND_LIST_TYPE_DIRECT : GPU가 직접 실행하는 명령 목록
	device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,IID_PPV_ARGS(&_cmdAlloc));

	// GPU가 하나인 시스템에서는 0으로
	// DIRECT
	// Allocator
	// 초기 상태 (그리기 명령은 nullptr 지정)

	//벡터에 clear()하면 다 날리는게 아니라 capacity는 유지, 사이즈만 바꾸는 느낌 
	device->CreateCommandList(0,D3D12_COMMAND_LIST_TYPE_DIRECT,_cmdAlloc.Get(),nullptr,IID_PPV_ARGS(&_cmdList));


	// CommandList는 Close / Open 상태가 있는데
	// Open 상태에서 Command를 넣다가 Close한 다음 제출하는 개념
	_cmdList->Close();

	device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,IID_PPV_ARGS(&_resCmdAlloc));
	device->CreateCommandList(0,D3D12_COMMAND_LIST_TYPE_DIRECT,
							_resCmdAlloc.Get(),nullptr,IID_PPV_ARGS(&_resCmdList));

	// CreateFence
	// - CPU와 GPU의 동기화 수단으로 쓰인다
	device->CreateFence(0,D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&_fence));
	_fenceEvent = ::CreateEvent(nullptr,FALSE,FALSE,nullptr);

}

//cpu가 gpu 일이 끝날떄까지 대기 -> 나쁜코드긴 함.. 
void CommandQueue::WaitSync()
{
	//펜스값 증가 
	// Advance the fence value to mark commands up to this fence point.
	_fenceValue++;

	//커멘드 큐에 해당 펜스값 보냄 
	_cmdQueue->Signal(_fence.Get(),_fenceValue);

	// Wait until the GPU has completed commands up to this fence point.
	if(_fence->GetCompletedValue() < _fenceValue)
	{
		// 펜스 번호까지 끝났으면 이벤트 실행 
		_fence->SetEventOnCompletion(_fenceValue,_fenceEvent);

		// cpu 살짝 대기 
		::WaitForSingleObject(_fenceEvent,INFINITE);
	}
}

void CommandQueue::RenderBegin(const D3D12_VIEWPORT* vp,const D3D12_RECT* rect)
{
	_cmdAlloc->Reset();
	_cmdList->Reset(_cmdAlloc.Get(),nullptr);

	// 스왑버퍼 왔다갔다 설정해주기 (Transition : Before(화면출력) -> After(외주결과물))
	D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		_swapChain->GetBackRTVBuffer().Get(),
		D3D12_RESOURCE_STATE_PRESENT, // 현재 화면 출력
		D3D12_RESOURCE_STATE_RENDER_TARGET); // 외주 결과물(뒤에서 작업되는 상태)

	_cmdList->SetGraphicsRootSignature(ROOT_SIGNATURE.Get());
	GEngine->GetCB()->Clear();
	GEngine->GetTableDescHeap()->Clear();

	ID3D12DescriptorHeap* descHeap = GEngine->GetTableDescHeap()->GetDescriptorHeap().Get();
	_cmdList->SetDescriptorHeaps(1,&descHeap);

	_cmdList->ResourceBarrier(1,&barrier);

	// _cmdList의 viewport and scissor rect.  This needs to be reset whenever the command list is reset.
	_cmdList->RSSetViewports(1,vp);
	_cmdList->RSSetScissorRects(1,rect);

	// Specify the buffers we are going to render to.
	// 어떤 버퍼에 그림 그려야하는지 다시 언급 
	// 백버퍼 꺼내온다음에 거기 대상으로 GPU한테 그려달라 요청하기 
	D3D12_CPU_DESCRIPTOR_HANDLE backBufferView = _swapChain->GetBackRTV();
	_cmdList->ClearRenderTargetView(backBufferView,Colors::LightSteelBlue,0,nullptr);
	_cmdList->OMSetRenderTargets(1,&backBufferView,FALSE,nullptr);

}

void CommandQueue::RenderEnd()
{
	//Transition : Begin(외주 결과물 : 백버퍼) -> After(화면 출력) 
	//Begin와 정반대;; 
	D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		_swapChain->GetBackRTVBuffer().Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, // 외주 결과물
		D3D12_RESOURCE_STATE_PRESENT); // 화면 출력

	_cmdList->ResourceBarrier(1,&barrier);
	_cmdList->Close();	// 커맨드 리스트 닫기 추가됌 (일감 여기서 끝~)

	// 커맨드 리스트 수행 (진.짜.실.행) 
	ID3D12CommandList* cmdListArr[] = {_cmdList.Get()};
	_cmdQueue->ExecuteCommandLists(_countof(cmdListArr),cmdListArr);

	//버퍼를 가지고 진짜로 보여줌. 
	_swapChain->Present();

	// Wait until frame commands are complete.  This waiting is inefficient and is
	// done for simplicity.  Later we will show how to organize our rendering code
	// so we do not have to wait per frame.
	// 일련의 과정들이 다 실행될때까지 대기 
	WaitSync();

	// 진짜 바꿔치기 
	_swapChain->SwapIndex();
}

// resCmdList 에 있는 일감들의 리소스 로드 
void CommandQueue::FlushResourceCommandQueue()
{
	_resCmdList->Close();	// 명령 기록 종료 

	// GPU에 명령 전달 (실행 시작)
	ID3D12CommandList* cmdListArr[] = {_resCmdList.Get()};
	_cmdQueue->ExecuteCommandLists(_countof(cmdListArr),cmdListArr);

	// CPU와 GPU와 동기화 (기다리기) 
	WaitSync();

	_resCmdAlloc->Reset();
	_resCmdList->Reset(_resCmdAlloc.Get(),nullptr);
}
