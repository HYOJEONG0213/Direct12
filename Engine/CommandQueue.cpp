#include "pch.h"
#include "CommandQueue.h"

CommandQueue::~CommandQueue()
{
	::CloseHandle(_fenceEvent);
}

void CommandQueue::Init(ComPtr<ID3D12Device> device, shared_ptr<SwapChain> swapChain, shared_ptr<DescriptorHeap> descHeap)
{
	_swapChain = swapChain;
	_descHeap = descHeap;

	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	//device를 통해 cmdQueue에 CommandQueue 생성
	device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&_cmdQueue));

	//cmdAlloc 으로 메모리 관리 
	// - D3D12_COMMAND_LIST_TYPE_DIRECT : GPU가 직접 실행하는 명령 목록
	device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_cmdAlloc));

	// GPU가 하나인 시스템에서는 0으로
	// DIRECT
	// Allocator
	// 초기 상태 (그리기 명령은 nullptr 지정)

	//벡터에 clear()하면 다 날리는게 아니라 capacity는 유지, 사이즈만 바꾸는 느낌 
	device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _cmdAlloc.Get(), nullptr, IID_PPV_ARGS(&_cmdList));


	// CommandList는 Close / Open 상태가 있는데
	// Open 상태에서 Command를 넣다가 Close한 다음 제출하는 개념
	_cmdList->Close();

	// CreateFence
	// - CPU와 GPU의 동기화 수단으로 쓰인다
	device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence));
	_fenceEvent = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);

}

//cpu가 gpu 일이 끝날떄까지 대기 -> 나쁜코드긴 함.. 
void CommandQueue::WaitSync()
{
	//펜스값 증가 
	// Advance the fence value to mark commands up to this fence point.
	_fenceValue++;

	//커멘드 큐에 해당 펜스값 보냄 
	_cmdQueue->Signal(_fence.Get(), _fenceValue);

	// Wait until the GPU has completed commands up to this fence point.
	if (_fence->GetCompletedValue() < _fenceValue)
	{
		// 펜스 번호까지 끝났으면 이벤트 실행 
		_fence->SetEventOnCompletion(_fenceValue, _fenceEvent);

		// cpu 살짝 대기 
		::WaitForSingleObject(_fenceEvent, INFINITE);
	}
}

void CommandQueue::RenderBegin(const D3D12_VIEWPORT* vp, const D3D12_RECT* rect)
{
}

void CommandQueue::RenderEnd()
{
}
